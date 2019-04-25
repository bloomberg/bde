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
//@DESCRIPTION: This component defines a 'struct', 'bsls::AssertImpUtil', that
// serves as a namespace for shared functions used by the various handlers
// provided by 'bsls_assert' and 'bsls_review'.
//
///'BSLS_ASSERTIMPUTIL_AVOID_STRING_CONSTANTS'
///-------------------------------------------
// On some platforms the string constants used to pass filenames to the assert
// macro invocations are not coalesced, so each inline function use containing
// such a macro puts an extra copy of the filename string into the resulting
// executable.  For these platforms, it is possible to locally alter the
// filename that assert macros will use by altering the definition of
// 'BSLS_ASSERTIMPUTIL_FILE'.
//
// At the start of your component header, after all other include directives,
// place the following block of code to detect if this is a platform where the
// workaround is needed and apply it:
//..
//  // my_component.h
//  // ...
//  // 'BSLS_ASSERT' filename fix -- See {bsls_assertimputil}
//  #ifdef BSLS_ASSERTIMPUTIL_AVOID_STRING_CONSTANTS
//  extern const char s_my_component_h[];
//  #undef BSLS_ASSERTIMPUTIL_FILE
//  #define BSLS_ASSERTIMPUTIL_FILE BloombergLP::s_my_component_h
//  #endif
//..
// Then, at the end of your header revert the definition of the filename macro
// to its default:
//..
//  // Undo 'BSLS_ASSERT' filename fix -- See {bsls_assertimputil}
//  #ifdef BSLS_ASSERTIMPUTIL_AVOID_STRING_CONSTANTS
//  #undef BSLS_ASSERTIMPUTIL_FILE
//  #define BSLS_ASSERTIMPUTIL_FILE BSLS_ASSERTIMPUTIL_DEFAULTFILE
//  #endif
//..
// Finally, in the '.cpp' file add the following:
//..
//  // 'BSLS_ASSERT' filename fix -- See {bsls_assertimputil}
//  #ifdef BSLS_ASSERTIMPUTIL_AVOID_STRING_CONSTANTS
//  extern const char s_my_component_h[] = "my_component.h";
//  #endif
//..
// Note that these constants should all be in an appropriate namespace and
// should have names and contents that match your actual component name.
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

#include <bsls_annotation.h>
#include <bsls_compilerfeatures.h>
#include <bsls_platform.h>

                 // =========================================
                 // BSLS_ASSERTIMPUTIL_AVOID_STRING_CONSTANTS
                 // =========================================

#ifdef BSLS_PLATFORM_CMP_SUN
// On sun, string constants are not coalesced, so uses of 'BSLS_ASSERT' and
// 'BSLS_REVIEW' macros in inlined functions lead to massive executable size
// bloat.  This flag indicates that a workaround should be enabled to minimize
// references to strings literals in these macros.  That workaround should
// involve redefining 'BSLS_ASSERTIMPUTIL_FILE' to reference a static constant
// that is initialized in the component's implementation file.

#define BSLS_ASSERTIMPUTIL_AVOID_STRING_CONSTANTS

#endif


namespace BloombergLP {
namespace bsls {

                        // ===========================
                        // Assert Macro Support Macros
                        // ===========================

// Assertion Filename
#define BSLS_ASSERTIMPUTIL_DEFAULTFILE __FILE__
#define BSLS_ASSERTIMPUTIL_FILE BSLS_ASSERTIMPUTIL_DEFAULTFILE

#define BSLS_ASSERTIMPUTIL_LINE __LINE__

#ifdef BSLS_PLATFORM_CMP_SUN
// On sun, if possible headers that use 'bsls_review' or 'bsls_assert' macros
// in inline functions should attempt to specify their filenames as an extern
// string defined in just the '.cpp' file of the component instead of just
// using '__FILE__'.

#define BSLS_ASSERTIMPUTIL_AVOID_STRING_CONSTANTS

#endif

                            // ====================
                            // struct AssertImpUtil
                            // ====================

struct AssertImpUtil {
    // This "implementation utility" 'struct' provides static functions with
    // shared functionality that is made use of by both 'bsls_assert' and
    // 'bsls_review'.

  public:
    // CLASS METHODS
    BSLS_ANNOTATION_NORETURN
    static void failByAbort();
        // Unconditionally abort the current application.  It is up to the
        // caller to first output a useful message describing the location of
        // the failure.

    BSLS_ANNOTATION_NORETURN
    static void failBySleep();
        // Spin in an infinite loop.  It is up to the caller to first output a
        // useful message describing the location of the failure.
};

}  // close package namespace
}  // close enterprise namespace

          // ========================================================
          // UNDEFINE THE LOCALLY-SCOPED IMPLEMENTATION DETAIL MACROS
          // ========================================================

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
