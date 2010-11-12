// bdesb_fixedmeminput.cpp               -*-C++-*-

#include <bdesb_fixedmeminput.h>

#include <bsls_performancehint.h>

#include <bsl_iomanip.h>
#include <bsl_ostream.h>

namespace BloombergLP {

// MANIPULATORS
bdesb_FixedMemInput::pos_type
bdesb_FixedMemInput::pubseekpos(bdesb_FixedMemInput::pos_type position,
                                bsl::ios_base::openmode       which)
{
    // This is an input buffer only, so cannot "seek" in "put" area.
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(!(which & bsl::ios_base::in))) {
        return pos_type(-1);
    }

    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(position > d_bufferLength)
     || BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(off_type(position) < 0)) {
        return pos_type(-1);
    }

    return d_pos = position;
}

bdesb_FixedMemInput::pos_type
bdesb_FixedMemInput::pubseekoff(bdesb_FixedMemInput::off_type offset,
                                bsl::ios_base::seekdir        fixedPosition,
                                bsl::ios_base::openmode       which)
{
    const char *basePtr;

    basePtr = (fixedPosition == bsl::ios_base::cur) ? d_buffer_p + d_pos
            : (fixedPosition == bsl::ios_base::beg) ? d_buffer_p
                                                    : d_buffer_p +
                                                      d_bufferLength;

    return pubseekpos(pos_type((basePtr - d_buffer_p) + offset), which);

}

} // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2007
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
