// ball_attributecollectorregistry.cpp                                -*-C++-*-
#include <ball_attributecollectorregistry.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ball_attributecollectorregistry_cpp,"$Id$ $CSID$")

#include <bslmt_readlockguard.h>
#include <bslmt_writelockguard.h>

namespace BloombergLP {
namespace ball {

                         // --------------------------------
                         // class AttributeCollectorRegistry
                         // --------------------------------

// MANIPULATORS
int AttributeCollectorRegistry::addCollector(const Collector&        collector,
                                             const bsl::string_view& name)
{
    bslmt::WriteLockGuard<bslmt::ReaderWriterMutex> guard(&d_rwMutex);

    Registry::iterator first = d_collectors.begin();
    Registry::iterator last  = d_collectors.end();

    while (first != last) {
        if (first->first == name) {
            return 1;                                                 // RETURN
        }
        ++first;
    }

    d_collectors.emplace_back(name, collector);
    return 0;
}

void AttributeCollectorRegistry::removeAll()
{
    bslmt::WriteLockGuard<bslmt::ReaderWriterMutex> guard(&d_rwMutex);
    d_collectors.clear();
}

int AttributeCollectorRegistry::removeCollector(const bsl::string_view& name)
{
    bslmt::WriteLockGuard<bslmt::ReaderWriterMutex> guard(&d_rwMutex);

    Registry::iterator first = d_collectors.begin();
    Registry::iterator last  = d_collectors.end();

    while (first != last) {
        if (first->first == name) {
            d_collectors.erase(first);
            return 0;                                                 // RETURN
        }
        ++first;
    }
    return 1;
}

// ACCESSORS
void AttributeCollectorRegistry::collect(const Visitor& visitor) const
{
    bslmt::ReadLockGuard<bslmt::ReaderWriterMutex> guard(&d_rwMutex);

    for (Registry::const_iterator it = d_collectors.cbegin();
         it != d_collectors.cend();
         ++it) {
        it->second(visitor);
    }
}

bool AttributeCollectorRegistry::hasCollector(const bsl::string_view& name)
                                                                          const
{
    bslmt::ReadLockGuard<bslmt::ReaderWriterMutex> guard(&d_rwMutex);

    Registry::const_iterator first = d_collectors.cbegin();
    Registry::const_iterator last  = d_collectors.cend();

    while (first != last) {
        if (first->first == name) {
            return true;                                              // RETURN
        }
        ++first;
    }
    return false;
}

int AttributeCollectorRegistry::numCollectors() const
{
    bslmt::ReadLockGuard<bslmt::ReaderWriterMutex> guard(&d_rwMutex);

    return static_cast<int>(d_collectors.size());
}

}  // close package namespace
}  // close enterprise namespace

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
