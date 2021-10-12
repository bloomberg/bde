// balst_stacktraceprinter.cpp                                        -*-C++-*-
#include <balst_stacktraceprinter.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(balst_streamer,"$Id$ $CSID$")

#include <balst_stacktraceprintutil.h>
#include <balst_stacktraceutil.h>

#include <bsls_assert.h>

#if defined(BSLS_PLATFORM_OS_WINDOWS)

#pragma optimize("", off)

#endif

namespace BloombergLP {
namespace balst {

                            // -----------------------
                            // class StackTracePrinter
                            // -----------------------

StackTracePrinter::StackTracePrinter(int  maxFrames,
                                     bool demanglingPreferredFlag,
                                     int  additionalIgnoreFrames)
: d_maxFrames(maxFrames)
, d_demanglingPreferredFlag(demanglingPreferredFlag)
, d_additionalIgnoreFrames(additionalIgnoreFrames)
{
    BSLS_ASSERT(-1 <= maxFrames);
    BSLS_ASSERT(0 <= additionalIgnoreFrames);
}

}  // close package namespace

// FREE OPERATORS
bsl::ostream& balst::operator<<(bsl::ostream&            stream,
                                const StackTracePrinter& object)
{
    stream << bsl::endl;

    bsl::ostream& rs = balst::StackTracePrintUtil::printStackTrace(
                                          stream,
                                          object.d_maxFrames,
                                          object.d_demanglingPreferredFlag,
                                          object.d_additionalIgnoreFrames + 1);

    // There was a problem in some optimized builds with the above function
    // call chaining to 'printStackTrace', which threw off our calculations of
    // the number of frames to be ignored.  So after the above, we examine the
    // value of 'rs' returned.  We know that it will match 'stream', but the
    // compiler cannot tell that because the implementation of
    // 'printStackTrace' is in a separate module.  So we do some entirely
    // predictable computation after the above call to return 'stream', which
    // prevents any chaining.

    if (&rs != &stream) {
        // never happens

        rs << bsl::endl;
    }

    return stream;
}

}  // close enterprise namespace

// ----------------------------------------------------------------------------
// Copyright 2021 Bloomberg Finance L.P.
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
