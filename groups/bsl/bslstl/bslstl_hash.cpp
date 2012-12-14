// bslstl_hash.cpp                                                    -*-C++-*-
#include <bslstl_hash.h>

#include <bsls_ident.h>
BSLS_IDENT("$Id$ $CSID$")

#ifndef BDE_OMIT_INTERNAL_DEPRECATED // DEPRECATED

#include <stdio.h>  // for 'printf'

#endif  // BDE_OMIT_INTERNAL_DEPRECATED

namespace bsl
{

#ifndef BDE_OMIT_INTERNAL_DEPRECATED  // DEPRECATED
std::size_t hash<const char *>::operator()(const char *x) const
{
    static bool firstCall = true;
    if (firstCall) {
        firstCall = false;

        printf("ERROR: bsl::hash called for 'const char *' strings\n");
    }

    unsigned long result = 0;

    for (; *x; ++x) {
        result = 5 * result + *x;
    }

    return std::size_t(result);
}
#endif  // BDE_OMIT_INTERNAL_DEPRECATED

}  // close namespace bsl
// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2012
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
