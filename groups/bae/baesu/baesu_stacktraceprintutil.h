// baesu_stacktraceprintutil.h                                        -*-C++-*-
#ifndef INCLUDED_BAESU_STACKTRACEPRINTUTIL
#define INCLUDED_BAESU_STACKTRACEPRINTUTIL

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide portable utilities for printing a stack trace.
//
//@CLASSES:
//  baesu_StackTracePrintUtil: namespace for functions that print a stack trace
//
//@AUTHOR: Oleg Semenov, Bill Chapman
//
//@SEE ALSO: baesu_stacktraceutil
//
//@DESCRIPTION: This component defines a namespace class containing static
// platform-independent functions that will print a stack trace to a supplied
// stream.  Not all properties of a stack trace are printed on all platforms
// because the set of properties describing a stack trace that are obtainable
// varies according to the platform and build parameters.  For example, on
// Solaris, Linux, and HP-UX, source file names and line numbers are not
// provided.  Function names and addresses are provided on all platforms.  The
// 'printStackTrace' function always prints a description of the stack of the
// calling thread.
//
///Usage Example
///-------------
// The following examples illustrate how to print a stack trace.
//
// This example shows how to print a stack trace to a stream, by calling the
// static function 'baesu_StackTraceUtil::printStackTrace'.
//..
//  static
//  void recurseAndPrintStack(int *depth)
//      // First, recurse to the specified 'depth', then print out the stack
//      // trace to 'cout'.
//  {
//      if (--*depth > 0) {
//          recurseAndPrintStack(depth);
//      }
//      else {
//..
// Call 'printStackTrace' to print out a stack trace.  In this case, the
// 'maxFrames' argument is unspecified, defaulting to 1000 (which is more than
// we need) and the 'demanglingPreferredFlag' argument is unspecified,
// defaulting to 'true'.
//..
//          baesu_StackTracePrintUtil::printStackTrace(cout);
//      }
//
//      ++*depth;   // Prevent compiler from optimizing tail recursion as a
//                  // loop.
//  }
//
//  int main()
//  {
//      int depth = 5;
//      recurseAndPrintUsage(&depth);
//      assert(5 == depth);
//  }
//..
// The following output is produced by this example on AIX (output for each
// frame is single line -- note that the lines were longer than 80 chars, so
// continuation is wrapped), and that the program name is truncated to 32
// characters.
//..
//  (0): BloombergLP::baesu_StackTracePrintUtil::.printStackTrace(
//                 std::basic_ostream<char,std::char_traits<char> >&,
//                 int,bool)+0x140 at 0x10009d68
//                 source:baesu_stacktraceprintutil.cpp:51 in
//                 baesu_stacktraceprintutil.t.dbg_
//  (1): .recurseAndPrintExample1(int*)+0x58 at 0x10009be8
//                 source:baesu_stacktraceprintutil.t.cpp:630 in
//                 baesu_stacktraceprintutil.t.dbg_
//  (2): .recurseAndPrintExample1(int*)+0x40 at 0x10009bd0
//                 source:baesu_stacktraceprintutil.t.cpp:623 in
//                 baesu_stacktraceprintutil.t.dbg_
//  (3): .recurseAndPrintExample1(int*)+0x40 at 0x10009bd0
//                 source:baesu_stacktraceprintutil.t.cpp:623 in
//                 baesu_stacktraceprintutil.t.dbg_
//  (4): .recurseAndPrintExample1(int*)+0x40 at 0x10009bd0
//                 source:baesu_stacktraceprintutil.t.cpp:623 in
//                 baesu_stacktraceprintutil.t.dbg_
//  (5): .recurseAndPrintExample1(int*)+0x40 at 0x10009bd0
//                 source:baesu_stacktraceprintutil.t.cpp:623 in
//                 baesu_stacktraceprintutil.t.dbg_
//  (6): .main+0x2d4 at 0x10000a2c
//                 source:baesu_stacktraceprintutil.t.cpp:687 in
//                 baesu_stacktraceprintutil.t.dbg_
//  (7): .__start+0x6c at 0x1000020c
//                 source:crt0main.s in
//                 baesu_stacktraceprintutil.t.dbg_
//..

#ifndef INCLUDED_BAESCM_VERSION
#include <baescm_version.h>
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

                     // ===============================
                     // class baesu_StackTracePrintUtil
                     // ===============================

struct baesu_StackTracePrintUtil {
    // This 'struct' serves as a namespace for static methods that print a
    // description of a stack trace.

    // CLASS METHODS
    static bsl::ostream& printStackTrace(
                                 bsl::ostream& stream,
                                 int           maxFrames = -1,
                                 bool          demanglingPreferredFlag = true);
        // Obtain a trace of the stack and print it to the specified 'stream'.
        // Optionally specify 'maxFrames' indicating the maximum number of
        // frames from the top of the stack that will be printed.  If
        // 'maxFrames' is not specified, a value of at least 1024 is used.
        // Optionally specify 'demanglingPreferredFlag', indicating whether to
        // attempt to demangle function names.  If an error occurs, print a
        // 1-line error message to 'stream'.  The behavior is undefined unless
        // 'maxFrames >= 0'.  Note that attempting to demangle symbol names
        // could involve calling 'malloc', and symbol names are always
        // demangled on the Windows platform.
};

                   // ====================================
                   // class baesu_StackTracePrintUtil_Test
                   // ====================================

struct baesu_StackTracePrintUtil_Test {
    // This 'struct' is not for use by clients of this component, it just
    // exists for testing within this component.

    // CLASS METHODS
    static void printStackTraceToString(bsl::string *string);
        // Obtain a stack trace and assign a description of the stack to the
        // specified 'string'.  Note that this is for testing only, and it must
        // be inline in an include file to test source file name resolution of
        // a routine in an include file.
};

// ===========================================================================
//                       INLINE FUNCTION DEFINITIONS
// ===========================================================================

                    // -------------------------------------
                    // struct baesu_StackTracePrintUtil_Test
                    // -------------------------------------

// CLASS METHOD
inline
void baesu_StackTracePrintUtil_Test::printStackTraceToString(
                                                           bsl::string *string)
{
    bslma_Allocator *a = string->get_allocator().mechanism();
    bslma_DefaultAllocatorGuard guard(a);

    bsl::ostringstream os;
    baesu_StackTracePrintUtil::printStackTrace(os);
    *string = os.str();
}

}  // close namespace BloombergLP

#endif

// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2011
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ------------------------------ END-OF-FILE ---------------------------------
