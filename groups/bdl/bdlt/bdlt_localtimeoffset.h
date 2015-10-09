// bdlt_localtimeoffset.h                                             -*-C++-*-
#ifndef INCLUDED_BDLT_LOCALTIMEOFFSET
#define INCLUDED_BDLT_LOCALTIMEOFFSET

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide utilities to retrieve the local time offset.
//
//@CLASSES:
//   bdlt::LocalTimeOffset: namespace for local time offset functions
//
//@SEE_ALSO: bsls_timeinterval, bsls_systemtime, bsls_currenttime
//
//@DESCRIPTION: This component provides a 'struct', 'bdlt::LocalTimeOffset', in
// which are defined a series of static methods for using a callback function
// to retrieve the local time offset (the difference between the currently
// executing task's local time and UTC time) at a specified UTC date and time.
// 'LocalTimeoffset' provides a function 'localTimeOffset' that delegates to
// the currently installed local time offset callback.  By default,
// 'localTimeOffsetDefault' is installed as the local time offset callback.
// Clients can configure the default callback function by calling the
// 'setLocalTimeOffsetCallback' function.
//
///Thread Safety
///-------------
// The functions provided by 'bdlt::LocalTimeOffset' are *thread-safe* (meaning
// they may be called concurrently from multiple threads), including those that
// set and retrieve the callback function.  In addition, user-supplied callback
// functions must be *thread-safe*.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Basic 'bdlt::LocalTimeOffset' Usage
/// - - - - - - - - - - - - - - - - - - - - - - -
// This example demonstrates how to use 'bdlt::LocalTimeOffset'.
//
// First, obtain the current UTC time - ignoring milliseconds - using
// 'bsls::SystemTime' and 'bdlt::EpochUtil' (note that clients may prefer
// 'bdlt_currenttime', which is not shown here for dependency reasons):
//..
//  bsls::TimeInterval now = bsls::SystemTime::nowRealtimeClock();
//
//  bdlt::Datetime utc = bdlt::EpochUtil::epoch() +
//                              bdlt::DatetimeInterval(0, 0, 0, now.seconds());
//..
// Then, obtain the local time offset:
//..
//  bsls::TimeInterval localOffset =
//                                 bdlt::LocalTimeOffset::localTimeOffset(utc);
//..
// Next, add the offset to the UTC time to obtain the local time:
//..
//  bsls::TimeInterval local = utc;
//  local.addSeconds(localOffset.seconds());
//..
// Finally, stream the two time values to 'stdout':
//..
//  bsl::cout << "utc   = " << utc << bsl::endl;
//  bsl::cout << "local = " << local << bsl::endl;
//..
// The streaming operator produces output in the following format on 'stdout':
//..
//  utc   = ddMONyyyy_hh:mm::ss.000
//  local = ddMONyyyy_hh:mm::ss.000
//..
//
///Example 2: Using the Local Time Offset Callback
///- - - - - - - - - - - - - - - - - - - - - - - -
// Suppose one has to provide time stamp values that always reflect local time
// for a given location, even when local time transitions into and out of
// daylight saving time.  Further suppose that one must do this quite often
// (e.g., for every record in a high frequency log), so the performance of the
// default method for calculating local time offset is not adequate.  Creation
// and installation of a specialized user-defined callback for local time
// offset allows one to solve this problem.
//
// First, create a utility class that provides a method of type
// 'bdlt::LocalTimeOffset::LocalTimeOffsetCallback' that is valid for the
// location of interest (New York) for the period of interest (the year 2013).
//..
//  struct MyLocalTimeOffsetUtilNewYork2013 {
//
//    private:
//      // DATA
//      static int            s_useCount;
//      static bdlt::Datetime s_startOfDaylightSavingTime;  // UTC Datetime
//      static bdlt::Datetime s_resumptionOfStandardTime;   // UTC Datetime
//
//    public:
//      // CLASS METHODS
//      static bsls::TimeInterval localTimeOffset(
//                                          const bdlt::Datetime& utcDatetime);
//          // Return a 'bsls::TimeInterval' value representing the difference
//          // between the local time for the "America/New_York" timezone and
//          // UTC time at the specified 'utcDatetime'.  The behavior is
//          // undefined unless '2013 == utcDatetime.date().year()'.
//
//      static int useCount();
//          // Return the number of invocations of the 'localTimeOffset' since
//          // the start of the process.
//  };
//
//  // DATA
//  int MyLocalTimeOffsetUtilNewYork2013::s_useCount = 0;
//
//  bdlt::Datetime
//  MyLocalTimeOffsetUtilNewYork2013::s_startOfDaylightSavingTime(2013,
//                                                                   3,
//                                                                  10,
//                                                                   7);
//  bdlt::Datetime
//  MyLocalTimeOffsetUtilNewYork2013::s_resumptionOfStandardTime(2013,
//                                                                 11,
//                                                                  3,
//                                                                  6);
//
//  // CLASS METHODS
//  bsls::TimeInterval MyLocalTimeOffsetUtilNewYork2013::localTimeOffset(
//                                           const bdlt::Datetime& utcDatetime)
//  {
//      assert(2013 == utcDatetime.date().year());
//
//      ++s_useCount;
//      int seconds = utcDatetime < s_startOfDaylightSavingTime ? -18000 :
//                    utcDatetime < s_resumptionOfStandardTime  ? -14400 :
//                                                                -18000;
//      return bsls::TimeInterval(seconds, 0);
//  }
//
//  int MyLocalTimeOffsetUtilNewYork2013::useCount()
//  {
//      return s_useCount;
//  }
//..
// Note that the transition times into and out of daylight saving for New York
// are given in UTC.  Also notice that we do not attempt to make the
// 'localTimeOffset' method 'inline', since we must take its address to install
// it as the callback.
//
// Then, we install this 'localTimeOffset' as the local time offset callback.
//..
//  bdlt::LocalTimeOffset::LocalTimeOffsetCallback defaultCallback =
//                           bdlt::LocalTimeOffset::setLocalTimeOffsetCallback(
//                                          &MyLocalTimeOffsetUtilNewYork2013::
//                                                            localTimeOffset);
//
//  assert(bdlt::LocalTimeOffset::localTimeOffsetDefault == defaultCallback);
//  assert(&MyLocalTimeOffsetUtilNewYork2013::localTimeOffset
//                        == bdlt::LocalTimeOffset::localTimeOffsetCallback());
//..
// Now, we can use the 'bdlt::LocalTimeOffset::localTimeOffset' method to
// obtain the local time offsets in New York on several dates of interest.  The
// increasing values from our 'useCount' method assures us that the callback we
// defined is indeed being used.
//..
//  assert(0 == MyLocalTimeOffsetUtilNewYork2013::useCount());
//
//  bsls::Types::Int64 offset;
//  bdlt::Datetime     newYearsDay(2013,  1,  1);
//  bdlt::Datetime     independenceDay(2013,  7,  4);
//  bdlt::Datetime     newYearsEve(2013, 12, 31);
//
//  offset = bdlt::LocalTimeOffset::localTimeOffset(newYearsDay).seconds();
//  assert(-5 * 3600 == offset);
//  assert(        1 == MyLocalTimeOffsetUtilNewYork2013::useCount());
//
//  offset = bdlt::LocalTimeOffset::localTimeOffset(independenceDay).seconds();
//  assert(-4 * 3600 == offset);
//  assert(        2 == MyLocalTimeOffsetUtilNewYork2013::useCount());
//
//  offset = bdlt::LocalTimeOffset::localTimeOffset(newYearsEve).seconds();
//  assert(-5 * 3600 == offset);
//  assert(        3 == MyLocalTimeOffsetUtilNewYork2013::useCount());
//..
// Finally, to be neat, we restore the local time offset callback to the
// default callback:
//..
//  bdlt::LocalTimeOffset::setLocalTimeOffsetCallback(defaultCallback);
//  assert(&bdlt::LocalTimeOffset::localTimeOffsetDefault
//                        == bdlt::LocalTimeOffset::localTimeOffsetCallback());
//..

#ifndef INCLUDED_BDLSCM_VERSION
#include <bdlscm_version.h>
#endif

#ifndef INCLUDED_BDLT_DATETIME
#include <bdlt_datetime.h>
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

                            // =====================
                            // class LocalTimeOffset
                            // =====================

struct LocalTimeOffset {
    // This 'struct' provides a namespace for local-time-offset procedures
    // including a configurable global callback mechanism.  The use of these
    // procedures is thread-safe (see 'Thread Safety').

    // TYPES
    typedef bsls::TimeInterval (*LocalTimeOffsetCallback)(
                                                  const Datetime& utcDatetime);
        // 'LocalTimeOffsetCallback' is an alias for the type of a function
        // that returns a 'bsls::TimeInterval' value representing the
        // difference between local time and UTC time at the specified
        // 'utcDatetime'.  This function must be thread-safe in multi-threaded
        // builds.  Note that the installed callback function must have
        // geographic information specifying the local timezone.

  private:
    static bsls::AtomicOperations::AtomicTypes::Pointer
                                  s_localTimeOffsetCallback_p;
                                       // address of local-time-offset callback

  public:
    // CLASS METHODS

                        // ** computation method **

    static bsls::TimeInterval localTimeOffset(const Datetime& utcDatetime);
        // Return a 'bsls::TimeInterval' value representing the difference
        // between local time and UTC time at the specified 'utcDatetime'.
        // This method uses the currently installed local-time-offset callback
        // mechanism.

                        // ** default callback **

    static bsls::TimeInterval localTimeOffsetDefault(
                                                  const Datetime& utcDatetime);
        // Return a 'bsls::TimeInterval' value representing the difference
        // between local time and UTC time at the specified 'utcDatetime'.
        // Note that the local time zone is determined by the 'TZ' environment
        // variable in the same manner as the 'localtime' POSIX function.

                        // ** set callback **

    static LocalTimeOffsetCallback setLocalTimeOffsetCallback(
                                             LocalTimeOffsetCallback callback);
        // Set the specified 'callback' as the function to be used to return a
        // 'bsls::TimeInterval' value representing the difference between
        // local time and UTC time at a specified UTC date and time.  Return
        // the previously installed 'LocalTimeOffsetCallback' function.  The
        // behavior is undefined unless '0 != callback'.

                        // ** get current callback **

    static LocalTimeOffsetCallback localTimeOffsetCallback();
        // Return the currently installed 'LocalTimeOffsetCallback' function.
};

// ============================================================================
//                           INLINE DEFINITIONS
// ============================================================================

                        // ** computation method **

inline
bsls::TimeInterval LocalTimeOffset::
                                   localTimeOffset(const Datetime& utcDatetime)
{
    return localTimeOffsetCallback()(utcDatetime);
}

                        // ** set callback **

inline
LocalTimeOffset::LocalTimeOffsetCallback
  LocalTimeOffset::setLocalTimeOffsetCallback(LocalTimeOffsetCallback callback)
{
    BSLS_ASSERT_SAFE(callback);

    LocalTimeOffsetCallback previousCallback = localTimeOffsetCallback();
    bsls::AtomicOperations::setPtrRelease(&s_localTimeOffsetCallback_p,
                                          reinterpret_cast<void *>(callback));
    return previousCallback;
}

                        // ** get current callback **

inline
LocalTimeOffset::LocalTimeOffsetCallback
                                     LocalTimeOffset::localTimeOffsetCallback()
{
    return reinterpret_cast<LocalTimeOffsetCallback>(
          bsls::AtomicOperations::getPtrAcquire(&s_localTimeOffsetCallback_p));
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
