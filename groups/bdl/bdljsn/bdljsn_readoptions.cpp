// bdljsn_readoptions.cpp                                             -*-C++-*-
#include <bdljsn_readoptions.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdljsn_datumreadoptions_cpp,"$Id$ $CSID$")

#include <bsl_cstring.h>
#include <bsl_iomanip.h>
#include <bsl_limits.h>
#include <bsl_ostream.h>

#include <bslim_printer.h>

#include <bsls_assert.h>
#include <bsls_review.h>


namespace BloombergLP {
namespace bdljsn {

                             // -----------------
                             // class ReadOptions
                             // -----------------

// CONSTANTS
const int ReadOptions::s_DEFAULT_INITIALIZER_MAX_NESTED_DEPTH = 64;

// CREATORS

ReadOptions::ReadOptions()
: d_maxNestedDepth(s_DEFAULT_INITIALIZER_MAX_NESTED_DEPTH)
{
}

ReadOptions::ReadOptions(const ReadOptions& original)
: d_maxNestedDepth(original.d_maxNestedDepth)
{
}

ReadOptions::~ReadOptions()
{
    BSLS_ASSERT(0 < d_maxNestedDepth);
}

// MANIPULATORS

ReadOptions&
ReadOptions::operator=(const ReadOptions& rhs)
{
    if (this != &rhs) {
        d_maxNestedDepth = rhs.d_maxNestedDepth;
    }

    return *this;
}

void ReadOptions::reset()
{
    d_maxNestedDepth = s_DEFAULT_INITIALIZER_MAX_NESTED_DEPTH;
}

// ACCESSORS

                                  // Aspects

bsl::ostream& ReadOptions::print(bsl::ostream& stream,
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
// Copyright 2022 Bloomberg Finance L.P.
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
