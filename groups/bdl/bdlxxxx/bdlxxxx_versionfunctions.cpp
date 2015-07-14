// bdlxxxx_versionfunctions.cpp                  -*-C++-*-
#include <bdlxxxx_versionfunctions.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdlxxxx_versionfunctions_cpp,"$Id$ $CSID$")


namespace BloombergLP {
namespace bdex_VersionFunctions {

bool versionFunctionsDeprecatedFunctionIsInUse() {
    // This function intentionally does nothing.  It exists only to provide an
    // out-of-line symbol indicating that an object file uses
    // 'bdex_VersionFunctions::maxSupportedVersion'.

    return true;
}

}  // close namespace bdex_VersionFunctions
}  // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2005
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
