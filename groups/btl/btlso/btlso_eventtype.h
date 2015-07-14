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
// enumerating socket event types, namely, 'BTESO_ACCEPT', 'BTESO_CONNECT',
// 'BTESO_READ', and 'BTESO_WRITE'.

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
        BTESO_ACCEPT  = 0,
        BTESO_CONNECT = 1,
        BTESO_READ    = 2,
        BTESO_WRITE   = 3
#ifndef BDE_OMIT_INTERNAL_DEPRECATED
      , ACCEPT  = BTESO_ACCEPT
      , CONNECT = BTESO_CONNECT
      , READ    = BTESO_READ
      , WRITE   = BTESO_WRITE
#endif // BDE_OMIT_INTERNAL_DEPRECATED
    };

    enum { BTESO_LENGTH = BTESO_WRITE + 1 };
        // Define 'BTESO_LENGTH' to be the number of consecutively-valued
        // enumerators in 'Type', in the range '[0 .. BTESO_LENGTH - 1]'.
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
