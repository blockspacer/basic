//
//  Copyright 2019 The Basic Authors.
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

#ifndef ABSL_FLAGS_USAGE_H_
#define ABSL_FLAGS_USAGE_H_

#include "basic/strings/string_view.h"

// --------------------------------------------------------------------
// Usage reporting interfaces

namespace basic {

// Sets the "usage" message to be used by help reporting routines.
// For example:
//  basic::SetProgramUsageMessage(
//      basic::StrCat("This program does nothing.  Sample usage:\n", argv[0],
//                   " <uselessarg1> <uselessarg2>"));
// Do not include commandline flags in the usage: we do that for you!
// Note: Calling SetProgramUsageMessage twice will trigger a call to std::exit.
void SetProgramUsageMessage(basic::string_view new_usage_message);

// Returns the usage message set by SetProgramUsageMessage().
basic::string_view ProgramUsageMessage();

}  // namespace basic

#endif  // ABSL_FLAGS_USAGE_H_
