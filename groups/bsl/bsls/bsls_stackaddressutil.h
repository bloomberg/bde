// bsls_stackaddressutil.h                                            -*-C++-*-
#ifndef INCLUDED_BSLS_STACKADDRESSUTIL
#define INCLUDED_BSLS_STACKADDRESSUTIL

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a utility for obtaining return addresses from the stack.
//
//@CLASSES:
//   bsls::StackAddressUtil: utilities for obtaining addresses from the stack
//
//@SEE_ALSO: balst_stacktraceutil
//
//@DESCRIPTION: This component defines a 'struct', 'bsls::StackAddressUtil',
// that provides a namespace for a function, 'getStackAddresses', that
// populates an array with an ordered sequence of return addresses from the
// current thread's function call stack.  Each return address points to the
// (text) memory location of the first instruction to be executed upon
// returning from a called routine.
//
// This component also provides a function, 'formatCheapStack', that builds a
// current stack trace and formats it with instructions on how to use
// 'showfunc.tsk' to print out a stack trace matching where this function was
// called.  This is a Bloomberg standard "cheapstack" output.
//
///Usage
///-----
// In this section we show the intended usage of this component.
//
///Example 1: Obtaining Return Addresses and Verifying Their Validity
/// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// In the following example we demonstrate how to obtain the sequence of
// function return addresses from the stack using 'getStackAddresses'.
//
// First, we define 'AddressEntry', which will contain a pointer to the
// beginning of a function and an index corresponding to the function.  The '<'
// operator is defined so that a vector of address entries can be sorted in the
// order of the function addresses.  The address entries will be populated so
// that the entry containing '&funcN' when 'N' is an integer will have an index
// of 'N'.
//..
//  struct AddressEntry {
//      void *d_funcAddress;
//      int   d_index;
//
//      // CREATORS
//      AddressEntry(void *funcAddress, int index)
//      : d_funcAddress(funcAddress)
//      , d_index(index)
//          // Create an 'AddressEntry' object and initialize it with the
//          // specified 'funcAddress' and 'index'.
//      {}
//
//      bool operator<(const AddressEntry& rhs) const
//          // Return 'true' if the address stored in the object is lower than
//          // the address stored in 'rhs' and 'false' otherwise.  Note that
//          // this is a member function for brevity, it only exists to
//          // facilitate sorting 'AddressEntry' objects in a vector.
//      {
//          return d_funcAddress < rhs.d_funcAddress;
//      }
//  };
//..
// Then, we define 'entries', a vector of address entries.  This will be
// populated such that a given entry will contain function address '&funcN' and
// index 'N'.  The elements will be sorted according to function address.
//..
//  bsl::vector<AddressEntry> entries;
//..
// Next, we define 'findIndex':
//..
//  static int findIndex(const void *retAddress)
//      // Return the index of the address entry whose function uses an
//      // instruction located at specified 'retAddress'.  The behavior is
//      // undefined unless 'retAddress' is the address of an instruction in
//      // use by a function referred to by an address entry in 'entries'.
//  {
//      unsigned int u = 0;
//      while (u < entries.size()-1 &&
//                                  retAddress >= entries[u+1].d_funcAddress) {
//          ++u;
//      }
//      assert(u < entries.size());
//      assert(retAddress >= entries[u].d_funcAddress);
//
//      int ret = entries[u].d_index;
//
//      if (veryVerbose) {
//          P_(retAddress) P_(entries[u].d_funcAddress) P(ret);
//      }
//
//      return ret;
//  }
//..
// Then, we define a volatile global variable that we will use in calculation
// to discourage compiler optimizers from inlining:
//..
//  volatile unsigned int volatileGlobal = 1;
//..
// Next, we define a set of functions that will be called in a nested fashion
// -- 'func5' calls 'func4' who calls 'fun3' and so on.  In each function, we
// will perform some inconsequential instructions to prevent the compiler from
// inlining the functions.
//
// Note that we know the 'if' conditions in these 5 subroutines never evaluate
// to 'true', however, the optimizer cannot figure that out, and that will
// prevent it from inlining here.
//..
//  static unsigned int func1();
//  static unsigned int func2()
//  {
//      if (volatileGlobal > 10) {
//          return (volatileGlobal -= 100) * 2 * func2();             // RETURN
//      }
//      else {
//          return volatileGlobal * 2 * func1();                      // RETURN
//      }
//  }
//  static unsigned int func3()
//  {
//      if (volatileGlobal > 10) {
//          return (volatileGlobal -= 100) * 2 * func3();             // RETURN
//      }
//      else {
//          return volatileGlobal * 3 * func2();                      // RETURN
//      }
//  }
//  static unsigned int func4()
//  {
//      if (volatileGlobal > 10) {
//          return (volatileGlobal -= 100) * 2 * func4();             // RETURN
//      }
//      else {
//          return volatileGlobal * 4 * func3();                      // RETURN
//      }
//  }
//  static unsigned int func5()
//  {
//      if (volatileGlobal > 10) {
//          return (volatileGlobal -= 100) * 2 * func5();             // RETURN
//      }
//      else {
//          return volatileGlobal * 5 * func4();                      // RETURN
//      }
//  }
//  static unsigned int func6()
//  {
//      if (volatileGlobal > 10) {
//          return (volatileGlobal -= 100) * 2 * func6();             // RETURN
//      }
//      else {
//          return volatileGlobal * 6 * func5();                      // RETURN
//      }
//  }
//..
// Next, we define the macro FUNC_ADDRESS, which will take a parameter of
// '&<function name>' and return a pointer to the actual beginning of the
// function's code, which is a non-trivial and platform-dependent exercise.
// Note: this doesn't work on Windows for global routines.
//..
//  #if   defined(BSLS_PLATFORM_OS_HPUX)
//  # define FUNC_ADDRESS(p) (((void **) (void *) (p))[sizeof(void *) == 4])
//  #elif defined(BSLS_PLATFORM_OS_AIX)
//  # define FUNC_ADDRESS(p) (((void **) (void *) (p))[0])
//  #else
//  # define FUNC_ADDRESS(p) ((void *) (p))
//  #endif
//..
// Then, we define 'func1', the last function to be called in the chain of
// nested function calls.  'func1' uses
// 'bsls::StackAddressUtil::getStackAddresses' to get an ordered sequence of
// return addresses from the current thread's function call stack and uses the
// previously defined 'findIndex' function to verify those address are correct.
//..
//  unsigned int func1()
//      // Call 'getAddresses' and verify that the returned set of addresses
//      // matches our expectations.
//  {
//..
// Next, we populate and sort the 'entries' table, a sorted array of
// 'AddressEntry' objects that will allow 'findIndex' to look up within which
// function a given return address can be found.
//..
//      entries.clear();
//      entries.push_back(AddressEntry(0, 0));
//      entries.push_back(AddressEntry(FUNC_ADDRESS(&func1), 1));
//      entries.push_back(AddressEntry(FUNC_ADDRESS(&func2), 2));
//      entries.push_back(AddressEntry(FUNC_ADDRESS(&func3), 3));
//      entries.push_back(AddressEntry(FUNC_ADDRESS(&func4), 4));
//      entries.push_back(AddressEntry(FUNC_ADDRESS(&func5), 5));
//      entries.push_back(AddressEntry(FUNC_ADDRESS(&func6), 6));
//      bsl::sort(entries.begin(), entries.end());
//..
// Then, we obtain the stack addresses with 'getStackAddresses'.
//..
//      enum { BUFFER_LENGTH = 100 };
//      void *buffer[BUFFER_LENGTH];
//      bsl::memset(buffer, 0, sizeof(buffer));
//      int numAddresses = bsls::StackAddressUtil::getStackAddresses(
//                                                              buffer,
//                                                              BUFFER_LENGTH);
//      assert(numAddresses >= (int) entries.size());
//      assert(numAddresses < BUFFER_LENGTH);
//      assert(0 != buffer[numAddresses-1]);
//      assert(0 == buffer[numAddresses]);
//..
// Finally, we go through several of the first addresses returned in 'buffer'
// and verify that each address corresponds to the routine we expect it to.
//
// Note that on some, but not all, platforms there is an extra "narcissistic"
// frame describing 'getStackAddresses' itself at the beginning of 'buffer'.
// By starting our iteration through 'buffer' at 'k_IGNORE_FRAMES', we
// guarantee that the first address we examine will be in 'func1' on all
// platforms.
//..
//      int funcIdx  = 1;
//      int stackIdx = bsls::StackAddressUtil::k_IGNORE_FRAMES;
//      for (; funcIdx < (int) entries.size(); ++funcIdx, ++stackIdx) {
//          assert(stackIdx < numAddresses);
//          assert(funcIdx == findIndex(buffer[stackIdx]));
//      }
//
//      if (testStatus || veryVerbose) {
//          Q(Entries:);
//          for (unsigned int u = 0; u < entries.size(); ++u) {
//              P_(u); P_((void *) entries[u].d_funcAddress);
//              P(entries[u].d_index);
//          }
//
//          Q(Stack:);
//          for (int i = 0; i < numAddresses; ++i) {
//              P_(i); P(buffer[i]);
//          }
//      }
//
//      return volatileGlobal;
//  }
//..
//
///Example 2: Obtaining a "Cheapstack"
///- - - - - - - - - - - - - - - - - -
// In this example we demonstrate how to use 'formatCheapStack' to generate a
// string containing the current stack trace and instructions on how to print
// it out from 'showfunc.tsk'.  Note that 'showfunc.tsk' is a Bloomberg tool
// that, when given an executable along with a series of function addresses
// from a process that was running that executable, will print out a
// human-readable stack trace with the names of the functions being called in
// that stack trace.
//
// First, we define our function where we want to format the stack:
//..
//  struct MyTest {
//      static void printCheapStack()
//      {
//          char str[128];
//          bsls::StackAddressUtil::formatCheapStack(str, 128);
//          printf("%s", str);
//      }
//  };
//..
// Calling this function will then result in something like this being printed
// to standard output:
//..
//  Please run "/bb/bin/showfunc.tsk <binary_name_here> 403308 402641 ...
//                               ... 3710C1ED1D 400F49" to see the stack trace.
//..
// Then, if you had encountered this output running the binary "mybinary.tsk",
// you could see your stack trace by running this command:
//..
//  /bb/bin/showfunc.tsk mybinary.tsk 403308 402641 3710C1ED1D 400F49
//..
// This will produce output like this:
//..
//  0x403308 _ZN6MyTest15printCheapStackEv + 30
//  0x402641 main + 265
//  0x3710c1ed1d ???
//  0x400f49 ???
//..
// telling you that 'MyTest::printCheapStack' was called directly from 'main'.
// Note that if you had access to the binary name that was invoked, then that
// could be provided as the optional last argument to 'printCheapStack' to get
// a 'showfunc.tsk' command that can be more easily invoked, like this:
//..
//  struct MyTest2 {
//      static void printCheapStack()
//      {
//          char str[128];
//          bsls::StackAddressUtil::formatCheapStack(str, 128, "mybinary.tsk");
//          printf("%s", str);
//      }
//  };
//..
// resulting in output that looks like this:
//..
//  Please run "/bb/bin/showfunc.tsk mybinary.tsk 403308 402641 3710C1ED1D ...
//                                          ... 400F49" to see the stack trace.
//..

#include <bsls_platform.h>

                      // ============================
                      // class bsls::StackAddressUtil
                      // ============================

namespace BloombergLP {
namespace bsls {

struct StackAddressUtil {
    // This struct provides a namespace for the function to obtain return
    // addresses from the stack.

    // On some platforms, 'getStackAddresses' finds a frame representing
    // 'getStackAddresses' itself.  This frame is usually unwanted.
    // 'k_IGNORE_FRAMES' instructs the caller as to whether the first frame is
    // such an unwanted frame.

#if   defined(BSLS_PLATFORM_OS_LINUX) || defined(BSLS_PLATFORM_OS_DARWIN)
    enum { k_IGNORE_FRAMES = 1 };
#else
    enum { k_IGNORE_FRAMES = 0 };
#endif

    // CLASS METHODS
    static
    int getStackAddresses(void   **buffer,
                          int      maxFrames);
        // Get an sequence of return addresses from the current thread's
        // function call stack, ordered from most recent call to least recent,
        // and load them into the specified array '*buffer', which is at least
        // the specified 'maxFrames' in length.  A return address is an address
        // stored on the stack that points to the first instruction that will
        // be executed after the called subroutine returns.  If there are more
        // than 'maxFrames' frames on the stack, only the return addresses for
        // the 'maxFrames' most recent routine calls are stored.  When this
        // routine completes, 'buffer' will contain an ordered sequence of
        // return addresses, sorted such that recent calls occur in the array
        // before calls that took place before them.  Return the number of
        // stack frames stored into 'buffer' on success, and a negative value
        // otherwise.  The behavior is undefined unless 'maxFrames >= 0' and
        // 'buffer' has room for at least 'maxFrames' addresses.  Note that
        // this routine may fill 'buffer' with garbage if the stack is corrupt,
        // or on Windows if some stack frames represent optimized routines.

    static
    void formatCheapStack(char *output, int length, const char *taskname = 0);
        // Load the specified 'output' buffer having the specified 'length'
        // with the Bloomberg standard "cheapstack" contents as a
        // null-terminated string.  On successfully obtaining the current call
        // stack, this will be instructions on how to run the Bloomberg tool
        // 'showfunc.tsk' (with the optionally specified 'taskname', otherwise
        // with an attempt to obtain the system-specific process name) to get
        // details of the current call stack where 'formatCheapStack' was
        // called.  On failure, text indicating that the call stack was not
        // obtainable will be written to 'output'.  If 'length' is not long
        // enough for the entire output it will be truncated.  The behavior is
        // undefined unless '0 <= length' and 'output' has the capacity for at
        // least 'length' bytes.
};

}  // close package namespace
}  // close enterprise namespace

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
