// bdlmtt_once.cpp                                                     -*-C++-*-
#include <bdlmtt_once.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdlmtt_once_cpp,"$Id$ $CSID$")

#include <bdlmtt_threadattributes.h>    // for testing only
#include <bdlmtt_threadutil.h>          // for testing only
#include <bdlmtt_barrier.h>             // for testing only

#include <bsl_exception.h>

#include <bsls_assert.h>

namespace BloombergLP {

namespace bdlmtt {
                        // ----------------
                        // class Once
                        // ----------------

bool Once::enter(Once::OnceLock *onceLock)
{
    if (BCEMT_DONE == bdlmtt::AtomicUtil::getInt(d_state)) {
        return false;
    }

    onceLock->lock(&d_mutex);  // Lock the mutex
    switch (bdlmtt::AtomicUtil::getInt(d_state)) {

      case BCEMT_NOT_ENTERED:
        bdlmtt::AtomicUtil::setInt(&d_state, BCEMT_IN_PROGRESS);
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

void Once::leave(Once::OnceLock *onceLock)
{
    BSLS_ASSERT(BCEMT_IN_PROGRESS == bdlmtt::AtomicUtil::getInt(d_state));

    bdlmtt::AtomicUtil::setInt(&d_state, BCEMT_DONE);
    onceLock->unlock();
}

void Once::cancel(Once::OnceLock *onceLock)
{
    BSLS_ASSERT(BCEMT_IN_PROGRESS == bdlmtt::AtomicUtil::getInt(d_state));

    bdlmtt::AtomicUtil::setInt(&d_state, BCEMT_NOT_ENTERED);
    onceLock->unlock();
}

                        // ---------------------
                        // class OnceGuard
                        // ---------------------

OnceGuard::~OnceGuard()
{
    if (BCEMT_IN_PROGRESS != d_state) {
        return;
    }
#if ! defined(BSLS_PLATFORM_CMP_MSVC)
    else if (bsl::uncaught_exception()) {
        d_once->cancel(&d_onceLock);
    }
#endif
    else {
        d_once->leave(&d_onceLock);
    }
}

bool OnceGuard::enter()
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

void OnceGuard::leave()
{
    if (BCEMT_IN_PROGRESS == d_state) {
        d_once->leave(&d_onceLock);
        d_state = BCEMT_DONE;
    }
}

void OnceGuard::cancel()
{
    if (BCEMT_IN_PROGRESS == d_state) {
        d_once->cancel(&d_onceLock);
        d_state = BCEMT_NOT_ENTERED;
    }
}
}  // close package namespace

}  // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2007
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
