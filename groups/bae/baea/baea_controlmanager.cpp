// baea_controlmanager.cpp   -*-C++-*-
#include <baea_controlmanager.h>

#include <bael_log.h>

#include <bcemt_readlockguard.h>
#include <bcemt_writelockguard.h>

#include <bslma_default.h>
#include <bsls_assert.h>
#include <bdeu_string.h>

#include <bsl_algorithm.h>
#include <bsl_functional.h>
#include <bsl_iostream.h>
#include <bsl_sstream.h>

namespace BloombergLP {

namespace {

template <typename FIRST, typename SECOND>
struct MapKeyExtractor {
   const FIRST& operator() (const bsl::pair<FIRST, SECOND>& pair) {
      return pair.first;
   }
};

template <typename FIRST, typename SECOND, typename TC, typename TA>
MapKeyExtractor<FIRST, SECOND>
GetExtractor(const bsl::map<FIRST, SECOND, TC, TA>&) {
   return MapKeyExtractor<FIRST, SECOND>();
}

inline bool isLessThanCaseless(const bsl::string& lhsString,
                        const bsl::string& rhsString)
{
   return -1 == bdeu_String::lowerCaseCmp(lhsString, rhsString);
}

const char LOG_CATEGORY[] = "BAEA.CONTROLMANAGER";

}  // close unnamed namespace

                      // -------------------------
                      // class baea_ControlManager
                      // -------------------------

// CREATORS

baea_ControlManager::baea_ControlManager(bslma_Allocator *basicAllocator)
: d_allocator_p(bslma_Default::allocator(basicAllocator))
, d_registry(&isLessThanCaseless, basicAllocator)
{
}

baea_ControlManager::~baea_ControlManager()
{}

// MANIPULATORS

int baea_ControlManager::registerHandler(
        const bsl::string&                         prefix,
        const bsl::string&                         arguments,
        const bsl::string&                         description,
        const baea_ControlManager::ControlHandler& handler)
{
    bcemt_WriteLockGuard<bcemt_RWMutex> guard(&d_registryMutex);

    int rc = 0;
    baea_ControlManager_Entry entry(handler, arguments, description);

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

int baea_ControlManager::deregisterHandler(const bsl::string& prefix)
{
    bcemt_WriteLockGuard<bcemt_RWMutex> guard(&d_registryMutex);

    return (0 == d_registry.erase(prefix));
}

int baea_ControlManager::dispatchMessage(const bsl::string& message) const
{
    BAEL_LOG_SET_CATEGORY(LOG_CATEGORY);
    BAEL_LOG_TRACE << "Dispatching control message '" << message << "'"
                   << BAEL_LOG_END;

    bsl::string token;
    bsl::istringstream messageStream(message);

    messageStream >> token;

    bcemt_ReadLockGuard<bcemt_RWMutex> registryGuard(&d_registryMutex);
    Registry::const_iterator it = d_registry.find(token);

    if (it != d_registry.end()) {
        ControlHandler callback =  it->second.callback();
        registryGuard.release()->unlock();
        callback(token, messageStream);
        return 0;
    }

    return -1;
}

int baea_ControlManager::dispatchMessage(
        const bsl::string& prefix,
        bsl::istream&      stream) const
{
    BAEL_LOG_SET_CATEGORY(LOG_CATEGORY);
    BAEL_LOG_TRACE << "Dispatching control message '" << prefix << "'"
                   << BAEL_LOG_END;

    bcemt_ReadLockGuard<bcemt_RWMutex> registryGuard(&d_registryMutex);
    Registry::const_iterator it = d_registry.find(prefix);

    if (it != d_registry.end()) {
        ControlHandler callback =  it->second.callback();
        registryGuard.release()->unlock();
        callback(prefix, stream);
        return 0;
    }

    return -1;
}

void baea_ControlManager::printUsage(
        bsl::ostream       &stream,
        const bsl::string&  preamble) const
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

                       // ------------------------------
                       // class baea_ControlManager_Entry
                       // ------------------------------

// CREATORS

baea_ControlManager_Entry::baea_ControlManager_Entry(
        bslma_Allocator *basicAllocator)
: d_callback(basicAllocator)
, d_arguments(basicAllocator)
, d_description(basicAllocator)
{}

baea_ControlManager_Entry::~baea_ControlManager_Entry()
{}

baea_ControlManager_Entry::baea_ControlManager_Entry(
        const baea_ControlManager::ControlHandler&  callback,
        const bsl::string&                          arguments,
        const bsl::string&                          description,
        bslma_Allocator                            *basicAllocator)
: d_callback(callback, basicAllocator)
, d_arguments(arguments, basicAllocator)
, d_description(description, basicAllocator)
{}

baea_ControlManager_Entry::baea_ControlManager_Entry(
        const baea_ControlManager_Entry&  rhs,
        bslma_Allocator                 *basicAllocator)
: d_callback(rhs.d_callback, basicAllocator)
, d_arguments(rhs.d_arguments, basicAllocator)
, d_description(rhs.d_description, basicAllocator)
{}

// MANIPULATORS

baea_ControlManager_Entry&
baea_ControlManager_Entry::operator=(const baea_ControlManager_Entry& rhs)
{
   if (&rhs != this) {
      d_callback    = rhs.d_callback;
      d_arguments   = rhs.d_arguments;
      d_description = rhs.d_description;
   }
   return *this;
}

}  // close namespace BloombergLP

// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2007, 2008
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ------------------------------ END-OF-FILE ---------------------------------
