// baem_defaultmetricsmanager.cpp  -*-C++-*-
#include <baem_defaultmetricsmanager.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(baem_defaultmetricsmanager_cpp,"$Id$ $CSID$")

#include <baem_streampublisher.h>
#include <baem_metricsmanager.h>

#include <bcema_sharedptr.h>

#include <bslma_allocator.h>
#include <bslma_default.h>
#include <bsls_assert.h>

#include <bsl_ostream.h>

namespace BloombergLP {

class baem_Publisher;

                     // --------------------------------
                     // class baem_DefaultMetricsManager
                     // --------------------------------

// CLASS DATA
baem_MetricsManager *baem_DefaultMetricsManager::s_singleton_p = 0;
bslma_Allocator     *baem_DefaultMetricsManager::s_allocator_p = 0;

// CLASS METHODS
baem_MetricsManager *baem_DefaultMetricsManager::create(
                                               bslma_Allocator *basicAllocator)
{
    BSLS_ASSERT(0 == s_singleton_p);
    BSLS_ASSERT(0 == s_allocator_p);

    s_allocator_p = bslma_Default::globalAllocator(basicAllocator);
    s_singleton_p = new (*s_allocator_p) baem_MetricsManager(s_allocator_p);

    return s_singleton_p;
}

baem_MetricsManager *baem_DefaultMetricsManager::create(
                                              bsl::ostream&    stream,
                                              bslma_Allocator *basicAllocator)
{
    baem_MetricsManager *defaultManager = create(basicAllocator);

    bcema_SharedPtr<baem_Publisher> defaultPublisher(
                         new (*s_allocator_p) baem_StreamPublisher(stream),
                         s_allocator_p);
    defaultManager->addGeneralPublisher(defaultPublisher);

    return defaultManager;
}

void baem_DefaultMetricsManager::destroy()
{
    BSLS_ASSERT(0 != s_singleton_p);
    BSLS_ASSERT(0 != s_allocator_p);

    baem_MetricsManager *singleton = s_singleton_p;
    bslma_Allocator     *allocator = s_allocator_p;
    s_singleton_p = 0;
    s_allocator_p = 0;

    allocator->deleteObjectRaw(singleton);
}

}  // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2009
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
