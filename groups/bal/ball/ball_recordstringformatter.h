// ball_recordstringformatter.h                                       -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#ifndef INCLUDED_BALL_RECORDSTRINGFORMATTER
#define INCLUDED_BALL_RECORDSTRINGFORMATTER

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a record formatter that uses a 'printf'-style format spec.
//
//@CLASSES:
//   ball::RecordStringFormatter: 'printf'-style formatter for log records
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
//  %i - timestamp in ISO 8601 format (without the millisecond field)
//  %I - timestamp in ISO 8601 format (*with* the millisecond field)
//  %p - process Id
//  %t - thread Id
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

#ifndef INCLUDED_BALSCM_VERSION
#include <balscm_version.h>
#endif

#ifndef INCLUDED_BDLT_DATETIMEINTERVAL
#include <bdlt_datetimeinterval.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITS
#include <bslalg_typetraits.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITUSESBSLMAALLOCATOR
#include <bslalg_typetraitusesbslmaallocator.h>
#endif

#ifndef INCLUDED_BSLMA_ALLOCATOR
#include <bslma_allocator.h>
#endif

#ifndef INCLUDED_BSL_IOSFWD
#include <bsl_iosfwd.h>
#endif

#ifndef INCLUDED_BSL_STRING
#include <bsl_string.h>
#endif

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

    // CLASS DATA
    static const int k_DISABLE_PUBLISH_IN_LOCALTIME;
                                              // Reserved offset (a value
                                              // corresponding to no known time
                                              // zone) that indicates that the
                                              // record time stamp (in UTC) is
                                              // *not* adjusted to the current
                                              // local time.

    static const int k_ENABLE_PUBLISH_IN_LOCALTIME;
                                              // Reserved offset (a value
                                              // corresponding to no known time
                                              // zone) that indicates that the
                                              // record time stamp (in UTC) is
                                              // adjusted to the current local
                                              // time.

    // DATA
    bsl::string            d_formatSpec;       // 'printf'-style format spec.
    bdlt::DatetimeInterval d_timestampOffset;  // offset added to timestamps

  public:
    // TRAITS
    BSLALG_DECLARE_NESTED_TRAITS(RecordStringFormatter,
                                 bslalg::TypeTraitUsesBslmaAllocator);

    // CREATORS
    explicit RecordStringFormatter(bslma::Allocator *basicAllocator = 0);
        // Create a record formatter having a default format specification and
        // a timestamp offset of 0.  Optionally specify a 'basicAllocator' used
        // to supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.  The default format specification is:
        //..
        //  "\n%d %p:%t %s %f:%l %c %m %u\n"
        //..

    explicit RecordStringFormatter(const char       *format,
                                   bslma::Allocator *basicAllocator = 0);
        // Create a record formatter having the specified 'format'
        // specification and a timestamp offset of 0.  Optionally specify a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0,
        // the currently installed default allocator is used.

    explicit RecordStringFormatter(
                            const bdlt::DatetimeInterval&  offset,
                            bslma::Allocator              *basicAllocator = 0);
        // !DEPRECATED!: Use a CTOR specifying 'publishInLocalTime' instead.
        //
        // Create a record formatter having a default format specification and
        // the specified timestamp 'offset'.  Optionally specify a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0,
        // the currently installed default allocator is used.  The default
        // format specification is:
        //..
        //  "\n%d %p:%t %s %f:%l %c %m %u\n"
        //..

    explicit RecordStringFormatter(bool              publishInLocalTime,
                                   bslma::Allocator *basicAllocator = 0);
        // Create a record formatter having a default format specification, and
        // if the specified 'publishInLocalTime' flag is 'true', format the
        // timestamp of each logged record in the local time of the current
        // task, and format the timestamp in UTC otherwise.  Optionally specify
        // a 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0,
        // the currently installed default allocator is used.  The default
        // format specification is:
        //..
        //  "\n%d %p:%t %s %f:%l %c %m %u\n"
        //..
        // Note that local time offsets are calculated for the timestamp of
        // each formatted record and so track transitions into and out of
        // Daylight Saving Time.

    RecordStringFormatter(const char                    *format,
                          const bdlt::DatetimeInterval&  offset,
                          bslma::Allocator              *basicAllocator = 0);
        // !DEPRECATED!: Use a CTOR specifying 'publishInLocalTime' instead.
        //
        // Create a record formatter having the specified 'format'
        // specification and the specified timestamp 'offset'.  Optionally
        // specify a 'basicAllocator' used to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.

    RecordStringFormatter(const char       *format,
                          bool              publishInLocalTime,
                          bslma::Allocator *basicAllocator = 0);
        // Create a record formatter having the specified 'format'
        // specification, and if the specified 'publishInLocalTime' flag is
        // 'true', format the timestamp of each log in the local time of the
        // current task, and format the timestamp in UTC otherwise.  Optionally
        // specify a 'basicAllocator' used to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.  Note that local time offsets are calculated for the timestamp
        // of each formatted record and so track transitions into and out of
        // Daylight Saving Time.

    RecordStringFormatter(const RecordStringFormatter&  original,
                          bslma::Allocator             *basicAllocator = 0);
        // Create a record formatter initialized to the value of the specified
        // 'original' record formatter.  Optionally specify a 'basicAllocator'
        // used to supply memory.  If 'basicAllocator' is 0, the currently
        // installed default allocator is used.

    ~RecordStringFormatter();
        // Destroy this record formatter.

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
        // !DEPRECATED!: Use 'enablePublishInLocalTime' instead.
        //
        // Set the timestamp offset of this record formatter to the specified
        // 'offset'.

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
        // !DEPRECATED!: Use the 'isPublishInLocalTimeEnabled' method instead.
        //
        // Return a reference to the non-modifiable timestamp offset of this
        // record formatter.

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
void RecordStringFormatter::disablePublishInLocalTime()
{
    d_timestampOffset.setTotalMilliseconds(k_DISABLE_PUBLISH_IN_LOCALTIME);
}

inline
void RecordStringFormatter::enablePublishInLocalTime()
{
    d_timestampOffset.setTotalMilliseconds(k_ENABLE_PUBLISH_IN_LOCALTIME);
}

inline
void RecordStringFormatter::setFormat(const char *format)
{
    d_formatSpec = format;
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
bool RecordStringFormatter::isPublishInLocalTimeEnabled() const
{
    return k_ENABLE_PUBLISH_IN_LOCALTIME ==
                                         d_timestampOffset.totalMilliseconds();
}

inline
const bdlt::DatetimeInterval&
RecordStringFormatter::timestampOffset() const
{
    return d_timestampOffset;
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
