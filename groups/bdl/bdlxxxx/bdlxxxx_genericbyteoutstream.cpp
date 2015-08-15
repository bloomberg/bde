// bdlxxxx_genericbyteoutstream.cpp                                   -*-C++-*-
#include <bdlxxxx_genericbyteoutstream.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdlxxxx_genericbyteoutstream_cpp,"$Id$ $CSID$")

#include <bslmf_assert.h>

#include <bsls_types.h>

namespace BloombergLP {

BSLMF_ASSERT(8 == sizeof(bsls::Types::Int64));
BSLMF_ASSERT(4 == sizeof(int));
BSLMF_ASSERT(2 == sizeof(short));
BSLMF_ASSERT(8 == sizeof(double));
BSLMF_ASSERT(4 == sizeof(float));

}  // close enterprise namespace

// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2007
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ----------------------------------
