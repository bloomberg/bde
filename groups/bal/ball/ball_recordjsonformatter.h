// ball_recordjsonformatter.h                                         -*-C++-*-

#ifndef INCLUDED_BALL_RECORDJSONFORMATTER
#define INCLUDED_BALL_RECORDJSONFORMATTER

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a formatter for log records that renders output in JSON.
//
//@CLASSES:
//  ball::RecordJsonFormatter: formatter for rendering log records in JSON
//
//@SEE_ALSO: ball_record, ball_recordattributes
//
//@DESCRIPTION: This component provides a function object class,
// `ball::RecordJsonFormatter`, that formats a log record as JSON text elements
// according to a format specification (see {`Record Format Specification`}).
// `ball::RecordJsonFormatter` is designed to match the function signature
// expected by many concrete `ball::Observer` implementations that publish log
// records (for example, see `ball::FileObserver2::setLogFileFunctor`).
//
// NOTE: `ball::RecordJsonFormatter` renders individual log records as JSON,
// but, for example, a resulting log file would contain a sequence of JSON
// strings, which is not itself valid JSON text.
//
///Simplified Record Format Specification
///---------------------------------------
// `RecordJsonFormatter` supports a simplified format using `printf`-style
// (`%`-prefixed) conversion specifications via the `setSimplifiedFormat`
// method.  This format provides a more concise way to specify common logging
// patterns without the verbosity of JSON arrays.
//
// Note: The `qjson://` scheme that uses this simplified format is registered
// by `ball::RecordFormatterRegistryUtil`, not by this component.  Other
// schemes could be registered to use the same simplified format syntax.
//
// The following table lists the `%`-prefixed conversion specifications that
// are recognized within a simplified format specification:
// ```
// %d - timestamp in 'DDMonYYYY_HH:MM:SS.mmm' format (28AUG2020_14:43:50.375)
// %i - timestamp in ISO 8601 format without fractional seconds
// %I - timestamp in ISO 8601 format with millisecond precision
// %T - thread Id in hexadecimal
// %t - thread Id in decimal
// %K - kernel thread Id in hexadecimal
// %k - kernel thread Id in decimal
// %p - process Id
// %F - filename (basename of __FILE__ only)
// %f - filename (full path from __FILE__)
// %l - line number
// %c - category name
// %s - severity
// %m - log message
// %A - all user-defined attributes
// %a[name] - specific user-defined attribute with the given name
// ```
// Field specifications may be separated by whitespace (spaces, tabs, newlines)
// or commas, which are ignored by the parser.  For example, these format
// specifications are equivalent:
// ```
// "%d %T %s %c %m"
// "%d, %T, %s, %c, %m"
// "%d,%T,%s,%c,%m"
// ```
// For example, the format specification:
// ```
// "%d %T %s %c %m"
// ```
// passed to `setSimplifiedFormat` would result in a log record like:
// ```
// { "timestamp": "28AUG2020_14:43:50.375",
//   "tid": "0xA7654EFF3540",
//   "severity": "INFO",
//   "category": "MyCategory",
//   "message": "Hello, world!"
// }
// ```
// Each `%`-prefixed field is rendered as a JSON key-value pair with a default
// field name (e.g., "timestamp", "tid", "severity").  Field names can be
// customized by prefixing a format specifier with `<fieldName>:`, for example:
// `myTime:%d` uses "myTime" as the field name instead of "timestamp".  For
// more advanced formatting options, use the full JSON array format
// specification described below.
//
///JSON Record Format Specification
///---------------------------------
// A full featured format specification is, itself, a JSON array, supplied to a
// `RecordJsonFormatter` object by the `setJsonFormat` function.  If no format
// is specified, the default format is used.  Each array element specifies the
// format of a log record field or a user-defined attribute.
//
// Note: The `json://` scheme that uses this JSON array format is registered
// by `ball::RecordFormatterRegistryUtil`, not by this component.  Other
// schemes could be registered to use the same JSON output format, like how
// `qjson://` is an additional format syntax also resulting in JSON log
// records.
//
// Here is a simple example:
// ```
// [{"timestamp":{"format":"iso8601"}}, "pid", "tid", "severity", "message"]
// ```
// would a result in a log record like:
// ```
// { "timestamp": "2020-08-28T14:43:50.375Z",
//   "pid": 2313,
//   "tid": 12349388604,
//   "severity": "INFO",
//   "message": "Hello, world!"
//  }
// ```
// The format specification is a JSON array, each element of which can be one
// of the following:
//
// * A string containing the name of the fixed record field or the name of the
//   user-defined attribute, in which case the field or attribute will be
//   published in the default format.  For example, `[timestamp]`, would
//   display the timestamp as: `{"timestamp": "2020-08-28T14:43:50.375Z"}`.
// * A JSON object having the name of the fixed field or user-defined
//   attribute and a set of key-values pairs used to customize the output
//   format.  For example,
//   `[{"timestamp": {"name": "My Time", "format": "bdePrint"}}]`, would
//   display the timestamp as: `{"My Time": "28AUG2020_14:43:50.375"}`.
//
///Field Format Specification
/// - - - - - - - - - - - - -
// The following table lists the predefined string values for each fixed field
// and user-defined attributes in the log record:
// ```
//    Tag                 Description                   Example
// --------------    -------------------------        -------------
// "timestamp"       creation date and time           ["timestamp"]
// "pid"             process id of creator            ["pid"]
// "tid"             thread id of creator             ["tid"]
// "ktid"            kernel thread id of creator      ["ktid"]
// "file"            file where created  (__FILE__)   ["file"]
// "line"            line number in file (__LINE__)   ["line"]
// "category"        category of logged record        ["category"]
// "severity"        severity of logged record        ["severity"]
// "message"         log message text                 ["message"]
// "attributes"      all user-defined attributes      ["attributes"]
// <attribute name>  specific user-defined attribute  ["bas.uuid"]
// ```
// The output format of each field can be customized by replacing a string
// value in the JSON array with a JSON object having the same name and a set of
// key-value pairs (attributes).
//
///Verifying the Format Specification for `setJsonFormat`
/// - - - - - - - - - - - - - - - - - - - - - - - - - - -
// The sections that follow describe the set of fields that can be provided in
// the format specification supplied to `setJsonFormat`.
// `RecordJsonFormatter::setJsonFormat` will ignore fields in the provided
// format specification that are unknown, but will report an error if a known
// field contains a property that is not supported.  For example: a format
// specification '["pid", { "timestamp" : {"unknown field!": "value"} }] will
// be accepted, but `["pid", {"timestamp": {"format": "unknown format" }}]`
// will produce an error.
//
// Each key-value pair of a JSON object that specifies a format of an output of
// a fixed record field or a user-defined attribute has the following
// constrains:
//
// * The key is a string of known value listed in the column "Key" in the
//   tables below.  Any string that does not match the listed values is
//   ignored.
// * The value is a string of known value (except for the "name" key) in the
//   column "Value Constraint" in the tables below.  If the value does not
//   match the string values specified in the tables, the format specification
//   is considered to be inconsistent with the expected schema, and is
//   rejected by the `RecordJsonFormatter::setJsonFormat` method.
//
///The "timestamp" field format
///-  -  -  -  -  -  -  -  -  -
// The format attributes of the "timestamp" object are given in the following
// table:
// ```
//                                             Value             Default
//            Key              Description     Constraint        Value
// ------------------------  ----------------  -----------     ------------
// "name"                    name by which     JSON string     "timestamp"
//                           "timestamp" will
//                           be published
//
// "format"                  datetime format   "iso8601",     "iso8601"
//                                             "bdePrint"
//                                             (*Note*)
//
// "fractionalSecPrecision"  second precision  "none",         "microseconds"
//                                             "milliseconds",
//                                             "microseconds"
//
// "timeZone"                time zone         "utc",          "utc"
//                                             "local"
// ```
// *Note*: The default "bdePrint" format denotes the following datetime format:
// ```
//  DDMonYYYY_HH:MM:SS.mmm
// ```
// For example, the following record format specification:
// ```
//  [ { "timestamp": { "name": "Time",
//                     "fractionalSecPrecision": "microseconds",
//                     "timeZone": "local" } }
//  ]
// ```
// would a result in a log record like:
// ```
//  { "Time": "28AUG2020_17:43:50.375345" }
// ```
//
///The "pid" (process Id) field format
/// -  -  -  -  -  -  -  -  -  -  -  -
// The format attributes of the process Id field are given in the following
// table:
// ```
//                                                  Value        Default
//   Key                 Description                Constraint   Value
// ------    -------------------------------------  -----------  -------
// "name"    name by which "pid" will be published  JSON string  "pid"
// ```
// For example, the following record format specification:
// ```
//  [ { "pid": { "name": "Process Id" } } ]
// ```
// would a result in a log record like:
// ```
//  { "Process Id": 2313 }
// ```
//
///The "tid"/"ktid" (thread/kernel thread Id) field format
///-  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
// The format attributes of the thread Id field are given in the following
// table:
// ```
//                                                    Value        Default
//   Key                   Description                Constraint   Value
// --------  -------------------------------------  -----------  ------------
// "name"    name by which "tid"/"ktid" will be     JSON string  "tid"/"ktid"
//           published
//
// "format"  output format                          "decimal",   "decimal"
//                                                  "hex"
// ```
// For example, the following record format specification:
// ```
//  [ { "tid": { "name": "Thread Id",
//               "format": "hex" } }
//  ]
// ```
// would a result in a log record like:
// ```
//  { "Thread Id": 0xA7654EFF3540 }
// ```
//
///The "file" field format
/// -  -  -  -  -  -  -  -  -  -  -  -
// The format attributes of the "file" field are given in the following
// table:
// ```
//                                                               Default
//   Key          Description             Value Constraint       Value
// ------    --------------------  ----------------------------- -------
// "name"    name by which "file"  JSON string                   "file"
//           will be published
//
// "path"    file path             "full" (__FILE__),            "full"
//                                 "file" (basename of __FILE__)
// ```
// For example, the following record format specification:
// ```
//  [ { "file": { "name": "File",
//                "path": "file" } }
//  ]
// ```
// would a result in a log record like:
// ```
//  { "File": "test.cpp" }
// ```
//
///The "line" field format
/// -  -  -  -  -  -  -  -
// The format attributes of the "line" field are given in the following
// table:
// ```
//                                                   Value        Default
//   Key                Description                  Constraint   Value
// ------   ---------------------------------------  -----------  -------
// "name"   name by which "line" will be published   JSON string  "line"
// ```
// For example, the following record format specification:
// ```
//  [ { "line": { "name": "Line" } } ]
// ```
// would a result in a log record like:
// ```
//  { "Line": 512 }
// ```
//
///The "category" field format
///  -  -  -  -  -  -  -  -  -
// The format attributes of the "category" field are given in the following
// table:
// ```
//                                                     Value         Default
//   Key                 Description                   Constraint    Value
// ------  ------------------------------------------  -----------  ----------
// "name"  name by which "category" will be published  JSON string  "category"
// ```
// For example, the following record format specification:
// ```
//  [ { "category": { "name": "Category" } } ]
// ```
// would a result in a log record like:
// ```
//  { "category": "Server" }
// ```
//
///The "severity" field format
///  -  -  -  -  -  -  -  -  -
// The format attributes of the "severity" field are given in the following
// table:
// ```
//                                                     Value         Default
//   Key                  Description                  Constraint    Value
// ------  ------------------------------------------  -----------  ----------
// "name"  name by which "severity" will be published  JSON string  "severity"
// ```
// For example, the following record format specification:
// ```
//  [ { "severity": { "name": "severity" } } ]
// ```
// would a result in a log record like:
// ```
//  { "Severity": "ERROR" }
// ```
//
///The "message" field format
///  -  -  -  -  -  -  -  -  -
// A message is a JSON string which is a sequence of zero or more Unicode
// characters, wrapped in double quotes, using backslash escapes: (\", \\, \/,
// \b, \f, \n, \r, \t, \u{4 hex digits}).
//
// The format attributes of the "message" field are given in the following
// table:
// ```
//                                                    Value         Default
//   Key                 Description                  Constraint    Value
// ------  -----------------------------------------  -----------  ---------
// "name"  name by which "message" will be published  JSON string  "message"
// ```
// For example, the following record format specification:
// ```
//  [ { "message": { "name": "msg" } } ]
// ```
// would a result in a log record like:
// ```
//  { "msg": "Log message" }
// ```
//
///The "attributes" format
///-  -  -  -  -  -  -  -  -  -  -
// The "attributes" JSON object has no attributes. For example, the following
// record format specification:
// ```
//  [ "attributes" ]
// ```
// would (assuming there are two attributes "bas.requestid" and
// "mylib.security") result in a log record like:
// ```
//  { "bas.requestid": 12345, "mylib.security": "My Security" }
// ```
//
///A user-defined attribute format
///-  -  -  -  -  -  -  -  -  -  -
// Each user-defined attribute has a single "name" attribute that can be used
// to rename the user-defined attribute:
// ```
//                                        Value        Default
//   Key         Description              Constraint   Value
// ------   ----------------------------  -----------  -------
// "name"   name by which a user-defined  JSON string   none
//          attribute will be published
// ```
// For example, the following record format specification:
// ```
//  [ { "bas.uuid": { "name": "BAS.UUID" } } ]
// ```
// would a result in a log record like:
// ```
//  { "BAS.UUID": 3593 }
// ```
//
///The Record Separator
///--------------------
// The record separator is a string that is printed after each formatted
// record.  The default value of the record separator is a single newline, but
// it can be set to any string of the user's choice using the
// `RecordJsonFormatter::setRecordSeparator` function.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example: Format log records as JSON and render them to `stdout`
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose an application needs to format log records as JSON and output them
// to `stdout`.
//
// First we instantiate a JSON record formatter:
// ```
//  ball::RecordJsonFormatter formatter;
// ```
// Next we set a format specification to the newly created `formatter`:
// ```
// const int rc = formatter.setJsonFormat("[\"tid\",\"message\"]");
// assert(0 == rc);  (void)rc;
// ```
// The chosen format specification indicates that, when a record is formatted
// using `formatter`, the thread Id attribute of the record will be output
// followed by the message attribute of the record.
//
// Then we create a default `ball::Record` and set the thread Id and message
// attributes of the record to dummy values:
// ```
// ball::Record record;
//
// record.fixedFields().setThreadID(6);
// record.fixedFields().setMessage("Hello, World!");
// ```
// Next, invocation of the `formatter` function object to format `record` to
// `bsl::cout`:
// ```
// formatter(bsl::cout, record);
// ```
// yields this output, which is terminated by a single newline:
// ```
// {"tid":6,"message":"Hello, World!"}
// ```
// Finally, we change the record separator and format the same record again:
// ```
// formatter.setRecordSeparator("\n\n");
// formatter(bsl::cout, record);
// ```
// The record is printed in the same format, but now terminated by two
// newlines:
// ```
// {"tid":6,"message":"Hello, World!"}
//
// ```

#include <balscm_version.h>

#include <ball_recordformatterfunctor.h>
#include <ball_recordformatteroptions.h>
#include <ball_recordformattertimezone.h>

#include <bslma_allocator.h>
#include <bslma_bslallocator.h>

#include <bslmf_movableref.h>

#include <bsla_deprecated.h>

#include <bsls_keyword.h>

#include <bsl_functional.h>
#include <bsl_iosfwd.h>
#include <bsl_string.h>
#include <bsl_vector.h>

#include <bsl_ostream.h>

namespace BloombergLP {
namespace baljsn { class SimpleFormatter; }
namespace ball {

class Record;
class RecordAttributes;
class RecordJsonFormatter_FieldFormatter;

                        // =========================
                        // class RecordJsonFormatter
                        // =========================

/// This class provides a function object that formats a log record as JSON
/// text elements and renders them to an output stream.  The overloaded
/// `operator()` provided by the class formats log record according to a
/// message format specification supplied either by `setJsonFormat` or
/// `setSimplifiedFormat` manipulator or the default format installed by the
/// constructor) and outputs the result to the stream.  While this functor type
/// is designed to match the function signature expected by many concrete
/// `ball::Observer` implementations that publish log records (for example,
///  see `ball::FileObserver2::setLogFileFunctor`) it is advised to use the
/// more flexible scheme-based format selection provided now by every BDE-made
/// observer.
class RecordJsonFormatter {

    // PRIVATE TYPES

    /// `MoveUtil` is an alias for `bslmf::MovableRefUtil`.
    typedef bslmf::MovableRefUtil  MoveUtil;

  public:
    // TYPES

    /// `FieldFormatters` is an alias for a vector of the
    /// `RecordJsonFormatter_FieldFormatter` objects, each of which is
    /// initialized from the format specification and responsible for
    /// rendering one field of a `ball::Record` to the output JSON stream.
    typedef bsl::vector<RecordJsonFormatter_FieldFormatter*> FieldFormatters;

    typedef bsl::allocator<> allocator_type;

    /// Enumerator to determine the syntax of the specification string.
    enum SpecSyntax {
        e_JSON,
        e_SIMPLIFIED
    };

  private:
    // DATA
    bsl::string     d_formatSpec;       // format specification (json or qjson)
    SpecSyntax      d_specSyntax;       // how to interpret `d_formatSpec`

    RecordFormatterTimezone::Enum
                    d_timezoneDefault;  // `e_LOCAL` to publish in local time,
                                        // `e_UTC` for UTC.

    bsl::string     d_recordSeparator;  // string to print after each record

    FieldFormatters d_fieldFormatters;  // field formatters configured
                                        // according to the format
                                        // specification

    // PRIVATE MANIPULATORS

    /// Apply the current format specification `d_formatSpec` according to the
    /// syntax determined by `d_specSyntax` to reconfigure this formatter's
    //  field formatters.
    void applyCurrentFormat();

    // CLASS METHODS

    /// Destroy the field formatters contained in the specified
    /// `formatters`.
    static
    void releaseFieldFormatters(FieldFormatters *formatters);

  public:
    // CLASS METHODS

    /// This class method configures a formatter for the "json" scheme using
    /// the specified `format` and `formatOptions` and if successful loads it
    /// into the specified `output` and returns zero. In case configuration
    /// fails a non-zero value is returned and `output` is not modified.
    static int loadJsonSchemeFormatter(
                  RecordFormatterFunctor::Type         *output,
                  const bsl::string_view&               format,
                  const RecordFormatterOptions&         formatOptions);

    /// This class method configures a formatter for the "qjson" scheme using
    /// the specified `format` and `formatOptions` and if successful loads it
    /// into the specified `output` and returns zero. In case configuration
    /// fails a non-zero value is returned and `output` is not modified.
    static int loadQjsonSchemeFormatter(
                  RecordFormatterFunctor::Type         *output,
                  const bsl::string_view&               format,
                  const RecordFormatterOptions&         formatOptions);

    // CREATORS

    /// Create a record JSON formatter having a default format specification
    /// and record separator.  Optionally specify an `allocator` (e.g., the
    /// address of a `bslma::Allocator` object) to supply memory; otherwise,
    /// the allocator is used.  The default format specification is:
    /// ```
    /// ["timestamp", "processId", "threadId", "severity", "file", "line",
    ///  "category",  "message", "attributes"]
    /// ```
    /// The default record separator is "\n".
    explicit RecordJsonFormatter(
                           const allocator_type& allocator = allocator_type());

    /// Create a record JSON formatter initialized to the value of the
    /// specified `original` record formatter.  Optionally specify an
    /// `allocator` (e.g., the address of a `bslma::Allocator` object) to
    /// supply memory; otherwise, the default allocator is used.
    RecordJsonFormatter(
                      const RecordJsonFormatter& original,
                      const allocator_type&      allocator = allocator_type());

    /// Create a record JSON formatter having the same format specification
    /// and record separator as in the specified `original` formatter, and
    /// adopting all outstanding memory allocations and the allocator
    /// associated with the `original` formatter.  `original` is left in a
    /// valid but unspecified state.
    RecordJsonFormatter(bslmf::MovableRef<RecordJsonFormatter> original)
                                                         BSLS_KEYWORD_NOEXCEPT;

    /// Create a record JSON formatter, having the same format specification
    /// and record separator as in the specified `original` formatter.  The
    /// format specification of `original` is moved to the new object, and
    /// all outstanding memory allocations and the specified `allocator` are
    /// adopted if `allocator == original.get_allocator()`.  `original` is
    /// left in a valid but unspecified state.
    RecordJsonFormatter(bslmf::MovableRef<RecordJsonFormatter> original,
                        const allocator_type&                  allocator);

    /// Destroy this object.
    ~RecordJsonFormatter();

    // MANIPULATORS

    /// Assign to this object the value of the specified `rhs` object, and
    /// return a reference providing modifiable access to this object.
    RecordJsonFormatter& operator=(const RecordJsonFormatter& rhs);

    /// Assign to this object the format specification and record separator
    /// of the specified `rhs` object, and return a reference providing
    /// modifiable access to this object.  The format specification and
    /// record separator of `rhs` are moved to this object, and all
    /// outstanding memory allocations and the allocator associated with
    /// `rhs` are adopted if `get_allocator() == rhs.get_allocator()`.
    /// `rhs` is left in a valid but unspecified state.
    RecordJsonFormatter& operator=(bslmf::MovableRef<RecordJsonFormatter> rhs);

    /// Set the message format specification (see {`Record Format
    /// Specification`}) of this record JSON formatter to the specified
    /// JSON-syntax `format`.  Return 0 on success, and a non-zero value
    /// otherwise (if `format` is not valid JSON *or* not a JSON conforming to
    /// the expected schema).  Note that this is the method used by the
    /// `json://` scheme.
    int setJsonFormat(const bsl::string_view& format);

    /// @DEPRECATED: Use `setJsonFormat` instead.
    ///
    /// Set the message format specification (see {`Record Format
    /// Specification`}) of this record JSON formatter to the specified
    /// JSON-syntax `format`.  Return 0 on success, and a non-zero value
    /// otherwise (if `format` is not valid JSON *or* not a JSON conforming to
    /// the expected schema).
    int setFormat(const bsl::string_view& format);

    /// Parse the simplified format specification (see {`Simplified Record
    /// Format Specification`}) in the specified `format` and configure this
    /// formatter accordingly. Return 0 on success, and a non-zero value
    /// otherwise.
    int setSimplifiedFormat(const bsl::string_view& format);

    /// Set the time zone default for every "timestamp" output without a
    /// timezone specified in the format string to the specified
    /// `timezoneDefault`.  Note that this method reapplies the current format
    /// specification to update all timestamp formatters.
    void setTimezoneDefault(RecordFormatterTimezone::Enum timezoneDefault);

    /// Set the record separator for this record JSON formatter to the
    /// specified `recordSeparator`.  The `recordSeparator` will be printed
    /// by each invocation of `operator()` after the formatted record.  The
    /// default is a single newline character, "\n".
    void setRecordSeparator(const bsl::string_view& recordSeparator);

    // ACCESSORS

    /// Format the specified `record` according to the current `format` and
    /// `recordSeparator` to the specified `stream`.
    void operator()(bsl::ostream& stream, const Record& record) const;

    /// Return the message format specification of this record JSON
    /// formatter.  See {`Record Format Specification`}.  Note that the syntax
    /// of this string depends on `formatSyntax()`.
    const bsl::string& format() const;

    /// Return the message format specification syntax of this record JSON
    /// formatter.  See {`Record Format Specification`}.
    SpecSyntax formatSyntax() const;

    /// Get the time zone default setting.
    RecordFormatterTimezone::Enum timezoneDefault() const;

    /// Return the record separator of this record JSON formatter.
    const bsl::string& recordSeparator() const;

                                  // Aspects

    /// @DEPRECATED: Use `get_allocator().mechanism()` instead.
    BSLS_DEPRECATE
    bslma::Allocator *allocator() const;

    /// Return the allocator used by this object to supply memory.
    allocator_type get_allocator() const;
};

// FREE OPERATORS

/// Return `true` if the specified `lhs` and `rhs` record formatters have
/// the same value, and `false` otherwise.  Two record formatters have the
/// same value if they have the same format syntax, format specification,
/// timezone default, and record separator.
bool operator==(const RecordJsonFormatter& lhs,
                const RecordJsonFormatter& rhs);

/// Return `true` if the specified `lhs` and `rhs` record formatters do not
/// have the same value, and `false` otherwise.  Two record formatters
/// differ in value if their format syntax, format specifications, timezone
/// defaults, or record separators differ.
bool operator!=(const RecordJsonFormatter& lhs,
                const RecordJsonFormatter& rhs);

// ============================================================================
//                              INLINE DEFINITIONS
// ============================================================================

                        // -------------------------
                        // class RecordJsonFormatter
                        // -------------------------

// CREATORS
inline
RecordJsonFormatter::RecordJsonFormatter(const RecordJsonFormatter& original,
                                         const allocator_type&      allocator)
: d_formatSpec(original.d_formatSpec, allocator)
, d_specSyntax(original.d_specSyntax)
, d_timezoneDefault(original.d_timezoneDefault)
, d_recordSeparator(original.d_recordSeparator, allocator)
, d_fieldFormatters(allocator)
{
    applyCurrentFormat();
}

inline
RecordJsonFormatter::RecordJsonFormatter(
    bslmf::MovableRef<RecordJsonFormatter> original) BSLS_KEYWORD_NOEXCEPT
: d_formatSpec(MoveUtil::move(MoveUtil::access(original).d_formatSpec))
, d_specSyntax(MoveUtil::access(original).d_specSyntax)
, d_timezoneDefault(
      MoveUtil::move(MoveUtil::access(original).d_timezoneDefault))
, d_recordSeparator(
      MoveUtil::move(MoveUtil::access(original).d_recordSeparator))
, d_fieldFormatters(
      MoveUtil::move(MoveUtil::access(original).d_fieldFormatters))
{
}

inline
RecordJsonFormatter::RecordJsonFormatter(
    bslmf::MovableRef<RecordJsonFormatter> original,
    const allocator_type&                  allocator)
: d_formatSpec(MoveUtil::move(MoveUtil::access(original).d_formatSpec),
               allocator)
, d_specSyntax(e_JSON)
, d_timezoneDefault(
      MoveUtil::move(MoveUtil::access(original).d_timezoneDefault))
, d_recordSeparator(
      MoveUtil::move(MoveUtil::access(original).d_recordSeparator),
      allocator)
, d_fieldFormatters(allocator)
{
    if (MoveUtil::access(original).get_allocator() == allocator) {
        d_fieldFormatters = MoveUtil::move(
                                 MoveUtil::access(original).d_fieldFormatters);
        d_specSyntax = MoveUtil::access(original).d_specSyntax;
    }
    else {
        d_specSyntax = MoveUtil::access(original).d_specSyntax;
        applyCurrentFormat();
    }
}

// MANIPULATORS
inline
RecordJsonFormatter& RecordJsonFormatter::operator=(
                                    bslmf::MovableRef<RecordJsonFormatter> rhs)
{
    if (this != &MoveUtil::access(rhs)) {
        d_recordSeparator = MoveUtil::move(
                                  MoveUtil::access(rhs).d_recordSeparator);
        d_timezoneDefault = MoveUtil::move(
                                  MoveUtil::access(rhs).d_timezoneDefault);

        if (MoveUtil::access(rhs).get_allocator() == get_allocator()) {
            releaseFieldFormatters(&d_fieldFormatters);
            d_formatSpec      = MoveUtil::move(
                                      MoveUtil::access(rhs).d_formatSpec);
            d_fieldFormatters = MoveUtil::move(
                                      MoveUtil::access(rhs).d_fieldFormatters);
            d_specSyntax = MoveUtil::access(rhs).d_specSyntax;
        }
        else {
            d_formatSpec = MoveUtil::access(rhs).d_formatSpec;
            d_specSyntax = MoveUtil::access(rhs).d_specSyntax;
            applyCurrentFormat();
        }
    }

    return *this;
}

inline
void RecordJsonFormatter::setRecordSeparator(
                                       const bsl::string_view& recordSeparator)
{
    d_recordSeparator = recordSeparator;
}

// ACCESSORS
inline
const bsl::string& RecordJsonFormatter::format() const
{
    return d_formatSpec;
}

inline
RecordJsonFormatter::SpecSyntax RecordJsonFormatter::formatSyntax() const
{
    return d_specSyntax;
}

inline
RecordFormatterTimezone::Enum
RecordJsonFormatter::timezoneDefault() const
{
    return d_timezoneDefault;
}

inline
const bsl::string& RecordJsonFormatter::recordSeparator() const
{
    return d_recordSeparator;
}

                                  // Aspects

inline
bslma::Allocator *RecordJsonFormatter::allocator() const
{
    return d_formatSpec.get_allocator().mechanism();
}

inline
RecordJsonFormatter::allocator_type
RecordJsonFormatter::get_allocator() const
{
    return d_formatSpec.get_allocator();
}

// PRIVATE MANIPULATORS
inline
void RecordJsonFormatter::applyCurrentFormat()
{
    if (d_specSyntax == e_SIMPLIFIED) {
        const int rc = setSimplifiedFormat(d_formatSpec);
        BSLS_ASSERT(0 == rc);  (void) rc;
    }
    else {
        const int rc = setJsonFormat(d_formatSpec);
        BSLS_ASSERT(0 == rc);  (void) rc;
    }
}

}  // close package namespace

// FREE OPERATORS
inline
bool ball::operator==(const RecordJsonFormatter& lhs,
                      const RecordJsonFormatter& rhs)
{
    return lhs.formatSyntax() == rhs.formatSyntax()       &&
           lhs.format()          == rhs.format()          &&
           lhs.timezoneDefault() == rhs.timezoneDefault() &&
           lhs.recordSeparator() == rhs.recordSeparator();
}

inline
bool ball::operator!=(const RecordJsonFormatter& lhs,
                      const RecordJsonFormatter& rhs)
{
    return !(lhs == rhs);
}

}  // close enterprise namespace

#endif  // INCLUDED_BALL_RECORDJSONFORMATTER

// ----------------------------------------------------------------------------
// Copyright 2020 Bloomberg Finance L.P.
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
