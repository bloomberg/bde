// baesu_stacktraceprintutil.h                                        -*-C++-*-
#ifndef INCLUDED_BAESU_STACKTRACEPRINTUTIL
#define INCLUDED_BAESU_STACKTRACEPRINTUTIL

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide a portable utility for generating a stack trace
//
//@CLASSES:
//  baesu_StackTracePrintUtil: namespace for function to print a stack trace
//
//@AUTHOR: Oleg Semenov, Bill Chapman
//
//@SEE ALSO: baesu_StackTraceUtil
//
//@DESCRIPTION: This component defines a namespace class containing a single
// static platform-independent function that will print a stack trace to a
// specfied stream.  Not all properties of the stack trace are printed on all
// platforms; the set of properties obtained for a stack trace are
// platform-specific.  Function names and addresses are supported on all
// platforms.  On Windows and AIX, source file names and line numbers are also
// provided.  The stack of the calling thread is always the one that is traced.
//
///Usage Example
///-------------
// The following examples illustrate 3 different ways to print a stack trace.
//
// This example shows how to write a stack trace to a stream, by calling the
// static function 'baesu_StackTraceUtil::printStackTrace'.
//
// We declare a function, 'recurseAndPrintExample1', that will recurse several
// times and then print a stack trace.
//..
//  static
//  void recurseAndPrintExample1(int *depth)
//  {
//      if (--*depth > 0) {
//..
// Recurse until '0 == *depth' before generating a stack trace.
//..
//          recurseAndPrintExample1(depth);
//      }
//      else {
//..
// Call 'printStackTrace' to print out a stack trace.  In this case, the
// 'maxFrames' argument is unspecified, defaulting to 1000 which is more than
// we need and the 'demangle' argument is unspecified, defaulting to 'on'.
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
//      recurseAndPrintExample1(&depth);
//      assert(5 == depth);
//  }
//..
// The following output is produced by this example on AIX (note that the lines
// were longer than 80 chars, so continuation is wrapped.
//..
//  (0) .recurseAndPrintExample1(int*)+0x60 at 0x10013060
//           source:baesu_stacktrace.t.cpp:488 in baesu_stacktrace.t.dbg_exc_mt
//  (1) .recurseAndPrintExample1(int*)+0x48 at 0x10013048
//           source:baesu_stacktrace.t.cpp:479 in baesu_stacktrace.t.dbg_exc_mt
//  (2) .recurseAndPrintExample1(int*)+0x48 at 0x10013048
//           source:baesu_stacktrace.t.cpp:479 in baesu_stacktrace.t.dbg_exc_mt
//  (3) .recurseAndPrintExample1(int*)+0x48 at 0x10013048
//           source:baesu_stacktrace.t.cpp:479 in baesu_stacktrace.t.dbg_exc_mt
//  (4) .recurseAndPrintExample1(int*)+0x48 at 0x10013048
//           source:baesu_stacktrace.t.cpp:479 in baesu_stacktrace.t.dbg_exc_mt
//  (5) .main+0x2e8 at 0x10000a68 source:baesu_stacktrace.t.cpp:575
//                                             in baesu_stacktrace.t.dbg_exc_mt
//  (6) .__start+0x9c at 0x100001ec source:crt0main.s in
//                                                baesu_stacktrace.t.dbg_exc_mt
//..

#ifndef INCLUDED_BAESCM_VERSION
#include <baescm_version.h>
#endif

#ifndef INCLUDED_BSL_IOSFWD
#include <bsl_iosfwd.h>
#endif

namespace BloombergLP {

                     // ===============================
                     // class baesu_StackTracePrintUtil
                     // ===============================

struct baesu_StackTracePrintUtil {
    // This 'struct' serves as a namespace for static methods that use
    // 'baesu_StackTrace'.

    // CLASS METHODS
    static
    void printStackTrace(bsl::ostream& stream,
                         int           maxFrames = -1,
                         bool          demangle = true);
        // Obtain a trace of the stack and print it to the specified 'stream'.
        // Optionally specify 'maxFrames' indicating the maximum number of
        // frames from the top of the stack that will be printed.  If
        // 'maxFrames' is not specified, a value of 1024 is assumed.
        // Optionally specify 'demangle', indicating whether function names
        // will be demangled.  If an error occurs, print a 1-line error message
        // to 'stream'.  The behavior is undefined unless 'maxFrames >= 0'.
        // Note that demangling, if specified, could involve calling 'malloc',
        // and that symbol names are always demangled on the Windows platform.
};

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
