// bdlb_randomdevice.cpp                                              -*-C++-*-
#include <bdlb_randomdevice.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdlb_sysrandom_cpp,"$Id$ $CSID$")

#include <bsls_platform.h>
#include <bsls_assert.h>

#if defined(BSLS_PLATFORM_OS_LINUX)                                           \
 || defined(BSLS_PLATFORM_OS_SUNOS)                                           \
 || defined(BSLS_PLATFORM_OS_SOLARIS)                                         \
 || defined(BSLS_PLATFORM_OS_DARWIN)                                          \
 || defined(BSLS_PLATFORM_OS_AIX)
#define BDLB_USE_DEV_RANDOM
#include <unistd.h>
#include <fcntl.h>

#elif defined(BSLS_PLATFORM_OS_WINDOWS)
#include <windows.h>
#include <Wincrypt.h>
#include <cstdlib>
#define BDLB_USE_WIN_CRYPT

#else
#error Unknown Platfrom
#endif // defined(BSLS_PLATFORM_OS_WINDOWS)

namespace BloombergLP {
namespace bdlb {
namespace {

#ifdef BDLB_USE_DEV_RANDOM

// STATIC HELPER FUNCTIONS
static
int readFile(unsigned char *buffer, size_t numBytes, const char *filename);
    // Reads the specified 'numBytes' from the file with the specified
    // 'filename' into the specified 'buffer'.  Return 0 on success, non-zero
    // otherwise.
#else
#ifdef BDLB_USE_WIN_CRYPT
                        // ========================
                        // class HCRYPTPROV_Adapter
                        // ========================

class HCRYPTPROV_Adapter {
  // This 'class' provides a guarded wrapper around the standard
  // 'HCRYPTPROV' type for the Windows platform.

    // DATA
    HCRYPTPROV d_hCryptProv;  // context for the Windows PRG

    // CREATORS
  public:
    explicit HCRYPTPROV_Adapter(LPCTSTR container     = NULL,
                                LPCTSTR provider      = MS_DEF_PROV,
                                DWORD   provider_type = PROV_RSA_FULL,
                                DWORD   flag          = CRYPT_VERIFYCONTEXT);
        // Create a 'HCRYPTPROV_Adapter'object passing each of its
        // optionally specified parameters to the underlying 'HCRYPTPROV'
        // object.  See the MSDN page for 'HCRYPTPROV' for more
        // information.  If this class is unable to obtain a context,
        // the installed assert handler is called.

    ~HCRYPTPROV_Adapter();
        // Destroy this object.

    // ACCESSORS
    const HCRYPTPROV& hCryptProv() const;
        // Return a unmodifiable reference to the underlying 'HCRYPTPROV'
        // object.
};

#endif // BDLB_USE_WIN_CRYPT
#endif // BDLB_USE_DEV_RANDOM
}  // close unnamed namespace

namespace {
#ifdef BDLB_USE_WIN_CRYPT
// Windows implementation
                        // ------------------------
                        // class HCRYPTPROV_Adapter
                        // ------------------------
// CREATORS
HCRYPTPROV_Adapter::HCRYPTPROV_Adapter(LPCTSTR container,
                                       LPCTSTR provider ,
                                       DWORD   provider_type,
                                       DWORD   flag)
{
    if (!CryptAcquireContext(&d_hCryptProv,
                              container,
                              provider,
                              provider_type,
                              flag))
    {
    //-------------------------------------------------------------------
    // An error occurred in acquiring the context.  This could mean that
    // the key container requested does not exist.  In this case, the
    // function can be called again to attempt to create a new key
    // container.  Error codes are defined in Winerror.h.
        if (GetLastError() == NTE_BAD_KEYSET)
        {
            if (!CryptAcquireContext(&d_hCryptProv,
                                      container,
                                      provider,
                                      provider_type,
                                      CRYPT_NEWKEYSET))
            {
                d_hCryptProv = NULL;
            }
        }
        else
        {
            d_hCryptProv = NULL;
        }
    }
    // if the context was unable to be initialized
    if (!d_hCryptProv) {
        // unconditionally call the installed 'ASSERT' handler
        bsls::Assert::invokeHandler("null d_hCryptProv", __FILE__, __LINE__);
        // invokeHandler is no return, so this line should never execute
        abort();
    }
}

HCRYPTPROV_Adapter::~HCRYPTPROV_Adapter()
{
    if (d_hCryptProv && !CryptReleaseContext(d_hCryptProv,0))
    {
        // put a logging call here when lowLevelLogging done.
    }
}

// ACCESSORS
const HCRYPTPROV& HCRYPTPROV_Adapter::hCryptProv() const
{
    return d_hCryptProv;
}

#else
#ifdef BDLB_USE_DEV_RANDOM
static
int readFile(unsigned char *buffer, size_t numBytes, const char *filename)
{
    int rval = 0;
    if (0 == numBytes)
    {
        return 0;                                                     // RETURN
    }

    int fileData = open(filename, O_RDONLY);
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
            count = static_cast<int>(read(fileData,
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
        close(fileData);
    }
    return rval;
}

#endif // BDLB_USE_DEV_RANDOM
#endif // BDLB_USE_WIN_CRYPT
}  // close unnamed namespace

                        // ------------------------
                        // class bdlb::RandomDevice
                        // ------------------------

// CLASS METHODS
int RandomDevice::getRandomBytes(unsigned char *buffer, size_t numBytes)
{
#ifdef BDLB_USE_WIN_CRYPT

    static HCRYPTPROV_Adapter hCryptProv;
    // if the context is NULL, return error, otherwise return the return value
    // of CryptGenRandom
    return hCryptProv.hCryptProv() &&
           !CryptGenRandom(hCryptProv.hCryptProv(),
                           numBytes,
                           static_cast<BYTE *>(buffer));

#else
    return readFile(buffer, numBytes, "/dev/random");
#endif
}

int RandomDevice::getRandomBytesNonBlocking(unsigned char *buffer,
                                            size_t         numBytes)
{
#ifdef BDLB_USE_WIN_CRYPT
    return getRandomBytes(buffer, numBytes);
#else
    return readFile(buffer, numBytes, "/dev/urandom");
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
