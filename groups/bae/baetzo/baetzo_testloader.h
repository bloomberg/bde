// baetzo_testloader.h                                                -*-C++-*-
#ifndef INCLUDED_BAETZO_TESTLOADER
#define INCLUDED_BAETZO_TESTLOADER

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide a test implementation of the 'baetzo_Loader' protocol.
//
//@CLASSES:
//  baetzo_TestLoader: concrete implementation of the 'baetzo_Loader' protocol
//
//@SEE_ALSO: baetzo_loader, baetzo_zoneinfo
//
//@AUTHOR: Stefano Pacifico (spacifico1), Henry Verschell (hverschell)
//
//@DESCRIPTION: This component provides a concrete test implementation of the
// 'baetzo_Loader' protocol for loading a 'baetzo_Zoneinfo' object.  The
// following inheritance hierarchy diagram shows the classes involved and
// their methods:
//..
//   ,-----------------.
//  ( baetzo_TestLoader )
//   `-----------------'
//            |      ctor
//            |      setTimeZone
//            V
//    ,-------------.
//   ( baetzo_Loader )
//    `-------------'
//                 dtor
//                 loadTimeZone
//..
// This test implementation maintains a mapping of time-zone identifiers to
// 'baetzo_Zoneinfo' objects.  Clients can associate a time-zone object
// with a time-zone identifier using the 'setTimeZone' method.   A subsequent
// call to the protocol method 'loadTimeZone' for that time-zone identifier
// will return the supplied 'baetzo_Zoneinfo' object.
//
///Usage
///-----
// The following examples demonstrate how to populate a 'baetzo_TestLoader'
// with time.zone information, and then access that information through the
// 'baetzo_Loader' protocol.
//
///Example 1: Populating a 'baetzo_TestLoader' with Time-Zone Information
/// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// We start by creating a 'baetzo_Zoneinfo' object, which we will eventually
// populate with a subset of data for "America/New_York":
//..
//  baetzo_Zoneinfo newYorkTimeZone;
//..
// Next, we populate 'newYorkTimeZone' with the correct time-zone identifier
// and two types of local time (standard time, and daylight-savings time):
//..
//  const char *NEW_YORK_ID = "America/New_York";
//  newYorkTimeZone.setIdentifier(NEW_YORK_ID);
//
//  baetzo_LocalTimeDescriptor est(-5 * 60 * 60, false, "EST");
//  baetzo_LocalTimeDescriptor edt(-4 * 60 * 60, true,  "EDT");
//..
// Then, we create a series of transitions between these local time
// descriptors for the years 2007-2011.  Note that the United States
// transitions to daylight savings time on the second Sunday in March, at 2am
// local time (7am UTC), and transitions back to standard time on the first
// Sunday in November at 2am local time (6am UTC).  Also note, that these rules
// for generating transitions was different prior to 2007, and may be changed
// at some point in the future.
//..
//  bdet_Time edtTime(7, 0, 0);  // UTC transition time
//  bdet_Time estTime(6, 0, 0);  // UTC transition time
//  for (int year = 2007; year < 2012; ++year) {
//      int edtDay =
//                bdetu_DayOfWeek::weekdayInMonth(year,
//                                                3,
//                                                bdet_DayOfWeek::BDET_SUNDAY,
//                                                2);
//      int estDay =
//                bdetu_DayOfWeek::weekdayInMonth(year,
//                                                11,
//                                                bdet_DayOfWeek::BDET_SUNDAY,
//                                                1);
//      bdet_Datetime edtTransition(bdet_Date(year, 3,  edtDay), edtTime);
//      bdet_Datetime estTransition(bdet_Date(year, 11, estDay), estTime);
//
//      bsls_Types::Int64 edtTransitionT =
//                     bdetu_Epoch::convertToTimeT64(edtTransition);
//
//      bsls_Types::Int64 estTransitionT =
//                     bdetu_Epoch::convertToTimeT64(estTransition);
//..
// Now, having created values representing the daylight savings time
// transitions (in UTC), we insert the transitions into the 'baetzo_Zoneinfo'
// object 'newYorkTimeZone':
//..
//      newYorkTimeZone.addTransition(edtTransitionT, edt);
//      newYorkTimeZone.addTransition(estTransitionT, est);
//  }
//..
// Now, we create a 'baetzo_TestLoader' object and configure it with
// 'newYorkTimeZone', which the test loader will associate with the identifier
// 'newYorkTimeZone.identifier()' (whose value is "America/New_York"):
//..
//  baetzo_TestLoader testLoader;
//  testLoader.setTimeZone(newYorkTimeZone);
//..
//
///Example 2: Accessing Time-Zone Information From a 'baetzo_TestLoader'
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// In the next example, we will use the 'baetzo_TestLoader' we initialized in
// the preceding example, to load time-zone information for New York via the
// 'baetzo_Loader' protocol.
//
// We start by creating a 'baetzo_Loader' reference to 'testLoader':
//..
//  baetzo_Loader& loader = testLoader;
//..
// Now we used the protocol method 'loadTimeZone' to load time-zone
// information for New York:
//..
//  baetzo_Zoneinfo resultNewYork;
//  int status = loader.loadTimeZone(&resultNewYork, "America/New_York");
//  assert(0 == status);
//..
// Finally, we verify that the returned time-zone information,
// 'resultNewYork', is equivalent to 'newYorkTimeZone', which we we used to
// configure 'testLoader':
//..
//  assert(newYorkTimeZone == resultNewYork);
//..

#ifndef INCLUDED_BAESCM_VERSION
#include <baescm_version.h>
#endif

#ifndef INCLUDED_BAETZO_LOADER
#include <baetzo_loader.h>
#endif

#ifndef INCLUDED_BAETZO_ZONEINFO
#include <baetzo_zoneinfo.h>
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

                        // =======================
                        // class baetzo_TestLoader
                        // =======================

class baetzo_TestLoader : public baetzo_Loader {
    // This class provides a concrete test implementation of the
    // 'baetzo_Loader' protocol (an abstract interface) for obtaining a time
    // zone.  This test implementation maintains a mapping of time-zone
    // identifiers to 'baetzo_Zoneinfo' objects.  Time zone information objects
    // are associated with a time-zone identifier using the 'setTimeZone'
    // method, and can be subsequently accessed by calling the protocol method
    // 'loadTimeZone' with the same identifier.

  private:
    typedef bsl::map<bsl::string, baetzo_Zoneinfo> TimeZoneMap;
        // A 'TimeZoneMap' is a type that maps a string time-zone identifier to
        // information about that time zone.

    // DATA
    TimeZoneMap      d_timeZones;    // set of time zones maintained by this
                                     // test loader

  private:
    // NOT IMPLEMENTED
    baetzo_TestLoader(const baetzo_TestLoader&);
    baetzo_TestLoader& operator=(const baetzo_TestLoader&);

  public:
    // CREATORS
    explicit baetzo_TestLoader(bslma_Allocator *basicAllocator = 0);
        // Create a 'baetzo_TestLoader' object.  Optionally specify a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0,
        // the currently installed default allocator is used.  By default the
        // test loader will return 'baetzo_ErrorCode::BAETZO_UNSUPPORTED_ID'
        // for all time-zone identifiers.

    virtual ~baetzo_TestLoader();
        // Destroy this 'baetzo_TestLoader' object;

    // MANIPULATORS
    void setTimeZone(const baetzo_Zoneinfo& timeZone);
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

    virtual int loadTimeZone(baetzo_Zoneinfo *result, const char *timeZoneId);
        // Load into the specified 'result' the time-zone information for the
        // time-zone identified by the specified 'timeZoneId'.  Return 0 on
        // success, and a non-zero value otherwise.  A return status of
        // 'baetzo_ErrorCode::BAETZO_UNSUPPORTED_ID' indicates that
        // 'timeZoneId' is not recognized.  If an error occurs during the
        // operation, 'result' will be left in a valid but unspecified state.

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
bsl::ostream& operator<<(bsl::ostream&            stream,
                         const baetzo_TestLoader& loader);
    // Write the set of time zones maintained by the specified 'loader' to the
    // specified output 'stream' in a single-line format, and return a
    // reference to 'stream'.  If 'stream' is not valid on entry, this
    // operation has no effect.  Note that this human-readable format is not
    // fully specified, can change without notice, and is logically equivalent
    // to:
    //..
    //  print(stream, 0, -1);
    //..

// ============================================================================
//                      INLINE FUNCTION DEFINITIONS
// ============================================================================

                        // -----------------------
                        // class baetzo_TestLoader
                        // -----------------------

// CREATORS
inline
baetzo_TestLoader::baetzo_TestLoader(bslma_Allocator *basicAllocator)
: d_timeZones(basicAllocator)
{
}

// FREE FUNCTIONS
inline
bsl::ostream& operator<<(bsl::ostream&            stream,
                         const baetzo_TestLoader& loader)
{
    return loader.print(stream, 0, -1);
}

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2011
//      All Rights Reserved.
//      Property of Bloomberg L.P.  (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
