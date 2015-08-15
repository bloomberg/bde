// ball_userfieldvalue.cpp                                            -*-C++-*-
#include <ball_userfieldvalue.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ball_userfieldvalue_cpp,"$Id$ $CSID$")

#include <bslim_printer.h>

#include <bsl_ostream.h>


namespace BloombergLP {
namespace ball {

                             // --------------------
                             // class UserFieldValue
                             // --------------------

// ACCESSORS
bsl::ostream& UserFieldValue::print(bsl::ostream& stream,
                                    int           level,
                                    int           spacesPerLevel) const
{
    if (stream.bad()) {
        return stream;                                                // RETURN
    }

    return stream << d_value;
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
