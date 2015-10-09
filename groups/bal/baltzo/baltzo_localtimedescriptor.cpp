// baltzo_localtimedescriptor.cpp                                     -*-C++-*-
#include <baltzo_localtimedescriptor.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(baltzo_localtimedescriptor_cpp,"$Id$ $CSID$")

#include <bslim_printer.h>

#include <bsl_ios.h>
#include <bsl_ostream.h>

namespace BloombergLP {
                         // -------------------------
                         // class LocalTimeDescriptor
                         // -------------------------

// ACCESSORS

                                  // Aspects

bsl::ostream& baltzo::LocalTimeDescriptor::print(
                                            bsl::ostream& stream,
                                            int           level,
                                            int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    printer.printAttribute("utcOffsetInSeconds", d_utcOffsetInSeconds);
    printer.printAttribute("dstInEffectFlag",    d_dstInEffectFlag);
    printer.printAttribute("description",        d_description.c_str());
    printer.end();

    return stream;
}

// FREE OPERATORS
bsl::ostream& baltzo::operator<<(bsl::ostream&              stream,
                                 const LocalTimeDescriptor& object)
{
    bslim::Printer printer(&stream, 0, -1);
    printer.start();
    printer.printValue(object.utcOffsetInSeconds());
    printer.printValue(object.dstInEffectFlag());
    printer.printValue(object.description().c_str());
    printer.end();

    return stream;
}

}  // close enterprise namespace

// ----------------------------------------------------------------------------
// Copyright 2015 Bloomberg Finance L.P.
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
