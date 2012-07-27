// baesu_stacktracetestallocator.h                                    -*-C++-*-
#ifndef INCLUDED_BAESU_STACKTRACETESTALLOCATOR
#define INCLUDED_BAESU_STACKTRACETESTALLOCATOR

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide an allocator that reports the call stack for leaks.
//
//@CLASSES:
//   baesu_StackTraceTestAllocator: reports the call stack for memory leaks.
//
//@AUTHOR: Bill Chapman (bchapman2)
//
//@DESCRIPTION: This component provides an instrumented allocator,
// 'baesu_StackTraceTestAllocator' that implements the
// 'bslma::ManagedAllocator' protocol.  An object of this type records the call
// stack for each allocation performed, and can report, either using the
// 'reportBlocksInUse' method or implicitly at destruction, the call stack
// associated with every allocated block that has not (yet) been freed.  It is
// optionally supplied a 'bslma::Allocator' at construction that it uses to
// allocator memory.
//..
//                    ,------------------------------.
//                   ( baesu_StackTraceTestAllocator  )
//                    `------------------------------'
//                                    |    ctor/dtor
//                                    |    numBlocksInUse
//                                    |    reportBlocksInUse
//                                    |    setNoAbort
//                                    V
//                         ,-----------------------.
//                        ( bslma::ManagedAllocator )
//                         `-----------------------'
//                                    |    release
//                                    V
//                             ,----------------.
//                            ( bslma::Allocator )
//                             `----------------'
//                                         allocate
//                                         deallocate
//..
// Note that allocation using a 'baesu_StackTraceTestAllocator' is deliberately
// incompatible with the default global 'new', 'malloc', 'delete', and 'free'.
// Using 'delete' or 'free' to free memory supplied by this allocator will
// corrupt the dynamic memory manager and also cause a memory leak (and will be
// reported by purify as freeing mismatched memory, freeing unallocated memory,
// and as a memory leak).  Using 'deallocate' to free memory supplied by global
// 'new' or 'malloc' will immediately cause an error to be reported to the
// associated 'ostream' and, unless the 'noAbort' flag is set, an abort (core
// dump).
//
///No-Abort Mode
///-------------
// A 'baesu_StackTraceTestAllocator' object can be put into 'noAbort' mode.  If
// 'true', the stack trace test allocator won't abort if it detects user
// errors, though it will still abort if it detects a consistency error within
// its own code.  The 'noAbort' flag is 'false' at construction, and can be set
// afterward with the 'setNoAbort' manipulator.
//
///Usage
///-----
// In this section we show the intended usage of this component.
//
///Example 1:
// First, we create a call stack where memory is allocated, and leaked, in 2
// places:
//..
//  void usageTop(int *depth, bslma::Allocator *alloc)
//  {
//      if (!*depth) {
//          for (int i = 0; i < 1000; ++i) {
//              (void) alloc->allocate(100);   // memory leaked here 1000 times
//          }
//      }
//  }
//
//  void usageRecurser(int *depth, bslma::Allocator *alloc)
//  {
//      if (--*depth > 0) {
//          if (3 == *depth) {
//              (void) alloc->allocate(50);          // memory leaked here once
//          }
//
//          usageRecurser(depth, alloc);
//      }
//      else {
//          usageTop(depth, alloc);
//      }
//
//      ++*depth;   // Prevent compiler from optimizing tail recursion as a
//                  // loop
//  }
//..
// Then, in our 'main' routine, we create our stack trace test allocator.  Note
// that the stack traces will default to a depth of 8:
//..
//  baesu_StackTraceTestAllocator ta("myTestAllocator");
//..
// Next, we call 'usageRecurse' to do the allocations:
//..
//  int depth = 4;
//  usageRecurser(&depth, &ta);
//..
// Then, we verify that we have allocated 1001 blocks.
//..
//  assert(1001 == ta.numBlocksInUse());
//..
// Next, if 'verbose' but not 'veryVerbose' was selected, we do a dump of all
// currently outstanding allocated segments:
//..
//  if (verbose && !veryVerbose) {
//      ta.reportBlocksInUse(&cout);
//  }
//..
// Then, if 'veryVerbose' is not selected, we can release all outstanding
// segments with the 'release' method, since this allocator supports the
// 'bslma::ManagedAllocator' protocol, which will free all the outstanding
// memory blocks.
//..
//  if (!veryVerbose) {
//      ta.release();
//      assert(0 == ta.numBlocksInUse());
//..
// Now that there are no segments leaked, we confirm this by doing a report of
// outstanding segments to a stringstream and verifying that no output was
// written:
//..
//      bsl::stringstream ss;
//      ta.reportBlocksInUse(&ss);
//      assert(ss.str().empty());
//  }
//..
// Finally, if 'veryVerbose' was set and we didn't call 'ta.release()', when
// 'ta' goes out of scope and is destroyed, an error condtion will occur, and a
// report of leaked segments will be written to the 'ostream' passed to 'ta' at
// construction (it defaulted to 'cout') and 'abort()' will be called.  Output:
// (long lines wrapped for aesthetics)
//..
//  ===========================================================================
//  Error: memory leaked:
//  1001 segment(s) in allocator 'myTestAllocator' in use.
//  Segment(s) allocated in 2 place(s).
//  ---------------------------------------------------------------------------
//  Allocation place 1, 1 segment(s) in use.
//  Stack trace at allocation time:
//  (0): .usageRecurser(int*,BloombergLP::bslma::Allocator*)+0x84 at 0x1001b80c
//        source:baesu_stacktracetestallocator.t.cpp:155 in
//        baesu_stacktracetestallocator.t.
//  (1): .main+0x1b4 at 0x1000090c
//        source:baesu_stacktracetestallocator.t.cpp:398 in
//        baesu_stacktracetestallocator.t.
//  (2): .__start+0x6c at 0x1000020c source:crt0main.s in
//        baesu_stacktracetestallocator.t.
//  ---------------------------------------------------------------------------
//  Allocation place 2, 1000 segment(s) in use.
//  Stack trace at allocation time:
//  (0): .usageTop(int*,BloombergLP::bslma::Allocator*)+0x70 at 0x1001b8e0
//        source:baesu_stacktracetestallocator.t.cpp:146 in
//        baesu_stacktracetestallocator.t.
//  (1): .usageRecurser(int*,BloombergLP::bslma::Allocator*)+0xa8 at 0x1001b830
//        source:baesu_stacktracetestallocator.t.cpp:161 in
//        baesu_stacktracetestallocator.t.
//  (2): .usageRecurser(int*,BloombergLP::bslma::Allocator*)+0x94 at 0x1001b81c
//        source:baesu_stacktracetestallocator.t.cpp:158 in
//        baesu_stacktracetestallocator.t.
//  (3): .usageRecurser(int*,BloombergLP::bslma::Allocator*)+0x94 at 0x1001b81c
//        source:baesu_stacktracetestallocator.t.cpp:158 in
//        baesu_stacktracetestallocator.t.
//  (4): .usageRecurser(int*,BloombergLP::bslma::Allocator*)+0x94 at 0x1001b81c
//        source:baesu_stacktracetestallocator.t.cpp:158 in
//        baesu_stacktracetestallocator.t.
//  (5): .main+0x1b4 at 0x1000090c
//        source:baesu_stacktracetestallocator.t.cpp:398 in
//        baesu_stacktracetestallocator.t.
//  (6): .__start+0x6c at 0x1000020c source:crt0main.s in
//        baesu_stacktracetestallocator.t.
//  IOT/Abort trap (core dumped)
//..

#ifndef INCLUDED_BCESCM_VERSION
#include <bcescm_version.h>
#endif

#ifndef INCLUDED_BCEMT_MUTEX
#include <bcemt_mutex.h>
#endif

#ifndef INCLUDED_BSLMA_MANAGEDALLOCATOR
#include <bslma_managedallocator.h>
#endif

#ifndef INCLUDED_BSLMF_ASSERT
#include <bslmf_assert.h>
#endif

#ifndef INCLUDED_BSLS_PLATFORM
#include <bsls_platform.h>
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

class baesu_StackTraceTestAllocator : public bslma::ManagedAllocator {
    // This class defines a concrete "test" allocator mechanism that implements
    // the 'bslma::ManagedAllocator' protocol, and provides instrumentation to
    // track the set of all segments allocted by this allocator that have yet
    // to be freed.  At any time it can produce a report about such segments,
    // listing for each place that any unfreed segments were allocated
    //: o the number of unfreed segments allocated at that place
    //: o the stack trace at that place
    // The allocator will also detect redundant frees of the same segment, and
    // frees by the wrong allocator.  The client can choose whether such
    // violations are handled by a core dump, or merely a report being written.
    //
    // Note that, unlike many other allocators, this allocator does DOES NOT
    // rely on the currently installed default allocator (see 'bslma_default')
    // at all, but instead -- by default -- uses 'MallocFreeAllocator'
    // singleton, which in turn calls the C Standard Library functions 'malloc'
    // and 'free' as needed.  Clients may, however, override this allocator by
    // supplying (at construction) any other allocator implementing the
    // 'Allocator' protocol.

    typedef bsls::Types::UintPtr UintPtr;

    enum SegmentHeaderMagic {
        // Magic number stored in every 'SegmentHeader', reflecting 3 possible
        // states.

#ifdef BSLS_PLATFORM__CPU_32_BIT
        HIGH_ONES = 0,
#else
        HIGH_ONES = (UintPtr) 111111111 * 10 * 1000 * 1000 * 1000,
#endif

        UNFREED_SEGMENT_MAGIC = (UintPtr) 1222222221 + HIGH_ONES,
        FREED_SEGMENT_MAGIC   = (UintPtr) 1333333331 + HIGH_ONES,
        HEAD_NODE_MAGIC       = (UintPtr) 1555555551 + HIGH_ONES
    };

    BSLMF_ASSERT(sizeof(SegmentHeaderMagic) == sizeof(void *));

    // PRIVATE TYPES
    struct SegmentHeader {
        // A record of this type is stored in each segment, after the stack
        // pointers and immediate before the user area of memory in the
        // segment.  These 'SegmentHeader' objects form a circular linked list
        // consisting of all segments which are unfreed plus one
        // 'SegmentHeader' object within the 'baesu_StackTraceTestAllocator'
        // record, which serves as the head node for the list.
        //
        // Note that the 'd_magic' and 'd_allocator_p' fields are at
        // the end of the 'SegmentHeader', putting them adjacent to the
        // client's area of memory, making them the most likely fields to be
        // corrupted.  A corrupted 'd_allocator_p' or especially a corrupted
        // 'd_magic' are much more likely to be properly diagnosed by the
        // allocator with a meaningful error message and no segfault than a
        // corrupted 'd_next_p' or 'd_prev_p'.

        // DATA
        SegmentHeader                 *d_next_p;      // next object in the
                                                      // doubly-linked list

        SegmentHeader                 *d_prev_p;      // previous object in
                                                      // doubly-linked list.

        baesu_StackTraceTestAllocator *d_allocator_p; // creator of segment

        SegmentHeaderMagic             d_magic;       // Magic number -- has
                                                      // different values for
                                                      // an unfreed segment, a
                                                      // freed segment, or the
                                                      // head node.

        // CREATOR
        SegmentHeader(SegmentHeader                 *next,
                      SegmentHeader                 *prev,
                      baesu_StackTraceTestAllocator *stackTraceTestAllocator,
                      SegmentHeaderMagic             magic);
            // Create a segment hear, populating the fields with the specified
            // 'alloc', 'next', 'prev', and 'magic' arguments.
    };

    // DATA
    SegmentHeader             d_headNode;       // Headnode of segment headers
                                                // list.

    mutable bcemt_Mutex       d_mutex;          // mutex used to synchronize
                                                // access to this object

    const char               *d_name;           // name of this allocator
                                                // (held, not owned)

    const int                 d_maxRecordedFrames; // max number of stack trace
                                                   // frames to store in each
                                                   // segment.

    bsl::ostream             *d_ostream;        // stream to which reports are
                                                // to be written.  Held, not
                                                // owned.

    bool                      d_demangleFlag;   // if 'true' demangling of
                                                // symbol names is attempted

    volatile bool             d_noAbortFlag;    // 'true' if we are not to
                                                // abort on errors

    volatile int              d_numBlocksInUse; // number of allocated blocks
                                                // currently unfreed

    bslma::Allocator         *d_allocator_p;    // held, not owned

  private:
    // NOT IMPLEMENTED
    baesu_StackTraceTestAllocator(const baesu_StackTraceTestAllocator&);
    baesu_StackTraceTestAllocator& operator=(
                                         const baesu_StackTraceTestAllocator&);

  private:
    // PRIVATE ACCESSORS
    int checkHeadNode() const;
        // Perform sanity checks on head node, reporting any irregularities to
        // '*d_ostream'.  Return 0 if no irregularities are found, and a
        // non-zero value otherwise.

    int preDeallocateCheckSegmentHeader(
                                     const SegmentHeader *segmentHdr) const;
        // Perform sanity checks on segment header, reporting any
        // irregularities to '*d_ostream' Return 0 if no irregularities are
        // found, and a non-zero value otherwise.

  public:
    // CREATORS
    explicit
    baesu_StackTraceTestAllocator(
                         bsl::ostream    *ostream                 = &bsl::cout,
                         int              numRecordedFrames       = 12,
                         bool             demanglingPreferredFlag = true,
                         bslma_Allocator *basicAllocator          = 0);
    explicit
    baesu_StackTraceTestAllocator(
                         const char      *name,
                         bsl::ostream    *ostream                 = &bsl::cout,
                         int              numRecordedFrames       = 12,
                         bool             demanglingPreferredFlag = true,
                         bslma_Allocator *basicAllocator          = 0);
        // Create a test allocator.  Optionally specify 'name' as the name of
        // the allocator, to be used in reports and error messages.  Optionally
        // specify 'ostream', the stream to which error messages are to be
        // reported.  If 'ostream' is not specified, 'bsl::cout' is used.
        // Optionally specify 'numRecordedFrames', the number of stack trace
        // frame pointers to be saved for every allocation.  Specifying a
        // larger value of 'numRecordedFrames' means that stack traces, when
        // given, will be more complete, but will also mean that both more cpu
        // time and more memory per allocation will be consumed.  Optionally
        // specify 'demanglingPreferredFlag' to indicate whether demangling is
        // desired.  'demanglingPreferredFlag' signals a preference only, on
        // some platforms demangling is not implemented or not optional, and on
        // those platforms the flag is ignored.  Optionally specify a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0,
        // the 'bslma::MallocFreeAllocator' singleton is used, since the client
        // may be trying to avoid using the default allocator.  The behavior is
        // undefined unless 'name', 'ostream', and 'basicAllocator' remain
        // valid for the lifetime of the object, and unless
        // 'numRecordedFrames >= 2'.

    virtual ~baesu_StackTraceTestAllocator();
        // Destroy this allocator.  Report any memory leaks to the 'ostream'
        // that was supplied at construction.  If no memory leaks are observed,
        // nothing is written to the output 'ostream'.  Abort if
        // 'numBlocksInUse() > 0' unless the 'noAbort' flag is set.  Note that
        // if the 'noAbort' flag is set, all outstanding memory blocks will be
        // released, but the report will still be written to 'ostream'.

    // MANIPULATORS
    virtual void *allocate(size_type size);
        // Return a newly allocated block of memory of the specified positive
        // 'size' (in bytes).  If 'size' is 0, a null pointer is returned with
        // no other other effect.  Otherwise, invoke the 'allocate' method of
        // the allocator supplied at construction and record the returned
        // segment in order to be able to report leaked segments upon
        // destruction.

    virtual void deallocate(void *address);
        // Return the memory block at the specified 'address' back to this
        // allocator.  If 'address' is 0, this function has no effect.
        // Otherwise, if the memory at 'address' is consistent with being
        // allocated from this test allocator, deallocate it using the
        // underlying allocator and delete it from the data structures keeping
        // track of blocks in use'.  If 'address' is not zero and is not the
        // address of a segment allocated with this allocator (or if it is
        // being deallocated a second time), write an error message and, unless
        // 'noAbortFlag' is set, abort.

    virtual void release();
        // Deallocate all memory held by this allocator.

    void setNoAbort(bool flagValue);
        // Set the no-abort mode for this test allcoator to the specified
        // (boolean) 'flagValue'.  'If flagValue' is 'true', aborting on fatal
        // errors by the client is suppressed, and the functions simply return.
        // Diagnostic reports are not affected.  Note that this function is
        // provided primarily to enable testing of error messages in this
        // component without having the test driver fail in the nightly build.
        // The value of the 'noAbort' flag is 'false' at construction.

    // ACCESSORS
    bool isNoAbort() const;
        // Return the value of the 'noAbort' flag;

    bsl::size_t numBlocksInUse() const;
        // Return the number of segments that have been allocated and are not
        // yet freed.  Note that this is not guaranteed to execute in constant
        // time.

    void reportBlocksInUse(bsl::ostream *ostream = 0) const;
        // Write a report to the output stream associated with this test
        // allocator, reporting the unique call-stacks for each segment that
        // has been allocated and has not yet been freed.
};

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2012
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
