// baesu_stacktraceprintutil.h                                        -*-C++-*-
#ifndef INCLUDED_BAESU_STACKTRACEPRINTUTIL
#define INCLUDED_BAESU_STACKTRACEPRINTUTIL

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide a portable utility for printing a stack trace.
//
//@CLASSES:
//  baesu_StackTracePrintUtil: namespace for function to print a stack trace
//
//@AUTHOR: Oleg Semenov, Bill Chapman
//
//@SEE ALSO: baesu_StackTraceUtil
//
//@DESCRIPTION: This component defines a namespace class containing a static
// platform-independent function that will print a stack trace to a supplied
// stream.  Not all properties of a stack trace are printed on all platforms;
// the set of properties describing a stack trace that are obtainable varies
// according to the platform and build parameters.  Function names and
// addresses are provided on all platforms.  On Windows and AIX, source file
// names and line numbers are also provided.  The 'printStackTrace' function
// always prints a description of the stack of the calling thread.
//
///Usage Example
///-------------
// The following examples illustrate 3 different ways to print a stack trace.
//
// This example shows how to write a stack trace to a stream, by calling the
// static function 'baesu_StackTraceUtil::printStackTrace'.
//..
//  static
//  void recurseAndPrintUsage(int *depth)
//..
// First, recurse the specified 'depth' number of times
//..
//  {
//      if (--*depth > 0) {
//          recurseAndPrintUsage(depth);
//      }
//      else {
//..
// Finally, call 'printStackTrace' to print out a stack trace.  In this case,
// the 'maxFrames' argument is unspecified, defaulting to 1000 which is more
// than we need and the 'demangle' argument is unspecified, defaulting to 'on'.
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
// The following output is produced by this example on AIX (note that the lines
// were longer than 80 chars, so continuation is wrapped).
//..
//  (0) .recurseAndPrintUsage(int*)+0x60 at 0x10013060
//           source:baesu_stacktrace.t.cpp:488 in baesu_stacktrace.t.dbg_exc_mt
//  (1) .recurseAndPrintUsage(int*)+0x48 at 0x10013048
//           source:baesu_stacktrace.t.cpp:479 in baesu_stacktrace.t.dbg_exc_mt
//  (2) .recurseAndPrintUsage(int*)+0x48 at 0x10013048
//           source:baesu_stacktrace.t.cpp:479 in baesu_stacktrace.t.dbg_exc_mt
//  (3) .recurseAndPrintUsage(int*)+0x48 at 0x10013048
//           source:baesu_stacktrace.t.cpp:479 in baesu_stacktrace.t.dbg_exc_mt
//  (4) .recurseAndPrintUsage(int*)+0x48 at 0x10013048
//           source:baesu_stacktrace.t.cpp:479 in baesu_stacktrace.t.dbg_exc_mt
//  (5) .main+0x2e8 at 0x10000a68 source:baesu_stacktrace.t.cpp:575
//                                             in baesu_stacktrace.t.dbg_exc_mt
//  (6) .__start+0x9c at 0x100001ec source:crt0main.s in
//                                                baesu_stacktrace.t.dbg_exc_mt
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
    static void forTestingOnlyDump(bsl::string *string);
        // Do a stack trace and assign the output to '*string'.  Note that this
        // is for testing only.

    static bsl::ostream& printStackTrace(
                                 bsl::ostream& stream,
                                 int           maxFrames = -1,
                                 bool          demanglingPreferredFlag = true);
        // Obtain a trace of the stack and print it to the specified 'stream'.
        // Optionally specify 'maxFrames' indicating the maximum number of
        // frames from the top of the stack that will be printed.  If
        // 'maxFrames' is not specified, a value of at least 1024 is assumed.
        // Optionally specify 'demangle', indicating whether function names
        // will be demangled.  If an error occurs, print a 1-line error message
        // to 'stream'.  The behavior is undefined unless 'maxFrames >= 0'.
        // Note that demangling, if specified, could involve calling 'malloc',
        // and symbol names are always demangled on the Windows platform.
};

// CLASS METHOD
inline
void baesu_StackTracePrintUtil::forTestingOnlyDump(bsl::string *string)
{
    bslma_Allocator *a = string->get_allocator().mechanism();
    bsl::ostringstream os(a);
    printStackTrace(os);
    {
        bslma_DefaultAllocatorGuard guard(a);
        *string = os.str();
    }
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
