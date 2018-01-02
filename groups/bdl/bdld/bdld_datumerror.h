// bdld_datumerror.h                                                  -*-C++-*-

#ifndef INCLUDED_BDLD_DATUMERROR
#define INCLUDED_BDLD_DATUMERROR

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id$ $CSID$")

//@PURPOSE: Provide a type for an error code with an optional error message.
//
//@CLASSES:
//  bdld::DatumError: type for an error code with an optional error message
//
//@SEE_ALSO: bdld_datum, bdld_datumudt
//
//@DESCRIPTION: This component defines a *complex-constrained* *value-semantic*
// *attribute* class 'bdld::DatumError' representing an error code with an
// optional descriptive error message.  This component holds a reference to the
// error message that was supplied at construction.  Accessors inside 'Datum'
// class that need to return an error value, return an instance of
// 'DatumError'.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Basic 'DatumError' usage
///- - - - - - - - - - - - - - - - - -
// Suppose we need a function to verify if newly created password meets basic
// security requirements.  Password must contain at least one uppercase letter,
// one lowercase letter, one numeral and one special symbol.  The following
// code illustrates how to use 'bdlb::DatumError' to notify user about
// password weaknesses.
//
// First, we need to write a verification function:
//..
//  bdld::DatumError verifyNewPassword(const char *password)
//      // Verify if specified 'password' meets basic security requirements.
//  {
//      bool uppercasePresence     = false;
//      bool lowercasePresence     = false;
//      bool numeralPresence       = false;
//      bool specialSymbolPresence = false;
//..
// Passed string analysis:
//..
//      while (*password) {
//          if (*password >= 'A' && *password <= 'Z') {
//              uppercasePresence = true;
//          }
//          if (*password >= 'a' && *password <= 'z') {
//              lowercasePresence = true;
//          }
//          if (*password >= '0' && *password <= '9') {
//              numeralPresence = true;
//          }
//          if (*password >= '!' && *password <= '.') {
//              specialSymbolPresence = true;
//          }
//          ++password;
//      }
//..
// Result compilation:
//..
//      bdld::DatumError result;
//
//      if (!uppercasePresence) {
//          result = bdld::DatumError(1, bslstl::StringRef("Uppercase"));
//      } else if (!lowercasePresence) {
//          result = bdld::DatumError(2, bslstl::StringRef("Lowercase"));
//      } else if (!numeralPresence) {
//          result = bdld::DatumError(3, bslstl::StringRef("Numeral"));
//      } else if (!specialSymbolPresence) {
//          result = bdld::DatumError(4, bslstl::StringRef("Special"));
//      }
//
//      return result;
//  }
//..
// Next, we need to create password for verification and call our function:
//..
//  bdld::DatumError error = verifyNewPassword("Test");
//..
// Then, check the results:
//..
//  assert(bdld::DatumError() != error);
//  assert(3                  == error.code());
//..
// Finally, we can print the result to the output stream:
//..
//  ostringstream out;
//  error.print(out);
//..

#ifndef INCLUDED_BDLSCM_VERSION
#include <bdlscm_version.h>
#endif

#ifndef INCLUDED_BSLMF_ISTRIVIALLYCOPYABLE
#include <bslmf_istriviallycopyable.h>
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

#ifndef INCLUDED_BSL_IOSFWD
#include <bsl_iosfwd.h>
#endif

namespace BloombergLP {

namespace bdld {
                               // ================
                               // class DatumError
                               // ================
class DatumError {
    // This component 'class' provides a *complex constrained* attribute type,
    // 'bdld::DatumError', that represents an error code with an optional
    // descriptive error message.  Note that this class holds a reference to
    // the error message and does not make a copy of it.

  private:
    // DATA
    int               d_code;     // error code
    bslstl::StringRef d_message;  // error message

  public:
    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(DatumError, bsl::is_trivially_copyable);

    // CREATORS
    DatumError();
        // Create a 'DatumError' object having the default error code of 0 and
        // an empty error message.

    explicit DatumError(int code);
        // Create a 'DatumError' object having the specified error 'code' value
        // and and empty error message.

    DatumError(int code, const bslstl::StringRef& message);
        // Create a 'DatumError' object having the specified error 'code' value
        // and the specified error 'message' value.  Note that the 'message' is
        // held by the reference and not copied.

    //! DatumError(const DatumError& origin) = default;
    //! ~DatumError() = default;

    // MANIPULATORS
    //! DatumError& operator=(const DatumError& rhs) = default;

    // ACCESSORS
    int code() const;
        // Return the error code.

    bslstl::StringRef message() const;
        // Return a reference to the non-modifyable error message.  The
        // returned reference remains valid as long as the underlying message
        // is not modified or destroyed -- irrespective of the state (or
        // existence) of this object.  The behavior is undefined unless the
        // underlying error message has been modified or destroyed since this
        // object was created.

    bsl::ostream& print(bsl::ostream& stream,
                        int           level          = 0,
                        int           spacesPerLevel = 4) const;
        // Write the value of this object to the specified output 'stream' in a
        // human-readable format, and return a reference to the modifyable
        // 'stream'.  Optionally specify an initial indentation 'level', whose
        // absolute value is incremented recursively for nested objects.  If
        // 'level' is specified, optionally specify 'spacesPerLevel', whose
        // absolute value indicates the number of spaces per indentation level
        // for this and all of its nested objects.  If 'level' is negative,
        // suppress indentation of the first line.  If 'spacesPerLevel' is
        // negative, format the entire output on one line, suppressing all but
        // the initial indentation (as governed by 'level').  If 'stream' is
        // not valid on entry, this operation has no effect.  Note that this
        // human-readable format is not fully specified, and can change without
        // notice.
};

// FREE OPERATORS
bool operator==(const DatumError& lhs, const DatumError& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' have the same value and
    // 'false' otherwise.  Two 'DatumError' objects have the same value if they
    // have same error code and message values.

bool operator!=(const DatumError& lhs, const DatumError& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' have different values and
    // 'false' otherwise.  Two 'DatumError' objects have different values if
    // they have different error code or message values.

bool operator<(const DatumError& lhs, const DatumError& rhs);
    // Return 'true' if value of the specified 'lhs' is less than value of the
    // specified 'rhs' and 'false' otherwise.  Value of 'lhs' is less than
    // value of 'rhs' if error code value of 'lhs' is less than error code
    // value of 'rhs', or they have the same error code value and error message
    // value of 'lhs' is less than error message value of 'rhs'.

bool operator<=(const DatumError& lhs, const DatumError& rhs);
    // Return 'true' if value of the specified 'lhs' is less than or equal to
    // value of the specified 'rhs' and 'false' otherwise.  Value of 'lhs' is
    // less than or equal to value of 'rhs' if error code value of 'lhs' is
    // less than or equal to error code value of 'rhs', or they have the same
    // error code value and error message value of 'lhs' is less than or equal
    // to error message value of 'rhs'.

bool operator>(const DatumError& lhs, const DatumError& rhs);
    // Return 'true' if value of the specified 'lhs' is greater than value of
    // the specified 'rhs' and 'false' otherwise.  Value of 'lhs' is greater
    // than value of 'rhs' if error code value of 'lhs' is greater than error
    // code value of 'rhs', or they have the same error code value and error
    // message value of 'lhs' is greater than error message value of 'rhs'.

bool operator>=(const DatumError& lhs, const DatumError& rhs);
    // Return 'true' if value of the specified 'lhs' is greater than or equal
    // to value of the specified 'rhs' and 'false' otherwise.  Value of 'lhs'
    // is greater than or equal to value of 'rhs' if error code value of 'lhs'
    // is greater than or equal to error code value of 'rhs', or they have the
    // same error code value and error message value of 'lhs' is greater than
    // or equal to error message value of 'rhs'.

bsl::ostream& operator<<(bsl::ostream& stream, const DatumError& rhs);
    // Write the value of the specified 'rhs' object to the specified output
    // 'stream' in a single-line format, and return a reference to the
    // modifyable 'stream'.  If 'stream' is not valid on entry, this operation
    // has no effect.  Note that this human-readable format is not fully
    // specified, can change without notice, and is logically equivalent to:
    //..
    //  print(stream, 0, -1);
    //..
    // Single line output format for the 'DatumError' object is shown below:
    //..
    //  error(n)
    //  error(n,'msg')
    //..
    // (where n is the integer error code value and 'msg' is the error message
    // value in single quotes).  Note that the first version will be output if
    // there is no error message string.

// ============================================================================
//                               INLINE DEFINITIONS
// ============================================================================

                                // ----------------
                                // class DatumError
                                // ----------------
// CREATORS
inline
DatumError::DatumError()
: d_code(0)
{
}

inline
DatumError::DatumError(int code)
: d_code(code)
{
}

inline
DatumError::DatumError(int code, const bslstl::StringRef& message)
: d_code(code)
, d_message(message)
{
}

// ACCESSORS
inline
int DatumError::code() const
{
    return d_code;
}

inline
bslstl::StringRef DatumError::message() const
{
    return d_message;
}

}  // close package namespace

// FREE OPERATORS
inline
bool bdld::operator==(const DatumError& lhs, const DatumError& rhs)
{
    return (lhs.code() == rhs.code() && lhs.message() == rhs.message());
}

inline
bool bdld::operator!=(const DatumError& lhs, const DatumError& rhs)
{
    return !(lhs == rhs);
}

inline
bool bdld::operator<(const DatumError& lhs, const DatumError& rhs)
{
    return (lhs.code() < rhs.code() ||
            (lhs.code() == rhs.code() && lhs.message() < rhs.message()));
}

inline
bool bdld::operator<=(const DatumError& lhs, const DatumError& rhs)
{
    return (lhs == rhs || lhs < rhs);
}

inline
bool bdld::operator>(const DatumError& lhs, const DatumError& rhs)
{
    return !(lhs <= rhs);
}

inline
bool bdld::operator>=(const DatumError& lhs, const DatumError& rhs)
{
    return !(lhs < rhs);
}

inline
bsl::ostream& bdld::operator<<(bsl::ostream& stream, const DatumError& rhs)
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

