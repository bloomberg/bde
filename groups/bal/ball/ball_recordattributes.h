// ball_recordattributes.h                                            -*-C++-*-
#ifndef INCLUDED_BALL_RECORDATTRIBUTES
#define INCLUDED_BALL_RECORDATTRIBUTES

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a container for a fixed set of fields suitable for logging.
//
//@CLASSES:
//  ball::RecordAttributes: container for a fixed set of log fields
//
//@SEE_ALSO: ball_record
//
//@DESCRIPTION: This component defines a container for aggregating a fixed set
// of fields intrinsically appropriate for logging.  Using
// `ball::RecordAttributes`, a logger can transmit log message text together
// with relevant auxiliary information (e.g., timestamp, filename, line number,
// etc.) as a single instance, rather than passing around individual attributes
// separately.
//
// The attributes held by `ball::RecordAttributes` are given in the following
// table:
// ```
//    Attribute        Type               Description               Default
//    ----------   -------------   ------------------------------   --------
//    timestamp    bdlt::Datetime  creation date and time           (*Note*)
//    processID    int             process id of creator            0
//    threadID     Uint64          thread id of creator             0
//    fileName     string          file where created  (__FILE__)   ""
//    lineNumber   int             line number in file (__LINE__)   0
//    category     string          category of logged record        ""
//    severity     int             severity of logged record        0
//    message      string          log message text                 ""
// ```
// *Note*: The default value given to the timestamp attribute is implementation
// defined.  (See the `bdlt_datetime` component-level documentation for more
// information.)
//
// *Cached Stream State*: Although a `RecordAttributes` object is nominally a
// value-semantic attribute type whose attributes are described in the table
// above, for performance and convenience a `RecordAttributes` object provides
// access to internal instances of `bdlsb::MemOutStreamBuf` and `bsl::ostream`
// objects (via `messageStreamBuf` and `messageStream` accessors, respectively)
// that allow users to build the `message` directly without having to construct
// any I/O stream objects independently.  This is useful because `ball::Record`
// objects and their `RecordAttributes` are cached for performance.  Note that
// it's possible for two equal instances of `RecordAttributes` objects to have
// different stream states.  Resetting the message will clear most stream
// state, except for the locale, installed callbacks, and any pword/iword data.
//
// For each attribute, there is a method to access its value and a method to
// change its value.  E.g., for the timestamp attribute, there is the
// `timestamp` accessor and the `setTimestamp` manipulator.  Note that for the
// message attribute, there is a `message` accessor which is *deprecated*; use
// the `messageRef` accessor instead.  The class also provides the ability to
// stream an object (whose class must support the `operator<<`) into the
// message attribute using `messageStreamBuf` method (see the usage example-2).
//
// Alternately, an object can be streamed directly into the message attribute
// via the stream exposed by the `messageStream` method: in that case, the
// `messageStreamBuf` should not also be used simultaneously.  More precisely,
// use of the references returned by the `messageStreamBuf` and `messageStream`
// methods to build a message results in undefined behavior if interleaved with
// each other, or if interspersed with calls to `setMessage`, `clearMessage`,
// `operator=`, or `message` (but not `messageRef`).  In other words, streaming
// an object into the message attribute must be done in one uninterrupted
// sequence of operations.
//
// The default values listed in the table above are the values given to the
// respective attributes by the default constructor of
// `ball::RecordAttributes`.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Syntax
///- - - - - - - - -
// The `ball::RecordAttributes` class holds sufficient information on which to
// base a rudimentary logging, tracing, or reporting facility.  The following
// code fragments illustrate the essentials of working with these attributes.
//
// Assume that our example is part of a financial application using categories
// and message severities as follows:
// ```
//     const char *Category[] = { "Bonds", "Equities", "Futures" };
//     enum { INFO, WARN, BUY, SELL };
// ```
// First define a `ball::RecordAttributes` object with each attribute
// initialized to its default value:
// ```
//     ball::RecordAttributes attributes;
// ```
// Next set each of the attributes to some meaningful value:
// ```
//     bdlt::Datetime now;
//     bdlt::EpochUtil::convertFromTimeT(&now, time(0));
//     attributes.setTimestamp(now);                // current time
//     attributes.setProcessID(getpid());
//     attributes.setThreadID((bsls::Types::Uint64) pthread_self());
//     attributes.setFileName(__FILE__);
//     attributes.setLineNumber(__LINE__);
//     attributes.setCategory(Category[2]);         // "Futures"
//     attributes.setSeverity(WARN);
//     attributes.setMessage("sugar up (locust infestations on the rise)");
// ```
// The message in this example briefly informs that something interesting may
// be happening with respect to sugar futures.  In general, the message
// attribute can contain an arbitrary amount of information.
//
// Now that the sample `ball::RecordAttributes` object has been populated with
// the desired information, it can be passed to a function, stored in a
// database, cached in a container of `ball::RecordAttributes` objects, etc.
// For the purposes of this illustration, we'll simply format and stream
// selected attributes to a specified `ostream` using the following function:
// ```
//     void printMessage(ostream&                      stream,
//                       const ball::RecordAttributes& attributes)
//     {
//         using namespace std;
//         stream << "\tTimestamp: " << attributes.timestamp()  << endl;
//         stream << "\tCategory:  " << attributes.category()   << endl;
//         stream << "\tMessage:   " << attributes.messageRef() << endl;
//         stream << endl;
//     }
// ```
// The following call:
// ```
//     printMessage(bsl::cout, attributes);
// ```
// prints these attributes to `stdout`:
// ```
//             Timestamp: 19JAN2004_23:07:38.000
//             Category:  Futures
//             Message:   sugar up (locust infestations on the rise)
// ```
//
///Example 2: Streaming Data Into a Message Attribute
/// - - - - - - - - - - - - - - - - - - - - - - - - -
// Following example demonstrates how an object of a class supporting `ostream`
// operation (`operator<<`) can be streamed into the message attribute.
// Suppose we want to stream objects of the following class.
// ```
//   class Information
//   {
//     private:
//       bsl::string d_heading;
//       bsl::string d_contents;
//
//     public:
//       Information(const char *heading, const char *contents);
//       const bsl::string& heading() const;
//       const bsl::string& contents() const;
//   };
// ```
// The component containing the `Information` must provide `operator<<`.  Here
// is a possible implementation.
// ```
//   bsl::ostream& operator<<(bsl::ostream&      stream,
//                            const Information& information)
//   {
//       stream << information.heading() << endl;
//       stream << '\t';
//       stream << information.contents() << endl;
//       return stream;
//   }
// ```
// The following function streams an `Information` object into the message
// attribute of a `ball::RecordAttributes` object.
// ```
//   void streamInformationIntoMessageAttribute(
//                                        ball::RecordAttributes& attributes,
//                                        const Information&      information)
//   {
//       // First clear the message attributes.
//       attributes.clearMessage();
//
//       // Create an 'ostream' from message stream buffer.
//       bsl::ostream os(&attributes.messageStreamBuf());
//
//       // Now stream the information object into the created ostream.
//       // This will set the message attribute of 'attributes' to the
//       // streamed contents.
//       os << information;
//   }
// ```

#include <balscm_version.h>

#include <bdlsb_memoutstreambuf.h>

#include <bdlt_datetime.h>

#include <bslma_allocator.h>
#include <bslma_usesbslmaallocator.h>

#include <bslmf_nestedtraitdeclaration.h>

#include <bsls_performancehint.h>
#include <bsls_platform.h>
#include <bsls_types.h>

#include <bsl_ostream.h>
#include <bsl_string.h>

namespace BloombergLP {
namespace ball {

                        // ======================
                        // class RecordAttributes
                        // ======================

/// This class provides a container for a fixed set of attributes
/// appropriate for logging.  For each attribute in this class (e.g.,
/// `category`), there is an accessor for obtaining the attribute's value
/// (the `category` accessor) and a manipulator for changing the attribute's
/// value (the `setCategory` manipulator).
///
/// Additionally, this class supports a complete set of *value* *semantic*
/// operations, including copy construction, assignment and equality
/// comparison, and `ostream` printing.  A precise operational definition of
/// when two instances have the same value can be found in the description
/// of `operator==` for the class.  This class is *exception* *neutral* with
/// no guarantee of rollback: If an exception is thrown during the
/// invocation of a method on a pre-existing instance, the object is left in
/// a valid state, but its value is undefined.  In no event is memory
/// leaked.  Finally, *aliasing* (e.g., using all or part of an object as
/// both source and destination) is supported in all cases.
class RecordAttributes {

    // PRIVATE TYPES
    typedef bsls::Types::Uint64 Uint64;

    // PRIVATE CONSTANTS
    enum {
        k_RESET_MESSAGE_STREAM_CAPACITY = 256  // maximum capacity above which
                                               // the message stream is reset
                                               // (and not rewound)
    };

    // DATA
    bdlt::Datetime   d_timestamp;    // creation date and time
    int              d_processID;    // process id of creator
    Uint64           d_threadID;     // thread id of creator
    bsl::string      d_fileName;     // name of file where created (__FILE__)
    int              d_lineNumber;   // line number of said file   (__LINE__)
    bsl::string      d_category;     // category of log record
    int              d_severity;     // severity of log record

    bdlsb::MemOutStreamBuf d_messageStreamBuf;  // stream buffer associated
                                                // with the message attribute

    bsl::ostream           d_messageStream;     // stream associated with the
                                                // message attribute

    // FRIENDS
    friend bool operator==(const RecordAttributes&, const RecordAttributes&);

    // PRIVATE MANIPULATORS

    /// Reset the message stream state to the default stream state, except
    /// for the imbued locale, any installed callbacks, and any pword/iword
    /// data (in practice these are unlikely to deviate from the default in
    /// the first place).
    void resetMessageStreamState();

  public:
    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(RecordAttributes,
                                   bslma::UsesBslmaAllocator);

    // CREATORS

    /// Create a record attributes object with all attributes having default
    /// values.  Optionally specify a `basicAllocator` used to supply
    /// memory.  If `basicAllocator` is 0, the currently installed default
    /// allocator is used.
    explicit RecordAttributes(bslma::Allocator *basicAllocator = 0);

    /// Create a record attributes object having the specified `timestamp`,
    /// `processID`, `threadID`, `fileName`, `lineNumber`, `category`,
    /// `severity` and `message` values, respectively.  Optionally specify a
    /// `basicAllocator` used to supply memory.  If `basicAllocator` is 0,
    /// the currently installed default allocator is used.  The behavior is
    /// undefined if any `const char *` argument is null.
    RecordAttributes(const bdlt::Datetime&  timestamp,
                     int                    processID,
                     bsls::Types::Uint64    threadID,
                     const char            *fileName,
                     int                    lineNumber,
                     const char            *category,
                     int                    severity,
                     const char            *message,
                     bslma::Allocator      *basicAllocator = 0);

    /// Create a record attributes object having the value of the specified
    /// `original` record attributes object.  Optionally specify a
    /// `basicAllocator` used to supply memory.  If `basicAllocator` is 0,
    /// the currently installed default allocator is used.
    RecordAttributes(const RecordAttributes&  original,
                     bslma::Allocator        *basicAllocator = 0);

    /// Destroy this record attributes object.
    //! ~RecordAttributes() = default;

    // MANIPULATORS

    /// Assign to this record attributes object the value of the specified
    /// `rhs` record attributes object.  Resets the objects returned by the
    /// `messageStreamBuf` and `messageStream` methods.
    RecordAttributes& operator=(const RecordAttributes& rhs);

    /// Set the message attribute of this record attributes object to the
    /// empty string.  Resets the objects returned by the `messageStreamBuf`
    /// and `messageStream` methods.
    void clearMessage();

    /// Return a reference to the modifiable stream buffer associated with
    /// the message attribute of this record attributes object.
    bdlsb::MemOutStreamBuf& messageStreamBuf();

    /// Return a reference to the modifiable stream associated with the
    /// message attribute of this record attributes object.
    bsl::ostream& messageStream();

    /// Set the category attribute of this record attributes object to the
    /// specified (non-null) `category`.
    void setCategory(const char *category);

    /// Set the filename attribute of this record attributes object to the
    /// specified (non-null) `fileName`.
    void setFileName(const char *fileName);

    /// Set the line number attribute of this record attributes object to
    /// the specified `lineNumber`.
    void setLineNumber(int lineNumber);

    /// Set the message attribute of this record attributes object to the
    /// specified (non-null) `message`.  Resets the objects returned by the
    /// `messageStreamBuf` and `messageStream` methods.
    void setMessage(const char *message);

    /// Set the processID attribute of this record attributes object to the
    /// specified `processID`.
    void setProcessID(int processID);

    /// Set the severity attribute of this record attributes object to the
    /// specified `severity`.
    void setSeverity(int severity);

    /// Set the threadID attribute of this record attributes object to the
    /// specified `threadID`.
    void setThreadID(bsls::Types::Uint64 threadID);

    /// Set the timestamp attribute of this record attributes object to the
    /// specified `timestamp`.
    void setTimestamp(const bdlt::Datetime& timestamp);

    // ACCESSORS

    /// Return the category attribute of this record attributes object.
    const char *category() const;

    /// Return the filename attribute of this record attributes object.
    const char *fileName() const;

    /// Return the line number attribute of this record attributes object.
    int lineNumber() const;

    /// Return the message attribute of this record attributes object.  Note
    /// that this method will return a truncated message if it contains
    /// embedded null ('\0') characters; see `messageRef` for an alternative
    /// to this method.  **Warning:** This method is *not* const thread-safe,
    /// and cannot be safely called concurrently.  It may modify the stream
    /// buffer returned by `messageStreamBuf`.
    ///
    /// **DEPRECATED**: Use `messageRef` instead.
    const char *message() const;

    /// Return a string reference providing non-modifiable access to the
    /// message attribute of this record attributes object.  Note that the
    /// returned string reference is not null-terminated, and may contain
    /// null ('\0') characters.
    bslstl::StringRef messageRef() const;

    /// Return the processID attribute of this record attributes object.
    int processID() const;

    /// Return the severity attribute of this record attributes object.
    int severity() const;

    /// Return the threadID attribute of this record attributes object.
    bsls::Types::Uint64 threadID() const;

    /// Return the timestamp attribute of this record attributes object.
    const bdlt::Datetime& timestamp() const;

    /// Return a reference to the non-modifiable stream buffer associated
    /// with the message attribute of this record attributes object.
    const bdlsb::MemOutStreamBuf& messageStreamBuf() const;

    /// Return a reference to the non-modifiable stream associated with the
    /// message attribute of this record attributes object.
    const bsl::ostream& messageStream() const;

    /// Format this object to the specified output `stream` at the
    /// optionally specified indentation `level` and return a reference to
    /// the modifiable `stream`.  If `level` is specified, optionally
    /// specify `spacesPerLevel`, the number of spaces per indentation level
    /// for this and all of its nested objects.  Each line is indented by
    /// the absolute value of `level * spacesPerLevel`.  If `level` is
    /// negative, suppress indentation of the first line.  If
    /// `spacesPerLevel` is negative, suppress line breaks and format the
    /// entire output on one line.  If `stream` is initially invalid, this
    /// operation has no effect.
    bsl::ostream& print(bsl::ostream& stream,
                        int           level = 0,
                        int           spacesPerLevel = 4) const;

};

// FREE OPERATORS

/// Return `true` if the specified `lhs` and `rhs` record attributes objects
/// have the same value, and `false` otherwise.  Two record attributes
/// objects have the same value if each respective pair of attributes have
/// the same value.
bool operator==(const RecordAttributes& lhs, const RecordAttributes& rhs);

/// Return `true` if the specified `lhs` and `rhs` record attributes objects
/// do not have the same value, and `false` otherwise.  Two record
/// attributes objects do not have the same value if one or more respective
/// attributes differ in value.
inline
bool operator!=(const RecordAttributes& lhs, const RecordAttributes& rhs);

/// Format the members of the specified `object` to the specified output
/// `stream` and return a reference to the modifiable `stream`.
inline
bsl::ostream& operator<<(bsl::ostream& stream, const RecordAttributes& object);

// ============================================================================
//                              INLINE DEFINITIONS
// ============================================================================

                        // ----------------------
                        // class RecordAttributes
                        // ----------------------

// PRIVATE MANIPULATORS
inline
void RecordAttributes::resetMessageStreamState()
{
    // See basic_ios::init; note that we intentionally avoid 'copyfmt' here as
    // re-imbuing the stream with a locale is quite expensive, and would defeat
    // the purpose of caching the stream object in the attributes in the first
    // place.
    d_messageStream.exceptions(bsl::ios_base::goodbit);
    d_messageStream.clear();
    d_messageStream.tie(0);
    d_messageStream.flags(bsl::ios_base::dec | bsl::ios_base::skipws);
    d_messageStream.fill(' ');
    d_messageStream.precision(6);
    d_messageStream.width(0);
}

// MANIPULATORS
inline
void RecordAttributes::clearMessage()
{
    // Note that the stream buffer holding the message attribute has initial
    // capacity of 256 bytes (by implementation).  Reset those stream buffers
    // that are bigger than the default and "rewind" those that are smaller or
    // equal.
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(
            k_RESET_MESSAGE_STREAM_CAPACITY < d_messageStreamBuf.capacity())) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        d_messageStreamBuf.reset();
    }
    else {
        d_messageStreamBuf.pubseekpos(0);
    }
    resetMessageStreamState();
}

inline
bdlsb::MemOutStreamBuf& RecordAttributes::messageStreamBuf()
{
    return d_messageStreamBuf;
}

inline
bsl::ostream& RecordAttributes::messageStream()
{
    return d_messageStream;
}

inline
void RecordAttributes::setCategory(const char *category)
{
    d_category = category;
}

inline
void RecordAttributes::setFileName(const char *fileName)
{
    d_fileName = fileName;
}

inline
void RecordAttributes::setLineNumber(int lineNumber)
{
    d_lineNumber = lineNumber;
}

inline
void RecordAttributes::setProcessID(int processID)
{
    d_processID = processID;
}

inline
void RecordAttributes::setSeverity(int severity)
{
    d_severity = severity;
}

inline
void RecordAttributes::setThreadID(bsls::Types::Uint64 threadID)
{
    d_threadID = threadID;
}

inline
void RecordAttributes::setTimestamp(const bdlt::Datetime& timestamp)
{
    d_timestamp = timestamp;
}

// ACCESSORS
inline
const char *RecordAttributes::category() const
{
    return d_category.c_str();
}

inline
const char *RecordAttributes::fileName() const
{
    return d_fileName.c_str();
}

inline
int RecordAttributes::lineNumber() const
{
    return d_lineNumber;
}

inline
int RecordAttributes::processID() const
{
    return d_processID;
}

inline
int RecordAttributes::severity() const
{
    return d_severity;
}

inline
bsls::Types::Uint64 RecordAttributes::threadID() const
{
    return d_threadID;
}

inline
const bdlsb::MemOutStreamBuf& RecordAttributes::messageStreamBuf() const
{
    return d_messageStreamBuf;
}

inline
const bsl::ostream& RecordAttributes::messageStream() const
{
    return d_messageStream;
}

inline
const bdlt::Datetime& RecordAttributes::timestamp() const
{
    return d_timestamp;
}

}  // close package namespace

// FREE OPERATORS
inline
bool ball::operator!=(const RecordAttributes& lhs, const RecordAttributes& rhs)
{
    return !(lhs == rhs);
}

inline
bsl::ostream& ball::operator<<(bsl::ostream&           stream,
                               const RecordAttributes& object)
{
    return object.print(stream, 0, -1);
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
