// balber_berconstants.cpp                                            -*-C++-*-
#include <balber_berconstants.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(balber_berconstants_cpp,"$Id$ $CSID$")

#include <bsls_assert.h>

#include <bsl_ostream.h>

namespace BloombergLP {

// FREE OPERATORS

bsl::ostream& balber::operator<<(bsl::ostream&          stream,
                                 BerConstants::TagClass tagClass)
{
    switch (tagClass) {
      case BerConstants::e_UNIVERSAL:
          return stream << "UNIVERSAL";
      case BerConstants::e_APPLICATION:
          return stream << "APPLICATION";
      case BerConstants::e_CONTEXT_SPECIFIC:
          return stream << "CONTEXT_SPECIFIC";
      case BerConstants::e_PRIVATE:
          return stream << "PRIVATE";
    }

    BSLS_ASSERT(!"encountered out-of-bound enumerated value");

    return stream;
}

bsl::ostream& balber::operator<<(bsl::ostream&         stream,
                                 BerConstants::TagType tagType)
{
    switch (tagType) {
      case BerConstants::e_BDEM_PRIMITIVE:
          return stream << "PRIMITIVE";
      case BerConstants::e_BDEM_CONSTRUCTED:
          return stream << "CONSTRUCTED";
    }

    BSLS_ASSERT(!"encountered out-of-bound enumerated value");

    return stream;
}

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
