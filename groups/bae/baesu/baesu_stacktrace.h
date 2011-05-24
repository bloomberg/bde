// baesu_stacktrace.h                                                 -*-C++-*-
#ifndef INCLUDED_BAESU_STACKTRACE
#define INCLUDED_BAESU_STACKTRACE

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide a set of portable utilities for generating a stack trace
//
//@CLASSES:
//  baesu_StackTrace: mechanism for obtaining a stack trace of function calls
//  baesu_StackTraceUtil: namespace for functions for printing a stack trace
//
//@AUTHOR: Oleg Semenov, Bill Chapman
//
//@SEE_ALSO: baesu_stacktraceframe, baesu_stackaddress
//
//@DESCRIPTION: This component defines a platform-independent interface for
// obtaining return addresses from the stack and resolving those addresses into
// human-readable debug information.  Not all properties of the stack trace are
// found on all platforms; the set of properties obtained for a stack trace are
// platform-specific.  Function names and addresses are supported on all
// platforms.  On Windows and AIX, source file names and line numbers are also
// provided.  The methods provided by this utility always trace the stack of
// the calling thread.  The class 'baesu_StackTrace' is a mechanism that
// contains the stack trace and a memory allocator, and methods to either
// gather address information from the stack, or take the address information
// as a argument, and translate that address information into human-readable
// debug information.  The class 'baesu_StackTraceUtil' contains a single
// static method that prints a stack trace.
//
///Usage Examples
///-------------
// The following examples illustrate 3 different ways to print a stack trace.
//
///1. Using 'baesu_StackTraceUtil::printStackTrace'
/// - - - - - - - - - - - - - - - - - - - - - - - -
// This example shows the easiest way to write a stack trace to a stream, by
// calling the static function 'baesu_StackTraceUtil::printStackTrace'.
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
// Calling 'printStackTrace' (a 'static' method) is the easiest way to print
// out a stack trace.  In this case, the 'maxFrames' argument is unspecified,
// defaulting to 1000, which is more than we need, and the 'demangle' argument
// is unspecified, defaulting to 'true'.
//..
//          baesu_StackTraceUtil::printStackTrace(cout);
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
///2. Stack trace using the 'initializeStackTraceFromStack' method
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//..
// This example employes the 'initializeStackTraceFromStack' method of
// 'baesu_StackTrace', which is easy to use.  After it is called, the
// 'baesu_StackTrace' object contains the stack trace which can be output to
// any stream using '<<'.
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
//         int rc = st.initializeFromStack();
//..
// 'initializeFromStack' will fail and there will be no frames on Windows
// compiled with the optimizer.
//..
// #if !defined(BSLS_PLATFORM__OS_WINDOWS) || !defined(BDE_BUILD_TARGET_OPT)
//         assert(0 == rc);
//         assert(st.numFrames() >= 6);                    // main + 5 recurse
// #endif
//
//         *out_p << st;    // print it out
//     }
//
//     ++*depth;   // Prevent compiler from optimizing tail recursion as a
//                 // loop.
// }
//..
///3. Getting the addresses and then initiailizing from the address array
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
//          int rc = st.initializeFromAddressArray(addresses, numAddresses);
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
//              const baesu_StackTraceFrame& frame = st.stackFrame(i);
//
//              const char *sn = frame.symbolName().c_str();
//              sn = sn ? sn : "--unknown--";
//              *out_p << '(' << i << "): " << sn << endl;
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

#ifndef INCLUDED_BDEMA_HEAPBYPASSALLOCATOR
#include <bdema_heapbypassallocator.h>
#endif

#ifndef INCLUDED_BAESU_STACKTRACEFRAME
#include <baesu_stacktraceframe.h>
#endif

#ifndef INCLUDED_BSL_IOSFWD
#include <bsl_iosfwd.h>
#endif

#ifndef INCLUDED_BSL_SSTREAM
#include <bsl_sstream.h>
#endif

#ifndef INCLUDED_BSL_VECTOR
#include <bsl_vector.h>
#endif

namespace BloombergLP {

                       // ======================
                       // class baesu_StackTrace
                       // ======================

class baesu_StackTrace {
    // This class is a value class (but not a value-semantic type) that can
    // contain a stack trace, and has methods with which to populate the stack
    // trace information, either directly from the stack or from an array of
    // addresses.  The stack trace can be printed out as an object using the
    // '<<' operator, also, individual stack trace frames can be accessed via
    // the 'stackFrame' operator.  The stack trace consists of a sequence of
    // indexed stack trace frames, where the frames with lower indexes
    // represent more recently called routines in the trace.

  public:
    enum {
        DEFAULT_MAX_FRAMES = 1000    // suggested default value for 'maxFrames'
                                     // arguments
    };

  private:
    // DATA
    bsl::vector<baesu_StackTraceFrame> *d_frames;    // pointer to owned vector
                                                     // of stack trace frames
                                                     // to contain debug
                                                     // information

    bdema_HeapBypassAllocator           d_allocator; // allocator, for
                                                     // allocating memory
                                                     // directly from virtual
                                                     // memory without going
                                                     // through the heap

  private:
    // NOT IMPLEMENTED
    baesu_StackTrace(const baesu_StackTrace&);
    baesu_StackTrace& operator=(const baesu_StackTrace&);

  public:
    // CLASS METHODS
    static void forTestingOnlyDump(bsl::string *string);
        // Do a stack trace and assign the output to '*string'.  Note that this
        // is for testing only.

    // CREATORS
    baesu_StackTrace();
        // Create an empty stack trace object.  Memory for this object will be
        // supplied by an allocator of type 'bdema_HeapBypassAllocator',
        // created and owned by this object.

    ~baesu_StackTrace();
        // Destroy this stack object.

    // MANIPULATORS
    bslma_Allocator *allocator();
        // Return the address of this object's modifiable allocator, which is
        // created upon creation of this object and destroyed, freeing any
        // memory it has allocated, at the end of this object's lifetime.  Note
        // that this is useful for allocating an address array to be used in
        // conjunction with 'initializeFromAddressArray'.

    int initializeFromAddressArray(void *addresses[],
                                   int   numAddresses,
                                   bool  demangle = true);
        // Populate this object with stack trace information from the stack,
        // given a specifid array 'addresses' of at least 'numAddresses'
        // instruction pointers from the stack that were obtained by calling
        // 'getStackPointers'.  Optionally specify 'demangle' indicating
        // whether or not demangling is to occur.  Return 0 on success and a
        // non-zero value otherwise.  The behavior is undefined if
        // 'addresses == 0' or if 'addresses' contains fewer than
        // 'numAddresses' addresses.  Note that demangling sometimes involves
        // calling 'malloc', and that demangling is always performed on the
        // Windows platform regardless of the value of 'demangle'.  Also note
        // that any frames previously contained in this object are discarded.

    int initializeFromStack(int  maxFrames = DEFAULT_MAX_FRAMES,
                            bool demangle  = true);
        // Populate this object with information about the current thread's
        // program stack.  Optionally specify 'maxFrames' indicating the
        // maximum number of frames to take from the top of the stack, if it is
        // not specified, the limit is 'DEFAULT_MAX_FRAMES'.  Optionally
        // specify 'demangle' to indicate whether function names are to be
        // demangled.  The behavior is undefined unless 'maxFrames >= 0'.
        // Return 0 on success and a non-zero value otherwise.  Note that
        // demangling involves calling 'malloc'.  Finally note that any frames
        // previously contained in this object are discarded.

    // ACCESSORS
    const baesu_StackTraceFrame& stackFrame(int index) const;
        // Return a reference to the 'index'th stack trace frame contained in
        // this object.  An index of 0 refers to the frame on top of the stack,
        // the most recently called routine, an index of '1' refers to the
        // frame representing the routine that called that one, and so on.  The
        // behavior is undefined unless '0 <= index < numFrames()'.

    int numFrames() const;
        // Return the number of stack trace frames contained in this object.

    bsl::ostream& printTerse(bsl::ostream& stream) const;
        // Print the stack trace frames described by this object to the
        // specified output 'stream', one frame per line, and return a
        // reference to 'stream'.  Note that if this object is empty, this
        // operation has no effect.
};

// FREE OPERATORS
bsl::ostream& operator<<(bsl::ostream&           stream,
                         const baesu_StackTrace& stackTrace);
    // Print any stack trace contained in the specified 'stackTrace' to the
    // specified 'stream', returning 'stream'.

                       // ==========================
                       // class baesu_StackTraceUtil
                       // ==========================

struct baesu_StackTraceUtil {
    // This 'struct' serves as a namespace for static methods that use
    // 'baesu_StackTrace'.

    enum {
        DEFAULT_MAX_FRAMES = 1000    // suggested default value for 'maxFrames'
                                     // arguments
    };

    // CLASS METHODS
    static
    void printStackTrace(bsl::ostream& stream,
                         int           maxFrames = DEFAULT_MAX_FRAMES,
                         bool          demangle = true);
        // Obtain a trace of the stack and print it to the specified 'stream'.
        // Optionally specify 'maxFrames' indicating the maximum number of
        // frames from the top of the stack that will be printed.  Optionally
        // specify 'demangle', indicating whether function names will be
        // demangled.  If an error occurs, print a 1-line error message to
        // 'stream'.  The behavior is undefined unless 'maxFrames >= 0'.
        // Note that demangling, if specified, could involve calling 'malloc',
        // and that symbol names are always demangled on the Windows platform.
};

//=============================================================================
//                         INLINE FUNCTION DEFINITIONS
//=============================================================================

                             // ----------------------
                             // class baesu_StackTrace
                             // ----------------------

// CLASS METHODS

inline
void baesu_StackTrace::forTestingOnlyDump(bsl::string *string)
{
    bsl::stringstream ss;

    baesu_StackTraceUtil::printStackTrace(ss);

    *string = ss.str();
}

// MANIPULATORS
inline
bslma_Allocator *baesu_StackTrace::allocator()
{
    return &d_allocator;
}

}  // close namespace BloombergLP

#endif

// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2010
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ------------------------------ END-OF-FILE ---------------------------------
