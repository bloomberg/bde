// baljsn_encoder.cpp                                                 -*-C++-*-
#include <baljsn_encoder.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(baljsn_encoder_cpp,"$Id$ $CSID$")

#include <baljsn_encoder_testtypes.h>  // for testing only
#include <baljsn_parserutil.h>         // for testing only

namespace BloombergLP {
namespace baljsn {

// The 'Encoder_Formatter' 'class' has been replaced by the 'baljsn::Formatter'
// 'class' in the 'baljsn_formatter' component.  Clients should use that
// 'class' instead.  The following method definitions are provided for
// backwards-compatibility as users have written code using this
// component-private 'class'.

                          // -----------------------
                          // class Encoder_Formatter
                          // -----------------------

// CREATORS
Encoder_Formatter::Encoder_Formatter(bsl::ostream&         stream,
                                     const EncoderOptions& options)
: d_outputStream(stream)
, d_isArrayElement(false)
{
    if (baljsn::EncoderOptions::e_PRETTY == options.encodingStyle()) {
        d_usePrettyStyle = true;
        d_indentLevel    = options.initialIndentLevel();
        d_spacesPerLevel = options.spacesPerLevel();
    }
    else {
        d_usePrettyStyle = false;
        d_indentLevel    = 0;
        d_spacesPerLevel = 0;
    }
}

// MANIPULATORS
void Encoder_Formatter::openObject()
{
    indent();

    d_outputStream << '{';

    if (d_usePrettyStyle) {
        d_outputStream << '\n';
    }

    ++d_indentLevel;
}

void Encoder_Formatter::closeObject()
{
    --d_indentLevel;

    if (d_usePrettyStyle) {
        d_outputStream << '\n';
        bdlb::Print::indent(d_outputStream, d_indentLevel, d_spacesPerLevel);
    }

    d_outputStream << '}';
}

void Encoder_Formatter::openArray(bool formatAsEmptyArrayFlag)
{
    d_outputStream << '[';

    if (d_usePrettyStyle && !formatAsEmptyArrayFlag) {
        d_outputStream << '\n';
    }

    ++d_indentLevel;
}

void Encoder_Formatter::closeArray(bool formatAsEmptyArrayFlag)
{
    --d_indentLevel;

    if (d_usePrettyStyle && !formatAsEmptyArrayFlag) {
        d_outputStream << '\n';
        bdlb::Print::indent(d_outputStream, d_indentLevel, d_spacesPerLevel);
    }

    d_outputStream << ']';

}

void Encoder_Formatter::indent()
{
    if (d_usePrettyStyle) {
        if (d_isArrayElement) {
            bdlb::Print::indent(d_outputStream,
                                d_indentLevel,
                                d_spacesPerLevel);
        }
    }
}

int Encoder_Formatter::openElement(const bsl::string& name)
{
    if (d_usePrettyStyle) {
        bdlb::Print::indent(d_outputStream, d_indentLevel, d_spacesPerLevel);
    }

    const int rc = PrintUtil::printValue(d_outputStream, name);
    if (rc) {
        return rc;                                                    // RETURN
    }

    d_outputStream << (d_usePrettyStyle ? ": " : ":");

    return 0;
}

void Encoder_Formatter::closeElement()
{
    d_outputStream << ',';
    if (d_usePrettyStyle) {
        d_outputStream << '\n';
    }
}

void Encoder_Formatter::openDocument()
{
    if (d_usePrettyStyle) {
        bdlb::Print::indent(d_outputStream, d_indentLevel, d_spacesPerLevel);
    }
}

void Encoder_Formatter::closeDocument()
{
    if (d_usePrettyStyle) {
        d_outputStream << '\n';
    }
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
