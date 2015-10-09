// btlso_resolveutil.h                                                -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#ifndef INCLUDED_BTLSO_RESOLVEUTIL
#define INCLUDED_BTLSO_RESOLVEUTIL

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide operations to resolve an IPv4 address given a name.
//
//@CLASSES:
//  btlso::ResolveUtil: namespace for platform-independent resolution utilities
//
//@SEE_ALSO: btlso_socketoptutil btlso_socketimputil
//
//@DESCRIPTION: This component provides a set of thread-safe resolution
// functions (based on DNS and local databases) to map a name to an IPv4
// address and a service name to the port number.  This component operates
// using a dynamically replaceable resolution mechanism.  For applications that
// choose to define their own mechanism for resolving addresses by name, this
// component provides the ability to install a custom resolution callback.
// Note that if an application provides its own mechanism to resolve by name,
// this mechanism will be used by all calls to 'getAddress' or 'getAddresses'.
// Otherwise, the default implementation will be used.  An application can
// always use the default implementation by calling the 'getAddressDefault' and
// 'getAddressDefault' methods explicitly.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Resolving an IP Address
/// - - - - - - - - - - - - - - - - -
// The following snippets of code illustrate how to get an IPv4address or a
// port number from a name.  First we create two 'btlso::IPv4Address' objects
// to store the return value for host 'p111', 'ibm1' respectively, and verify
// the return addresses:
//..
//  int                retCode = 0, errorCode = 0;
//  btlso::IPv4Address result1, result2;
//
//  const char *hostname = "p111";
//  retCode = btlso::ResolveUtil::getAddress(&result1,
//                                           hostname,
//                                           &errorCode);
//  assert(0 == retCode);
//  assert(0 == errorCode);
//  bsl::cout << "IPv4 address for '" << hostname << "': " << result1
//            << bsl::endl;
//
//  const char *hostname2 = "ibm1";
//  retCode = btlso::ResolveUtil::getAddress(&result2,
//                                           hostname2,
//                                           &errorCode);
//  assert(0 == retCode);
//  assert(0 == errorCode);
//  bsl::cout << "IPv4 address for '" << hostname2 << "': " << result2
//            << bsl::endl;
//..
// Next we try retrieving all IPv4 addresses for the given host name 'n024' and
// verify the return addresses:
//..
//  bsl::vector<btlso::IPv4Address> addr_array;
//  btlso::IPv4Address              result3;
//
//  const char *hostname3 = "n024";
//  retCode = btlso::ResolveUtil::getAddresses(&addr_array,
//                                             hostname3,
//                                             &errorCode);
//  assert(0 == retCode);
//  assert(0 == errorCode);
//  bsl::cout << "IPv4 address for '" << hostname3 << "': " << addr_array
//            << bsl::endl;
//..
// Finally, we try to get the service port number of the given service names
// 'ftp', 'systat', 'telnet' respectively, and verify the result:
//..
//  const char *servname = "ftp";
//  retCode = btlso::ResolveUtil::getServicePort(&result1,
//                                               servname,
//                                               0,
//                                               &errorCode);
//  assert(0 == retCode);
//  bsl::cout << "Port number for 'ftp': " << result1 << bsl::endl;
//
//  const char *servname2 = "systat";
//  retCode = btlso::ResolveUtil::getServicePort(&result2,
//                                               servname2,
//                                               0,
//                                               &errorCode);
//  assert(0 == retCode);
//  bsl::cout << "Port number for 'systat': " << result1 << bsl::endl;
//
//  const char *servname3 = "telnet";
//  retCode = btlso::ResolveUtil::getServicePort(&result3,
//                                               servname3,
//                                               0,
//                                               &errorCode);
//  assert(0 == retCode);
//  bsl::cout << "Port number for 'telnet': " << result1 << bsl::endl;
//..

#ifndef INCLUDED_BTLSCM_VERSION
#include <btlscm_version.h>
#endif

#ifndef INCLUDED_BSL_STRING
#include <bsl_string.h>
#endif

#ifndef INCLUDED_BSL_VECTOR
#include <bsl_vector.h>
#endif

namespace BloombergLP {

namespace btlso {

class IPv4Address;

                        // ==================
                        // struct ResolveUtil
                        // ==================

struct ResolveUtil {
    // This class provides a namespace for platform-independent thread-safe
    // procedures to get the IPv4address and/or port number based on the names.
    // These procedures take an 'errorCode' as an optional parameter, which is
    // loaded with platform-specific error number if an error occurs during the
    // operation.  All functions return 0 on success or a negative value on
    // error.  By default, they use the platform networking libraries for
    // resolving addresses, but a user can substitute her own resolution
    // mechanism by installing her own 'ResolveByNameCallback' callback
    // globally.  Although invoking the callback itself is thread-safe, the
    // behavior is undefined if an application attempts to set up a callback
    // mechanism via 'setResolveByNameCallback' in a multi-threaded environment
    // while another thread executes 'getAddress' or 'getAddresses'.

  public:
    // TYPES
    typedef int (*ResolveByNameCallback)(
                                       bsl::vector<IPv4Address> *hostAddresses,
                                       const char               *hostName,
                                       int                       numAddresses,
                                       int                      *errorCode);
        // Invoked by 'getAddress' and 'getAddresses', 'ResolveByNameCallback'
        // is an alias for a pointer to a re-entrant function that returns
        // 'int' and takes as arguments a vector of 'hostAddresses', the
        // 'hostName', a 'numAddresses' parameter, and an 'errorCode' pointer.
        // Upon invocation, this function resolves the IP address(es) of the
        // specified 'hostname' and loads the specified 'hostAddresses' with up
        // to the specified 'numAddresses' of the resolved IPv4 addresses if
        // resolution succeeds.  It returns 0 with no effect on 'errorCode'
        // upon success, and otherwise returns a negative value.  Upon failure
        // and if 'errorCode' is not 0, it also loads a native error code into
        // 'errorCode'.  The behavior is undefined unless '0 < numAddresses'.
        // Note that, in any case, any 'hostAddresses' entry present upon
        // return must contain a valid IPv4Address corresponding to the
        // 'hostName', and 'hostAddresses' must be resized accordingly.

    // CLASS METHODS
    static int getAddress(IPv4Address *result,
                          const char  *hostName,
                          int         *errorCode = 0);
    static int getAddressDefault(IPv4Address *result,
                                 const char  *hostName,
                                 int         *errorCode = 0);
        // Load into the specified 'result' the primary IPv4 address of the
        // specified 'hostName'.  Return 0, with no effect on 'errorCode', on
        // success, and return a negative value otherwise.  If an error occurs,
        // the optionally specified 'errorCode' is set to the native error code
        // of the operation and 'result' is unchanged.  Note that 'getAddress'
        // uses the user-installed resolution mechanism, while
        // 'getAddressDefault' uses the default resolution mechanism.

    static int getAddresses(bsl::vector<IPv4Address> *result,
                            const char               *hostName,
                            int                      *errorCode = 0);
        // Load into the specified array 'result' all IPv4 addresses of the
        // specified 'hostName'.  Return 0, with no effect on 'errorCode', on
        // success, and return a negative value otherwise.  If an error occurs,
        // the optionally specified 'errorCode' is set to the native error code
        // of the operation.  In any case, any addresses present in 'result'
        // upon return will contain a valid IPv4Address corresponding to the
        // 'hostName', and 'hostAddress' will be resized accordingly.  Note
        // that 'getAddresses' uses the user-installed resolution mechanism; to
        // use the default resolution mechanism, simply call:
        //..
        //  (*defaultResolveByNameCallback())(result,
        //                                    hostName,
        //                                    INT_MAX,
        //                                    errorCode);
        //..

    static int getHostnameByAddress(bsl::string        *canonicalHostname,
                                    const IPv4Address&  address,
                                    int                *errorCode = 0);
        // Load into the specified 'canonicalHostname' string, the name
        // associated with the specified 'address'.  Return 0, with no effect
        // on 'errorCode', on success, or a negative value otherwise.  If an
        // error occurs, the optionally specified 'errorCode' is set to the
        // native error code of the operation and 'canonicalHostname' is
        // unchanged.

    static int getServicePort(IPv4Address *result,
                              const char  *serviceName,
                              const char  *protocol = 0,
                              int         *errorCode = 0);
        // Load into the specified 'result' the service port number associated
        // with the specified 'serviceName'.  Optionally specify a 'protocol'
        // string, such as "tcp" or "udp"; if 'protocol is 0, an entry with any
        // protocol may be returned.  Return 0, with no effect on 'errorCode',
        // on success, and a negative value otherwise.  If an error occurs,
        // load into the optionally specified 'errorCode' the native error code
        // of the operation only on Windows platform.  Note that the IP address
        // of 'result' is not modified.  The behavior is undefined if 'result'
        // is 0.

    static int getLocalHostname(bsl::string *result);
        // Load into the specified 'result' the name of the local machine as
        // configured by the system administrator.  Return 0 on success, and
        // non-zero (with no effect on 'result') otherwise.  Note that on
        // Windows 'SocketImpUtil::startup()' must have been called to
        // initialize the socket layer before calling this method.

    static ResolveByNameCallback setResolveByNameCallback(
                                               ResolveByNameCallback callback);
        // Install the user-specified custom 'callback' function to resolve an
        // IP address by name.  Return the address of the previously installed
        // callback.  The behavior of 'getAddress' and 'getAddresses' will be
        // corrupted unless 'callback' conforms to the contract laid out in the
        // 'ResolveByNameCallback' type documentation.

    static ResolveByNameCallback currentResolveByNameCallback();
        // Return the currently installed 'ResolveByNameCallback' function.

    static ResolveByNameCallback defaultResolveByNameCallback();
        // Return the default implementation of 'ResolveByNameCallback'
        // function.
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
