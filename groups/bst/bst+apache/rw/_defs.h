// _defs.h                                                            -*-C++-*-
#ifndef INCLUDED__DEFS
#define INCLUDED__DEFS

#ifndef INCLUDED_BDES_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Defintions used to compile Apache tests
//
//@CLASSES:
//
//@SEE_ALSO:
//
//@AUTHOR: Pablo Halpern (phalpern)
//
//@DESCRIPTION: Minimal set of _RW definitions necessary to compile the test
// drivers for non-RW libraries.

#ifndef TEST_RW_EXTENSIONS
   // if non-zero, tests Apache/RW extensions to the library.
#  define  TEST_RW_EXTENSIONS 0
#endif // TEST_RW_EXTENSIONS

#if ! TEST_RW_EXTENSIONS && ! defined(_RWSTD_NO_EXT_NUM_GET)
#  define _RWSTD_NO_EXT_NUM_GET 1
#endif

#if TEST_RW_EXTENSIONS == 0
#define _RWSTD_NO_EXT_KEEP_WIDTH_ON_FAILURE
   // stlport resets the width to 0.
#endif

#ifndef TEST_RW_PEDANTIC
   // If non-zero, checks that algorithms and containers can be instantiated
   // with types having the minimal required interface.  For example, some
   // implementations of the standard library assume that "convertible to
   // bool" is a type that supports the ! and && operators, even though the
   // staandard does not mandate that.  By setting this flag to zero, such
   // libraries have a chance of passing the test suite, even though they are
   // not technically 100% standards-conformant.
#  define  TEST_RW_PEDANTIC 1
#endif // TEST_RW_PEDANTIC

#if ! TEST_RW_EXTENSIONS && ! defined(_RWSTD_NO_EXT_BITSET_CTOR_CHAR_ARRAY)
   // C++03 bitset doesn't have a const char[] constructor.
#  define _RWSTD_NO_EXT_BITSET_CTOR_CHAR_ARRAY 1
#endif

#include <iostream>

#include <rw/_config.h>

// helper macros for token pasting
#define _RWSTD_DO_PASTE(a, b)    a ## b
#define _RWSTD_PASTE(a, b)       _RWSTD_DO_PASTE (a, b)

#define _RWSTD_STR(x)       #x
#define _RWSTD_STRSTR(x)    _RWSTD_STR(x)

#if defined(_WIN64)
#  define _RWSTD_SYS_TYPES_H <basetsd.h>
#elif defined(_WIN32)
#  define _RWSTD_SYS_TYPES_H <rw/_defs.h>
#else
#  define _RWSTD_SYS_TYPES_H <sys/types.h>
#endif   // _WIN32

#  define _rwnamespace_bsl bsl
#  define _rwnamespace_std _RW_STD
#  define _rwnamespace__STD _STD
#  define _rwnamespace___rw __rw
#  define _rwnamespace__RW  _RW

#  define _RWSTD_NAMESPACE(name)    namespace _RWSTD_PASTE(_rwnamespace_,name)
#  define _USING(name)              using name
#  define _RWSTD_EXPORT
#  define _TYPENAME                 typename

#ifndef _EXPLICIT
#  define  _EXPLICIT explicit
#endif

#  define _RW_STD  std
#  undef  _APACHE_STD
#  define _APACHE_STD       std
#  define _RW    __rw
#  define _RWSTD_LONG_LONG long long

#  define _TRY          try
#  undef  _CATCH
#  define _CATCH(ex) catch (ex)
#  define _RETHROW      throw
#  undef  _THROW
#  define _THROW(ex) throw ex

#  undef  _RWSTD_STATIC_CAST
#  define _RWSTD_STATIC_CAST(T, x)      static_cast< T >(x)
#  undef  _RWSTD_REINTERPRET_CAST
#  define _RWSTD_REINTERPRET_CAST(T, x) reinterpret_cast< T >(x)
#  undef  _RWSTD_CONST_CAST
#  define _RWSTD_CONST_CAST(T, x)       const_cast< T >(x)
#  define _RWSTD_DYNAMIC_CAST(T, x)     dynamic_cast< T >(x)

#  define _RWSTD_SPECIALIZED_CLASS template <>
#  define _RWSTD_SPECIALIZED_FUNCTION template <>

#  define _RWSTD_OPERATOR_ARROW(signature) signature { return &**this; }
#  define _THROWS(type_id_list)   throw type_id_list
#  define _LIBC_THROWS(/* empty */)   /* throw () */
#  define _RWSTD_INTERNAL   static

#  define _RWSTD_NO_EXT_BIN_IO
#  define _RWSTD_NO_EXT_REENTRANT_IO
#  define _RWSTD_NO_EXTENSIONS

#ifndef _RWSTD_TMPBUF_SIZE
#  define _RWSTD_TMPBUF_SIZE 4096
#endif

#ifdef __GNUC__
   // putenv() takes a non-const char* argument
#  define _RWSTD_NO_PUTENV_CONST_CHAR
#endif

#  define _RWSTD_ITERATOR_CATEGORY(iterT, ignore) \
         _TYPENAME _RW_STD::iterator_traits< iterT >::iterator_category ()

// to prevent warnings about unused arguments
#define _RWSTD_UNUSED(arg)   ((void)&arg)

#  define _STD_HAS_FACET(type, arg)    _RW_STD::has_facet < type >(arg)
#  define _HAS_FACET(type, arg)        has_facet < type >(arg)

#  define _STD_USE_FACET(type, arg)    _RW_STD::use_facet < type >(arg)
#  define _USE_FACET(type, arg)        use_facet < type >(arg)
#define _RWSTD_USE_FACET(type, arg)   _STD_USE_FACET(type, arg)

#  define _V3_USE_FACET(type, arg)   std::use_facet(arg, (type*)(0))

#  define _GET_TEMP_BUFFER(type, size) get_temporary_buffer < type >(size)

#ifdef _MSC_VER
#  define _RWSTD_DLLIMPORT _CRTIMP
#else
#  define _RWSTD_DLLIMPORT
#endif
#define _RWSTD_PATH_SEP '/'

#if defined (va_copy) || !defined _RWSTD_NO_VA_COPY
   // either va_copy() is already #defined (because <stdarg.h>
   // is already #included), or it was detected at configuration
#  define _RWSTD_VA_COPY(va_dst, va_src) \
          va_copy (va_dst, va_src)
#elif 2 < __GNUG__
   // no va_copy() macro detected, use gcc builtin
#  define _RWSTD_VA_COPY(va_dst, va_src) \
          __builtin_va_copy (va_dst, va_src)
#elif defined (_RWSTD_NO_VA_LIST_ARRAY)
   // va_list is not an array, use ordinary assignment to copy
#  define _RWSTD_VA_COPY(va_dst, va_src) \
          va_dst = va_src
#else   // if defined (_RWSTD_NO_VA_LIST_ARRAY)
   // va_list is an array, use memcpy()
#  define _RWSTD_VA_COPY(va_dst, va_src) \
          memcpy (va_dst, va_src, sizeof (va_list))
#endif   // _RWSTD_NO_VA_LIST_ARRAY

     // C99 specifies the __func__ special identifier
#    define _RWSTD_ASSERT(expr)                                            \
     ((expr) ? (void)0 : _RW::__rw_assert_fail (#expr, __FILE__, __LINE__, \
                                                (const char*) 0))

_RWSTD_NAMESPACE(_RW)
{
    void __rw_assert_fail(const char* expr, const char* file, int line,
                          const char* func);

    inline void* __rw_allocate(unsigned sz, const void* = 0)
    { return ::operator new(sz); }
    inline void __rw_deallocate(void* p, int=0) { ::operator delete(p); }

    inline void __rw_throw(int i, ...) { _THROW(i); }
}

// Define _RWSTD_UWCHAR_INT_T be the largest unsigned integer that fits
// a wchar_t without sign-extension.
#if _RWSTD_WCHAR_MIN == 0
   // wchar_t is unsigned.  Sign-extension is a non-issue.  Just use ulong.
#  define _RWSTD_UWCHAR_INT_T ULong
#elif _RWSTD_WCHAR_MIN == _RWSTD_SHRT_MIN
#  define _RWSTD_UWCHAR_INT_T UShrt
#elif _RWSTD_WCHAR_MIN ==_RWSTD_INT_MIN
#  define _RWSTD_UWCHAR_INT_T UInt
#elif _RWSTD_WCHAR_MIN == _RWSTD_LONG_MIN
#  define _RWSTD_UWCHAR_INT_T ULong
#endif   // if _RWSTD_WCHAR_MIN == 0

#if (defined (__linux__) || defined (__linux)) && !defined (__CYGWIN__)
#  ifndef _RWSTD_LINUX_RELEASE
#    error _RWSTD_LINUX_RELEASE is not set
// #    define _RWSTD_LINUX_RELEASE "generic"
#  endif
#endif

#ifndef BDE_BUILD_TARGET_DBG_SAFE2
#ifndef _RWSTD_NO_DEBUG_ITER
#  define _RWSTD_NO_DEBUG_ITER 1
#endif
#endif

#define _RWSTD_FUNC(name)  _RWSTD_FILE_LINE, name
#define _RWSTD_FILE_LINE  __FILE__ ":" _RWSTD_STRSTR(__LINE__)
#define _RWSTD_REQUIRES(pred, args) (pred) ? (void)0 : _RW::__rw_throw args

#if TEST_RW_EXTENSIONS
// RW allows imbuing locales with const facets.  The standard doesn't
// require that that work.
#  define FACET_CONST const
#else // !TEST_RW_EXTENSIONS
#  define FACET_CONST
#endif // !TEST_RW_EXTENSIONS

// TBD: use real atomic increment?
#define _RWSTD_ATOMIC_PREINCREMENT(i, mutex) (++(i))

#define _RWSTD_NO_EXT_TIME_GET 1

#endif // INCLUDED__DEFS
