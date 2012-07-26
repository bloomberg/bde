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

namespace BloombergLP {

namespace {

typedef baesu_StackAddressUtil AddressUtil;

enum {
    IGNORE_FRAMES = baesu_StackAddressUtil::BAESU_IGNORE_FRAMES,
        // On some platforms, gathering the stack pointers wastes one frame
        // gathering the address of 'AddressUtil::getStackAddresses', which is
        // reflected in whether 'AddressUtil::BAESU_IGNORE_FRAMES' is 0 or 1.
        // This constant allows us to adjust for this and not display that
        // frame as it is of no interest to the user.

    MAX_ALIGNMENT = bsls::AlignmentUtil::BSLS_MAX_ALIGNMENT
};

typedef bsls::Types::UintPtr UintPtr;
typedef bslma::Allocator::size_type size_type;

template <int A, int B>
struct Max {
    // Calculate, at compile time, the maximum of two ints

    enum { VALUE = A > B ? A : B };
};

}  // close unnamed namespace

                            // ----------------
                            // static functions
                            // ----------------

static
int getMaxRecordedFrames(int specifiedMaxRecordedFrames)
    // Specify 'specifiedMaxRecordedFrames', the value passed to the
    // 'baesu_StackTraceTestAllocator' constructor.  Return the necessary size
    // of the buffer area needed, per segment, in pointers, to achieve the
    // depth of stack trace requested by the client in the constructor.  Extra
    // space needs to be added for ignored frames (if any), and it is necessary
    // that the buffer length in bytes be a multiple of 'MAX_ALIGNMENT'.
{
    enum { PTRS_PER_MAX = MAX_ALIGNMENT / sizeof(void *) };

    BSLMF_ASSERT(0 == MAX_ALIGNMENT % sizeof(void *));

    int ret = ((specifiedMaxRecordedFrames + IGNORE_FRAMES + PTRS_PER_MAX - 1)
                                                / PTRS_PER_MAX) * PTRS_PER_MAX;
    BSLS_ASSERT(ret >= specifiedMaxRecordedFrames);

    return ret;
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
    else {
        for (SegmentHeader *adjacent = d_headNode.d_next_p; true;
                                              adjacent = d_headNode.d_prev_p) {
            if (UNFREED_SEGMENT_MAGIC != adjacent->d_next_p->d_magic &&
                                           &d_headNode != adjacent->d_next_p) {
                if (FREED_SEGMENT_MAGIC == adjacent->d_next_p->d_magic) {
                    *d_ostream << "Error: freed object on unfreed segment list"
                               << " of allocator '" << d_name << "' at "
                               << adjacent->d_next_p << bsl::endl;

                    // don't set rc, allow to proceed
                }
                else {
                    *d_ostream << "Error: segment list of allocator '"
                               << d_name << "' corrupted, bad magic number: "
                               << adjacent->d_next_p->d_magic << bsl::endl;

                    rc = -1;
                }
            }

            if (d_headNode.d_prev_p == adjacent) {
                break;
            }
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

            *d_ostream << "Error: segment at " << &segmentHdr[1]
                       << " freed second time by allocator '" << d_name
                       << "'\n";
        }
        else {
            *d_ostream << "Error: corrupted segment at "
                       << &segmentHdr[1] << " attempted to be freed by"
                       << " allocator '" << d_name << "'\n";
        }

        return -1;                                                    // RETURN
    }

    if (this != segmentHdr->d_allocator_p) {
        baesu_StackTraceTestAllocator *otherAlloc = segmentHdr->d_allocator_p;
        bool notOurs = HEAD_NODE_MAGIC != otherAlloc->d_headNode.d_magic;
        *d_ostream << "Error: attempt to free segment by wrong"
                   << " allocator.\n    Segment belongs to allocator '"
                   << (notOurs ? "<<Not a baesu_StackTraceTestAllocator>>"
                               : otherAlloc->d_name)
                   << "'\n    Attempted to free by allocator '"
                   << d_name << "'\n";

        if (notOurs) {
            return -1;                                                // RETURN
        }

        rc = -1;
    }

    if (0 == segmentHdr->d_next_p || 0 == segmentHdr->d_prev_p) {
        *d_ostream << "Error: segment at " << &segmentHdr[1]
                   << " corrupted: null list ptr(s)\n";

        return -1;                                                    // RETURN
    }

    for (SegmentHeader *adjacent = segmentHdr->d_next_p; true;
                                             adjacent = segmentHdr->d_prev_p) {
        if (UNFREED_SEGMENT_MAGIC != adjacent->d_next_p->d_magic &&
                                           &d_headNode != adjacent->d_next_p) {
            if (FREED_SEGMENT_MAGIC == adjacent->d_next_p->d_magic) {
                *d_ostream << "Error: freed object on unfreed segment list"
                           << " of allocator '" << d_name << "' at "
                           << adjacent->d_next_p << bsl::endl;
            }
            else {
                *d_ostream << "Error: segment list of allocator '"
                           << d_name << "' corrupted, bad magic number: "
                           << adjacent->d_next_p->d_magic << bsl::endl;
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
                                     int               numRecordedFrames,
                                     bool              demanglingPreferredFlag,
                                     bslma::Allocator *basicAllocator)
: d_headNode(&d_headNode, &d_headNode, this, HEAD_NODE_MAGIC)
, d_mutex()
, d_name("<unnamed>")
, d_maxRecordedFrames(getMaxRecordedFrames(numRecordedFrames))
, d_ostream(ostream)
, d_demangleFlag(demanglingPreferredFlag)
, d_noAbortFlag(false)
, d_numBlocksInUse(0)
, d_allocator_p(basicAllocator ? basicAllocator
                               : &bslma::MallocFreeAllocator::singleton())
{
    BSLMF_ASSERT(0 == sizeof(SegmentHeader) % MAX_ALIGNMENT);
    BSLS_ASSERT(0 == d_maxRecordedFrames * sizeof(void **) % MAX_ALIGNMENT);
}

baesu_StackTraceTestAllocator::baesu_StackTraceTestAllocator(
                                     const char       *name,
                                     bsl::ostream     *ostream,
                                     int               numRecordedFrames,
                                     bool              demanglingPreferredFlag,
                                     bslma::Allocator *basicAllocator)
: d_headNode(&d_headNode, &d_headNode, this, HEAD_NODE_MAGIC)
, d_mutex()
, d_name(name ? name : "<unnamed>")
, d_maxRecordedFrames(getMaxRecordedFrames(numRecordedFrames))
, d_ostream(ostream)
, d_demangleFlag(demanglingPreferredFlag)
, d_noAbortFlag(false)
, d_numBlocksInUse(0)
, d_allocator_p(basicAllocator ? basicAllocator
                               : &bslma::MallocFreeAllocator::singleton())
{
    BSLMF_ASSERT(0 == sizeof(SegmentHeader) % MAX_ALIGNMENT);
    BSLS_ASSERT(0 == d_maxRecordedFrames * sizeof(void **) % MAX_ALIGNMENT);
}

baesu_StackTraceTestAllocator::~baesu_StackTraceTestAllocator()
{
    if (checkHeadNode() && !d_noAbortFlag) {
        BSLS_ASSERT_OPT(0 &&
           "baesu_StackTraceTestAllocator: defective head node in destructor");
    }

    if (numBlocksInUse() > 0) {
        *d_ostream << "======================================================="
                   << "========================\nError: memory leaked:\n";

        reportBlocksInUse();

        if (!d_noAbortFlag) {
            BSLS_ASSERT_OPT(0 && "baesu_StackTraceTestAllocator:"
                                        " Memory leak detected by destructor");
        }

        release();
    }
}

void *baesu_StackTraceTestAllocator::allocate(size_type size)
{
    if (0 == size) {
        return 0;                                                     // RETURN
    }

    bcemt_LockGuard<bcemt_Mutex> guard(&d_mutex);

    if (checkHeadNode() && !d_noAbortFlag) {
        BSLS_ASSERT_OPT(0 && "baesu_StackTraceTestAllocator:"
                                 " defective headnode detected by 'allocate'");
    }

    // The underlying allocator might align the segment differently depending
    // on the size passed.  The alignment must be large enough to accomodate
    // the stack addresses (type 'void *') in the buffer, it must be large
    // enough to accomodate the 'SegmentHeader's alignment requirements, and it
    // must be large enough to accomodate whatever the alignment requirements
    // of whatever the client intends to store in their portion of the segment.
    // We can infer the requirements of our pointers and segment header at
    // compile time in 'FIXED_ALIGN', then we infer the alignment requirement
    // of the client's section from the size passed, and take the maximum of
    // the two to get the alignment required.  We then round the size we will
    // pass to the underlying allocator up to a multiple of 'align', so that
    // the underlying allocator cannot infer a lower value of the alignment
    // requirement.

    enum { FIXED_ALIGN =
                   Max<bsls::AlignmentFromType<void *>::VALUE,
                       bsls::AlignmentFromType<SegmentHeader>::VALUE>::VALUE };
    const int align = bsl::max<int>(
                        FIXED_ALIGN,
                        bsls::AlignmentUtil::calculateAlignmentFromSize(size));
    const int lowBits = align - 1;
    BSLS_ASSERT(0 == (align & lowBits));    // verify 'align' is power of 2
    size = (size + lowBits) & ~lowBits;     // round 'size' up to multiple of
                                            // 'align'

    void **framesBegin = (void **) d_allocator_p->allocate(
          d_maxRecordedFrames * sizeof(void *) + sizeof(SegmentHeader) + size);

    void **framesEnd = framesBegin + d_maxRecordedFrames;
    SegmentHeader *segmentHdr = (SegmentHeader *) framesEnd;

    new (segmentHdr) SegmentHeader(d_headNode.d_next_p,
                                   &d_headNode,
                                   this,
                                   UNFREED_SEGMENT_MAGIC);

    d_headNode.d_next_p->d_prev_p = segmentHdr;
    d_headNode.d_next_p           = segmentHdr;

    bsl::fill(framesBegin, framesEnd, (void *) 0);
    AddressUtil::getStackAddresses(framesBegin,
                                   d_maxRecordedFrames);

    void *ret = segmentHdr + 1;

    BSLS_ASSERT(0 == ((UintPtr) ret & ((sizeof(void *) - 1) | (align - 1))));

    ++d_numBlocksInUse;

    return ret;
}

void baesu_StackTraceTestAllocator::deallocate(void *address)
{
    if (!address) {
        return;                                                       // RETURN
    }

    if (0 != ((UintPtr) address & (sizeof(void *) - 1))) {
        // Badly aligned, can't be a segment we allocated.

        *d_ostream << "Badly aligned segment passed to allocator '"
                   << d_name << "' must have been allocated by another type"
                   << " of allocator\n";
        if (d_noAbortFlag) {
            BSLS_ASSERT_OPT(0 && "baesu_StackTraceTestAllocator:"
                                " badly align segment passed to 'deallocate'");
        }

        return;                                                       // RETURN
    }

    SegmentHeader *segmentHdr = (SegmentHeader *) address - 1;

    bcemt_LockGuard<bcemt_Mutex> guard(&d_mutex);

    if (checkHeadNode()) {
        if (!d_noAbortFlag) {
            BSLS_ASSERT_OPT(0 && "baesu_StackTraceTestAllocator:"
                               " defective headnode detected by 'deallocate'");
        }

        return;                                                       // RETURN
    }
    if (preDeallocateCheckSegmentHeader(segmentHdr)) {
        if (!d_noAbortFlag) {
            BSLS_ASSERT_OPT(0 && "baesu_StackTraceTestAllocator:"
                         " defective segment header detected by 'deallocate'");
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
            BSLS_ASSERT_OPT(0 && "baesu_StackTraceTestAllocator:"
                                  " defective headnode detected by 'release'");
        }

        return;                                                       // RETURN
    }

    for (SegmentHeader *segmentHdr = d_headNode.d_next_p;
                &d_headNode != segmentHdr; segmentHdr = segmentHdr->d_next_p) {
        if (preDeallocateCheckSegmentHeader(segmentHdr)) {
            if (!d_noAbortFlag) {
                BSLS_ASSERT_OPT(0 && "baesu_StackTraceTestAllocator:"
                            " defective segment header detected by 'release'");
            }

            return;                                                   // RETURN
        }
    }

    int numSegments = 0;
    for (SegmentHeader *segmentHdr = d_headNode.d_next_p;
                                                 &d_headNode != segmentHdr; ) {
        ++numSegments;

        SegmentHeader *condemned = segmentHdr;
        segmentHdr = segmentHdr->d_next_p;

        d_allocator_p->deallocate((void **) condemned - d_maxRecordedFrames);
    }

    d_headNode.d_next_p = &d_headNode;
    d_headNode.d_prev_p = &d_headNode;

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
    bcemt_LockGuard<bcemt_Mutex> guard(&d_mutex);

    return d_noAbortFlag;
}

bsl::size_t baesu_StackTraceTestAllocator::numBlocksInUse() const
{
    bcemt_LockGuard<bcemt_Mutex> guard(&d_mutex);

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

    *ostream << d_numBlocksInUse << " segment(s) in allocator '"
             << d_name << "' in use.\n";

    StackTraceVecMap stackTraceVecMap(d_allocator_p);
    StackTraceVec traceVec(d_allocator_p);

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
                         << " detected, possible buffer underrun\n";

                // We don't want to go on traversing
                // the list because we don't even know that 'segmentHdr' points
                // at a 'segmentHdr', so we can't trust 'segmentHdr->d_next_p'.

                break;
            }
        }

        void **endTrace   = (void **) segmentHdr;
        void **startTrace = endTrace - d_maxRecordedFrames + IGNORE_FRAMES;

        // trim off the zeroes at the end of the trace

        while (endTrace > startTrace && 0 == endTrace[-1]) {
            --endTrace;
        }

        traceVec.clear();
        traceVec.insert(traceVec.end(), startTrace, endTrace);
#if 0
        // This would be a better way to do it, but 'bsl::map' uses the
        // default allocator to implement 'operator[]'.  We want to avoid using
        // the default allocator in a test allocator since the client may be
        // debugging memory issues, and may want to closely monitor traffic on
        // the default allocator.

        ++stackTraceVecMap[traceVec];
#else
        StackTraceVecMap::iterator stmit = stackTraceVecMap.find(traceVec);
        if (stackTraceVecMap.end() == stmit) {
            StackTraceVecMap::value_type pr(traceVec, 1, d_allocator_p);
            stackTraceVecMap.insert(pr);
        }
        else {
            ++stmit->second;
        }
#endif

        ++numBlocksInUse;
    }
    BSLS_ASSERT(d_numBlocksInUse == numBlocksInUse);

    *ostream << "Segment(s) allocated from "
             << stackTraceVecMap.size() << " trace(s).\n";

    int place = 0;
    baesu_StackTrace st(d_allocator_p);
    const StackTraceVecMap::iterator mapend = stackTraceVecMap.end();
    for (StackTraceVecMap::iterator it = stackTraceVecMap.begin();
                                                      mapend != it; ++it) {
        *ostream << "------------------------------------------"
                 << "-------------------------------------\n"
                 << "Allocation trace " << ++place << ", "
                 << it->second << " segment(s) in use.\n"
                 << "Stack trace at allocation time:\n";

        int rc = baesu_StackTraceUtil::loadStackTraceFromAddressArray(
                                                       &st,
                                                       it->first.begin(),
                                                       (int) it->first.size(),
                                                       d_demangleFlag);
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
