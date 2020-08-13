// baljsn_datumdecoderoptions.cpp                                     -*-C++-*-
#include <baljsn_datumdecoderoptions.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(baljsn_datumdecoderoptions_cpp,"$Id$ $CSID$")

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
                         // class DatumDecoderOptions
                         // -------------------------
// CONSTANTS

const int DatumDecoderOptions::s_DEFAULT_INITIALIZER_MAX_NESTED_DEPTH = 64;

// CREATORS

DatumDecoderOptions::DatumDecoderOptions()
: d_maxNestedDepth(s_DEFAULT_INITIALIZER_MAX_NESTED_DEPTH)
{
}

DatumDecoderOptions::DatumDecoderOptions(const DatumDecoderOptions& original)
: d_maxNestedDepth(original.d_maxNestedDepth)
{
}

DatumDecoderOptions::~DatumDecoderOptions()
{
    BSLS_ASSERT(0 < d_maxNestedDepth);
}

// MANIPULATORS

DatumDecoderOptions&
DatumDecoderOptions::operator=(const DatumDecoderOptions& rhs)
{
    if (this != &rhs) {
        d_maxNestedDepth = rhs.d_maxNestedDepth;
    }

    return *this;
}

void DatumDecoderOptions::reset()
{
    d_maxNestedDepth = s_DEFAULT_INITIALIZER_MAX_NESTED_DEPTH;
}

// ACCESSORS

                                  // Aspects

bsl::ostream& DatumDecoderOptions::print(bsl::ostream& stream,
                                         int           level,
                                         int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    printer.printAttribute("maxNestedDepth", d_maxNestedDepth);
    printer.end();
    return stream;
}

}  // close package namespace
}  // close enterprise namespace

// ----------------------------------------------------------------------------
// Copyright 2020 Bloomberg Finance L.P.
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
