// balm_defaultmetricsmanager.cpp                                     -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <balm_defaultmetricsmanager.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(balm_defaultmetricsmanager_cpp,"$Id$ $CSID$")

#include <balm_streampublisher.h>
#include <balm_metricsmanager.h>

#include <bslma_allocator.h>
#include <bslma_default.h>
#include <bsls_assert.h>

#include <bsl_memory.h>
#include <bsl_ostream.h>
#include <bsl_string.h>

namespace BloombergLP {

namespace balm { class Publisher; }

                     // ---------------------------------
                     // class balm::DefaultMetricsManager
                     // ---------------------------------

// CLASS DATA
balm::MetricsManager *balm::DefaultMetricsManager::s_singleton_p = 0;
bslma::Allocator     *balm::DefaultMetricsManager::s_allocator_p = 0;

namespace balm {
// CLASS METHODS
MetricsManager *DefaultMetricsManager::create(bslma::Allocator *basicAllocator)
{
    BSLS_ASSERT(0 == s_singleton_p);
    BSLS_ASSERT(0 == s_allocator_p);

    s_allocator_p = bslma::Default::globalAllocator(basicAllocator);
    s_singleton_p = new (*s_allocator_p) MetricsManager(s_allocator_p);

    return s_singleton_p;
}

MetricsManager *DefaultMetricsManager::create(bsl::ostream&     stream,
                                              bslma::Allocator *basicAllocator)
{
    MetricsManager *defaultManager = create(basicAllocator);

    bsl::shared_ptr<Publisher> defaultPublisher(
                         new (*s_allocator_p) StreamPublisher(stream),
                         s_allocator_p);
    defaultManager->addGeneralPublisher(defaultPublisher);

    return defaultManager;
}

void DefaultMetricsManager::destroy()
{
    BSLS_ASSERT(0 != s_singleton_p);
    BSLS_ASSERT(0 != s_allocator_p);

    MetricsManager *singleton = s_singleton_p;
    bslma::Allocator    *allocator = s_allocator_p;
    s_singleton_p = 0;
    s_allocator_p = 0;

    allocator->deleteObjectRaw(singleton);
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
