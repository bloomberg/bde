// bcemt_configuration.cpp                                            -*-C++-*-
#include <bcemt_configuration.h>

#include <bces_atomicutil.h>
#include <bces_platform.h>

#include <bslmf_assert.h>
#include <bsls_assert.h>
#include <bsls_platform.h>
#include <bsls_types.h>

#include <bsl_c_limits.h>

#if defined(BCES_PLATFORM__POSIX_THREADS)
# include <pthread.h>
#elif defined(BCES_PLATFORM__WIN32_THREADS)
# include <windows.h>
#else
# error unrecognized threading platform
#endif

#include <bdes_ident.h>
BDES_IDENT_RCSID(bcemt_configuration_cpp,"$Id$ $CSID$")

#if defined(BCES_PLATFORM__POSIX_THREADS)

# ifdef BSLS_PLATFORM__OS_SOLARIS
static int nativeDefaultThreadStackSizeImp()
    // Return the native thread stack size for Solaris.
{
    // 1 megabyte on 32 bit, 2 megabytes on 64 bit

    enum { SOLARIS_DEFAULT_STACK_SIZE = 256 * 1024 * sizeof(void *) };
    return SOLARIS_DEFAULT_STACK_SIZE;
}
# else // POSIX_THREADS && !Solaris
static int nativeDefaultThreadStackSizeImp()
    // Return the native thread stack size for pthreads platforms other than
    // Solaris.
{
    bsl::size_t threadStackSize;

    {
        pthread_attr_t attr;

        int rc = pthread_attr_init(&attr);
        BSLS_ASSERT(0 == rc);
        rc = pthread_attr_getstacksize(&attr, &threadStackSize);
        BSLS_ASSERT(0 == rc);
        rc = pthread_attr_destroy(&attr);
        BSLS_ASSERT(0 == rc);
    }

    BSLS_ASSERT(1 <= threadStackSize);
    BSLS_ASSERT(threadStackSize <= static_cast<bsl::size_t>(INT_MAX));

#  ifdef PTHREAD_STACK_MIN
    BSLS_ASSERT(threadStackSize >= PTHREAD_STACK_MIN);
#  endif

#  ifdef BSLS_PLATFORM__OS_HPUX
    // On HPUX (Itanium) there is an additional stack for register storage,
    // effectively halving the amount of memory available to clients, so we
    // adjust the returned value to reflect the amount of memory that will be
    // available to clients for initializing data on the stack.  Note that
    // mechanisms for creating a thread should have a corresponding inverse of
    // this adjustment (see 'bcemt_threadutil').

    threadStackSize /= 2;
#  endif

    return static_cast<int>(threadStackSize);
}
# endif
#else // WIN32_THREADS
static int nativeDefaultThreadStackSizeImp()
    // Return the native thread stack size for Windows.
{
    enum { WINDOWS_DEFAULT_STACK_SIZE = 0x100000 };    // 1 megabyte

    bsl::size_t threadStackSize = WINDOWS_DEFAULT_STACK_SIZE;

    // obtain default stack reserve size from the PE header

    char *imageBase = (char *) GetModuleHandle(NULL);
    if (imageBase) {
        IMAGE_OPTIONAL_HEADER *header =
             (IMAGE_OPTIONAL_HEADER *) (imageBase
                   + ((IMAGE_DOS_HEADER *) imageBase)->e_lfanew
                   + sizeof (IMAGE_NT_SIGNATURE) + sizeof (IMAGE_FILE_HEADER));
        threadStackSize = header->SizeOfStackReserve;
    }

    BSLS_ASSERT(1 <= threadStackSize);
    BSLS_ASSERT(threadStackSize <= static_cast<bsl::size_t>(INT_MAX));

    return static_cast<int>(threadStackSize);
}
#endif


namespace BloombergLP {

static bces_AtomicUtil::Int defaultThreadStackSizeValue = { -1 };

int bcemt_Configuration::defaultThreadStackSize()
{
    if (bces_AtomicUtil::getIntRelaxed(defaultThreadStackSizeValue) < 0) {
        return nativeDefaultThreadStackSize();                        // RETURN
    }

    return bces_AtomicUtil::getIntRelaxed(defaultThreadStackSizeValue);
}

int bcemt_Configuration::nativeDefaultThreadStackSize()
{
    static bces_AtomicUtil::Int ret = { -1 };

    if (bces_AtomicUtil::getIntRelaxed(ret) < 0) {
        bces_AtomicUtil::setIntRelaxed(&ret,
                                       nativeDefaultThreadStackSizeImp());
        BSLS_ASSERT(bces_AtomicUtil::getIntRelaxed(ret) >= 1);
    }

    return bces_AtomicUtil::getIntRelaxed(ret);
}

int bcemt_Configuration::nativeDefaultThreadGuardSize()
{
#if defined(BCES_PLATFORM__POSIX_THREADS)
    static bces_AtomicUtil::Int ret = { -1 };

    if (bces_AtomicUtil::getIntRelaxed(ret) < 0) {
        pthread_attr_t attr;
        int rc = pthread_attr_init(&attr);
        BSLS_ASSERT(0 == rc);

        bsl::size_t guardSizeT;
        rc = pthread_attr_getguardsize(&attr, &guardSizeT);
        BSLS_ASSERT(0 == rc);

        rc = pthread_attr_destroy(&attr);
        BSLS_ASSERT(0 == rc);

        BSLS_ASSERT(guardSizeT > 0);
        BSLS_ASSERT(guardSizeT <= static_cast<bsl::size_t>(INT_MAX));

        bces_AtomicUtil::setIntRelaxed(&ret, static_cast<int>(guardSizeT));
    }

    return bces_AtomicUtil::getIntRelaxed(ret);
#else
    // WIN32_THREADS

    return 0;    // guard size is ignored on Windows
#endif
}

int bcemt_Configuration::recommendedDefaultThreadStackSize()
{
    // 1 megabyte on 32 bit, 2 megabytes on 64 bit, constant across platforms

    enum { RECOMMENDED_DEFAULT_STACKSIZE = 256 * 1024 * sizeof(void *) };

#ifdef PTHREAD_STACK_MIN
    // Note -- this cannot be a BSLMF_ASSERT -- 'PTHREAD_STACK_MIN' is a
    // function call on some platforms.

    BSLS_ASSERT(RECOMMENDED_DEFAULT_STACKSIZE >= PTHREAD_STACK_MIN);
#endif

    return RECOMMENDED_DEFAULT_STACKSIZE;
}

void bcemt_Configuration::setDefaultThreadStackSize(int numBytes)
{
#if defined(BCES_PLATFORM__POSIX_THREADS) && defined(PTHREAD_STACK_MIN)
    BSLS_ASSERT_OPT(numBytes >= static_cast<int>(PTHREAD_STACK_MIN));
#else
    BSLS_ASSERT_OPT(numBytes > 0);
#endif

    bces_AtomicUtil::setIntRelaxed(&defaultThreadStackSizeValue, numBytes);
}

}  // close enterprise namespace

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2011
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
