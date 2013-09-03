// btes5_credentials.h                                            -*-C++-*-

#ifndef INCLUDED_BTES5_USERCREDENTIALS
#define INCLUDED_BTES5_USERCREDENTIALS

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide a class for SOCKS5 authentication credentials.
//
//@CLASSES:
//   bteso_UserCredentials: data to authenticate a SOCKS5 connection
//
//@SEE ALSO:
//  btes5_Negotiator
//
//@DESCRIPTION: This component provides a value-semantic attribute class,
// bteso_UserCredentials, containing the credentials to authenticate a
// a connection with a SOCKS5 proxy. Currently, the only authentication method
// supported is USERNAME/PASSWORD as defined in RFC 1929.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Assign User Name and Password
///- - - - - -
// Suppose that we want to specify user name and password for authenticating
// with a SOCKS5 host (see RFC 1929).  We can use a 'btes5_Credentials' object
// to represent the credentials.
//
// First, we construct an empty object:
//..
//      btes5_Credentials credentials1;
//      assert(!credentials1.isSet());
//..
// Now, we set the user name and password passing in literals:
//      credentials1.set("john.smith", "pass1");
//..
// Finally, we verify that 'credentials1' is no longer empty, and its username
// and password values are as expected:
//..
//      assert(credentials1.isSet());
//      assert(credentials1.username() == "john.smith");
//      assert(credentials1.password() == "pass1");

#ifndef INCLUDED_BSLALG_TYPETRAITS
#include <bslalg_typetraits.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITUSESBSLMAALLOCATOR
#include <bslalg_typetraitusesbslmaallocator.h>
#endif

#ifndef INCLUDED_BSL_STRING
#include <bsl_string.h>
#endif

#ifndef INCLUDED_BSLMA_ALLOCATOR
#include <bslma_allocator.h>
#endif

#ifndef INCLUDED_BSLMA_USESBSLMAALLOCATOR
#include <bslma_usesbslmaallocator.h>
#endif

namespace BloombergLP {

                         // ===========================
                         // class btes5_Credentials
                         // ===========================
class btes5_Credentials {
    // This value-semantic type defines the username and password for SOCKS5
    // authentication per RFC 1929. An object of this type can be unset,
    // signifying that password authentication is not available.

    // DATA
    bsl::string              d_username; // authentication username
    bsl::string              d_password; // authentication password

  public:
    // CREATORS
    explicit btes5_Credentials(bslma::Allocator *allocator = 0);
        // Create an unset 'btes5_Credentials' object. If the optionally
        // specified 'allocator' is not 0 use it to supply memory,
        // otherwise use the default allocator.

    btes5_Credentials(const bslstl::StringRef& username,
                      const bslstl::StringRef& password,
                      bslma::Allocator         *allocator = 0);
        // Create a 'btes5_Credentials' object with the specified
        // 'username' and 'password'. If the optionally specified
        // 'allocator' is not 0 use it to supply memory, otherwise use the
        // default allocator. The behavior is undefined unless the length of
        // both 'username' and 'password' is between 1 and 255 inclusive.

    btes5_Credentials(const btes5_Credentials&  original,
                      bslma::Allocator         *allocator = 0);
        // Create a 'btes5_Credentials' object having the same value as the
        // specified 'original' object. If the optionally specified
        // 'allocator' is not 0 use it to supply memory, otherwise use the
        // default allocator.

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
        // Reset this object. 'isSet()' will return 'true' until 'set()' is
        // called on this object.

    // ACCESSORS
    bool isSet() const;
        // Return 'true' if both username and password have been set and
        // 'false' otherwise.

    const bsl::string& username() const;
        // Return a reference to a read-only username attribute of this object.

    const bsl::string& password() const;
        // Return a reference to a read-only password attribute of this object.

};

// FREE OPERATORS
bool operator==(const btes5_Credentials& lhs,
                const btes5_Credentials& rhs);
    // Return 'true' if and only if
    // 'lhs.username() == rhs.username() && lhs.password() == rhs.password()'.

bool operator!=(const btes5_Credentials& lhs,
                const btes5_Credentials& rhs);
    // Return 'true' if and only if
    // 'lhs.username() != rhs.username() || lhs.password() != rhs.password()'.

bsl::ostream& operator<<(bsl::ostream&                output,
                         const btes5_Credentials& object);

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
