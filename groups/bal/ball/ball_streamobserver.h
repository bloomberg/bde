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
// `ball::Observer` protocol for receiving and processing log records:
// ```
//                ,--------------------.
//               ( ball::StreamObserver )
//                `--------------------'
//                          |              ctor
//                          |              disablePublishInLocalTime
//                          |              enablePublishInLocalTime
//                          |              setFormat
//                          |              setRecordFormatFunctor
//                          |              getFormat
//                          |              isPublishInLocalTimeEnabled
//                          V
//                   ,--------------.
//                  ( ball::Observer )
//                   `--------------'
//                                         publish
//                                         releaseRecords
//                                         dtor
// ```
// `ball::StreamObserver` is a concrete class derived from `ball::Observer`
// that processes the log records it receives through its `publish` method by
// writing them to an output stream.  Given its minimal functionality,
// `ball::StreamObserver` should be used with care in a production environment.
// It is not recommended to construct this observer with file-based streams due
// to lack of any file rotation functionality.
//
///Log Record Formatting
///---------------------
// By default, the output format of published log records is:
// ```
// DATE_TIME PID:THREAD-ID SEVERITY FILE:LINE CATEGORY MESSAGE USER-FIELDS
// ```
// where `DATE` and `TIME` are of the form `DDMonYYYY` and `HH:MM:SS.mmm`,
// respectively (`Mon` being the 3-letter abbreviation for the month).  For
// example, a log record will have the following appearance when the default
// format is in effect (assuming that no user-defined fields are present):
// ```
// 18MAY2005_18:58:12.076 7959:1 WARN ball_streamobserver.t.cpp:404 TEST hello!
// ```
// For additional flexibility, the `setFormat` method can be called to
// configure the format of published records to the stream.  The format
// specifications can be either scheme-tagged (recommended) or legacy
// `printf`-style format strings that results in a `RecordStringFormatter`
// being used.
//
///Scheme-Based Format Specifications (Recommended)
/// - - - - - - - - - - - - - - - - - - - - - - - -
// The recommended way to specify log record formats is using URI-like
// scheme-tagged format configuration strings.  A scheme-tagged format string
// begins with a scheme identifier followed by `://` and then a
// scheme-specific format specification:
// ```
// <scheme>://<format-specification>
// ```
// The scheme determines which formatter will be used and the syntax of the
// format specification.  The following schemes are currently supported: text,
// json, qjson.  See [Scheme-Based Formatters](ball#Scheme-Based Formatters)
// for more details of the supported schemes and their accompanying format
// specification syntaxes.
//
// For example, to log records to a file in JSON format with printf-style
// format specification:
// ```
// asyncFileObserver.setFormat("qjson://%d %p:%t %s %f:%l %c %m");
// ```
//
///Legacy Format Specifications
/// - - - - - - - - - - - - - - -
// For backward compatibility, format specifications that do not begin with a
// scheme tag are treated as legacy `printf`-style format strings.  Such
// specifications are implicitly treated as if they had a `text://` prefix
// and use `ball::RecordStringFormatter`.  For example, the following two
// calls are equivalent:
// ```
// streamObserver.setFormat("%d %p:%t %s %f:%l %c %m %a\n");
// streamObserver.setFormat("text://%d %p:%t %s %f:%l %c %m %a\n");
// ```
// These `%`-prefixed conversion specifications are defined in
// {`ball_recordstringformatter`}.
//
///Thread Safety
///-------------
// All methods of `ball::StreamObserver` are thread-safe, and can be called
// concurrently by multiple threads.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Basic Usage
/// - - - - - - - - - - -
// The following snippets of code illustrate the basic usage of
// `ball::StreamObserver`.
//
// First create a `ball::Record` object `record` and a `ball::Context` object
// `context`.  Note that the default values for these objects (or their
// contained objects) are perfectly suitable for logging purposes.
// ```
// ball::RecordAttributes attributes;
// ball::UserFields       fieldValues;
// ball::Context          context;
//
// bslma::Allocator *ga = bslma::Default::globalAllocator(0);
// const bsl::shared_ptr<const ball::Record>
//            record(new (*ga) ball::Record(attributes, fieldValues, ga), ga);
// ```
// Next, create a stream observer `observer` with the `bsl::cout` as the output
// stream.
// ```
// ball::StreamObserver observer(&bsl::cout);
// ```
// Finally, publish `record` and `context` to `observer`.
// ```
// observer.publish(record, context);
// ```
// This will produce the following output on `stdout`:
// ```
// 01JAN0001_24:00:00.000 0 0 OFF  0
// ```

#include <balscm_version.h>

#include <ball_observerformatterimp.h>
#include <ball_observer.h>
#include <ball_recordformatterfunctor.h>
#include <ball_recordformattertimezone.h>

#include <bsla_deprecated.h>

#include <bslma_allocator.h>
#include <bslma_bslallocator.h>

#include <bslmt_mutex.h>

#include <bsls_assert.h>
#include <bsls_keyword.h>
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

/// This class provides a concrete implementation of the `Observer`
/// protocol.  The `publish` method of this class outputs the log records
/// that it receives to an instance of `bsl::ostream` supplied at
/// construction.
class StreamObserver : public Observer {
  public:
    // TYPES

    /// `LogRecordFunctor` is an alias for the type of the functor used for
    /// formatting log records to a stream.
    typedef RecordFormatterFunctor::Type RecordFormatter;

    typedef bsl::allocator<char> allocator_type;

  private:
    // PRIVATE TYPES
    typedef RecordFormatterTimezone::Enum TimezoneEnum;

  private:
    // DATA
    bsl::ostream             *d_stream_p;           // output sink for log
                                                    // records

    mutable bslmt::Mutex      d_mutex;              // serializes concurrent
                                                    // calls to 'publish'

    ObserverFormatterImp      d_observerFormatterImp;
                                                    // formatter manager that
                                                    // handles all formatting
                                                    // operations

    // NOT IMPLEMENTED
    StreamObserver(const StreamObserver&);
    StreamObserver& operator=(const StreamObserver&);

  public:
    // CREATORS

    /// Create a stream observer that transmits log records to the specified
    /// `stream`.  Optionally specify an `allocator` (e.g., the address of a
    /// `bslma::Allocator` object) to supply memory; otherwise, the default
    /// allocator is used.  Note that a default record format is in effect
    /// for stream logging (see `setLogFileFunctor`).
    explicit
    StreamObserver(bsl::ostream          *stream,
                   const allocator_type&  allocator = allocator_type());

    /// Destroy this stream observer.
    ~StreamObserver() BSLS_KEYWORD_OVERRIDE;

    // MANIPULATORS

    using Observer::publish;  // Picks up the deprecated `publish` overload.

    /// Process the specified log `record` having the specified publishing
    /// `context`.  Print `record` and `context` to the `bsl::ostream`
    /// supplied at construction.  The behavior is undefined if `record` or
    /// `context` is modified during the execution of this method.
    void publish(const bsl::shared_ptr<const Record>& record,
                 const Context&                       context)
                                                         BSLS_KEYWORD_OVERRIDE;

    /// Discard any shared reference to a `Record` object that was supplied
    /// to the `publish` method, and is held by this observer.  Note that
    /// this operation should be called if resources underlying the
    /// previously provided shared-pointers must be released.  This method
    /// intentionally does nothing as such resources are held, we publish all
    /// records immediately.
    void releaseRecords() BSLS_KEYWORD_OVERRIDE;

    /// Disable publishing of the timestamp attribute of records in local
    /// time by this stream observer; henceforth, timestamps will be in UTC
    /// time.  This method has no effect if publishing in local time is not
    /// enabled.
    void disablePublishInLocalTime();

    /// Enable publishing of the timestamp attribute of records in local
    /// time by this stream observer.  By default, timestamps are published
    /// in UTC time.  Note that this method also affects timestamps for
    /// formatters that use them.
    void enablePublishInLocalTime();

    /// Set the formatting functor used when writing records to the stream of
    /// this stream observer to the specified `formatter` functor.  Note that
    /// a default format ("\n%d %p %t %s %f %l %c %m %u\n") is in effect until
    /// this method or `setFormat` is called (see
    /// `ball_recordstringformatter`).  Also note that the observer emits
    /// newline characters at the beginning and at the end of a log record
    /// by default, so the user needs to add them explicitly to the format
    /// string to preserve this behavior.  Note that this method is not
    /// able to communicate the timezone default settings to the
    /// `formatter`, prefer `setFormat`.
    void setRecordFormatFunctor(const RecordFormatter& formatter);

    /// Set the formatting functor used when writing records to the stream of
    /// this stream observer to a log file functor created according to the
    /// specified, possibly URI-like scheme tagged, `format`.  Return zero if
    /// the setup with the specified arguments was successful and also save
    /// the `format` to be later retrievable using `getFormat`.  Otherwise (if
    /// no matching scheme could be found or the configuration is invalid)
    /// return a non-zero value and do not change the log record formatter
    /// used by this object.  Note that a default format
    /// ("\n%d %p %t %s %f %l %c %m %u\n") is in effect until this method or
    /// `setRecordFormatFunctor` is called.  Also, notice that the observer
    /// emits newline characters at the beginning and at the end of a log
    /// record by (the) default (format), so the user needs to add them
    /// explicitly to (text) format strings to preserve that behavior.
    int setFormat(const bsl::string_view& format);

    // ACCESSORS

    /// Return `true` if this observer writes the timestamp attribute of
    /// records that it publishes in local time by default, and `false`
    /// otherwise (in which case timestamps are written by default in UTC
    ///  time).
    bool isPublishInLocalTimeEnabled() const;

    /// Return the format config of the last successful `setFormat` call.
    const bsl::string& getFormat() const;
};

// ============================================================================
//                             INLINE DEFINITIONS
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
