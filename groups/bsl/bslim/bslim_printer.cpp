// bslim_printer.cpp                                                  -*-C++-*-
#include <bslim_printer.h>

#include <bsls_ident.h>
BSLS_IDENT("$Id$ $CSID$")

#include <bsls_assert.h>

#include <bsl_cctype.h>
#include <bsl_iomanip.h>
#include <bsl_ios.h>

namespace BloombergLP {

namespace {

class FormatGuard {
    // Class that saves the format flags from a stream.  Note 'ios_base' is
    // a base class that both 'ostream' and 'istream' inherit from.

    // DATA
    bsl::ios_base           *d_stream;
    bsl::ios_base::fmtflags  d_flags;

  public:
    // CREATORS
    explicit
    FormatGuard(bsl::ios_base *stream);
        // Save a pointer to the specified 'stream', and save its format flags,
        // to be restored upon this object's destruction.

    ~FormatGuard();
        // Restore the format flags that were saved at construction to the
        // stream whose pointer we saved at construction.
};

// CREATORS
inline
FormatGuard::FormatGuard(bsl::ios_base *stream)
{
    d_stream = stream;
    d_flags  = stream->flags();
}

inline
FormatGuard::~FormatGuard()
{
    d_stream->flags(d_flags);
}

}  // close unnamed namespace


namespace bslim {

                        // -------------
                        // class Printer
                        // -------------

// PRIVATE ACCESSORS
void Printer::printEndIndentation() const
{
    if (d_spacesPerLevel < 0) {
        *d_stream_p << ' ';
    }
    else {
        *d_stream_p << bsl::setw(d_spacesPerLevel * d_level) << "";
    }
}

void Printer::printIndentation() const
{
    if (d_spacesPerLevel < 0) {
        *d_stream_p << ' ';
    }
    else {
        *d_stream_p << bsl::setw(d_spacesPerLevel * d_levelPlusOne) << "";
    }
}

// CREATORS
Printer::Printer(bsl::ostream *stream, int level, int spacesPerLevel)
: d_stream_p(stream)
, d_spacesPerLevel(spacesPerLevel)
{
    BSLS_ASSERT(stream);

    d_suppressInitialIndentFlag = level < 0;
    d_level                     = level < 0 ? -level : level;
    d_levelPlusOne              = d_level + 1;
}

Printer::~Printer()
{
}

// ACCESSORS
int Printer::absLevel() const
{
    return d_level;
}

void Printer::end(bool suppressBracket) const
{
    if (!suppressBracket) {
        printEndIndentation();
        *d_stream_p << ']';
    }

    if (d_spacesPerLevel >= 0) {
        *d_stream_p << '\n';
    }
}

void Printer::printHexAddr(const void *address, const char *name) const
{
    printIndentation();

    if (name != NULL) {
        *d_stream_p << name << " = ";
    }

    Printer_Helper::print(*d_stream_p,
                          address,
                          -d_levelPlusOne,
                          d_spacesPerLevel);
}

int Printer::spacesPerLevel() const
{
    return d_spacesPerLevel;
}

void Printer::start(bool suppressBracket) const
{
    if (!suppressInitialIndentFlag()) {
        const int absSpacesPerLevel = d_spacesPerLevel < 0
                                      ? -d_spacesPerLevel
                                      :  d_spacesPerLevel;
        *d_stream_p << bsl::setw(absSpacesPerLevel * d_level) << "";
    }

    if (!suppressBracket) {
        *d_stream_p << '[';
        if (d_spacesPerLevel >= 0) {
            *d_stream_p << '\n';
        }
    }
}

bool Printer::suppressInitialIndentFlag() const
{
    return d_suppressInitialIndentFlag;
}

                        // ---------------------
                        // struct Printer_Helper
                        // ---------------------

// CLASS METHODS
void Printer_Helper::printRaw(bsl::ostream&                  stream,
                              char                           data,
                              int                            ,
                              int                            spacesPerLevel,
                              bslmf::SelectTraitCase<bsl::is_fundamental>)
{
#define HANDLE_CONTROL_CHAR(value) case value: stream << #value; break;
    if (bsl::isprint(data)) {
        // print within quotes

        stream << "'" << data <<"'";
    }
    else {
        switch(data) {
          HANDLE_CONTROL_CHAR('\n');
          HANDLE_CONTROL_CHAR('\t');
          HANDLE_CONTROL_CHAR('\0');

          default: {
            // Print as hex.

            FormatGuard guard(&stream);
            stream << bsl::hex
                   << bsl::showbase
                   << static_cast<bsls::Types::UintPtr>(data);
          }
        }
    }
#undef HANDLE_CONTROL_CHAR

    if (spacesPerLevel >= 0) {
        stream << '\n';
    }
}

void Printer_Helper::printRaw(bsl::ostream&                  stream,
                              bool                           data,
                              int                            ,
                              int                            spacesPerLevel,
                              bslmf::SelectTraitCase<bsl::is_fundamental>)
{
    {
        FormatGuard guard(&stream);
        stream << bsl::boolalpha
               << data;
    }

    if (spacesPerLevel >= 0) {
        stream << '\n';
    }
}

void Printer_Helper::printRaw(bsl::ostream&              stream,
                              const char                *data,
                              int                        ,
                              int                        spacesPerLevel,
                              bslmf::SelectTraitCase<bsl::is_pointer>)
{
    if (0 == data) {
        stream << "NULL";
    }
    else {
        stream << '"' << data << '"';
    }
    if (spacesPerLevel >= 0) {
        stream << '\n';
    }
}

void Printer_Helper::printRaw(bsl::ostream&              stream,
                              const void                *data,
                              int                        ,
                              int                        spacesPerLevel,
                              bslmf::SelectTraitCase<bsl::is_pointer>)
{
    if (0 == data) {
        stream << "NULL";
    }
    else {
        FormatGuard guard(&stream);
        stream << bsl::hex
               << bsl::showbase
               << reinterpret_cast<bsls::Types::UintPtr>(data);
    }
    if (spacesPerLevel >= 0) {
        stream << '\n';
    }
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
