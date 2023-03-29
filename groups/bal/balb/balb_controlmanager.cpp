// balb_controlmanager.cpp                                            -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <balb_controlmanager.h>

#include <bslmt_readlockguard.h>
#include <bslmt_writelockguard.h>

#include <bdlsb_fixedmeminstreambuf.h>

#include <bdlf_bind.h>

#include <bdlb_string.h>
#include <bdlb_stringviewutil.h>

#include <bslma_default.h>
#include <bsls_assert.h>
#include <bsls_log.h>

#include <bsl_algorithm.h>
#include <bsl_functional.h>
#include <bsl_iostream.h>
#include <bsl_memory.h>
#include <bsl_sstream.h>
#include <bsl_utility.h>

namespace BloombergLP {
namespace balb {

                    // --------------------------------------
                    // class ControlManager::CaselessLessThan
                    // --------------------------------------

inline
bool ControlManager::CaselessLessThan::operator()(
                                             const bsl::string_view& lhs,
                                             const bsl::string_view& rhs) const
{
    return bdlb::StringViewUtil::lowerCaseCmp(lhs, rhs) < 0;
}

                            // --------------------
                            // class ControlManager
                            // --------------------

// CREATORS

ControlManager::ControlManager(bslma::Allocator *basicAllocator)
: d_allocator_p(bslma::Default::allocator(basicAllocator))
, d_registry(basicAllocator)
{ }

ControlManager::~ControlManager()
{ }

// MANIPULATORS
int ControlManager::registerHandler(const bsl::string_view& prefix,
                                    const bsl::string_view& arguments,
                                    const bsl::string_view& description,
                                    const ControlHandler&   handler)
{
    typedef Registry::iterator Iterator;

    bslmt::WriteLockGuard<bslmt::RWMutex> guard(&d_registryMutex);

    int rc = 0;
    ControlManager_Entry entry(handler, arguments, description);

    Iterator it = d_registry.find(prefix);
    if (it != d_registry.end()) {
        it->second = entry;
        rc = 1;
    }
    else {
        bsl::pair<Iterator, bool> rcPair = d_registry.emplace(prefix, entry);
        BSLS_ASSERT(rcPair.second);    (void) rcPair;
    }

    return rc;
}

int ControlManager::registerUsageHandler(bsl::ostream& stream)
{
    return registerHandler(
                "HELP",
                "",  // Help message takes no additional fields.
                "Display this message",
                 bdlf::BindUtil::bind(
                         &ControlManager::printUsageHelper,
                         this,
                         &stream,
                         "This process responds to the following messages: "));
}

int ControlManager::deregisterHandler(const bsl::string_view& prefix)
{
    bslmt::WriteLockGuard<bslmt::RWMutex> guard(&d_registryMutex);

    Registry::iterator it = d_registry.find(prefix);
    if (d_registry.end() != it) {
        d_registry.erase(it);

        return 0;                                                     // RETURN
    }

    return 1;
}

int ControlManager::dispatchMessage(const bsl::string_view& message) const
{
    const int len = static_cast<int>(message.length());
    BSLS_LOG_TRACE("Dispatching control message '%.*s'", len, message.data());

    bdlsb::FixedMemInStreamBuf isb(message.data(), message.length());
    bsl::istream messageStream(&isb);

    bsl::string token;
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
    // Imp note: 'prefix' has to be 'bsl::string' and not a string view for the
    // sake of being able to be passed to the callback.

    BSLS_LOG_TRACE("Dispatching control message '%s'", prefix.c_str());

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

void ControlManager::printUsage(bsl::ostream&           stream,
                                const bsl::string_view& preamble) const
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

ControlManager::ControlManager_Entry::ControlManager_Entry(
                                              bslma::Allocator *basicAllocator)
: d_callback(bsl::allocator_arg_t(),
             bsl::allocator<ControlManager::ControlHandler>(basicAllocator))
, d_arguments(basicAllocator)
, d_description(basicAllocator)
{}

ControlManager::ControlManager_Entry::~ControlManager_Entry()
{}

ControlManager::ControlManager_Entry::ControlManager_Entry(
                         const ControlManager::ControlHandler&  callback,
                         const bsl::string_view&                arguments,
                         const bsl::string_view&                description,
                         bslma::Allocator                      *basicAllocator)
: d_callback(bsl::allocator_arg_t(),
             bsl::allocator<ControlManager::ControlHandler>(basicAllocator),
             callback)
, d_arguments(arguments, basicAllocator)
, d_description(description, basicAllocator)
{}

ControlManager::ControlManager_Entry::ControlManager_Entry(
                                   const ControlManager_Entry&  original,
                                   bslma::Allocator            *basicAllocator)
: d_callback(bsl::allocator_arg_t(),
             bsl::allocator<ControlManager::ControlHandler>(basicAllocator),
             original.d_callback)
, d_arguments(original.d_arguments, basicAllocator)
, d_description(original.d_description, basicAllocator)
{}

// MANIPULATORS

ControlManager::ControlManager_Entry&
ControlManager::ControlManager_Entry::operator=(
                                               const ControlManager_Entry& rhs)
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
