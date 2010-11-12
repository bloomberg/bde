// bteso_event.h                -*-C++-*-
#ifndef INCLUDED_BTESO_EVENT
#define INCLUDED_BTESO_EVENT

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")


//@PURPOSE: Provide component to describe an event on a socket.
//
//@CLASSES:
//      bteso_Event: representation of an event on a socket
//  bteso_EventHash: namespace for a hash function
//
//@AUTHOR: Daniel Glaser (dglaser),  Herve Bronnimann (hbronnim)
//
//@DESCRIPTION: This component provides a straightforward attribute class,
// 'bteso_Event', which represents an event on a particular socket.  The two
// attributes are the socket handle and the event type.  The recognized events
// are 'ACCEPT', 'CONNECT', 'READ', and 'WRITE'.  Accessors and manipulators
// are provided for the socket handle and the event type.  In order to
// facilitate storing 'bteso_Event' objects into 'bsl::hash_map' or other
// unordered associative containers, a hash functor, 'bteso_EventHash', is also
// provided.

#ifndef INCLUDED_BTESCM_VERSION
#include <btescm_version.h>
#endif

#ifndef INCLUDED_BTESO_SOCKETHANDLE
#include <bteso_sockethandle.h>
#endif

#ifndef INCLUDED_BTESO_EVENTTYPE
#include <bteso_eventtype.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITS
#include <bslalg_typetraits.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITBITWISECOPYABLE
#include <bslalg_typetraitbitwisecopyable.h>
#endif

#ifndef INCLUDED_BSL_CSTDDEF
#include <bsl_cstddef.h>
#endif

namespace BloombergLP {

                             // =================
                             // class bteso_Event
                             // =================

class bteso_Event {
    // This attribute class represents an event on a socket.  The recognized
    // events are ACCEPT, CONNECT, READ, and WRITE.  Accessors and manipulators
    // are provided for the socket handle and the event type.

    // DATA
    bteso_SocketHandle::Handle d_handle;
    bteso_EventType::Type      d_type;

    // FRIENDS
    friend bool operator==(const bteso_Event& lhs, const bteso_Event& rhs);

  public:
    // TRAITS
    BSLALG_DECLARE_NESTED_TRAITS(bteso_Event,
                                 bslalg_TypeTraitBitwiseCopyable);

    // CREATORS
    bteso_Event(const bteso_Event& original);
        // Copy constructor.

    bteso_Event(const bteso_SocketHandle::Handle& handle,
                const bteso_EventType::Type       type);
        // Construct an event of type 'type' on socket 'handle'.

    ~bteso_Event();
        // Destroy this event object.

    // MANIPULATORS
    void setHandle(const bteso_SocketHandle::Handle& handle);
        // Reset handle to 'handle'.  The type is unchanged.

    void setType(bteso_EventType::Type type);
        // Reset type to 'type'.  The socket handle is unchanged.

    // ACCESSORS
    bteso_SocketHandle::Handle handle() const;
        // Get the socket handle of this event.

    bteso_EventType::Type type() const;
        // Get the type of this event.
};

// FREE OPERATORS
bool operator==(const bteso_Event& lhs, const bteso_Event& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' events have the same
    // value, and 'false' otherwise.  Two events have the same value if and
    // only if they have the same socket handle and event type field values.

bool operator!=(const bteso_Event& lhs, const bteso_Event& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' events do not have the
    // same value, and 'false' otherwise.  Two events have the same value if
    // and only if they have the same socket handle and event type field
    // values.

                           // =====================
                           // class bteso_EventHash
                           // =====================

struct bteso_EventHash {
    // This 'struct' provides a functor that computes a hash value for a
    // 'bteso_Event' object, and is suitable for use with 'bsl::hash_map' or
    // other unordered associative containers.

    // ACCESSORS
    bsl::size_t operator()(const bteso_Event& event) const;
        // Return the hash value for the specified 'event'.
};

// ===========================================================================
//                        INLINE FUNCTION DEFINITIONS
// ===========================================================================

                             // -----------------
                             // class bteso_Event
                             // -----------------

// CREATORS
inline
bteso_Event::bteso_Event(const bteso_Event& original)
: d_handle(original.d_handle)
, d_type(original.d_type)
{
}

inline
bteso_Event::bteso_Event(const bteso_SocketHandle::Handle& handle,
                         const bteso_EventType::Type       type)
: d_handle(handle)
, d_type(type)
{
}

inline
bteso_Event::~bteso_Event()
{
}

// MANIPULATORS
inline
void bteso_Event::setHandle(const bteso_SocketHandle::Handle& handle)
{
    d_handle = handle;
}

inline
void bteso_Event::setType(const bteso_EventType::Type type)
{
    d_type = type;
}

// ACCESSORS
inline
bteso_SocketHandle::Handle bteso_Event::handle() const
{
    return d_handle;
}

inline
bteso_EventType::Type bteso_Event::type() const
{
    return d_type;
}

// FREE OPERATORS
inline
bool operator==(const bteso_Event& lhs, const bteso_Event& rhs)
{
    return lhs.d_handle == rhs.d_handle && lhs.d_type == rhs.d_type;
}

inline
bool operator!=(const bteso_Event& lhs, const bteso_Event& rhs)
{
    return !(lhs == rhs);
}

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2008
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
