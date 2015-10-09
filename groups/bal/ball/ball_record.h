// ball_record.h                                                      -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

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
//@SEE_ALSO: ball_recordattributes, ball_logger
//
//@DESCRIPTION: This component defines a container, 'ball::Record', that
// aggregates a set of fixed fields and a set of user-defined fields into one
// record type, useful for transmitting a customized log record as a single
// instance rather than passing around individual attributes separately.  Note
// that this class is a pure attribute class with no constraints, other than
// the total memory required for the class.  Also note that this class is not
// thread-safe.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Basic Use of 'ball::Record'
/// - - - - - - - - - - - - - - - - - - -
// The following example demonstrates how to create and set the properties of
// a 'ball::Record'.  Note that users of the 'ball' logging subsystem are not
// expected to create records directly.
//
// First we default create a 'ball::Record', 'record', and verify it has a
// default set of attributes:
//..
//  ball::Record record;
//
//  ASSERT(ball::RecordAttributes() == record.fixedFields());
//  ASSERT(0                        == record.userFields().length());
//..
// Then, we set the fixed fields of the record to contain a simple message:
//..
//  int                 processId = bdlsu::ProcessUtil::getProcessId();
//  bsls::Types::Uint64 threadId  = bslmt::ThreadUtil::selfIdAsUint64();
//
//  ball::RecordAttributes attributes(bdlt::CurrentTime::utc(), // time stamp
//                                    processId,                // process id
//                                    threadId,                 // thread id
//                                    __FILE__,                 // filename
//                                    __LINE__,                 // line number
//                                    "ExampleCategory",        // category
//                                    ball::Severity::e_WARN,   // severity
//                                    "Simple Test Message");   // message
//  record.setFixedFields(attributes);
//
//  ASSERT(attributes == record.fixedFields());
//..
// Finally, we write the record to a stream:
//..
//  bsl::ostringstream output;
//  output << record << bsl::endl;
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

#ifndef INCLUDED_BALL_USERFIELDS
#include <ball_userfields.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITS
#include <bslalg_typetraits.h>
#endif

#ifndef INCLUDED_BSLMA_ALLOCATOR
#include <bslma_allocator.h>
#endif

#ifndef INCLUDED_BSLMA_USESBSLMAALLOCATOR
#include <bslma_usesbslmaallocator.h>
#endif

#ifndef INCLUDED_BSLMF_NESTEDTRAITDECLARATION
#include <bslmf_nestedtraitdeclaration.h>
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
                           // ============
                           // class Record
                           // ============

class Record {
    // This class provides a container for a set of fields that are appropriate
    // for a user-configurable log record.  The class contains a
    // 'RecordAttributes' object that in turn holds a fixed set of fields, and
    // a 'ball::UserFields' object that holds a set of optional, user-defined
    // fields.  For each of these two sub-containers there is an accessor for
    // obtaining the container value and a manipulator for changing that value.
    //
    // Additionally, this class supports a complete set of *value* *semantic*
    // operations, including copy construction, assignment and equality
    // comparison, and 'ostream' printing.  A precise operational definition of
    // when two instances have the same value can be found in the description
    // of 'operator==' for the class.  This class is *exception* *neutral* with
    // no guarantee of rollback: If an exception is thrown during the
    // invocation of a method on a pre-existing instance, the object is left in
    // a valid state, but its value is undefined.  In no event is memory
    // leaked.  Finally, *aliasing* (e.g., using all or part of an object as
    // both source and destination) is supported in all cases.

    // DATA
    CountingAllocator  d_allocator;    // memory allocator

    RecordAttributes   d_fixedFields;  // bytes used by fixed fields

    ball::UserFields   d_userFields;   // bytes used by user fields

    bslma::Allocator  *d_allocator_p;  // allocator used to supply memory;
                                       // held but not own

    // FRIENDS
    friend bool operator==(const Record&, const Record&);

  public:

    // CLASS METHODS
    static void deleteObject(const Record *object);
        // Destroy the specified '*object' and use the allocator held by
        // '*object' to deallocate its memory footprint.  The behavior is
        // undefined unless 'object' is the address of a valid log record.


    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(Record, bslma::UsesBslmaAllocator);

    // CREATORS
    explicit Record(bslma::Allocator *basicAllocator = 0);
        // Create a log record having default values for its fixed fields and
        // its user-defined fields.  Optionally specify a 'basicAllocator'
        // used to supply memory.  If 'basicAllocator' is 0, the currently
        // installed default allocator is used.

    Record(const RecordAttributes&  fixedFields,
           const ball::UserFields&  userFields,
           bslma::Allocator        *basicAllocator = 0);
        // Create a log record with fixed fields having the value of the
        // specified 'fixedFields' and user-defined fields having the value of
        // the specified 'userFields'.  Optionally specify a 'basicAllocator'
        // used to supply memory.  If 'basicAllocator' is 0, the currently
        // installed default allocator is used.

    Record(const Record&     original,
           bslma::Allocator *basicAllocator = 0);
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

    void setUserFields(const ball::UserFields& userFields);
        // Set the user-defined fields of this log record to the value of the
        // specified 'userFields'.

    ball::UserFields& userFields();
        // Return the modifiable user-defined fields of this log record.

    // ACCESSORS
    const RecordAttributes& fixedFields() const;
        // Return the non-modifiable fixed fields of this log record.

    const ball::UserFields& userFields() const;
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

};

// FREE OPERATORS
bool operator==(const Record& lhs, const Record& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' log records have the same
    // value, and 'false' otherwise.  Two log records have the same value if
    // the respective fixed fields have the same value and the respective
    // user-defined fields have the same value.

bool operator!=(const Record& lhs, const Record& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' log records do not have
    // the same value, and 'false' otherwise.  Two log records do not have the
    // same value if either the respective fixed fields or user-defined fields
    // do not have the same value.

bsl::ostream& operator<<(bsl::ostream& stream, const Record& record);
    // Format the members of the specified 'record' to the specified output
    // 'stream' and return a reference to the modifiable 'stream'.

// ============================================================================
//                              INLINE DEFINITIONS
// ============================================================================

                           // ------------
                           // class Record
                           // ------------

// CLASS METHODS
inline
void Record::deleteObject(const Record *object)
{
    object->d_allocator_p->deleteObjectRaw(object);
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
               const ball::UserFields&  userFields,
               bslma::Allocator        *basicAllocator)
: d_allocator(basicAllocator)
, d_fixedFields(fixedFields, &d_allocator)
, d_userFields(userFields, &d_allocator)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

inline
Record::Record(const Record&     original,
               bslma::Allocator *basicAllocator)
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
void Record::setUserFields(const ball::UserFields& userFields)
{
    d_userFields = userFields;
}

inline
ball::UserFields& Record::userFields()
{
    return d_userFields;
}

// ACCESSORS
inline
const RecordAttributes& Record::fixedFields() const
{
    return d_fixedFields;
}

inline
const ball::UserFields& Record::userFields() const
{
    return d_userFields;
}

inline
int Record::numAllocatedBytes() const
{
    return d_allocator.numBytesTotal();
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
