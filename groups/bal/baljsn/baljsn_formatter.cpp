// baljsn_formatter.cpp                                                 -*-C++-*-
#include <baljsn_formatter.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(baljsn_formatter_cpp,"$Id$ $CSID$")

#include <baljsn_encoderoptions.h>
#include <baljsn_printutil.h>

#include <bdlb_print.h>

namespace BloombergLP {
namespace baljsn {

                          // ---------------
                          // class Formatter
                          // ---------------

// CREATORS
Formatter::Formatter(bsl::ostream& stream,
                     bool          usePrettyStyle,
                     int           initialIndentLevel,
                     int           spacesPerLevel)
: d_outputStream(stream)
, d_usePrettyStyle(usePrettyStyle)
, d_indentLevel(initialIndentLevel)
, d_spacesPerLevel(spacesPerLevel)
, d_isArrayElement(false)
{
}

// MANIPULATORS
void Formatter::openObject()
{
    if (d_isArrayElement) {
        indent();
    }

    d_outputStream << '{';

    if (d_usePrettyStyle) {
        d_outputStream << '\n';
        ++d_indentLevel;
    }
}

void Formatter::closeObject()
{
    if (d_usePrettyStyle) {
        --d_indentLevel;
        d_outputStream << '\n';
        bdlb::Print::indent(d_outputStream, d_indentLevel, d_spacesPerLevel);
    }

    d_outputStream << '}';
}

void Formatter::openArray(bool formatAsEmptyArrayFlag)
{
    d_outputStream << '[';

    if (d_usePrettyStyle && !formatAsEmptyArrayFlag) {
        d_outputStream << '\n';
        ++d_indentLevel;
    }
}

void Formatter::closeArray(bool formatAsEmptyArrayFlag)
{
    if (d_usePrettyStyle) {
        --d_indentLevel;
        if (!formatAsEmptyArrayFlag) {
            d_outputStream << '\n';
            bdlb::Print::indent(d_outputStream, d_indentLevel, d_spacesPerLevel);
        }
    }

    d_outputStream << ']';
}

int Formatter::openElement(const bsl::string& name)
{
    if (d_usePrettyStyle) {
        bdlb::Print::indent(d_outputStream, d_indentLevel, d_spacesPerLevel);
    }

    const int rc = PrintUtil::printValue(d_outputStream, name);
    if (rc) {
        return rc;                                                    // RETURN
    }

    d_outputStream << (d_usePrettyStyle ? " : " : ":");

    return 0;
}

void Formatter::closeElement()
{
    d_outputStream << ',';
    if (d_usePrettyStyle) {
        d_outputStream << '\n';
    }
}

void Formatter::indent()
{
    if (d_usePrettyStyle) {
        bdlb::Print::indent(d_outputStream,
                            d_indentLevel,
                            d_spacesPerLevel);
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
