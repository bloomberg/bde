// balber_beruniversaltagnumber.cpp                                   -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <balber_beruniversaltagnumber.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(balber_beruniversaltagnumber_cpp,"$Id$ $CSID$")

#include <bdlb_string.h>

namespace BloombergLP {
namespace balber {

                        // ----------------------------
                        // struct BerUniversalTagNumber
                        // ----------------------------

// CLASS METHODS
const char *BerUniversalTagNumber::toString(BerUniversalTagNumber::Value value)
{
#ifdef CASE
#undef CASE
#endif

#define CASE(VALUE, STRING) case(VALUE): return #STRING;

    switch (value) {
      CASE(e_BER_BOOL,           BOOL)
      CASE(e_BER_INT,            INT)
      CASE(e_BER_OCTET_STRING,   OCTET_STRING)
      CASE(e_BER_REAL,           REAL)
      CASE(e_BER_ENUMERATION,    ENUMERATION)
      CASE(e_BER_UTF8_STRING,    UTF8_STRING)
      CASE(e_BER_SEQUENCE,       SEQUENCE)
      CASE(e_BER_VISIBLE_STRING, VISIBLE_STRING)
      default: {
      } break;
    }

    return "(* UNKNOWN *)";
}

int BerUniversalTagNumber::fromString(
                                    BerUniversalTagNumber::Value *result,
                                    const char                   *string,
                                    int                           stringLength)
{
    enum { k_SUCCESS = 0, k_NOT_FOUND = 1 };

#ifdef CHECK
#undef CHECK
#endif

#define CHECK(STRING, VALUE)                                                  \
        if (bdlb::String::areEqualCaseless(string, stringLength, #STRING)) {  \
            *result = VALUE;                                                  \
            return k_SUCCESS;                                                 \
        }

    CHECK(BOOL,           e_BER_BOOL)                                 // RETURN
    CHECK(INT,            e_BER_INT)                                  // RETURN
    CHECK(OCTET_STRING,   e_BER_OCTET_STRING)                         // RETURN
    CHECK(REAL,           e_BER_REAL)                                 // RETURN
    CHECK(ENUMERATION,    e_BER_ENUMERATION)                          // RETURN
    CHECK(UTF8_STRING,    e_BER_UTF8_STRING)                          // RETURN
    CHECK(SEQUENCE,       e_BER_SEQUENCE)                             // RETURN
    CHECK(VISIBLE_STRING, e_BER_VISIBLE_STRING)                       // RETURN

    return k_NOT_FOUND;
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
