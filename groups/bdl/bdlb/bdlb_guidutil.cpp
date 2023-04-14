// bdlb_guidutil.cpp                                                  -*-C++-*-
#include <bdlb_guidutil.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(RCSid_bdlb_guidutil_cpp,"$Id$ $CSID$")

#include <bdlb_guid.h>
#include <bdlb_pcgrandomgenerator.h>
#include <bdlb_randomdevice.h>

#include <bdlde_sha1.h>

#include <bslmf_assert.h>

#include <bslmt_lockguard.h>
#include <bslmt_mutex.h>
#include <bslmt_once.h>

#include <bsls_assert.h>
#include <bsls_atomic.h>
#include <bsls_byteorder.h>
#include <bsls_performancehint.h>
#include <bsls_platform.h>

#include <bsl_algorithm.h>
#include <bsl_cstdio.h>
#include <bsl_cstring.h>
#include <bsl_ctime.h>
#include <bsl_sstream.h>
#include <bsl_vector.h>

// The following is added so that this component does not need a dependency on
// bdls_processutil, since 'getProcessId' is only used to for random number
// seeding.
#ifndef BSLS_PLATFORM_OS_WINDOWS
#include <unistd.h>
#include <pthread.h>
#endif

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

int vaildateGuidString(const bsl::string_view& guidString)
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
                                    &guidString[static_cast<int>(length) - 1]);

    // check for braces
    if (('[' == *front_p && ']' == *back_p) ||
        ('{' == *front_p && '}' == *back_p)) {
        // check for spaces
        front_p++;
        back_p--;
        if (' ' == *front_p) {
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

    int digitCount = 0;
    while (front_p <= back_p) {
        switch (*front_p++) {
        default:
            return -1;                                                // RETURN
        case '0': case '1': case '2': case '3': case '4':
        case '5': case '6': case '7': case '8': case '9':
        case 'A': case 'B': case 'C': case 'D': case 'E': case 'F':
        case 'a': case 'b': case 'c': case 'd': case 'e': case 'f':
            ++digitCount;
        case '-':
            ;
        }
    }
    if (32 != digitCount) {
        return -1;                                                    // RETURN
    }
    return 0;
}

inline
void makeRfc4122Compliant(unsigned char *bytes, const unsigned char *end)
    // Make the buffer starting at the specified 'bytes' and ending at the
    // address just before the specified 'end' comply with RFC 4122 version 4.
    // The behavior is undefined unless 'bytes <= end' and the size of the
    // buffer defined by 'end - bytes' is a non-negative multiple of
    // 'Guid::k_GUID_NUM_BYTES'.
{
    BSLS_ASSERT(bytes);
    BSLS_ASSERT(end);
    BSLS_ASSERT(bytes <= end);
    BSLS_ASSERT(0 == (end - bytes) % Guid::k_GUID_NUM_BYTES);

    typedef unsigned char Uchar;
    while (bytes < end) {
        bytes[6] = Uchar(0x40 | (bytes[6] & 0x0F));
        bytes[8] = Uchar(0x80 | (bytes[8] & 0x3F));
        bytes += Guid::k_GUID_NUM_BYTES;
    }
}

template <class STRING, class OSTRINGSTREAM>
void guidToStringImpl(STRING *result, const Guid& guid)
    // Load into the specified 'result' the string representation of the
    // specified 'guid'.
{
    BSLS_ASSERT(result);

    OSTRINGSTREAM oss;
    guid.print(oss, 0, -1);
    *result = oss.str();
}

inline int getPid()
    // Return the process id.  Having this be separate from 'Obj::getProcessId'
    // allows us to call it inline within the component.
{
#ifdef BSLS_PLATFORM_OS_WINDOWS
    // Return a placeholder; the actual PID is not important since there is no
    // possibility of forking on Windows.
    return 1;
#else
    return static_cast<int>(::getpid());
#endif
}

static bsls::AtomicInt s_pid;

extern "C" void guidUtilForkChildCallback()
    // Callback for the child process as set by pthread_atfork.  If the process
    // forks we need to reset the state and s_pid to force a reseed.  At this
    // point during the forking process we are guaranteed to only have a single
    // running thread.
{
    s_pid = 0;
}

void registerForkCallback()
    // Register 'guidUtilForkChildCallback' as a callback function for the
    // child of a call to 'fork'.
{
#ifndef BSLS_PLATFORM_OS_WINDOWS
    pthread_atfork(NULL, NULL, (guidUtilForkChildCallback));
#endif
}

inline
void reseed(GuidState_Imp *guidStatePtr)
    // Reseed the array of random number generators of length 4 in the
    // specified 'guidStatePtr' with seeds from 'RandomDevice'.
{
    BSLS_ASSERT(guidStatePtr);

    typedef bsl::array<bsl::uint64_t, GuidState_Imp::k_GENERATOR_COUNT>
        state_t;

    state_t state;
    if (0 != RandomDevice::getRandomBytesNonBlocking(
                               reinterpret_cast<unsigned char *>(state.data()),
                               sizeof(state_t::value_type) * state.size())) {
        // fallback state: Combine the time and (for unix only) the process id
        // with the addresses of a library function, a static variable, a stack
        // variable and a local function to approximate process-specific
        // semi-random seed values.
        bsl::uint64_t seed = static_cast<bsl::uint64_t>(bsl::time(0)) ^
                             reinterpret_cast<uintptr_t>(&bsl::printf);
        state[0] = seed;
        state[1] = seed ^ (static_cast<bsl::uint64_t>(s_pid) << 32) ^
                   reinterpret_cast<uintptr_t>(&state);
        state[2] = (seed << 32) ^
                   reinterpret_cast<uintptr_t>(&registerForkCallback);
        state[3] = seed ^ reinterpret_cast<uintptr_t>(&s_pid);
    }

    guidStatePtr->seed(state);
}

}  // close unnamed namespace

                              // ===================
                              // class GuidState_Imp
                              // ===================

// MANIPULATORS
void GuidState_Imp::seed(
      const bsl::array<bsl::uint64_t, GuidState_Imp::k_GENERATOR_COUNT>& state)
{
    for (int i = 0; i != GuidState_Imp::k_GENERATOR_COUNT; ++i) {
        d_generators[i].seed(state[i], i);
    }
}

                               // ==============
                               // class GuidUtil
                               // ==============

// CLASS METHODS
void GuidUtil::generate(unsigned char *result, bsl::size_t numGuids)
{
    BSLS_ASSERT(result);

    unsigned char *bytes = result;
    unsigned char *end   = bytes + numGuids * Guid::k_GUID_NUM_BYTES;

    // Note that, on a correcly installed system, the RandomDevice call whould
    // be expected to never fail.
    if (BSLS_PERFORMANCEHINT_PREDICT_LIKELY(
                  0 == RandomDevice::getRandomBytesNonBlocking(bytes,
                                                               end - bytes))) {
        makeRfc4122Compliant(bytes, end);
    }
    else {
        for (bsl::size_t i = 0; i < numGuids; i++) {
            Guid temp = generateNonSecure();
            memcpy(result + i * Guid::k_GUID_NUM_BYTES,
                   temp.data(),
                   Guid::k_GUID_NUM_BYTES);
        }
    }
}

Guid GuidUtil::generate()
{
    Guid result;
    generate(&result);
    return result;
}

void GuidUtil::generateNonSecure(Guid *result, bsl::size_t numGuids)
{
    BSLS_ASSERT(result);

    static GuidState_Imp *guidStatePtr;
    static bslmt::Mutex  *pcgMutexPtr;

    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(!s_pid.loadRelaxed())) {
        BSLMT_ONCE_DO
        {
            registerForkCallback();

            static GuidState_Imp guidState;
            guidStatePtr = &guidState;

            static bslmt::Mutex pcgMutex;
            pcgMutexPtr = &pcgMutex;
        }

        bslmt::LockGuard<bslmt::Mutex> guard(pcgMutexPtr);

        if (!s_pid.load()) {
            s_pid = getPid();
            reseed(guidStatePtr);
        }
    }

    {
        bslmt::LockGuard<bslmt::Mutex> guard(pcgMutexPtr);

        for (bsl::size_t i = 0; i < numGuids; i++) {
            bsl::uint32_t temp[Guid::k_GUID_NUM_32BITS];
            guidStatePtr->generateRandomBits(&temp);
            result[i] = temp;
        }
    }

    unsigned char *bytes = reinterpret_cast<unsigned char *>(result);
    unsigned char *end   = bytes + numGuids * Guid::k_GUID_NUM_BYTES;

    makeRfc4122Compliant(bytes, end);
}

Guid GuidUtil::generateNonSecure()
{
    Guid result;
    generateNonSecure(&result);
    return result;
}

Guid GuidUtil::generateFromName(const Guid&             namespaceId,
                                const bsl::string_view& name)
{
    bdlde::Sha1 sha1(namespaceId.data(), Guid::k_GUID_NUM_BYTES);
    sha1.update(name.data(), name.length());

    unsigned char digest[bdlde::Sha1::k_DIGEST_SIZE];
    sha1.loadDigest(digest);
    // Overwrite the 4 most significant bits of 'time_hi_and_version' (bytes 6
    // and 7) to 5 (the version number).
    digest[6] = (0x0f & digest[6]) | 0x50;
    // Overwrite the 2 most significant bits of 'clock_seq_hi_and_reserved'
    // (byte 8) to 1 and 0 as specified by RFC 4122.
    digest[8] = (0x3f & digest[8]) | 0x80;

    unsigned char guidBytes[bdlb::Guid::k_GUID_NUM_BYTES];
    bsl::copy(digest, digest + sizeof(guidBytes), guidBytes);

    return Guid(guidBytes);
}

Guid GuidUtil::dnsNamespace()
{
    static const unsigned char k_DNS_UUID[] = {
        0x6b, 0xa7, 0xb8, 0x10, 0x9d, 0xad, 0x11, 0xd1,
        0x80, 0xb4, 0x00, 0xc0, 0x4f, 0xd4, 0x30, 0xc8
    };
    return Guid(k_DNS_UUID);
}

Guid GuidUtil::urlNamespace()
{
    static const unsigned char k_URL_UUID[] = {
        0x6b, 0xa7, 0xb8, 0x11, 0x9d, 0xad, 0x11, 0xd1,
        0x80, 0xb4, 0x00, 0xc0, 0x4f, 0xd4, 0x30, 0xc8
    };
    return Guid(k_URL_UUID);
}

Guid GuidUtil::oidNamespace()
{
    static const unsigned char k_OID_UUID[]  = {
        0x6b, 0xa7, 0xb8, 0x12, 0x9d, 0xad, 0x11, 0xd1,
        0x80, 0xb4, 0x00, 0xc0, 0x4f, 0xd4, 0x30, 0xc8
    };
    return Guid(k_OID_UUID);
}

Guid GuidUtil::x500Namespace()
{
    static const unsigned char k_X500_UUID[] = {
        0x6b, 0xa7, 0xb8, 0x14, 0x9d, 0xad, 0x11, 0xd1,
        0x80, 0xb4, 0x00, 0xc0, 0x4f, 0xd4, 0x30, 0xc8
    };
    return Guid(k_X500_UUID);
};

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

int GuidUtil::guidFromString(Guid *result, const bsl::string_view& guidString)
{
    BSLS_ASSERT(result);

    int valid = vaildateGuidString(guidString);
    if (0 != valid) {
        return -1;                                                    // RETURN
    }

    unsigned char t_guid[Guid::k_GUID_NUM_BYTES];

    bsl::string_view::const_iterator iter = guidString.begin();
    bsl::string_view::const_iterator end  = guidString.end();
    bsl::size_t i = 0;
    while (iter != end && i < Guid::k_GUID_NUM_BYTES) {
        unsigned char upper, lower;
        if (0 == charToHex(&upper, *iter)
        &&  0 == charToHex(&lower, *(iter + 1))) {
            t_guid[i] = static_cast<unsigned char>((
                                        (upper << 4) & 0Xf0) | (lower & 0x0f));
            i++;
            iter +=2;
        }
        else {
            ++iter;
        }
    }
    *result =  Guid(t_guid);
    return 0;
}

Guid GuidUtil::guidFromString(const bsl::string_view& guidString)
{
    Guid result;
    guidFromString(&result, guidString);
    return result;
}

void GuidUtil::guidToString(bsl::string *result, const Guid& guid)
{
    guidToStringImpl<bsl::string, bsl::ostringstream>(result, guid);
}

void GuidUtil::guidToString(std::string *result, const Guid& guid)
{
    guidToStringImpl<std::string, std::ostringstream>(result, guid);
}

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_PMR
void GuidUtil::guidToString(std::pmr::string *result, const Guid& guid)
{
    guidToStringImpl<std::pmr::string, std::ostringstream>(result, guid);
}
#endif

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
