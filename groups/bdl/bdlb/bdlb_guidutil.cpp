// bdlb_guidutil.cpp                                                  -*-C++-*-
#include <bdlb_guidutil.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(RCSid_bdlb_guidutil_cpp,"$Id$ $CSID$")

#include <bdlb_guid.h>
#include <bdlb_randomdevice.h>

#include <bslmf_assert.h>
#include <bsls_byteorder.h>
#include <bsls_platform.h>

#include <bsl_cstring.h>
#include <bsl_iostream.h>
#include <bsl_sstream.h>

namespace BloombergLP {
namespace bdlb {

namespace {

                        // ---------------
                        // struct GuidUtil
                        // ---------------

// LOCAL METHODS
int charToHex(unsigned char* hex, unsigned char c)
    // Convert the character in the specified 'c' to it suitable hex
    // equivalent if one exists, load the value into the specified 'hex'.
    // Return 0 if the conversion was successful, and non-zero otherwise.
{
    switch (c) {
      default: return -1;                                             // RETURN
      case '0': *hex = 0; break;
      case '1': *hex = 1; break;
      case '2': *hex = 2; break;
      case '3': *hex = 3; break;
      case '4': *hex = 4; break;
      case '5': *hex = 5; break;
      case '6': *hex = 6; break;
      case '7': *hex = 7; break;
      case '8': *hex = 8; break;
      case '9': *hex = 9; break;
      case 'a': case 'A': *hex = 10; break;
      case 'b': case 'B': *hex = 11; break;
      case 'c': case 'C': *hex = 12; break;
      case 'd': case 'D': *hex = 13; break;
      case 'e': case 'E': *hex = 14; break;
      case 'f': case 'F': *hex = 15; break;
    }
    return 0;
}

int vaildateGuidString(bslstl::StringRef guidString)
    // Validate the specified 'guidString' against the language given in
    // {LANGUAGE SPECIFICATION}.  Return 0 if 'guidString' is accepted, and
    // non-zero otherwise.
{
    bsl::size_t length = guidString.length();
    if (length < 32 || length > 40){
        return -1;                                                    // RETURN
    }

    const unsigned char *front_p = reinterpret_cast<const unsigned char *>(
                                                      &guidString[0]);
    const unsigned char *back_p = reinterpret_cast<const unsigned char *>(
                                                      &guidString[length - 1]);

    // check for braces
    if (('[' == *front_p && ']' == *back_p) ||
        ('{' == *front_p && '}' == *back_p)) {
        // check for spaces
        front_p++;
        back_p--;
        if (' ' == *front_p)  {
            if (' ' != *back_p) {
                return -1;                                            // RETURN
            }
            front_p++;
            back_p--;
        }
    }

    // check for '-'
    if ('-' == front_p[8] &&
        ('-' != front_p[13] || '-' != front_p[18] || '-' != front_p[23]))
    {
        return -1;                                                    // RETURN
    }

    int digitCnt = 0;
    while (front_p <= back_p) {
        switch (*front_p++) {
          default:
            return -1;                                                // RETURN
          case '0': case '1': case '2': case '3': case '4':
          case '5': case '6': case '7': case '8': case '9':
          case 'A': case 'B': case 'C': case 'D': case 'E': case 'F':
          case 'a': case 'b': case 'c': case 'd': case 'e': case 'f':
            ++digitCnt;
          case '-':
            ;
        }
    }
    if (32 != digitCnt) {
        return -1;                                                    // RETURN
    }
    return 0;
}

}  // close unnamed namespace

// CLASS METHODS
Guid GuidUtil::generate()
{
    Guid result;
    generate(&result);
    return result;
}

void GuidUtil::generate(unsigned char *result, bsl::size_t numGuids)
{
    unsigned char *bytes = result;
    unsigned char *end = bytes + numGuids * Guid::k_GUID_NUM_BYTES;
    RandomDevice::getRandomBytesNonBlocking(bytes, end - bytes);
    while (bytes != end) {
        typedef unsigned char uc;
        bytes[6] = uc(0x40 | (bytes[6] & 0x0F));
        bytes[8] = uc(0x80 | (bytes[8] & 0x3F));
        bytes += Guid::k_GUID_NUM_BYTES;
    }
}

void GuidUtil::generate(Guid *result, bsl::size_t numGuids)
{
    generate(reinterpret_cast<unsigned char *>(result), numGuids);
}

bsls::Types::Uint64 GuidUtil::getLeastSignificantBits(const Guid& guid)
{
    bsls::Types::Uint64 result = 0;
    bsl::memcpy(&result, &guid[8], sizeof(result));
    return result;
}

bsls::Types::Uint64 GuidUtil::getMostSignificantBits(const Guid& guid)
{
    bsls::Types::Uint64 result = 0;
    bsl::memcpy(&result, &guid[0], sizeof(result));
    return result;
}

int GuidUtil::guidFromString(Guid *result, bslstl::StringRef guidString)
{
    int valid = vaildateGuidString(guidString);
    if (0 != valid) {
        return -1;                                                    // RETURN
    }
    // CHECK ASCII
    BSLS_ASSERT_SAFE('f' - 'a' == 5);
    BSLS_ASSERT_SAFE('F' - 'A' == 5);

    unsigned char        t_guid[Guid::k_GUID_NUM_BYTES];
    const unsigned char *curr_p =
                   reinterpret_cast<const unsigned char *>(guidString.begin());
    const unsigned char *end =
                   reinterpret_cast<const unsigned char *>(guidString.end());
    bsl::size_t          i = 0;
    while (curr_p != end && i < Guid::k_GUID_NUM_BYTES) {
        unsigned char upper, lower;
        if (0 == charToHex(&upper, *curr_p)
        &&  0 == charToHex(&lower, *(curr_p + 1))) {
            t_guid[i] = static_cast<unsigned char>((
                                        (upper << 4) & 0Xf0) | (lower & 0x0f));
            i++;
            curr_p +=2;
        }
        else {
            curr_p++;
        }
    }
    *result =  Guid(t_guid);
    return 0;
}

Guid GuidUtil::guidFromString(bslstl::StringRef guidString)
{
    Guid result;
    guidFromString(&result, guidString);
    return result;
}

void GuidUtil::guidToString(bsl::string *result, const Guid& guid)
{
    bsl::ostringstream oss;
    guid.print(oss, 0, -1);
    *result = oss.str();
}

bsl::string GuidUtil::guidToString(const Guid& guid)
{
    bsl::string result;
    guidToString(&result, guid);
    return result;
}

}  // close package namespace
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
