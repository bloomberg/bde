// baesu_stacktraceutil.h                                             -*-C++-*-
#ifndef INCLUDED_BAESU_STACKTRACEUTIL
#define INCLUDED_BAESU_STACKTRACEUTIL

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide platform-independent utilities for obtaining a stack trace
//
//@CLASSES: baesu_StackTraceUtil
//
//@AUTHOR: Bill Chapman
//
//@SEE_ALSO: baesu_stacktrace, baesu_stacktraceframe, baesu_stacktraceprintutil
//
//@DESCRIPTION: This component provides a namespace for functions used in
// obtaining and printing a stack trace.  Note that clients interested in the
// simplest way of obtaining a stack trace will usually prefer to use the
// 'baesu_stacktraceprintutil' component rather than this one.
//
///Usage Examples
///--------------
// The following examples illustrate 2 different ways to load and print a stack
// trace using 'baesu_StackTraceUtil.
//
// Example 1: Loading a Stack Trace with 'loadStackTraceFromStack':
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// This example demonstrates using 'loadStackTraceFromStack' to load a stack
// trace.  After the 'loadStackTraceFromStack' function is called, the stack
// trace object contains the stack trace, which can be output using
// 'printFormatted'.
//..
//  void example1(int *depth)
//      // Recurse the specified 'depth' times, then print a stack trace.
//  {
//      if (--*depth > 0) {
//..
// Recurse until '0 == *depth' before generating a stack trace.
//..
//          example1(depth);
//      }
//      else {
//..
// Here we create a 'StackTrace' object and call 'loadStackTraceFrameStack' to
// load the information from the stack of the current thread into the stack
// trace object.  We then use the 'printFormatted' function to print out the
// stack trace.
//
// In this call to 'loadStackTraceFromStack', 'maxFrames' defaultings to 1024
// and 'demanglingPreferredFlag' defaults to 'true', meaning that the function
// will attempt to demangle function names.  Note that the object 'stackTrace'
// takes very little room on the stack, allocating most of its memory directly
// from virtual memory without going through the heap, minimizing potential
// complications due to stack size limits and possible heap corruption.
//..
//          baesu_StackTrace stackTrace;
//          int rc = baesu_StackTraceUtil::loadStackTraceFromStack(
//                                                                &stackTrace);
//          assert(0 == rc);
//..
// 'printFormatted' will stream out the stack trace, one frame per line, in a
// concise, human-friendly format.
//..
//          baesu_StackTraceUtil::printFormatted(cout, stackTrace);
//      }
//
//      ++*depth;   // Prevent compiler from optimizing tail recursion as a
//                  // loop.
//  }
//..
// Output from example 1 on Solaris:
// - - - - - - - - - - - - - - - - -
//..
// (0): example1(int*)+0x78 at 0x324ec in baesu_stacktraceutil.t.dbg_exc_mt
// (1): example1(int*)+0x44 at 0x324b8 in baesu_stacktraceutil.t.dbg_exc_mt
// (2): example1(int*)+0x44 at 0x324b8 in baesu_stacktraceutil.t.dbg_exc_mt
// (3): example1(int*)+0x44 at 0x324b8 in baesu_stacktraceutil.t.dbg_exc_mt
// (4): example1(int*)+0x44 at 0x324b8 in baesu_stacktraceutil.t.dbg_exc_mt
// (5): main+0x2a8 at 0x36310 in baesu_stacktraceutil.t.dbg_exc_mt
// (6): _start+0x5c at 0x31a88 in baesu_stacktraceutil.t.dbg_exc_mt
//..
// Note that on AIX or Windows, source file name and line number information
// will also be displayed.
//
// Example 2: loading the stack trace from an array of stack addresses:
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// In this example, we create an array of pointers to hold the return addresses
// from the stack, and this may not be desirable if we are in a situation where
// there isn't much room on the stack.  This approach may be desirable if one
// wants to quickly save the addresses that are the basis for a stack trace,
// postponing the more time-consuming translation of those addresses to more
// human-readable debug information until later.
//..
//  static void example2(int *depth)
//      // Recurse the specified 'depth' number of times, then do a stack
//      // trace.
//  {
//      if (--*depth > 0) {
//..
// Recurse until '0 == *depth' before generating a stack trace.
//..
//          example2(depth);
//      }
//      else {
//          enum { ARRAY_LENGTH = 50 };
//          void *addresses[ARRAY_LENGTH];
//          baesu_StackTrace stackTrace;
//..
// First, we call 'getStackAddresses' to get the stored return addresses from
// the stack and load them into the array 'addresses'.  The call returns the
// number of addresses saved into the array, which will be less than or equal
// to 'ARRAY_LENGTH'.
//..
//          int numAddresses = baesu_StackAddressUtil::getStackAddresses(
//                                                               addresses,
//                                                               ARRAY_LENGTH);
//..
// Next we call 'loadStackTraceFrameAddressArray' to initialize the information
// in the stack trace object, such as function names, source file names, and
// line numbers, if they are available.  The optional argument,
// 'demanglingPreferredFlag', defaults to 'true'.
//..
//          int rc = baesu_StackTraceUtil::loadStackTraceFromAddressArray(
//                                                               &stackTrace,
//                                                               addresses,
//                                                               numAddresses);
//          assert(0 == rc);
//..
// Finally, we can now print out the stack trace object using 'printFormatted',
// or iterate through the stack trace frames, printing them out one by one.  In
// this example, we want only function names, and not line numbers, source file
// names, or library names, so we iterate through the stack trace frames and
// print out only the properties we want.
//..
//          for (int i = 0; i < stackTrace.length(); ++i) {
//              const baesu_StackTraceFrame& frame = stackTrace[i];
//
//              const char *symbol = frame.isSymbolNameKnown()
//                                 ? frame.symbolName().c_str()
//                                 : "--unknown__";
//              cout << '(' << i << "): " << symbol << endl;
//          }
//      }
//
//      ++*depth;   // Prevent compiler from optimizing tail recursion as a
//                  // loop.
//  }
//..
// Output From Example 2 on Solaris:
// - - - - - - - - - - - - - - - - -
//..
// (0): example2(int*)
// (1): example2(int*)
// (2): example2(int*)
// (3): example2(int*)
// (4): example2(int*)
// (5): main
// (6): _start
//..

#ifndef INCLUDED_BAESCM_VERSION
#include <baescm_version.h>
#endif

#ifndef INCLUDED_BAESU_STACKTRACE
#include <baesu_stacktrace.h>
#endif

#ifndef INCLUDED_BSL_IOSFWD
#include <bsl_iosfwd.h>
#endif

namespace BloombergLP {

class bslma_Allocator;

struct baesu_StackTraceUtil {
    // This 'struct' is a namespace for a collection of functions that are
    // useful for obtaining a stack trace, specifically, for initializing a
    // stack trace object and printing it.

    static
    int loadStackTraceFromAddressArray(
                           baesu_StackTrace   *result,
                           const void * const  addresses[],
                           int                 numAddresses,
                           bool                demanglingPreferredFlag = true);
        // Populate the specified 'result' with stack trace information from
        // the stack, described by the specified array of 'addresses' of at
        // least 'numAddresses' instruction addresses.  Optionally specify
        // 'demanglingPreferredFlag' indicating whether or not to attempt to
        // perform demangling.  Return 0 on success and a non-zero value
        // otherwise.  Any frames previously contained in the stack trace
        // object are discarded.  The behavior is undefined if '0 == addresses'
        // or if 'addresses' contains fewer than 'numAddresses' addresses.
        // Note that the return addresses from the stack which can be obtained
        // by calling 'baesu_StackAddressUtil::getStackAddresses'.  Also note
        // that demangling sometimes involves calling 'malloc', and that
        // demangling is always performed on the Windows platform and never
        // performed on Solaris using the CC compiler regardless of the value
        // of 'demanglingPreferredFlag'.

    static
    int loadStackTraceFromStack(
                             baesu_StackTrace *result,
                             int               maxFrames = -1,
                             bool              demanglingPreferredFlag = true);
        // Populate the specified 'result' object with information about the
        // current thread's program stack.  Optionally specify 'maxFrames'
        // indicating the maximum number of frames to take from the top of the
        // stack, if it is not specified, the limit is at least 1024.
        // Optionally specify 'demanglingPreferredFlag' to indicate whether to
        // attempt to perform demangling.  Any frames previously contained in
        // the 'stackTrace' object are discarded.  Return 0 on success and a
        // non-zero value otherwise.  The behavior is undefined unless
        // 'maxFrames' (if specified) is greater than 0.  Note that demangling
        // involves calling 'malloc', and that demangling is always performed
        // on the Windows platform and never performed on Solaris using the CC
        // compiler regardless of the value of 'demanglingPreferredFlag'.

    static
    bsl::ostream& printFormatted(bsl::ostream& stream,
                                 const baesu_StackTrace& stackTrace);
        // Stream the specifid 'stackTrace' to the specified 'stream' in a
        // human-friendly format.  Note that this operation attempts to avoid
        // using the default allocator.

    static
    bsl::ostream& printFormatted(bsl::ostream&                stream,
                                 const baesu_StackTraceFrame& stackTraceFrame);
        // Write the value of the specified 'stackTraceFrame' to the specified
        // output 'stream' in a human-readable format, on a single line, and
        // return a reference to 'stream'.  The name of the symbol is
        // represented by the 'symbolName' property of 'stackTraceFrame', if
        // known; otherwise it is represented by the 'mangledSymbolName'
        // property, if known; otherwise, it is represented by "--unknown--".
        // Other frame attributes are written only if their values are known.
        // Note that the format is not fully specified, and can change without
        // notice.  Also note that this is attempted to be done without using
        // the default allocator.
};

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2011
//      All Rights Reserved.
//      Property of Bloomberg L.P.  (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
