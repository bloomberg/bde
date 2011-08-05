// bdema_managedptrdeleter.cpp                                        -*-C++-*-
#include <bdema_managedptrdeleter.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(bdema_managedptrdeleter_cpp,"$Id$ $CSID$")

#include <bslim_printer.h>

#include <bsl_ios.h>
#include <bsl_ostream.h>

#include <bsls_assert.h>


namespace BloombergLP {

                     // -----------------------------
                     // class bdema_ManagedPtrDeleter
                     // -----------------------------

// ACCESSORS
                                  // Aspects

bsl::ostream& 
bdema_ManagedPtrDeleter::print(bsl::ostream& stream,
                               int           level,
                               int           spacesPerLevel) const
{
    const bsl::ios_base::fmtflags fmtFlags = stream.flags();
    stream << bsl::boolalpha;

    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    printer.printAttribute("object",  d_object_p  );
    printer.printAttribute("factory", d_factory_p );

    // TBD: switch back to 'printAttribute' when 'bslim' supports function
    // pointers
    printer.printHexAddr((const void*)(d_deleter), "deleter" );
//  printer.printAttribute("deleter", d_deleter   );

    printer.end();

    stream.flags(fmtFlags);

    return stream;
}

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
