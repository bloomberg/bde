// bslmt_entrypointfunctoradapter.cpp                                 -*-C++-*-
#include <bslmt_entrypointfunctoradapter.h>

#include <bsls_ident.h>
BSLS_IDENT("$Id$ $CSID$")

namespace BloombergLP {

void *bslmt_EntryPointFunctorAdapter_invoker(void* argument) {
    bslmt::EntryPointFunctorAdapter_Base* threadArg =
        (bslmt::EntryPointFunctorAdapter_Base*)argument;
    threadArg->function()(argument);
    return 0;
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
