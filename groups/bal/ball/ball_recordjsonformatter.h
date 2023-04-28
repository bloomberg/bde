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
// 'ball::RecorJsonFormatter', that formats a log record as JSON text elements
// according to a format specification (see {'Record Format Specification'}).
// 'ball::RecordJsonFormatter' is designed to match the function signature
// expected by many concrete 'ball::Observer' implementations that publish log
// records (for example, see 'ball::FileObserver2::setLogFileFunctor').
//
// NOTE: 'ball::RecorJsonFormatter' renders individual log records as JSON,
// but, for example, a resulting log file would contain a sequence of JSON
// strings, which is not itself valid JSON text.
//
///Record Format Specification
///---------------------------
// A format specification is, itself, a JSON array, supplied to a
// 'RecordJsonFormatter' object by the 'setFormat' function.  If no format is
// specified, the default format is used.  Each array element specifies the
// format of a log record field or a user-defined attribute.  Here is a simple
// example:
//..
//  [{"timestamp":{"format":"iso8601"}}, "pid", "tid", "severity", "message"]
//..
// would a result in a log record like:
//..
// { "timestamp": "2020-08-28T14:43:50.375Z",
//   "pid": 2313,
//   "tid": 12349388604,
//   "severity": "INFO",
//   "message": "Hello, world!"
//  }
//..
// Again, the format specification is a JSON array, each element of which can
// be one of the following:
//
//: o A string containing the name of the fixed record field or the name of the
//:   user-defined attribute, in which case the field or attribute will be
//:   published in the default format.  For example, '[timestamp]', would
//:   display the timestamp as: '{"timestamp": "2020-08-28T14:43:50.375Z"}'.
//:
//: o A JSON object having the name of the fixed field or user-defined
//:   attribute and a set of key-values pairs used to customize the output
//:   format.  For example,
//:   '[{"timestamp": {"name": "My Time", "format": "bdePrint"}}]', would
//:   display the timestamp as: '{"My Time": "28AUG2020_14:43:50.375"}'.
//
///Field Format Specification
/// - - - - - - - - - - - - -
// The following table lists the predefined string values for each fixed field
// and user-defined attributes in the log record:
//..
//     Tag                 Description                   Example
// --------------    -------------------------        -------------
// "timestamp"       creation date and time           ["timestamp"]
// "pid"             process id of creator            ["pid"]
// "tid"             thread id of creator             ["tid"]
// "file"            file where created  (__FILE__)   ["file"]
// "line"            line number in file (__LINE__)   ["line"]
// "category"        category of logged record        ["category"]
// "severity"        severity of logged record        ["severity"]
// "message"         log message text                 ["message"]
// "attributes"      all user-defined attributes      ["attributes"]
// <attribute name>  specific user-defined attribute  ["bas.uuid"]
//..
// The output format of each field can be customized by replacing a string
// value in the JSON array with a JSON object having the same name and a set of
// key-value pairs (attributes).
//
///Verifying the Format Specification for 'setFormat'
///- - - - - - - - - - - - - - - - - - - - - -- - - -
// The sections that follow describe the set of fields that can be provided in
// the format specification supplied to 'setFormat'.
// 'RecordJsonFormatter::setFormat' will ignore fields in the provided format
// specification that are unknown, but will report an error if a known field
// contains a property that is not supported.  For example: a format
// specification '["pid", { "timestamp" : {"unknown field!": "value"} }] will
// be accepted, but '["pid", {"timestamp": {"format": "unknown format" }}]'
// will produce an error.
//
// Each key-value pair of a JSON object that specifies a format of an output of
// a fixed record field or a user-defined attribute has the following
// constrains:
//
//: o The key is a string of known value listed in the column "Key" in the
//:   tables below.  Any string that does not match the listed values is
//:   ignored.
//:
//: o The value is a string of known value (except for the "name" key) in the
//:   column "Value Constraint" in the tables below.  If the value does not
//:   match the string values specified in the tables, the format specification
//:   is considered to be inconsistent with the expected schema, and is
//:   rejected by the 'RecordJsonFormatter::setFormat' method.
//
///The "timestamp" field format
///-  -  -  -  -  -  -  -  -  -
// The format attributes of the "timestamp" object are given in the following
// table:
//..
//                                              Value             Default
//             Key              Description     Constraint        Value
//  ------------------------  ----------------  -----------     ------------
//  "name"                    name by which     JSON string     "timestamp"
//                            "timestamp" will
//                            be published
//
//  "format"                  datetime format   "iso8601",     "iso8601"
//                                              "bdePrint"
//                                              (*Note*)
//
//  "fractionalSecPrecision"  second precision  "none",         "microseconds"
//                                              "milliseconds",
//                                              "microseconds"
//
//  "timeZone"                time zone         "utc",          "utc"
//                                              "local"
//..
// *Note*: The default "bdePrint" format denotes the following datetime format:
//..
//   DDMonYYYY_HH:MM:SS.mmm
//..
// For example, the following record format specification:
//..
//   [ { "timestamp": { "name": "Time",
//                      "fractionalSecPrecision": "microseconds",
//                      "timeZone": "local" } }
//   ]
//..
// would a result in a log record like:
//..
//   { "Time": "28AUG2020_17:43:50.375345" }
//..
//
///The "pid" (process Id) field format
/// -  -  -  -  -  -  -  -  -  -  -  -
// The format attributes of the process Id field are given in the following
// table:
//..
//                                                   Value        Default
//    Key                 Description                Constraint   Value
//  ------    -------------------------------------  -----------  -------
//  "name"    name by which "pid" will be published  JSON string  "pid"
//..
// For example, the following record format specification:
//..
//   [ { "pid": { "name": "Process Id" } } ]
//..
// would a result in a log record like:
//..
//   { "Process Id": 2313 }
//..
//
///The "tid" (thread Id) field format
///-  -  -  -  -  -  -  -  -  -  -  -
// The format attributes of the thread Id field are given in the following
// table:
//..
//                                                     Value        Default
//    Key                   Description                Constraint   Value
//  --------    -------------------------------------  -----------  ---------
//  "name"      name by which "tid" will be published  JSON string  "tid"
//
//  "format"    output format                          "decimal",   "decimal"
//                                                     "hex"
//..
// For example, the following record format specification:
//..
//   [ { "tid": { "name": "Thread Id",
//                "format": "hex" } }
//   ]
//..
// would a result in a log record like:
//..
//   { "Thread Id": 0xA7654EFF3540 }
//..
//
///The "file" field format
/// -  -  -  -  -  -  -  -  -  -  -  -
// The format attributes of the "file" field are given in the following
// table:
//..
//                                                                Default
//    Key          Description             Value Constraint       Value
//  ------    --------------------  ----------------------------- -------
//  "name"    name by which "file"  JSON string                   "file"
//            will be published
//
//  "path"    file path             "full" (__FILE__),            "full"
//                                  "file" (basename of __FILE__)
//..
// For example, the following record format specification:
//..
//   [ { "file": { "name": "File",
//                 "path": "file" } }
//   ]
//..
// would a result in a log record like:
//..
//   { "File": "test.cpp" }
//..
//
///The "line" field format
/// -  -  -  -  -  -  -  -
// The format attributes of the "line" field are given in the following
// table:
//..
//                                                    Value        Default
//    Key                Description                  Constraint   Value
//  ------   ---------------------------------------  -----------  -------
//  "name"   name by which "line" will be published   JSON string  "line"
//..
// For example, the following record format specification:
//..
//   [ { "line": { "name": "Line" } } ]
//..
// would a result in a log record like:
//..
//   { "Line": 512 }
//..
//
///The "category" field format
///  -  -  -  -  -  -  -  -  -
// The format attributes of the "category" field are given in the following
// table:
//..
//                                                      Value         Default
//    Key                 Description                   Constraint    Value
//  ------  ------------------------------------------  -----------  ----------
//  "name"  name by which "category" will be published  JSON string  "category"
//..
// For example, the following record format specification:
//..
//   [ { "category": { "name": "Category" } } ]
//..
// would a result in a log record like:
//..
//   { "category": "Server" }
//..
//
///The "severity" field format
///  -  -  -  -  -  -  -  -  -
// The format attributes of the "severity" field are given in the following
// table:
//..
//                                                      Value         Default
//    Key                  Description                  Constraint    Value
//  ------  ------------------------------------------  -----------  ----------
//  "name"  name by which "severity" will be published  JSON string  "severity"
//..
// For example, the following record format specification:
//..
//   [ { "severity": { "name": "severity" } } ]
//..
// would a result in a log record like:
//..
//   { "Severity": "ERROR" }
//..
//
///The "message" field format
///  -  -  -  -  -  -  -  -  -
// A message is a JSON string which is a sequence of zero or more Unicode
// characters, wrapped in double quotes, using backslash escapes: (\", \\, \/,
// \b, \f, \n, \r, \t, \u{4 hex digits}).
//
// The format attributes of the "message" field are given in the following
// table:
//..
//                                                     Value         Default
//    Key                 Description                  Constraint    Value
//  ------  -----------------------------------------  -----------  ---------
//  "name"  name by which "message" will be published  JSON string  "message"
//..
// For example, the following record format specification:
//..
//   [ { "message": { "name": "msg" } } ]
//..
// would a result in a log record like:
//..
//   { "msg": "Log message" }
//..
//
///The "attributes" format
///-  -  -  -  -  -  -  -  -  -  -
// The "attributes" JSON object has no attributes. For example, the following
// record format specification:
//..
//   [ "attributes" ]
//..
// would (assuming their are two attributes "bas.requestid" and
// "mylib.security") result in a log record like:
//..
//   { "bas.requestid": 12345, "mylib.security": "My Security" }
//..
//
///A user-defined attribute format
///-  -  -  -  -  -  -  -  -  -  -
// Each user-defined attribute has a single "name" attribute that can be used
// to rename the user-defined attribute:
//..
//                                         Value        Default
//    Key         Description              Constraint   Value
//  ------   ----------------------------  -----------  -------
//  "name"   name by which a user-defined  JSON string   none
//           attribute will be published
//..
// For example, the following record format specification:
//..
//   [ { "bas.uuid": { "name": "BAS.UUID" } } ]
//..
// would a result in a log record like:
//..
//   { "BAS.UUID": 3593 }
//..
//
///The Record Separator
///--------------------
// The record separator is a string that is printed after each formatted
// record.  The default value of the record separator is a single newline, but
// it can be set to any string of the user's choice using the
// 'RecordJsonFormatter::setRecordSeparator' function.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example: Format log records as JSON and render them to 'stdout'
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose an application needs to format log records as JSON and output them
// to 'stdout'.
//
// First we instantiate a JSON record formatter:
//..
//   ball::RecordJsonFormatter formatter;
//..
// Next we set a format specification to the newly created 'formatter':
//..
//  int rc = formatter.setFormat("[\"tid\",\"message\"]");
//  assert(0 == rc);
//..
// The chosen format specification indicates that, when a record is formatted
// using 'formatter', the thread Id attribute of the record will be output
// followed by the message attribute of the record.
//
// Then we create a default 'ball::Record' and set the thread Id and message
// attributes of the record to dummy values:
//..
//  ball::Record record;
//
//  record.fixedFields().setThreadID(6);
//  record.fixedFields().setMessage("Hello, World!");
//..
// Next, invocation of the 'formatter' function object to format 'record' to
// 'bsl::cout':
//..
//  formatter(bsl::cout, record);
//..
// yields this output, which is terminated by a single newline:
//..
//  {"tid":6,"message":"Hello, World!"}
//..
// Finally, we change the record separator and format the same record again:
//..
//  formatter.setFormat("\n\n");
//  formatter(bsl::cout, record);
//..
// The record is printed in the same format, but now terminated by two
// newlines:
//..
//  {"tid":6,"message":"Hello, World!"}
//
//..

#include <balscm_version.h>

#include <bslma_allocator.h>
#include <bslma_stdallocator.h>
#include <bslma_usesbslmaallocator.h>

#include <bslmf_isbitwisemoveable.h>
#include <bslmf_movableref.h>
#include <bslmf_nestedtraitdeclaration.h>

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

class RecordJsonFormatter {
    // This class provides a function object that formats a log record as JSON
    // text elements and renders them to an output stream.  The overloaded
    // 'operator()' provided by the class formats log record according to JSON
    // message format specification supplied at construction (either by the
    // 'setFormat' manipulator or by default) and outputs the result to the
    // stream.  This functor type is designed to match the function signature
    // expected by many concrete 'ball::Observer' implementations that publish
    // log records (for example, see 'ball::FileObserver2::setLogFileFunctor').

    // PRIVATE TYPES
    typedef bslmf::MovableRefUtil  MoveUtil;
        // 'MoveUtil' is an alias for 'bslmf::MovableRefUtil'.

  public:
    // TYPES
    typedef bsl::vector<RecordJsonFormatter_FieldFormatter*> FieldFormatters;
        // 'FieldFormatters' is an alias for a vector of the
        // 'RecordJsonFormatter_FieldFormatter' objects, each of which is
        // initialized from the format specification and responsible for
        // rendering one field of a 'ball::Record' to the output JSON stream.

    typedef bsl::allocator<char> allocator_type;

  private:
    // DATA
    bsl::string     d_formatSpec;       // format specification (in JSON)

    bsl::string     d_recordSeparator;  // string to print after each record

    FieldFormatters d_fieldFormatters;  // field formatters configured
                                        // according to the format
                                        // specification

    // CLASS METHODS
    static
    void releaseFieldFormatters(FieldFormatters *formatters);
        // Destroy the field formatters contained in the specified
        // 'formatters'.

  public:
    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(RecordJsonFormatter,
                                   bslma::UsesBslmaAllocator);

    // CREATORS
    explicit RecordJsonFormatter(
                           const allocator_type& allocator = allocator_type());
        // Create a record JSON formatter having a default format specification
        // and record separator.  Optionally specify an 'allocator' (e.g., the
        // address of a 'bslma::Allocator' object) to supply memory; otherwise,
        // the allocator is used.  The default format specification is:
        //..
        //  ["timestamp", "processId", "threadId", "severity", "file", "line",
        //   "category",  "message", "attributes"]
        //..
        // The default record separator is "\n".

    RecordJsonFormatter(
                      const RecordJsonFormatter& original,
                      const allocator_type&      allocator = allocator_type());
        // Create a record JSON formatter initialized to the value of the
        // specified 'original' record formatter.  Optionally specify an
        // 'allocator' (e.g., the address of a 'bslma::Allocator' object) to
        // supply memory; otherwise, the default allocator is used.

    RecordJsonFormatter(bslmf::MovableRef<RecordJsonFormatter> original)
                                                         BSLS_KEYWORD_NOEXCEPT;
        // Create a record JSON formatter having the same format specification
        // and record separator as in the specified 'original' formatter, and
        // adopting all outstanding memory allocations and the allocator
        // associated with the 'original' formatter.  'original' is left in a
        // valid but unspecified state.

    RecordJsonFormatter(bslmf::MovableRef<RecordJsonFormatter> original,
                        const allocator_type&                  allocator);
        // Create a record JSON formatter, having the same format specification
        // and record separator as in the specified 'original' formatter.  The
        // format specification of 'original' is moved to the new object, and
        // all outstanding memory allocations and the specified 'allocator' are
        // adopted if 'allocator == original.allocator()'.  'original' is left
        // in a valid but unspecified state.

    ~RecordJsonFormatter();
        // Destroy this object.

    // MANIPULATORS
    RecordJsonFormatter& operator=(const RecordJsonFormatter& rhs);
        // Assign to this object the value of the specified 'rhs' object, and
        // return a reference providing modifiable access to this object.

    RecordJsonFormatter& operator=(bslmf::MovableRef<RecordJsonFormatter> rhs);
        // Assign to this object the format specification and record separator
        // of the specified 'rhs' object, and return a reference providing
        // modifiable access to this object.  The format specification and
        // record separator of 'rhs' are moved to this object, and all
        // outstanding memory allocations and the allocator associated with
        // 'rhs' are adopted if 'allocator() == rhs.allocator()'.  'rhs' is
        // left in a valid but unspecified state.

    int setFormat(const bsl::string_view& format);
        // Set the message format specification (see
        // {'Record Format Specification'}) of this record JSON formatter to
        // the specified 'format'.  Return 0 on success, and a non-zero value
        // otherwise (if 'format' is not valid JSON *or* not a JSON conforming
        // to the expected schema).

    void setRecordSeparator(const bsl::string_view& recordSeparator);
        // Set the record separator for this record JSON formatter to the
        // specified 'recordSeparator'.  The 'recordSeparator' will be printed
        // by each invocation of 'operator()' after the formatted record.  The
        // default is a single newline character, "\n".

    // ACCESSORS
    void operator()(bsl::ostream& stream, const Record& record) const;
        // Format the specified 'record' according to the current 'format' and
        // 'recordSeparator' to the specified 'stream'.

    const bsl::string& format() const;
        // Return the message format specification of this record JSON
        // formatter.  See {'Record Format Specification'}.

    const bsl::string& recordSeparator() const;
        // Return the record separator of this record JSON formatter.

                                  // Aspects

    const allocator_type& allocator() const;
        // Return the allocator used by this object to supply memory.

};

// FREE OPERATORS
bool operator==(const RecordJsonFormatter& lhs,
                const RecordJsonFormatter& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' record formatters have
    // the same value, and 'false' otherwise.  Two record formatters have the
    // same value if they have the same format specification and record
    // separator.

bool operator!=(const RecordJsonFormatter& lhs,
                const RecordJsonFormatter& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' record formatters do not
    // have the same value, and 'false' otherwise.  Two record formatters
    // differ in value if their format specifications or record separators
    // differ.

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
, d_recordSeparator(original.d_recordSeparator, allocator)
, d_fieldFormatters(allocator)
{
    int rc = setFormat(d_formatSpec);
    (void) rc;
    BSLS_ASSERT(0 == rc);
}

inline
RecordJsonFormatter::RecordJsonFormatter(
    bslmf::MovableRef<RecordJsonFormatter> original) BSLS_KEYWORD_NOEXCEPT
: d_formatSpec(MoveUtil::move(MoveUtil::access(original).d_formatSpec)),
  d_recordSeparator(
      MoveUtil::move(MoveUtil::access(original).d_recordSeparator)),
  d_fieldFormatters(
      MoveUtil::move(MoveUtil::access(original).d_fieldFormatters))
{
}

inline
RecordJsonFormatter::RecordJsonFormatter(
    bslmf::MovableRef<RecordJsonFormatter> original,
    const allocator_type&                  allocator)
: d_formatSpec(MoveUtil::move(MoveUtil::access(original).d_formatSpec),
               allocator)
, d_recordSeparator(
      MoveUtil::move(MoveUtil::access(original).d_recordSeparator),
      allocator)
, d_fieldFormatters(allocator)
{
    if (MoveUtil::access(original).allocator() == allocator) {
        d_fieldFormatters = MoveUtil::move(
                                 MoveUtil::access(original).d_fieldFormatters);
    }
    else {
        int rc = setFormat(d_formatSpec);
        (void) rc;
        BSLS_ASSERT(0 == rc);
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
        if (MoveUtil::access(rhs).allocator() == allocator()) {
            releaseFieldFormatters(&d_fieldFormatters);
            d_formatSpec      = MoveUtil::move(
                                      MoveUtil::access(rhs).d_formatSpec);
            d_fieldFormatters = MoveUtil::move(
                                      MoveUtil::access(rhs).d_fieldFormatters);
        }
        else {
            int rc = setFormat(MoveUtil::access(rhs).d_formatSpec);
            (void) rc;
            BSLS_ASSERT(0 == rc);
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
const bsl::string& RecordJsonFormatter::recordSeparator() const
{
    return d_recordSeparator;
}

                                  // Aspects

inline
const RecordJsonFormatter::allocator_type&
RecordJsonFormatter::allocator() const
{
    return d_formatSpec.allocator();
}


}  // close package namespace

// FREE OPERATORS
inline
bool ball::operator==(const RecordJsonFormatter& lhs,
                      const RecordJsonFormatter& rhs)
{
    return lhs.format() == rhs.format() &&
           lhs.recordSeparator() == rhs.recordSeparator();
}

inline
bool ball::operator!=(const RecordJsonFormatter& lhs,
                      const RecordJsonFormatter& rhs)
{
    return !(lhs == rhs);
}

}  // close enterprise namespace

#endif

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
