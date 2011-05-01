// bdetu_epoch.cpp                                                    -*-C++-*-
#include <bdetu_epoch.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(bdetu_epoch_cpp,"$Id$ $CSID$")

#include <bsls_assert.h>

namespace BloombergLP {

                        // *** CONSTANTS ***

// The following structure represents the footprint of a 'bdet_Datetime' object
// (implemented as two 32-bit integers (today)).  Either the static integers
// combine to represent the correct date (i.e., 1970/1/1_00:00:00.000) or they
// don't, in which case the test driver will fail pretty darn quickly (but see
// the supererogatory check in debug mode below).  If 'bdet_Datetime' is
// changed to a single 'bsls_Types::Int64' in the future, the 'struct' 'lucky'
// should be updated to contain a single 'bsls_Types::Int64' member.  The
// initializer would also need to be updated appropriately to reflect the
// implementation change in the 'bdet_Datetime' component to ensure correctness
// on both little- and big-endian machines.

static struct {
    int d_a;
    int d_b;
} lucky = { 719165, 0 };

const bdet_Datetime *bdetu_Epoch::s_epoch_p = (bdet_Datetime *) &lucky;

#ifndef NDEBUG // Supererogatory check in debug mode only.

static inline
int iGuessedRight() {
    BSLS_ASSERT(bdet_Datetime(1970, 1, 1) == bdetu_Epoch::epoch());
    return 0;
}
static int iReallyWantToBeSureThat = iGuessedRight();

#endif

}  // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2004
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
