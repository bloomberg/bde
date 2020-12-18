// ball_streamobserver.h                                              -*-C++-*-
#ifndef INCLUDED_BALL_STREAMOBSERVER
#define INCLUDED_BALL_STREAMOBSERVER

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide an observer that emits log records to a stream.
//
//@CLASSES:
//  ball::StreamObserver: observer that emits log records to a stream
//
//@SEE_ALSO: ball_record, ball_context, ball_loggermanager
//
//@DESCRIPTION: This component provides a concrete implementation of the
// 'ball::Observer' protocol for receiving and processing log records:
//..
//                 ,--------------------.
//                ( ball::StreamObserver )
//                 `--------------------'
//                           |              ctor
//                           |              setRecordFormatFunctor
//                           V
//                    ,--------------.
//                   ( ball::Observer )
//                    `--------------'
//                                          publish
//                                          releaseRecords
//                                          dtor
//..
// 'ball::StreamObserver' is a concrete class derived from 'ball::Observer'
// that processes the log records it receives through its 'publish' method by
// writing them to an output stream.  Given its minimal functionality,
// 'ball::StreamObserver' should be used with care in a production environment.
// It is not recommended to construct this observer with file-based streams due
// to lack of any file rotation functionality.
//
///Log Record Formatting
///---------------------
// By default, the output format of published log records is:
//..
//  DATE_TIME PID THREAD-ID SEVERITY FILE LINE CATEGORY MESSAGE USER-FIELDS
//..
// where 'DATE' and 'TIME' are of the form 'DDMonYYYY' and 'HH:MM:SS.mmm',
// respectively ('Mon' being the 3-letter abbreviation for the month).  For
// example, assuming that no user-defined fields are present, a log record
// will have the following appearance when the default format is in effect:
//..
//  18MAY2005_18:58:12.076 7959 1 WARN ball_streamobserver2.t.cpp 404 TEST hi!
//..
// The default format can be overridden by supplying a suitable formatting
// functor to 'setRecordFormatFunctor'.  For example, an instance of
// 'ball::RecordStringFormatter' conveniently provides such a functor:
//..
//  streamObserver.setRecordFormatFunctor(
//              ball::RecordStringFormatter("\n%I %p:%t %s %f:%l %c %m %u\n"));
//..
// The above statement will cause subsequent records to be logged in a format
// that is almost identical to the default format except that the timestamp
// attribute will be written in ISO 8601 format.  See
// {'ball_recordstringformatter'} for information on how format specifications
// are defined and interpreted.
//
// Note that the observer emits newline characters at the beginning and at the
// end of a log record by default, so the user needs to add them explicitly to
// the format string to preserve this behavior.
//
// Also note that in the sample message above the timestamp has millisecond
// precision ('18MAY2005_18:58:12.076').  If microsecond precision is desired
// instead, consider using either the '%D' or '%O' format specification
// supported by 'ball_recordstringformatter'.
//
///Thread Safety
///-------------
// All methods of 'ball::StreamObserver' are thread-safe, and can be called
// concurrently by multiple threads.
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

#include <bslma_allocator.h>
#include <bslma_stdallocator.h>
#include <bslma_usesbslmaallocator.h>

#include <bslmf_nestedtraitdeclaration.h>

#include <bslmt_mutex.h>

#include <bsls_assert.h>
#include <bsls_review.h>

#include <bsl_iosfwd.h>
#include <bsl_functional.h>

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

  public:
    // TYPES
    typedef bsl::function<void(bsl::ostream&, const Record&)>
                                                           RecordFormatFunctor;
        // 'RecordFormatFunctor' is an alias for the type of the functor used
        // for formatting log records to a stream.

    typedef bsl::allocator<char> allocator_type;

  private:
    // DATA
    bsl::ostream        *d_stream_p;   // output sink for log records

    bslmt::Mutex         d_mutex;      // serializes concurrent calls to
                                       // 'publish'

    RecordFormatFunctor  d_formatter;  // formatting functor used when writing
                                       // to log file

    // NOT IMPLEMENTED
    StreamObserver(const StreamObserver&);
    StreamObserver& operator=(const StreamObserver&);

    // CLASS METHODS
    static
    void logRecordDefault(bsl::ostream& stream, const Record& record);
        // Write the specified log 'record' to the specified output 'stream'
        // using the default record format of this stream observer.

    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(StreamObserver, bslma::UsesBslmaAllocator);

  public:
    // CREATORS
    explicit
    StreamObserver(bsl::ostream          *stream,
                   const allocator_type&  allocator = allocator_type());
        // Create a stream observer that transmits log records to the specified
        // 'stream'.  Optionally specify an 'allocator' (e.g., the address of a
        // 'bslma::Allocator' object) to supply memory; otherwise, the default
        // allocator is used.  Note that a default record format is in effect
        // for stream logging (see 'setLogFileFunctor').

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

    void setRecordFormatFunctor(const RecordFormatFunctor& formatter);
        // Set the formatting functor used when writing records to the log file
        // of this file observer to the specified 'formatter' functor.  Note
        // that a default format ("\n%d %p %t %s %f %l %c %m %u\n") is in
        // effect until this method is called (see
        // 'ball_recordstringformatter').  Also note that the observer emits
        // newline characters at the beginning and at the end of a log record
        // by default, so the user needs to add them explicitly to the format
        // string to preserve this behavior.
};

// ============================================================================
//                              INLINE DEFINITIONS
// ============================================================================

                           // --------------------
                           // class StreamObserver
                           // --------------------

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
