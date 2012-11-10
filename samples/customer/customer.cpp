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
