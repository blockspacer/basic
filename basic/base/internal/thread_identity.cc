// Copyright 2017 The Basic Authors.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "basic/base/internal/thread_identity.h"

#ifndef _WIN32
#include <pthread.h>
#include <signal.h>
#endif

#include <atomic>
#include <cassert>
#include <memory>

#include "basic/base/call_once.h"
#include "basic/base/internal/raw_logging.h"
#include "basic/base/internal/spinlock.h"

namespace basic {
namespace base_internal {

#if ABSL_THREAD_IDENTITY_MODE != ABSL_THREAD_IDENTITY_MODE_USE_CPP11
namespace {
// Used to co-ordinate one-time creation of our pthread_key
basic::once_flag init_thread_identity_key_once;
pthread_key_t thread_identity_pthread_key;
std::atomic<bool> pthread_key_initialized(false);

void AllocateThreadIdentityKey(ThreadIdentityReclaimerFunction reclaimer) {
  pthread_key_create(&thread_identity_pthread_key, reclaimer);
  pthread_key_initialized.store(true, std::memory_order_release);
}
}  // namespace
#endif

#if ABSL_THREAD_IDENTITY_MODE == ABSL_THREAD_IDENTITY_MODE_USE_TLS || \
    ABSL_THREAD_IDENTITY_MODE == ABSL_THREAD_IDENTITY_MODE_USE_CPP11
// The actual TLS storage for a thread's currently associated ThreadIdentity.
// This is referenced by inline accessors in the header.
// "protected" visibility ensures that if multiple instances of Basic code
// exist within a process (via dlopen() or similar), references to
// thread_identity_ptr from each instance of the code will refer to
// *different* instances of this ptr.
#ifdef __GNUC__
__attribute__((visibility("protected")))
#endif  // __GNUC__
  ABSL_PER_THREAD_TLS_KEYWORD ThreadIdentity* thread_identity_ptr;
#endif  // TLS or CPP11

void SetCurrentThreadIdentity(
    ThreadIdentity* identity, ThreadIdentityReclaimerFunction reclaimer) {
  assert(CurrentThreadIdentityIfPresent() == nullptr);
  // Associate our destructor.
  // NOTE: This call to pthread_setspecific is currently the only immovable
  // barrier to CurrentThreadIdentity() always being async signal safe.
#if ABSL_THREAD_IDENTITY_MODE == ABSL_THREAD_IDENTITY_MODE_USE_POSIX_SETSPECIFIC
  // NOTE: Not async-safe.  But can be open-coded.
  basic::call_once(init_thread_identity_key_once, AllocateThreadIdentityKey,
                  reclaimer);

#ifdef __EMSCRIPTEN__
  // Emscripten PThread implementation does not support signals.
  // See https://kripken.github.io/emscripten-site/docs/porting/pthreads.html
  // for more information.
  pthread_setspecific(thread_identity_pthread_key,
                      reinterpret_cast<void*>(identity));
#else
  // We must mask signals around the call to setspecific as with current glibc,
  // a concurrent getspecific (needed for GetCurrentThreadIdentityIfPresent())
  // may zero our value.
  //
  // While not officially async-signal safe, getspecific within a signal handler
  // is otherwise OK.
  sigset_t all_signals;
  sigset_t curr_signals;
  sigfillset(&all_signals);
  pthread_sigmask(SIG_SETMASK, &all_signals, &curr_signals);
  pthread_setspecific(thread_identity_pthread_key,
                      reinterpret_cast<void*>(identity));
  pthread_sigmask(SIG_SETMASK, &curr_signals, nullptr);
#endif  // !__EMSCRIPTEN__

#elif ABSL_THREAD_IDENTITY_MODE == ABSL_THREAD_IDENTITY_MODE_USE_TLS
  // NOTE: Not async-safe.  But can be open-coded.
  basic::call_once(init_thread_identity_key_once, AllocateThreadIdentityKey,
                  reclaimer);
  pthread_setspecific(thread_identity_pthread_key,
                      reinterpret_cast<void*>(identity));
  thread_identity_ptr = identity;
#elif ABSL_THREAD_IDENTITY_MODE == ABSL_THREAD_IDENTITY_MODE_USE_CPP11
  thread_local std::unique_ptr<ThreadIdentity, ThreadIdentityReclaimerFunction>
      holder(identity, reclaimer);
  thread_identity_ptr = identity;
#else
#error Unimplemented ABSL_THREAD_IDENTITY_MODE
#endif
}

void ClearCurrentThreadIdentity() {
#if ABSL_THREAD_IDENTITY_MODE == ABSL_THREAD_IDENTITY_MODE_USE_TLS || \
    ABSL_THREAD_IDENTITY_MODE == ABSL_THREAD_IDENTITY_MODE_USE_CPP11
  thread_identity_ptr = nullptr;
#elif ABSL_THREAD_IDENTITY_MODE == \
      ABSL_THREAD_IDENTITY_MODE_USE_POSIX_SETSPECIFIC
  // pthread_setspecific expected to clear value on destruction
  assert(CurrentThreadIdentityIfPresent() == nullptr);
#endif
}

#if ABSL_THREAD_IDENTITY_MODE == ABSL_THREAD_IDENTITY_MODE_USE_POSIX_SETSPECIFIC
ThreadIdentity* CurrentThreadIdentityIfPresent() {
  bool initialized = pthread_key_initialized.load(std::memory_order_acquire);
  if (!initialized) {
    return nullptr;
  }
  return reinterpret_cast<ThreadIdentity*>(
      pthread_getspecific(thread_identity_pthread_key));
}
#endif

}  // namespace base_internal
}  // namespace basic
