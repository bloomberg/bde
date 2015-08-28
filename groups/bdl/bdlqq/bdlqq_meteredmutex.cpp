// bdlqq_meteredmutex.cpp                                             -*-C++-*-
#include <bdlqq_meteredmutex.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdlqq_meteredmutex_cpp,"$Id$ $CSID$")

#include <bdlqq_barrier.h>  // for testing only

#include <bsls_types.h>

namespace BloombergLP {

                             // ------------------
                             // class MeteredMutex
                             // ------------------

// MANIPULATORS
void bdlqq::MeteredMutex::resetMetrics()
{
    // Loop is used here to ensure that 'd_lastResetTime' is not
    // modified (by some other thread) between the computation
    // of 't1' and the modification of d_lastResetTime by 'testAndSwap'.

    bsls::Types::Int64 t1, old;

    d_holdTime = 0;
    d_waitTime = 0;
    do {
        old = d_lastResetTime;
        t1 = bsls::TimeUtil::getTimer();
    } while(d_lastResetTime.testAndSwap(old, t1) != old);
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
