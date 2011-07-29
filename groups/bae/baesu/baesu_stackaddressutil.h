// baesu_stackaddressutil.h                                           -*-C++-*-
#ifndef INCLUDED_BAESU_STACKADDRESSUTIL
#define INCLUDED_BAESU_STACKADDRESSUTIL

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide a utility for obtaining return addresses from the stack
//
//@CLASSES:
//   baesu_StackAddressUtil: utilities for obtaining addresses from the stack
//
//@SEE_ALSO: baesu_stacktrace, baesu_stacktraceframe, baesu_stacktraceutil
//
//@AUTHOR: Oleg Semenov, Bill Chapman
//
//@DESCRIPTION: This component provides namespace containing a function that
// populates an array with an ordered sequence of return addresses from the
// current thread's function call stack.  Each return address points to the
// (text) memory location of the first instruction to be executed upon
// returning from a called routine.
//
///Usage
///-----
// In this section we show the intended usage of this component.
//
///Example 1: Obtaining return addresses and verifying their validity
/// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// In the following example we demonstrate how to obtain a collection of
// function return addresses using 'getStackAddresses'.
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
//      {}
//
//      bool operator<(const AddressEntry rhs) const
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
//      // Given the specfied 'retAddress' which should point to code within
//      // one of the functions described by the sorted vector 'entries',
//      // identify the index of the function containing that return address.
//  {
//      unsigned u = 0;
//      while (u < entries.size()-1 &&
//                                  retAddress >= entries[u+1].d_funcAddress) {
//          ++u;
//      }
//      assert(u < entries.size());
//      assert(retAddress >= entries[u].d_funcAddress);
//
//      int ret = entries[u].d_index;
//
//      return ret;
//  }
//..
// Have a volatile global in calculations to discourange optimizers from
// inlining.
//..
//  volatile int volatileGlobal = 2;
//..
// Then, we define a chain of functions that will call each other and do some
// random calculation to generate some code, and eventually call 'func1' which
// will call 'getAddresses' and verify that the addresses returned correspond
// to the functions we expect them to.
//..
//  static int func1();
//  static int func2()
//  {
//      return volatileGlobal * 2 * func1();
//  }
//  static int func3()
//  {
//      return volatileGlobal * 3 * func2();
//  }
//  static int func4()
//  {
//      return volatileGlobal * 4 * func3();
//  }
//  static int func5()
//  {
//      return volatileGlobal * 5 * func4();
//  }
//  static int func6()
//  {
//      return volatileGlobal * 6 * func5();
//  }
//..
// Next, we define the macro FUNC_ADDRESS, which will take as an arg a
// '&<function name>' and return a pointer to the actual beginning of the
// function's code, which is a non-trivial and platform-dependent exercise.
// (Note: this doesn't work on Windows for global routines).
//..
//  #if   defined(BSLS_PLATFORM__OS_HPUX)
//  # define FUNC_ADDRESS(p) (((void **) (void *) (p))[sizeof(void *) == 4])
//  #elif defined(BSLS_PLATFORM__OS_AIX)
//  # define FUNC_ADDRESS(p) (((void **) (void *) (p))[0])
//  #else
//  # define FUNC_ADDRESS(p) ((void *) (p))
//  #endif
//..
// Then, we define 'func1', which is the last of the chain of our functions
// that is called, which will do most of our work.
//..
//  int func1()
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
//      int numAddresses = baesu_StackAddressUtil::getStackAddresses(
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
// Note that on some, but not all, platforms there is an extra 'narcissic'
// frame describing 'getStackAddresses' itself at the beginning of 'buffer'.
// By starting our iteration through 'buffer' at 'BAESU_IGNORE_FRAMES', we
// guarantee that the first address we examine will be in 'func1' on all
// platforms.
//..
//      int funcIdx  = 1;
//      int stackIdx = baesu_StackAddressUtil::BAESU_IGNORE_FRAMES;
//      for (; funcIdx < (int) entries.size(); ++funcIdx, ++stackIdx) {
//          assert(stackIdx < numAddresses);
//          assert(funcIdx == findIndex(buffer[stackIdx]));
//      }
//
//      return 3;    // random value
//  }
//
//  int main()
//  {
//      func6();
//  }
//..

#ifndef INCLUDED_BSLS_PLATFORM
#include <bsls_platform.h>
#endif

#ifndef INCLUDED_BAESCM_VERSION
#include <baescm_version.h>
#endif

                       // ============================
                       // class baesu_StackAddressUtil
                       // ============================

namespace BloombergLP {

struct baesu_StackAddressUtil {
    // This struct provides a namespace for the function to obtain return
    // addresses from the stack.

    // On some platforms, 'getStackAddresses' finds a frame representing
    // 'getStackAddresses' itself.  This frame is usually unwanted.
    // 'BAESU_IGNORE_FRAMES' instructs the caller as to whether the first frame
    // is such an unwanted frame.

#if defined(BSLS_PLATFORM__OS_LINUX) || defined(BSLS_PLATFORM__OS_WINDOWS)
    enum { BAESU_IGNORE_FRAMES = 1 };
#else
    enum { BAESU_IGNORE_FRAMES = 0 };
#endif

    // CLASS METHODS
    static
    int getStackAddresses(void   **buffer,
                          int      maxFrames);
        // Get an ordered sequence of return addresses from the current
        // thread's function call stack and load them into the specified array
        // 'buffer', which is at least the specified 'maxFrames' in length.  A
        // return address is an address stored on the stack that points to the
        // first instruction that will be executed after the called subroutine
        // returns.  If there are more than 'maxFrames' frames on the stack,
        // only the return addresses for the 'maxFrames' most recent routine
        // calls are stored.  When this routine completes, 'buffer' will
        // contain an ordered sequence of return addresses, sorted such that
        // recent calls occur in the array before calls which took place before
        // them.  Return the number of stack frames stored into 'buffer' on
        // success, and a negative value otherwise.  The behavior is undefined
        // unless 'maxFrames >= 0' and 'buffer' has room for at least
        // 'maxFrames' addresses.  Note that this routine may fill 'buffer'
        // with garbage if the stack is corrupt, or on Windows if some stack
        // frames represent optimized routines.
};

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2010
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
