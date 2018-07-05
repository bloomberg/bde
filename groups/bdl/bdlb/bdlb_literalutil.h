// bdlb_literalutil.h                                                 -*-C++-*-
#ifndef INCLUDED_BDLB_LITERALUTIL
#define INCLUDED_BDLB_LITERALUTIL

# include <bsls_ident.h>
BSLS_IDENT("$Id$")

//@PURPOSE: Provide utility routines for programming language literals.
//
//@CLASSES:
//  bdlb::LiteralUtil: namespace for utilies for programming language literals
//
//@SEE ALSO: hslc_lexer, hslc_parser
//
//@DESCRIPTION: This component provides a namespace, 'bdlb::LiteralUtil', for a
// set of utility routines that operate on (programming language) literals.

#include <bdlscm_version.h>

#include <bsl_string.h>

namespace BloombergLP {
namespace bdlb {

                             // =================
                             // class LiteralUtil
                             // =================

struct LiteralUtil {
    // CLASS METHODS
    static void createQuotedEscapedCString(bsl::string              *result,
                                           const bslstl::StringRef&  input);
        // Load into the specified 'result' string the '"' delimited and
        // escaped C/C++ string literal equivalent representing the same value
        // as that of the specified 'input' string.  When the C string literal
        // equivalent is translated by a compiler having C-compatible string
        // literals, it will result in a string identical to the 'input'
        // string.  Note that this code uses the (ASCII) '\' character, rather
        // than Unicode code points for escapes.
};

}  // close package namespace
}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2016
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ----------------------------------
