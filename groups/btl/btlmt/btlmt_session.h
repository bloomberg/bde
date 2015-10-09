// btlmt_session.h                                                    -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#ifndef INCLUDED_BTLMT_SESSION
#define INCLUDED_BTLMT_SESSION

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a *pure* protocol class for a session
//
//@CLASSES:
//  btlmt::Session: protocol for an abstract session.
//
//@SEE_ALSO: btlmt_sessionpool, btlmt_sessionfactory
//
//@DESCRIPTION: This component provides a class, 'btlmt::Session', for
// aggregating application data in one type that addtionally allows starting
// and stopping a network connection.  Concrete implementations of this
// protocol are intended to be used with 'btlmt_sessionpool' and will rarely
// be used stand-alone.
//
///Usage
///-----
// For a usage example, see {'btlmt_sessionpool'}.

#ifndef INCLUDED_BTLSCM_VERSION
#include <btlscm_version.h>
#endif

namespace BloombergLP {

namespace btlmt {

class AsyncChannel;

                            // =============
                            // class Session
                            // =============

class Session {
   // This class defines a *pure* protocol for the starting and stopping of
   // communication sessions.

  public:
    // CREATORS
    virtual ~Session();
        // Destroy this session object.

    // MANIPULATORS
    virtual int start() = 0;
        // Begin the asynchronous operation of the session.  Return 0 on
        // success and a non-zero value otherwise.

    virtual int stop() = 0;
        // Stop the operation of this session.  Return 0 on success and a
        // non-zero value otherwise.

    // ACCESSORS
    virtual AsyncChannel *channel() const = 0;
        // Return the channel associated with the session.
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
