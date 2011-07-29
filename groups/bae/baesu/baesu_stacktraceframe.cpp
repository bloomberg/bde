// baesu_stacktraceframe.cpp                                          -*-C++-*-
#include <baesu_stacktraceframe.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(baesu_stacktraceframe_cpp,"$Id$ $CSID$")

#include <bsls_platformutil.h>
#include <bslim_printer.h>

namespace BloombergLP {

                       // ---------------------------
                       // class baesu_StackTraceFrame
                       // ---------------------------

// ACCESSORS
bsl::ostream& baesu_StackTraceFrame::print(bsl::ostream& stream,
                                           int           level,
                                           int           spacesPerLevel) const
{
    if (stream.bad()) {
        return stream;                                                // RETURN
    }

    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    printer.print(d_address,                   "address");
    printer.print(d_libraryFileName.c_str(),   "library file name");
    printer.print(d_lineNumber,                "line number");
    printer.print(d_mangledSymbolName.c_str(), "mangled symbol name");
    printer.print(d_offsetFromSymbol,          "offset from symbol");
    printer.print(d_sourceFileName.c_str(),    "source file name");
    printer.print(d_symbolName.c_str(),        "symbol name");
    printer.end();

    return stream;
}

// FREE OPERATORS
bsl::ostream& operator<<(bsl::ostream&                stream,
                         const baesu_StackTraceFrame& object)
{
    if (stream.bad()) {
        return stream;                                                // RETURN
    }

    bslim::Printer printer(&stream, 0, -1);
    printer.start();
    printer.print(object.address(),                   0);
    printer.print(object.libraryFileName().c_str(),   0);
    printer.print(object.lineNumber(),                0);
    printer.print(object.mangledSymbolName().c_str(), 0);
    printer.print(object.offsetFromSymbol(),          0);
    printer.print(object.sourceFileName().c_str(),    0);
    printer.print(object.symbolName().c_str(),        0);
    printer.end();

    return stream;
}

}  // close enterprise namespace

// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2011
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ------------------------------ END-OF-FILE ---------------------------------
