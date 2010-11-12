// bcemt_once.cpp                                                     -*-C++-*-
#include <bcemt_once.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(bcemt_once_cpp,"$Id$ $CSID$")

#include <bcemt_threadattributes.h>    // for testing only
#include <bcemt_threadutil.h>          // for testing only
#include <bcemt_barrier.h>             // for testing only

#include <bsl_exception.h>

#include <bsls_assert.h>

namespace BloombergLP {

                        // ----------------
                        // class bcemt_Once
                        // ----------------

bool bcemt_Once::enter(bcemt_Once::OnceLock *onceLock)
{
    if (BCEMT_DONE == bces_AtomicUtil::getInt(d_state)) {
        return false;
    }

    onceLock->lock(&d_mutex);  // Lock the mutex
    switch (bces_AtomicUtil::getInt(d_state)) {

      case BCEMT_NOT_ENTERED:
        bces_AtomicUtil::setInt(&d_state, BCEMT_IN_PROGRESS);
        return true;  // Leave mutex locked

      case BCEMT_IN_PROGRESS:
        BSLS_ASSERT(! "Can't get here!");
        break;

      case BCEMT_DONE:
        onceLock->unlock();
        return false;
    }

    return false;
}

void bcemt_Once::leave(bcemt_Once::OnceLock *onceLock)
{
    BSLS_ASSERT(BCEMT_IN_PROGRESS == bces_AtomicUtil::getInt(d_state));

    bces_AtomicUtil::setInt(&d_state, BCEMT_DONE);
    onceLock->unlock();
}

void bcemt_Once::cancel(bcemt_Once::OnceLock *onceLock)
{
    BSLS_ASSERT(BCEMT_IN_PROGRESS == bces_AtomicUtil::getInt(d_state));

    bces_AtomicUtil::setInt(&d_state, BCEMT_NOT_ENTERED);
    onceLock->unlock();
}

                        // ---------------------
                        // class bcemt_OnceGuard
                        // ---------------------

bcemt_OnceGuard::~bcemt_OnceGuard()
{
    if (BCEMT_IN_PROGRESS != d_state) {
        return;
    }
#if ! defined(BSLS_PLATFORM__CMP_MSVC)
    else if (bsl::uncaught_exception()) {
        d_once->cancel(&d_onceLock);
    }
#endif
    else {
        d_once->leave(&d_onceLock);
    }
}

bool bcemt_OnceGuard::enter()
{
    if (BCEMT_DONE == d_state) {
        return false;
    }

    BSLS_ASSERT(BCEMT_IN_PROGRESS != d_state);
    BSLS_ASSERT(d_once);

    if (d_once->enter(&d_onceLock)) {
        d_state = BCEMT_IN_PROGRESS;
        return true;
    }
    else {
        return false;
    }
}

void bcemt_OnceGuard::leave()
{
    if (BCEMT_IN_PROGRESS == d_state) {
        d_once->leave(&d_onceLock);
        d_state = BCEMT_DONE;
    }
}

void bcemt_OnceGuard::cancel()
{
    if (BCEMT_IN_PROGRESS == d_state) {
        d_once->cancel(&d_onceLock);
        d_state = BCEMT_NOT_ENTERED;
    }
}

}  // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2007
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
