// baesu_testallocator.cpp                                            -*-C++-*-
#include <baesu_testallocator.h>

#include <bsls_ident.h>
BSLS_IDENT("$Id$ $CSID$")

#include <baesu_stackaddressutil.h>
#include <baesu_stacktraceutil.h>

#include <bcemt_lockguard.h>

#include <bslma_mallocfreeallocator.h>
#include <bsls_alignmentutil.h>
#include <bsls_assert.h>

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

typedef bslma::Allocator::size_type size_type;

}  // close unnamed namespace

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

static inline
bslma::Allocator *defaultAllocator(bslma::Allocator *allocator)
{
    return allocator ? allocator
                     : &bslma::MallocFreeAllocator::singleton();
}

// CREATORS
baesu_TestAllocator::baesu_TestAllocator(
                                     bsl::ostream     *ostream,
                                     int               segFrames,
                                     bool              demanglingPreferredFlag,
                                     bslma::Allocator *basicAllocator)
: d_mutex()
, d_name(0)
, d_segFrames(getSegFrames(segFrames))
// , d_rawTraceFrames - in method
, d_ostream(ostream)
, d_demangle(demanglingPreferredFlag)
, d_noAbortFlag(false)
, d_allocatedSegments(defaultAllocator(basicAllocator))
// , d_rawTraceBuffer - in method
, d_allocator_p(      defaultAllocator(basicAllocator))
{
    d_rawTraceFrames = d_segFrames + IGNORE_FRAMES;

    d_rawTraceBuffer = (void **) d_allocator_p->allocate(sizeof(void **) *
                                                             d_rawTraceFrames);
}

baesu_TestAllocator::baesu_TestAllocator(
                                     const char       *name,
                                     bsl::ostream     *ostream,
                                     int               segFrames,
                                     bool              demanglingPreferredFlag,
                                     bslma::Allocator *basicAllocator)
: d_mutex()
, d_name(name)
, d_segFrames(getSegFrames(segFrames))
// , d_rawTraceFrames -- in method
, d_ostream(ostream)
, d_demangle(demanglingPreferredFlag)
, d_noAbortFlag(false)
, d_allocatedSegments(defaultAllocator(basicAllocator))
// , d_rawTraceBuffer - in method
, d_allocator_p(      defaultAllocator(basicAllocator))
{
    d_rawTraceFrames = d_segFrames + IGNORE_FRAMES;

    d_rawTraceBuffer = (void **) d_allocator_p->allocate(sizeof(void **) *
                                                             d_rawTraceFrames);
}

baesu_TestAllocator::~baesu_TestAllocator()
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

void *baesu_TestAllocator::allocate(size_type size)
{
    if (0 == size) {
        return 0;
    }

    bcemt_LockGuard<bcemt_Mutex> guard(&d_mutex);

    BSLMF_ASSERT((int) 0 - IGNORE_FRAMES < 0);

    // The underlying allocator might align the segment differently depending
    // on the size passed.  Since we will be storing 'void *'s at the front,
    // we need it aligned to accomodate 'void *'s.

    enum { SZ_VOID = sizeof(void *) };
    BSLMF_ASSERT(0 == (SZ_VOID & (SZ_VOID - 1)));    // one bit set
    size = (size + SZ_VOID - 1) & ~(SZ_VOID - 1);

    void **ret = (void **) d_allocator_p->allocate(
                                         size + d_segFrames * sizeof(void **));
    int depth = AddressUtil::getStackAddresses(
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

    BSLS_ASSERT_SAFE(0 == ((bsls::Types::UintPtr) ret & (SZ_VOID - 1)));
    return (void *) ret;
}

void baesu_TestAllocator::deallocate(void *address)
{
    if (!address) {
        return;
    }

    bcemt_LockGuard<bcemt_Mutex> guard(&d_mutex);

    size_type rc = d_allocatedSegments.erase(address);
    if (1 == rc) {
        d_allocator_p->deallocate((void *) ((void **) address - d_segFrames));
    }
    else {
        *d_ostream << "Segment at " << address;
        if (d_name) {
            *d_ostream << " attempted to be freed by allocator '" << d_name <<
                                                                          '\'';
        }
        *d_ostream << " freed twice or freed with wrong allocator";

        if (!d_noAbortFlag) {
            abort();
        }
    }
}

void baesu_TestAllocator::release()
{
    bcemt_LockGuard<bcemt_Mutex> guard(&d_mutex);

    typedef AllocatedSegmentHashSet::const_iterator HSIt;
    HSIt hsend =    d_allocatedSegments.end();
    for  (HSIt it = d_allocatedSegments.begin(); hsend != it; ++it) {
        d_allocator_p->deallocate((void **) *it - d_segFrames);
    }
    d_allocatedSegments.clear();
}

void baesu_TestAllocator::setNoAbort(bool flagValue)
{
    bcemt_LockGuard<bcemt_Mutex> guard(&d_mutex);

    d_noAbortFlag = flagValue;
}

// ACCESSORS
bool baesu_TestAllocator::isNoAbort() const
{
    return d_noAbortFlag;
}

bsl::size_t baesu_TestAllocator::numBlocksInUse() const
{
    bcemt_LockGuard<bcemt_Mutex> guard(&d_mutex);

    return d_allocatedSegments.size();
}

void baesu_TestAllocator::reportBlocksInUse(bsl::ostream *ostream) const
{
    typedef bsl::vector<const void *>    StackTraceVec;
    typedef bsl::map<StackTraceVec, int> StackTraceVecMap;

    bcemt_LockGuard<bcemt_Mutex> guard(&d_mutex);

    if (0 == ostream) {
        ostream = d_ostream;
    }

    size_type segmentsNotFreed = d_allocatedSegments.size();
    if (segmentsNotFreed > 0) {
        *ostream << segmentsNotFreed << " segments";
        if (d_name) {
            *ostream << " in allocator '" << d_name << '\'';
        }
        *ostream << " in use.\n";

        StackTraceVecMap stackTraceVecMap(d_allocator_p);
        StackTraceVec v(d_allocator_p);

        typedef AllocatedSegmentHashSet::const_iterator HSIt;
        HSIt hsend =      d_allocatedSegments.end();
        for  (HSIt it = d_allocatedSegments.begin(); hsend != it; ++it) {
            void **startTrace = (void **) *it - d_segFrames;
            void **endTrace = bsl::find(startTrace,
                                        startTrace + d_segFrames,
                                        (void *) 0);
            v.resize(endTrace - startTrace);
            bsl::replace_copy(startTrace,
                              endTrace,
                              v.begin(),
                              (void **) -1,
                              (void **) 0);
            StackTraceVecMap::iterator stmit = stackTraceVecMap.find(v);
            if (stackTraceVecMap.end() == stmit) {
                StackTraceVecMap::value_type pr(v, 1, d_allocator_p);
                stackTraceVecMap.insert(pr);
            }
            else {
                ++stmit->second;
            }
        }

        *ostream << "Segments allocated in " <<
                                     stackTraceVecMap.size() << " place(s).\n";

        int place = 0;
        baesu_StackTrace st(d_allocator_p);
        StackTraceVecMap::iterator mapend = stackTraceVecMap.end();
        for (StackTraceVecMap::iterator it = stackTraceVecMap.begin();
                                                          mapend != it; ++it) {
            *ostream << "-------------------------------------------"
                                      "-------------------------------------\n"
                                   << "Allocation place " << ++place << ", " <<
                                          it->second << " segments in use.\n";
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
