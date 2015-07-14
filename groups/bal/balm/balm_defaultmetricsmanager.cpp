// balm_defaultmetricsmanager.cpp                                     -*-C++-*-
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

namespace BloombergLP {


namespace balm {class Publisher;
}  // close package namespace

                     // --------------------------------
                     // class balm::DefaultMetricsManager
                     // --------------------------------

// CLASS DATA
balm::MetricsManager *balm::DefaultMetricsManager::s_singleton_p = 0;
bslma::Allocator    *balm::DefaultMetricsManager::s_allocator_p = 0;

namespace balm {
// CLASS METHODS
MetricsManager *DefaultMetricsManager::create(
                                              bslma::Allocator *basicAllocator)
{
    BSLS_ASSERT(0 == s_singleton_p);
    BSLS_ASSERT(0 == s_allocator_p);

    s_allocator_p = bslma::Default::globalAllocator(basicAllocator);
    s_singleton_p = new (*s_allocator_p) MetricsManager(s_allocator_p);

    return s_singleton_p;
}

MetricsManager *DefaultMetricsManager::create(
                                              bsl::ostream&     stream,
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

}  // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2009
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
