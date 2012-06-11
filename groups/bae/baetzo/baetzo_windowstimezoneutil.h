// baetzo_windowstimezoneutil.h                                       -*-C++-*-
#ifndef INCLUDED_BAETZO_WINDOWSTIMEZONEUTIL
#define INCLUDED_BAETZO_WINDOWSTIMEZONEUTIL

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide utilities to map Zoneinfo identifiers to other systems.
//
//@CLASSES:
//  baetzo_WindowsTimeZoneUtil: utilities for mapping Zoneinfo time-zone ids
//
//@SEE_ALSO: bsitzo_tzdftimezoneutil
//
//@AUTHOR: Steven Breitstein (sbreitstein)
//
//@DESCRIPTION: This component provides a namespace,
// 'baetzo_WindowsTimeZoneUtil', containing utility functions supporting the
// use of Windows time-zone facilities with 'baetzo' facilities.
//
// Currently, this provides functions that map Windows time-zone identifiers
// to and from Zoneinfo time-zone identifiers.
//
///Windows Time-Zone Identifiers
///-----------------------------
// The mapping from Windows to to Zoneinfo identifiers is defined in the table
// titled "Mapping for: windows" at
// 'http://unicode.org/repos/cldr-tmp/trunk/diff/supplemental/zone_tzid.html'.
// The Zoneinfo values are given in the column labeled "TZID".  Each Windows
// identifier is qualified by one or more "Region" attributes so, in general,
// there may be more than one Zoneinfo identifier for a given Windows
// identifier.  The mapping in this component uses the default mapping (Region
// "001").  The 99 entries are:
//..
//  +--------------------------------+---------------------+
//  |Windows Identifier              | Zoneinfo Identifier |
//  +--------------------------------+---------------------+
//  |      AUS Central Standard Time | Australia/Darwin    |
//  |      AUS Eastern Standard Time | Australia/Sydney    |
//  |      Afghanistan Standard Time | Asia/Kabul          |
//  |          Alaskan Standard Time | America/Anchorage   |
//  |             Arab Standard Time | Asia/Riyadh         |
//  |          Arabian Standard Time | Asia/Dubai          |
//  |           Arabic Standard Time | Asia/Baghdad        |
//  |        Argentina Standard Time | America/Buenos_Aires|
//  |         Atlantic Standard Time | America/Halifax     |
//  |       Azerbaijan Standard Time | Asia/Baku           |
//  |           Azores Standard Time | Atlantic/Azores     |
//  |            Bahia Standard Time | America/Bahia       |
//  |       Bangladesh Standard Time | Asia/Dhaka          |
//  |   Canada Central Standard Time | America/Regina      |
//  |       Cape Verde Standard Time | Atlantic/Cape_Verde |
//  |         Caucasus Standard Time | Asia/Yerevan        |
//  |   Cen. Australia Standard Time | Australia/Adelaide  |
//  |  Central America Standard Time | America/Guatemala   |
//  |     Central Asia Standard Time | Asia/Almaty         |
//  |Central Brazilian Standard Time | America/Cuiaba      |
//  |   Central Europe Standard Time | Europe/Budapest     |
//  | Central European Standard Time | Europe/Warsaw       |
//  |  Central Pacific Standard Time | Pacific/Guadalcanal |
//  |          Central Standard Time | America/Chicago     |
//  | Central Standard Time (Mexico) | America/Mexico_City |
//  |            China Standard Time | Asia/Shanghai       |
//  |         Dateline Standard Time | Etc/GMT+12          |
//  |        E. Africa Standard Time | Africa/Nairobi      |
//  |     E. Australia Standard Time | Australia/Brisbane  |
//  |        E. Europe Standard Time | Asia/Nicosia        |
//  | E. South America Standard Time | America/Sao_Paulo   |
//  |          Eastern Standard Time | America/New_York    |
//  |            Egypt Standard Time | Africa/Cairo        |
//  |     Ekaterinburg Standard Time | Asia/Yekaterinburg  |
//  |              FLE Standard Time | Europe/Kiev         |
//  |             Fiji Standard Time | Pacific/Fiji        |
//  |              GMT Standard Time | Europe/London       |
//  |              GTB Standard Time | Europe/Bucharest    |
//  |         Georgian Standard Time | Asia/Tbilisi        |
//  |        Greenland Standard Time | America/Godthab     |
//  |        Greenwich Standard Time | Atlantic/Reykjavik  |
//  |         Hawaiian Standard Time | Pacific/Honolulu    |
//  |            India Standard Time | Asia/Calcutta       |
//  |             Iran Standard Time | Asia/Tehran         |
//  |           Israel Standard Time | Asia/Jerusalem      |
//  |           Jordan Standard Time | Asia/Amman          |
//  |      Kaliningrad Standard Time | Europe/Kaliningrad  |
//  |            Korea Standard Time | Asia/Seoul          |
//  |          Magadan Standard Time | Asia/Magadan        |
//  |        Mauritius Standard Time | Indian/Mauritius    |
//  |      Middle East Standard Time | Asia/Beirut         |
//  |       Montevideo Standard Time | America/Montevideo  |
//  |          Morocco Standard Time | Africa/Casablanca   |
//  |         Mountain Standard Time | America/Denver      |
//  |Mountain Standard Time (Mexico) | America/Chihuahua   |
//  |          Myanmar Standard Time | Asia/Rangoon        |
//  |  N. Central Asia Standard Time | Asia/Novosibirsk    |
//  |          Namibia Standard Time | Africa/Windhoek     |
//  |            Nepal Standard Time | Asia/Katmandu       |
//  |      New Zealand Standard Time | Pacific/Auckland    |
//  |     Newfoundland Standard Time | America/St_Johns    |
//  |  North Asia East Standard Time | Asia/Irkutsk        |
//  |       North Asia Standard Time | Asia/Krasnoyarsk    |
//  |       Pacific SA Standard Time | America/Santiago    |
//  |          Pacific Standard Time | America/Los_Angeles |
//  | Pacific Standard Time (Mexico) | America/Santa_Isabel|
//  |         Pakistan Standard Time | Asia/Karachi        |
//  |         Paraguay Standard Time | America/Asuncion    |
//  |          Romance Standard Time | Europe/Paris        |
//  |          Russian Standard Time | Europe/Moscow       |
//  |       SA Eastern Standard Time | America/Cayenne     |
//  |       SA Pacific Standard Time | America/Bogota      |
//  |       SA Western Standard Time | America/La_Paz      |
//  |          SE Asia Standard Time | Asia/Bangkok        |
//  |            Samoa Standard Time | Pacific/Apia        |
//  |        Singapore Standard Time | Asia/Singapore      |
//  |     South Africa Standard Time | Africa/Johannesburg |
//  |        Sri Lanka Standard Time | Asia/Colombo        |
//  |            Syria Standard Time | Asia/Damascus       |
//  |           Taipei Standard Time | Asia/Taipei         |
//  |         Tasmania Standard Time | Australia/Hobart    |
//  |            Tokyo Standard Time | Asia/Tokyo          |
//  |            Tonga Standard Time | Pacific/Tongatapu   |
//  |           Turkey Standard Time | Europe/Istanbul     |
//  |       US Eastern Standard Time | America/Indianapolis|
//  |      US Mountain Standard Time | America/Phoenix     |
//  |                            UTC | Etc/GMT             |
//  |                         UTC+12 | Etc/GMT-12          |
//  |                         UTC-02 | Etc/GMT+2           |
//  |                         UTC-11 | Etc/GMT+11          |
//  |      Ulaanbaatar Standard Time | Asia/Ulaanbaatar    |
//  |        Venezuela Standard Time | America/Caracas     |
//  |      Vladivostok Standard Time | Asia/Vladivostok    |
//  |     W. Australia Standard Time | Australia/Perth     |
//  |W. Central Africa Standard Time | Africa/Lagos        |
//  |        W. Europe Standard Time | Europe/Berlin       |
//  |        West Asia Standard Time | Asia/Tashkent       |
//  |     West Pacific Standard Time | Pacific/Port_Moresby|
//  |          Yakutsk Standard Time | Asia/Yakutsk        |
//  +--------------------------------+---------------------+
//..
//
///Usage
///-----
// In this section we show intended use of this component.
//
///Example 1: Converting Between Windows and Zoneinfo Time-Zone Identifiers
/// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// These code snippets show the basic syntax of this component's methods.
//
// The 'getZoneinfoId' method converts a Windows time-zone
// identifer to the default Zoneinfo equivalent time-zone identifer.
//..
//      int         rc;
//      const char *timeZoneId;
//      const char *windowsTimeZoneId;
//
//      rc = baetzo_WindowsTimeZoneUtil::getZoneinfoId(
//                                           &timeZoneId,
//                                           "Central Standard Time (Mexico)");
//      assert(0 == rc);
//      assert(0 == bsl::strcmp("America/Mexico_City", timeZoneId));
//..
// The 'getWindowsTimeZoneId' method performs the inverse mapping.
//..
//      rc = baetzo_WindowsTimeZoneUtil::getWindowsTimeZoneId(
//                                                      &windowsTimeZoneId,
//                                                      "America/Mexico_City");
//      assert(0 == rc);
//      assert(0 == bsl::strcmp("Central Standard Time (Mexico)",
//                               windowsTimeZoneId));
//..
//
///Example 2: Creating a 'baet_LocalDatetime' Object on Windows
/// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// The Windows API provides date, time, and time-zone information (e.g.,
// 'GetSystemTime', 'GetTimeZoneInformation').  Windows clients may wish to use
// that information to create a 'baet_LocalDatetime' object (or other
// date/time-related vocabulary types) to allow use of BDE's rich facilities
// for manipulating date/time values.
//
// First, use the Windows 'GetTimeZoneInformation' function to load a
// 'LPTIME_ZONE_INFORMATION' structure.  That structure has a 'StandardName'
// array (represented by a simple array below).
//..
//  const char StandardName[32] = "Arab Standard Time";
//..
// Next, use the 'getZoneinfoId' method to find the
// corresponding Zoneinfo time-zone identifier.
//..
//  const char *zoneinfoId;
//  int         rc = baetzo_WindowsTimeZoneUtil::getZoneinfoId(&zoneinfoId,
//                                                             StandardName);
//  assert(0 == rc);
//  assert(0 == bsl::strcmp("Asia/Riyadh", zoneinfoId));
//..
// Then, use the Windows 'GetSystemTime' function to load an 'LPSYTEMTIME'
// structure with UTC time information.  This includes year, month
// ('[1 .. 12]'), day-of-month ('[1 .. 31]'), and hour-of-day ('[0 .. 23]').
// Note 'bdet_date' and 'bdet_time' use the same numerical values to represent
// month, day, etc.  The range of years is different but practically the same
// as they overlap for several centuries around the current time.
//
// The members of the 'LPSYTEMTIME' structure are represented by simple
// variables below:
//..
//  const int wYear  = 2012;
//  const int wMonth =    5;
//  const int wDay   =   28;
//  const int wHour  =   23;
//..
// Finally, use the these Windows SystemTime values and and the calculated
// Zoneinfo time-zone identifier to set the value of a 'baet_LocalDatetime'
// object.
//..
//  baet_LocalDatetime localDatetime;
//
//  rc = baetzo_TimeZoneUtil::convertUtcToLocalTime(
//                                  &localDatetime,
//                                  zoneinfoId,
//                                  bdet_Datetime(wYear, wMonth, wDay, wHour));
//  assert(0             == rc);
//  assert("Asia/Riyadh" == localDatetime.timeZoneId());
//..

#ifndef INCLUDED_BAESCM_VERSION
#include <baescm_version.h>
#endif

namespace BloombergLP {

                        // ================================
                        // class baetzo_WindowsTimeZoneUtil
                        // ================================

struct baetzo_WindowsTimeZoneUtil {
    // This 'struct' provides a namespace for utility functions that convert
    // Zoneinfo time-zone identifiers both to and from other systems of
    // time-zone identifiers.

    // CLASS METHODS
    static int getZoneinfoId(const char **result,
                             const char  *windowsTimeZoneId);
        // Load into the specified 'result' the address a 0 terminated C-string
        // containing the default Zoneinfo time-zone identifier for the
        // specified 'windowsTimeZoneId'.  Return 0 on success, and non-zero
        // value with no other effect otherwise.  The returned address is valid
        // for the life-time of the process.

    static int getWindowsTimeZoneId(const char **result,
                                    const char  *zoneinfoId);
        // Load into the specified 'result' the address a 0 terminated C-string
        // containing the Windows time-zone identifier that has a default
        // mapping to the specified 'zoneinfoId'.  Return 0 on success, and
        // non-zero value with no other effect otherwise.  The returned address
        // is valid for the life-time of the process.
};

// ============================================================================
//                      INLINE FUNCTION DEFINITIONS
// ============================================================================

}  // close enterprise namespace

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2012
//      All Rights Reserved.
//      Property of Bloomberg L.P.  (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
