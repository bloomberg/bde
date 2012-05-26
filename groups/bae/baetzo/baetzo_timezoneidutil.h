// baetzo_timezoneidutil.h                                            -*-C++-*-
#ifndef INCLUDED_BAETZO_TIMEZONEIDUTIL
#define INCLUDED_BAETZO_TIMEZONEIDUTIL

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide utilities for mapping between time zone identifier systems.
//
//@CLASSES:
//  baetzo_TimeZoneIdUtil: utilities for mapping time zone identifiers
//
//@SEE_ALSO: bsitzo_tzdftimezoneutil
//
//@AUTHOR: Steven Breitstein (sbreitstein)
//
//@DESCRIPTION: This component provides a namespace, 'baetzo_TimeZoneIdUtil',
// containing utility functions for mapping time zone identifiers between
// different naming systems.
//
// Currently, the only supported mapping is between Windows time zone
// identifiers and IANA (Olson) time zone identifers.  The mapping is defined
// at:
// 'http://unicode.org/repos/cldr-tmp/trunk/diff/supplemental/zone_tzid.html',
// where the default IANA time zone identifer for a given Windows time zone
// identifier corresponds to the table entry with a "Region" value of "001".
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Converting Between Windows and IANA (Olson) Time Zone Identifiers
/// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// First, given a Windows time zone identifer, one can convert it to the
// designated equivilant IANA (Olson) identifier using the
// 'getTimeZoneIdFromWindowsTimeZoneId' method.
//..
//  int         rc;
//  const char *timeZoneId;
//  rc = baetzo_TimeZoneIdUtil::getTimeZoneIdFromWindowsTimeZoneId(
//                                           &timeZoneId,
//                                           "Central Standard Time (Mexico))";
//  assert(0 == rc);
//  assert(0 == strcmp("America/Mexico_City", timeZoneId);
//..
// Next, one can use the 'getWindowsTimeZoneIdFromTimeZoneId' method, to
// perform the inverse mapping.
//..
//  const char *windowsTimeZoneId;
//  rc = baetzo_TimeZoneIdUtil::getWindowsTimeZoneIdFromTimeZoneId(
//                                                      &windowsTimeZoneId,
//                                                      "America/Mexico_City");
//  assert(0 == rc);
//  assert(0 == strcmp("Central Standard Time (Mexico)", windowsTimeZoneId);
//..
// Finally, neither method provides a mapping if given an unknown time zone
// identifier.
//
//  timeZoneId = 0xdeadbeef;
//  rc = baetzo_TimeZoneIdUtil::getTimeZoneIdFromWindowsTimeZoneId(&timeZoneId,
//                                                                 "ABCZ");
//  assert(0          != rc);
//  assert(0xdeadbeef == timeZoneId);
//
//  windowsTimeZoneId = 0xcafed00d;
//  rc = baetzo_TimeZoneIdUtil::getWindowsTimeZoneIdFromTimeZoneId(
//                                                         &windowsTimeZoneId,
//                                                         "XYZA");
//  assert(0          != rc);
//  assert(0xcafed00d == windowsTimeZoneId);
//..

#ifndef INCLUDED_BAESCM_VERSION
#include <baescm_version.h>
#endif

namespace BloombergLP {

                        // ===========================
                        // class baetzo_TimeZoneIdUtil
                        // ===========================

struct baetzo_TimeZoneIdUtil {
    // This 'struct' provides a namespace for utility functions that convert
    // time zone identifiers to- and from- IANA (Olson) and other systems of
    // time zone identifiers.

    // CLASS METHODS
    static int getTimeZoneIdFromWindowsTimeZoneId(
                                               const char **timeZoneId,
                                               const char  *windowsTimeZoneId);
        // Load the specified 'timeZoneId' with the address of the default IANA
        // (Olson) time zone identifier corresponding to the specified
        // 'windowsTimeZoneId'.  Return 0 on success, and non-zero value with
        // no other effect otherwise.  The returned address is valid for the
        // life-time of the process.

    static int getWindowsTimeZoneIdFromTimeZoneId(
                                               const char **windowsTimeZoneId,
                                               const char  *timeZoneId);
        // Load the specified 'windowsTimeZoneId' with the Windows time zone
        // identifier that has the specified 'timeZoneId' -- an IANA (Olson)
        // time zone identifier -- as its default equivalent.  Return 0 on
        // success, and non-zero value with no other effect otherwise.  The
        // returned address is valid for the life-time of the process.
};

// ============================================================================
//                      INLINE FUNCTION DEFINITIONS
// ============================================================================

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2012
//      All Rights Reserved.
//      Property of Bloomberg L.P.  (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
