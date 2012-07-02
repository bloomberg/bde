// baesu_stacktracetestallocator.h                                    -*-C++-*-
#ifndef INCLUDED_BAESU_STACKTRACETESTALLOCATOR
#define INCLUDED_BAESU_STACKTRACETESTALLOCATOR

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide an allocator with leak detection coupled with stack trace
//
//@CLASSES:
//   baesu_StackTraceTestAllocator: allocation w/ leak detection / stack trace
//
//@AUTHOR: Bill Chapman (bchapman2)
//
//@DESCRIPTION: This component provides an allocator,
// 'baesu_StackTraceTestAllocator' that implements the 'bslma_Allocator'
// protocol.  It requires another allocator of type 'bslma::Allocator' to be
// passed to it at construction.  The underlying allocator supplies all memory,
// but the 'baesu_StackTraceTestAllocator' layer keeps track of the segments
// allocated until they are freed, so a report can be generated at any time
// using the 'reportNumBlocksInUse' method listing all unfreed blocks, along
// with the stack traces as of the point where they were allocated.
//..
//                    ,------------------------------.
//                   ( baesu_StackTraceTestAllocator )
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
// Note that allocation using this component is deliberately incompatible with
// the default global 'new' ('malloc') and 'delete' ('free').  Using 'delete'
// to free memory supplied by this allocator will corrupt the dynamic memory
// manager and also cause a memory leak (and will be reported by purify as
// freeing mismatched memory, freeing unallocated memory, and as a memory
// leak).  Using 'deallocate' to free memory supplied by global 'new' will
// immediately cause an error to be reported to the associated 'ostream' and,
// unless the 'noAbort' flag is set, an abort.
//
///NO-ABORT MODE
///-------------
// (Defaults 0) Specifies that all abort statements are replaced by return
// statements without suppressing diagnostics.
//
///USAGE
///-----
// First, we create a call stack where memory is allocated, and leaked, in 2
// places:
//..
//  void usageTop(int *depth, bslma::Allocator *alloc)
//  {
//      if (!*depth) {
//          for (int i = 0; i < 1000; ++i) {
//              (void) alloc->allocate(100);    // memory leaked here 1000 times
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
//      ++*depth;   // Prevent compiler from optimizing tail recursion as a loop
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
//  ASSERT(1001 == ta.numBlocksInUse());
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
//      ASSERT(0 == ta.numBlocksInUse());
//..
// Now that there are no segments leaked, we confirm this by doing a report of
// outstanding segments to a stringstream and verifying that no output was
// written:
//..
//      bsl::stringstream ss;
//      ta.reportBlocksInUse(&ss);
//      ASSERT(ss.str().empty());
//  }
//..
// Finally, if 'veryVerbose' was set and we didn't call 'ta.release()', when
// 'ta' goes out of scope and is destroyed, an error condtion will occur, and a
// report of leaked segments will be written to the 'ostream' passed to 'ta' at
// construction (it defaulted to 'cout') and 'abort()' will be called.  Output:
// (long lines wrapped for aesthetics)
//..
//  ============================================================================
//  Error: memory leaked:
//  1001 segment(s) in allocator 'myTestAllocator' in use.
//  Segment(s) allocated in 2 place(s).
//  ----------------------------------------------------------------------------
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
//  ----------------------------------------------------------------------------
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
    // PRIVATE TYPES
    struct SegmentHdr {
        // DATA
        baesu_StackTraceTestAllocator *d_allocator;    // creator of segment

        SegmentHdr                    *d_next;         // doubly linked list
        SegmentHdr                    *d_prev;         // ''

        void                          *d_magic;        // Ptr so will show in
                                                       // hex in debugger
        // CREATOR
        SegmentHdr(baesu_StackTraceTestAllocator *alloc,
                   SegmentHdr                    *next,
                   SegmentHdr                    *prev,
                   void                          *magic);
    };

    // DATA
    SegmentHdr                d_headNode;       // Headnode of segment headers
                                                // list.

    mutable bcemt_Mutex       d_mutex;          // mutex used to synchronize
                                                // access to this object

    const char               *d_name;           // name of this allocator
                                                // (held, not owned)

    const int                 d_segFrames;      // max number of stack trace
                                                // frames to store in each
                                                // segment.

    int                       d_rawTraceFrames; // total number of frames
                                                // traced, including ignored
                                                // frames.  Of these frames,
                                                // 'd_segFrames' frames will be
                                                // copied to the allocated
                                                // segments.

    bsl::ostream             *d_ostream;        // stream to which reports are
                                                // to be written.  Held, not
                                                // owned.

    bool                      d_demangle;       // is demangling requested?

    volatile bool             d_noAbortFlag;    // do we abort after delivering
                                                // a report on error?

    void                    **d_rawTraceBuffer; // buffer for doing stack
                                                // traces

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
        // Check sanity of headnode.

    int preDeallocateCheckSegmentHdr(const SegmentHdr *segmentHdr) const;
        // Check sanity of segment header

  public:
    // CREATORS
    explicit
    baesu_StackTraceTestAllocator(
                        bsl::ostream    *ostream                 = &bsl::cout,
                        int              segFrames               = 8,
                        bool             demanglingPreferredFlag = true,
                        bslma_Allocator *basicAllocator          = 0);
    explicit
    baesu_StackTraceTestAllocator(
                        const char      *name,
                        bsl::ostream    *ostream                 = &bsl::cout,
                        int              segFrames               = 8,
                        bool             demanglingPreferredFlag = true,
                        bslma_Allocator *basicAllocator          = 0);
        // Create a test allocator.  Optionally specify 'name' as the name of
        // the allocator, to be used in reports and error messages.  Optionally
        // specify 'ostream', the stream to which error messages are to be
        // reported.  If 'ostream' is not specified, 'bsl::cout' is used.
        // Optionally specify 'segFrames', the number of stack trace frame
        // pointers to be saved for every allocation, which, if not specified,
        // is '8'.  Specifying a larger number can give a more complete stack
        // trace, but will consume more cpu time per allocation and more
        // memory.  Optionally specify 'demanglingPreferredFlag' to indicate
        // whether demangling is desired.  Demangling defaults to enabled.
        // 'demanglingPreferredFlag' signals a preference only, on some
        // platforms demangling is not implemented or not optional, and on
        // those platforms the flag is ignored.  Optionally specify a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0,
        // the 'bslma::MallocFreeAllocator' singleton is used.  The behavior is
        // undefined unless the lifetimes of 'name', 'ostream', and
        // 'basicAllocator' are all longer than the lifetime of this object.

    virtual ~baesu_StackTraceTestAllocator();
        // Destroy this allocator.  Report any memory leaks to the 'ostream'
        // that was supplied at construction.  If no leaking occurred, nothing
        // is written to the output 'ostream'.  Abort if 'numBlocksInUse() > 0'
        // unless the 'noAbort' flag is set.  Note that if the 'noAbort' flag
        // is set, all outstanding memory blocks will be released, but the
        // report will still be written to 'ostream'.

    // MANIPULATORS
    virtual void *allocate(size_type size);
        // Return a newly allocated block of memory of the specified positive
        // 'size' (in bytes).  If 'size' is 0, a null pointer is returned with
        // no other other effect.  Otherwise, invoke the 'allocate' method of
        // the allocator supplied at construction and keep track of the
        // allocated segment so we know whether it has been freed.

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
        // errors is suppressed, and the functions simply return.  Diagnostic
        // reports are not affected.  Note that this function is provided
        // primarily to enable visual testing of bizarre error messages in this
        // component.  The value of the 'noAbort' flag is 'false' at
        // construction.

    // ACCESSORS
    bool isNoAbort() const;
        // Return the value of the 'noAbort' flag;

    bsl::size_t numBlocksInUse() const;
        // Return the number of segments that have been allocated and are not
        // yet freed.  Note that this is not guaranteed to execute in constant
        // time.

    void reportBlocksInUse(bsl::ostream *ostream = 0) const;
        // Write a report to the output stream associated with this test
        // allocator, giving stack traces for segments that have been allocated
        // and are not yet freed.  If the destructor finds that any segments
        // have been leaked, this method is called.
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
