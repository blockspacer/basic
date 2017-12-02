// Copyright 2017 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BASE_TEST_BIND_TEST_UTIL_H_
#define BASE_TEST_BIND_TEST_UTIL_H_

#include "base/bind.h"

namespace base {

// A variant of Bind() that can bind capturing lambdas for testing.
// This doesn't support extra arguments binding as the lambda itself can do.
template <typename F>
RepeatingCallback<internal::ExtractCallableRunType<std::decay_t<F>>>
BindLambdaForTesting(F&& f) {
  return BindRepeating([](const std::decay_t<F>& f) { return f(); },
                       std::forward<F>(f));
}

}  // namespace base

#endif  // BASE_TEST_BIND_TEST_UTIL_H_
