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
// IANA (Olson) time zone identifiers and those in other naming systems.
//
///Windows Time Zone Identifiers
///-----------------------------
// Currently, the only supported mapping is between Windows time zone
// identifiers and IANA (Olson) time zone identifiers.  The mapping is defined
// at:
// 'http://unicode.org/repos/cldr-tmp/trunk/diff/supplemental/zone_tzid.html',
// where the default IANA time zone identifier for a given Windows time zone
// identifier corresponds to the table entry with a "Region" value of "001".
// The 99 entries are:
//..
//  +--------------------------------+--------+---------------------+
//  |Windows Time Zone Identifier    | Region | Olson Identifier    |
//  +--------------------------------+--------+---------------------+
//  |      AUS Central Standard Time |   001  | Australia/Darwin    |
//  |      AUS Eastern Standard Time |   001  | Australia/Sydney    |
//  |      Afghanistan Standard Time |   001  | Asia/Kabul          |
//  |          Alaskan Standard Time |   001  | America/Anchorage   |
//  |             Arab Standard Time |   001  | Asia/Riyadh         |
//  |          Arabian Standard Time |   001  | Asia/Dubai          |
//  |           Arabic Standard Time |   001  | Asia/Baghdad        |
//  |        Argentina Standard Time |   001  | America/Buenos_Aires|
//  |         Atlantic Standard Time |   001  | America/Halifax     |
//  |       Azerbaijan Standard Time |   001  | Asia/Baku           |
//  |           Azores Standard Time |   001  | Atlantic/Azores     |
//  |            Bahia Standard Time |   001  | America/Bahia       |
//  |       Bangladesh Standard Time |   001  | Asia/Dhaka          |
//  |   Canada Central Standard Time |   001  | America/Regina      |
//  |       Cape Verde Standard Time |   001  | Atlantic/Cape_Verde |
//  |         Caucasus Standard Time |   001  | Asia/Yerevan        |
//  |   Cen. Australia Standard Time |   001  | Australia/Adelaide  |
//  |  Central America Standard Time |   001  | America/Guatemala   |
//  |     Central Asia Standard Time |   001  | Asia/Almaty         |
//  |Central Brazilian Standard Time |   001  | America/Cuiaba      |
//  |   Central Europe Standard Time |   001  | Europe/Budapest     |
//  | Central European Standard Time |   001  | Europe/Warsaw       |
//  |  Central Pacific Standard Time |   001  | Pacific/Guadalcanal |
//  |          Central Standard Time |   001  | America/Chicago     |
//  | Central Standard Time (Mexico) |   001  | America/Mexico_City |
//  |            China Standard Time |   001  | Asia/Shanghai       |
//  |         Dateline Standard Time |   001  | Etc/GMT+12          |
//  |        E. Africa Standard Time |   001  | Africa/Nairobi      |
//  |     E. Australia Standard Time |   001  | Australia/Brisbane  |
//  |        E. Europe Standard Time |   001  | Asia/Nicosia        |
//  | E. South America Standard Time |   001  | America/Sao_Paulo   |
//  |          Eastern Standard Time |   001  | America/New_York    |
//  |            Egypt Standard Time |   001  | Africa/Cairo        |
//  |     Ekaterinburg Standard Time |   001  | Asia/Yekaterinburg  |
//  |              FLE Standard Time |   001  | Europe/Kiev         |
//  |             Fiji Standard Time |   001  | Pacific/Fiji        |
//  |              GMT Standard Time |   001  | Europe/London       |
//  |              GTB Standard Time |   001  | Europe/Bucharest    |
//  |         Georgian Standard Time |   001  | Asia/Tbilisi        |
//  |        Greenland Standard Time |   001  | America/Godthab     |
//  |        Greenwich Standard Time |   001  | Atlantic/Reykjavik  |
//  |         Hawaiian Standard Time |   001  | Pacific/Honolulu    |
//  |            India Standard Time |   001  | Asia/Calcutta       |
//  |             Iran Standard Time |   001  | Asia/Tehran         |
//  |           Israel Standard Time |   001  | Asia/Jerusalem      |
//  |           Jordan Standard Time |   001  | Asia/Amman          |
//  |      Kaliningrad Standard Time |   001  | Europe/Kaliningrad  |
//  |            Korea Standard Time |   001  | Asia/Seoul          |
//  |          Magadan Standard Time |   001  | Asia/Magadan        |
//  |        Mauritius Standard Time |   001  | Indian/Mauritius    |
//  |      Middle East Standard Time |   001  | Asia/Beirut         |
//  |       Montevideo Standard Time |   001  | America/Montevideo  |
//  |          Morocco Standard Time |   001  | Africa/Casablanca   |
//  |         Mountain Standard Time |   001  | America/Denver      |
//  |Mountain Standard Time (Mexico) |   001  | America/Chihuahua   |
//  |          Myanmar Standard Time |   001  | Asia/Rangoon        |
//  |  N. Central Asia Standard Time |   001  | Asia/Novosibirsk    |
//  |          Namibia Standard Time |   001  | Africa/Windhoek     |
//  |            Nepal Standard Time |   001  | Asia/Katmandu       |
//  |      New Zealand Standard Time |   001  | Pacific/Auckland    |
//  |     Newfoundland Standard Time |   001  | America/St_Johns    |
//  |  North Asia East Standard Time |   001  | Asia/Irkutsk        |
//  |       North Asia Standard Time |   001  | Asia/Krasnoyarsk    |
//  |       Pacific SA Standard Time |   001  | America/Santiago    |
//  |          Pacific Standard Time |   001  | America/Los_Angeles |
//  | Pacific Standard Time (Mexico) |   001  | America/Santa_Isabel|
//  |         Pakistan Standard Time |   001  | Asia/Karachi        |
//  |         Paraguay Standard Time |   001  | America/Asuncion    |
//  |          Romance Standard Time |   001  | Europe/Paris        |
//  |          Russian Standard Time |   001  | Europe/Moscow       |
//  |       SA Eastern Standard Time |   001  | America/Cayenne     |
//  |       SA Pacific Standard Time |   001  | America/Bogota      |
//  |       SA Western Standard Time |   001  | America/La_Paz      |
//  |          SE Asia Standard Time |   001  | Asia/Bangkok        |
//  |            Samoa Standard Time |   001  | Pacific/Apia        |
//  |        Singapore Standard Time |   001  | Asia/Singapore      |
//  |     South Africa Standard Time |   001  | Africa/Johannesburg |
//  |        Sri Lanka Standard Time |   001  | Asia/Colombo        |
//  |            Syria Standard Time |   001  | Asia/Damascus       |
//  |           Taipei Standard Time |   001  | Asia/Taipei         |
//  |         Tasmania Standard Time |   001  | Australia/Hobart    |
//  |            Tokyo Standard Time |   001  | Asia/Tokyo          |
//  |            Tonga Standard Time |   001  | Pacific/Tongatapu   |
//  |           Turkey Standard Time |   001  | Europe/Istanbul     |
//  |       US Eastern Standard Time |   001  | America/Indianapolis|
//  |      US Mountain Standard Time |   001  | America/Phoenix     |
//  |                            UTC |   001  | Etc/GMT             |
//  |                         UTC+12 |   001  | Etc/GMT-12          |
//  |                         UTC-02 |   001  | Etc/GMT+2           |
//  |                         UTC-11 |   001  | Etc/GMT+11          |
//  |      Ulaanbaatar Standard Time |   001  | Asia/Ulaanbaatar    |
//  |        Venezuela Standard Time |   001  | America/Caracas     |
//  |      Vladivostok Standard Time |   001  | Asia/Vladivostok    |
//  |     W. Australia Standard Time |   001  | Australia/Perth     |
//  |W. Central Africa Standard Time |   001  | Africa/Lagos        |
//  |        W. Europe Standard Time |   001  | Europe/Berlin       |
//  |        West Asia Standard Time |   001  | Asia/Tashkent       |
//  |     West Pacific Standard Time |   001  | Pacific/Port_Moresby|
//  |          Yakutsk Standard Time |   001  | Asia/Yakutsk        |
//  +--------------------------------+--------+---------------------+
//..
//
///Usage
///-----
// In this section we show intended usage of this component.
//
///Example 1: Basic Syntax
///- - - - - - - - - - - -
// The 'getTimeZoneIdFromWindowsTimeZoneId' method will convert a Windows
// timezone identifer to the designated Olson equivalent timezone identifer.
//..
//      int         rc;
//      const char *timeZoneId;
//      const char *windowsTimeZoneId;
//
//      rc = baetzo_TimeZoneIdUtil::getTimeZoneIdFromWindowsTimeZoneId(
//                                           &timeZoneId,
//                                           "Central Standard Time (Mexico)");
//      assert(0 == rc);
//      assert(0 == bsl::strcmp("America/Mexico_City", timeZoneId));
//..
// The 'getWindowsTimeZoneIdFromTimeZoneId' method performs the inverse
// mapping.
//..
//      rc = baetzo_TimeZoneIdUtil::getWindowsTimeZoneIdFromTimeZoneId(
//                                                      &windowsTimeZoneId,
//                                                      "America/Mexico_City");
//      assert(0 == rc);
//      assert(0 == bsl::strcmp("Central Standard Time (Mexico)",
//                               windowsTimeZoneId));
//..
// When given an unknown timezone identifier, both methods return a non-zero
// value and neither method changes the pointer value at the address given
// as the first argument.
//..
//      timeZoneId = (const char *)0xdeadbeef;
//      rc = baetzo_TimeZoneIdUtil::getTimeZoneIdFromWindowsTimeZoneId(
//                                                                 &timeZoneId,
//                                                                 "ABCZ");
//      assert(0                        != rc);
//      assert(0xdeadbeef == (unsigned)timeZoneId);
//
//      windowsTimeZoneId = (const char*)0xcafef00d;
//      rc = baetzo_TimeZoneIdUtil::getWindowsTimeZoneIdFromTimeZoneId(
//                                                          &windowsTimeZoneId,
//                                                          "XYZA");
//      assert(0          != rc);
//      assert(0xcafef00d == (unsigned)windowsTimeZoneId);
//..
//
///Example 2: Converting from Windows to Olson Time Zone Idenitifers
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// The 'getTimeZoneIdFromWindowsTimeZoneId' method allows Windows clients to
// use their configured time zone information (from the registry) to create the
// vocabulary types used in many BDE libraries.  The following example shows
// how to create and use a function that can assemble an 'baet_LocalDatetime'
// object given values for 'year', 'month', 'day', 'hour', and a Windows
// timezone identifer.
//
// First, we define our function interface.
//..
//  int myLoadLocalDatetime(baet_LocalDatetime   *resultPtr,
//                          const char           *timezoneIdFromRegistry,
//                          int                   year,
//                          int                   month,
//                          int                   day,
//                          int                   hour);
//      // Load, into the specified 'result', the local date-time value (in the
//      // (Windows) time zone indicated by the specified
//      // 'timezoneIdFromRegistry') corresponding the specified 'year',
//      // 'month', 'hour', 'day', and 'hour'.  Return 0 on success, and a
//      // non-zero value otherwise.
//..
//  Next, we implement our function.
//..
//  int myLoadLocalDatetime(baet_LocalDatetime   *resultPtr,
//                          const char           *timezoneIdFromRegistry,
//                          int                   year,
//                          int                   month,
//                          int                   day,
//                          int                   hour)
//  {
//      BSLS_ASSERT(resultPtr);
//      BSLS_ASSERT(timezoneIdFromRegistry);
//      BSLS_ASSERT(bdet_Date::isValid(year, month, day));
//      BSLS_ASSERT(bdet_Time::isValid(hour));
//
//      const char *timeZoneId;
//
//      int rc = baetzo_TimeZoneIdUtil::getTimeZoneIdFromWindowsTimeZoneId(
//                                                     &timeZoneId,
//                                                     timezoneIdFromRegistry);
//      if (0 != rc) {
//          return -1;                                                // RETURN
//      }
//
//      bdet_Datetime datetime(year, month, day, hour);
//
//      return baetzo_TimeZoneUtil::initLocalTime(
//                                          resultPtr,
//                                          datetime,
//                                          timeZoneId,
//                                          baetzo_DstPolicy::BAETZO_STANDARD);
//  }
//..
// Notice that the 'dstPolicy' parameters has been set to
// 'baetzo_DstPolicy::BAETZO_STANDARD', because each of the supported Windows
// timezone identifiers is a "Standard Time".
//
// Now, we can use 'myLoadLocalDatetime' to initialize 'localDatetime'.
//..
//  baet_LocalDatetime localDatetime;
//  int                rc  = myLoadLocalDatetime(&localDatetime,
//                                               "Arab Standard Time",
//                                               2012, 5, 28, 22);
//  assert(0 == rc);
//  assert(0 == bsl::strcmp("Asia/Riyadh",
//                          localDatetime.timeZoneId().c_str()));
//..
// Finally, having an object of a vocabulary type, we can easily perform
// a wide range of operations on our date-value (e.g., date arithmetic).
//..
//  bdet_Date localDate = localDatetime.datetimeTz().dateTz().localDate();
//  bdet_Date yesterday = localDate - 1;
//  bdet_Date tomorrow  = localDate + 1;
//  // ...
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
