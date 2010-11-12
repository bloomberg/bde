// bael_predicate.cpp                 -*-C++-*-
#include <bael_predicate.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(bael_predicate_cpp,"$Id$ $CSID$")

namespace BloombergLP {

// ACCESSORS
bsl::ostream& bael_Predicate::print(bsl::ostream& stream,
                                    int           level ,
                                    int           spacesPerLevel) const

{
    return d_attribute.print(stream, level, spacesPerLevel);
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
