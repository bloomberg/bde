// baljsn_datumencoderoptions.cpp                                     -*-C++-*-
#include <baljsn_datumencoderoptions.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(baljsn_datumencoderoptions_cpp,"$Id$ $CSID$")

#include <baljsn_encodingstyle.h>

#include <bsl_cstring.h>
#include <bsl_iomanip.h>
#include <bsl_limits.h>
#include <bsl_ostream.h>

#include <bslim_printer.h>

#include <bsls_assert.h>
#include <bsls_review.h>

///Implementation Note
///- - - - - - - - - -
// This file was generated from a script and was subsequently modified to add
// documentation and to make other changes.  The steps to generate and update
// this file can be found in the 'doc/generating_codec_options.txt' file.

namespace BloombergLP {
namespace baljsn {

                         // -------------------------
                         // class DatumEncoderOptions
                         // -------------------------
// CONSTANTS

const bool DatumEncoderOptions::s_DEFAULT_INITIALIZER_STRICT_TYPES = false;

const bool DatumEncoderOptions::s_DEFAULT_INITIALIZER_ENCODE_QUOTED_DECIMAL64
                                                                        = true;

const int DatumEncoderOptions::s_DEFAULT_INITIALIZER_INITIAL_INDENT_LEVEL = 0;

const int DatumEncoderOptions::s_DEFAULT_INITIALIZER_SPACES_PER_LEVEL = 0;

const baljsn::EncodingStyle::Value
    DatumEncoderOptions::s_DEFAULT_INITIALIZER_ENCODING_STYLE =
        baljsn::EncodingStyle::e_COMPACT;

// CREATORS

DatumEncoderOptions::DatumEncoderOptions()
: d_strictTypes(s_DEFAULT_INITIALIZER_STRICT_TYPES)
, d_encodeQuotedDecimal64(s_DEFAULT_INITIALIZER_ENCODE_QUOTED_DECIMAL64)
, d_initialIndentLevel(s_DEFAULT_INITIALIZER_INITIAL_INDENT_LEVEL)
, d_spacesPerLevel(s_DEFAULT_INITIALIZER_SPACES_PER_LEVEL)
, d_encodingStyle(s_DEFAULT_INITIALIZER_ENCODING_STYLE)
{
}

DatumEncoderOptions::DatumEncoderOptions(const DatumEncoderOptions& original)
: d_strictTypes(original.d_strictTypes)
, d_encodeQuotedDecimal64(original.d_encodeQuotedDecimal64)
, d_initialIndentLevel(original.d_initialIndentLevel)
, d_spacesPerLevel(original.d_spacesPerLevel)
, d_encodingStyle(original.d_encodingStyle)
{
}

DatumEncoderOptions::~DatumEncoderOptions()
{
    BSLS_ASSERT(0 <= d_initialIndentLevel);
    BSLS_ASSERT(0 <= d_spacesPerLevel);
}

// MANIPULATORS

DatumEncoderOptions&
DatumEncoderOptions::operator=(const DatumEncoderOptions& rhs)
{
    if (this != &rhs) {
        d_strictTypes = rhs.d_strictTypes;
        d_encodeQuotedDecimal64 = rhs.d_encodeQuotedDecimal64;
        d_initialIndentLevel = rhs.d_initialIndentLevel;
        d_spacesPerLevel = rhs.d_spacesPerLevel;
        d_encodingStyle = rhs.d_encodingStyle;
    }

    return *this;
}

void DatumEncoderOptions::reset()
{
    d_strictTypes = s_DEFAULT_INITIALIZER_STRICT_TYPES;
    d_encodeQuotedDecimal64 = s_DEFAULT_INITIALIZER_ENCODE_QUOTED_DECIMAL64;
    d_initialIndentLevel = s_DEFAULT_INITIALIZER_INITIAL_INDENT_LEVEL;
    d_spacesPerLevel = s_DEFAULT_INITIALIZER_SPACES_PER_LEVEL;
    d_encodingStyle = s_DEFAULT_INITIALIZER_ENCODING_STYLE;
}

// ACCESSORS

                                  // Aspects

bsl::ostream& DatumEncoderOptions::print(bsl::ostream& stream,
                                         int           level,
                                         int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    printer.printAttribute("strictTypes", d_strictTypes);
    printer.printAttribute("encodeQuotedDecimal64", d_encodeQuotedDecimal64);
    printer.printAttribute("initialIndentLevel", d_initialIndentLevel);
    printer.printAttribute("spacesPerLevel", d_spacesPerLevel);
    printer.printAttribute("encodingStyle", d_encodingStyle);
    printer.end();
    return stream;
}

}  // close package namespace
}  // close enterprise namespace

// ----------------------------------------------------------------------------
// Copyright 2019 Bloomberg Finance L.P.
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
