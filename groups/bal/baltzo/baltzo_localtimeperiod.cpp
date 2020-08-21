// baltzo_localtimeperiod.cpp                                         -*-C++-*-
#include <baltzo_localtimeperiod.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(baltzo_localtimeperiod_cpp,"$Id$ $CSID$")

#include <bslim_printer.h>

#include <bsl_ostream.h>

namespace BloombergLP {
namespace baltzo {

                           // ---------------------
                           // class LocalTimePeriod
                           // ---------------------

                        // Aspects

// ACCESSORS
bsl::ostream& LocalTimePeriod::print(bsl::ostream& stream,
                                     int           level,
                                     int           spacesPerLevel) const
{
    if (stream.bad()) {
        return stream;                                                // RETURN
    }

    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    printer.printAttribute("descriptor",   d_descriptor);
    printer.printAttribute("utcStartTime", d_utcStartTime);
    printer.printAttribute("utcEndTime",   d_utcEndTime);
    printer.end();

    return stream;
}

}  // close package namespace

// FREE FUNCTIONS
void baltzo::swap(LocalTimePeriod& a, LocalTimePeriod& b)
{
    if (a.get_allocator() == b.get_allocator()) {
        a.swap(b);

        return;                                                       // RETURN
    }

    LocalTimePeriod futureA(b, a.get_allocator());
    LocalTimePeriod futureB(a, b.get_allocator());

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
