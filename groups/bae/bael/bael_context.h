// bael_context.h                                                     -*-C++-*-
#ifndef INCLUDED_BAEL_CONTEXT
#define INCLUDED_BAEL_CONTEXT

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide a container for the context of a transmitted log record.
//
//@CLASSES:
//    bael_Context: context of a transmitted log record
//
//@SEE_ALSO: bael_transmission, bael_observer, bael_record
//
//@AUTHOR: Hong Shi (hshi2)
//
//@DESCRIPTION: This component defines a container for aggregating a message's
// publication cause, as well as the record (or message) index and sequence
// length of messages delivered as part of a message sequence.  Note that
// messages that are not part of a sequence (i.e., PASSTHROUGH) will have the
// index and sequence length fields set to 0 and 1, respectively.
//
// The context attributes held by 'bael_Context' are detailed in the following
// table:
//..
//  Attribute          Type                      Description        Default
//  -----------------  ------------------------  -----------------  -----------
//  transmissionCause  bael_Transmission::Cause  cause of output    PASSTHROUGH
//  recordIndex        int                       index in sequence  0
//  sequenceLength     int                       # records in seq.  1
//..
///CONSTRAINTS
///-----------
// This attribute class assumes that the following constraints on contained
// values hold:
//..
//    if (bael_Transmission::BAEL_PASSTHROUGH == transmissionCause()) {
//        assert(0 == recordIndex());
//        assert(1 == sequenceLength());
//    }
//    else {
//        assert(
//          bael_Transmission::BAEL_TRIGGER            == transmissionCause()
//       || bael_Transmission::BAEL_TRIGGER_ALL        == transmissionCause()
//       || bael_Transmission::BAEL_MANUAL_PUBLISH     == transmissionCause()
//       || bael_Transmission::BAEL_MANUAL_PUBLISH_ALL == transmissionCause());
//        assert(0 <= recordIndex());
//        assert(1 <= sequenceLength());
//        assert(recordIndex() < sequenceLength());
//    }
//..
// A static 'isValid' method is provided to verify that particular
// 'transmissionCause', 'recordIndex', and 'sequenceLength' values are valid
// before they are used to create or (unilaterally) modify a context object.
//
///Usage
///-----
// A 'bael_Context' object holds sufficient information to determine the length
// of a message sequence and the index of a message within that sequence.  In
// addition, 'bael_Context' indicates the cause for the transmission of a
// message.  The following example illustrates the essentials of working with
// these contextual attributes.
//
// This example illustrates the use of 'bael_Context' by a hypothetical
// logging system.  First we define a simple logger class named 'my_Logger':
//..
//    // my_logger.h
//
//    #include <string>
//    #include <vector>
//
//    class my_Logger {
//
//        bsl::vector<bsl::string> archive;  // log message archive
//
//        // NOT IMPLEMENTED
//        my_Logger(const my_Logger&);
//        my_Logger& operator=(const my_Logger&);
//
//        // PRIVATE MANIPULATORS
//        void publish(const bsl::string&  message,
//                     const bael_Context& context);
//
//      public:
//        // TYPES
//        enum Severity { ERROR = 0, WARN = 1, TRACE = 2 };
//
//        // CREATORS
//        my_Logger();
//        ~my_Logger();
//
//        // MANIPULATORS
//        void logMessage(const bsl::string& message, Severity severity);
//    };
//..
// Clients of 'my_Logger' log messages at one of three severity levels through
// the 'logMessage' method.  Messages logged with 'TRACE' severity are simply
// archived by 'my_Logger'.  Messages logged with 'WARN' severity are archived
// and also output to 'stdout' (say, to a console terminal overseen by an
// operator) through the 'publish' method.  Messages logged with 'ERROR'
// severity report serious conditions; these trigger a dump of the backlog of
// messages that 'my_Logger' has archived to that point.  The 'bael_Context'
// argument passed to 'publish' provides contextual information regarding the
// message it is being asked to publish.
//
// A complete implementation of this trivial logger follows:
//..
//    // my_Logger.cpp
//
//    // PRIVATE MANIPULATORS
//    void my_Logger::publish(const bsl::string&  message,
//                            const bael_Context& context)
//    {
//        using namespace std;
//
//        switch (context.transmissionCause()) {
//          case bael_Transmission::BAEL_PASSTHROUGH: {
//            cout << "Single Pass-through Message: ";
//          } break;
//          case bael_Transmission::BAEL_TRIGGER_ALL: {
//            cout << "Remotely ";               // no 'break'; concatenated
//                                               // output
//          } break;
//          case bael_Transmission::BAEL_TRIGGER: {
//            cout << "Triggered Publication Sequence: Message "
//                 << context.recordIndex() + 1  // Account for 0-based index.
//                 << " of " << context.sequenceLength() << ": ";
//          } break;
//          case bael_Transmission::BAEL_MANUAL_PUBLISH: {
//            cout << "Manually triggered Message: ";
//          } break;
//          default: {
//            cout << "***ERROR*** Unsupported Message Cause: ";
//            return;
//          } break;
//        }
//        cout << message << endl;
//    }
//
//    // CREATORS
//    my_Logger::my_Logger() { }
//    my_Logger::~my_Logger() { }
//
//    // MANIPULATORS
//    void my_Logger::logMessage(const bsl::string& message, Severity severity)
//    {
//        archive.append(message);
//        switch (severity) {
//          case TRACE: {
//            // Do nothing beyond archiving the message.
//          } break;
//          case WARN: {
//            bael_Context context(bael_Transmission::BAEL_PASSTHROUGH, 0, 1);
//            publish(message, context);
//          } break;
//          case ERROR: {
//            int index  = 0;
//            int length = archive.length();
//            bael_Context context(bael_Transmission::BAEL_TRIGGER,
//                                 index, length);
//            while (length--) {
//                publish(archive[length], context);
//                context.setRecordIndexRaw(++index);
//            }
//            archive.removeAll();  // flush archive
//          } break;
//        }
//    }
//..
// Note that 'bael_Transmission::BAEL_TRIGGER_ALL' is not used by 'my_Logger',
// but is included in the switch statement for completeness.
//
// Finally, we declare a 'my_Logger' named 'logger' and simulate the logging of
// several messages of varying severity:
//..
//      my_Logger   logger;
//      bsl::string message;
//
//      message = "TRACE 1";  logger.logMessage(message, my_Logger::TRACE);
//      message = "TRACE 2";  logger.logMessage(message, my_Logger::TRACE);
//      message = "WARNING";  logger.logMessage(message, my_Logger::WARN);
//      message = "TRACE 3";  logger.logMessage(message, my_Logger::TRACE);
//      message = "TROUBLE!"; logger.logMessage(message, my_Logger::ERROR);
//..
// The following output is produced on 'stdout':
//..
//      Single Pass-through Message: WARNING
//      Triggered Publication Sequence: Message 1 of 5: TROUBLE!
//      Triggered Publication Sequence: Message 2 of 5: TRACE 3
//      Triggered Publication Sequence: Message 3 of 5: WARNING
//      Triggered Publication Sequence: Message 4 of 5: TRACE 2
//      Triggered Publication Sequence: Message 5 of 5: TRACE 1
//..
// Note that the warning message (severity 'WARN') was emitted first since the
// trace messages (severity 'TRACE') were simply archived.  When the error
// message (severity 'ERROR') was logged, it triggered a dump of the complete
// message archive (in reverse order).

#ifndef INCLUDED_BAESCM_VERSION
#include <baescm_version.h>
#endif

#ifndef INCLUDED_BAEL_TRANSMISSION
#include <bael_transmission.h>
#endif

#ifndef INCLUDED_BSLS_PLATFORM
#include <bsls_platform.h>
#endif

#ifndef INCLUDED_BSL_IOSFWD
#include <bsl_iosfwd.h>
#endif

#ifndef INCLUDED_BSLFWD_BSLMA_ALLOCATOR
#include <bslfwd_bslma_allocator.h>
#endif

#ifndef BDE_OMIT_INTERNAL_DEPRECATED
#if defined(BSLS_PLATFORM_CMP_MSVC) && defined(PASSTHROUGH)
    // Note: on Windows -> WinGDI.h:#define PASSTHROUGH 19
#undef PASSTHROUGH
#endif
#endif // BDE_OMIT_INTERNAL_DEPRECATED

namespace BloombergLP {

                        // ==================
                        // class bael_Context
                        // ==================

class bael_Context {
    // This class provides a container for aggregating the auxiliary
    // information needed to transmit a log record.  For each context attribute
    // in this class (e.g., 'recordIndex'), there is an accessor for obtaining
    // the attribute's value ('recordIndex') and there are manipulators for
    // changing the contained attribute values ('setAttributes' checks
    // attribute constraints; 'setAttributesRaw' and 'setRecordIndexRaw' do
    // not).  A static 'isValid' method is also provided to verify that
    // particular attribute values are consistent before they are used to
    // create or modify a context object.  Note that it is the client's
    // responsibility not to construct or unilaterally modify a context object
    // to hold incompatible attribute values.
    //
    // Additionally, this class supports a complete set of *value* *semantic*
    // operations, including copy construction, assignment and equality
    // comparison, 'ostream' printing, and 'bdex' serialization.  A precise
    // operational definition of when two instances have the same value can be
    // found in the description of 'operator==' for the class.  This class is
    // *exception* *neutral* with no guarantee of rollback: if an exception is
    // thrown during the invocation of a method on a pre-existing instance, the
    // object is left in a valid state, but its value is undefined.  In no
    // event is memory leaked.  Finally, *aliasing* (e.g., using all or part of
    // an object as both source and destination) is supported in all cases.

    // DATA
    bael_Transmission::Cause d_transmissionCause;  // cause of transmitted
                                                   // record

    int                      d_recordIndex;        // 0-based index within
                                                   // sequence

    int                      d_sequenceLength;     // number of records in
                                                   // sequence

    // PRIVATE TYPES
    enum { BAEL_SUCCESS = 0, BAEL_FAILURE = -1 };

    // FRIENDS
    friend bool operator==(const bael_Context&, const bael_Context&);

  public:
    // CLASS METHODS
    static bool isValid(bael_Transmission::Cause transmissionCause,
                        int                      recordIndex,
                        int                      sequenceLength);
        // Return 'true' if the specified 'transmissionCause', 'recordIndex',
        // and 'sequenceLength' represent a valid context, and 'false'
        // otherwise.  (See the CONSTRAINTS section of the component-level
        // documentation above for a complete specification of the constraints
        // on attribute values.)

    static int maxSupportedBdexVersion();
        // Return the most current 'bdex' streaming version number supported by
        // this class.  (See the package-group-level documentation for more
        // information on 'bdex' streaming of container types.)

    static int maxSupportedVersion();
        // Return the most current 'bdex' streaming version number supported by
        // this class.  (See the package-group-level documentation for more
        // information on 'bdex' streaming of container types.)
        //
        // DEPRECATED: replaced by 'maxSupportedBdexVersion'.

    // CREATORS
    bael_Context(bslma_Allocator *basicAllocator = 0);
        // Create a context object with all attributes having default values.
        // Optionally specify a 'basicAllocator' used to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.

    bael_Context(bael_Transmission::Cause  transmissionCause,
                 int                       recordIndex,
                 int                       sequenceLength,
                 bslma_Allocator          *basicAllocator = 0);
        // Create a context object indicating the specified
        // 'transmissionCause', 'recordIndex', and 'sequenceLength' values.
        // Optionally specify a 'basicAllocator' used to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.  The behavior is undefined if the resulting attribute values
        // are incompatible.

    bael_Context(const bael_Context&  original,
                 bslma_Allocator     *basicAllocator = 0);
        // Create a context object having the value of the specified 'original'
        // context object.  Optionally specify a 'basicAllocator' used to
        // supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.

    // ~bael_Context();
        // Destroy this context object.  Note that this trivial destructor is
        // generated by the compiler.

    // MANIPULATORS
    bael_Context& operator=(const bael_Context& rhs);
        // Assign to this context object the value of the specified 'rhs'
        // context object.

    int setAttributes(bael_Transmission::Cause transmissionCause,
                      int                      recordIndex,
                      int                      sequenceLength);
        // Set the value of this context object to the specified
        // 'transmissionCause', 'recordIndex', and 'sequenceLength' values
        // if 'transmissionCause', 'recordIndex', and 'sequenceLength'
        // represent a valid context.  Return 0 on success, and a non-zero
        // value (with no effect on this context object) otherwise.

    void setAttributesRaw(bael_Transmission::Cause transmissionCause,
                          int                      recordIndex,
                          int                      sequenceLength);
        // Set the value of this context object to the specified
        // 'transmissionCause', 'recordIndex', and 'sequenceLength' values.
        // The behavior is undefined if the resulting attribute values are
        // incompatible.

    void setRecordIndexRaw(int index);
        // Set the record index attribute of this context object to the
        // specified 'index'.  The behavior is undefined if the resulting
        // attribute values are incompatible.

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
        // information on 'bdex' streaming of container types.)

    // ACCESSORS
    bael_Transmission::Cause transmissionCause() const;
        // Return the transmission cause attribute of this context object.

    int recordIndex() const;
        // Return the record index attribute of this context object.

    int sequenceLength() const;
        // Return the sequence length attribute of this context object.

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
        // 'version' is not supported, 'stream' is left unmodified.  (See the
        // package-group-level documentation for more information on 'bdex'
        // streaming of container types).
};

// FREE OPERATORS
inline
bool operator==(const bael_Context& lhs, const bael_Context& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' context objects have the
    // same value, and 'false' otherwise.  Two context objects have the same
    // value if each respective pair of corresponding attributes have the same
    // value.

inline
bool operator!=(const bael_Context& lhs, const bael_Context& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' context objects do not
    // have the same value, and 'false' otherwise.  Two context objects do not
    // have the same value if one or more respective attributes differ in
    // value.

inline
bsl::ostream& operator<<(bsl::ostream& stream, const bael_Context& rhs);
    // Write the specified 'rhs' context to the specified output 'stream' and
    // return a reference to the modifiable 'stream'.

// ============================================================================
//                          INLINE FUNCTION DEFINITIONS
// ============================================================================

                        // ------------------
                        // class bael_Context
                        // ------------------

// CLASS METHODS
inline
int bael_Context::maxSupportedBdexVersion()
{
    return 1;  // Required by BDE policy; versions start at 1.
}

inline
int bael_Context::maxSupportedVersion()
{
    return maxSupportedBdexVersion();
}

// CREATORS
inline
bael_Context::bael_Context(bslma_Allocator *basicAllocator)
: d_transmissionCause(bael_Transmission::BAEL_PASSTHROUGH)
, d_recordIndex(0)
, d_sequenceLength(1)
{
    (void) basicAllocator;
}

inline
bael_Context::bael_Context(bael_Transmission::Cause  transmissionCause,
                           int                       recordIndex,
                           int                       sequenceLength,
                           bslma_Allocator          *basicAllocator)
: d_transmissionCause(transmissionCause)
, d_recordIndex(recordIndex)
, d_sequenceLength(sequenceLength)
{
    (void) basicAllocator;
}

inline
bael_Context::bael_Context(const bael_Context&  original,
                           bslma_Allocator     *basicAllocator)
: d_transmissionCause(original.d_transmissionCause)
, d_recordIndex(original.d_recordIndex)
, d_sequenceLength(original.d_sequenceLength)
{
    (void) basicAllocator;
}

// MANIPULATORS
inline
bael_Context& bael_Context::operator=(const bael_Context& rhs)
{
    d_transmissionCause = rhs.d_transmissionCause;
    d_recordIndex       = rhs.d_recordIndex;
    d_sequenceLength    = rhs.d_sequenceLength;
    return *this;
}

inline
void bael_Context::setAttributesRaw(bael_Transmission::Cause transmissionCause,
                                    int                      recordIndex,
                                    int                      sequenceLength)
{
    d_transmissionCause = transmissionCause;
    d_recordIndex       = recordIndex;
    d_sequenceLength    = sequenceLength;
}

inline
int bael_Context::setAttributes(bael_Transmission::Cause transmissionCause,
                                int                      recordIndex,
                                int                      sequenceLength)
{
    if (isValid(transmissionCause, recordIndex, sequenceLength)) {
        setAttributesRaw(transmissionCause, recordIndex, sequenceLength);
        return BAEL_SUCCESS;
    }
    return BAEL_FAILURE;
}

inline
void bael_Context::setRecordIndexRaw(int index)
{
    d_recordIndex = index;
}

template <class STREAM>
STREAM& bael_Context::bdexStreamIn(STREAM& stream, int version)
{
    switch(version) {  // Switch on Context version (starting with 1).
      case 1: {
        bael_Transmission::Cause readCause;
        bael_Transmission::bdexStreamIn(stream, readCause, version);

        if (!stream) {
            return stream;                                            // RETURN
        }

        int readIndex;
        stream.getInt32(readIndex);

        if (!stream) {
            return stream;                                            // RETURN
        }

        int readLength;
        stream.getInt32(readLength);

        if (!stream) {
            return stream;                                            // RETURN
        }

        if (isValid(readCause, readIndex, readLength)) {
            setAttributesRaw(readCause, readIndex, readLength);
        }
        else {
            stream.invalidate();
        }
      } break;
      default: {
        stream.invalidate();          // unrecognized version number
      } break;
    }
    return stream;
}

// ACCESSORS
inline
bael_Transmission::Cause bael_Context::transmissionCause() const
{
    return d_transmissionCause;
}

inline
int bael_Context::recordIndex() const
{
    return d_recordIndex;
}

inline
int bael_Context::sequenceLength() const
{
    return d_sequenceLength;
}

template <class STREAM>
STREAM& bael_Context::bdexStreamOut(STREAM& stream, int version) const
{
    switch (version) {
      case 1: {
        bael_Transmission::bdexStreamOut(stream, d_transmissionCause, version);
        stream.putInt32(d_recordIndex);
        stream.putInt32(d_sequenceLength);
      } break;
    }
    return stream;
}

// FREE OPERATORS
inline
bool operator==(const bael_Context& lhs, const bael_Context& rhs)
{
    return lhs.d_transmissionCause == rhs.d_transmissionCause
        && lhs.d_recordIndex       == rhs.d_recordIndex
        && lhs.d_sequenceLength    == rhs.d_sequenceLength;
}

inline
bool operator!=(const bael_Context& lhs, const bael_Context& rhs)
{
    return !(lhs == rhs);
}

inline
bsl::ostream& operator<<(bsl::ostream& stream, const bael_Context& rhs)
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
