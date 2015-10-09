// btlso_ipv4address.h                                                -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#ifndef INCLUDED_BTLSO_IPV4ADDRESS
#define INCLUDED_BTLSO_IPV4ADDRESS

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a representation of an IPv4 address.
//
//@CLASSES:
// btlso::IPv4Address: IPv4 (Internet Protocol version 4) address
//
//@SEE_ALSO: btlso_endpoint
//
//@DESCRIPTION: An IPv4 address includes a logical IP address and a port
// number.  A logical IP address is represented using a 32-bit integer, which
// is commonly segmented into 4 8-bit fields called octets, each of which can
// be converted into a decimal number in the range [0, 255].  The resulting
// value can be expressed in dotted decimal notation (e.g., "220.218.5.28").  A
// port number has the range [0, 65535].
//
// A 'btlso::IPv4Address' object represents an IPv4 address containing both the
// logical IP address and the port number.  Note that in the public interface
// of the 'btlso::IPv4Address' class, an 'int' is used to specify a 32-bit
// logical IP *in* *network* *byte* *order*.  In this circumstance, the
// *numerical* *value* of the 'int' is irrelevant.  (Actually it will be
// different on Big-Endian and Little-Endian machines.)  This 'int' acts as a
// *place* *holder* for the four bytes in the 32-bit logical IP *in* *network*
// *byte* *order*, and therefore must be interpreted accordingly.
//
///Valid String Representations of IPv4 Addresses
///----------------------------------------------
// Many methods in this component accept or return IP addresses represented as
// strings.  Strings representing IP addresses are considered valid only if
// they are in one of the following four formats:
//..
//  "a.b.c.d"  where a, b, c, and d are each 8-bit ints
//  "a.b.c"    where a and b are both 8-bit ints and c is a 16-bit int
//  "a.b"      where a is an 8-bit int and b is a 24-bit int
//  "a"        where a is a 32-bit int
//..
// Additionally, an IP 'address' in string format represents 255.255.255.255 if
// it has one of the following four formats:
//..
//  "a.b.c.d"  where a, b, c, and d are each 8-bit ints representing -1 or 255
//  "a.b.c"    where a and b are both 8-bit ints representing -1 or
//             255 and c is a 16-bit int representing -1 or 65536.
//  "a.b"      where a is an 8-bit int representing -1 or 255 and b is
//             a 24-bit int representing -1 or 16777215.
//  "a"        where a is a 32-bit int representing -1 or 4294967295.
//..
// In both those cases a, b, c, and d can each be represented in decimal,
// octal, or (upper or lower case) hexadecimal format, e.g., 0xeA.0277.3.5.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Basic Syntax
///- - - - - - - - - - - -
// The following snippets of code illustrate how to create and use a
// 'btlso::IPv4Address' object.  First create a default object 'ip1'.
//..
//    btlso::IPv4Address ip1;
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
//    btlso::IPv4Address ip2(ip1);
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
// Confirm that 'ip2' still has the same IP address as 'ip1', as the 32-bit IP
// address 0x7f000001 has the dotted decimal notation "127.0.0.1".
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

#ifndef INCLUDED_BTLSCM_VERSION
#include <btlscm_version.h>
#endif

#ifndef INCLUDED_BSL_IOSFWD
#include <bsl_iosfwd.h>
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

#ifdef BSLS_PLATFORM_OS_WINDOWS
#include <Winsock2.h>
#else
#include <arpa/inet.h>
#endif

namespace BloombergLP {
namespace btlso {

                         // =================
                         // class IPv4Address
                         // =================

class IPv4Address {
    // Each instance of this class represents an IPv4 address.  A static method
    // 'isValidAddress' is provided to verify that an IP address in dotted
    // decimal notation is valid before it is used to create or modify an IPv4
    // address object.  More generally, this class supports a complete set of
    // *value* *semantic* operations, including copy construction, assignment,
    // equality comparison, 'ostream' printing, and BDEX serialization.  (A
    // precise operational definition of when two instances have the same value
    // can be found in the description of 'operator==' for the class.)  This
    // class is *exception* *neutral* with no guarantee of rollback: if an
    // exception is thrown during the invocation of a method on a pre-existing
    // instance, the object is left in a valid state, but its value is
    // undefined.  In no event is memory leaked.  Finally, *aliasing* (e.g.,
    // using all or part of an object as both source and destination) is
    // supported in all cases.

    int            d_address;
    unsigned short d_portNumber;    // Note that the port number is internally
                                    // represented as an 'unsigned' 'short'.
                                    // This ensures that during BDEX stream
                                    // operations, when the stream becomes
                                    // invalid, the port number is still valid,
                                    // i.e., it is in the range [0, 65535],
                                    // although its actual value is undefined.

    // PRIVATE CLASS METHODS
    static int machineIndependentInetPtonIPv4(int *addr, const char *address);
        // Convert the specified Internet host 'address' from the IPv4
        // numbers-and-dots notation into binary form (in network byte order)
        // and store it in the location the specified 'addr' points to.  Return
        // non-zero if the specified address is valid, and 0 if not.  This
        // function is intended as a wrapper for the similar functions used on
        // different platforms.  See {Valid String Representations of IPv4
        // Addresses} under {DESCRIPTION} for details of valid string
        // representations of IP addresses.

  public:
    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(IPv4Address, bsl::is_trivially_copyable)

    // TYPES
    enum {
        k_ANY_ADDRESS = 0, // Indicate that it is up to the service provider to
                           // assign an appropriate IP.  Note that a local
                           // platform usually defines 'INADDR_ANY' to have
                           // value 0 for the same implication.

        k_ANY_PORT    = 0  // Indicate that it is up to the service provider to
                           // assign an appropriate port.


    };

    // CLASS METHODS
    static bool isValidAddress(const char *address);
        // Return 'true' if the specified IP 'address' is valid, and 'false'
        // otherwise.  See {Valid String Representations of IPv4 Addresses}
        // under {DESCRIPTION} for details of valid string representations of
        // IP addresses.

    static int isLocalBroadcastAddress(const char *address);
        // Return 0 if the IP 'address' is invalid or does not represent
        // 255.255.255.255, and non-zero otherwise.  See {Valid String
        // Representations of IPv4 Addresses} under {DESCRIPTION} for details
        // of valid string representations of IP addresses.
        //
        // Windows XP currently does not support the inet_aton function as
        // specified by the contract above (inet_pton does not handle
        // hexadecimal or octal numerals.)  In some cases 255.255.255.255,
        // while being a valid address, is not parsed correctly by inet_addr
        // because -1 is used as an error code.  This function checks if the
        // specified 'address' is an IP representation of a address with an
        // integer value of -1.  This function is intended to detect all cases
        // in which a valid address of 255.255.255.255 is wrongfully detected
        // as an invalid address by inet_addr.

    // CREATORS
    IPv4Address();
        // Create a 'IPv4Address' object having an IP of 'k_ANY_ADDRESS' value
        // and a port number of 'k_ANY_PORT' value.

    IPv4Address(const char *address, int portNumber);
        // Create a 'IPv4Address' object having the value given by the
        // specified IP 'address' and 'portNumber'.  The value is undefined
        // unless 'portNumber' is in the range [0, 65535] and the IP 'address'
        // is valid.  See {Valid String Representations of IPv4 Addresses}
        // under {DESCRIPTION} for details of valid string representations of
        // IP addresses.

    IPv4Address(int address, int portNumber);
        // Create a 'IPv4Address' object having the value given by the
        // specified IP 'address' and 'portNumber'.  The value is undefined
        // unless 'address' specifies a 32-bit IP address in network byte order
        // as an 'int' on the local platform and 'portNumber' is in the range
        // [0, 65535].

    IPv4Address(const IPv4Address& original);
        // Create a 'IPv4Address' object having the value of the specified
        // 'original' object.

    ~IPv4Address();
        // Destroy this object.

    // MANIPULATORS
    IPv4Address& operator=(const IPv4Address& rhs);
        // Assign to this object the value of the specified 'rhs' object and
        // return a reference to this modifiable object.

    int setIpAddress(const char *address);
        // Set this object to have the specified IP 'address'.  Return 0 if the
        // IP 'address' is valid, and non-zero otherwise.  See {Valid String
        // Representations of IPv4 Addresses} under {DESCRIPTION} for details
        // of valid string representations of IP addresses.

    void setIpAddress(int address);
        // Set this object to have the specified IP 'address'.  The behavior is
        // undefined unless 'address' specifies a 32-bit IP address in network
        // byte order as an 'int' on the local platform.

    void setPortNumber(int portNumber);
        // Set this object to have the specified 'portNumber'.  The behavior is
        // undefined unless 'portNumber' is in the range [0, 65535].

    template <class STREAM>
    STREAM& bdexStreamIn(STREAM& stream, int version);
        // Assign to this object the value read from the specified input
        // 'stream' using the specified 'version' format, and return a
        // reference to 'stream'.  If 'stream' is initially invalid, this
        // operation has no effect.  If 'version' is not supported, this object
        // is unaltered and 'stream' is invalidated but otherwise unmodified.
        // If 'version' is supported but 'stream' becomes invalid during this
        // operation, this object has an undefined, but valid, state.  Note
        // that no version is read from 'stream'.  See the 'bslx' package-level
        // documentation for more information on BDEX streaming of
        // value-semantic types and containers.

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
        // colon and the port as a null-terminated string and return the number
        // of bytes used to store the result (including the null character).
        // The behavior is undefined unless 'result' refers to a valid array of
        // at least 22 characters.

    int ipAddress() const;
        // Return the 32-bit IP address of this object in network byte order as
        // an 'int' on the local platform.

    int portNumber() const;
        // Return the port number of this object.

    static int maxSupportedBdexVersion(int versionSelector);
        // Return the maximum valid BDEX format version, as indicated by the
        // specified 'versionSelector', to be passed to the 'bdexStreamOut'
        // method.  Note that the 'versionSelector' is expected to be formatted
        // as 'yyyymmdd', a date representation.  See the 'bslx' package-level
        // documentation for more information on BDEX streaming of
        // value-semantic types and containers.


    bsl::ostream& streamOut(bsl::ostream& stream) const;
        // Write the specified IPv4 'address' value to the specified 'output'
        // stream in the format of address:portNumber, e.g., "127.0.0.1:5".

    template <class STREAM>
    STREAM& bdexStreamOut(STREAM& stream, int version) const;
        // Assign to this object the value read from the specified input
        // 'stream' using the specified 'version' format, and return a
        // reference to 'stream'.  If 'stream' is initially invalid, this
        // operation has no effect.  If 'version' is not supported, this object
        // is unaltered and 'stream' is invalidated but otherwise unmodified.
        // If 'version' is supported but 'stream' becomes invalid during this
        // operation, this object has an undefined, but valid, state.  Note
        // that no version is read from 'stream'.  See the 'bslx' package-level
        // documentation for more information on BDEX streaming of
        // value-semantic types and containers.
};

// FREE OPERATORS
inline
bool operator==(const IPv4Address& lhs, const IPv4Address& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects have the same
    // value, and 'false' otherwise.  Two 'IPv4Address' objects have the same
    // value if and only if their respective fields for IP address and port
    // number, each have the same value.

inline
bool operator!=(const IPv4Address& lhs, const IPv4Address& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects do not have the
    // same value, and 'false' otherwise.  Two 'IPv4Address' objects do not
    // have the same value if their respective fields for IP address or port
    // number differ in values.

inline
bsl::ostream& operator<<(bsl::ostream& output, const IPv4Address& address);
    // Write the specified IPv4 'address' value to the specified 'output'
    // stream in the format of address:portNumber, e.g., "127.0.0.1:5".

// ============================================================================
//                        INLINE FUNCTION DEFINITIONS
// ============================================================================

                            // ------------------
                            // struct IPv4Address
                            // ------------------

// CLASS METHODS
inline
bool IPv4Address::isValidAddress(const char *address)
{
    BSLS_ASSERT_SAFE(address);

    int addr;
    return 0 != machineIndependentInetPtonIPv4(&addr, address);
}

// CREATORS
inline
IPv4Address::IPv4Address()
: d_address(k_ANY_ADDRESS)
, d_portNumber(k_ANY_PORT)
{
}

inline
IPv4Address::IPv4Address(int address, int portNumber)
: d_address(address)
, d_portNumber((unsigned short)portNumber)
{
}

inline
IPv4Address::IPv4Address(const IPv4Address& original)
: d_address(original.d_address)
, d_portNumber(original.d_portNumber)
{
}

inline
IPv4Address::~IPv4Address()
{
}

// MANIPULATORS
inline
IPv4Address& IPv4Address::operator=(const IPv4Address& rhs)
{
    d_address    = rhs.d_address;
    d_portNumber = rhs.d_portNumber;
    return *this;
}

inline
void IPv4Address::setIpAddress(int address)
{
    d_address = address;
}

inline
void IPv4Address::setPortNumber(int portNumber)
{
    d_portNumber = (unsigned short)portNumber;
}

template <class STREAM>
STREAM& IPv4Address::bdexStreamIn(STREAM& stream, int version)
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
int IPv4Address::ipAddress() const
{
    return d_address;
}

inline
int IPv4Address::portNumber() const
{
    return d_portNumber;
}

inline
int IPv4Address::maxSupportedBdexVersion(int /* versionSelector */)
{
    return 1;
}


template <class STREAM>
STREAM& IPv4Address::bdexStreamOut(STREAM& stream, int version) const
{
    switch (version) {
      case 1: {
        const unsigned char *ip =
                           reinterpret_cast<const unsigned char *>(&d_address);
        stream.putUint8(ip[0]);
        stream.putUint8(ip[1]);
        stream.putUint8(ip[2]);
        stream.putUint8(ip[3]);
        stream.putUint16(d_portNumber);
      } break;
      default: {
        stream.invalidate();
      }
    }
    return stream;
}
}  // close package namespace

// FREE OPERATORS
inline
bool btlso::operator==(const IPv4Address& lhs, const IPv4Address& rhs)
{
    return lhs.ipAddress()  == rhs.ipAddress()
        && lhs.portNumber() == rhs.portNumber();
}

inline
bool btlso::operator!=(const IPv4Address& lhs, const IPv4Address& rhs)
{
    return lhs.ipAddress()  != rhs.ipAddress()
        || lhs.portNumber() != rhs.portNumber();
}

inline
bsl::ostream& btlso::operator<<(bsl::ostream&      output,
                                const IPv4Address& address)
{
    return address.streamOut(output);
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
