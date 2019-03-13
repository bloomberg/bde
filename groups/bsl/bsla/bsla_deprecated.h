// bsla_deprecated.h                                                  -*-C++-*-
#ifndef INCLUDED_BSLA_DEPRECATED
#define INCLUDED_BSLA_DEPRECATED

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide macro to hint to compiler that an entity is deprecated.
//
//@CLASSES:
//
//@MACROS:
//  BSLA_DEPRECATED: warn if annotated entity is used
//
//  BSLA_DEPRECATED_IS_ACTIVE
//
//@AUTHOR: Andrew Paprocki (apaprock), Bill Chapman (bchapman2)
//
//@DESCRIPTION: This component provides a preprocessor macros that
// hints to the compile that a function, variable, or type is deprecated.
// The 'BSLA_DEPRECATED_IS_ACTIVE' macro is defined to 0 if 'BSLA_DEPRECATED'
// expands to nothing and 1 otherwise.
//
///Macro
///-----------
//..
//  BSLA_DEPRECATED
//..
// This annotation will, when used, cause a compile-time warning if the
// so-annotated function, variable, or type is used anywhere within the source
// file.  This is useful, for example, when identifying functions that are
// expected to be removed in a future version of a library.  The warning
// includes the location of the declaration of the deprecated entity to enable
// users to find further information about the deprecation, or what they should
// use instead.  For example:
//..
//  int oldFnc() BSLA_DEPRECATED;
//  int oldFnc();
//  int (*fncPtr)() = oldFnc;
//..
// In the above code, the third line results in a compiler warning.
//
///Usage
///-----

#include <bsls_platform.h>

#if defined(BSLS_PLATFORM_CMP_GNU) || defined(BSLS_PLATFORM_CMP_CLANG)
    #define BSLA_DEPRECATED __attribute__((__deprecated__))

    #define BSLA_DEPRECATED_IS_ACTIVE 1
#else
    #define BSLA_DEPRECATED

    #define BSLA_DEPRECATED_IS_ACTIVE 0
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
