// bdljsn_error.h                                                     -*-C++-*-
#ifndef INCLUDED_BDLJSN_ERROR
#define INCLUDED_BDLJSN_ERROR

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a description of an error processing a document.
//
//@CLASSES:
//  bdljsn::Error: a description of a document processing error
//
//@SEE_ALSO: bdljsn_jsonutil, bdljsn_json
//
//@DESCRIPTION: This component provides a single, un-constrained
// (value-semantic) attribute class, 'bdljsn::Error', that is used to describe
// an error in the occured processing a (JSON) document.
//
///Attributes
///----------
//..
//  Name                Type          Default
//  ------------------  -----------   -------
//  location            Location      Location(0)
//  message             string        ""
//..
//: o 'location': the location in the document where the error occured
//: o 'message': a description of the error that occured
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Populating an 'bdljsn::Error' Object
///- - - - - - - - - - - - - - - - - - - - - - - -
// This component is designed to describe an error that occured when processing
// a (JSON) document.  Suppose we are implementing a function,
// 'extractIntegerToken', that parses a numeric token and obtains an 'int'
// value:
//
// First, we define the function signature:
//..
//  int extractIntegerToken(int              *value,
//                          bdljsn::Error    *error,
//                          bsl::string_view  inputText)
//      // Load to the specified 'value' the 'int' value represented by the
//      // specified 'inputText'.  Return 0 on success, and a non-zero value
//      // otherwise with no effect on '*value' and the specified 'error' is
//      // set.
//  {
//      BSLS_ASSERT(value);
//      BSLS_ASSERT(error);
//
//      enum { e_SUCCESS, e_FAILURE };
//      // ...
//..
// Then, we attempt to exact a 'int' value from the 'inputText':
//..
//      int                                      result;
//      bsl::pair<MyParseStatus::Enum, unsigned> status =
//                                 MyNumericUtil::parseInt(&result, inputText);
//..
// Now, we check the parse status and if unsuccessful, we use the status
// information to set the 'bsljsn::Error' object expected by our caller:
//..
//      if (MyParseStatus::e_OK != status.first) {
//          unsigned position = status.second;
//          error->setLocation(bdljsn::Location(static_cast<bsl::uint64_t>(
//                                                                 position)));
//          error->setMessage(MyParseStatus::toAscii(status.first));
//          return e_FAILURE;                                         // RETURN
//      }
//..
// Finally, if the parse was successful, set the output parameter and return
// with status value that indicates success.
//..
//      *value = result;
//      return e_SUCCESS;
//  }
//..

#include <bdlscm_version.h>

#include <bdljsn_location.h>

#include <bslh_hash.h>     // 'bslh::hashAppend'

#include <bslalg_swaputil.h>

#include <bslmf_isbitwisemoveable.h>
#include <bslmf_nestedtraitdeclaration.h>

#include <bsls_assert.h>
#include <bsls_keyword.h>

#include <bsl_ostream.h>
#include <bsl_string.h>
#include <bsl_string_view.h>

namespace BloombergLP {
namespace bdljsn {

                                // ===========
                                // class Error
                                // ===========

class Error {
    // This unconstrained (value-semantic) attribute class specifies a
    // description of an error in processing a (JSON) document.  See the
    // {Attributes} section under {DESCRIPTION} in the component-level
    // documentation for information on the class attributes.  Note that the
    // class invariants are identically the constraints on the individual
    // attributes.

    // DATA
    Location    d_location;  // location where the error occurred
    bsl::string d_message;   // a description of the error

    // FRIENDS
    friend void swap(Error& , Error& );

  public:
    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(Error, bslma::UsesBslmaAllocator);
    BSLMF_NESTED_TRAIT_DECLARATION(Error, bslmf::IsBitwiseMoveable);

    // CREATORS
    Error();
    explicit Error(bslma::Allocator *basicAllocator);
        // Create an 'Error' object having the default value (see
        // {Attributes}).  Optionally specify a 'basicAllocator' used to supply
        // memory.  If 'basicAllocator' is 0, the currently installed default
        // allocator is used.

    Error(const Location&          location,
          const bsl::string_view&  message,
          bslma::Allocator        *basicAllocator = 0);
        // Create an 'Error' object having the specified 'location' and
        // 'message'.  Optionally specify a 'basicAllocator' used to supply
        // memory.  If 'basicAllocator' is 0, the currently installed default
        // allocator is used.

    Error(const Error& original, bslma::Allocator *basicAllocator = 0);
        // Create an 'Error' object having the value of the specified
        // 'original' object.  Optionally specify a 'basicAllocator' used to
        // supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.

    Error(bslmf::MovableRef<Error> original) BSLS_KEYWORD_NOEXCEPT;
        // Create a 'Error' object having the same value and the same allocator
        // as the specified 'original' object.  The value of 'original' becomes
        // unspecified but valid, and its allocator remains unchanged.

    Error(bslmf::MovableRef<Error>  original,
          bslma::Allocator         *basicAllocator);
        // Create a 'Error' object having the same value as the specified
        // 'original' object, and using the specified 'basicAllocator' to
        // supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.  The allocator of 'original' remains
        // unchanged.  If 'original' and the newly created object have the same
        // allocator then the value of 'original' becomes unspecified but
        // valid, and no exceptions will be thrown; otherwise 'original' is
        // unchanged and an exception may be thrown.

    ~Error();
        // Destroy this object.

    // MANIPULATORS
    Error& operator=(const Error& rhs);
        // Assign to this object the value of the specified 'rhs' object, and
        // return a non-'const' reference to this object.

    Error& operator=(bslmf::MovableRef<Error> rhs);
        // Assign to this object the value of the specified 'rhs' object, and
        // return a non-'const' reference to this object.  The allocators of
        // this object and 'rhs' both remain unchanged.  If 'rhs' and this
        // object have the same allocator then the value of 'rhs' becomes
        // unspecified but valid, and no exceptions will be thrown; otherwise
        // 'rhs' is unchanged (and an exception may be thrown).

    Error& reset();
        // Reset this object to the default value (i.e., its value upon default
        // construction).

    Error& setLocation(const Location& value);
        // Set the 'location' attribute of this object to the specified
        // 'value'.

    Error& setMessage(const bsl::string_view& value);
        // Set the 'message' attribute of this object to the specified 'value'.

                        // Aspects

    void swap(Error& other);
        // Efficiently exchange the value of this object with the value of the
        // specified 'other' object.  This method provides the no-throw
        // exception-safety guarantee.  The behavior is undefined unless this
        // object was created with the same allocator as 'other'.

    // ACCESSORS
    const Location& location() const;
        // Return the 'location' attribute of this object.

    const bsl::string&  message() const;
        // Return the 'message' attribute of this object.

                                  // Aspects

    bslma::Allocator *allocator() const;
        // Return the allocator used by this object to supply memory.  Note
        // that if no allocator was supplied at construction the default
        // allocator in effect at construction is used.

    bsl::ostream& print(bsl::ostream& stream,
                        int           level = 0,
                        int           spacesPerLevel = 4) const;
        // Write the value of this object to the specified output 'stream' in a
        // human-readable format, and return a non-'const' reference to
        // 'stream'.  Optionally specify an initial indentation 'level', whose
        // absolute value is incremented recursively for nested objects.  If
        // 'level' is specified, optionally specify 'spacesPerLevel', whose
        // absolute value indicates the number of spaces per indentation level
        // for this and all of its nested objects.  If 'level' is negative,
        // suppress indentation of the first line.  If 'spacesPerLevel' is
        // negative, format the entire output on one line, suppressing all but
        // the initial indentation (as governed by 'level').  If 'stream' is
        // not valid on entry, this operation has no effect.  Note that the
        // format is not fully specified, and can change without notice.
};

// FREE OPERATORS
bool operator==(const Error& lhs, const Error& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects have
    // the same value, and 'false' otherwise.  Two attribute objects have the
    // same value if each respective attribute has the same value.

bool operator!=(const Error& lhs, const Error& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects do not
    // have the same value, and 'false' otherwise.  Two attribute objects do
    // not have the same value if one or more respective attributes differ in
    // values.

bsl::ostream& operator<<(bsl::ostream& stream, const Error& object);
    // Write the value of the specified 'object' to the specified output
    // 'stream' in a single-line format, and return a non-'const' reference to
    // 'stream'.  If 'stream' is not valid on entry, this operation has no
    // effect.  Note that this human-readable format is not fully specified and
    // can change without notice.  Also note that this method has the same
    // behavior as 'object.print(stream, 0, -1)', but with the attribute names
    // elided.

// FREE FUNCTIONS
template <class HASHALG>
void hashAppend(HASHALG& hashAlgorithm, const Error& object);
    // Pass the specified 'object' to the specified 'hashAlgorithm'.  This
    // function integrates with the 'bslh' modular hashing system and
    // effectively provides a 'bsl::hash' specialization for 'ErroError'.

void swap(Error& a, Error& b);
    // Exchange the values of the specified 'a' and 'b' objects.  This function
    // provides the no-throw exception-safety guarantee if the two objects were
    // created with the same allocator and the basic guarantee otherwise.

// ============================================================================
//                         INLINE DEFINITIONS
// ============================================================================

                                // -----------
                                // class Error
                                // -----------

// CREATORS
inline
Error::Error()
: d_location()
, d_message()
{
}

inline
Error::Error(bslma::Allocator *basicAllocator)
: d_location()
, d_message(basicAllocator)
{
}

inline
Error::Error(const Location&          location,
             const bsl::string_view&  message,
             bslma::Allocator        *basicAllocator)
: d_location(location)
, d_message(message, basicAllocator)
{
}

inline
Error::Error(const Error& original, bslma::Allocator *basicAllocator)
: d_location(original.d_location)
, d_message(original.d_message, basicAllocator)
{
}

inline
Error::Error(bslmf::MovableRef<Error> original) BSLS_KEYWORD_NOEXCEPT
: d_location(bslmf::MovableRefUtil::move(
                           bslmf::MovableRefUtil::access(original).d_location))
, d_message(bslmf::MovableRefUtil::move(
                            bslmf::MovableRefUtil::access(original).d_message))
{
}

inline
Error::Error(bslmf::MovableRef<Error>  original,
             bslma::Allocator         *basicAllocator)
: d_location(bslmf::MovableRefUtil::move(
                           bslmf::MovableRefUtil::access(original).d_location))
, d_message(bslmf::MovableRefUtil::move(
                            bslmf::MovableRefUtil::access(original).d_message),
            basicAllocator)
{
}

inline
Error::~Error()
{
}

// MANIPULATORS
inline
Error& Error::operator=(const Error& rhs)
{
    if (this != &rhs) {
        d_message  = rhs.d_message;  // do first for strong guarantee
        d_location = rhs.d_location;
    }

    return *this;
}

inline
Error& Error::operator=(bslmf::MovableRef<Error> rhs)
{
    Error error(bslmf::MovableRefUtil::move(rhs), allocator());
    error.swap(*this);
    return *this;
}

inline
Error& Error::reset()
{
    d_location.reset();
    d_message.clear();
    return *this;
}

inline
Error& Error::setLocation(const Location& value)
{
    d_location = value;
    return *this;
}

inline
Error& Error::setMessage(const bsl::string_view& value)
{
    d_message = value;
    return *this;
}

                        // Aspects

inline
void Error::swap(Error& other)
{
    BSLS_ASSERT(allocator() == other.allocator());

    bslalg::SwapUtil::swap(&d_location, &other.d_location);
    bslalg::SwapUtil::swap(&d_message,  &other.d_message);
}

// ACCESSORS
inline
const Location& Error::location() const
{
    return d_location;
}

inline
const bsl::string& Error::message() const
{
    return d_message;
}

                        // Aspects

inline
bslma::Allocator *Error::allocator() const
{
    return d_message.get_allocator().mechanism();
}

}  // close package namespace

// FREE OPERATORS
inline
bool bdljsn::operator==(const bdljsn::Error& lhs, const bdljsn::Error& rhs)
{
    return lhs.location() == rhs.location() && lhs.message() == rhs.message();
}

inline
bool bdljsn::operator!=(const bdljsn::Error& lhs, const bdljsn::Error& rhs)
{
    return lhs.location() != rhs.location() || lhs.message() != rhs.message();
}

// FREE FUNCTIONS
template <class HASHALG>
inline
void bdljsn::hashAppend(HASHALG& hashAlgorithm, const Error& object)
{
    using ::BloombergLP::bslh::hashAppend;
    hashAppend(hashAlgorithm, object.location());
    hashAppend(hashAlgorithm, object.message());
}

inline
void bdljsn::swap(Error& a, Error& b)
{
    bslalg::SwapUtil::swap(&a.d_location, &b.d_location);
    bslalg::SwapUtil::swap(&a.d_message,  &b.d_message);
}

}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// Copyright 2022 Bloomberg Finance L.P.
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
