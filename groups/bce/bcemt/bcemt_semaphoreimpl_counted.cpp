// bcemt_semaphoreimpl_counted.cpp                                    -*-C++-*-
#include <bcemt_semaphoreimpl_counted.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(bcemt_semaphoreimpl_counted_cpp,"$Id$ $CSID$")

#ifdef BCES_PLATFORM_COUNTED_SEMAPHORE

#include <bcemt_lockguard.h>    // for testing only
#include <bcemt_mutex.h>        // for testing only
#include <bcemt_threadutil.h>   // for testing only

namespace BloombergLP {
}  // close namespace BloombergLP

#endif  // BCES_PLATFORM_POSIX_THREADS

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2010
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
