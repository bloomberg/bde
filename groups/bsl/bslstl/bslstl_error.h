// bslstl_error.h                                                     -*-C++-*-
#ifndef INCLUDED_BSLSTL_ERROR
#define INCLUDED_BSLSTL_ERROR

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

// BDE_VERIFY pragma: -TP25  // CLASSES are not defined in C++11

//@PURPOSE: Provide C++11-defined error classes and functions for C++03.
//
//@CLASSES:
//  bsl::error_category: C++03 version of std::error_category
//  bsl::error_code: C++03 version of std::error_code
//  bsl::error_condition: C++03 version of std::error_condition
//
//@DESCRIPTION: This component defines classes 'bsl::error_category',
// 'bsl::error_code', and 'bsl::error_condition', global functions
// 'bsl::generic_category', 'bsl::system_category', 'bsl::make_error_code', and
// 'bsl::make_error_condition', and a variety of operators that provide
// implementations of the C++11 'system_error' facility.  In C++11 mode, the
// vendor-supplied '<system_error>' implementation is used instead, and the
// corresponding names from 'std' are imported into 'bsl'.
//
///Usage
///-----
// In this section we show intended use of this component.
//
///Example 1: Dedicated Error Category
///- - - - - - - - - - - - - - - - - -
// Suppose we have a dedicated system with a set of possible errors, and we
// want to check if such an error has occurred.  We can use the 'system_error'
// capabilities of the C++ standard for this.
//
// First, we define the set of error codes for our system.
//..
//  struct car_errc {
//      // TYPES
//      enum Enum {
//          car_wheels_came_off = 1,
//          car_engine_fell_out = 2
//      };
//  };
//..
// Then, we enable the traits marking this as an error code and condition.
//..
//  namespace BSL_IS_ERROR_CODE_ENUM_NAMESPACE {
//  template <> struct is_error_code_enum<car_errc::Enum>
//  : public bsl::true_type { };
//  }  // close namespace BSL_IS_ERROR_CODE_ENUM_NAMESPACE
//
//  namespace BSL_IS_ERROR_CONDITION_ENUM_NAMESPACE {
//  template <> struct is_error_condition_enum<car_errc::Enum>
//  : public bsl::true_type { };
//  }  // close namespace BSL_IS_ERROR_CONDITION_ENUM_NAMESPACE
//..
// Next, we create an error category that will give us descriptive messages.
//..
//  namespace {
//  struct car_category_impl : public bsl::error_category {
//      // ACCESSORS
//      native_std::string message(int value) const BSLS_KEYWORD_OVERRIDE;
//          // Return a string describing the specified 'value'.
//
//      const char *name() const BSLS_KEYWORD_NOEXCEPT BSLS_KEYWORD_OVERRIDE;
//          // Return a string describing this error category.
//  };
//
//  // ACCESSORS
//  native_std::string car_category_impl::message(int value) const {
//      switch (value) {
//        case car_errc::car_wheels_came_off: return "The wheels came off";
//        case car_errc::car_engine_fell_out: return "The engine fell out";
//        default:                            return "Some car problem";
//      }
//  }
//
//  const char *car_category_impl::name() const BSLS_KEYWORD_NOEXCEPT {
//      return "car";
//  }
//  }  // close unnamed namespace
//..
// Then, we define functions to get our unique category object, and to make
// error codes and error conditions from our enumeration values.
//..
//  const error_category& car_category()
//      // Return a 'const' reference to the unique car category object.
//  {
//      static car_category_impl car_category_object;
//      return car_category_object;
//  }
//
//  bsl::error_code make_error_code(car_errc::Enum value)
//      // Return a car category error code of the specified 'value'.
//  {
//      return bsl::error_code(static_cast<int>(value), car_category());
//  }
//
//  bsl::error_condition make_error_condition(car_errc::Enum value)
//      // Return a car category error condition of the specified 'value'.
//  {
//      return bsl::error_condition(static_cast<int>(value), car_category());
//  }
//..
// Now, we write a function that can potentially have errors.
//..
//  void drive(bsl::error_code *code, int distance)
//      // Drive a car for the specified 'distance' and set the specified
//      // 'code' to describe any problems encountered.
//  {
//      if (distance > 1000) {
//          *code = make_error_code(car_errc::car_engine_fell_out);
//      }
//      else if (distance > 100) {
//           *code = make_error_code(car_errc::car_wheels_came_off);
//      }
//      else {
//           *code = bsl::error_code(0, car_category());
//      }
//  }
//..
// Finally, we can exercise the function and check for errors.
//..
//  bsl::error_code code;
//  drive(&code, 50);
//  assert(!code);
//  drive(&code, 500);
//  assert(strstr(code.message().c_str(), "wheels"));
//  drive(&code, 5000);
//  assert(strstr(code.message().c_str(), "engine"));
//..

#include <bslscm_version.h>
#include <bslstl_errc.h>
#include <bslstl_iserrorcodeenum.h>
#include <bslstl_iserrorconditionenum.h>

#include <bslh_hash.h>

#include <bslmf_enableif.h>
#include <bslmf_integralconstant.h>

#include <bsls_keyword.h>
#include <bsls_libraryfeatures.h>
#include <bsls_nativestd.h>
#include <bsls_unspecifiedbool.h>

#include <errno.h>
#include <string.h>

#include <cstring>
#include <functional>
#include <ostream>
#include <stdexcept>
#include <string>

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY

#include <system_error>

namespace bsl {
using native_std::error_category;
using native_std::error_code;
using native_std::error_condition;
using native_std::generic_category;
using native_std::system_category;
using native_std::make_error_code;
using native_std::make_error_condition;
}  // close namespace bsl

#else

namespace bsl {

// FORWARD DECLARATIONS
class error_code;
class error_condition;

                            // ====================
                            // class error_category
                            // ====================

class error_category {
    // This 'class' acts as a base for types that represent the source and
    // encoding of error categories.

  private:
    // PRIVATE CREATORS
    error_category(const error_category&);             // = delete

    // PRIVATE MANIPULATORS
    error_category& operator=(const error_category&);  // = delete

  public:
    // CREATORS
    error_category();
        // Create an object of this type.

    virtual ~error_category();
        // Destroy this object.

    // ACCESSORS
    virtual error_condition default_error_condition(int value) const
    BSLS_KEYWORD_NOEXCEPT;
        // Return an 'error_condition' object initialized with the specified
        // 'value' and this object as the error category.

    virtual bool equivalent(int code, const error_condition& condition) const
    BSLS_KEYWORD_NOEXCEPT;
    virtual bool equivalent(const error_code& code, int condition) const
    BSLS_KEYWORD_NOEXCEPT;
        // Return, for the error category defined by this object, whether the
        // specified 'code' and 'condition' are considered equivalent.

    virtual native_std::string message(int value) const = 0;
        // Return a string describing the error condition denoted by the
        // specified 'value'.

    virtual const char *name() const BSLS_KEYWORD_NOEXCEPT = 0;
        // Return the name of this error category.

    bool operator==(const error_category& other) const BSLS_KEYWORD_NOEXCEPT;
        // Return whether this object is the same as the specified 'other'.

    bool operator!=(const error_category& other) const BSLS_KEYWORD_NOEXCEPT;
        // Return whether this object is different than the specified 'other'.

    bool operator<(const error_category& other) const BSLS_KEYWORD_NOEXCEPT;
        // Return whether this object precedes the specified 'other' in a total
        // ordering of 'error_category' objects.
};

                              // ================
                              // class error_code
                              // ================

class error_code {
    // This 'class' represents a system-specific error value.

  private:
    // PRIVATE TYPES
    typedef BloombergLP::bsls::UnspecifiedBool<error_code> UnspecifiedBool;
    typedef UnspecifiedBool::BoolType                      BoolType;

  public:
    // CREATORS
    error_code();
        // Create an object of this type initialized with value 0 and system
        // category.

    error_code(int value, const error_category& category);
        // Create an object of this type initialized with the specified 'value'
        // and 'category'.

    template <class ERROR_CODE_ENUM>
    error_code(ERROR_CODE_ENUM value,
               typename enable_if<is_error_code_enum<ERROR_CODE_ENUM>::value,
                                  BoolType>::type = 0);              // IMPLICT
        // Construct an object of this type initialized with the specified
        // 'value' and its category (found from an overloaded call to
        // 'make_error_code').  Note that this constructor exists only for
        // those types designated as error codes via the 'is_error_code_enum'
        // trait template.

    // MANIPULATORS
    void assign(int value, const error_category& category);
        // Set this object to hold the specified 'value' and 'category'.

    template <class ERROR_CODE_ENUM>
    typename enable_if<is_error_code_enum<ERROR_CODE_ENUM>::value,
                       error_code&>::type
    operator=(ERROR_CODE_ENUM value);
        // Set this object to hold the specified 'value' and its category
        // (found from an overloaded call to 'make_error_code').  Note that
        // this operator exists only for those types designated as error codes
        // via the 'is_error_code_enum' trait template.  Note that this object
        // is set to the generic rather than system category, because that is
        // what the standard specifies.

    void clear();
        // Set this object to hold the value 0 and the system category.

    // ACCESSORS
    const error_category& category() const;
        // Return a 'const' reference to the category held by this object.

    error_condition default_error_condition() const;
        // Return an 'error_condition' object initialized with the value and
        // category of this object.

    native_std::string message() const;
        // Return a string describing this object.

    int value() const;
        // Return the value held by this object.

    operator BoolType() const;
        // Return whether the value held by this object is non-zero.

  private:
    // DATA
    int                   d_value;       // error code value
    const error_category *d_category_p;  // error category
};

                           // =====================
                           // class error_condition
                           // =====================

class error_condition {
    // This 'class' represents a portable error value.

  private:
    // PRIVATE TYPES
    typedef BloombergLP::bsls::UnspecifiedBool<error_condition>
                                      UnspecifiedBool;
    typedef UnspecifiedBool::BoolType BoolType;

  public:
    // CREATORS
    error_condition();
        // Create an object of this type initialized with value 0 and generic
        // category.

    error_condition(int value, const error_category& category);
        // Create an object of this type initialized with the specified 'value'
        // and 'category'.

    template <class ERROR_CONDITION_ENUM>
    error_condition(ERROR_CONDITION_ENUM value,
                    typename enable_if<
                        is_error_condition_enum<ERROR_CONDITION_ENUM>::value,
                        BoolType>::type = 0);                       // IMPLICIT
        // Construct an object of this type initialized with the specified
        // 'value' and its category (found from an overloaded call to
        // 'make_error_condition').  Note that this constructor exists only for
        // those types designated as error conditions via the
        // 'is_error_condition_enum' trait template.

    // MANIPULATORS
    void assign(int value, const error_category& category);
        // Set this object to hold the specified 'value' and 'category'.

    template <class ERROR_CONDITION_ENUM>
    typename enable_if<is_error_condition_enum<ERROR_CONDITION_ENUM>::value,
                       error_condition&>::type
    operator=(ERROR_CONDITION_ENUM value);
        // Set this object to hold the specified 'value' and its category
        // (found from an overloaded call to 'make_error_condition').  Note
        // that this operator exists only for those types designated as error
        // conditions via the 'is_error_condition_enum' trait template.

    void clear();
        // Set this object to hold the value 0 and the generic category.

    // ACCESSORS
    const error_category& category() const;
        // Return a 'const' reference to the category held by this object.

    native_std::string message() const;
        // Return a string describing this object.

    int value() const;
        // Return the value held by this object.

    operator BoolType() const;
        // Return whether the value held by this object is non-zero.

  private:
    // DATA
    int                   d_value;       // error code value
    const error_category *d_category_p;  // error category
};

// FREE FUNCTIONS
const error_category& generic_category();
    // Return a 'const' reference to the unique generic category object.

const error_category& system_category();
    // Return a 'const' reference to the unique system category object.

error_code make_error_code(errc::Enum value);
    // Return an 'error_code' object holding the specified 'value' and generic
    // category.  Note that the category is generic rather than system because
    // that is what the standard specifies.

error_condition make_error_condition(errc::Enum value);
    // Return an 'error_condition' object holding the specified 'value' and
    // generic category.

template <class HASHALG>
void hashAppend(HASHALG& hashAlgorithm, const error_code& object);
    // Hash the specified 'object' using the specified 'hashAlgorithm'.

template <class HASHALG>
void hashAppend(HASHALG& hashAlgorithm, const error_condition& object);
    // Hash the specified 'object' using the specified 'hashAlgorithm'.

// FREE OPERATORS
bool operator==(const error_code& lhs, const error_code& rhs);
bool operator==(const error_code& lhs, const error_condition& rhs);
bool operator==(const error_condition& lhs, const error_code& rhs);
bool operator==(const error_condition& lhs, const error_condition& rhs);
    // Return whether the specified 'lhs' and 'rhs' are equal or equivalent.

bool operator!=(const error_code&, const error_code&);
bool operator!=(const error_code&, const error_condition&);
bool operator!=(const error_condition&, const error_code&);
bool operator!=(const error_condition&, const error_condition&);
    // Return whether the specified 'lhs' and 'rhs' are not equal or
    // equivalent.

bool operator<(const error_code& lhs, const error_code& rhs);
bool operator<(const error_condition& lhs, const error_condition& rhs);
    // Return whether the specified 'lhs' is lexicographically less than the
    // specified 'rhs', ordered by category then value.

template <class CHAR_TYPE, class CHAR_TRAITS>
inline
native_std::basic_ostream<CHAR_TYPE, CHAR_TRAITS>& operator<<(
                     native_std::basic_ostream<CHAR_TYPE, CHAR_TRAITS>& stream,
                     const error_code&                                  code);
    // Write the specified 'code' to 'stream'.

// ============================================================================
//                             INLINE DEFINITIONS
// ============================================================================

                            // --------------------
                            // class error_category
                            // --------------------

//CREATORS
inline
error_category::error_category()
{
}

inline
error_category::~error_category()
{
}

// ACCESSORS
inline
error_condition error_category::default_error_condition(
                                         int value) const BSLS_KEYWORD_NOEXCEPT
{
    return error_condition(value, *this);
}

inline
bool error_category::equivalent(
                  int                    code,
                  const error_condition& condition) const BSLS_KEYWORD_NOEXCEPT
{
    return default_error_condition(code) == condition;
}

inline
bool error_category::equivalent(
                       const error_code& code,
                       int               condition) const BSLS_KEYWORD_NOEXCEPT
{
    return *this == code.category() && code.value() == condition;
}

inline
native_std::string error_category::message(int value) const
{
    return strerror(value);
}

inline
const char *error_category::name() const BSLS_KEYWORD_NOEXCEPT
{
    return "error_category";
}

inline
bool error_category::operator==(
                       const error_category& other) const BSLS_KEYWORD_NOEXCEPT
{
    return this == &other;
}

inline
bool error_category::operator!=(
                       const error_category& other) const BSLS_KEYWORD_NOEXCEPT
{
    return !(*this == other);
}

inline
bool error_category::operator<(
                       const error_category& other) const BSLS_KEYWORD_NOEXCEPT
{
    return native_std::less<const error_category *>()(this, &other);
}

                              // ----------------
                              // class error_code
                              // ----------------

// CREATORS
inline
error_code::error_code()
: d_value(0)
, d_category_p(&system_category())
{
}

inline
error_code::error_code(int value, const error_category& category)
: d_value(value)
, d_category_p(&category)
{
}

template <class ERROR_CODE_ENUM>
inline
error_code::error_code(ERROR_CODE_ENUM value,
                       typename enable_if<
                           is_error_code_enum<ERROR_CODE_ENUM>::value,
                           BoolType>::type)                         // IMPLICIT
: d_value(make_error_code(value).value())
, d_category_p(&make_error_code(value).category())
{
}


// MANIPULATORS
inline
void error_code::assign(int value, const error_category& category)
{
    d_value = value;
    d_category_p = &category;
}

inline
void error_code::clear()
{
    d_value = 0;
    d_category_p = &system_category();
}

template <class ERROR_CODE_ENUM>
inline
typename enable_if<is_error_code_enum<ERROR_CODE_ENUM>::value,
                   error_code&>::type
error_code::operator=(ERROR_CODE_ENUM value)
{
    d_value = make_error_code(value).value();
    d_category_p = &make_error_code(value).category();
    return *this;
}

// ACCESSORS
inline
const error_category& error_code::category() const
{
    return *d_category_p;
}

inline
error_condition error_code::default_error_condition() const
{
    return category().default_error_condition(value());
}

inline
native_std::string error_code::message() const
{
    return category().message(value());
}

inline
int error_code::value() const
{
    return d_value;
}

inline
error_code::operator BoolType() const
{
    return UnspecifiedBool::makeValue(value());
}

// FREE FUNCTIONS
template <class HASHALG>
void hashAppend(HASHALG& hashAlgorithm, const error_code& object)
{
    using ::BloombergLP::bslh::hashAppend;
    hashAppend(hashAlgorithm, static_cast<const void *>(&object.category()));
    hashAppend(hashAlgorithm, object.value());
}

inline
error_code make_error_code(errc::Enum value)
{
    return error_code(static_cast<int>(value), generic_category());
}

// FREE OPERATORS
template <class CHAR_TYPE, class CHAR_TRAITS>
inline
native_std::basic_ostream<CHAR_TYPE, CHAR_TRAITS>& operator<<(
                     native_std::basic_ostream<CHAR_TYPE, CHAR_TRAITS>& stream,
                     const error_code&                                  code)
{
    return stream << code.category().name() << ':' << code.value();
}

                           // ---------------------
                           // class error_condition
                           // ---------------------

// CREATORS
inline
error_condition::error_condition()
: d_value(0)
, d_category_p(&generic_category())
{
}

inline
error_condition::error_condition(int value, const error_category& category)
: d_value(value)
, d_category_p(&category)
{
}

template <class ERROR_CONDITION_ENUM>
inline
error_condition::error_condition(
       ERROR_CONDITION_ENUM value,
       typename enable_if<is_error_condition_enum<ERROR_CONDITION_ENUM>::value,
                          BoolType>::type)                          // IMPLICIT
: d_value(make_error_condition(value).value())
, d_category_p(&make_error_condition(value).category())
{
}

// MANIPULATORS
inline
void error_condition::assign(int value, const error_category& category)
{
    d_value = value;
    d_category_p = &category;
}

inline
void error_condition::clear()
{
    d_value = 0;
    d_category_p = &generic_category();
}

template <class ERROR_CONDITION_ENUM>
inline
typename enable_if<is_error_condition_enum<ERROR_CONDITION_ENUM>::value,
                   error_condition&>::type
error_condition::operator=(ERROR_CONDITION_ENUM value)
{
    d_value = make_error_condition(value).value();
    d_category_p = &make_error_condition(value).category();
    return *this;
}

// ACCESSORS
inline
const error_category& error_condition::category() const
{
    return *d_category_p;
}

inline
native_std::string error_condition::message() const
{
    return category().message(value());
}

inline
int error_condition::value() const
{
    return d_value;
}

inline
error_condition::operator BoolType() const
{
    return UnspecifiedBool::makeValue(value());
}

// FREE FUNCTIONS
template <class HASHALG>
void hashAppend(HASHALG& hashAlgorithm, const error_condition& object)
    // Hash the specified 'object' using the specified 'hashAlgorithm'.
{
    using ::BloombergLP::bslh::hashAppend;
    hashAppend(hashAlgorithm, static_cast<const void *>(&object.category()));
    hashAppend(hashAlgorithm, object.value());
}

inline
error_condition make_error_condition(errc::Enum value)
{
    return error_condition(static_cast<int>(value), generic_category());
}

// FREE OPERATORS
inline
bool operator==(const error_code& lhs, const error_code& rhs)
{
    return lhs.category() == rhs.category() && lhs.value() == rhs.value();
}

inline
bool operator==(const error_code& lhs, const error_condition& rhs)
{
    return lhs.category().equivalent(lhs.value(), rhs) ||
           rhs.category().equivalent(lhs, rhs.value());
}

inline
bool operator==(const error_condition& lhs, const error_code& rhs)
{
    return rhs.category().equivalent(rhs.value(), lhs) ||
           lhs.category().equivalent(rhs, lhs.value());
}

inline
bool operator==(const error_condition& lhs, const error_condition& rhs)
{
    return lhs.category() == rhs.category() && lhs.value() == rhs.value();
}

inline
bool operator!=(const error_code& lhs, const error_code& rhs)
{
    return !(lhs == rhs);
}

inline
bool operator!=(const error_code& lhs, const error_condition& rhs)
{
    return !(lhs == rhs);
}

inline
bool operator!=(const error_condition& lhs, const error_code& rhs)
{
    return !(lhs == rhs);
}

inline
bool operator!=(const error_condition& lhs, const error_condition& rhs)
{
    return !(lhs == rhs);
}

inline
bool operator<(const error_code& lhs, const error_code& rhs)
{
    return lhs.category() < rhs.category() ||
           (lhs.category() == rhs.category() && lhs.value() < rhs.value());
}

inline
bool operator<(const error_condition& lhs, const error_condition& rhs)
{
    return lhs.category() < rhs.category() ||
           (lhs.category() == rhs.category() && lhs.value() < rhs.value());
}
}  // close namespace bsl

#ifdef std
#undef std
#define BSLSTL_ERROR_STD_DEFINED
#endif

namespace std {
template <class TYPE>
struct hash;

template <>
struct hash<bsl::error_code> : BloombergLP::bslh::Hash<>
{
};

template <>
struct hash<bsl::error_condition> : BloombergLP::bslh::Hash<>
{
};
}  // close namespace std

#ifdef BSLSTL_ERROR_STD_DEFINED
#undef BSLSTL_ERROR_STD_DEFINED
#define std bsl
#endif

#endif
#endif

// ----------------------------------------------------------------------------
// Copyright 2019 Bloomberg Finance L.P.
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
