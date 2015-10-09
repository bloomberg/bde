// btlmt_sessionfactory.h                                             -*-C++-*-
#ifndef INCLUDED_BTLMT_SESSIONFACTORY
#define INCLUDED_BTLMT_SESSIONFACTORY

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a *pure* protocol class for creating sessions
//
//@CLASSES:
//  btlmt::SessionFactory: abstract factory for sessions
//
//@SEE_ALSO: btlmt_session, btlmt_sessionpool
//
//@DESCRIPTION: The class provided by this component are intended to be used
// with 'btlmt::SessionPool', which allocates/manages TCP sessions given a
// session factory.  See the 'btlmt_sessionpool' component for a more detailed
// documentation.
//
///Usage
///-----
// For a usage example, see {'btlmt_sessionpool'}.

#ifndef INCLUDED_BTLSCM_VERSION
#include <btlscm_version.h>
#endif

#ifndef INCLUDED_BSL_FUNCTIONAL
#include <bsl_functional.h>
#endif

#ifndef INCLUDED_BSL_MEMORY
#include <bsl_memory.h>
#endif

#ifndef INCLUDED_BSL_MEMORY
#include <bsl_memory.h>
#endif

namespace BloombergLP {

namespace btlmt {

class AsyncChannel;
class Session;

                            // ====================
                            // class SessionFactory
                            // ====================

class SessionFactory {
    // This class defines a *pure* protocol for the allocation and deallocation
    // of 'Session' objects.

  public:
    // PUBLIC TYPES
    typedef bsl::function<void(int, Session*)> Callback;
       // The signature of a function to be called after a new session is
       // allocated.  The first argument is taken to indicate the status of the
       // allocation - 0 if successful, nonzero otherwise.  The second argument
       // is a pointer to the newly allocated session.

    // CREATORS
    virtual ~SessionFactory();
       // Destroy this factory

    // MANIPULATORS
    virtual void allocate(const bsl::shared_ptr<AsyncChannel>& channel,
                          const SessionFactory::Callback&      callback);
       // Asynchronously allocate a 'Session' object for the specified
       // 'channel', and invoke the specified 'callback' with this session.
       //
       // Note that the default implementation of this (non-pure virtual)
       // method is to call 'allocate' with the raw channel pointer.

    virtual void allocate(AsyncChannel                    *channel,
                          const SessionFactory::Callback&  callback) = 0;
       // Asynchronously allocate a 'Session' object for the specified
       // 'channel', and invoke the specified 'callback' with this session.

    virtual void deallocate(Session *session) = 0;
       // Deallocate the specified 'session'.
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
