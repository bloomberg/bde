// bsl_ctgmath.h                                                      -*-C++-*-
#ifndef INCLUDED_BSL_CTGMATH
#define INCLUDED_BSL_CTGMATH

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide functionality of the corresponding C++ Standard header.
//
//@DESCRIPTION: Provide types, in the `bsl` namespace, equivalent to those
// defined in the corresponding C++ standard header.  Include the native
// compiler-provided standard header, and also directly include Bloomberg's
// implementation of the C++ standard type (if one exists).  Finally, place the
// included symbols from the `std` namespace (if any) into the `bsl` namespace.

// Following usual practice, the `<bsl_ctgmath.h>` header would include
// `<ctgmath>`.  However, that header is not available in C++03 mode, and the
// only effect of including it in C++11 and later is to include the
// `<ccomplex>` and `<cmath>` headers, so we include the corresponding bsl
// headers, intentionally making their contents available transitively.
#include <bsl_cmath.h>
#include <bsl_complex.h>

#ifndef BDE_DONT_ALLOW_TRANSITIVE_INCLUDES
#include <bsls_nativestd.h>
#endif // BDE_DONT_ALLOW_TRANSITIVE_INCLUDES

#endif

// ----------------------------------------------------------------------------
// Copyright 2015 Bloomberg Finance L.P.
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
