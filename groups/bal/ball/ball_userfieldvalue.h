// ball_userfieldvalue.h                                              -*-C++-*-
#ifndef INCLUDED_BALL_USERFIELDVALUE
#define INCLUDED_BALL_USERFIELDVALUE

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a type for the value of a user supplied field.
//
//@CLASSES:
//  ball::UserFieldValue: the value of a user supplied field
//
//@SEE_ALSO: ball_userfields, ball_userfieldtype
//
//@DESCRIPTION: This component provides a value-semantic class,
// 'ball::UserFieldValue', that represents the value of a user supplied log
// field value.  A user field value acts as a discriminated union, and may
// represent a value of any of types described in 'ball::UserFieldType' or an
// unset value (indicated by the type 'ball::UserFieldType::e_VOID').
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Basic Use of 'ball::UserFieldValue'
/// - - - - - - - - - - - - - - - - - - - - - - -
// The following snippets of code illustrate how to create and use a
// 'ball::UserFieldValue' object.  Note that 'ball::UserFieldValue' objects are
// typically used in a description of a sequence of user fields (see
// 'ball_userfields').
//
// First, we create a default 'ball::UserFieldValue', 'valueA', and observe
// that it is in the unset state, meaning that 'isUnset' is true and its type
// is 'ball::UserFieldValue::e_VOID':
//..
//  ball::UserFieldValue valueA;
//
//  assert(true                         == valueA.isUnset());
//  assert(ball::UserFieldValue::e_VOID == valueA.type());
//..
// Next, we create a second 'ball::UserFieldValue' having the value 5, and then
// confirm its value and observe that it does not compare equal to the
// 'valueA':
//..
//  ball::UserFieldValue valueB(5);
//
//  assert(false                         == valueB.isUnset());
//  assert(ball::UserFieldValue::e_INT64 == valueB.type());
//  assert(5                             == valueB.theInt64();
//
//  assert(valueA != valueB);
//..
// Finally, we call 'reset' of 'valueB' resetting it to the unset state, and
// observe that 'valueA' now compares equal to 'valueB':
//..
//  valueB.reset();
//
//  assert(valueA == valueB);
//..

#include <balscm_version.h>

#include <ball_userfieldtype.h>

#include <bdlb_variant.h>

#include <bdlt_datetimetz.h>

#include <bslma_allocator.h>
#include <bslma_usesbslmaallocator.h>

#include <bslmf_nestedtraitdeclaration.h>

#include <bsls_assert.h>
#include <bsls_review.h>
#include <bsls_types.h>

#include <bsl_string.h>
#include <bsl_vector.h>

namespace BloombergLP {
namespace ball {

                        // ====================
                        // class UserFieldValue
                        // ====================

class UserFieldValue {
    // This class implements a value-semantic type for representing the value
    // of a user field in a log record.  A user field value acts as a
    // discriminated union, and may represent a value of any of the types
    // described in 'ball::UserFieldType' or an unset value (indicated by type
    // 'ball::UserFieldType::e_VOID').

    // PRIVATE TYPES
    typedef bdlb::Variant<bsls::Types::Int64,
                          double,
                          bsl::string,
                          bdlt::DatetimeTz,
                          bsl::vector<char> > ValueVariant;

    // DATA
    ValueVariant d_value;  // value

    // FRIENDS
    friend bool operator==(const UserFieldValue&, const UserFieldValue&);
    friend void swap(UserFieldValue&, UserFieldValue&);

  public:
    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(UserFieldValue, bslma::UsesBslmaAllocator);

    // CREATORS
    explicit UserFieldValue(bslma::Allocator *basicAllocator = 0);
        // Create a user field value having the unset value.  Optionally
        // specify a 'basicAllocator' used to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.

    explicit UserFieldValue(bsls::Types::Int64        value,
                            bslma::Allocator         *basicAllocator = 0);
    explicit UserFieldValue(double                    value,
                            bslma::Allocator         *basicAllocator = 0);
    explicit UserFieldValue(const bsl::string_view&   value,
                            bslma::Allocator         *basicAllocator = 0);
    explicit UserFieldValue(const bdlt::DatetimeTz&   value,
                            bslma::Allocator         *basicAllocator = 0);
    explicit UserFieldValue(const bsl::vector<char>&  value,
                            bslma::Allocator         *basicAllocator = 0);
        // Create a user field value having the specified 'value'.  Optionally
        // specify a 'basicAllocator' used to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.

    template <class t_INTEGRAL_TYPE>
    explicit UserFieldValue(
        t_INTEGRAL_TYPE   value,
        bslma::Allocator *basicAllocator = 0,
        typename bsl::enable_if<bsl::is_integral<t_INTEGRAL_TYPE>::value>::type
            * = 0)
    : d_value(static_cast<bsls::Types::Int64>(value), basicAllocator) {}
        // Create a user field value having the specified integral 'value'.
        // Optionally specify a 'basicAllocator' used to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.
        //
        // Note that this constructor is provided to disambiguate between
        // constructors taking 'double' and 'bsls::Types::Int64' when supplied
        // an integer that is not of type 'bsls::Types::Int64'.  Also note that
        // the implementation is (temporarily) provided inline to avoid issues
        // with MSVC 2008.

    UserFieldValue(const UserFieldValue&  original,
                   bslma::Allocator      *basicAllocator = 0);
        // Create a 'UserFieldValue' object having the same value as the
        // specified 'original' object.  Optionally specify a 'basicAllocator'
        // used to supply memory.  If 'basicAllocator' is 0, the currently
        // installed default allocator is used.

    //! ~UserFieldValue() = default;
        // Destroy this object.

    // MANIPULATORS
    UserFieldValue& operator=(const UserFieldValue& rhs);
        // Assign to this object the value of the specified 'rhs' object, and
        // return a reference providing modifiable access to this object.

    void reset();
        // Set this object to have the unset value.  After this operation,
        // 'type() == ball::UserFieldType::e_VOID'.

    void setInt64(bsls::Types::Int64 value);
        // Set this object to have the specified 'value'.  After this
        // operation, 'type() == ball::UserFieldType::e_INT64'.

    void setDouble(double value);
        // Set this object to have the specified 'value'.  After this
        // operation, 'type() == ball::UserFieldType::e_DOUBLE'.

    void setString(const bsl::string_view& value);
        // Set this object to have the specified 'value'.  After this
        // operation, 'type() == ball::UserFieldType::e_STRING'.

    void setDatetimeTz(const bdlt::DatetimeTz& value);
        // Set this object to have the specified 'value'.  After this
        // operation, 'type() == ball::UserFieldType::e_DATETIMETZ'.

    void setCharArray(const bsl::vector<char>& value);
        // Set this object to have the specified 'value'.  After this
        // operation, 'type() == ball::UserFieldType::e_CHAR_ARRAY'.

                                  // Aspects

    void swap(UserFieldValue& other);
        // Efficiently exchange the value of this object with the value of the
        // specified 'other' object.  This method provides the no-throw
        // exception-safety guarantee if either 'type()' is the same as
        // 'other.type()', or neither 'type()' nor 'other.type()' is a type
        // that requires allocation; otherwise, it provides the basic
        // guarantee.  The behavior is undefined unless this object was created
        // with the same allocator as 'other'.

    // ACCESSORS
    bool isUnset() const;
        // Return 'true' if this object has the unset value, and 'false'
        // otherwise.  Note that if 'isUnset()' returns 'true', then 'type()'
        // returns 'ball::UserFieldType::e_VOID'.

    ball::UserFieldType::Enum type() const;
        // Return the type of this user field value.  The type
        // 'ball::UserFieldValue::e_VOID' represents the unset value.

    const bsls::Types::Int64& theInt64() const;
        // Return a reference providing non-modifiable access to the 64-bit
        // integer value of this object.  The behavior is undefined unless
        // 'type() == ball::UserFieldType::e_INT64'.

    const double& theDouble() const;
        // Return a reference providing non-modifiable access to the double
        // value of this object.  The behavior is undefined unless
        // 'type() == ball::UserFieldType::e_DOUBLE'.

    const bsl::string& theString() const;
        // Return a reference providing non-modifiable access to the string
        // value of this object.  The behavior is undefined unless
        // 'type() == ball::UserFieldType::e_STRING'.

    const bdlt::DatetimeTz& theDatetimeTz() const;
        // Return a reference providing non-modifiable access to the
        // 'DatetimeTz' value of this object.  The behavior is undefined
        // unless 'type() == ball::UserFieldType::e_DATETIMETZ'.

    const bsl::vector<char>& theCharArray() const;
        // Return a reference providing non-modifiable access to the
        // 'bsl::vector<char>' value of this object.  The behavior is undefined
        // unless 'type() == ball::UserFieldType::e_CHAR_ARRAY'.

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
bool operator==(const UserFieldValue& lhs, const UserFieldValue& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects have the same
    // value, and 'false' otherwise.  Two 'UserFieldValue' objects have the
    // same value if they have the same type, and (if the type is not
    // 'e_VOID') the value of that type (as accessed through 'the*' methods)
    // is the same.

bool operator!=(const UserFieldValue& lhs, const UserFieldValue& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects do not have the
    // same value, and 'false' otherwise.  Two 'UserFieldValue' objects do not
    // have the same value if their type is not the same, or (if their type
    // is not 'e_VOID') the value of that type (as accessed through 'the*'
    // methods) is not the same.

bsl::ostream& operator<<(bsl::ostream& stream, const UserFieldValue& object);
    // Write the value of the specified 'object' to the specified output
    // 'stream' in a single-line format, and return a reference to 'stream'.
    // If 'stream' is not valid on entry, this operation has no effect.  Note
    // that this human-readable format is not fully specified, can change
    // without notice, and is logically equivalent to:
    //..
    //  print(stream, 0, -1);
    //..

// FREE FUNCTIONS
void swap(ball::UserFieldValue& a, ball::UserFieldValue& b);
    // Swap the value of the specified 'a' object with the value of the
    // specified 'b' object.  This method provides the no-throw
    // exception-safety guarantee if either 'a.type()' is the same as
    // 'b.type()' and 'a' and 'b' were created with the same allocator, or
    // neither 'a.type()' nor 'b.type()' is a type that requires allocation;
    // otherwise, it provides the basic guarantee.

// ============================================================================
//                              INLINE DEFINITIONS
// ============================================================================

                        // --------------------
                        // class UserFieldValue
                        // --------------------

// CREATORS
inline
UserFieldValue::UserFieldValue(bslma::Allocator *basicAllocator)
: d_value(basicAllocator)
{
}

inline
UserFieldValue::UserFieldValue(bsls::Types::Int64  value,
                               bslma::Allocator   *basicAllocator)
: d_value(value, basicAllocator)
{
}

inline
UserFieldValue::UserFieldValue(double value, bslma::Allocator *basicAllocator)
: d_value(value, basicAllocator)
{
}

inline
UserFieldValue::UserFieldValue(const bsl::string_view&  value,
                               bslma::Allocator        *basicAllocator)
: d_value(basicAllocator)
{
    d_value.assignTo<bsl::string>(value);
}

inline
UserFieldValue::UserFieldValue(const bdlt::DatetimeTz&  value,
                               bslma::Allocator        *basicAllocator)
: d_value(value, basicAllocator)
{
}

inline
UserFieldValue::UserFieldValue(const bsl::vector<char>&  value,
                               bslma::Allocator         *basicAllocator)
: d_value(value, basicAllocator)
{
}

inline
UserFieldValue::UserFieldValue(const UserFieldValue&  original,
                               bslma::Allocator      *basicAllocator)
: d_value(original.d_value, basicAllocator)
{
}

// MANIPULATORS
inline
UserFieldValue& UserFieldValue::operator=(const UserFieldValue& rhs)
{
    d_value = rhs.d_value;
    return *this;
}

inline
void UserFieldValue::reset()
{
    d_value.reset();
}

inline
void UserFieldValue::setInt64(bsls::Types::Int64 value)
{
    d_value.assign(value);
}

inline
void UserFieldValue::setDouble(double value)
{
    d_value.assign(value);
}

inline
void UserFieldValue::setString(const bsl::string_view& value)
{
    d_value.assignTo<bsl::string>(value);
}

inline
void UserFieldValue::setDatetimeTz(const bdlt::DatetimeTz& value)
{
    d_value.assign(value);
}

inline
void UserFieldValue::setCharArray(const bsl::vector<char>& value)
{
    d_value.assign(value);
}

inline
void UserFieldValue::swap(UserFieldValue& other)
{
    BSLS_ASSERT(allocator() == other.allocator());

    d_value.swap(other.d_value);
}

// ACCESSORS
inline
bool UserFieldValue::isUnset() const
{
    return d_value.isUnset();
}

inline
const bsls::Types::Int64& UserFieldValue::theInt64() const
{
    BSLS_ASSERT_SAFE(d_value.is<bsls::Types::Int64>());

    return d_value.the<bsls::Types::Int64>();
}

inline
const double& UserFieldValue::theDouble() const
{
    BSLS_ASSERT_SAFE(d_value.is<double>());

    return d_value.the<double>();
}

inline
const bsl::string& UserFieldValue::theString() const
{
    BSLS_ASSERT_SAFE(d_value.is<bsl::string>());

    return d_value.the<bsl::string>();
}

inline
const bdlt::DatetimeTz& UserFieldValue::theDatetimeTz() const
{
    BSLS_ASSERT_SAFE(d_value.is<bdlt::DatetimeTz>());

    return d_value.the<bdlt::DatetimeTz>();
}

inline
const bsl::vector<char>& UserFieldValue::theCharArray() const
{
    BSLS_ASSERT_SAFE(d_value.is<bsl::vector<char> >());

    return d_value.the<bsl::vector<char> >();
}

                                  // Aspects

inline
bslma::Allocator *UserFieldValue::allocator() const
{
    return d_value.getAllocator();
}

}  // close package namespace

// FREE OPERATORS
inline
bool ball::operator==(const UserFieldValue& lhs, const UserFieldValue& rhs)
{
    return lhs.d_value == rhs.d_value;
}

inline
bool ball::operator!=(const UserFieldValue& lhs, const UserFieldValue& rhs)
{
    return !(lhs == rhs);
}

inline
bsl::ostream& ball::operator<<(bsl::ostream&         stream,
                               const UserFieldValue& object)
{
    return object.print(stream, 0, -1);
}

// FREE FUNCTIONS
inline
void ball::swap(UserFieldValue& a, UserFieldValue& b)
{
    // 'bdlb::Variant' member 'swap' supports differing allocators.

    a.d_value.swap(b.d_value);
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
