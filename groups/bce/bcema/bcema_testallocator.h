// bcema_testallocator.h                                              -*-C++-*-
#ifndef INCLUDED_BCEMA_TESTALLOCATOR
#define INCLUDED_BCEMA_TESTALLOCATOR

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide concrete test allocator that maintains blocks and bytes.
//
//@CLASSES:
//   bcema_TestAllocator: support new/delete style allocation with malloc/free
//
//@SEE_ALSO bslma_testallocator
//
//@AUTHOR: John Lakos (jlakos)
//
//@DESCRIPTION: This component provides an allocator, 'bcema_TestAllocator'
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

#ifndef INCLUDED_BCEMT_LOCKGUARD
#include <bcemt_lockguard.h>
#endif

#ifndef INCLUDED_BCEMT_THREAD
#include <bcemt_thread.h>
#endif

#ifndef INCLUDED_BSLMA_TESTALLOCATOR
#include <bslma_testallocator.h>
#endif

#ifndef INCLUDED_BSLMA_ALLOCATOR
#include <bslma_allocator.h>
#endif

#ifndef INCLUDED_BSLS_PLATFORMUTIL
#include <bsls_platformutil.h>
#endif

#ifndef INCLUDED_BSL_IOSFWD
#include <bsl_iosfwd.h>
#endif


namespace BloombergLP {

                        // =========================
                        // class bcema_TestAllocator
                        // =========================

class bcema_TestAllocator : public bslma_Allocator {
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

    // DATA
    bslma_TestAllocator d_imp;    // thread unsafe test allocator

    mutable bcemt_Mutex d_mutex;  // mutex used to synchronize access to this
                                  // object

    // FRIENDS
    friend bsl::ostream& operator<<(bsl::ostream&              stream,
                                    const bcema_TestAllocator& rhs);

    // NOT IMPLEMENTED
    bcema_TestAllocator(const bcema_TestAllocator&);
    bcema_TestAllocator& operator=(const bcema_TestAllocator&);

  public:
    // CREATORS
    explicit
    bcema_TestAllocator(bslma_Allocator *basicAllocator = 0);
        // Create a test allocator.  Optionally specify a 'basicAllocator' used
        // to supply memory.  If 'basicAllocator' is 0, the
        // 'bslma_MallocFreeAllocator' singleton is used.

    explicit
    bcema_TestAllocator(bool verboseFlag, bslma_Allocator *basicAllocator = 0);
        // Create a test allocator with the specified 'verboseFlag' -- i.e.,
        // whether this test allocator should write allocation/deallocation
        // messages to 'stdout'.  Optionally specify a 'basicAllocator' used to
        // supply memory.  If 'basicAllocator' is 0, the
        // 'bslma_MallocFreeAllocator' singleton is used.

    bcema_TestAllocator(const char      *name,
                        bool             verboseFlag,
                        bslma_Allocator *basicAllocator = 0);
        // Create a test allocator having the specified 'name' with the
        // specified 'verboseFlag' -- i.e., whether this test allocator should
        // write allocation/deallocation messages to 'stdout'.  The 'name' is
        // included in messages written to 'stdout' to distinguish this test
        // allocator from other test allocators that may be used in the same
        // program.  Optionally specify a 'basicAllocator' used to supply
        // memory.  If 'basicAllocator' is 0, the 'bslma_MallocFreeAllocator'
        // singleton is used.  The behavior is undefined unless the lifetime of
        // 'name' exceeds that of this test allocator.

    ~bcema_TestAllocator();
        // Destroy this allocator.  Note that the behavior of destroying an
        // allocator while memory is allocated from it is not specified.
        // (Unless you *know* that it is valid to do so, don't!)
        //
        // For this concrete object, destroying this allocator object has no
        // effect on allocated memory (i.e., memory leaks will be detected by
        // purify).  In verbose mode, print all contained state values of this
        // allocator object to 'stdout'.  Except in quiet mode, report any
        // memory leaks or mismatched deallocations to 'stdout'.  Abort if not
        // in quiet mode unless both 'numBytesInUse()' or 'numBlocksInUse()' is
        // not 0.

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

    void setQuiet(bool flagValue);
        // Set the quiet mode for this test allocator to the specified
        // (boolean) 'flagValue'.  If 'flagValue' is 'true', mismatched
        // allocation, overrun/underrun errors, and memory leak messages will
        // not be displayed to 'stdout' and the process will not abort as a
        // result of such conditions.  Note that the default mode is *not*
        // quiet.  Also note that this function is provided primarily to enable
        // testing of this component; in quiet mode, situations that would
        // otherwise abort will just quietly increment the 'numMismatches'
        // and/or 'numBoundsErrors' counters.

    void setVerbose(bool flagValue);
        // Set the verbose mode for this test allocator to the specified
        // (boolean) 'flagValue'.  If 'flagValue' is 'true', all
        // allocation/deallocation events will be reported automatically on
        // 'stdout', as will accumulated statistics upon destruction of this
        // object.  Note that the default mode is *not* verbose.

    void setAllocationLimit(bsls_PlatformUtil::Int64 limit);
        // Set the number of valid allocation requests before an exception is
        // to be thrown to the specified 'limit'.  If 'limit' is less than 0,
        // no exception is to be thrown.  By default, no exception is
        // scheduled.

    // ACCESSORS
    bool isNoAbort() const;
        // Return 'true' if this allocator is currently in no-abort mode, and
        // 'false' otherwise.  In no-abort mode all diagnostic messages are
        // printed, but all aborts are replaced by return statements.  Note
        // that quiet mode implies no-abort mode.

    bool isQuiet() const;
        // Return 'true' if this allocator is currently in quiet mode, and
        // 'false' otherwise.  In quiet mode, messages about mismatched
        // deallocations, overrun/underrun errors, and memory leaks will be not
        // displayed to 'stdout' and will not cause the program to abort.

    bool isVerbose() const;
        // Return 'true' if this allocator is currently in verbose mode, and
        // 'false' otherwise.  In verbose mode, all allocation/deallocation
        // events will be reported on 'stdout', as will summary statistics
        // upon destruction of this object.

    bsls_PlatformUtil::Int64 numBytesInUse() const;
        // Return the number of bytes currently allocated from this object.
        // Note that 'numBytesInUse() <= numBytesMax()'.

    bsls_PlatformUtil::Int64 numBlocksInUse() const;
        // Return the number of blocks currently allocated from this object.
        // Note that 'numBlocksInUse() <= numBlocksMax()'.

    bsls_PlatformUtil::Int64 numBytesMax() const;
        // Return the maximum number of bytes ever allocated from this object
        // at any one time.  Note that
        // 'numBytesInUse() <= numBytesMax() <= numBytesTotal()'.

    bsls_PlatformUtil::Int64 numBlocksMax() const;
        // Return the maximum number of blocks ever allocated from this object
        // at any one time.  Note that
        // 'numBlocksInUse() <= numBlocksMax() <= numBlocksTotal()'.

    bsls_PlatformUtil::Int64 numBytesTotal() const;
        // Return the cumulative number of bytes ever allocated from this
        // object.  Note that 'numBytesMax() <= numBytesTotal()'.

    bsls_PlatformUtil::Int64 numBlocksTotal() const;
        // Return the cumulative number of blocks ever allocated from this
        // object.  Note that 'numBlocksMax() <= numBlocksTotal()'.

    bsls_PlatformUtil::Int64 numMismatches() const;
        // Return the number of mismatched memory deallocations that have
        // occurred since this object was created.  A memory deallocation is
        // *mismatched* if that memory was not allocated directly from this
        // allocator.

    bsls_PlatformUtil::Int64 numBoundsErrors() const;
        // Return the number of times memory deallocations have detected that
        // pad areas at the front or back of the user segment had been
        // overwritten.

    int status() const;
        // Return 0 on success, and non-zero otherwise.  If there have been any
        // mismatched memory deallocations or over/under runs, return the
        // number of such errors that have occurred as a positive number, if
        // the number of blocks and bytes are not both 0, return an arbitrary
        // negative number; else return 0.  Note that this function is used to
        // define the criteria for an abort at destruction if quiet mode has
        // not been set.

    bsls_PlatformUtil::Int64 allocationLimit() const;
        // Return the current number of allocation requests left before an
        // exception is thrown.  A negative value indicated that no exception
        // is scheduled.

    size_type lastAllocatedNumBytes() const;
        // Return the number of bytes of the most recent memory request.  Note
        // that this number is always recorded regardless of the validity of
        // the request.

    size_type lastDeallocatedNumBytes() const;
        // Return the number of bytes of the most recent memory deallocation
        // request.  Return 0 if the request was invalid (e.g., deallocate
        // memory not allocated through this allocator).

    void *lastAllocatedAddress() const;
        // Return the allocated memory address of the most recent memory
        // request.  Return 0 if the request was invalid (e.g., allocate non-
        // positive number of bytes).

    void *lastDeallocatedAddress() const;
        // Return the memory address of the last memory deallocation request.
        // Note that the address is always recorded regardless of the validity
        // of the request.

    bsls_PlatformUtil::Int64 numAllocations() const;
        // Return the cumulative number of allocation requests.  Note that this
        // number is incremented for every 'allocate' invocation, regardless of
        // the validity of the request.

    bsls_PlatformUtil::Int64 numDeallocations() const;
        // Return the cumulative number of deallocation requests.  Note that
        // this number is incremented for every 'deallocate' invocation,
        // regardless of the validity of the request.

    const char *name() const;
        // Return the name of this test allocator, or 0 if no name was
        // specified at construction.

    void print() const;
        // Write the accumulated state information held in this allocator to
        // 'stdout' in some reasonable (multi-line) format.

// TBD #if !defined(BSL_LEGACY) || 1 == BSL_LEGACY

    void *lastAllocateAddress() const;
        // Return the allocated memory address of the most recent memory
        // request.  Return 0 if the request was invalid (e.g., allocate non-
        // positive number of bytes).
        //
        // DEPRECATED: use 'lastAllocatedAddress' instead.

    size_type lastAllocateNumBytes() const;
        // Return the number of bytes of the most recent memory request.  Note
        // that this number is always recorded regardless of the validity of
        // the request.
        //
        // DEPRECATED: use 'lastAllocatedNumBytes' instead.

    void *lastDeallocateAddress() const;
        // Return the memory address of the last memory deallocation request.
        // Note that the address is always recorded regardless of the validity
        // of the request.
        //
        // DEPRECATED: use 'lastDeallocatedAddress' instead.

    size_type lastDeallocateNumBytes() const;
        // Return the number of bytes of the most recent memory deallocation
        // request.  Return 0 if the request was invalid (e.g., deallocating
        // memory not allocated through this allocator).
        //
        // DEPRECATED: use 'lastDeallocatedNumBytes' instead.

    bsls_PlatformUtil::Int64 numAllocation() const;
        // Return the cumulative number of allocation requests.  Note that this
        // number is incremented for every 'allocate' invocation, regardless of
        // the validity of the request.
        //
        // DEPRECATED: use 'numAllocations' instead.

    bsls_PlatformUtil::Int64 numDeallocation() const;
        // Return the cumulative number of deallocation requests.  Note that
        // this number is incremented for every 'deallocate' invocation,
        // regardless of the validity of the request.
        //
        // DEPRECATED: use 'numDeallocations' instead.

// TBD #endif
};

bsl::ostream& operator<<(bsl::ostream& stream, const bcema_TestAllocator& rhs);
    // Write the accumulated state information held in the specified 'rhs'
    // allocator to the specified 'stream' in some reasonable (multi-line)
    // format, and return a reference to 'stream'.  Note that there is a slight
    // difference between 'operator<<' and the 'print' method.  The 'print'
    // method prints out the indices of the currently allocated memory blocks,
    // while 'operator<<' does not.

// ===========================================================================
//                      INLINE FUNCTION DEFINITIONS
// ===========================================================================

                        // -------------------------
                        // class bcema_TestAllocator
                        // -------------------------

// CREATORS
inline
bcema_TestAllocator::bcema_TestAllocator(bslma_Allocator *basicAllocator)
: d_imp(basicAllocator)
{
}

inline
bcema_TestAllocator::bcema_TestAllocator(bool             verboseFlag,
                                         bslma_Allocator *basicAllocator)
: d_imp(verboseFlag, basicAllocator)
{
}

inline
bcema_TestAllocator::bcema_TestAllocator(const char      *name,
                                         bool             verboseFlag,
                                         bslma_Allocator *basicAllocator)
: d_imp(name, verboseFlag, basicAllocator)
{
}

inline
bcema_TestAllocator::~bcema_TestAllocator()
{
}

// MANIPULATORS
inline
void *bcema_TestAllocator::allocate(size_type size)
{
    bcemt_LockGuard<bcemt_Mutex> guard(&d_mutex);
    return d_imp.allocate(size);
}

inline
void bcema_TestAllocator::deallocate(void *address)
{
    bcemt_LockGuard<bcemt_Mutex> guard(&d_mutex);
    d_imp.deallocate(address);
}

inline
void bcema_TestAllocator::setNoAbort(bool flagValue)
{
    d_imp.setNoAbort(flagValue);
}

inline
void bcema_TestAllocator::setQuiet(bool flagValue)
{
    d_imp.setQuiet(flagValue);
}

inline
void bcema_TestAllocator::setVerbose(bool flagValue)
{
    d_imp.setVerbose(flagValue);
}

inline
void bcema_TestAllocator::setAllocationLimit(bsls_PlatformUtil::Int64 limit)
{
    d_imp.setAllocationLimit(limit);
}

// ACCESSORS
inline
bool bcema_TestAllocator::isNoAbort() const
{
    return d_imp.isNoAbort();
}

inline
bool bcema_TestAllocator::isQuiet() const
{
    return d_imp.isQuiet();
}

inline
bool bcema_TestAllocator::isVerbose() const
{
    return d_imp.isVerbose();
}

inline
bsls_PlatformUtil::Int64 bcema_TestAllocator::numBlocksInUse() const
{
    return d_imp.numBlocksInUse();
}

inline
bsls_PlatformUtil::Int64 bcema_TestAllocator::numBytesInUse() const
{
    return d_imp.numBytesInUse();
}

inline
bsls_PlatformUtil::Int64 bcema_TestAllocator::numBlocksMax() const
{
    return d_imp.numBlocksMax();
}

inline
bsls_PlatformUtil::Int64 bcema_TestAllocator::numBytesMax() const
{
    return d_imp.numBytesMax();
}

inline
bsls_PlatformUtil::Int64 bcema_TestAllocator::numBlocksTotal() const
{
    return d_imp.numBlocksTotal();
}

inline
bsls_PlatformUtil::Int64 bcema_TestAllocator::numBytesTotal() const
{
    return d_imp.numBytesTotal();
}

inline
bsls_PlatformUtil::Int64 bcema_TestAllocator::numMismatches() const
{
    return d_imp.numMismatches();
}

inline
bsls_PlatformUtil::Int64 bcema_TestAllocator::numBoundsErrors() const
{
    return d_imp.numBoundsErrors();
}

inline
int bcema_TestAllocator::status() const
{
    bcemt_LockGuard<bcemt_Mutex> guard(&d_mutex);
    return d_imp.status();
}

inline
bsls_PlatformUtil::Int64 bcema_TestAllocator::allocationLimit() const
{
    return d_imp.allocationLimit();
}

inline
bcema_TestAllocator::size_type
bcema_TestAllocator::lastAllocatedNumBytes() const
{
    return d_imp.lastAllocatedNumBytes();
}

inline
bcema_TestAllocator::size_type
bcema_TestAllocator::lastDeallocatedNumBytes() const
{
    return d_imp.lastDeallocatedNumBytes();
}

inline
void *bcema_TestAllocator::lastAllocatedAddress() const
{
    return d_imp.lastAllocatedAddress();
}

inline
void *bcema_TestAllocator::lastDeallocatedAddress() const
{
    return d_imp.lastDeallocatedAddress();
}

inline
bsls_PlatformUtil::Int64 bcema_TestAllocator::numAllocations() const
{
    return d_imp.numAllocations();
}

inline
bsls_PlatformUtil::Int64 bcema_TestAllocator::numDeallocations() const
{
    return d_imp.numDeallocations();
}

inline
const char *bcema_TestAllocator::name() const
{
    return d_imp.name();
}

inline void bcema_TestAllocator::print() const
{
    bcemt_LockGuard<bcemt_Mutex> guard(&d_mutex);
    d_imp.print();
}

// TBD #if !defined(BSL_LEGACY) || 1 == BSL_LEGACY
inline
void *bcema_TestAllocator::lastAllocateAddress() const
{
    return lastAllocatedAddress();
}

inline
bcema_TestAllocator::size_type
bcema_TestAllocator::lastAllocateNumBytes() const
{
    return lastAllocatedNumBytes();
}

inline
void *bcema_TestAllocator::lastDeallocateAddress() const
{
    return lastDeallocatedAddress();
}

inline
bcema_TestAllocator::size_type
bcema_TestAllocator::lastDeallocateNumBytes() const
{
    return lastDeallocatedNumBytes();
}

inline
bsls_PlatformUtil::Int64 bcema_TestAllocator::numAllocation() const
{
    return numAllocations();
}

inline
bsls_PlatformUtil::Int64 bcema_TestAllocator::numDeallocation() const
{
    return numDeallocations();
}

// TBD #endif

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
