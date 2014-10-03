// bsls_byteorderutil_impl.h                                          -*-C++-*-
#ifndef INCLUDED_BSLS_BYTEORDERUTIL_IMPL
#define INCLUDED_BSLS_BYTEORDERUTIL_IMPL

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide implementation of byte-order manipulation functions.
//
//@CLASSES:
//   bsls::ByteOrderUtil_Impl: namespace for swapping functions
//
//@MACROS:
//   BSLS_BYTEORDERUTIL_IMPL_CUSTOM_16:  'customeSwap16'  function is defined
//   BSLS_BYTEORDERUTIL_IMPL_CUSTOM_32:  'customeSwap32'  function is defined
//   BSLS_BYTEORDERUTIL_IMPL_CUSTOM_64:  'customeSwap64'  function is defined
//   BSLS_BYTEORDERUTIL_IMPL_CUSTOM_P16: 'customeSwapP16' function is defined
//   BSLS_BYTEORDERUTIL_IMPL_CUSTOM_P32: 'customeSwapP32' function is defined
//   BSLS_BYTEORDERUTIL_IMPL_CUSTOM_P64: 'customeSwapP64' function is defined
//
//@SEE ALSO: bsls_byteorderutil, bsls_byteorder
//
//@DESCRIPTION: This component provides a namespace 'struct',
// 'bsls::ByteOrderUtil_Impl', for a number of high performance
// platform-specific static function implementations for functions and macros
// in the 'bsls_byteorder' and 'bsls_byteorderutil' components.
//
// This component is only intended to be used by 'bsls_byteorderutil'.  No
// other component should call anything in it directly.
//
// The functions come in 3 groups:
//: o 'customSwapNN' -- take an 'NN' bit value by value and return it with its
//:   byte order swapped.
//: o 'customSwapPNN' -- take a pointer to a constant 'NN' bit value and return
//:   the dereferenced value with its byte order swapped.
//: o 'genericSwapNN' -- take an 'NN' bit value by value and return it with its
//:   byte order swapped.
// the 'customSwap*' functions are not always defined on all platforms.  When
// they are defined the corresponding macro is defined to signal
// 'bsls_byteorderutil' that it may use that function.  If neither the
// 'customeSwapNN' nor the 'customSwapPNN' macros are defined,
// 'bsls_byteorderutil' is to use the 'genericSwapNN' function, which is always
// defined.

#ifndef INCLUDED_BSLS_PLATFORM
#include <bsls_platform.h>
#endif

#ifndef INCLUDED_BSLS_TYPES
#include <bsls_types.h>
#endif

#ifdef BSLS_PLATFORM_CMP_HP

#ifndef INCLUDED_MACHINE_SYS_BUILTINS
#include <machine/sys/builtins.h>  // '_Asm_xchg'
#define INCLUDED_MACHINE_SYS_BUILTINS
#endif

#ifndef INCLUDED_MACHINE_SYS_INLINE
#include <machine/sys/inline.h>    // '_SZ_H', '_SZ_W', '_SZ_D'
#define INCLUDED_MACHINE_SYS_INLINE
#endif

#endif

#ifdef BSLS_PLATFORM_CMP_MSVC

#ifndef INCLUDED_STDLIB
#include <stdlib.h>        // '_byteswap_*'
#define INCLUDED_STDLIB
#endif

#endif

namespace BloombergLP {

// ============================================================================
//                                  LOCAL MACROS
// ============================================================================

                  // --------------------------------------------
                  // macro BSLS_BYTEORDERUTIL_COMPILE_TIME_ASSERT
                  // --------------------------------------------

// We don't have access to 'BSLMF_ASSERT' here in 'bsls' -- do a crude
// compile-time assert for use in 'bsls_byteorderutil'.  This macro will
// deliberately cause a compilation error if 'expr' evaluates to 'false'.
// 'expr' must be a compile-time expression.  Note that this macro can only be
// called in a code body.  Also note that this macro is not to be used outside
// this file.

#define BSLS_BYTEORDERUTIL_IMPL_COMPILE_TIME_ASSERT(expr)                     \
        { enum { k_NOT_INFINITY = 1 / static_cast<int>(expr) };               \
        (void) k_NOT_INFINITY; }

// ============================================================================
//                          INLINE FUNCTION DEFINITIONS
// ============================================================================

                         // -------------------------
                         // struct ByteOrderUtil_Impl
                         // -------------------------

#if defined(BSLS_PLATFORM_CMP_GNU) && BSLS_PLATFORM_CMP_VER_MAJOR >= 40300

// ----------------------------------------------------------------------------
// Advanced GNU

// Let the 16-bit gnu implementation default to 'genericSwap16' or other
// platform-specific gnu implementations.

#define BSLS_BYTEORDERUTIL_IMPL_CUSTOM_32 1
#define BSLS_BYTEORDERUTIL_IMPL_CUSTOM_64 1

// LEVEL 0 METHODS

#define BSLS_BYTEORDERUTIL_IMPL_CUSTOMSWAP_32(dstType, x)                     \
    return static_cast<dstType>(__builtin_bswap32(static_cast<int>(x)))

#define BSLS_BYTEORDERUTIL_IMPL_CUSTOMSWAP_64(dstType, x)                     \
    return static_cast<dstType>(__builtin_bswap64(                            \
                              static_cast<BloombergLP::bsls::Types::Int64>(x)))

#elif defined(BSLS_PLATFORM_CMP_MSVC)

// ----------------------------------------------------------------------------
// MSVC

#define BSLS_BYTEORDERUTIL_IMPL_CUSTOM_16 1
#define BSLS_BYTEORDERUTIL_IMPL_CUSTOM_32 1
#define BSLS_BYTEORDERUTIL_IMPL_CUSTOM_64 1

#define BSLS_BYTEORDERUTIL_IMPL_CUSTOMSWAP_16(dstType, x)                     \
    return static_cast<dstType>(_byteswap_ushort(                             \
                                               static_cast<unsigned short>(x)))

#define BSLS_BYTEORDERUTIL_IMPL_CUSTOMSWAP_32(dstType, x)                     \
    return static_cast<dstType>(_byteswap_ulong(static_cast<unsigned int>(x)))

#define BSLS_BYTEORDERUTIL_IMPL_CUSTOMSWAP_64(dstType, x)                     \
    return static_cast<dstType>(_byteswap_uint64(                             \
                             static_cast<BloombergLP::bsls::Types::Uint64>(x)))

#elif defined(BSLS_PLATFORM_CPU_POWERPC) && defined(BSLS_PLATFORM_CMP_IBM)    \
    && BSLS_PLATFORM_CMP_VER_MAJOR >= 0x0800

// ----------------------------------------------------------------------------
// POWERPC-AIX

#define BSLS_BYTEORDERUTIL_IMPL_CUSTOM_P16 1
#define BSLS_BYTEORDERUTIL_IMPL_CUSTOM_P32 1
#define BSLS_BYTEORDERUTIL_IMPL_CUSTOM_P64 1

unsigned short
bsls_byteOrderUtil_Impl_powerpc_swap_p16(const unsigned short *x);
    // Return the specified '*x' with byte order swapped.

// The following is equivalent to:
//..
//  lhbrx r3,0,r3
//..
// The following is necessary to work around the bug reported in DRQS 16073004
// using inline assembly with xlC10:
#pragma mc_func bsls_byteOrderUtil_Impl_powerpc_swap_p16 { "7c601e2c" }
#pragma reg_killed_by bsls_byteOrderUtil_Impl_powerpc_swap_p16 gr3

unsigned int bsls_byteOrderUtil_Impl_powerpc_swap_p32(const unsigned int *x);
    // Return the specified '*x' with byte order swapped.

// The following is equivalent to:
//..
//  lwbrx r3,0,r3
//..
// This follows the AIX ABI: the first argument is received in 'r3' and the
// return value is stored in 'r3'.  The hex value specified for 'mc_func' is
// the opcode of the above code.

#pragma mc_func bsls_byteOrderUtil_Impl_powerpc_swap_p32 { "7c601c2c" }
#pragma reg_killed_by bsls_byteOrderUtil_Impl_powerpc_swap_p32 gr3

unsigned long long bsls_byteOrderUtil_Impl_powerpc_swap_p64(
                                                  const unsigned long long *x);
    // Return the specified '*x' with byte order swapped.

#ifdef BSLS_PLATFORM_CPU_32_BIT
// The following is equivalent to:
//..
//  lwbrx r4,0,r3      // reverse the 4 higher-order bytes
//  addi r3,r3,4       // change r3 to point to the 4 lower-order bytes
//  lwbrx r3,0,r3      // reverse the 4 lower-order bytes
//..
#pragma mc_func bsls_byteOrderUtil_Impl_powerpc_swap_p64                   \
                                            { "7c801c2c" "38630004" "7c601c2c"}
#pragma reg_killed_by bsls_byteOrderUtil_Impl_powerpc_swap_p64 gr3,gr4

#else  // BSLS_PLATFORM_CPU_64_BIT
// The following is equivalent to:
//..
//  addi r4,r3,4       // move address of the 4 lower-order bytes to 'r4'
//  lwbrx r3,0,r3      // reverse the 4 higher-order bytes
//  lwbrx r4,0,r4      // reverse the 4 lower-order bytes
//  rldimi r3,r4,32,0  // rotate 'r4' left and insert to 'r3' with a mask
//..
#pragma mc_func bsls_byteOrderUtil_Impl_powerpc_swap_p64                   \
                                { "38830004" "7c601c2c" "7c80242c" "7883000e" }
#pragma reg_killed_by bsls_byteOrderUtil_Impl_powerpc_swap_p64 gr3,gr4,cr0

#endif  // BSLS_PLATFORM_CPU_32_BIT else

#define BSLS_BYTEORDERUTIL_IMPL_CUSTOMSWAP_P16(dstType, x)                    \
    do {                                                                      \
        BSLS_BYTEORDERUTIL_IMPL_COMPILE_TIME_ASSERT(2 == sizeof(*x));         \
        return static_cast<dstType>(bsls_byteOrderUtil_Impl_powerpc_swap_p16( \
                               reinterpret_cast<const unsigned short *>(x))); \
    } while(false)

#define BSLS_BYTEORDERUTIL_IMPL_CUSTOMSWAP_P32(dstType, x)                    \
    do {                                                                      \
        BSLS_BYTEORDERUTIL_IMPL_COMPILE_TIME_ASSERT(4 == sizeof(*x));         \
        return static_cast<dstType>(bsls_byteOrderUtil_Impl_powerpc_swap_p32( \
                               reinterpret_cast<const unsigned int *>(x)));   \
    } while(false)

#define BSLS_BYTEORDERUTIL_IMPL_CUSTOMSWAP_P64(dstType, x)                    \
    do {                                                                      \
        BSLS_BYTEORDERUTIL_IMPL_COMPILE_TIME_ASSERT(8 == sizeof(*x));         \
        return static_cast<dstType>(bsls_byteOrderUtil_Impl_powerpc_swap_p64( \
            reinterpret_cast<const BloombergLP::bsls::Types::Uint64 *>(x)));  \
    } while(false)

#endif  // BSLS_PLATFORM_CPU_POWERPC

#if defined(BSLS_PLATFORM_CPU_SPARC)

// ----------------------------------------------------------------------------
// Sparc

#if defined(BSLS_PLATFORM_CMP_GNU)

#define BSLS_BYTEORDERUTIL_IMPL_CUSTOM_P16 1

// LEVEL 1 METHODS

// We have to use "r"(x) instead of "m"(*x) because certain instructions do not
// support the 'm' constraint.  The 'm' constraint is the only way to tell the
// compiler we are reading the value of '*x' and not just 'x'.

#define BSLS_BYTEORDERUTIL_IMPL_CUSTOMSWAP_P16(dstType, x)                    \
    do {                                                                      \
        BSLS_BYTEORDERUTIL_IMPL_COMPILE_TIME_ASSERT(2 == sizeof(*x));         \
        register unsigned int y;                                              \
        asm("lduha [%1] %2, %0"                                               \
          : "=r" (y)                                                          \
          : "r" (x), "i"(0x88), "m" (                                         \
                     *reinterpret_cast<const unsigned short *>(x)));          \
                                                                              \
        return static_cast<dstType>(y);                                       \
    } while(false)

#if   !defined(BSLS_BYTEORDERUTIL_IMPL_CUSTOM_32)                             \
   && !defined(BSLS_BYTEORDERUTIL_IMPL_CUSTOM_64)                             \
   && !defined(BSLS_BYTEORDERUTIL_IMPL_CUSTOM_P32)                            \
   && !defined(BSLS_BYTEORDERUTIL_IMPL_CUSTOM_P64)

#define BSLS_BYTEORDERUTIL_IMPL_CUSTOM_P32 1
#define BSLS_BYTEORDERUTIL_IMPL_CUSTOM_P64 1

// sparc gnu pre-4.03 impl

#define BSLS_BYTEORDERUTIL_IMPL_CUSTOMSWAP_P32(dstType, x)                    \
    do {                                                                      \
        BSLS_BYTEORDERUTIL_IMPL_COMPILE_TIME_ASSERT(4 == sizeof(*x));         \
        register unsigned int y;                                              \
        asm("lduwa [%1] %2, %0"                                               \
          : "=r" (y)                                                          \
          : "r" (x), "i"(0x88), "m" (                                         \
                                *reinterpret_cast<const unsigned int *>(x))); \
                                                                              \
        return static_cast<dstType>(y);                                       \
    } while(false)

#if defined(BSLS_PLATFORM_CPU_64_BIT)

// sparc gnu pre-4.03 impl

#define BSLS_BYTEORDERUTIL_IMPL_CUSTOMSWAP_P64(dstType, x)                    \
    do {                                                                      \
        BSLS_BYTEORDERUTIL_IMPL_COMPILE_TIME_ASSERT(8 == sizeof(*x));         \
        register Types::Uint64 y;                                             \
        asm("ldxa [%1] %2, %0"                                                \
          : "=r" (y)                                                          \
          : "r" (x), "i"(0x88), "m" (                                         \
            *reinterpret_cast<const BloombergLP::bsls::Types::Uint64 *>(x))); \
                                                                              \
        return static_cast<dstType>(y);                                       \
    } while(false)

#else

// sparc gnu pre-4.03 impl

//  asm("ldxa [%1] %2, %0\n\t"  // After the load, the full data is in '%0'.
//                              // But we have to split it into two registers
//                              // since we are running in 32-bit mode.
//
//      "srl   %0, 0, %R0\n\t"  // The '%R0' specifies the lower-order bits of
//                              // a pair register, while '%0' specifies the
//                              // higher-order bits.  Move the lower-order
//                              // bits of the result to '%R0'.
//
//      "srlx  %0, 32, %0"      // Shift the higher-order bits of the result.
//    : "=r" (y)
//    : "r" (x), "i"(0x88), "m" (*x));

#define BSLS_BYTEORDERUTIL_IMPL_CUSTOMSWAP_P64(dstType, x)                    \
    do {                                                                      \
        BSLS_BYTEORDERUTIL_IMPL_COMPILE_TIME_ASSERT(8 == sizeof(*x));         \
        register BloombergLP::bsls::Types::Uint64 y;                          \
        asm("ldxa [%1] %2, %0\n\t"                                            \
            "srl   %0, 0, %R0\n\t"                                            \
            "srlx  %0, 32, %0"                                                \
          : "=r" (y)                                                          \
          : "r" (x), "i"(0x88), "m" (                                         \
               *reinterpret_cast<BloombergLP::bsls::Types::Uint64 *>(x)));    \
                                                                              \
        return static_cast<dstType>(y);                                       \
    } while(false)

#endif  // BSLS_PLATFORM_CPU_64_BIT else

#endif  // 32 & 64 bit not defined

#else  // BSLS_PLATFORM_CMP_GNU else

// Commented out assembly implementation since 'genericSwap{16,32} is
// probably faster than the function call for 16 and 32 bits.

#define BSLS_BYTEORDERUTIL_IMPL_CUSTOM_P64 1

extern "C" {
unsigned long long bsls_byteOrderUtil_Impl_sparc_CC_swap_p64(
                                                  const unsigned long long *x);
    // Return the specified '*x' with byte order swapped.  Sparc CC impl in
    // .cpp file.
}

#define BSLS_BYTEORDERUTIL_IMPL_CUSTOMSWAP_P64(dstType, x)                    \
    do {                                                                      \
        BSLS_BYTEORDERUTIL_IMPL_COMPILE_TIME_ASSERT(8 == sizeof(*x));         \
        return static_cast<dstType>(                                          \
                   bsls_byteOrderUtil_Impl_sparc_CC_swap_p64(                 \
                          reinterpret_cast<const unsigned long long *>(x)));  \
    } while(false)

#endif  // BSLS_PLATFORM_CMP_GNU else

#elif (defined(BSLS_PLATFORM_CPU_X86) || defined(BSLS_PLATFORM_CPU_X86_64))   \
    && defined(BSLS_PLATFORM_CMP_GNU)

// Note that 32 and 64 bit may have already been defined by the advanced GNU
// case.

#define BSLS_BYTEORDERUTIL_IMPL_CUSTOM_16 1

// LEVEL 1 METHODS

// x86 gnu impl

#define BSLS_BYTEORDERUTIL_IMPL_CUSTOMSWAP_16(dstType, x)                     \
    do {                                                                      \
        register unsigned short y;                                            \
        __asm__ ("xchg %b0, %h0" : "=Q" (y) : "0" (                           \
					static_cast<unsigned short>(x)));     \
                                                                              \
        return static_cast<dstType>(y);                                       \
    } while(false)

#if   !defined(BSLS_BYTEORDERUTIL_IMPL_CUSTOM_32)                             \
   && !defined(BSLS_BYTEORDERUTIL_IMPL_CUSTOM_64)                             \
   && !defined(BSLS_BYTEORDERUTIL_IMPL_CUSTOM_P32)                            \
   && !defined(BSLS_BYTEORDERUTIL_IMPL_CUSTOM_P64)

#define BSLS_BYTEORDERUTIL_IMPL_CUSTOM_32 1
#define BSLS_BYTEORDERUTIL_IMPL_CUSTOM_64 1

// x86 gnu pre-4.03 impl

#define BSLS_BYTEORDERUTIL_IMPL_CUSTOMSWAP_32(dstType, x)                     \
    do {                                                                      \
        register unsigned int y;                                              \
        __asm__ ("bswap %0" : "=r" (y) : "0" (static_cast<unsigned int>(x))); \
                                                                              \
        return static_cast<dstType>(y);                                       \
    } while(false)

#if BSLS_PLATFORM_CPU_32_BIT

// x86 gnu pre-4.03 impl

#define BSLS_BYTEORDERUTIL_IMPL_CUSTOMSWAP_64(dstType, x)                     \
    do {                                                                      \
        typedef BloombergLP::bsls::Types::Uint64 Uint64;                      \
                                                                              \
        BSLS_BYTEORDERUTIL_IMPL_COMPILE_TIME_ASSERT(8 == sizeof(x));          \
                                                                              \
        register unsigned int res, tmp;                                       \
        __asm__ ("bswap %0\n\t"                                               \
                 "bswap %1\n\t"                                               \
               : "=r" (res), "=r" (tmp)                                       \
               : "0" ((unsigned) x), "1" ((unsigned) (x >> 32)));             \
                                                                              \
        return static_cast<dstType>(((Uint64) res << 32) | (Uint64) tmp);     \
    } while(false)

#else  // BSLS_PLATFORM_CPU_64_BIT

// x86 gnu pre-4.03 impl

#define BSLS_BYTEORDERUTIL_IMPL_CUSTOMSWAP_64(dstType, x)                     \
    do {                                                                      \
        typedef BloombergLP::bsls::Types::Uint64 Uint64;                      \
                                                                              \
        BSLS_BYTEORDERUTIL_IMPL_COMPILE_TIME_ASSERT(8 == sizeof(x));          \
                                                                              \
        register Types::Uint64 y;                                             \
        __asm__ ("bswap %0" : "=r" (y) : "0" (x));                            \
                                                                              \
        return static_cast<dstType>(y);                                       \
    } while(false)

#endif  // 32 & 64 not previously defined
#endif  // BSLS_PLATFORM_CPU_32_BIT else
#endif  // BSLS_PLATFORM_CMP_GNU && X86

#define BSLS_BYTEORDERUTIL_IMPL_GENERICSWAP_16(dstType, x)                    \
    do {                                                                      \
        BSLS_BYTEORDERUTIL_IMPL_COMPILE_TIME_ASSERT(2 == sizeof(x));          \
        BSLS_BYTEORDERUTIL_IMPL_COMPILE_TIME_ASSERT(2 == sizeof(dstType));    \
                                                                              \
        return static_cast<dstType>(                                          \
                        (static_cast<unsigned short>(x) >> 8) | (x << 8));    \
    } while(false)

#define BSLS_BYTEORDERUTIL_IMPL_GENERICSWAP_32(dstType, x)                    \
    do {                                                                      \
        BSLS_BYTEORDERUTIL_IMPL_COMPILE_TIME_ASSERT(4 == sizeof(x));          \
        BSLS_BYTEORDERUTIL_IMPL_COMPILE_TIME_ASSERT(4 == sizeof(dstType));    \
                                                                              \
        return static_cast<dstType>((x                           << 24)       \
				 | ((x & 0x0000ff00)             <<  8)       \
				 | ((x & 0x00ff0000)             >>  8)       \
				 | (static_cast<unsigned int>(x) >> 24));     \
    } while(false)

#define BSLS_BYTEORDERUTIL_IMPL_GENERICSWAP_64(dstType, x)                    \
    do {                                                                      \
        typedef BloombergLP::bsls::Types::Uint64 Uiint64;                     \
                                                                              \
        BSLS_BYTEORDERUTIL_IMPL_COMPILE_TIME_ASSERT(8 == sizeof(x));          \
        BSLS_BYTEORDERUTIL_IMPL_COMPILE_TIME_ASSERT(8 == sizeof(dstType));    \
                                                                              \
        return static_cast<dstType>(( x                         << 56)        \
                                  | ((x & 0x000000000000ff00LL) << 40)        \
                                  | ((x & 0x0000000000ff0000LL) << 24)        \
                                  | ((x & 0x00000000ff000000LL) <<  8)        \
                                  | ((x & 0x000000ff00000000LL) >>  8)        \
                                  | ((x & 0x0000ff0000000000LL) >> 24)        \
                                  | ((x & 0x00ff000000000000LL) >> 40)        \
                                  | (static_cast<Uint64>(x)     >> 56));      \
    } while(false)

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
