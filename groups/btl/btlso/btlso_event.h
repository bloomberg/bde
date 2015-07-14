// btlso_event.h                -*-C++-*-
#ifndef INCLUDED_BTLSO_EVENT
#define INCLUDED_BTLSO_EVENT

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")


//@PURPOSE: Provide component to describe an event on a socket.
//
//@CLASSES:
//      btlso::Event: representation of an event on a socket
//  btlso::EventHash: namespace for a hash function
//
//@AUTHOR: Daniel Glaser (dglaser),  Herve Bronnimann (hbronnim)
//
//@DESCRIPTION: This component provides a straightforward attribute class,
// 'btlso::Event', which represents an event on a particular socket.  The two
// attributes are the socket handle and the event type.  The recognized events
// are 'ACCEPT', 'CONNECT', 'READ', and 'WRITE'.  Accessors and manipulators
// are provided for the socket handle and the event type.  In order to
// facilitate storing 'btlso::Event' objects into 'bsl::unordered_map' or other
// unordered associative containers, a hash functor, 'btlso::EventHash', is also
// provided.

#ifndef INCLUDED_BTLSCM_VERSION
#include <btlscm_version.h>
#endif

#ifndef INCLUDED_BTLSO_SOCKETHANDLE
#include <btlso_sockethandle.h>
#endif

#ifndef INCLUDED_BTLSO_EVENTTYPE
#include <btlso_eventtype.h>
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

namespace btlso {
                             // =================
                             // class Event
                             // =================

class Event {
    // This attribute class represents an event on a socket.  The recognized
    // events are ACCEPT, CONNECT, READ, and WRITE.  Accessors and manipulators
    // are provided for the socket handle and the event type.

    // DATA
    SocketHandle::Handle d_handle;
    EventType::Type      d_type;

    // FRIENDS
    friend bool operator==(const Event& lhs, const Event& rhs);

  public:
    // TRAITS
    BSLALG_DECLARE_NESTED_TRAITS(Event,
                                 bslalg::TypeTraitBitwiseCopyable);

    // CREATORS
    Event(const Event& original);
        // Copy constructor.

    Event(const SocketHandle::Handle& handle,
                const EventType::Type       type);
        // Construct an event of type 'type' on socket 'handle'.

    ~Event();
        // Destroy this event object.

    // MANIPULATORS
    void setHandle(const SocketHandle::Handle& handle);
        // Reset handle to 'handle'.  The type is unchanged.

    void setType(EventType::Type type);
        // Reset type to 'type'.  The socket handle is unchanged.

    // ACCESSORS
    SocketHandle::Handle handle() const;
        // Get the socket handle of this event.

    EventType::Type type() const;
        // Get the type of this event.
};

// FREE OPERATORS
bool operator==(const Event& lhs, const Event& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' events have the same
    // value, and 'false' otherwise.  Two events have the same value if and
    // only if they have the same socket handle and event type field values.

bool operator!=(const Event& lhs, const Event& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' events do not have the
    // same value, and 'false' otherwise.  Two events have the same value if
    // and only if they have the same socket handle and event type field
    // values.

                           // =====================
                           // class EventHash
                           // =====================

struct EventHash {
    // This 'struct' provides a functor that computes a hash value for a
    // 'Event' object, and is suitable for use with 'bsl::unordered_map'
    // or other unordered associative containers.

    // ACCESSORS
    bsl::size_t operator()(const Event& event) const;
        // Return the hash value for the specified 'event'.
};

// ===========================================================================
//                        INLINE FUNCTION DEFINITIONS
// ===========================================================================

                             // -----------------
                             // class Event
                             // -----------------

// CREATORS
inline
Event::Event(const Event& original)
: d_handle(original.d_handle)
, d_type(original.d_type)
{
}

inline
Event::Event(const SocketHandle::Handle& handle,
                         const EventType::Type       type)
: d_handle(handle)
, d_type(type)
{
}

inline
Event::~Event()
{
}

// MANIPULATORS
inline
void Event::setHandle(const SocketHandle::Handle& handle)
{
    d_handle = handle;
}

inline
void Event::setType(const EventType::Type type)
{
    d_type = type;
}

// ACCESSORS
inline
SocketHandle::Handle Event::handle() const
{
    return d_handle;
}

inline
EventType::Type Event::type() const
{
    return d_type;
}
}  // close package namespace

// FREE OPERATORS
inline
bool btlso::operator==(const Event& lhs, const Event& rhs)
{
    return lhs.d_handle == rhs.d_handle && lhs.d_type == rhs.d_type;
}

inline
bool btlso::operator!=(const Event& lhs, const Event& rhs)
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
