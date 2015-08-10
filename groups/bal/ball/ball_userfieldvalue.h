// ball_userfieldvalue.h                                             -*-C++-*-
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
//@AUTHOR: Henry Verschell (hversche)
//
//@SEE_ALSO: 
//
//@DESCRIPTION: This component provides a value-semantic class,
// 'ball::UserFieldValue', that represents the value of a user supplied log
// field value.
//

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

#ifndef INCLUDED_BSL_VECTOR
#include <bsl_vector.h>
#endif

#warning TBD: Replace with bsl_cstdint.h
#ifndef INCLUDED_STDINT
#include <stdint.h>
#define INCLUDED_STDINT
#endif

namespace BloombergLP {

namespace ball {

                        // ====================
                        // class UserFieldValue
                        // ====================

class UserFieldValue {

    // TYPES
    typedef bdlb::Variant<int64_t, 
                          double, 
                          bsl::string, 
                          bdlt::DatetimeTz> ValueVariant;

    // DATA
    ValueVariant d_value;      

    // FRIENDS
    friend bool operator==(const UserFieldValue&, const UserFieldValue&);

  public:
    // TRAITS
    BSLALG_DECLARE_NESTED_TRAITS(UserFieldValue,
                                 bslalg::TypeTraitUsesBslmaAllocator);

    // CREATORS
    explicit UserFieldValue(bslma::Allocator *basicAllocator = 0);
        // Create a user field value having the default (unset) value.
        // Optionally specify a 'basicAllocator' used to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.
 
    explicit UserFieldValue(int64_t                  value, 
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

    UserFieldValue(const UserFieldValue&  original,
                   bslma::Allocator       *basicAllocator = 0);

    // MANIPULATORS
    UserFieldValue& operator=(const UserFieldValue& rhs);

    void reset();
    void setValue(int64_t value);
    void setValue(double value);
    void setValue(bslstl::StringRef value);
    void setValue(const bdlt::DatetimeTz& value);

    void swap(UserFieldValue& other);

    // ACCESSORS
    bslma::Allocator *allocator() const;

    ball::UserFieldType::Enum type() const;
    
    const int64_t& theInt64() const;
    const double& theDouble() const;
    const bsl::string& theString() const;
    const bdlt::DatetimeTz& theDatetimeTz() const;
        

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
        // valid on entry, this operation has no effect.  Note that the
        // format is not fully specified, and can change without notice.
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

bsl::ostream& operator<<(bsl::ostream&          stream,
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
    // specified 'b' object.  This method provides the no-throw guarantee.  The
    // behavior is undefined if the two objects being swapped have non-equal
    // allocators.

// ============================================================================
//                      INLINE FUNCTION DEFINITIONS
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
UserFieldValue::UserFieldValue(int64_t value, bslma::Allocator *basicAllocator)
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
                               bslma::Allocator       *basicAllocator)
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
void UserFieldValue::setValue(int64_t value)
{
    d_value.assign(value);
}

inline
void UserFieldValue::setValue(double value)
{
    d_value.assign(value);
}

inline
void UserFieldValue::setValue(bslstl::StringRef value)
{
    d_value.assign<bsl::string>(value);
}

inline
void UserFieldValue::setValue(const bdlt::DatetimeTz& value)
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
ball::UserFieldType::Enum UserFieldValue::type() const
{
    switch (d_value.typeIndex()) {
      case 0: {
          BSLS_ASSERT_SAFE(d_value.isUnset());
          return ball::UserFieldType::e_VOID;
      } break;
      case 1: {
          BSLS_ASSERT_SAFE(d_value.is<int64_t>());
          return ball::UserFieldType::e_INT64;
      } break;
      case 2: {
          BSLS_ASSERT_SAFE(d_value.is<double>());
          return ball::UserFieldType::e_DOUBLE;
      } break;

      case 3: {
          BSLS_ASSERT_SAFE(d_value.is<bsl::string>());
          return ball::UserFieldType::e_STRING;
      } break;

      case 4: {
          BSLS_ASSERT_SAFE(d_value.is<bdlt::DatetimeTz>());
          return ball::UserFieldType::e_DATETIMETZ;
      } break;
      default: {
          BSLS_ASSERT_OPT(false);
      }
    }
    BSLS_ASSERT_OPT(false);
    return ball::UserFieldType::e_INT64;
}

inline
const int64_t& UserFieldValue::theInt64() const
{
    BSLS_ASSERT_SAFE(d_value.is<int64_t>());
    return d_value.the<int64_t>();
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
bsl::ostream& ball::operator<<(bsl::ostream&          stream, 
                               const UserFieldValue& rhs)
{
    return rhs.print(stream, 0, -1);
}


// FREE FUNCTIONS
inline
void swap(ball::UserFieldValue& a, ball::UserFieldValue& b)
{
    a.swap(b);
}

}  // close namespace BloombergLP


#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2011
//      All Rights Reserved.
//      Property of Bloomberg L.P.  (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
