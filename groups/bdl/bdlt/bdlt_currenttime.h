// bdlt_currenttime.h                                                 -*-C++-*-
#ifndef INCLUDED_BDLT_CURRENTTIME
#define INCLUDED_BDLT_CURRENTTIME

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide utilities to retrieve the current time.
//
//@CLASSES:
//   bdlt::CurrentTime: namespace for current-time procedures
//
//@SEE_ALSO: bsls_timeinterval, bdlt_systemtimeutil
//
//@DESCRIPTION: This component, 'bdlt::CurrentTime', provides static methods
// for retrieving the current time in Coordinated Universal Time (UTC) and in
// the local time zone of the executing process.  It also provides a facility
// for customizing the means by which the time is retrieved.
//
///Thread Safety
///-------------
// The functions provided by 'bdlt::CurrentTime' are *thread-safe* (meaning
// they may be called concurrently from multiple threads), including those that
// set and retrieve the callback function.  In addition, user-supplied callback
// functions must be *thread-safe*.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Repeatable Tests Involving Current Time
/// - - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose we are writing an application which involves dealing with the
// current time (for example, a clock displaying it).  In order to test the
// application, we would like to be able to control the time it sees.  We can
// use 'bdlt::CurrentTime' for this purpose.
//
// First, we create a sample application.  For this example, we simply have it
// retrieve the current time in several formats:
//..
//  void sampleApplication(bdlt::Datetime     *local,
//                         bdlt::Datetime     *utc,
//                         bsls::TimeInterval *now)
//      // Retrieve versions of the current time into the specified 'local',
//      // 'utc', and 'now' parameters.
//  {
//      *local = bdlt::CurrentTime::local();
//      *utc   = bdlt::CurrentTime::utc();
//      *now   = bdlt::CurrentTime::now();
//  }
//..
// Then, we create a method to test whether the application is producing the
// expected results:
//..
//  bool checkApplication(bdlt::Datetime     expectedLocal,
//                        bdlt::Datetime     expectedUtc,
//                        bsls::TimeInterval expectedNow)
//      // Return 'true' iff 'sampleApplication' returns values matching the
//      // specified expected values 'expectedLocal', 'expectedUtc', and
//      // 'expectedNow'.
//  {
//      bdlt::Datetime     local;
//      bdlt::Datetime     utc;
//      bsls::TimeInterval now;
//
//      sampleApplication(&local, &utc, &now);
//      return expectedLocal == local &&
//             expectedUtc   == utc   &&
//             expectedNow   == now;
//  }
//..
// Next, we create a class allowing us to set the current time which will be
// seen by the application:
//..
//  class TestCurrentTimeGuard {
//      // Maintain and return a "current time" value.
//    private:
//      bdlt::CurrentTime::CurrentTimeCallback d_prev;  // old callback
//
//    public:
//      TestCurrentTimeGuard();
//          // Create an object of this type, installing the handler.
//
//      ~TestCurrentTimeGuard();
//          // Destroy an object of this type, restoring the handler.
//
//      static bsls::TimeInterval time();
//          // Return 's_time'.
//
//      static bsls::TimeInterval s_time;               // the "current time"
//  };
//
//  bsls::TimeInterval TestCurrentTimeGuard::s_time;
//
//  TestCurrentTimeGuard::TestCurrentTimeGuard()
//  : d_prev(bdlt::CurrentTime::setCurrentTimeCallback(time))
//  {
//  }
//
//  TestCurrentTimeGuard::~TestCurrentTimeGuard()
//  {
//      bdlt::CurrentTime::setCurrentTimeCallback(d_prev);
//  }
//
//  bsls::TimeInterval TestCurrentTimeGuard::time()
//  {
//      return s_time;
//  }
//..
// Finally, we write a method that tests that our application is functioning
// correctly:
//..
//  void testApplication()
//      // Test the application.
//  {
//      TestCurrentTimeGuard tct;
//      TestCurrentTimeGuard::s_time = bdlt::EpochUtil::convertToTimeInterval(
//                                                   bdlt::EpochUtil::epoch());
//
//      bdlt::Datetime     local;
//      bdlt::Datetime     utc;
//      bsls::TimeInterval now;
//
//      sampleApplication(&local, &utc, &now);
//
//      TestCurrentTimeGuard::s_time += 1E6;
//      local.addSeconds(1000000);
//      utc.addSeconds(1000000);
//      now += 1E6;
//
//      assert(checkApplication(local, utc, now));
//  }
//..

#ifndef INCLUDED_BDLSCM_VERSION
#include <bdlscm_version.h>
#endif

#ifndef INCLUDED_BDLT_DATETIME
#include <bdlt_datetime.h>
#endif

#ifndef INCLUDED_BDLT_DATETIMETZ
#include <bdlt_datetimetz.h>
#endif

#ifndef INCLUDED_BDLT_EPOCHUTIL
#include <bdlt_epochutil.h>
#endif

#ifndef INCLUDED_BDLT_INTERVALCONVERSIONUTIL
#include <bdlt_intervalconversionutil.h>
#endif

#ifndef INCLUDED_BDLT_LOCALTIMEOFFSET
#include <bdlt_localtimeoffset.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_BSLS_ATOMICOPERATIONS
#include <bsls_atomicoperations.h>
#endif

#ifndef INCLUDED_BSLS_TIMEINTERVAL
#include <bsls_timeinterval.h>
#endif

namespace BloombergLP {
namespace bdlt {

                             // ==================
                             // struct CurrentTime
                             // ==================

struct CurrentTime {
    // This 'struct' provides a namespace for current-time-retrieval procedures
    // including a configurable global callback mechanism.  The use of a
    // *subset* of these procedures is thread-safe (see 'Thread Safety').
  public:
    // TYPES
    typedef bsls::TimeInterval (*CurrentTimeCallback)();
        // 'CurrentTimeCallback' is a callback function pointer for a callback
        // function that returns a 'bsls::TimeInterval' object.  This function
        // must be thread-safe in multi-threaded builds.

  private:
    static bsls::AtomicOperations::AtomicTypes::Pointer
                                 s_currenttimeCallback_p;
                                            // address of current-time callback

  public:
    // CLASS METHODS

                        // ** now functions **

    static Datetime local();
        // Return the 'Datetime' value representing the current date/time in
        // the local time zone, as provided by the installed callback function
        // and the facilities of 'LocalTimeOffset'.

    static bsls::TimeInterval now();
        // Return the 'TimeInterval' value between 'EpochUtil::epoch()' and the
        // current date/time as provided by the installed callback function.
        // Note that this value is independent of time zone.

    static Datetime utc();
        // Return the 'Datetime' value representing the current date/time in
        // Coordinated Universal Time (UTC) as provided by the installed
        // callback function.

    static DatetimeTz asDatetimeTz();
        // Return the 'DatetimeTz' value representing the current time.

                        // ** callback functions **

    static CurrentTimeCallback currentTimeCallback();
        // Return the installed function used to retrieve the interval between
        // 'EpochUtil::epoch()' and the current date/time,

    static CurrentTimeCallback
    setCurrentTimeCallback(CurrentTimeCallback callback);
        // Set the specified 'callback' as the function to be used to retrieve
        // the interval between 'EpochUtil::epoch()' and the current date/time,
        // and return the previously set function.

                        // ** default callback function **

    static bsls::TimeInterval currentTimeDefault();
        // Return the 'TimeInterval' value between 'EpochUtil::epoch()' and the
        // current date/time.  This is the default current-time callback
        // implementation for current time retrieval.  The obtained time is
        // expressed as a time interval from '00:00 UTC, January 1, 1970'.  On
        // UNIX (Solaris, LINUX and DG-UNIX) this function provides a
        // microsecond resolution.  On Windows (NT, WIN2000, 95, 98 etc) it
        // provides a resolution of 100 nanoseconds.
};

// ============================================================================
//                          INLINE DEFINITIONS
// ============================================================================

                            // -----------------
                            // class CurrentTime
                            // -----------------

// CLASS METHODS

                        // ** now functions **

inline
Datetime CurrentTime::local()
{
    Datetime now = utc();

    now.addSeconds(LocalTimeOffset::localTimeOffset(now).totalSeconds());

    return now;
}

inline
bsls::TimeInterval CurrentTime::now()
{
    return currentTimeCallback()();
}

inline
Datetime CurrentTime::utc()
{
    return EpochUtil::epoch() +
           IntervalConversionUtil::convertToDatetimeInterval(now());
}

                        // ** callback functions **

inline
CurrentTime::CurrentTimeCallback CurrentTime::currentTimeCallback()
{
    return reinterpret_cast<CurrentTimeCallback>(
              bsls::AtomicOperations::getPtrAcquire(&s_currenttimeCallback_p));
}

inline
CurrentTime::CurrentTimeCallback
CurrentTime::setCurrentTimeCallback(CurrentTimeCallback callback)
{
    BSLS_ASSERT_OPT(callback);

    CurrentTimeCallback previousCallback = currentTimeCallback();
    bsls::AtomicOperations::setPtrRelease(&s_currenttimeCallback_p,
                                          reinterpret_cast<void *>(callback));
    return previousCallback;
}

}  // close package namespace
}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// Copyright 2014 Bloomberg Finance L.P.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// ----------------------------- END-OF-FILE ----------------------------------
