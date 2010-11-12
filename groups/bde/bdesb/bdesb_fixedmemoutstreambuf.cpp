// bdesb_fixedmemoutstreambuf.cpp               -*-C++-*-
#include <bdesb_fixedmemoutstreambuf.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(bdesb_fixedmemoutstreambuf_cpp,"$Id$ $CSID$")


#include <bsl_iomanip.h>
#include <bsl_ostream.h>

namespace BloombergLP {

// PROTECTED MANIPULATORS
bdesb_FixedMemOutStreamBuf::pos_type
bdesb_FixedMemOutStreamBuf::seekoff(off_type                offset,
                                    bsl::ios_base::seekdir  fixedPosition,
                                    bsl::ios_base::openmode which)
{
    // This is an output buffer only, so cannot "seek" in "get" area.
    if (!(which & bsl::ios_base::out)) {
        return pos_type(-1);
    }

    // Compute offset from current position.  In this stream, pptr() defines
    // both the current position and the end of the logical byte stream.
    // Thus, bsl::ios_base::curr and bsl::ios_base::end are handled
    // identically.
    off_type currOffset = (bsl::ios_base::beg == fixedPosition ?
                           offset - length() : offset);

    // currOffset id invalid if it is positive or has an
    // absolute-value > length.
    if (currOffset > 0 || -currOffset > length()) {
        return pos_type(-1);
    }

    pbump(currOffset);

    return pos_type(length());
}

bdesb_FixedMemOutStreamBuf::pos_type
bdesb_FixedMemOutStreamBuf::seekpos(pos_type                position,
                                    bsl::ios_base::openmode which)
{
    return seekoff(off_type(position), bsl::ios_base::beg, which);
}

} // end namespace

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2004
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
