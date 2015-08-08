// btlsc_flag.cpp            -*-C++-*-
#include <btlsc_flag.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(btlsc_flag_cpp,"$Id$ $CSID$")

#include <bsl_ostream.h>

namespace BloombergLP {

const char *btesc_Flag::toAscii(Flag value)
{
#define CASE(X) case(k_ ## X): return #X

    switch (value) {
      CASE(ASYNC_INTERRUPT);
      CASE(RAW);
      default: return "(* UNKNOWN *)";
    }

#undef CASE
}

bsl::ostream& operator<<(bsl::ostream& stream, btesc_Flag::Flag rhs)
{
    return stream << btesc_Flag::toAscii(rhs);
}

}  // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2007
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
