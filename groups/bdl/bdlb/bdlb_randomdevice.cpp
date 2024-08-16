// bdlb_randomdevice.cpp                                              -*-C++-*-
#include <bdlb_randomdevice.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdlb_sysrandom_cpp,"$Id$ $CSID$")

#include <bsls_platform.h>
#include <bsls_assert.h>

#if defined(BSLS_PLATFORM_OS_UNIX)
# include <unistd.h>
# include <fcntl.h>
#elif defined(BSLS_PLATFORM_OS_WINDOWS)
# include <windows.h>
# include <bcrypt.h>
# include <cstdlib>
#else
# error Unknown Platfrom
#endif    // UNIX / WINDOWS

namespace {
namespace u {

using namespace BloombergLP;

#if defined(BSLS_PLATFORM_OS_UNIX)

int readFile(unsigned char *buffer, size_t numBytes, const char *filename)
    // Reads the specified 'numBytes' from the file with the specified
    // 'filename' into the specified 'buffer'.  Return 0 on success, non-zero
    // otherwise.
{
    int rval = 0;
    if (0 == numBytes)
    {
        return 0;                                                     // RETURN
    }

    int fileData = ::open(filename, O_RDONLY);
    int count = 0;
    if (fileData < 0)
    {
        // Issue opening the file

        rval = -1;
    }

    // successfully opened the file

    else
    {
        size_t fileDataLen = 0;
        do
        {
            count = static_cast<int>(::read(fileData,
                                            buffer + fileDataLen,
                                            numBytes - fileDataLen));
            if (count < 0)
            {
                rval = -2;
                break;
            }
            fileDataLen += count;
        }
        while (fileDataLen < numBytes); // continue read until the requested
                                        // number bytes read
        ::close(fileData);
    }
    return rval;
}

#endif    // UNIX

}  // close namespace u
}  // close unnamed namespace

namespace BloombergLP {
namespace bdlb {

                        // ------------------------
                        // class bdlb::RandomDevice
                        // ------------------------

// CLASS METHODS
int RandomDevice::getRandomBytes(unsigned char *buffer, size_t numBytes)
{
    if (0 == numBytes) {
        return 0;                                                     // RETURN
    }
    else {
        BSLS_ASSERT(buffer);

#ifdef BSLS_PLATFORM_OS_WINDOWS
        return ::BCryptGenRandom(0,
                                 buffer,
                                 numBytes,
                                 BCRYPT_USE_SYSTEM_PREFERRED_RNG);    // RETURN
#else
        return u::readFile(buffer, numBytes, "/dev/random");          // RETURN
#endif
    }
}

int RandomDevice::getRandomBytesNonBlocking(unsigned char *buffer,
                                            size_t         numBytes)
{
#ifdef BSLS_PLATFORM_OS_WINDOWS
    return RandomDevice::getRandomBytes(buffer, numBytes);
#else
    return u::readFile(buffer, numBytes, "/dev/urandom");
#endif
}

}  // close package namespace
}  // close enterprise namespace

// ----------------------------------------------------------------------------
// Copyright 2014 Bloomberg Finance L.P.
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
