// ball_userfieldvalue.h                                              -*-C++-*-
#ifndef INCLUDED_BALL_USERFIELDVALUE
#define INCLUDED_BALL_USERFIELDVALUE

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a type for the value of a user supplied field.
//
//@CLASSES:
//  ball::UserFieldValue: the value of a user supplied field
//
//@SEE_ALSO: ball_userfields, ball_userfieldsschema
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
// 'ball::UserFieldValue' object.  Note that 'ball::UserFieldValue' objects
// are typically used in a description of a sequence of user fields (see
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
// Next, we create a second 'ball::UserFieldValue' having the value 5, and
// then confirm its value and observe that it does not compare equal to the
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
// observer that 'valueA' now compares equal to 'valueB':
//..
//  valueB.reset();
//
//  assert(valueA == valueB);
//..

#ifndef INCLUDED_BALSCM_VERSION
#include <balscm_version.h>
#endif

#ifndef INCLUDED_BALL_USERFIELDTYPE
#include <ball_userfieldtype.h>
#endif

#ifndef INCLUDED_BDLB_VARIANT
#include <bdlb_variant.h>
#endif

#ifndef INCLUDED_BDLT_DATETIMETZ
#include <bdlt_datetimetz.h>
#endif

#ifndef INCLUDED_BSLMA_USESBSLMAALLOCATOR
#include <bslma_usesbslmaallocator.h>
#endif

#ifndef INCLUDED_BSLMF_NESTEDTRAITDECLARATION
#include <bslmf_nestedtraitdeclaration.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_BSL_STRING
#include <bsl_string.h>
#endif

#ifndef INCLUDED_BSL_VECTOR
#include <bsl_vector.h>
#endif

#ifndef INCLUDED_BSL_CSTDINT
#include <bsl_cstdint.h>
#endif

namespace BloombergLP {

namespace ball {

                        // ====================
                        // class UserFieldValue
                        // ====================

class UserFieldValue {
    // This class implements a value-semantic type for representing the value
    // of a user field in a log record.  A user field value acts as a
    // discriminated union, and may represent a value of any of types described
    // in 'ball::UserFieldType' or an unset value (indicated type
    // 'ball::UserFieldType::e_VOID').

    // TYPES
    typedef bdlb::Variant<bsl::int64_t,
                          double,
                          bsl::string,
                          bdlt::DatetimeTz> ValueVariant;

    // DATA
    ValueVariant d_value;  // value

    // FRIENDS
    friend bool operator==(const UserFieldValue&, const UserFieldValue&);

  public:
    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(UserFieldValue, bslma::UsesBslmaAllocator);

    // CREATORS
    explicit UserFieldValue(bslma::Allocator *basicAllocator = 0);
        // Create a user field value having the unset value.  Optionally
        // specify a 'basicAllocator' used to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.

    explicit UserFieldValue(bsl::int64_t             value,
                            bslma::Allocator        *basicAllocator = 0);
    explicit UserFieldValue(double                   value,
                            bslma::Allocator        *basicAllocator = 0);
    explicit UserFieldValue(bslstl::StringRef        value,
                            bslma::Allocator        *basicAllocator = 0);
    explicit UserFieldValue(const bdlt::DatetimeTz&  value,
                            bslma::Allocator        *basicAllocator = 0);
        // Create a user field value having the specified 'value'.  Optionally
        // specify a 'basicAllocator' used to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.

    template <class INTEGRAL_TYPE>
    explicit UserFieldValue(
                INTEGRAL_TYPE     value,
                bslma::Allocator *basicAllocator = 0,
                typename bsl::enable_if<
                     bsl::is_integral<INTEGRAL_TYPE>::value>::type * = 0)
    : d_value(static_cast<bsl::int64_t>(value), basicAllocator) {}
        // Create a user field value having the specified integral 'value'.
        // Optionally specify a 'basicAllocator' used to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.
        //
        // Note that this constructor is provided to disambiguate between
        // constructors taking 'double' and 'bsl::int64_t' when supplied a
        // integer that is not of type 'bsl::int64_t'.  Note also that the
        // implementation is (temporarily) provided inline to avoid issues with
        // MSVC 2008.

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
        // Set this object to the unset value.  After this operation 'type'
        // will be 'ball::UserFieldType::e_VOID'.

    void setInt64(bsl::int64_t value);
        // Set this object to the specified 'value'.  After this operation,
        // 'type' is 'ball::UserFieldType::e_INT64'.

    void setDouble(double value);
        // Set this object to the specified 'value'.  After this operation,
        // 'type' is 'ball::UserFieldType::e_DOUBLE'.

    void setString(bslstl::StringRef value);
        // Set this object to the specified 'value'.  After this operation,
        // 'type' is 'ball::UserFieldType::e_STRING'.

    void setDatetimeTz(const bdlt::DatetimeTz& value);
        // Set this object to the specified 'value'.  After this operation,
        // 'type' is 'ball::UserFieldType::e_DATETIMETZ'.

                                  // Aspects

    void swap(UserFieldValue& other);
        // Efficiently exchange the value of this object with the value of the
        // specified 'other' object.  This method provides the no-throw
        // guarantee if 'type' is the same as 'other.type()';  otherwise it
        // provides the basic guarantee.

    // ACCESSORS
    bool isUnset() const;
        // Return 'true' if this object has the unset value, and 'false'
        // otherwise.  Note that if 'isUnset' is 'true', then 'type' is
        // 'ball::UserFieldType::e_VOID'.

    ball::UserFieldType::Enum type() const;
        // Return the type of this user field value.  The type
        // 'ball::UserFieldValue::e_VOID' represents the unset value.

    const bsl::int64_t& theInt64() const;
        // Return a reference providing non-modifiable access to the 64-bit
        // integer value of this object.  The behavior is undefined unless
        // 'type' is 'ball::UserFieldType::e_INT64'.

    const double& theDouble() const;
        // Return a reference providing non-modifiable access to the double
        // value of this object.  The behavior is undefined unless 'type' is
        // 'ball::UserFieldType::e_DOUBLE'.

    const bsl::string& theString() const;
        // Return a reference providing non-modifiable access to the string
        // value of this object.  The behavior is undefined unless 'type' is
        // 'ball::UserFieldType::e_STRING'.

    const bdlt::DatetimeTz& theDatetimeTz() const;
        // Return a reference providing non-modifiable access to the
        // 'DatetimeTz' value of this object.  The behavior is undefined
        // unless 'type' is 'ball::UserFieldType::e_DATETIMETZ'.


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
    // same value if they have the same 'type', and (if the type is not
    // 'e_VOID') the value of that type (as accessed through 'the*' methods)
    // is the same.

bool operator!=(const UserFieldValue& lhs, const UserFieldValue& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects do not have the
    // same value, and 'false' otherwise.  Two 'UserFieldValue' objects do not
    // have the same value if their 'type' is not the same, or (if their type
    // is not 'e_VOID') the value of that type (as accessed through 'the*'
    // methods) is not the same.

bsl::ostream& operator<<(bsl::ostream&         stream,
                         const UserFieldValue& object);
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
    // specified 'b' object.  This method provides the no-throw guarantee if
    // 'a.type()' is the same as 'b.type()'; otherwise it provides the basic
    // guarantee.

// ============================================================================
//                              INLINE DEFINITIONS
// ============================================================================

                        // ---------------------
                        // class UserFieldValue
                        // ---------------------

inline
UserFieldValue::UserFieldValue(bslma::Allocator *basicAllocator)
: d_value(basicAllocator)
{
}

inline
UserFieldValue::UserFieldValue(bsl::int64_t value,
                               bslma::Allocator *basicAllocator)
: d_value(value, basicAllocator)
{
}

inline
UserFieldValue::UserFieldValue(double value, bslma::Allocator *basicAllocator)
: d_value(value, basicAllocator)
{
}

inline
UserFieldValue::UserFieldValue(bslstl::StringRef  value,
                               bslma::Allocator  *basicAllocator)
: d_value(basicAllocator)
{
    d_value.assign<bsl::string>(value);
}

inline
UserFieldValue::UserFieldValue(const bdlt::DatetimeTz&  value,
                               bslma::Allocator        *basicAllocator)
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
void UserFieldValue::setInt64(bsl::int64_t value)
{
    d_value.assign(value);
}

inline
void UserFieldValue::setDouble(double value)
{
    d_value.assign(value);
}

inline
void UserFieldValue::setString(bslstl::StringRef value)
{
    d_value.assign<bsl::string>(value);
}

inline
void UserFieldValue::setDatetimeTz(const bdlt::DatetimeTz& value)
{
    d_value.assign(value);
}

inline
void UserFieldValue::swap(UserFieldValue& other)
{
    d_value.swap(other.d_value);
}

// ACCESSORS
inline
bslma::Allocator *UserFieldValue::allocator() const
{
    return d_value.getAllocator();
}

inline
bool UserFieldValue::isUnset() const
{
    return d_value.isUnset();
}

inline
ball::UserFieldType::Enum UserFieldValue::type() const
{
    switch (d_value.typeIndex()) {
      case 0: {
          BSLS_ASSERT_SAFE(d_value.isUnset());
          return ball::UserFieldType::e_VOID;                         // RETURN
      } break;
      case 1: {
          BSLS_ASSERT_SAFE(d_value.is<bsl::int64_t>());
          return ball::UserFieldType::e_INT64;                        // RETURN
      } break;
      case 2: {
          BSLS_ASSERT_SAFE(d_value.is<double>());
          return ball::UserFieldType::e_DOUBLE;                       // RETURN
      } break;

      case 3: {
          BSLS_ASSERT_SAFE(d_value.is<bsl::string>());
          return ball::UserFieldType::e_STRING;                       // RETURN
      } break;

      case 4: {
          BSLS_ASSERT_SAFE(d_value.is<bdlt::DatetimeTz>());
          return ball::UserFieldType::e_DATETIMETZ;                   // RETURN
      } break;
      default: {
          BSLS_ASSERT_OPT(false);
      }
    }
    BSLS_ASSERT_OPT(false);
    return ball::UserFieldType::e_INT64;
}

inline
const bsl::int64_t& UserFieldValue::theInt64() const
{
    BSLS_ASSERT_SAFE(d_value.is<bsl::int64_t>());
    return d_value.the<bsl::int64_t>();
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
void swap(ball::UserFieldValue& a, ball::UserFieldValue& b)
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
