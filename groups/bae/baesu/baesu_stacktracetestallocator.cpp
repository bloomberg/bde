// baesu_stacktracetestallocator.cpp                                  -*-C++-*-
#include <baesu_stacktracetestallocator.h>

#include <bsls_ident.h>
BSLS_IDENT("$Id$ $CSID$")

#include <baesu_stackaddressutil.h>
#include <baesu_stacktraceutil.h>

#include <bcemt_lockguard.h>

#include <bslma_mallocfreeallocator.h>
#include <bsls_alignmentutil.h>
#include <bsls_assert.h>
#include <bsls_types.h>

#include <bsl_algorithm.h>
#include <bsl_map.h>
#include <bsl_vector.h>
#include <bsl_utility.h>

#include <cstdlib>  // abort

namespace BloombergLP {

namespace {

typedef baesu_StackAddressUtil AddressUtil;

enum {
    IGNORE_FRAMES = baesu_StackAddressUtil::BAESU_IGNORE_FRAMES + 1,
        // On some platforms, gathering the stack pointers wastes one frame
        // gathering the address of 'AddressUtil::getStackAddresses', which is
        // reflected in 'AddressUtil::BAESU_IGNORE_FRAMES' is 0 or 1.  Also, we
        // will always waste one frame gathering the address of 'allocate'.
        // Rather than waste one or two pointers of space in each segment
        // storing these useless pointers, we save the raw pointers into
        // 'd_rawTraceBuffer' and then copy only those pointers that are of
        // interest to the segments.

    MAX_ALIGNMENT = bsls::AlignmentUtil::BSLS_MAX_ALIGNMENT
};

typedef bsls::Types::UintPtr UintPtr;
typedef bslma::Allocator::size_type size_type;

}  // close unnamed namespace

                            // ----------------
                            // static functions
                            // ----------------

static
int getMaxRecordedFrames(int specifiedMaxRecordedFrames)
    // round the section for segment pointers up to the nearest multiple of
    // MAX_ALIGNMENT.
{
    int minRecordedFramesBytes = specifiedMaxRecordedFrames * sizeof(void *);
    int roundedUpBytes = (((minRecordedFramesBytes + MAX_ALIGNMENT - 1) /
                                               MAX_ALIGNMENT) * MAX_ALIGNMENT);

    return roundedUpBytes / sizeof(void *);
}

                   // --------------------------------------------
                   // baesu_StackTraceTestAllocator::SegmentHeader
                   // --------------------------------------------

inline
baesu_StackTraceTestAllocator::SegmentHeader::SegmentHeader(
                        SegmentHeader                 *next,
                        SegmentHeader                 *prev,
                        baesu_StackTraceTestAllocator *stackTraceTestAllocator,
                        SegmentHeaderMagic             magic)
: d_next_p(next)
, d_prev_p(prev)
, d_allocator_p(stackTraceTestAllocator)
, d_magic(magic)
{}

                        // -----------------------------
                        // baesu_StackTraceTestAllocator
                        // -----------------------------

// PRIVATE ACCESSORS
int baesu_StackTraceTestAllocator::checkHeadNode() const
{
    int rc = 0;

    if (HEAD_NODE_MAGIC != d_headNode.d_magic) {
        *d_ostream << "Error: headnode of allocator '" << d_name <<
                     "' corrupted: magic: " << d_headNode.d_magic << bsl::endl;
        rc = -1;
    }
    else if (this != d_headNode.d_allocator_p) {
        *d_ostream << "Error: headnode of allocator '" << d_name <<
                                            "' corrupted: bad allocator ptr\n";
        rc = -1;
    }
    else if (!d_headNode.d_next_p || !d_headNode.d_prev_p) {
        *d_ostream << "Error: headnode of allocator '" << d_name <<
                                             "' corrupted: null list ptr(s)\n";
        rc = -1;
    }
    else for (SegmentHeader *adjacent = d_headNode.d_next_p; true;
                                                adjacent = d_headNode.d_prev_p) {
        if (UNFREED_SEGMENT_MAGIC != adjacent->d_next_p->d_magic &&
                                             &d_headNode != adjacent->d_next_p) {
            if (FREED_SEGMENT_MAGIC == adjacent->d_next_p->d_magic) {
                *d_ostream << "Error: freed object on unfreed segment list"
                                      " of allocator '" << d_name << "' at " <<
                                                 adjacent->d_next_p << bsl::endl;

                // don't set rc, allow to proceed
            }
            else {
                *d_ostream << "Error: segment list of allocator '" << d_name <<
                                           "' corrupted, bad magic number: " <<
                                        adjacent->d_next_p->d_magic << bsl::endl;

                rc = -1;
            }
        }

        if (d_headNode.d_prev_p == adjacent) {
            break;
        }
    }

    return rc;
}

int baesu_StackTraceTestAllocator::preDeallocateCheckSegmentHeader(
                                         const SegmentHeader *segmentHdr) const
{
    int rc = 0;

    if (UNFREED_SEGMENT_MAGIC != segmentHdr->d_magic) {
        if (FREED_SEGMENT_MAGIC == segmentHdr->d_magic) {
            // Note: this write may allocate the freed segment, trashing
            // '*segmentHdr'.

            *d_ostream << "Error: segment at " << &segmentHdr[1] <<
                        " freed second time by allocator '" << d_name << "'\n";
        }
        else {
            *d_ostream << "Error: corrupted segment at " <<
                   &segmentHdr[1] << " attempted to be freed by allocator '" <<
                                                               d_name << "'\n";
        }

        return -1;                                                    // RETURN
    }

    if (this != segmentHdr->d_allocator_p) {
        baesu_StackTraceTestAllocator *otherAlloc = segmentHdr->d_allocator_p;
        bool notOurs = HEAD_NODE_MAGIC != otherAlloc->d_headNode.d_magic;
        *d_ostream << "Error: attempt to free segment by wrong"
                           " allocator.\n    Segment belongs to allocator '" <<
                          (notOurs ? "<<Not a baesu_StackTraceTestAllocator>>"
                                   : otherAlloc->d_name) <<
                                   "'\n    Attempted to free by allocator '" <<
                                                               d_name << "'\n";

        if (notOurs) {
            return -1;                                                // RETURN
        }

        rc = -1;
    }

    if (0 == segmentHdr->d_next_p || 0 == segmentHdr->d_prev_p) {
        *d_ostream << "Error: segment at " << &segmentHdr[1] <<
                                              " corrupted: null list ptr(s)\n";

        return -1;                                                    // RETURN
    }

    for (SegmentHeader *adjacent = segmentHdr->d_next_p; true;
                                               adjacent = segmentHdr->d_prev_p) {
        if (UNFREED_SEGMENT_MAGIC != adjacent->d_next_p->d_magic &&
                                             &d_headNode != adjacent->d_next_p) {
            if (FREED_SEGMENT_MAGIC == adjacent->d_next_p->d_magic) {
                *d_ostream << "Error: freed object on unfreed segment list"
                                      " of allocator '" << d_name << "' at " <<
                                                 adjacent->d_next_p << bsl::endl;
            }
            else {
                *d_ostream << "Error: segment list of allocator '" <<
                                 d_name << "' corrupted, bad magic number: " <<
                                        adjacent->d_next_p->d_magic << bsl::endl;
            }

            rc = -1;
        }

        if (segmentHdr->d_prev_p == adjacent) {
            break;
        }
    }

    return rc;
}

// CREATORS
baesu_StackTraceTestAllocator::baesu_StackTraceTestAllocator(
                                     bsl::ostream     *ostream,
                                     int               segFrames,
                                     bool              demanglingPreferredFlag,
                                     bslma::Allocator *basicAllocator)
: d_headNode(&d_headNode, &d_headNode, this, HEAD_NODE_MAGIC)
, d_mutex()
, d_name("<unnamed>")
, d_maxRecordedFrames(getMaxRecordedFrames(segFrames))
, d_topFrame(0)
, d_ostream(ostream)
, d_demangle(demanglingPreferredFlag)
, d_noAbortFlag(false)
, d_numBlocksInUse(0)
, d_allocator_p(basicAllocator ? basicAllocator
                               : &bslma::MallocFreeAllocator::singleton())
{
    BSLMF_ASSERT(0 == sizeof(SegmentHeader) % MAX_ALIGNMENT);
    BSLS_ASSERT(0 == d_maxRecordedFrames * sizeof(void **) % MAX_ALIGNMENT);

    d_rawTraceFrames = d_maxRecordedFrames + IGNORE_FRAMES;

    d_rawTraceBuffer = (void **) d_allocator_p->allocate(sizeof(void **) *
                                                             d_rawTraceFrames);
}

baesu_StackTraceTestAllocator::baesu_StackTraceTestAllocator(
                                     const char       *name,
                                     bsl::ostream     *ostream,
                                     int               segFrames,
                                     bool              demanglingPreferredFlag,
                                     bslma::Allocator *basicAllocator)
: d_headNode(&d_headNode, &d_headNode, this, HEAD_NODE_MAGIC)
, d_mutex()
, d_name(name ? name : "<unnamed>")
, d_maxRecordedFrames(getMaxRecordedFrames(segFrames))
, d_topFrame(0)
, d_ostream(ostream)
, d_demangle(demanglingPreferredFlag)
, d_noAbortFlag(false)
, d_numBlocksInUse(0)
, d_allocator_p(basicAllocator ? basicAllocator
                               : &bslma::MallocFreeAllocator::singleton())
{
    BSLMF_ASSERT(0 == sizeof(SegmentHeader) % MAX_ALIGNMENT);
    BSLS_ASSERT(0 == d_maxRecordedFrames * sizeof(void **) % MAX_ALIGNMENT);

    d_rawTraceFrames = d_maxRecordedFrames + IGNORE_FRAMES;

    d_rawTraceBuffer = (void **) d_allocator_p->allocate(sizeof(void **) *
                                                             d_rawTraceFrames);
}

baesu_StackTraceTestAllocator::~baesu_StackTraceTestAllocator()
{
    d_allocator_p->deallocate(d_rawTraceBuffer);

    if (numBlocksInUse() > 0) {
        *d_ostream << "======================================================="
                      "========================\nError: memory leaked:\n";

        reportBlocksInUse();
        release();

        if (!d_noAbortFlag) {
            abort();
        }
    }
}

void *baesu_StackTraceTestAllocator::allocate(size_type size)
{
    if (0 == size) {
        return 0;                                                     // RETURN
    }

    bcemt_LockGuard<bcemt_Mutex> guard(&d_mutex);

    if (checkHeadNode()) {
        if (!d_noAbortFlag) {
            abort();
        }

        return 0;                                                     // RETURN
    }

    // The underlying allocator might align the segment differently depending
    // on the size passed.  Since we will be storing 'void *'s at the front,
    // we need it aligned to accomodate 'void *'s.

    BSLMF_ASSERT((int) bsls::AlignmentFromType<SegmentHeader>::VALUE ==
                                      bsls::AlignmentFromType<void **>::VALUE);
    const int align = bsl::max<int>(
                        bsls::AlignmentFromType<SegmentHeader>::VALUE,
                        bsls::AlignmentUtil::calculateAlignmentFromSize(size));
    BSLS_ASSERT(0 == (align & (align - 1)));            // one bit set
    size = (size + align - 1) & ~(align - 1);

    void **to = (void **) d_allocator_p->allocate(
         d_maxRecordedFrames * sizeof(void **) + sizeof(SegmentHeader) + size);

    SegmentHeader *segmentHdr = (SegmentHeader *) (to + d_maxRecordedFrames);

    new (segmentHdr) SegmentHeader(d_headNode.d_next_p,
                                   &d_headNode,
                                   this,
                                   UNFREED_SEGMENT_MAGIC);

    d_headNode.d_next_p->d_prev_p = segmentHdr;
    d_headNode.d_next_p         = segmentHdr;

    int depth = AddressUtil::getStackAddresses(
                                           d_rawTraceBuffer, d_rawTraceFrames);
    if (0 == d_topFrame && depth >= IGNORE_FRAMES) {
        BSLMF_ASSERT(IGNORE_FRAMES > 0);

        d_topFrame = d_rawTraceBuffer[IGNORE_FRAMES - 1];
    }
    depth = bsl::max(depth - IGNORE_FRAMES, 0);

    // The stack trace may have screwed up and put '0's into the stack trace.
    // omit them, since we can't resolve them anyway, and if
    // 'depth < d_maxRecordedFrames', we'll want to 0 terminate the buffer of
    // stack addresses.

    {
        void **from    = d_rawTraceBuffer + IGNORE_FRAMES;
        void **endFrom = from + depth;

        while (from < endFrom) {
            if ((*to = *from++)) {
                ++to;
            }
        }

        if (to < (void **) segmentHdr) {
            *to = 0;
        }
        else {
            BSLS_ASSERT((void **) segmentHdr == to);
            BSLS_ASSERT(d_maxRecordedFrames == depth);
        }
    }

    void *ret = segmentHdr + 1;

    BSLS_ASSERT(0 == ((UintPtr) ret & (sizeof(void **) - 1)));
    BSLS_ASSERT(0 == ((UintPtr) ret & (align - 1)));

    ++d_numBlocksInUse;

    return ret;
}

void baesu_StackTraceTestAllocator::deallocate(void *address)
{
    if (!address) {
        return;                                                       // RETURN
    }
    BSLS_ASSERT(0 == ((UintPtr) address & (sizeof(void **) - 1)));
    BSLS_ASSERT(0 == ((UintPtr) address & (MAX_ALIGNMENT   - 1)));

    SegmentHeader *segmentHdr = (SegmentHeader *) address - 1;

    bcemt_LockGuard<bcemt_Mutex> guard(&d_mutex);

    if (checkHeadNode() || preDeallocateCheckSegmentHeader(segmentHdr)) {
        if (!d_noAbortFlag) {
            abort();
        }

        return;                                                       // RETURN
    }

    segmentHdr->d_next_p->d_prev_p = segmentHdr->d_prev_p;
    segmentHdr->d_prev_p->d_next_p = segmentHdr->d_next_p;
    segmentHdr->d_magic = FREED_SEGMENT_MAGIC;

    d_allocator_p->deallocate((void **) segmentHdr - d_maxRecordedFrames);

    --d_numBlocksInUse;
    BSLS_ASSERT(d_numBlocksInUse >= 0);
}

void baesu_StackTraceTestAllocator::release()
{
    bcemt_LockGuard<bcemt_Mutex> guard(&d_mutex);

    if (checkHeadNode()) {
        if (!d_noAbortFlag) {
            abort();
        }

        return;                                                       // RETURN
    }

    int numSegments = 0;
    for (SegmentHeader *segmentHdr = d_headNode.d_next_p;
                                                 &d_headNode != segmentHdr; ) {
        if (preDeallocateCheckSegmentHeader(segmentHdr)) {
            if (!d_noAbortFlag) {
                abort();
            }

            return;                                                   // RETURN
        }

        ++numSegments;

        SegmentHeader *condemned = segmentHdr;
        segmentHdr = segmentHdr->d_next_p;

        BSLS_ASSERT(&d_headNode == condemned->d_prev_p);
        condemned->d_next_p->d_prev_p = &d_headNode;
        d_headNode.d_next_p           = condemned->d_next_p;

        d_allocator_p->deallocate((void **) condemned - d_maxRecordedFrames);
    }

    BSLS_ASSERT(&d_headNode == d_headNode.d_next_p);
    BSLS_ASSERT(&d_headNode == d_headNode.d_prev_p);

    BSLS_ASSERT(numSegments == d_numBlocksInUse);
    d_numBlocksInUse = 0;
}

void baesu_StackTraceTestAllocator::setNoAbort(bool flagValue)
{
    bcemt_LockGuard<bcemt_Mutex> guard(&d_mutex);

    d_noAbortFlag = flagValue;
}

// ACCESSORS
bool baesu_StackTraceTestAllocator::isNoAbort() const
{
    return d_noAbortFlag;
}

bsl::size_t baesu_StackTraceTestAllocator::numBlocksInUse() const
{
    return d_numBlocksInUse;
}

void baesu_StackTraceTestAllocator::reportBlocksInUse(
                                                   bsl::ostream *ostream) const
{
    typedef bsl::vector<const void *>     StackTraceVec;
    typedef bsl::map<StackTraceVec, int>  StackTraceVecMap;

    bcemt_LockGuard<bcemt_Mutex> guard(&d_mutex);

    if (0 == ostream) {
        ostream = d_ostream;
    }

    if (0 == d_numBlocksInUse) {
        return;                                                       // RETURN
    }

    *ostream << d_numBlocksInUse << " segment(s) in allocator '" <<
                                                       d_name << "' in use.\n";

    StackTraceVecMap stackTraceVecMap(d_allocator_p);
    StackTraceVec v(d_allocator_p);

    int numBlocksInUse = 0;
    for (SegmentHeader *segmentHdr = d_headNode.d_next_p;
                                      !segmentHdr || &d_headNode != segmentHdr;
                                             segmentHdr = segmentHdr->d_next_p) {
        if (!segmentHdr || UNFREED_SEGMENT_MAGIC != segmentHdr->d_magic) {
            if (segmentHdr && FREED_SEGMENT_MAGIC != segmentHdr->d_magic) {
                *ostream << "baesu_StackTraceTestAllocator: freed segment at "
                                   << (segmentHdr + 1) << " in alloced list\n";
            }
            else {
                *ostream << "baesu_StackTraceTestAllocator: memory corruption"
                                       " detected, possible buffer underrun\n";

                // if we're in the destructor, more detailed info will be given
                // when we call 'release'.  We don't want to go on traversing
                // the list because we don't even know that 'segmentHdr' points
                // at a 'segmentHdr', so we can't trust 'segmentHdr->d_next_p'.

                break;
            }
        }

        void **endTrace   = (void **) segmentHdr;
        void **startTrace = endTrace - d_maxRecordedFrames;
        endTrace = bsl::find(startTrace, endTrace, (void *) 0);
        v.clear();
        if (0 != d_topFrame) {
            v.push_back(d_topFrame);
        }
        v.insert(v.end(), startTrace, endTrace);
#if 0
        // This would be a better way to do it, but 'bsl::map' uses the
        // default allocator to implement 'operator[]'.  We want to avoid using
        // the default allocator in a test allocator since the client may be
        // debugging memory issues, and may want to closely monitor traffic on
        // the default allocator.

        ++stackTraceVecMap[v];
#else
        StackTraceVecMap::iterator stmit = stackTraceVecMap.find(v);
        if (stackTraceVecMap.end() == stmit) {
            StackTraceVecMap::value_type pr(v, 1, d_allocator_p);
            stackTraceVecMap.insert(pr);
        }
        else {
            ++stmit->second;
        }
#endif

        ++numBlocksInUse;
    }
    BSLS_ASSERT(d_numBlocksInUse == numBlocksInUse);

    *ostream << "Segment(s) allocated from " <<
                                     stackTraceVecMap.size() << " trace(s).\n";

    int place = 0;
    baesu_StackTrace st(d_allocator_p);
    const StackTraceVecMap::iterator mapend = stackTraceVecMap.end();
    for (StackTraceVecMap::iterator it = stackTraceVecMap.begin();
                                                      mapend != it; ++it) {
        *ostream << "------------------------------------------"
                              "-------------------------------------\n"
                                   << "Allocation trace " << ++place << ", " <<
                                         it->second << " segment(s) in use.\n";
        *ostream << "Stack trace at allocation time:\n";

        int rc = baesu_StackTraceUtil::loadStackTraceFromAddressArray(
                                                             &st,
                                                             it->first.begin(),
                                                             it->first.size(),
                                                             d_demangle);
        if (rc || 0 == st.length()) {
            *ostream << "... stack trace failed ...\n";
        }
        else {
            baesu_StackTraceUtil::printFormatted(*ostream, st);
        }
        st.removeAll();
    }
}

}  // close enterprise namespace

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2012
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
