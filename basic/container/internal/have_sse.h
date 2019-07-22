// Copyright 2018 The Basic Authors.
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
//
// Shared config probing for SSE instructions used in Swiss tables.
#ifndef ABSL_CONTAINER_INTERNAL_HAVE_SSE_H_
#define ABSL_CONTAINER_INTERNAL_HAVE_SSE_H_

#ifndef SWISSTABLE_HAVE_SSE2
#if defined(__SSE2__) ||  \
    (defined(_MSC_VER) && \
     (defined(_M_X64) || (defined(_M_IX86) && _M_IX86_FP >= 2)))
#define SWISSTABLE_HAVE_SSE2 1
#else
#define SWISSTABLE_HAVE_SSE2 0
#endif
#endif

#ifndef SWISSTABLE_HAVE_SSSE3
#ifdef __SSSE3__
#define SWISSTABLE_HAVE_SSSE3 1
#else
#define SWISSTABLE_HAVE_SSSE3 0
#endif
#endif

#if SWISSTABLE_HAVE_SSSE3 && !SWISSTABLE_HAVE_SSE2
#error "Bad configuration!"
#endif

#if SWISSTABLE_HAVE_SSE2
#include <emmintrin.h>
#endif

#if SWISSTABLE_HAVE_SSSE3
#include <tmmintrin.h>
#endif

#endif  // ABSL_CONTAINER_INTERNAL_HAVE_SSE_H_
