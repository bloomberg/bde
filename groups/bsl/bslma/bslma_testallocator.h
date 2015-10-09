// bslma_testallocator.h                                              -*-C++-*-
#ifndef INCLUDED_BSLMA_TESTALLOCATOR
#define INCLUDED_BSLMA_TESTALLOCATOR

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide instrumented malloc/free allocator to track memory usage.
//
//@CLASSES:
//  bslma::TestAllocator: instrumented 'malloc'/'free' memory allocator
//
//@MACROS:
//  BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN: macro to begin testing exceptions
//  BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END: macro to end testing exceptions
//
//@SEE_ALSO: bslma_newdeleteallocator, bslma_mallocfreeallocator
//
//@DESCRIPTION: This component provides an instrumented allocator,
// 'bslma::TestAllocator', that implements the 'bslma::Allocator' protocol and
// can be used to track various aspects of memory allocated from it.  Available
// statistics include the number of outstanding blocks (and bytes) that are
// currently in use, the cumulative number of blocks (and bytes) that have been
// allocated, and the maximum number of blocks (and bytes) that have been in
// use at any one time.  A 'print' function formats these values to 'stdout':
//..
//   ,--------------------.
//  ( bslma::TestAllocator )
//   `--------------------'
//             |         ctor/dtor
//             |         lastAllocatedAddress/lastDeallocatedAddress
//             |         lastAllocatedNumBytes/lastDeallocatedNumBytes
//             |         numAllocations/numDeallocations
//             |         numBlocksInUse/numBlocksMax/numBlocksTotal
//             |         numBytesInUse/numBytesMax/numBytesTotal
//             |         numMismatches/numBoundsErrors
//             |         print/name
//             |         setAllocationLimit/allocationLimit
//             |         setNoAbort/isNoAbort
//             |         setQuiet/isQuiet
//             |         setVerbose/isVerbose
//             |         status
//             V
//     ,----------------.
//    ( bslma::Allocator )
//     `----------------'
//                       allocate
//                       deallocate
//..
// If exceptions are enabled, this allocator can be configured to throw an
// exception after the number of allocation requests exceeds some specified
// limit (see the subsection on "Allocation Limit" below).  The level of
// verbosity can also be adjusted.  Each allocator object also maintains a
// current status.
//
// By default this allocator gets its memory from the C Standard Library
// functions 'malloc' and 'free', but can be overridden to take memory from any
// thread-safe allocator (supplied at construction) that implements the
// 'bslma::Allocator' protocol.  Note that allocation and deallocation using a
// 'bslma::TestAllocator' object is explicitly incompatible with 'malloc' and
// 'free' (or any other allocation mechanism).  Attempting to use 'free' to
// deallocate memory allocated from a 'bslma::TestAllocator' -- even when
// 'malloc' and 'free' are used by default -- will result in undefined
// behavior, almost certainly corrupting the C Standard Library's runtime
// memory manager.
//
// Memory dispensed from a 'bslma::TestAllocator' is marked such that
// attempting to deallocate previously unallocated (or already deallocated)
// memory will (with high probability) be flagged as an error (unless quiet
// mode is set for the purpose of testing the test allocator itself).  A
// 'bslma::TestAllocator' also supports a buffer overrun / underrun feature --
// each allocation has "pads", areas of extra memory before and after the
// segment which are initialized to a particular value and checked upon
// deallocation to see if they have been modified.  If they have, a message is
// printed and the allocator aborts, unless it is in quiet mode.
//
///Modes
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
// the 'numMismatches' and 'numBoundsErrors' counter.
//
// NO-ABORT MODE: (Default 0) Specifies that the test allocator should not
// invoke 'abort' under any circumstances without suppressing diagnostics.
// Although the internal state values are independent, quiet mode implies the
// behavior of no-abort mode in all cases.  Note that this mode is used
// primarily for visual inspection of unusual error diagnostics in this
// component's test driver (in non-quiet mode only).
//
// Taking the default mode settings, memory allocation/deallocation will not be
// displayed individually.  However, in the event of a mismatched deallocation
// or a memory leak, the problem will be announced, any relevant state of the
// object will be displayed, and the program will abort.
//
// The three modes are independently set using the 'setVerbose', 'setQuiet',
// and 'setNoAbort' manipulators.
//
///Allocation Limit
///----------------
// If exceptions are enabled at compile time, the test allocator can be
// configured to throw a 'bslma::TestAllocatorException' after a specified
// number of allocation requests is exceeded.  If the allocation limit is less
// than 0 (default), then the allocator won't throw a 'TestAllocatorException'
// exception.  Note that a non-negative allocation limit is decremented after
// each allocation attempt, and an exception is thrown only when the current
// allocation limit transitions from 0 to -1; no additional exceptions will be
// thrown until the allocation limit is again reset to a non-negative value.
//
// The allocation limit is set using the 'setAllocationLimit' manipulator.
//
///Exception Test Macros
///---------------------
// This component also provides a pair of macros:
//
//: o 'BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(BSLMA_TESTALLOCATOR)'
//: o 'BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END'
//
// These macros can be used for testing exception-safety of classes and their
// methods when memory allocation is needed.  A reference to an object of type
// 'bslma::TestAllocator' must be supplied as an argument to the '_BEGIN'
// macro.  Note that if exception-handling is disabled (i.e., if
// 'BDE_BUILD_TARGET_EXC' is not defined when building the code under test),
// then the macros simply print the following:
//..
//  BSLMA EXCEPTION TEST -- (NOT ENABLED) --
//..
// When exception-handling is enabled, the '_BEGIN' macro will set the
// allocation limit of the supplied allocator to 0, 'try' the code being
// tested, 'catch' any 'TestAllocatorException's that are thrown, and keep
// increasing the allocation limit until the code being tested completes
// successfully.
//
///Thread Safety
///-------------
// The 'bslma::TestAllocator' class is fully thread-safe (see
// 'bsldoc_glossary') provided that the allocator supplied at construction (if
// any) is fully thread-safe.  Note that the 'bslma::MallocFreeAllocator'
// singleton (the allocator used by the test allocator if none is supplied at
// construction) is fully thread-safe.
//
///Usage
///-----
// The 'bslma::TestAllocator' defined in this component can be used in
// conjunction with the 'BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN' and
// 'BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END' macros to test the memory usage
// patterns of an object that uses the 'bslma::Allocator' protocol in its
// interface.  In this example, we illustrate how we might test that an object
// under test is exception-neutral.  For illustration purposes, we will assume
// the existence of a 'my_shortarray' component implementing an
// 'std::vector'-like array type, 'myShortArray':
//..
//  // my_shortarray.t.cpp
//  #include <my_shortarray.h>
//
//  #include <bslma_testallocator.h>
//  #include <bslma_testallocatorexception.h>
//
//  // ...
//
//..
// Below we provide a 'static' function, 'areEqual', that will allow us to
// compare two short arrays:
//..
//  static
//  bool areEqual(const short *array1, const short *array2, int numElements)
//      // Return 'true' if the specified initial 'numElements' in the
//      // specified 'array1' and 'array2' have the same values, and 'false'
//      // otherwise.
//  {
//      for (int i = 0; i < numElements; ++i) {
//          if (array1[i] != array2[i]) {
//              return false;                                         // RETURN
//          }
//      }
//      return true;
//  }
//
//  // ...
//
//..
// The following is an abbreviated standard test driver.  Note that the number
// of arguments specify the verbosity level that the test driver uses for
// printing messages:
//..
//  int main(int argc, char *argv[])
//  {
//      int                 test = argc > 1 ? atoi(argv[1]) : 0;
//      bool             verbose = argc > 2;
//      bool         veryVerbose = argc > 3;
//      bool     veryVeryVerbose = argc > 4;
//      bool veryVeryVeryVerbose = argc > 5;
//
//..
// We now define a 'bslma::TestAllocator', 'sa', named "supplied" to indicate
// that it is the allocator to be supplied to our object under test, as well as
// to the 'BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN' macro (below).  Note that
// if 'veryVeryVeryVerbose' is 'true', then 'sa' prints all allocation and
// deallocation requests to 'stdout' and also prints the accumulated statistics
// on destruction:
//..
//  bslma::TestAllocator sa("supplied", veryVeryVeryVerbose);
//
//  switch (test) { case 0:
//
//    // ...
//
//    case 6: {
//
//      // ...
//
//      struct {
//          int   d_line;
//          int   d_numElem;
//          short d_exp[NUM_VALUES];
//      } DATA[] = {
//          { L_, 0, { } },
//          { L_, 1, { V0 } },
//          { L_, 5, { V0, V1, V2, V3, V4 } }
//      };
//      const int NUM_DATA = sizeof DATA / sizeof *DATA;
//
//      for (int ti = 0; ti < NUM_DATA; ++ti) {
//          const int    LINE     = DATA[ti].d_line;
//          const int    NUM_ELEM = DATA[ti].d_numElem;
//          const short *EXP      = DATA[ti].d_exp;
//
//          if (veryVerbose) { T_ P_(ti) P_(NUM_ELEM) }
//
//          // ...
//
//..
// All code that we want to test for exception-safety must be enclosed within
// the 'BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN' and
// 'BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END' macros, which internally implement
// a 'do'-'while' loop.  Code provided by the
// 'BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN' macro sets the allocation limit
// of the supplied allocator to 0 causing it to throw an exception on the first
// allocation.  This exception is caught by code provided by the
// 'BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END' macro, which increments the
// allocation limit by 1 and re-runs the same code again.  Using this scheme we
// can check that our code does not leak memory for any memory allocation
// request.  Note that the curly braces surrounding these macros, although
// visually appealing, are not technically required:
//..
//      BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(sa) {
//        my_ShortArray mA(&sa);
//        const my_ShortArray& A = mA;
//        for (int ei = 0; ei < NUM_ELEM; ++ei) {
//            mA.append(VALUES[ei]);
//        }
//        if (veryVerbose) { T_ T_  P_(NUM_ELEM) P(A) }
//        LOOP_ASSERT(LINE, areEqual(EXP, A, NUM_ELEM));
//      } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
//  }
//..
// After the exception-safety test we can ensure that all the memory allocated
// from 'sa' was successfully deallocated:
//..
//          if (veryVerbose) sa.print();
//
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
// indicate whether or not exceptions are enabled.

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLMA_ALLOCATOR
#include <bslma_allocator.h>
#endif

#ifndef INCLUDED_BSLMA_TESTALLOCATOREXCEPTION
#include <bslma_testallocatorexception.h>
#endif

#ifndef INCLUDED_BSLS_ATOMIC
#include <bsls_atomic.h>
#endif

#ifndef INCLUDED_BSLS_BSLLOCK
#include <bsls_bsllock.h>
#endif

#ifndef INCLUDED_BSLS_TYPES
#include <bsls_types.h>
#endif

#ifndef INCLUDED_CSTDIO
#include <cstdio>     // for printing in macros
#define INCLUDED_CSTDIO
#endif

namespace BloombergLP {
namespace bslma {

struct TestAllocator_List;

                             // ===================
                             // class TestAllocator
                             // ===================

class TestAllocator : public Allocator {
    // This class defines a concrete "test" allocator mechanism that implements
    // the 'Allocator' protocol, and provides instrumentation to track (1) the
    // number of blocks/bytes currently in use, (2) the maximum number of
    // blocks/bytes that have been outstanding at any one time, and (3) the
    // cumulative number of blocks/bytes that have ever been allocated by this
    // test allocator object.  The accumulated statistics are based solely on
    // the number of bytes requested.  Additional testing facilities include
    // allocation limits, verbosity modes, status, and automated report
    // printing.
    //
    // Note that, unlike many other allocators, this allocator does NOT rely on
    // the currently installed default allocator (see 'bslma_default'), but
    // instead -- by default -- uses the 'MallocFreeAllocator' singleton, which
    // in turn calls the C Standard Library functions 'malloc' and 'free' as
    // needed.  Clients may, however, override this allocator by supplying (at
    // construction) any other allocator implementing the 'Allocator' protocol
    // provided that it is fully thread-safe.

    // DATA

                        // Control Points

    const char *d_name_p;                // optionally specified name of
                                         // this test allocator object (or 0)

    bsls::AtomicInt
                d_noAbortFlag;           // whether or not to suppress
                                         // aborting on fatal errors

    bsls::AtomicInt
                d_quietFlag;             // whether or not to suppress
                                         // reporting hard errors

    bsls::AtomicInt
                d_verboseFlag;           // whether or not to report
                                         // allocation/deallocation events and
                                         // print statistics on destruction

    bsls::AtomicInt64
                d_allocationLimit;       // number of allocations before
                                         // exception is thrown by this object

                        // Statistics

    bsls::AtomicInt64
                d_numAllocations;        // total number of allocation
                                         // requests on this object (including
                                         // those for 0 bytes)

    bsls::AtomicInt64
                d_numDeallocations;      // total number of deallocation
                                         // requests on this object (including
                                         // those supplying a 0 address)

    bsls::AtomicInt64
                d_numMismatches;         // number of mismatched memory
                                         // deallocation errors encountered by
                                         // this object
    bsls::AtomicInt64
                d_numBoundsErrors;       // number of overrun/underrun errors
                                         // encountered by this object

    bsls::AtomicInt64
                d_numBlocksInUse;        // number of blocks currently
                                         // allocated from this object

    bsls::AtomicInt64
                d_numBytesInUse;         // number of bytes currently
                                         // allocated from this object

    bsls::AtomicInt64
                d_numBlocksMax;          // maximum number of blocks ever
                                         // allocated from this object at any
                                         // one time

    bsls::AtomicInt64
                d_numBytesMax;           // maximum number of bytes ever
                                         // allocated from this object at any
                                         // one time

    bsls::AtomicInt64
                d_numBlocksTotal;        // cumulative number of blocks ever
                                         // allocated from this object

    bsls::AtomicInt64
                d_numBytesTotal;         // cumulative number of bytes ever
                                         // allocated from this object

                        // Other Data

    bsls::AtomicInt64
                d_lastAllocatedNumBytes; // size (in bytes) of the most recent
                                         // allocation request

    bsls::AtomicInt64
                d_lastDeallocatedNumBytes;
                                         // size (in bytes) of the most
                                         // recently deallocated memory

    bsls::AtomicPointer<int>
                d_lastAllocatedAddress_p;// address of the most recently
                                         // allocated memory (or 0)

    bsls::AtomicPointer<int>
                d_lastDeallocatedAddress_p;
                                         // address of the most recently
                                         // deallocated memory (or 0)

    TestAllocator_List
               *d_list_p;                // list of allocated memory (owned)

    mutable bsls::BslLock
                d_lock;                  // ensure mutual exclusion in
                                         // 'allocate', 'deallocate', 'print',
                                         // and 'status'

    Allocator  *d_allocator_p;           // memory allocator (held, not owned)

  private:
    // NOT IMPLEMENTED
    TestAllocator(const TestAllocator&);             // = delete
    TestAllocator& operator=(const TestAllocator&);  // = delete

  public:
    // CREATORS
    explicit
    TestAllocator(Allocator  *basicAllocator = 0);
    explicit
    TestAllocator(const char *name,
                  Allocator  *basicAllocator = 0);
    explicit
    TestAllocator(bool        verboseFlag,
                  Allocator  *basicAllocator = 0);
    TestAllocator(const char *name,
                  bool        verboseFlag,
                  Allocator  *basicAllocator = 0);
        // Create an instrumented "test" allocator.  Optionally specify a
        // 'name' (associated with this object) to be included in diagnostic
        // messages written to 'stdout', thereby distinguishing this test
        // allocator from others that might be used in the same program.  If
        // 'name' is 0 (or not specified), no distinguishing name is
        // incorporated in diagnostics.  Optionally specify a 'verboseFlag'
        // indicating whether this test allocator should automatically report
        // all allocation/deallocation events to 'stdout' and print accumulated
        // statistics on destruction.  If 'verboseFlag' is 'false' (or not
        // specified), allocation/deallocation and summary messages will not be
        // written automatically.  Optionally specify a 'basicAllocator' used
        // to supply memory.  If 'basicAllocator' is 0, the
        // 'MallocFreeAllocator' singleton is used.

    ~TestAllocator();
        // Destroy this allocator.  In verbose mode, print all contained state
        // values of this allocator object to 'stdout'.  Except in quiet mode,
        // automatically report any memory leaks to 'stdout'.  Abort if either
        // 'numBlocksInUse' or 'numBytesInUse' return non-zero unless in
        // no-abort mode or quiet mode.  Note that, in all cases, destroying
        // this object has no effect on outstanding memory blocks allocated
        // from this test allocator (and may result in memory leaks -- e.g., if
        // the (default) 'MallocFreeAllocator' singleton was used).

    // MANIPULATORS
    void *allocate(size_type size);
        // Return a newly-allocated block of memory of the specified 'size' (in
        // bytes).  If 'size' is 0, a null pointer is returned.  Otherwise,
        // invoke the 'allocate' method of the allocator supplied at
        // construction, increment the number of currently (and cumulatively)
        // allocated blocks, and increase the number of currently allocated
        // bytes by 'size'.  Update all other fields accordingly.

    void deallocate(void *address);
        // Return the memory block at the specified 'address' back to this
        // allocator.  If 'address' is 0, this function has no effect (other
        // than to record relevant statistics).  Otherwise, if the memory at
        // 'address' is consistent with being allocated from this test
        // allocator, decrement the number of currently allocated blocks, and
        // decrease the number of currently allocated bytes by the size (in
        // bytes) originally requested for the block.  Although technically
        // undefined behavior, if the memory can be determined not to have been
        // allocated from this test allocator, increment the number of
        // mismatches, and -- unless in quiet mode -- immediately report the
        // details of the mismatch to 'stdout' (e.g., as an 'std::hex' memory
        // dump) and abort.

    void setAllocationLimit(bsls::Types::Int64 limit);
        // Set the number of valid allocation requests before an exception is
        // to be thrown for this allocator to the specified 'limit'.  If
        // 'limit' is less than 0, no exception is to be thrown.  By default,
        // no exception is scheduled.

    void setNoAbort(bool flagValue);
        // Set the no-abort mode for this test allocator to the specified
        // (boolean) 'flagValue'.  'If flagValue' is 'true', aborting on fatal
        // errors is suppressed, and the functions simply return.  Diagnostics
        // are not affected.  Note that the default mode is to abort.  Also
        // note that this function is provided primarily to enable visual
        // testing of diagnostic messages produced by this component.

    void setQuiet(bool flagValue);
        // Set the quiet mode for this test allocator to the specified
        // (boolean) 'flagValue'.  If 'flagValue' is 'true', mismatched
        // allocations, overrun/underrun errors, and memory leak messages will
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

    // ACCESSORS
    bsls::Types::Int64 allocationLimit() const;
        // Return the current number of allocation requests left before an
        // exception is thrown.  A negative value indicates that no exception
        // is scheduled.

    bool isNoAbort() const;
        // Return 'true' if this allocator is currently in no-abort mode, and
        // 'false' otherwise.  In no-abort mode all diagnostic messages are
        // printed, but all aborts are suppressed.  Note that quiet mode
        // implies no-abort mode.

    bool isQuiet() const;
        // Return 'true' if this allocator is currently in quiet mode, and
        // 'false' otherwise.  In quiet mode, messages about mismatched
        // deallocations, overrun/underrun errors, and memory leaks will not be
        // displayed to 'stdout' and will not cause the program to abort.

    bool isVerbose() const;
        // Return 'true' if this allocator is currently in verbose mode, and
        // 'false' otherwise.  In verbose mode, all allocation/deallocation
        // events will be reported on 'stdout', as will summary statistics upon
        // destruction of this object.

    void *lastAllocatedAddress() const;
        // Return the address that was returned by the most recent allocation
        // request.  Return 0 if the most recent allocation request was for 0
        // bytes.

    size_type lastAllocatedNumBytes() const;
        // Return the number of bytes of the most recent allocation request.

    void *lastDeallocatedAddress() const;
        // Return the address that was supplied to the most recent deallocation
        // request.  Return 0 if a null pointer was most recently deallocated.
        // Note that the address is always recorded regardless of the validity
        // of the request.

    size_type lastDeallocatedNumBytes() const;
        // Return the number of bytes of the most recent deallocation request.
        // Return 0 if a null pointer was most recently deallocated, or if the
        // request was invalid (e.g., an attempt to deallocate memory not
        // allocated through this allocator).

    const char *name() const;
        // Return the name of this test allocator, or 0 if no name was
        // specified at construction.

    bsls::Types::Int64 numAllocations() const;
        // Return the cumulative number of allocation requests.  Note that this
        // number is incremented for every 'allocate' invocation.

    bsls::Types::Int64 numBlocksInUse() const;
        // Return the number of blocks currently allocated from this object.
        // Note that 'numBlocksInUse() <= numBlocksMax()'.

    bsls::Types::Int64 numBlocksMax() const;
        // Return the maximum number of blocks ever allocated from this object
        // at any one time.  Note that
        // 'numBlocksInUse() <= numBlocksMax() <= numBlocksTotal()'.

    bsls::Types::Int64 numBlocksTotal() const;
        // Return the cumulative number of blocks ever allocated from this
        // object.  Note that 'numBlocksMax() <= numBlocksTotal()'.

    bsls::Types::Int64 numBoundsErrors() const;
        // Return the number of times memory deallocations have detected that
        // pad areas at the front or back of the user segment had been
        // overwritten.

    bsls::Types::Int64 numBytesInUse() const;
        // Return the number of bytes currently allocated from this object.
        // Note that 'numBytesInUse() <= numBytesMax()'.

    bsls::Types::Int64 numBytesMax() const;
        // Return the maximum number of bytes ever allocated from this object
        // at any one time.  Note that
        // 'numBytesInUse() <= numBytesMax() <= numBytesTotal()'.

    bsls::Types::Int64 numBytesTotal() const;
        // Return the cumulative number of bytes ever allocated from this
        // object.  Note that 'numBytesMax() <= numBytesTotal()'.

    bsls::Types::Int64 numDeallocations() const;
        // Return the cumulative number of deallocation requests.  Note that
        // this number is incremented for every 'deallocate' invocation,
        // regardless of the validity of the request.

    bsls::Types::Int64 numMismatches() const;
        // Return the number of mismatched memory deallocations that have
        // occurred since this object was created.  A memory deallocation is
        // *mismatched* if that memory was not allocated directly from this
        // allocator.

    void print() const;
        // Write the accumulated state information held in this allocator to
        // 'stdout' in some reasonable (multi-line) format.

    int status() const;
        // Return 0 on success, and non-zero otherwise: If there have been any
        // mismatched memory deallocations or over/under runs, return the
        // number of such errors that have occurred as a positive number; if
        // either '0 < numBlocksInUse()' or '0 < numBytesInUse()', return an
        // arbitrary negative number; else return 0.
};

}  // close package namespace

               // ==============================================
               // macro BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN
               // ==============================================

// The following is a workaround for an intermittent Visual Studio 2005
// exception-handling failure.

#if defined(BSLS_PLATFORM_CMP_MSVC) && BSLS_PLATFORM_CMP_VER_MAJOR < 1500
#define BSLMA_EXCEPTION_TEST_WORKAROUND try {} catch (...) {}
#else
#define BSLMA_EXCEPTION_TEST_WORKAROUND
#endif

#ifdef BDE_BUILD_TARGET_EXC

namespace bslma {

class TestAllocator_ProxyBase {
    // This class provides a common base class for the parameterized
    // 'TestAllocator_Proxy' class (below).  Note that the 'virtual'
    // 'setAllocationLimit' method, although a "setter", *must* be declared
    // 'const'.

  public:
    virtual ~TestAllocator_ProxyBase()
    {
    }

    // ACCESSORS
    virtual void setAllocationLimit(bsls::Types::Int64 limit) const = 0;
};

template <class BSLMA_ALLOC_TYPE>
class TestAllocator_Proxy: public TestAllocator_ProxyBase {
    // This class provides a proxy to the test allocator that is supplied to
    // the 'BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN' macro.  This proxy may be
    // instantiated with 'TestAllocator', or with a type that supports the same
    // interface as 'TestAllocator'.

    // DATA
    BSLMA_ALLOC_TYPE *d_allocator_p;  // allocator used in '*_BEGIN' and
                                      // '*_END' macros (held, not owned)

  public:
    // CREATORS
    TestAllocator_Proxy(BSLMA_ALLOC_TYPE *allocator)
    : d_allocator_p(allocator)
    {
    }

    ~TestAllocator_Proxy()
    {
    }

    // ACCESSORS
    virtual void setAllocationLimit(bsls::Types::Int64 limit) const
    {
        d_allocator_p->setAllocationLimit(limit);
    }
};

template <class BSLMA_ALLOC_TYPE>
inline
TestAllocator_Proxy<BSLMA_ALLOC_TYPE>
TestAllocator_getProxy(BSLMA_ALLOC_TYPE *allocator)
    // Return, by value, a test allocator proxy for the specified parameterized
    // 'allocator'.
{
    return TestAllocator_Proxy<BSLMA_ALLOC_TYPE>(allocator);
}

}  // close package namespace

#ifndef BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN
#define BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(BSLMA_TESTALLOCATOR) {     \
    BSLMA_EXCEPTION_TEST_WORKAROUND                                         \
    {                                                                       \
        static int firstTime = 1;                                           \
        if (veryVerbose && firstTime) {                                     \
            std::puts("\t\tBSLMA EXCEPTION TEST -- (ENABLED) --");          \
        }                                                                   \
        firstTime = 0;                                                      \
    }                                                                       \
    if (veryVeryVerbose) {                                                  \
        std::puts("\t\tBegin bslma exception test.");                       \
    }                                                                       \
    int bslmaExceptionCounter = 0;                                          \
    const bslma::TestAllocator_ProxyBase& bslmaExceptionTestAllocator =     \
                       bslma::TestAllocator_getProxy(&BSLMA_TESTALLOCATOR); \
    bslmaExceptionTestAllocator.setAllocationLimit(bslmaExceptionCounter);  \
    do {                                                                    \
        try {
#endif  // BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN

#else   // !defined(BDE_BUILD_TARGET_EXC)

#ifndef BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN
#define BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(BSLMA_TESTALLOCATOR)       \
{                                                                           \
    static int firstTime = 1;                                               \
    if (verbose && firstTime) {                                             \
        std::puts("\t\tBSLMA EXCEPTION TEST -- (NOT ENABLED) --");          \
        firstTime = 0;                                                      \
    }                                                                       \
}
#endif  // BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN

#endif  // BDE_BUILD_TARGET_EXC

                 // ============================================
                 // macro BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
                 // ============================================

#ifdef BDE_BUILD_TARGET_EXC

#ifndef BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
#define BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END                              \
        } catch (bslma::TestAllocatorException& e) {                        \
            if (veryVeryVerbose) {                                          \
                std::printf("\t*** BSLMA_EXCEPTION: "                       \
                            "alloc limit = %d, last alloc size = %d ***\n", \
                            bslmaExceptionCounter,                          \
                            static_cast<int>(e.numBytes()));                \
            }                                                               \
            bslmaExceptionTestAllocator.setAllocationLimit(                 \
                                                 ++bslmaExceptionCounter);  \
            continue;                                                       \
        }                                                                   \
        bslmaExceptionTestAllocator.setAllocationLimit(-1);                 \
        break;                                                              \
    } while (1);                                                            \
    if (veryVeryVerbose) {                                                  \
        std::puts("\t\tEnd bslma exception test.");                         \
    }                                                                       \
    BSLMA_EXCEPTION_TEST_WORKAROUND                                         \
}

#endif  // BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

#else   // !defined(BDE_BUILD_TARGET_EXC)

#ifndef BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
#define BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
#endif

#endif

namespace bslma {

// ============================================================================
//                      INLINE FUNCTION DEFINITIONS
// ============================================================================

                        // -------------------
                        // class TestAllocator
                        // -------------------

// MANIPULATORS
inline
void TestAllocator::setAllocationLimit(bsls::Types::Int64 limit)
{
    d_allocationLimit.storeRelaxed(limit);
}

inline
void TestAllocator::setNoAbort(bool flagValue)
{
    d_noAbortFlag.storeRelaxed(flagValue);
}

inline
void TestAllocator::setQuiet(bool flagValue)
{
    d_quietFlag.storeRelaxed(flagValue);
}

inline
void TestAllocator::setVerbose(bool flagValue)
{
    d_verboseFlag.storeRelaxed(flagValue);
}

// ACCESSORS
inline
bsls::Types::Int64 TestAllocator::allocationLimit() const
{
    return d_allocationLimit.loadRelaxed();
}

inline
bool TestAllocator::isNoAbort() const
{
    return d_noAbortFlag.loadRelaxed();
}

inline
bool TestAllocator::isQuiet() const
{
    return d_quietFlag.loadRelaxed();
}

inline
bool TestAllocator::isVerbose() const
{
    return d_verboseFlag.loadRelaxed();
}

inline
void *TestAllocator::lastAllocatedAddress() const
{
    return reinterpret_cast<void *>(d_lastAllocatedAddress_p.loadRelaxed());
}

inline
Allocator::size_type TestAllocator::lastAllocatedNumBytes() const
{
    return static_cast<size_type>(d_lastAllocatedNumBytes.loadRelaxed());
}

inline
void *TestAllocator::lastDeallocatedAddress() const
{
    return reinterpret_cast<void *>(d_lastDeallocatedAddress_p.loadRelaxed());
}

inline
Allocator::size_type TestAllocator::lastDeallocatedNumBytes() const
{
    return static_cast<size_type>(d_lastDeallocatedNumBytes.loadRelaxed());
}

inline
const char *TestAllocator::name() const
{
    return d_name_p;
}

inline
bsls::Types::Int64 TestAllocator::numAllocations() const
{
    return d_numAllocations.loadRelaxed();
}

inline
bsls::Types::Int64 TestAllocator::numBoundsErrors() const
{
    return d_numBoundsErrors.loadRelaxed();
}

inline
bsls::Types::Int64 TestAllocator::numBlocksInUse() const
{
    return d_numBlocksInUse.loadRelaxed();
}

inline
bsls::Types::Int64 TestAllocator::numBlocksMax() const
{
    return d_numBlocksMax.loadRelaxed();
}

inline
bsls::Types::Int64 TestAllocator::numBlocksTotal() const
{
    return d_numBlocksTotal.loadRelaxed();
}

inline
bsls::Types::Int64 TestAllocator::numBytesInUse() const
{
    return d_numBytesInUse.loadRelaxed();
}

inline
bsls::Types::Int64 TestAllocator::numBytesMax() const
{
    return d_numBytesMax.loadRelaxed();
}

inline
bsls::Types::Int64 TestAllocator::numBytesTotal() const
{
    return d_numBytesTotal.loadRelaxed();
}

inline
bsls::Types::Int64 TestAllocator::numDeallocations() const
{
    return d_numDeallocations.loadRelaxed();
}

inline
bsls::Types::Int64 TestAllocator::numMismatches() const
{
    return d_numMismatches.loadRelaxed();
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
