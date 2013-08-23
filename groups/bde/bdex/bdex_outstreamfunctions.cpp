// bdex_outstreamfunctions.cpp           -*-C++-*-
#include <bdex_outstreamfunctions.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(bdex_outstreamfunctions_cpp,"$Id$ $CSID$")

namespace BloombergLP {
namespace bdex_OutStreamFunctions {

bool outStreamFunctionsDeprecatedFunctionIsInUse() {
    // This function intentionally does nothing.  It exists only to provide an
    // out-of-line symbol indicating that an object file uses the deprecated
    // 'streamOutVersionAndObject' family of functions that underlie
    // 'operator<<'.

    return true;
}

}  // close namespace bdex_OutStreamFunctions
}  // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2004
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
