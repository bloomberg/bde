// bcemt_default.cpp                                                  -*-C++-*-
#include <bcemt_default.h>

#include <bces_platform.h>

#include <bsls_assert.h>
#include <bsls_platform.h>
#include <bsls_types.h>

#include <limits.h>

#if   defined(BCES_PLATFORM__POSIX_THREADS)
# include <pthread.h>
#elif defined(BCES_PLATFORM__WIN32_THREADS)
# include <windows.h>
#else
# error unrecognized threading platform
#endif

#include <bdes_ident.h>
BDES_IDENT_RCSID(bcemt_default_cpp,"$Id$ $CSID$")

static bool defaultThreadStackSizeSet = false;
static int  defaultThreadStackSize;

namespace BloombergLP {

int bcemt_Default::defaultThreadStackSize()
{
    if (!defaultThreadStackSizeSet) {
        defaultThreadStackSize = bcemt_Default::nativeDefaultThreadStackSize();
        defaultThreadStackSizeSet = true;
    }

    return defaultThreadStackSize;
}

int bcemt_Default::nativeDefaultThreadStackSize()
{
#ifdef BCES_PLATFORM__POSIX_THREADS
# ifndef BSLS_PLATFORM__OS_SOLARIS

    pthread_attr_t attr;
    pthread_attr_init(&attr);
    bsl::size_t threadStackSize;
    int rc = pthread_attr_getstacksize(&attr, &threadStackSize);
    BSLS_ASSERT(0 == rc);
    pthread_attr_destroy(&attr);

#   ifdef PTHREAD_STACK_MIN
    BSLS_ASSERT(threadStackSize >= PTHREAD_STACK_MIN);
#   else
    BSLS_ASSERT(threadStackSize > 0);
#   endif
    BSLS_ASSERT(threadStackSize <= INT_MAX);

    return threadStackSize;

# else
    // Solaris

    enum { SOLARIS_DEFAULT_STACK_SIZE = 256 * 1024 * sizeof(void *) };
    return SOLARIS_DEFAULT_STACK_SIZE;

# endif
#else
    // Windows

    enum { WINDOWS_DEFAULT_STACK_SIZE = 0x100000 };
    return WINDOWS_DEFAULT_STACK_SIZE;
#endif
}

int bcemt_Default::recommendedDefaultThreadStackSize()
{
    enum { RECOMMENDED_DEFAULT_STACKSIZE = 256 * 1024 * sizeof(void *) };
    return RECOMMENDED_DEFAULT_STACKSIZE;
}

void bcemt_Default::setDefaultThreadStackSize(int stackSize)
{
    if (stackSize <= 0) {
        // TBD: do we want to do this?  In 'bcemt_threadattributes.h' is says
        // if the stack size of an attribute is set to a negative value the
        // platform's default should be used.  I think we should stick with
        // this and avoid ever having a ridiculous value for the default thread
        // stack size.

        stackSize = nativeDefaultThreadStackSize();
    }
#if defined(BCES_PLATFORM__POSIX_THREADS) && defined(PTHREAD_STACK_MIN)
    if (stackSize < PTHREAD_STACK_MIN) {
        // TBD: do we want to do this?

        stackSize = PTHREAD_STACK_MIN;
    }
#endif

    defaultThreadStackSize = stackSize;
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
