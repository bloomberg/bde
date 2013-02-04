// bsl_stdexcept.h                                                    -*-C++-*-
#ifndef INCLUDED_BSL_STDEXCEPT
#define INCLUDED_BSL_STDEXCEPT

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

#ifndef INCLUDED_BSLS_NATIVESTD
#include <bsls_nativestd.h>
#endif

#ifndef INCLUDED_BSLS_PLATFORM
#include <bsls_platform.h>
#endif

#include <stdexcept>

namespace bsl {

    // Import selected symbols into bsl namespace
    using native_std::terminate_handler;
    using native_std::unexpected_handler;
    using native_std::bad_exception;
    using native_std::domain_error;
    using native_std::exception;
    using native_std::invalid_argument;
    using native_std::length_error;
    using native_std::logic_error;
    using native_std::out_of_range;
    using native_std::overflow_error;
    using native_std::range_error;
    using native_std::runtime_error;
    using native_std::set_terminate;
    using native_std::set_unexpected;
    using native_std::terminate;
    using native_std::uncaught_exception;
    using native_std::underflow_error;
    using native_std::unexpected;

}  // close namespace bsl

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2009
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
