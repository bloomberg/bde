// bsla_nodiscard.h                                                   -*-C++-*-
#ifndef INCLUDED_BSLA_NODISCARD
#define INCLUDED_BSLA_NODISCARD

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide support for compiler annotations for compile-time safety.
//
//@CLASSES:
//
//@MACROS:
//  BSLA_NODISCARD: warn if annotated function result is not used
//
//  BSLA_NODISCARD_IS_ACTIVE
//
//@AUTHOR: Andrew Paprocki (apaprock), Bill Chapman (bchapman2)
//
//@DESCRIPTION: This component provides a preprocessor macros that
// annotates a function such that a compiler warning will be generated if the
// return value of the function is ignored.
//
// The macro 'BSLA_NODISCARD_IS_ACTIVE' is defined to 0 if 'BSLA_NODISCARD'
// expands to nothing and 1 otherwise.
//
///Macro
///-----
//..
//  BSLA_NODISCARD
//..
// This annotation causes a warning to be emitted if the caller of a
// so-annotated function does not use its return value.  This is useful for
// functions where not checking the result is either a security problem or
// always a bug, such as with the 'realloc' function.
//
///Usage
///-----

#include <bsls_platform.h>
#include <bsls_compilerfeatures.h>

#if defined(BSLS_COMPILERFEATURES_SUPPORT_ATTRIBUTE_NODISCARD)
    #define BSLA_NODISCARD [[ nodiscard ]]

    #define BSLA_NODISCARD_IS_ACTIVE 1
#elif defined(BSLS_PLATFORM_CMP_GNU) || defined(BSLS_PLATFORM_CMP_CLANG)
    #define BSLA_NODISCARD __attribute__((warn_unused_result))

    #define BSLA_NODISCARD_IS_ACTIVE 1
#else
    #define BSLA_NODISCARD

    #define BSLA_NODISCARD_IS_ACTIVE 0
#endif

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
