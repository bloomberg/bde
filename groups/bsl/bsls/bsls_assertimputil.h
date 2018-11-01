// bsls_assertimputil.h                                               -*-C++-*-
#ifndef INCLUDED_BSLS_ASSERTIMPUTIL
#define INCLUDED_BSLS_ASSERTIMPUTIL

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide utilities to implement 'bsls_assert' and 'bsls_review'.
//
//@CLASSES:
//  bsls::AssertImpUtil: namespace for shared assert and review functions
//
//@AUTHOR: Joshua Berne (jberne4)
//
//@DESCRIPTION: This component defines a 'struct', 'bsls::AssertImpUtil', that
// serves as a namespace for shared functions used by the various handlers
// provided by 'bsls_assert' and 'bsls_review'.
//
///Usage
///-----
// This section illustrates the intended use of this component.
//
///Example 1: Aborting the Current Process
///- - - - - - - - - - - - - - - - - - - -
// Suppose you are implementing an assertion handler that should cause a
// process to terminate when invoked.  In order to stop the process
// immediately, you would call 'failByAbort' like this:
//..
//  void myAbort()
//  {
//      bsls::AssertImpUtil::failByAbort();
//      // This code should never be reached.
//  }
//..
// This function would then abort the current process.
//
///Example 2: Sleeping Forever
///- - - - - - - - - - - - - -
// Suppose you want a process to no longer continue doing anything, but you
// want to leave it running in order to attach a debugger to it and diagnose
// the full state of your system.  In order to have your process sleep forever,
// you might call 'failBySleep' like this:
//..
//  void mySleep()
//  {
//      bsls::AssertImpUtil::failBySleep();
//      // This code should never be reached.
//  }
//..
// This function would then sleep forever and never return.

#include <bsls_compilerfeatures.h>
#include <bsls_platform.h>

                       // ===========================
                       // BSLS_ASSERTIMPUTIL_NORETURN
                       // ===========================

// define 'BSLS_ASSERTIMPUTIL_NORETURN' for use in this header
#ifdef BSLS_ASSERTIMPUTIL_NORETURN
#error BSLS_ASSERTIMPUTIL_NORETURN must be a macro scoped locally to this \
    header file
#endif

#if defined(BSLS_COMPILERFEATURES_SUPPORT_ATTRIBUTE_NORETURN)
#   define BSLS_ASSERTIMPUTIL_NORETURN [[noreturn]]
#elif defined(BSLS_PLATFORM_CMP_MSVC)
#   define BSLS_ASSERTIMPUTIL_NORETURN __declspec(noreturn)
#else
#   define BSLS_ASSERTIMPUTIL_NORETURN
#endif

namespace BloombergLP {
namespace bsls {

                          // ====================
                          // struct AssertImpUtil
                          // ====================

struct AssertImpUtil {
    // This "implementation utility" 'struct' provides static functions with
    // shared functionality that is made use of by both 'bsls_assert' and
    // 'bsls_review'.

  public:
    // CLASS METHODS
    BSLS_ASSERTIMPUTIL_NORETURN
    static void failByAbort();
        // Unconditionally abort the current application.  It is up to the
        // caller to first output a useful message describing the location of
        // the failure.

    BSLS_ASSERTIMPUTIL_NORETURN
    static void failBySleep();
        // Spin in an infinite loop.  It is up to the caller to first output a
        // useful message describing the location of the failure.
};

}  // close package namespace
}  // close enterprise namespace

        // ========================================================
        // UNDEFINE THE LOCALLY-SCOPED IMPLEMENTATION DETAIL MACROS
        // ========================================================

#undef BSLS_ASSERTIMPUTIL_NORETURN

#endif

// ----------------------------------------------------------------------------
// Copyright 2018 Bloomberg Finance L.P.
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
