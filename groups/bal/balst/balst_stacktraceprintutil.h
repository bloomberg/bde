// balst_stacktraceprintutil.h                                        -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#ifndef INCLUDED_BALST_STACKTRACEPRINTUTIL
#define INCLUDED_BALST_STACKTRACEPRINTUTIL

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a single function to perform and print a stack trace.
//
//@CLASSES:
//  balst::StackTracePrintUtil: namespace for functions to print a stack trace
//
//@SEE_ALSO: balst_stacktraceutil
//
//@DESCRIPTION: This component defines a namespace 'struct',
// 'balst::StackTracePrintUtil', containing static platform-independent
// functions that will perform a stack trace and print it to a supplied stream.
// Not all properties of a stack trace are printed on all platforms because the
// set of properties describing a stack trace that are obtainable varies
// according to both the platform and build parameters.  For example, on
// Solaris, Linux, and HP-UX, source file names and line numbers are not
// provided.  Function names and addresses are provided on all platforms.  The
// 'printStackTrace' function always prints a description of the stack of the
// calling thread.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Printing a Stack Trace
///- - - - - - - - - - - - - - - - -
// This example shows how to obtain a stack trace and print it to a stream, all
// by calling just the static function
// 'balst::StackTracePrintUtil::printStackTrace'.
//
// First, we define a recursive function 'recurseAndPrintStack' that recurses
// to the specified 'depth', then calls
// 'balst::StackTracePrintUtil::printStackTrace' to obtain a stack trace and
// print it to 'cout'.  When we call 'printStackTrace', neither of the optional
// arguments corresponding to 'maxFrames' or 'demanglingPreferredFlag' are
// supplied; 'maxFrames' defaults to at least 1024 (which is more than we
// need), and 'demanglingPreferredFlag' defaults to 'true'.
//..
//  static
//  void recurseAndPrintStack(int *depth)
//      // Recurse to the specified 'depth', then print out the stack trace to
//      // 'cout'.
//  {
//      if (--*depth > 0) {
//          recurseAndPrintStack(depth);
//      }
//      else {
//          balst::StackTracePrintUtil::printStackTrace(cout);
//      }
//
//      ++*depth;   // Prevent compiler from optimizing tail recursion as a
//                  // loop.
//  }
//..
// Then, we call 'recurseAndPrintStack' from the main program.
//..
//  int main()
//  {
//      int depth = 5;
//      recurseAndPrintStack(&depth);
//      assert(5 == depth);
//  }
//..
// Now, invoking the main program on AIX produces the following output:
//..
//  (0): BloombergLP::balst::StackTracePrintUtil::.printStackTrace(
//       std::basic_ostream<char,std::char_traits<char> >&,int,bool)+0x170 at
//       0x1000a2c8 source:balst_stacktraceprintutil.cpp:52 in
//       balst_stacktraceprintutil.t.dbg_
//  (1): .recurseAndPrintStack(int*)+0x58 at 0x1000a118
//       source:balst_stacktraceprintutil.t.cpp:652 in
//       balst_stacktraceprintutil.t.dbg_
//  (2): .recurseAndPrintStack(int*)+0x40 at 0x1000a100
//       source:balst_stacktraceprintutil.t.cpp:650
//       in balst_stacktraceprintutil.t.dbg_
//  (3): .recurseAndPrintStack(int*)+0x40 at 0x1000a100
//       source:balst_stacktraceprintutil.t.cpp:650 in
//       balst_stacktraceprintutil.t.dbg_
//  (4): .recurseAndPrintStack(int*)+0x40 at 0x1000a100
//       source:balst_stacktraceprintutil.t.cpp:650 in
//       balst_stacktraceprintutil.t.dbg_
//  (5): .recurseAndPrintStack(int*)+0x40 at 0x1000a100
//       source:balst_stacktraceprintutil.t.cpp:650 in
//       balst_stacktraceprintutil.t.dbg_
//  (6): .main+0x2f4 at 0x10000a4c source:balst_stacktraceprintutil.t.cpp:724
//       in balst_stacktraceprintutil.t.dbg_
//  (7): .__start+0x6c at 0x1000020c source:crt0main.s in
//       balst_stacktraceprintutil.t.dbg_
//..
// Finally, we observe the following about the above output to 'cout'.  Notice
// that since the actual output would write each stack trace frame all on a
// single line, and all the lines here were longer than 80 characters, it has
// been manually edited to wrap and have every line be less than 80 columns.
// Also note the program name is truncated to 32 characters in length.

#ifndef INCLUDED_BALSCM_VERSION
#include <balscm_version.h>
#endif

#ifndef INCLUDED_BSLMA_ALLOCATOR
#include <bslma_allocator.h>
#endif

#ifndef INCLUDED_BSLMA_DEFAULTALLOCATORGUARD
#include <bslma_defaultallocatorguard.h>
#endif

#ifndef INCLUDED_BSL_IOSFWD
#include <bsl_iosfwd.h>
#endif

#ifndef INCLUDED_BSL_SSTREAM
#include <bsl_sstream.h>
#endif

#ifndef INCLUDED_BSL_STRING
#include <bsl_string.h>
#endif

namespace BloombergLP {

namespace balst {
                         // =========================
                         // class StackTracePrintUtil
                         // =========================

struct StackTracePrintUtil {
    // This 'struct' serves as a namespace for static methods that perform and
    // print a description of a stack trace.

    // CLASS METHODS
    static bsl::ostream& printStackTrace(
                                 bsl::ostream& stream,
                                 int           maxFrames = -1,
                                 bool          demanglingPreferredFlag = true);
        // Obtain a trace of the stack and print it to the specified 'stream'.
        // Optionally specify 'maxFrames' indicating the maximum number of
        // frames from the top of the stack that will be printed.  If
        // 'maxFrames' is not specified, a value of at least 1024 is used.
        // Optionally specify 'demanglingPreferredFlag', indicating a
        // preference whether to attempt to demangle function names.  If
        // 'damanglingPreferredFlag' is not specified, an attempt to demangle
        // is assumed.  If an error occurs, a single-line error message is
        // printed to 'stream'.  The behavior is undefined unless
        // '-1 <= maxFrames'.  Note that attempting to demangle symbol names
        // could involve calling 'malloc', and that symbol names are always
        // demangled on the Windows platform.
};

                       // ==============================
                       // class StackTracePrintUtil_Test
                       // ==============================

struct StackTracePrintUtil_Test {
    // This 'struct' is not intended for use by clients of this component; it
    // exists only for testing purposes.

    // CLASS METHODS
    static void printStackTraceToString(bsl::string *string);
        // Obtain a stack trace and assign a description of the stack to the
        // specified 'string'.  Note that this method is for testing only, and
        // must be declared inline in an include file in order to test source
        // file name resolution of a routine in an include file.
};

// ============================================================================
//                        INLINE FUNCTION DEFINITIONS
// ============================================================================

                      // -------------------------------
                      // struct StackTracePrintUtil_Test
                      // -------------------------------

// CLASS METHOD
inline
void StackTracePrintUtil_Test::printStackTraceToString(bsl::string *string)
{
    bslma::Allocator *a = string->get_allocator().mechanism();
    bslma::DefaultAllocatorGuard guard(a);

    bsl::ostringstream os;
    StackTracePrintUtil::printStackTrace(os);
    *string = os.str();
}

}  // close package namespace
}  // close enterprise namespace

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
