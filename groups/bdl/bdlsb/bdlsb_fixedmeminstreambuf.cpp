// bdlsb_fixedmeminstreambuf.cpp                                      -*-C++-*-
#include <bdlsb_fixedmeminstreambuf.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdlsb_fixedmeminstreambuf_cpp,"$Id$ $CSID$")

#include <bsls_types.h>

#include <bsl_ostream.h>
#include <bsl_iomanip.h>
#include <bsls_assert.h>


namespace BloombergLP {

namespace bdlsb {
// PROTECTED MANIPULATORS
FixedMemInStreamBuf::pos_type FixedMemInStreamBuf::seekpos(
                                    FixedMemInStreamBuf::pos_type pos,
                                    bsl::ios_base::openmode             which)
{
    // This is an input buffer only, so cannot "seek" in "put" area.
    if (!(which & bsl::ios_base::in)) {
        return pos_type(-1);
    }

    if (off_type(pos) > d_length || off_type(pos) < 0) {
        return pos_type(-1);
    }

    setg(d_buffer_p,
         d_buffer_p + static_cast<bsls::Types::IntPtr>(pos),
         d_buffer_p + d_length);

    return pos;
}

// Seekoff follows seekpos out of order so that seekpos is inlined.
FixedMemInStreamBuf::pos_type FixedMemInStreamBuf::seekoff(
                             FixedMemInStreamBuf::off_type offset,
                             bsl::ios_base::seekdir              fixedPosition,
                             bsl::ios_base::openmode             which)
{
    char_type *basePtr =
        (fixedPosition == bsl::ios_base::cur ? gptr() :
         fixedPosition == bsl::ios_base::beg ? eback() : egptr());

    return seekpos(pos_type(basePtr - eback() + offset), which);
}

FixedMemInStreamBuf::int_type FixedMemInStreamBuf::pbackfail(
                                                            int_type character)
{
    if (traits_type::eof() == character) {
        return character;
    }

    BSLS_ASSERT(*pptr() == character);

    return character;
}
}  // close package namespace

} // end namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2004
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
