// bslma_testallocatorexception.h                                     -*-C++-*-
#ifndef INCLUDED_BSLMA_TESTALLOCATOREXCEPTION
#define INCLUDED_BSLMA_TESTALLOCATOREXCEPTION

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide an exception class for memory allocation operations.
//
//@CLASSES:
//  bslma::TestAllocatorException: exception containing allocation information
//
//@DESCRIPTION: This component defines a simple exception object for testing
// exceptions during memory allocation operations.  The exception object
// 'bslma::TestAllocatorException' contains information about the allocation
// request, which can be queried for by the "catcher" of this exception.
//
///Usage
///-----
// In the following example, the 'bslma::TestAllocatorException' object is
// thrown by the 'allocate' method of the 'my_Allocator' object after the
// number of allocation requests exceeds the allocator's allocation limit.
// This example demonstrates how to use a user-defined allocator (e.g.,
// 'my_Allocator') and 'bslma::TestAllocatorException' to verify that an object
// (e.g., 'my_ShortArray') under test is exception-neutral:
//..
//  // my_allocator.h
//  #include <bslma_allocator.h>
//
//  class my_Allocator : public bslma::Allocator {
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
//      void deallocate(void *address) { free(address); }
//      void setAllocationLimit(int limit){ d_allocationLimit = limit; }
//      int allocationLimit() const { return d_allocationLimit; }
//      // ...
//  };
//
//  // my_allocator.cpp
//  #include <my_allocator.h>
//
//  void *my_Allocator::allocate(int size)
//  {
//  #ifdef BDE_BUILD_TARGET_EXC
//      if (0 <= d_allocationLimit) {
//          --d_allocationLimit;
//          if (0 > d_allocationLimit) {
//              throw bslma::TestAllocatorException(size);
//          }
//      }
//  #endif
//      return (void *)malloc(size);
//  }
//..
// Note that the macro 'BDE_BUILD_TARGET_EXC' is defined at compile-time to
// indicate whether exceptions are enabled.  In the above code, if exceptions
// are not enabled, the code that throws 'bslma::TestAllocatorException' is
// never executed.  The following is the test driver for 'my_ShortArray'.
//
// Note that "\$" must be replaced by "\" in the preprocessor macro definitions
// that follow.  The "$" symbols are present in this header file to avoid a
// diagnostic elicited by some compilers (e.g., "warning: multi-line comment").
//..
//  // my_shortarray.t.cpp
//  #include <my_shortarray.h>
//  #include <my_testallocator.h>
//  #include <bslma_testallocatorexception.h>
//
//  // ...
//
//  #ifdef BDE_BUILD_TARGET_EXC
//  #define BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN {                       \$
//  {                                                                        \$
//      static int firstTime = 1;                                            \$
//      if (veryVerbose && firstTime) std::cout <<                           \$
//          "### BSLMA EXCEPTION TEST -- (ENABLED) --" << std::endl;         \$
//      firstTime = 0;                                                       \$
//  }                                                                        \$
//  if (veryVeryVerbose) std::cout <<                                        \$
//      "### Begin bslma exception test." << std::endl;                      \$
//  int bslmaExceptionCounter = 0;                                           \$
//  static int bslmaExceptionLimit = 100;                                    \$
//  testAllocator.setAllocationLimit(bslmaExceptionCounter);                 \$
//  do {                                                                     \$
//      try {
//
//  #define BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END                           \$
//      } catch (bslma::TestAllocatorException& e) {                         \$
//          if (veryVerbose && bslmaExceptionLimit || veryVeryVerbose) {     \$
//              --bslmaExceptionLimit;                                       \$
//              std::cout << "(*** " << bslmaExceptionCounter << ')';        \$
//              if (veryVeryVerbose) { std::cout << " BSLMA_EXCEPTION: "     \$
//                  << "alloc limit = " << bslmaExceptionCounter << ", "     \$
//                  << "last alloc size = " << e.numBytes();                 \$
//              }                                                            \$
//              else if (0 == bslmaExceptionLimit) {                         \$
//                  std::cout << " [ Note: 'bslmaExceptionLimit' reached. ]";\$
//              }                                                            \$
//              std::cout << std::endl;                                      \$
//          }                                                                \$
//          testAllocator.setAllocationLimit(++bslmaExceptionCounter);       \$
//          continue;                                                        \$
//      }                                                                    \$
//      testAllocator.setAllocationLimit(-1);                                \$
//      break;                                                               \$
//  } while (1);                                                             \$
//  if (veryVeryVerbose) std::cout <<                                        \$
//      "### End bslma exception test." << std::endl;                        \$
//  }
//  #else
//  #define BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN                         \$
//  {                                                                        \$
//      static int firstTime = 1;                                            \$
//      if (verbose && firstTime) { std::cout <<                             \$
//          "### BSLMA EXCEPTION TEST -- (NOT ENABLED) --" << std::endl;     \$
//          firstTime = 0;                                                   \$
//      }                                                                    \$
//  }
//  #define BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
//  #endif
//
//  // ...
//
//  static
//  bool areEqual(const short *array1, const short *array2, int numElement)
//      // Return 'true' if the specified initial 'numElement' in the specified
//      // 'array1' and 'array2' have the same values, and 'false' otherwise.
//  {
//      for (int i = 0; i < numElement; ++i) {
//          if (array1[i] != array2[i]) return false;
//      }
//      return true;
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
//              BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN {
//                  my_ShortArray mA(&testAllocator);
//                  const my_ShortArray& A = mA;
//                  for (int ei = 0; ei < NUM_ELEM; ++ei) {
//                      mA.append(VALUES[ei]);
//                  }
//                  if (veryVerbose) { P_(ti); P_(NUM_ELEM); P(A); }
//                  LOOP2_ASSERT(LINE, ti, areEqual(EXP, A, NUM_ELEM));
//              } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
//          }
//
//          if (veryVerbose) std::cout << testAllocator << std::endl;
//        } break;
//
//        // ...
//
//      }
//
//      // ...
//  }
//..

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLMA_ALLOCATOR
#include <bslma_allocator.h>
#endif

namespace BloombergLP {

namespace bslma {

                        // ============================
                        // class TestAllocatorException
                        // ============================

class TestAllocatorException {
    // This class defines an exception object for memory allocation operations.
    // Objects of this class contain information about an allocation request.

  public:
    // PUBLIC TYPES
    typedef Allocator::size_type size_type;
        // Alias for the type used by the 'Allocator' protocol to request a
        // memory allocation of a given size.

  private:
    // DATA
    size_type d_numBytes;  // number of bytes requested in an allocation
                           // operation

  public:
    // CREATORS
    TestAllocatorException(size_type numBytes);
        // Create an exception object initialized with the specified 'numBytes'
        // that indicates an allocation request size.

    //! ~TestAllocatorException();
        // Destroy this object.  Note that this method's definition is compiler
        // generated.

    // ACCESSORS
    size_type numBytes() const;
        // Return the number of bytes (supplied at construction) that indicates
        // an allocation request size.
};

// ============================================================================
//                      INLINE FUNCTION DEFINITIONS
// ============================================================================

                        // ----------------------------
                        // class TestAllocatorException
                        // ----------------------------

// CREATORS
inline
TestAllocatorException::TestAllocatorException(size_type numBytes)
: d_numBytes(numBytes)
{
}

// ACCESSORS
inline
TestAllocatorException::size_type
TestAllocatorException::numBytes() const
{
    return d_numBytes;
}

}  // close package namespace


}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// Copyright 2013 Bloomberg Finance L.P.
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
