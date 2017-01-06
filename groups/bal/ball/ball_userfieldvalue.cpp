// ball_userfieldvalue.cpp                                            -*-C++-*-
#include <ball_userfieldvalue.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ball_userfieldvalue_cpp,"$Id$ $CSID$")

#include <bsl_ostream.h>

namespace BloombergLP {
namespace ball {

                             // --------------------
                             // class UserFieldValue
                             // --------------------

// ACCESSORS
ball::UserFieldType::Enum UserFieldValue::type() const
{
    switch (d_value.typeIndex()) {
      case 0: {
        BSLS_ASSERT(d_value.isUnset());
        return ball::UserFieldType::e_VOID;                           // RETURN
      } break;
      case 1: {
        BSLS_ASSERT(d_value.is<bsls::Types::Int64>());
        return ball::UserFieldType::e_INT64;                          // RETURN
      } break;
      case 2: {
        BSLS_ASSERT(d_value.is<double>());
        return ball::UserFieldType::e_DOUBLE;                         // RETURN
      } break;
      case 3: {
        BSLS_ASSERT(d_value.is<bsl::string>());
        return ball::UserFieldType::e_STRING;                         // RETURN
      } break;
      case 4: {
        BSLS_ASSERT(d_value.is<bdlt::DatetimeTz>());
        return ball::UserFieldType::e_DATETIMETZ;                     // RETURN
      } break;
      case 5: {
        BSLS_ASSERT(d_value.is<bsl::vector<char> >());
        return ball::UserFieldType::e_CHAR_ARRAY;                     // RETURN
      } break;
      default: {
        BSLS_ASSERT_OPT(false);
      }
    }
    BSLS_ASSERT_OPT(false);
    return ball::UserFieldType::e_INT64;
}

                                  // Aspects

bsl::ostream& UserFieldValue::print(bsl::ostream& stream,
                                    int           /* level */,
                                    int           /* spacesPerLevel */) const
{
    if (stream.bad()) {
        return stream;                                                // RETURN
    }

    return stream << d_value;
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
