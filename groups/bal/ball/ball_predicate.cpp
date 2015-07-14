// ball_predicate.cpp                 -*-C++-*-
#include <ball_predicate.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ball_predicate_cpp,"$Id$ $CSID$")

namespace BloombergLP {

namespace ball {
// ACCESSORS
bsl::ostream& Predicate::print(bsl::ostream& stream,
                                    int           level ,
                                    int           spacesPerLevel) const

{
    return d_attribute.print(stream, level, spacesPerLevel);
}
}  // close package namespace

} // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2007
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
