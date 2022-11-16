// baltzo_windowstimezoneutil.h                                       -*-C++-*-
#ifndef INCLUDED_BALTZO_WINDOWSTIMEZONEUTIL
#define INCLUDED_BALTZO_WINDOWSTIMEZONEUTIL

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide utilities to map Zoneinfo identifiers to other systems.
//
//@CLASSES:
//  baltzo::WindowsTimeZoneUtil: utilities for mapping Zoneinfo time-zone ids
//
//@SEE_ALSO: bsitzo_tzdftimezoneutil
//
//@DESCRIPTION: This component provides a namespace,
// 'baltzo::WindowsTimeZoneUtil', containing utility functions supporting the
// use of Windows time-zone facilities with 'baltzo' facilities.  Currently,
// this component provides functions that map Windows time-zone identifiers to
// and from 'baltzo' (Zoneinfo) time-zone identifiers.
//
///Windows Time-Zone Identifiers
///-----------------------------
// The mapping from Windows to Zoneinfo identifiers used by the 'baltzo'
// package is defined in the table titled "Mapping for: windows" at
// 'http://unicode.org/cldr/charts/32/supplemental/zone_tzid.html' (with any
// differences noted in the section {Differences from CLDR} below).  The
// Zoneinfo values on the unicode webpage are given in the column labeled
// "TZID".  Each Windows identifier is qualified by one or more "Region"
// attributes so, in general, there may be more than one Zoneinfo identifier
// for a given Windows identifier.  The mapping in this component uses the
// default mapping (Region "001").  The 99 entries are:
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
//  |            India Standard Time | Asia/Kolkata        |
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
///Differences from CLDR
///---------------------
// Current differences from canonical CLDR data:
//: o "Asia/Calcutta" -> "Asia/Kolkata".  The city was renamed, and the IANA
//:   time zone identifier changed in 1993 but CLDR incorrectly uses the old
//:   name for the city.  There is an open issue for this:
//:   'https://unicode-org.atlassian.net/browse/CLDR-9892'.
//
///Usage
///-----
// In this section we show intended use of this component.
//
///Example 1: Converting Between Windows and Zoneinfo Time-Zone Identifiers
/// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// This example shows how to find the Zoneinfo time-zone time-zone identifier
// for a given Windows time-zone identifier, and the inverse operation.
//
// First, given the "Central Standard Time (Mexico)" Windows time-zone
// identifier, use the 'getZoneinfoId' method to find the corresponding
// Zoneinfo time-zone identifier.
//..
//  int         rc;
//  const char *timeZoneId;
//  const char *windowsTimeZoneId;
//
//  rc = baltzo::WindowsTimeZoneUtil::getZoneinfoId(
//                                           &timeZoneId,
//                                           "Central Standard Time (Mexico)");
//  assert(0 == rc);
//  assert(0 == bsl::strcmp("America/Mexico_City", timeZoneId));
//..
// Notice that the corresponding Zoneinfo time-zone identifier is
// "America/Mexico_City".
//
// Next, use 'getWindowsTimeZoneId' method to find the Windows time-zone
// identifier corresponding to "America/Mexico_City".
//..
//  rc = baltzo::WindowsTimeZoneUtil::getWindowsTimeZoneId(
//                                                      &windowsTimeZoneId,
//                                                      "America/Mexico_City");
//  assert(0 == rc);
//  assert(0 == bsl::strcmp("Central Standard Time (Mexico)",
//                           windowsTimeZoneId));
//..
// Notice that the time zone returned is "Central Standard Time (Mexico)", the
// original time-zone identifier.
//
///Example 2: Creating a 'baltzo::LocalDatetime' Object on Windows
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// The following example demonstrates how to create on a Windows platform a
// 'baltzo::LocalDatetime' object with the value of the current time.
//
// First, use the Windows 'GetTimeZoneInformation' function to load a
// 'TIME_ZONE_INFORMATION' structure.
//..
//  int                   rc;
//  TIME_ZONE_INFORMATION tzi;
//  rc = GetTimeZoneInformation(&tzi);
//  assert(TIME_ZONE_ID_UNKNOWN  == rc
//      || TIME_ZONE_ID_STANDARD == rc
//      || TIME_ZONE_ID_DAYLIGHT == rc);
//..
// The 'StandardName' member of the structure, of type 'WCHAR[32]', contains
// the Windows time-zone identifier for Standard Time for the system's local
// time zone.
//
// Next, use the 'wcstombs_s' function to convert the wide string in the
// 'StandardName' member to its multi-byte equivalent in the 'standardName'
// buffer, and assign the result to 'localTimezone'.  Note that every Windows
// time-zone identifier mapped by this component consists entirely of 7-bit
// ASCII characters.
//..
//  bsl::string localTimezone;
//
//  {
//      // Convert 'StandardName' field ('WCHAR[32]') to 'bsl::string'.
//
//      char    standardName[sizeof(tzi.StandardName) * 2 + 1] = { '\0' };
//      errno_t error = wcstombs_s(NULL,
//                                 standardName,
//                                 sizeof(standardName),
//                                 tzi.StandardName,
//                                 _TRUNCATE);
//      assert(0 == errno);
//      localTimezone.assign(standardName);
//  }
//  assert("Arab Standard Time" == localTimezone);
//..
// Now, use the 'getZoneinfoId' method to find the corresponding Zoneinfo
// time-zone identifier.
//..
//  const char *zoneinfoId;
//  rc = baltzo::WindowsTimeZoneUtil::getZoneinfoId(&zoneinfoId,
//                                                 localTimezone.c_str());
//  assert(0 == rc);
//  assert(0 == bsl::strcmp("Asia/Riyadh", zoneinfoId));
//..
// Then, use the Windows 'GetSystemTime' function to load an 'SYSTEMTIME'
// structure with UTC time information.  The returned information includes
// year, month ('[1 .. 12]'), day-of-month ('[1 .. 31]'), and hour-of-day
// ('[0 .. 23]').  Note 'bdlt_date' and 'bdlt_time' use the same numerical
// values to represent month, day, etc.  The range of years is different but
// practically the same as they overlap for several centuries around the
// current time.
//..
//  SYSTEMTIME systemTime;
//  GetSystemTime(&systemTime);
//..
// Finally, use these Windows SystemTime values and the calculated Zoneinfo
// time-zone identifier to set the value of a 'baltzo::LocalDatetime' object.
//..
//  baltzo::LocalDatetime localDatetime;
//
//  rc = baltzo::TimeZoneUtil::convertUtcToLocalTime(
//                                  &localDatetime,
//                                  zoneinfoId,
//                                  bdlt::Datetime(systemTime.wYear,
//                                                systemTime.wMonth,
//                                                systemTime.wDay,
//                                                systemTime.wHour));
//  assert(0             == rc);
//  assert("Asia/Riyadh" == localDatetime.timeZoneId());
//..

#include <balscm_version.h>

namespace BloombergLP {
namespace baltzo {
                         // ==========================
                         // struct WindowsTimeZoneUtil
                         // ==========================

struct WindowsTimeZoneUtil {
    // This 'struct' provides a namespace for utility functions that convert
    // Zoneinfo time-zone identifiers both to and Windows time-zone
    // identifiers.

    // CLASS METHODS
    static int getZoneinfoId(const char **result,
                             const char  *windowsTimeZoneId);
        // Load into the specified 'result' the address of a 0 terminated
        // C-string containing the default Zoneinfo time-zone identifier for
        // the specified 'windowsTimeZoneId'.  Return 0 on success, and
        // non-zero value with no other effect otherwise.  The returned address
        // is valid for the life-time of the process.

    static int getWindowsTimeZoneId(const char **result,
                                    const char  *zoneinfoId);
        // Load into the specified 'result' the address of a 0 terminated
        // C-string containing the Windows time-zone identifier for the
        // specified 'zoneinfoId'.  Return 0 on success, and non-zero value
        // with no other effect otherwise.  The returned address is valid for
        // the life-time of the process.
};

}  // close package namespace
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
