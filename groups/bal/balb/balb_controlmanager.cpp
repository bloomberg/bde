// balb_controlmanager.cpp                                            -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <balb_controlmanager.h>

#include <ball_log.h>

#include <bslmt_readlockguard.h>
#include <bslmt_writelockguard.h>

#include <bslma_default.h>
#include <bsls_assert.h>
#include <bdlb_string.h>

#include <bsl_algorithm.h>
#include <bsl_functional.h>
#include <bsl_iostream.h>
#include <bsl_memory.h>
#include <bsl_sstream.h>
#include <bsl_utility.h>

namespace BloombergLP {

namespace {

template <class FIRST, class SECOND>
struct MapKeyExtractor {
   const FIRST& operator() (const bsl::pair<FIRST, SECOND>& pair) {
      return pair.first;
   }
};

template <class FIRST, class SECOND, class TC, class TA>
MapKeyExtractor<FIRST, SECOND>
GetExtractor(const bsl::map<FIRST, SECOND, TC, TA>&) {
   return MapKeyExtractor<FIRST, SECOND>();
}

inline
bool isLessThanCaseless(const bsl::string& lhsString,
                        const bsl::string& rhsString)
{
   return -1 == bdlb::String::lowerCaseCmp(lhsString, rhsString);
}

const char LOG_CATEGORY[] = "BALB.CONTROLMANAGER";

}  // close unnamed namespace

namespace balb {
                            // --------------------
                            // class ControlManager
                            // --------------------

// CREATORS

ControlManager::ControlManager(bslma::Allocator *basicAllocator)
: d_allocator_p(bslma::Default::allocator(basicAllocator))
, d_registry(&isLessThanCaseless, basicAllocator)
{ }

ControlManager::~ControlManager()
{ }

// MANIPULATORS

int ControlManager::registerHandler(const bsl::string&    prefix,
                                    const bsl::string&    arguments,
                                    const bsl::string&    description,
                                    const ControlHandler& handler)
{
    bslmt::WriteLockGuard<bslmt::RWMutex> guard(&d_registryMutex);

    int rc = 0;
    ControlManager_Entry entry(handler, arguments, description);

    Registry::iterator it = d_registry.find(prefix);
    if (it != d_registry.end()) {
        it->second = entry;
        rc = 1;
    }
    else {
        d_registry.insert(bsl::make_pair(prefix, entry));
    }

    return rc;
}

int ControlManager::deregisterHandler(const bsl::string& prefix)
{
    bslmt::WriteLockGuard<bslmt::RWMutex> guard(&d_registryMutex);

    return (0 == d_registry.erase(prefix));
}

int ControlManager::dispatchMessage(const bsl::string& message) const
{
    BALL_LOG_SET_CATEGORY(LOG_CATEGORY);
    BALL_LOG_TRACE << "Dispatching control message '" << message << "'"
                   << BALL_LOG_END;

    bsl::string token;
    bsl::istringstream messageStream(message);

    messageStream >> token;

    bslmt::ReadLockGuard<bslmt::RWMutex> registryGuard(&d_registryMutex);
    Registry::const_iterator it = d_registry.find(token);

    if (it != d_registry.end()) {
        ControlHandler callback =  it->second.callback();
        registryGuard.release()->unlock();
        callback(token, messageStream);
        return 0;                                                     // RETURN
    }

    return -1;
}

int ControlManager::dispatchMessage(const bsl::string& prefix,
                                    bsl::istream&      stream) const
{
    BALL_LOG_SET_CATEGORY(LOG_CATEGORY);
    BALL_LOG_TRACE << "Dispatching control message '" << prefix << "'"
                   << BALL_LOG_END;

    bslmt::ReadLockGuard<bslmt::RWMutex> registryGuard(&d_registryMutex);
    Registry::const_iterator it = d_registry.find(prefix);

    if (it != d_registry.end()) {
        ControlHandler callback =  it->second.callback();
        registryGuard.release()->unlock();
        callback(prefix, stream);
        return 0;                                                     // RETURN
    }

    return -1;
}

void ControlManager::printUsage(bsl::ostream&      stream,
                                const bsl::string& preamble) const
{
    stream << preamble << bsl::endl;
    d_registryMutex.lockRead();
    for (Registry::const_iterator it =  d_registry.begin();
                                  it != d_registry.end();
                                ++it)
    {
       stream << "    " << it->first << " " << it->second.arguments()
              << bsl::endl;
       if (!it->second.description().empty()) {
          stream << "        " << it->second.description() << bsl::endl;
       }
    }
    d_registryMutex.unlock();
}

                        // --------------------------
                        // class ControlManager_Entry
                        // --------------------------

// CREATORS

ControlManager_Entry::ControlManager_Entry(bslma::Allocator *basicAllocator)
: d_callback(bsl::allocator_arg_t(),
             bsl::allocator<ControlManager::ControlHandler>(basicAllocator))
, d_arguments(basicAllocator)
, d_description(basicAllocator)
{}

ControlManager_Entry::~ControlManager_Entry()
{}

ControlManager_Entry::ControlManager_Entry(
                         const ControlManager::ControlHandler&  callback,
                         const bsl::string&                     arguments,
                         const bsl::string&                     description,
                         bslma::Allocator                      *basicAllocator)
: d_callback(bsl::allocator_arg_t(),
             bsl::allocator<ControlManager::ControlHandler>(basicAllocator),
             callback)
, d_arguments(arguments, basicAllocator)
, d_description(description, basicAllocator)
{}

ControlManager_Entry::ControlManager_Entry(
                                   const ControlManager_Entry&  original,
                                   bslma::Allocator            *basicAllocator)
: d_callback(bsl::allocator_arg_t(),
             bsl::allocator<ControlManager::ControlHandler>(basicAllocator),
             original.d_callback)
, d_arguments(original.d_arguments, basicAllocator)
, d_description(original.d_description, basicAllocator)
{}

// MANIPULATORS

ControlManager_Entry&
ControlManager_Entry::operator=(const ControlManager_Entry& rhs)
{
   if (&rhs != this) {
      d_callback    = rhs.d_callback;
      d_arguments   = rhs.d_arguments;
      d_description = rhs.d_description;
   }
   return *this;
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
