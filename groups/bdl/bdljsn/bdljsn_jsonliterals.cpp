// bdljsn_jsonliterals.cpp                                            -*-C++-*-
#include <bdljsn_jsonliterals.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdljsn_jsonliterals_cpp, "$Id$ $CSID$")

#include <bdljsn_error.h>
#include <bdljsn_json.h>
#include <bdljsn_jsonutil.h>

#include <bsls_assert.h>
#include <bsls_compilerfeatures.h>

#include <bsl_string_view.h>

namespace BloombergLP {
namespace bdljsn {

                               // ------------------
                               // class JsonLiterals
                               // ------------------

#if defined(BSLS_COMPILERFEATURES_SUPPORT_INLINE_NAMESPACE)  && \
    defined(BSLS_COMPILERFEATURES_SUPPORT_USER_DEFINED_LITERALS)

inline namespace literals {
inline namespace JsonLiterals {
bdljsn::Json operator"" _json(const char *text, bsl::size_t numBytes)
{
    bdljsn::Json result(bslma::Default::globalAllocator());
    bdljsn::Error error;

    int rc = bdljsn::JsonUtil::read(&result,
                                    &error,
                                    bsl::string_view(text, numBytes));
    if (0 != rc) {
        BSLS_ASSERT_INVOKE_NORETURN("Invalid JSON Literal");
    }
    return result;
}
}  // close JsonLiterals namespace
}  // close literals namespace

#endif


}  // close package namespace
}  // close enterprise namespace

// ----------------------------------------------------------------------------
// Copyright 2022 Bloomberg Finance L.P.
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
