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
//@SEE_ALSO: baesu_StackTrace, baesu_StackTraceFrame, baesu_StackTracePrintUtil
//
//@DESCRIPTION: This component provides a namespace for functions used in
// obtaining and printing a stack trace.  The simplest way to stream out a
// stack trace is by calling 'baesu_StackTracePrintUtil::printStackTrace', but
// other methods are available here.
//
///Usage Examples
///--------------
// The following examples illustrate 2 different ways to print a stack trace.
//
///1. Stack trace using the 'initializeStackTraceFromStack' method
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//..
// This example employes the 'loadStackTraceFromStack' method of
// 'baesu_StackTraceUtil', which is easy to use.  After it is called, the
// 'baesu_StackTrace' object contains the stack trace which can be output to
// any stream using 'printTerse'.
//..
// static
// void recurseAndPrintExample2(int *depth)
//..
// This function does the stack trace.
//..
// {
//     if (--*depth > 0) {
//..
// Recurse until '0 == *depth' before generating a stack trace.
//..
//         recurseAndPrintExample2(depth);
//     }
//     else {
//..
// Here we create a 'StackTrace' object and call 'initializeFromStack' to load
// the information from the stack of the current thread into the stack trace
// object.  We then use the '<<' operator to print out the stack trace.  In
// this case, 'maxFrames' is defaulting to 1000 and demangling is defaulting to
// 'on'.  Note that the object 'st' takes very little room on the stack,
// allocating most of its memory directly from virtual memory without going
// through the heap, thus minimizing potential complications due to stack size
// limits and possible heap corruption.
//..
//         baesu_StackTrace st;
//         int rc = baesu_StackTraceUtil::loadStackTraceFromStack(&st);
//..
// 'initializeFromStack' will fail and there will be no frames on Windows
// compiled with the optimizer.
//..
// #if !defined(BSLS_PLATFORM__OS_WINDOWS) || !defined(BDE_BUILD_TARGET_OPT)
//         assert(0 == rc);
//         assert(st.numFrames() >= 6);                    // main + 5 recurse
// #endif
//
//         baesu_StackTraceUtil::printFormatted(cout, st);
//     }
//
//     ++*depth;   // Prevent compiler from optimizing tail recursion as a
//                 // loop.
// }
//..
///2. Getting the addresses and then initiailizing from the address array
/// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// This is the least easy way to get a stack trace.  It is necessary to create
// an array of pointers to hold the return addresses from the stack, and this
// may not be desirable if we are in a situation where there isn't much room on
// the stack.  This approach may be desirable if one wants to quickly save the
// addresses that are the basis for a stack trace, postponing the more
// time-consuming translation of those addresses to more human-readable debug
// information until later.
//..
//  static
//  void recurseAndPrintExample3(int *depth)
//  {
//      if (--*depth > 0) {
//..
// Recurse until '0 == *depth' before generating a stack trace.
//..
//          recurseAndPrintExample3(depth);
//      }
//      else {
//          enum { ARRAY_LENGTH = 50 };
//          void *addresses[ARRAY_LENGTH];
//          baesu_StackTrace st;
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
// Next we call 'initializeFromAddressArray' to initialize the information in
// the stack trace object, such as function names, source file names, and line
// numbers, if they are available.  The third argument, demangle, is 'true'.
//..
//          int rc = baesu_StackTraceUtil::loadStackTraceFromAddressArray(
//                                                               &st
//                                                               addresses,
//                                                               numAddresses);
//..
// There will be no frames and 'initializeFromAddressArray' will fail if
// optimized on Windows with the optimizer.
//..
//  #if !defined(BSLS_PLATFORM__OS_WINDOWS) || !defined(BDE_BUILD_TARGET_OPT)
//          assert(0 == rc);
//          assert(st.numFrames() >= 6);                    // main + 5 recurse
//  #endif
//..
// Finally, wne can now print out the stack trace object using 'cout << st;',
// or iterate through the stack trace frames, printing them out one by one.  In
// this example, we want only function names, and not line numbers, source file
// names, or library names, so we iterate through the stack trace frames and
// print out only those properties.
//..
//          for (int i = 0; i < st.numFrames(); ++i) {
//              const baesu_StackTraceFrame& frame = st[i];
//
//              const char *sn = frame.symbolName().c_str();
//              sn = sn ? sn : "--unknown--";
//              cout << '(' << i << "): " << sn << endl;
//          }
//      }
//
//      ++*depth;   // Prevent compiler from optimizing tail recursion as a
//                  // loop.
//  }
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
    // useful for obtaining a stack trace.  Their main purpose is to support
    // the function 'baesu_StackTracePrintUtil::printStackTrace', but it is
    // possible, though less convenient, to obtain a stack trace using these
    // functions directly.  However, using these functions

    static
    int loadStackTraceFromAddressArray(
                             baesu_StackTrace *stackTrace,
                             void             *addresses[],
                             int               numAddresses,
                             bool              demanglingPreferredFlag = true);
        // Populate the specified '*stackTrace' object with stack trace
        // information from the stack, given a specifid array 'addresses' of at
        // least 'numAddresses' instruction pointers from the stack which can
        // be obtained by calling 'baesu_StackAddressUtil::getStackAddresses'.
        // Optionally specify 'demanglingPreferredFlag' indicating whether or
        // not demangling is to occur.  Return 0 on success and a non-zero
        // value otherwise.  The behavior is undefined if 'addresses == 0' or
        // if 'addresses' contains fewer than 'numAddresses' addresses.  Note
        // that demangling sometimes involves calling 'malloc', and that
        // demangling is always performed on the Windows platform and never
        // performed on Solaris using the CC compiler regardless of the value
        // of 'demanglingPreferredFlag'.  Also note that any frames previously
        // contained in the stack trace object are discarded.

    static
    int loadStackTraceFromStack(
                             baesu_StackTrace *stackTrace,
                             int               maxFrames = -1,
                             bool              demanglingPreferredFlag = true);
        // Populate the specified '*stackTrace' object with information about
        // the current thread's program stack.  Optionally specify 'maxFrames'
        // indicating the maximum number of frames to take from the top of the
        // stack, if it is not specified, the limit is 1024.  Optionally
        // specify 'demangle' to indicate whether function names are to be
        // demangled.  The behavior is undefined unless 'maxFrames', if
        // specified, is greater than 0.  Return 0 on success and a non-zero
        // value otherwise.  Note that demangling involves calling 'malloc',
        // and that demangling is always performed on the Windows platform and
        // never performed on Solaris using the CC compiler regardless of the
        // value of 'demanglingPreferredFlag'.  Finally note that any frames
        // previously contained in the 'stackTrace' object are discarded.

    static
    bsl::ostream& printFormatted(bsl::ostream& stream,
                                 const baesu_StackTrace& stackTrace);
        // Stream the specifid 'stackTrace' to the specified 'stream' in a
        // human-friendly format, one stack trace frame per line, without
        // using the default allocator.
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
