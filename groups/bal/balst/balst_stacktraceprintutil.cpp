// balst_stacktraceprintutil.cpp                                      -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <balst_stacktraceprintutil.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(balst_stacktraceprintutil_cpp,"$Id$ $CSID$")

#include <balst_stackaddressutil.h>
#include <balst_stacktrace.h>
#include <balst_stacktraceutil.h>

#include <bsls_assert.h>
#include <bsls_platform.h>

#include <bsl_iostream.h>

#if defined(BSLS_PLATFORM_OS_WINDOWS) && defined(BDE_BUILD_TARGET_OPT)
#pragma optimize("", off)
#endif

namespace BloombergLP {

namespace balst {
                         // -------------------------
                         // class StackTracePrintUtil
                         // -------------------------

// CLASS METHOD
bsl::ostream& StackTracePrintUtil::printStackTrace(
                                         bsl::ostream& stream,
                                         int           maxFrames,
                                         bool          demanglingPreferredFlag)
{
    BSLS_ASSERT(0 <= maxFrames || -1 == maxFrames);

    enum {
        k_DEFAULT_MAX_FRAMES = 1024,
        k_IGNORE_FRAMES      = StackAddressUtil::k_IGNORE_FRAMES
    };

    if (maxFrames < 0) {
        maxFrames = k_DEFAULT_MAX_FRAMES;
    }

    // The value 'IGNORE_FRAMES' indicates the number of additional frames to
    // be ignored because they contained function calls within the stack trace
    // facility.

    maxFrames += k_IGNORE_FRAMES;

    StackTrace st;

    void **addresses = static_cast<void **>(st.allocator()->allocate(
                                                  maxFrames * sizeof(void *)));
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
                                                addresses    + k_IGNORE_FRAMES,
                                                numAddresses - k_IGNORE_FRAMES,
                                                demanglingPreferredFlag);
    if (rc) {
        stream << "Stack Trace: Internal Error initializing frames\n";
        return stream;                                                // RETURN
    }

    return StackTraceUtil::printFormatted(stream, st);
}

}  // close package namespace
}  // close enterprise namespace

// ----------------------------------------------------------------------------
// Copyright 2015 Bloomberg Finance L.P.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// ----------------------------- END-OF-FILE ----------------------------------
