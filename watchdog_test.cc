// Copyright 2008, Google Inc.
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
//    * Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
//    * Redistributions in binary form must reproduce the above
// copyright notice, this list of conditions and the following disclaimer
// in the documentation and/or other materials provided with the
// distribution.
//    * Neither the name of Google Inc. nor the names of its
// contributors may be used to endorse or promote products derived from
// this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

// Tests for Watchdog class.

#include "base/logging.h"
#include "base/watchdog.h"
#include "base/spin_wait.h"
#include "base/time.h"
#include "testing/gtest/include/gtest/gtest.h"

namespace {

//------------------------------------------------------------------------------
// Provide a derived class to facilitate testing.

// TODO(JAR): Remove default argument from constructor, and make mandatory.
class WatchdogCounter : public Watchdog {
 public:
  WatchdogCounter(const TimeDelta& duration,
                  const std::wstring& thread_watched_name,
                  bool enabled = true)
      : Watchdog(duration, thread_watched_name, enabled), alarm_counter_(0) {
  }

  virtual ~WatchdogCounter() {}

  virtual void Alarm() {
    alarm_counter_++;
    Watchdog::Alarm();
  }

  int alarm_counter() { return alarm_counter_; }

 private:
  int alarm_counter_;

  DISALLOW_EVIL_CONSTRUCTORS(WatchdogCounter);
};

class WatchdogTest : public testing::Test {
};


//------------------------------------------------------------------------------
// Actual tests

// Minimal constructor/destructor test.
TEST(WatchdogTest, StartupShutdownTest) {
  Watchdog watchdog1(TimeDelta::FromMilliseconds(300), L"Disabled", false);
  Watchdog watchdog2(TimeDelta::FromMilliseconds(300), L"Enabled", true);

  // The following test is depricated, and should be removed when the
  // default argument constructor is no longer accepted.
  Watchdog watchdog3(TimeDelta::FromMilliseconds(300), L"Default");
}

// Test ability to call Arm and Disarm repeatedly.
TEST(WatchdogTest, ArmDisarmTest) {
  Watchdog watchdog1(TimeDelta::FromMilliseconds(300), L"Disabled", false);
  watchdog1.Arm();
  watchdog1.Disarm();
  watchdog1.Arm();
  watchdog1.Disarm();

  Watchdog watchdog2(TimeDelta::FromMilliseconds(300), L"Enabled", true);
  watchdog2.Arm();
  watchdog2.Disarm();
  watchdog2.Arm();
  watchdog2.Disarm();

  // The following test is depricated, and should be removed when the
  // default argument constructor is no longer accepted.
  Watchdog watchdog3(TimeDelta::FromMilliseconds(300), L"Default");
  watchdog3.Arm();
  watchdog3.Disarm();
  watchdog3.Arm();
  watchdog3.Disarm();
}

// Make sure a basic alarm fires when the time has expired.
TEST(WatchdogTest, AlarmTest) {
  WatchdogCounter watchdog(TimeDelta::FromMilliseconds(10), L"Enabled", true);
  watchdog.Arm();
  SPIN_FOR_TIMEDELTA_OR_UNTIL_TRUE(TimeDelta::FromSeconds(1),
                                   watchdog.alarm_counter() > 0);
  EXPECT_EQ(1, watchdog.alarm_counter());

  // Set a time greater than the timeout into the past.
  watchdog.ArmSomeTimeDeltaAgo(TimeDelta::FromSeconds(2));
  // It should instantly go off, but certainly in less than a second.
  SPIN_FOR_TIMEDELTA_OR_UNTIL_TRUE(TimeDelta::FromSeconds(1),
                                   watchdog.alarm_counter() > 1);

  EXPECT_EQ(2, watchdog.alarm_counter());
}

// Make sure a disable alarm does nothing, even if we arm it.
TEST(WatchdogTest, ConstructorDisabledTest) {
  WatchdogCounter watchdog(TimeDelta::FromMilliseconds(10), L"Disabled", false);
  watchdog.Arm();
  // Alarm should not fire, as it was disabled.
  Sleep(500);
  EXPECT_EQ(0, watchdog.alarm_counter());
}

// Make sure Disarming will prevent firing, even after Arming.
TEST(WatchdogTest, DisarmTest) {
  WatchdogCounter watchdog(TimeDelta::FromSeconds(1), L"Enabled", true);
  watchdog.Arm();
  Sleep(100);  // Don't sleep too long
  watchdog.Disarm();
  // Alarm should not fire.
  Sleep(1500);
  EXPECT_EQ(0, watchdog.alarm_counter());

  // ...but even after disarming, we can still use the alarm...
  // Set a time greater than the timeout into the past.
  watchdog.ArmSomeTimeDeltaAgo(TimeDelta::FromSeconds(2));
  // It should almost instantly go off, but certainly in less than a second.
  SPIN_FOR_TIMEDELTA_OR_UNTIL_TRUE(TimeDelta::FromSeconds(1),
                                   watchdog.alarm_counter() > 0);

  EXPECT_EQ(1, watchdog.alarm_counter());
}

}  // namespace
