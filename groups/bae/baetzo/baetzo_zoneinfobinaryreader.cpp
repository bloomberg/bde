// baetzo_zoneinfobinaryreader.cpp                                    -*-C++-*-
#include <baetzo_zoneinfobinaryreader.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(baetzo_zoneinfobinaryreader_cpp,"$Id$ $CSID$")

#include <baetzo_zoneinfo.h>
#include <baetzo_zoneinfobinaryheader.h>

#include <bael_log.h>

#include <bcema_sharedptr.h>

#include <bdeut_bigendian.h>

#include <bdeu_string.h>

#include <bslmf_assert.h>

#include <bsl_fstream.h>
#include <bsl_istream.h>
#include <bsl_ostream.h>
#include <bsl_sstream.h>
#include <bsl_vector.h>
#include <bsl_cstring.h>

#include <bsls_byteorder.h>

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

static const char LOG_CATEGORY[] = "baetzo.ZONEINFOBINARYREADER";

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
    // format.

    char d_transition[4];  // POSIX time at which the leap second occur
    char d_correction[4];  // accumulated leap correction
};

BSLMF_ASSERT(8 == sizeof(RawLeapInfo));

}  // close unnamed namespace

static const char *EXPECTED_HEADER_ID = "TZif";
    // The first 4 bytes of a valid Zoneinfo database file.

template <typename TYPE>
static inline
int readRawArray(bsl::vector<TYPE> *result,
                 bsl::istream&      stream,
                 int                numValues)
    // Read the specific 'numValues' of the parameterized 'TYPE' from
    // the specified 'stream' into the specified 'result'.  Return 0 on success
    // and -1 if the read failed.  Note that 'sizeof(TYPE)' must equal the size
    // of the packed stream data.
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

template <typename TYPE>
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
bsls_Types::Int64 decode64(const char *address)
    // Read the 64-bit big-endian integer in the array of bytes located at the
    // specified 'address' and return that value.  The behavior is undefined
    // unless 'address' points to an accessible memory location.  Note that
    // this function is currently not used, but will be needed when version 2
    // of the Zoneinfo binary file is supported.
{
    BSLS_ASSERT_SAFE(address);

    bsls_Types::Int64 temp;
    bsl::memcpy(&temp, address, sizeof(temp));
    return BSLS_BYTEORDER_BE_U64_TO_HOST(temp);
}

static inline
int readHeader(baetzo_ZoneinfoBinaryHeader *result,
               bsl::istream&                stream)
    // Extract header information from the specified 'stream' and, if the data
    // meets the requirements of the Zoneinfo binary file format, populate the
    // specified 'result' with the extracted information.  Return 0 if 'result'
    // is successfully read, and a non-zero value otherwise.
{
    BSLS_ASSERT(result);

    BAEL_LOG_SET_CATEGORY(LOG_CATEGORY);

    RawHeader rawHeader;
    if (!stream.read((char *)&rawHeader, sizeof(RawHeader))) {
        BAEL_LOG_ERROR << "Unable to read Zoneinfo header." << BAEL_LOG_END;
        return -1;                                                    // RETURN
    }

    if (0 != bsl::memcmp(EXPECTED_HEADER_ID, rawHeader.d_headerId, 4)) {
        bsl::string headerId(rawHeader.d_headerId, 4);
        BAEL_LOG_ERROR << "Did not find expected header id.  Expecting "
                       << "'TZif', found '" + headerId + "'"
                       << BAEL_LOG_END;
        return -2;                                                    // RETURN
    }

    char version = *rawHeader.d_version;
    if ('\0' != version && '2' != version) {
        BAEL_LOG_ERROR << "Found unexpected version value: "
                       << (int)version
                       << " ('"
                       << version
                       << "').  Expecting '\\0' or '2'."
                       << BAEL_LOG_END;
        return -3;                                                    // RETURN
    }
    result->setVersion(version);

    int numLocalTimeTypes = decode32(rawHeader.d_numLocalTimeTypes);
    if (0 >= numLocalTimeTypes) {
        BAEL_LOG_ERROR << "Empty list of local-time types in Zoneinfo file."
                       << BAEL_LOG_END;
        return -4;                                                    // RETURN
    }
    result->setNumLocalTimeTypes(numLocalTimeTypes);

    int numIsGmt = decode32(rawHeader.d_numIsGmt);
    if (0 > numIsGmt) {
        BAEL_LOG_ERROR << "Invalid number of 'isGmt' flags "
                       << numIsGmt
                       << " found in Zoneinfo file."
                       << BAEL_LOG_END;
        return -5;                                                    // RETURN
    }
    result->setNumIsGmt(numIsGmt);

    int numIsStd = decode32(rawHeader.d_numIsStd);
    if (0 > numIsStd) {
        BAEL_LOG_ERROR << "Invalid number of 'isStd' flags "
                       << numIsStd
                       << " found in Zoneinfo file."
                       << BAEL_LOG_END;
        return -6;                                                    // RETURN
    }
    result->setNumIsStd(numIsStd);

    // The 'isGmt' and 'isStd' values provide information about the transition
    // times associated with a local-time type, so the number of values (i.e.,
    // 'numIsGmt' and 'numIsStd') should be the same as the number of
    // local-time types.  They may also be 0 for backward compatibility reason.

    if ((0 != numIsGmt && numIsGmt != numLocalTimeTypes)
        || (0 != numIsStd && numIsStd != numLocalTimeTypes)) {
        BAEL_LOG_WARN << "Unexpected number of isGmt or isStd values in "
                      << "Zoneinfo file."
                      << BAEL_LOG_END;
    }

    int numLeaps = decode32(rawHeader.d_numLeaps);
    if (0 != numLeaps) {
        BAEL_LOG_ERROR << "Non-zero number of leap corrections found in "
                       << "Zoneinfo file.  Leap correction is not supported "
                       << "by 'baetzo_ZoneinfoBinaryReader'."
                       << BAEL_LOG_END;
        return -7;
    }
    result->setNumLeaps(numLeaps);

    int numTransitions = decode32(rawHeader.d_numTransitions);
    if (0 > numTransitions) {
        BAEL_LOG_ERROR << "Invalid number of transitions found in Zoneinfo "
                       << "file."
                       << BAEL_LOG_END;
        return -8;
    }
    result->setNumTransitions(numTransitions);

    int abbrevDataSize = decode32(rawHeader.d_abbrevDataSize);
    if (0 >= abbrevDataSize) {
        BAEL_LOG_ERROR << "No abbreviations data found in Zoneinfo file."
                       << BAEL_LOG_END;
        return -9;
    }
    result->setAbbrevDataSize(abbrevDataSize);

    return 0;
}

                      // ---------------------------------
                      // class baetzo_ZoneinfoBinaryReader
                      // ---------------------------------

int baetzo_ZoneinfoBinaryReader::read(baetzo_Zoneinfo *zoneinfoResult,
                                      bsl::istream&    stream)
{
    baetzo_ZoneinfoBinaryHeader description;
    return read(zoneinfoResult, &description, stream);
}

int baetzo_ZoneinfoBinaryReader::read(
                                   baetzo_Zoneinfo             *zoneinfoResult,
                                   baetzo_ZoneinfoBinaryHeader *headerResult,
                                   bsl::istream&                stream)
{
    BAEL_LOG_SET_CATEGORY(LOG_CATEGORY);

    int rc = readHeader(headerResult, stream);
    if (0 != rc) {
        return rc;                                                    // RETURN
    }

    bsl::vector<bdeut_BigEndianInt32> transitions;
    if (0 != readRawArray(&transitions,
                          stream,
                          headerResult->numTransitions())) {
        BAEL_LOG_ERROR << "Error reading transitions from Zoneinfo file."
                       << BAEL_LOG_END
        return -10;                                                   // RETURN
    }

    bsl::vector<unsigned char> localTimeIndices;
    if (0 != readRawArray(&localTimeIndices,
                          stream,
                          headerResult->numTransitions())){
        BAEL_LOG_ERROR << "Error reading local time indices from "
                       << "Zoneinfo file."
                       << BAEL_LOG_END;
        return -11;                                                   // RETURN
    }

    bsl::vector<RawLocalTimeType> localTimeDescriptors;
    if (0 != readRawArray(&localTimeDescriptors,
                          stream,
                          headerResult->numLocalTimeTypes())){
        BAEL_LOG_ERROR << "Error reading local-time types from Zoneinfo file."
                       << BAEL_LOG_END;
        return -12;                                                   // RETURN
    }

    bsl::vector<char> abbreviationBuffer;
    if (0 != readRawArray(&abbreviationBuffer,
                          stream,
                          headerResult->abbrevDataSize())) {
        BAEL_LOG_ERROR << "Error reading abbreviation buffer from Zoneinfo "
                       << "file."
                       << BAEL_LOG_END;
        return -13;                                                   // RETURN
    }

    bsl::vector<RawLeapInfo> leapInfos;
    if (0 != readRawArray(&leapInfos, stream, headerResult->numLeaps())) {
        BAEL_LOG_ERROR << "Error reading leap information from Zoneinfo file."
                       << BAEL_LOG_END;
        return -14;                                                   // RETURN
    }

    bsl::vector<unsigned char> isGmt;
    if (0 != readRawArray(&isGmt, stream, headerResult->numIsGmt())) {
        BAEL_LOG_ERROR << "Error reading 'isGmt' information from Zoneinfo "
                       << "file."
                       << BAEL_LOG_END;
        return -15;                                                   // RETURN
    }

    bsl::vector<unsigned char> isStd;
    if (0 != readRawArray(&isStd, stream, headerResult->numIsStd())) {
        BAEL_LOG_ERROR << "Error reading 'isStd' information from Zoneinfo "
                       << "file."
                       << BAEL_LOG_END;
        return -16;                                                   // RETURN
    }

    // Convert raw type objects into their associated types exposed by
    // 'baetzo_Zoneinfo'.  Verify any data offsets read from the file to ensure
    // they are within valid boundaries.

    // Convert the 'Raw' local-time types into
    // 'zoneinfoResult->localTimeDescriptors()'.

    bsl::vector<baetzo_LocalTimeDescriptor> descriptors;
    for (bsl::size_t i = 0; i < localTimeDescriptors.size(); ++i) {
        if (!validIndex(abbreviationBuffer,
                        localTimeDescriptors[i].d_abbreviationIndex)) {
            BAEL_LOG_ERROR << "Invalid abbreviation buffer index "
                           << (int)localTimeDescriptors[i].d_abbreviationIndex
                           << " found in Zoneinfo file.  Expecting [0 .. "
                           << abbreviationBuffer.size() - 1
                           << "]."
                           << BAEL_LOG_END;
            return -17;                                               // RETURN
        }

        const int utcOffset = decode32(localTimeDescriptors[i].d_offset);

        if (!baetzo_LocalTimeDescriptor::isValidUtcOffsetInSeconds(utcOffset)){
            BAEL_LOG_ERROR << "Invalid UTC offset "
                           << utcOffset
                           << " found in Zoneinfo file.  Expecting "
                           << "[-86399 .. 86399]."
                           << BAEL_LOG_END;

            return -18;                                               // RETURN
        }
        const bool isDst = localTimeDescriptors[i].d_isDst;

        // Passing the address of the first character pointed by the index (C
        // string).

        const char *description =
              &abbreviationBuffer[localTimeDescriptors[i].d_abbreviationIndex];

        // Check if 'description' is null-terminated.

        const int maxLength = headerResult->abbrevDataSize()
                              - localTimeDescriptors[i].d_abbreviationIndex
                              - 1;
        if (maxLength < bdeu_String::strnlen(description, maxLength + 1)) {
            BAEL_LOG_ERROR << "Abbreviation string is not null-terminated."
                           << BAEL_LOG_END;
            return -19;
        }

        descriptors.push_back(baetzo_LocalTimeDescriptor(utcOffset,
                                                         isDst,
                                                          description));
    }

    // Add default transition.
    const bsls_Types::Int64 firstTransitionTime =
                        bdetu_Epoch::convertToTimeT64(bdet_Datetime(1, 1, 1));
    zoneinfoResult->addTransition(firstTransitionTime, descriptors.front());

    // Convert the 'Raw' transitions information into
    // 'zoneinfoResult->transitions()'.

    for (bsl::size_t i = 0; i < transitions.size(); ++i) {
        if (!validIndex(descriptors, localTimeIndices[i])) {
            BAEL_LOG_ERROR << "Invalid local-type type index "
                           << (int)localTimeIndices[i]
                           << " found in Zoneinfo file.  Expecting [0 .. "
                           << descriptors.size() - 1
                           << "]."
                           << BAEL_LOG_END;
            return -21;                                               // RETURN
        }

        if (i > 0 && transitions[i - 1] >= transitions[i]) {
            BAEL_LOG_ERROR << "Transition time is not in ascending order."
                           << BAEL_LOG_END;
            return -22;                                               // RETURN
        }

        const int curDescriptorIndex = localTimeIndices[i];

        zoneinfoResult->addTransition(transitions[i],
                                      descriptors[curDescriptorIndex]);
    }

    return 0;
}

}  // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2011
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
