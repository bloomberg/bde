// balst_stacktraceframe.cpp                                          -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <balst_stacktraceframe.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(balst_stacktraceframe_cpp,"$Id$ $CSID$")

#include <bslim_printer.h>

#include <bsl_ostream.h>

namespace BloombergLP {
namespace balst {

                           // ---------------------
                           // class StackTraceFrame
                           // ---------------------

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

// MANIPULATORS
void StackTraceFrame::swap(StackTraceFrame& other)
{
    // 'swap' is undefined for objects with non-equal allocators.

    BSLS_ASSERT(allocator() == other.allocator());

    bslalg::SwapUtil::swap(&d_address,           &other.d_address);
    bslalg::SwapUtil::swap(&d_libraryFileName,   &other.d_libraryFileName);
    bslalg::SwapUtil::swap(&d_lineNumber,        &other.d_lineNumber);
    bslalg::SwapUtil::swap(&d_mangledSymbolName, &other.d_mangledSymbolName);
    bslalg::SwapUtil::swap(&d_offsetFromSymbol,  &other.d_offsetFromSymbol);
    bslalg::SwapUtil::swap(&d_sourceFileName,    &other.d_sourceFileName);
    bslalg::SwapUtil::swap(&d_symbolName,        &other.d_symbolName);
}

}  // close package namespace

// FREE OPERATORS
bsl::ostream& balst::operator<<(bsl::ostream&          stream,
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

// FREE FUNCTIONS
void balst::swap(StackTraceFrame& a, StackTraceFrame& b)
{
    if (a.allocator() == b.allocator()) {
        a.swap(b);

        return;                                                       // RETURN
    }

    StackTraceFrame futureA(b, a.allocator());
    StackTraceFrame futureB(a, b.allocator());

    futureA.swap(a);
    futureB.swap(b);
}

}  // close enterprise namespace

// ----------------------------------------------------------------------------
// Copyright 2018 Bloomberg Finance L.P.
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
