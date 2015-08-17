// btlso_eventtype.h                                                  -*-C++-*-
#ifndef INCLUDED_BTLSO_EVENTTYPE
#define INCLUDED_BTLSO_EVENTTYPE

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide an enumeration of socket event types.
//
//@CLASSES:
// btlso::EventType: namespace for enumerating socket event types
//
//@AUTHOR: Daniel Glaser (dglaser)
//
//@DESCRIPTION: This component provides a namespace, 'btlso::EventType', for
// enumerating socket event types, namely, 'e_ACCEPT', 'e_CONNECT',
// 'e_READ', and 'e_WRITE'.

#ifndef INCLUDED_BTLSCM_VERSION
#include <btlscm_version.h>
#endif

                    // ===========================
                    // class bteso::SocketEventType
                    // ===========================

namespace BloombergLP {

namespace btlso {
struct EventType {
    // This class provides a namespace for enumerating socket event types.

    // TYPES
    enum Type {
        e_ACCEPT  = 0
      , e_CONNECT = 1
      , e_READ    = 2
      , e_WRITE   = 3
#ifndef BDE_OMIT_INTERNAL_DEPRECATED
      , BTESO_ACCEPT  = e_ACCEPT
      , BTESO_CONNECT = e_CONNECT
      , BTESO_READ    = e_READ
      , BTESO_WRITE   = e_WRITE
      , ACCEPT  = e_ACCEPT
      , CONNECT = e_CONNECT
      , READ    = e_READ
      , WRITE   = e_WRITE
#endif // BDE_OMIT_INTERNAL_DEPRECATED
    };

    enum {
        k_LENGTH = e_WRITE + 1  // Define 'k_LENGTH' to be the number of
                                // consecutively-valued enumerators in 'Type',
                                // in the range '[0 .. k_LENGTH - 1]'.
#ifndef BDE_OMIT_INTERNAL_DEPRECATED
      , BTESO_LENGTH = k_LENGTH
#endif // BDE_OMIT_INTERNAL_DEPRECATED
    };
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
