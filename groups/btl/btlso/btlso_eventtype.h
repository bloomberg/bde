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

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2002
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
