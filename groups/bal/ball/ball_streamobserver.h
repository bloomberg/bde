// ball_streamobserver.h                                              -*-C++-*-
#ifndef INCLUDED_BALL_STREAMOBSERVER
#define INCLUDED_BALL_STREAMOBSERVER

#include <bsls_ident.h>
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
//                           V
//                    ,--------------.
//                   ( ball::Observer )
//                    `--------------'
//                                          publish
//                                          dtor
//..
// 'ball::StreamObserver' is a concrete class derived from 'ball::Observer'
// that processes the log records it receives through its 'publish' method by
// writing them to an output stream.  Given its minimal functionality,
// 'ball::StreamObserver' should be used with care in a production environment.
// It is not recommended to construct this observer with file-based streams due
// to lack of any file rotation functionality.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Basic Usage
/// - - - - - - - - - - -
// The following snippets of code illustrate the basic usage of
// 'ball::StreamObserver'.
//
// First create a 'ball::Record' object 'record' and a 'ball::Context' object
// 'context'.  Note that the default values for these objects (or their
// contained objects) are perfectly suitable for logging purposes.
//..
//  ball::RecordAttributes attributes;
//  ball::UserFields       fieldValues;
//  ball::Context          context;
//
//  bslma::Allocator *ga = bslma::Default::globalAllocator(0);
//  const bsl::shared_ptr<const ball::Record>
//             record(new (*ga) ball::Record(attributes, fieldValues, ga), ga);
//..
// Next, create a stream observer 'observer' with the 'bsl::cout' as the output
// stream.
//..
//  ball::StreamObserver observer(&bsl::cout);
//..
// Finally, publish 'record' and 'context' to 'observer'.
//..
//  observer.publish(record, context);
//..
// This will produce the following output on 'stdout':
//..
//  01JAN0001_24:00:00.000 0 0 OFF  0
//..

#include <balscm_version.h>

#include <ball_observer.h>

#include <bslmt_mutex.h>

#include <bsls_assert.h>
#include <bsls_review.h>

#include <bsl_iosfwd.h>

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

    virtual void releaseRecords();
        // Discard any shared reference to a 'Record' object that was supplied
        // to the 'publish' method, and is held by this observer.  Note that
        // this operation should be called if resources underlying the
        // previously provided shared-pointers must be released.
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
    BSLS_REVIEW(d_stream_p);
}

// MANIPULATORS
inline
void StreamObserver::releaseRecords()
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
