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

namespace bsl
{
    // Import selected symbols into bsl namespace
    using native_std::domain_error;
    using native_std::invalid_argument;
    using native_std::length_error;
    using native_std::logic_error;
    using native_std::out_of_range;
    using native_std::overflow_error;
    using native_std::range_error;
    using native_std::runtime_error;
    using native_std::underflow_error;

    // Make std::exception available even though it comes from <exception>
    // because, as a base class of the above, it may be assumed complete.
    using native_std::exception;

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
