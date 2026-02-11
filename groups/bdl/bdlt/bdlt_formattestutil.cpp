// bdlt_formattestutil.cpp                                            -*-C++-*-
#include <bdlt_formattestutil.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdlt_formattestutil_cpp,"$Id$ $CSID$")

#include <bsls_assert.h>

namespace BloombergLP {
namespace bdlt {

                          // --------------------
                          // class FormatTestUtil
                          // --------------------

// CLASS METHODS
bsl::string FormatTestUtil::narrow(const bsl::wstring_view& viewIn)
{
   // We anticipate that this function will mostly be used to narrow `wstring`
   // values being displayed by `ASSERTV`, so it is not important that this
   // function be particularly fast, therefore we make it non-inline to
   // minimize code size.  This is also why this function returns by value,
   // when `widen` more efficiently returns its result through the argument
   // list.

    typedef bsl::wstring_view::iterator Iter;

    bsl::string ret;
    for (Iter it = viewIn.begin(), end = viewIn.end(); it < end; ++it) {
        const wchar_t w = *it;

        BSLS_ASSERT_SAFE(0 == (~0x7f & w));

        ret += char(w);
    }
    return ret;
}

}  // close package namespace
}  // close enterprise namespace

// ----------------------------------------------------------------------------
// Copyright 2026 Bloomberg Finance L.P.
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
