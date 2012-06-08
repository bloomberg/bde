// bael_recordattributes.h                                            -*-C++-*-
#ifndef INCLUDED_BAEL_RECORDATTRIBUTES
#define INCLUDED_BAEL_RECORDATTRIBUTES

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide a container for a fixed set of fields suitable for logging.
//
//@CLASSES:
//     bael_RecordAttributes: container for a fixed set of log fields
//
//@SEE_ALSO: bael_record
//
//@AUTHOR: Hong Shi (hshi2)
//
//@DESCRIPTION: This component defines a container for aggregating a fixed set
// of fields intrinsically appropriate for logging.  Using
// 'bael_RecordAttributes', a logger can transmit log message text together
// with relevant auxiliary information (e.g., timestamp, filename, line number,
// etc.) as a single instance, rather than passing around individual attributes
// separately.
//
// The attributes held by 'bael_RecordAttributes' are given in the following
// table:
//..
//     Attribute        Type               Description               Default
//     ----------   -------------   ------------------------------   -------
//     timestamp    bdet_Datetime   creation date and time           (*Note*)
//     processID    int             process id of creator              0
//     threadID     Uint64          thread id of creator               0
//     fileName     string          file where created  (__FILE__)     ""
//     lineNumber   int             line number in file (__LINE__)     0
//     category     string          category of logged record          ""
//     severity     int             severity of logged record          0
//     message      string          log message text                   ""
//..
// *Note*: The default value given to the timestamp attribute is implementation
// defined.  (See the 'bdet_datetime' component-level documentation for more
// information.)
//
// For each attribute, there is a method to access its value and a method to
// change its value.  E.g., for the timestamp attribute, there is the
// 'timestamp' accessor and the 'setTimestamp' manipulator.  The class
// also provides the ability to stream an object (whose class must support
// the 'operator<<') into the message attribute using 'messageStreamBuf'
// method (see the usage example-2).  The default values listed in the
// table above are the values given to the respective attributes by
// the default constructor of 'bael_RecordAttributes'.
//
///Usage
///------
//
///Example 1:
///- - - - -
// The 'bael_RecordAttributes' class holds sufficient information on which to
// base a rudimentary logging, tracing, or reporting facility.  The following
// code fragments illustrate the essentials of working with these attributes.
//
// Assume that our example is part of a financial application using categories
// and message severities as follows:
//..
//      const char *Category[] = { "Bonds", "Equities", "Futures" };
//      enum { INFO, WARN, BUY, SELL };
//..
// First define a 'bael_RecordAttributes' object with each attribute
// initialized to its default value:
//..
//      bael_RecordAttributes attributes;
//..
// Next set each of the attributes to some meaningful value:
//..
//      bdet_Datetime now;
//      bdetu_Epoch::convertFromTimeT(&now, time(0));
//      attributes.setTimestamp(now);                // current time
//      attributes.setProcessID(getpid());
//      attributes.setThreadID((bsls_PlatformUtil::Uint64) pthread_self());
//      attributes.setFileName(__FILE__);
//      attributes.setLineNumber(__LINE__);
//      attributes.setCategory(Category[2]);         // "Futures"
//      attributes.setSeverity(WARN);
//      attributes.setMessage("sugar up (locust infestations on the rise)");
//..
// The message in this example briefly informs that something interesting may
// be happening with respect to sugar futures.  In general, the message
// attribute can contain an arbitrary amount of information.
//
// Now that the sample 'bael_RecordAttributes' object has been populated with
// the desired information, it can be passed to a function, stored in a
// database, cached in a container of 'bael_RecordAttributes' objects, etc.
// For the purposes of this illustration, we'll simply format and stream
// selected attributes to a specified 'ostream' using the following function:
//..
//      void printMessage(ostream&                     stream,
//                        const bael_RecordAttributes& attributes)
//      {
//          using namespace std;
//          stream << "\tTimestamp: " << attributes.timestamp() << endl;
//          stream << "\tCategory:  " << attributes.category()  << endl;
//          stream << "\tMessage:   " << attributes.message()   << endl;
//          stream << endl;
//      }
//..
// The following call:
//..
//      printMessage(bsl::cout, attributes);
//..
// prints these attributes to 'stdout':
//..
//              Timestamp: 19JAN2004_23:07:38.000
//              Category:  Futures
//              Message:   sugar up (locust infestations on the rise)
//..
///Example 2:
///- - - - -
// Following example demonstrates how an object of a class supporting 'ostream'
// operation ('operator<<') can be streamed into the message
// attribute.  Suppose we want to stream objects of following class.
//..
//    class Information
//    {
//      private:
//        bsl::string d_heading;
//        bsl::string d_contents;
//
//      public:
//        Information(const char *heading, const char *contents);
//        const bsl::string& heading() const;
//        const bsl::string& contents() const;
//    };
//..
// The component containing the 'Information' must provide 'operator<<'.
// Here is a possible implementation.
//..
//    bsl::ostream& operator<<(bsl::ostream& stream,
//                             const Information& information)
//    {
//        stream << information.heading() << endl;
//        stream << '\t';
//        stream << information.contents() << endl;
//        return stream;
//    }
//..
//
// The following function streams an 'Information' object into the message
// attribute of a 'bael_RecordAttributes' object.
//..
//    void streamInformationIntoMessageAttribute(
//                                          bael_RecordAttributes& attributes,
//                                          const Information&     information)
//    {
//        // first clear the message attributes
//        attributes.clearMessage();
//
//        // create an 'ostream' from message stream buffer
//        bsl::ostream os(&attributes.messageStreamBuf());
//
//        // now stream the information object into the created ostream,
//        // this will set the message attribute of 'attributes' to the
//        // streamed contents.
//        os << information;
//    }
//..

#ifndef INCLUDED_BAESCM_VERSION
#include <baescm_version.h>
#endif

#ifndef INCLUDED_BDESB_MEMOUTSTREAMBUF
#include <bdesb_memoutstreambuf.h>
#endif

#ifndef INCLUDED_BDET_DATETIME
#include <bdet_datetime.h>
#endif

#ifndef INCLUDED_BSLS_PLATFORM
#include <bsls_platform.h>
#endif

#ifdef BSLS_PLATFORM__OS_WINDOWS

#ifndef INCLUDED_BSL_OSTREAM
#include <bsl_ostream.h>
#endif

#else

#ifndef INCLUDED_BSL_IOSFWD
#include <bsl_iosfwd.h>
#endif

#endif

#ifndef INCLUDED_BSL_STRING
#include <bsl_string.h>
#endif

namespace BloombergLP {

class bslma_Allocator;

                        // ===========================
                        // class bael_RecordAttributes
                        // ===========================

class bael_RecordAttributes {
    // This class provides a container for a fixed set of attributes
    // appropriate for logging.  For each attribute in this class (e.g.,
    // 'category'), there is an accessor for obtaining the attribute's value
    // (the 'category' accessor) and a manipulator for changing the attribute's
    // value (the 'setCategory' manipulator).
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

    // PRIVATE TYPES
    typedef bsls_PlatformUtil::Uint64 Uint64;

    // DATA
    bdet_Datetime    d_timestamp;    // creation date and time
    int              d_processID;    // process id of creator
    Uint64           d_threadID;     // thread id of creator
    bsl::string      d_fileName;     // name of file where created (__FILE__)
    int              d_lineNumber;   // line number of said file   (__LINE__)
    bsl::string      d_category;     // category of log record
    int              d_severity;     // severity of log record

    bdesb_MemOutStreamBuf
                     d_messageStreamBuf;
                                     // stream buffer associated with the
                                     // message attribute

    // FRIENDS
    friend bool operator==(const bael_RecordAttributes&,
                           const bael_RecordAttributes&);

  public:
    // CLASS METHODS
    static int maxSupportedBdexVersion();
        // Return the most current 'bdex' streaming version number supported by
        // this class.  (See the package-group-level documentation for more
        // information on 'bdex' streaming of value-semantic types and
        // containers.)

    static int maxSupportedVersion();
        // Return the most current 'bdex' streaming version number supported by
        // this class.  (See the package-group-level documentation for more
        // information on 'bdex' streaming of value-semantic types and
        // containers.)
        //
        // DEPRECATED: Replaced by the 'maxSupportedBdexVersion' method.

    // CREATORS
    bael_RecordAttributes(bslma_Allocator *basicAllocator = 0);
        // Create a record attributes object with all attributes having default
        // values.  Optionally specify a 'basicAllocator' used to supply
        // memory.  If 'basicAllocator' is 0, the currently installed default
        // allocator is used.

    bael_RecordAttributes(const bdet_Datetime&      timestamp,
                          int                       processID,
                          bsls_PlatformUtil::Uint64 threadID,
                          const char               *fileName,
                          int                       lineNumber,
                          const char               *category,
                          int                       severity,
                          const char               *message,
                          bslma_Allocator          *basicAllocator = 0);
        // Create a record attributes object having the specified 'timestamp',
        // 'processID', 'threadID', 'fileName', 'lineNumber', 'category',
        // 'severity' and 'message' values, respectively.  Optionally specify a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0,
        // the currently installed default allocator is used.  The behavior is
        // undefined if any 'const char *' argument is null.

    bael_RecordAttributes(const bael_RecordAttributes&  original,
                          bslma_Allocator              *basicAllocator = 0);
        // Create a record attributes object having the value of the specified
        // 'original' record attributes object.  Optionally specify a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0,
        // the currently installed default allocator is used.

    ~bael_RecordAttributes();
        // Destroy this record attributes object.

    // MANIPULATORS
    bael_RecordAttributes& operator=(const bael_RecordAttributes& rhs);
        // Assign to this record attributes object the value of the specified
        // 'rhs' record attributes object.

    void clearMessage();
        // Set the message attribute of this record attributes object to
        // empty string.

    bdesb_MemOutStreamBuf& messageStreamBuf();
        // Return a reference to the modifiable stream buffer associated with
        // the message attribute of this record attributes object.

    void setCategory(const char *category);
        // Set the category attribute of this record attributes object to the
        // specified (non-null) 'category'.

    void setFileName(const char *fileName);
        // Set the filename attribute of this record attributes object to the
        // specified (non-null) 'fileName'.

    void setLineNumber(int lineNumber);
        // Set the line number attribute of this record attributes object to
        // the specified 'lineNumber'.

    void setMessage(const char *message);
        // Set the message attribute of this record attributes object to the
        // specified (non-null) 'message'.

    void setMessageRef(const bslstl_StringRef& strref);
        // Set the message attribute of this record attributes object to the
        // specified string reference 'strref'.  This function does not
        // truncate the string from 'strref' if there are embedded '\0'
        // characters in it.

    void setProcessID(int processID);
        // Set the processID attribute of this record attributes object to the
        // specified 'processID'.

    void setSeverity(int severity);
        // Set the severity attribute of this record attributes object to the
        // specified 'severity'.

    void setThreadID(bsls_PlatformUtil::Uint64 threadID);
        // Set the threadID attribute of this record attributes object to the
        // specified 'threadID'.

    void setTimestamp(const bdet_Datetime& timestamp);
        // Set the timestamp attribute of this record attributes object to the
        // specified 'timestamp'.

    template <class STREAM>
    STREAM& bdexStreamIn(STREAM& stream, int version);
        // Assign to this object the value read from the specified input
        // 'stream' using the specified 'version' format and return a reference
        // to the modifiable 'stream'.  If 'stream' is initially invalid, this
        // operation has no effect.  If 'stream' becomes invalid during this
        // operation, this object is valid, but its value is undefined.  If the
        // specified 'version' is not supported, 'stream' is marked invalid,
        // but this object is unaltered.  Note that no version is read from
        // 'stream'.  (See the package-group-level documentation for more
        // information on 'bdex' streaming of value-semantic types and
        // containers.)

    // ACCESSORS
    const char *category() const;
        // Return the category attribute of this record attributes object.

    const char *fileName() const;
        // Return the filename attribute of this record attributes object.

    int lineNumber() const;
        // Return the line number attribute of this record attributes object.

    const char *message() const;
        // Return the message attribute of this record attributes object.

    bslstl_StringRef messageRef() const;
        // Return a modifiable message attribute of this record attributes
        // object.

    int processID() const;
        // Return the processID attribute of this record attributes object.

    int severity() const;
        // Return the severity attribute of this record attributes object.

    bsls_PlatformUtil::Uint64 threadID() const;
        // Return the threadID attribute of this record attributes object.

    const bdet_Datetime& timestamp() const;
        // Return the timestamp attribute of this record attributes object.

    const bdesb_MemOutStreamBuf& messageStreamBuf() const;
        // Return a reference to the non-modifiable stream buffer associated
        // with the message attribute of this record attributes object.

    bsl::ostream& print(bsl::ostream& stream,
                        int           level = 0,
                        int           spacesPerLevel = 4) const;
        // Format this object to the specified output 'stream' at the
        // optionally specified indentation 'level' and return a reference to
        // the modifiable 'stream'.  If 'level' is specified, optionally
        // specify 'spacesPerLevel', the number of spaces per indentation level
        // for this and all of its nested objects.  Each line is indented by
        // the absolute value of 'level * spacesPerLevel'.  If 'level' is
        // negative, suppress indentation of the first line.  If
        // 'spacesPerLevel' is negative, suppress line breaks and format the
        // entire output on one line.  If 'stream' is initially invalid, this
        // operation has no effect.

    template <class STREAM>
    STREAM& bdexStreamOut(STREAM& stream, int version) const;
        // Write this value to the specified output 'stream' and return a
        // reference to the modifiable 'stream'.  Optionally specify an
        // explicit 'version' format; by default, the maximum supported version
        // is written to 'stream' and used as the format.  If 'version' is
        // specified, that format is used but *not* written to 'stream'.  If
        // 'version' is not supported, 'stream' is left unmodified.  (See
        // the package-group-level documentation for more information on 'bdex'
        // streaming of value-semantic types and containers.)
};

// FREE OPERATORS
bool operator==(const bael_RecordAttributes& lhs,
                const bael_RecordAttributes& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' record attributes objects
    // have the same value, and 'false' otherwise.  Two record attributes
    // objects have the same value if each respective pair of attributes have
    // the same value.

inline
bool operator!=(const bael_RecordAttributes& lhs,
               const bael_RecordAttributes& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' record attributes objects
    // do not have the same value, and 'false' otherwise.  Two record
    // attributes objects do not have the same value if one or more respective
    // attributes differ in value.

inline
bsl::ostream& operator<<(bsl::ostream&                stream,
                         const bael_RecordAttributes& attributes);
    // Format the members of the specified 'attributes' to the specified
    // output 'stream' and return a reference to the modifiable 'stream'.

// ============================================================================
//                          INLINE FUNCTION DEFINITIONS
// ============================================================================

                        // ---------------------------
                        // class bael_RecordAttributes
                        // ---------------------------

// CLASS METHODS
inline
int bael_RecordAttributes::maxSupportedBdexVersion()
{
    return 1;  // required by BDE policy; versions start at 1
}

inline
int bael_RecordAttributes::maxSupportedVersion()
{
    return maxSupportedBdexVersion();
}

// CREATORS
inline
bael_RecordAttributes::~bael_RecordAttributes()
{
}

// MANIPULATORS
inline
void bael_RecordAttributes::clearMessage()
{
    d_messageStreamBuf.pubseekpos(0);
}

inline
bdesb_MemOutStreamBuf& bael_RecordAttributes::messageStreamBuf()
{
    return d_messageStreamBuf;
}

inline
void bael_RecordAttributes::setCategory(const char *category)
{
    d_category = category;
}

inline
void bael_RecordAttributes::setFileName(const char *fileName)
{
    d_fileName = fileName;
}

inline
void bael_RecordAttributes::setLineNumber(int lineNumber)
{
    d_lineNumber = lineNumber;
}

inline
void bael_RecordAttributes::setProcessID(int processID)
{
    d_processID = processID;
}

inline
void bael_RecordAttributes::setSeverity(int severity)
{
    d_severity = severity;
}

inline
void bael_RecordAttributes::setThreadID(bsls_PlatformUtil::Uint64 threadID)
{
    d_threadID = threadID;
}

inline
void bael_RecordAttributes::setTimestamp(const bdet_Datetime& timestamp)
{
    d_timestamp = timestamp;
}

template <class STREAM>
STREAM& bael_RecordAttributes::bdexStreamIn(STREAM& stream, int version)
{
    switch(version) {  // switch on RecordAttributes version (starting with 1)
      case 1: {
        d_timestamp.bdexStreamIn(stream, 1);
        if (!stream) {
            return stream;                                            // RETURN
        }

        stream.getInt32(d_processID);

        // TBD: d_threadID is 64 bit.  Temporarily we are just streaming the
        // low order 32 bits of it.  Eventually stream all 64 bits.
        int tmp;
        stream.getInt32(tmp);
        d_threadID = tmp;

        stream.getString(d_fileName);
        if (!stream) {
            return stream;                                            // RETURN
        }

        stream.getInt32(d_lineNumber);
        if (!stream) {
            return stream;                                            // RETURN
        }

        stream.getString(d_category);
        if (!stream) {
            return stream;                                            // RETURN
        }

        stream.getInt32(d_severity);
        if (!stream) {
            return stream;                                            // RETURN
        }

        d_messageStreamBuf.pubseekpos(0);
        int len;
        stream.getInt32(len);
        if (!stream) {
            return stream;                                            // RETURN
        }

        char c;
        while(stream && len--) {
            stream.getInt8(c);
            if (!stream) {
                return stream;                                        // RETURN
            }
            d_messageStreamBuf.sputc(c);
        }

      } break;
      default: {
        stream.invalidate();  // unrecognized version number
      } break;
    }
    return stream;
}

// ACCESSORS
inline
const char *bael_RecordAttributes::category() const
{
    return d_category.c_str();
}

inline
const char *bael_RecordAttributes::fileName() const
{
    return d_fileName.c_str();
}

inline
int bael_RecordAttributes::lineNumber() const
{
    return d_lineNumber;
}

inline
int bael_RecordAttributes::processID() const
{
    return d_processID;
}

inline
int bael_RecordAttributes::severity() const
{
    return d_severity;
}

inline
bsls_PlatformUtil::Uint64 bael_RecordAttributes::threadID() const
{
    return d_threadID;
}

inline
const bdesb_MemOutStreamBuf& bael_RecordAttributes::messageStreamBuf() const
{
    return d_messageStreamBuf;
}

inline
const bdet_Datetime& bael_RecordAttributes::timestamp() const
{
    return d_timestamp;
}

template <class STREAM>
STREAM& bael_RecordAttributes::bdexStreamOut(STREAM& stream, int version) const
{
    switch (version) {
      case 1: {
        d_timestamp.bdexStreamOut(stream, 1);
        stream.putInt32(d_processID);

        // TBD: d_threadID is 64 bit.  Temporarily, for compatibility, we are
        // just streaming the low order 32 bits.  Eventually stream all 64
        // bits.
        stream.putInt32((int) d_threadID);

        stream.putString(d_fileName);

        stream.putInt32(d_lineNumber);

        stream.putString(d_category);

        stream.putInt32(d_severity);

        const int len = static_cast<int>(d_messageStreamBuf.length());
        const char *data = d_messageStreamBuf.data();

        stream.putInt32(len);
        for (int i = 0; i < len; ++i) {
            stream.putInt8(data[i]);
        }

        // COMMENT NEEDED: Would it be wrong to use the following instead (but
        // using following breaks one test case to my surprise!!)
        //         stream.putInt32(len);
        //         stream.putArrayInt8(data, len);

      } break;
      default: {
      } break;
    }
    return stream;
}

// FREE OPERATORS
inline
bool operator!=(const bael_RecordAttributes& lhs,
                const bael_RecordAttributes& rhs)
{
    return !(lhs == rhs);
}

inline
bsl::ostream& operator<<(bsl::ostream&                stream,
                         const bael_RecordAttributes& rhs)
{
    return rhs.print(stream, 0, -1);
}

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2004
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
