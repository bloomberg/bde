// btlmt_connectoptions.cpp                                           -*-C++-*-

#include <btlmt_connectoptions.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(btlmt_connectoptions_cpp,"$Id$ $CSID$")

#include <bslim_printer.h>

#include <bsl_ios.h>
#include <bsl_ostream.h>

namespace BloombergLP {
namespace btlmt {

                       // --------------------
                       // class ConnectOptions
                       // --------------------

// CREATORS
ConnectOptions::ConnectOptions(bslma::Allocator *basicAllocator)
: d_serverEndpoint(basicAllocator)
, d_numAttempts(1)
, d_timeout()
, d_enableRead(true)
, d_allowHalfOpenConnections(false)
, d_resolutionMode(btlmt::ResolutionMode::e_RESOLVE_ONCE)
, d_socketOptions()
, d_localAddress()
, d_socketPtr()
{
}

// MANIPULATORS
ConnectOptions& ConnectOptions::operator=(const ConnectOptions& rhs)
{
    ConnectOptions(rhs, d_serverEndpoint.getAllocator()).swap(*this);

    return *this;
}

void ConnectOptions::reset()
{
    d_serverEndpoint = ServerEndpoint();
    d_numAttempts = 1;
    d_timeout = bsls::TimeInterval();
    d_enableRead = true;
    d_allowHalfOpenConnections = false;
    d_resolutionMode = btlmt::ResolutionMode::e_RESOLVE_ONCE;
    d_socketOptions.reset();
    d_localAddress.reset();
    d_socketPtr = 0;
}

// ACCESSORS
bsl::ostream& ConnectOptions::print(bsl::ostream& stream,
                                    int           level,
                                    int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    printer.printAttribute("serverEndpoint", d_serverEndpoint);
    printer.printAttribute("numAttempts",    d_numAttempts);
    printer.printAttribute("timeout",        d_timeout);
    printer.printAttribute("enableRead",     d_enableRead);
    printer.printAttribute("allowHalfOpenConnections",
                                             d_allowHalfOpenConnections);
    printer.printAttribute("resolutionMode",
                           static_cast<int>(d_resolutionMode));
    printer.printAttribute("socketOptions",  d_socketOptions);
    printer.printAttribute("localAddress",   d_localAddress);
    if (d_socketPtr) {
        printer.printAttribute("socketPtr",
                               static_cast<void *>(d_socketPtr->get()));
    }
    printer.end();

    return stream;
}

bsl::ostream& operator<<(bsl::ostream&                stream,
                         const btlmt::ConnectOptions& object)
{
    bslim::Printer printer(&stream, 0, -1);
    printer.start();
    if (object.serverEndpoint().is<btlso::IPv4Address>()) {
        printer.printValue(object.serverEndpoint().the<btlso::IPv4Address>());
    }
    else if (object.serverEndpoint().is<btlso::Endpoint>()) {
        printer.printValue(object.serverEndpoint().the<btlso::Endpoint>());
    }
    printer.printValue(object.numAttempts());
    printer.printValue(object.timeout());
    printer.printValue(object.enableRead());
    printer.printValue(object.allowHalfOpenConnections());
    printer.printValue(static_cast<int>(object.resolutionMode()));
    printer.printValue(object.socketOptions());
    printer.printValue(object.localAddress());
    if (object.socketPtr()) {
        printer.printValue(static_cast<void *>(object.socketPtr()->get()));
    }
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
