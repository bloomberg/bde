// btlmt_listenoptions.cpp                                            -*-C++-*-

#include <btlmt_listenoptions.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(btlmt_listenoptions_cpp,"$Id$ $CSID$")

#include <bslim_printer.h>

#include <bsl_ios.h>
#include <bsl_ostream.h>

namespace BloombergLP {
namespace btlmt {

                       // -------------------
                       // class ListenOptions
                       // -------------------

// CREATORS
ListenOptions::ListenOptions()
: d_serverAddress()
, d_backlog(1)
, d_timeout()
, d_enableRead(true)
, d_allowHalfOpenConnections(false)
, d_socketOptions()
{
}

// MANIPULATORS
ListenOptions& ListenOptions::operator=(const ListenOptions& rhs)
{
    d_serverAddress = rhs.d_serverAddress;
    d_backlog = rhs.d_backlog;
    d_timeout = rhs.d_timeout;
    d_enableRead = rhs.d_enableRead;
    d_allowHalfOpenConnections = rhs.d_allowHalfOpenConnections;
    d_socketOptions = rhs.d_socketOptions;

    return *this;
}

void ListenOptions::reset()
{
    d_serverAddress = btlso::IPv4Address();
    d_backlog = 1;
    d_timeout.reset();
    d_enableRead = true;
    d_allowHalfOpenConnections = false;
    d_socketOptions.reset();
}

// ACCESSORS
bsl::ostream& ListenOptions::print(bsl::ostream& stream,
                                   int           level,
                                   int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    printer.printAttribute("serverAddress", d_serverAddress);
    printer.printAttribute("backlog",       d_backlog);
    printer.printAttribute("timeout",       d_timeout);
    printer.printAttribute("enableRead",    d_enableRead);
    printer.printAttribute("allowHalfOpenConnections",
                                            d_allowHalfOpenConnections);
    printer.printAttribute("socketOptions", d_socketOptions);
    printer.end();

    return stream;
}

bsl::ostream& operator<<(bsl::ostream&               stream,
                         const btlmt::ListenOptions& object)
{
    bslim::Printer printer(&stream, 0, -1);
    printer.start();
    printer.printValue(object.serverAddress());
    printer.printValue(object.backlog());
    printer.printValue(object.timeout());
    printer.printValue(object.enableRead());
    printer.printValue(object.allowHalfOpenConnections());
    printer.printValue(object.socketOptions());
    printer.end();

    return stream;
}

}  // close package namespace

}  // close enterprise namespace

// ----------------------------------------------------------------------------
// Copyright 2016 Bloomberg Finance L.P.
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
