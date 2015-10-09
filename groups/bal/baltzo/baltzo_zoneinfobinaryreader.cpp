// baltzo_zoneinfobinaryreader.cpp                                    -*-C++-*-
#include <baltzo_zoneinfobinaryreader.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(baltzo_zoneinfobinaryreader_cpp,"$Id$ $CSID$")

#include <baltzo_localtimedescriptor.h>
#include <baltzo_zoneinfo.h>
#include <baltzo_zoneinfobinaryheader.h>

#include <ball_log.h>

#include <bdlb_bigendian.h>
#include <bdlb_chartype.h>
#include <bdlb_print.h>
#include <bdlb_string.h>

#include <bdlt_timeunitratio.h>

#include <bslmf_assert.h>

#include <bsl_fstream.h>
#include <bsl_iostream.h>
#include <bsl_istream.h>
#include <bsl_memory.h>
#include <bsl_ostream.h>
#include <bsl_sstream.h>
#include <bsl_string.h>
#include <bsl_vector.h>
#include <bsl_cstring.h>

#include <bsls_assert.h>
#include <bsls_byteorder.h>
#include <bsls_types.h>

///Implementation Notes
///--------------------
// Addition information about the implementation of Zoneinfo binary file
// readers can be found on the internet:
//
//: o http://linux.about.com/library/cmd/blcmdl5_tzfile.htm
//:     A version of the 'tzfile' linux man page
//:
//: o http://sources.redhat.com/glibc/
//:     glib C source code.  Including:
//:        o timezone/tzfile.h    (headers for olson tz file)
//:        o time/time.h          (header for GNU time functions)
//:        o time/tzset.c         (source for working with tz data)
//:        o time/tzfile.c        (source for loading tz data)

namespace BloombergLP {

static const char LOG_CATEGORY[] = "baltzo.ZONEINFOBINARYREADER";

namespace {

struct RawHeader {
    // The byte sequence of the header of the Zoneinfo binary data format.

    char d_headerId[4];           // must be 'EXPECTED_HEADER_ID'
    char d_version[1];            // must be '\0' or '2' (as of 2005)
    char d_reserved[15];          // unused
    char d_numIsGmt[4];           // number of encoded UTC/local indicators
    char d_numIsStd[4];           // number of encoded standard/wall indicators
    char d_numLeaps[4];           // number of leap info's encoded in the file
    char d_numTransitions[4];     // number of transition times
    char d_numLocalTimeTypes[4];  // number of transition types
    char d_abbrevDataSize[4];     // size of the sequence of abbreviation chars
};

BSLMF_ASSERT(44 == sizeof(RawHeader));

struct RawLocalTimeType {
    // The byte sequence of a local-time type in the Zoneinfo binary data
    // format.

    char          d_offset[4];          // UTC offset in number of seconds
    unsigned char d_isDst;              // indicates whether local time is DST
    unsigned char d_abbreviationIndex;  // index to abbreviation string
};

BSLMF_ASSERT(6 == sizeof(RawLocalTimeType));

struct RawLeapInfo {
    // The byte sequence of a leap correction in the Zoneinfo binary data
    // format version '\0'.

    char d_transition[4];  // POSIX time at which the leap second occur
    char d_correction[4];  // accumulated leap correction
};

BSLMF_ASSERT(8 == sizeof(RawLeapInfo));

struct RawLeapInfo64 {
    // The byte sequence of a leap correction in the Zoneinfo binary data
    // format version '2'.

    char d_transition[8];  // POSIX time at which the leap second occur
    char d_correction[4];  // accumulated leap correction
};

BSLMF_ASSERT(12 == sizeof(RawLeapInfo64));

}  // close unnamed namespace

static
bool areAllPrintable(const char *buffer, int length)
    // Return 'true' if every character in the specified 'buffer' of the
    // specified 'length' is printable, and 'false' otherwise.
{
    BSLS_ASSERT(buffer);
    BSLS_ASSERT(0 <= length);

    for (int i = 0; i < length; ++i) {
        if (!bdlb::CharType::isPrint(buffer[i])) {
            return false;                                             // RETURN
        }
    }
    return true;
}

static
void formatHeaderId(bsl::string *formattedHeader,
                    const char  *buffer,
                    int          length)
    // Load the specified 'formattedHeader' with characters from the specified
    // 'buffer' of the specified 'length' if each of those characters is
    // printable, and with the hexadecimal representation of those characters
    // otherwise.
{
    BSLS_ASSERT(formattedHeader);
    BSLS_ASSERT(buffer);
    BSLS_ASSERT(0 <= length);

    if (areAllPrintable(buffer, length)) {
        formattedHeader->assign(buffer, length);
    } else {
        bsl::ostringstream oss;
        oss << bdlb::PrintStringSingleLineHexDumper(buffer, length);
        formattedHeader->assign(oss.str());
    }
}

static const char *EXPECTED_HEADER_ID = "TZif";
    // The first 4 bytes of a valid Zoneinfo database file.

template <class TYPE>
static inline
int readRawArray(bsl::vector<TYPE> *result,
                 bsl::istream&      stream,
                 int                numValues)
    // Read the specified 'numValues' of the parameterized 'TYPE' from the
    // specified 'stream' into the specified 'result'.  Return 0 on success and
    // -1 if the read failed.  Note that 'sizeof(TYPE)' must equal the size of
    // the packed stream data.
{
    BSLS_ASSERT(result);

    if (0 != numValues) {
        int numBytes = sizeof(TYPE) * numValues;
        result->resize(numValues);
        if (!stream.read((char *)&result->front(), numBytes)) {
            return -1;                                                // RETURN
        }
    }
    return 0;
}

template <class TYPE>
static inline
bool validIndex(const bsl::vector<TYPE>& vector, int index)
    // Return 'true' if the specified 'index' is within the range of valid
    // indices of the specified 'vector', and 'false' otherwise.
{
    return 0 <= index && (unsigned int) index < vector.size();
}

static inline
int decode32(const char *address)
    // Read the 32-bit big-endian integer in the array of bytes located at the
    // specified 'address' and return that value.  The behavior is undefined
    // unless 'address' points to an accessible memory location.
{
    BSLS_ASSERT_SAFE(address);

    int temp;
    bsl::memcpy(&temp, address, sizeof(temp));
    return BSLS_BYTEORDER_BE_U32_TO_HOST(temp);
}

static inline
int readHeader(baltzo::ZoneinfoBinaryHeader *result, bsl::istream& stream)
    // Extract header information from the specified 'stream' and, if the data
    // meets the requirements of the Zoneinfo binary file format, populate the
    // specified 'result' with the extracted information.  Return 0 if 'result'
    // is successfully read, and a non-zero value otherwise.
{
    BSLS_ASSERT(result);

    BALL_LOG_SET_CATEGORY(LOG_CATEGORY);

    RawHeader rawHeader;
    if (!stream.read((char *)&rawHeader, sizeof(RawHeader))) {
        BALL_LOG_ERROR << "Unable to read Zoneinfo header." << BALL_LOG_END;
        return -1;                                                    // RETURN
    }

    if (0 != bsl::memcmp(EXPECTED_HEADER_ID, rawHeader.d_headerId, 4)) {
        bsl::string headerId;
        formatHeaderId(&headerId, rawHeader.d_headerId, 4);
        BALL_LOG_ERROR << "Did not find expected header id.  Expecting "
                       << "'TZif', found '" + headerId + "'"
                       << BALL_LOG_END;
        return -2;                                                    // RETURN
    }

    char version = *rawHeader.d_version;
    if ('\0' != version && '2' != version) {
        BALL_LOG_ERROR << "Found unexpected version value: "
                       << (int)version
                       << " ('"
                       << version
                       << "').  Expecting '\\0' or '2'."
                       << BALL_LOG_END;
        return -3;                                                    // RETURN
    }
    result->setVersion(version);

    int numLocalTimeTypes = decode32(rawHeader.d_numLocalTimeTypes);
    if (0 >= numLocalTimeTypes) {
        BALL_LOG_ERROR << "Empty list of local-time types in Zoneinfo file."
                       << BALL_LOG_END;
        return -4;                                                    // RETURN
    }
    result->setNumLocalTimeTypes(numLocalTimeTypes);

    int numIsGmt = decode32(rawHeader.d_numIsGmt);
    if (0 > numIsGmt) {
        BALL_LOG_ERROR << "Invalid number of 'isGmt' flags "
                       << numIsGmt
                       << " found in Zoneinfo file."
                       << BALL_LOG_END;
        return -5;                                                    // RETURN
    }
    result->setNumIsGmt(numIsGmt);

    int numIsStd = decode32(rawHeader.d_numIsStd);
    if (0 > numIsStd) {
        BALL_LOG_ERROR << "Invalid number of 'isStd' flags "
                       << numIsStd
                       << " found in Zoneinfo file."
                       << BALL_LOG_END;
        return -6;                                                    // RETURN
    }
    result->setNumIsStd(numIsStd);

    // The 'isGmt' and 'isStd' values provide information about the transition
    // times associated with a local-time type, so the number of values (i.e.,
    // 'numIsGmt' and 'numIsStd') should be the same as the number of
    // local-time types.  They may also be 0 for backward compatibility reason.

    if ((0 != numIsGmt && numIsGmt != numLocalTimeTypes)
        || (0 != numIsStd && numIsStd != numLocalTimeTypes)) {
        BALL_LOG_WARN << "Unexpected number of isGmt or isStd values in "
                      << "Zoneinfo file."
                      << BALL_LOG_END;
    }

    int numLeaps = decode32(rawHeader.d_numLeaps);
    if (0 != numLeaps) {
        BALL_LOG_ERROR << "Non-zero number of leap corrections found in "
                       << "Zoneinfo file.  Leap correction is not supported "
                       << "by 'baltzo::ZoneinfoBinaryReader'."
                       << BALL_LOG_END;
        return -7;                                                    // RETURN
    }
    result->setNumLeaps(numLeaps);

    int numTransitions = decode32(rawHeader.d_numTransitions);
    if (0 > numTransitions) {
        BALL_LOG_ERROR << "Invalid number of transitions found in Zoneinfo "
                       << "file."
                       << BALL_LOG_END;
        return -8;                                                    // RETURN
    }
    result->setNumTransitions(numTransitions);

    int abbrevDataSize = decode32(rawHeader.d_abbrevDataSize);
    if (0 >= abbrevDataSize) {
        BALL_LOG_ERROR << "No abbreviations data found in Zoneinfo file."
                       << BALL_LOG_END;
        return -9;                                                    // RETURN
    }
    result->setAbbrevDataSize(abbrevDataSize);

    return 0;
}

static
int loadLocalTimeDescriptors(
                bsl::vector<baltzo::LocalTimeDescriptor> *descriptors,
                const bsl::vector<RawLocalTimeType>&      localTimeDescriptors,
                const bsl::vector<char>&                  abbreviationBuffer)
    // Load the specified 'descriptors' with the sequence of local time
    // descriptors described by the specified 'localTimeDescriptors' holding
    // raw information read from the file, and referring to null-terminated
    // abbreviations in the specified 'abbreviationBuffer'.  Return 0 on
    // success, and a non-zero value otherwise.
{
    BALL_LOG_SET_CATEGORY(LOG_CATEGORY);

    for (bsl::size_t i = 0; i < localTimeDescriptors.size(); ++i) {
        if (!validIndex(abbreviationBuffer,
                        localTimeDescriptors[i].d_abbreviationIndex)) {
            BALL_LOG_ERROR << "Invalid abbreviation buffer index "
                           << (int)localTimeDescriptors[i].d_abbreviationIndex
                           << " found in Zoneinfo file.  Expecting [0 .. "
                           << abbreviationBuffer.size() - 1
                           << "]."
                           << BALL_LOG_END;
            return -20;                                               // RETURN
        }

        const int utcOffset = decode32(localTimeDescriptors[i].d_offset);

        if (!baltzo::LocalTimeDescriptor::isValidUtcOffsetInSeconds(
                                                                  utcOffset)) {
            BALL_LOG_ERROR << "Invalid UTC offset "
                           << utcOffset
                           << " found in Zoneinfo file.  Expecting "
                           << "[-86399 .. 86399]."
                           << BALL_LOG_END;

            return -21;                                               // RETURN
        }
        const bool isDst = localTimeDescriptors[i].d_isDst;

        // Passing the address of the first character pointed by the index (C
        // string).

        const char *description =
              &abbreviationBuffer[localTimeDescriptors[i].d_abbreviationIndex];

        // Check if 'description' is null-terminated.

        const int maxLength = abbreviationBuffer.size()
                              - localTimeDescriptors[i].d_abbreviationIndex
                              - 1;
        if (maxLength < bdlb::String::strnlen(description, maxLength + 1)) {
            BALL_LOG_ERROR << "Abbreviation string is not null-terminated."
                           << BALL_LOG_END;
            return -22;                                               // RETURN
        }

        descriptors->push_back(baltzo::LocalTimeDescriptor(utcOffset,
                                                          isDst,
                                                          description));
    }

    return 0;
}

static
int readVersion2FormatData(baltzo::Zoneinfo             *zoneinfoResult,
                           baltzo::ZoneinfoBinaryHeader *headerResult,
                           bsl::istream&                 stream)
    // Read time zone information in the version '2' file format from the
    // specified 'stream', and load the description into the specified
    // 'zoneinfoResult', and the header information into the specified
    // 'headerResult'.  Return 0 on success and a non-zero value if 'stream'
    // does not provide a sequence of bytes consistent with version '2'
    // Zoneinfo binary format.  The 'stream' must refer to the first byte of
    // the version '2' header (which typically follows the version '\0' format
    // data in a Zoneinfo binary file).  If an error occurs during the
    // operation, the resulting value of 'zoneinfoResult' is unspecified.
{
    BALL_LOG_SET_CATEGORY(LOG_CATEGORY);

    int rc = readHeader(headerResult, stream);
    if (0 != rc) {
        return rc;                                                    // RETURN
    }

    bsl::vector<bdlb::BigEndianInt64> transitions;
    if (0 != readRawArray(&transitions,
                          stream,
                          headerResult->numTransitions())) {
        BALL_LOG_ERROR << "Error reading transitions from Zoneinfo file."
                       << BALL_LOG_END
        return -23;                                                   // RETURN
    }

    bsl::vector<unsigned char> localTimeIndices;
    if (0 != readRawArray(&localTimeIndices,
                          stream,
                          headerResult->numTransitions())){
        BALL_LOG_ERROR << "Error reading local time indices from "
                       << "Zoneinfo file."
                       << BALL_LOG_END;
        return -24;                                                   // RETURN
    }

    bsl::vector<RawLocalTimeType> localTimeDescriptors;
    if (0 != readRawArray(&localTimeDescriptors,
                          stream,
                          headerResult->numLocalTimeTypes())){
        BALL_LOG_ERROR << "Error reading local-time types from Zoneinfo file."
                       << BALL_LOG_END;
        return -25;                                                   // RETURN
    }

    bsl::vector<char> abbreviationBuffer;
    if (0 != readRawArray(&abbreviationBuffer,
                          stream,
                          headerResult->abbrevDataSize())) {
        BALL_LOG_ERROR << "Error reading abbreviation buffer from Zoneinfo "
                       << "file."
                       << BALL_LOG_END;
        return -26;                                                   // RETURN
    }

    bsl::vector<RawLeapInfo64> leapInfos;
    if (0 != readRawArray(&leapInfos, stream, headerResult->numLeaps())) {
        BALL_LOG_ERROR << "Error reading leap information from Zoneinfo file."
                       << BALL_LOG_END;
        return -27;                                                   // RETURN
    }

    bsl::vector<unsigned char> isGmt;
    if (0 != readRawArray(&isGmt, stream, headerResult->numIsGmt())) {
        BALL_LOG_ERROR << "Error reading 'isGmt' information from Zoneinfo "
                       << "file."
                       << BALL_LOG_END;
        return -28;                                                   // RETURN
    }

    bsl::vector<unsigned char> isStd;
    if (0 != readRawArray(&isStd, stream, headerResult->numIsStd())) {
        BALL_LOG_ERROR << "Error reading 'isStd' information from Zoneinfo "
                       << "file."
                       << BALL_LOG_END;
        return -29;                                                   // RETURN
    }

    // Convert raw type objects into their associated types exposed by
    // 'baltzo::Zoneinfo'.  Verify any data offsets read from the file to
    // ensure they are within valid boundaries.

    // Convert the 'Raw' local-time types into
    // 'zoneinfoResult->localTimeDescriptors()'.

    bsl::vector<baltzo::LocalTimeDescriptor> descriptors;
    if (0 != loadLocalTimeDescriptors(&descriptors,
                                      localTimeDescriptors,
                                      abbreviationBuffer)) {
        BALL_LOG_ERROR << "Error reading local time descriptors from Zoneinfo "
                       << "file."
                       << BALL_LOG_END;
        return -30;                                                   // RETURN
    }

    // Add default transition.

    const bsls::Types::Int64 firstTransitionTime =
                   baltzo::Zoneinfo::convertToTimeT64(bdlt::Datetime(1, 1, 1));

    zoneinfoResult->addTransition(firstTransitionTime, descriptors.front());

    // Convert the 'Raw' transitions information into
    // 'zoneinfoResult->transitions()'.

    for (bsl::size_t i = 0; i < transitions.size(); ++i) {
        if (!validIndex(descriptors, localTimeIndices[i])) {
            BALL_LOG_ERROR << "Invalid local-type type index "
                           << (int)localTimeIndices[i]
                           << " found in Zoneinfo file.  Expecting [0 .. "
                           << descriptors.size() - 1
                           << "]."
                           << BALL_LOG_END;
            return -31;                                               // RETURN
        }

        if (i > 0 && transitions[i - 1] >= transitions[i]) {
            BALL_LOG_ERROR << "Transition time is not in ascending order."
                           << BALL_LOG_END;
            return -32;                                               // RETURN
        }

        const int curDescriptorIndex = localTimeIndices[i];

        zoneinfoResult->addTransition(transitions[i],
                                      descriptors[curDescriptorIndex]);
    }

    return 0;
}

                         // --------------------------
                         // class ZoneinfoBinaryReader
                         // --------------------------

int baltzo::ZoneinfoBinaryReader::read(Zoneinfo      *zoneinfoResult,
                                       bsl::istream&  stream)
{
    ZoneinfoBinaryHeader description;
    return read(zoneinfoResult, &description, stream);
}

int baltzo::ZoneinfoBinaryReader::read(Zoneinfo             *zoneinfoResult,
                                       ZoneinfoBinaryHeader *headerResult,
                                       bsl::istream&         stream)
{
    BALL_LOG_SET_CATEGORY(LOG_CATEGORY);

    int rc = readHeader(headerResult, stream);
    if (0 != rc) {
        return rc;                                                    // RETURN
    }

    bsl::vector<bdlb::BigEndianInt32> transitions;
    if (0 != readRawArray(&transitions,
                          stream,
                          headerResult->numTransitions())) {
        BALL_LOG_ERROR << "Error reading transitions from Zoneinfo file."
                       << BALL_LOG_END
        return -10;                                                   // RETURN
    }

    bsl::vector<unsigned char> localTimeIndices;
    if (0 != readRawArray(&localTimeIndices,
                          stream,
                          headerResult->numTransitions())){
        BALL_LOG_ERROR << "Error reading local time indices from "
                       << "Zoneinfo file."
                       << BALL_LOG_END;
        return -11;                                                   // RETURN
    }

    bsl::vector<RawLocalTimeType> localTimeDescriptors;
    if (0 != readRawArray(&localTimeDescriptors,
                          stream,
                          headerResult->numLocalTimeTypes())){
        BALL_LOG_ERROR << "Error reading local-time types from Zoneinfo file."
                       << BALL_LOG_END;
        return -12;                                                   // RETURN
    }

    bsl::vector<char> abbreviationBuffer;
    if (0 != readRawArray(&abbreviationBuffer,
                          stream,
                          headerResult->abbrevDataSize())) {
        BALL_LOG_ERROR << "Error reading abbreviation buffer from Zoneinfo "
                       << "file."
                       << BALL_LOG_END;
        return -13;                                                   // RETURN
    }

    bsl::vector<RawLeapInfo> leapInfos;
    if (0 != readRawArray(&leapInfos, stream, headerResult->numLeaps())) {
        BALL_LOG_ERROR << "Error reading leap information from Zoneinfo file."
                       << BALL_LOG_END;
        return -14;                                                   // RETURN
    }

    bsl::vector<unsigned char> isGmt;
    if (0 != readRawArray(&isGmt, stream, headerResult->numIsGmt())) {
        BALL_LOG_ERROR << "Error reading 'isGmt' information from Zoneinfo "
                       << "file."
                       << BALL_LOG_END;
        return -15;                                                   // RETURN
    }

    bsl::vector<unsigned char> isStd;
    if (0 != readRawArray(&isStd, stream, headerResult->numIsStd())) {
        BALL_LOG_ERROR << "Error reading 'isStd' information from Zoneinfo "
                       << "file."
                       << BALL_LOG_END;
        return -16;                                                   // RETURN
    }

    if ('2' == headerResult->version()) {
        // If the file is version '2', then the data containing 32-bit epoch
        // offsets is immediately followed by another header and set of data
        // containing 64-bit epoch offsets.  The data containing 64-bit epoch
        // offsets is always used because it contains additional transitions
        // that can not be represented in 32-bit values.

        return readVersion2FormatData(zoneinfoResult, headerResult, stream);
                                                                      // RETURN
    }

    // Convert raw type objects into their associated types exposed by
    // 'Zoneinfo'.  Verify any data offsets read from the file to ensure they
    // are within valid boundaries.

    // Convert the 'Raw' local-time types into
    // 'zoneinfoResult->localTimeDescriptors()'.

    bsl::vector<LocalTimeDescriptor> descriptors;
    if (0 != loadLocalTimeDescriptors(&descriptors,
                                      localTimeDescriptors,
                                      abbreviationBuffer)) {
        BALL_LOG_ERROR << "Error reading local time descriptors from Zoneinfo "
                       << "file."
                       << BALL_LOG_END;
        return -17;                                                   // RETURN
    }

    // Add default transition.
    const bsls::Types::Int64 firstTransitionTime =
                     Zoneinfo::convertToTimeT64(bdlt::Datetime(1, 1, 1));
    zoneinfoResult->addTransition(firstTransitionTime, descriptors.front());

    // Convert the 'Raw' transitions information into
    // 'zoneinfoResult->transitions()'.

    for (bsl::size_t i = 0; i < transitions.size(); ++i) {
        if (!validIndex(descriptors, localTimeIndices[i])) {
            BALL_LOG_ERROR << "Invalid local-type type index "
                           << (int)localTimeIndices[i]
                           << " found in Zoneinfo file.  Expecting [0 .. "
                           << descriptors.size() - 1
                           << "]."
                           << BALL_LOG_END;
            return -18;                                               // RETURN
        }

        if (i > 0 && transitions[i - 1] >= transitions[i]) {
            BALL_LOG_ERROR << "Transition time is not in ascending order."
                           << BALL_LOG_END;
            return -19;                                               // RETURN
        }

        const int curDescriptorIndex = localTimeIndices[i];

        zoneinfoResult->addTransition(transitions[i],
                                      descriptors[curDescriptorIndex]);
    }

    return 0;
}

}  // close enterprise namespace

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
