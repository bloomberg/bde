// bdesu_stacktrace.cpp                                               -*-C++-*-
#include <bdesu_stacktrace.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(bdesu_stacktrace_cpp,"$Id$ $CSID$")

#include <bdesu_objectfileformat.h>
#include <bdesu_stackaddressutil.h>
#include <bdesu_stacktraceframe.h>

#include <bdesu_stacktraceresolverimpl_elf.h>
#include <bdesu_stacktraceresolverimpl_xcoff.h>
#include <bdesu_stacktraceresolverimpl_windows.h>

#include <bslma_allocator.h>

#include <bsl_ostream.h>
#include <bsl_vector.h>

#if defined(BSLS_PLATFORM__OS_WINDOWS) && defined(BDE_BUILD_TARGET_OPT)
#pragma optimize("", off)
#endif

// Note that the 'class' 'bdesu_StackTrace' contains a pointer to a vector that
// is owned, rather than containing the vector directly, so that the vector,
// when created, can take a pointer to 'd_allocator'.

namespace BloombergLP {

                           // ----------------------
                           // class bdesu_StackTrace
                           // ----------------------

// CREATORS
bdesu_StackTrace::bdesu_StackTrace()
: d_frames(0)
, d_allocator()
{
    typedef bsl::vector<bdesu_StackTraceFrame> FrameVec;

    d_frames = new (d_allocator) FrameVec(&d_allocator);
}

bdesu_StackTrace::~bdesu_StackTrace()
{
    // All allocated memory is freed when 'd_allocator' is destroyed, and the
    // memory occupied by the vector 'd_frames' with it.  Note that the
    // individual stack trace frames have trivial destructors, so there is no
    // point in destroying the vector.
}

// MANIPULATORS
int bdesu_StackTrace::initializeFromAddressArray(void *addresses[],
                                                 int   numAddresses,
                                                 bool  demangle)
{
    BSLS_ASSERT(numAddresses >= 0);
    BSLS_ASSERT(0 == numAddresses || 0 != addresses);

    typedef bdesu_ObjectFileFormat::Policy Policy;
    typedef bdesu_StackTraceResolverImpl<Policy> Resolver;

    d_frames->clear();
    d_frames->resize(numAddresses);

    for (int i = 0; i < numAddresses; ++i) {
        (*d_frames)[i].setAddress(addresses[i]);
    }

    return Resolver::resolve(d_frames, demangle, &d_allocator);
}

int bdesu_StackTrace::initializeFromStack(int maxFrames, bool demangle)
{
#if defined(BSLS_PLATFORM__OS_LINUX) || defined(BSLS_PLATFORM__OS_WINDOWS)
    enum { IGNORE_OFFSET = 2 };
#else
    enum { IGNORE_OFFSET = 1 };
#endif
    maxFrames += IGNORE_OFFSET;    // allocate extra frame(s) to be ignored

    void **addresses = (void **) d_allocator.allocate(
                                                   maxFrames * sizeof(void *));
    int numAddresses = bdesu_StackAddressUtil::getStackAddresses(addresses,
                                                                 maxFrames);
    if (numAddresses <= 0 || numAddresses > maxFrames) {
        return -1;                                                    // RETURN
    }

    // Throw away the first frame, since it refers to this routine.

    return initializeFromAddressArray(addresses + IGNORE_OFFSET,
                                      numAddresses - IGNORE_OFFSET,
                                      demangle);
}

// ACCESSORS
const bdesu_StackTraceFrame& bdesu_StackTrace::stackFrame(int index) const
{
    BSLS_ASSERT(index >= 0);
    BSLS_ASSERT(index < (int) d_frames->size());

    return (*d_frames)[index];
}

int bdesu_StackTrace::numFrames() const
{
    return d_frames->size();
}

bsl::ostream& bdesu_StackTrace::printTerse(bsl::ostream& stream) const
{
    for (unsigned i = 0; i < d_frames->size(); ++i) {
        stream << "(" << i << ") " << (*d_frames)[i] << bsl::endl;
    }

    return stream;
}

// FREE OPERATORS
bsl::ostream& operator<<(bsl::ostream&           stream,
                         const bdesu_StackTrace& stackTrace)
{
    return stackTrace.printTerse(stream);
}

                          // --------------------------
                          // class bdesu_StackTraceUtil
                          // --------------------------

// CLASS METHOD
void bdesu_StackTraceUtil::printStackTrace(bsl::ostream& stream,
                                           int           maxFrames,
                                           bool          demangle)
{
#if defined(BSLS_PLATFORM__OS_LINUX) || defined(BSLS_PLATFORM__OS_WINDOWS)
    enum { IGNORE_OFFSET = 2 };
#else
    enum { IGNORE_OFFSET = 1 };
#endif
    maxFrames += IGNORE_OFFSET;    // allocate extra frame(s) to be ignored

    bdesu_StackTrace st;

    void **addresses = (void **) st.allocator()->allocate(
                                                   maxFrames * sizeof(void *));
    int numAddresses = bdesu_StackAddressUtil::getStackAddresses(addresses,
                                                                 maxFrames);
    if (numAddresses <= 0 || numAddresses > maxFrames) {
        stream << "Stack Trace: Internal Error getting stack addresses\n";
        return;                                                       // RETURN
    }

    // Throw away the first frame, since it refers to this routine.

    const int rc = st.initializeFromAddressArray(addresses + IGNORE_OFFSET,
                                                 numAddresses - IGNORE_OFFSET,
                                                 demangle);
    if (rc) {
        stream << "Stack Trace: Internal Error initializing frames\n";
        return;                                                       // RETURN
    }

    stream << st;
}

}  // close namespace BloombergLP

// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2010
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ------------------------------ END-OF-FILE ---------------------------------
