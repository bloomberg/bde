// baljsn_formatter.cpp                                                 -*-C++-*-
#include <baljsn_formatter.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(baljsn_formatter_cpp,"$Id$ $CSID$")

#include <baljsn_encoderoptions.h>
#include <baljsn_printutil.h>

namespace BloombergLP {
namespace baljsn {

                          // ---------------
                          // class Formatter
                          // ---------------

// CREATORS
Formatter::Formatter(bsl::ostream&     stream,
                     bool              usePrettyStyle,
                     int               initialIndentLevel,
                     int               spacesPerLevel,
                     bslma::Allocator *basicAllocator)
: d_outputStream(stream)
, d_usePrettyStyle(usePrettyStyle)
, d_indentLevel(initialIndentLevel)
, d_spacesPerLevel(spacesPerLevel)
, d_callSequenceVec(basicAllocator)
{
    // Add a dummy value so we don't have to check whether 'd_callSequenceVec'
    // is empty in 'openObject' when we access 'd_callSequenceVec.back()'.

    d_callSequenceVec.push_back(false);
}

// MANIPULATORS
void Formatter::openObject()
{
    if (d_usePrettyStyle && d_callSequenceVec.back()) {
        indent();
    }

    d_outputStream << '{';

    if (d_usePrettyStyle) {
        d_outputStream << '\n';
        ++d_indentLevel;
        d_callSequenceVec.push_back(false);
    }
}

void Formatter::closeObject()
{
    if (d_usePrettyStyle) {
        --d_indentLevel;
        d_outputStream << '\n';
        indent();

        BSLS_ASSERT_SAFE(false == d_callSequenceVec.back());
        d_callSequenceVec.pop_back();
    }

    d_outputStream << '}';
}

void Formatter::openArray(bool formatAsEmptyArrayFlag)
{
    d_outputStream << '[';

    if (d_usePrettyStyle && !formatAsEmptyArrayFlag) {
        d_outputStream << '\n';
        ++d_indentLevel;
        d_callSequenceVec.push_back(true);
    }
}

void Formatter::closeArray(bool formatAsEmptyArrayFlag)
{
    if (d_usePrettyStyle && !formatAsEmptyArrayFlag) {
        --d_indentLevel;
        d_outputStream << '\n';
        indent();

        BSLS_ASSERT_SAFE(true == d_callSequenceVec.back());
        d_callSequenceVec.pop_back();
    }

    d_outputStream << ']';
}

int Formatter::openMember(const bsl::string& name)
{
    if (d_usePrettyStyle) {
        indent();
    }

    const int rc = PrintUtil::printValue(d_outputStream, name);
    if (rc) {
        return rc;                                                    // RETURN
    }

    d_outputStream << (d_usePrettyStyle ? " : " : ":");

    return 0;
}

void Formatter::closeMember()
{
    d_outputStream << ',';
    if (d_usePrettyStyle) {
        d_outputStream << '\n';
    }
}

}  // close package namespace

}  // close enterprise namespace

// ----------------------------------------------------------------------------
// Copyright 2017 Bloomberg Finance L.P.
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
