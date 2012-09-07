// bsl_limits.h                                                       -*-C++-*-
#ifndef INCLUDED_BSL_LIMITS
#define INCLUDED_BSL_LIMITS

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide functionality of the corresponding C++ Standard header.
//
//@SEE_ALSO: package bsl+stdhdrs
//
//@AUTHOR: Arthur Chiu (achiu21)
//
//@DESCRIPTION: Provide types, in the 'bsl' namespace, equivalent to those
// defined in the corresponding C++ standard header.  Include the native
// compiler-provided standard header, and also directly include Bloomberg's
// implementation of the C++ standard type (if one exists).  Finally, place the
// included symbols from the 'std' namespace (if any) into the 'bsl' namespace.

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLS_NATIVESTD
#include <bsls_nativestd.h>
#endif

#include <limits>

namespace bsl
{
    // Import selected symbols into bsl namespace
    using native_std::numeric_limits;
    using native_std::float_round_style;
    using native_std::float_denorm_style;
    using native_std::round_indeterminate;
    using native_std::round_toward_zero;
    using native_std::round_to_nearest;
    using native_std::round_toward_infinity;
    using native_std::round_toward_neg_infinity;
    using native_std::denorm_indeterminate;
    using native_std::denorm_absent;
    using native_std::denorm_present;
}

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2009
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
