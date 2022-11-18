// bdljsn_error.cpp                                                   -*-C++-*-
#include <bdljsn_error.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdljsn_error_cpp,"$Id$ $CSID$")

#include <bdljsn_location.h>

#include <bsl_cstring.h>
#include <bsl_iomanip.h>
#include <bsl_limits.h>
#include <bsl_ostream.h>

#include <bslim_printer.h>

#include <bslalg_swaputil.h>
#include <bsls_assert.h>
#include <bsls_review.h>

namespace BloombergLP {
namespace bdljsn {

                                // -----------
                                // class Error
                                // -----------

// ACCESSORS

                                  // Aspects

bsl::ostream& Error::print(bsl::ostream& stream,
                           int           level,
                           int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    printer.printAttribute("location", d_location);
    printer.printAttribute("message",  d_message);
    printer.end();
    return stream;
}

}  // close package namespace

// FREE OPERATORS
bsl::ostream& bdljsn::operator<<(bsl::ostream& stream,
                                 const Error&  object)
{
    bslim::Printer printer(&stream, 0, -1);
    printer.start();
    printer.printValue(object.location());
    printer.printValue(object.message());
    printer.end();

    return stream;
}
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
