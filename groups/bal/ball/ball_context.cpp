// ball_context.cpp                                                   -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <ball_context.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ball_context_cpp,"$Id$ $CSID$")

#include <bdlb_print.h>
#include <bsl_ostream.h>

namespace BloombergLP {

namespace ball {
                        // -------------
                        // class Context
                        // -------------

// CLASS METHODS
bool Context::isValid(Transmission::Cause transmissionCause,
                      int                 recordIndex,
                      int                 sequenceLength)
{
    bool status;

    switch (transmissionCause) {
      case Transmission::e_PASSTHROUGH: {
        status = 0 == recordIndex && 1 == sequenceLength;
      } break;
      case Transmission::e_TRIGGER:
      case Transmission::e_TRIGGER_ALL:
      case Transmission::e_MANUAL_PUBLISH:
      case Transmission::e_MANUAL_PUBLISH_ALL: {
        status = 0 <= recordIndex
              && 1 <= sequenceLength
              && recordIndex < sequenceLength;
      } break;
      default: {
        status = false;
      } break;
    }
    return status;
}

// ACCESSORS
bsl::ostream& Context::print(bsl::ostream& stream,
                             int           level,
                             int           spacesPerLevel) const
{
    if (0 <= spacesPerLevel) {
        bdlb::Print::indent(stream, level, spacesPerLevel);
        stream << "[\n";
    }
    else {
        // No newlines if 0 > spacesPerLevel.
        stream << '[';
    }

    if (level < 0) {
        level = -level;
    }

    int levelPlus1 = level + 1;

    if (0 <= spacesPerLevel) {
        bdlb::Print::indent(stream, levelPlus1, spacesPerLevel);
    }
    else {
        stream << ' ';
    }
    stream << d_transmissionCause;

    if (0 <= spacesPerLevel) {
        stream << '\n';
        bdlb::Print::indent(stream, levelPlus1, spacesPerLevel);
    }
    else {
        stream << ' ';
    }
    stream << d_recordIndex;

    if (0 <= spacesPerLevel) {
        stream << '\n';
        bdlb::Print::indent(stream, levelPlus1, spacesPerLevel);
    }
    else {
        stream << ' ';
    }
    stream << d_sequenceLength;

    if (0 <= spacesPerLevel) {
        stream << '\n';
        bdlb::Print::indent(stream, level, spacesPerLevel);
        stream << "]\n";
    }
    else {
        stream << " ]";
    }

    return stream << bsl::flush;
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
