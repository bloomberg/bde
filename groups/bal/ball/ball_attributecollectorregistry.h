// ball_attributecollectorregistry.h                                  -*-C++-*-
#ifndef INCLUDED_BALL_ATTRIBUTECOLLECTORREGISTRY
#define INCLUDED_BALL_ATTRIBUTECOLLECTORREGISTRY

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a registry for attribute collector functors.
//
//@CLASSES:
//  ball::AttributeCollectorRegistry: registry of attribute collector functors
//
//@SEE_ALSO: ball_record, ball_loggermanager
//
//@DESCRIPTION: This component provides a mechanism,
// 'ball::AttributeCollectorRegistry', that allows clients to register
// 'ball::Attribute' collection functions, and to separately apply a visitor to
// all the attributes collected using those collection functions.  A client can
// register a 'ball::Attribute' collection function by calling 'addCollector'
// and supplying a function matching the
// 'AttributeCollectorRegistry::Collector' signature.  Clients can also apply a
// visitor to collected 'ball::Attribute' objects by calling 'collect' and
// supplying a function matching the
// 'ball::AttributeCollectorRegistry::Visitor' signature.  This call to
// 'collect' will use the supplied 'Visitor' as the argument when calling each
// of the 'Collector' functors registered with 'addCollector'.
//
///Thread Safety
///-------------
// 'ball::AttributeCollectorRegistry' is fully *thread-safe*, meaning that all
// non-creator operations on an object can be safely invoked simultaneously
// from multiple threads.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Collecting Attributes From Non-uniform Sources
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// In this example we will collect a set of application properties and perform
// some manipulation of the collected data.  Note that this usage example uses
// lambdas and requires C++11.  Lambdas can be replaced with named functions
// for C++03.
//
// First, we define a few collector functions that will collect the application
// properties from various parts of an application and call the specified
// visitor functor for every collected attribute:
//..
//  void userInfo(const bsl::function<void(const ball::Attribute &)>& visitor)
//  {
//      int         uuid     = 12345;    // getUuid();
//      bsl::string userName = "proxy";  // getUserName();
//
//      visitor(ball::Attribute("myLib.uuid", uuid));
//      visitor(ball::Attribute("myLib.user", userName));
//  }
//
//  void threadInfo(
//                 const bsl::function<void(const ball::Attribute &)>& visitor)
//  {
//      int threadId = 87654;            // getThreadId();
//
//      visitor(ball::Attribute("myLib.threadId", threadId));
//  }
//..
//  Then, we register collector functions with the attribute collector
//  registry:
//..
//  ball::AttributeCollectorRegistry registry;
//
//  int rc = registry.addCollector(&userInfo, "userInfoCollector");
//  assert(0 == rc);
//  assert(true == registry.hasCollector("userInfoCollector"));
//  rc = registry.addCollector(&threadInfo, "threadInfoCollector");
//  assert(0 == rc);
//  assert(true == registry.hasCollector("threadInfoCollector"));
//  assert(2 == registry.numCollectors());
//..
//  Next, we print every attribute gathered by all registered attribute
//  collectors in the registry:
//..
//  bsl::stringstream output1;
//
//  registry.collect([&output1](const ball::Attribute& attribute)
//      {
//          output1 << attribute.name() << "=" << attribute.value() << " ";
//      });
//
//  assert("myLib.uuid=12345 myLib.user=proxy myLib.threadId=87654 "
//         == output1.str());
//..
//  Finally, we remove one of the collectors and collect attributes again:
//..
//  int rc = registry.removeCollector("threadInfoCollector");
//  assert(0 == rc);
//  assert(false == registry.hasCollector("threadInfoCollectory"));
//  assert(1 == registry.numCollectors());
//
//  bsl::stringstream output2;
//  registry.collect([&output2](const ball::Attribute& attribute)
//      {
//          output2 << attribute.name() << "=" << attribute.value() << " ";
//      });
//
//  assert("myLib.uuid=12345 myLib.user=proxy " == output2.str());
//..

#include <balscm_version.h>

#include <ball_attribute.h>

#include <bslma_allocator.h>
#include <bslma_stdallocator.h>
#include <bslma_usesbslmaallocator.h>

#include <bslmf_nestedtraitdeclaration.h>

#include <bslmt_readerwritermutex.h>

#include <bsls_compilerfeatures.h>

#include <bsl_functional.h>
#include <bsl_memory.h>
#include <bsl_string.h>
#include <bsl_utility.h>
#include <bsl_vector.h>

namespace BloombergLP {
namespace ball {

                         // ================================
                         // class AttributeCollectorRegistry
                         // ================================

class AttributeCollectorRegistry {
    // This component maintains a registry of named functors ("collectors")
    // that are used to transform opaque user data into a set of
    // 'ball::Attribute' objects.

  public:
    // TYPES
    typedef bsl::function<void(const ball::Attribute&)> Visitor;
        // 'Visitor' is the type of a user-supplied visit functor.

    typedef bsl::function<void(const Visitor&)> Collector;
        // 'Collector' is the type of a user-supplied attribute collector
        // functor.

    typedef bsl::allocator<char> allocator_type;
        // This 'typedef' is an alias for the allocator used by this object.

  private:
    // PRIVATE TYPES
    typedef bsl::pair<bsl::string, Collector>  CollectorEntry;
        // This 'typedef' is an alias for a single named attribute collector.

    typedef bsl::vector<CollectorEntry> Registry;
        // This 'typedef' is an alias for the type of the registry maintained
        // by this object.  Note that 'vector' is used to preserve the order in
        // which collectors are registered.

    // DATA
    Registry                         d_collectors; // collector registry

    mutable bslmt::ReaderWriterMutex d_rwMutex;    // protects concurrent
                                                   // access to 'd_collectors'

    // NOT IMPLEMENTED
    AttributeCollectorRegistry(const AttributeCollectorRegistry&);
    AttributeCollectorRegistry& operator=(const AttributeCollectorRegistry&);

  public:
    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(AttributeCollectorRegistry,
                                   bslma::UsesBslmaAllocator);

    // CREATORS
    AttributeCollectorRegistry();
    explicit AttributeCollectorRegistry(const allocator_type& allocator);
        // Create a registry having no registered collectors.  Optionally
        // specify an 'allocator' (e.g., the address of a 'bslma::Allocator'
        // object) to supply memory; otherwise, the default allocator is used.

#ifdef BSLS_COMPILERFEATURES_SUPPORT_DEFAULTED_FUNCTIONS
    ~AttributeCollectorRegistry() = default;
        // Destroy this registry.
#endif

    // MANIPULATORS
    int addCollector(const Collector& collector, const bsl::string_view& name);
        // Add the specified 'collector' with the specified 'name' to this
        // registry.  Return 0 if 'collector' was successfully registered, and
        // a non-zero value (with no effect) otherwise.  Note that this method
        // will fail if a collector having 'name' is already registered.

    void removeAll();
        // Remove all collectors from this registry.

    int removeCollector(const bsl::string_view& name);
        // Remove the collector having the specified 'name' from this registry.
        // Return 0 if the collector with 'name' was successfully removed, and
        // a non-zero value (with no effect) otherwise.

    // ACCESSORS
    void collect(const Visitor& visitor) const;
        // Invoke all registered collectors with the specified 'visitor'
        // functor.  Note that collectors are invoked in the order in which
        // collectors are registered.

    allocator_type get_allocator() const;
        // Return the allocator used by this object to supply memory.  Note
        // that if no allocator was supplied at construction the default
        // allocator in effect at construction is used.

    bool hasCollector(const bsl::string_view& name) const;
        // Return 'true' if a collector having the specified 'name' is in the
        // registry maintained by this object, and 'false' otherwise.  Note
        // that this method is provided primarily for debugging purposes (i.e.,
        // its return value can be invalidated from another thread).

    int numCollectors() const;
        // Return the number of collectors registered with this object.  Note
        // that this method is provided primarily for debugging purposes (i.e.,
        // its return value can be invalidated from another thread).
};

// ============================================================================
//                              INLINE DEFINITIONS
// ============================================================================

                         // --------------------------------
                         // class AttributeCollectorRegistry
                         // --------------------------------

// CREATORS
inline
AttributeCollectorRegistry::AttributeCollectorRegistry()
: d_collectors()
{
}

inline
AttributeCollectorRegistry::AttributeCollectorRegistry(
                                               const allocator_type& allocator)
: d_collectors(allocator)
{
}

// ACCESSORS
inline
AttributeCollectorRegistry::allocator_type
AttributeCollectorRegistry::get_allocator() const
{
    return d_collectors.get_allocator();
}

}  // close package namespace
}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// Copyright 2020 Bloomberg Finance L.P.
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
