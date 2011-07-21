// baesu_stacktraceprintutil.cpp                                      -*-C++-*-
#include <baesu_stacktraceprintutil.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(baesu_stacktraceprintutil_cpp,"$Id$ $CSID$")

#include <baesu_stackaddressutil.h>
#include <baesu_stacktrace.h>
#include <baesu_stacktraceutil.h>

#include <bsls_assert.h>
#include <bsls_platform.h>

#if defined(BSLS_PLATFORM__OS_WINDOWS) && defined(BDE_BUILD_TARGET_OPT)
#pragma optimize("", off)
#endif

namespace BloombergLP {

                       // -------------------------------
                       // class baesu_StackTracePrintUtil
                       // -------------------------------

// CLASS METHOD
bsl::ostream& baesu_StackTracePrintUtil::printStackTrace(
                                         bsl::ostream& stream,
                                         int           maxFrames,
                                         bool          demanglingPreferredFlag)
{
    BSLS_ASSERT(0 <= maxFrames || -1 == maxFrames);

    enum {
        DEFAULT_MAX_FRAMES = 1024,
        IGNORE_FRAMES      = baesu_StackAddressUtil::BAESU_IGNORE_FRAMES
    };

    if (maxFrames < 0) {
        maxFrames = DEFAULT_MAX_FRAMES;
    }

    // The value 'IGNORE_FRAMES' indicates the number of additional frames to
    // be ignored because they contained function calls within the stack trace
    // facility.

    maxFrames += IGNORE_FRAMES;

    baesu_StackTrace st;

    void **addresses = (void **) st.allocator()->allocate(
                                                   maxFrames * sizeof(void *));
    int numAddresses = baesu_StackAddressUtil::getStackAddresses(addresses,
                                                                 maxFrames);
    if (numAddresses <= 0 || numAddresses > maxFrames) {
        stream << "Stack Trace: Internal Error getting stack addresses\n";
        return stream;                                                // RETURN
    }

    const int rc = baesu_StackTraceUtil::loadStackTraceFromAddressArray(
                                                  &st,
                                                  addresses    + IGNORE_FRAMES,
                                                  numAddresses - IGNORE_FRAMES,
                                                  demanglingPreferredFlag);
    if (rc) {
        stream << "Stack Trace: Internal Error initializing frames\n";
        return stream;                                                // RETURN
    }

    return baesu_StackTraceUtil::printFormatted(stream, st);
}

}  // close namespace BloombergLP

// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2011
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ------------------------------ END-OF-FILE ---------------------------------
