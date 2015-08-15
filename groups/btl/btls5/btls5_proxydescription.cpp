// btls5_proxydescription.cpp                                         -*-C++-*-
#include <btls5_proxydescription.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(btls5_proxydescription_cpp, "$Id$ $CSID$")

#include <bslim_printer.h>

#include <bsl_ostream.h>

namespace BloombergLP {

namespace btls5 {
                         // ----------------------
                         // class ProxyDescription
                         // ----------------------

// ACCESSORS
                                  // Aspects

bsl::ostream& ProxyDescription::print(bsl::ostream& stream,
                                      int           level,
                                      int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    printer.printAttribute("address",     d_address);
    printer.printAttribute("credentials", d_credentials);
    printer.end();

    return stream;
}

}  // close package namespace

bsl::ostream& btls5::operator<<(bsl::ostream&           stream,
                                const ProxyDescription& object)
{
    stream << "[ " << object.address();
    if (!object.credentials().username().empty()) {
        stream << ' ' << object.credentials();
    }
    stream << " ]";

    return stream;
}

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
