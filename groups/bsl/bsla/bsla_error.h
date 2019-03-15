// bsla_error.h                                                       -*-C++-*-
#ifndef INCLUDED_BSLA_ERROR
#define INCLUDED_BSLA_ERROR

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a macro to flag an error when a function is called.
//
//@CLASSES:
//
//@MACROS:
//  BSLA_ERROR("msg"): emit error message and fail compilation
//
//  BSLA_ERROR_IS_ACTIVE
//
//@AUTHOR: Andrew Paprocki (apaprock), Bill Chapman (bchapman2)
//
//@DESCRIPTION: This component provides a preprocessor macros that
// flags a function such that a compiler error will occur when the function is
// called.  On platforms where the appropriate attribute is not supported, the
// macro expands to nothing.
//
///Macros
///------
//..
//  BSLA_ERROR("message")
//..
// This annotation, when used, will cause a compile-time error when a call to
// the so-annotated function is not removed through dead-code elimination or
// other optimizations.  While it is possible to leave the function undefined,
// thus incurring a link-time failure, with the use of this macro the invalid
// call will be diagnosed earlier (i.e., at compile time), and the diagnostic
// will include the exact location of the function call.  The macro
// 'BSLA_ERROR_IS_ACTIVE' is defined to 0 when 'BSLA_ERROR' expands to nothing
// and 1 otherwise.
//
///Usage
///-----

#include <bsls_platform.h>

#if defined(BSLS_PLATFORM_CMP_GNU)
    // '__error__' and '__warning__' attributes are not supported by clang as
    // of version 7.0.
    #define BSLA_ERROR(x)   __attribute__((__error__(x)))

    #define BSLA_ERROR_IS_ACTIVE 1
#else
    #define BSLA_ERROR(x)

    #define BSLA_ERROR_IS_ACTIVE 0
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
