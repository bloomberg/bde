// bsla_fallthrough.h                                                 -*-C++-*-
#ifndef INCLUDED_BSLA_FALLTHROUGH
#define INCLUDED_BSLA_FALLTHROUGH

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide macro to suppress warnings on 'switch' fall throughs.
//
//@CLASSES:
//
//@MACROS:
//  BSLA_FALLTHROUGH: do not warn if case fall through
//
//  BSLA_FALLTHROUGH_IS_ACTIVE
//
//@AUTHOR: Andrew Paprocki (apaprock), Bill Chapman (bchapman2)
//
//@DESCRIPTION: This component provides a preprocessor macro that suppresses
// compiler warnings about flow of control fall through from one 'case' or
// 'default' of a 'switch' statement to another.  On compilers where the
// appropriate attribute is not supported, the macro expands to nothing.
//
// The macro 'BSLA_FALLTHROUGH_IS_ACTIVE' expands to 0 when 'BSLA_FALLTHROUGH'
// expands to nothing and 1 otherwise.
//
///Macro
///-----
//..
//  BSLA_FALLTHROUGH
//..
// This annotation should be placed as a the statement before a 'case' in a
// 'switch' statement that is expceted to allow control to fall through instead
// of ending with a 'break', 'continue', or 'return'.  This will prevent
// compilers from warning about fallthrough.
//
///Usage
///-----

#include <bsls_platform.h>
#include <bsls_compilerfeatures.h>

#ifdef BSLA_FALLTHROUGH
#error BSLA_FALLTHROUGH previously #defined
#endif
#if defined(BSLS_COMPILERFEATURES_SUPPORT_ATTRIBUTE_FALLTHROUGH)
    #define BSLA_FALLTHROUGH [[ fallthrough ]]
#elif defined(BSLS_PLATFORM_CMP_GNU) && BSLS_PLATFORM_CMP_VERSION >= 70000
    #define BSLA_FALLTHROUGH __attribute__((fallthrough))
#elif defined(BSLS_PLATFORM_CMP_CLANG)
    #if __cplusplus >= 201103L && defined(__has_warning)
        #if  __has_feature(cxx_attributes) && \
             __has_warning("-Wimplicit-fallthrough")
            #define BSLA_FALLTHROUGH [[clang::fallthrough]]
        #endif
    #endif
#endif
#ifdef BSLA_FALLTHROUGH
    #define BSLA_FALLTHROUGH_IS_ACTIVE 1
#else
    #define BSLA_FALLTHROUGH

    #define BSLA_FALLTHROUGH_IS_ACTIVE 0
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
