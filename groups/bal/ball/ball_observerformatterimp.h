// ball_observerformatterimp.h                                        -*-C++-*-
#ifndef INCLUDED_BALL_OBSERVERFORMATTERIMP
#define INCLUDED_BALL_OBSERVERFORMATTERIMP

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide common methods for scheme-based formatters for observers
//
//@CLASSES:
//  ball::ObserverFormatterImp: common formatting-related methods for observers
//
//@SEE_ALSO: ball_cstdioobserver, ball_fileobserver2, ball_streamobserver
//
//@DESCRIPTION: This component provides a common implementation of methods
// necessary to support scheme-based formatter configuration.
//
///Log Record Formatting
///---------------------
// By default, the output format for log records is set by constructor
// arguments `format` and `timezoneDefault`.  The default format (and the
// formatter used) can be changed by calling the `setFormat` method.  See
// {Scheme-Based Formatters} for more information.
// ```
// observerFormatterImp.setFormat("qjson://%d %s %m");
// ```
// The above statement will cause subsequent records to be formatted as JSON
// objects that contains a timestamp in 'DDMonYYYY_HH:MM:SS.mmm' format, the
// severity, and the log message.
//
///Time zone default
///- - - - - - - - -
// The default time zone (UTC or local) for timestamps may be changed using the
// `setTimezoneDefault` method.  Note that this method creates and installs a
// new formatter that uses the last successfully set format and the new time
// zone default.
//
// The time zone default is called default because certain format configuration
// syntaxes (JSON:// at the time of writing) allow the user to explicitly
// specify the time zone of the timestamp.  For fields where it is not
// specified the default is used.  The `%` formats (at the time of writing) do
// not support specifying the time zone, so for such syntaxes the time zone
// default is used for all timestamp fields.
//
///Format Strings
/// - - - - - - -
// When using `printf`-style format strings, the respective formats are
// specified using `%`-prefixed conversion specifications.  (See
// {`ball_recordstringformatter`} for information on how format specifications
// are defined and interpreted.)  For example, the following statement will
// force subsequent records to be logged in a format that is almost identical
// to the default format except that the timestamp attribute will be written in
// ISO 8601 format:
// ```
// observerFormatterImp.setFormat("text://%I %p %t %s %f %l %c %m %a\n");
// ```
// The `setFormat` method uses the "text" scheme by default for format config
// strings that do not have a scheme, so the following is an equivalent (though
// less expressive and deprecated) method to produce the same configuration:
// ```
// observerFormatterImp.setFormat("%I %p %t %s %f %l %c %m %a\n");
// ```
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
// Examples:
// ```
// // Use text formatter with custom format
// observerFormatterImp.setFormat("text://%d %p %t %s %f %l %c %m %a\n");
//
// // Use JSON formatter with selected fields
// observerFormatterImp.setFormat(
//                          "json://[\"timestamp\",\"severity\",\"message\"]");
//
// // Use simplified printf-style JSON formatter
// observerFormatterImp.setFormat("qjson://%d %s %m");
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
// observerFormatterImp.setFormat("%d %p:%t %s %f:%l %c %m %a\n");
// observerFormatterImp.setFormat("text://%d %p:%t %s %f:%l %c %m %a\n");
// ```
// These `%`-prefixed conversion specifications are defined in
// {`ball_recordstringformatter`}.
//
///Legacy Custom Formatter Functor
///- - - - - - - - - - - - - - - -
// There is also a legacy way to change the format by supplying a suitable
// formatting functor using `setFormatFunctor`.  For example, an instance of
// `ball::RecordStringFormatter` conveniently is such a functor:
// ```
// ObserverFormatterImp.setFormatFunctor(
//                ball::RecordStringFormatter("%I %p:%t %s %f:%l %c %m %a\n"));
// ```
// The above statement will cause subsequent records to be formatted by that
// string formatter.  When the formatter is set up this legacy way the
// `setTimezoneDefault` method will not affect the formatter and so the return
// value of `getTimezoneDefault` is meaningless.
//
///Thread Safety
///-------------
// This class is deliberately only const thread-safe, meaning that the concrete
// observer implementation has to provide and lock a mutex before calling the
// methods of this class (except for the constructor).
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Implementing a Formatting Observer
///- - - - - - - - - - - - - - - - - - - - - - -
// In this example, we demonstrate how to use `ObserverFormatterImp` to
// implement a simple observer that writes formatted log records to standard
// output.  First, we define a simple observer class that uses
// `ObserverFormatterImp` to manage formatting:
// ```
//  /// This class provides a simple observer implementation that writes
//  /// formatted log records to 'bsl::cout'.
//  class MySimpleObserver : public ball::Observer {
//
//    public:
//      // TYPES
//      typedef bsl::allocator<char> allocator_type;
//
//    private:
//      // DATA
//      mutable bslmt::Mutex       d_mutex;         // synchronize access
//      ball::ObserverFormatterImp d_formatterImp;  // formatter manager
//
//    public:
//      // CREATORS
//      explicit MySimpleObserver(const allocator_type& allocator
//                                                          = allocator_type())
//      : d_formatterImp("text://%t %s %m\n",
//                       ball::RecordFormatterTimezone::e_UTC,
//                       allocator)
//      {
//      }
//
//      // MANIPULATORS
//      void disablePublishInLocalTime()
//      {
//          bslmt::LockGuard<bslmt::Mutex> guard(&d_mutex);
//          d_formatterImp.setTimezoneDefault(
//                                       ball::RecordFormatterTimezone::e_UTC);
//      }
//
//      void enablePublishInLocalTime()
//      {
//          bslmt::LockGuard<bslmt::Mutex> guard(&d_mutex);
//          d_formatterImp.setTimezoneDefault(
//                                     ball::RecordFormatterTimezone::e_LOCAL);
//      }
//
//      using Observer::publish;
//
//      void publish(const bsl::shared_ptr<const ball::Record>& record,
//                   const ball::Context&                       context)
//                                                        BSLS_KEYWORD_OVERRIDE
//      {
//          bslmt::LockGuard<bslmt::Mutex> guard(&d_mutex);
//          d_formatterImp.formatLogRecord(bsl::cout, record);
//      }
//
//      void releaseRecords() BSLS_KEYWORD_OVERRIDE
//      {
//          // No-op for this observer
//      }
//
//      int setFormat(const bsl::string_view& format)
//      {
//          bslmt::LockGuard<bslmt::Mutex> guard(&d_mutex);
//          return d_formatterImp.setFormat(format);
//      }
//
//      void setFormatFunctor(
//         const ball::RecordFormatterFunctor::Type& formatter)
//      {
//          bslmt::LockGuard<bslmt::Mutex> guard(&d_mutex);
//          d_formatterImp.setFormatFunctor(formatter);
//      }
//
//      // ACCESSORS
//      const bsl::string& getFormat() const
//      {
//          bslmt::LockGuard<bslmt::Mutex> guard(&d_mutex);
//          return d_formatterImp.getFormat();
//      }
//
//      bool isPublishInLocalTimeEnabled() const
//      {
//          bslmt::LockGuard<bslmt::Mutex> guard(&d_mutex);
//          return ball::RecordFormatterTimezone::e_LOCAL ==
//                                        d_formatterImp.getTimezoneDefault();
//      }
//  };
// ```
// Now, we can create an instance of our observer and configure its formatting.
// First, we create an observer with default text-based format:
// ```
//  MySimpleObserver observer;
// ```
// Next, we can change the format to JSON format using the `setFormat` method:
// ```
//  const int rc = observer.setFormat("qjson://%t %s %m");
//  assert(0 == rc);
// ```
// We can also enable local time for timestamps:
// ```
//  observer.enablePublishInLocalTime();
//  assert(observer.isPublishInLocalTimeEnabled());
// ```
// To revert to UTC time:
// ```
//  observer.disablePublishInLocalTime();
//  assert(!observer.isPublishInLocalTimeEnabled());
// ```
// We can also retrieve the current format configuration:
// ```
//  const bsl::string& currentFormat = observer.getFormat();
//  assert("qjson://%t %s %m" == currentFormat);
// ```
// For backwards compatibility, we can also use a custom formatter functor:
// ```
//  observer.setFormatFunctor(
//                ball::RecordStringFormatter("%I %p:%t %s %f:%l %c %m %a\n"));
// ```
// Now let's demonstrate actually publishing a log record. First, we create a
// sample record and context with a fixed timestamp:
// ```
//  const bdlt::Datetime    timestamp(2024, 1, 15, 12, 34, 56, 789);
//  ball::RecordAttributes  attributes(
//                              timestamp,                 // timestamp
//                              42,                        // process ID
//                              123,                       // thread ID
//                              "example.cpp",             // file name
//                              456,                       // line number
//                              "EXAMPLE",                 // category
//                              ball::Severity::e_WARN,    // severity
//                              "Sample warning message"); // message
//
//  bsl::shared_ptr<ball::Record>  record;
//  record.createInplace(bslma::Default::allocator(),
//                       attributes,
//                       ball::UserFields());
//  ball::Context context;
// ```
// To capture the output for verification, we redirect 'bsl::cout' to a string
// stream:
// ```
//  bsl::ostringstream  oss;
//  bsl::streambuf     *originalRdbuf = bsl::cout.rdbuf();
// ```
// When we publish this record with text format, it produces plain text output:
// ```
//  observer.setFormat("text://%d %p:%t %s %f:%l %c %m\n");
//  bsl::cout.rdbuf(oss.rdbuf());
//  observer.publish(record, context);
//
//  bsl::string output = oss.str();
//  bsl::cout.rdbuf(originalRdbuf);
//  assert("15JAN2024_12:34:56.789 42:123 WARN example.cpp:456 EXAMPLE "
//         "Sample warning message\n" == output);
//
//  oss.str("");  // Clear the stream for next test
// ```
// Next, we publish the same record with JSON format that produces structured
// JSON output:
// ```
//  observer.setFormat("qjson://%d %p %t %s %F %l %c %m");
//  bsl::cout.rdbuf(oss.rdbuf());
//  observer.publish(record, context);
//
//  output = oss.str();
//  bsl::cout.rdbuf(originalRdbuf);
// ```
// Finally, we verify the JSON fields:
// ```
//  assert(bsl::string::npos != output.find("\"timestamp\""));
//  assert(bsl::string::npos != output.find("\"15JAN2024_12:34:56.789\""));
//  assert(bsl::string::npos != output.find("\"pid\""));
//  assert(bsl::string::npos != output.find("42"));
//  assert(bsl::string::npos != output.find("\"tid\""));
//  assert(bsl::string::npos != output.find("123"));
//  assert(bsl::string::npos != output.find("\"severity\""));
//  assert(bsl::string::npos != output.find("\"WARN\""));
//  assert(bsl::string::npos != output.find("\"message\""));
//  assert(bsl::string::npos != output.find("\"Sample warning message\""));
// ```

#include <balscm_version.h>

#include <ball_recordformatterfunctor.h>
#include <ball_recordformatteroptions.h>
#include <ball_recordformatterregistryutil.h>
#include <ball_recordformattertimezone.h>

#include <bsla_deprecated.h>

#include <bslma_bslallocator.h>

#include <bsls_assert.h>

#include <bsl_iosfwd.h>
#include <bsl_memory.h>
#include <bsl_string.h>
#include <bsl_string_view.h>

namespace BloombergLP {
namespace ball {

                        // ==========================
                        // class ObserverFormatterImp
                        // ==========================

/// This class provides a common implementation of formatting-related methods
/// for observer components that support scheme-based log record formatting.
/// It manages a log record formatter, format configuration strings, and time
/// zone preferences.  This implementation is intended to be used as a data
/// member in concrete observer classes to provide consistent formatting
/// behavior across different observer types.
class ObserverFormatterImp {
  public:
    // TYPES

    /// `RecordFormatter` is an alias for the type of the functor used for
    /// formatting log records to a stream.
    typedef RecordFormatterFunctor::Type RecordFormatter;

    typedef bsl::allocator<char> allocator_type;

  private:
    // PRIVATE TYPES
    typedef RecordFormatterTimezone::Enum TimezoneEnum;

  private:
    // DATA
    bsl::string           d_format;           // canonical format that
                                              // resulted in `d_formatter` or
                                              // an empty string if it was set
                                              // using `setFormatFunctor`

    TimezoneEnum          d_timezoneDefault;  // Local, or UTC.

    RecordFormatter       d_formatter;        // formatting functor used when
                                              // writing to log
    // NOT IMPLEMENTED
    ObserverFormatterImp(const ObserverFormatterImp&);
    ObserverFormatterImp& operator=(const ObserverFormatterImp&);

  private:
    // PRIVATE MANIPULATORS

  public:
    // CREATORS

    /// Create a `ObserverFormatterImp` instance that formats log records
    /// using the specified `format` and `timezoneDefault`.  It is strongly
    /// advised to specify UTC for timezoneDefault.  Optionally specify an
    /// `allocator` (e.g., the address of a `bslma::Allocator` object) to
    /// supply memory; otherwise, the default allocator is used.  The
    /// behavior is undefined unless `format` is a valid format string
    /// (either one that starts with a scheme or a "text" scheme format).
    /// Note that not giving a valid default format string by the observer
    /// will result in no logging output if no valid format is set
    /// afterwards, so it is a serious error.
    explicit
    ObserverFormatterImp(const bsl::string_view& format,
                         TimezoneEnum            timezoneDefault,
                         const allocator_type&   allocator = allocator_type());

    // MANIPULATORS

    /// Set the default time zone used for timestamps that have no timezone
    /// preference specified in the format config string.  This method
    /// recreates the formatter used with the new timezone setting and the
    /// last successfully-set format config string.  Note that (at the time of
    /// writing) the printf-style format strings do not have syntax to specify
    /// the time zone so for such-configured formatters all timestamps will use
    /// this time zone default.
    void setTimezoneDefault(TimezoneEnum timezoneDefault);

    /// Set the formatting functor used when writing records to the specified
    /// `formatter` functor.  Note that this
    /// method is not able to communicate the timezone
    /// default settings to the `formatter`, prefer `setFormat`.
    void setFormatFunctor(const RecordFormatter& formatter);

    /// Set the log record format specifications for this object according to
    /// the specified `format`.  Return 0 on success, and a non-zero value
    /// otherwise.  This method has no effect on format if `format` is not a
    /// valid format specification.  If `format` does not start with a scheme
    /// (it has no "://" in it) use the default "text" scheme.
    int setFormat(const bsl::string_view& format);

    // ACCESSORS

    /// Format the specified log `record` into the specified output `stream`
    /// using the current record formatter functor.
    void formatLogRecord(bsl::ostream&                        stream,
                         const bsl::shared_ptr<const Record>& record) const;

    /// Return the currently active format configuration string of this object.
    const bsl::string& getFormat() const;

    /// Return the currently active time zone default of this object.
    TimezoneEnum getTimezoneDefault() const;

                              // Aspects

    /// Return the allocator used by this object to supply memory.  Note
    /// that if no allocator was supplied at construction the default
    /// allocator in effect at construction is used.
    allocator_type get_allocator() const;
};

// ============================================================================
//                             INLINE DEFINITIONS
// ============================================================================

                        // --------------------------
                        // class ObserverFormatterImp
                        // --------------------------

// CREATORS
inline
ObserverFormatterImp::ObserverFormatterImp(
                                       const bsl::string_view& format,
                                       TimezoneEnum            timezoneDefault,
                                       const allocator_type&   allocator)
: d_format(allocator)
, d_timezoneDefault(timezoneDefault)
, d_formatter(bsl::allocator_arg, allocator)
{
    const int setFormatResult = setFormat(format);
    BSLS_ASSERT(0 == setFormatResult);  (void)setFormatResult;
}

// MANIPULATORS
inline
int ObserverFormatterImp::setFormat(const bsl::string_view& format)
{
    const int rc = RecordFormatterRegistryUtil::createRecordFormatter(
                                    &d_formatter,
                                    format,
                                    RecordFormatterOptions(d_timezoneDefault));

    if (0 == rc) {
        // Update format string on success
        d_format = format;
    }
    return rc;
}

inline
void ObserverFormatterImp::setFormatFunctor(const RecordFormatter& formatter)
{
    d_formatter = formatter;
    d_format.clear();
}

inline
void ObserverFormatterImp::setTimezoneDefault(TimezoneEnum timezoneDefault)
{
    if (d_timezoneDefault == timezoneDefault) {
        // No change, nothing to do
        return;                                                       // RETURN
    }

    d_timezoneDefault = timezoneDefault;

    // If the formatter was created from a format string, recreate it with
    // the new timezone setting.
    if (!d_format.empty()) {
        RecordFormatterRegistryUtil::createRecordFormatter(
                                    &d_formatter,
                                    d_format,
                                    RecordFormatterOptions(d_timezoneDefault));
    }
}

// ACCESSORS
inline
const bsl::string& ObserverFormatterImp::getFormat() const
{
    return d_format;
}

inline
void ObserverFormatterImp::formatLogRecord(
                              bsl::ostream&                        stream,
                              const bsl::shared_ptr<const Record>& record) const
{
    d_formatter(stream, *record);
}

inline
ObserverFormatterImp::TimezoneEnum
ObserverFormatterImp::getTimezoneDefault() const
{
    return d_timezoneDefault;
}

                              // Aspects
inline
ObserverFormatterImp::allocator_type
ObserverFormatterImp::get_allocator() const
{
    return d_format.get_allocator();
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
