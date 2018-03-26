// btlmt_readdatapolicy.cpp                                           -*-C++-*-

#include <btlmt_readdatapolicy.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(btlmt_readdatapolicy_cpp,"$Id$ $CSID$")

#include <bdlb_print.h>

#include <bsl_ostream.h>

namespace BloombergLP {
namespace btlmt {

                              // ---------------------
                              // struct ReadDataPolicy
                              // ---------------------

// CONSTANTS
const char ReadDataPolicy::CLASS_NAME[] = "ReadDataPolicy";

const bdlat_EnumeratorInfo ReadDataPolicy::ENUMERATOR_INFO_ARRAY[] = {
    {
        ReadDataPolicy::e_GREEDY,
        "GREEDY",
        sizeof("GREEDY") - 1,
        ""
    },
    {
        ReadDataPolicy::e_ROUND_ROBIN,
        "ROUND_ROBIN",
        sizeof("ROUND_ROBIN") - 1,
        ""
    }
};

// CLASS METHODS
int ReadDataPolicy::fromInt(ReadDataPolicy::Value *result,
                            int                    number)
{
    switch (number) {
      case e_GREEDY:                                            // FALL THROUGH
      case e_ROUND_ROBIN: {
        *result = static_cast<ReadDataPolicy::Value>(number);
        return 0;                                                     // RETURN
      }
      default:
        return -1;                                                    // RETURN
    }
}

int ReadDataPolicy::fromString(ReadDataPolicy::Value *result,
                               const char            *string,
                               int                    stringLength)
{
    for (int i = 0; i < 2; ++i) {
        const bdlat_EnumeratorInfo& enumeratorInfo =
                                      ReadDataPolicy::ENUMERATOR_INFO_ARRAY[i];

        if (stringLength == enumeratorInfo.d_nameLength
        &&  0 == bsl::memcmp(enumeratorInfo.d_name_p, string, stringLength)) {
            *result = static_cast<ReadDataPolicy::Value>(
                                                       enumeratorInfo.d_value);
            return 0;                                                 // RETURN
        }
    }

    return -1;
}

bsl::ostream& ReadDataPolicy::print(bsl::ostream&         stream,
                                    ReadDataPolicy::Value value,
                                    int                   level,
                                    int                   spacesPerLevel)
{
    bdlb::Print::indent(stream, level, spacesPerLevel);

    stream << ReadDataPolicy::toAscii(value);

    if (spacesPerLevel >= 0) {
        stream << '\n';
    }

    return stream;
}

const char *ReadDataPolicy::toAscii(ReadDataPolicy::Value value)
{
#define CASE(X) case(e_ ## X): return #X;

    switch (value) {
      CASE(GREEDY)
      CASE(ROUND_ROBIN)
      default: return "(* UNKNOWN *)";
    }

#undef CASE
}

}  // close package namespace
}  // close enterprise namespace

// ----------------------------------------------------------------------------
// Copyright 2018 Bloomberg Finance L.P.
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
