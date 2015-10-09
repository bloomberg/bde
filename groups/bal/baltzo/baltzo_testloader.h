// baltzo_testloader.h                                                -*-C++-*-
#ifndef INCLUDED_BALTZO_TESTLOADER
#define INCLUDED_BALTZO_TESTLOADER

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a test implementation of the 'baltzo::Loader' protocol.
//
//@CLASSES:
//  baltzo::TestLoader: concrete implementation of 'baltzo::Loader' protocol
//
//@SEE_ALSO: baltzo_loader, baltzo_zoneinfo
//
//@DESCRIPTION: This component provides 'baltzo::TestLoader', a concrete test
// implementation of the 'baltzo::Loader' protocol for loading a
// 'baltzo::Zoneinfo' object.  The following inheritance hierarchy diagram
// shows the classes involved and their methods:
//..
//   ,------------------.
//  ( baltzo::TestLoader )
//   `------------------'
//            |      ctor
//            |      setTimeZone
//            V
//    ,--------------.
//   ( baltzo::Loader )
//    `--------------'
//                 dtor
//                 loadTimeZone
//..
// This test implementation maintains a mapping of time-zone identifiers to
// 'baltzo::Zoneinfo' objects.  Clients can associate a time-zone object with a
// time-zone identifier using the 'setTimeZone' method.  A subsequent call to
// the protocol method 'loadTimeZone' for that time-zone identifier will return
// the supplied 'baltzo::Zoneinfo' object.
//
///Usage
///-----
// The following examples demonstrate how to populate a 'baltzo::TestLoader'
// with time.zone information, and then access that information through the
// 'baltzo::Loader' protocol.
//
///Example 1: Populating a 'baltzo::TestLoader' with Time-Zone Information
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// We start by creating a 'baltzo::Zoneinfo' object, which we will eventually
// populate with a subset of data for "America/New_York":
//..
//  baltzo::Zoneinfo newYorkTimeZone;
//..
// Next, we populate 'newYorkTimeZone' with the correct time-zone identifier
// and two types of local time (standard time, and daylight-saving time):
//..
//  const char *NEW_YORK_ID = "America/New_York";
//  newYorkTimeZone.setIdentifier(NEW_YORK_ID);
//
//  baltzo::LocalTimeDescriptor est(-5 * 60 * 60, false, "EST");
//  baltzo::LocalTimeDescriptor edt(-4 * 60 * 60, true,  "EDT");
//..
// Then, we create a series of transitions between these local time
// descriptors for the years 2007-2011.  Note that the United States
// transitions to daylight saving time on the second Sunday in March, at 2am
// local time (7am UTC), and transitions back to standard time on the first
// Sunday in November at 2am local time (6am UTC).  Also note, that these rules
// for generating transitions was different prior to 2007, and may be changed
// at some point in the future.
//..
//  bdlt::Time edtTime(7, 0, 0);  // UTC transition time
//  bdlt::Time estTime(6, 0, 0);  // UTC transition time
//  static const int edtDays[5] = { 11,  9,  8, 14, 13 };
//  static const int estDays[5] = {  4,  2,  1,  7,  6 };
//  for (int year = 2007; year < 2012; ++year) {
//      int edtDay = edtDays[year - 2007];
//      int estDay = estDays[year - 2007];
//
//      bdlt::Datetime edtTransition(bdlt::Date(year, 3,  edtDay), edtTime);
//      bdlt::Datetime estTransition(bdlt::Date(year, 11, estDay), estTime);
//
//      bsls::Types::Int64 edtTransitionT =
//                            bdlt::EpochUtil::convertToTimeT64(edtTransition);
//
//      bsls::Types::Int64 estTransitionT =
//                            bdlt::EpochUtil::convertToTimeT64(estTransition);
//..
// Now, having created values representing the daylight saving time
// transitions (in UTC), we insert the transitions into the 'baltzo::Zoneinfo'
// object 'newYorkTimeZone':
//..
//      newYorkTimeZone.addTransition(edtTransitionT, edt);
//      newYorkTimeZone.addTransition(estTransitionT, est);
//  }
//..
// Now, we create a 'baltzo::TestLoader' object and configure it with
// 'newYorkTimeZone', which the test loader will associate with the identifier
// 'newYorkTimeZone.identifier()' (whose value is "America/New_York"):
//..
//  baltzo::TestLoader testLoader;
//  testLoader.setTimeZone(newYorkTimeZone);
//..
//
///Example 2: Accessing Time-Zone Information From a 'baltzo::TestLoader'
/// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// In the next example, we will use the 'baltzo::TestLoader' we initialized in
// the preceding example, to load time-zone information for New York via the
// 'baltzo::Loader' protocol.
//
// We start by creating a 'baltzo::Loader' reference to 'testLoader':
//..
//  baltzo::Loader& loader = testLoader;
//..
// Now we used the protocol method 'loadTimeZone' to load time-zone
// information for New York:
//..
//  baltzo::Zoneinfo resultNewYork;
//  int status = loader.loadTimeZone(&resultNewYork, "America/New_York");
//  assert(0 == status);
//..
// Finally, we verify that the returned time-zone information,
// 'resultNewYork', is equivalent to 'newYorkTimeZone', which we we used to
// configure 'testLoader':
//..
//  assert(newYorkTimeZone == resultNewYork);
//..

#ifndef INCLUDED_BALSCM_VERSION
#include <balscm_version.h>
#endif

#ifndef INCLUDED_BALTZO_LOADER
#include <baltzo_loader.h>
#endif

#ifndef INCLUDED_BALTZO_ZONEINFO
#include <baltzo_zoneinfo.h>
#endif

#ifndef INCLUDED_BSLMA_DEFAULT
#include <bslma_default.h>
#endif

#ifndef INCLUDED_BSL_IOSFWD
#include <bsl_iosfwd.h>
#endif

#ifndef INCLUDED_BSL_MAP
#include <bsl_map.h>
#endif

#ifndef INCLUDED_BSL_STRING
#include <bsl_string.h>
#endif

namespace BloombergLP {
namespace baltzo {
                              // ================
                              // class TestLoader
                              // ================

class TestLoader : public Loader {
    // This class provides a concrete test implementation of the 'Loader'
    // protocol (an abstract interface) for obtaining a time zone.  This test
    // implementation maintains a mapping of time-zone identifiers to
    // 'Zoneinfo' objects.  Time zone information objects are associated with a
    // time-zone identifier using the 'setTimeZone' method, and can be
    // subsequently accessed by calling the protocol method 'loadTimeZone' with
    // the same identifier.

  private:
    typedef bsl::map<bsl::string, Zoneinfo> TimeZoneMap;
        // A 'TimeZoneMap' is a type that maps a string time-zone identifier to
        // information about that time zone.

    // DATA
    TimeZoneMap d_timeZones;  // set of time zones maintained by this test
                              // loader

  private:
    // NOT IMPLEMENTED
    TestLoader(const TestLoader&);
    TestLoader& operator=(const TestLoader&);

  public:
    // TRAITS
    BSLALG_DECLARE_NESTED_TRAITS(TestLoader,
                                          bslalg::TypeTraitUsesBslmaAllocator);

    // CREATORS
    explicit TestLoader(bslma::Allocator *basicAllocator = 0);
        // Create a 'TestLoader' object.  Optionally specify a 'basicAllocator'
        // used to supply memory.  If 'basicAllocator' is 0, the currently
        // installed default allocator is used.  By default the test loader
        // will return 'ErrorCode::k_UNSUPPORTED_ID' for all time-zone
        // identifiers.

    virtual ~TestLoader();
        // Destroy this 'TestLoader' object;

    // MANIPULATORS
    void setTimeZone(const Zoneinfo& timeZone);
        // Set, to the specified 'timeZone', the time-zone information that
        // will be returned by 'loadTimeZone' for the identifier
        // 'timeZone.identifier()'.

    int setTimeZone(const char *timeZoneId,
                    const char *timeZoneData,
                    int         timeZoneDataNumBytes);
        // Set the time-zone data this test loader will return for the
        // specified 'timeZoneId' to the Zoneinfo value defined by reading the
        // specified 'timeZoneData' buffer, holding data in the Zoneinfo
        // standard binary file format, of at least the specified
        // 'timeZoneDataNumBytes'.  Return 0 on success, or a negative value
        // if an error occurs.  The behavior is undefined unless
        // 'timeZoneData' contains at least 'timeZoneDataNumBytes' bytes.

    virtual int loadTimeZone(Zoneinfo *result, const char *timeZoneId);
        // Load into the specified 'result' the time-zone information for the
        // time-zone identified by the specified 'timeZoneId'.  Return 0 on
        // success, and a non-zero value otherwise.  A return status of
        // 'ErrorCode::k_UNSUPPORTED_ID' indicates that 'timeZoneId' is not
        // recognized.  If an error occurs during the operation, 'result' will
        // be left in a valid but unspecified state.

    // ACCESSORS

                        // Aspects

    bsl::ostream& print(bsl::ostream& stream,
                        int           level = 0,
                        int           spacesPerLevel = 4) const;
        // Write the set of time zones maintained by this object to the
        // specified output 'stream' in a human-readable format, and return a
        // reference to 'stream'.  Optionally specify an initial indentation
        // 'level', whose absolute value is incremented recursively for nested
        // objects.  If 'level' is specified, optionally specify
        // 'spacesPerLevel', whose absolute value indicates the number of
        // spaces per indentation level for this and all of its nested objects.
        // If 'level' is negative, suppress indentation of the first line.  If
        // 'spacesPerLevel' is negative, format the entire output on one line,
        // suppressing all but the initial indentation (as governed by
        // 'level').  If 'stream' is not valid on entry, this operation has no
        // effect.  Note that the format is not fully specified, and can change
        // without notice.

};

// FREE OPERATORS
bsl::ostream& operator<<(bsl::ostream& stream, const TestLoader& loader);
    // Write the set of time zones maintained by the specified 'loader' to the
    // specified output 'stream' in a single-line format, and return a
    // reference to 'stream'.  If 'stream' is not valid on entry, this
    // operation has no effect.  Note that this human-readable format is not
    // fully specified, can change without notice, and is logically equivalent
    // to:
    //..
    //  print(stream, 0, -1);
    //..

}  // close package namespace

// ============================================================================
//                            INLINE DEFINITIONS
// ============================================================================

                              // ----------------
                              // class TestLoader
                              // ----------------

// CREATORS
inline
baltzo::TestLoader::TestLoader(bslma::Allocator *basicAllocator)
: d_timeZones(basicAllocator)
{
}

// FREE FUNCTIONS
inline
bsl::ostream& baltzo::operator<<(bsl::ostream&     stream,
                                 const TestLoader& loader)
{
    return loader.print(stream, 0, -1);
}

}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// Copyright 2015 Bloomberg Finance L.P.
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
