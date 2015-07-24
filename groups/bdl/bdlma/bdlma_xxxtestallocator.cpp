// bdlma_xxxtestallocator.cpp                                            -*-C++-*-
#include <bslma_testallocator.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdlma_xxxtestallocator_cpp,"$Id$ $CSID$")

#include <bdlqq_lockguard.h>
#include <bsl_ostream.h>

namespace BloombergLP {

                        // -------------------------
                        // class bslma::TestAllocator
                        // -------------------------

// FREE OPERATORS
bsl::ostream& bdlma::operator<<(bsl::ostream& lhs, const TestAllocator& rhs)
{
    bdlqq::LockGuard<bdlqq::Mutex> lock(&rhs.d_mutex);

    lhs << '\n';
    lhs << "==================================================" << '\n';
    lhs << "                TEST ALLOCATOR STATE" << '\n';
    lhs << "--------------------------------------------------" << '\n';
    lhs << "        Category\tBlocks\tBytes" << '\n';
    lhs << "        --------\t------\t-----" << '\n';
    lhs << "          IN USE\t" << rhs.numBlocksInUse() <<
                   '\t' << rhs.numBytesInUse() << '\n';
    lhs << "             MAX\t" << rhs.numBlocksMax() <<
                   '\t' << rhs.numBytesMax() << '\n';
    lhs << "           TOTAL\t" << rhs.numBlocksTotal() <<
                   '\t' << rhs.numBytesTotal() << '\n';
    lhs << "  NUM MISMATCHES\t" << rhs.numMismatches() << '\n';
    lhs << "--------------------------------------------------" << '\n';

    return lhs;
}

}  // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2010
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
