// btemt_internetaddress.h               -*-C++-*-
#ifndef INCLUDED_BTEMT_INTERNETADDRESS
#define INCLUDED_BTEMT_INTERNETADDRESS

//@PURPOSE: Provide a value-semantic type for/to..
//
//@CLASSES:
//   InternetAddress: <<description>>
//
//@AUTHOR: Kevin McMahon (kmcmahon)
//
//@SEE ALSO:
//
//@DESCRIPTION: This component defines a value-semantic type,
// 'InternetAddress', which ...
// It contains the attributes ...
//
///Usage Example
///-------------
// Text
//..
//  Example code here
//..

#ifndef INCLUDED_BTESO_IPV4ADDRESS
#include <bteso_ipv4address.h>
#endif

#ifndef INCLUDED_BDEUT_VARIANT
#include <bdeut_variant.h>
#endif

#ifndef INCLUDED_BDEU_PRINTMETHODS
#include <bdeu_printmethods.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITS
#include <bslalg_typetraits.h>
#endif

#ifndef INCLUDED_BSL_IOSFWD
#include <bsl_iosfwd.h>
#endif

#ifndef INCLUDED_BSL_STRING
#include <bsl_string.h>
#endif

namespace BloombergLP {

class bdeut_StringRef;
namespace bslma {class Allocator;}

namespace btemt {

class InternetAddress_StringPort {
public:
    // DATA
    bsl::string    d_host;
    unsigned short d_port;

    // TRAITS
    BSLALG_DECLARE_NESTED_TRAITS(InternetAddress_StringPort,
                                 bslalg::TypeTraitUsesBslmaAllocator);

    // CREATORS
    InternetAddress_StringPort(const bdeut_StringRef&  host,
                               unsigned short          port,
                               bslma::Allocator        *allocator = 0);

    InternetAddress_StringPort(
                             const InternetAddress_StringPort&  rhs,
                             bslma::Allocator                   *allocator = 0);
};

                        // =====================
                        // class InternetAddress
                        // =====================

class InternetAddress {
    // This class defines a value-semantic type that holds an IP and port either
    // as a numeric IP or as a DNS name.

  public:
    // PUBLIC TYPES
    typedef bdeut_Variant<InternetAddress_StringPort, bteso_IPv4Address>
                                                                       Address;

  private:
    // PRIVATE TYPES

    // DATA
    Address d_address;

    // PRIVATE MANIPULATORS

    // PRIVATE ACCESSORS

  public:
    // TRAITS
    BSLALG_DECLARE_NESTED_TRAITS2(InternetAddress,
                                  bdeu_TypeTraitHasPrintMethod,
                                  bslalg::TypeTraitUsesBslmaAllocator);

  public:
    // CREATORS
    InternetAddress(const bdeut_StringRef&  host,
                    unsigned short          port,
                    bslma::Allocator        *allocator = 0);
        // Create a 'InternetAddress' object having ...
        // Use the optionally specified 'allocator' to supply memory.

    InternetAddress(const bteso_IPv4Address&  address,
                    bslma::Allocator          *allocator = 0);
        // Create a 'InternetAddress' object having ...
        // Use the optionally specified 'allocator' to supply memory.

    explicit InternetAddress(bslma::Allocator *allocator = 0);

    InternetAddress(const InternetAddress&  other,
                    bslma::Allocator        *allocator = 0);
        // Create a 'InternetAddress' object having the same value as
        // the specified 'other' object.  Use the optionally specified
        // 'allocator' to supply memory.

    ~InternetAddress();
        // Destroy this object.

    // MANIPULATORS
    InternetAddress& operator=(const InternetAddress& rhs);
        // Assign to this object the value of the specified 'rhs' object, and
        // return a reference to this modifiable object.

    // ACCESSORS
    bool isDomainname() const;
    bool isIPv4() const;
    bool isUnset() const;
    
    const bsl::string& domainnameHost() const;
        // Behavior is undefined if 'isDomainname' is false.

    unsigned short port() const;
        // Returns 0 if 'isUnset' is true.

    const bteso_IPv4Address& ipv4() const;
        // Behavior is undefined if 'isIPv4' is false.

    bsl::ostream& print(bsl::ostream& stream,
                        int           level = 0,
                        int           spacesPerLevel = 4) const;
        // Format this object to the specified output 'stream' at the
        // optionally specified indentation 'level' and return a reference to
        // the modifiable 'stream'.  If 'level' is specified, optionally
        // specify 'spacesPerLevel', the number of spaces per indentation level
        // for this and all of its nested objects.  Each line is indented by
        // the absolute value of 'level * spacesPerLevel'.  If 'level' is
        // negative, suppress indentation of the first line.  If
        // 'spacesPerLevel' is negative, suppress line breaks and format the
        // entire output on one line.  If 'stream' is initially invalid, this
        // operation has no effect.  Note that a trailing newline is provided
        // in multiline mode only.
};

// FREE OPERATORS
bool operator==(const InternetAddress& lhs, const InternetAddress& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects have the same
    // value, and 'false' otherwise.  Two 'InternetAddress' objects
    // have the same if ...

bool operator!=(const InternetAddress& lhs, const InternetAddress& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects do not have the
    // same value, and 'false' otherwise. Two 'InternetAddress' objects
    // do not have the same value if ...

bsl::ostream& operator<<(bsl::ostream& stream, const InternetAddress& rhs);
    // Write the specified 'rhs' value to the specified 'stream' in a
    // single-line format (without a trailing newline) and return a reference
    // to the modifiable 'stream'.

struct InternetAddressUtil {
    static bsl::string getHostName(const InternetAddress& addr);

    static int loadAddressPort(InternetAddress         *addr,
                               const bdeut_StringRef&  hostPort);
        // Load a 'InternetAddress' object using a string of format
        // "host:port".

    static int resolve(bteso_IPv4Address      *result,
                       const InternetAddress&  address);
        // returns negative value on error.
};

// ===========================================================================
//                  INLINE AND TEMPLATE FUNCTION IMPLEMENTATIONS
// ===========================================================================

                        // ---------------------
                        // class InternetAddress
                        // ---------------------

// CREATORS

// MANIPULATORS

// ACCESSORS

}  // close package namespace

// FREE OPERATORS
inline
bsl::ostream& btemt::operator<<(bsl::ostream&                stream,
                               const btemt::InternetAddress& rhs)
{
    return rhs.print(stream, 0, -1);
}

}  // close enterprise namespace

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2011
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
