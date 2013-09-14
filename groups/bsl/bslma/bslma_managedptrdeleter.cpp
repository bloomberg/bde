// bslma_managedptrdeleter.cpp                                        -*-C++-*-
#include <bslma_managedptrdeleter.h>

#include <bsls_ident.h>
BSLS_IDENT("$Id$ $CSID$")

//#include <bslim_printer.h>  // Levelization violation
#include <bsls_assert.h>

//#include <bsl_ios.h>        // Levelization violation
//#include <bsl_ostream.h>    // Levelization violation

namespace BloombergLP {
namespace bslma {

                        // -----------------------
                        // class ManagedPtrDeleter
                        // -----------------------

// ACCESSORS
                                  // Aspects
#if defined(WE_HAVE_SOLVED_OUR_LEVELIZATION_ISSUES)
bsl::ostream&
ManagedPtrDeleter::print(bsl::ostream& stream,
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
#endif  // WE_HAVE_SOLVED_OUR_LEVELIZATION_ISSUES

}  // close package namespace

// FREE OPERATORS
#if defined(WE_HAVE_SOLVED_OUR_LEVELIZATION_ISSUES)
bsl::ostream& bslma::operator<<(bsl::ostream&            stream,
                                const ManagedPtrDeleter& object)
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
#endif  // WE_HAVE_SOLVED_OUR_LEVELIZATION_ISSUES

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
