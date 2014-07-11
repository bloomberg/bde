// bdlb_sysrandom.cpp                                                 -*-C++-*-
#include <bdlb_sysrandom.h>
#include <bdlb_bitutil.h>

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
BSLMF_ASSERT(!"Unsupported platform.");
#endif

#include <fstream>
#include <bsl_iostream.h>
namespace BloombergLP {

#ifdef BDLB_USE_WIN_CRYPT

namespace {
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
            // object. See the MSDN page for 'HCRYPTPROV' for more information.

        ~HCRYPTPROV_Adapter();
            // Destroy this object.

        // ACCESSORS
        const HCRYPTPROV& hCryptProv() const;
            // Return a unmodifiable reference to the underlying 'HCRYPTPROV'
            // object.
    };
}
#endif


#ifdef BDLB_USE_SYS_RAND

// STATIC HELPER FUNCTIONS
static
int readFile(void *buffer, unsigned numBytes, const char *filename);
    // Reads the specified 'numBytes' from the file with the specified
    // 'filename'  into the specified 'buffer'.
#endif

#ifdef BDLB_USE_WIN_CRYPT

// Windows implementation
namespace {
                        // ------------------------
                        // class HCRYPTPROV_Adapter
                        // ------------------------
    // CREATORS
    HCRYPTPROV_Adapter::HCRYPTPROV_Adapter(LPCTSTR container,
                                           LPCTSTR provider ,
                                           DWORD   provider_type,
                                           DWORD   flag)
    {
        if(!CryptAcquireContext(&d_hCryptProv,
                                container,
                                provider,
                                provider_type,
                                flag))
        {
        //-------------------------------------------------------------------
        // An error occurred in acquiring the context. This could mean
        // that the key container requested does not exist. In this case,
        // the function can be called again to attempt to create a new key
        // container. Error codes are defined in Winerror.h.
            if (GetLastError() == NTE_BAD_KEYSET)
            {
                if(!CryptAcquireContext(&d_hCryptProv,
                                        container,
                                        provider,
                                        provider_type,
                                        CRYPT_NEWKEYSET))
                {
                    bsl::cerr << "Could not create a new key container."
                              << bsl::endl;
                    d_hCryptProv = NULL;
                }
            }
            else
            {
                bsl::cerr << "A cryptographic service handle could not be "
                                 "acquired." << bsl::endl;
                d_hCryptProv = NULL;
            }
        }
    }

    HCRYPTPROV_Adapter::~HCRYPTPROV_Adapter()
    {
        if (!CryptReleaseContext(d_hCryptProv,0))
        {
            std::cerr << "The handle could not be released." << bsl::endl;
        }
    }

    // ACCESSORS
    const HCRYPTPROV& HCRYPTPROV_Adapter::hCryptProv() const
    {
        return d_hCryptProv;
    }
}
#endif

#ifdef BDLB_USE_SYS_RAND

static
int readFile(void *buffer, unsigned numBytes, const char *filename)
{
    char *cbuffer = static_cast<char *>(buffer);
    int rval = 0;
    std::ifstream fileData (filename, std::ios::binary);

    if (!fileData)
    {
        // Issue opening the file
        rval = -1;
    }

    // successfully opened the file
    else
    {
        std::streamsize fileDataLen = 0;
        do
        {
            if (!fileData.read(cbuffer + fileDataLen, numBytes - fileDataLen))
            {
                rval = -2;
                break;
            }
            fileDataLen += fileData.gcount();
        }
        while (fileDataLen < numBytes); // continue read until the requested
                                        // number bytes read
    }
    return rval;
}

#endif
                        // --------------------
                        // class bdlb_SysRandom
                        // --------------------

// CLASS METHODS
int bdlb::SysRandom::randomN(void *buffer, unsigned numBytes)
{
#ifdef BDLB_USE_WIN_CRYPT

    static HCRYPTPROV_Adapter hCryptProv;
    return !CryptGenRandom(hCryptProv.hCryptProv(),
                           numBytes,
                           static_cast<BYTE *>(buffer));

#else
    return readFile(buffer, numBytes, "/dev/random");
#endif
}

int bdlb::SysRandom::urandomN(void *buffer, unsigned numBytes)
{
#ifdef BDLB_USE_WIN_CRYPT
    return randomN(buffer, numBytes);
#else
    return readFile(buffer, numBytes, "/dev/urandom");
#endif
}

}

// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2014
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ----------------------------------
