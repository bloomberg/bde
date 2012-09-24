// bteso_eventtype.h      -*-C++-*-
#ifndef INCLUDED_BTESO_EVENTTYPE
#define INCLUDED_BTESO_EVENTTYPE

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Enumerate all socket event types.
//
//@CLASSES:
// bteso_EventType: namespace for enumerating all socket event types
//
//@AUTHOR: Daniel Glaser (dglaser)
//
//@DESCRIPTION: This component provides a namespace, 'bteso_EventType',
//for enumerating all socket event types, namely ACCEPT, CONNECT, READ, and
//WRITE.

#ifndef INCLUDED_BTESCM_VERSION
#include <btescm_version.h>
#endif

                    // ===========================
                    // class bteso_SocketEventType
                    // ===========================

namespace BloombergLP {

struct bteso_EventType {
    // This class provides a namespace for enumerating all socket event types.

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
        // Define LENGTH to be the number of consecutively valued enumerators
        // in 'Type', in the range [ 0 .. LENGTH - 1 ].
};

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
