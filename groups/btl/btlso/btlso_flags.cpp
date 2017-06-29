// btlso_flags.cpp                                                    -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <btlso_flags.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(btlso_flags_cpp,"$Id$ $CSID$")

#include <bsl_ostream.h>

namespace BloombergLP {
namespace btlso {

const char *Flags::toAscii(Flags::Flag value)
{
    switch (value) {
        case k_ASYNC_INTERRUPT: return "ASYNC_INTERRUPT";
        default: return "(* UNKNOWN *)";
    }
}

const char *Flags::toAscii(Flags::BlockingMode value)
{
#define CASE(X) case(e_ ## X): return #X;

    switch (value) {
      CASE(BLOCKING_MODE)
      CASE(NONBLOCKING_MODE)
      default: return "(* UNKNOWN *)";
    }

#undef CASE
}

const char *Flags::toAscii(Flags::ShutdownType value)
{
#define CASE(X) case(e_ ## X): return #X;

    switch (value) {
      CASE(SHUTDOWN_RECEIVE)
      CASE(SHUTDOWN_SEND)
      CASE(SHUTDOWN_BOTH)
      CASE(SHUTDOWN_GRACEFUL)
      default: return "(* UNKNOWN *)";
    }

#undef CASE
}

const char *Flags::toAscii(Flags::IOWaitType value)
{
#define CASE(X) case(e_ ## X): return #X;

    switch (value) {
      CASE(IO_READ)
      CASE(IO_WRITE)
      CASE(IO_RW)
      default: return "(* UNKNOWN *)";
    }

#undef CASE
}

bsl::ostream& Flags::streamOut(bsl::ostream& stream, Flags::Flag rhs)
{
    return stream << Flags::toAscii(rhs);
}

bsl::ostream& Flags::streamOut(bsl::ostream& stream, Flags::BlockingMode rhs)
{
    return stream << Flags::toAscii(rhs);
}

bsl::ostream& Flags::streamOut(bsl::ostream& stream, Flags::ShutdownType rhs)
{
    return stream << Flags::toAscii(rhs);
}

bsl::ostream& Flags::streamOut(bsl::ostream& stream, Flags::IOWaitType rhs)
{
    return stream << Flags::toAscii(rhs);
}

}  // close package namespace
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
