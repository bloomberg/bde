// bdldfp_uint128.h                                                   -*-C++-*-
#ifndef INCLUDED_BDLDFP_UINT128
#define INCLUDED_BDLDFP_UINT128

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a representation of a 128-bit 'int' for bitwise operations.
//
//@CLASSES:
//  bdldfp::Uint128: A representation of a 128-bit unsigned integer
//
//@SEE_ALSO: bsl::bitset
//
//@DESCRIPTION: This component provides a value-semantic type,
// 'bdldfp::Uint128', that is used to represent a 128-bit unsigned integer
// having host-native byte order.  This component also provides a set of useful
// bitwise manipulation operators for this type.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Representing a 128 bit pattern for IPv6
/// - - - - - - - - - - - - - - - - - - - - - - - - -
// Starting in 1996, the world's TCP/IP infrastructure started the move to a
// 128-bit addressing scheme, IPv6.  IPv4 had a useful quality in that it could
// be represented by a single 32-bit machine word for internal routing
// purposes.  With IPv6, the need arises to manipulate 128-bit values
// representing IPv6 addresses for similar routing purposes.
//
// Suppose we need to write a function that needs to take 128-bit addresses
// indicating downstream routers in our network.
//
// First, we forward declare the "addRouter" function that takes an IPv6
// address indicating a router, and an IPv6 network address (a partial IP
// address, with trailing 0's) that the router covers:
//..
//  void addRouter(bdldfp::Uint128 router, bdldfp::Uint128 network);
//..
// Now we create a function that loads a set of networks and routers:
//..
//  void setupNetwork()
//  {
//      bdldfp::Uint128 network;
//      network.setHigh(0x4242000042420000L);
//      network.setLow(0x0);
//
//      bdldfp::Uint128 router(0x000012345678ABCDL, 0xDCBA000087654321L);
//..
// Finally we invoke addRouter, on our network and router.
//..
//      addRouter(router, network);
//  }
//..
// Notice that Uint128 values can be created from high/low pairs.
//
///Example 2: Checking a 128-bit IPv6 Network Mask
///- - - - - - - - - - - - - - - - - - - - - - - -
// In networks, checking a network mask is a fundamental operation.  A 128-bit
// mask can be used to indicate if an address is in a network, and where in the
// network an address is.
//
// Suppose we need to decide if an address is within a network, and extract the
// sub-address from the IPv6 address.
//
// First, we define a function that checks a passed IPv6 address and indicates
// the sub-address, and network membership:
//..
//  bool checkNetworkAddress(bdldfp::Uint128 *subAddress,
//                           bdldfp::Uint128  network,
//                           int              maskLength
//                           bdldfp::Uint128  address)
// {
//..
// Then, we compute a net mask for the specified 'maskLength':
//..
//      bdldfp::Uint128 netMask;
//      for (int i = 0; i < maskLength; ++i) {
//          netMask |= 1;
//          if (i != maskLength - 1) {
//              netMask <<= 1;
//          }
//      }
//..
// Notice that it is possible to shift 'Uint128' values as if they were a
// native type.  Meaning that it is possible to shift a 'Uint128' by 64-bits,
// or more, in single operation, because 'Uint128' functions just like a
// built-in 128-bit integer type would.
//
// Next, we calculate whether the passed address is within the network:
//..
//      bool inNetwork = network == (address & ~netMask);
//..
// Then, we compute the subAddress, if the address is in the network:
//..
//      if (inNetwork) {
//          *subAddress = address & netMask;
//      }
//..
// Now, we return whether the address is in the network, and close the
// function:
//..
//      return inNetwork
//  }
//..
// Finally, we call 'checkNetworkAddress' on a test network and address:
//..
//  bdldfp::Uint128 subAddress;
//  assert(checkNetworkAddress(
//              &subAddress,
//              bdldfp::Uint128(0xABCD424200001234L,0x22FF12345678L),
//              bdldfp::Uint128(0xABCD424200001234L,0x22FF00000000L),
//              32); // The network has a 32-bit internal address mask.
//  assert(subAddress == 0x12345678L);
//..
// Notice that primitive 64-bit words can be promoted to 128-bit addresses.

#ifndef INCLUDED_BSLS_TYPES
#include <bsls_types.h>
#endif

#ifndef INCLUDED_BSLS_PLATFORM
#include <bsls_platform.h>
#endif

namespace BloombergLP {
namespace bdldfp {

                             // =============
                             // class Uint128
                             // =============

class Uint128 {
    // This value-semantic type represents a 128-bit integer, with host machine
    // byte order.

  private:
    // DATA

    // The high and low values are laid out in an architecture dependent
    // fashion to facilitate a native-endian layout.  This class is expected to
    // have standard layout.

    #ifdef BSLS_PLATFORM_IS_BIG_ENDIAN
    bsls::Types::Uint64 d_high;
    bsls::Types::Uint64 d_low;
    #elif defined(BSLS_PLATFORM_IS_LITTLE_ENDIAN)
    bsls::Types::Uint64 d_low;
    bsls::Types::Uint64 d_high;
    #else
    #error Only big or little endian is supported.
    #endif

  public:
    // CREATORS
    Uint128();
        // Create an Uint128 object having the value '0'

    Uint128(bsls::Types::Uint64 initialValue);                      // IMPLICIT
        // Create an 'Uint128' object having the 128-bit integer bit pattern of
        // the value of the specified 'initialValue'.

    Uint128(bsls::Types::Uint64 initialHigh, bsls::Types::Uint64 initialLow);
        // Create an 'Uint128' object having the 128-bit pattern specified by
        // 'initialHigh..initialLow'

    //! ~Uint128() = default;
        // Destroy this object.

    // MANIPULATORS
    //! Uint128& operator=(const Uint128& rhs) = default;
        // Assign to this object the value of the specified 'rhs' object, and
        // return a reference providing modifiable access to this object.

    Uint128& operator|=(const Uint128& rhs);
        // Set the value of this object to the value of a the bitwise or
        // between this 128 bit integer and the specified 'rhs' value, and
        // return a reference providing mofifiable access to this object.

    Uint128& operator&=(const Uint128& rhs);
        // Set the value of this object to the value of a the bitwise and
        // between this 128 bit integer and the specified 'rhs' value, and
        // return a reference providing mofifiable access to this object.

    Uint128& operator^=(const Uint128& rhs);
        // Set the value of this object to the value of a the bitwise xor
        // between this 128 bit integer and the specified 'rhs' value, and
        // return a reference providing mofifiable access to this object.

    Uint128& operator>>=(int rhs);
        // Set the value of this object to the value of a bitwise right shift
        // of this 128 bit integer shifted by the specified 'rhs' value, and
        // return a reference providing mofifiable access to this object.  The
        // behavior is undefined unless '0 <= rhs < 128'.

    Uint128& operator<<=(int rhs);
        // Set the value of this object to the value of a bitwise left shift of
        // this 128 bit integer shifted by the specified 'rhs' value, and
        // return a reference providing mofifiable access to this object.  The
        // behavior is undefined unless '0 <= rhs < 128'.

    void setHigh(bsls::Types::Uint64 value);
        // Set the high order bits of this integer to the specified 'value'.

    void setLow(bsls::Types::Uint64 value);
        // Set the low order bits of this integer to the specified 'value'.

    // ACCESSORS
    bsls::Types::Uint64 high() const;
        // Return the high order bits of this integer.

    bsls::Types::Uint64 low() const;
        // Return the low order bits of this integer.
};

// FREE OPERATORS
bool operator==(const Uint128& lhs, const Uint128& rhs);
    // Return 'true' if the specified 'lhs' and the specified 'rhs' objects
    // have the same value, and 'false' otherwise.  Two 'Uint128' objects have
    // the same value if both of their 'low' and 'high' attributes are the
    // same.

bool operator!=(const Uint128& lhs, const Uint128& rhs);
    // Return 'true' if the specified 'lhs' and the specified 'rhs' objects do
    // not have the same value, and 'false' otherwise.  Two 'Uint128' objects
    // do not have the same value if either of their 'low' and 'high'
    // attributes are not the same.

Uint128 operator|(Uint128 lhs, const Uint128& rhs);
    // Return an Uint128 object having the value of a the bitwise or between
    // the specified 'lhs' and the specified 'rhs' value.

Uint128 operator&(Uint128 lhs, const Uint128& rhs);
    // Return an Uint128 object having the value of a the bitwise and between
    // the specified 'lhs' and the specified 'rhs' value.

Uint128 operator^(Uint128 lhs, const Uint128& rhs);
    // Return an Uint128 object having the value of a the bitwise xor between
    // the specified 'lhs' and the specified 'rhs' value.

Uint128 operator<<(Uint128 lhs, int rhs);
    // Return an 'Uint128' value equal to the value of a bitwise left shift of
    // the specified 'lhs' 128-bit integer shifted by the specified 'rhs'
    // value.  The behavior is undefined unless '0 <= rhs < 128'.

Uint128 operator>>(Uint128 lhs, int rhs);
    // Return an 'Uint128' value equal to the value of a bitwise right shift of
    // the specified 'lhs' 128-bit integer shifted by the specified 'rhs'
    // value.  The behavior is undefined unless '0 <= rhs < 128'.

Uint128 operator ~(Uint128 value);
    // Return an 'Uint128' value equal to the bitwise ones compliment of the
    // specified 'value'.

// ============================================================================
//                      INLINE FUNCTION DEFINITIONS
// ============================================================================

                        // -------------
                        // class Uint128
                        // -------------

// CREATORS
inline
Uint128::Uint128()
{
    d_low  = 0;
    d_high = 0;
}

inline
Uint128::Uint128(bsls::Types::Uint64 initialValue)
{
    d_high = 0;
    d_low  = initialValue;
}

inline
Uint128::Uint128(bsls::Types::Uint64 initialHigh,
                 bsls::Types::Uint64 initialLow)
{
    d_high = initialHigh;
    d_low  = initialLow;
}

// MANIPULATORS
inline
Uint128& Uint128::operator|=(const Uint128& rhs)
{
    d_high |= rhs.d_high;
    d_low  |= rhs.d_low;

    return *this;
}

inline
Uint128& Uint128::operator&=(const Uint128& rhs)
{
    d_high &= rhs.d_high;
    d_low  &= rhs.d_low;

    return *this;
}

inline
Uint128& Uint128::operator^=(const Uint128& rhs)
{
    d_high ^= rhs.d_high;
    d_low  ^= rhs.d_low;

    return *this;
}

inline
Uint128& Uint128::operator>>=(int rhs)
{
    if (rhs == 0) {
        return *this;                                                 // RETURN
    }

    if (rhs < 64) {
        d_low  >>= rhs;
        d_low  |= d_high << (64 - rhs);
        d_high >>= rhs;
    }
    else {
        d_low  = d_high >> (rhs - 64);
        d_high = 0;
    }

    return *this;
}

inline
Uint128& Uint128::operator<<=(int rhs)
{
    if (rhs == 0) {
        return *this;                                                 // RETURN
    }

    if (rhs < 64) {
        d_high <<= rhs;
        d_high |= d_low >> (64 - rhs);
        d_low  <<= rhs;
    }
    else {
        d_high = d_low << (rhs - 64);
        d_low  = 0;
    }

    return *this;
}

inline
void Uint128::setHigh(bsls::Types::Uint64 value)
{
    d_high = value;
}

inline
void Uint128::setLow(bsls::Types::Uint64 value)
{
    d_low = value;
}

// ACCESSORS
inline
bsls::Types::Uint64 Uint128::high() const
{
    return d_high;
}

inline
bsls::Types::Uint64 Uint128::low() const
{
    return d_low;
}


}  // close package namespace

// FREE OPERATORS
inline
bool bdldfp::operator==(const bdldfp::Uint128& lhs,
                        const bdldfp::Uint128& rhs)
{
    return lhs.high() == rhs.high() && lhs.low() == rhs.low();
}

inline
bool bdldfp::operator!=(const bdldfp::Uint128& lhs,
                        const bdldfp::Uint128& rhs)
{
    return lhs.high() != rhs.high() || lhs.low() != rhs.low();
}

inline
bdldfp::Uint128 bdldfp::operator|(      bdldfp::Uint128  lhs,
                                  const bdldfp::Uint128& rhs)
{
    return lhs |= rhs;
}

inline
bdldfp::Uint128 bdldfp::operator&(      bdldfp::Uint128  lhs,
                                  const bdldfp::Uint128& rhs)
{
    return lhs &= rhs;
}

inline
bdldfp::Uint128 bdldfp::operator^(      bdldfp::Uint128  lhs,
                                  const bdldfp::Uint128& rhs )
{
    return lhs ^= rhs;
}

inline
bdldfp::Uint128 bdldfp::operator<<(bdldfp::Uint128 lhs, int rhs)
{
    return lhs <<= rhs;
}

inline
bdldfp::Uint128 bdldfp::operator>>(bdldfp::Uint128 lhs, int rhs)
{
    return lhs >>= rhs;
}

inline
bdldfp::Uint128 bdldfp::operator~(bdldfp::Uint128 value)
{
    value.setHigh(~value.high());
    value.setLow( ~value.low());

    return value;
}

}  // close enterprise namespace
#endif

// ----------------------------------------------------------------------------
// Copyright 2014 Bloomberg Finance L.P.
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
