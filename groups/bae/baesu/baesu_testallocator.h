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
// that implements the 'bslma_Allocator' protocol.  It requires another
// allocator of type 'bslma::Allocator' to be passed to it at construction.
// The underlying allocator supplies all memory, but the 'baesu_TestAllocator'
// layer keeps track of the segments allocated until they are freed, so a
// report can be generated at any time using the 'reportNumBlocksInUse' method
// listing all unfreed blocks, along with the stack traces as of the point
// where they were allocated.
//..
//                          ,-------------------.
//                         ( baesu_TestAllocator )
//                          `-------------------'
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
//  baesu_TestAllocator ta("myTestAllocator");
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
//  1001 segments in allocator 'myTestAllocator' in use.
//  Segments allocated in 2 place(s).
//  ----------------------------------------------------------------------------
//  Allocation place 1, 1 segments in use.
//  Stack trace at allocation time:
//  (0): .usageRecurser(int*,BloombergLP::bslma::Allocator*)+0x84 at 0x1001e724
//      source:baesu_testallocator.t.cpp:155 in baesu_testallocator.t.dbg_exc_mt
//  (1): .main+0x1b4 at 0x1000090c source:baesu_testallocator.t.cpp:398 in
//       baesu_testallocator.t.dbg_exc_mt
//  (2): .__start+0x6c at 0x1000020c source:crt0main.s in
//        baesu_testallocator.t.dbg_exc_mt
//  ----------------------------------------------------------------------------
//  Allocation place 2, 1000 segments in use.
//  Stack trace at allocation time:
//  (0): .usageTop(int*,BloombergLP::bslma::Allocator*)+0x70 at 0x1001e7f8
//      source:baesu_testallocator.t.cpp:146 in baesu_testallocator.t.dbg_exc_mt
//  (1): .usageRecurser(int*,BloombergLP::bslma::Allocator*)+0xa8 at 0x1001e748
//      source:baesu_testallocator.t.cpp:161 in baesu_testallocator.t.dbg_exc_mt
//  (2): .usageRecurser(int*,BloombergLP::bslma::Allocator*)+0x94 at 0x1001e734
//      source:baesu_testallocator.t.cpp:158 in baesu_testallocator.t.dbg_exc_mt
//  (3): .usageRecurser(int*,BloombergLP::bslma::Allocator*)+0x94 at 0x1001e734
//      source:baesu_testallocator.t.cpp:158 in baesu_testallocator.t.dbg_exc_mt
//  (4): .usageRecurser(int*,BloombergLP::bslma::Allocator*)+0x94 at 0x1001e734
//      source:baesu_testallocator.t.cpp:158 in baesu_testallocator.t.dbg_exc_mt
//  (5): .main+0x1b4 at 0x1000090c source:baesu_testallocator.t.cpp:398 in
//      baesu_testallocator.t.dbg_exc_mt
//  (6): .__start+0x6c at 0x1000020c source:crt0main.s in
//       baesu_testallocator.t.dbg_exc_mt
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

class baesu_TestAllocator_List;

class baesu_TestAllocator : public bslma::ManagedAllocator {
    // PRIVATE TYPES
    struct VoidStarHash {
        // CLASS METHODS
        bsl::size_t operator()(const void *x) const;
            // Hash function for pointer.  At the time of this writing,
            // 'bsl::hash<void *>' wasn't very good.
    };

    typedef bsl::hash_set<void *, VoidStarHash> AllocatedSegmentHashSet;

    // DATA
    mutable bcemt_Mutex       d_mutex;          // mutex used to synchronize
                                                // access to this object

    const char               *d_name;           // name of this allocator
                                                // (held, not owned)

    int                       d_segFrames;      // max number of stack trace
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

    AllocatedSegmentHashSet   d_allocatedSegments;  // hash table of
                                                    // allocated segments not
                                                    // deallocated yet

    void                    **d_rawTraceBuffer; // buffer for doing stack
                                                // traces

    bslma::Allocator         *d_allocator_p;    // held, not owned

  private:
    // NOT IMPLEMENTED
    baesu_TestAllocator(const baesu_TestAllocator&);
    baesu_TestAllocator& operator=(const baesu_TestAllocator&);

  public:
    // CREATORS
    explicit
    baesu_TestAllocator(bsl::ostream    *ostream                 = &bsl::cout,
                        int              segFrames               = 8,
                        bool             demanglingPreferredFlag = true,
                        bslma_Allocator *basicAllocator          = 0);
    explicit
    baesu_TestAllocator(const char      *name,
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
        // trace, but will consume more cpu time per allocation and more memory.
        // Optionally specify 'demanglingPreferredFlag' to indicate whether
        // demangling is desired.  Demangling defaults to enabled.
        // 'demanglingPreferredFlag' signals a preference only, on some
        // platforms demangling is not implemented or not optional, and on
        // those platforms the flag is ignored.  Optionally specify a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0,
        // the 'bslma::MallocFreeAllocator' singleton is used.  The behavior is
        // undefined unless the lifetimes of 'name', 'ostream', and
        // 'basicAllocator' are all longer than the lifetime of this object.

    virtual ~baesu_TestAllocator();
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

//=============================================================================
//                            Inline Function Definition
//=============================================================================

                        // ---------------------------------
                        // baesu_TestAllocator::VoidStarHash
                        // ---------------------------------
                        
// CLASS METHODS
inline
bsl::size_t baesu_TestAllocator::VoidStarHash::operator()(const void *x) const
{
    // This is written with the assumption that we don't know the imp of the
    // hash table.  A hash table will generally mode the result of the hash
    // function to index into an array.  It may always keep the table a power
    // of 2 long, so that the mod operation can be achieved with '&' rather
    // than '%', since '%' can be very slow.
    //
    // If the table is a power of 2 long, it is vital that there be variation
    // in the low order bits of the result.  If you're hashing on pointers that
    // point to structs containing doubles allocated by 'malloc', then on most
    // architectures all those pointers will be 8 byte aligned, so the low
    // order 3 bits of the point will always be zero.  This will result in
    // 7/8 of the slots in your hash table being empty -- EXTREMELY
    // sub-optimal.
    //
    // We want our hash function to be fast, so using '*', '/', '%', or any
    // further subroutine calls it out.  As much of the calculation as possible
    // should be done at compile time.  'If's should be avoided.
    //
    // The obvious thing to do is shift the bits down an amount so that the
    // low order zero bits are shifted out the bottom.  It is bad form to just
    // throw bits away, so the low order bits we will shift to the top.
    //
    // However, if the pointers passed are NOT to malloc'ed structs containing
    // doubles, but are pointers into an array of char, then there WAS useful
    // variation in those low order bits.  And if the hash table length is a
    // low power of two, they will be ignored.  So we add in the raw value of
    // the pointer again, to get those bits back into varying the bits that
    // the hash table will pay attention to.

    enum { MAX_ALIGNMENT = bsls::AlignmentUtil::BSLS_MAX_ALIGNMENT };

    BSLMF_ASSERT(MAX_ALIGNMENT <= 0x100);
    BSLMF_ASSERT(0 == (MAX_ALIGNMENT & (MAX_ALIGNMENT - 1)));      // 1 bit set

    enum { SHIFT_DOWN =   1 == MAX_ALIGNMENT ? 0
                      :   2 == MAX_ALIGNMENT ? 1
                      :   4 == MAX_ALIGNMENT ? 2
                      :   8 == MAX_ALIGNMENT ? 3
                      :  16 == MAX_ALIGNMENT ? 4
                      :  32 == MAX_ALIGNMENT ? 5
                      :  64 == MAX_ALIGNMENT ? 6
                      : 128 == MAX_ALIGNMENT ? 7
                      :                        8,

           SHIFT_UP   = 0 == SHIFT_DOWN ? 0
                                        : sizeof(void *) * 8 - SHIFT_DOWN
    };

    BSLMF_ASSERT(1 == (MAX_ALIGNMENT >> SHIFT_DOWN));

    // Note we want 'SHIFT_MASK' to be in a separate enum than the other 2
    // because on 64 bit architectures, it will be a 64 bit quantity.

    enum { SHIFT_MASK = ((bsls::Types::UintPtr) 0 - (SHIFT_DOWN > 0)) };

    bsls::Types::UintPtr y = (bsls::Types::UintPtr) x;
    return ((y >> SHIFT_DOWN) | (y << SHIFT_UP)) + (y & SHIFT_MASK);
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
