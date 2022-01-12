// balst_stacktraceconfigurationutil.h                                -*-C++-*-

#include <balst_stacktraceconfigurationutil.h>

#include <bsls_atomicoperations.h>

using namespace BloombergLP;

namespace {
namespace u {

bsls::AtomicOperations::AtomicTypes::Int disableStackResolving = { false };

}  // close namespace u
}  // close unnamed namespace

namespace BloombergLP {
namespace balst {

                             // -----------------
                             // StackTraceConfigurationUtil
                             // -----------------

void StackTraceConfigurationUtil::disableResolution()
{
    bsls::AtomicOperations::setInt(&u::disableStackResolving, true);
}

void StackTraceConfigurationUtil::enableResolution()
{
    bsls::AtomicOperations::setInt(&u::disableStackResolving, false);
}

bool StackTraceConfigurationUtil::isResolutionDisabled()
{
    return bsls::AtomicOperations::getInt(&u::disableStackResolving);
};

}  // close package namespace
}  // close enterprise namespace

// ----------------------------------------------------------------------------
// Copyright 2021 Bloomberg Finance L.P.
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
