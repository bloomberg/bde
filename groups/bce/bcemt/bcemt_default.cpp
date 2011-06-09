// bcemt_default.cpp                                                  -*-C++-*-
#include <bcemt_default.h>

#include <bces_platform.h>

#include <bslmf_assert.h>
#include <bsls_assert.h>
#include <bsls_platform.h>
#include <bsls_types.h>

#include <bsl_limits.h>

#if    defined(BCES_PLATFORM__POSIX_THREADS)
# include <pthread.h>
#elif !defined(BCES_PLATFORM__WIN32_THREADS)
# error unrecognized threading platform
#endif

#include <bdes_ident.h>
BDES_IDENT_RCSID(bcemt_default_cpp,"$Id$ $CSID$")

static bool defaultThreadStackSizeSet = false;
static int  defaultThreadStackSizeValue;

namespace BloombergLP {

int bcemt_Default::defaultThreadStackSize()
{
    if (0 == defaultThreadStackSizeSet) {
        defaultThreadStackSizeValue = nativeDefaultThreadStackSize();
        defaultThreadStackSizeSet = true;
    }

    return defaultThreadStackSizeValue;
}

int bcemt_Default::nativeDefaultThreadStackSize()
{
#ifdef BCES_PLATFORM__POSIX_THREADS
# ifndef BSLS_PLATFORM__OS_SOLARIS

    bsl::size_t threadStackSize = 0;

    {
        pthread_attr_t attr;

        pthread_attr_init(&attr);
        int rc = pthread_attr_getstacksize(&attr, &threadStackSize);
        BSLS_ASSERT(0 == rc);
        pthread_attr_destroy(&attr);
    }

#  ifdef PTHREAD_STACK_MIN
    BSLS_ASSERT(threadStackSize >= PTHREAD_STACK_MIN);
#  endif

    BSLS_ASSERT(threadStackSize > 0);
    BSLS_ASSERT(threadStackSize <= static_cast<bsl::size_t>(INT_MAX));

    return threadStackSize;

# else
    // Solaris -- 1 megabyte on 32 bit, 2 megabytes on 64 bit

    enum { SOLARIS_DEFAULT_STACK_SIZE = 256 * 1024 * sizeof(void *) };
    return SOLARIS_DEFAULT_STACK_SIZE;

# endif
#else
    // Windows

    enum { WINDOWS_DEFAULT_STACK_SIZE = 0x100000 };    // 1 megabyte always
    return WINDOWS_DEFAULT_STACK_SIZE;
#endif
}

int bcemt_Default::recommendedDefaultThreadStackSize()
{
    // 1 megabyte on 32 bit, 2 megabytes on 64 bit, constant across platforms

#ifdef PTHREAD_STACK_MIN
    BSLMF_ASSERT(RECOMMENDED_DEFAULT_STACKSIZE >= PTHREAD_STACK_MIN);
#endif

    enum { RECOMMENDED_DEFAULT_STACKSIZE = 256 * 1024 * sizeof(void *) };
    return RECOMMENDED_DEFAULT_STACKSIZE;
}

void bcemt_Default::setDefaultThreadStackSize(int stackSize)
{
    if (stackSize <= 0) {
        // TBD: do we want to do this?  In 'bcemt_threadattributes.h' is says
        // if the stack size of an attribute is set to a negative value the
        // platform's default should be used.  I think we should stick with
        // this and avoid ever having a ridiculous value returned by
        // 'defaultThreadStackSize'.

        stackSize = nativeDefaultThreadStackSize();
    }
#if defined(BCES_PLATFORM__POSIX_THREADS) && defined(PTHREAD_STACK_MIN)
    if (stackSize < PTHREAD_STACK_MIN) {
        // TBD: do we want to do this?

        stackSize = PTHREAD_STACK_MIN;
    }
#endif

    defaultThreadStackSizeValue = stackSize;
    defaultThreadStackSizeSet = true;
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
