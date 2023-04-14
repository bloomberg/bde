// bslmt_threadattributes.cpp                                         -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <bslmt_threadattributes.h>

#include <bslim_printer.h>

#include <bslmt_platform.h>

#include <bsls_assert.h>
#include <bsls_platform.h>

#include <bsl_cstdlib.h>
#include <bsl_cstring.h>
#include <bsl_c_limits.h>
#include <bsl_ostream.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bslmt_threadattributes_cpp,"$Id$ $CSID$")

namespace BloombergLP {

// CREATORS
bslmt::ThreadAttributes::ThreadAttributes()
: d_detachedState(e_CREATE_JOINABLE)
, d_guardSize(e_UNSET_GUARD_SIZE)
, d_inheritScheduleFlag(true)
, d_schedulingPolicy(e_SCHED_DEFAULT)
, d_schedulingPriority(e_UNSET_PRIORITY)
, d_stackSize(e_UNSET_STACK_SIZE)
, d_threadName(static_cast<bslma::Allocator *>(0))
{
}

bslmt::ThreadAttributes::ThreadAttributes(bslma::Allocator *basicAllocator)
: d_detachedState(e_CREATE_JOINABLE)
, d_guardSize(e_UNSET_GUARD_SIZE)
, d_inheritScheduleFlag(true)
, d_schedulingPolicy(e_SCHED_DEFAULT)
, d_schedulingPriority(e_UNSET_PRIORITY)
, d_stackSize(e_UNSET_STACK_SIZE)
, d_threadName(basicAllocator)
{
}

bslmt::ThreadAttributes::ThreadAttributes(
                                const bslmt::ThreadAttributes&  original,
                                bslma::Allocator               *basicAllocator)

: d_detachedState(original.d_detachedState)
, d_guardSize(original.d_guardSize)
, d_inheritScheduleFlag(original.d_inheritScheduleFlag)
, d_schedulingPolicy(original.d_schedulingPolicy)
, d_schedulingPriority(original.d_schedulingPriority)
, d_stackSize(original.d_stackSize)
, d_threadName(original.d_threadName, basicAllocator)
{
}

// MANIPULATORS
bslmt::ThreadAttributes& bslmt::ThreadAttributes::operator=(
                                            const bslmt::ThreadAttributes& rhs)
{
    d_detachedState       = rhs.d_detachedState;
    d_guardSize           = rhs.d_guardSize;
    d_inheritScheduleFlag = rhs.d_inheritScheduleFlag;
    d_schedulingPolicy    = rhs.d_schedulingPolicy;
    d_schedulingPriority  = rhs.d_schedulingPriority;
    d_stackSize           = rhs.d_stackSize;
    d_threadName          = rhs.d_threadName;

    return *this;
}

// ACCESSORS
bsl::ostream& bslmt::ThreadAttributes::print(
                                            bsl::ostream& stream,
                                            int           level,
                                            int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();

    printer.printAttribute("detachedState", d_detachedState);
    printer.printAttribute("guardSize", d_guardSize);
    printer.printAttribute("inheritSchedule", d_inheritScheduleFlag);
    printer.printAttribute("schedulingPolicy", d_schedulingPolicy);
    printer.printAttribute("schedulingPriority", d_schedulingPriority);
    printer.printAttribute("stackSize", d_stackSize);
    printer.printAttribute("threadName", d_threadName);

    printer.end();
    return stream;
}

// FREE OPERATORS
bool bslmt::operator==(const ThreadAttributes& lhs,
                       const ThreadAttributes& rhs)
{
    return lhs.detachedState()      == rhs.detachedState()      &&
           lhs.guardSize()          == rhs.guardSize()          &&
           lhs.inheritSchedule()    == rhs.inheritSchedule()    &&
           lhs.schedulingPolicy()   == rhs.schedulingPolicy()   &&
           lhs.schedulingPriority() == rhs.schedulingPriority() &&
           lhs.stackSize()          == rhs.stackSize()          &&
           lhs.threadName()         == rhs.threadName();
}

bool bslmt::operator!=(const ThreadAttributes& lhs,
                       const ThreadAttributes& rhs)
{
    return lhs.detachedState()      != rhs.detachedState()      ||
           lhs.guardSize()          != rhs.guardSize()          ||
           lhs.inheritSchedule()    != rhs.inheritSchedule()    ||
           lhs.schedulingPolicy()   != rhs.schedulingPolicy()   ||
           lhs.schedulingPriority() != rhs.schedulingPriority() ||
           lhs.stackSize()          != rhs.stackSize()          ||
           lhs.threadName()         != rhs.threadName();
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
