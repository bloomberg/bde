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
//@AUTHOR: Arthur Chiu (achiu21)
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
//  short              x = 0xabcd;
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

#ifndef INCLUDED_BSLS_PLATFORM
#include <bsls_platform.h>
#endif

#ifndef INCLUDED_BSLS_TYPES
#include <bsls_types.h>
#endif

#ifdef BSLS_PLATFORM__CMP_HP

#ifndef INCLUDED_MACHINE_SYS_BUILTINS
#include <machine/sys/builtins.h>  // '_Asm_xchg'
#define INCLUDED_MACHINE_SYS_BUILTINS
#endif

#ifndef INCLUDED_MACHINE_SYS_INLINE
#include <machine/sys/inline.h>    // '_SZ_H', '_SZ_W', '_SZ_D'
#define INCLUDED_MACHINE_SYS_INLINE
#endif

#endif

#ifdef BSLS_PLATFORM__CMP_MSVC

#ifndef INCLUDED_STDLIB
#include <stdlib.h>        // '_byteswap_*'
#define INCLUDED_STDLIB
#endif

#endif

namespace BloombergLP {

namespace bsls {

                     // ======================
                     // struct ByteOrder__Util
                     // ======================

struct ByteOrder__Util {
    // This 'struct' provides a namespace for a suite of platform-dependent
    // byte-swapping operations.  Do *not* use this directly; it is meant for
    // *internal* use only.  Use the macros provided for endian conversion
    // instead.

    // CLASS METHODS
    static unsigned short generic_swap_16(unsigned short x);
        // Return the byte-swapped value of the specified 16-bit value 'x'.

    static unsigned int generic_swap_32(unsigned int x);
        // Return the byte-swapped value of the specified 32-bit value 'x'.

    static Types::Uint64 generic_swap_64(Types::Uint64 x);
        // Return the byte-swapped value of the specified 64-bit value 'x'.
};

}  // close package namespace

// ======
// MACROS
// ======

// STANDARD NETWORK AND HOST CONVERSIONS

#if defined(BSLS_PLATFORM__IS_BIG_ENDIAN)

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

#else  // BSLS_PLATFORM__IS_LITTLE_ENDIAN

#define BSLS_BYTEORDER_NTOHS(x)                                               \
                         BloombergLP::bsls::ByteOrder__Util::generic_swap_16(x)

#define BSLS_BYTEORDER_NTOHL(x)                                               \
                         BloombergLP::bsls::ByteOrder__Util::generic_swap_32(x)

#define BSLS_BYTEORDER_NTOHLL(x)                                              \
                         BloombergLP::bsls::ByteOrder__Util::generic_swap_64(x)

#define BSLS_BYTEORDER_HTONS(x)  BSLS_BYTEORDER_NTOHS(x)

#define BSLS_BYTEORDER_HTONL(x)  BSLS_BYTEORDER_NTOHL(x)

#define BSLS_BYTEORDER_HTONLL(x) BSLS_BYTEORDER_NTOHLL(x)

#define BSLS_BYTEORDER_NTOHS_CONSTANT(x)                                      \
                                    ((((x) >> 8) & 0xFF) | (((x) & 0xFF) << 8))

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

#endif  // BSLS_PLATFORM__IS_BIG_ENDIAN

// ---------------------------------------------------------------------------

// ENDIAN CONVERSION MACROS

#if defined(BSLS_PLATFORM__IS_LITTLE_ENDIAN)

#define BSLS_BYTEORDER_LE_U16_TO_HOST(x) (x)
#define BSLS_BYTEORDER_LE_U32_TO_HOST(x) (x)
#define BSLS_BYTEORDER_LE_U64_TO_HOST(x) (x)

#define BSLS_BYTEORDER_HOST_U16_TO_LE(x) (x)
#define BSLS_BYTEORDER_HOST_U32_TO_LE(x) (x)
#define BSLS_BYTEORDER_HOST_U64_TO_LE(x) (x)

#define BSLS_BYTEORDER_BE_U16_TO_HOST(x)                                      \
                         BloombergLP::bsls::ByteOrder__Util::generic_swap_16(x)
#define BSLS_BYTEORDER_BE_U32_TO_HOST(x)                                      \
                         BloombergLP::bsls::ByteOrder__Util::generic_swap_32(x)
#define BSLS_BYTEORDER_BE_U64_TO_HOST(x)                                      \
                         BloombergLP::bsls::ByteOrder__Util::generic_swap_64(x)

#define BSLS_BYTEORDER_HOST_U16_TO_BE(x)                                      \
                         BloombergLP::bsls::ByteOrder__Util::generic_swap_16(x)
#define BSLS_BYTEORDER_HOST_U32_TO_BE(x)                                      \
                         BloombergLP::bsls::ByteOrder__Util::generic_swap_32(x)
#define BSLS_BYTEORDER_HOST_U64_TO_BE(x)                                      \
                         BloombergLP::bsls::ByteOrder__Util::generic_swap_64(x)

#else  // BSLS_PLATFORM__IS_BIG_ENDIAN

#define BSLS_BYTEORDER_LE_U16_TO_HOST(x)                                      \
                         BloombergLP::bsls::ByteOrder__Util::generic_swap_16(x)
#define BSLS_BYTEORDER_LE_U32_TO_HOST(x)                                      \
                         BloombergLP::bsls::ByteOrder__Util::generic_swap_32(x)
#define BSLS_BYTEORDER_LE_U64_TO_HOST(x)                                      \
                         BloombergLP::bsls::ByteOrder__Util::generic_swap_64(x)

#define BSLS_BYTEORDER_HOST_U16_TO_LE(x)                                      \
                         BloombergLP::bsls::ByteOrder__Util::generic_swap_16(x)
#define BSLS_BYTEORDER_HOST_U32_TO_LE(x)                                      \
                         BloombergLP::bsls::ByteOrder__Util::generic_swap_32(x)
#define BSLS_BYTEORDER_HOST_U64_TO_LE(x)                                      \
                         BloombergLP::bsls::ByteOrder__Util::generic_swap_64(x)

#define BSLS_BYTEORDER_BE_U16_TO_HOST(x) (x)
#define BSLS_BYTEORDER_BE_U32_TO_HOST(x) (x)
#define BSLS_BYTEORDER_BE_U64_TO_HOST(x) (x)

#define BSLS_BYTEORDER_HOST_U16_TO_BE(x) (x)
#define BSLS_BYTEORDER_HOST_U32_TO_BE(x) (x)
#define BSLS_BYTEORDER_HOST_U64_TO_BE(x) (x)

#endif  // BSLS_PLATFORM__IS_LITTLE_ENDIAN

// ===========================================================================
//                        INLINE FUNCTION DEFINITIONS
// ===========================================================================

// ---- Anything below this line is implementation specific.  Do not use.  ----

// ----------------------------------------------------------------------------
// LINUX
#if (defined(BSLS_PLATFORM__CPU_X86) || defined(BSLS_PLATFORM__CPU_X86_64))   \
  && defined(BSLS_PLATFORM__CMP_GNU)

namespace bsls {

inline
unsigned short
ByteOrder__Util_x86_swap_16(const unsigned short x)
{
    register unsigned short y;
    __asm__ ("xchg %b0, %h0" : "=Q" (y) : "0" (x));
    return y;
}

inline
unsigned int
ByteOrder__Util_x86_swap_32(const unsigned int x)
{
    register unsigned int y;
    __asm__ ("bswap %0" : "=r" (y) : "0" (x));
    return y;
}

}  // close package namespace

#if BSLS_PLATFORM__CPU_32_BIT

namespace bsls {

inline
Types::Uint64
ByteOrder__Util_x86_swap_64(const Types::Uint64 x)
{
    register unsigned int res, tmp;
    __asm__ ("bswap %0\n\t"
             "bswap %1\n\t"
           : "=r" (res), "=r" (tmp)
           : "0" ((unsigned) x), "1" ((unsigned) (x >> 32)));

    return ((Types::Uint64)res << 32ULL)
          | (Types::Uint64)tmp;
}

}  // close package namespace

#else  // BSLS_PLATFORM__CPU_64_BIT

namespace bsls {

inline
Types::Uint64
ByteOrder__Util_x86_swap_64(const Types::Uint64 x)
{
    register Types::Uint64 y;
    __asm__ ("bswap %0" : "=r" (y) : "0" (x));
    return y;
}

}  // close package namespace

#endif  // BSLS_PLATFORM__CPU_32_BIT
#endif  // BSLS_PLATFORM__CMP_GNU && X86

// ----------------------------------------------------------------------------
// HP
#if defined(BSLS_PLATFORM__CPU_IA64) && defined(BSLS_PLATFORM__CMP_HP)

namespace bsls {

inline
unsigned short
ByteOrder__Util_ia64_swap_16(const unsigned short x)
{
    register Types::Uint64 y;
    y = _Asm_shl(x, 48);
    return _Asm_mux1(_MBTYPE_REV, y);
}

inline
unsigned int
ByteOrder__Util_ia64_swap_32(const unsigned int x)
{
    register Types::Uint64 y;
    y = _Asm_shl(x, 32);
    return _Asm_mux1(_MBTYPE_REV, y);
}

inline
Types::Uint64
ByteOrder__Util_ia64_swap_64(const Types::Uint64 x)
{
    return _Asm_mux1(_MBTYPE_REV, x);
}

}  // close package namespace

#endif  // BSLS_PLATFORM__CMP_HP && IA64

// ----------------------------------------------------------------------------
// SUN

#if defined(BSLS_PLATFORM__CPU_SPARC)

// Note that inline assembly fails with '-O' (default '-O3') with Studio12, so
// disabling on Sun for now.
//..
//#if ((defined(BSLS_PLATFORM__CMP_SUN)
//   && BSLS_PLATFORM__CMP_VER_MAJOR >= 0x590
//   && defined(BDE_BUILD_TARGET_OPT))
// || defined(BSLS_PLATFORM__CMP_GNU))
//..

#if defined(BSLS_PLATFORM__CMP_GNU)

namespace bsls {

inline
unsigned short
ByteOrder__Util_sparc_swap_16(const unsigned short *x)
{
    register unsigned int y;
    asm("lduha [%1] %2, %0"
      : "=r" (y)
      : "r" (x), "i"(0x88), "m" (*x));
            // We have to use "r"(x) instead of "m"(*x) because certain
            // instructions do not support the 'm' constraint.  The 'm'
            // constraint is the only way to tell the compiler we are reading
            // the value of '*x' and not just 'x'.

    return static_cast<unsigned short>(y);
}

inline
unsigned int
ByteOrder__Util_sparc_swap_32(const unsigned int *x)
{
    register unsigned int y;
    asm("lduwa [%1] %2, %0"
      : "=r" (y)
      : "r" (x), "i"(0x88), "m" (*x));

    return y;
}

}  // close package namespace

#if defined(BSLS_PLATFORM__CPU_64_BIT)

inline
bsls::Types::Uint64
bsls_ByteOrder__Util_sparc_swap_64(const bsls::Types::Uint64 *x)
{
    register bsls::Types::Uint64 y;
    asm("ldxa [%1] %2, %0"
      : "=r" (y)
      : "r" (x), "i"(0x88), "m" (*x));

    return y;
}

#else

inline
bsls::Types::Uint64
bsls_ByteOrder__Util_sparc_swap_64(const bsls::Types::Uint64 *x)
{
    register bsls::Types::Uint64 y;
    asm("ldxa [%1] %2, %0\n\t"  // After the load, the full data is in '%0'.
                                // But we have to split it into two registers
                                // since we are running in 32-bit mode.

        "srl   %0, 0, %R0\n\t"  // The '%R0' specifies the lower-order bits of
                                // a pair register, while '%0' specifies the
                                // higher-order bits.  Move the lower-order
                                // bits of the result to '%R0'.

        "srlx  %0, 32, %0"      // Shift the higher-order bits of the result.
      : "=r" (y)
      : "r" (x), "i"(0x88), "m" (*x));

    return y;
}

#endif  // BSLS_PLATFORM__CPU_64_BIT

#else  // BSLS_PLATFORM__CMP_GNU

namespace bsls {

// Commented out assembly implementation since the generic implementation is
// probably faster than the function call for 16 and 32 bits.  Define the swap
// functions here to simplify the macro definitions at the 'generic_swap'
// level.

inline
unsigned short ByteOrder__Util_sparc_swap_16(const unsigned short *x)
{
    register const unsigned short y = *x;
    return (y >> 8) | (y << 8);
}

inline
unsigned int ByteOrder__Util_sparc_swap_32(const unsigned int *x)
{
    register const unsigned int y = *x;
    return ((y & 0x000000FF) << 24)
         | ((y & 0x0000FF00) <<  8)
         | ((y & 0x00FF0000) >>  8)
         |  (y >> 24);
}

}  // close package namespace

extern "C" {
// unsigned short bsls_ByteOrder__Util_sparc_swap_16(const unsigned short *x);
// unsigned int bsls_ByteOrder__Util_sparc_swap_32(const unsigned int *x);
unsigned long long bsls_ByteOrder__Util_sparc_swap_64(
                                                  const unsigned long long *x);
}

#endif  // BSLS_PLATFORM__CMP_GNU

#endif  // BSLS_PLATFORM__CPU_SPARC

// ----------------------------------------------------------------------------
// AIX
#if defined(BSLS_PLATFORM__CPU_POWERPC) && defined(BSLS_PLATFORM__CMP_IBM)    \
    && BSLS_PLATFORM__CMP_VER_MAJOR >= 0x0800

unsigned short bsls_ByteOrder__Util_powerpc_swap_16(const unsigned short *x);
// The following is equivalent to:
//..
//  lhbrx r3,0,r3
//..
// The following is necessary to work around the bug reported in DRQS 16073004
// using inline assembly with xlC10:
#pragma mc_func bsls_ByteOrder__Util_powerpc_swap_16 { "7c601e2c" }
#pragma reg_killed_by bsls_ByteOrder__Util_powerpc_swap_16 gr3

unsigned int bsls_ByteOrder__Util_powerpc_swap_32(const unsigned int *x);
// The following is equivalent to:
//..
//  lwbrx r3,0,r3
//..
// This follows the AIX ABI: the first argument is received in 'r3' and the
// return value is stored in 'r3'.  The hex value specified for 'mc_func' is
// the opcode of the above code.

#pragma mc_func bsls_ByteOrder__Util_powerpc_swap_32 { "7c601c2c" }
#pragma reg_killed_by bsls_ByteOrder__Util_powerpc_swap_32 gr3

unsigned long long bsls_ByteOrder__Util_powerpc_swap_64(
                                                  const unsigned long long *x);
#ifdef BSLS_PLATFORM__CPU_32_BIT
// The following is equivalent to:
//..
//  lwbrx r4,0,r3      // reverse the 4 higher-order bytes
//  addi r3,r3,4       // change r3 to point to the 4 lower-order bytes
//  lwbrx r3,0,r3      // reverse the 4 lower-order bytes
//..
#pragma mc_func bsls_ByteOrder__Util_powerpc_swap_64                   \
                                            { "7c801c2c" "38630004" "7c601c2c"}
#pragma reg_killed_by bsls_ByteOrder__Util_powerpc_swap_64 gr3,gr4

#else  // BSLS_PLATFORM__CPU_64_BIT
// The following is equivalent to:
//..
//  addi r4,r3,4       // move address of the 4 lower-order bytes to 'r4'
//  lwbrx r3,0,r3      // reverse the 4 higher-order bytes
//  lwbrx r4,0,r4      // reverse the 4 lower-order bytes
//  rldimi r3,r4,32,0  // rotate 'r4' left and insert to 'r3' with a mask
//..
#pragma mc_func bsls_ByteOrder__Util_powerpc_swap_64                   \
                                { "38830004" "7c601c2c" "7c80242c" "7883000e" }
#pragma reg_killed_by bsls_ByteOrder__Util_powerpc_swap_64 gr3,gr4,cr0

#endif  // BSLS_PLATFORM__CPU_32_BIT
#endif  // BSLS_PLATFORM__CPU_POWERPC

namespace bsls {

// ----------------------------------------------------------------------------

                         // ----------------------
                         // struct ByteOrder__Util
                         // ----------------------

// CLASS METHODS
inline
unsigned short
ByteOrder__Util::generic_swap_16(unsigned short x)
{
// Use built-in if possible; provided by Windows intrinsics.
#if defined(BSLS_PLATFORM__CMP_MSVC)
    return _byteswap_ushort(x);

#elif (defined(BSLS_PLATFORM__CPU_X86) || defined(BSLS_PLATFORM__CPU_X86_64)) \
    && defined(BSLS_PLATFORM__CMP_GNU)
    return ByteOrder__Util_x86_swap_16(x);

#elif defined(BSLS_PLATFORM__CPU_SPARC)
    return ByteOrder__Util_sparc_swap_16(&x);

#elif defined(BSLS_PLATFORM__CPU_POWERPC) && defined(BSLS_PLATFORM__CMP_IBM)  \
   && BSLS_PLATFORM__CMP_VER_MAJOR >= 0x0800
    return bsls_ByteOrder__Util_powerpc_swap_16(&x);

#elif defined(BSLS_PLATFORM__CPU_IA64) && defined(BSLS_PLATFORM__CMP_HP)
    return ByteOrder__Util_ia64_swap_16(x);

#else
    return (x >> 8) | (x << 8);

#endif
}

inline
unsigned int
ByteOrder__Util::generic_swap_32(unsigned int x)
{
// Use built-in if possible; provided in gcc 4.3+.
#if defined(BSLS_PLATFORM__CMP_GNU) && BSLS_PLATFORM__CMP_VER_MAJOR >= 40300
    return __builtin_bswap32((int)x);

// Use built-in if possible; provided by Windows intrinsics.
#elif defined(BSLS_PLATFORM__CMP_MSVC)
    return _byteswap_ulong(x);

#elif (defined(BSLS_PLATFORM__CPU_X86) || defined(BSLS_PLATFORM__CPU_X86_64)) \
   && defined(BSLS_PLATFORM__CMP_GNU)
    return ByteOrder__Util_x86_swap_32(x);

#elif defined(BSLS_PLATFORM__CPU_SPARC)
    return ByteOrder__Util_sparc_swap_32(&x);

#elif defined(BSLS_PLATFORM__CPU_POWERPC) && defined(BSLS_PLATFORM__CMP_IBM)  \
   && BSLS_PLATFORM__CMP_VER_MAJOR >= 0x0800
    return bsls_ByteOrder__Util_powerpc_swap_32(&x);

#elif defined(BSLS_PLATFORM__CPU_IA64) && defined(BSLS_PLATFORM__CMP_HP)
    return ByteOrder__Util_ia64_swap_32(x);

#else
    return ((x & 0x000000FF) << 24)
         | ((x & 0x0000FF00) <<  8)
         | ((x & 0x00FF0000) >>  8)
         |  (x >> 24);
#endif
}

inline
Types::Uint64
ByteOrder__Util::generic_swap_64(Types::Uint64 x)
{
// Use built-in if possible; provided in gcc 4.3+.
#if defined(BSLS_PLATFORM__CMP_GNU) && BSLS_PLATFORM__CMP_VER_MAJOR >= 40300
    return __builtin_bswap64((Types::Int64)x);

// Use built-in if possible; provided by Windows intrinsics.
#elif defined(BSLS_PLATFORM__CMP_MSVC)
    return _byteswap_uint64(x);

#elif (defined(BSLS_PLATFORM__CPU_X86) || defined(BSLS_PLATFORM__CPU_X86_64)) \
    && defined(BSLS_PLATFORM__CMP_GNU)
    return ByteOrder__Util_x86_swap_64(x);

#elif defined(BSLS_PLATFORM__CPU_SPARC)
    return bsls_ByteOrder__Util_sparc_swap_64(&x);

#elif defined(BSLS_PLATFORM__CPU_POWERPC) && defined(BSLS_PLATFORM__CMP_IBM)  \
   && BSLS_PLATFORM__CMP_VER_MAJOR >= 0x0800
    return bsls_ByteOrder__Util_powerpc_swap_64(&x);

#elif defined(BSLS_PLATFORM__CPU_IA64) && defined(BSLS_PLATFORM__CMP_HP)
    return ByteOrder__Util_ia64_swap_64(x);

#else
    return ((x & 0x00000000000000FFLL) << 56)
         | ((x & 0x000000000000FF00LL) << 40)
         | ((x & 0x0000000000FF0000LL) << 24)
         | ((x & 0x00000000FF000000LL) <<  8)
         | ((x & 0x000000FF00000000LL) >>  8)
         | ((x & 0x0000FF0000000000LL) >> 24)
         | ((x & 0x00FF000000000000LL) >> 40)
         |  (x >> 56);
#endif
}

}  // close package namespace

}  // close enterprise namespace

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2009
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
