// btesc_flag.cpp            -*-C++-*-
#include <btesc_flag.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(btesc_flag_cpp,"$Id$ $CSID$")

#include <bsl_ostream.h>

namespace BloombergLP {

const char *btesc_Flag::toAscii(Flag value)
{
#define CASE(X) case(BTESC_ ## X): return #X

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
