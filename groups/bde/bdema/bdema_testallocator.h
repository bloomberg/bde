// bdema_testallocator.h   -*-C++-*-
#ifndef INCLUDED_BDEMA_TESTALLOCATOR
#define INCLUDED_BDEMA_TESTALLOCATOR

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide concrete test allocator that maintains blocks and bytes.
//
//@DEPRECATED: Use 'bslma_testallocator' instead.
//
//@CLASSES:
//   bdema_TestAllocator: support new/delete style allocation with malloc/free
//
//@AUTHOR: John Lakos (jlakos)
//
//@DESCRIPTION: This component implements the 'bdema_Allocator' protocol that
// bypasses global operators 'new' and 'delete', instead calling the 'C'
// library functions 'malloc' and 'free' directly.  If exceptions are enabled,
// this allocator can be configured to throw an exception after the number of
// allocation requests exceeds some specified limit.  This allocator also
// maintains a count of the number of outstanding blocks (and bytes) that are
// currently in use, the cumulative number of blocks (and bytes) that have been
// allocated, and the maximum number of blocks (and bytes) that have been in
// use at any one time.  An print() function formats these values to stdout.
//..
//                    ( bdema_TestAllocator )
//                               |          ctor/dtor
//                               |          numBlocksInUse/numBytesInUse
//                               |          numBlocksMax/numBytesMax
//                               |          numBlocksTotal/numBytesTotal
//                               |          numMismatches/status
//                               |          setQuiet/isQuiet
//                               |          setVerbose/isVerbose
//                               |          setAllocationLimit/allocationLimit
//                               V
//                      ( bdema_Allocator )
//                                          allocate
//                                          deallocate
//..
// Note that allocation using this component is deliberately incompatible with
// the default global 'new' ('malloc') and 'delete' ('free').  Using 'delete'
// to free memory supplied by this allocator will corrupt the dynamic memory
// manager and also cause a memory leak (and will be reported by purify as
// freeing mismatched memory, freeing unallocated memory, and as a memory
// leak).  Using 'deallocate' to free memory supplied by global 'new' will
// almost always abort immediately (unless quiet mode is set for the purpose of
// testing this component itself).
//
///MODES
///-----
// The test allocator's behavior is controlled by three basic *mode* flags:
//
// VERBOSE MODE: (Default 0) Specifies that each allocation and deallocation
// should be printed to standard output.  In verbose mode all state variables
// will be displayed at destruction.
//
// QUIET MODE: (Default 0) Specifies that mismatched memory and memory leaks
// should *not* be reported, and should not cause the process to terminate when
// detected.  Note that this mode is used primarily for testing the test
// allocator itself; behavior that would otherwise abort now quietly increments
// the 'numMismatches' counter.
//
// NO-ABORT MODE: (Default 0) Specifies that all abort statements are replaced
// by return statements without suppressing diagnostics.  Although the internal
// state values are independent, Quiet Mode implies the behavior of No-Abort
// mode in all cases.  Note that this mode is used primarily for visual
// inspection of unusual error diagnostics in this component's test drivers (in
// verbose mode only).
//
// Taking the default mode settings, memory allocation/deallocation will not be
// displayed individually, but in the event of a mismatched deallocation or a
// memory leak, the problem will be announced, any relevant state of the object
// will be displayed, and the program will abort.
//
///ALLOCATION LIMIT
///----------------
// If exceptions are enabled at compile time, the test allocator can be
// configured to throw an exception after a specified number of allocation
// requests is exceeded.  If the allocation limit is less than zero (default),
// then the allocator never throws an exception.  Note that a non-negative
// allocation limit is decremented after each allocation attempt, and throws
// only when the current allocation limit transitions from 0 to -1; no
// additional exceptions will be thrown until the allocation limit is again
// reset to a non-negative value.
//
///USAGE
///-----
// The 'bdema_TestAllocator' object defined in this component can be used to
// test other objects that take a 'bdema_Allocator' (e.g., at construction).
// The following code example illustrates how to verify that an object under
// test (e.g., 'my_ShortArray') is exception neutral.
//
// Note that "\$" must be replaced by "\" in the preprocessor macro definitions
// that follow.  The "$" symbols are present in this header file to avoid a
// diagnostic elicited by some compilers (e.g., "warning: multi-line comment").
//..
//  // my_shortarray.t.cpp
//  #include <my_shortarray.h>
//  #include <bdema_testallocator.h>
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
//      bdema_TestAllocator testAllocator(veryVeryVerbose);
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
//          const int NUM_DATA = sizeof DATA / sizeof *DATA;
//
//          for (int ti = 0; ti < NUM_DATA; ++ti) {
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
// Note that the 'BDE_BUILD_TARGET_EXC' macro is defined at compile-time to
// indicate that exceptions are enabled.

#ifndef INCLUDED_BDESCM_VERSION
#include <bdescm_version.h>
#endif

#ifndef INCLUDED_BSLMA_TESTALLOCATOR
#include <bslma_testallocator.h>
#endif

namespace BloombergLP {

typedef bslma_TestAllocator bdema_TestAllocator;
    // This class provides an allocator that DOES NOT USE global operators
    // 'new' or 'delete' at all, but instead uses 'malloc' and 'free' directly.
    // This allocator object also tracks the number of blocks (and bytes) that
    // are currently in use from this allocator, the cumulative number of
    // blocks (and bytes) that have be allocated by this allocator, and the
    // maximum number of blocks (and bytes) that have ever been in use from
    // this allocator at any one time.  Note that the number of bytes in use is
    // defined to be the number requested (e.g., if 5 bytes are requested and,
    // for whatever reason, 8 are returned, only 5 bytes are considered to be
    // in use).

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
