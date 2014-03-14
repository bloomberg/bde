// btes5_detailederror.h                                              -*-C++-*-
#ifndef INCLUDED_BTES5_DETAILEDERROR
#define INCLUDED_BTES5_DETAILEDERROR

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide a class to describe SOCKS5 connection errors.
//
//@CLASSES:
//   btes5_DetailedError: detailed information about a SOCKS5 connection error
//
//@SEE ALSO:
//
//@DESCRIPTION: This component provides a value-semantic attribute class,
// 'btes5_DetailedError', describing an error as a result of SOCKS5 connection.
//
// [!WARNING!] This component should only be used by API at this time.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Assign Error Information
///- - - - - - - - - - - - - - - - - -
// Suppose we encounter an authentication failure during a SOCKS5 negotiation.
// We would like to encode the information about this error before signaling
// the client code of the error.
//
// First, we construct an empty 'btes5_DetailedError object with the failure
// description:
//..
//  btes5_DetailedError error("authentication failure");
//..
//  Now, we set the address of the proxy host that reported the error:
//..
//  bteso_Endpoint proxy("proxy1.corp.com", 1080);
//  error.setAddress(proxy);
//..
// Finally, we have an encoded 'error' which provides detailed information
// about the failure.
//..
//  assert("authentication failure" == error.description());
//  assert(proxy == error.address());
//..

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

#ifndef INCLUDED_BTESO_ENDPOINT
#include <bteso_endpoint.h>
#endif

#ifndef INCLUDED_BSL_IOSFWD
#include <bsl_iosfwd.h>
#endif

namespace BloombergLP {

                          // =========================
                          // class btes5_DetailedError
                          // =========================
struct btes5_DetailedError {
    // This value-semantic struct provides information about an error during a
    // connection attempt using SOCKS5. Note that the address attribute may not
    // be set if there is no specific proxy host associated with this
    // 'btes5_DetailedError' object.

    // DATA
    bsl::string    d_description; // text message
    bteso_Endpoint d_address;     // address of the host originating this error

  public:
    // CREATORS
    btes5_DetailedError(const bslstl::StringRef&  description,
                        bslma::Allocator         *allocator = 0);
        // Create a 'btes5_DetailedError' object with the specified
        // 'description'. If the optionally specified 'allocator' is not 0 use
        // it to supply memory, otherwise use the default allocator.

    btes5_DetailedError(const bslstl::StringRef& description,
                        const bteso_Endpoint&    address,
                        bslma::Allocator        *allocator = 0);
        // Create a 'btes5_DetailedError' object with the specified
        // 'description' and specified 'address'. If the optionally specified
        // 'allocator' is not 0 use it to supply memory, otherwise use the
        // default allocator.

    btes5_DetailedError(const btes5_DetailedError&  original,
                        bslma::Allocator           *allocator = 0);
        // Create a 'btes5_DetailedError' object having the same value
        // as the specified 'original' object. If the optionally specified
        // 'allocator' is not 0 use it to supply memory, otherwise use the
        // default allocator.

    // ~btes5_DetailedError() = default;
        // Destroy this object.

    // MANIPULATORS
    void setDescription(const bslstl::StringRef& value);
        // Set the description attribute of this object to the specified
        // 'value'.

    void setAddress(const bteso_Endpoint& value);
        // Set the adddress attribute of this object to the specified 'value'.

    // ACCESSORS
    const bsl::string& description() const;
        // Return a reference providing non-modifiable access to the
        // description attribute of this object.

    const bteso_Endpoint& address() const;
        // Return a reference providing non-modifiable access to the adress
        // attribute of this object.

};

// FREE OPERATORS
bool operator==(const btes5_DetailedError& lhs,
                const btes5_DetailedError& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects have the same
    // description and address attributes, and 'false' otherwise.

bool operator!=(const btes5_DetailedError& lhs,
                const btes5_DetailedError& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects have different
    // description or different address attributes, and 'false' otherwise.

bsl::ostream& operator<<(bsl::ostream&              output,
                         const btes5_DetailedError& error);
    // Write the specified 'error' to the specified 'output' in human-readable
    // format, and return the reference to 'output'.

// TRAITS
namespace bslma {
    template<> struct UsesBslmaAllocator<btes5_DetailedError>
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
