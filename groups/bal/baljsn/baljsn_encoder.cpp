// baljsn_encoder.cpp                                                 -*-C++-*-
#include <baljsn_encoder.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(baljsn_encoder_cpp,"$Id$ $CSID$")

#include <bdlde_base64encoder.h>

namespace BloombergLP {
namespace baljsn {

                          // -----------------------
                          // class Encoder_Formatter
                          // -----------------------

// CREATORS
Encoder_Formatter::Encoder_Formatter(bsl::ostream&         stream,
                                     const EncoderOptions& options)
: d_outputStream(stream)
, d_isArrayElement(false)
{
    if (EncoderOptions::e_PRETTY == options.encodingStyle()) {
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

    if (d_usePrettyStyle) {
        d_outputStream << " : ";
    }
    else {
        d_outputStream << ':';
    }

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

                          // ------------------------
                          // class Encoder_EncodeImpl
                          // ------------------------

// PRIVATE MANIPULATORS
int Encoder_EncodeImpl::encodeImp(const bsl::vector<char>&  value,
                                  int,
                                  bdlat_TypeCategory::Array)
{
    bsl::string base64String;
    bdlde::Base64Encoder encoder(0);
    base64String.resize(
       bdlde::Base64Encoder::encodedLength(static_cast<int>(value.size()), 0));

    // Ensure length is a multiple of 4.

    BSLS_ASSERT(0 == (base64String.length() & 0x03));

    int numOut;
    int numIn;
    int rc = encoder.convert(base64String.begin(),
                             &numOut,
                             &numIn,
                             value.begin(),
                             value.end());

    if (rc < 0) {
        return rc;                                                    // RETURN
    }

    rc = encoder.endConvert(base64String.begin() + numOut);
    if (rc < 0) {
        return rc;                                                    // RETURN
    }

    return encode(base64String, 0);
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
