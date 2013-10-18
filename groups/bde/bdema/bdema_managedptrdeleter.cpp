// bdema_managedptrdeleter.cpp                                        -*-C++-*-
#include <bdema_managedptrdeleter.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(bdema_managedptrdeleter_cpp,"$Id$ $CSID$")

#include <bslim_printer.h>

#include <bsl_ios.h>
#include <bsl_ostream.h>

namespace BloombergLP {

                     // -----------------------------
                     // class bdema_ManagedPtrDeleter
                     // -----------------------------

// FREE OPERATORS
bsl::ostream& operator<<(bsl::ostream&                  stream,
                         const bdema_ManagedPtrDeleter& object)
{
    const bsl::ios_base::fmtflags fmtFlags = stream.flags();
    stream << bsl::boolalpha;

    bslim::Printer printer(&stream, 0, -1);
    printer.start();
    printer.printValue(object.object());
    printer.printValue(object.factory());

    // TBD: Remove 'void *' cast when 'bslim' supports function pointers.
    printer.printValue((void*)object.deleter());
    printer.end();

    stream.flags(fmtFlags);

    return stream;
}

}  // close namespace BloombergLP

// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2010
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ------------------------------ END-OF-FILE ---------------------------------
