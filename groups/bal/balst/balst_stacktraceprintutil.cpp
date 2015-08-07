// balst_stacktraceprintutil.cpp                                      -*-C++-*-
#include <balst_stacktraceprintutil.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(balst_stacktraceprintutil_cpp,"$Id$ $CSID$")

#include <balst_stackaddressutil.h>
#include <balst_stacktrace.h>
#include <balst_stacktraceutil.h>

#include <bsls_assert.h>
#include <bsls_platform.h>

#if defined(BSLS_PLATFORM_OS_WINDOWS) && defined(BDE_BUILD_TARGET_OPT)
#pragma optimize("", off)
#endif

namespace BloombergLP {

namespace balst {
                       // -------------------------------
                       // class StackTracePrintUtil
                       // -------------------------------

// CLASS METHOD
bsl::ostream& StackTracePrintUtil::printStackTrace(
                                         bsl::ostream& stream,
                                         int           maxFrames,
                                         bool          demanglingPreferredFlag)
{
    BSLS_ASSERT(0 <= maxFrames || -1 == maxFrames);

    enum {
        DEFAULT_MAX_FRAMES = 1024,
        IGNORE_FRAMES      = StackAddressUtil::BALST_IGNORE_FRAMES
    };

    if (maxFrames < 0) {
        maxFrames = DEFAULT_MAX_FRAMES;
    }

    // The value 'IGNORE_FRAMES' indicates the number of additional frames to
    // be ignored because they contained function calls within the stack trace
    // facility.

    maxFrames += IGNORE_FRAMES;

    StackTrace st;

    void **addresses = (void **) st.allocator()->allocate(
                                                   maxFrames * sizeof(void *));
#if !defined(BSLS_PLATFORM_OS_CYGWIN)
    int numAddresses = StackAddressUtil::getStackAddresses(addresses,
                                                                 maxFrames);
#else
    int numAddresses = 0;
#endif
    if (numAddresses <= 0 || numAddresses > maxFrames) {
        stream << "Stack Trace: Internal Error getting stack addresses\n";
        return stream;                                                // RETURN
    }

    const int rc = StackTraceUtil::loadStackTraceFromAddressArray(
                                                  &st,
                                                  addresses    + IGNORE_FRAMES,
                                                  numAddresses - IGNORE_FRAMES,
                                                  demanglingPreferredFlag);
    if (rc) {
        stream << "Stack Trace: Internal Error initializing frames\n";
        return stream;                                                // RETURN
    }

    return StackTraceUtil::printFormatted(stream, st);
}
}  // close package namespace

}  // close namespace BloombergLP

// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2011
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ------------------------------ END-OF-FILE ---------------------------------
