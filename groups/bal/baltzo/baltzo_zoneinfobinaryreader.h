// baltzo_zoneinfobinaryreader.h                                      -*-C++-*-
#ifndef INCLUDED_BALTZO_ZONEINFOBINARYREADER
#define INCLUDED_BALTZO_ZONEINFOBINARYREADER

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide utilities for reading the Zoneinfo binary data format.
//
//@CLASSES:
//  baltzo::ZoneinfoBinaryReader: utilities for reading Zoneinfo binary data
//
//@SEE_ALSO: baltzo_zoneinfo
//
//@DESCRIPTION: This component provides a 'struct',
// 'baltzo::ZoneinfoBinaryReader', containing utilities to read the binary data
// format of the Zoneinfo database from a 'bsl::istream' and populate a
// 'baltzo::Zoneinfo' object.  The Zoneinfo database (also known as the Olson
// database or TZ database) records information about the world's time zones.
// It includes information such as the historic changes to the rules of local
// times, the time of transitions to daylight saving time, and corrections for
// leap seconds.
//
///Zoneinfo (TZ Database) Files
///----------------------------
// The Zoneinfo binary data is generated from a set of text-based rule file.  A
// rule file may be compiled into multiple binary files, each describing one
// time zone.  Information about the Zoneinfo database can be found online at
// 'http://www.iana.org/time-zones', including the time zone rules for the
// supported time zones, and source code for the 'zic' compiler (for compiling
// those rules into the binary representation read by utility functions
// provided by this component).
//
///Zoneinfo Binary File Format
///---------------------------
// A binary byte stream conforming to the Zoneinfo binary data format begins
// with a 44-byte header that describes the file.  The structure of the header
// is shown below:
//..
//     Byte | Name              | Description
// ---------+-------------------+----------------------------------------------
//   0 -  3 | 'T''Z''i''f'      | magic characters to identify the file
//        4 | version           | '\0' or '2' to identify the version
//   5 - 19 | reserved          | unused
//  20 - 23 | numIsGmt          | number of UTC/local-time indicators
//  24 - 27 | numIsStd          | number of standard/local-time indicators
//  28 - 31 | numLeaps          | number of leap seconds corrections
//  32 - 35 | numTransitions    | number of transition times
//  36 - 39 | numLocalTimeTypes | number of local-time types
//  40 - 43 | numAbbrevChar     | length of time-zone abbreviation string
// ---------+-------------------+----------------------------------------------
//..
// NOTE: All the numbers are 4-byte signed integers stored in big-endian format
// (higher order byte first).
//
// The binary file has the following layout:
//..
//   File Segment                      Number of bytes
// +--------------------------------+  ---------------
// | HEADER                         |  44
// +--------------------------------+
// | TRANSITION TIMES               |  4 * header.numTransitions
// +--------------------------------+
// | TRANSITION-TIME INDEXES        |  1 * header.numTransitions
// +--------------------------------+
// | LOCAL-TIME TYPES               |  (4+1+1) * header.numLocalTimeTypes
// +--------------------------------+
// | TIME-ZONE ABBREVIATION STRINGS |  1 * header.numAbbrevChar
// +--------------------------------+
// | LEAP SECONDS                   |  (4+4) * header.numLeaps
// +--------------------------------+
// | IS_GMT(s)                      |  1 * header.numIsGmt
// +--------------------------------+
// | IS_STD(s)                      |  1 * header.numIsStd
// +--------------------------------+
//..
//: o HEADER -- The header is 44 bytes that describe the file.
//:
//: o TRANSITION TIMES -- 'numTransitions' number of 4-byte big-endian signed
//:   integers representing the transition times in POSIX time format (number
//:   of seconds elapsed since midnight UTC, January 1, 1970) sorted in
//:   ascending order.  Each transition time represents a time at which the
//:   rule for computing the local time in that time zone changes.  (NOTE: See
//:   "Version '2'" section for additional information.)
//:
//: o TRANSITION-TIME INDEXES -- 'numTransitions' number of unsigned bytes.
//:   The byte at a given offset N stores the index of the local-time type
//:   entry (in the array of local-time types appearing next in the file) that
//:   corresponds to the transition at index N in the preceding sequence of
//:   transitions.
//:
//: o LOCAL-TIME TYPES -- An array of 'numLocalTimeTypes' local-time types,
//:   where each entry consist of:
//:   o gmtOffset -- 4-bytes big-endian signed integer indicating the number of
//:     seconds added to UTC to compute the local time in the time zone
//:   o isDst -- 1-byte boolean indicating whether the local-time type is
//:     considered daylight saving time
//:   o abbreviationIndex -- 1 unsigned byte indicating the index of the first
//:     byte of the null terminated string abbreviation of this local-time type
//:     in the array of bytes holding abbreviation strings later in the file.
//:
//: o TIME-ZONE ABBREVIATION STRINGS -- 'numAbbrevChar' number of bytes
//:   containing the abbreviation strings of the local-time types of this time
//:   zone.  Each abbreviation string ends with the '\0' character.
//:
//: o LEAP SECONDS -- 'numLeaps' number of *pairs* of 4-byte signed big-endian
//:   integers (totaling 8 bytes).  The first value of the pair is the POSIX
//:   time at which the leap seconds occur.  The second value of the pair gives
//:   the number of leap seconds to be applied to times after the time defined
//:   by the first element of the pair.  The pairs are stored in ascending
//:   order by time.  (NOTE: See "Version '2'" section for additional
//:   information.)
//:
//: o IS_GMT(s) -- 'numIsGmt' number of 1-byte booleans indicating whether the
//:   transition time for the local-time type with the corresponding index in
//:   the preceding array of local-time types was specified in UTC time or
//:   local time in the original rule file (from which the binary file was
//:   presumably compiled) (see next section for details).
//:
//: o IS_STD(s) -- 'numIsStd' number of 1-byte booleans indicating whether the
//:   transition time for the local-time type with the corresponding index in
//:   the preceding array of local-time types was specified in standard time or
//:   local time in the original rule file (from which the binary file was
//:   presumably compiled).  Standard time refers to the time without
//:   adjustment for daylight-saving time whether daylight saving is in effect
//:   or not.  (see next section for details).
//
///'isGmt' and 'isStd'
///- - - - - - - - - -
// The 'isGmt' and 'isStd' boolean flags are not used by this component.
//
// They indicate properties of the original rule file that were compiled into
// this binary representation.  Each 'isGmt' boolean flag indicates whether a
// transition time was originally specified as a UTC time or as a local time in
// the *rule* *file*.  'isStd' indicates whether a transition time was
// originally specified as a standard time or local time.  Note that all the
// transitions in the *binary* Zoneinfo representation are UTC times regardless
// of whether the original representation in the rule file was in local time.
// Also note that the value of 'numIsGmt' and 'numIsStd' should equal to
// 'numLocalTimeTypes', but for backward compatibility reasons, they may be set
// to zero.
//
///Leap Corrections
/// - - - - - - - -
// Leap corrections are currently not supported by this component.  Attempts to
// read a stream containing leap corrections will result in an error.
//
// There is a slight difference between the mean length of a day and 86400
// (24 * 60 * 60) seconds.  Leap corrections are adjustments to the UTC time to
// account for this difference.  In general, the time in a computer is updated
// through the network to account for leap seconds.  Thus, it is not necessary
// to for this component to consider leap corrections for accurate time
// calculations.
//
///Version '2'
///- - - - - -
// Version '2' format of the Zoneinfo binary data can be divided into two
// parts.  The first part contains the header and data described above.  The
// second part contains header and data in the same format with the exception
// that eight bytes are used for transition time and leap correction time.  The
// second header and data is followed by a formatted string used for handling
// time after the last transition time.
//
// The version '2' format uses 8 bytes to store date-time values because the
// 4-byte values used in the version '\0' format provide a limited date range.
// The standard IANA data contains many transitions that are outside the range
// of representable values of a 4-byte offset (these transitions are ignored by
// the version '\0' format, which leads to inaccurate information for dates far
// in the past or future).
//
// This component will always load version '2' data if it is present in the
// supplied binary data.
//
///Additional Information
/// - - - - - - - - - - -
// Additional documentation for the Zoneinfo file format can be found at
// various places on the internet:
//
//: o http://www.iana.org/time-zones -- Central links for Zoneinfo time-zone DB
//:   information.  Up-to-date documentation on the file specification can be
//:   found inside the code distribution, in a file named 'tzfile.5.txt'.  This
//:   file contains the most recent man-page text.
//:
//: o https://github.com/eggert/tz -- Unofficial GitHub repository for unstable
//:   changes not yet published into an official release on the IANA website.
//:   This repository is published and maintained by the official IANA
//:   maintainer, Paul Eggert.
//
///Usage
///-----
// In this section, we illustrate how this component can be used.
//
///Example 1: Reading Zoneinfo Binary Data
///- - - - - - - - - - - - - - - - - - - -
// The following demonstrates how to read a byte stream in the Zoneinfo binary
// data format into a 'baltzo::Zoneinfo' object.  We start by creating Zoneinfo
// data in memory for "Asia/Bangkok", which was chosen due to its small size.
// Note that this data was generated by the 'zic' compiler, which is publicly
// obtainable as part of the standard Zoneinfo distribution (see
// 'http://www.iana.org/time-zones'):
//..
//  const char ASIA_BANGKOK_DATA[] = {
//      0x54, 0x5a, 0x69, 0x66, 0x32, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
//      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02,
//      0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
//      0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x08, 0xa2, 0x6a, 0x67, 0xc4,
//      0x01, 0x00, 0x00, 0x5e, 0x3c, 0x00, 0x00, 0x00, 0x00, 0x62, 0x70, 0x00,
//      0x04, 0x42, 0x4d, 0x54, 0x00, 0x49, 0x43, 0x54, 0x00, 0x00, 0x00, 0x00,
//      0x00, 0x54, 0x5a, 0x69, 0x66, 0x32, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
//      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
//      0x03, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
//      0x02, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x0c, 0xff, 0xff, 0xff,
//      0xff, 0x56, 0xb6, 0x85, 0xc4, 0xff, 0xff, 0xff, 0xff, 0xa2, 0x6a, 0x67,
//      0xc4, 0x01, 0x02, 0x00, 0x00, 0x5e, 0x3c, 0x00, 0x00, 0x00, 0x00, 0x5e,
//      0x3c, 0x00, 0x04, 0x00, 0x00, 0x62, 0x70, 0x00, 0x08, 0x4c, 0x4d, 0x54,
//      0x00, 0x42, 0x4d, 0x54, 0x00, 0x49, 0x43, 0x54, 0x00, 0x00, 0x00, 0x00,
//      0x00, 0x00, 0x00, 0x0a, 0x49, 0x43, 0x54, 0x2d, 0x37, 0x0a
//  };
//..
// Then, we load this data into a stream buffer.
//..
//  bdlsb::FixedMemInStreamBuf inStreamBuf(ASIA_BANGKOK_DATA,
//                                         sizeof(ASIA_BANGKOK_DATA));
//  bsl::istream inputStream(&inStreamBuf);
//..
// Now, we read the 'inputStream' using 'baltzo::ZoneinfoBinaryReader::read'.
//..
//
//  baltzo::Zoneinfo timeZone;
//  if (0 != baltzo::ZoneinfoBinaryReader::read(&timeZone,
//                                              inputStream)) {
//      bsl::cerr << "baltzo::ZoneinfoBinaryReader::load failed"
//                << bsl::endl;
//      return 1;                                                      //RETURN
//  }
//..
// Finally, we write a description of the loaded Zoneinfo to the console.
//..
//  timeZone.print(bsl::cout, 1, 3);
//..
// The output of the preceding statement should look like:
//..
// [
//    identifier = ""
//    transitions = [
//    [
//       time = 01JAN0001_00:00:00.000
//       descriptor = [
//          utcOffsetInSeconds = 24124
//          dstInEffectFlag = false
//          description = "LMT"
//       ]
//    ]
//    [
//       time = 31DEC1879_17:17:56.000
//       descriptor = [
//          utcOffsetInSeconds = 24124
//          dstInEffectFlag = false
//          description = "BMT"
//       ]
//    ]
//    [
//       time = 31MAR1920_17:17:56.000
//       descriptor = [
//          utcOffsetInSeconds = 25200
//          dstInEffectFlag = false
//          description = "ICT"
//       ]
//    ]
//    ]
// ]
//..

#ifndef INCLUDED_BALSCM_VERSION
#include <balscm_version.h>
#endif

#ifndef INCLUDED_BSL_IOSFWD
#include <bsl_iosfwd.h>
#endif

namespace BloombergLP {


namespace baltzo {

class Zoneinfo;
class ZoneinfoBinaryHeader;

                        // ===========================
                        // struct ZoneinfoBinaryReader
                        // ===========================

struct ZoneinfoBinaryReader {
    // This struct provides a namespace for functions that read Zoneinfo time
    // zone data from a binary input stream.  The primary method, 'read', makes
    // use of a stream containing a Zoneinfo time zone database to populate a
    // 'Zoneinfo' object.

    // CLASS METHODS
    static int read(Zoneinfo      *zoneinfoResult,
                    bsl::istream&  stream);
    static int read(Zoneinfo             *zoneinfoResult,
                    ZoneinfoBinaryHeader *headerResult,
                    bsl::istream&         stream);
        // Read time zone information from the specified 'stream', and load the
        // description into the specified 'zoneinfoResult'.  Return 0 on
        // success and a non-zero value if 'stream' does not provide a sequence
        // of bytes consistent with the Zoneinfo binary format.  If an error
        // occurs during the operation, 'zoneinfoResult' is unspecified.
        // Optionally specify a 'headerResult' that, on success, will be
        // populated with a summary of the 'stream' contents.
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
