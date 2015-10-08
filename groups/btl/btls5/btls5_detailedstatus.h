// btls5_detailedstatus.h                                             -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#ifndef INCLUDED_BTLS5_DETAILEDSTATUS
#define INCLUDED_BTLS5_DETAILEDSTATUS

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide an attribute class to describe SOCKS5 connection status.
//
//@CLASSES:
//  btls5::DetailedStatus: attributes describing a SOCKS5 connection status
//
//@SEE_ALSO:
//
//@DESCRIPTION: This component provides a single, unconstrained
// (value-semantic) attribute class, 'btls5::DetailedStatus', that is used to
// describe the status of a SOCKS5 connection attempt.
//
///Attributes
///----------
//..
//  Name           Type              Default             Simple Constraints
//  -------------  -----------       -------             ------------------
//  description    bsl::string       ""                  none
//  address        btlso::Endpoint   btlso::Endpoint()   none
//..
//: o 'description': non-canonical, human-readable status information, suitable
//:   for logging and diagnostics
//:
//: o 'address': address of the node (proxy) associated with the status
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Assign Error Information
///- - - - - - - - - - - - - - - - - -
// Suppose we encounter an authentication failure during a SOCKS5 negotiation.
// We would like to encode the information about this error before informing
// the client code of the error.
//
// First, we construct an empty 'btls5::DetailedStatus' object with the failure
// description:
//..
//  btls5::DetailedStatus error("authentication failure");
//..
// Now, we set the address of the proxy host that reported the error:
//..
//  btlso::Endpoint proxy("proxy1.corp.com", 1080);
//  error.setAddress(proxy);
//..
// Finally, we have an encoded 'error' which provides detailed information
// about the failure.
//..
//  assert("authentication failure" == error.description());
//  assert(proxy                    == error.address());
//..

#ifndef INCLUDED_BTLSCM_VERSION
#include <btlscm_version.h>
#endif

#ifndef INCLUDED_BTLSO_ENDPOINT
#include <btlso_endpoint.h>
#endif

#ifndef INCLUDED_BSLALG_SWAPUTIL
#include <bslalg_swaputil.h>
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

                            // ====================
                            // class DetailedStatus
                            // ====================

class DetailedStatus {
    // This value-semantic class provides information about the status of a
    // connection attempt using SOCKS5.

    // DATA
    bsl::string     d_description; // text message
    btlso::Endpoint d_address;     // address associated with this status

  public:
    // CREATORS
    explicit DetailedStatus(bslma::Allocator *basicAllocator = 0);
        // Create a 'DetailedStatus' object having the (default) attribute
        // values:
        //..
        //  description() == ""
        //  address()     == btlso::Endpoint()
        //..
        // Optionally specify a 'basicAllocator' used to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.

    explicit DetailedStatus(const bslstl::StringRef&  description,
                            bslma::Allocator         *basicAllocator = 0);
        // Create a 'DetailedStatus' object having the specified 'description',
        // and having a default-constructed address value.  Optionally specify
        // a 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0,
        // the currently installed default allocator is used.

    DetailedStatus(const bslstl::StringRef&  description,
                   const btlso::Endpoint&    address,
                   bslma::Allocator         *basicAllocator = 0);
        // Create a 'DetailedStatus' object having the specified 'description'
        // and 'address'.  Optionally specify a 'basicAllocator' used to supply
        // memory.  If 'basicAllocator' is 0, the currently installed default
        // allocator is used.

    DetailedStatus(const DetailedStatus&  original,
                   bslma::Allocator      *basicAllocator = 0);
        // Create a 'DetailedStatus' object having the same value as the
        // specified 'original' object.  Optionally specify a 'basicAllocator'
        // used to supply memory.  If 'basicAllocator' is 0, the currently
        // installed default allocator is used.

    // ~DetailedStatus() = default;
        // Destroy this object.

    // MANIPULATORS
    //! DetailedStatus& operator=(const DetailedStatus& rhs) = default;
        // Assign to this object the value of the specified 'rhs' object, and
        // return a reference providing modifiable access to this object.

    void setDescription(const bslstl::StringRef& value);
        // Set the 'description' attribute of this object to the specified
        // 'value'.

    void setAddress(const btlso::Endpoint& value);
        // Set the 'address' attribute of this object to the specified 'value'.

                                  // Aspects

    void swap(DetailedStatus& other);
        // Efficiently exchange the value of this object with the value of the
        // specified 'other' object.  This method provides the no-throw
        // exception-safety guarantee.  The behavior is undefined unless this
        // object was created with the same allocator as 'other'.

    // ACCESSORS
    const bsl::string& description() const;
        // Return a reference providing non-modifiable access to the
        // description attribute of this object.

    const btlso::Endpoint& address() const;
        // Return a reference providing non-modifiable access to the address
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
bool operator==(const DetailedStatus& lhs, const DetailedStatus& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects have the same
    // value, and 'false' otherwise.  Two 'DetailedStatus' objects have the
    // same value if both of the corresponding values of their 'description'
    // and 'address' attributes are the same.

bool operator!=(const DetailedStatus& lhs, const DetailedStatus& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects do not have the
    // same value, and 'false' otherwise.  Two 'DetailedStatus' objects do not
    // have the same value if either of the corresponding values of their
    // 'description' and 'address' attributes are not the same.

bsl::ostream& operator<<(bsl::ostream&         stream,
                         const DetailedStatus& object);
    // Write the value of the specified 'object' to the specified output
    // 'stream' in a single-line format, and return a reference providing
    // modifiable access to 'stream'.  If 'stream' is not valid on entry, this
    // operation has no effect.  Note that this human-readable format is not
    // fully specified and can change without notice.

// FREE FUNCTIONS
void swap(btls5::DetailedStatus& a, btls5::DetailedStatus& b);
    // Efficiently exchange the values of the specified 'a' and 'b' objects.
    // This function provides the no-throw exception-safety guarantee.  The
    // behavior is undefined unless the two objects were created with the same
    // allocator.

// ============================================================================
//                        INLINE FUNCTION DEFINITIONS
// ============================================================================

                            // --------------------
                            // class DetailedStatus
                            // --------------------

// CREATORS
inline
DetailedStatus::DetailedStatus(bslma::Allocator *basicAllocator)
: d_description(basicAllocator)
, d_address(basicAllocator)
{
}

inline
DetailedStatus::DetailedStatus(const bslstl::StringRef&  description,
                               bslma::Allocator         *basicAllocator)
: d_description(description, basicAllocator)
, d_address(basicAllocator)
{
    BSLS_ASSERT_SAFE(0 != description.data());
}

inline
DetailedStatus::DetailedStatus(const bslstl::StringRef&  description,
                               const btlso::Endpoint&    address,
                               bslma::Allocator         *basicAllocator)
: d_description(description, basicAllocator)
, d_address(address, basicAllocator)
{
    BSLS_ASSERT_SAFE(0 != description.data());
}

inline
DetailedStatus::DetailedStatus(const DetailedStatus&  original,
                               bslma::Allocator      *basicAllocator)
: d_description(original.d_description, basicAllocator)
, d_address(original.d_address, basicAllocator)
{
}

// MANIPULATORS
inline
void DetailedStatus::setDescription(const bslstl::StringRef& value)
{
    BSLS_ASSERT_SAFE(0 != value.data());

    d_description.assign(value.begin(), value.end());
}

inline
void DetailedStatus::setAddress(const btlso::Endpoint& value)
{
    d_address = value;
}

                                  // Aspects

inline
void DetailedStatus::swap(DetailedStatus& other)
{
    BSLS_ASSERT_SAFE(allocator() == other.allocator());

    bslalg::SwapUtil::swap(&d_description, &other.d_description);
    bslalg::SwapUtil::swap(&d_address, &other.d_address);
}

// ACCESSORS
inline
const bsl::string& DetailedStatus::description() const
{
    return d_description;
}

inline
const btlso::Endpoint& DetailedStatus::address() const
{
    return d_address;
}

                                  // Aspects

inline
bslma::Allocator *DetailedStatus::allocator() const
{
    return d_description.get_allocator().mechanism();
}

}  // close package namespace

// FREE OPERATORS
inline
bool btls5::operator==(const DetailedStatus& lhs, const DetailedStatus& rhs)
{
    return lhs.description() == rhs.description()
            && lhs.address() == rhs.address();
}

inline
bool btls5::operator!=(const DetailedStatus& lhs, const DetailedStatus& rhs)
{
    return lhs.description() != rhs.description()
            || lhs.address() != rhs.address();
}

// FREE FUNCTIONS
inline
void btls5::swap(btls5::DetailedStatus& a, btls5::DetailedStatus& b)
{
    a.swap(b);
}

// TRAITS
namespace bslma {

template<>
struct UsesBslmaAllocator<btls5::DetailedStatus> : bsl::true_type {
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
