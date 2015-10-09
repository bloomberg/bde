// btlso_flag.cpp                                                     -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <btlso_flag.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(btlso_flag_cpp,"$Id$ $CSID$")

#include <bsl_ostream.h>

namespace BloombergLP {
namespace btlso {

const char *Flag::toAscii(Flag::FlagType value)
{
    switch (value) {
        case k_ASYNC_INTERRUPT: return "ASYNC_INTERRUPT";
        default: return "(* UNKNOWN *)";
    }
}

const char *Flag::toAscii(Flag::BlockingMode value)
{
#define CASE(X) case(e_ ## X): return #X;

    switch (value) {
      CASE(BLOCKING_MODE)
      CASE(NONBLOCKING_MODE)
      default: return "(* UNKNOWN *)";
    }

#undef CASE
}

const char *Flag::toAscii(Flag::ShutdownType value)
{
#define CASE(X) case(e_ ## X): return #X;

    switch (value) {
      CASE(SHUTDOWN_RECEIVE)
      CASE(SHUTDOWN_SEND)
      CASE(SHUTDOWN_BOTH)
      default: return "(* UNKNOWN *)";
    }

#undef CASE
}

const char *Flag::toAscii(Flag::IOWaitType value)
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

bsl::ostream& Flag::streamOut(bsl::ostream& stream, Flag::FlagType rhs)
{
    return stream << Flag::toAscii(rhs);
}

bsl::ostream& Flag::streamOut(bsl::ostream& stream, Flag::BlockingMode rhs)
{
    return stream << Flag::toAscii(rhs);
}

bsl::ostream& Flag::streamOut(bsl::ostream& stream, Flag::ShutdownType rhs)
{
    return stream << Flag::toAscii(rhs);
}

bsl::ostream& Flag::streamOut(bsl::ostream& stream, Flag::IOWaitType rhs)
{
    return stream << Flag::toAscii(rhs);
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
