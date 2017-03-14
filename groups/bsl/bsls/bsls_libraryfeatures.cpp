// bsls_libraryfeatures.cpp                                           -*-C++-*-
#include <bsls_libraryfeatures.h>

#include <bsls_ident.h>
BSLS_IDENT("$Id$ $CSID$")

#include <bsls_compilerfeatures.h>

#include <bsls_bsltestutil.h>  // for testing only
#include <bsls_cpp11.h>        // for testing only
#include <bsls_nativestd.h>    // for testing only

///Implementation Notes
///--------------------
// Since the tool chain does not provide explicit information about library
// features, this component infers that information heuristically from the
// values of various compiler flags and flags set by the 'bsls_platform' and
// 'bsls_compilerfeature' components.  The inference rules were determined
// empirically: the test driver program was tested on supported platforms for a
// variety of compiler versions (and, with 'gcc', with and without the '-std'
// compiler option).  The flag for each feature (e.g.,
// 'BSLS_LIBRARYFEATURES_HAS_CPP11_UNORDERED_MAP') was set experimentally and
// if the test driver could be built and demonstrate (in verbose mode) that the
// feature had been used, that combination inputs was deemed successful.
//
// Limitations:
//
//: o There can be false negatives for clients using platform and compiler
//:   configurations that were not explicitly tested.
//:
//: o There can be false positives for clients using build configurations that
//:   match BDE standard configurations in platform/compiler/version but differ
//:   in some invisible but significant detail (e.g., the version of the
//:   standard library installed in a custom environment).
//:
//: o The actual range of compiler versions supporting a given feature may
//:   differ from the range informed by these macros.  The earliest version is
//:   the earliest observed among those tested.  No effort was made to discover
//:   the historically earliest version.  Also, once a feature is observed in a
//:   compiler, all future versions are assumed to provide that feature.

                        // Enforce invariants

#if defined(BSLS_LIBRARYFEATURES_HAS_C99_FULL_LIBRARY)

    #ifndef BSLS_LIBRARYFEATURES_HAS_C99_BASELINE_LIBRARY
    #error "'BSLS_LIBRARYFEATURES_HAS_C99_FULL_LIBRARY' requires \
            'BSLS_LIBRARYFEATURES_HAS_C99_BASELINE_LIBRARY'"
    #endif

#endif

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP98_AUTO_PTR)
#else
    #error "'BSLS_LIBRARYFEATURES_HAS_CPP98_AUTO_PTR' should be defined for \
            all libraries/platforms until the introduction of C++17."

#endif

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP11_EXCEPTION_HANDLING)

    #ifndef BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY
    #error "'BSLS_LIBRARYFEATURES_HAS_CPP11_EXCEPTION_HANDLING' requires \
            'BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY'"
    #endif

#endif

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP11_GARBAGE_COLLECTION_API)

    #ifndef BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY
    #error "'BSLS_LIBRARYFEATURES_HAS_CPP11_GARBAGE_COLLECTION_API' requires \
            'BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY'"
    #endif

#endif

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP11_MISCELLANEOUS_UTILITIES)

    #ifndef BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY
    #error "'BSLS_LIBRARYFEATURES_HAS_CPP11_MISCELLANEOUS_UTILITIES' requires \
            'BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY'"
    #endif

#endif

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP11_PAIR_PIECEWISE_CONSTRUCTOR)

    #ifndef BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY
    #error "'BSLS_LIBRARYFEATURES_HAS_CPP11_PAIR_PIECEWISE_CONSTRUCTOR' \
             requires 'BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY'"
    #endif

    #ifndef BSLS_LIBRARYFEATURES_HAS_CPP11_TUPLE
    #error "'BSLS_LIBRARYFEATURES_HAS_CPP11_PAIR_PIECEWISE_CONSTRUCTOR' \
            requires 'BSLS_LIBRARYFEATURES_HAS_CPP11_TUPLE'"
    #endif

    #ifndef BSLS_COMPILERFEATURES_SUPPORT_VARIADIC_TEMPLATES
    #error "'BSLS_LIBRARYFEATURES_HAS_CPP11_PAIR_PIECEWISE_CONSTRUCTOR' \
            requires 'BSLS_COMPILERFEATURES_SUPPORT_VARIADIC_TEMPLATES'"
    #endif

#endif

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP17_PRECISE_BITWIDTH_ATOMICS)

    #ifndef BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY
    #error "'BSLS_LIBRARYFEATURES_HAS_CPP17_PRECISE_BITWIDTH_ATOMICS' \
            requires 'BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY'"
    #endif

#endif

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP11_PROGRAM_TERMINATION)

    #ifndef BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY
    #error "'BSLS_LIBRARYFEATURES_HAS_CPP11_PROGRAM_TERMINATION' requires \
            'BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY'"
    #endif

#endif

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP11_TUPLE)

    #ifndef BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY
    #error "'BSLS_LIBRARYFEATURES_HAS_CPP11_TUPLE' requires \
            'BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY'"
    #endif

    #ifndef BSLS_COMPILERFEATURES_SUPPORT_VARIADIC_TEMPLATES
    #error "'BSLS_LIBRARYFEATURES_HAS_CPP11_TUPLE' requires \
            'BSLS_COMPILERFEATURES_SUPPORT_VARIADIC_TEMPLATES'"
    #endif

#endif

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP11_UNIQUE_PTR)

    #ifndef BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY
    #error "'BSLS_LIBRARYFEATURES_HAS_CPP11_UNIQUE_PTR' requires \
            'BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY'"
    #endif

    #ifndef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
    #error "'BSLS_LIBRARYFEATURES_HAS_CPP11_UNIQUE_PTR' requires \
            'BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES'"
    #endif

#endif

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP17_BOOL_CONSTANT)
#error "See 'BSLS_LIBRARYFEATURES_HAS_CPP17_BOOL_CONSTANT': !NOT DEFINED! \
        in component-level documentation."
#endif

// ----------------------------------------------------------------------------
// Copyright 2017 Bloomberg Finance L.P.
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
