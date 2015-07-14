// ball_record.h                                                      -*-C++-*-
#ifndef INCLUDED_BALL_RECORD
#define INCLUDED_BALL_RECORD

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a container for all fields of a log record.
//
//@CLASSES:
//  ball::Record: container for fixed and user-defined log record fields
//
//@SEE_ALSO: ball_recordattributes, bdlmxxx_list, bael_logger
//
//@AUTHOR: Hong Shi (hshi2)
//
//@DESCRIPTION: This component defines a container, 'ball::Record', that
// aggregates a set of fixed fields and a set of user-defined fields into one
// record type, useful for transmitting a customized log record as a single
// instance rather than passing around individual attributes separately.  Note
// that this class is a pure attribute class with no constraints, other than
// the total memory required for the class.  Also note that this class is
// not thread-safe.
//
///Usage
///------
//
///Example 1
///- - - - -
// The following example shows the operations necessary for creating a
// 'ball::Record' suitable for logging.  Record creation proceeds in three
// steps:
//..
//    (1) Populate the fixed fields; these fields comprise a
//        'ball::RecordAttributes' "sub-container".
//    (2) Populate the user-specified fields; these fields are aggregated as a
//        'bdlmxxx::List' "sub-container".
//    (3) Create the log record container.
//..
// This example uses a data record that a financial application might create
// for logging.  The application deals with equities and is assumed to
// have a simple logger that merely transmits the value of a 'ball::Record'
// to a log file.
//
// Assume that the application's logger has been supplied with a schema that
// allows receipt of the following five data items pertaining to a given
// equity: TICKER SYMBOL, HIGH, LOW, OPEN, CLOSE.  The following schema
// definition shows the constraints to which the user-defined fields must
// conform:
//..
//      RECORD Equity {
//          STRING ticker;
//          DOUBLE high;
//          DOUBLE low;
//          DOUBLE open;
//          DOUBLE close;
//      }
//
//      Figure 1: Schema for ticker log records.
//..
// We first create a 'bdlmxxx::ElemType::Type' array that matches the schema in
// Figure 1.  Since this array is initialized at load time, it is safe, even in
// the "BIG", to define the array at file scope:
//..
//      static bdlmxxx::ElemType::Type listTypes[5] = { bdlmxxx::ElemType::BDEM_STRING,
//                                                  bdlmxxx::ElemType::BDEM_DOUBLE,
//                                                  bdlmxxx::ElemType::BDEM_DOUBLE,
//                                                  bdlmxxx::ElemType::BDEM_DOUBLE,
//                                                  bdlmxxx::ElemType::BDEM_DOUBLE
//      };
//..
// The log records are created in the following 'logPrices' function:
//..
//      void logPrices(const char *ticker, double low,
//                     double open, double close, double high)
//      {
//..
// First create the fixed fields of the log record.  In this particular
// application, only the timestamp field changes value from record to record,
// so a 'static' instance of 'ball::RecordAttributes' is defined for
// efficiency.  Note that this performance optimizations is suitable only
// for single-threaded applications.
//..
//          static ball::RecordAttributes fixedFields(bdlt::Datetime(),
//                                                   getpid(),
//                                                   0,  // threadID
//                                                   __FILE__,
//                                                   __LINE__,
//                                                   "EQUITY.NASD",
//                                                   ball::Severity::BAEL_INFO,
//                                                   "Ticker Summary");
//          bdlt::Datetime now;
//          bdlt::EpochUtil::convertFromTimeT(&now, time(0));
//          fixedFields.setTimestamp(now);
//..
// Now create the user-defined fields of the log record.  A 'bdlmxxx::List' of
// the appropriate size is defined that conforms to our 'listTypes' array,
// then values are assigned to the individual list elements:
//..
//          static bdlmxxx::List userFields(listTypes, 5);
//
//          userFields.theString(0) = ticker;
//          userFields.theDouble(1) = high;
//          userFields.theDouble(2) = low;
//          userFields.theDouble(3) = open;
//          userFields.theDouble(4) = close;
//..
// Finally, create the log record from the fixed fields and user-defined fields
// that have just been assembled:
//..
//          ball::Record record(fixedFields, userFields);
//..
// Suppose that 'os' is an ostream associated with the log file, than logger
// can log this message as follows:
//..
//           os << message << endl;
//      }
//..
//
///Example 2
///- - - - -
// Following example demonstrates how an object of a class supporting 'ostream'
// operation ('operator<<') can be logged into a log file.
// Suppose we want to log objects of following class.
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
// The following function logs an instance of 'Information' object to
// a log stream.  For simplicity, we won't log user fields.
//..
//    void logInformation(ostream& logStream,
//                        const Information& information,
//                        ball::Severity::Level severity,
//                        const char *category,
//                        const char* fileName,
//                        int lineNumber)
//    {
//        ball::Record record;
//
//        // get the modifiable reference to the fixed fields
//        ball::RecordAttributes& attributes = record.fixedFields();
//
//        // set various attributes
//        bdlt::Datetime now;
//        bdlt::EpochUtil::convertFromTimeT(&now, time(0));
//        attributes.setTimestamp(now);
//
//        attributes.setProcessID(getpid());
//
//        attributes.setThreadID(0);
//
//        attributes.setFileName(fileName);
//
//        attributes.setLineNumber(lineNumber);
//
//        attributes.setCategory(category);
//
//        attributes.setSeverity(severity);
//
//        // create an 'ostream' from message stream buffer
//        ostream os(&attributes.messageStreamBuf());
//
//        // now stream the information object into the created ostream,
//        // this will set the message attribute of 'attributes' to
//        // the streamed contents.
//        os << information;
//
//        // finally log the record into the log stream
//        logStream << record;
//    }
//..
// Following snippets shows how to use 'logInformation' function.
//..
//    Information info("MY-HEADING", "MY-CONTENTS");
//    logInformation(cout,
//                   info,
//                   ball::Severity::BAEL_INFO,
//                   "my-category",
//                   __FILE__,
//                   __LINE__);
//..

#ifndef INCLUDED_BALSCM_VERSION
#include <balscm_version.h>
#endif

#ifndef INCLUDED_BALL_COUNTINGALLOCATOR
#include <ball_countingallocator.h>
#endif

#ifndef INCLUDED_BALL_RECORDATTRIBUTES
#include <ball_recordattributes.h>
#endif

#ifndef INCLUDED_BDLMXXX_LIST
#include <bdlmxxx_list.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITS
#include <bslalg_typetraits.h>
#endif

#ifndef INCLUDED_BSLMA_ALLOCATOR
#include <bslma_allocator.h>
#endif

#ifndef INCLUDED_BSLMA_DEFAULT
#include <bslma_default.h>
#endif

#ifndef INCLUDED_BSLS_ALIGNMENT
#include <bsls_alignment.h>
#endif

#ifndef INCLUDED_BSL_IOSFWD
#include <bsl_iosfwd.h>
#endif

namespace BloombergLP {

namespace ball {
                           // =================
                           // class Record
                           // =================

class Record {
    // This class provides a container for a set of fields that are
    // appropriate for a user-configurable log record.  The class contains a
    // 'RecordAttributes' object that in turn holds a fixed set of
    // fields, and a 'bdlmxxx::List' object that holds a set of optional,
    // user-defined fields.  For each of these two sub-containers there is an
    // accessor for obtaining the container value and a manipulator for
    // changing that value.
    //
    // Additionally, this class supports a complete set of *value* *semantic*
    // operations, including copy construction, assignment and equality
    // comparison, 'ostream' printing, and 'bdex' serialization.  A precise
    // operational definition of when two instances have the same value can be
    // found in the description of 'operator==' for the class.  This class is
    // *exception* *neutral* with no guarantee of rollback: If an exception is
    // thrown during the invocation of a method on a pre-existing instance,
    // the object is left in a valid state, but its value is undefined.  In no
    // event is memory leaked.  Finally, *aliasing* (e.g., using all or part
    // of an object as both source and destination) is supported in all cases.

    // DATA
    CountingAllocator  d_allocator;    // memory allocator

    RecordAttributes   d_fixedFields;  // bytes used by fixed fields

    bdlmxxx::List               d_userFields;   // bytes used by user fields

    bslma::Allocator       *d_allocator_p;  // allocator used to supply
                                            // memory; held but not own

    // FRIENDS
    friend bool operator==(const Record&, const Record&);

  public:
    // CLASS METHODS
    static void deleteObject(const Record *object);
        // Destroy the specified '*object' and use the allocator held by
        // '*object' to deallocate its memory footprint.  The behavior is
        // undefined unless 'object' is the address of a valid log record.

    static int maxSupportedBdexVersion();
        // Return the most current 'bdex' streaming version number supported
        // by this class.  (See the package-group-level documentation for more
        // information on 'bdex' streaming of value-semantic types and
        // containers.)

    static int maxSupportedVersion();
        // Return the most current 'bdex' streaming version number supported
        // by this class.  (See the package-group-level documentation for more
        // information on 'bdex' streaming of value-semantic types and
        // containers.)
        //
        // DEPRECATED: Replaced by the 'maxSupportedBdexVersion' method.

    // TRAITS
    BSLALG_DECLARE_NESTED_TRAITS(Record,
                                 bslalg::TypeTraitUsesBslmaAllocator);

    // CREATORS
    explicit Record(bslma::Allocator *basicAllocator = 0);
        // Create a log record having default values for its fixed fields and
        // its user-defined fields.  Optionally specify a 'basicAllocator'
        // used to supply memory.  If 'basicAllocator' is 0, the currently
        // installed default allocator is used.

    Record(const RecordAttributes&  fixedFields,
                const bdlmxxx::List&              userFields,
                bslma::Allocator             *basicAllocator = 0);
        // Create a log record with fixed fields having the value of the
        // specified 'fixedFields' and user-defined fields having the value of
        // the specified 'userFields'.  Optionally specify a 'basicAllocator'
        // used to supply memory.  If 'basicAllocator' is 0, the currently
        // installed default allocator is used.

    Record(const Record&  original,
                bslma::Allocator   *basicAllocator = 0);
        // Create a log record having the value of the specified 'original'
        // log record.  Optionally specify a 'basicAllocator' used to supply
        // memory.  If 'basicAllocator' is 0, the currently installed default
        // allocator is used.

    ~Record();
        // Destroy this log record.

    // MANIPULATORS
    Record& operator=(const Record& rhs);
        // Assign to this log record the value of the specified 'rhs' log
        // record and return the reference to this modifiable record.

    RecordAttributes& fixedFields();
        // Return the modifiable fixed fields of this log record.

    void setFixedFields(const RecordAttributes& fixedFields);
        // Set the fixed fields of this log record to the value of the
        // specified 'fixedFields'.

    void setUserFields(const bdlmxxx::List& userFields);
        // Set the user-defined fields of this log record to the value of the
        // specified 'userFields'.

    bdlmxxx::List& userFields();
        // Return the modifiable user-defined fields of this log record.

    template <class STREAM>
    STREAM& bdexStreamIn(STREAM& stream, int version);
        // Assign to this object the value read from the specified input
        // 'stream' using the specified 'version' format and return a
        // reference to the modifiable 'stream'.  If 'stream' is initially
        // invalid, this operation has no effect.  If 'stream' becomes invalid
        // during this operation, this object is valid, but its value is
        // undefined.  If 'version' is not supported, 'stream' is marked
        // invalid and this object is unaltered.  Note that no version is read
        // from 'stream'.  See the 'bdex' package-level documentation for more
        // information on 'bdex' streaming of value-semantic types and
        // containers.

    // ACCESSORS
    const RecordAttributes& fixedFields() const;
        // Return the non-modifiable fixed fields of this log record.

    const bdlmxxx::List& userFields() const;
        // Return the non-modifiable user-defined fields of this log record.

    int numAllocatedBytes() const;
        // Return the total number of bytes of dynamic memory allocated by
        // this log record object.  Note that this value does not include
        // 'sizeof *this'.

    bsl::ostream& print(bsl::ostream& stream,
                        int           level = 0,
                        int           spacesPerLevel = 4) const;
        // Format this object to the specified output 'stream' at the
        // optionally specified indentation 'level' and return a reference to
        // the modifiable 'stream'.  If 'level' is specified, optionally
        // specify 'spacesPerLevel', the number of spaces per indentation
        // level for this and all of its nested objects.  Each line is
        // indented by the absolute value of 'level * spacesPerLevel'.  If
        // 'level' is negative, suppress indentation of the first line.  If
        // 'spacesPerLevel' is negative, suppress line breaks and format the
        // entire output on one line.  If 'stream' is initially invalid, this
        // operation has no effect.

    template <class STREAM>
    STREAM& bdexStreamOut(STREAM& stream, int version) const;
        // Write this value to the specified output 'stream' using the
        // specified 'version' format and return a reference to the modifiable
        // 'stream'.  If 'version' is not supported, 'stream' is unmodified.
        // Note that 'version' is not written to 'stream'.  See the 'bdex'
        // package-level documentation for more information on 'bdex'
        // streaming of value-semantic types and containers.
};

// FREE OPERATORS
inline
bool operator==(const Record& lhs, const Record& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' log records have the same
    // value, and 'false' otherwise.  Two log records have the same value if
    // the respective fixed fields have the same value and the respective
    // user-defined fields have the same value.

inline
bool operator!=(const Record& lhs, const Record& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' log records do not have
    // the same value, and 'false' otherwise.  Two log records do not have the
    // same value if either the respective fixed fields or user-defined fields
    // do not have the same value.

inline
bsl::ostream& operator<<(bsl::ostream& stream, const Record& record);
    // Format the members of the specified 'record' to the specified output
    // 'stream' and return a reference to the modifiable 'stream'.
// ===========================================================================
//                        INLINE FUNCTION DEFINITIONS
// ===========================================================================

                           // -----------------
                           // class Record
                           // -----------------

// CLASS METHODS
inline
void Record::deleteObject(const Record *object)
{
    object->d_allocator_p->deleteObjectRaw(object);
}

inline
int Record::maxSupportedBdexVersion()
{
    return 1;  // Required by BDE policy; versions start at 1.
}

inline
int Record::maxSupportedVersion()
{
    return maxSupportedBdexVersion();
}

// CREATORS
inline
Record::Record(bslma::Allocator *basicAllocator)
: d_allocator(basicAllocator)
, d_fixedFields(&d_allocator)
, d_userFields(&d_allocator)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

inline
Record::Record(const RecordAttributes&  fixedFields,
                         const bdlmxxx::List&              userFields,
                         bslma::Allocator             *basicAllocator)
: d_allocator(basicAllocator)
, d_fixedFields(fixedFields, &d_allocator)
, d_userFields(userFields, &d_allocator)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

inline
Record::Record(const Record&  original,
                         bslma::Allocator   *basicAllocator)
: d_allocator(basicAllocator)
, d_fixedFields(original.d_fixedFields, &d_allocator)
, d_userFields(original.d_userFields, &d_allocator)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

inline
Record::~Record()
{
}

// MANIPULATORS
inline
Record& Record::operator=(const Record& rhs)
{
    if (this != &rhs) {
        d_fixedFields = rhs.d_fixedFields;
        d_userFields  = rhs.d_userFields;
    }
    return *this;
}

inline
RecordAttributes& Record::fixedFields()
{
    return d_fixedFields;
}

inline
void Record::setFixedFields(const RecordAttributes& fixedFields)
{
    d_fixedFields = fixedFields;
}

inline
void Record::setUserFields(const bdlmxxx::List& userFields)
{
    d_userFields = userFields;
}

inline
bdlmxxx::List& Record::userFields()
{
    return d_userFields;
}

template <class STREAM>
STREAM& Record::bdexStreamIn(STREAM& stream, int version)
{
    if (stream) {
        switch(version) {  // Switch on Record version (starting with 1).
          case 1: {

            d_fixedFields.bdexStreamIn(stream, 1);
            if (!stream) {
                return stream;                                  // RETURN
            }

            d_userFields.removeAll();
            d_userFields.bdexStreamIn(stream, 1);
            if (!stream) {
                return stream;                                  // RETURN
            }

          } break;
          default: {
            stream.invalidate();          // unrecognized version number
          } break;
        }
    }
    return stream;
}

// ACCESSORS
inline
const RecordAttributes& Record::fixedFields() const
{
    return d_fixedFields;
}

inline
const bdlmxxx::List& Record::userFields() const
{
    return d_userFields;
}

inline
int Record::numAllocatedBytes() const
{
    return d_allocator.numBytesTotal();
}

template <class STREAM>
STREAM& Record::bdexStreamOut(STREAM& stream, int version) const
{
    switch (version) {
      case 1: {
        d_fixedFields.bdexStreamOut(stream, 1);
        d_userFields.bdexStreamOut(stream, 1);
      } break;
    }
    return stream;
}
}  // close package namespace

// FREE OPERATORS
inline
bool ball::operator==(const Record& lhs, const Record& rhs)
{
    return lhs.d_fixedFields == rhs.d_fixedFields
        && lhs.d_userFields  == rhs.d_userFields;
}

inline
bool ball::operator!=(const Record& lhs, const Record& rhs)
{
    return !(lhs == rhs);
}

inline
bsl::ostream& ball::operator<<(bsl::ostream& stream, const Record& record)
{
    return record.print(stream, 0, -1);
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
