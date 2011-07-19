// baesu_stacktraceutil.h                                             -*-C++-*-
#ifndef INCLUDED_BAESU_STACKTRACEUTIL
#define INCLUDED_BAESU_STACKTRACEUTIL

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide low-level utilities for obtaining & printing a stack-trace.
//
//@CLASSES: baesu_StackTraceUtil
//
//@AUTHOR: Bill Chapman
//
//@SEE_ALSO: baesu_stacktraceprintutil
//
//@DESCRIPTION: This component provides a namespace for functions used in
// obtaining and printing a stack-trace.  Note that clients interested in
// simply printing a stack-trace are encouraged to use the
// 'baesu_stacktraceprintutil' component instead.
//
///Usage
///-----
// This section illustrates intended usage for this component.  The following
// examples demonstrate two distinct ways to load and print a stack-trace with
// 'baesu_StackTraceUtil' using (1) 'loadStackTraceFromStack' and
// (2) 'loadStackTraceFromAddresses'.
//
///Example 1: Loading Stack-Trace Directly from the Stack
/// - - - - - - - - - - - - - - - - - - - - - - - - - - -
// We start by defining a routine, 'recurseExample1', that will recurse the
// specified 'depth' times, then call 'traceExample1':
//..
//  void traceExample1();    // forward declaration
//
//  void recurseExample1(int *depth)
//      // Recurse the specified 'depth' number of times, then call
//      // 'traceExample1'.
//  {
//      if (--*depth > 0) {
//          recurseExample1(depth);
//      }
//      else {
//          traceExample1();
//      }
//
//      ++*depth;   // Prevent compiler from optimizing tail recursion as a
//                  // loop.
//  }
//..
// Then, we define the function 'traceExample1', that will print a stack-trace:
//..
//  void traceExample1()
//  {
//..
// Now, we create a 'baesu_StackTrace' object and call
// 'loadStackTraceFrameStack' to load the information from the stack of the
// current thread into the stack-trace object.
//
// In this call to 'loadStackTraceFromStack', we use the default value of
// 'maxFrames', which is at least 1024 and the default value for
// 'demanglingPreferredFlag', which is 'true', meaning that the operation will
// attempt to demangle function names.  Note that the object 'stackTrace' takes
// very little room on the stack, and by default allocates most of its memory
// directly from virtual memory without going through the heap, minimizing
// potential complications due to stack-size limits and possible heap
// corruption.
//..
//      baesu_StackTrace stackTrace;
//      int rc = baesu_StackTraceUtil::loadStackTraceFromStack(&stackTrace);
//      assert(0 == rc);
//..
// Finally, we use 'printFormatted' to stream out the stack-trace, one frame
// per line, in a concise, human-readable format.
//..
//      baesu_StackTraceUtil::printFormatted(cout, stackTrace);
//  }
//..
// The output from the preceding example on Solaris is as follows:
//..
// (0): traceExample1()+0x28 at 0x327d0 in baesu_stacktraceutil.t.dbg_exc_mt
// (1): recurseExample1(int*)+0x54 at 0x32e30 in baesu_stacktraceutil.t.dbg_exc
// (2): recurseExample1(int*)+0x44 at 0x32e20 in baesu_stacktraceutil.t.dbg_exc
// (3): recurseExample1(int*)+0x44 at 0x32e20 in baesu_stacktraceutil.t.dbg_exc
// (4): recurseExample1(int*)+0x44 at 0x32e20 in baesu_stacktraceutil.t.dbg_exc
// (5): recurseExample1(int*)+0x44 at 0x32e20 in baesu_stacktraceutil.t.dbg_exc
// (6): main+0x24c at 0x36c10 in baesu_stacktraceutil.t.dbg_exc_mt
// (7): _start+0x5c at 0x31d4c in baesu_stacktraceutil.t.dbg_exc_mt
//..
// Notice that the lines have been truncated to fit this 79 column source file,
// and that on AIX or Windows, source file name and line number information
// will also be displayed.
//
///Example 2: Loading a Stack-Trace from an Array of Stack Addresses
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// In this example, we demonstrate obtaining return addresses from the stack
// using 'baesu_StackAddressUtil', and later using them to load a
// 'baesu_StackTrace' object with a description of the stack.  This approach
// may be desirable if one wants to quickly save the addresses that are the
// basis for a stack-trace, postponing the more time-consuming translation of
// those addresses to more human-readable debug information until later.  To do
// this, we create an array of pointers to hold the return addresses from the
// stack, which may not be desirable if we are in a situation where there isn't
// much room on the stack.
//
// First, we define a routine 'recurseExample2' which will recurse the
// specified 'depth' times, then call 'traceExample2'.
//..
//  void traceExample2();    // forward declaration
//
//  static void recurseExample2(int *depth)
//      // Recurse the specified 'depth' number of times, then call
//      // 'traceExample2', which will print a stack-trace.
//  {
//      if (--*depth > 0) {
//          recurseExample2(depth);
//      }
//      else {
//          traceExample2();
//      }
//
//      ++*depth;   // Prevent compiler from optimizing tail recursion as a
//                  // loop.
//  }
//
//  void traceExample2()
//  {
//..
// Then, within 'traceExample2', we create a stack-trace object and an array
// 'addresses' to hold some addresses.
//..
//      baesu_StackTrace stackTrace;
//      enum { ARRAY_LENGTH = 50 };
//      void *addresses[ARRAY_LENGTH];
//..
// Next, we call 'baesu_StackAddressUtil::getStackAddresses' to get the stored
// return addresses from the stack and load them into the array 'addresses'.
// The call returns the number of addresses saved into the array, which will be
// less than or equal to 'ARRAY_LENGTH'.
//..
//      int numAddresses = baesu_StackAddressUtil::getStackAddresses(
//                                                           addresses,
//                                                           ARRAY_LENGTH);
//..
// Then, we call 'loadStackTraceFromAddressArray' to initialize the information
// in the stack-trace object, such as function names, source file names, and
// line numbers, if they are available.  The optional argument,
// 'demanglingPreferredFlag', defaults to 'true'.
//..
//      int rc = baesu_StackTraceUtil::loadStackTraceFromAddressArray(
//                                                               &stackTrace,
//                                                               addresses,
//                                                               numAddresses);
//      assert(0 == rc);
//..
// Finally, we can print out the stack-trace object using 'printFormatted', or
// iterate through the stack-trace frames, printing them out one by one.  In
// this example, we want instead to output only function names, and not line
// numbers, source file names, or library names, so we iterate through the
// stack-trace frames and print out only the properties we want.  Note that if
// a string is unknown, it is represented as "", here we print it out as
// "--unknown--" to let the user see that the name was unresolved.
//..
//      for (int i = 0; i < stackTrace.length(); ++i) {
//          const baesu_StackTraceFrame& frame = stackTrace[i];
//
//          const char *symbol = frame.isSymbolNameKnown()
//                             ? frame.symbolName().c_str()
//                             : "--unknown__";
//          cout << '(' << i << "): " << symbol << endl;
//      }
//  }
//..
// Running this example would produce the following output:
//..
// (0): traceExample2()
// (1): recurseExample2(int*)
// (2): recurseExample2(int*)
// (3): recurseExample2(int*)
// (4): recurseExample2(int*)
// (5): recurseExample2(int*)
// (6): main
// (7): _start
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
    // This 'struct' serves as a namespace for a collection of functions that
    // are useful for initializing and printing a stack-trace object.

    static
    int loadStackTraceFromAddressArray(
                           baesu_StackTrace   *result,
                           const void * const  addresses[],
                           int                 numAddresses,
                           bool                demanglingPreferredFlag = true);
        // Populate the specified 'result' with stack-trace information from
        // the stack, described by the specified array of 'addresses' of length
        // 'numAddresses'.  Optionally specify 'demanglingPreferredFlag' to
        // indicate whether or not to attempt to perform demangling, however,
        // demangling is always performed on the Windows platform and never
        // performed on Solaris using the CC compiler regardless of the value
        // of 'demanglingPreferredFlag'.  If 'demanglingPreferredFlag' is not
        // specified, demangling is performed on those platforms that support
        // it.  Return 0 on success, and a non-zero value otherwise.  Any
        // frames previously contained in the stack-trace object are discarded.
        // The behavior is undefined unless 'addresses' contains at least
        // 'numAddresses' addresses.  Note that the return addresses from the
        // stack can be obtained by calling
        // 'baesu_StackAddressUtil::getStackAddresses', and that demangling
        // sometimes involves calling 'malloc'.

    static
    int loadStackTraceFromStack(
                             baesu_StackTrace *result,
                             int               maxFrames = -1,
                             bool              demanglingPreferredFlag = true);
        // Populate the specified 'result' object with information about the
        // current thread's program stack.  Optionally specify 'maxFrames' to
        // indicate the maximum number of frames to take from the top of the
        // stack.  If 'maxFrames' is not specified, the default limit is at
        // least 1024.  Optionally specify 'demanglingPreferredFlag' to
        // indicate whether to attempt to perform demangling, if possible.  If
        // 'demanglingPreferredFlag' is not specfied, demangling is assumed to
        // be preferred, however, demangling is always performed on the Windows
        // platform and never performed on Solaris using the CC compiler
        // regardless of the value of 'demanglingPreferredFlag'.  Any frames
        // previously contained in the 'stackTrace' object are discarded.
        // Return 0 on success, and a non-zero value otherwise.  The behavior
        // is undefined unless 'maxFrames' (if specified) is greater than 0.
        // Note that demangling may involve calling 'malloc'.

    static
    bsl::ostream& printFormatted(bsl::ostream&           stream,
                                 const baesu_StackTrace& stackTrace);
        // Stream the specified 'stackTrace' to the specified 'stream' in some
        // multi-line, human-readable format.  Note that this operation
        // attempts to avoid using the default allocator.

    static
    bsl::ostream& printFormatted(bsl::ostream&                stream,
                                 const baesu_StackTraceFrame& stackTraceFrame);
        // Write the value of the specified 'stackTraceFrame' to the specified
        // output 'stream' in some single-line, human-readable format and
        // return a reference to 'stream'.  The name of the symbol is
        // represented by the 'symbolName' property of 'stackTraceFrame', if
        // known; otherwise, it is represented by the 'mangledSymbolName'
        // property, if known; otherwise, it is represented by "--unknown--".
        // Other frame attributes are written only if their values are known.
        // Note that the format is not fully specified, and can change without
        // notice.  Also note that this method attempts to avoid using the
        // default allocator.
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
