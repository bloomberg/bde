// bdeu_cstringhash.cpp                                               -*-C++-*-
#include <bdeu_cstringhash.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(bdeu_cstringhash_cpp,"$Id$ $CSID$")

#include <bdeu_hashutil.h>

#include <bsls_assert.h>

#include <bsl_cstring.h>

namespace BloombergLP
{

                        // -----------------------
                        // struct bdeu_CStringHash
                        // -----------------------

// ACCESSORS
bsl::size_t bdeu_CStringHash::operator()(const char *argument) const
{
    BSLS_ASSERT_SAFE(argument);

    return bdeu_HashUtil::hash1(argument, bsl::strlen(argument));
}

}  // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2012
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
