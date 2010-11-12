// bdema_testallocatorexception.h   -*-C++-*-
#ifndef INCLUDED_BDEMA_TESTALLOCATOREXCEPTION
#define INCLUDED_BDEMA_TESTALLOCATOREXCEPTION

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide exception class for memory allocation operations.
//
//@DEPRECATED: Use 'bslma_testallocatorexception' instead.
//
//@CLASSES:
//   bdema_TestAllocatorException: exception containing allocation information
//
//@AUTHOR: Shao-wei Hung (shung1)
//
//@DESCRIPTION: This component defines a simple exception object for testing
// exceptions during memory allocation operations.  The exception object
// 'bdema_TestAllocatorException' contains information about the allocation
// request, which can be queried for by the "catcher" of this exception.
//
///USAGE
///-----
// In the following example, the 'bdema_TestAllocatorException' object is
// thrown by the 'allocate' method of the 'my_Allocator' object after the
// number of allocation requests exceeds the allocator's allocation limit.
// This example demonstrates how to use a user-defined allocator (e.g.,
// 'my_Allocator') and 'bdema_TestAllocatorException' to verify that an object
// (e.g., 'my_ShortArray') under test is exception neutral:
//..
//  // my_allocator.h
//  #include <bdema_allocator.h>
//
//  class my_Allocator : public bdema_Allocator {
//      int d_allocationLimit;
//      // ...
//
//    private:
//      // NOT IMPLEMENTED
//      my_Allocator(const my_Allocator&);
//      my_Allocator& operator=(const my_Allocator&);
//
//    public:
//      // CREATORS
//      my_Allocator() : d_allocationLimit(-1) {}
//      ~my_Allocator() {}
//
//      void *allocate(int size);
//      void deallocate(void *address)     { free(address); }
//      void setAllocationLimit(int limit) { d_allocationLimit = limit; }
//      int allocationLimit() const        { return d_allocationLimit; }
//      // ...
//  };
//
//  inline
//  void *my_Allocator::allocate(int size)
//  {
//  #ifdef BDE_BUILD_TARGET_EXC
//      if (0 <= d_allocationLimit) {
//          --d_allocationLimit;
//          if (0 > d_allocationLimit) {
//              throw bdema_TestAllocatorException(size);
//          }
//      }
//  #endif
//      return (void *) malloc(size);
//  }
//
//..
// Note that the macro 'BDE_BUILD_TARGET_EXC' is defined at compile-time to
// indicate whether exceptions are enabled.  In the above code, if exceptions
// are not enabled, the code that throws 'bdema_TestAllocatorException' is
// never executed.  The following is the test driver for 'my_ShortArray'.
//
// Note that "\$" must be replaced by "\" in the preprocessor macro definitions
// that follow.  The "$" symbols are present in this header file to avoid a
// diagnostic elicited by some compilers (e.g., "warning: multi-line comment").
//..
//  // my_shortarray.t.cpp
//  #include <my_shortarray.h>
//  #include <my_testallocator.h>
//  #include <bdema_testallocatorexception.h>
//
//  // ...
//
//  #ifdef BDE_BUILD_TARGET_EXC
//  #define BEGIN_BDEMA_EXCEPTION_TEST {                                     \$
//  {                                                                        \$
//      static int firstTime = 1;                                            \$
//      if (veryVerbose && firstTime) bsl::cout <<                           \$
//          "### BDEMA EXCEPTION TEST -- (ENABLED) --" << bsl::endl;         \$
//      firstTime = 0;                                                       \$
//  }                                                                        \$
//  if (veryVeryVerbose) bsl::cout <<                                        \$
//      "### Begin bdema exception test." << bsl::endl;                      \$
//  int bdemaExceptionCounter = 0;                                           \$
//  static int bdemaExceptionLimit = 100;                                    \$
//  testAllocator.setAllocationLimit(bdemaExceptionCounter);                 \$
//  do {                                                                     \$
//      try {
//
//  #define END_BDEMA_EXCEPTION_TEST                                         \$
//      } catch (bdema_TestAllocatorException& e) {                          \$
//          if (veryVerbose && bdemaExceptionLimit || veryVeryVerbose) {     \$
//              --bdemaExceptionLimit;                                       \$
//              bsl::cout << "(*** " << bdemaExceptionCounter << ')';        \$
//              if (veryVeryVerbose) { bsl::cout << " BDEMA_EXCEPTION: "     \$
//                  << "alloc limit = " << bdemaExceptionCounter << ", "     \$
//                  << "last alloc size = " << e.numBytes();                 \$
//              }                                                            \$
//              else if (0 == bdemaExceptionLimit) {                         \$
//                  bsl::cout << " [ Note: 'bdemaExceptionLimit' reached. ]";\$
//              }                                                            \$
//              bsl::cout << bsl::endl;                                      \$
//          }                                                                \$
//          testAllocator.setAllocationLimit(++bdemaExceptionCounter);       \$
//          continue;                                                        \$
//      }                                                                    \$
//      testAllocator.setAllocationLimit(-1);                                \$
//      break;                                                               \$
//  } while (1);                                                             \$
//  if (veryVeryVerbose) bsl::cout <<                                        \$
//      "### End bdema exception test." << bsl::endl;                        \$
//  }
//  #else
//  #define BEGIN_BDEMA_EXCEPTION_TEST                                       \$
//  {                                                                        \$
//      static int firstTime = 1;                                            \$
//      if (verbose && firstTime) { bsl::cout <<                             \$
//          "### BDEMA EXCEPTION TEST -- (NOT ENABLED) --" << bsl::endl;     \$
//          firstTime = 0;                                                   \$
//      }                                                                    \$
//  }
//  #define END_BDEMA_EXCEPTION_TEST
//  #endif
//
//  // ...
//
//  static
//  int areEqual(const short *array1, const short *array2, int numElement)
//      // Return 1 if the specified initial 'numElement' in the specified
//      // 'array1' and 'array2' have the same values, and 0 otherwise.
//  {
//      for (int i = 0; i < numElement; ++i) {
//          if (array1[i] != array2[i]) return 0;
//      }
//      return 1;
//  }
//
//  int main(int argc, char *argv[]) {
//      int test = argc > 1 ? atoi(argv[1]) : 0;
//      int verbose = argc > 2;
//      int veryVerbose = argc > 3;
//      int veryVeryVerbose = argc > 4;
//
//      my_Allocator testAllocator;
//
//      switch (test) { case 0:
//
//        // ...
//
//        case 6: {
//          struct {
//              int d_line;
//              int d_numElem;
//              short d_exp[NUM_VALUES];
//          } DATA[] = {
//              { L_, 0, {} },
//              { L_, 1, { V0 } },
//              { L_, 5, { V0, V1, V2, V3, V4 } }
//          };
//
//          const int NUM_TEST = sizeof DATA / sizeof *DATA;
//
//          for (int ti = 0; ti < NUM_TEST; ++ti) {
//              const int    LINE     = DATA[ti].d_line;
//              const int    NUM_ELEM = DATA[ti].d_numElem;
//              const short *EXP      = DATA[ti].d_exp;
//
//              BEGIN_BDEMA_EXCEPTION_TEST {
//                  my_ShortArray mA(&testAllocator);
//                  const my_ShortArray& A = mA;
//                  for (int ei = 0; ei < NUM_ELEM; ++ei) {
//                      mA.append(VALUES[ei]);
//                  }
//                  if (veryVerbose) { P_(ti); P_(NUM_ELEM); P(A); }
//                  LOOP2_ASSERT(LINE, ti, areEqual(EXP, A, NUM_ELEM));
//              } END_BDEMA_EXCEPTION_TEST
//          }
//
//          if (veryVerbose) bsl::cout << testAllocator << bsl::endl;
//        } break;
//
//        // ...
//
//      }
//
//      // ...
//  }
//..

#ifndef INCLUDED_BDESCM_VERSION
#include <bdescm_version.h>
#endif

#ifndef INCLUDED_BSLMA_TESTALLOCATOREXCEPTION
#include <bslma_testallocatorexception.h>
#endif

namespace BloombergLP {

typedef bslma_TestAllocatorException bdema_TestAllocatorException;
    // This class defines an exception object for memory allocation operations.
    // Instances of this class contain information about the allocation
    // request.

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2002
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
