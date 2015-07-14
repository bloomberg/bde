// baltzo_dstpolicy.cpp                                               -*-C++-*-

#include <baltzo_dstpolicy.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(baltzo_dstpolicy_cpp,"$Id$ $CSID$")

#include <bdlb_print.h>

#include <bsl_ostream.h>

namespace BloombergLP {

namespace baltzo {
                     // -----------------------
                     // struct DstPolicy
                     // -----------------------

// CLASS METHODS
bsl::ostream& DstPolicy::print(bsl::ostream&          stream,
                                      DstPolicy::Enum value,
                                      int                    level,
                                      int                    spacesPerLevel)
{
    bdlb::Print::indent(stream, level, spacesPerLevel);

    stream << DstPolicy::toAscii(value);

    if (spacesPerLevel >= 0) {
        stream << '\n';
    }

    return stream;
}

const char *DstPolicy::toAscii(DstPolicy::Enum value)
{
#define CASE(X) case(BAETZO_ ## X): return #X;

    switch (value) {
      CASE(DST)
      CASE(STANDARD)
      CASE(UNSPECIFIED)
      default: return "(* UNKNOWN *)";
    }

#undef CASE
}
}  // close package namespace

}  // close namespace BloombergLP

// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2011
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ----------------------------------
