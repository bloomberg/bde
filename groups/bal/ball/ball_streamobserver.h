// ball_streamobserver.h                                              -*-C++-*-
#ifndef INCLUDED_BALL_STREAMOBSERVER
#define INCLUDED_BALL_STREAMOBSERVER

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide an observer that emits log records to a stream.
//
//@CLASSES:
//    ball::StreamObserver: observer that emits log records to a stream
//
//@SEE_ALSO: ball_record, ball_context, ball_loggermanager
//
//@AUTHOR: Oleg Subbotin
//
//@DESCRIPTION: This component provides a concrete implementation of the
// 'ball::Observer' protocol for receiving and processing log records:
//..
//                 ,--------------------.
//                ( ball::StreamObserver )
//                 `--------------------'
//                           |              ctor
//                           |              publish
//                           V
//                    ,--------------.
//                   ( ball::Observer )
//                    `--------------'
//                                          dtor
//..
// 'ball::StreamObserver' is a concrete class derived from 'ball::Observer'
// that processes the log records it receives through its 'publish' method by
// writing them to an output stream.  Given its minimal functionality,
// 'ball::StreamObserver' should be used with care in a production environment.
// It is not recommended to construct this observer with file-based streams due
// to luck of any file rotation functionality.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Publication Through Logger Manager
///- - - - - - - - - - - - - - - - - - - - - - -
// This example demonstrates using a 'ball::StreamObserver' within a 'ball'
// logging system.
//
// First, we initialize 'ball' logging subsystem with the default
// configuration:
//..
//  ball::LoggerManagerConfiguration configuration;
//  ball::LoggerManagerScopedGuard   guard(configuration);
//
//  ball::LoggerManager& manager = ball::LoggerManager::singleton();
//..
// Note that the application is now prepared to log messages using the 'ball'
// logging subsystem, but until the application registers an observer, all log
// messages will be discarded.
//
// Then, we create a shared pointer to a 'ball::StreamObserver' object,
// 'observerPtr', passing a pointer to the 'bsl::cout'.
//..
//  bslma::Allocator *alloc =  bslma::Default::globalAllocator(0);
//  bsl::shared_ptr<ball::Observer> observerPtr(
//                                new(*alloc) ball::StreamObserver(&bsl::cout),
//                                alloc);
//..
// Then, we register the stream observer with the logger manager.  Upon
// successful registration, the observer will start to receive log records via
// 'publish' method:
//..
//  int rc = manager.registerObserver(observerPtr, "observer");
//  assert(0 == rc);
//..
// Next, we set the log category and log few messages with different logging
// severity:
//..
//  BALL_LOG_SET_CATEGORY("ball::StreamObserver");
//
//  BALL_LOG_INFO << "Info Log Message."    << BALL_LOG_END;
//  BALL_LOG_WARN << "Warning Log Message." << BALL_LOG_END;
//..

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

                           // ====================
                           // class StreamObserver
                           // ====================

class StreamObserver : public Observer {
    // This class provides a concrete implementation of the 'Observer'
    // protocol.  The 'publish' method of this class outputs the log records
    // that it receives to an instance of 'bsl::ostream' supplied at
    // construction.

    // DATA
    bsl::ostream *d_stream_p;  // output sink for log records
    bslmt::Mutex  d_mutex;     // serializes concurrent calls to 'publish'

    // NOT IMPLEMENTED
    StreamObserver(const StreamObserver&);
    StreamObserver& operator=(const StreamObserver&);

  public:
    // CREATORS
    explicit StreamObserver(bsl::ostream *stream);
        // Create a stream observer that transmits log records to the specified
        // 'stream'.

    virtual ~StreamObserver();
        // Destroy this stream observer.

    // MANIPULATORS
    using Observer::publish;

    virtual void publish(const bsl::shared_ptr<const Record>& record,
                         const Context&                       context);
        // Process the specified log 'record' having the specified publishing
        // 'context'.  Print 'record' and 'context' to the 'bsl::ostream'
        // supplied at construction.  The behavior is undefined if 'record' or
        // 'context' is modified during the execution of this method.
};

// ============================================================================
//                              INLINE DEFINITIONS
// ============================================================================

                           // --------------------
                           // class StreamObserver
                           // --------------------

// CREATORS
inline
StreamObserver::StreamObserver(bsl::ostream *stream)
: d_stream_p(stream)
{
}

}  // close package namespace
}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// Copyright 2017 Bloomberg Finance L.P.
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
