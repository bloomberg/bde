// bsla_noreturn.h                                                    -*-C++-*-
#ifndef INCLUDED_BSLA_NORETURN
#define INCLUDED_BSLA_NORETURN

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide support for compiler annotations for compile-time safety.
//
//@CLASSES:
//
//@MACROS:
//  BSLA_NORETURN: error if function returns normally
//
//  BSLA_NORETURN_IS_ACTIVE
//
//@AUTHOR: Andrew Paprocki (apaprock), Bill Chapman (bchapman2)
//
//@DESCRIPTION: This component provides a preprocessor macro that
// annotates a function as never returning, resulting in a compiler warning
// if a path of control exists such that the function does return.
//
// The macro 'BSLA_RETURN_IS_ACTIVE' is defined to 0 if 'BSLA_NORETURN' expands
// to nothing and 1 otherwise.
//
///Macro
///-----
//..
//  BSLA_NORETURN
//..
// This annotation is used to tell the compiler that a specified function will
// not return in a normal fashion.  The function can still exit via other means
// such as throwing an exception or aborting the process.
//
///Usage
///-----

#include <bsls_platform.h>
#include <bsls_compilerfeatures.h>

#if defined(BSLS_COMPILERFEATURES_SUPPORT_ATTRIBUTE_NORETURN)
    #define BSLA_NORETURN [[ noreturn ]]

    #define BSLA_NORETURN_IS_ACTIVE 1
#elif defined(BSLS_PLATFORM_CMP_MSVC)
    #define BSLA_NORETURN __declspec(noreturn)

    #define BSLA_NORETURN_IS_ACTIVE 1
#else
    #define BSLA_NORETURN

    #define BSLA_NORETURN_IS_ACTIVE 0
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
