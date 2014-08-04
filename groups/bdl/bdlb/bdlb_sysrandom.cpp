// bdlb_sysrandom.cpp                                                 -*-C++-*-
#include <bdlb_sysrandom.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdlb_sysrandom_cpp,"$Id$ $CSID$")

#include <bsls_platform.h>

#if defined(BSLS_PLATFORM_OS_LINUX)                                           \
 || defined(BSLS_PLATFORM_OS_SUNOS)                                           \
 || defined(BSLS_PLATFORM_OS_SOLARIS)                                         \
 || defined(BSLS_PLATFORM_OS_DARWIN)
#define BDLB_USE_SYS_RAND
#elif defined(BSLS_PLATFORM_OS_WINDOWS)
#include <windows.h>
#include <Wincrypt.h>
#define BDLB_USE_WIN_CRYPT
#else
#error
#endif
#include <unistd.h>
 #include <fcntl.h>
#include <bsl_fstream.h>
#include <bsl_iostream.h>

namespace BloombergLP {
namespace bdlb {
namespace {

#ifdef BDLB_USE_SYS_RAND
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
                                    LPCTSTR provider      = NULL,
                                    DWORD   provider_type = PROV_RSA_FULL,
                                    DWORD   flag          = 0   );
            // Create a 'HCRYPTPROV_Adapter'object passing each of its
            // optionally specified parameters to the underlying 'HCRYPTPROV'
            // object.  See the MSDN page for 'HCRYPTPROV' for more
            // information.

        ~HCRYPTPROV_Adapter();
            // Destroy this object.

        // ACCESSORS
        const HCRYPTPROV& hCryptProv() const;
            // Return a unmodifiable reference to the underlying 'HCRYPTPROV'
            // object.
    };

#endif // BDLB_USE_WIN_CRYPT
#endif // BDLB_USE_SYS_RAND
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
}

HCRYPTPROV_Adapter::~HCRYPTPROV_Adapter()
{
    if (d_hCryptProv && !CryptReleaseContext(d_hCryptProv,0))
    {
    }
}

// ACCESSORS
const HCRYPTPROV& HCRYPTPROV_Adapter::hCryptProv() const
{
    return d_hCryptProv;
}

#else
#ifdef BDLB_USE_SYS_RAND
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
            count = read(fileData,
                         buffer + fileDataLen,
                         numBytes - fileDataLen);
            if (count < 0)
            {
                rval = -2;
                break;
            }
            fileDataLen += count;
        }
        while (fileDataLen < numBytes); // continue read until the requested
                                        // number bytes read
    }
    close(fileData);
    return rval;
}

#endif // BDLB_USE_SYS_RAND
#endif // BDLB_USE_WIN_CRYPT
}  // close unnamed namespace

                        // --------------------
                        // class bdlb_SysRandom
                        // --------------------

// CLASS METHODS
int SysRandom::getRandomBytes(unsigned char *buffer, size_t numBytes)
{
#ifdef BDLB_USE_WIN_CRYPT

    static HCRYPTPROV_Adapter hCryptProv;
    return !CryptGenRandom(hCryptProv.hCryptProv(),
                           numBytes,
                           static_cast<BYTE *>(buffer));

#else
    return readFile(static_cast<unsigned char *>(buffer),
                    numBytes,
                    "/dev/random");
#endif
}

int SysRandom::getRandomBytesNonBlocking(unsigned char *buffer,
                                         size_t    numBytes)
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
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2014
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ----------------------------------
