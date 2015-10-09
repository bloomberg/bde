// btls5_networkdescriptionutil.h                                     -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#ifndef INCLUDED_BTLS5_NETWORKDESCRIPTIONUTIL
#define INCLUDED_BTLS5_NETWORKDESCRIPTIONUTIL

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide operations on 'btls5::NetworkDescription' objects.
//
//@CLASSES:
//  btls5::NetworkDescriptionUtil: namespace for network description operations
//
//@SEE_ALSO: btls5_networkdescription, btls5_networkconnector
//
//@DESCRIPTION: This component provides a namespace,
// 'btls5::NetworkDescriptionUtil', containing utility functions on
// 'btls5::NetworkDescription' objects, including setting credentials on some
// or all proxies in the object, and checking a 'btls5::NetworkDescription'
// object for being well-formed.  A 'btls5::NetworkDescription' object is
// well-formed if it contains at least one level and each level has at least
// one proxy.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Set Credentials and Check for Well-Formedness
/// - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose we would like to configure a 'btls5::NetworkDescription' object,
// first with uniform credentials, then with variable credentials, and finally
// check if the description is well-formed.
//
// First, we define a 'btls5::NetworkDescription' object with two proxies in
// level 0 ("corporate proxies") and two proxies in level 1 ("regional
// proxies"):
//..
//  btls5::NetworkDescription network;
//  network.addProxy(0, btlso::Endpoint("proxy1.bloomberg.com", 1080));
//  network.addProxy(0, btlso::Endpoint("proxy2.bloomberg.com", 1080));
//  network.addProxy(1, btlso::Endpoint("proxy1.ny.bloomberg.com", 1080));
//  network.addProxy(1, btlso::Endpoint("proxy2.ny.bloomberg.com", 1080));
//..
// Then, we set all proxies to use the same username/password credentials for
// authentication:
//..
//  const btls5::Credentials c1("Jane", "Pass1");
//  btls5::NetworkDescriptionUtil::setAllCredentials(&network, c1);
//  assert(network.beginLevel(0)[0].credentials() == c1);
//  assert(network.beginLevel(0)[1].credentials() == c1);
//  assert(network.beginLevel(1)[0].credentials() == c1);
//  assert(network.beginLevel(1)[1].credentials() == c1);
//..
// Now, we change the credentials for only level 1 proxies to another
// username/password:
//..
//  const btls5::Credentials c2("John", "Pass2");
//  btls5::NetworkDescriptionUtil::setLevelCredentials(&network, 1, c2);
//  assert(network.beginLevel(0)[0].credentials() == c1);
//  assert(network.beginLevel(0)[1].credentials() == c1);
//  assert(network.beginLevel(1)[0].credentials() == c2);
//  assert(network.beginLevel(1)[1].credentials() == c2);
//..
// Finally, we verify that the network description is well-formed:
//..
//  assert(btls5::NetworkDescriptionUtil::isWellFormed(network));
//..

#ifndef INCLUDED_BTLSCM_VERSION
#include <btlscm_version.h>
#endif

#ifndef INCLUDED_BTLS5_NETWORKDESCRIPTION
#include <btls5_networkdescription.h>
#endif

#ifndef INCLUDED_BSL_CSTDDEF
#include <bsl_cstddef.h>
#endif

namespace BloombergLP {

namespace btls5 {

                       // =============================
                       // struct NetworkDescriptionUtil
                       // =============================

struct NetworkDescriptionUtil {
    // This 'struct' provides a namespace for utility functions that operate on
    // 'NetworkDescription' objects.

    // CLASS METHODS
    static bool isWellFormed(const NetworkDescription& socks5Servers);
        // Return 'true' if the specified 'socks5Servers' has at least one
        // level, and each level has at least one proxy; and 'false' otherwise.

    static void setLevelCredentials(NetworkDescription *proxyNetwork,
                                    bsl::size_t         level,
                                    const Credentials&  credentials);
        // Assign the specified 'credentials' for authenticating with every
        // proxy in the specified 'level' of the specified 'proxyNetwork'.  The
        // behavior is undefined unless 'level < proxyNetwork->numLevels()'.

    static void setAllCredentials(NetworkDescription *proxyNetwork,
                                  const Credentials&  credentials);
        // Assign the specified 'credentials' for authenticating with every
        // proxy in the specified 'proxyNetwork'.
};

}  // close package namespace

}  // close enterprise namespace

#endif

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
