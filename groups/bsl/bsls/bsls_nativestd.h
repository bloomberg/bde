// bsls_nativestd.h                                                   -*-C++-*-
#ifndef INCLUDED_BSLS_NATIVESTD
#define INCLUDED_BSLS_NATIVESTD

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Define the namespace `native_std` as an alias for `::std`.
//
//@DEPRECATED: Use `std` explicitly.
//
//@CLASSES:
//
//@DESCRIPTION: This component defines a namespace alias, `native_std`, that
// serves as an alias for the `std` namespace.  The `bsl` package group
// contains a Bloomberg implementation of the C++ standard library.  In the
// past, this implementation supported using the preprocessor to replace `std`
// with `bsl` and intercepting all standard library headers with BDE-provided
// implementations.  When building in this bde-stl mode by defining
// `BSL_OVERRIDES_STD`, there was still occasionally a need to refer to the
// "real" `std` namespace, and that could be accomplished with the `native_std`
// namespace.  This is no longer needed or used within BDE itself.
//
// Note: It is safe to `#include` this header only if `std` is not yet defined
// as a macro or if `BSL_DEFINED_NATIVE_STD` is already defined.  Any other
// header that defines the `native_std` namespace should also define the
// `BSL_DEFINED_NATIVE_STD` macro.  For more info on bde-stl mode, please see
// `bos/bos+stdhdrs/doc/bos+stdhdrs.txt`.
//
///Usage
///-----
// In the following example we use the `native_std` prefix to access the native
// implementation of `std::string`, even when the compilation is configured to
// override the use of native STL types with Bloomberg provided STL types
// (i.e., bde-stl mode, used by Bloomberg managed code):
// ```
// #include <bsls_nativestd.h>
// #include <string>
//
// std::string        s1;  // now always 'std::string'
// native_std::string s2;  // always was 'std::string'
// ```

#ifndef BSL_DEFINED_NATIVE_STD

# ifdef std
#   error "Cannot include bsls_nativestd.h when 'std' is a macro."
# endif

namespace std {
}
namespace native_std = ::std;

#endif  // ! defined(BSL_DEFINED_NATIVE_STD)

#endif

// ----------------------------------------------------------------------------
// Copyright 2019 Bloomberg Finance L.P.
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
