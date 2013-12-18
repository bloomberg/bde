// customer.cpp                                                       -*-C++-*-
#include <customer.h>

#include <bslim_printer.h>

#include <bsl_ios.h>
#include <bsl_ostream.h>

namespace Enterprise {
namespace pkg {

                        // --------------
                        // class Customer
                        // --------------

// ACCESSORS

                                  // Aspects

bsl::ostream& Customer::print(bsl::ostream& stream,
                              int           level,
                              int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
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
    bslim::Printer printer(&stream, 0, -1);
    printer.start();
    printer.print(object.firstName(), 0);
    printer.print(object.lastName(),  0);
    printer.print(object.accounts(),  0);
    printer.print(object.id(),        0);
    printer.end();

    return stream;
}

}  // close package namespace
}  // close enterprise namespace

// ----------------------------------------------------------------------------
// Copyright (C) 2013 Bloomberg L.P.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
// ----------------------------- END-OF-FILE ----------------------------------
