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
//   BSLS_BYTEORDERUTIL_IMPL_CUSTOMSWAP_16(x)   return 'x' with bytes swapped
//   BSLS_BYTEORDERUTIL_IMPL_CUSTOMSWAP_32(x)   return 'x' with bytes swapped
//   BSLS_BYTEORDERUTIL_IMPL_CUSTOMSWAP_64(x)   return 'x' with bytes swapped
//   BSLS_BYTEORDERUTIL_IMPL_CUSTOMSWAP_P16(&x) return 'x' with bytes swapped
//   BSLS_BYTEORDERUTIL_IMPL_CUSTOMSWAP_P32(&x) return 'x' with bytes swapped
//   BSLS_BYTEORDERUTIL_IMPL_CUSTOMSWAP_P64(&x) return 'x' with bytes swapped
//   BSLS_BYTEORDERUTIL_IMPL_GENERICSWAP_16(x)  return 'x' with bytes swapped
//   BSLS_BYTEORDERUTIL_IMPL_GENERICSWAP_32(x)  return 'x' with bytes swapped
//   BSLS_BYTEORDERUTIL_IMPL_GENERICSWAP_64(x)  return 'x' with bytes swapped
//
//@SEE_ALSO: bsls_byteorderutil, bsls_byteorder
//
//@DESCRIPTION: This component provides a template class 'ByteOrderUtil_Impl'
// and a set of macros suitable for swapping byte orders of all integral types.
// The '*_CUSTOMSWAP_*' macros use assembly language or compiler primitives,
// whereas the '*_GENERICSWAP_*' macros use C bitwise operations to perform the
// swap.  The '*_CUSTOMSWAP_*' macros are not defined on all platforms; callers
// must perform an '#ifdef' to see if they are defined before calling them.  At
// most one of '*_CUSTOMSWAP_NN' and '*_CUSTOMSWAP_PNN' are defined on any one
// platform for any value of 'NN', while '*_GENERICSWAP_NN' macros are defined
// on all platforms and are meant to be called when the other macros are not
// available, and are also used for benchmarking.

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_BSLS_PLATFORM
#include <bsls_platform.h>
#endif

#ifndef INCLUDED_BSLS_TYPES
#include <bsls_types.h>
#endif

#ifdef BSLS_PLATFORM_CMP_MSVC

#ifndef INCLUDED_STDLIB_H
#include <stdlib.h>        // '_byteswap_*'
#define INCLUDED_STDLIB_H
#endif

#endif

namespace BloombergLP {
namespace bsls {

                         // =========================
                         // struct ByteOrderUtil_Impl
                         // =========================

template <class TYPE, Types::size_type WIDTH = sizeof(TYPE)>
struct ByteOrderUtil_Impl;

template <class TYPE>
struct ByteOrderUtil_Impl<TYPE, 1> {
    // This 'class' provides a namespace for functions used for reversing the
    // byte order of values having integral type.

    // CLASS METHODS
    static TYPE swapBytes(TYPE x);
        // Return the value that results from reversing the order of the bytes
        // in the specified 'x'.
};

template <class TYPE>
struct ByteOrderUtil_Impl<TYPE, 2> {
    // This 'class' provides a namespace for functions used for reversing the
    // byte order of values having integral type.

    // CLASS METHODS
    static TYPE swapBytes(TYPE x);
        // Return the value that results from reversing the order of the bytes
        // in the specified 'x'.
};

template <class TYPE>
struct ByteOrderUtil_Impl<TYPE, 4> {
    // This 'class' provides a namespace for functions used for reversing the
    // byte order of values having integral type.

    // CLASS METHODS
    static TYPE swapBytes(TYPE x);
        // Return the value that results from reversing the order of the bytes
        // in the specified 'x'.
};

template <class TYPE>
struct ByteOrderUtil_Impl<TYPE, 8> {
    // This 'class' provides a namespace for functions used for reversing the
    // byte order of values having integral type.

    // CLASS METHODS
    static TYPE swapBytes(TYPE x);
        // Return the value that results from reversing the order of the bytes
        // in the specified 'x'.
};

}  // close package namespace

// ============================================================================
//                                  MACROS
// ============================================================================

// These macros are only intended to be used in this component and
// 'bsls_byteorderutil'.

// We did benchmarks and found that many of the custom assembly implementations
// below were slower than the generic implementation, so we disable the slow
// ones unless 'BSLS_BYTEORDERUTIL_IMPL_ENABLE_COUNTERPRODUCTIVE_MACROS' is
// defined.

               // -------------------------------------------------
               // macro BSLS_BYTEORDERUTIL_IMPL_COMPILE_TIME_ASSERT
               // -------------------------------------------------

// We don't have access to 'BSLMF_ASSERT' here in 'bsls' -- do a crude
// compile-time assert for use in 'bsls_byteorderutil'.  This macro will
// deliberately cause a compilation error if 'expr' evaluates to 'false'.
// 'expr' must be a compile-time expression.  Note that this macro can only be
// called in a code body.  This macro is only intended to be used in this
// component and 'bsls_byteorderutil'.

#if (defined(BSLS_ASSERT_SAFE_IS_ACTIVE) ||                                   \
     defined(BSLS_ASSERT_IS_ACTIVE))

# define BSLS_BYTEORDERUTIL_IMPL_COMPILE_TIME_ASSERT(expr)                    \
        { enum { k_NOT_INFINITY = 1 / static_cast<int>(expr) };               \
        (void) k_NOT_INFINITY; }

#else

# define BSLS_BYTEORDERUTIL_IMPL_COMPILE_TIME_ASSERT(expr)

#endif

#ifndef BSLS_BYTEORDERUTIL_IMPL_ENABLE_COUNTERPRODUCTIVE_MACROS
#define BSLS_BYTEORDERUTIL_IMPL_DISABLE_COUNTERPRODUCTIVE_MACROS 1
#endif

#if (defined(BSLS_PLATFORM_CMP_GNU) && BSLS_PLATFORM_CMP_VERSION >= 40300)    \
 || defined(BSLS_PLATFORM_CMP_CLANG)

// ----------------------------------------------------------------------------
// Advanced GNU

// Let the 16-bit GNU implementation default to
// 'BSLS_BYTEORDERUTIL_IMPL_GENERICSWAP_16' or other platform-specific GNU
// implementations.

#if !defined(BSLS_BYTEORDERUTIL_IMPL_DISABLE_COUNTERPRODUCTIVE_MACROS) ||     \
    !defined(BSLS_PLATFORM_OS_SOLARIS)

#define BSLS_BYTEORDERUTIL_IMPL_CUSTOMSWAP_32(dstType, x)                     \
    return static_cast<dstType>(__builtin_bswap32(static_cast<int>(x)))

#endif

#if !defined(BSLS_BYTEORDERUTIL_IMPL_DISABLE_COUNTERPRODUCTIVE_MACROS) ||     \
    !defined(BSLS_PLATFORM_OS_SOLARIS) ||                                     \
    !defined(BSLS_PLATFORM_CPU_64_BIT) || !defined(BDE_BUILD_TARGET_OPT)

#define BSLS_BYTEORDERUTIL_IMPL_CUSTOMSWAP_64(dstType, x)                     \
    return static_cast<dstType>(__builtin_bswap64(                            \
                              static_cast<BloombergLP::bsls::Types::Int64>(x)))

#endif

#elif defined(BSLS_PLATFORM_CMP_MSVC)

// ----------------------------------------------------------------------------
// Microsoft Visual C++

#if !defined(BSLS_BYTEORDERUTIL_IMPL_DISABLE_COUNTERPRODUCTIVE_MACROS)

#define BSLS_BYTEORDERUTIL_IMPL_CUSTOMSWAP_16(dstType, x)                     \
    return static_cast<dstType>(_byteswap_ushort(                             \
                                               static_cast<unsigned short>(x)))

#endif

#define BSLS_BYTEORDERUTIL_IMPL_CUSTOMSWAP_32(dstType, x)                     \
    return static_cast<dstType>(_byteswap_ulong(static_cast<unsigned int>(x)))

#define BSLS_BYTEORDERUTIL_IMPL_CUSTOMSWAP_64(dstType, x)                     \
    return static_cast<dstType>(_byteswap_uint64(                             \
                             static_cast<BloombergLP::bsls::Types::Uint64>(x)))

#elif defined(BSLS_PLATFORM_CPU_POWERPC) && defined(BSLS_PLATFORM_CMP_IBM)    \
    && BSLS_PLATFORM_CMP_VER_MAJOR >= 0x0800 &&                               \
     !defined(BSLS_BYTEORDERUTIL_IMPL_DISABLE_COUNTERPRODUCTIVE_MACROS)

// ----------------------------------------------------------------------------
// POWERPC-AIX

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
#pragma mc_func bsls_byteOrderUtil_Impl_powerpc_swap_p64                      \
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
#pragma mc_func bsls_byteOrderUtil_Impl_powerpc_swap_p64                      \
                                { "38830004" "7c601c2c" "7c80242c" "7883000e" }
#pragma reg_killed_by bsls_byteOrderUtil_Impl_powerpc_swap_p64 gr3,gr4,cr0

#endif  // BSLS_PLATFORM_CPU_32_BIT else

#define BSLS_BYTEORDERUTIL_IMPL_CUSTOMSWAP_P16(dstType, x)                    \
    {                                                                         \
        BSLS_BYTEORDERUTIL_IMPL_COMPILE_TIME_ASSERT(2 == sizeof *x);          \
        return static_cast<dstType>(bsls_byteOrderUtil_Impl_powerpc_swap_p16( \
                               reinterpret_cast<const unsigned short *>(x))); \
    }

#define BSLS_BYTEORDERUTIL_IMPL_CUSTOMSWAP_P32(dstType, x)                    \
    {                                                                         \
        BSLS_BYTEORDERUTIL_IMPL_COMPILE_TIME_ASSERT(4 == sizeof *x);          \
        return static_cast<dstType>(bsls_byteOrderUtil_Impl_powerpc_swap_p32( \
                               reinterpret_cast<const unsigned int *>(x)));   \
    }

#define BSLS_BYTEORDERUTIL_IMPL_CUSTOMSWAP_P64(dstType, x)                    \
    {                                                                         \
        BSLS_BYTEORDERUTIL_IMPL_COMPILE_TIME_ASSERT(8 == sizeof *x);          \
        return static_cast<dstType>(bsls_byteOrderUtil_Impl_powerpc_swap_p64( \
            reinterpret_cast<const BloombergLP::bsls::Types::Uint64 *>(x)));  \
    }

#endif  // BSLS_PLATFORM_CPU_POWERPC

#if defined(BSLS_PLATFORM_CPU_SPARC)

// ----------------------------------------------------------------------------
// Sparc

#if defined(BSLS_PLATFORM_CMP_GNU) || defined(BSLS_PLATFORM_CMP_CLANG)

#if !defined(BSLS_BYTEORDERUTIL_IMPL_DISABLE_COUNTERPRODUCTIVE_MACROS) ||     \
    !defined(BDE_BUILD_TARGET_OPT)

// LEVEL 1 METHODS

// We have to use "r"(x) instead of "m"(*x) because certain instructions do not
// support the 'm' constraint.  The 'm' constraint is the only way to tell the
// compiler we are reading the value of '*x' and not just 'x'.

#define BSLS_BYTEORDERUTIL_IMPL_CUSTOMSWAP_P16(dstType, x)                    \
    {                                                                         \
        BSLS_BYTEORDERUTIL_IMPL_COMPILE_TIME_ASSERT(2 == sizeof *x);          \
        register unsigned int y;                                              \
        asm("lduha [%1] %2, %0"                                               \
          : "=r" (y)                                                          \
          : "r" (x), "i"(0x88), "m" (                                         \
                     *reinterpret_cast<const unsigned short *>(x)));          \
                                                                              \
        return static_cast<dstType>(y);                                       \
    }

#endif // !disabled || !opt

#if !defined(BSLS_BYTEORDERUTIL_IMPL_DISABLE_COUNTERPRODUCTIVE_MACROS) &&     \
    !defined(BSLS_BYTEORDERUTIL_IMPL_CUSTOMSWAP_32) &&                        \
    !defined(BSLS_BYTEORDERUTIL_IMPL_CUSTOMSWAP_P32)

// sparc GNU pre-4.03 impl

#define BSLS_BYTEORDERUTIL_IMPL_CUSTOMSWAP_P32(dstType, x)                    \
    {                                                                         \
        BSLS_BYTEORDERUTIL_IMPL_COMPILE_TIME_ASSERT(4 == sizeof *x);          \
        register unsigned int y;                                              \
        asm("lduwa [%1] %2, %0"                                               \
          : "=r" (y)                                                          \
          : "r" (x), "i"(0x88), "m" (                                         \
                                *reinterpret_cast<const unsigned int *>(x))); \
                                                                              \
        return static_cast<dstType>(y);                                       \
    }

#endif // not disabled, ...CUSTOMSWAP_32, CUSTOMSWAP_P32 not defined

#if   !defined(BSLS_BYTEORDERUTIL_IMPL_CUSTOMSWAP_64)                         \
   && !defined(BSLS_BYTEORDERUTIL_IMPL_CUSTOMSWAP_P64)

#if defined(BSLS_PLATFORM_CPU_64_BIT)

// sparc GNU pre-4.03 impl

#define BSLS_BYTEORDERUTIL_IMPL_CUSTOMSWAP_P64(dstType, x)                    \
    {                                                                         \
        BSLS_BYTEORDERUTIL_IMPL_COMPILE_TIME_ASSERT(8 == sizeof *x);          \
        register BloombergLP::bsls::Types::Uint64 y;                          \
        asm("ldxa [%1] %2, %0"                                                \
          : "=r" (y)                                                          \
          : "r" (x), "i"(0x88), "m" (                                         \
            *reinterpret_cast<const BloombergLP::bsls::Types::Uint64 *>(x))); \
                                                                              \
        return static_cast<dstType>(y);                                       \
    }

#else

// sparc GNU pre-4.03 impl

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
    {                                                                         \
        BSLS_BYTEORDERUTIL_IMPL_COMPILE_TIME_ASSERT(8 == sizeof *x);          \
        register BloombergLP::bsls::Types::Uint64 y;                          \
        asm("ldxa [%1] %2, %0\n\t"                                            \
            "srl   %0, 0, %R0\n\t"                                            \
            "srlx  %0, 32, %0"                                                \
          : "=r" (y)                                                          \
          : "r" (x), "i"(0x88), "m" (                                         \
               *reinterpret_cast<BloombergLP::bsls::Types::Uint64 *>(x)));    \
                                                                              \
        return static_cast<dstType>(y);                                       \
    }

#endif  // BSLS_PLATFORM_CPU_64_BIT else

#endif  // 64 bit not defined

#else  // BSLS_PLATFORM_CMP_GNU || BSLS_PLATFORM_CMP_CLANG else

#if !defined(BSLS_BYTEORDERUTIL_IMPL_DISABLE_COUNTERPRODUCTIVE_MACROS) ||     \
    !defined(BDE_BUILD_TARGET_OPT)

// Solaris non-GNU assembly implementations must be out of line.  Removed 16-
// and 32-bit implementations since the generic implementations, which are
// always inline, are probably faster than a function call.

extern "C" {
unsigned long long bsls_byteOrderUtil_Impl_sparc_CC_swap_p64(
                                                  const unsigned long long *x);
    // Return the specified '*x' with byte order swapped.  Sparc CC impl in
    // .cpp file.
}

#define BSLS_BYTEORDERUTIL_IMPL_CUSTOMSWAP_P64(dstType, x)                    \
    {                                                                         \
        BSLS_BYTEORDERUTIL_IMPL_COMPILE_TIME_ASSERT(8 == sizeof *x);          \
        return static_cast<dstType>(                                          \
                   bsls_byteOrderUtil_Impl_sparc_CC_swap_p64(                 \
                          reinterpret_cast<const unsigned long long *>(x)));  \
    }

#endif  // !disabled || !opt

#endif  // BSLS_PLATFORM_CMP_GNU || BSLS_PLATFORM_CMP_CLANG else

#elif  (defined(BSLS_PLATFORM_CPU_X86) || defined(BSLS_PLATFORM_CPU_X86_64))  \
    && (defined(BSLS_PLATFORM_CMP_GNU) || defined(BSLS_PLATFORM_CMP_CLANG))

#if    !defined(BSLS_BYTEORDERUTIL_IMPL_DISABLE_COUNTERPRODUCTIVE_MACROS)

// Note that 32 and 64 bit may have already been defined by the advanced GNU
// case.

// x86 GNU impl

#define BSLS_BYTEORDERUTIL_IMPL_CUSTOMSWAP_16(dstType, x)                     \
    {                                                                         \
        register unsigned short y;                                            \
        __asm__ ("xchg %b0, %h0" : "=Q" (y) : "0" (                           \
                                        static_cast<unsigned short>(x)));     \
                                                                              \
        return static_cast<dstType>(y);                                       \
    }

#endif // !disabled

#if   !defined(BSLS_BYTEORDERUTIL_IMPL_CUSTOMSWAP_32)                         \
   && !defined(BSLS_BYTEORDERUTIL_IMPL_CUSTOMSWAP_64)                         \
   && !defined(BSLS_BYTEORDERUTIL_IMPL_CUSTOMSWAP_P32)                        \
   && !defined(BSLS_BYTEORDERUTIL_IMPL_CUSTOMSWAP_P64)

// x86 GNU pre-4.03 impl

#define BSLS_BYTEORDERUTIL_IMPL_CUSTOMSWAP_32(dstType, x)                     \
    {                                                                         \
        register unsigned int y;                                              \
        __asm__ ("bswap %0" : "=r" (y) : "0" (static_cast<unsigned int>(x))); \
                                                                              \
        return static_cast<dstType>(y);                                       \
    }

#if BSLS_PLATFORM_CPU_32_BIT

// x86 GNU pre-4.03 impl

#define BSLS_BYTEORDERUTIL_IMPL_CUSTOMSWAP_64(dstType, x)                     \
    {                                                                         \
        typedef BloombergLP::bsls::Types::Uint64 Uint64;                      \
                                                                              \
        BSLS_BYTEORDERUTIL_IMPL_COMPILE_TIME_ASSERT(8 == sizeof x);           \
                                                                              \
        register unsigned int res, tmp;                                       \
        __asm__ ("bswap %0\n\t"                                               \
                 "bswap %1\n\t"                                               \
               : "=r" (res), "=r" (tmp)                                       \
               : "0" ((unsigned)x), "1" ((unsigned)(x >> 32)));               \
                                                                              \
        return static_cast<dstType>(((Uint64)res << 32) | (Uint64)tmp);       \
    }

#else  // BSLS_PLATFORM_CPU_64_BIT

// x86 GNU pre-4.03 impl

#define BSLS_BYTEORDERUTIL_IMPL_CUSTOMSWAP_64(dstType, x)                     \
    {                                                                         \
        typedef BloombergLP::bsls::Types::Uint64 Uint64;                      \
                                                                              \
        BSLS_BYTEORDERUTIL_IMPL_COMPILE_TIME_ASSERT(8 == sizeof x);           \
                                                                              \
        register Uint64 y;                                                    \
        __asm__ ("bswap %0" : "=r" (y) : "0" (x));                            \
                                                                              \
        return static_cast<dstType>(y);                                       \
    }

#endif  // 32 & 64 not previously defined
#endif  // BSLS_PLATFORM_CPU_32_BIT else
#endif  // (BSLS_PLATFORM_CMP_GNU || BSLS_PLATFORM_CMP_CLANG) && X86

#define BSLS_BYTEORDERUTIL_IMPL_GENERICSWAP_16(dstType, x)                    \
    {                                                                         \
        BSLS_BYTEORDERUTIL_IMPL_COMPILE_TIME_ASSERT(2 == sizeof x);           \
        BSLS_BYTEORDERUTIL_IMPL_COMPILE_TIME_ASSERT(2 == sizeof(dstType));    \
                                                                              \
        return static_cast<dstType>(                                          \
                        (static_cast<unsigned short>(x) >> 8) | (x << 8));    \
    }

#define BSLS_BYTEORDERUTIL_IMPL_GENERICSWAP_32(dstType, x)                    \
    {                                                                         \
        BSLS_BYTEORDERUTIL_IMPL_COMPILE_TIME_ASSERT(4 == sizeof x);           \
        BSLS_BYTEORDERUTIL_IMPL_COMPILE_TIME_ASSERT(4 == sizeof(dstType));    \
                                                                              \
        return static_cast<dstType>((x                           << 24)       \
                                 | ((x & 0x0000ff00U)            <<  8)       \
                                 | ((x & 0x00ff0000U)            >>  8)       \
                                 | (static_cast<unsigned int>(x) >> 24));     \
    }

#define BSLS_BYTEORDERUTIL_IMPL_GENERICSWAP_64(dstType, x)                    \
    {                                                                         \
        typedef BloombergLP::bsls::Types::Uint64 Uint64;                      \
                                                                              \
        BSLS_BYTEORDERUTIL_IMPL_COMPILE_TIME_ASSERT(8 == sizeof x);           \
        BSLS_BYTEORDERUTIL_IMPL_COMPILE_TIME_ASSERT(8 == sizeof(dstType));    \
                                                                              \
        return static_cast<dstType>(( x                          << 56)       \
                                  | ((x & 0x000000000000ff00ULL) << 40)       \
                                  | ((x & 0x0000000000ff0000ULL) << 24)       \
                                  | ((x & 0x00000000ff000000ULL) <<  8)       \
                                  | ((x & 0x000000ff00000000ULL) >>  8)       \
                                  | ((x & 0x0000ff0000000000ULL) >> 24)       \
                                  | ((x & 0x00ff000000000000ULL) >> 40)       \
                                  | (static_cast<Uint64>(x)      >> 56));     \
    }

namespace bsls {

// ============================================================================
//                               INLINE DEFINITIONS
// ============================================================================

                            // -------------------------
                            // struct ByteOrderUtil_Impl
                            // -------------------------

// CLASS METHODS
template <class TYPE>
inline
TYPE ByteOrderUtil_Impl<TYPE, 1>::swapBytes(TYPE x)
{
    return x;
}

template <class TYPE>
inline
TYPE ByteOrderUtil_Impl<TYPE, 2>::swapBytes(TYPE x)
{
    // These macros all return a value of type 'TYPE'.

#if   defined(BSLS_BYTEORDERUTIL_IMPL_CUSTOMSWAP_16)
    BSLS_BYTEORDERUTIL_IMPL_CUSTOMSWAP_16( TYPE, x);
#elif defined(BSLS_BYTEORDERUTIL_IMPL_CUSTOMSWAP_P16)
    BSLS_BYTEORDERUTIL_IMPL_CUSTOMSWAP_P16(TYPE, &x);
#else
    BSLS_BYTEORDERUTIL_IMPL_GENERICSWAP_16(TYPE, x);                  // RETURN
#endif
}

template <class TYPE>
inline
TYPE ByteOrderUtil_Impl<TYPE, 4>::swapBytes(TYPE x)
{
    // These macros all return a value of type 'TYPE'.

#if   defined(BSLS_BYTEORDERUTIL_IMPL_CUSTOMSWAP_32)
    BSLS_BYTEORDERUTIL_IMPL_CUSTOMSWAP_32( TYPE, x);
#elif defined(BSLS_BYTEORDERUTIL_IMPL_CUSTOMSWAP_P32)
    BSLS_BYTEORDERUTIL_IMPL_CUSTOMSWAP_P32(TYPE, &x);
#else
    BSLS_BYTEORDERUTIL_IMPL_GENERICSWAP_32(TYPE, x);
#endif
}

template <class TYPE>
inline
TYPE ByteOrderUtil_Impl<TYPE, 8>::swapBytes(TYPE x)
{
    // These macros all return a value of type 'TYPE'.

#if   defined(BSLS_BYTEORDERUTIL_IMPL_CUSTOMSWAP_64)
    BSLS_BYTEORDERUTIL_IMPL_CUSTOMSWAP_64( TYPE, x);
#elif defined(BSLS_BYTEORDERUTIL_IMPL_CUSTOMSWAP_P64)
    BSLS_BYTEORDERUTIL_IMPL_CUSTOMSWAP_P64(TYPE, &x);
#else
    BSLS_BYTEORDERUTIL_IMPL_GENERICSWAP_64(TYPE, x);
#endif
}

}  // close package namespace
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
