// btls5_credentials.h                                                -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#ifndef INCLUDED_BTLS5_CREDENTIALS
#define INCLUDED_BTLS5_CREDENTIALS

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a class for describing SOCKS5 authentication credentials.
//
//@CLASSES:
//  btls5::Credentials: describe SOCKS5 authentication credentials
//
//@SEE_ALSO: btls5_negotiator
//
//@DESCRIPTION: This component provides a single, complex-constrained
// (value-semantic) attribute class, 'btls5::Credentials', that is used to
// describe user credentials used to authenticate a connection with a SOCSK5
// proxy using the "Username/Password" method as defined in RFC 1929.
//
///Attributes
///----------
//..
//  Name                Type         Default  Constraints
//  ------------------  -----------  -------  -----------
//  username            bsl::string  ""       empty or 1-255 characters long
//  password            bsl::string  ""       empty or 1-255 characters long
//..
//: o 'username': username, empty or 1 to 255 octets
//: o 'password': the password associated with 'username', 1 to 255 octets
//
// A 'btls5::Credentials' can be either in the default constructed state with
// both 'username' and 'password' empty, or with both attributes at least 1 and
// at most 255 characters in length.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Assign Username and Password
///- - - - - - - - - - - - - - - - - - - -
// Suppose that we want to specify username and password for authenticating
// with a SOCKS5 host (see RFC 1929).  We can use a 'btls5::Credentials' object
// to represent the credentials.
//
// First, we construct an empty object:
//..
//  btls5::Credentials credentials;
//..
// Now, we set the username and password passing in literals:
//..
//  credentials.set("john.smith", "pass1");
//..
// Finally, we verify that 'credentials' is no longer empty, and its username
// and password values have been assigned the expected values:
//..
//  assert(credentials.username() == "john.smith");
//  assert(credentials.password() == "pass1");
//..

#ifndef INCLUDED_BTLSCM_VERSION
#include <btlscm_version.h>
#endif

#ifndef INCLUDED_BSLALG_SWAPUTIL
#include <bslalg_swaputil.h>
#endif

#ifndef INCLUDED_BSLMA_ALLOCATOR
#include <bslma_allocator.h>
#endif

#ifndef INCLUDED_BSLMA_USESBSLMAALLOCATOR
#include <bslma_usesbslmaallocator.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_BSL_IOSFWD
#include <bsl_iosfwd.h>
#endif

#ifndef INCLUDED_BSL_STRING
#include <bsl_string.h>
#endif

namespace BloombergLP {

namespace bslma { class Allocator; }

namespace btls5 {
                             // =================
                             // class Credentials
                             // =================

class Credentials {
    // This complex-constrained (value-semantic) type describes the username
    // and password for SOCKS5 authentication per RFC 1929.

    // DATA
    bsl::string d_username;  // authentication username
    bsl::string d_password;  // authentication password

  public:
    // CREATORS
    explicit Credentials(bslma::Allocator *basicAllocator = 0);
        // Create a 'Credentials' object having the (default) attribute
        // values:
        //..
        //  username() == ""
        //  password() == ""
        //..
        // Optionally specify a 'basicAllocator' used to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.

    Credentials(const bslstl::StringRef&  username,
                const bslstl::StringRef&  password,
                bslma::Allocator         *basicAllocator = 0);
        // Create a 'Credentials' object having the specified 'username' and
        // 'password'.  Optionally specify a 'basicAllocator' used to supply
        // memory.  If 'basicAllocator' is 0, the currently installed default
        // allocator is used.  The behavior is undefined unless both 'username'
        // and 'password' are empty, or the lengths of both 'username' and
        // 'password' are between 1 and 255 characters inclusive.

    Credentials(const Credentials&  original,
                bslma::Allocator   *basicAllocator = 0);
        // Create a 'Credentials' object having the same value as the specified
        // 'original' object.  Optionally specify a 'basicAllocator' used to
        // supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.

    //! ~Credentials() = default;
        // Destroy this object.

    // MANIPULATORS
    //! Credentials& operator=(const Credentials& rhs) = default;
        // Assign to this object the value of the specified 'rhs' object, and
        // return a reference providing modifiable access to this object.

    void set(const bslstl::StringRef& username,
             const bslstl::StringRef& password);
        // Set the attributes of this object to the specified 'username' and
        // 'password'.  The behavior is undefined unless both 'username' and
        // 'password' are empty, or the lengths of both 'username' and
        // 'password' are between 1 and 255 characters inclusive.

                                  // Aspects

    void swap(Credentials& other);
        // Efficiently exchange the value of this object with the value of the
        // specified 'other' object.  This method provides the no-throw
        // exception-safety guarantee.  The behavior is undefined unless this
        // object was created with the same allocator as 'other'.

    // ACCESSORS
    const bsl::string& username() const;
        // Return a reference providing non-modifiable access to the 'username'
        // attribute of this object.

    const bsl::string& password() const;
        // Return a reference providing non-modifiable access to the 'password'
        // attribute of this object.

                                  // Aspects

    bslma::Allocator *allocator() const;
        // Return the allocator used by this object to supply memory.  Note
        // that if no allocator was supplied at construction the currently
        // installed default allocator is used.

    bsl::ostream& print(bsl::ostream& stream,
                        int           level = 0,
                        int           spacesPerLevel = 4) const;
        // Write the value of this object to the specified output 'stream' in a
        // human-readable format, and return a reference to 'stream'.
        // Optionally specify an initial indentation 'level', whose absolute
        // value is incremented recursively for nested objects.  If 'level' is
        // specified, optionally specify 'spacesPerLevel', whose absolute value
        // indicates the number of spaces per indentation level for this and
        // all of its nested objects.  If 'level' is negative, suppress
        // indentation of the first line.  If 'spacesPerLevel' is negative,
        // format the entire output on one line, suppressing all but the
        // initial indentation (as governed by 'level').  If 'stream' is not
        // valid on entry, this operation has no effect.  Note that the format
        // is not fully specified, and can change without notice.
};

// FREE OPERATORS
bool operator==(const Credentials& lhs, const Credentials& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects have the same
    // value, and 'false' otherwise.  Two 'Credentials' objects have the same
    // value if both of the corresponding values of their 'username' and
    // 'password' attributes are the same.

bool operator!=(const Credentials& lhs, const Credentials& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects do not have the
    // same value, and 'false' otherwise.  Two 'Credentials' objects do not
    // have the same value if either of the corresponding values of their
    // 'username' and 'password' attributes are not the same.

bsl::ostream& operator<<(bsl::ostream& stream, const Credentials& object);
    // Write the value of the specified 'object' to the specified output
    // 'stream' in a single-line format, and return a reference providing
    // modifiable access to 'stream'.  If 'stream' is not valid on entry, this
    // operation has no effect.  Note that this human-readable format is not
    // fully specified and can change without notice.

// FREE FUNCTIONS
void swap(btls5::Credentials& a, btls5::Credentials& b);
    // Efficiently exchange the values of the specified 'a' and 'b' objects.
    // This function provides the no-throw exception-safety guarantee.  The
    // behavior is undefined unless the two objects were created with the same
    // allocator.

// ============================================================================
//                        INLINE FUNCTION DEFINITIONS
// ============================================================================

                             // -----------------
                             // class Credentials
                             // -----------------

// CREATORS
inline
Credentials::Credentials(bslma::Allocator *basicAllocator)
: d_username(basicAllocator)
, d_password(basicAllocator)
{
}

inline
Credentials::Credentials(const Credentials&  original,
                         bslma::Allocator   *basicAllocator)
: d_username(original.d_username, basicAllocator)
, d_password(original.d_password, basicAllocator)
{
}

inline
Credentials::Credentials(const bslstl::StringRef&  username,
                         const bslstl::StringRef&  password,
                         bslma::Allocator         *basicAllocator)
: d_username(basicAllocator)
, d_password(basicAllocator)
{
    set(username, password);  // assert preconditions and set attributes
}

// MANIPULATORS

                                  // Aspects

inline
void Credentials::swap(Credentials& other)
{
    BSLS_ASSERT_SAFE(allocator() == other.allocator());

    bslalg::SwapUtil::swap(&d_username, &other.d_username);
    bslalg::SwapUtil::swap(&d_password, &other.d_password);
}

// ACCESSORS
inline
const bsl::string& Credentials::username() const
{
    return d_username;
}

inline
const bsl::string& Credentials::password() const
{
    return d_password;
}

                                  // Aspects

inline
bslma::Allocator *Credentials::allocator() const
{
    return d_username.get_allocator().mechanism();
}

}  // close package namespace

// FREE OPERATORS
inline
bool btls5::operator==(const btls5::Credentials& lhs,
                       const btls5::Credentials& rhs)
{
    return lhs.username() == rhs.username()
        && lhs.password() == rhs.password();
}

inline
bool btls5::operator!=(const btls5::Credentials& lhs,
                       const btls5::Credentials& rhs)
{
    return lhs.username() != rhs.username()
        || lhs.password() != rhs.password();
}

// FREE FUNCTIONS
inline
void btls5::swap(btls5::Credentials& a, btls5::Credentials& b)
{
    a.swap(b);
}

// TRAITS
namespace bslma {

template<>
struct UsesBslmaAllocator<btls5::Credentials> : bsl::true_type {
};

}  // close namespace bslma

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
