// balst_stacktraceframe.cpp                                          -*-C++-*-
#include <balst_stacktraceframe.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(balst_stacktraceframe_cpp,"$Id$ $CSID$")

#include <bslim_printer.h>

namespace BloombergLP {

namespace balst {
                       // ---------------------------
                       // class StackTraceFrame
                       // ---------------------------

// ACCESSORS
bsl::ostream& StackTraceFrame::print(bsl::ostream& stream,
                                           int           level,
                                           int           spacesPerLevel) const
{
    if (stream.bad()) {
        return stream;                                                // RETURN
    }

    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    printer.printAttribute("address",             d_address);
    printer.printAttribute("library file name",   d_libraryFileName.c_str());
    printer.printAttribute("line number",         d_lineNumber);
    printer.printAttribute("mangled symbol name", d_mangledSymbolName.c_str());
    printer.printAttribute("offset from symbol",  d_offsetFromSymbol);
    printer.printAttribute("source file name",    d_sourceFileName.c_str());
    printer.printAttribute("symbol name",         d_symbolName.c_str());
    printer.end();

    return stream;
}
}  // close package namespace

// FREE OPERATORS
bsl::ostream& balst::operator<<(bsl::ostream&                stream,
                         const StackTraceFrame& object)
{
    if (stream.bad()) {
        return stream;                                                // RETURN
    }

    bslim::Printer printer(&stream, 0, -1);
    printer.start();
    printer.printValue(object.address());
    printer.printValue(object.libraryFileName().c_str());
    printer.printValue(object.lineNumber());
    printer.printValue(object.mangledSymbolName().c_str());
    printer.printValue(object.offsetFromSymbol());
    printer.printValue(object.sourceFileName().c_str());
    printer.printValue(object.symbolName().c_str());
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
