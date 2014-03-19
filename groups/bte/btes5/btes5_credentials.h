// btes5_credentials.h                                            -*-C++-*-

#ifndef INCLUDED_BTES5_USERCREDENTIALS
#define INCLUDED_BTES5_USERCREDENTIALS

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide a class for describing SOCKS5 authentication credentials.
//
//@CLASSES:
//  bteso_Credentials: describe SOCKS5 authenticatoin credentials
//
//@SEE ALSO:
//  btes5_Negotiator
//
//@DESCRIPTION: This component provides a single, complex-constrained
// (value-semantic) attribute class, 'bteso_Credentials', that is used to
// describe user credentials to authenticate a connection with a SOCSK5 proxy.
// Currently, the only authentication method supported is "Username/Password"
// as defined in RFC 1929.
//
// [!WARNING!] This component should only be used by API at this time.
//
///Attributes
///----------
//..
//  Name                Type         Default  Constraints
//  ------------------  -----------  -------  -----------
//  username            bsl::string  N/A      empty or 1-255 characters long
//  password            bsl::string  N/A      empty or 1-255 characters long
//..
//: o 'username': username, empty or 1 to 255 octets
//: o 'password': the password associated with 'username', 1 to 255 octets
//
// A 'btes5_Credentials' can be either in the default constucted state with
// both 'username' and 'password' empty, or with both attributes at least 1 and
// at most 255 characters in length.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Assign User Name and Password
/// - - - - - - - - - - - - - - - - - - - -
// Suppose that we want to specify user name and password for authenticating
// with a SOCKS5 host (see RFC 1929).  We can use a 'btes5_Credentials' object
// to represent the credentials.
//
// First, we construct an empty object:
//..
//  btes5_Credentials credentials;
//  assert(!credentials1.isSet());
//..
// Now, we set the user name and password to "john.smith" and "pass1",
// respectively:
//..
//  credentials.set("john.smith", "pass1");
//..
// Finally, we verify that 'credentials1' is no longer empty, and its username
// and password have been assigned the epxected values:
//..
//  assert(credentials1.isSet());
//  assert(credentials1.username() == "john.smith");
//  assert(credentials1.password() == "pass1");
//..

#ifndef INCLUDED_BSLALG_SWAPUTIL
#include <bslalg_swaputil.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITS
#include <bslalg_typetraits.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITUSESBSLMAALLOCATOR
#include <bslalg_typetraitusesbslmaallocator.h>
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

#ifndef INCLUDED_BSL_ALGORITHM
#include <bsl_algorithm.h>
#endif

#ifndef INCLUDED_BSL_IOSFWD
#include <bsl_iosfwd.h>
#endif

#ifndef INCLUDED_BSL_STRING
#include <bsl_string.h>
#endif

namespace BloombergLP {

                         // =======================
                         // class btes5_Credentials
                         // =======================

class btes5_Credentials {
    // This complex-constrained (value-semantic) type describes the username
    // and password for SOCKS5 authentication per RFC 1929.  An object of this
    // type can be unset, signifying that password authentication is not
    // available.

    // DATA
    bsl::string d_username;  // authentication username
    bsl::string d_password;  // authentication password

  public:
    // CREATORS
    explicit btes5_Credentials(bslma::Allocator *basicAllocator = 0);
        // Create an unset 'btes5_Credentials' object.  Optionally specify a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0,
        // the currently installed default allocator is used.

    btes5_Credentials(const bslstl::StringRef&  username,
                      const bslstl::StringRef&  password,
                      bslma::Allocator         *basicAllocator = 0);
        // Create a 'btes5_Credentials' object having the specified 'username'
        // and 'password'.  Optionally specify a 'basicAllocator' used to
        // supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.  The behavior is undefined unless the
        // length of both 'username' and 'password' is between 1 and 255
        // inclusive.

    btes5_Credentials(const btes5_Credentials&  original,
                      bslma::Allocator         *basicAllocator = 0);
        // Create a 'btes5_Credentials' object having the same value as the
        // specified 'original' object.  Optionally specify a 'basicAllocator'
        // used to supply memory.  If 'basicAllocator' is 0, the currently
        // installed default allocator is used.

    // ~btes5_Credentials() = default;
        // Destroy this object.

    // MANIPULATORS
    // operator=(const btes5_Credentials& rhs) = default;
        // Assign to this object the value of the specified 'rhs' object, and
        // return a reference providing modifiable access to this object.

    void set(const bslstl::StringRef& username,
             const bslstl::StringRef& password);
        // Set user credentials to the specified 'username' and 'password'. The
        // behavior is undefined unless the length of both 'username' and
        // 'password' is between 1 and 255 inclusive.

    void reset();
        // Reset this object by setting the 'username' and 'password'
        // attributes to empty strings. After calling 'reset', 'isSet()' will
        // return 'true' until 'set()' is called on this object.

                                  // Aspects

    void swap(btes5_Credentials& other);
        // Efficiently exchange the value of this object with the value of the
        // specified 'other' object.  This method provides the no-throw
        // exception-safety guarantee.  The behavior is undefined unless this
        // object was created with the same allocator as 'other'.

    // ACCESSORS
    bool isSet() const;
        // Return 'true' if both username and password have been set and
        // 'false' otherwise.

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
bool operator==(const btes5_Credentials& lhs,
                const btes5_Credentials& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects have the same
    // value, and 'false' otherwise.  Two 'btes5_Credentials' objects have the
    // same value if all of the corresponding values of their 'username' and
    // 'password' attributes are the same.
 
bool operator!=(const btes5_Credentials& lhs,
                const btes5_Credentials& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects do not have the
    // same value, and 'false' otherwise.  Two 'btes5_Credentials' objects do
    // not have the same value if any of the corresponding values of their
    // 'username' and 'password' attributes are not the same.
 
bool operator!=(const btes5_Credentials& lhs, const btes5_Credentials& rhs);
    // Return 'true' if and only if 'lhs.username() != rhs.username() ||
    // lhs.password() != rhs.password()'.  Return 'true' if and only if
    // 'lhs.username() != rhs.username() || lhs.password() != rhs.password()'.

bsl::ostream& operator<<(bsl::ostream&            stream,
                         const btes5_Credentials& object);
    // Write the value of the specified 'object' to the specified
    // output 'stream' in a single-line format, and return a reference
    // providing modifiable access to 'stream'.  If 'stream' is not valid on
    // entry, this operation has no effect.  Note that this human-readable
    // format is not fully specified and can change without notice.  Also note
    // that this method has the same behavior as 'object.print(stream, 0, -1)',
    // but with the attribute names elided.


// ============================================================================
//                      INLINE FUNCTION DEFINITIONS
// ============================================================================

                        // -----------------------
                        // class btes5_Credentials
                        // -----------------------


// CREATORS
inline
btes5_Credentials::btes5_Credentials(bslma::Allocator *basicAllocator)
: d_username(basicAllocator)
, d_password(basicAllocator)
{
}

inline
btes5_Credentials::btes5_Credentials(const btes5_Credentials&  original,
                                     bslma::Allocator         *basicAllocator)
: d_username(original.d_username, basicAllocator)
, d_password(original.d_password, basicAllocator)
{
}

inline
btes5_Credentials::btes5_Credentials(
    const bslstl::StringRef&  username,
    const bslstl::StringRef&  password,
    bslma::Allocator         *basicAllocator)
: d_username(basicAllocator)
, d_password(basicAllocator)
{
    set(username, password);
}

// MANIPULATORS

                                  // Aspects

inline
void btes5_Credentials::swap(btes5_Credentials& other)
{
    BSLS_ASSERT_SAFE(allocator() == other.allocator());

    bslalg::SwapUtil::swap(&d_username, &other.d_username);
    bslalg::SwapUtil::swap(&d_password, &other.d_password);
}

inline
void btes5_Credentials::reset()
{
    d_username.clear();
    d_password.clear();
}

// ACCESSORS
inline
bool btes5_Credentials::isSet() const
{
    return d_username.length() > 0;
}

inline
const bsl::string& btes5_Credentials::username() const
{
    return d_username;
}

inline
const bsl::string& btes5_Credentials::password() const
{
    return d_password;
}

                                  // Aspects

inline
bslma::Allocator *btes5_Credentials::allocator() const
{
    return d_username.get_allocator().mechanism();
}

// FREE OPERATORS
inline
bool operator==(const btes5_Credentials& lhs,
                const btes5_Credentials& rhs)
{
    return lhs.username() == rhs.username()
        && lhs.password() == rhs.password();
}

inline
bool operator!=(const btes5_Credentials& lhs,
                const btes5_Credentials& rhs)
{
    return lhs.username() != rhs.username()
        || lhs.password() != rhs.password();
}


// FREE FUNCTIONS
inline
void swap(btes5_Credentials& a, btes5_Credentials& b)
{
    a.swap(b);
}

// TRAITS
namespace bslma {
    template<> struct UsesBslmaAllocator<btes5_Credentials>
        : bsl::true_type {
    };
}

}  // close enterprise namespace

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2013
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
