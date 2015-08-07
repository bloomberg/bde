// balst_stacktracetestallocator.cpp                                  -*-C++-*-
#include <balst_stacktracetestallocator.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(balst_stacktracetestallocator_cpp,"$Id$ $CSID$")

#include <balst_stackaddressutil.h>
#include <balst_stacktrace.h>
#include <balst_stacktraceutil.h>

#include <bdlqq_lockguard.h>

#include <bslma_allocator.h>
#include <bslma_mallocfreeallocator.h>
#include <bslmf_assert.h>
#include <bsls_alignmentutil.h>
#include <bsls_alignmentfromtype.h>
#include <bsls_assert.h>
#include <bsls_platform.h>
#include <bsls_types.h>

#include <bsl_algorithm.h>
#include <bsl_cstdlib.h>
#include <bsl_iostream.h>
#include <bsl_map.h>
#include <bsl_new.h>
#include <bsl_vector.h>
#include <bsl_utility.h>

namespace {

typedef BloombergLP::balst::StackAddressUtil AddressUtil;

enum {
    IGNORE_FRAMES = AddressUtil::BALST_IGNORE_FRAMES,
        // On some platforms, gathering the stack pointers wastes one frame
        // gathering the address of 'AddressUtil::getStackAddresses', which is
        // reflected in whether 'AddressUtil::BALST_IGNORE_FRAMES' is 0 or 1.
        // This constant allows us to adjust for this and not display that
        // frame as it is of no interest to the user.

    MAX_ALIGNMENT = BloombergLP::bsls::AlignmentUtil::BSLS_MAX_ALIGNMENT,

    DEFAULT_NUM_RECORDED_FRAMES = 12
};

BSLMF_ASSERT(0 == MAX_ALIGNMENT % sizeof(void *));

typedef BloombergLP::bsls::Types::UintPtr UintPtr;

template <int A, int B>
struct Max {
    // Calculate, at compile time, the maximum of two 'int' values.

    enum { VALUE = A > B ? A : B };
};

}  // close unnamed namespace

// Magic number.  This must be a const variable, since AIX does not support 64
// bit enums.

#ifdef BSLS_PLATFORM_CPU_64_BIT
static
const UintPtr HIGH_ONES = (UintPtr) 1111111111 * 10 * 1000 * 1000 * 1000;
#else
static
const UintPtr HIGH_ONES = 0;
#endif

static const UintPtr ALLOCATED_BLOCK_MAGIC   = 1222222221 + HIGH_ONES;
static const UintPtr DEALLOCATED_BLOCK_MAGIC = 1999999991 + HIGH_ONES;

                            // ----------------
                            // static functions
                            // ----------------

static
int getTraceBufferLength(int specifiedMaxRecordedFrames)
    // Return the size, in pointers, of buffer space that must be reserved for
    // each block for storing frame pointers, given a specified
    // 'specifiedMaxRecordedFrames'.  The specify value may need to be
    // adjusted upward to include room for ignored frames, and for the buffer
    // size in bytes being a multiple of 'MAX_ALIGNMENT'.
{
    enum { PTRS_PER_MAX = MAX_ALIGNMENT / sizeof(void *) };

    int ret = ((specifiedMaxRecordedFrames + IGNORE_FRAMES + PTRS_PER_MAX - 1)
                                                / PTRS_PER_MAX) * PTRS_PER_MAX;

    BSLS_ASSERT(ret >= specifiedMaxRecordedFrames);
    BSLS_ASSERT(0 == ret % PTRS_PER_MAX);

    return ret;
}

namespace BloombergLP {

namespace balst {
                // ================================================
                // class StackTraceTestAllocator::BlockHeader
                // ================================================

struct StackTraceTestAllocator::BlockHeader {
    // A record of this type is stored in each block, after the stack pointers
    // and immediately before the user area of memory in the block.  These
    // 'BlockHeader' objects form a doubly linked list consisting of all blocks
    // which are unfreed.
    //
    // Note that the 'd_magic' and 'd_allocator_p' fields are at the end of the
    // 'BlockHeader', putting them adjacent to the client's area of memory,
    // making them the most likely fields to be corrupted.  A corrupted
    // 'd_allocator_p' or especially a corrupted 'd_magic' are much more likely
    // to be properly diagnosed by the allocator with a meaningful error
    // message and no segfault than a corrupted 'd_next_p' or 'd_prevNext_p'.

    // DATA
    BlockHeader                   *d_next_p;      // next object in the
                                                  // doubly-linked list

    BlockHeader                  **d_prevNext_p;  // pointer to the 'd_next_p'
                                                  // field of the previous
                                                  // object, or the head ptr of
                                                  // the linked list if there
                                                  // is no previous object

    StackTraceTestAllocator *d_allocator_p; // allocator of block

    UintPtr                        d_magic;       // magic number -- has
                                                  // different values for
                                                  // an allocated block vs a
                                                  // freed block

    // CREATOR
    BlockHeader(BlockHeader                   *next,
                BlockHeader                  **prevNext,
                StackTraceTestAllocator *stackTraceTestAllocator,
                UintPtr                        magic);
        // Create a block here, populating the fields with the specified
        // 'next', 'prevNext', 'stackTraceTestAllocator', and 'magic'
        // arguments.
};
}  // close package namespace


// CREATORS
inline
balst::StackTraceTestAllocator::BlockHeader::BlockHeader(
                       BlockHeader                   *next,
                       BlockHeader                  **prevNext,
                       balst::StackTraceTestAllocator *stackTraceTestAllocator,
                       UintPtr                        magic)
: d_next_p(next)
, d_prevNext_p(prevNext)
, d_allocator_p(stackTraceTestAllocator)
, d_magic(magic)
{
    BSLMF_ASSERT(0 == sizeof(BlockHeader) % MAX_ALIGNMENT);
    BSLMF_ASSERT(sizeof(BlockHeader) == 4 * sizeof(void *));
}

namespace balst {
                // ------------------------------------------------
                // class StackTraceTestAllocator::BlockHeader
                // ------------------------------------------------

// PRIVATE ACCESSORS
int StackTraceTestAllocator::checkBlockHeader(
                                             const BlockHeader *blockHdr) const
{
    int rc = 0;

    if (ALLOCATED_BLOCK_MAGIC != blockHdr->d_magic) {
        if (DEALLOCATED_BLOCK_MAGIC == blockHdr->d_magic) {
            // Note: if '*d_ostream' is a stringstream based on this allocator,
            // this write may allocate the freed block, trashing '*blockHdr',
            // not much we can do about that.

            *d_ostream << "Error: block at " << &blockHdr[1]
                       << " freed second time by allocator '" << d_name
                       << "'\n";
        }
        else {
            *d_ostream << "Error: corrupted block at "
                       << &blockHdr[1] << " attempted to be freed by"
                       << " allocator '" << d_name << "'\n";
        }

        return -1;                                                    // RETURN
    }

    if (this != blockHdr->d_allocator_p) {
        StackTraceTestAllocator *otherAlloc = blockHdr->d_allocator_p;
        bool notOurs = STACK_TRACE_TEST_ALLOCATOR_MAGIC != otherAlloc->d_magic;
        *d_ostream << "Error: attempt to free block by wrong"
                   << " allocator.\n    Block belongs to allocator '"
                   << (notOurs ? "<<Not a StackTraceTestAllocator>>"
                               : otherAlloc->d_name)
                   << "'\n    Attempted to free by allocator '"
                   << d_name << "'\n";

        if (notOurs) {
            return -1;                                                // RETURN
        }

        rc = -1;
    }

    if (0 == d_blocks || 0 == blockHdr->d_prevNext_p ||
                                                0 == *blockHdr->d_prevNext_p) {
        *d_ostream << "Error: block at " << &blockHdr[1]
                   << " corrupted: unexpected null list ptr(s) encountered\n";

        return -1;                                                    // RETURN
    }

    // check out next node

    const BlockHeader *next = blockHdr->d_next_p;
    if (next) {
        if (ALLOCATED_BLOCK_MAGIC != next->d_magic) {
            if (DEALLOCATED_BLOCK_MAGIC == next->d_magic) {
                *d_ostream << "Error: freed object on allocted block list"
                           << " of allocator '" << d_name << "' at "
                           << next << bsl::endl;
            }
            else {
                *d_ostream << "Error: block list of allocator '"
                           << d_name << "' corrupted, bad magic number: "
                           << next->d_magic << bsl::endl;
            }

            rc = -1;
        }
    }

    return rc;
}

// CLASS METHODS
void StackTraceTestAllocator::failAbort()
{
    bsl::abort();
}

void StackTraceTestAllocator::failNoop()
{
    // do nothing
}

// CREATORS
StackTraceTestAllocator::StackTraceTestAllocator(
                                              bslma::Allocator *basicAllocator)
: d_magic(STACK_TRACE_TEST_ALLOCATOR_MAGIC)
, d_numBlocksInUse(0)
, d_blocks(0)
, d_mutex()
, d_name("<unnamed>")
, d_failureHandler(basicAllocator ? basicAllocator
                                  : &bslma::MallocFreeAllocator::singleton())
, d_maxRecordedFrames(DEFAULT_NUM_RECORDED_FRAMES + IGNORE_FRAMES)
, d_traceBufferLength(getTraceBufferLength(DEFAULT_NUM_RECORDED_FRAMES))
, d_ostream(&bsl::cerr)
, d_demangleFlag(true)
, d_allocator_p(basicAllocator ? basicAllocator
                               : &bslma::MallocFreeAllocator::singleton())
{
    BSLS_ASSERT_SAFE(d_maxRecordedFrames >= DEFAULT_NUM_RECORDED_FRAMES);
    BSLS_ASSERT_SAFE(d_traceBufferLength >= d_maxRecordedFrames);

    // This must be assigned in a statement in the body of the c'tor rather
    // than in the initializer list to work around a microsoft bug with
    // function pointers.

    d_failureHandler = &failAbort;
}

StackTraceTestAllocator::StackTraceTestAllocator(
                                           int               numRecordedFrames,
                                           bslma::Allocator *basicAllocator)
: d_magic(STACK_TRACE_TEST_ALLOCATOR_MAGIC)
, d_numBlocksInUse(0)
, d_blocks(0)
, d_mutex()
, d_name("<unnamed>")
, d_failureHandler(basicAllocator ? basicAllocator
                                  : &bslma::MallocFreeAllocator::singleton())
, d_maxRecordedFrames(numRecordedFrames + IGNORE_FRAMES)
, d_traceBufferLength(getTraceBufferLength(numRecordedFrames))
, d_ostream(&bsl::cerr)
, d_demangleFlag(true)
, d_allocator_p(basicAllocator ? basicAllocator
                               : &bslma::MallocFreeAllocator::singleton())
{
    BSLS_ASSERT_OPT(numRecordedFrames >= 2);
    BSLS_ASSERT(d_maxRecordedFrames >= numRecordedFrames);
    BSLS_ASSERT(d_traceBufferLength >= d_maxRecordedFrames);

    // This must be assigned in a statement in the body of the c'tor rather
    // than in the initializer list to work around a microsoft bug with
    // function pointers.

    d_failureHandler = &failAbort;
}

StackTraceTestAllocator::~StackTraceTestAllocator()
{
    if (numBlocksInUse() > 0) {
        *d_ostream << "======================================================="
                   << "========================\nError: memory leaked:\n";

        reportBlocksInUse();

        d_failureHandler();

        release();
    }
}

// MANIPULATORS
void *StackTraceTestAllocator::allocate(size_type size)
{
    if (0 == size) {
        return 0;                                                     // RETURN
    }

    bdlqq::LockGuard<bdlqq::Mutex> guard(&d_mutex);

    // The underlying allocator might align the block differently depending on
    // the size passed.  The alignment must be large enough to accommodate the
    // stack addresses (type 'void *') in the buffer, it must be large enough
    // to accommodate the 'BlockHeader's alignment requirements, and it must be
    // large enough to accommodate whatever the alignment requirements of
    // whatever the client intends to store in their portion of the block.  We
    // can infer the requirements of our pointers and block header at compile
    // time in 'FIXED_ALIGN', then we infer the alignment requirement of the
    // client's section from the size passed, and take the maximum of the two
    // to get the alignment required.  We then round the size we will pass to
    // the underlying allocator up to a multiple of 'align', so that the
    // underlying allocator cannot infer a lower value of the alignment
    // requirement.

    enum { FIXED_ALIGN =
                   Max<bsls::AlignmentFromType<void *>::VALUE,
                       bsls::AlignmentFromType<BlockHeader>::VALUE>::VALUE };
    const int align = bsl::max<int>(
                        FIXED_ALIGN,
                        bsls::AlignmentUtil::calculateAlignmentFromSize(size));
    const int lowBits = align - 1;
    BSLS_ASSERT_SAFE(0 == (align & lowBits));   // verify 'align' is power of 2
    size = (size + lowBits) & ~lowBits;     // round 'size' up to multiple of
                                            // 'align'

    void **framesBegin = (void **) d_allocator_p->allocate(
          d_traceBufferLength * sizeof(void *) + sizeof(BlockHeader) + size);

    BlockHeader *blockHdr = (BlockHeader*) (framesBegin + d_traceBufferLength);

    new (blockHdr) BlockHeader(d_blocks,
                               &d_blocks,
                               this,
                               ALLOCATED_BLOCK_MAGIC);
    if (d_blocks) {
        d_blocks->d_prevNext_p = &blockHdr->d_next_p;
    }
    d_blocks = blockHdr;

    bsl::fill(framesBegin, framesBegin + d_maxRecordedFrames, (void *) 0);
    AddressUtil::getStackAddresses(framesBegin, d_maxRecordedFrames);

    void *ret = blockHdr + 1;

    BSLS_ASSERT(0 == ((UintPtr) ret & ((sizeof(void *) - 1) | lowBits)));

    ++d_numBlocksInUse;

    return ret;
}

void StackTraceTestAllocator::deallocate(void *address)
{
    if (!address) {
        return;                                                       // RETURN
    }

    if (0 != ((UintPtr) address & (sizeof(void *) - 1))) {
        // Badly aligned, can't be a block we allocated.

        *d_ostream << "Badly aligned block passed to allocator '"
                   << d_name << "' must have been allocated by another type"
                   << " of allocator\n";
        d_failureHandler();

        return;                                                       // RETURN
    }

    BlockHeader *blockHdr = (BlockHeader *) address - 1;

    bdlqq::LockGuard<bdlqq::Mutex> guard(&d_mutex);

    if (checkBlockHeader(blockHdr)) {
        guard.release()->unlock();
        d_failureHandler();

        return;                                                       // RETURN
    }

    if (blockHdr->d_next_p) {
        blockHdr->d_next_p->d_prevNext_p = blockHdr->d_prevNext_p;
    }
    *blockHdr->d_prevNext_p = blockHdr->d_next_p;
    blockHdr->d_magic = DEALLOCATED_BLOCK_MAGIC;

    d_allocator_p->deallocate((void **) blockHdr - d_traceBufferLength);

    --d_numBlocksInUse;
    BSLS_ASSERT(d_numBlocksInUse >= 0);
}

void StackTraceTestAllocator::release()
{
    bdlqq::LockGuard<bdlqq::Mutex> guard(&d_mutex);

    for (BlockHeader *blockHdr = d_blocks;
                                     blockHdr; blockHdr = blockHdr->d_next_p) {
        if (checkBlockHeader(blockHdr)) {
            guard.release()->unlock();
            d_failureHandler();

            return;                                                   // RETURN
        }
    }

    int numBlocks = 0;
    for (BlockHeader *blockHdr = d_blocks; blockHdr; ) {
        ++numBlocks;

        BlockHeader *condemned = blockHdr;
        blockHdr = blockHdr->d_next_p;

        d_allocator_p->deallocate((void **) condemned - d_traceBufferLength);
    }

    d_blocks = 0;

    BSLS_ASSERT(numBlocks == d_numBlocksInUse);
    d_numBlocksInUse = 0;
}

void StackTraceTestAllocator::setDemanglingPreferredFlag(bool value)
{
    d_demangleFlag = value;
}

void StackTraceTestAllocator::setFailureHandler(
                                        const bdlf::Function<void (*)()>& func)
{
    bdlqq::LockGuard<bdlqq::Mutex> guard(&d_mutex);

    d_failureHandler = func;
}

void StackTraceTestAllocator::setName(const char *name)
{
    BSLS_ASSERT(0 != name);

    bdlqq::LockGuard<bdlqq::Mutex> guard(&d_mutex);

    d_name = name;
}

void StackTraceTestAllocator::setOstream(bsl::ostream *ostream)
{
    BSLS_ASSERT(0 != ostream);

    bdlqq::LockGuard<bdlqq::Mutex> guard(&d_mutex);

    d_ostream = ostream;
}

// ACCESSORS
void StackTraceTestAllocator::reportBlocksInUse(bsl::ostream *ostream) const
{
    typedef bsl::vector<const void *>    StackTraceVec;
    typedef bsl::map<StackTraceVec, int> StackTraceVecMap;

    bdlqq::LockGuard<bdlqq::Mutex> guard(&d_mutex);

    if (0 == ostream) {
        ostream = d_ostream;
    }

    if (0 == d_numBlocksInUse) {
        return;                                                       // RETURN
    }

    *ostream << d_numBlocksInUse << " block(s) in allocator '"
             << d_name << "' in use.\n";

    StackTraceVecMap stackTraceVecMap(d_allocator_p);
    StackTraceVec traceVec(d_allocator_p);

    int numBlocksInUse = 0;
    for (BlockHeader *blockHdr = d_blocks; blockHdr;
                                               blockHdr = blockHdr->d_next_p) {
        if (ALLOCATED_BLOCK_MAGIC != blockHdr->d_magic) {
            if (DEALLOCATED_BLOCK_MAGIC == blockHdr->d_magic) {
                *ostream << "StackTraceTestAllocator: freed block at "
                         << (blockHdr + 1) << " in allocated list\n";
            }
            else {
                *ostream << "StackTraceTestAllocator: memory corruption"
                         << " detected, possible buffer underrun\n";

                // We don't want to go on traversing the list because we don't
                // even know that 'blockHdr' points at a 'blockHdr', so we
                // can't trust 'blockHdr->d_next_p'.

                break;
            }
        }

        void **startTrace = (void **) blockHdr - d_traceBufferLength;
        void **endTrace   = startTrace + d_maxRecordedFrames;
        startTrace += IGNORE_FRAMES;

        // trim off the zeroes at the end of the trace

        while (endTrace > startTrace && 0 == endTrace[-1]) {
            --endTrace;
        }

        traceVec.clear();
        traceVec.insert(traceVec.end(), startTrace, endTrace);

        // We avoid using:
        //..
        //  ++stackTraceVecMap[traceVec];
        //..
        // Because 'bsl::map' uses the default allocator to create a temporary
        // object in 'operator[]', and we want to avoid using the default
        // allocator in a test allocator since the client may be debugging
        // memory issues, and may want to closely monitor traffic on the
        // default allocator.  What's more, *THIS* *ALLOCATOR* may be the
        // default allocator, which would cause a mutex deadlock.

        StackTraceVecMap::iterator stmit = stackTraceVecMap.find(traceVec);
        if (stackTraceVecMap.end() == stmit) {
            StackTraceVecMap::value_type pr(traceVec, 1, d_allocator_p);
            stackTraceVecMap.insert(pr);
        }
        else {
            ++stmit->second;
        }

        ++numBlocksInUse;
    }
    BSLS_ASSERT(d_numBlocksInUse == numBlocksInUse);

    *ostream << "Block(s) allocated from "
             << stackTraceVecMap.size() << " trace(s).\n";

    int place = 0;
    StackTrace st(d_allocator_p);
    const StackTraceVecMap::iterator mapend = stackTraceVecMap.end();
    for (StackTraceVecMap::iterator it = stackTraceVecMap.begin();
                                                      mapend != it; ++it) {
        *ostream << "------------------------------------------"
                 << "-------------------------------------\n"
                 << "Allocation trace " << ++place << ", "
                 << it->second << " block(s) in use.\n"
                 << "Stack trace at allocation time:\n";

        int rc = StackTraceUtil::loadStackTraceFromAddressArray(
                                                       &st,
                                                       it->first.begin(),
                                                       (int) it->first.size(),
                                                       d_demangleFlag);
        if (rc || 0 == st.length()) {
            *ostream << "... stack trace failed ...\n";
        }
        else {
            StackTraceUtil::printFormatted(*ostream, st);
        }
        st.removeAll();
    }
}
}  // close package namespace

}  // close enterprise namespace

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2012
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
