// btlso_flag.cpp                                                     -*-C++-*-
#include <btlso_flag.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(btlso_flag_cpp,"$Id$ $CSID$")

#include <bsl_ostream.h>

namespace BloombergLP {

const char *bteso_Flag::toAscii(bteso_Flag::Flag value)
{
    switch (value) {
        case k_ASYNC_INTERRUPT: return "ASYNC_INTERRUPT";
        default: return "(* UNKNOWN *)";
    }
}

const char *bteso_Flag::toAscii(bteso_Flag::BlockingMode value)
{
    switch (value) {
        case e_BLOCKING_MODE: return "BLOCKING_MODE";
        case e_NONBLOCKING_MODE: return "NONBLOCKING_MODE";
        default: return "(* UNKNOWN *)";
    }
}

const char *bteso_Flag::toAscii(bteso_Flag::ShutdownType value)
{
    switch (value) {
        case e_SHUTDOWN_RECEIVE: return "SHUTDOWN_RECEIVE";
        case e_SHUTDOWN_SEND: return "SHUTDOWN_SEND";
        case e_SHUTDOWN_BOTH: return "SHUTDOWN_BOTH";
        default: return "(* UNKNOWN *)";
    }
}

const char *bteso_Flag::toAscii(bteso_Flag::IOWaitType value)
{
    switch (value) {
        case e_IO_READ: return "IO_READ";
        case e_IO_WRITE: return "IO_WRITE";
        case e_IO_RW: return "IO_RW";
        default: return "(* UNKNOWN *)";
    }
}

bsl::ostream& bteso_Flag::streamOut(bsl::ostream&    stream,
                                    bteso_Flag::Flag rhs)
{
    return stream << bteso_Flag::toAscii(rhs);
}

bsl::ostream&  bteso_Flag::streamOut(bsl::ostream& stream,
                                     bteso_Flag::BlockingMode rhs)
{
    return stream << bteso_Flag::toAscii(rhs);
}

bsl::ostream&  bteso_Flag::streamOut(bsl::ostream& stream,
                                     bteso_Flag::ShutdownType rhs)
{
    return stream << bteso_Flag::toAscii(rhs);
}

bsl::ostream&  bteso_Flag::streamOut(bsl::ostream& stream,
                                     bteso_Flag::IOWaitType rhs)
{
    return stream << bteso_Flag::toAscii(rhs);
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
