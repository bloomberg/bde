// baetzo_loader.h                                                    -*-C++-*-
#ifndef INCLUDED_BAETZO_LOADER
#define INCLUDED_BAETZO_LOADER

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide a protocol for obtaining information about a time zone.
//
//@CLASSES:
//  baetzo_Loader: protocol for obtaining time-zone information
//
//@SEE_ALSO: baetzo_zoneinfo, baetzo_datafileloader, baetzo_testloader
//
//@AUTHOR: Stefano Pacifico (spacifico1), Henry Verschell (hverschell)
//
//@DESCRIPTION: This component provides a protocol, 'baetzo_Loader', for
// obtaining information about a time zone from some data-source.  The
// protocol's primary method, 'loadTimeZone', loads a 'baetzo_Zoneinfo' object
// describing the time zone identified by a user supplied time-zone string
// identifier.
//
///Usage
///-----
// This section illustrates intended usage of this component.
//
///Example 1: Implementing 'baetzo_Loader'
///- - - - - - - - - - - - - - - - - - - -
// This example demonstrates an implementation of 'baetzo_Loader' that can only
// return data for "America/New_York".
//
// Note that in general, an implementation of 'baetzo_Loader' should obtain
// time-zone information from an external data source (see
// 'baetzo_datafileloader').
//
// First, we define the interface of our implementation:
//..
//  class MyLoaderImp : public baetzo_Loader {
//      // This class provides a concrete implementation of the 'baetzo_Loader'
//      // protocol (an abstract interface) for obtaining a time zone.  This
//      // test implementation contains only partial data of the
//      // "America/New_York" time zone, and is unable to obtain time-zone
//      // information for any other time zones.
//
//    public:
//      // CREATORS
//      MyLoaderImp();
//          // Create a 'MyLoaderImp' object.
//
//      ~MyLoaderImp();
//          // Destroy this object.
//
//      // MANIPULATORS
//      virtual int loadTimeZone(baetzo_Zoneinfo *result,
//                               const char      *timeZoneId);
//          // Load into the specified 'result' the "Zoneinfo" time zone
//          // information for the time zone identified by the specified
//          // 'timeZoneId'.  Return 0 on success, and non-zero otherwise.
//  };
//..
// Then, we implement the creators, trivially, as this class contains no
// instance data members.
//..
//  MyLoaderImp::MyLoaderImp()
//  {
//  }
//
//  MyLoaderImp::~MyLoaderImp()
//  {
//  }
//..
// Next, we implement the 'loadTimeZone' function:
//..
//  int MyLoaderImp::loadTimeZone(baetzo_Zoneinfo *result,
//                                 const char      *timeZoneId)
//  {
//..
// Then, we check the 'timeZoneId' equals to "America/New_York" as this
// implementation is designed to demonstrate only one time zone:
//..
//      if (0 != strcmp("America/New_York", timeZoneId)) {
//          return 1;                                                 // RETURN
//      }
//..
// Next, we load 'result' with the time zone identifier for New York
//..
//      result->setIdentifier("America/New_York");
//..
// Then, we create two local-time descriptors, one for standard time and one
// for daylight-saving time.
//..
//      baetzo_LocalTimeDescriptor edt(-14400, true, "EDT");
//      baetzo_LocalTimeDescriptor est(-18000, false, "EST");
//..
// Next, we create a series of transitions between these local time descriptors
// for the years 2007-2011.  Note that the United States transitions to
// daylight savings time on the second Sunday in March, at 2am local time (7am
// UTC), and transition back to standard time on the first Sunday in November
// at 2am local time (6am UTC), resulting in an even number of transitions.
// Also note that, the rules for generating these transitions were changed in
// 2007, and may be changed again at some point in the future.
//..
//      static const bdet_Datetime TRANSITION_TIMES[] = {
//          bdet_Datetime(2007,  3, 11, 7),
//          bdet_Datetime(2007, 11,  4, 6),
//          bdet_Datetime(2008,  3,  9, 7),
//          bdet_Datetime(2008, 11,  2, 6),
//          bdet_Datetime(2009,  3,  8, 7),
//          bdet_Datetime(2009, 11,  1, 6),
//          bdet_Datetime(2010,  3, 14, 7),
//          bdet_Datetime(2010, 11,  7, 6),
//          bdet_Datetime(2011,  3, 13, 7),
//          bdet_Datetime(2011, 11,  6, 6),
//      };
//      const int NUM_TRANSITION_TIMES =
//                          sizeof TRANSITION_TIMES / sizeof *TRANSITION_TIMES;
//
//      assert(0 == NUM_TRANSITION_TIMES % 2);
//
//      for (int i = 0; i < NUM_TRANSITION_TIMES; i += 2) {
//          result->addTransition(
//               bdetu_Epoch::convertToTimeT64(TRANSITION_TIMES[i]),
//               edt);
//          result->addTransition(
//           bdetu_Epoch::convertToTimeT64(TRANSITION_TIMES[i + 1]),
//           est);
//      }
//      return 0;
//  }
//..
// Finally, we define a function 'f' that instantiates an object of type
// 'MyLoaderImp':
//..
//  void f() {
//      MyLoaderImp a;
//  }
//..
///Example 2: Using a 'baetzo_Loader'
/// - - - - - - - - - - - - - - - - -
// In this example we use a 'MyLoaderImpl' to load the data for one time zone,
// and print the time transitions, contained in the obtained time zone data, to
// standard output.  Note that, the implementation of this example is for
// illustrative purpose only, and in general, clients should use an
// implementation that loads data from an external data source (e.g.,
// 'baetzo_datafileloader').
//
// First, we obtain a 'MyLoaderImpl' reference to 'myLoader' we constructed in
// the previous example:
//..
//  baetzo_Loader& loader = myLoader;
//..
// Now, we load the time zone data for New York:
//..
//  baetzo_Zoneinfo nyTimeZone;
//  if (0 != loader.loadTimeZone(&nyTimeZone, "America/New_York")) {
//     bsl::cout << "Failed to find time zone data." << bsl::endl;
//     return -1;
//  }
//..
// Then, we verify some basic properties of the time zone:
//..
//  assert("America/New_York" == nyTimeZone.identifier());
//..
// Finally, we write to standard output the information about the
// daylight-savings time transitions of the loaded time zone, in New York local
// time:
//..
//  baetzo_Zoneinfo::TransitionConstIterator tIt =
//                                               nyTimeZone.beginTransitions();
//  for (; tIt != nyTimeZone.endTransitions(); ++tIt) {
//     bdet_Datetime transition =
//                   bdetu_Epoch::convertFromTimeT64(tIt->utcTransitionTime());
//     const baetzo_LocalTimeDescriptor& descriptor = tIt->descriptor();
//
//     bsl::cout << "transition to "
//               << descriptor.description()
//               << " at "
//               << transition
//               << " UTC"
//               << bsl::endl;
//  }
//..
// The resulting output will look like:
//..
//  transition to EDT at 11MAR2007_07:00:00.000 UTC
//  transition to EST at 04NOV2007_06:00:00.000 UTC
//  transition to EDT at 09MAR2008_07:00:00.000 UTC
//  transition to EST at 02NOV2008_06:00:00.000 UTC
//  transition to EDT at 08MAR2009_07:00:00.000 UTC
//  transition to EST at 01NOV2009_06:00:00.000 UTC
//  transition to EDT at 14MAR2010_07:00:00.000 UTC
//  transition to EST at 07NOV2010_06:00:00.000 UTC
//  transition to EDT at 13MAR2011_07:00:00.000 UTC
//  transition to EST at 06NOV2011_06:00:00.000 UTC
//..

#ifndef INCLUDED_BAESCM_VERSION
#include <baescm_version.h>
#endif

namespace BloombergLP {

class baetzo_Zoneinfo;

                        // ===================
                        // class baetzo_Loader
                        // ===================

class baetzo_Loader {
    // This class provides a protocol (a pure abstract interface) for loading a
    // time zone object.

  public:
    // CREATORS
    virtual ~baetzo_Loader();
        // Destroy this object.

    // MANIPULATORS
    virtual int loadTimeZone(baetzo_Zoneinfo *result,
                             const char      *timeZoneId) = 0;
        // Load into the specified 'result' the time zone information for the
        // time zone identified by the specified 'timeZoneId'.  Return 0 on
        // success, and a non-zero value otherwise.  A return status of
        // 'baetzo_ErrorCode::BAETZO_UNSUPPORTED_ID' indicates that
        // 'timeZoneId' is not recognized.  If an error occurs during this
        // operation, 'result' will be left in a valid, but otherwise
        // unspecified state.
};

// ============================================================================
//                      INLINE FUNCTION DEFINITIONS
// ============================================================================

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
