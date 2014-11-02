// pkg_customer.cpp                                                   -*-C++-*-
#include <pkg_customer.h>

#include <bslim_printer.h>

#include <bsl_ios.h>
#include <bsl_ostream.h>

namespace Enterprise {
namespace pkg {

                        // --------------
                        // class Customer
                        // --------------

                                  // Aspects

bsl::ostream& Customer::print(bsl::ostream& stream,
                              int           level,
                              int           spacesPerLevel) const
{
    BloombergLP::bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    printer.printAttribute("firstName", d_firstName);
    printer.printAttribute("lastName",  d_lastName);
    printer.printAttribute("accounts",  d_accounts);
    printer.printAttribute("id",        d_id);
    printer.end();

    return stream;
}

// FREE OPERATORS
bsl::ostream& operator<<(bsl::ostream& stream, const Customer& object)
{
    BloombergLP::bslim::Printer printer(&stream, 0, -1);
    printer.start();
    printer.printValue(object.firstName());
    printer.printValue(object.lastName());
    printer.printValue(object.accounts());
    printer.printValue(object.id());
    printer.end();

    return stream;
}

}  // close package namespace
}  // close enterprise namespace

// ----------------------------------------------------------------------------
// Copyright 2013 Bloomberg Finance L.P.
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
