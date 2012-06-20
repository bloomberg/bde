// baesu_testallocator.h                                              -*-C++-*-
#ifndef INCLUDED_BAESU_TESTALLOCATOR
#define INCLUDED_oBAESU_TESTALLOCATOR

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide an allocator with leak detection coupled with stack trace
//
//@CLASSES:
//   baesu_TestAllocator: support allocation with lead detection / stack trace
//
//@AUTHOR: Bill Chapman (bchapman2)
//
//@DESCRIPTION: This component provides an allocator, 'baesu_TestAllocator'
// and implements the 'bslma_Allocator' protocol and bypasses global operators
// 'new' and 'delete', instead calling the 'C' library functions 'malloc' and
// 'free' directly.  If exceptions are enabled, this allocator can be
// configured to throw an exception after the number of allocation requests
// exceeds some specified limit.  This allocator also maintains a count of the
// number of outstanding blocks (and bytes) that are currently in use, the
// cumulative number of blocks (and bytes) that have been allocated, and the
// maximum number of blocks (and bytes) that have been in use at any one time.
// An overloaded 'operator<<' formats these values to its 'ostream'
// (right-hand-side argument) stream.
//..
//   ,-------------------.
//  ( bcema_TestAllocator )
//   `-------------------'
//             |         ctor/dtor
//             |         numBlocksInUse/numBytesInUse
//             |         numBlocksMax/numBytesMax
//             |         numBlocksTotal/numBytesTotal
//             |         numMismatches/numBoundsErrors
//             |         setAllocationLimit/allocationLimit
//             |         setQuiet/isQuiet
//             |         setVerbose/isVerbose
//             V
//     ,---------------.
//    ( bslma_Allocator )
//     `---------------'
//                     allocate
//                     deallocate
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
// The 'bcema_TestAllocator' object defined in this component can be used to
// test other objects that take a 'bslma_Allocator' (e.g., at construction).
// The following code example illustrates how to verify that an object under
// test (e.g., 'my_ShortArray') is exception neutral:
//..
//  // my_shortarray.t.cpp
//  #include <my_shortarray.h>
//  #include <bcema_testallocator.h>
//  #include <bslma_testallocator.h>
//  #include <bslma_testallocatorexception.h>
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
//      bcema_TestAllocator testAllocator(veryVeryVerbose);
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
//              BEGIN_BSLMA_EXCEPTION_TEST {
//                  my_ShortArray mA(&testAllocator);
//                  const my_ShortArray& A = mA;
//                  for (int ei = 0; ei < NUM_ELEM; ++ei) {
//                      mA.append(VALUES[ei]);
//                  }
//                  if (veryVerbose) { P_(ti); P_(NUM_ELEM); P(A); }
//                  LOOP2_ASSERT(LINE, ti, areEqual(EXP, A, NUM_ELEM));
//              } END_BSLMA_EXCEPTION_TEST
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

#ifndef INCLUDED_BCESCM_VERSION
#include <bcescm_version.h>
#endif

#ifndef INCLUDED_BCEMT_MUTEX
#include <bcemt_mutex.h>
#endif

#ifndef INCLUDED_BSLMA_ALLOCATOR
#include <bslma_allocator.h>
#endif

#ifndef INCLUDED_BSL_HASH_SET
#include <bsl_hash_set.h>
#endif

#ifndef INCLUDED_BSL_IOSTREAM
#include <bsl_iostream.h>
#endif

namespace BloombergLP {

                        // =========================
                        // class bcema_TestAllocator
                        // =========================

class baesu_TestAllocator_List;

class baesu_TestAllocator : public bslma::Allocator {
    // PRIVATE TYPES
    struct VoidStarHash {
        // CLASS METHODS
        static inline
        bsl::size_t operator()(const void *x);
            // Hash function for pointer.  At the time of this writing, the
            // hash function for pointers wasn't very good.
    };
    
    typedef bsl::hash_set<void *, VoidStarHash> AllocatedSegmentHT;

    // DATA
    bcemt_Mutex               d_mutex;          // mutex used to synchronize
                                                // access to this object

    const char               *d_name;           // name of this allocator
                                                // (held, not owned)

    int                       d_rawTraceFrames; // total number of frames
                                                // traced, including ignored
                                                // frames

    int                       d_segFrames;      // max number of stack trace
						// frames to trace

    bsl::ostream             *d_ostream;        // stream to which error
						// reports are to be written.
						// Held, not owned.

    bool                      d_demangle;       // is demangling requested?

    bool                      d_noAbortFlag;    // do we abort after delivering
                                                // a report on error?

    AllocatedSegmentHT        d_allocatedSegments;  // hash table of
                                                    // allocated segments not
                                                    // deallocated yet

    void                    **d_rawTraceBuffer; // buffer for doing stack
                                                // traces

    bslma::Allocator         *d_allocator_p;    // held, not owned

  private:
    // NOT IMPLEMENTED
    baesu_TestAllocator(const bcema_TestAllocator&);
    baesu_TestAllocator& operator=(const bcema_TestAllocator&);

  public:
    // CREATORS
    explicit
    baesu_TestAllocator(int              segFrames               = 8,
                        bsl::ostream    *ostream                 = &bsl::cout,
			bool             demanglingPreferredFlag = true,
                        bslma_Allocator *basicAllocator          = 0);
        // Create a test allocator.  Optionally specify a 'basicAllocator' used
        // to supply memory.  If 'basicAllocator' is 0, the
        // 'bslma_MallocFreeAllocator' singleton is used.

    explicit
    baesu_TestAllocator(const char      *name,
                        int              segFrames               = 8,
                        bsl::ostream    *ostream                 = &bsl::cout,
			bool             demanglingPreferredFlag = true,
                        bslma_Allocator *basicAllocator          = 0);
        // Create a test allocator having the specified 'name'.  The 'name' is
        // included in messages written to 'stdout' to distinguish this test
        // allocator from other test allocators that may be used in the same
        // program.  Optionally specify a 'basicAllocator' used to supply
        // memory.  If 'basicAllocator' is 0, the 'bslma_MallocFreeAllocator'
        // singleton is used.  The behavior is undefined unless the lifetime of
        // 'name' exceeds that of this test allocator.

    ~baesu_TestAllocator();
        // Destroy this allocator.  In verbose mode, print all contained state
        // values of this allocator object to 'stdout'.  Except in quiet mode,
        // automatically report any memory leaks or mismatched deallocations to
        // 'stdout'.  Abort if either 'numBlocksInUse' or 'numBytesInUse'
        // return non-zero unless in no-abort mode or quiet mode.  Note that,
        // in all cases, destroying this object has no effect on outstanding
        // memory blocks allocated from this test allocator (and may result in
        // memory leaks -- e.g., if the (default) 'bslma_MallocFreeAllocator'
        // singleton was used).

    // MANIPULATORS
    void *allocate(size_type size);
        // Return a newly allocated block of memory of the specified positive
        // 'size' (in bytes).  If 'size' is 0, a null pointer is returned with
        // no other other effect (e.g., on allocation/deallocation statistics).
        // Otherwise, invoke the 'allocate' method of the allocator supplied at
        // construction, increment the number of currently (and cumulatively)
        // allocated blocks and increase the number of currently allocated
        // bytes by 'size'.  Update all other fields accordingly.

    void deallocate(void *address);
        // Return the memory block at the specified 'address' back to this
        // allocator.  If 'address' is 0, this function has no effect (e.g.,
        // on allocation/deallocation statistics).  Otherwise, if the memory at
        // 'address' is consistent with being allocated from this test
        // allocator, decrement the number of currently allocated blocks,
        // and decrease the number of currently allocated bytes by the size
        // (in bytes) originally requested for this block.  Although
        // technically undefined behavior, if the memory can be determined
        // not to have been allocated from this test allocator, increment the
        // number of mismatches, and -- unless in quiet mode -- immediately
        // report the details of the mismatch to 'stdout' (e.g., as an
        // 'bsl::hex' memory dump) and abort.

    void setNoAbort(bool flagValue);
        // Set the no-abort mode for this test allcoator to the specified
        // (boolean) 'flagValue'.  'If flagValue' is 'true', aborting on fatal
        // errors is suppressed, and the functions simply return.  Diagnostics
        // are not affected.  Note that this function is provided primarily to
        // enable visual testing of bizarre error messages in this component.

    // ACCESSORS
    bool isNoAbort() const;
        // Return the value of the 'noAbort' flag;

    void reportSegmentsOutstanding() const;
        // Give stack traces from segments that have been allocated and are
        // not yet freed.
};

//=============================================================================
//                            Inline Function Definition
//=============================================================================

                        // ---------------------------------
                        // baesu_TestAllocator::VoidStarHash
                        // ---------------------------------
                        
// CLASS METHODS
inline
bsl::size_t baesu_TestAllocator::VoidStarHash::operator()(const void *x) {
    BSLMF_ASSERT(MAX_ALIGNMENT <= 0x10000);
    BSLMF_ASSERT(0 == (MAX_ALIGNMENT & (MAX_ALIGNMENT - 1)));      // 1 bit set

    enum {

        // SHIFT_DOWN is the number of zeroes following the one in
        // MAX_ALIGNMENT.  Determine this by counting the bits in
        // 'MAX_ALIGNMENT - 1'.

	BITS_TO_COUNT = MAX_ALIGNMENT - 1,
        A = BITS_TO_COUNT - ((BITS_TO_COUNT >> 1) & 0x5555),
        B =          ((A >> 2)      & 0x3333) + (A & 0x3333),
        C =          ((B >> 4) + B) & 0x0f0f,
        SHIFT_DOWN = ((C >> 8) + C) &   0xff,

        SHIFT_UP = SHIFT_DOWN ? sizeof(x) * 8 - SHIFT_DOWN
                              : 0
    };

    BSLMF_ASSERT(1 == (MAX_ALIGNMENT >> SHIFT_DOWN));

    bsls::Types::UintPtr y = (bsls::Types::UintPtr) x;
    return ((y >> SHIFT_DOWN) | (y << SHIFT_UP)) + y;
}

                                // -------------------
                                // baesu_TestAllocator
                                // -------------------

// MANIPULATORS
inline
void setNoAbort(bool flagValue)
{
    d_noAbort = flagValue;
}

// ACCESSORS
inline
bool isNoAbort() const
{
    return d_noAbort;
}

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
