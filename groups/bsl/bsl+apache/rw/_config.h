// _config.h                  -*-C++-*-
#ifndef INCLUDED__CONFIG
#define INCLUDED__CONFIG

#ifndef INCLUDED_BDES_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Stub _config.h file for Apache tests
//
//@CLASSES:
//
//@SEE_ALSO:
//
//@AUTHOR: Pablo Halpern (phalpern)
//
//@DESCRIPTION:
//

#ifndef INCLUDED_BSLS_PLATFORM
#include <bsls_platform.h>
#endif

#ifndef INCLUDED_BSLS_NATIVESTD
#include <bsls_nativestd.h>
#endif

#define _RWSTD_NO_IEEEFP_H
#ifdef _MSC_VER
#define _RWSTD_NO_PTHREAD_H
#endif
// #define _RWSTD_NO_NAMESPACE
// #define _RWSTD_NO_ACOS
// #define _RWSTD_NO_ASIN
// #define _RWSTD_NO_ATAN
// #define _RWSTD_NO_ATAN2
// #define _RWSTD_NO_CEIL
// #define _RWSTD_NO_COS
// #define _RWSTD_NO_COSH
// #define _RWSTD_NO_EXP
// #define _RWSTD_NO_FABS
// #define _RWSTD_NO_FLOOR
// #define _RWSTD_NO_FMOD
// #define _RWSTD_NO_FREXP
// #define _RWSTD_NO_LDEXP
// #define _RWSTD_NO_LOG
// #define _RWSTD_NO_LOG10
// #define _RWSTD_NO_MODF
// #define _RWSTD_NO_POW
// #define _RWSTD_NO_SIN
// #define _RWSTD_NO_SINH
// #define _RWSTD_NO_SQRT
// #define _RWSTD_NO_TAN
// #define _RWSTD_NO_TANH
// #define _RWSTD_NO_ACOSF
// #define _RWSTD_NO_ASINF
// #define _RWSTD_NO_ATANF
// #define _RWSTD_NO_ATAN2F
// #define _RWSTD_NO_CEILF
// #define _RWSTD_NO_COSF
// #define _RWSTD_NO_COSHF
// #define _RWSTD_NO_EXPF
// #define _RWSTD_NO_FABSF
// #define _RWSTD_NO_FLOORF
// #define _RWSTD_NO_FMODF
// #define _RWSTD_NO_FREXPF
// #define _RWSTD_NO_LDEXPF
// #define _RWSTD_NO_LOGF
// #define _RWSTD_NO_LOG10F
// #define _RWSTD_NO_MODFF
// #define _RWSTD_NO_POWF
// #define _RWSTD_NO_SINF
// #define _RWSTD_NO_SINHF
// #define _RWSTD_NO_SQRTF
// #define _RWSTD_NO_TANF
// #define _RWSTD_NO_TANHF
// #define _RWSTD_NO_ACOSL
// #define _RWSTD_NO_ASINL
// #define _RWSTD_NO_ATANL
// #define _RWSTD_NO_ATAN2L
// #define _RWSTD_NO_CEILL
// #define _RWSTD_NO_COSL
// #define _RWSTD_NO_COSHL
// #define _RWSTD_NO_EXPL
// #define _RWSTD_NO_FABSL
// #define _RWSTD_NO_FLOORL
// #define _RWSTD_NO_FMODL
// #define _RWSTD_NO_FREXPL
// #define _RWSTD_NO_LDEXPL
// #define _RWSTD_NO_LOGL
// #define _RWSTD_NO_LOG10L
// #define _RWSTD_NO_MODFL
// #define _RWSTD_NO_POWL
// #define _RWSTD_NO_SINL
// #define _RWSTD_NO_SINHL
// #define _RWSTD_NO_SQRTL
// #define _RWSTD_NO_TANL
// #define _RWSTD_NO_TANHL
// #define _RWSTD_NO_CLEARERR
// #define _RWSTD_NO_FCLOSE
// #define _RWSTD_NO_FEOF
// #define _RWSTD_NO_FERROR
// #define _RWSTD_NO_FFLUSH
// #define _RWSTD_NO_FGETC
// #define _RWSTD_NO_FGETPOS
// #define _RWSTD_NO_FGETS
// #define _RWSTD_NO_FOPEN
// #define _RWSTD_NO_FPRINTF
// #define _RWSTD_NO_FPUTC
// #define _RWSTD_NO_FPUTS
// #define _RWSTD_NO_FREAD
// #define _RWSTD_NO_FREOPEN
// #define _RWSTD_NO_FSCANF
// #define _RWSTD_NO_FSEEK
// #define _RWSTD_NO_FSETPOS
// #define _RWSTD_NO_FTELL
// #define _RWSTD_NO_FWRITE
// #define _RWSTD_NO_GETC
// #define _RWSTD_NO_GETCHAR
// #define _RWSTD_NO_GETS
// #define _RWSTD_NO_PERROR
// #define _RWSTD_NO_PRINTF
// #define _RWSTD_NO_PUTC
// #define _RWSTD_NO_PUTCHAR
// #define _RWSTD_NO_PUTS
// #define _RWSTD_NO_REMOVE
// #define _RWSTD_NO_RENAME
// #define _RWSTD_NO_REWIND
// #define _RWSTD_NO_SCANF
// #define _RWSTD_NO_SETBUF
// #define _RWSTD_NO_SETVBUF
// #define _RWSTD_NO_SPRINTF
// #define _RWSTD_NO_SSCANF
// #define _RWSTD_NO_TMPFILE
// #define _RWSTD_NO_TMPNAM
// #define _RWSTD_NO_UNGETC
// #define _RWSTD_NO_VFPRINTF
// #define _RWSTD_NO_VPRINTF
// #define _RWSTD_NO_VSPRINTF
#ifdef _MSC_VER
#define _RWSTD_NO_SNPRINTF
#define _RWSTD_NO_SNPRINTF_IN_LIBC
#endif
// #define _RWSTD_NO_VSNPRINTF
// #define _RWSTD_NO_FILENO
// #define _RWSTD_NO_ABORT
// #define _RWSTD_NO_ABS
// #define _RWSTD_NO_ATEXIT
// #define _RWSTD_NO_ATOF
// #define _RWSTD_NO_ATOI
// #define _RWSTD_NO_ATOL
#ifdef _MSC_VER
#define _RWSTD_NO_ATOLL
#define _RWSTD_NO_ATOLL_IN_LIBC
#endif
// #define _RWSTD_NO_BSEARCH
// #define _RWSTD_NO_CALLOC
#ifdef _MSC_VER
#define _RWSTD_NO_DIV
#endif
// #define _RWSTD_NO_DIV_IN_LIBC
// #define _RWSTD_NO_EXIT
// #define _RWSTD_NO_FREE
// #define _RWSTD_NO_GETENV
// #define _RWSTD_NO_LABS
// #define _RWSTD_NO_LDIV
#ifdef _MSC_VER
#define _RWSTD_NO_LLABS
#define _RWSTD_NO_LLABS_IN_LIBC
#define _RWSTD_NO_LLDIV
#define _RWSTD_NO_LLDIV_IN_LIBC
#endif
// #define _RWSTD_NO_MALLOC
// #define _RWSTD_NO_MBLEN
// #define _RWSTD_NO_MBSTOWCS
// #define _RWSTD_NO_MBTOWC
// #define _RWSTD_NO_QSORT
// #define _RWSTD_NO_RAND
// #define _RWSTD_NO_REALLOC
// #define _RWSTD_NO_SRAND
// #define _RWSTD_NO_STRTOD
// #define _RWSTD_NO_STRTOL
// #define _RWSTD_NO_STRTOUL
// #define _RWSTD_NO_SYSTEM
// #define _RWSTD_NO_WCSTOMBS
// #define _RWSTD_NO_WCTOMB
#ifdef _MSC_VER
#define _RWSTD_NO_STRTOF
#define _RWSTD_NO_STRTOF_IN_LIBC
#define _RWSTD_NO_STRTOLD
#define _RWSTD_NO_STRTOLD_IN_LIBC
#define _RWSTD_NO_STRTOLL
#define _RWSTD_NO_STRTOLL_IN_LIBC
#define _RWSTD_NO_STRTOULL
#define _RWSTD_NO_STRTOULL_IN_LIBC
#define _RWSTD_NO_MKSTEMP
#define _RWSTD_NO_MKSTEMP_IN_LIBC
// #define _RWSTD_NO_PUTENV
#define _RWSTD_NO_SETENV
#define _RWSTD_NO_SETENV_IN_LIBC
#endif
#define _RWSTD_NO_UNSETENV
#define _RWSTD_NO_UNSETENV_IN_LIBC
// #define _RWSTD_NO_MEMCHR
// #define _RWSTD_NO_MEMCMP
// #define _RWSTD_NO_MEMCPY
// #define _RWSTD_NO_MEMMOVE
// #define _RWSTD_NO_MEMSET
// #define _RWSTD_NO_STRCAT
// #define _RWSTD_NO_STRCHR
// #define _RWSTD_NO_STRCMP
// #define _RWSTD_NO_STRCOLL
// #define _RWSTD_NO_STRCPY
// #define _RWSTD_NO_STRCSPN
// #define _RWSTD_NO_STRERROR
// #define _RWSTD_NO_STRLEN
// #define _RWSTD_NO_STRNCAT
// #define _RWSTD_NO_STRNCMP
// #define _RWSTD_NO_STRNCPY
// #define _RWSTD_NO_STRPBRK
// #define _RWSTD_NO_STRRCHR
// #define _RWSTD_NO_STRSPN
// #define _RWSTD_NO_STRSTR
// #define _RWSTD_NO_STRTOK
// #define _RWSTD_NO_STRXFRM
// #define _RWSTD_NO_BTOWC
// #define _RWSTD_NO_FGETWC
// #define _RWSTD_NO_FGETWS
// #define _RWSTD_NO_FPUTWC
// #define _RWSTD_NO_FPUTWS
// #define _RWSTD_NO_FWIDE
// #define _RWSTD_NO_FWPRINTF
// #define _RWSTD_NO_FWSCANF
// #define _RWSTD_NO_GETWC
// #define _RWSTD_NO_GETWCHAR
// #define _RWSTD_NO_MBRLEN
// #define _RWSTD_NO_MBRTOWC
// #define _RWSTD_NO_MBSINIT
// #define _RWSTD_NO_MBSRTOWCS
// #define _RWSTD_NO_PUTWC
// #define _RWSTD_NO_PUTWCHAR
#ifdef _MSC_VER
#define _RWSTD_NO_SWPRINTF
// #define _RWSTD_NO_SWPRINTF_IN_LIBC
// #define _RWSTD_NO_SWSCANF
// #define _RWSTD_NO_UNGETWC
// #define _RWSTD_NO_VFWPRINTF
#define _RWSTD_NO_VSWPRINTF
// #define _RWSTD_NO_VSWPRINTF_IN_LIBC
// #define _RWSTD_NO_VWPRINTF
#define _RWSTD_NO_VWSCANF
#endif
// #define _RWSTD_NO_VWSCANF_IN_LIBC
// #define _RWSTD_NO_WCRTOMB
// #define _RWSTD_NO_WCSCAT
// #define _RWSTD_NO_WCSCHR
// #define _RWSTD_NO_WCSCMP
// #define _RWSTD_NO_WCSCOLL
// #define _RWSTD_NO_WCSCPY
// #define _RWSTD_NO_WCSCSPN
// #define _RWSTD_NO_WCSFTIME
// #define _RWSTD_NO_WCSLEN
// #define _RWSTD_NO_WCSNCAT
// #define _RWSTD_NO_WCSNCMP
// #define _RWSTD_NO_WCSNCPY
// #define _RWSTD_NO_WCSPBRK
// #define _RWSTD_NO_WCSRCHR
// #define _RWSTD_NO_WCSRTOMBS
// #define _RWSTD_NO_WCSSPN
// #define _RWSTD_NO_WCSSTR
// #define _RWSTD_NO_WCSTOD
// #define _RWSTD_NO_WCSTOD
#define _RWSTD_NO_WCSTOF
#define _RWSTD_NO_WCSTOF_IN_LIBC
// #define _RWSTD_NO_WCSTOK
// #define _RWSTD_NO_WCSTOL
#define _RWSTD_NO_WCSTOLD
#define _RWSTD_NO_WCSTOLD_IN_LIBC
#define _RWSTD_NO_WCSTOLL
#define _RWSTD_NO_WCSTOLL_IN_LIBC
// #define _RWSTD_NO_WCSTOUL
#define _RWSTD_NO_WCSTOULL
#define _RWSTD_NO_WCSTOULL_IN_LIBC
// #define _RWSTD_NO_WCSXFRM
// #define _RWSTD_NO_WCTOB
// #define _RWSTD_NO_WMEMCHR
// #define _RWSTD_NO_WMEMCMP
// #define _RWSTD_NO_WMEMCPY
// #define _RWSTD_NO_WMEMMOVE
// #define _RWSTD_NO_WMEMSET
// #define _RWSTD_NO_WPRINTF
// #define _RWSTD_NO_WSCANF
// #define _RWSTD_NO_ISWALPHA
// #define _RWSTD_NO_ISWALNUM
// #define _RWSTD_NO_ISWCNTRL
// #define _RWSTD_NO_ISWDIGIT
// #define _RWSTD_NO_ISWGRAPH
// #define _RWSTD_NO_ISWLOWER
// #define _RWSTD_NO_ISWPRINT
// #define _RWSTD_NO_ISWPUNCT
// #define _RWSTD_NO_ISWSPACE
// #define _RWSTD_NO_ISWUPPER
// #define _RWSTD_NO_ISWXDIGIT
#define _RWSTD_NO_WCTYPE
#define _RWSTD_NO_WCTYPE_IN_LIBC
// #define _RWSTD_NO_ISWCTYPE
// #define _RWSTD_NO_TOWLOWER
// #define _RWSTD_NO_TOWUPPER
#define _RWSTD_NO_WCTRANS
#define _RWSTD_NO_WCTRANS_IN_LIBC
#define _RWSTD_NO_TOWCTRANS
#define _RWSTD_NO_TOWCTRANS_IN_LIBC
// #define _RWSTD_NO_REENTRANT
// #define _RWSTD_NO_ATOMIC_OPS
#define _RWSTD_INTERLOCKED_T volatile long

// #define _RWSTD_NO_BOOL
// #define _RWSTD_NO_CLASS_PARTIAL_SPEC
// #define _RWSTD_NO_COLLAPSE_STATIC_LOCALS
// #define _RWSTD_NO_collapse_static_locals.lib
// #define _RWSTD_NO_COLLAPSE_TEMPLATE_LOCALS
// #define _RWSTD_NO_collapse_template_locals.lib
// #define _RWSTD_NO_COLLAPSE_TEMPLATE_STATICS
// #define _RWSTD_NO_collapse_template_statics.lib
// #define _RWSTD_NO_CONST_CAST
// #define _RWSTD_NO_CTYPE_BITS
#define _RWSTD_CTYPE_UPPER = 0x00000001   /* computed */
#define _RWSTD_CTYPE_LOWER = 0x00000002   /* computed */
#define _RWSTD_CTYPE_DIGIT = 0x00000004   /* computed */
#define _RWSTD_CTYPE_SPACE = 0x00000008   /* computed */
#define _RWSTD_CTYPE_PUNCT = 0x00000010   /* computed */
#define _RWSTD_CTYPE_CNTRL = 0x00000020   /* computed */
#define _RWSTD_CTYPE_BLANK = 0x00000040   /* computed */
#define _RWSTD_CTYPE_XDIGIT = 0x00000080   /* computed */
#define _RWSTD_CTYPE_GRAPH = 0x00000100   /* computed */
#define _RWSTD_CTYPE_ALPHA = 0x00000200   /* computed */
#define _RWSTD_CTYPE_PRINT = 0x00000400   /* computed */
#define _RWSTD_CTYPE_ALNUM = 0x00000800   /* computed */

// #define _RWSTD_NO_DAYLIGHT
// #define _RWSTD_NO_DEFAULT_TEMPLATE_ARGS
#define _RWSTD_NO_DEPRECATED_LIBC_IN_STD
#define _RWSTD_NO_DUMMY_DEFAULT_ARG
// #define _RWSTD_NO_EMPTY_MEM_INITIALIZER
// #define _RWSTD_NO_EXCEPTION_SPECIFICATION_ON_NEW
// #define _RWSTD_NO_EXPLICIT
// #define _RWSTD_NO_EXPLICIT_ARG
// #define _RWSTD_NO_EXPLICIT_CTOR_INSTANTIATION
// #define _RWSTD_NO_EXPLICIT_FUNC_INSTANTIATION
// #define _RWSTD_NO_EXPLICIT_INSTANTIATION
// #define _RWSTD_NO_EXPLICIT_INSTANTIATION_WITH_IMPLICIT_INCLUSION
// #define _RWSTD_NO_EXPLICIT_MEMBER_INSTANTIATION
#define _RWSTD_NO_EXPLICIT_MEMBER_SPECIALIZATION
#define _RWSTD_NO_EXPORT_KEYWORD
// #define _RWSTD_NO_EXTERN_C_COMPATIBILITY
// #define _RWSTD_NO_EXTERN_C_EXCEPTIONS
#define _RWSTD_NO_EXTERN_C_OVERLOAD
// #define _RWSTD_NO_extern_function_template_imp
// #define _RWSTD_NO_EXTERN_INLINE
// #define _RWSTD_NO_extern_inline.lib
// #define _RWSTD_NO_FPOS_T
// #define _RWSTD_NO_FRIEND_TEMPLATE
#define _RWSTD_NO_FUNC
// #define _RWSTD_NO_FUNCTION_TRY_BLOCK
// #define _RWSTD_NO_FUNC_PARTIAL_SPEC
#define _RWSTD_NO_GLOBAL_NOTHROW
#define _RWSTD_NO_GLOBAL_NOTHROW_T
// #define _RWSTD_NO_GLOBAL_SET_NEW_HANDLER
// #define _RWSTD_NO_GLOBAL_SET_TERMINATE
// #define _RWSTD_NO_GLOBAL_SET_UNEXPECTED
// #define _RWSTD_NO_GLOBAL_TERMINATE
// #define _RWSTD_NO_GLOBAL_TYPE_INFO
// #define _RWSTD_NO_GLOBAL_UNEXPECTED
// #define _RWSTD_NO_HONOR_STD
#define _RWSTD_NO_ICONV
#define _RWSTD_NO_ICONV_CONST_CHAR
// #define _RWSTD_NO_IMPLICIT_INSTANTIATION
// #define _RWSTD_NO_INLINE_MEMBER_TEMPLATES
#define _RWSTD_NO_INSTANTIATE_DEFAULT_ARGS
#define _RWSTD_NO_LCONV_INT_FMAT
// #define _RWSTD_NO_LDBL_PRINTF_PREFIX
#define _RWSTD_LDBL_PRINTF_PREFIX "L"

// #define _RWSTD_NO_LIBC_EXCEPTION_SPEC
// #define _RWSTD_NO_lib_exceptions.lib
// #define _RWSTD_NO_LLONG_PRINTF_PREFIX
#define _RWSTD_LLONG_PRINTF_PREFIX "ll"

// #define _RWSTD_NO_LOCALE_NAME_FMAT
#define _RWSTD_LC_COLLATE            1
#define _RWSTD_LC_CTYPE              2
#define _RWSTD_LC_MONETARY           3
#define _RWSTD_LC_NUMERIC            4
#define _RWSTD_LC_TIME               5
#define _RWSTD_LC_ALL                0
#define _RWSTD_LC_MAX               _RWSTD_LC_TIME
#define _RWSTD_LC_MIN               _RWSTD_LC_ALL
#define _RWSTD_CAT_0(pfx) { 1, "LC_COLLATE", pfx::_C_collate }
#define _RWSTD_CAT_1(pfx) { 2, "LC_CTYPE", pfx::_C_ctype }
#define _RWSTD_CAT_2(pfx) { 3, "LC_MONETARY", pfx::_C_monetary }
#define _RWSTD_CAT_3(pfx) { 4, "LC_NUMERIC", pfx::_C_numeric }
#define _RWSTD_CAT_4(pfx) { 5, "LC_TIME", pfx::_C_time }
#define _RWSTD_NO_SETLOCALE_ENVIRONMENT
// #define _RWSTD_NO_CAT_NAMES
#define _RWSTD_CAT_SEP ";"
#define _RWSTD_CAT_EQ "="
#define _RWSTD_NO_INITIAL_CAT_SEP
// #define _RWSTD_NO_CONDENSED_NAME

// #define _RWSTD_NO_LONG_DOUBLE
// #define _RWSTD_NO_LONG_LONG
#define _RWSTD_NO_MADVISE
#define _RWSTD_NO_MATH_EXCEPTION
// #define _RWSTD_NO_MATH_OVERLOADS
// #define _RWSTD_NO_MBSTATE_T
#define _RWSTD_MBSTATE_T_SIZE 4

// #define _RWSTD_NO_MEMBER_TEMPLATES
// #define _RWSTD_NO_MEMBER_TEMPLATE_OVERLOAD
#define _RWSTD_NO_MUNMAP
#ifdef __SUNPRO_CC
#  define _RWSTD_MUNMAP_ARG1_T char*
#else
#  define _RWSTD_MUNMAP_ARG1_T void*
#endif
// #define _RWSTD_NO_NAMESPACE
// #define _RWSTD_NO_NATIVE_WCHAR_T
// #define _RWSTD_NO_NESTED_CLASS_ACCESS
// #define _RWSTD_NO_NEWLINE
#define _RWSTD_NEWLINE "\r\n"
#define _RWSTD_NEWLINE_CR_LF

// #define _RWSTD_NO_NEW_CLASS_TEMPLATE_SYNTAX
// #define _RWSTD_NO_NEW_FUNC_TEMPLATE_SYNTAX
// #define _RWSTD_NO_NEW_HEADER
#define _RWSTD_NO_NL_LANGINFO
#define _RWSTD_NO_NL_TYPES_H
// #define _RWSTD_NO_NONCLASS_ARROW_RETURN
#define _RWSTD_NO_DBL_TRAPS
// #define _RWSTD_NO_FOR_LOCAL_SCOPE
// #define _RWSTD_NO_INT_TRAPS
// #define _RWSTD_NO_object_mangling_imp
// #define _RWSTD_NO_OFFSETOF
// #define _RWSTD_NO_OPERATOR_DELETE_ARRAY
// #define _RWSTD_NO_OPERATOR_NEW_ARRAY
// #define _RWSTD_NO_OVERLOAD_OF_TEMPLATE_FUNCTION
// #define _RWSTD_NO_PART_SPEC_OVERLOAD
// #define _RWSTD_NO_PLACEMENT_DELETE
#define _RWSTD_NO_PRETTY_FUNCTION
// #define _RWSTD_NO_PTR_EXCEPTION_SPEC
#ifdef __SUNPRO_CC
#define _RWSTD_NO_PUTENV_CONST_CHAR
#endif
// #define _RWSTD_NO_QUIET_NAN
// #define _RWSTD_NO_REINTERPRET_CAST
// #define _RWSTD_NO_SETLOCALE
#define _RWSTD_NO_SETRLIMIT
// #define _RWSTD_NO_SIG_ATOMIC_T
#define _RWSTD_SIG_DFL          0
#define _RWSTD_SIG_ERR         -1
#define _RWSTD_SIG_IGN          1
#define _RWSTD_SIGABRT         22
#define _RWSTD_SIGFPE           8
#define _RWSTD_SIGILL           4
#define _RWSTD_SIGINT           2
#define _RWSTD_SIGSEGV         11
#define _RWSTD_SIGTERM         15
#define _RWSTD_SIG_ATOMIC_T   int
#define _RWSTD_SIG_ATOMIC_MIN _RWSTD_INT_MIN
#define _RWSTD_SIG_ATOMIC_MAX _RWSTD_INT_MAX

// #define _RWSTD_NO_SIZE_T
#define _RWSTD_CLOCK_T        long
#define _RWSTD_PTRDIFF_T      int
#define _RWSTD_SIZE_T         std::size_t
#define _RWSTD_SIZE_MAX       _RWSTD_UINT_MAX
#define _RWSTD_PTRDIFF_MIN    _RWSTD_INT_MIN
#define _RWSTD_PTRDIFF_MAX    _RWSTD_INT_MAX
#define _RWSTD_TIME_T         long long
#define _RWSTD_CLOCKS_PER_SEC 1000
#define _RWSTD_RAND_MAX       32767
#define _RWSTD_EOF            -1
#define _RWSTD_WEOF           65535
#define _RWSTD_L_TMPNAM       14
#define _RWSTD_IOFBF          0
#define _RWSTD_IOLBF          64
#define _RWSTD_IONBF          4
#define _RWSTD_BUFSIZ         512
#define _RWSTD_FOPEN_MAX      20
#define _RWSTD_FILENAME_MAX   260
#define _RWSTD_TMP_MAX        32767
#define _RWSTD_FPOS_T         long long

// #define _RWSTD_NO_SPECIALIZATION_ON_RETURN_TYPE
// #define _RWSTD_NO_SPECIALIZED_FRIEND
// #define _RWSTD_NO_STATICS_IN_TEMPLATE
// #define _RWSTD_NO_STATIC_CAST
// #define _RWSTD_NO_STATIC_CONST_MEMBER_INIT
// #define _RWSTD_NO_STATIC_TEMPLATE_MEMBER_INIT
#define _RWSTD_NO_STD_MBSTATE_T
// #define _RWSTD_NO_STD_NOTHROW
// #define _RWSTD_NO_STD_NOTHROW_T
#define _RWSTD_NO_STD_SET_NEW_HANDLER
#define _RWSTD_NO_STD_SET_TERMINATE
#define _RWSTD_NO_STD_SET_UNEXPECTED
#define _RWSTD_NO_STD_TERMINATE
#define _RWSTD_NO_STD_TYPE_INFO
#define _RWSTD_NO_STD_UNEXPECTED
// #define _RWSTD_NO_STRUCT_TM
#define _RWSTD_STRUCT_TM {   /* sizeof (struct tm) == 36          */ \
    int tm_sec;              /* seconds after the minute [O..61]  */ \
    int tm_min;              /* minutes after the hour   [0..59]  */ \
    int tm_hour;             /* hours since midnight     [0..23]  */ \
    int tm_mday;             /* day of the month         [1..31]  */ \
    int tm_mon;              /* months since January     [0..11]  */ \
    int tm_year;             /* years since 1900                  */ \
    int tm_wday;             /* days since Sunday        [0..6]   */ \
    int tm_yday;             /* days since January 1     [0..365] */ \
    int tm_isdst;            /* Daylight Saving Time              */ \
  }

// #define _RWSTD_NO_STRUCT_TM_IN_WCHAR_H
// #define _RWSTD_NO_TEMPLATE_DEFAULT_ARG_CONVERSION
// #define _RWSTD_NO_TEMPLATE_ON_RETURN_TYPE
// #define _RWSTD_NO_THREAD_SAFE_ERRNO
// #define _RWSTD_NO_THREAD_SAFE_EXCEPTIONS
#define _RWSTD_NO_THREAD_SAFE_LOCALE
// #define _RWSTD_NO_TIMEZONE
// #define _RWSTD_NO_TLS
#define _RWSTD_THREAD __declspec (thread)

// #define _RWSTD_NO_TYPENAME
// #define _RWSTD_NO_UNAME
#ifdef _MSC_VER
#define _RWSTD_OS_WINDOWS
#define _RWSTD_OS_SYSNAME "WINDOWS"
#define _RWSTD_OS_MAJOR 5
#define _RWSTD_OS_MINOR 1
#define _RWSTD_OS_MICRO 2600 /* build number */
#define _RWSTD_OS_RELEASE "Windows XP"
#define _RWSTD_OS_WINDOWS_XP
#define _RWSTD_OS_VERSION "Service Pack 2"
#endif

// #define _RWSTD_NO_UNISTD_DECL
#define _RWSTD_STDIN_FILENO                0   // STDIN_FILENO
#define _RWSTD_STDOUT_FILENO             0x1   // STDOUT_FILENO
#define _RWSTD_STDERR_FILENO             0x2   // STDERR_FILENO
#define _RWSTD_O_RDONLY                    0   // O_RDONLY
#define _RWSTD_O_WRONLY                  0x1   // O_WRONLY
#define _RWSTD_O_RDWR                    0x2   // O_RDWR
#define _RWSTD_O_APPEND                  0x8   // O_APPEND
#define _RWSTD_O_EXCL                  0x400   // O_EXCL
#define _RWSTD_O_CREAT                 0x100   // O_CREAT
#define _RWSTD_O_TRUNC                 0x200   // O_TRUNC
#define _RWSTD_SEEK_SET                    0   // SEEK_SET
#define _RWSTD_SEEK_CUR                  0x1   // SEEK_CUR
#define _RWSTD_SEEK_END                  0x2   // SEEK_END
#define _RWSTD_OFF_T long   // off_t
#define _RWSTD_SSIZE_T long   // ssize_t

//#define _RWSTD_NO_VA_LIST

#if defined(BSLS_PLATFORM__CMP_MSVC)
    #define _RWSTD_NO_VA_COPY
#endif

typedef char* __rw_va_list;
#define _RWSTD_NO_VA_LIST_ARRAY   // va_list is object type

#define _RWSTD_VA_LIST __rw_va_list

// #define _RWSTD_NO_VSNPRINTF_RETURN
#define _RWSTD_NO_VSNPRINTF_VFLOW_VALID /* return value negative on overflow */

// #define _RWSTD_NO_WCHAR_T
// #define _RWSTD_NO_WCSFTIME_WCHAR_T_FMAT
#define _RWSTD_WCSFTIME_ARG3_T const wchar_t*

// #define _RWSTD_NO_WCTYPE_T
#define _RWSTD_WCTYPE_T unsigned short

// #define _RWSTD_NO_WINT_T
#if defined(BSLS_PLATFORM__CMP_GNU)
    // On gcc, when unsigned short is passed through '...', it gets promoted to
    // 'int'.  This causes 0.printf to segfault.  Change wint_t to int.
#define _RWSTD_WINT_T   int
#define _RWSTD_WINT_MIN _RWSTD_INT_MIN
#define _RWSTD_WINT_MAX _RWSTD_INT_MAX

#else

#define _RWSTD_WINT_T   unsigned short
#define _RWSTD_WINT_MIN _RWSTD_USHRT_MIN
#define _RWSTD_WINT_MAX _RWSTD_USHRT_MAX

#endif  // defined(BSLS_PLATFORM__CMP_GNU)

// #define _RWSTD_NO_ABS_OVERLOADS
#define _RWSTD_NO_ABS_BOOL
#define _RWSTD_NO_ABS_CHAR
#define _RWSTD_NO_ABS_SCHAR
#define _RWSTD_NO_ABS_UCHAR
#define _RWSTD_NO_ABS_SHRT
#define _RWSTD_NO_ABS_USHRT
// #define _RWSTD_NO_ABS_INT
#define _RWSTD_NO_ABS_UINT
// #define _RWSTD_NO_ABS_LONG
#define _RWSTD_NO_ABS_ULONG
#define _RWSTD_NO_ABS_LLONG
#define _RWSTD_NO_ABS_ULLONG

// #define _RWSTD_NO_CV_VOID_SPECIALIZATIONS
// #define _RWSTD_NO_DEPENDENT_TEMPLATE
// #define _RWSTD_NO_DIV_OVERLOADS
// #define _RWSTD_NO_DIV_INT
// #define _RWSTD_NO_DIV_LONG
#define _RWSTD_NO_DIV_LLONG

// #define _RWSTD_NO_DYNAMIC_CAST
// #define _RWSTD_NO_EXCEPTIONS
#define _RWSTD_NO_EXCEPTION_SPECIFICATION
#define _RWSTD_NO_EXPORT
// #define _RWSTD_NO_EXTERN_FUNCTION_TEMPLATE
// #define _RWSTD_NO_extern_template_imp
// #define _RWSTD_NO_FLOAT
#define _RWSTD_FLT_ROUNDS            1   /* round to nearest */
#define _RWSTD_DBL_DIG              15
#define _RWSTD_DBL_MANT_DIG         53
#define _RWSTD_DBL_MAX_10_EXP      308
#define _RWSTD_DBL_MAX_EXP        1024
#define _RWSTD_DBL_MIN_10_EXP     -307
#define _RWSTD_DBL_MIN_EXP       -1021
#define _RWSTD_FLT_DIG               6
#define _RWSTD_FLT_MANT_DIG         24
#define _RWSTD_FLT_MAX_10_EXP       38
#define _RWSTD_FLT_MAX_EXP         128
#define _RWSTD_FLT_MIN_10_EXP      -37
#define _RWSTD_FLT_MIN_EXP        -125
#define _RWSTD_FLT_RADIX             2
#define _RWSTD_LDBL_DIG             15
#define _RWSTD_LDBL_MANT_DIG        53
#define _RWSTD_LDBL_MAX_10_EXP     308
#define _RWSTD_LDBL_MAX_EXP       1024
#define _RWSTD_LDBL_MIN_10_EXP    -307
#define _RWSTD_LDBL_MIN_EXP      -1021
#define _RWSTD_DBL_MAX          1.7976931348623158e+308
#define _RWSTD_FLT_MAX          3.402823466e+38F
#define _RWSTD_LDBL_MAX         1.7976931348623158e+308
#define _RWSTD_DBL_EPSILON      2.2204460492503131e-016
#define _RWSTD_DBL_MIN          2.2250738585072014e-308
#define _RWSTD_FLT_EPSILON      1.192092896e-07F
#define _RWSTD_FLT_MIN          1.175494351e-38F
#define _RWSTD_LDBL_EPSILON     2.2204460492503131e-016
#define _RWSTD_LDBL_MIN         2.2250738585072014e-308
// #define _RWSTD_NO_STRTOD_UFLOW    // 34 (ERANGE)

#define _RWSTD_NO_GLOBAL_BAD_CAST
#define _RWSTD_NO_GLOBAL_BAD_EXCEPTION
#define _RWSTD_NO_GLOBAL_BAD_TYPEID
#define _RWSTD_NO_GLOBAL_EXCEPTION
#define _RWSTD_NO_GLOBAL_UNCAUGHT_EXCEPTION
#define _RWSTD_NO_IMPLICIT_INCLUSION
// #define _RWSTD_NO_INFINITY
// computed infinities and NANs for a little endian architecture
#undef  _RWSTD_FLT_INF_BITS
#define _RWSTD_FLT_INF_BITS  { 0, 0, '\x80', '\x7f' }
#undef  _RWSTD_FLT_QNAN_BITS
#define _RWSTD_FLT_QNAN_BITS  { 0, 0, '\xc0', '\xff' }
#undef  _RWSTD_FLT_SNAN_BITS
#define _RWSTD_FLT_SNAN_BITS  { '\x1', 0, '\x80', '\x7f' }
#undef  _RWSTD_FLT_DENORM_MIN_BITS
#define _RWSTD_FLT_DENORM_MIN_BITS  { '\x1', 0, 0, 0 }
#define _RWSTD_FLT_HAS_DENORM  1
#undef  _RWSTD_DBL_INF_BITS
#define _RWSTD_DBL_INF_BITS  { 0, 0, 0, 0, 0, 0, '\xf0', '\x7f' }
#undef  _RWSTD_DBL_QNAN_BITS
#define _RWSTD_DBL_QNAN_BITS  { 0, 0, 0, 0, 0, 0, '\xf8', '\xff' }
#undef  _RWSTD_DBL_SNAN_BITS
#define _RWSTD_DBL_SNAN_BITS  { '\x1', 0, 0, 0, 0, 0, '\xf0', '\x7f' }
#undef  _RWSTD_DBL_DENORM_MIN_BITS
#define _RWSTD_DBL_DENORM_MIN_BITS  { '\x1', 0, 0, 0, 0, 0, 0, 0 }
#define _RWSTD_DBL_HAS_DENORM  1
#undef  _RWSTD_LDBL_INF_BITS
#define _RWSTD_LDBL_INF_BITS  { 0, 0, 0, 0, 0, 0, '\xf0', '\x7f' }
#undef  _RWSTD_LDBL_QNAN_BITS
#define _RWSTD_LDBL_QNAN_BITS  { 0, 0, 0, 0, 0, 0, '\xf8', '\xff' }
#undef  _RWSTD_LDBL_SNAN_BITS
#define _RWSTD_LDBL_SNAN_BITS  { '\x1', 0, 0, 0, 0, 0, '\xf0', '\x7f' }
#undef  _RWSTD_LDBL_DENORM_MIN_BITS
#define _RWSTD_LDBL_DENORM_MIN_BITS  { '\x1', 0, 0, 0, 0, 0, 0, 0 }
#define _RWSTD_LDBL_HAS_DENORM  1

// #define _RWSTD_NO_LCONV
#define _RWSTD_LCONV {   /* sizeof (lconv) == 48 */ \
    char* decimal_point;   \
    char* thousands_sep;   \
    char* grouping;   \
    char* int_curr_symbol;   \
    char* currency_symbol;   \
    char* mon_decimal_point;   \
    char* mon_thousands_sep;   \
    char* mon_grouping;   \
    char* positive_sign;   \
    char* negative_sign;   \
    char int_frac_digits;   \
    char frac_digits;   \
    char p_cs_precedes;   \
    char p_sep_by_space;   \
    char n_cs_precedes;   \
    char n_sep_by_space;   \
    char p_sign_posn;   \
    char n_sign_posn;   \
  }

// #define _RWSTD_NO_LIBC_IN_STD
// #define _RWSTD_NO_LIB_EXCEPTIONS
// #define _RWSTD_NO_LIMITS
#define _RWSTD_BOOL_SIZE    1 /* sizeof (bool) */
#define _RWSTD_CHAR_SIZE    1 /* sizeof (char) */
#define _RWSTD_SHRT_SIZE    2 /* sizeof (short) */
#define _RWSTD_INT_SIZE     4 /* sizeof (int) */
#define _RWSTD_LONG_SIZE    4 /* sizeof (long) */
#define _RWSTD_FLT_SIZE     4 /* sizeof (float) */
#define _RWSTD_DBL_SIZE     8 /* sizeof (double) */
#define _RWSTD_LDBL_SIZE    8 /* sizeof (long double) */
#define _RWSTD_PTR_SIZE     4 /* sizeof (void*) */
#define _RWSTD_FUNPTR_SIZE  4 /* sizeof (void(*)()) */
#define _RWSTD_MEMPTR_SIZE  4 /* sizeof (void (struct::*)()) */
#define _RWSTD_CHAR_BIT     8
#define _RWSTD_BOOL_MIN   !!0
#define _RWSTD_BOOL_MAX    !0
// #define _RWSTD_NO_TWOS_COMPLEMENT
#define _RWSTD_CHAR_MAX    '\x7f'
#define _RWSTD_CHAR_MIN    (-_RWSTD_CHAR_MAX - 1)
#define _RWSTD_SCHAR_MAX   0x7f
#define _RWSTD_SCHAR_MIN   (-_RWSTD_SCHAR_MAX - 1)
#define _RWSTD_UCHAR_MAX   0xffU
#define _RWSTD_UCHAR_MIN   0x0U
#define _RWSTD_SHRT_MAX    0x7fff
#define _RWSTD_SHRT_MIN    (-_RWSTD_SHRT_MAX - 1)
#define _RWSTD_USHRT_MAX   0xffffU
#define _RWSTD_USHRT_MIN   0x0U
#define _RWSTD_INT_MAX     0x7fffffff
#define _RWSTD_INT_MIN     (-_RWSTD_INT_MAX - 1)
#define _RWSTD_UINT_MAX    0xffffffffU
#define _RWSTD_UINT_MIN    0x0U
#define _RWSTD_LONG_MAX    0x7fffffffL
#define _RWSTD_LONG_MIN    (-_RWSTD_LONG_MAX - 1L)
#define _RWSTD_ULONG_MAX   0xffffffffUL
#define _RWSTD_ULONG_MIN   0x0UL
#define _RWSTD_LLONG_SIZE   8
#define _RWSTD_LLONG_MAX   0x7fffffffffffffffLL
#undef  _RWSTD_LLONG_MIN
#define _RWSTD_LLONG_MIN   (-_RWSTD_LLONG_MAX - 1LL)
#define _RWSTD_ULLONG_MAX  0xffffffffffffffffULL
#define _RWSTD_ULLONG_MIN  0x0ULL

#if defined(BSLS_PLATFORM__CMP_SUN) || defined (BSLS_PLATFORM__CMP_GNU)
    #define _RWSTD_WCHAR_SIZE  4
#else
    #define _RWSTD_WCHAR_SIZE   2 /* sizeof (wchar_t) */
#endif
#define _RWSTD_WCHAR_MAX   0xffffU
#define _RWSTD_WCHAR_MIN   0x0U
#define _RWSTD_MB_LEN_MAX    5   /* libc value */
#define _RWSTD_INT8_T            signed char
#define _RWSTD_UINT8_T           unsigned char
#define _RWSTD_INT16_T           short
#define _RWSTD_UINT16_T          unsigned short
#define _RWSTD_INT32_T           int
#define _RWSTD_UINT32_T          unsigned int
#define _RWSTD_INT64_T           long long
#define _RWSTD_UINT64_T          unsigned long long
#define _RWSTD_INT_LEAST8_T     _RWSTD_INT8_T
#define _RWSTD_UINT_LEAST8_T    _RWSTD_UINT8_T
#define _RWSTD_INT_LEAST16_T    _RWSTD_INT16_T
#define _RWSTD_UINT_LEAST16_T   _RWSTD_UINT16_T
#define _RWSTD_INT_LEAST32_T    _RWSTD_INT32_T
#define _RWSTD_UINT_LEAST32_T   _RWSTD_UINT32_T
#define _RWSTD_INT_LEAST64_T    _RWSTD_INT64_T
#define _RWSTD_UINT_LEAST64_T   _RWSTD_UINT64_T

// #define _RWSTD_NO_NEW_THROWS
// #define _RWSTD_NO_NEW_OFLOW_SAFE

// #define _RWSTD_NO_NONDEDUCED_CONTEXT
// #define _RWSTD_NO_OBJECT_MANGLING
// #define _RWSTD_NO_SIGNALING_NAN
// #define _RWSTD_NO_OPERATOR_DELETE_ARRAY_NOTHROW
#define _RWSTD_NO_OPERATOR_DELETE_ARRAY_PLACEMENT
// #define _RWSTD_NO_OPERATOR_DELETE_NOTHROW
#define _RWSTD_NO_OPERATOR_DELETE_PLACEMENT
// #define _RWSTD_NO_OPERATOR_NEW_ARRAY_NOTHROW
#define _RWSTD_NO_OPERATOR_NEW_ARRAY_PLACEMENT
// #define _RWSTD_NO_OPERATOR_NEW_NOTHROW
#define _RWSTD_NO_OPERATOR_NEW_PLACEMENT
#define _RWSTD_NO_POD_ZERO_INIT
// #define _RWSTD_NO_STATIC_CONST_MEMBER_EXPR_CONST
#define _RWSTD_NO_STD_BAD_ALLOC
// #define _RWSTD_NO_STD_BAD_CAST
#define _RWSTD_NO_STD_BAD_EXCEPTION
// #define _RWSTD_NO_STD_BAD_TYPEID
// #define _RWSTD_NO_STD_EXCEPTION
#define _RWSTD_NO_STD_UNCAUGHT_EXCEPTION
// #define _RWSTD_NO_WCTRANS_T
#define _RWSTD_WCTRANS_T int

// #define _RWSTD_NO_EXPLICIT_INSTANTIATION_BEFORE_DEFINITION
// #define _RWSTD_NO_EXTERN_MEMBER_TEMPLATE
// #define _RWSTD_NO_EXTERN_TEMPLATE
// #define _RWSTD_NO_extern_template_before_definition_imp
#define _RWSTD_NO_GLOBAL_BAD_ALLOC
// #define _RWSTD_NO_RUNTIME_IN_STD
#undef _RWSTD_NO_GLOBAL_BAD_ALLOC
#undef _RWSTD_NO_GLOBAL_BAD_EXCEPTION
#undef _RWSTD_NO_GLOBAL_UNCAUGHT_EXCEPTION
// number of runtime library symbols examined: 11
// symbols found only in namespace std:         3
// symbols found only in the global namespace:  5
// symbols found in both namespaces:            0
// symbols found in neither namespace:          3
// #define _RWSTD_NO_RUNTIME_IN_STD


// #define _RWSTD_NO_TYPE_INFO_DTOR
// #define _RWSTD_NO_TYPE_INFO_EQUALITY
// #define _RWSTD_NO_TYPE_INFO_INEQUALITY
#define _RWSTD_NO_TYPE_INFO_NAME
#define _RWSTD_NO_BAD_ALLOC_ASSIGNMENT
// #define _RWSTD_NO_BAD_ALLOC_COPY_CTOR
// #define _RWSTD_NO_BAD_ALLOC_DEFAULT_CTOR
// #define _RWSTD_NO_BAD_ALLOC_DTOR
#define _RWSTD_NO_BAD_ALLOC_WHAT
#define _RWSTD_NO_BAD_CAST_ASSIGNMENT
// #define _RWSTD_NO_BAD_CAST_COPY_CTOR
#define _RWSTD_NO_BAD_CAST_DEFAULT_CTOR
// #define _RWSTD_NO_BAD_CAST_DTOR
#define _RWSTD_NO_BAD_CAST_WHAT
#define _RWSTD_NO_BAD_EXCEPTION_ASSIGNMENT
// #define _RWSTD_NO_BAD_EXCEPTION_COPY_CTOR
#define _RWSTD_NO_BAD_EXCEPTION_DEFAULT_CTOR
// #define _RWSTD_NO_BAD_EXCEPTION_DTOR
#define _RWSTD_NO_BAD_EXCEPTION_WHAT
#define _RWSTD_NO_BAD_TYPEID_ASSIGNMENT
// #define _RWSTD_NO_BAD_TYPEID_COPY_CTOR
#define _RWSTD_NO_BAD_TYPEID_DEFAULT_CTOR
// #define _RWSTD_NO_BAD_TYPEID_DTOR
#define _RWSTD_NO_BAD_TYPEID_WHAT
// #define _RWSTD_NO_EXCEPTION_ASSIGNMENT
// #define _RWSTD_NO_EXCEPTION_COPY_CTOR
// #define _RWSTD_NO_EXCEPTION_DEFAULT_CTOR
// #define _RWSTD_NO_EXCEPTION_DTOR
// #define _RWSTD_NO_EXCEPTION_WHAT
// #define _RWSTD_NO_EXTERN_TEMPLATE_BEFORE_DEFINITION
#define _RWSTD_NO_TYPE_INFO_BEFORE


#endif // ! defined(INCLUDED__CONFIG)

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2009
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
