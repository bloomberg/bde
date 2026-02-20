// ball_cstdioobserver.h                                              -*-C++-*-
#ifndef INCLUDED_BALL_CSTDIOOBSERVER
#define INCLUDED_BALL_CSTDIOOBSERVER

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide an observer that emits log records to a `FILE *`.
//
//@CLASSES:
//  ball::CstdioObserver: observer that emits log records to a `FILE *`
//
//@SEE_ALSO: ball_record, ball_context, ball_loggermanager
//
//@DESCRIPTION: This component provides a concrete implementation of the
// `ball::Observer` protocol for receiving and processing log records:
// ```
//                ,--------------------.
//               ( ball::CstdioObserver )
//                `--------------------'
//                          |              ctor
//                          |              disablePublishInLocalTime
//                          |              enablePublishInLocalTime
//                          |              setFormat
//                          |              setFormatFunctor
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
// `ball::CstdioObserver` is a concrete class derived from `ball::Observer`
// that processes the log records it receives through its `publish` method by
// writing them to a C `stdio` output file (`FILE *`).  Given its minimal
// functionality, `ball::CstdioObserver` should be used with care in a
// production environment.  It is not recommended to construct this observer
// with file-based streams due to lack of any file rotation functionality,
// however it is ideal for writing to `stdout` or `stderr`.
//
///Log Record Formatting
///---------------------
// By default, the output format of published log records is:
// ```
// DATE_TIME PID THREAD-ID SEVERITY FILE LINE CATEGORY MESSAGE USER-FIELDS
// ```
// where `DATE` and `TIME` are of the form `DDMonYYYY` and `HH:MM:SS.mmm`,
// respectively (`Mon` being the 3-letter abbreviation for the month).  For
// example, assuming that no user-defined fields are present, a log record
// will have the following appearance when the default format is in effect:
// ```
// 18MAY2005_18:58:12.076 7959 1 WARN ball_cstdioobserver.t.cpp 404 TEST hi!
// ```
// The default format and the formatter used can be changed by calling the
// `setFormat` method.  The format specifications can be either scheme-tagged
// (recommended) or legacy format strings that results in a
// `RecordStringFormatter` being used.  See {Scheme-Based Format Specifications
// (Recommended)} and {Legacy Format Specifications}.
// ```
// streamObserver.setFormat("qjson://%d %s %m");
// ```
// The above statement will cause subsequent records to be logged as JSON
// objects that contain a timestamp in 'DDMonYYYY_HH:MM:SS.mmm' format, the
// severity, and the log message.
//
///Scheme-Based Format Specifications (Recommended)
///- - - - - - - - - - - - - - - - - - - - - - - - -
// The log record format can be specified using a URI-like scheme-tagged
// format configuration string passed to the `setFormat` method.  The scheme
// determines which formatter will be used:
//
// * text://<format-specification>
//   > Uses `ball::RecordStringFormatter` with the provided format
//   > specification.  For example:
//   > `text://\n%d %p:%t %s %f:%l %c %m %a\n`
//
// * qjson://<format-specification>
//   > Uses `ball::RecordJsonFormatter` to output log records in JSON format
//   > using a simplified printf-style format specification.  The format
//   > specification uses `%`-prefixed fields (e.g., `%d` for timestamp, `%s`
//   > for severity, `%m` for message).  For example: `qjson://%d %s %m`.  See
//   > `ball_recordjsonformatter` for the complete list of supported `%`
//   > fields.
//
// * json://<format-specification>
//   > Uses `ball::RecordJsonFormatter` to output log records in JSON format.
//   > The format specification must be a JSON array (using `[]` brackets)
//   > listing the field names to include in the output.  For example:
//   > `json://["timestamp","severity","message"]`.  See
//   > `ball_recordjsonformatter` for details on supported field names and
//   > format specifications.
//
// Examples:
// ```
// // Use JSON formatter with simplified printf-style format specification
// streamObserver.setFormat("qjson://%d %s %m");
//
// // Use text formatter with custom format
// streamObserver.setFormat("text://%d %p %t %s %f %l %c %m %a\n");
//
// // Use JSON formatter with selected fields
// streamObserver.setFormat("json://[\"timestamp\",\"severity\",\"message\"]");
// ```
//
///Legacy Format Specifications
///- - - - - - - - - - - - - - -
// If no scheme is specified (i.e., the configuration doesn't contain `://`),
// the configuration is treated as a format specification for
// `ball::RecordStringFormatter` (equivalent to `text://`).  The `%`-prefixed
// conversion specifications for the "text://" format are defined in
// {`ball_recordstringformatter`}.
//
// Note that the observer's default text format emits newline characters at the
// beginning and at the end of a log record, so the user needs to add them
// explicitly to (text) format strings to preserve that behavior.
//
// There is also a legacy way to override the default format by supplying a
// suitable formatting functor using `setFormatFunctor`.  For example, an
// instance of `ball::RecordStringFormatter` is such a functor:
// ```
// ball::CstdioObserver streamObserver(stdout);
// streamObserver.setFormatFunctor(
//             ball::RecordStringFormatter("%I %p:%t %s %f:%l %c %m %a\n"));
// ```
// The above statement will cause subsequent records to be logged in a format
// that is almost identical to the default format except that the timestamp
// attribute will be written in ISO 8601 format, and the user fields will be
// replaced by the more modern attributes.
//
// Example using format specification directly (no scheme):
// ```
// // Backward compatible: no scheme means text format
// streamObserver.setFormat("%d %p %t %s %f %l %c %m %a\n");
// ```
//
///Thread Safety
///-------------
// All methods of `ball::CstdioObserver` are thread-safe, and can be called
// concurrently by multiple threads.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Basic Usage
/// - - - - - - - - - - -
// The following snippets of code illustrate the basic usage of
// `ball::CstdioObserver`.
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
// Next, create a cstdio observer `observer` with the `stdout` as the output
// stream.
// ```
// ball::CstdioObserver observer(stdout);
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
#include <bslma_usesbslmaallocator.h>

#include <bslmf_nestedtraitdeclaration.h>

#include <bslmt_mutex.h>

#include <bsls_assert.h>
#include <bsls_keyword.h>
#include <bsls_review.h>

#include <bsl_cstdio.h>
#include <bsl_functional.h>

namespace BloombergLP {
namespace ball {

class Context;
class Record;

                           // ====================
                           // class CstdioObserver
                           // ====================

/// This class provides a concrete implementation of the `Observer`
/// protocol.  The `publish` method of this class outputs the log records
/// that it receives to a `FILE *` supplied at construction and then flushes
/// the stream.
class CstdioObserver : public Observer {
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
    FILE                       *d_file_p;           // output sink for log
                                                    // records

    mutable bslmt::Mutex        d_mutex;            // serializes concurrent
                                                    // calls to 'publish'

    ObserverFormatterImp        d_observerFormatterImp;
                                                    // formatter manager that
                                                    // handles all formatting
                                                    // operations

    // NOT IMPLEMENTED
    CstdioObserver(const CstdioObserver&);
    CstdioObserver& operator=(const CstdioObserver&);

  public:
    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(CstdioObserver, bslma::UsesBslmaAllocator);

    // CREATORS

    /// Create a cstdio observer that transmits log records to the specified
    /// `stream`.  Optionally specify an `allocator` (e.g., the address of a
    /// `bslma::Allocator` object) to supply memory; otherwise, the default
    /// allocator is used.  Note that a default record format is in effect
    /// for cstdio logging (see `setFormat`).
    explicit
    CstdioObserver(FILE                  *stream,
                   const allocator_type&  allocator = allocator_type());

    /// Destroy this cstdio observer.
    ~CstdioObserver() BSLS_KEYWORD_OVERRIDE;

    // MANIPULATORS

      using Observer::publish;  // Picks up the deprecated `publish` overload.

    /// Process the specified log `record` having the specified publishing
    /// `context`.  Print `record` and `context` to the `FILE *` supplied at
    /// construction.  The behavior is undefined if `record` or `context`
    /// is modified during the execution of this method.
    void publish(const bsl::shared_ptr<const Record>& record,
                 const Context&                       context)
                                                         BSLS_KEYWORD_OVERRIDE;

    /// Discard any shared reference to a `Record` object that was supplied
    /// to the `publish` method, and is held by this observer.  Note that
    /// this operation should be called if resources underlying the
    /// previously provided shared-pointers must be released.  This method
    /// intentionally does nothing as no such resources are held; we publish
    /// all records immediately.
    void releaseRecords() BSLS_KEYWORD_OVERRIDE;

    /// Disable publishing of the timestamp attribute of records in local
    /// time by this cstdio observer; henceforth, timestamps will be in UTC
    /// time.  This method has no effect if publishing in local time is not
    /// enabled.
    void disablePublishInLocalTime();

    /// Enable publishing of the timestamp attribute of records in local
    /// time by this cstdio observer.  By default, timestamps are published
    /// in UTC time.  Note that this method also affects timestamps for
    /// formatters that use them.
    void enablePublishInLocalTime();

    /// Set the formatting functor used when writing records to the log of
    /// this cstdio observer to the specified `formatter` functor.  Note
    /// that a default format ("\n%d %p %t %s %f %l %c %m %u\n") is in
    /// effect until this method or `setFormat` is called (see
    /// `ball_recordstringformatter`).  Also note that the observer emits
    /// newline characters at the beginning and at the end of a log record
    /// by default, so the user needs to add them explicitly to the format
    /// string to preserve this behavior.  Note that this method is not
    /// able to communicate the timezone default settings to the
    /// `formatter`, prefer `setFormat`.
    void setFormatFunctor(const RecordFormatter& formatter);

    /// Set the formatting functor used when writing records to the log of
    /// this cstdio observer to a functor created according to the specified,
    /// possibly URI-like, scheme-tagged `format`.  Return zero if the setup
    /// with the specified arguments was successful and also save the `format`
    /// to be later retrievable using `getFormat`.  Otherwise (if no matching
    /// scheme could be found or the configuration is invalid) return a
    /// non-zero value and do not change the log record formatter used by
    /// this object.  Note that a default format
    /// ("\n%d %p %t %s %f %l %c %m %u\n") is in effect until this method or
    /// `setFormatFunctor` is called.  Also, notice that the observer emits
    /// newline characters at the beginning and at the end of a log record by
    /// (the) default (format), so the user needs to add them explicitly to
    /// (text) format strings to preserve that behavior.
    int setFormat(const bsl::string_view& format);

    // ACCESSORS

    /// Return `true` if this observer writes the timestamp attribute of
    /// records that it publishes in local time by default, and `false`
    /// otherwise (in which case timestamps are written by default in UTC time).
    bool isPublishInLocalTimeEnabled() const;

    /// Return the format config of the last successful `setFormat` call.
    const bsl::string& getFormat() const;
};

// ============================================================================
//                             INLINE DEFINITIONS
// ============================================================================

                           // --------------------
                           // class CstdioObserver
                           // --------------------

// MANIPULATORS
inline
void CstdioObserver::releaseRecords()
{
}

}  // close package namespace
}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// Copyright 2025 Bloomberg Finance L.P.
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
