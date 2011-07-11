// bcemt_default.cpp                                                  -*-C++-*-
#include <bcemt_default.h>

#include <bces_atomictypes.h>
#include <bces_platform.h>

#include <bslmf_assert.h>
#include <bsls_assert.h>
#include <bsls_platform.h>
#include <bsls_types.h>

#include <bsl_limits.h>

#if   defined(BCES_PLATFORM__POSIX_THREADS)
# include <pthread.h>
#elif defined(BCES_PLATFORM__WIN32_THREADS)
# include <windows.h>
#else
# error unrecognized threading platform
#endif

#include <bdes_ident.h>
BDES_IDENT_RCSID(bcemt_default_cpp,"$Id$ $CSID$")

static BloombergLP::bces_AtomicInt defaultThreadStackSizeValue = -1;

#ifdef BSLS_PLATFORM__OS_SOLARIS
static int solarisNativeDefaultThreadStackSize()
    // Return the native thread stack size for Solaris.
{
    // 1 megabyte on 32 bit, 2 megabytes on 64 bit

    enum { SOLARIS_DEFAULT_STACK_SIZE = 256 * 1024 * sizeof(void *) };
    return SOLARIS_DEFAULT_STACK_SIZE;
}
#endif

#if  defined(BCES_PLATFORM__POSIX_THREADS) && \
    !defined(BSLS_PLATFORM__OS_SOLARIS)
static int pthreadsNativeDefaultThreadStackSize()
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

        BSLS_ASSERT(threadStackSize > 0);
        const int maxint = bsl::numeric_limits<int>::max();
        BSLS_ASSERT(threadStackSize <= static_cast<bsl::size_t>(maxint));
    }

# ifdef PTHREAD_STACK_MIN
    BSLS_ASSERT(threadStackSize >= PTHREAD_STACK_MIN);
# endif

# ifdef BSLS_PLATFORM__OS_HPUX
    // Default size reported by pthreads is the sum of two equal stack sizes,
    // we want our 'stack size' to mean the size of just one stack.  This will
    // be doubled back again in 'create'.

    threadStackSize /= 2;
# endif

    return (int) threadStackSize;
}
#endif

#if defined(BCES_PLATFORM__WIN32_THREADS)
static int windowsNativeDefaultThreadStackSize()
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

    BSLS_ASSERT(threadStackSize > 0);
    BSLS_ASSERT(threadStackSize <= static_cast<bsl::size_t>(
                                             bsl::numeric_limits<int>::max()));

    return (int) threadStackSize;
}
#endif


namespace BloombergLP {

int bcemt_Default::defaultThreadStackSize()
{
    if (defaultThreadStackSizeValue.relaxedLoad() < 0) {
        defaultThreadStackSizeValue = nativeDefaultThreadStackSize();
    }

    return defaultThreadStackSizeValue.relaxedLoad();
}

int bcemt_Default::nativeDefaultThreadStackSize()
{
    static bces_AtomicInt ret = -1;

    if (ret.relaxedLoad() < 0) {

#if  defined(BCES_PLATFORM__POSIX_THREADS)
# if defined(BSLS_PLATFORM__OS_SOLARIS)
        ret = solarisNativeDefaultThreadStackSize();
# else
        ret = pthreadsNativeDefaultThreadStackSize();
# endif
#elif defined(BCES_PLATFORM__WIN32_THREADS)
        ret = windowsNativeDefaultThreadStackSize();
#else
# error unrecognized threading platform
#endif

    }

    return ret.relaxedLoad();
}

int bcemt_Default::nativeDefaultThreadGuardSize()
{
    static bces_AtomicInt ret = -1;

    if (ret.relaxedLoad() < 0) {

#if defined(BCES_PLATFORM__POSIX_THREADS)
        pthread_attr_t attr;
        int rc = pthread_attr_init(&attr);
        BSLS_ASSERT(0 == rc);

        bsl::size_t guardSizeT;
        rc = pthread_attr_getguardsize(&attr, &guardSizeT);
        BSLS_ASSERT(0 == rc);

        rc = pthread_attr_destroy(&attr);
        BSLS_ASSERT(0 == rc);

        BSLS_ASSERT(guardSizeT > 0);
        BSLS_ASSERT(guardSizeT <= static_cast<bsl::size_t>(
                                             bsl::numeric_limits<int>::max()));

        ret = (int) guardSizeT;
#else
        ret = 0;    // ignored on Windows
#endif

    }

    return ret.relaxedLoad();
}

int bcemt_Default::recommendedDefaultThreadStackSize()
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

void bcemt_Default::setDefaultThreadStackSize(int stackSize)
{
#if defined(BCES_PLATFORM__POSIX_THREADS) && defined(PTHREAD_STACK_MIN)
    BSLS_ASSERT_OPT(stackSize >= (int) PTHREAD_STACK_MIN);
#else
    BSLS_ASSERT_OPT(stackSize > 0);
#endif

    defaultThreadStackSizeValue = stackSize;
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
