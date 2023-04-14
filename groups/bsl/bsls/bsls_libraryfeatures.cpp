// bsls_libraryfeatures.cpp                                           -*-C++-*-
#include <bsls_libraryfeatures.h>

#include <bsls_ident.h>
BSLS_IDENT("$Id$ $CSID$")

#include <bsls_compilerfeatures.h>

#include <bsls_bsltestutil.h>   // for testing only
#include <bsls_buildtarget.h>   // for testing only
#include <bsls_keyword.h>       // for testing only

///Implementation Notes
///--------------------
// Since the tool chain does not provide explicit information about library
// features, this component infers that information heuristically from the
// values of various compiler flags and flags set by the 'bsls_platform' and
// 'bsls_compilerfeatures' components.  The inference rules were determined
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


         // Standard library implementation detection verification

// This code attempts to verify that we have detected the right standard
// library implementation by including a non-standard header that is
// (hopefully) unique to and part of that implementation we have detected.  To
// be paranoid, the code also verifies a macro that is defined by the known
// implementations.  The headers have been selected to be not only
// representative of the implementation, but also such that existed throughout
// many many versions of the given implementation and so we can assume they
// will exists for the foreseeable future.

#if defined(BSLS_LIBRARYFEATURES_STDCPP_GNU)
#include <bits/os_defines.h>
#ifndef _GLIBCXX_OS_DEFINES
#error <bits/os_defines.h> does not seem to #define _GLIBCXX_OS_DEFINES.
#endif

#elif defined(BSLS_LIBRARYFEATURES_STDCPP_MSVC)
#if BSLS_PLATFORM_CMP_VERSION < 1900
#include <vadefs.h>
#ifndef _INC_VADEFS
#error <vadefs.h> does not seem to #define _INC_VADEFS.
#endif
#else
#include <vcruntime.h>
#ifndef _UCRT
#error <vcruntime.h> does not seem to #define _UCRT.
#endif
#endif

#elif defined(BSLS_LIBRARYFEATURES_STDCPP_LLVM)
#include <__config>
#ifndef _LIBCPP_VERSION
#error <__config> does not seem to #define _LIBCPP_VERSION.
#endif

#elif defined(BSLS_LIBRARYFEATURES_STDCPP_LIBCSTD)
#include <rw/rwstderr_macros.h>
#ifndef _RWSTD_ERROR_MSG_FILE
#error <rw/rwstderr_macros.h> does not seem to #define _RWSTD_ERROR_MSG_FILE.
#endif

#elif defined(BSLS_LIBRARYFEATURES_STDCPP_STLPORT)
#include <config/stlcomp.h>
#ifndef _STLP_COMP_H
#error <config/stlcomp.h> does not seem to #define _STLP_COMP_H.
#endif

#elif defined(BSLS_LIBRARYFEATURES_STDCPP_IBM)
#include <irtllock.h>
#ifndef THREAD_SAFE_GLOBAL_LOCK
#error <irtllock.h> does not seem to #define THREAD_SAFE_GLOBAL_LOCK.
#endif

#elif defined(BSLS_LIBRARYFEATURES_STDCPP_INTELLISENSE)
// We do not attempt to check anything here, because in Intellisense passes we
// have no idea what the target compiler or the target standard library
// implementation is.  We only have this case (#elif) to ensure that people
// will not see following #error in their IDEs while looking at their code.
#else
#error Unexpected standard library implementation.  Please update test driver.
#endif


                        // Enforce invariants

#if defined(BSLS_LIBRARYFEATURES_HAS_C99_FULL_LIBRARY)

    #ifndef BSLS_LIBRARYFEATURES_HAS_C99_BASELINE_LIBRARY
    #error "'BSLS_LIBRARYFEATURES_HAS_C99_FULL_LIBRARY' requires \
            'BSLS_LIBRARYFEATURES_HAS_C99_BASELINE_LIBRARY'"
    #endif

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

namespace BloombergLP {
#define STRINGIFY2(a) #a
#define STRINGIFY(a) STRINGIFY2(a)

const char *BSLS_LIBRARYFEATURES_LINKER_CHECK_NAME =
                             "BSLS_LIBRARYFEATURES_ABI_CHECK: "
                             STRINGIFY(BSLS_LIBRARYFEATURES_LINKER_CHECK_NAME);
}  // close enterprise namespace

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
