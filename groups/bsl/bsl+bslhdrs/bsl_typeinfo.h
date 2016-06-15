// bsl_typeinfo.h                                                     -*-C++-*-
#ifndef INCLUDED_BSL_TYPEINFO
#define INCLUDED_BSL_TYPEINFO

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

#include <typeinfo>

#ifndef BDE_OMIT_INTERNAL_DEPRECATED
// We have traditionally imported names from <exception> under the assumption
// that <typeinfo> must include <exception> because <typeinfo> defines classes
// derived from std::exception.  But we have encountered runtimes where this is
// not so, nor is this required by the standard, so we explicitly include
// <exception> if we are going to import its names.
#include <exception>
#endif

namespace bsl
{
    // Import selected symbols into bsl namespace
    using native_std::bad_cast;
    using native_std::bad_typeid;
    using native_std::type_info;

#ifndef BDE_OMIT_INTERNAL_DEPRECATED
    // Import additional names expected by existing code, but not mandated by
    // the standard header.
    using native_std::bad_exception;
    using native_std::exception;
    using native_std::set_terminate;
    using native_std::set_unexpected;
    using native_std::terminate;
    using native_std::terminate_handler;
    using native_std::uncaught_exception;
    using native_std::unexpected;
    using native_std::unexpected_handler;
#endif  // BDE_OMIT_INTERNAL_DEPRECATED
}  // close package namespace

#endif

// ----------------------------------------------------------------------------
// Copyright 2013 Bloomberg Finance L.P.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// ----------------------------- END-OF-FILE ----------------------------------
