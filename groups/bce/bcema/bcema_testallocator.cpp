// bcema_testallocator.cpp                                            -*-C++-*-
#include <bcema_testallocator.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(bcema_testallocator_cpp,"$Id$ $CSID$")

#include <bcemt_lockguard.h>
#include <bsl_ostream.h>

namespace BloombergLP {

                        // -------------------------
                        // class bcema_TestAllocator
                        // -------------------------

// FREE OPERATORS
bsl::ostream& operator<<(bsl::ostream& lhs, const bcema_TestAllocator& rhs)
{
    bcemt_LockGuard<bcemt_Mutex> lock(&rhs.d_mutex);

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
