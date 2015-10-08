// btls5_networkdescriptionutil.cpp                                   -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <btls5_networkdescriptionutil.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(btls5_networkdescriptionutil_cpp, "$Id$ $CSID$")

namespace BloombergLP {

namespace btls5 {

                       // -----------------------------
                       // struct NetworkDescriptionUtil
                       // -----------------------------

// CLASS METHODS
bool NetworkDescriptionUtil::isWellFormed(
                                       const NetworkDescription& socks5Servers)
{
    const bsl::size_t levels = socks5Servers.numLevels();
    bool hasEmptyLevel = false;
    for (bsl::size_t i = 0; i < levels; ++i) {
        if (0 == socks5Servers.numProxies(i)) {
            hasEmptyLevel = true;
            break;
        }
    }
    return 0 < levels && !hasEmptyLevel;
}

void NetworkDescriptionUtil::setLevelCredentials(
                                              NetworkDescription *proxyNetwork,
                                              bsl::size_t         level,
                                              const Credentials&  credentials)
{
    BSLS_ASSERT(proxyNetwork);
    BSLS_ASSERT(level < proxyNetwork->numLevels());

    bsl::size_t order = 0;
    for (NetworkDescription::ProxyIterator proxy
                                             = proxyNetwork->beginLevel(level),
             end = proxyNetwork->endLevel(level);
         proxy != end;
         ++proxy) {
        proxyNetwork->setCredentials(level, order, credentials);
        ++order;
    }
}

void NetworkDescriptionUtil::setAllCredentials(
                                              NetworkDescription *proxyNetwork,
                                              const Credentials&  credentials)
{
    BSLS_ASSERT(proxyNetwork);

    for (bsl::size_t level = 0, end = proxyNetwork->numLevels();
         level != end;
         ++level) {
        setLevelCredentials(proxyNetwork, level, credentials);
    }
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
