// bteso_ipv4address.h          -*-C++-*-
#ifndef INCLUDED_BTESO_IPV4ADDRESS
#define INCLUDED_BTESO_IPV4ADDRESS

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide a representation of an IPv4 address.
//
//@CLASSES:
// bteso_IPv4Address: IPv4 (Internet Protocol version 4) address
//
//@SEE_ALSO:
//
//@AUTHOR: Xinyu Xiang (xxiang)
//
//@DESCRIPTION: An IPv4 address includes a logical IP address and a port
// number.  A logical IP address is represented using a 32-bit integer, which
// is commonly segmented into 4 8-bit fields called octets, each of which can
// be converted into a decimal number in the range [0, 255].  The resulting
// value can be expressed in dotted decimal notation (e.g., "220.218.5.28").
// A port number has the range [0, 65535].
//
// A 'bteso_IPv4Address' object represents an IPv4 address containing both the
// logical IP address and the port number.  Note that in the public interface
// of the 'bteso_IPv4Address' class, an 'int' is used to specify a 32-bit
// logical IP *in* *network* *byte* *order*.  In this circumstance, the
// *numerical* *value* of the 'int' is irrelevant.  (Actually it will be
// different on Big-Endian and Little-Endian machines.)  This 'int' acts as a
// *place* *holder* for the four bytes in the 32-bit logical IP *in* *network*
// *byte* *order*, and therefore must be interpreted accordingly.
//
///USAGE
///-----
// The following snippets of code illustrate how to create and use a
// 'bteso_IPv4Address' object.  First create a default object 'ip1'.
//..
//    bteso_IPv4Address ip1;
//..
// Next, set the value of 'ip1' to have IP address "127.0.0.1" and port number
// 8142.
//..
//    ip1.setIpAddress("127.0.0.1")
//    ip1.setPortNumber(8142);
//    char ip[16];
//    ip1.loadIpAddress(ip);          assert( 0 == strcmp("127.0.0.1", ip) );
//                                    assert( 8142 == ip1.portNumber() );
//..
// Then create 'ip2' as a copy of 'ip1'.
//..
//    bteso_IPv4Address ip2(ip1);
//    ip2.loadIpAddress(ip);          assert( 0 == strcmp("127.0.0.1", ip) );
//                                    assert( 8142 == ip2.portNumber() );
//..
// Now confirm that 'ip2' has the same value as that of 'ip1'.
//..
//    assert( ip2 == ip1 );
//..
// Alternately, set 'ip2' to have a 32-bit IP address in network byte order
// specified as an 'int' on the local platform.
//..
//    const int IP = htonl(0x7f000001UL);
//    ip2.setIpAddress(IP);           assert( IP == ip2.ipAddress() );
//..
// Confirm that 'ip2' still has the same IP address as 'ip1', as the 32-bit
// IP address 0x7f000001 has the dotted decimal notation "127.0.0.1".
//..
//    assert( ip2.ipAddress() == ip1.ipAddress() );
//..
// Finally, write the value of 'ip2' to 'stdout'.
//..
//    bsl::cout << ip2 << bsl::endl;
//..
// The output operator produces the following (single-line) format:
//..
//    127.0.0.1:8142
//..

#ifndef INCLUDED_BTESCM_VERSION
#include <btescm_version.h>
#endif

#ifndef INCLUDED_BSL_IOSFWD
#include <bsl_iosfwd.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITS
#include <bslalg_typetraits.h>
#endif


namespace BloombergLP {

                         // =======================
                         // class bteso_IPv4Address
                         // =======================

class bteso_IPv4Address {
    // Each instance of this class represents an IPv4 address.  A static method
    // 'isValid' is provided to verify that an IP address in dotted decimal
    // notation is valid before it is used to create or modify an IPv4 address
    // object.  More generally, this class supports a complete set of *value*
    // *semantic* operations, including copy construction, assignment, equality
    // comparison, 'ostream' printing, and 'bdex' serialization.  (A precise
    // operational definition of when two instances have the same value can be
    // found in the description of 'operator==' for the class.)  This class is
    // *exception* *neutral* with no guarantee of rollback: if an exception is
    // thrown during the invocation of a method on a pre-existing instance, the
    // object is left in a valid state, but its value is undefined.  In no
    // event is memory leaked.  Finally, *aliasing* (e.g., using all or part of
    // an object as both source and destination) is supported in all cases.

    int            d_address;
    unsigned short d_portNumber;    // Note that the port number is internally
                                    // represented as an 'unsigned' 'short'.
                                    // This ensures that during 'bdex' stream
                                    // operations, when the stream becomes
                                    // invalid, the port number is still valid,
                                    // i.e., it is in the range [0, 65535].
                                    // However, its value is undefined.

  public:
    // TRAITS
    BSLALG_DECLARE_NESTED_TRAITS(bteso_IPv4Address,
                                 bslalg_TypeTraitBitwiseCopyable);

    // TYPES
    enum {
        BTESO_ANY_ADDRESS = 0,      // Indicate that it is up to the service
                                    // provider to assign an appropriate IP.
                                    // Note that a local platform usually
                                    // defines 'INADDR_ANY' to have value 0
                                    // for the same implication.
        BTESO_ANY_PORT    = 0       // Indicate that it is up to the service
                                    // provider to assign an appropriate port.
#if !defined(BSL_LEGACY) || 1 == BSL_LEGACY
      , ANY_ADDRESS = BTESO_ANY_ADDRESS
      , ANY_PORT    = BTESO_ANY_PORT
#endif
    };

    // CLASS METHODS
    static int isValid(const char *address);
        // Return 0 if the IP 'address' is valid, and non-zero otherwise.  The
        // IP 'address' is not valid if it does not have one of the following
        // four formats:
        //..
        //   a.b.c.d,  where a, b, c, and d are each 8-bit ints;
        //   a.b.c,    where a and b are both 8-bit ints and c is a 16-bit int;
        //   a.b,      where a is an 8-bit int and b is a 24-bit int;
        //   a,        where a is a 32-bit int.
        //..
        // a, b, c, and d can each be represented in decimal, octal, or (upper
        // or lower case) hexadecimal format, e.g., 0xeA.0277.3.5.

    // CREATORS
    bteso_IPv4Address();
        // Create a 'bteso_IPv4Address' object having an IP of
        // 'BTESO_ANY_ADDRESS' value and a port number of 'BTESO_ANY_PORT'
        // value.

    bteso_IPv4Address(const char *address, int portNumber);
        // Create a 'bteso_IPv4Address' object having the value given by the
        // specified IP 'address' and 'portNumber'.  The value is undefined
        // unless 'portNumber' is in the range [0, 65535] and the IP 'address'
        // has one of the following four formats (See 'isValid):
        //..
        //   a.b.c.d,  where a, b, c, and d are each 8-bit ints;
        //   a.b.c,    where a and b are both 8-bit ints and c is a 16-bit int;
        //   a.b,      where a is an 8-bit int and b is a 24-bit int;
        //   a,        where a is a 32-bit int.
        //..
        // a, b, c, and d can each be represented in decimal, octal, or (upper
        // or lower case) hexadecimal format, e.g., 0xeA.0277.3.5.

    bteso_IPv4Address(int address, int portNumber);
        // Create a 'bteso_IPv4Address' object having the value given by the
        // specified IP 'address' and 'portNumber'.  The value is undefined
        // unless 'address' specifies a 32-bit IP address in network byte order
        // as an 'int' on the local platform and 'portNumber' is in the range
        // [0, 65535].

    bteso_IPv4Address(const bteso_IPv4Address& original);
        // Create a 'bteso_IPv4Address' object having the value of the
        // specified 'original' object.

    ~bteso_IPv4Address();
        // Destroy this object.

    // MANIPULATORS
    bteso_IPv4Address& operator=(const bteso_IPv4Address& rhs);
        // Assign to this object the value of the specified 'rhs' object and
        // return a reference to this modifiable object.

    int setIpAddress(const char *address);
        // Set this object to have the specified IP 'address'.  Return 0 if the
        // IP 'address' is valid, and non-zero otherwise.  The IP 'address' is
        // not valid if it does not have one of the following four formats
        // (See 'isValid'):
        //..
        //   a.b.c.d,  where a, b, c, and d are each 8-bit ints;
        //   a.b.c,    where a and b are both 8-bit ints and c is a 16-bit int;
        //   a.b,      where a is an 8-bit int and b is a 24-bit int;
        //   a,        where a is a 32-bit int.
        //..
        // a, b, c, and d can each be represented in decimal, octal, or (upper
        // or lower case) hexadecimal format, e.g., 0xeA.0277.3.5.

    void setIpAddress(int address);
        // Set this object to have the specified IP 'address'.  The behavior is
        // undefined unless 'address' specifies a 32-bit IP address in network
        // byte order as an 'int' on the local platform.

    void setPortNumber(int portNumber);
        // Set this object to have the specified 'portNumber'.  The behavior
        // is undefined unless 'portNumber' is in the range [0, 65535].

    template <class STREAM>
    STREAM& bdexStreamIn(STREAM& stream, int version);
        // Assign to this object the value read from the specified input
        // 'stream' using the specified 'version' format and return a reference
        // to the modifiable 'stream'.  If 'stream' is initially invalid, this
        // operation has no effect.  If 'stream' becomes invalid during this
        // operation, this object is valid, but its value is undefined.  If
        // 'version' is not supported, 'stream' is marked invalid and this
        // object is unaltered.  Note that no version is read from 'stream'.
        // See the 'bdex' package-level documentation for more information on
        // 'bdex' streaming of value-semantic types and containers.

    // ACCESSORS
    int loadIpAddress(char *result) const;
        // Load into the specified 'result' this object's logical IP address
        // represented in the dotted decimal notation as a null-terminated
        // string and return number of bytes used to store the result
        // (including the null character).  The behavior is undefined unless
        // 'result' refers to a valid array of at least 16 characters.

    int formatIpAddress(char *result) const;
        // Load into the specified 'result' this object's logical IP address
        // and port represented in the dotted decimal notation followed by a
        // colon and the port as a null-terminated string and return the
        // number of bytes used to store the result (including the null
        // character).  The behavior is undefined unless 'result' refers to a
        // valid array of at least 22 characters.

    int ipAddress() const;
        // Return the 32-bit IP address of this object in network byte
        // order as an 'int' on the local platform.

    int portNumber() const;
        // Return the port number of this object.

    static int maxSupportedBdexVersion();
        // Return the most current 'bdex' streaming version number supported by
        // this class.  (See the package-group-level documentation for more
        // information on 'bdex' streaming of container types.)

    static int maxSupportedVersion();
        // Return the most current 'bdex' streaming version number supported by
        // this class.  (See the package-group-level documentation for more
        // information on 'bdex' streaming of container types.)
        //
        // DEPRECATED: replaced by 'maxSupportedBdexVersion()'

    bsl::ostream& streamOut(bsl::ostream& stream) const;
        // Write the specified IPv4 'address' value to the specified 'output'
        // stream in the format of address:portNumber, e.g., "127.0.0.1:5".

    template <class STREAM>
    STREAM& bdexStreamOut(STREAM& stream, int version) const;
        // Write this value to the specified output 'stream' using the
        // specified 'version' format and return a reference to the
        // modifiable 'stream'.  If 'version' is not supported, 'stream' is
        // unmodified.  Note that 'version' is not written to 'stream'.
        // See the 'bdex' package-level documentation for more information
        // on 'bdex' streaming of value-semantic types and containers.
};

// FREE OPERATORS
inline
bool operator==(const bteso_IPv4Address& lhs, const bteso_IPv4Address& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects have the same
    // value, and 'false' otherwise.  Two 'bteso_IPv4Address' objects have the
    // same value if and only if their respective fields for IP address and
    // port number, each have the same value.

inline
bool operator!=(const bteso_IPv4Address& lhr, const bteso_IPv4Address& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects do not have the
    // same value, and 'false' otherwise.  Two 'bteso_IPv4Address' objects do
    // not have the same value if their respective fields for IP address or
    // port number differ in values.

inline
bsl::ostream& operator<<(bsl::ostream&            output,
                         const bteso_IPv4Address& address);
    // Write the specified IPv4 'address' value to the specified 'output'
    // stream in the format of address:portNumber, e.g., "127.0.0.1:5".

// ===========================================================================
//                        INLINE FUNCTION DEFINITIONS
// ===========================================================================

// CREATORS
inline
bteso_IPv4Address::bteso_IPv4Address()
: d_address(BTESO_ANY_ADDRESS)
, d_portNumber(BTESO_ANY_PORT)
{
}

inline
bteso_IPv4Address::bteso_IPv4Address(int address, int portNumber)
: d_address(address)
, d_portNumber((unsigned short)portNumber)
{
}

inline
bteso_IPv4Address::bteso_IPv4Address(const bteso_IPv4Address& original)
: d_address(original.d_address)
, d_portNumber(original.d_portNumber)
{
}

inline
bteso_IPv4Address::~bteso_IPv4Address()
{
}

// MANIPULATORS
inline
bteso_IPv4Address& bteso_IPv4Address::operator=(const bteso_IPv4Address& rhs)
{
    d_address    = rhs.d_address;
    d_portNumber = rhs.d_portNumber;
    return *this;
}

inline
void bteso_IPv4Address::setIpAddress(int address)
{
    d_address = address;
}

inline
void bteso_IPv4Address::setPortNumber(int portNumber)
{
    d_portNumber = (unsigned short)portNumber;
}

template <class STREAM>
STREAM& bteso_IPv4Address::bdexStreamIn(STREAM& stream, int version)
{
    if (stream) {
        switch (version) { // switch on the version
          case 1: {
            unsigned char *ip = (unsigned char *) &d_address;
            stream.getUint8(ip[0]);
            stream.getUint8(ip[1]);
            stream.getUint8(ip[2]);
            stream.getUint8(ip[3]);
            stream.getUint16(d_portNumber);
          } break;
          default: {
            stream.invalidate();
          }
        }
    }
    return stream;
}

// ACCESSORS
inline
int bteso_IPv4Address::ipAddress() const
{
    return d_address;
}

inline
int bteso_IPv4Address::portNumber() const
{
    return d_portNumber;
}

inline
int bteso_IPv4Address::maxSupportedBdexVersion()
{
    return 1;
}

inline
int bteso_IPv4Address::maxSupportedVersion()
{
    return maxSupportedBdexVersion();
}

template <class STREAM>
STREAM& bteso_IPv4Address::bdexStreamOut(STREAM& stream, int version) const
{
    switch (version) {
      case 1: {
        const unsigned char *ip = (const unsigned char *) &d_address;
        stream.putUint8(ip[0]);
        stream.putUint8(ip[1]);
        stream.putUint8(ip[2]);
        stream.putUint8(ip[3]);
        stream.putUint16(d_portNumber);
      } break;
    }
    return stream;
}

// FREE OPERATORS
inline
bool operator==(const bteso_IPv4Address& lhs,
                const bteso_IPv4Address& rhs)
{
    return lhs.ipAddress()  == rhs.ipAddress()
        && lhs.portNumber() == rhs.portNumber();
}

inline
bool operator!=(const bteso_IPv4Address& lhs,
                const bteso_IPv4Address& rhs)
{
    return lhs.ipAddress()  != rhs.ipAddress()
        || lhs.portNumber() != rhs.portNumber();
}

inline
bsl::ostream& operator<<(bsl::ostream&            output,
                         const bteso_IPv4Address& address)
{
    return address.streamOut(output);
}

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2002
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
