// btemt_session.h   -*-C++-*-
#ifndef INCLUDED_BTEMT_SESSION
#define INCLUDED_BTEMT_SESSION

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")



//@PURPOSE: Provide a *pure* protocol class for creating sessions
//
//@CLASSES:
//        btemt_Session: protocol for an abstract session.
// btemt_SessionFactory: abstract factory for sessions.
//
//@SEE_ALSO: btemt_sessionpool
//
//@AUTHOR: Ilougino Rocha (irocha), Olga Zykova (ozykova)
//
//@DESCRIPTION: The classes provided by this component are intended to be used
// with 'btemt_SessionPool', which allocates/manages TCP sessions given a
// session factory.  See the 'btemt_sessionpool' component for a more detailed
// documentation.


#ifndef INCLUDED_BTESCM_VERSION
#include <btescm_version.h>
#endif


#ifndef INCLUDED_BDEF_FUNCTION
#include <bdef_function.h>
#endif

namespace BloombergLP {

class btemt_AsyncChannel;

                            // ===================
                            // class btemt_Session
                            // ===================

class btemt_Session {
   // This class defines a *pure* protocol for the starting and stopping
   // of communication sessions.

  public:
    // CREATORS
    virtual ~btemt_Session();
        // Destroy this session object.

    // MANIPULATORS
    virtual int start() = 0;
        // Begin the asynchronous operation of the session.
        // Return 0 on success and a non-zero value otherwise.

    virtual int stop() = 0;
        // Stop the operation of this session.
        // Return 0 on success and a non-zero value otherwise.

    // ACCESSORS
    virtual btemt_AsyncChannel* channel() const = 0;
        // Return the channel associated with the session.
};

                            // ==========================
                            // class btemt_SessionFactory
                            // ==========================

class btemt_SessionFactory {
    // This class defines a *pure* protocol for the allocation & deallocation
    // of 'btemt_Session' objects.

  public:
    // PUBLIC TYPES
    typedef bdef_Function<void (*)(int,btemt_Session*)> Callback;
       // The signature of a function to be called after a new session is
       // allocated.  The first argument is taken to indicate the status
       // of the allocation - 0 if successful, nonzero otherwise.  The second
       // argument is a pointer to the newly allocated session.

    // CREATORS
    virtual ~btemt_SessionFactory();
       // Destroy this factory

    // MANIPULATORS
    virtual void allocate(btemt_AsyncChannel                   *channel,
                          const btemt_SessionFactory::Callback& callback) = 0;
       // Asynchronously allocate a 'btemt_Session' object for the specified
       // 'channel', and invoke the specified 'callback' with this session.

    virtual void deallocate(btemt_Session *session) = 0;
       // Deallocate the specified 'session'.

};

} // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2007
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
