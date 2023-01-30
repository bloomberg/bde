// ball_recordstringformatter.h                                       -*-C++-*-
#ifndef INCLUDED_BALL_RECORDSTRINGFORMATTER
#define INCLUDED_BALL_RECORDSTRINGFORMATTER

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a record formatter that uses a 'printf'-style format spec.
//
//@CLASSES:
//  ball::RecordStringFormatter: 'printf'-style formatter for log records
//
//@SEE_ALSO: ball_record, ball_recordattributes
//
//@DESCRIPTION: This component provides a value-semantic function-object class,
// 'ball::RecordStringFormatter', that is used to format log records according
// to a 'printf'-style format specification (see "Record Format Specification"
// below).  A format specification and a timestamp offset (in the form of a
// 'bdlt::DatetimeInterval') are optionally supplied upon construction of a
// 'ball::RecordStringFormatter' object (or simply "record formatter").  If a
// format specification is not supplied, a default one (defined below) is used.
// If a timestamp offset is not supplied, it defaults to 0.  Both the format
// specification and timestamp offset of a record formatter can be modified
// following construction.
//
// An overloaded 'operator()' is defined for 'ball::RecordStringFormatter' that
// takes a 'ball::Record' and an 'bsl::ostream' as arguments.  This method
// formats the given record according to the format specification of the record
// formatter and outputs the result to the given stream.  Additionally, each
// timestamp indicated in the format specification is biased by the timestamp
// offset of the record formatter prior to outputting it to the stream.  This
// facilitates the logging of records in local time, if desired, in the event
// that the timestamp attribute of records are in UTC.
//
///Record Format Specification
///---------------------------
// The following table lists the 'printf'-style ('%'-prefixed) conversion
// specifications, including their expansions, that are recognized within the
// format specification of a record formatter:
//..
//  %d - timestamp in 'DDMonYYYY_HH:MM:SS.mmm' format (27AUG2007_16:09:46.161)
//  %D - timestamp in 'DDMonYYYY_HH:MM:SS.mmmuuu' format
//                                                  (27AUG2007_16:09:46.161324)
//  %dtz - timestamp in 'DDMonYYYY_HH:MM:SS.mmm(+|-)HHMM' format
//                                                (27AUG2007_16:09:46.161+0000)
//  %Dtz - timestamp in 'DDMonYYYY_HH:MM:SS.mmmuuu(+|-)HHMM' format
//                                             (27AUG2007_16:09:46.161324+0000)
//  %i - timestamp in ISO 8601 format (without the millisecond or microsecond
//                                     fields)
//  %I - timestamp in ISO 8601 format (*with* the millisecond field)
//  %O - timestamp in ISO 8601 format (*with* the millisecond and microsecond
//                                     fields)
//  %p - process Id
//  %t - thread Id
//  %T - thread Id in hex
//  %s - severity
//  %f - filename (as provided by '__FILE__')
//  %F - filename abbreviated (basename of '__FILE__' only)
//  %l - line number (as provided by '__LINE__')
//  %c - category name
//  %m - log message
//  %x - log message with non-printable characters in hex
//  %X - log message entirely in hex
//  %u - user-defined fields
//  %% - single '%' character
//  %A - log all the attributes of the record
//  %a - log only those attributes not already logged by the %a[name] or
//       %av[name] specifier(s)
//  %a[name] - log an attribute with the specified 'name' as "name=value",
//       log nothing if the attribute with the specified 'name' is not found
//  %av[name] - log only the value of an attribute with the specified 'name',
//       log nothing if the attribute with the specified 'name' is not found
//..
// (Note that '%F' is used to indicate the shortened form of '__FILE__' rather
// than '%f' because '%f' was given its current interpretation in an earlier
// version of this component.)
//
// In addition, the following '\'-escape sequences are interpolated in the
// formatted output as indicated when they occur in the format specification:
//..
//  \n - newline character
//  \t - tab character
//  \\ - single '\' character
//..
// Any other text included in the format specification of the record formatter
// is output verbatim.
//
// When not supplied at construction, the default format specification of a
// record formatter is:
//..
//  "\n%d %p:%t %s %f:%l %c %m %u\n"
//..
// A default-formatted record having no user-defined fields will have the
// following appearance:
//..
// 27AUG2007_16:09:46.161 2040:1 WARN subdir/process.cpp:542 FOO.BAR.BAZ <text>
//..
//
///Log Record Attributes Rendering Details
///---------------------------------------
// Log record attributes are rendered as space-separated 'name="value"' pairs
// (for example: mylibrary.username="mbloomberg").  Note that attribute names
// are *not* quoted, whereas attribute values, if they are strings, are
// *always* quoted.
//
///Usage
///-----
// The following snippets of code illustrate how to use an instance of
// 'ball::RecordStringFormatter' to format log records.
//
// First we instantiate a record formatter with an explicit format
// specification (but we accept the default timestamp offset since it will not
// be used in this example):
//..
//  ball::RecordStringFormatter formatter("\n%t: %m\n");
//..
// The chosen format specification indicates that, when a record is formatted
// using 'formatter', the thread Id attribute of the record will be output
// followed by the message attribute of the record.
//
// Next we create a default 'ball::Record' and set the thread Id and message
// attributes of the record to dummy values:
//..
//  ball::Record record;
//
//  record.fixedFields().setThreadID(6);
//  record.fixedFields().setMessage("Hello, World!");
//..
// The following "invocation" of the 'formatter' function object formats
// 'record' to 'bsl::cout' according to the format specification supplied at
// construction:
//..
//  formatter(bsl::cout, record);
//..
// As a result of this call, the following is printed to 'stdout':
//..
//  6: Hello, World!
//..

#include <balscm_version.h>

#include <bdlt_datetimeinterval.h>

#include <bslma_allocator.h>
#include <bslma_usesbslmaallocator.h>

#include <bslmf_nestedtraitdeclaration.h>

#include <bsl_functional.h>
#include <bsl_iosfwd.h>
#include <bsl_string.h>
#include <bsl_set.h>
#include <bsl_vector.h>

#ifndef BDE_DONT_ALLOW_TRANSITIVE_INCLUDES
#include <bslalg_typetraits.h>
#endif // BDE_DONT_ALLOW_TRANSITIVE_INCLUDES

namespace BloombergLP {
namespace ball {

class Record;

                        // ===========================
                        // class RecordStringFormatter
                        // ===========================

class RecordStringFormatter {
    // This class provides a value-semantic log record formatter that holds a
    // 'printf'-style format specification and a timestamp offset.  The
    // overloaded 'operator()' provided by the class formats a given record
    // according to the format specification and outputs the formatted result
    // to a given stream.  The timestamp offset of the record formatter is
    // added to each timestamp that is output to the stream.

    // PRIVATE TYPES
    typedef bsl::function<void(bsl::string *, const Record&)>
                                              FieldStringFormatter;
        // 'FieldStringFormatter' is an alias for a functional object that
        // render fields provided by a 'ball::Record' to a string.

    typedef bsl::vector<FieldStringFormatter> FieldStringFormatters;
        // 'FieldStringFormatters' is an alias for a vector of the
        // 'FieldStringFormatter' objects.

    typedef bsl::set<bsl::string_view>        SkipAttributes;
        // 'SkipAttributes' is an alias for a set of keys of attributes that
        // should not be printed as part of a '%a' format specifier.

  public:
    // TYPES
    typedef bsl::allocator<char>  allocator_type;

  private:
    // DATA
    bsl::string              d_formatSpec;       // 'printf'-style format spec.
    FieldStringFormatters    d_fieldFormatters;  // field formatter collection
    SkipAttributes           d_skipAttributes;   // set of skipped attributes
    bdlt::DatetimeInterval   d_timestampOffset;  // offset added to timestamps

    // PRIVATE MANIPULATORS
    void parseFormatSpecification();
        // Parse the format specification.

  public:
    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(RecordStringFormatter,
                                   bslma::UsesBslmaAllocator);


    // PUBLIC CONSTANTS
    static const char *k_DEFAULT_FORMAT;
        // The default log format specification used by
        // 'RecordStringFormatter'.

    static const char *k_BASIC_ATTRIBUTE_FORMAT;
        // A simple standard record format that renders 'ball::Attribute'
        // values in the formatted output.  Note that this format is
        // recommended over the default format, 'k_DEFAULT_FORMAT', for most
        // applications (the default format is currently maintained for
        // backwards compatibility).

    // CREATORS
    explicit RecordStringFormatter(
                          const allocator_type&  allocator = allocator_type());
    explicit RecordStringFormatter(
                          bslma::Allocator      *basicallocator);
        // Create a record formatter having a default format specification and
        // a timestamp offset of 0.  Optionally specify an 'allocator' (e.g.,
        // the address of a 'bslma::Allocator' object) to supply memory.  If
        // 'basicAllocator' is not supplied or 0, the currently installed
        // default allocator is used.  The default format specification is:
        //..
        //  "\n%d %p:%t %s %f:%l %c %m %u\n"
        //..

    explicit RecordStringFormatter(
                          const char            *format,
                          const allocator_type&  allocator = allocator_type());
    RecordStringFormatter(const char            *format,
                          bslma::Allocator      *basicAllocator);
        // Create a record formatter having the specified 'format'
        // specification and a timestamp offset of 0.  Optionally specify an
        // 'allocator' (e.g., the address of a 'bslma::Allocator' object) to
        // supply memory.  If 'basicAllocator' is not supplied or 0, the
        // currently installed default allocator is used.

    explicit RecordStringFormatter(
                   const bdlt::DatetimeInterval& offset,
                   const allocator_type&         allocator = allocator_type());
        // Create a record formatter having a default format specification and
        // the specified timestamp 'offset'.  Optionally specify an 'allocator'
        // (e.g., the address of a 'bslma::Allocator' object) to supply memory;
        // otherwise, the default allocator is used.  The default format
        // specification is:
        //..
        //  "\n%d %p:%t %s %f:%l %c %m %u\n"
        //..
        //
        // !DEPRECATED!: Use a constructor taking 'publishInLocalTime' instead.

    explicit RecordStringFormatter(
                           bool                  publishInLocalTime,
                           const allocator_type& allocator = allocator_type());
        // Create a record formatter having a default format specification, and
        // if the specified 'publishInLocalTime' flag is 'true', format the
        // timestamp of each logged record in the local time of the current
        // task, and format the timestamp in UTC otherwise.  Optionally specify
        // an 'allocator' (e.g., the address of a 'bslma::Allocator' object) to
        // supply memory; otherwise, the default allocator is used.  The
        // default format specification is:
        //..
        //  "\n%d %p:%t %s %f:%l %c %m %u\n"
        //..
        // Note that local time offsets are calculated for the timestamp of
        // each formatted record and so track transitions into and out of
        // Daylight Saving Time.

    RecordStringFormatter(
                  const char                    *format,
                  const bdlt::DatetimeInterval&  offset,
                  const allocator_type&          allocator = allocator_type());
        // Create a record formatter having the specified 'format'
        // specification and the specified timestamp 'offset'.  Optionally
        // specify an 'allocator' (e.g., the address of a 'bslma::Allocator'
        // object) to supply memory; otherwise, the default allocator is used.
        //
        // !DEPRECATED!: Use a constructor taking 'publishInLocalTime' instead.

    RecordStringFormatter(const char            *format,
                          bool                   publishInLocalTime,
                          const allocator_type&  allocator = allocator_type());
        // Create a record formatter having the specified 'format'
        // specification, and if the specified 'publishInLocalTime' flag is
        // 'true', format the timestamp of each log in the local time of the
        // current task, and format the timestamp in UTC otherwise.  Optionally
        // specify an 'allocator' (e.g., the address of a 'bslma::Allocator'
        // object) to supply memory; otherwise, the default allocator is used.
        // Note that local time offsets are calculated for the timestamp of
        // each formatted record and so track transitions into and out of
        // Daylight Saving Time.

    RecordStringFormatter(
                    const RecordStringFormatter& original,
                    const allocator_type&        allocator = allocator_type());
        // Create a record formatter initialized to the value of the specified
        // 'original' record formatter.  Optionally specify an 'allocator'
        // (e.g., the address of a 'bslma::Allocator' object) to supply memory;
        // otherwise, the default allocator is used.

    //! ~RecordStringFormatter() = default;
        // Destroy this object.

    // MANIPULATORS
    RecordStringFormatter& operator=(const RecordStringFormatter& rhs);
        // Assign to this record formatter the value of the specified 'rhs'
        // record formatter.

    void disablePublishInLocalTime();
        // Disable adjust of the timestamp attribute of to the current local
        // time by this file observer.  This method has no effect if adjustment
        // to the current local time is not enabled.

    void enablePublishInLocalTime();
        // Enable adjustment of the timestamp attribute to the current local
        // time.  This method has no effect if adjustment to the current local
        // time is already enabled.

    void setFormat(const char *format);
        // Set the format specification of this record formatter to the
        // specified 'format'.

    void setTimestampOffset(const bdlt::DatetimeInterval& offset);
        // Set the timestamp offset of this record formatter to the specified
        // 'offset'.
        //
        // !DEPRECATED!: Use 'enablePublishInLocalTime' instead.

    // ACCESSORS
    void operator()(bsl::ostream& stream, const Record& record) const;
        // Format the specified 'record' according to the format specification
        // of this record formatter and output the result to the specified
        // 'stream'.  The timestamp offset of this record formatter is added to
        // each timestamp that is output to 'stream'.

    const char *format() const;
        // Return the format specification of this record formatter.

    bool isPublishInLocalTimeEnabled() const;
        // Return 'true' if this formatter adjusts the timestamp attribute to
        // the current local time, and 'false' otherwise.

    const bdlt::DatetimeInterval& timestampOffset() const;
        // Return a reference to the non-modifiable timestamp offset of this
        // record formatter.
        //
        // !DEPRECATED!: Use 'isPublishInLocalTimeEnabled' instead.

                                  // Aspects

    allocator_type get_allocator() const;
        // Return the allocator used by this object to supply memory.  Note
        // that if no allocator was supplied at construction the default
        // allocator in effect at construction is used.

};

// FREE OPERATORS
bool operator==(const RecordStringFormatter& lhs,
                const RecordStringFormatter& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' record formatters have
    // the same value, and 'false' otherwise.  Two record formatters have the
    // same value if they have the same format specification and the same
    // timestamp offset.

bool operator!=(const RecordStringFormatter& lhs,
                const RecordStringFormatter& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' record formatters do not
    // have the same value, and 'false' otherwise.  Two record formatters
    // differ in value if their format specifications differ or their timestamp
    // offsets differ.

bsl::ostream& operator<<(bsl::ostream&                output,
                         const RecordStringFormatter& rhs);
    // Write the specified 'rhs' record formatter to the specified 'output'
    // stream in some reasonable (single-line) format and return a reference
    // to the modifiable 'stream'.

// ============================================================================
//                              INLINE DEFINITIONS
// ============================================================================

                        // ---------------------------
                        // class RecordStringFormatter
                        // ---------------------------

// MANIPULATORS
inline
void RecordStringFormatter::setFormat(const char *format)
{
    d_formatSpec = format;
    parseFormatSpecification();
}

inline
void RecordStringFormatter::setTimestampOffset(
                                          const bdlt::DatetimeInterval& offset)
{
    d_timestampOffset = offset;
}

// ACCESSORS
inline
const char *RecordStringFormatter::format() const
{
    return d_formatSpec.c_str();
}

inline
const bdlt::DatetimeInterval&
RecordStringFormatter::timestampOffset() const
{
    return d_timestampOffset;
}

                                  // Aspects

inline
RecordStringFormatter::allocator_type
RecordStringFormatter::get_allocator() const
{
    return d_formatSpec.get_allocator();
}

}  // close package namespace

// FREE OPERATORS
inline
bool ball::operator!=(const RecordStringFormatter& lhs,
                      const RecordStringFormatter& rhs)
{
    return !(lhs == rhs);
}

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
