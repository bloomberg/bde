// ball_context.h                                                     -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#ifndef INCLUDED_BALL_CONTEXT
#define INCLUDED_BALL_CONTEXT

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a container for the context of a transmitted log record.
//
//@CLASSES:
//  ball::Context: context of a transmitted log record
//
//@SEE_ALSO: ball_transmission, ball_observer, ball_record
//
//@DESCRIPTION: This component defines a container for aggregating a message's
// publication cause, as well as the record (or message) index and sequence
// length of messages delivered as part of a message sequence.  Note that
// messages that are not part of a sequence (i.e., PASSTHROUGH) will have the
// index and sequence length fields set to 0 and 1, respectively.
//
// The context attributes held by 'ball::Context' are detailed in the following
// table:
//..
//  Attribute          Type                      Description        Default
//  -----------------  ------------------------  -----------------  -----------
//  transmissionCause  ball::Transmission::Cause cause of output    PASSTHROUGH
//  recordIndex        int                       index in sequence  0
//  sequenceLength     int                       # records in seq.  1
//..
//
///Constraints
///-----------
// This attribute class assumes that the following constraints on contained
// values hold:
//..
//    if (ball::Transmission::e_PASSTHROUGH == transmissionCause()) {
//        assert(0 == recordIndex());
//        assert(1 == sequenceLength());
//    }
//    else {
//        assert(
//         ball::Transmission::e_TRIGGER            == transmissionCause()
//      || ball::Transmission::e_TRIGGER_ALL        == transmissionCause()
//      || ball::Transmission::e_MANUAL_PUBLISH     == transmissionCause()
//      || ball::Transmission::e_MANUAL_PUBLISH_ALL == transmissionCause());
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
// A 'ball::Context' object holds sufficient information to determine the
// length of a message sequence and the index of a message within that
// sequence.  In addition, 'ball::Context' indicates the cause for the
// transmission of a message.  The following example illustrates the essentials
// of working with these contextual attributes.
//
// This example illustrates the use of 'ball::Context' by a hypothetical
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
//                     const ball::Context& context);
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
// messages that 'my_Logger' has archived to that point.  The 'ball::Context'
// argument passed to 'publish' provides contextual information regarding the
// message it is being asked to publish.
//
// A complete implementation of this trivial logger follows:
//..
//    // my_Logger.cpp
//
//    // PRIVATE MANIPULATORS
//    void my_Logger::publish(const bsl::string&  message,
//                            const ball::Context& context)
//    {
//        using namespace std;
//
//        switch (context.transmissionCause()) {
//          case ball::Transmission::e_PASSTHROUGH: {
//            cout << "Single Pass-through Message: ";
//          } break;
//          case ball::Transmission::e_TRIGGER_ALL: {
//            cout << "Remotely ";               // no 'break'; concatenated
//                                               // output
//          } break;
//          case ball::Transmission::e_TRIGGER: {
//            cout << "Triggered Publication Sequence: Message "
//                 << context.recordIndex() + 1  // Account for 0-based index.
//                 << " of " << context.sequenceLength() << ": ";
//          } break;
//          case ball::Transmission::e_MANUAL_PUBLISH: {
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
//            ball::Context context(ball::Transmission::e_PASSTHROUGH,
//                                  0,
//                                  1);
//            publish(message, context);
//          } break;
//          case ERROR: {
//            int index  = 0;
//            int length = archive.length();
//            ball::Context context(ball::Transmission::e_TRIGGER,
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
// Note that 'ball::Transmission::e_TRIGGER_ALL' is not used by 'my_Logger',
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

#ifndef INCLUDED_BALSCM_VERSION
#include <balscm_version.h>
#endif

#ifndef INCLUDED_BALL_TRANSMISSION
#include <ball_transmission.h>
#endif

#ifndef INCLUDED_BSLMA_USESBSLMAALLOCATOR
#include <bslma_usesbslmaallocator.h>
#endif

#ifndef INCLUDED_BSLMF_NESTEDTRAITDECLARATION
#include <bslmf_nestedtraitdeclaration.h>
#endif

#ifndef INCLUDED_BSLMA_ALLOCATOR
#include <bslma_allocator.h>
#endif

#ifndef INCLUDED_BSLS_PLATFORM
#include <bsls_platform.h>
#endif

#ifndef INCLUDED_BSL_IOSFWD
#include <bsl_iosfwd.h>
#endif


namespace BloombergLP {

namespace ball {
                        // =============
                        // class Context
                        // =============

class Context {
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
    // comparison, and 'ostream' printing.  A precise operational definition of
    // when two instances have the same value can be found in the description
    // of 'operator==' for the class.  This class is *exception* *neutral* with
    // no guarantee of rollback: if an exception is thrown during the
    // invocation of a method on a pre-existing instance, the object is left in
    // a valid state, but its value is undefined.  In no event is memory
    // leaked.  Finally, *aliasing* (e.g., using all or part of an object as
    // both source and destination) is supported in all cases.

    // DATA
    Transmission::Cause d_transmissionCause;  // cause of transmitted record
    int                 d_recordIndex;        // 0-based index within sequence
    int                 d_sequenceLength;     // number of records in sequence

    // PRIVATE TYPES
    enum { k_SUCCESS = 0, k_FAILURE = -1 };

    // FRIENDS
    friend bool operator==(const Context&, const Context&);

  public:
    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(Context, bslma::UsesBslmaAllocator);

    // CLASS METHODS
    static bool isValid(Transmission::Cause transmissionCause,
                        int                 recordIndex,
                        int                 sequenceLength);
        // Return 'true' if the specified 'transmissionCause', 'recordIndex',
        // and 'sequenceLength' represent a valid context, and 'false'
        // otherwise.  (See the CONSTRAINTS section of the component-level
        // documentation above for a complete specification of the constraints
        // on attribute values.)


    // CREATORS
    Context(bslma::Allocator *basicAllocator = 0);
        // Create a context object with all attributes having default values.
        // Optionally specify a 'basicAllocator' used to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.

    Context(Transmission::Cause  transmissionCause,
            int                  recordIndex,
            int                  sequenceLength,
            bslma::Allocator    *basicAllocator = 0);
        // Create a context object indicating the specified
        // 'transmissionCause', 'recordIndex', and 'sequenceLength' values.
        // Optionally specify a 'basicAllocator' used to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.  The behavior is undefined if the resulting attribute values
        // are incompatible.

    Context(const Context&    original,
            bslma::Allocator *basicAllocator = 0);
        // Create a context object having the value of the specified 'original'
        // context object.  Optionally specify a 'basicAllocator' used to
        // supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.

    // ~Context();
        // Destroy this context object.  Note that this trivial destructor is
        // generated by the compiler.

    // MANIPULATORS
    Context& operator=(const Context& rhs);
        // Assign to this context object the value of the specified 'rhs'
        // context object.

    int setAttributes(Transmission::Cause transmissionCause,
                      int                 recordIndex,
                      int                 sequenceLength);
        // Set the value of this context object to the specified
        // 'transmissionCause', 'recordIndex', and 'sequenceLength' values if
        // 'transmissionCause', 'recordIndex', and 'sequenceLength' represent a
        // valid context.  Return 0 on success, and a non-zero value (with no
        // effect on this context object) otherwise.

    void setAttributesRaw(Transmission::Cause transmissionCause,
                          int                 recordIndex,
                          int                 sequenceLength);
        // Set the value of this context object to the specified
        // 'transmissionCause', 'recordIndex', and 'sequenceLength' values.
        // The behavior is undefined if the resulting attribute values are
        // incompatible.

    void setRecordIndexRaw(int index);
        // Set the record index attribute of this context object to the
        // specified 'index'.  The behavior is undefined if the resulting
        // attribute values are incompatible.

    // ACCESSORS
    Transmission::Cause transmissionCause() const;
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

};

// FREE OPERATORS
bool operator==(const Context& lhs, const Context& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' context objects have the
    // same value, and 'false' otherwise.  Two context objects have the same
    // value if each respective pair of corresponding attributes have the same
    // value.

bool operator!=(const Context& lhs, const Context& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' context objects do not
    // have the same value, and 'false' otherwise.  Two context objects do not
    // have the same value if one or more respective attributes differ in
    // value.

bsl::ostream& operator<<(bsl::ostream& stream, const Context& rhs);
    // Write the specified 'rhs' context to the specified output 'stream' and
    // return a reference to the modifiable 'stream'.

// ============================================================================
//                              INLINE DEFINITIONS
// ============================================================================

                        // -------------
                        // class Context
                        // -------------

// CREATORS
inline
Context::Context(bslma::Allocator *basicAllocator)
: d_transmissionCause(Transmission::e_PASSTHROUGH)
, d_recordIndex(0)
, d_sequenceLength(1)
{
    (void) basicAllocator;
}

inline
Context::Context(Transmission::Cause  transmissionCause,
                 int                  recordIndex,
                 int                  sequenceLength,
                 bslma::Allocator    *basicAllocator)
: d_transmissionCause(transmissionCause)
, d_recordIndex(recordIndex)
, d_sequenceLength(sequenceLength)
{
    (void) basicAllocator;
}

inline
Context::Context(const Context&    original,
                 bslma::Allocator *basicAllocator)
: d_transmissionCause(original.d_transmissionCause)
, d_recordIndex(original.d_recordIndex)
, d_sequenceLength(original.d_sequenceLength)
{
    (void) basicAllocator;
}

// MANIPULATORS
inline
Context& Context::operator=(const Context& rhs)
{
    d_transmissionCause = rhs.d_transmissionCause;
    d_recordIndex       = rhs.d_recordIndex;
    d_sequenceLength    = rhs.d_sequenceLength;
    return *this;
}

inline
void Context::setAttributesRaw(Transmission::Cause transmissionCause,
                               int                 recordIndex,
                               int                 sequenceLength)
{
    d_transmissionCause = transmissionCause;
    d_recordIndex       = recordIndex;
    d_sequenceLength    = sequenceLength;
}

inline
int Context::setAttributes(Transmission::Cause transmissionCause,
                           int                 recordIndex,
                           int                 sequenceLength)
{
    if (isValid(transmissionCause, recordIndex, sequenceLength)) {
        setAttributesRaw(transmissionCause, recordIndex, sequenceLength);
        return k_SUCCESS;                                             // RETURN
    }
    return k_FAILURE;
}

inline
void Context::setRecordIndexRaw(int index)
{
    d_recordIndex = index;
}

// ACCESSORS
inline
Transmission::Cause Context::transmissionCause() const
{
    return d_transmissionCause;
}

inline
int Context::recordIndex() const
{
    return d_recordIndex;
}

inline
int Context::sequenceLength() const
{
    return d_sequenceLength;
}

}  // close package namespace

// FREE OPERATORS
inline
bool ball::operator==(const Context& lhs, const Context& rhs)
{
    return lhs.d_transmissionCause == rhs.d_transmissionCause
        && lhs.d_recordIndex       == rhs.d_recordIndex
        && lhs.d_sequenceLength    == rhs.d_sequenceLength;
}

inline
bool ball::operator!=(const Context& lhs, const Context& rhs)
{
    return !(lhs == rhs);
}

inline
bsl::ostream& ball::operator<<(bsl::ostream& stream, const Context& rhs)
{
    return rhs.print(stream, 0, -1);
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
