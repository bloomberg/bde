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
                            // static variables
                            // ----------------

#ifdef BSLS_PLATFORM__CPU_32_BIT
static void * const unfreedSegmentMagic = (void *) 0xbab1face;
static void * const freedSegmentMagic   = (void *) 0xdecea5ed;
static void * const headNodeMagic       = (void *) 0xb055cafe;
#else
static void * const unfreedSegmentMagic = (void *) 0xbab1facebab1faceULL;
static void * const freedSegmentMagic   = (void *) 0xdecea5eddecea5edULL;
static void * const headNodeMagic       = (void *) 0xb055cafeb055cafeULL;
#endif

                            // ----------------
                            // static functions
                            // ----------------

static
int getSegFrames(int segFramesArg)
{
    // round the section for segment pointers up to the nearest multiple of
    // MAX_ALIGNMENT.

    int minSegFramesBytes = segFramesArg * sizeof(void *);
    int roundedUpBytes = (((minSegFramesBytes + MAX_ALIGNMENT - 1) /
                                               MAX_ALIGNMENT) * MAX_ALIGNMENT);

    return roundedUpBytes / sizeof(void *);
}

                   // -----------------------------------------
                   // baesu_StackTraceTestAllocator::SegmentHdr
                   // -----------------------------------------

inline
baesu_StackTraceTestAllocator::SegmentHdr::SegmentHdr(
                                          baesu_StackTraceTestAllocator *alloc,
                                          SegmentHdr                    *next,
                                          SegmentHdr                    *prev,
                                          void                          *magic)
: d_allocator(alloc)
, d_next(next)
, d_prev(prev)
, d_magic(magic)
{}

                        // -----------------------------
                        // baesu_StackTraceTestAllocator
                        // -----------------------------

// PRIVATE ACCESSORS
int baesu_StackTraceTestAllocator::checkHeadNode() const
{
    int rc = 0;

    if (headNodeMagic != d_headNode.d_magic) {
        *d_ostream << "Error: headnode of allocator '" << d_name <<
                     "' corrupted: magic: " << d_headNode.d_magic << bsl::endl;
        rc = -1;
    }
    else if (this != d_headNode.d_allocator) {
        *d_ostream << "Error: headnode of allocator '" << d_name <<
                                            "' corrupted: bad allocator ptr\n";
        rc = -1;
    }
    else if (!d_headNode.d_next || !d_headNode.d_prev) {
        *d_ostream << "Error: headnode of allocator '" << d_name <<
                                             "' corrupted: null list ptr(s)\n";
        rc = -1;
    }
    else for (SegmentHdr *adjacent = d_headNode.d_next; true;
                                                adjacent = d_headNode.d_prev) {
        if (unfreedSegmentMagic != adjacent->d_next->d_magic &&
                                             &d_headNode != adjacent->d_next) {
            if (freedSegmentMagic == adjacent->d_next->d_magic) {
                *d_ostream << "Error: freed object on unfreed segment list"
                                      " of allocator '" << d_name << "' at " <<
                                                 adjacent->d_next << bsl::endl;

                // don't set rc, allow to proceed
            }
            else {
                *d_ostream << "Error: segment list of allocator '" << d_name <<
                                           "' corrupted, bad magic number: " <<
                                        adjacent->d_next->d_magic << bsl::endl;

                rc = -1;
            }
        }

        if (d_headNode.d_prev == adjacent) {
            break;
        }
    }

    return rc;
}

int baesu_StackTraceTestAllocator::preDeallocateCheckSegmentHdr(
                                            const SegmentHdr *segmentHdr) const
{
    int rc = 0;

    if (unfreedSegmentMagic != segmentHdr->d_magic) {
        if (freedSegmentMagic == segmentHdr->d_magic) {
            *d_ostream << "Error: segment at " << &segmentHdr[1] <<
                     " freed second time by allocator " << d_name << bsl::endl;

            rc = -1;
        }
        else {
            *d_ostream << "Error: corrupted segment at " << &segmentHdr[1] <<
                    " attempted to be freed by '" << d_name << "'\n";

            return -1;                                                // RETURN
        }
    }
    
    if (this != segmentHdr->d_allocator) {
        baesu_StackTraceTestAllocator *otherAlloc = segmentHdr->d_allocator;
        bool notOurs = headNodeMagic != otherAlloc->d_headNode.d_magic;
        *d_ostream << "Error: attempt to free segment by wrong allocator.\n"
                                        "    Segment belongs to allocator '" <<
                          (notOurs ? "<<Not a baesu_StackTraceTestAllocator>>"
                                   : otherAlloc->d_name) <<
                         "'\n    Attempted to free by " << d_name << bsl::endl;
        if (notOurs) {
            return -1;                                                // RETURN
        }

        rc = -1;
    }

    if (0 == segmentHdr->d_next || 0 == segmentHdr->d_prev) {
        *d_ostream << "Error: segment at " << &segmentHdr[1] <<
                                          " corrupted: null list ptr(s)\n";

        return -1;                                                    // RETURN
    }

    for (SegmentHdr *adjacent = segmentHdr->d_next; true;
                                               adjacent = segmentHdr->d_prev) {
        if (unfreedSegmentMagic != adjacent->d_next->d_magic &&
                                             &d_headNode != adjacent->d_next) {
            if (freedSegmentMagic == adjacent->d_next->d_magic) {
                *d_ostream << "Error: freed object on unfreed segment list"
                                      " of allocator '" << d_name << "' at " <<
                                                 adjacent->d_next << bsl::endl;
            }
            else {
                *d_ostream << "Error: segment list of allocator '" <<
                                 d_name << "' corrupted, bad magic number: " <<
                                        adjacent->d_next->d_magic << bsl::endl;
            }
    
            rc = -1;
        }
    
        if (segmentHdr->d_prev == adjacent) {
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
: d_headNode(this, &d_headNode, &d_headNode, headNodeMagic)
, d_mutex()
, d_name("<unnamed>")
, d_segFrames(getSegFrames(segFrames))
// , d_rawTraceFrames - in method
, d_ostream(ostream)
, d_demangle(demanglingPreferredFlag)
, d_noAbortFlag(false)
// , d_rawTraceBuffer - in method
, d_numBlocksInUse(0)
, d_allocator_p(basicAllocator ? basicAllocator
                               : &bslma::MallocFreeAllocator::singleton())
{
    BSLMF_ASSERT(0 == sizeof(SegmentHdr) % MAX_ALIGNMENT);
    BSLS_ASSERT(0 == d_segFrames * sizeof(void **) % MAX_ALIGNMENT);

    d_rawTraceFrames = d_segFrames + IGNORE_FRAMES;

    d_rawTraceBuffer = (void **) d_allocator_p->allocate(sizeof(void **) *
                                                             d_rawTraceFrames);
}

baesu_StackTraceTestAllocator::baesu_StackTraceTestAllocator(
                                     const char       *name,
                                     bsl::ostream     *ostream,
                                     int               segFrames,
                                     bool              demanglingPreferredFlag,
                                     bslma::Allocator *basicAllocator)
: d_headNode(this, &d_headNode, &d_headNode, headNodeMagic)
, d_mutex()
, d_name(name ? name : "<unnamed>")
, d_segFrames(getSegFrames(segFrames))
// , d_rawTraceFrames -- in method
, d_ostream(ostream)
, d_demangle(demanglingPreferredFlag)
, d_noAbortFlag(false)
// , d_rawTraceBuffer - in method
, d_numBlocksInUse(0)
, d_allocator_p(basicAllocator ? basicAllocator
                               : &bslma::MallocFreeAllocator::singleton())
{
    BSLMF_ASSERT(0 == sizeof(SegmentHdr) % MAX_ALIGNMENT);
    BSLS_ASSERT(0 == d_segFrames * sizeof(void **) % MAX_ALIGNMENT);

    d_rawTraceFrames = d_segFrames + IGNORE_FRAMES;

    d_rawTraceBuffer = (void **) d_allocator_p->allocate(sizeof(void **) *
                                                             d_rawTraceFrames);
}

baesu_StackTraceTestAllocator::~baesu_StackTraceTestAllocator()
{
    d_allocator_p->deallocate(d_rawTraceBuffer);

    if (numBlocksInUse() > 0) {
        *d_ostream << "========================================"
                                   "========================================\n"
                                                    "Error: memory leaked:\n";

        reportBlocksInUse();

        if (d_noAbortFlag) {
            release();
        }
        else {
            abort();
        }
    }
}

void *baesu_StackTraceTestAllocator::allocate(size_type size)
{
    if (0 == size) {
        return 0;
    }

    bcemt_LockGuard<bcemt_Mutex> guard(&d_mutex);

    BSLMF_ASSERT(IGNORE_FRAMES > 0);

    if (checkHeadNode()) {
        if (!d_noAbortFlag) {
            abort();
        }

        return 0;                                                     // RETURN
    }

    // The underlying allocator might align the segment differently depending
    // on the size passed.  Since we will be storing 'void *'s at the front,
    // we need it aligned to accomodate 'void *'s.

    BSLMF_ASSERT((int) bsls::AlignmentFromType<SegmentHdr>::VALUE ==
                                      bsls::AlignmentFromType<void **>::VALUE);
    const int align = bsl::max<int>(
                        bsls::AlignmentFromType<SegmentHdr>::VALUE,
                        bsls::AlignmentUtil::calculateAlignmentFromSize(size));
    BSLS_ASSERT(0 == (align & (align - 1)));            // one bit set
    size = (size + align - 1) & ~(align - 1);

    void **to = (void **) d_allocator_p->allocate(
                    d_segFrames * sizeof(void **) + sizeof(SegmentHdr) + size);

    SegmentHdr *segmentHdr = (SegmentHdr *) (to + d_segFrames);

    new (segmentHdr) SegmentHdr(this,
                                d_headNode.d_next,
                                &d_headNode,
                                unfreedSegmentMagic);

    d_headNode.d_next->d_prev = segmentHdr;
    d_headNode.d_next         = segmentHdr;

    int depth = AddressUtil::getStackAddresses(
                                           d_rawTraceBuffer, d_rawTraceFrames);
    depth = bsl::max(depth - IGNORE_FRAMES, 0);

    // The stack trace may have screwed up and put '0's into the stack trace.
    // omit them, since we can't resolve them anyway, and if
    // 'depth < d_segFrames', we'll want to 0 terminate the buffer of stack
    // addresses.

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
            BSLS_ASSERT(d_segFrames == depth);
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
        return;
    }
    BSLS_ASSERT(0 == ((UintPtr) address & (sizeof(void **) - 1)));

    SegmentHdr *segmentHdr = (SegmentHdr *) address - 1;

    bcemt_LockGuard<bcemt_Mutex> guard(&d_mutex);

    if (checkHeadNode() || preDeallocateCheckSegmentHdr(segmentHdr)) {
        if (!d_noAbortFlag) {
            abort();
        }

        return;
    }

    segmentHdr->d_next->d_prev = segmentHdr->d_prev;
    segmentHdr->d_prev->d_next = segmentHdr->d_next;
    segmentHdr->d_magic = freedSegmentMagic;

    d_allocator_p->deallocate((void **) segmentHdr - d_segFrames);

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

        return;
    }

    int numSegments = 0;
    for (SegmentHdr *segmentHdr = d_headNode.d_next;
                                                 &d_headNode != segmentHdr; ) {
        if (preDeallocateCheckSegmentHdr(segmentHdr)) {
            if (!d_noAbortFlag) {
                abort();
            }

            return;
        }

        ++numSegments;

        SegmentHdr *condemned = segmentHdr;
        segmentHdr = segmentHdr->d_next;

        BSLS_ASSERT(&d_headNode == condemned->d_prev);
        condemned->d_next->d_prev = &d_headNode;
        d_headNode.d_next         = condemned->d_next;

        d_allocator_p->deallocate((void **) condemned - d_segFrames);
    }

    BSLS_ASSERT(&d_headNode == d_headNode.d_next);
    BSLS_ASSERT(&d_headNode == d_headNode.d_prev);

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
    typedef bsl::vector<const void *>    StackTraceVec;
    typedef bsl::map<StackTraceVec, int> StackTraceVecMap;

    bcemt_LockGuard<bcemt_Mutex> guard(&d_mutex);

    if (0 == ostream) {
        ostream = d_ostream;
    }

    if (0 == d_numBlocksInUse) {
        return;
    }

    *ostream << d_numBlocksInUse << " segment(s) in allocator '" <<
                                                       d_name << "' in use.\n";

    StackTraceVecMap stackTraceVecMap(d_allocator_p);
    StackTraceVec v(d_allocator_p);

    for (SegmentHdr *segmentHdr = d_headNode.d_next; &d_headNode != segmentHdr;
                                             segmentHdr = segmentHdr->d_next) {
        void **endTrace   = (void **) segmentHdr;
        void **startTrace = endTrace - d_segFrames;
        endTrace = bsl::find(startTrace, endTrace, (void *) 0);
        v.clear();
        v.insert(v.begin(), startTrace, endTrace);
#if 0
        // This would be a better way to do it, but the idiotic bsl::map
        // implementatin in bslstp uses the default allocator to implement
        // 'operator[]'.  This should be updated after we go to the new
        // 'bslstl_map'.

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
    }

    *ostream << "Segment(s) allocated in " <<
                                     stackTraceVecMap.size() << " place(s).\n";

    int place = 0;
    baesu_StackTrace st(d_allocator_p);
    const StackTraceVecMap::iterator mapend = stackTraceVecMap.end();
    for (StackTraceVecMap::iterator it = stackTraceVecMap.begin();
                                                      mapend != it; ++it) {
        *ostream << "-------------------------------------------"
                                      "-------------------------------------\n"
                                   << "Allocation place " << ++place << ", " <<
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
