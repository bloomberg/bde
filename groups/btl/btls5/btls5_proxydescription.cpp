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
    bslim::Printer printer(&stream, 0, -1);
    printer.start();
    stream << " [";
    printer.printValue(object.address().hostname());
    printer.printValue(object.address().port());
    stream << " ] [";
    printer.printValue(object.credentials().username());
    printer.printValue(object.credentials().password());
    stream << " ]";
    printer.end();

    return stream;
}

}  // close enterprise namespace

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2015
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
