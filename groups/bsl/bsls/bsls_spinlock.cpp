// bsls_spinlock.cpp                                                  -*-C++-*-
#include <bsls_spinlock.h>

#include <bsls_ident.h>
BSLS_IDENT("$Id$ $CSID$")

#if defined(BSLS_PLATFORM_CPU_X86) || defined(BSLS_PLATFORM_CPU_X86_64)
#include <immintrin.h>
#include <emmintrin.h>
#endif

#if (BSLS_COMPILERFEATURES_CPLUSPLUS < 201703L)
#define BSLS_SPINLOCK_USES_AGGREGATE_INITIALIZATION
#endif

namespace BloombergLP {

namespace bsls {

#ifdef BSLS_SPINLOCK_USES_AGGREGATE_INITIALIZATION
const SpinLock SpinLock::s_unlocked = BSLS_SPINLOCK_UNLOCKED;
#endif

void SpinLock::pause() {
#if defined(BSLS_PLATFORM_CPU_X86) || defined(BSLS_PLATFORM_CPU_X86_64)
    _mm_pause();
#endif
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
