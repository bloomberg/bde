// ball_userfields.h                                                  -*-C++-*-
#ifndef INCLUDED_BALL_USERFIELDS
#define INCLUDED_BALL_USERFIELDS

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a container of user supplied field values.
//
//@CLASSES:
//  ball::UserFields: a container of user supplied field values
//
//@SEE_ALSO: ball_userfieldvalue, ball_userfieldsschema
//
//@DESCRIPTION: This component provides a value-semantic container-type,
// 'ball::UserFields', that represents a (randomly accessible) sequence of
// 'ball::UserFieldValue' objects.  Each user field value contained in the
// sequence functions as a discriminated union of the types described by
// 'ball::UserFieldType::Enum' (integer, double, string, etc).  Values can be
// added to the sequence using the 'append*' manipulators, and can be
// manipulated and accessed using 'operator[]'.  Additionally,
// 'ball::UserFields' exposes a random-access iterator providing non-modifiable
// access to the sequence through the 'begin' and 'end' methods.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Basic Use of 'ball::UserFields'
/// - - - - - - - - - - - - - - - - - - - - -
// In the following example we demonstrate populating a 'ball::UserFields'
// object with a sequence of field values.
//
// First, we define the signature for a callback, 'populateUserFields'.  Most
// often 'ball::UserFields' objects are populated by a callback, such as the
// one described by the 'ball::LoggerManagerConfiguration'
// 'UserFieldsPopulatorCallback'.
//..
//  void populateLoggingFields(ball::UserFields              *fields,
//                             const ball::UserFieldsSchema&  fieldsSchema)
//      // Populate the specifield 'fields' with the user name and current
//      // task identifier so that in matches the specified 'fieldsSchema'.
//      // The behavior is undefiend unless 'fields' is empty, and
//      // 'fieldsSchema' describes a user fields object whose fist element is
//      // a string called "username" and whose second element is a integer
//      // called "taskId".
//  {
//..
// Notice that we have decided for this application the schema for the custom
// logging fields are fixed at compile time.
//
// Next, we assert that the schema matches the preconditions for this function:
//..
//    typedef ball::UserFieldType Type;
//    BSLS_ASSERT(2 == fieldsSchema.length());
//    BSLS_ASSERT("username"     == fieldsSchema.name(0));
//    BSLS_ASSERT(Type::e_STRING == fieldsSchema.type(0));
//    BSLS_ASSERT("taskId"       == fieldsSchema.name(1));
//    BSLS_ASSERT(Type::e_INT64  == fieldsSchema.type(1));
//..
// Then we assert the additional precondition that 'fields' is empty:
//..
//    BSLS_ASSERT(0 == fields->length());
//..
// Now we populate the 'fields' object with the user name and current task
// identifier (for the purpose of illustration, these are simply constants):
//..
//    static const char    *TEST_USER = "testUser";
//    static const bsl::int64_t  TEST_TASK = 4315;
//
//    fields->appendString(TEST_USER);
//    fields->appendInt64(TEST_TASK);
//..
// Finally, for the purposes of illustration, we verify that 'fields' has been
// set correctly:
//..
//    assert(2 == fields->length());
//    assert(Type::e_STRING == fields->value(0).type());
//    assert(TEST_USER      == fields->value(0).theString());
//    assert(Type::e_INT64  == fields->value(1).type());
//    assert(TEST_TASK      == fields->value(1).theInt64());
//  }
//..

#ifndef INCLUDED_BALSCM_VERSION
#include <balscm_version.h>
#endif

#ifndef INCLUDED_BALL_USERFIELDVALUE
#include <ball_userfieldvalue.h>
#endif

#ifndef INCLUDED_BSLMA_USESBSLMAALLOCATOR
#include <bslma_usesbslmaallocator.h>
#endif

#ifndef INCLUDED_BSLMF_NESTEDTRAITDECLARATION
#include <bslmf_nestedtraitdeclaration.h>
#endif

#ifndef INCLUDED_BSL_VECTOR
#include <bsl_vector.h>
#endif

#ifndef INCLUDED_BSL_CSTDINT
#include <bsl_cstdint.h>
#endif

namespace BloombergLP {

namespace ball {


                        // ================
                        // class UserFields
                        // ================

class UserFields {
    // This class implements a value-semantic type for representing a sequence
    // of (randomly accessible) user field values.

  private:
    // DATA
    bsl::vector<ball::UserFieldValue> d_values;  // sequence of values

    // FRIENDS
    friend bool operator==(const UserFields&, const UserFields&);

  public:
    // TYPES
    typedef bsl::vector<ball::UserFieldValue>::const_iterator ConstIterator;

    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(UserFields, bslma::UsesBslmaAllocator);

    // CREATORS
    explicit UserFields(bslma::Allocator *basicAllocator = 0);
        // Create an empty 'UserFields' object.  Optionally specify a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0,
        // the currently installed default allocator is used.

    UserFields(const UserFields&  original,
               bslma::Allocator  *basicAllocator = 0);
        // Create a 'UserFields' object having the same value as the specified
        // 'original' object.  Optionally specify a 'basicAllocator' used to
        // supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.

    //! ~UserFields() = default;
        // Destroy this object.

    // MANIPULATORS
    UserFields& operator=(const UserFields& rhs);
        // Assign to this object the value of the specified 'rhs' object, and
        // return a reference providing modifiable access to this object.

    void removeAll();
        // Remove all of the user field values from this object.  After this
        // method is called 'length' is 0.

    void append(const UserFieldValue& value);
        // Append the specified 'value' to this object.

    void appendNull();
        // Append an element having the unset value to this object.

    void appendInt64(bsl::int64_t value);
    void appendDouble(double value);
    void appendString(bslstl::StringRef value);
    void appendDatetimeTz(const bdlt::DatetimeTz& value);
        // Append the specified 'value' to this object.

    ball::UserFieldValue& operator[](int index);
    ball::UserFieldValue& value(int index);
        // Return a reference providing modifiable access to the value at the
        // specified 'index'.  The behavior is undefined unless
        // '0 <= index && index < length()'.

                                  // Aspects

    void swap(UserFields& other);
        // Efficiently exchange the value of this object with the value of the
        // specified 'other' object.   This method provides the no-throw
        // exception guarantee if 'allocator' is the same as
        // 'other.allocator()', and the basic exception guarantee otherwise.

    // ACCESSORS
    ConstIterator begin() const;
        // Return an iterator providing non-modifiable access to the first
        // element in the sequence of user field values maintained by this
        // object, or the 'end' iterator if this object is empty.

    ConstIterator end() const;
        // Return an iterator providing non-modifiable access to the
        // past-the-end element in the sequence of user field values
        // maintained by this object.

    int length () const;
        // Return the number of user field values in this object.

    const ball::UserFieldValue& operator[](int index) const;
    const ball::UserFieldValue& value(int index) const;
        // Return a reference providing non-modifiable access to the value at
        // the specified 'index'.  The behavior is undefined unless
        // '0 <= index && index < length()'.

                                  // Aspects

    bslma::Allocator *allocator() const;
        // Return the allocator used by this object to supply memory.  Note
        // that if no allocator was supplied at construction the currently
        // installed default allocator is used.

    bsl::ostream& print(bsl::ostream& stream,
                        int           level = 0,
                        int           spacesPerLevel = 4) const;
        // Write the value of this object to the specified output 'stream' in
        // a human-readable format, and return a reference to 'stream'.
        // Optionally specify an initial indentation 'level', whose absolute
        // value is incremented recursively for nested objects.  If 'level' is
        // specified, optionally specify 'spacesPerLevel', whose absolute
        // value indicates the number of spaces per indentation level for this
        // and all of its nested objects.  If 'level' is negative, suppress
        // indentation of the first line.  If 'spacesPerLevel' is negative,
        // format the entire output on one line, suppressing all but the
        // initial indentation (as governed by 'level').  If 'stream' is not
        // valid on entry, this operation has no effect.  Note that the format
        // is not fully specified, and can change without notice.
};

// FREE OPERATORS
bool operator==(const UserFields& lhs, const UserFields& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects have the same
    // value, and 'false' otherwise.  Two 'ball::UserFields' objects have the
    // same value if they have the same number of elements, and each element in
    // 'lhs' has the same value as corresponding element at the same index in
    // 'rhs'.

bool operator!=(const UserFields& lhs, const UserFields& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects do not have the
    // same value, and 'false' otherwise.  Two 'UserFields' objects do not
    // have the same value if they have a different number of elements, or if
    // any element in 'lhs' has a different value from the corresponding
    // element at the same index in 'rhs'.

bsl::ostream& operator<<(bsl::ostream&     stream,
                         const UserFields& object);
    // Write the value of the specified 'object' to the specified output
    // 'stream' in a single-line format, and return a reference to 'stream'.
    // If 'stream' is not valid on entry, this operation has no effect.  Note
    // that this human-readable format is not fully specified, can change
    // without notice, and is logically equivalent to:
    //..
    //  print(stream, 0, -1);
    //..

// FREE FUNCTIONS
void swap(ball::UserFields& a, ball::UserFields& b);
    // Swap the value of the specified 'a' object with the value of the
    // specified 'b' object.  This method provides the no-throw exception
    // guarantee if 'a.allocator()' is the same as 'b.allocator()', and the
    // basic exception guarantee otherwise.


// ============================================================================
//                              INLINE DEFINITIONS
// ============================================================================

                        // ----------------
                        // class UserFields
                        // ----------------

inline
UserFields::UserFields(bslma::Allocator *basicAllocator)
: d_values(basicAllocator)
{
}

inline
UserFields::UserFields(const UserFields&  original,
                       bslma::Allocator  *basicAllocator)
: d_values(original.d_values, basicAllocator)
{
}

// MANIPULATORS
inline
UserFields& UserFields::operator=(const UserFields& rhs)
{
    d_values = rhs.d_values;
    return *this;
}

inline
void UserFields::removeAll()
{
    d_values.clear();
}

inline
void UserFields::appendNull()
{
    d_values.emplace_back();
}

inline
void UserFields::appendInt64(bsl::int64_t value)
{
    d_values.emplace_back(value);
}

inline
void UserFields::appendDouble(double value)
{
    d_values.emplace_back(value);
}

inline
void UserFields::appendString(bslstl::StringRef value)
{
    d_values.emplace_back(value);
}

inline
void UserFields::appendDatetimeTz(const bdlt::DatetimeTz& value)
{
    d_values.emplace_back(value);
}

inline
UserFieldValue& UserFields::operator[](int index)
{
    return d_values[index];
}

inline
UserFieldValue& UserFields::value(int index)
{
    return d_values[index];
}

inline
void UserFields::swap(UserFields& other)
{
    d_values.swap(other.d_values);
}

// ACCESSORS
inline
bslma::Allocator *UserFields::allocator() const
{
    return d_values.get_allocator().mechanism();
}

inline
UserFields::ConstIterator UserFields::begin() const
{
    return d_values.begin();
}

inline
UserFields::ConstIterator UserFields::end() const
{
    return d_values.end();
}

inline
int UserFields::length() const
{
    return d_values.size();
}

inline
const UserFieldValue& UserFields::operator[](int index) const
{
    return d_values[index];
}

inline
const UserFieldValue& UserFields::value(int index) const
{
    return d_values[index];
}

}  // close package namespace

// FREE OPERATORS
inline
bool ball::operator==(const UserFields& lhs, const UserFields& rhs)
{
    return lhs.d_values == rhs.d_values;
}

inline
bool ball::operator!=(const UserFields& lhs, const UserFields& rhs)
{
    return !(lhs == rhs);
}

inline
bsl::ostream& ball::operator<<(bsl::ostream&     stream,
                               const UserFields& object)
{
    return object.print(stream, 0, -1);
}


// FREE FUNCTIONS
inline
void swap(ball::UserFields& a, ball::UserFields& b)
{
    a.swap(b);
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
