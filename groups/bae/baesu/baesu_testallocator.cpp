// baesu_testallocator.cpp                                            -*-C++-*-
#include <baesu_testallocator.h>

#include <bsls_ident.h>
BSLS_IDENT("$Id$ $CSID$")

#include <bslma_mallocfreeallocator.h>
#include <bslma_testallocatorexception.h>

#include <bsls_alignment.h>
#include <bsls_alignmentutil.h>
#include <bsls_assert.h>
#include <bsls_platform.h>

#include <bsl_algorithm.h>

#include <cstdlib>  // abort

namespace BloombergLP {

namespace {

enum {
    IGNORE_FRAMES = baesu_StackAddressUtil::IGNORE_FRAMES + 1,
    MAX_ALIGNMENT = bsls::AlignmentUtil::BSLS_MAX_ALIGNMENT
};

typedef bslma::Allocator::size_type size_type;

}  // close unnamed namespace



                            // ----------------
                            // static variables
                            // ----------------

int baesu_TestAllocator::VoidStarHash::s_shiftDown = shiftDown();

int baesu_TestAllocator::VoidStarHash::shiftDown()
{
    BSLMF_ASSERT(0 == (MAX_ALIGNMENT & (MAX_ALIGNMENT - 1)));    // 1 bit set
    BSLMF_ASSERT(MAX_ALIGNMENT > 1);

    int shift = 0;
    while (0 == ((MAX_ALIGNMENT >> shift) & 1)) {
	++shift;
    }

    // It's important that 'shift > 0', otherwise the
    // 'y << (BITS_IN_PTR - s_shiftDown)' in the hash function will have
    // undefined behavior.

    BSLS_ASSERT_SAFE(shift > 0);

    return shift;
}

                            // ----------------
                            // static functions
                            // ----------------

static
int getRawTraceFrames(int segFramesArg)
{
    BSLMF_ASSERT(0 == MAX_ALIGNMENT % sizeof(void *));

    segFramesArg += IGNORE_FRAMES;

    int extra = (segFramesArg * sizeof(void *)) % MAX_ALIGNMENT;
    extra = extra ? (MAX_ALIGNMENT - extra) / sizeof(void *) : 0;

    return segFramesArg + extra;
}

static inline
bsl::Allocator *defaultAllocator(bsl::Allocator *allocator)
{
    return allocator ? allocator
                     : &bslma::MallocFreeAllocator::singleton();
}

// CREATORS
baesu_TestAllocator::baesu_TestAllocator(
                                      int              segFrames
                                      bsl::ostream    *ostream,
                                      bool             demanglingPreferredFlag,
                                      bslma_Allocator *basicAllocator)
: d_mutex()
, d_name(0)
, d_rawTraceFrames(getRawTraceFrames(segFrames))
// , d_segFrames - in method
, d_ostream(ostream)
, d_demangle(demanglingPreferredFlag)
, d_noAbortFlag(false)
, d_allocatedSeqments(defaultAllocator(basicAllocator))
// , d_rawTraceBuffer - in method
, d_allocator_p(      defaultAllocator(basicAllocator))
{
    d_segFrames = d_rawTraceFrames - IGNORE_FRAMES;

    d_rawTraceBuffer = (void **) d_allocator_p->allocate(sizeof(void **) *
                                                             d_rawTraceFrames);
}

baesu_TestAllocator::baesu_TestAllocator(
                                      const char      *name,
                                      int              segFrames,
                                      bsl::ostream    *ostream,
                                      bool             demanglingPreferredFlag,
                                      bslma_Allocator *basicAllocator)
: d_mutex()
, d_name(name)
, d_rawTraceFrames(getRawTraceFrames(segFrames))
// , d_segFrames - in method
, d_ostream(ostream)
, d_demangle(demanglingPreferredFlag)
, d_noAbortFlag(false)
, d_allocatedSeqments(defaultAllocator(basicAllocator))
// , d_rawTraceBuffer - in method
, d_allocator_p(      defaultAllocator(basicAllocator))
{
    d_segFrames = d_rawTraceFrames - IGNORE_FRAMES;

    d_rawTraceBuffer = (void **) d_allocator_p->allocate(sizeof(void **) *
                                                             d_rawTraceFrames);
}

baesu_TestAllocator::~baesu_TestAllocator()
{
    d_allocator_p->deallocate(d_rawTraceBuffer);

    reportSegmentsOutstanding();
}

void *baesu_TestAllocator::allocate(size_type size)
{
    BSLMF_ASSERT((int) 0 - IGNORE_FRAMES < 0);

    void **ret = (void **) d_allocator_p->allocate(
                                         size + d_segFrames * sizeof(void **));
    int depth = baesu_StackAddressUtil::getStackAddresses(
                                           d_rawTraceBuffer, d_rawTraceFrames);
    depth = bsl::max(depth - IGNORE_FRAMES, 0);

    // If there are any '0's in the middle of the stack trace, turn them into
    // '-1's.  Since we will 0 terminate the the stack addresses, 0's in the
    // middle would be harmful.  Before we resolve the addresses, we will turn
    // the '-1's back to '0's.

    bsl::replace_copy(d_rawTraceBuffer + IGNORE_FRAMES,
                      d_rawTraceBuffer + IGNORE_FRAMES + depth,
                      ret,
                      (void **) 0,
                      (void **) -1);
    if (depth < d_segFrames) {
        ret[depth] = 0;
    }
    else {
        BSLS_ASSERT(d_segFrames == depth);
    }

    ret += d_segFrames;
    d_allocatedSegments.insert((void *) ret);

    return (void *) ret;
}

void *baesu_TestAllocator::deallocate(void *segment)
{
    size_type rc = d_allocatedSegments.erase(segment);
    if (1 == rc) {
        d_allocator_p->deallocate((void *) ((void **) segment - d_segFrames));
    }
    else {
        *d_ostream << "Segment at " << segment;
        if (d_name) {
            *d_ostream << " attempted to be freed by allocator '" << d_name <<
                                                                          '\'';
        }
        *d_ostream << " freed twice or freed with wrong allocator";

        if (!d_noAbort) {
            abort();
        }
    }
}

// ACCESSORS
void baesu_TestAllocator::reportSegmentsOutstanding() const
{
    typedef bsl::vector<const void *>    StackTraceVec;
    typedef bsl::map<StackTraceVec, int> StackTraceVecMap;

    size_type segmentsNotFreed = d_allocatedSegments.size();
    if (segmentsNotFreed > 0) {
        *d_ostream << "================"
                "===========================================================\n"
			       << "Error: " << segmentsNotFreed << " segments";
        if (d_name) {
            *d_ostream << " in allocator '" << d_name;
        }
        *d_ostream << " leaked\n";

        StackTraceVecMap stackTraceVecMap(d_allocator_p);
        StackTraceVec v(d_allocator_p);

        typedef AllocatedSegmentHT::const_iterator HTIt;
        HTIt end =      d_allocatedSegments.end();
        for  (HTIt it = d_allocatedSegments.begin(); end != it; ++it) {
            void **startTrace = (void **) *it - d_segFrames;
            void **endTrace = bsl::find(startTrace,
                                        startTrace + d_segFrames,
                                        0);
	    v.resize(endTrace - startTrace);
	    bsl::replace_copy(startTrace,
			      endTrace,
			      v.begin(),
			      (void **) -1,
			      (void **) 0);
            StackTraceMap::iterator stmit = stackTraceMap.find(v);
            if (stackTraceMap.end() == stmit) {
                stackTraceMap[v] = 1;
            }
            else {
                ++stmit->second;
            }
        }

        *d_ostream << "Unfreed segments allocated in " <<
                                        stackTraceMap.size() << " place(s).\n";
        
        int place = 0;
        baesu_StackTrace st(d_allocator_p);
        StackTraceMap::iterator end = stackTraceMap.end();
        for (StackTraceMap::iterator it = stackTraceMap.begin(); end != it;
                                                                        ++it) {
            *d_ostream << "----------------"
                "-----------------------------------------------------------\n"
                                   << "Allocation place " << ++place << ", " <<
                                           it->second << " segments leaked.\n";
            *d_ostream << "Stack trace at allocation time:\n";

            int rc = baesu_StackTraceUtil::loadStackTraceFromeAddressArray(
                                                             &st,
                                                             it->first.begin(),
                                                             it->first.size(),
                                                             d_demangle);
            if (rc || 0 == st.length()) {
                *d_ostream << "... stack trace failed ...\n";
            }
            else {
                *d_ostream << st;
            }
        }           
    }
}

}  // close enterprise namespace

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2010
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
