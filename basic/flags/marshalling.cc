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

#include "basic/flags/marshalling.h"

#include <limits>

#include "basic/base/macros.h"
#include "basic/strings/match.h"
#include "basic/strings/numbers.h"
#include "basic/strings/str_cat.h"
#include "basic/strings/str_format.h"
#include "basic/strings/str_join.h"
#include "basic/strings/str_split.h"

namespace basic {
namespace flags_internal {

// --------------------------------------------------------------------
// AbslParseFlag specializations for boolean type.

bool AbslParseFlag(basic::string_view text, bool* dst, std::string*) {
  const char* kTrue[] = {"1", "t", "true", "y", "yes"};
  const char* kFalse[] = {"0", "f", "false", "n", "no"};
  static_assert(sizeof(kTrue) == sizeof(kFalse), "true_false_equal");

  text = basic::StripAsciiWhitespace(text);

  for (size_t i = 0; i < ABSL_ARRAYSIZE(kTrue); ++i) {
    if (basic::EqualsIgnoreCase(text, kTrue[i])) {
      *dst = true;
      return true;
    } else if (basic::EqualsIgnoreCase(text, kFalse[i])) {
      *dst = false;
      return true;
    }
  }
  return false;  // didn't match a legal input
}

// --------------------------------------------------------------------
// AbslParseFlag for integral types.

// Return the base to use for parsing text as an integer.  Leading 0x
// puts us in base 16.  But leading 0 does not put us in base 8. It
// caused too many bugs when we had that behavior.
static int NumericBase(basic::string_view text) {
  const bool hex = (text.size() >= 2 && text[0] == '0' &&
                    (text[1] == 'x' || text[1] == 'X'));
  return hex ? 16 : 10;
}

template <typename IntType>
inline bool ParseFlagImpl(basic::string_view text, IntType* dst) {
  text = basic::StripAsciiWhitespace(text);

  return basic::numbers_internal::safe_strtoi_base(text, dst, NumericBase(text));
}

bool AbslParseFlag(basic::string_view text, short* dst, std::string*) {
  int val;
  if (!ParseFlagImpl(text, &val)) return false;
  if (static_cast<short>(val) != val)  // worked, but number out of range
    return false;
  *dst = static_cast<short>(val);
  return true;
}

bool AbslParseFlag(basic::string_view text, unsigned short* dst, std::string*) {
  unsigned int val;
  if (!ParseFlagImpl(text, &val)) return false;
  if (static_cast<unsigned short>(val) !=
      val)  // worked, but number out of range
    return false;
  *dst = static_cast<unsigned short>(val);
  return true;
}

bool AbslParseFlag(basic::string_view text, int* dst, std::string*) {
  return ParseFlagImpl(text, dst);
}

bool AbslParseFlag(basic::string_view text, unsigned int* dst, std::string*) {
  return ParseFlagImpl(text, dst);
}

bool AbslParseFlag(basic::string_view text, long* dst, std::string*) {
  return ParseFlagImpl(text, dst);
}

bool AbslParseFlag(basic::string_view text, unsigned long* dst, std::string*) {
  return ParseFlagImpl(text, dst);
}

bool AbslParseFlag(basic::string_view text, long long* dst, std::string*) {
  return ParseFlagImpl(text, dst);
}

bool AbslParseFlag(basic::string_view text, unsigned long long* dst,
                   std::string*) {
  return ParseFlagImpl(text, dst);
}

// --------------------------------------------------------------------
// AbslParseFlag for floating point types.

bool AbslParseFlag(basic::string_view text, float* dst, std::string*) {
  return basic::SimpleAtof(text, dst);
}

bool AbslParseFlag(basic::string_view text, double* dst, std::string*) {
  return basic::SimpleAtod(text, dst);
}

// --------------------------------------------------------------------
// AbslParseFlag for strings.

bool AbslParseFlag(basic::string_view text, std::string* dst, std::string*) {
  dst->assign(text.data(), text.size());
  return true;
}

// --------------------------------------------------------------------
// AbslParseFlag for vector of strings.

bool AbslParseFlag(basic::string_view text, std::vector<std::string>* dst,
                   std::string*) {
  // An empty flag value corresponds to an empty vector, not a vector
  // with a single, empty std::string.
  if (text.empty()) {
    dst->clear();
    return true;
  }
  *dst = basic::StrSplit(text, ',', basic::AllowEmpty());
  return true;
}

// --------------------------------------------------------------------
// AbslUnparseFlag specializations for various builtin flag types.

std::string Unparse(bool v) { return v ? "true" : "false"; }
std::string Unparse(short v) { return basic::StrCat(v); }
std::string Unparse(unsigned short v) { return basic::StrCat(v); }
std::string Unparse(int v) { return basic::StrCat(v); }
std::string Unparse(unsigned int v) { return basic::StrCat(v); }
std::string Unparse(long v) { return basic::StrCat(v); }
std::string Unparse(unsigned long v) { return basic::StrCat(v); }
std::string Unparse(long long v) { return basic::StrCat(v); }
std::string Unparse(unsigned long long v) { return basic::StrCat(v); }
template <typename T>
std::string UnparseFloatingPointVal(T v) {
  // digits10 is guaranteed to roundtrip correctly in std::string -> value -> std::string
  // conversions, but may not be enough to represent all the values correctly.
  std::string digit10_str =
      basic::StrFormat("%.*g", std::numeric_limits<T>::digits10, v);
  if (std::isnan(v) || std::isinf(v)) return digit10_str;

  T roundtrip_val = 0;
  std::string err;
  if (basic::ParseFlag(digit10_str, &roundtrip_val, &err) &&
      roundtrip_val == v) {
    return digit10_str;
  }

  // max_digits10 is the number of base-10 digits that are necessary to uniquely
  // represent all distinct values.
  return basic::StrFormat("%.*g", std::numeric_limits<T>::max_digits10, v);
}
std::string Unparse(float v) { return UnparseFloatingPointVal(v); }
std::string Unparse(double v) { return UnparseFloatingPointVal(v); }
std::string AbslUnparseFlag(basic::string_view v) { return std::string(v); }
std::string AbslUnparseFlag(const std::vector<std::string>& v) {
  return basic::StrJoin(v, ",");
}

}  // namespace flags_internal
}  // namespace basic
