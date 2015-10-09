// bsls_byteorder.h                                                   -*-C++-*-
#ifndef INCLUDED_BSLS_BYTEORDER
#define INCLUDED_BSLS_BYTEORDER

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide byte-order manipulation macros.
//
//@CLASSES:
//
//@SEE_ALSO: bsls_byteorderutil
//
//@MACROS:
//  BSLS_BYTEORDER_HTONS(x):  Convert 16-bit value from host to network order
//  BSLS_BYTEORDER_HTONL(x):  Convert 32-bit value from host to network order
//  BSLS_BYTEORDER_HTONLL(x): Convert 64-bit value from host to network order
//  BSLS_BYTEORDER_NTOHS(x):  Convert 16-bit value from network to host order
//  BSLS_BYTEORDER_NTOHL(x):  Convert 32-bit value from network to host order
//  BSLS_BYTEORDER_NTOHLL(x): Convert 64-bit value from network to host order
//
//  BSLS_BYTEORDER_HTONS_CONSTANT(x):  static 16-bit network to host order
//  BSLS_BYTEORDER_HTONL_CONSTANT(x):  static 32-bit network to host order
//  BSLS_BYTEORDER_HTONLL_CONSTANT(x): static 64-bit network to host order
//  BSLS_BYTEORDER_NTOHS_CONSTANT(x):  static 16-bit network to host order
//  BSLS_BYTEORDER_NTOHL_CONSTANT(x):  static 32-bit network to host order
//  BSLS_BYTEORDER_NTOHLL_CONSTANT(x): static 64-bit network to host order
//
//  BSLS_BYTEORDER_LE_U16_TO_HOST(x): 16-bit little-endian to host-endian
//  BSLS_BYTEORDER_LE_U32_TO_HOST(x): 32-bit little-endian to host-endian
//  BSLS_BYTEORDER_LE_U64_TO_HOST(x): 64-bit little-endian to host-endian
//  BSLS_BYTEORDER_BE_U16_TO_HOST(x): 16-bit    big-endian to host-endian
//  BSLS_BYTEORDER_BE_U32_TO_HOST(x): 32-bit    big-endian to host-endian
//  BSLS_BYTEORDER_BE_U64_TO_HOST(x): 64-bit    big-endian to host-endian
//
//  BSLS_BYTEORDER_HOST_U16_TO_LE(x): 16-bit host-endian to little-endian
//  BSLS_BYTEORDER_HOST_U32_TO_LE(x): 32-bit host-endian to little-endian
//  BSLS_BYTEORDER_HOST_U64_TO_LE(x): 64-bit host-endian to little-endian
//  BSLS_BYTEORDER_HOST_U16_TO_BE(x): 16-bit host-endian to    big-endian
//  BSLS_BYTEORDER_HOST_U32_TO_BE(x): 32-bit host-endian to    big-endian
//  BSLS_BYTEORDER_HOST_U64_TO_BE(x): 64-bit host-endian to    big-endian
//
//@DESCRIPTION: This component provides a set of byte-order manipulation macros
// that replace the standard 'htonl', 'htons', 'ntohl', and 'ntohs' functions,
// and which do not require including any system header files:
//..
//  BSLS_BYTEORDER_HTONS(x)
//  BSLS_BYTEORDER_HTONL(x)
//  BSLS_BYTEORDER_HTONLL(x)
//  BSLS_BYTEORDER_NTOHS(x)
//  BSLS_BYTEORDER_NTOHL(x)
//  BSLS_BYTEORDER_NTOHLL(x)
//..
// The "S", "L", and "LL" suffices in the names of the above macros indicate
// their applicability to 16-bit ('short'), 32-bit ('int', *not* 'long'), and
// 64-bit ('long long') values, respectively.
//
// This set of host-to-network and network-to-host conversion macros are very
// efficient, but sacrifices the ability to perform compile-time
// initialization.  To compensate, another set of functionally equivalent
// "CONSTANT" macros are provided.  These macros can be used for compile-time
// initialization, but are less efficient than non-"CONSTANT" versions:
//..
//  BSLS_BYTEORDER_HTONS_CONSTANT(x)
//  BSLS_BYTEORDER_HTONL_CONSTANT(x)
//  BSLS_BYTEORDER_HTONLL_CONSTANT(x)
//  BSLS_BYTEORDER_NTOHS_CONSTANT(x)
//  BSLS_BYTEORDER_NTOHL_CONSTANT(x)
//  BSLS_BYTEORDER_NTOHLL_CONSTANT(x)
//..
// Another set of macros provides conversion from big-endian or little-endian
// byte order to host-endian order.  The macros take 16-, 32- or 64-bit values
// and perform the indicated byte-order conversion on those values:
//..
//  BSLS_BYTEORDER_LE_U16_TO_HOST(x)
//  BSLS_BYTEORDER_LE_U32_TO_HOST(x)
//  BSLS_BYTEORDER_LE_U64_TO_HOST(x)
//  BSLS_BYTEORDER_BE_U16_TO_HOST(x)
//  BSLS_BYTEORDER_BE_U32_TO_HOST(x)
//  BSLS_BYTEORDER_BE_U64_TO_HOST(x)
//..
// The "LE" and "BE" embedded in the above macro names indicate Little-Endian
// and Big-Endian, respectively.
//
// Finally, a complementary set of macros provides conversion from host-endian
// byte order to big-endian or little-endian order:
//..
//  BSLS_BYTEORDER_HOST_U16_TO_LE(x)
//  BSLS_BYTEORDER_HOST_U32_TO_LE(x)
//  BSLS_BYTEORDER_HOST_U64_TO_LE(x)
//  BSLS_BYTEORDER_HOST_U16_TO_BE(x)
//  BSLS_BYTEORDER_HOST_U32_TO_BE(x)
//  BSLS_BYTEORDER_HOST_U64_TO_BE(x)
//..
//
///Usage
///-----
// To use these macros, simply pass a 16-, 32-, or 64-bit value to the macros.
// To demonstrate the change in byte order effected by the macros, we first
// write a function to print, in hex, a character buffer of a specified size:
//..
//  void printHex(const char *c, int size)
//      // Print the specified character array 'c', having the specified 'size'
//      // (in bytes), to 'stdout' in hex.
//  {
//      const char *hex = "0123456789abcdef";
//      for (int i = 0; i < size; ++i) {
//          std::cout << hex[(c[i] >> 4) & 0xf]
//                    << hex[ c[i]       & 0xf];
//      }
//  }
//
//  template <class T>
//  void printHex(T x)
//      // Print the specified object 'x' of parameterized type 'T' in hex.
//  {
//      printHex((const char*)&x, sizeof x);
//  }
//..
// For example, to use the little-endian/big-endian to host-endian macros:
//..
//  short              x = static_cast<short>(0xabcd);
//  int                y = 0xabcdef12;
//  bsls::Types::Int64 z = 0xabcdef1234567890LL;
//
//  // Note the use of macros within the calls to 'printHex'.
//
//  printf("\nLE to Host(x): ");
//  printHex(BSLS_BYTEORDER_LE_U16_TO_HOST(x));
//
//  printf("\nLE to Host(y): ");
//  printHex(BSLS_BYTEORDER_LE_U32_TO_HOST(y));
//
//  printf("\nLE to Host(z): ");
//  printHex(BSLS_BYTEORDER_LE_U64_TO_HOST(z));
//
//  printf("\nBE to Host(x): ");
//  printHex(BSLS_BYTEORDER_BE_U16_TO_HOST(x));
//
//  printf("\nBE to Host(y): ");
//  printHex(BSLS_BYTEORDER_BE_U32_TO_HOST(y));
//
//  printf("\nBE to Host(z): ");
//  printHex(BSLS_BYTEORDER_BE_U64_TO_HOST(z));
//..
// On little-endian machines (e.g., x86, IA64), this will print the following
// to 'stdout':
//..
//  LE to Host(x): abcd
//  LE to Host(y): abcdef12
//  LE to Host(z): abcdef1234567890
//  BE to Host(x): cdab
//  BE to Host(y): 12efcdab
//  BE to Host(z): 9078563412efcdab
//..
// On big-endian machines (e.g., sparc, powerpc), the following will be printed
// instead:
//..
//  LE to Host(x): cdab
//  LE to Host(y): 12efcdab
//  LE to Host(z): 9078563412efcdab
//  BE to Host(x): abcd
//  BE to Host(y): abcdef12
//  BE to Host(z): abcdef1234567890
//..
// The other macros can be used in a similar manner.

#ifndef INCLUDED_BSLS_BYTEORDERUTIL
#include <bsls_byteorderutil.h>
#endif

#ifndef INCLUDED_BSLS_PLATFORM
#include <bsls_platform.h>
#endif

// ============================================================================
//                                  MACROS
// ======

// STANDARD NETWORK AND HOST CONVERSIONS

#if defined(BSLS_PLATFORM_IS_BIG_ENDIAN)

#define BSLS_BYTEORDER_NTOHS(x)  (x)
#define BSLS_BYTEORDER_NTOHL(x)  (x)
#define BSLS_BYTEORDER_NTOHLL(x) (x)

#define BSLS_BYTEORDER_HTONS(x)  (x)
#define BSLS_BYTEORDER_HTONL(x)  (x)
#define BSLS_BYTEORDER_HTONLL(x) (x)

#define BSLS_BYTEORDER_NTOHS_CONSTANT(x)  (x)
#define BSLS_BYTEORDER_NTOHL_CONSTANT(x)  (x)
#define BSLS_BYTEORDER_NTOHLL_CONSTANT(x) (x)

#define BSLS_BYTEORDER_HTONS_CONSTANT(x)  (x)
#define BSLS_BYTEORDER_HTONL_CONSTANT(x)  (x)
#define BSLS_BYTEORDER_HTONLL_CONSTANT(x) (x)

#else  // BSLS_PLATFORM_IS_LITTLE_ENDIAN

#define BSLS_BYTEORDER_NTOHS(x)                                               \
                               BloombergLP::bsls::ByteOrderUtil::swapBytes16(x)

#define BSLS_BYTEORDER_NTOHL(x)                                               \
                               BloombergLP::bsls::ByteOrderUtil::swapBytes32(x)

#define BSLS_BYTEORDER_NTOHLL(x)                                              \
                               BloombergLP::bsls::ByteOrderUtil::swapBytes64(x)

#define BSLS_BYTEORDER_HTONS(x)  BSLS_BYTEORDER_NTOHS(x)

#define BSLS_BYTEORDER_HTONL(x)  BSLS_BYTEORDER_NTOHL(x)

#define BSLS_BYTEORDER_HTONLL(x) BSLS_BYTEORDER_NTOHLL(x)

#define BSLS_BYTEORDER_NTOHS_CONSTANT(x)                                      \
             static_cast<unsigned short>(                                     \
                            (static_cast<unsigned short>(x) >> 8) | ((x) << 8))

#define BSLS_BYTEORDER_NTOHL_CONSTANT(x)                                      \
                    ((((x) >> 24) & 0x000000FF) | (((x) & 0x00FF0000) >>  8)  \
                   | (((x) & 0x0000FF00) <<  8) | (((x) & 0x000000FF) << 24))

#define BSLS_BYTEORDER_NTOHLL_CONSTANT(x)                                     \
                                        ((((x) & 0x00000000000000FFLL) << 56) \
                                       | (((x) & 0x000000000000FF00LL) << 40) \
                                       | (((x) & 0x0000000000FF0000LL) << 24) \
                                       | (((x) & 0x00000000FF000000LL) <<  8) \
                                       | (((x) & 0x000000FF00000000LL) >>  8) \
                                       | (((x) & 0x0000FF0000000000LL) >> 24) \
                                       | (((x) & 0x00FF000000000000LL) >> 40) \
                                       | (((x) >> 56) & 0x00000000000000FFLL))

#define BSLS_BYTEORDER_HTONS_CONSTANT(x)  BSLS_BYTEORDER_NTOHS_CONSTANT(x)

#define BSLS_BYTEORDER_HTONL_CONSTANT(x)  BSLS_BYTEORDER_NTOHL_CONSTANT(x)

#define BSLS_BYTEORDER_HTONLL_CONSTANT(x) BSLS_BYTEORDER_NTOHLL_CONSTANT(x)

#endif  // BSLS_PLATFORM_IS_BIG_ENDIAN

// ----------------------------------------------------------------------------

// ENDIAN CONVERSION MACROS

#if defined(BSLS_PLATFORM_IS_LITTLE_ENDIAN)

#define BSLS_BYTEORDER_LE_U16_TO_HOST(x) (x)
#define BSLS_BYTEORDER_LE_U32_TO_HOST(x) (x)
#define BSLS_BYTEORDER_LE_U64_TO_HOST(x) (x)

#define BSLS_BYTEORDER_HOST_U16_TO_LE(x) (x)
#define BSLS_BYTEORDER_HOST_U32_TO_LE(x) (x)
#define BSLS_BYTEORDER_HOST_U64_TO_LE(x) (x)

#define BSLS_BYTEORDER_BE_U16_TO_HOST(x)                                      \
                               BloombergLP::bsls::ByteOrderUtil::swapBytes16(x)
#define BSLS_BYTEORDER_BE_U32_TO_HOST(x)                                      \
                               BloombergLP::bsls::ByteOrderUtil::swapBytes32(x)
#define BSLS_BYTEORDER_BE_U64_TO_HOST(x)                                      \
                               BloombergLP::bsls::ByteOrderUtil::swapBytes64(x)

#define BSLS_BYTEORDER_HOST_U16_TO_BE(x)                                      \
                               BloombergLP::bsls::ByteOrderUtil::swapBytes16(x)
#define BSLS_BYTEORDER_HOST_U32_TO_BE(x)                                      \
                               BloombergLP::bsls::ByteOrderUtil::swapBytes32(x)
#define BSLS_BYTEORDER_HOST_U64_TO_BE(x)                                      \
                               BloombergLP::bsls::ByteOrderUtil::swapBytes64(x)

#else  // BSLS_PLATFORM_IS_BIG_ENDIAN

#define BSLS_BYTEORDER_LE_U16_TO_HOST(x)                                      \
                               BloombergLP::bsls::ByteOrderUtil::swapBytes16(x)
#define BSLS_BYTEORDER_LE_U32_TO_HOST(x)                                      \
                               BloombergLP::bsls::ByteOrderUtil::swapBytes32(x)
#define BSLS_BYTEORDER_LE_U64_TO_HOST(x)                                      \
                               BloombergLP::bsls::ByteOrderUtil::swapBytes64(x)

#define BSLS_BYTEORDER_HOST_U16_TO_LE(x)                                      \
                               BloombergLP::bsls::ByteOrderUtil::swapBytes16(x)
#define BSLS_BYTEORDER_HOST_U32_TO_LE(x)                                      \
                               BloombergLP::bsls::ByteOrderUtil::swapBytes32(x)
#define BSLS_BYTEORDER_HOST_U64_TO_LE(x)                                      \
                               BloombergLP::bsls::ByteOrderUtil::swapBytes64(x)

#define BSLS_BYTEORDER_BE_U16_TO_HOST(x) (x)
#define BSLS_BYTEORDER_BE_U32_TO_HOST(x) (x)
#define BSLS_BYTEORDER_BE_U64_TO_HOST(x) (x)

#define BSLS_BYTEORDER_HOST_U16_TO_BE(x) (x)
#define BSLS_BYTEORDER_HOST_U32_TO_BE(x) (x)
#define BSLS_BYTEORDER_HOST_U64_TO_BE(x) (x)

#endif  // BSLS_PLATFORM_IS_LITTLE_ENDIAN

#endif

// ----------------------------------------------------------------------------
// Copyright 2013 Bloomberg Finance L.P.
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
