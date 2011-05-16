// baesu_stacktrace.cpp                                               -*-C++-*-
#include <baesu_stacktrace.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(baesu_stacktrace_cpp,"$Id$ $CSID$")

#include <baesu_objectfileformat.h>
#include <baesu_stackaddressutil.h>
#include <baesu_stacktraceframe.h>

#include <baesu_stacktraceresolverimpl_elf.h>
#include <baesu_stacktraceresolverimpl_xcoff.h>
#include <baesu_stacktraceresolverimpl_windows.h>

#include <bslma_allocator.h>

#include <bsl_ostream.h>
#include <bsl_vector.h>

#if defined(BSLS_PLATFORM__OS_WINDOWS) && defined(BDE_BUILD_TARGET_OPT)
#pragma optimize("", off)
#endif

// Note that the 'class' 'baesu_StackTrace' contains a pointer to a vector that
// is owned, rather than containing the vector directly, so that the vector,
// when created, can take a pointer to 'd_allocator'.

namespace BloombergLP {

                           // ----------------------
                           // class baesu_StackTrace
                           // ----------------------

// CREATORS
baesu_StackTrace::baesu_StackTrace()
: d_frames(0)
, d_allocator()
{
    typedef bsl::vector<baesu_StackTraceFrame> FrameVec;

    d_frames = new (d_allocator) FrameVec(&d_allocator);
}

baesu_StackTrace::~baesu_StackTrace()
{
    // All allocated memory is freed when 'd_allocator' is destroyed, and the
    // memory occupied by the vector 'd_frames' with it.  Note that the
    // individual stack trace frames have trivial destructors, so there is no
    // point in destroying the vector.
}

// MANIPULATORS
int baesu_StackTrace::initializeFromAddressArray(void *addresses[],
                                                 int   numAddresses,
                                                 bool  demangle)
{
    BSLS_ASSERT(numAddresses >= 0);
    BSLS_ASSERT(0 == numAddresses || 0 != addresses);

    typedef baesu_ObjectFileFormat::Policy Policy;
    typedef baesu_StackTraceResolverImpl<Policy> Resolver;

    d_frames->clear();
    d_frames->resize(numAddresses);

    for (int i = 0; i < numAddresses; ++i) {
        (*d_frames)[i].setAddress(addresses[i]);
    }

    return Resolver::resolve(d_frames, demangle, &d_allocator);
}

int baesu_StackTrace::initializeFromStack(int maxFrames, bool demangle)
{
#if defined(BSLS_PLATFORM__OS_LINUX) || defined(BSLS_PLATFORM__OS_WINDOWS)
    enum { IGNORE_OFFSET = 2 };
#else
    enum { IGNORE_OFFSET = 1 };
#endif
    maxFrames += IGNORE_OFFSET;    // allocate extra frame(s) to be ignored

    void **addresses = (void **) d_allocator.allocate(
                                                   maxFrames * sizeof(void *));
    int numAddresses = baesu_StackAddressUtil::getStackAddresses(addresses,
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
const baesu_StackTraceFrame& baesu_StackTrace::stackFrame(int index) const
{
    BSLS_ASSERT(index >= 0);
    BSLS_ASSERT(index < (int) d_frames->size());

    return (*d_frames)[index];
}

int baesu_StackTrace::numFrames() const
{
    return d_frames->size();
}

bsl::ostream& baesu_StackTrace::printTerse(bsl::ostream& stream) const
{
    for (unsigned i = 0; i < d_frames->size(); ++i) {
        stream << "(" << i << ") " << (*d_frames)[i] << bsl::endl;
    }

    return stream;
}

// FREE OPERATORS
bsl::ostream& operator<<(bsl::ostream&           stream,
                         const baesu_StackTrace& stackTrace)
{
    return stackTrace.printTerse(stream);
}

                          // --------------------------
                          // class baesu_StackTraceUtil
                          // --------------------------

// CLASS METHOD
void baesu_StackTraceUtil::printStackTrace(bsl::ostream& stream,
                                           int           maxFrames,
                                           bool          demangle)
{
#if defined(BSLS_PLATFORM__OS_LINUX) || defined(BSLS_PLATFORM__OS_WINDOWS)
    enum { IGNORE_OFFSET = 2 };
#else
    enum { IGNORE_OFFSET = 1 };
#endif
    maxFrames += IGNORE_OFFSET;    // allocate extra frame(s) to be ignored

    baesu_StackTrace st;

    void **addresses = (void **) st.allocator()->allocate(
                                                   maxFrames * sizeof(void *));
    int numAddresses = baesu_StackAddressUtil::getStackAddresses(addresses,
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
