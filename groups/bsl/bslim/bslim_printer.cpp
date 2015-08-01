// bslim_printer.cpp                                                  -*-C++-*-
#include <bslim_printer.h>

#include <bsls_ident.h>
BSLS_IDENT("$Id$ $CSID$")

#include <bslim_testutil.h>  // for testing only

#include <bsls_assert.h>

#include <bsl_cctype.h>
#include <bsl_iomanip.h>
#include <bsl_ios.h>

namespace BloombergLP {

namespace {

static
void putSpaces(bsl::ostream& stream, int numSpaces)
    // Efficiently insert the specified 'numSpaces' spaces into the specified
    // 'stream'.  This function has no effect on 'stream' if 'numSpaces < 0'.
{
    // Algorithm: Write spaces in chunks.  The chunk size is large enough so
    // that most times only a single call to the 'write' method is needed.

    // Define the largest chunk of spaces:
    static const char k_SPACES[]    = "                                      ";
           const int  k_SPACES_SIZE = sizeof(k_SPACES) - 1;

    while (k_SPACES_SIZE < numSpaces) {
        stream.write(k_SPACES, k_SPACES_SIZE);
        numSpaces -= k_SPACES_SIZE;
    }

    if (0 < numSpaces) {
        stream.write(k_SPACES, numSpaces);
    }
}

class FormatGuard {
    // Class that saves the format flags from a stream.  Note 'ios_base' is a
    // base class that both 'ostream' and 'istream' inherit from.

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
    putSpaces(*d_stream_p, d_spacesPerLevel < 0
                           ? 1
                           : d_spacesPerLevel * d_level);
}

void Printer::printIndentation() const
{
    putSpaces(*d_stream_p, d_spacesPerLevel < 0
                           ? 1
                           : d_spacesPerLevel * d_levelPlusOne);
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
        putSpaces(*d_stream_p, absSpacesPerLevel * d_level);
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
    if (bsl::isprint(static_cast<unsigned char>(data))) {
        // print within quotes

        stream << "'" << data << "'";
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
                   << static_cast<bsls::Types::UintPtr>(
                                             static_cast<unsigned char>(data));
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

void Printer_Helper::printRaw(bsl::ostream&              stream,
                              const bslstl::StringRef&   data,
                              int                        ,
                              int                        spacesPerLevel,
                              bslmf::SelectTraitCase<>)
{
    // Use the defined 'operator<<' for 'bslstl::StringRef' because it does
    // not define a 'print' method and also does not define the
    // 'bslalg::HasStlIterators' type trait due to only having defined a
    // 'const_iterator' interface.

    stream << '"' << data << '"';
    if (spacesPerLevel >= 0) {
        stream << '\n';
    }
}

}  // close package namespace
}  // close enterprise namespace

// ----------------------------------------------------------------------------
// Copyright 2013 Bloomberg Finance L.P.
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
