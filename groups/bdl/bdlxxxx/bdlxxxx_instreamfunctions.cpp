// bdlxxxx_instreamfunctions.cpp            -*-C++-*-
#include <bdlxxxx_instreamfunctions.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdlxxxx_instreamfunctions_cpp,"$Id$ $CSID$")

namespace BloombergLP {
namespace bdex_InStreamFunctions {

bool inStreamFunctionsDeprecatedFunctionIsInUse() {
    // This function intentionally does nothing.  It exists only to provide an
    // out-of-line symbol indicating that an object file uses the deprecated
    // 'streamInVersionAndObject' family of functions that underlie
    // 'operator<<'.

    return true;
}

}  // close namespace bdex_InStreamFunctions
}  // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2004
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
