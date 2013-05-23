// btes5_usercredentials.h                                            -*-C++-*-

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
// bteso_UserCredentials containing the credentials to authenticate a
// a connection with a SOCKS5 proxy. Currently, the only authentication method
// supported is USERNAME/PASSWORD as defined in RFC 1929.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1:
///- - - - - -
// Suppose that ...
//

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

namespace BloombergLP {

                         // ===========================
                         // class btes5_UserCredentials
                         // ===========================
class btes5_UserCredentials {
    // This value-semantic type defines the username and password for SOCKS5
    // authentication per RFC 1929. An object of this type can be unset,
    // signifying that password authentication is not available.

    // DATA
    bsl::string              d_username; // authentication username
    bsl::string              d_password; // authentication password

  public:
    // CREATORS
    explicit btes5_UserCredentials(bslma::Allocator *allocator = 0);
        // Create an unset 'btes5_UserCredentials' object. If the optionally
        // specified 'allocator' is not 0 use it to supply memory,
        // otherwise use the default allocator.

    btes5_UserCredentials(const bslstl::StringRef&  username,
                          const bslstl::StringRef&  password,
                          bslma::Allocator         *allocator = 0);
        // Create a 'btes5_UserCredentials' object with the specified
        // 'username' and 'password'. If the optionally specified
        // 'allocator' is not 0 use it to supply memory, otherwise use the
        // default allocator. The behavior is undefined unless the length of
        // both 'username' and 'password' is between 1 and 255 inclusive.

    btes5_UserCredentials(const btes5_UserCredentials&  original,
                          bslma::Allocator             *allocator = 0);
        // Create a 'btes5_UserCredentials' object having the same value as the
        // specified 'original' object. If the optionally specified
        // 'allocator' is not 0 use it to supply memory, otherwise use the
        // default allocator.

    ~btes5_UserCredentials();
        // Destroy this object.

    // MANIPULATORS
    // operator=(const btes5_UserCredentials& rhs) = default;
        // Assign to this object the value of the specified 'rhs' object, and
        // return a reference providing modifiable access to this object.

    void set(const bslstl::StringRef& username,
             const bslstl::StringRef& password);
        // Set user credentials to the specified 'username' and 'password'. The
        // behavior is undefined unless the length of both 'username' and
        // 'password' is between 1 and 255 inclusive.

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
bool operator==(const btes5_UserCredentials& lhs,
                const btes5_UserCredentials& rhs);
    // Return 'true' if and only if
    // 'lhs.username() == rhs.username() && lhs.password() == rhs.password()'.

bool operator!=(const btes5_UserCredentials& lhs,
                const btes5_UserCredentials& rhs);
    // Return 'true' if and only if
    // 'lhs.username() != rhs.username() || lhs.password() != rhs.password()'.

bsl::ostream& operator<<(bsl::ostream&                output,
                         const btes5_UserCredentials& object);

// TRAITS
namespace bslma {
    template<> struct UsesBslmaAllocator<btes5_UserCredentials>
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
