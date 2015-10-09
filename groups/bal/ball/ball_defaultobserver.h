// ball_defaultobserver.h                                             -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#ifndef INCLUDED_BALL_DEFAULTOBSERVER
#define INCLUDED_BALL_DEFAULTOBSERVER

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a default observer that emits log records to 'stdout'.
//
//@CLASSES:
//    ball::DefaultObserver: observer that outputs log records to 'stdout'
//
//@SEE_ALSO: ball_record, ball_context, ball_loggermanager
//
//@DESCRIPTION: This component provides a default concrete implementation of
// the 'ball::Observer' protocol for receiving and processing log records:
//..
//               ( ball::DefaultObserver )
//                           |              ctor
//                           |
//                           V
//                   ( ball::Observer )
//                                          dtor
//                                          publish
//..
// 'ball::DefaultObserver' is a concrete class derived from 'ball::Observer'
// that processes the log records it receives through its 'publish' method by
// printing them to 'stdout'.  Given its minimal functionality,
// 'ball::DefaultObserver' is intended for development use only.
// 'ball::DefaultObserver' is not recommended for use in a production
// environment.
//
///Usage
///-----
// The following code fragments illustrate the essentials of using a default
// observer within a 'ball' logging system.
//
// First create a 'ball::DefaultObserver' named 'defaultObserver':
//..
//     ball::DefaultObserver defaultObserver(&bsl::cout);
//..
// The default observer must then be installed within a 'ball' logging system.
// This is done by passing 'defaultObserver' to the 'ball::LoggerManager'
// constructor, which also requires a 'ball::LoggerManagerConfiguration'
// object.
//..
//     ball::LoggerManagerConfiguration lmc;
//     ball::LoggerManager              loggerManager(&defaultObserver, lmc);
//..
// Henceforth, all messages that are published by the logging system will be
// transmitted to the 'publish' method of 'defaultObserver'.

#ifndef INCLUDED_BALSCM_VERSION
#include <balscm_version.h>
#endif

#ifndef INCLUDED_BALL_OBSERVER
#include <ball_observer.h>
#endif

#ifndef INCLUDED_BSLMT_MUTEX
#include <bslmt_mutex.h>
#endif

#ifndef INCLUDED_BSL_IOSFWD
#include <bsl_iosfwd.h>
#endif

namespace BloombergLP {

namespace ball {

class Context;
class Record;

                           // =====================
                           // class DefaultObserver
                           // =====================

class DefaultObserver : public Observer {
    // This class provides a default implementation of the 'Observer' protocol.
    // The 'publish' method of this class outputs the log records that it
    // receives to an instance of 'bsl::ostream' that is supplied at
    // construction.

    // DATA
    bsl::ostream *d_stream;  // output sink for log records
    bslmt::Mutex  d_mutex;   // serializes concurrent calls to 'publish'

    // NOT IMPLEMENTED
    DefaultObserver(const DefaultObserver&);
    DefaultObserver& operator=(const DefaultObserver&);

  public:
    using Observer::publish;

    // CREATORS
    DefaultObserver(bsl::ostream *stream);
        // Create a default observer that transmits log records to the
        // specified 'stream'.


    virtual ~DefaultObserver();
        // Destroy this default observer.

    // MANIPULATORS
    virtual void publish(const Record&  record,
                         const Context& context);
        // Process the specified log 'record' having the specified publishing
        // 'context'.
        //
        // Print 'record' and 'context' to the 'bsl::ostream' supplied at
        // construction.  The behavior is undefined if 'record' or 'context' is
        // modified during the execution of this method.
};

// ============================================================================
//                              INLINE DEFINITIONS
// ============================================================================

                           // ---------------------
                           // class DefaultObserver
                           // ---------------------

// CREATORS
inline
DefaultObserver::DefaultObserver(bsl::ostream *stream)
: d_stream(stream)
{
}


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
