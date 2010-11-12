// bael_recordstringformatter.h                                       -*-C++-*-
#ifndef INCLUDED_BAEL_RECORDSTRINGFORMATTER
#define INCLUDED_BAEL_RECORDSTRINGFORMATTER

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide a record formatter that uses a 'printf'-style format spec.
//
//@CLASSES:
//   bael_RecordStringFormatter: 'printf'-style formatter for log records
//
//@SEE_ALSO: bael_record, bael_recordattributes
//
//@AUTHOR: Gang Chen (gchen20)
//
//@DESCRIPTION: This component provides a value-semantic function-object class,
// 'bael_RecordStringFormatter', that is used to format log records according
// to a 'printf'-style format specification (see "Record Format Specification"
// below).  A format specification and a timestamp offset (in the form of a
// 'bdet_DatetimeInterval') are optionally supplied upon construction of a
// 'bael_RecordStringFormatter' object (or simply "record formatter").  If a
// format specification is not supplied, a default one (defined below) is used.
// If a timestamp offset is not supplied, it defaults to 0.  Both the format
// specification and timestamp offset of a record formatter can be modified
// following construction.
//
// An overloaded 'operator()' is defined for 'bael_RecordStringFormatter' that
// takes a 'bael_Record' and an 'bsl::ostream' as arguments.  This method
// formats the given record according to the format specification of the record
// formatter and outputs the result to the given stream.  Additionally, each
// timestamp indicated in the format specification is biased by the timestamp
// offset of the record formatter prior to outputting it to the stream.  This
// facilitates the logging of records in local time, if desired, in the event
// that the timestamp attribute of records are in GMT.
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
///Usage
///-----
// The following snippets of code illustrate how to use an instance of
// 'bael_RecordStringFormatter' to format log records.
//
// First we instantiate a record formatter with an explicit format
// specification (but we accept the default timestamp offset since it will not
// be used in this example):
//..
//  bael_RecordStringFormatter formatter("\n%t: %m\n");
//..
// The chosen format specification indicates that, when a record is formatted
// using 'formatter', the thread Id attribute of the record will be output
// followed by the message attribute of the record.
//
// Next we create a default 'bael_Record' and set the thread Id and message
// attributes of the record to dummy values:
//..
//  bael_Record record;
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

#ifndef INCLUDED_BAESCM_VERSION
#include <baescm_version.h>
#endif

#ifndef INCLUDED_BDET_DATETIMEINTERVAL
#include <bdet_datetimeinterval.h>
#endif

#ifndef INCLUDED_BDEX_INSTREAMFUNCTIONS
#include <bdex_instreamfunctions.h>
#endif

#ifndef INCLUDED_BDEX_OUTSTREAMFUNCTIONS
#include <bdex_outstreamfunctions.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITS
#include <bslalg_typetraits.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITUSESBSLMAALLOCATOR
#include <bslalg_typetraitusesbslmaallocator.h>
#endif

#ifndef INCLUDED_BSL_IOSFWD
#include <bsl_iosfwd.h>
#endif

#ifndef INCLUDED_BSL_STRING
#include <bsl_string.h>
#endif

namespace BloombergLP {

class bael_Record;

class bslma_Allocator;

                        // ================================
                        // class bael_RecordStringFormatter
                        // ================================

class bael_RecordStringFormatter {
    // This class provides a value-semantic log record formatter that holds a
    // 'printf'-style format specification and a timestamp offset.  The
    // overloaded 'operator()' provided by the class formats a given record
    // according to the format specification and outputs the formatted result
    // to a given stream.  The timestamp offset of the record formatter is
    // added to each timestamp that is output to the stream.
    //
    // Additionally, this class supports a complete set of *value* *semantic*
    // operations, including copy construction, assignment and equality
    // comparison, 'ostream' printing, and 'bdex' serialization.  A precise
    // operational definition of when two instances have the same value can be
    // found in the description of 'operator==' for the class.  This class is
    // *exception* *neutral* with no guarantee of rollback: If an exception is
    // thrown during the invocation of a method on a pre-existing instance, the
    // object is left in a valid state, but its value is undefined.  In no
    // event is memory leaked.  Finally, *aliasing* (e.g., using all or part of
    // an object as both source and destination) is supported in all cases.

    // DATA
    bsl::string           d_formatSpec;       // 'printf'-style format spec.
    bdet_DatetimeInterval d_timestampOffset;  // offset added to timestamps

  public:
    // TRAITS
    BSLALG_DECLARE_NESTED_TRAITS(bael_RecordStringFormatter,
                                 bslalg_TypeTraitUsesBslmaAllocator);

    // CLASS METHODS
    static int maxSupportedBdexVersion();
        // Return the most current 'bdex' streaming version number supported by
        // this class.

    // CREATORS
    explicit bael_RecordStringFormatter(bslma_Allocator *basicAllocator = 0);
        // Create a record formatter having a default format specification and
        // a timestamp offset of 0.  Optionally specify a 'basicAllocator' used
        // to supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.  The default format specification is:
        //..
        //  "\n%d %p:%t %s %f:%l %c %m %u\n"
        //..

    explicit bael_RecordStringFormatter(const char      *format,
                                        bslma_Allocator *basicAllocator = 0);
        // Create a record formatter having the specified 'format'
        // specification and a timestamp offset of 0.  Optionally specify a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0,
        // the currently installed default allocator is used.

    explicit bael_RecordStringFormatter(
                             const bdet_DatetimeInterval&  offset,
                             bslma_Allocator              *basicAllocator = 0);
        // Create a record formatter having a default format specification and
        // the specified timestamp 'offset'.  Optionally specify a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0,
        // the currently installed default allocator is used.  The default
        // format specification is:
        //..
        //  "\n%d %p:%t %s %f:%l %c %m %u\n"
        //..

    bael_RecordStringFormatter(
                             const char                   *format,
                             const bdet_DatetimeInterval&  offset,
                             bslma_Allocator              *basicAllocator = 0);
        // Create a record formatter having the specified 'format'
        // specification and the specified timestamp 'offset'.  Optionally
        // specify a 'basicAllocator' used to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.

    bael_RecordStringFormatter(
                        const bael_RecordStringFormatter&  original,
                        bslma_Allocator                   *basicAllocator = 0);
        // Create a record formatter initialized to the value of the specified
        // 'original' record formatter.  Optionally specify a 'basicAllocator'
        // used to supply memory.  If 'basicAllocator' is 0, the currently
        // installed default allocator is used.

    ~bael_RecordStringFormatter();
        // Destroy this record formatter.

    // MANIPULATORS
    bael_RecordStringFormatter& operator=(
                                        const bael_RecordStringFormatter& rhs);
        // Assign to this record formatter the value of the specified 'rhs'
        // record formatter.

    void setFormat(const char *format);
        // Set the format specification of this record formatter to the
        // specified 'format'.

    void setTimestampOffset(const bdet_DatetimeInterval& offset);
        // Set the timestamp offset of this record formatter to the specified
        // 'offset'.

    template <class STREAM>
    STREAM& bdexStreamIn(STREAM& stream, int version);
        // Assign to this object the value read from the specified input
        // 'stream' using the specified 'version' format and return a reference
        // to the modifiable 'stream'.  If 'stream' is initially invalid, this
        // operation has no effect.  If 'stream' becomes invalid during this
        // operation, this object is valid, but its value is undefined.  If
        // 'version' is not supported, 'stream' is marked invalid and this
        // object is unaltered.  Note that no version is read from 'stream'.

    // ACCESSORS
    void operator()(bsl::ostream& stream, const bael_Record& record) const;
        // Format the specified 'record' according to the format specification
        // of this record formatter and output the result to the specified
        // 'stream'.  The timestamp offset of this record formatter is added to
        // each timestamp that is output to 'stream'.

    const char *format() const;
        // Return the format specification of this record formatter.

    const bdet_DatetimeInterval& timestampOffset() const;
        // Return a reference to the non-modifiable timestamp offset of this
        // record formatter.

    template <class STREAM>
    STREAM& bdexStreamOut(STREAM& stream, int version) const;
        // Write this value to the specified output 'stream' using the
        // specified 'version' format and return a reference to the modifiable
        // 'stream'.  If 'version' is not supported, 'stream' is unmodified.
        // Note that 'version' is not written to 'stream'.
};

// FREE OPERATORS
bool operator==(const bael_RecordStringFormatter& lhs,
                const bael_RecordStringFormatter& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' record formatters have
    // the same value, and 'false' otherwise.  Two record formatters have the
    // same value if they have the same format specification and the same
    // timestamp offset.

inline
bool operator!=(const bael_RecordStringFormatter& lhs,
                const bael_RecordStringFormatter& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' record formatters do not
    // have the same value, and 'false' otherwise.  Two record formatters
    // differ in value if their format specifications differ or their timestamp
    // offsets differ.

bsl::ostream& operator<<(bsl::ostream&                     output,
                         const bael_RecordStringFormatter& rhs);
    // Write the specified 'rhs' record formatter to the specified output
    // 'stream' in some reasonable (single-line) format and return a reference
    // to the modifiable 'stream'.

// ============================================================================
//                      INLINE FUNCTION DEFINITIONS
// ============================================================================

                        // --------------------------------
                        // class bael_RecordStringFormatter
                        // --------------------------------

// CLASS METHODS
inline
int bael_RecordStringFormatter::maxSupportedBdexVersion()
{
    return 1;
}

// MANIPULATORS
inline
void bael_RecordStringFormatter::setFormat(const char *format)
{
    d_formatSpec = format;
}

inline
void bael_RecordStringFormatter::setTimestampOffset(
                                           const bdet_DatetimeInterval& offset)
{
    d_timestampOffset = offset;
}

template <class STREAM>
STREAM& bael_RecordStringFormatter::bdexStreamIn(STREAM& stream, int version)
{
    if (stream) {
        switch (version) { // switch on the schema version (starting with 1)
          case 1: {
            bsl::string format;
            bdex_InStreamFunctions::streamIn(stream, format, 0);
            if (!stream) {
                return stream;                                        // RETURN
            }

            bdet_DatetimeInterval offset;
            offset.bdexStreamIn(stream, 1);
            if (!stream) {
                return stream;                                        // RETURN
            }

            d_formatSpec      = format;
            d_timestampOffset = offset;
          } break;
          default: {
            stream.invalidate();
          }
        }
    }
    return stream;
}

// ACCESSORS
inline
const char *bael_RecordStringFormatter::format() const
{
    return d_formatSpec.c_str();
}

inline
const bdet_DatetimeInterval&
bael_RecordStringFormatter::timestampOffset() const
{
    return d_timestampOffset;
}

template <class STREAM>
STREAM& bael_RecordStringFormatter::bdexStreamOut(STREAM& stream,
                                                  int     version) const
{
    switch (version) {
      case 1: {
        bdex_OutStreamFunctions::streamOut(stream, d_formatSpec, 0);
        d_timestampOffset.bdexStreamOut(stream, 1);
      } break;
    }
    return stream;
}

// FREE OPERATORS
inline
bool operator!=(const bael_RecordStringFormatter& lhs,
                const bael_RecordStringFormatter& rhs)
{
    return !(lhs == rhs);
}

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2005
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
