// bslma_testallocator.h                                              -*-C++-*-
#ifndef INCLUDED_BSLMA_TESTALLOCATOR
#define INCLUDED_BSLMA_TESTALLOCATOR

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide instrumented malloc/free allocator to track memory usage.
//
//@CLASSES:
//  bslma::TestAllocator: instrumented `malloc`/`free` memory allocator
//
//@MACROS:
//  BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN: macro to begin testing exceptions
//  BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END: macro to end testing exceptions
//
//@SEE_ALSO: bslma_newdeleteallocator, bslma_mallocfreeallocator,
//  balst_stacktracetestallocator
//
//@DESCRIPTION: This component provides an instrumented allocator,
// `bslma::TestAllocator`, that implements the `bslma::Allocator` protocol and
// can be used to track various aspects of memory allocated from it.  Available
// statistics include the number of outstanding blocks (and bytes) that are
// currently in use, the cumulative number of blocks (and bytes) that have been
// allocated, and the maximum number of blocks (and bytes) that have been in
// use at any one time.  A `print` function formats these values to `stdout`:
// ```
//  ,--------------------.
// ( bslma::TestAllocator )
//  `--------------------'
//            |         ctor/dtor
//            |         lastAllocatedAddress/lastDeallocatedAddress
//            |         lastAllocatedNumBytes/lastDeallocatedNumBytes
//            |         numAllocations/numDeallocations
//            |         numBlocksInUse/numBlocksMax/numBlocksTotal
//            |         numBytesInUse/numBytesMax/numBytesTotal
//            |         numMismatches/numBoundsErrors
//            |         print/name
//            |         setAllocationLimit/allocationLimit
//            |         setNoAbort/isNoAbort
//            |         setQuiet/isQuiet
//            |         setVerbose/isVerbose
//            |         status
//            V
//    ,----------------.
//   ( bslma::Allocator )
//    `----------------'
//                      allocate
//                      deallocate
// ```
// If exceptions are enabled, this allocator can be configured to throw an
// exception after the number of allocation requests exceeds some specified
// limit (see the subsection on "Allocation Limit" below).  The level of
// verbosity can also be adjusted.  Each allocator object also maintains a
// current status.
//
// By default this allocator gets its memory from the C Standard Library
// functions `malloc` and `free`, but can be overridden to take memory from any
// allocator (supplied at construction) that implements the `bslma::Allocator`
// protocol.  Note that allocation and deallocation using a
// `bslma::TestAllocator` object is explicitly incompatible with `malloc` and
// `free` (or any other allocation mechanism).  Attempting to use `free` to
// deallocate memory allocated from a `bslma::TestAllocator` -- even when
// `malloc` and `free` are used by default -- will result in undefined
// behavior, almost certainly corrupting the C Standard Library's runtime
// memory manager.
//
// Memory dispensed from a `bslma::TestAllocator` is marked such that
// attempting to deallocate previously unallocated (or already deallocated)
// memory will (with high probability) be flagged as an error (unless quiet
// mode is set for the purpose of testing the test allocator itself).  A
// `bslma::TestAllocator` also supports a buffer overrun / underrun feature --
// each allocation has "pads", areas of extra memory before and after the
// segment that are initialized to a particular value and checked upon
// deallocation to see if they have been modified.  If they have, a message is
// printed and the allocator aborts, unless it is in quiet mode.
//
///Detecting Memory Leaks
///----------------------
// The `bslma::TestAllocator` is useful for detecting memory leaks, unless
// configured in quiet mode.  With the default configuration, if a test
// allocator is destroyed before all memory is reclaimed, a report will be
// logged and `abort` will be called.  When such a memory leak is detected,
// clients can substitute `balst::StackTraceTestAllocator` for
// `bslma::TestAllocator` to report stack traces of allocations that were
// leaked.  Note that `balst::StackTraceTestAllocator` is slower and consumes
// more memory than `bslma::TestAllocator`, and usually is not appropriate for
// automated tests.
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
// the `numMismatches` and `numBoundsErrors` counter.
//
// NO-ABORT MODE: (Default 0) Specifies that the test allocator should not
// invoke `abort` under any circumstances without suppressing diagnostics.
// Although the internal state values are independent, quiet mode implies the
// behavior of no-abort mode in all cases.  Note that this mode is used
// primarily for visual inspection of unusual error diagnostics in this
// component's test driver (in non-quiet mode only).
//
// Taking the default mode settings, memory allocation/deallocation will not be
// displayed individually.  However, in the event of a mismatched deallocation
// or a memory leak detected at destruction, the problem will be announced, any
// relevant state of the object will be displayed, and the program will abort.
//
// The three modes are independently set using the `setVerbose`, `setQuiet`,
// and `setNoAbort` manipulators.
//
///Allocation Limit
///----------------
// If exceptions are enabled at compile time, the test allocator can be
// configured to throw a `bslma::TestAllocatorException` after a specified
// number of allocation requests is exceeded.  If the allocation limit is less
// than 0 (default), then the allocator won't throw a `TestAllocatorException`
// exception.  Note that a non-negative allocation limit is decremented after
// each allocation attempt, and an exception is thrown only when the current
// allocation limit transitions from 0 to -1; no additional exceptions will be
// thrown until the allocation limit is again reset to a non-negative value.
//
// The allocation limit is set using the `setAllocationLimit` manipulator.
//
///Exception Test Macros
///---------------------
// This component also provides a pair of macros:
//
// * `BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(BSLMA_TESTALLOCATOR)`
// * `BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END`
//
// These macros can be used for testing exception-safety of classes and their
// methods when memory allocation is needed.  A reference to an object of type
// `bslma::TestAllocator` must be supplied as an argument to the `_BEGIN`
// macro.  Note that if exception-handling is disabled (i.e., if
// `BDE_BUILD_TARGET_EXC` is not defined when building the code under test),
// then the macros simply print the following:
// ```
// BSLMA EXCEPTION TEST -- (NOT ENABLED) --
// ```
// When exception-handling is enabled, the `_BEGIN` macro will set the
// allocation limit of the supplied allocator to 0, `try` the code being
// tested, `catch` any `TestAllocatorException`s that are thrown, and keep
// increasing the allocation limit until the code being tested completes
// successfully.
//
///Thread Safety
///-------------
// The `bslma::TestAllocator` class is fully thread-safe (see
// `bsldoc_glossary`).  Note that the `bslma::MallocFreeAllocator` singleton
// (the allocator used by the test allocator if none is supplied at
// construction) is fully thread-safe.
//
///Usage
///-----
// The `bslma::TestAllocator` defined in this component can be used in
// conjunction with the `BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN` and
// `BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END` macros to test the memory usage
// patterns of an object that uses the `bslma::Allocator` protocol in its
// interface.  In this example, we illustrate how we might test that an object
// under test is exception-neutral.  For illustration purposes, we will assume
// the existence of a `my_shortarray` component implementing an
// `std::vector`-like array type, `myShortArray`:
// ```
// // my_shortarray.t.cpp
// #include <my_shortarray.h>
//
// #include <bslma_testallocator.h>
// #include <bslma_testallocatorexception.h>
//
// // ...
// ```
// Below we provide a `static` function, `areEqual`, that will allow us to
// compare two short arrays:
// ```
// static
// bool areEqual(const short *array1, const short *array2, int numElements)
//     // Return 'true' if the specified initial 'numElements' in the
//     // specified 'array1' and 'array2' have the same values, and 'false'
//     // otherwise.
// {
//     for (int i = 0; i < numElements; ++i) {
//         if (array1[i] != array2[i]) {
//             return false;                                         // RETURN
//         }
//     }
//     return true;
// }
//
// // ...
// ```
// The following is an abbreviated standard test driver.  Note that the number
// of arguments specify the verbosity level that the test driver uses for
// printing messages:
// ```
// int main(int argc, char *argv[])
// {
//     int                 test = argc > 1 ? atoi(argv[1]) : 0;
//     bool             verbose = argc > 2;
//     bool         veryVerbose = argc > 3;
//     bool     veryVeryVerbose = argc > 4;
//     bool veryVeryVeryVerbose = argc > 5;
// ```
// We now define a `bslma::TestAllocator`, `sa`, named "supplied" to indicate
// that it is the allocator to be supplied to our object under test, as well as
// to the `BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN` macro (below).  Note that
// if `veryVeryVeryVerbose` is `true`, then `sa` prints all allocation and
// deallocation requests to `stdout` and also prints the accumulated statistics
// on destruction:
// ```
// bslma::TestAllocator sa("supplied", veryVeryVeryVerbose);
//
// switch (test) { case 0:
//
//   // ...
//
//   case 6: {
//
//     // ...
//
//     struct {
//         int   d_line;
//         int   d_numElem;
//         short d_exp[NUM_VALUES];
//     } DATA[] = {
//         { L_, 0, { } },
//         { L_, 1, { V0 } },
//         { L_, 5, { V0, V1, V2, V3, V4 } }
//     };
//     const int NUM_DATA = sizeof DATA / sizeof *DATA;
//
//     for (int ti = 0; ti < NUM_DATA; ++ti) {
//         const int    LINE     = DATA[ti].d_line;
//         const int    NUM_ELEM = DATA[ti].d_numElem;
//         const short *EXP      = DATA[ti].d_exp;
//
//         if (veryVerbose) { T_ P_(ti) P_(NUM_ELEM) }
//
//         // ...
// ```
// All code that we want to test for exception-safety must be enclosed within
// the `BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN` and
// `BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END` macros, which internally implement
// a `do`-`while` loop.  Code provided by the
// `BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN` macro sets the allocation limit
// of the supplied allocator to 0 causing it to throw an exception on the first
// allocation.  This exception is caught by code provided by the
// `BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END` macro, which increments the
// allocation limit by 1 and re-runs the same code again.  Using this scheme we
// can check that our code does not leak memory for any memory allocation
// request.  Note that the curly braces surrounding these macros, although
// visually appealing, are not technically required:
// ```
//     BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(sa) {
//       my_ShortArray mA(&sa);
//       const my_ShortArray& A = mA;
//       for (int ei = 0; ei < NUM_ELEM; ++ei) {
//           mA.append(VALUES[ei]);
//       }
//       if (veryVerbose) { T_ T_  P_(NUM_ELEM) P(A) }
//       LOOP_ASSERT(LINE, areEqual(EXP, A, NUM_ELEM));
//     } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
// }
// ```
// After the exception-safety test we can ensure that all the memory allocated
// from `sa` was successfully deallocated:
// ```
//         if (veryVerbose) sa.print();
//
//       } break;
//
//       // ...
//
//     }
//
//     // ...
// }
// ```
// Note that the `BDE_BUILD_TARGET_EXC` macro is defined at compile-time to
// indicate whether or not exceptions are enabled.

#include <bslscm_version.h>

#include <bslma_allocator.h>
#include <bslma_testallocatorexception.h>

#include <bsls_atomic.h>
#include <bsls_bsllock.h>
#include <bsls_buildtarget.h>
#include <bsls_keyword.h>
#include <bsls_types.h>

#include <cstdio>     // 'std::FILE' and (in macros) 'std::puts'

#ifdef BDE_VERIFY
# pragma bde_verify -AQK01 // Suppress "Need #include <stdio.h> for 'stdout'"
#endif

namespace BloombergLP {
namespace bslma {

// FORWARD REFERENCES
struct TestAllocator_BlockHeader;
class  TestAllocatorStashedStatistics;

                             // ===================
                             // class TestAllocator
                             // ===================

/// This class defines a concrete "test" allocator mechanism that implements
/// the `Allocator` protocol, and provides instrumentation to track (1) the
/// number of blocks/bytes currently in use, (2) the maximum number of
/// blocks/bytes that have been outstanding at any one time, and (3) the
/// cumulative number of blocks/bytes that have ever been allocated by this
/// test allocator object.  The accumulated statistics are based solely on
/// the number of bytes requested.  Additional testing facilities include
/// allocation limits, verbosity modes, status, and automated report
/// printing.
///
/// Note that, unlike many other allocators, this allocator does NOT rely on
/// the currently installed default allocator (see `bslma_default`), but
/// instead -- by default -- uses the `MallocFreeAllocator` singleton, which
/// in turn calls the C Standard Library functions `malloc` and `free` as
/// needed.  Clients may, however, override this allocator by supplying (at
/// construction) any other allocator implementing the `Allocator` protocol.
class TestAllocator : public Allocator {

    // CONSTANTS
    enum {
        // Compute size of buffer needed to hold ascii-formatted statistics.
        // The computation is a high estimate and includes the null terminator.
        k_INT64_MAXDIGITS  = 20, // max digits in `Int64, including sign
        k_LABLETEXT_LEN    = 51, // max label characters per line, including NL
        k_NUM_STATLINES    = 11, // Number of nonblank lines in formatted stats
        k_PRINTED_STATS_SZ =
            k_NUM_STATLINES * (k_LABLETEXT_LEN + 2 * k_INT64_MAXDIGITS) + 1,

        // Compute size of buffer needed to hold one line of block IDs
        // separated by tabs (including newline and null terminator).
        k_BLOCKID_LINE_SZ  = 8 * (k_INT64_MAXDIGITS + 1) + 1 + 1
    };

    // DATA

                        // Control Points

    const char *d_name_p;                // optionally specified name of this
                                         // test allocator object (or 0)

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

    // Statistics and other attributes are updated in bulk while holding an
    // object-wide mutex ('d_lock') but are read atomically by individual
    // accessors without acquiring the mutex; hence, each such data member has
    // an atomic type.

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

                        // Other Attributes

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

                        // Other Data

    TestAllocator_BlockHeader
               *d_blockListHead_p;      // first allocated block (owned)

    TestAllocator_BlockHeader
               *d_blockListTail_p;      // last allocated block (owned)

    mutable bsls::BslLock
                d_lock;                 // Ensure mutual exclusion in
                                        // 'allocate', 'deallocate', 'print',
                                        // and 'status'.

    Allocator  *d_allocator_p;          // upstream allocator (held, not owned)

    // PRIVATE ACCESSORS

    /// Traverse up to 8 blocks from the specified `*blockList` and write
    /// their IDs to the specified `output` buffer, advance `*blockList` to
    /// point to the first block in the list not not traversed (null if the
    /// the last block was traversed), then return the number of characters
    /// written to `output`, excluding the null terminator.  The output
    /// consists of a sequence of decimal-formatted IDs, each proceeded by
    /// a tab character and ending with a newline character and null
    /// terminator.  The behavior is undefined unless `output` has
    /// sufficient space for at least `k_BLOCKID_LINE_SZ` characters.
    std::size_t
    formatEightBlockIds(const TestAllocator_BlockHeader** blockList,
                        char*                             output   ) const;

    /// Write the accumulated statistics held in this allocator to the
    /// specified `output` in a reasonable (multi-line) format and return
    /// the number of characters written, excluding the null terminator.
    /// The behavior is undefined unless `output` has sufficient space for
    /// at least `k_PRINTED_STATS_SZ` characters.
    std::size_t formatStats(char *output) const;

    /// Write the accumulated state information held in this allocator to
    /// the specified `stream` in a reasonable (multi-line) format and
    /// return a reference offering modifiable access to `stream`.  Output
    /// is performed via calls to `stream.write(s, count)`.
    template <class t_OS>
    t_OS& printToStream(t_OS& stream) const;

    // NOT IMPLEMENTED
    TestAllocator(const TestAllocator&);             // = delete
    TestAllocator& operator=(const TestAllocator&);  // = delete

  public:
    // CREATORS

    /// Create an instrumented "test" allocator.  Optionally specify a
    /// `name` (associated with this object) to be included in diagnostic
    /// messages written to `stdout`, thereby distinguishing this test
    /// allocator from others that might be used in the same program.  If
    /// `name` is 0 (or not specified), no distinguishing name is
    /// incorporated in diagnostics.  Optionally specify a `verboseFlag`
    /// indicating whether this test allocator should automatically report
    /// all allocation/deallocation events to `stdout` and print accumulated
    /// statistics on destruction.  If `verboseFlag` is `false` (or not
    /// specified), allocation/deallocation and summary messages will not be
    /// written automatically.  Optionally specify a `basicAllocator` used
    /// to supply memory.  If `basicAllocator` is 0, the
    /// `MallocFreeAllocator` singleton is used.
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

    /// Destroy this allocator.  In verbose mode, print all contained state
    /// values of this allocator object to `stdout`.  Except in quiet mode,
    /// automatically report any memory leaks to `stdout`.  Abort if either
    /// `numBlocksInUse` or `numBytesInUse` return non-zero unless in
    /// no-abort mode or quiet mode.  Note that, in all cases, destroying
    /// this object has no effect on outstanding memory blocks allocated
    /// from this test allocator (and may result in memory leaks -- e.g., if
    /// the (default) `MallocFreeAllocator` singleton was used).
    ~TestAllocator() BSLS_KEYWORD_OVERRIDE;

    // MANIPULATORS

    /// Return a newly-allocated block of memory of the specified `size` (in
    /// bytes).  If `size` is 0, a null pointer is returned.  Otherwise,
    /// invoke the `allocate` method of the allocator supplied at
    /// construction, increment the number of currently (and cumulatively)
    /// allocated blocks, and increase the number of currently allocated
    /// bytes by `size`.  Update all other fields accordingly; if the
    /// allocation fails via an exception, `numAllocations()` is
    /// incremented, `lastAllocatedNumBytes()` is set to `size`, and
    /// `lastDeallocatedAddress()` is set to 0.
    void *allocate(size_type size) BSLS_KEYWORD_OVERRIDE;

    /// Return the memory block at the specified `address` back to this
    /// allocator.  If `address` is 0, this function has no effect (other
    /// than to record relevant statistics).  Otherwise, if the memory at
    /// `address` is consistent with being allocated from this test
    /// allocator, decrement the number of currently allocated blocks, and
    /// decrease the number of currently allocated bytes by the size (in
    /// bytes) originally requested for the block.  Although technically
    /// undefined behavior, if the memory can be determined not to have been
    /// allocated from this test allocator, increment the number of
    /// mismatches, and -- unless in quiet mode -- immediately report the
    /// details of the mismatch to `stdout` (e.g., as an `std::hex` memory
    /// dump) and abort.
    void deallocate(void *address) BSLS_KEYWORD_OVERRIDE;

    /// Return the current statistics that may later be passed to
    /// `restoreStatistics`, and reset the current statistic as follows:
    ///
    /// Statistic        | Reset value to
    /// ---------------- | --------------
    /// numAllocations   | numBlocksInUse
    /// numDeallocations | ZERO
    /// numMismatches    | ZERO
    /// numBoundsErrors  | ZERO
    /// numBlocksMax     | numBlocksInUse
    /// numBytesMax      | numBytesInUse
    /// numBlocksTotal   | numBlocksInUse
    /// numBytesTotal    | numBytesInUse
    ///
    /// See also `restoreStatistics`.
    TestAllocatorStashedStatistics stashStatistics();

    /// Restore the statistics from the specified `savedStatistics` by
    /// appropriately combining the current values and the saved values.  The
    /// behavior is undefined unless `savedStatistics` is a value returned by
    /// a previous call to `stashStatictics` on this same object, which has not
    /// been passed to `restoreStatistics` yet.  This method restores the state
    /// of the statistics as if the corresponding `stashStatistics` call has
    /// never happened, in the following manner:
    ///
    /// Statistic        | Restore value as
    /// ---------------- | ---------------------------------------------------
    /// numAllocations   | saved + current - saved.numBlocksInUse
    /// numDeallocations | saved.numDeallocations + current.numDeallocations
    /// numMismatches    | saved.numMismatches    + current.numMismatches
    /// numBoundsErrors  | saved.numBoundsErrors  + current.numBoundsErrors
    /// numBlocksMax     | max(saved.numBlocksMax, current.numBlocksMax)
    /// numBytesMax      | max(saved.numBytesMax,  current.numBytesMax)
    /// numBlocksTotal   | saved + current - saved.numBlocksInUse
    /// numBytesTotal    | saved + current - saved.numBytesInUse
    ///
    /// See also `stashStatistics`.
    void restoreStatistics(TestAllocatorStashedStatistics *savedStatistics);

    /// Set the number of valid allocation requests before an exception is
    /// to be thrown for this allocator to the specified `limit`.  If
    /// `limit` is less than 0, no exception is to be thrown.  By default,
    /// no exception is scheduled.
    void setAllocationLimit(bsls::Types::Int64 limit);

    /// Set the no-abort mode for this test allocator to the specified
    /// (boolean) `flagValue`.  `If flagValue` is `true`, aborting on fatal
    /// errors is suppressed, and the functions simply return.  Diagnostics
    /// are not affected.  Note that the default mode is to abort.  Also
    /// note that this function is provided primarily to enable visual
    /// testing of diagnostic messages produced by this component.
    void setNoAbort(bool flagValue);

    /// Set the quiet mode for this test allocator to the specified
    /// (boolean) `flagValue`.  If `flagValue` is `true`, mismatched
    /// allocations, overrun/underrun errors, and memory leak messages will
    /// not be displayed to `stdout` and the process will not abort as a
    /// result of such conditions.  Note that the default mode is *not*
    /// quiet.  Also note that this function is provided primarily to enable
    /// testing of this component; in quiet mode, situations that would
    /// otherwise abort will just quietly increment the `numMismatches`
    /// and/or `numBoundsErrors` counters.
    void setQuiet(bool flagValue);

    /// Set the verbose mode for this test allocator to the specified
    /// (boolean) `flagValue`.  If `flagValue` is `true`, all
    /// allocation/deallocation events will be reported automatically on
    /// `stdout`, as will accumulated statistics upon destruction of this
    /// object.  Note that the default mode is *not* verbose.
    void setVerbose(bool flagValue);

    // ACCESSORS

    /// Return the current number of allocation requests left before an
    /// exception is thrown.  A negative value indicates that no exception
    /// is scheduled.
    bsls::Types::Int64 allocationLimit() const;

    /// Return `true` if this allocator is currently in no-abort mode, and
    /// `false` otherwise.  In no-abort mode all diagnostic messages are
    /// printed, but all aborts are suppressed.  Note that quiet mode
    /// implies no-abort mode.
    bool isNoAbort() const;

    /// Return `true` if this allocator is currently in quiet mode, and
    /// `false` otherwise.  In quiet mode, messages about mismatched
    /// deallocations, overrun/underrun errors, and memory leaks will not be
    /// displayed to `stdout` and will not cause the program to abort.
    bool isQuiet() const;

    /// Return `true` if this allocator is currently in verbose mode, and
    /// `false` otherwise.  In verbose mode, all allocation/deallocation
    /// events will be reported on `stdout`, as will summary statistics upon
    /// destruction of this object.
    bool isVerbose() const;

    /// Return the address that was returned by the most recent allocation
    /// request.  Return 0 if the most recent allocation request was for 0
    /// bytes.
    void *lastAllocatedAddress() const;

    /// Return the number of bytes of the most recent allocation request.
    size_type lastAllocatedNumBytes() const;

    /// Return the address that was supplied to the most recent deallocation
    /// request.  Return 0 if a null pointer was most recently deallocated.
    /// Note that the address is always recorded regardless of the validity
    /// of the request.
    void *lastDeallocatedAddress() const;

    /// Return the number of bytes of the most recent deallocation request.
    /// Return 0 if a null pointer was most recently deallocated, or if the
    /// request was invalid (e.g., an attempt to deallocate memory not
    /// allocated through this allocator).
    size_type lastDeallocatedNumBytes() const;

    /// Return the name of this test allocator, or 0 if no name was
    /// specified at construction.
    const char *name() const;

    /// Return the cumulative number of allocation requests.  Note that this
    /// number is incremented for every `allocate` invocation.
    bsls::Types::Int64 numAllocations() const;

    /// Return the number of blocks currently allocated from this object.
    /// Note that `numBlocksInUse() <= numBlocksMax()`.
    bsls::Types::Int64 numBlocksInUse() const;

    /// Return the maximum number of blocks ever allocated from this object
    /// at any one time.  Note that
    /// `numBlocksInUse() <= numBlocksMax() <= numBlocksTotal()`.
    bsls::Types::Int64 numBlocksMax() const;

    /// Return the cumulative number of blocks ever allocated from this
    /// object.  Note that `numBlocksMax() <= numBlocksTotal()`.
    bsls::Types::Int64 numBlocksTotal() const;

    /// Return the number of times memory deallocations have detected that
    /// pad areas at the front or back of the user segment had been
    /// overwritten.
    bsls::Types::Int64 numBoundsErrors() const;

    /// Return the number of bytes currently allocated from this object.
    /// Note that `numBytesInUse() <= numBytesMax()`.
    bsls::Types::Int64 numBytesInUse() const;

    /// Return the maximum number of bytes ever allocated from this object
    /// at any one time.  Note that
    /// `numBytesInUse() <= numBytesMax() <= numBytesTotal()`.
    bsls::Types::Int64 numBytesMax() const;

    /// Return the cumulative number of bytes ever allocated from this
    /// object.  Note that `numBytesMax() <= numBytesTotal()`.
    bsls::Types::Int64 numBytesTotal() const;

    /// Return the cumulative number of deallocation requests.  Note that
    /// this number is incremented for every `deallocate` invocation,
    /// regardless of the validity of the request.
    bsls::Types::Int64 numDeallocations() const;

    /// Return the number of mismatched memory deallocations that have
    /// occurred since this object was created.  A memory deallocation is
    /// *mismatched* if that memory was not allocated directly from this
    /// allocator.
    bsls::Types::Int64 numMismatches() const;

    /// Write the accumulated state information held in this allocator to
    /// the optionally specified file `f` (default `stdout`) in a reasonable
    /// (multi-line) format.
    void print(std::FILE *f = stdout) const;

    /// Return 0 on success, and non-zero otherwise: If there have been any
    /// mismatched memory deallocations or over/under runs, return the
    /// number of such errors that have occurred as a positive number; if
    /// either `0 < numBlocksInUse()` or `0 < numBytesInUse()`, return an
    /// arbitrary negative number; else return 0.
    int status() const;

#ifndef BDE_OPENSOURCE_PUBLICATION  // DEPRECATED

    /// Return the allocated memory address of the most recent memory
    /// request.  Return 0 if the request was invalid (e.g., allocate non-
    /// positive number of bytes).
    ///
    /// DEPRECATED: use `lastAllocatedAddress` instead.
    void *lastAllocateAddress() const;

    /// Return the number of bytes of the most recent memory request.  Note
    /// that this number is always recorded regardless of the validity of
    /// the request.
    ///
    /// DEPRECATED: use `lastAllocatedNumBytes` instead.
    size_type lastAllocateNumBytes() const;

    /// Return the memory address of the last memory deallocation request.
    /// Note that the address is always recorded regardless of the validity
    /// of the request.
    ///
    /// DEPRECATED: use `lastDeallocatedAddress` instead.
    void *lastDeallocateAddress() const;

    /// Return the number of bytes of the most recent memory deallocation
    /// request.  Return 0 if the request was invalid (e.g., deallocating
    /// memory not allocated through this allocator).
    ///
    /// DEPRECATED: use `lastDeallocatedNumBytes` instead.
    size_type lastDeallocateNumBytes() const;

    /// Return the cumulative number of allocation requests.  Note that this
    /// number is incremented for every `allocate` invocation, regardless of
    /// the validity of the request.
    ///
    /// DEPRECATED: use `numAllocations` instead.
    bsls::Types::Int64 numAllocation() const;

    /// Return the cumulative number of deallocation requests.  Note that
    /// this number is incremented for every `deallocate` invocation,
    /// regardless of the validity of the request.
    ///
    /// DEPRECATED: use `numDeallocations` instead.
    bsls::Types::Int64 numDeallocation() const;
#endif  // BDE_OPENSOURCE_PUBLICATION

    // HIDDEN FRIENDS

    /// Write the accumulated state information held in the specified
    /// allocator `ta` to the specified `stream` in a reasonable
    /// (multi-line) format identical to the format produced by `ta.print()`
    /// and return a reference offering modifiable access to `stream`.
    /// Output is performed via calls to `stream.write(s, count)`, where
    /// `write` is a required method of class `t_OS`, `s` is a `const char*`
    /// holding the formatted output, and `count` is the length of `s`
    /// excluding any null terminator.  Note that `std::ostream` meets the
    /// requirements for `t_OS`.
    template <class t_OS>
    friend t_OS& operator<<(t_OS& stream, const TestAllocator& ta)
        { return ta.printToStream(stream); }
};

                   // ====================================
                   // class TestAllocatorStashedStatistics
                   // ====================================

/// This simple unconstrained attribute type is used to store all information
/// necessary to restore the state of all statistics of a `TestAllocator` after
/// a `stashStatistics()` call that resets them.  See also
/// `TestAllocator::stashStatistics` and `TestAllocator::restoreStatistics`.
class TestAllocatorStashedStatistics {
  private:
    // DATA
    TestAllocator      *d_origin_p;

    bsls::Types::Int64  d_numBlocksInUse;
    bsls::Types::Int64  d_numBytesInUse;

    bsls::Types::Int64  d_numAllocations;
    bsls::Types::Int64  d_numDeallocations;
    bsls::Types::Int64  d_numMismatches;
    bsls::Types::Int64  d_numBoundsErrors;
    bsls::Types::Int64  d_numBlocksMax;
    bsls::Types::Int64  d_numBytesMax;
    bsls::Types::Int64  d_numBlocksTotal;
    bsls::Types::Int64  d_numBytesTotal;

  public:
    // CREATORS

    /// Create a `TestAllocatorStashedStatistics` object with its members
    /// initialized by the current statistics values of the specified
    /// `testAllocator`, and the address of it as `origin`.
    TestAllocatorStashedStatistics(TestAllocator *testAllocator);

    // MANIPULATORS

    /// Set the `origin` to a null pointer to indicate that this stash has been
    /// restored and must not be used again with
    /// `TestAllocator::restoreStatistics`.  The behavior is undefined unless
    /// this method is called only once during the lifetime of this object.
    void markRestored();

    /// Call `origin->restoreStatistics(*this)`.
    void restore();

    // ACCESSORS

    /// Return the `numBlocksInUse` attribute value.
    bsls::Types::Int64 numBlocksInUse() const;

    /// Return the `numBytesInUse` attribute value.
    bsls::Types::Int64 numBytesInUse() const;

    /// Return the `numAllocations` attribute value.
    bsls::Types::Int64 numAllocations() const;

    /// Return the `numDeallocations` attribute value.
    bsls::Types::Int64 numDeallocations() const;

    /// Return the `numMismatches` attribute value.
    bsls::Types::Int64 numMismatches() const;

    /// Return the `numBoundsErrors` attribute value.
    bsls::Types::Int64 numBoundsErrors() const;

    /// Return the `numBlocksMax` attribute value.
    bsls::Types::Int64 numBlocksMax() const;

    /// Return the `numBytesMax` attribute value.
    bsls::Types::Int64 numBytesMax() const;

    /// Return the `numBlocksTotal` attribute value.
    bsls::Types::Int64 numBlocksTotal() const;

    /// Return the `numBytesTotal` attribute value.
    bsls::Types::Int64 numBytesTotal() const;

    /// Return a const pointer to the test allocator that was saved.
    TestAllocator *origin() const;
};

}  // close package namespace

               // ==============================================
               // macro BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN
               // ==============================================

#ifdef BDE_BUILD_TARGET_EXC

namespace bslma {

/// This class provides a common base class for the parameterized
/// `TestAllocator_Proxy` class (below).  Note that the `virtual`
/// `setAllocationLimit` method, although a "setter", *must* be declared
/// `const`.
class TestAllocator_ProxyBase {

  public:
    // CREATOR
    virtual ~TestAllocator_ProxyBase()
    {
    }

    // ACCESSORS
    virtual void setAllocationLimit(bsls::Types::Int64 limit) const = 0;
};

/// This class provides a proxy to the test allocator that is supplied to
/// the `BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN` macro.  This proxy may be
/// instantiated with `TestAllocator`, or with a type that supports the same
/// interface as `TestAllocator`.
template <class BSLMA_ALLOC_TYPE>
class TestAllocator_Proxy : public TestAllocator_ProxyBase {

    // DATA
    BSLMA_ALLOC_TYPE *d_allocator_p;  // allocator used in '*_BEGIN' and
                                      // '*_END' macros (held, not owned)

  public:
    // CREATORS
    explicit TestAllocator_Proxy(BSLMA_ALLOC_TYPE *allocator)
    : d_allocator_p(allocator)
    {
    }

    ~TestAllocator_Proxy() BSLS_KEYWORD_OVERRIDE
    {
    }

    // ACCESSORS
    void setAllocationLimit(bsls::Types::Int64 limit) const
                                                          BSLS_KEYWORD_OVERRIDE
    {
        d_allocator_p->setAllocationLimit(limit);
    }
};

/// Return, by value, a test allocator proxy for the specified parameterized
/// `allocator`.
template <class BSLMA_ALLOC_TYPE>
inline
TestAllocator_Proxy<BSLMA_ALLOC_TYPE>
TestAllocator_getProxy(BSLMA_ALLOC_TYPE *allocator)
{
    return TestAllocator_Proxy<BSLMA_ALLOC_TYPE>(allocator);
}

}  // close package namespace

#ifndef BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN
// Note that the `while` loop in the following code uses a flag
// `bslmaKeepLoopingInTestAllocatorExceptionTest`.  This is a workaround for an
// XLC16 bug: a `continue` statement in a `catch` block can result in
// segmentation faults on optimized XLC16 builds.  Bug raised with IBM - see
// DRQS 169604597
#define BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(BSLMA_TESTALLOCATOR) {     \
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
    const BloombergLP::bslma::TestAllocator_ProxyBase&                      \
        bslmaExceptionTestAllocator =                                       \
          BloombergLP::bslma::TestAllocator_getProxy(&BSLMA_TESTALLOCATOR); \
    bslmaExceptionTestAllocator.setAllocationLimit(bslmaExceptionCounter);  \
    bool bslmaKeepLoopingInTestAllocatorExceptionTest = true;               \
    while(bslmaKeepLoopingInTestAllocatorExceptionTest) {                   \
        bslmaKeepLoopingInTestAllocatorExceptionTest = false;               \
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
        } catch (BloombergLP::bslma::TestAllocatorException& e) {           \
            if (veryVeryVerbose) {                                          \
                std::printf("\t*** BSLMA_EXCEPTION: "                       \
                            "alloc limit = %d, last alloc size = %d ***\n", \
                            bslmaExceptionCounter,                          \
                            static_cast<int>(e.numBytes()));                \
            }                                                               \
            bslmaExceptionTestAllocator.setAllocationLimit(                 \
                                                 ++bslmaExceptionCounter);  \
            bslmaKeepLoopingInTestAllocatorExceptionTest = true;            \
        }                                                                   \
    };                                                                      \
    bslmaExceptionTestAllocator.setAllocationLimit(-1);                     \
    if (veryVeryVerbose) {                                                  \
        std::puts("\t\tEnd bslma exception test.");                         \
    }                                                                       \
}

#endif  // BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

#else   // !defined(BDE_BUILD_TARGET_EXC)

#ifndef BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
#define BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
#endif

#endif

namespace bslma {

// ============================================================================
//                          INLINE DEFINITIONS
// ============================================================================

                        // -------------------
                        // class TestAllocator
                        // -------------------

// PRIVATE ACCESSORS
template <class t_OS>
t_OS& TestAllocator::printToStream(t_OS& stream) const
{
    bsls::BslLockGuard guard(&d_lock);

    char        buffer[k_PRINTED_STATS_SZ];
    std::size_t cnt = formatStats(buffer);
    stream.write(buffer, cnt);

    static const char        k_ID_STR[] =
                               " Indices of Outstanding Memory Allocations:\n";
    static const std::size_t k_ID_STR_LEN = sizeof(k_ID_STR) - 1;

    if (d_blockListHead_p) {
        stream.write(k_ID_STR, k_ID_STR_LEN);

        // Traverse the linked list starting from 'd_blockListHead_p' and print
        // the ID of each block in the list, 8 to a line
        const TestAllocator_BlockHeader *next_p = d_blockListHead_p;
        while (next_p) {
            cnt = formatEightBlockIds(&next_p, buffer);
            stream.write(buffer, cnt);
        }
    }

    return stream;
}

// MANIPULATORS
inline
TestAllocatorStashedStatistics TestAllocator::stashStatistics()
{
    bsls::BslLockGuard guard(&d_lock);

    const TestAllocatorStashedStatistics rv(this);

    d_numAllocations.storeRelaxed(d_numBlocksInUse.loadRelaxed());
    d_numDeallocations.storeRelaxed(0);

    d_numMismatches.storeRelaxed(0);
    d_numBoundsErrors.storeRelaxed(0);
    d_numBlocksMax.storeRelaxed(d_numBlocksInUse.loadRelaxed());
    d_numBytesMax.storeRelaxed(d_numBytesInUse.loadRelaxed());
    d_numBlocksTotal.storeRelaxed(d_numBlocksInUse.loadRelaxed());
    d_numBytesTotal.storeRelaxed(d_numBytesInUse.loadRelaxed());

    return rv;
}

inline
void TestAllocator::restoreStatistics(
                               TestAllocatorStashedStatistics *savedStatistics)
{
    BSLS_ASSERT(savedStatistics->origin() == this);

    bsls::BslLockGuard guard(&d_lock);

    d_numAllocations.storeRelaxed(
        savedStatistics->numAllocations() - savedStatistics->numBlocksInUse()
                                             + d_numAllocations.loadRelaxed());
    d_numDeallocations.storeRelaxed(
        savedStatistics->numDeallocations() + d_numDeallocations.loadRelaxed());

    d_numMismatches.storeRelaxed(
              savedStatistics->numMismatches() + d_numMismatches.loadRelaxed());

    d_numBoundsErrors.storeRelaxed(
          savedStatistics->numBoundsErrors() + d_numBoundsErrors.loadRelaxed());

    if (d_numBlocksMax.loadRelaxed() < savedStatistics->numBlocksMax()) {
        d_numBlocksMax.storeRelaxed(savedStatistics->numBlocksMax());
    }

    if (d_numBytesMax.loadRelaxed() < savedStatistics->numBytesMax()) {
        d_numBytesMax.storeRelaxed(savedStatistics->numBytesMax());
    }

    d_numBlocksTotal.storeRelaxed(
        savedStatistics->numBlocksTotal() - savedStatistics->numBlocksInUse()
                                             + d_numBlocksTotal.loadRelaxed());

    d_numBytesTotal.storeRelaxed(
        savedStatistics->numBytesTotal() - savedStatistics->numBytesInUse()
                                              + d_numBytesTotal.loadRelaxed());

    savedStatistics->markRestored();
}

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
bsls::Types::Int64 TestAllocator::numBoundsErrors() const
{
    return d_numBoundsErrors.loadRelaxed();
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

#ifndef BDE_OPENSOURCE_PUBLICATION  // DEPRECATED
inline
void *TestAllocator::lastAllocateAddress() const
{
    return lastAllocatedAddress();
}

inline
TestAllocator::size_type
TestAllocator::lastAllocateNumBytes() const
{
    return lastAllocatedNumBytes();
}

inline
void *TestAllocator::lastDeallocateAddress() const
{
    return lastDeallocatedAddress();
}

inline
TestAllocator::size_type
TestAllocator::lastDeallocateNumBytes() const
{
    return lastDeallocatedNumBytes();
}

inline
bsls::Types::Int64 TestAllocator::numAllocation() const
{
    return numAllocations();
}

inline
bsls::Types::Int64 TestAllocator::numDeallocation() const
{
    return numDeallocations();
}

#endif  // BDE_OPENSOURCE_PUBLICATION

                      // -----------------------------
                      // class TestAllocatorStatistics
                      // -----------------------------

// CREATORS
inline
TestAllocatorStashedStatistics::TestAllocatorStashedStatistics(
                                                 TestAllocator *testAllocator)
: d_origin_p(testAllocator)
, d_numBlocksInUse(testAllocator->numBlocksInUse())
, d_numBytesInUse(testAllocator->numBytesInUse())
, d_numAllocations(testAllocator->numAllocations())
, d_numDeallocations(testAllocator->numDeallocations())
, d_numMismatches(testAllocator->numMismatches())
, d_numBoundsErrors(testAllocator->numBoundsErrors())
, d_numBlocksMax(testAllocator->numBlocksMax())
, d_numBytesMax(testAllocator->numBytesMax())
, d_numBlocksTotal(testAllocator->numBlocksTotal())
, d_numBytesTotal(testAllocator->numBytesTotal())
{ }

// MANIPULATORS

inline
void TestAllocatorStashedStatistics::markRestored()
{
    BSLS_ASSERT(d_origin_p);

    d_origin_p = 0;
}

inline
void TestAllocatorStashedStatistics::restore()
{
    BSLS_ASSERT(d_origin_p);

    d_origin_p->restoreStatistics(this);
}

// ACCESSORS
inline
bsls::Types::Int64 TestAllocatorStashedStatistics::numBlocksInUse() const
{
    return d_numBlocksInUse;
}

inline
bsls::Types::Int64 TestAllocatorStashedStatistics::numBytesInUse() const
{
    return d_numBytesInUse;
}

inline
bsls::Types::Int64 TestAllocatorStashedStatistics::numAllocations() const
{
    return d_numAllocations;
}

inline
bsls::Types::Int64 TestAllocatorStashedStatistics::numDeallocations() const
{
    return d_numDeallocations;
}

inline
bsls::Types::Int64 TestAllocatorStashedStatistics::numMismatches() const
{
    return d_numMismatches;
}

inline
bsls::Types::Int64 TestAllocatorStashedStatistics::numBoundsErrors() const
{
    return d_numBoundsErrors;
}

inline
bsls::Types::Int64 TestAllocatorStashedStatistics::numBlocksMax() const
{
    return d_numBlocksMax;
}

inline
bsls::Types::Int64 TestAllocatorStashedStatistics::numBytesMax() const
{
    return d_numBytesMax;
}

inline
bsls::Types::Int64 TestAllocatorStashedStatistics::numBlocksTotal() const
{
    return d_numBlocksTotal;
}

inline
bsls::Types::Int64 TestAllocatorStashedStatistics::numBytesTotal() const
{
    return d_numBytesTotal;
}

inline
TestAllocator *TestAllocatorStashedStatistics::origin() const
{
    return d_origin_p;
}

}  // close package namespace

#ifndef BDE_OPENSOURCE_PUBLICATION  // BACKWARD_COMPATIBILITY
// ============================================================================
//                           BACKWARD COMPATIBILITY
// ============================================================================

/// This alias is defined for backward compatibility.
typedef bslma::TestAllocator bslma_TestAllocator;

// The following two macros can be deleted when they are no longer referenced
// in any .t.cpp files.

#ifndef BEGIN_BSLMA_EXCEPTION_TEST
#define BEGIN_BSLMA_EXCEPTION_TEST                                     \
              BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator)
#endif

#ifndef END_BSLMA_EXCEPTION_TEST
#define END_BSLMA_EXCEPTION_TEST BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
#endif

#endif  // BDE_OPENSOURCE_PUBLICATION -- BACKWARD_COMPATIBILITY

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
