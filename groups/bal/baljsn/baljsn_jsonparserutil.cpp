// baljsn_parserutil.cpp                                              -*-C++-*-
#include <baljsn_jsonparserutil.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(baljsn_jsonparserutil_cpp,"$Id$ $CSID$")

#include <bdlde_base64decoder.h>

namespace {
namespace u {

using namespace BloombergLP;

int base64Decode(bsl::vector<char>       *value,
                 const bsl::string_view&  base64String)
{
    BSLS_ASSERT(value);

    value->clear();

    bdlde::Base64Decoder base64Decoder(true);
    int                  length = static_cast<int>(base64String.length());

    value->resize(static_cast<bsl::size_t>(
                              bdlde::Base64Decoder::maxDecodedLength(length)));

    int rc = base64Decoder.convert(value->begin(),
                                   base64String.begin(),
                                   base64String.end());

    if (rc < 0) {
        return rc;                                                    // RETURN
    }

    rc = base64Decoder.endConvert(value->begin() +
                                  base64Decoder.outputLength());

    if (rc < 0) {
        return rc;                                                    // RETURN
    }

    value->resize(static_cast<bsl::size_t>(base64Decoder.outputLength()));

    return 0;
}

}  // close namespace u
}  // close unnamed namespace

namespace BloombergLP {
namespace baljsn {

                             // ---------------------
                             // struct JsonParserUtil
                             // ---------------------

int JsonParserUtil::getValue(bsl::vector<char>   *value,
                             const bdljsn::Json&  data)
{
    BSLS_ASSERT(value);
    BSLS_ASSERT(data.isString());

    int rc = u::base64Decode(value, data.theString());

    return rc;
}

}  // close package namespace
}  // close enterprise namespace

// ----------------------------------------------------------------------------
// Copyright 2025 Bloomberg Finance L.P.
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
