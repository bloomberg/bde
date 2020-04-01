// bsls_libraryfeatures.h                                             -*-C++-*-
#ifndef INCLUDED_BSLS_LIBRARYFEATURES
#define INCLUDED_BSLS_LIBRARYFEATURES

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide macros to identify native standard library features.
//
//@CLASSES:
//
//@MACROS:
//  BSLS_LIBRARYFEATURES_HAS_C90_GETS: C90 'gets' provided
//  BSLS_LIBRARYFEATURES_HAS_C99_FP_CLASSIFY: fpclassify et al. provided in std
//  BSLS_LIBRARYFEATURES_HAS_C99_LIBRARY: C99 library provided
//  BSLS_LIBRARYFEATURES_HAS_C99_SNPRINTF: C99 'snprintf' provided
//  BSLS_LIBRARYFEATURES_HAS_CPP98_AUTO_PTR: 'auto_ptr' provided
//  BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY: C++11 base lib provided
//  BSLS_LIBRARYFEATURES_HAS_CPP11_EXCEPTION_HANDLING: except handling provided
//  BSLS_LIBRARYFEATURES_HAS_CPP11_GARBAGE_COLLECTION_API: GC support provided
//  BSLS_LIBRARYFEATURES_HAS_CPP11_MISCELLANEOUS_UTILITIES: misc utils provided
//  BSLS_LIBRARYFEATURES_HAS_CPP11_PAIR_PIECEWISE_CONSTRUCTOR: use "piecewise"
//  BSLS_LIBRARYFEATURES_HAS_CPP11_PROGRAM_TERMINATION: "program exit" provided
//  BSLS_LIBRARYFEATURES_HAS_CPP11_RANGE_FUNCTIONS: 'begin' and 'end' provided
//  BSLS_LIBRARYFEATURES_HAS_CPP11_STREAM_MOVE: basic_stream move operations
//  BSLS_LIBRARYFEATURES_HAS_CPP11_TUPLE: 'tuple' provided
//  BSLS_LIBRARYFEATURES_HAS_CPP11_UNIQUE_PTR: 'unique_ptr' provided
//  BSLS_LIBRARYFEATURES_HAS_CPP14_BASELINE_LIBRARY: C++14 base lib provided
//  BSLS_LIBRARYFEATURES_HAS_CPP14_INTEGER_SEQUENCE: 'integer_sequence' defined
//  BSLS_LIBRARYFEATURES_HAS_CPP14_RANGE_FUNCTIONS: range functions extension
//  BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY: C++17 base lib provided
//  BSLS_LIBRARYFEATURES_HAS_CPP17_BOOL_CONSTANT: !NOT DEFINED! see below
//  BSLS_LIBRARYFEATURES_HAS_CPP17_EXCEPTION_HANDLING: except handling provided
//  BSLS_LIBRARYFEATURES_HAS_CPP17_PRECISE_BITWIDTH_ATOMICS: optional atomics
//  BSLS_LIBRARYFEATURES_HAS_CPP17_SEARCH_FUNCTORS: searcher function objects
//  BSLS_LIBRARYFEATURES_HAS_CPP17_SEARCH_OVERLOAD: searcher object overload
//  BSLS_LIBRARYFEATURES_HAS_CPP17_DEPRECATED_REMOVED: 'ptr_fun' et al. gone
//  BSLS_LIBRARYFEATURES_STDCPP_GNU: implementation is GNU libstdc++
//  BSLS_LIBRARYFEATURES_STDCPP_IBM: implementation is IBM
//  BSLS_LIBRARYFEATURES_STDCPP_INTELLISENSE: Intellisense is running
//  BSLS_LIBRARYFEATURES_STDCPP_LLVM: implementation is LLVM libc++
//  BSLS_LIBRARYFEATURES_STDCPP_MSVC: implementation is MSVC
//  BSLS_LIBRARYFEATURES_STDCPP_LIBCSTD: implementation is Sun's (RogueWave)
//  BSLS_LIBRARYFEATURES_STDCPP_STLPORT: implementation is STLPort
//  BSLS_LIBRARYFEATURES_SUPPORT_CHARCONV: <charconv>
//  BSLS_LIBRARYFEATURES_SUPPORT_FILESYSTEM: <filesystem>
//  BSLS_LIBRARYFEATURES_SUPPORT_PARALLEL_ALGORITHMS: <execution>
//  BSLS_LIBRARYFEATURES_SUPPORT_PMR: <memory_resource>
//
//@SEE_ALSO: bsls_platform, bsls_compilerfeatures, bsls_nativestd
//
//@DESCRIPTION: This component provides a suite of preprocessor macros that
// indicate the availability of library-specific support of features that may
// not be available in all standard library versions in use across an
// organization.  None of these macros will be defined unless the compiler is
// building with a tool-chain targeting at least experimental support for a
// more recent standard than C++03.  For example, the C++11 standard library
// introduced several functions and types that were not available in libraries
// for earlier standards.  The macro
// 'BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY' can be used to
// conditionally compile BDE code depending on those functions when they are
// available, and exclude that code when those functions are not present,
// thereby allowing compilation by earlier tool-chains.
//
// The standard library version described by these macros always corresponds to
// that used to build this component and is the source of the symbols found in
// the 'native_std' namespace.  Recall that symbols in the 'native_std'
// namespace (alias) always refer to symbols in the native standard library,
// whereas 'std' symbols may or may not be aliases for symbols in the 'bsl'
// namespace, depending on build configuration.
//
///Converse Logic
///--------------
// These macros describe features empirically observed in typical
// platform/compiler/library combinations used in BDE distributions.  The
// definition of any of these macros imply that a resource is available; the
// converse is *not* guaranteed.  If a macro is not defined, the associated
// resource may or may not exist in the library.  Additionally, the resource
// may exist but may be of too early an implementation to be of use to the
// clients of this component.
//
///'BSLS_LIBRARYFEATURES_HAS_C90_GETS'
///-----------------------------------
// The 'BSLS_LIBRARYFEATURES_HAS_C90_GETS' macro is defined if the 'gets'
// function (defined in '<cstdio>') is provided in namespace 'std' by the
// native standard library.  This dangerous function is removed from the C++14
// standard library, and its use with earlier dialects is strongly discouraged.
//
///'BSLS_LIBRARYFEATURES_HAS_C99_FP_CLASSIFY'
///-----------------------------------------
// The 'BSLS_LIBRARYFEATURES_HAS_C99_FP_CLASSIFY' macro is defined if *all* of
// the listed floating-point classification functions, defined in the headers
// named below, are implemented by the native standard library in namespace
// 'std':
//:
//:   o Functions defined in '<cmath>'
//:     o 'fpclassify'
//:     o 'isfinite'
//:     o 'isinf'
//:     o 'isnan'
//:     o 'isnormal'
//:     o 'signbit'
//:     o 'isgreater'
//:     o 'isgreaterequal'
//:     o 'isless'
//:     o 'islessequal'
//:     o 'islessgreater'
//:     o 'isunordered'
//
///'BSLS_LIBRARYFEATURES_HAS_C99_LIBRARY'
///--------------------------------------
// The 'BSLS_LIBRARYFEATURES_HAS_C99_LIBRARY' macro is defined if *all* of the
// listed functions and types, defined in the headers named below, are
// implemented by the native standard library in namespace 'std':
//:
//:   o Functions defined in '<cctype>'
//:     o 'isblank'
//:
//:   o Functions defined in '<cmath>'
//:     o 'double_t'
//:     o 'float_t'
//:     o 'acosh'
//:     o 'asinh'
//:     o 'atanh'
//:     o 'cbrt'
//:     o 'copysign'
//:     o 'erf'
//:     o 'erfc'
//:     o 'exp2'
//:     o 'expm1'
//:     o 'fdim'
//:     o 'fma'
//:     o 'fmax'
//:     o 'fmin'
//:     o 'hypot'
//:     o 'ilogb'
//:     o 'lgamma'
//:     o 'llrint'
//:     o 'log1p'
//:     o 'log2'
//:     o 'logb'
//:     o 'lrint'
//:     o 'lround'
//:     o 'llround'
//:     o 'nan'
//:     o 'nanl'
//:     o 'nanf'
//:     o 'nearbyint'
//:     o 'nextafter'
//:     o 'nexttoward'
//:     o 'remainder'
//:     o 'remquo'
//:     o 'rint'
//:     o 'round'
//:     o 'scalbln'
//:     o 'scalbn'
//:     o 'tgamma'
//:     o 'trunc'
//:
//:   o Functions defined in '<cstdlib>'
//:     o 'atoll'
//:     o 'llabs'
//:     o 'lldiv'
//:     o 'lldiv_t'
//:     o 'strtof'
//:     o 'strtold'
//:     o 'strtoll'
//:     o 'strtoull'
//:
//:   o Functions defined in '<cstdio>'
//:     o 'vfscanf'
//:     o 'vsscanf'
//:     o 'vsnprintf'
//:     o 'vsscanf'
//:
//:   o Functions defined in '<cwchar>'
//:     o 'vfwscanf'
//:     o 'vswscanf'
//:     o 'vwscanf'
//:     o 'wcstof'
//:     o 'wcstold'
//:     o 'wcstoll'
//:     o 'wcstoull'
//:
//:   o Functions defined in '<cwctype>'
//:     o 'iswblank'
//
// Currently the following compilers will have this macro defined by default:
//
//:   o GCC 4.8
//:   o Clang 3.0 (using at least GCC 4.8 STL)
//:   o MSVC 2013
//
// Notice that the above list does *not* include 'snprintf' as that is included
// in the separate 'BSLS_LIBRARYFEATURES_HAS_C99_SNPRINTF' macro.  Also note
// that AIX and Solaris compilers generally have implementations of these
// functions in the global namespace when including the corresponding C header,
// but do not provide the standard C++ mapping.
//
///'BSLS_LIBRARYFEATURES_HAS_C99_SNPRINTF'
///---------------------------------------
// The 'BSLS_LIBRARYFEATURES_HAS_C99_SNPRINTF' macro is defined if 'snprintf'
// function (defined in '<cstdio>') is implemented by the native standard
// library in namespace 'std'.
//
//:   o Function defined in '<cstdio>'
//:     o 'snprintf'
//
// Currently the following compilers will have this macro defined by default:
//
//:   o GCC 4.8
//:   o Clang 3.0 (using at least GCC 4.8 STL)
//:   o MSVC 2015
//
///'BSLS_LIBRARYFEATURES_HAS_CPP98_AUTO_PTR'
///-----------------------------------------
// The 'BSLS_LIBRARYFEATURES_HAS_CPP98_AUTO_PTR' macro is defined if the
// 'auto_ptr' class template (defined in '<memory>') is provided by the native
// standard library.  This macro is expected to be defined for all
// libaries/platforms at least until the introduction of C++17 to our build
// systems.
//
///'BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY'
///-------------------------------------------------
// The 'BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY' macro is used to
// identify whether the current platform's standard library supports a baseline
// set of C++11 library features (which are defined below).  This is especially
// important in BSL when importing standard library functions and types into
// the 'bsl::' namespace, as is done in 'bsl+bslhdrs'.  In particular this
// macro is used where C++11 has introduced new functions and types into
// headers available in older versions of the C++ standard.  For example, C++11
// introduced the type 'move_iterator' to the existing '<iterator>' header, so
// 'bsl_iterator.h' uses this macro to determine whether to import
// 'std::move_iterator' into the 'bsl' namespace.
//
// This macro is defined if *both* of the listed conditions are true:
//
//: o The compiler supports C++11 language features.
//:
//: o The following functions and types are provided by the native standard
//:   library:
//:
//:   o Functions defined in '<algorithm>'
//:     o 'all_of'
//:     o 'any_of'
//:     o 'copy_if'
//:     o 'copy_n'
//:     o 'find_if_not'
//:     o 'is_heap'
//:     o 'is_heap_until'
//:     o 'is_partitioned'
//:     o 'is_permutation'
//:     o 'is_sorted'
//:     o 'is_sorted_until'
//:     o 'minmax'
//:     o 'minmax_element'
//:     o 'move'
//:     o 'move_backward'
//:     o 'none_of'
//:     o 'partition_copy'
//:     o 'partition_point'
//:     o 'shuffle'
//:
//:   o Types defined in '<atomic>'
//:     o atomic class template and specializations for integral types
//:
//:   o Functions defined in '<complex>'
//:     o 'acosh'
//:     o 'asinh'
//:     o 'atanh'
//:     o 'proj'
//:
//:   o Functions and types defined in '<exception>'
//:     o 'current_exception'
//:     o 'exception_ptr'
//:     o 'rethrow_exception'
//:
//:   o Functions and types defined in '<functional>'
//:     o 'bind'
//:     o 'bit_and'
//:     o 'bit_or'
//:     o 'bit_xor'
//:     o 'is_bind_expression'
//:     o 'is_placeholder'
//:     o 'mem_fn'
//:
//:   o Functions defined in '<iomanip>'
//:     o 'get_money'
//:     o 'put_money'
//:
//:   o Functions and types defined in '<ios>'
//:     o 'io_errc'
//:     o 'iostream_category'
//:     o 'is_error_code_enum'
//:     o 'make_error_code'
//:     o 'make_error_condition'
//:     o 'hexfloat'
//:     o 'defaultfloat'
//
//:   o Functions defined in '<iterator>'
//:     o 'begin'
//:     o 'end'
//:     o 'move_iterator'
//:     o 'make_move_iterator'
//:     o 'next'
//:     o 'prev'
//:
//:   o Function defined in '<locale>'
//:     o 'isblank'
//:
//:   o Functions defined in '<memory>'
//:     o 'addressof'
//:     o 'pointer_traits'
//:     o 'uninitialized_copy_n'
//:
//:   o Function defined in '<numeric>'
//:     o 'iota'
//:
//:   o Member function defined in '<typeinfo>'
//:     o 'type_info::hash_code'
//:
//:   o Functions defined in '<utility>'
//:     o 'declval'
//:     o 'forward'
//:     o 'move'
//:     o 'move_if_noexcept'
//:     o 'swap'
//:
//: o The following headers can be included:
//:
//:     o <array>
//:     o <atomic>
//:     o <chrono>
//:     o <codecvt>
//:     o <condition_variable>
//:     o <forward_list>
//:     o <future>
//:     o <mutex>
//:     o <random>
//:     o <regex>
//:     o <scoped_allocator>
//:     o <system_error>
//:     o <thread>
//:     o <typeindex>
//:     o <unordered_map>
//:     o <unordered_set>
//
// Currently the following compilers will have this macro defined by default:
//
//:   o GCC 4.8
//:   o Clang 3.0 (using at least GCC 4.8 STL)
//:   o MSVC 2013
//
// Notice that the above list does *not* include 'random_shuffle' as that is
// deprecated in C++14 and will be removed in C++17.
//
///'BSLS_LIBRARYFEATURES_HAS_CPP14_BASELINE_LIBRARY'
///----------------------------------------------------
// The 'BSLS_LIBRARYFEATURES_HAS_CPP14_BASELINE_LIBRARY' macro is used to
// identify whether the current platform's standard library supports a baseline
// set of C++14 library features (which are defined below).  This is especially
// important in BSL when importing standard library functions and types into
// the 'bsl::' namespace, as is done in 'bsl+bslhdrs'.
//
// This macro is defined if *all* of the listed conditions are true:
//
//: o The compiler supports C++14 language features.
//:
//: o The following functions and types are provided by the native standard
//:   library:
//:
//:   o UDLs support for <'chrono>'
//:
//:   o UDLs support for '<complex>'
//:
//:   o Type defined in '<functional>'
//:     o bit_not
//:
//:   o Function defined in '<iterator>'
//:     o make_reverse_iterator
//:
//:   o Function defined in '<iomanip>'
//:     o quoted
//:
//:   o Function defined in '<memory>'
//:     o 'make_unique'
//:
//:   o Function defined in '<type_traits>'
//:     o 'is_null_pointer'
//:     o 'is_final'
//:
//:   o Function defined in '<utility>'
//:     o 'exchange'
//:
//: o The header <shared_mutex> is available providing the standard components.
//
// Currently the following compilers will have this macro defined by default:
//
//:   o gcc 4.9
//:   o clang 3.4
//:   o MSVC 2015
//
///'BSLS_LIBRARYFEATURES_HAS_CPP14_INTEGER_SEQUENCE'
///-------------------------------------------------
// The 'BSLS_LIBRARYFEATURES_HAS_CPP14_INTEGER_SEQUENCE' macro is defined if
// *all* of the listed conditions are true:
//
//: o The 'BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY' macro is defined.
//:
//: o The 'BSLS_COMPILERFEATURES_SUPPORT_VARIADIC_TEMPLATES' macro is also
//:   defined.
//
//: o The 'BSLS_COMPILERFEATURES_SUPPORT_ALIAS_TEMPLATES' macro is also
//:   defined.
//
// Note that though 'struct' 'integer_sequence' was introduced in the C++14
// standard, 'bsl::integer_sequence' is supported by BDE when compiling in
// C++11 mode.
//
// Currently the following compilers will have this macro defined by default:
//
//:   o GCC 4.8
//:   o Clang 3.0 (using at least GCC 4.8 STL)
//:   o MSVC 2015
//
///'BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY'
///----------------------------------------------------
// This macro is used to identify whether the current platform's standard
// library supports a baseline set of C++17 library features (which are defined
// below).  This is especially important in BSL when importing standard library
// functions and types into the 'bsl::' namespace, as is done in 'bsl+bslhdrs'.
//
// This macro is defined if *both* of the listed conditions are true:
//
//: o The compiler supports C++17 language features.
//:
//: o The following functions and types are provided by the native standard
//:   library in <type_traits>:
//:
//:   o 'conjunction'
//:   o 'disjunction'
//:   o 'negation'
//
// Currently the following compilers will have this macro defined by default:
//
//:   o gcc 7, gcc 8, gcc 9, clang 7, clang 8
//
///'BSLS_LIBRARYFEATURES_HAS_CPP11_EXCEPTION_HANDLING'
///---------------------------------------------------
// The 'BSLS_LIBRARYFEATURES_HAS_CPP11_EXCEPTION_HANDLING' macro is defined if
// *both* of the listed conditions are true:
//
//: o The 'BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY' macro is defined.
//:
//: o The following functions and types are provided by the native standard
//:   library in '<exception>':
//:
//:   o 'exception_ptr'
//:   o 'make_exception_ptr'
//:   o 'nested_exception'
//:   o 'rethrow_if_nested'
//:   o 'throw_with_nested'
//
// Currently the following compilers will have this macro defined by default:
//
//:   o GCC 4.8
//:   o Clang 3.0 (using at least GCC 4.8 STL)
//:   o MSVC 2015
//
///'BSLS_LIBRARYFEATURES_HAS_CPP11_GARBAGE_COLLECTION_API'
///-------------------------------------------------------
// The 'BSLS_LIBRARYFEATURES_HAS_CPP11_GARBAGE_COLLECTION_API' macro is defined
// if *both* of the listed conditions are true:
//
//: o The 'BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY' macro is defined.
//:
//: o The following functions and types are provided by the native standard
//:   library in '<memory>':
//:
//:   o 'declare_no_pointers'
//:   o 'declare_reachable'
//:   o 'get_pointer_safety'
//:   o 'pointer_safety'
//:   o 'undeclare_no_pointers'
//:   o 'undeclare_reachable'
//
// Currently the following compilers will have this macro defined by default:
//
//:   o GCC 6.0
//:   o Clang 3.0 (using at least GCC 6.0 STL)
//:   o MSVC 2015
//
///'BSLS_LIBRARYFEATURES_HAS_CPP11_MISCELLANEOUS_UTILITIES'
///--------------------------------------------------------
// The 'BSLS_LIBRARYFEATURES_HAS_CPP11_MISCELLANEOUS_UTILITIES' macro is
// defined if *both* of the listed conditions are true:
//
//: o The 'BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY' macro is defined.
//:
//: o The following functions and types are provided by the native standard
//:   library:
//:
//:   o Type defined in '<cstddef>'
//:     o 'max_align_t'
//:
//:   o Functions defined in '<exception>'
//:     o 'get_terminate'
//:     o 'get_unexpected'
//:
//:   o Functions defined in '<iomanip>'
//:     o 'get_time'
//:     o 'put_time'
//:
//:   o Functions defined in '<locale>'
//:     o 'isblank'
//:     o 'wstring_convert'
//:     o 'wbuffer_convert'
//:
//:   o Function defined in '<memory>'
//:     o 'align'
//:
//:   o Class and function defined in '<new>'
//:     o 'bad_array_new_length'
//:     o 'get_new_handler'
//
// Currently the following compilers will have this macro defined by default:
//
//:   o GCC 5.0
//:   o Clang 3.0 (using at least GCC 5.0 STL)
//:   o MSVC 2013
//
// Notice that the function 'get_unexpected' will be removed in C++17.
//
///'BSLS_LIBRARYFEATURES_HAS_CPP11_PAIR_PIECEWISE_CONSTRUCTOR'
///-----------------------------------------------------------
// The 'BSLS_LIBRARYFEATURES_HAS_CPP11_PAIR_PIECEWISE_CONSTRUCTOR' macro is
// defined if the 'pair' class template (defined in '<utility>') provided by
// the native library has a constructor with the signature:
//..
//   template <class... Args1, class... Args2>
//   pair(piecewise_construct_t ,
//        tuple<Args1...>       first_args,
//        tuple<Args2...>       second_args);
//..
// Notice that the existence of the above constructor implies that each of the
// following conditions are also true:
//
//: o The 'piecewise_construct_t' (tag) type (defined in '<utility>') is
//:   provided by the native standard library.
//:
//: o The 'BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY' macro is defined.
//:
//: o The 'BSLS_COMPILERFEATURES_SUPPORT_VARIADIC_TEMPLATES' macro is defined.
//:
//: o The 'BSLS_LIBRARYFEATURES_HAS_CPP11_TUPLE' macro is defined.
//
// Currently the following compilers will have this macro defined by default:
//
//:   o GCC 4.8
//:   o Clang 3.0 (using at least GCC 4.8 STL)
//:   o MSVC 2015
//
///'BSLS_LIBRARYFEATURES_HAS_CPP17_PRECISE_BITWIDTH_ATOMICS'
///---------------------------------------------------------
// The 'BSLS_LIBRARYFEATURES_HAS_CPP17_PRECISE_BITWIDTH_ATOMICS' macro is
// defined if *both* of the listed conditions are true: defined if the
// '<atomic>' header provided by the native standard library provides type
// aliases for all of the following precise bit-width atomic types:
//
//: o The 'BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY' macro is defined.
//:
//: o The following type aliases are provided by the native standard library in
//:   the '<atomic>' header:
//:   o 'atomic_int8_t'
//:   o 'atomic_int16_t'
//:   o 'atomic_int32_t'
//:   o 'atomic_int64_t'
//:   o 'atomic_uint8_t'
//:   o 'atomic_uint16_t'
//:   o 'atomic_uint32_t'
//:   o 'atomic_uint64_t'
//:   o 'atomic_intptr_t'
//:   o 'atomic_uintptr_t'
//
// Currently the following compilers will have this macro defined by default:
//
//:   o GCC 7.0
//:   o Clang 3.0 (using at least GCC 7.0 STL)
//:   o MSVC 2013
//
///'BSLS_LIBRARYFEATURES_HAS_CPP17_SEARCH_FUNCTORS'
///------------------------------------------------
// The 'BSLS_LIBRARYFEATURES_HAS_CPP17_SEARCH_FUNCTORS' macro is defined if the
// native library supplies the following class templates in '<functional>':
//
//: o 'default_searcher'
//: o 'boyer_moore_searcher'
//: o 'boyer_moore_horspool_searcher'
//
// Currently the following compilers define this function template in C++17 or
// later modes:
//
//:   o GCC 8.3.0
//:   o MSVC 2019
//:   o clang 3.0 with GNU library
//
///'BSLS_LIBRARYFEATURES_HAS_CPP17_SEARCH_OVERLOAD'
///------------------------------------------------
// The 'BSLS_LIBRARYFEATURES_HAS_CPP17_SEARCH_OVERLOAD' macro is defined if
// '<algorithm>' defines an overload for the 'search' function template that
// accepts instances of the searcher classes introduced in C++17.  See
// [alg.search]:
//..
//  template<class ForwardIterator, class Searcher>
//  constexpr ForwardIterator std::search(ForwardIterator first,
//                                        ForwardIterator last,
//                                        const Searcher& searcher);
//..
//
// Currently the following compilers define this function template in C++17 or
// later modes:
//
//:   o GCC 8.3.0
//:   o MSVC 2017+
//:   o clang above 3.0 with GNU library
//:   0 Apple clang all supported versions
//
///'BSLS_LIBRARYFEATURES_HAS_CPP17_DEPRECATED_REMOVED'
///---------------------------------------------------
// The 'BSLS_LIBRARYFEATURES_HAS_CPP17_DEPRECATED_REMOVED' macro is defined for
// libraries that do not export names removed in C++17, such as 'std::ptr_fun'.
// 'BSLS_LIBRARYFEATURES_HAS_CPP17_DEPRECATED_REMOVED' is generally the
// negation of 'BSLS_LIBRARYFEATURES_HAS_CPP98_AUTO_PTR'.  Although the removal
// of deprecated C++17 types is conceptually equivalent to
// '__cplusplus >= 201703L', standard library implementations often provide
// configuration flags to expose the deprecated library features.
//
///'BSLS_LIBRARYFEATURES_HAS_CPP11_PROGRAM_TERMINATION'
///----------------------------------------------------
// The 'BSLS_LIBRARYFEATURES_HAS_CPP11_PROGRAM_TERMINATION' macro is defined if
// of the listed conditions are true:
//
//: o The 'BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY' macro is defined.
//:
//: o The following functions are provided by the native standard library in
//:   the '<cstdlib>' header:
//:
//: o '_Exit'
//: o 'quick_exit'
//: o 'at_quick_exit'
//
// Currently the following compilers will have this macro defined by default:
//
//:   o GCC 4.8
//:   o Clang 3.0 (using at least GCC 4.8 STL)
//:   o MSVC 2015
//
///'BSLS_LIBRARYFEATURES_HAS_CPP11_RANGE_FUNCTIONS'
///------------------------------------------------
// The 'BSLS_LIBRARYFEATURES_HAS_CPP11_RANGE_FUNCTIONS' is defined if the
// following functions are provided by the native standard library in the
// '<iterator>' header:
//
//..
//  template <class C> auto begin(C& c) -> decltype(c.begin());
//  template <class C> auto begin(const C& c) -> decltype(c.begin());
//  template <class C> auto end(C& c) -> decltype(c.end());
//  template <class C> auto end(const C& c) -> decltype(c.end());
//  template <class T, size_t N> T* begin(T (&array)[N]);
//  template <class T, size_t N> T* end(T (&array)[N]);
//..
//
// Currently the following compilers will have this macro defined by default:
//
//:   o GCC 4.6
//:   o Clang 3.0
//:   o MSVC 2012
//
///'BSLS_LIBRARYFEATURES_HAS_CPP14_RANGE_FUNCTIONS'
///------------------------------------------------
// The 'BSLS_LIBRARYFEATURES_HAS_CPP14_RANGE_FUNCTIONS' macro is defined if the
// following functions are provided by the native standard library in the
// '<iterator>' header:
//
//..
//  template <class C> auto cbegin(const C& c) -> decltype(std::begin(c));
//  template <class C> auto cend(const C& c) -> decltype(std::end(c));
//  template <class C> auto rbegin(C& c) -> decltype(c.rbegin());
//  template <class C> auto rbegin(const C& c) -> decltype(c.rbegin());
//  template <class C> auto rend(C& c) -> decltype(c.rend());
//  template <class C> auto rend(const C& c) -> decltype(c.rend());
//  template <class T, size_t N> reverse_iterator<T*> rbegin(T (&array)[N]);
//  template <class T, size_t N> reverse_iterator<T*> rend(T (&array)[N]);
//  template <class E> reverse_iterator<const E*> rbegin(
//                                                    initializer_list<E> il);
//  template <class E> reverse_iterator<const E*> rend(initializer_list<E> il);
//  template <class C> auto crbegin(const C& c) -> decltype(std::rbegin(c));
//  template <class C> auto crend(const C& c) -> decltype(std::rend(c));
//..
//
// Currently the following compilers will have this macro defined by default:
//
//:   o GCC 4.9
//:   o Clang 3.4
//:   o MSVC 2013
//
///'BSLS_LIBRARYFEATURES_HAS_CPP11_STREAM_MOVE'
///--------------------------------------------
// The 'BSLS_LIBRARYFEATURES_HAS_CPP11_STREAM_MOVE' is defined if the standard
// stream classes ('basic_istream', 'basic_ostream', 'basic_stream') all have
// the C++11 mandated move operations (constructor and assignment operator)
// defined.  Notice that the this macro is separate from the macro
// 'BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY' as standard library
// implementations exist that implement almost all of C++11 (which is detected
// by the 'BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY' macro) but not this
// feature.  Note that when 'BSLS_LIBRARYFEATURES_HAS_CPP11_STREAM_MOVE' is
// defined 'BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES' will also be
// defined, as the standard feature cannot be implemented without rvalue
// reference support from the compiler.
//
//: o Supported by the compiler vendor's STL implementation
//:   o gcc 5.0
//:   o clang 3.0
//:   o MSVC 2013
//
///'BSLS_LIBRARYFEATURES_HAS_CPP11_TUPLE'
///--------------------------------------
// The 'BSLS_LIBRARYFEATURES_HAS_CPP11_TUPLE' macro is defined if *both* of the
// listed conditions are true:
//
//: o The 'BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY' macro is defined.
//:
//: o The 'tuple' type template (defined in '<tuple>') is provided by the
//:   native standard library.
//:
//: o The 'BSLS_COMPILERFEATURES_SUPPORT_VARIADIC_TEMPLATES' macro is also
//:   defined.
//
// Currently the following compilers will have this macro defined by default:
//
//:   o GCC 4.8
//:   o Clang 3.0 (using at least GCC 4.8 STL)
//:   o MSVC 2015
//
///'BSLS_LIBRARYFEATURES_HAS_CPP11_UNIQUE_PTR'
///-------------------------------------------
// The 'BSLS_LIBRARYFEATURES_HAS_CPP11_UNIQUE_PTR' macro is defined if *both*
// of the listed conditions are true:
//
//: o The 'BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY' macro is defined.
//:
//: o The 'unique_ptr' class template (defined in '<memory>') is provided by
//:   the native standard library.
//:
//: o The 'BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES' macro is also
//:   defined.
//
// Currently the following compilers will have this macro defined by default:
//
//:   o GCC 4.8
//:   o Clang 3.0 (using at least GCC 4.8 STL)
//:   o MSVC 2013
//
///'BSLS_LIBRARYFEATURES_HAS_CPP17_BOOL_CONSTANT': !NOT DEFINED!
///-------------------------------------------------------------
// The 'BSLS_LIBRARYFEATURES_HAS_CPP17_BOOL_CONSTANT' macro is never defined
// and is mentioned in this component as documentation until its appearance in
// several BDE test drivers is removed.
//
// The 'BSLS_LIBRARYFEATURES_HAS_CPP17_BOOL_CONSTANT' macro was intended to
// indicate whether or not the 'bool_constant' alias template (defined in
// '<type_traits>') is provided by the native standard library.  It has since
// been discovered that, as 'bsl::integral_constant' is actually a distinct
// class template derived from 'native_std::integral_constant', there must
// always be a distinct 'bsl' alias template, which will be provided in a
// future BDE release.  Given this new direction, the need for a macro for the
// native type is obviated so 'BSLS_LIBRARYFEATURES_HAS_CPP17_BOOL_CONSTANT' is
// being removed.
//
///'BSLS_LIBRARYFEATURES_STDCPP_GNU_LIBSTDCPP'
///-------------------------------------------
// The 'BSLS_LIBRARYFEATURES_STDCPP_GNU' macro is defined if the C++ standard
// library implementation is GNU libstdc++.
//
///'BSLS_LIBRARYFEATURES_STDCPP_IBM'
///---------------------------------
// The 'BSLS_LIBRARYFEATURES_STDCPP_IBM' macro is defined if the C++ standard
// library implementation is IBM's VACPP.
//
/// 'BSLS_LIBRARYFEATURES_STDCPP_INTELLISENSE'
///-------------------------------------------
// The 'BSLS_LIBRARYFEATURES_STDCPP_INTELLISENSE' macro is defined if
// Microsoft's Intellisense is running (instead of a production compiler).
// Note that detecting Intellisense does *not* mean that the MSVC compiler is
// used.  Visual Studio 2017 is capable of cross platform development using
// clang and Visual Studio Code (also featuring Intellisense) may be used with
// virtually any compiler.
//
///'BSLS_LIBRARYFEATURES_STDCPP_LLVM'
///----------------------------------
// The 'BSLS_LIBRARYFEATURES_STDCPP_LLVM_LIBCPP' macro is defined if the C++
// standard library implementation is LLVM's libc++.
//
///'BSLS_LIBRARYFEATURES_STDCPP_MSVC'
///----------------------------------
// The 'BSLS_LIBRARYFEATURES_STDCPP_MSVC' macro is defined if the C++ standard
// library implementation is Microsoft's (Dinkumware based) implementation.
//
///'BSLS_LIBRARYFEATURES_STDCPP_LIBCSTD'
///-------------------------------------
// The 'BSLS_LIBRARYFEATURES_STDCPP_LIBCSTD' macro is defined if the C++
// standard library implementation is a RogueWave implementation.
//
///'BSLS_LIBRARYFEATURES_STDCPP_STLPORT'
///-------------------------------------
// The 'BSLS_LIBRARYFEATURES_STDCPP_STLPORT' macro is defined if the C++
// standard library implementation is an STLPort implementation.
//
///Usage
///-----
// In this section we show intended usage of this component.
//
///Example 1: Managing Library-Dependent Interfaces
/// - - - - - - - - - - - - - - - - - - - - - - - -
// When building software across multiple platforms a programmer may have to
// deal with different versions of the native standard library, some providing
// features that the others do not.  The macros defined in this component can
// be used make features visible only if the required native standard library
// features are present.
//
// For example, the 'tuple'-type is not available in older versions of the
// native standard library.  Suppose we have a utility component that returns
// an instance of a 'tuple' of values *if* the underlying version of the
// standard library provides that type, and yet remain compilable otherwise.
//
// First, we conditionally include the header file we will need if we define an
// interface that returns a 'native_std::tuple'.
//..
//  #if defined(BSLS_LIBRARYFEATURES_HAS_CPP11_TUPLE)
//  # ifndef INCLUDED_TUPLE
//  # include <tuple>
//  # define INCLUDED_TUPLE
//  # endif
//  #endif // BSLS_LIBRARYFEATURES_HAS_CPP11_TUPLE
//..
// Then, we declare the methods that will be unconditionally provided by our
// utility component:
//..
//  struct MyStatisticalUtil
//  {
//      static double     mean(const int *begin, const int *end);
//      static int      median(const int *begin, const int *end);
//      static double variance(const int *begin, const int *end);
//          // Return the median (mean, variance) of the sequence of values in
//          // the specified non-empty, semi-open range '[begin, end)'.  The
//          // behavior is undefined unless 'begin < end'.
//..
// Now, we conditionally define an interface that returns a 'bsl::type', if
// that type is available.  Note that, if all three values are needed, calling
// this interface is more efficient than calling the earlier three individually
// because the input need be traversed one time, not three.
//..
//  #ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_TUPLE
//      static native_std::tuple<int, double, double> getMedianMeanVariance(
//                                                            const int *begin,
//                                                            const int *end);
//          // Return the median, mean, and variance (in that order) of the
//          // sequence of values in the specified non-empty, semi-open range
//          // '[begin, end)'.  The behavior is undefined unless 'begin < end'.
//
//  #endif // BSLS_LIBRARYFEATURES_HAS_CPP11_TUPLE
//
//  };
//..
// Finally, we find that our code compiles when we build our code against
// versions of the native standard library that provide a 'tuple' type, *and*
// those that do not.  Of course, in the later case the interface that returns

#include <bsls_platform.h>
#include <bsls_compilerfeatures.h>

// ============================================================================
//                     STANDARD LIBRARY DETECTION
// ----------------------------------------------------------------------------

#define BSLS_LIBRARYFEATURES_DETECTION_IN_PROGRESS
// We are including a *standard* header to get the macros defined that allow us
// to determine what standard library implementation we have.  Do *not* change
// these to any other header.  These headers were selected because they do not
// include (m)any other standard headers *and* they do not define any dangerous
// (non-uppercase) macros.  '<ciso646>' would define dangerous macros on
// Microsoft platform, so we use the standard header '<cstddef>' to get to the
// platform-determining macros.  '<ciso646>' is effectively empty on older GNU
// implementations, so we also include '<cstddef>'.  Note that we cannot
// include non-standard headers if they define anything with 'std' in it,
// because this header may be included while '#define std bsl' is active.  If
// we include standard headers, our "intercept" headers (see 'bos+stdhdrs'
// package in the 'bos' package group) will ensure the proper inclusion.

#ifdef BSLS_COMPILERFEATURES_SUPPORT_HAS_INCLUDE
// On platforms that support it we use '__has_include' to give a better error
// message if/when our assumptions about the platforms and its supported header
// files break.

// First we determine if we have clang that uses the GNU library.  We need this
// info because for the GNU library we need to include another header, not
// '<ciso646>', because '<ciso646>' does not define the required macros prior
// to version 6.1.  See http://en.cppreference.com/w/cpp/header/ciso646

# if defined(BSLS_PLATFORM_CMP_CLANG)
#   if __has_include(<bits/c++config.h>)
#     define BSLS_LIBRARYFEATURES_SUSPECT_CLANG_WITH_GLIBCPP  1
#   endif
# endif

# ifdef BSLS_PLATFORM_CMP_MSVC
#   if !__has_include(<yvals.h>)
#     error Standard library implementation detection is broken.  The        \
            implementation specific header file <yvals.h> we assumed exists  \
            is not found.  'bsls_libraryfeatures.h' needs to be updated.
#   endif

# elif defined(BSLS_PLATFORM_CMP_GNU) || \
       defined(BSLS_LIBRARYFEATURES_SUSPECT_CLANG_WITH_GLIBCPP)
#   if !__has_include(<bits/c++config.h>)
#     error Standard library implementation detection is broken.  The        \
            implementation specific header file <bits/c++config.h> we assumed\
            exists is not found.  'bsls_libraryfeatures.h' needs to be updated.
#   endif

# elif defined(BSLS_PLATFORM_CMP_SUN)
#   if !__has_include(<valarray>)
#     error Standard library implementation detection is broken.  The        \
            standard header file <valarray> we assumed exists is not found.  \
            'bsls_libraryfeatures.h' needs to be updated.
#   endif

# else
#   if !__has_include(<ciso646>)
#     error Standard library implementation detection is broken.  The        \
            standard header file <ciso646> we assumed exists is not found.   \
            'bsls_libraryfeatures.h' needs to be updated.
#   endif
# endif

#endif  // '__has_include' is supported

// Include a header from the standard library that defines the macros that we
// use for detection.  Note that once C++20 is out (and we know what the value
// of the __cplusplus macro for that is) we can add to this code the use of the
// newly standardized <version> header.  See
// http://en.cppreference.com/w/cpp/header/version

#ifdef BSLS_PLATFORM_CMP_MSVC
# include <cstddef>
#elif defined(BSLS_PLATFORM_CMP_GNU) || \
      defined(BSLS_LIBRARYFEATURES_SUSPECT_CLANG_WITH_GLIBCPP)
# include <cstddef>
# if defined(BSLS_LIBRARYFEATURES_SUSPECT_CLANG_WITH_GLIBCPP)
  // No macro pollution
#   undef BSLS_LIBRARYFEATURES_SUSPECT_CLANG_WITH_GLIBCPP
# endif
#elif defined(BSLS_PLATFORM_CMP_SUN)
// Unfortunately, SUN does *nothing* in its '<ciso646>', its '<exception>'.
// Its '<new>', and '<typeinfo>' headers are shared between the RogueWave and
// the STLPort implementations, so they do not define anything standard library
// specific (see
// https://docs.oracle.com/cd/E19205-01/819-5267/6n7c46e4p/index.html 12.7.1),
// therefore we have to include something that is different between RogueWave
// and STLPort implementations.  All such headers are large and not cheap.  :(

# include <valarray>
#else
# include <ciso646>
#endif
#undef BSLS_LIBRARYFEATURES_DETECTION_IN_PROGRESS

// The implementation detection logic itself.  Note that on IBM we just detect
// the compiler, because the IBM library does not define a specific macro.

#if defined (__GLIBCPP__) || defined(__GLIBCXX__)
# define BSLS_LIBRARYFEATURES_STDCPP_GNU                              1
#elif defined(_CPPLIB_VER) || (defined(_YVALS) && !defined(__IBMCPP__))
# define BSLS_LIBRARYFEATURES_STDCPP_MSVC                             1
#elif defined(_LIBCPP_VERSION)
# define BSLS_LIBRARYFEATURES_STDCPP_LLVM                             1
#elif defined(__STD_RWCOMPILER_H__) || defined(_RWSTD_VER)
# define BSLS_LIBRARYFEATURES_STDCPP_LIBCSTD                          1
#elif defined(__SGI_STL_PORT) || defined(_STLPORT_VERSION)
# define BSLS_LIBRARYFEATURES_STDCPP_STLPORT                          1
#elif defined(__IBMCPP__)
# define BSLS_LIBRARYFEATURES_STDCPP_IBM                              1
#elif defined(__INTELLISENSE__)
# define BSLS_LIBRARYFEATURES_STDCPP_INTELLISENSE                     1
#else
# error Could not determine standard library platform.
#endif


// ============================================================================
//                     UNIVERSAL FEATURE SUPPORT
// ----------------------------------------------------------------------------

// Unconditionally define macros for C++98 features that may be detected as
// absent from later standards.  All removed libraries will have their macros
// undefined in a consistent manner at the end of this header

#define BSLS_LIBRARYFEATURES_HAS_C90_GETS                             1
    // Set unconditionally for compilers supporting an earlier standard than
    // C++14; this feature macro will be undefined for those platforms with
    // partial support for C++14, implementing the removal of this dangerous
    // function.

#define BSLS_LIBRARYFEATURES_HAS_CPP98_AUTO_PTR                       1
    // Set unconditionally.  This features is found on each compiler
    // version/platform combination tested.  Assume universally available until
    // the day tool chains start removing this deprecated class template.

#if BSLS_COMPILERFEATURES_CPLUSPLUS >= 201703L
#define BSLS_LIBRARYFEATURES_HAS_CPP17_DEPRECATED_REMOVED             1
    // Set when C++17 is detected.  Adjusted below for implementations that
    // keep deprecated functions available.
#endif

// ============================================================================
//                     PLATFORM SPECIFIC FEATURE DETECTION
// ----------------------------------------------------------------------------

#if defined(BSLS_PLATFORM_CMP_GNU)
    #define BSLS_LIBRARYFEATURES_HAS_C99_FP_CLASSIFY                  1
    #if (__cplusplus >= 201103L) ||                                           \
           (defined(__GXX_EXPERIMENTAL_CXX0X__) &&                            \
            BSLS_PLATFORM_CMP_VERSION >= 40800)
        // C99 functions are available in C++11 builds.
        #define BSLS_LIBRARYFEATURES_HAS_C99_LIBRARY                  1
    #endif
    #if (__cplusplus >= 201103L) ||                                           \
           (defined(__GXX_EXPERIMENTAL_CXX0X__) &&                            \
            BSLS_PLATFORM_CMP_VERSION >= 40800) ||                            \
            (defined(_GLIBCXX_USE_C99) && _GLIBCXX_USE_C99 == 1)
        // snprintf is also available in C++03 builds with new gcc versions

        #define BSLS_LIBRARYFEATURES_HAS_C99_SNPRINTF                 1
    #endif
    #if defined(__GXX_EXPERIMENTAL_CXX0X__) && (__cplusplus >= 201103L)
        #if BSLS_PLATFORM_CMP_VERSION >= 40600
            #define BSLS_LIBRARYFEATURES_HAS_CPP11_RANGE_FUNCTIONS    1
        #endif

        #if BSLS_PLATFORM_CMP_VERSION >= 40800
            #define BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY   1
            #define BSLS_LIBRARYFEATURES_HAS_CPP11_EXCEPTION_HANDLING 1
            #define BSLS_LIBRARYFEATURES_HAS_CPP11_PAIR_PIECEWISE_CONSTRUCTOR 1
            #define BSLS_LIBRARYFEATURES_HAS_CPP11_TUPLE              1
            #define BSLS_LIBRARYFEATURES_HAS_CPP11_UNIQUE_PTR         1
            #if defined(_GLIBCXX_HAVE_AT_QUICK_EXIT) && \
                defined(_GLIBCXX_HAVE_QUICK_EXIT)
                // This feature depends on GLIBC exposing support by defining
                // the above two pre-processor definitions.
                #define BSLS_LIBRARYFEATURES_HAS_CPP11_PROGRAM_TERMINATION    1
            #endif
        #endif

        #if BSLS_PLATFORM_CMP_VERSION >= 50000
            #define BSLS_LIBRARYFEATURES_HAS_CPP11_MISCELLANEOUS_UTILITIES    1
            #define BSLS_LIBRARYFEATURES_HAS_CPP11_STREAM_MOVE        1
        #endif

        #if BSLS_PLATFORM_CMP_VERSION >= 60000
            #define BSLS_LIBRARYFEATURES_HAS_CPP11_GARBAGE_COLLECTION_API     1
        #endif
    #endif
    #if __cplusplus > 201103L
        #define BSLS_LIBRARYFEATURES_HAS_CPP14_BASELINE_LIBRARY       1
        #define BSLS_LIBRARYFEATURES_HAS_CPP14_RANGE_FUNCTIONS        1
    #endif
    #if __cplusplus > 201402L  // > C++14
        #if BSLS_PLATFORM_CMP_VERSION >= 70301
            #define BSLS_LIBRARYFEATURES_HAS_CPP17_SEARCH_OVERLOAD            1
            #define BSLS_LIBRARYFEATURES_HAS_CPP17_SEARCH_FUNCTORS            1
        #endif
    #endif
    #if __cplusplus >= 201703L
        #if BSLS_PLATFORM_CMP_VERSION >= 70000
            #define BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY   1
        #endif
        #if BSLS_PLATFORM_CMP_VERSION >= 90000
            #define BSLS_LIBRARYFEATURES_SUPPORT_CHARCONV             1
            #define BSLS_LIBRARYFEATURES_SUPPORT_FILESYSTEM           1
            #define BSLS_LIBRARYFEATURES_SUPPORT_PMR                  1
        #endif
        //  #define BSLS_LIBRARYFEATURES_SUPPORT_PARALLEL_ALGORITHMS  1
    #endif
    #if defined(__cpp_lib_atomic_is_always_lock_free)
        // There is no pre-processor define declared in libstdc++ to indicate
        // that precise bitwidth atomics exist, but the libstdc++ shipping with
        // GCC 7 also includes lock-free support in C++17 mode.  That feature
        // does include a pre-processor definition, so use it as a stand-in for
        // detecting precise bitwidth atomics.  This pre-processor definition
        // will already only be defined when compiling in at least C++17
        // standard mode, so there is no need for an additional check.
        #define BSLS_LIBRARYFEATURES_HAS_CPP17_PRECISE_BITWIDTH_ATOMICS       1
    #endif
    #if _GLIBCXX_USE_DEPRECATED
        # undef BSLS_LIBRARYFEATURES_HAS_CPP17_DEPRECATED_REMOVED
    #endif
#endif

#if defined(BSLS_PLATFORM_CMP_IBM)
    // #define BSLS_LIBRARYFEATURES_HAS_C99_FP_CLASSIFY
    // #define BSLS_LIBRARYFEATURES_HAS_C99_LIBRARY
    // #define BSLS_LIBRARYFEATURES_HAS_C99_SNPRINTF
    // #define BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY
    // #define BSLS_LIBRARYFEATURES_HAS_CPP11_EXCEPTION_HANDLING
    // #define BSLS_LIBRARYFEATURES_HAS_CPP11_GARBAGE_COLLECTION_API
    // #define BSLS_LIBRARYFEATURES_HAS_CPP11_MISCELLANEOUS_UTILITIES
    // #define BSLS_LIBRARYFEATURES_HAS_CPP11_PAIR_PIECEWISE_CONSTRUCTOR
    // #define BSLS_LIBRARYFEATURES_HAS_CPP11_PROGRAM_TERMINATION
    // #define BSLS_LIBRARYFEATURES_HAS_CPP11_RANGE_FUNCTIONS
    // #define BSLS_LIBRARYFEATURES_HAS_CPP11_STREAM_MOVE
    // #define BSLS_LIBRARYFEATURES_HAS_CPP11_TUPLE
    // #define BSLS_LIBRARYFEATURES_HAS_CPP11_UNIQUE_PTR
    // #define BSLS_LIBRARYFEATURES_HAS_CPP14_BASELINE_LIBRARY
    // #define BSLS_LIBRARYFEATURES_HAS_CPP14_RANGE_FUNCTIONS
    // #define BSLS_LIBRARYFEATURES_HAS_CPP17_PRECISE_BITWIDTH_ATOMICS
    // #define BSLS_LIBRARYFEATURES_HAS_CPP17_SEARCH_OVERLOAD
    // #define BSLS_LIBRARYFEATURES_HAS_CPP17_SEARCH_FUNCTORS
#endif

#if defined(BSLS_PLATFORM_CMP_SUN)
    #if __cplusplus >= 201103L
        // It would be simpler if we could simply identify as the corresping
        // gcc library version: CC CMP_VERSION libstdc++ version
        // 12.4     0x5130          4.8.4
        // 12.5     0x5140          5.1.0
        // 12.6     0x5150          5.4.0
        #if BSLS_PLATFORM_CMP_VERSION >= 0x5130
            #define BSLS_LIBRARYFEATURES_HAS_C99_FP_CLASSIFY          1
            #define BSLS_LIBRARYFEATURES_HAS_C99_LIBRARY              1
            #define BSLS_LIBRARYFEATURES_HAS_C99_SNPRINTF             1
            #define BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY   1
            #define BSLS_LIBRARYFEATURES_HAS_CPP11_EXCEPTION_HANDLING 1
            #define BSLS_LIBRARYFEATURES_HAS_CPP11_PAIR_PIECEWISE_CONSTRUCTOR 1
            #define BSLS_LIBRARYFEATURES_HAS_CPP11_RANGE_FUNCTIONS    1
            #define BSLS_LIBRARYFEATURES_HAS_CPP11_STREAM_MOVE        1
            #define BSLS_LIBRARYFEATURES_HAS_CPP11_TUPLE              1
            #define BSLS_LIBRARYFEATURES_HAS_CPP11_UNIQUE_PTR         1
        #endif

        #if BSLS_PLATFORM_CMP_VERSION >= 0x5140
             #define BSLS_LIBRARYFEATURES_HAS_CPP11_MISCELLANEOUS_UTILITIES   1
        #endif

        #if BSLS_PLATFORM_CMP_VERSION >= 0x5150
            // Currently have an issue with rvalue-references on the CC 12.6
            // (beta) compiler, so undefining library feature macros that rely
            // on that language feature being marked as available
            #undef BSLS_LIBRARYFEATURES_HAS_CPP11_PAIR_PIECEWISE_CONSTRUCTOR
            #undef BSLS_LIBRARYFEATURES_HAS_CPP11_TUPLE
            #undef BSLS_LIBRARYFEATURES_HAS_CPP11_UNIQUE_PTR
        #endif
    #endif
    #if __cplusplus > 201103L
        #if BSLS_PLATFORM_CMP_VERSION >= 0x5140
            #define BSLS_LIBRARYFEATURES_HAS_CPP14_BASELINE_LIBRARY   1
            #define BSLS_LIBRARYFEATURES_HAS_CPP14_RANGE_FUNCTIONS    1
        #endif
    #endif

    // #define BSLS_LIBRARYFEATURES_HAS_CPP11_GARBAGE_COLLECTION_API
    // #define BSLS_LIBRARYFEATURES_HAS_CPP11_PROGRAM_TERMINATION
    // #define BSLS_LIBRARYFEATURES_HAS_CPP17_PRECISE_BITWIDTH_ATOMICS
    // #define BSLS_LIBRARYFEATURES_HAS_CPP17_SEARCH_OVERLOAD
    // #define BSLS_LIBRARYFEATURES_HAS_CPP17_SEARCH_FUNCTORS
#endif

#if defined(BSLS_LIBRARYFEATURES_STDCPP_STLPORT)
    // #define BSLS_LIBRARYFEATURES_HAS_C99_FP_CLASSIFY
    // #define BSLS_LIBRARYFEATURES_HAS_C99_LIBRARY
    // #define BSLS_LIBRARYFEATURES_HAS_C99_SNPRINTF
    // #define BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY
    // #define BSLS_LIBRARYFEATURES_HAS_CPP11_EXCEPTION_HANDLING
    // #define BSLS_LIBRARYFEATURES_HAS_CPP11_GARBAGE_COLLECTION_API
    // #define BSLS_LIBRARYFEATURES_HAS_CPP11_MISCELLANEOUS_UTILITIES
    // #define BSLS_LIBRARYFEATURES_HAS_CPP11_PAIR_PIECEWISE_CONSTRUCTOR
    // #define BSLS_LIBRARYFEATURES_HAS_CPP11_PROGRAM_TERMINATION
    // #define BSLS_LIBRARYFEATURES_HAS_CPP11_STREAM_MOVE
    // #define BSLS_LIBRARYFEATURES_HAS_CPP11_TUPLE
    // #define BSLS_LIBRARYFEATURES_HAS_CPP11_UNIQUE_PTR
    // #define BSLS_LIBRARYFEATURES_HAS_CPP14_BASELINE_LIBRARY
    // #define BSLS_LIBRARYFEATURES_HAS_CPP17_PRECISE_BITWIDTH_ATOMICS
    // #define BSLS_LIBRARYFEATURES_HAS_CPP17_SEARCH_OVERLOAD
    // #define BSLS_LIBRARYFEATURES_HAS_CPP17_SEARCH_FUNCTORS
#endif

#if defined(BSLS_PLATFORM_CMP_CLANG)

    #define BSLS_LIBRARYFEATURES_HAS_C99_FP_CLASSIFY

    #if defined(__APPLE_CC__) && (__APPLE_CC__ >= 6000)

        #define BSLS_LIBRARYFEATURES_HAS_CPP11_RANGE_FUNCTIONS        1
            // libc++ provides this C++11 feature as a C++98 extension.

        #if defined(__GXX_EXPERIMENTAL_CXX0X__) || (_LIBCPP_STD_VER >= 11)
            #define BSLS_LIBRARYFEATURES_HAS_C99_LIBRARY              1
            #define BSLS_LIBRARYFEATURES_HAS_C99_SNPRINTF             1
        #endif
        #if (defined(__GXX_EXPERIMENTAL_CXX0X__)  || (_LIBCPP_STD_VER >= 11)) \
          && (__cplusplus >= 201103L)
            #define BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY   1
            #define BSLS_LIBRARYFEATURES_HAS_CPP11_EXCEPTION_HANDLING 1
            #define BSLS_LIBRARYFEATURES_HAS_CPP11_PAIR_PIECEWISE_CONSTRUCTOR 1
            #define BSLS_LIBRARYFEATURES_HAS_CPP11_TUPLE              1
            #define BSLS_LIBRARYFEATURES_HAS_CPP11_UNIQUE_PTR         1
        #endif

        #if __cplusplus > 201103L
            #define BSLS_LIBRARYFEATURES_HAS_CPP14_BASELINE_LIBRARY   1
        #endif

        #if __cplusplus >= 201402L
            #define BSLS_LIBRARYFEATURES_HAS_CPP14_RANGE_FUNCTIONS    1
        #endif
        #if __cplusplus >= 201703L
            #define BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY   1

            #define BSLS_LIBRARYFEATURES_HAS_CPP17_SEARCH_OVERLOAD    1
            //  #define BSLS_LIBRARYFEATURES_HAS_CPP17_SEARCH_FUNCTORS    1

            //  #define BSLS_LIBRARYFEATURES_SUPPORT_CHARCONV             1
            //  #define BSLS_LIBRARYFEATURES_SUPPORT_FILESYSTEM           1
            //  #define BSLS_LIBRARYFEATURES_SUPPORT_PARALLEL_ALGORITHMS  1
            //  #define BSLS_LIBRARYFEATURES_SUPPORT_PMR                  1
        #endif

    #elif BSLS_PLATFORM_CMP_VERSION >= 30000

        #if defined(__GXX_EXPERIMENTAL_CXX0X__)
            #define BSLS_LIBRARYFEATURES_HAS_C99_LIBRARY              1
            #define BSLS_LIBRARYFEATURES_HAS_C99_SNPRINTF             1
        #endif
        #if defined(__GXX_EXPERIMENTAL_CXX0X__) && (__cplusplus >= 201103L)
            #define BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY   1
            #define BSLS_LIBRARYFEATURES_HAS_CPP11_RANGE_FUNCTIONS    1
            #define BSLS_LIBRARYFEATURES_HAS_CPP11_EXCEPTION_HANDLING 1
            #define BSLS_LIBRARYFEATURES_HAS_CPP11_PAIR_PIECEWISE_CONSTRUCTOR 1
            #define BSLS_LIBRARYFEATURES_HAS_CPP11_STREAM_MOVE        1
            #define BSLS_LIBRARYFEATURES_HAS_CPP11_TUPLE              1
            #define BSLS_LIBRARYFEATURES_HAS_CPP11_UNIQUE_PTR         1

            #if defined(_GLIBCXX_HAVE_AT_QUICK_EXIT) && \
                defined(_GLIBCXX_HAVE_QUICK_EXIT)
                // Assume a minimum of a GCC 4.8 toolchain and check for GLIBC
                // support for the feature.
                #define BSLS_LIBRARYFEATURES_HAS_CPP11_PROGRAM_TERMINATION    1
            #endif
        #endif

        #if defined(__cpp_lib_atomic_is_always_lock_free)
        // There is no pre-processor define declared in libstdc++ to indicate
        // that precise bitwidth atomics exist, but the libstdc++ shipping with
        // GCC 7 also includes lock-free support in C++17 mode.  That feature
        // does include a pre-processor definition, so use it as a stand-in for
        // detecting precise bitwidth atomics.  This pre-processor definition
        // will already only be defined when compiling in at least C++17
        // standard mode, so there is no need for an additional check.
            #define BSLS_LIBRARYFEATURES_HAS_CPP17_PRECISE_BITWIDTH_ATOMICS   1
        #endif

        #if __cplusplus > 201103L
            #if BSLS_PLATFORM_CMP_VERSION >= 30400
                #define BSLS_LIBRARYFEATURES_HAS_CPP14_BASELINE_LIBRARY   1
                #define BSLS_LIBRARYFEATURES_HAS_CPP14_RANGE_FUNCTIONS    1
            #endif
        #endif
        #if __cplusplus >= 201703L
            #define BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY       1
            #if defined(BSLS_LIBRARYFEATURES_STDCPP_GNU)
                #define BSLS_LIBRARYFEATURES_HAS_CPP17_SEARCH_OVERLOAD    1
                #define BSLS_LIBRARYFEATURES_HAS_CPP17_SEARCH_FUNCTORS    1
            #elif defined(BSLS_LIBRARYFEATURES_STDCPP_LLVM)
                #define BSLS_LIBRARYFEATURES_HAS_CPP17_SEARCH_OVERLOAD    1
            #else
                #error Unsupported standard library for g++
            #endif

            //  #define BSLS_LIBRARYFEATURES_SUPPORT_CHARCONV             1
            //  #define BSLS_LIBRARYFEATURES_SUPPORT_FILESYSTEM           1
            //  #define BSLS_LIBRARYFEATURES_SUPPORT_PARALLEL_ALGORITHMS  1
            //  #define BSLS_LIBRARYFEATURES_SUPPORT_PMR                  1
        #endif
    #endif

    // Do not define these until there is a pre-processor definition given by
    // the build system to indicate at least the major version number of the
    // GCC toolchain used by Clang.
    //
    // #define BSLS_LIBRARYFEATURES_HAS_CPP11_MISCELLANEOUS_UTILITIES
    // #define BSLS_LIBRARYFEATURES_HAS_CPP11_GARBAGE_COLLECTION_API
#endif

#if defined(BSLS_PLATFORM_CMP_MSVC)

    // Notice that BDE does not support C99 and C++11 library features prior to
    // Microsoft C++ compiler version 1800 (MSVC 2013)

    // We assume at least Visual Studio 2013
    #define BSLS_LIBRARYFEATURES_HAS_C99_FP_CLASSIFY                  1
    #define BSLS_LIBRARYFEATURES_HAS_C99_LIBRARY                      1
    #define BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY           1
    #define BSLS_LIBRARYFEATURES_HAS_CPP11_STREAM_MOVE                1
    #define BSLS_LIBRARYFEATURES_HAS_CPP11_MISCELLANEOUS_UTILITIES    1
    #define BSLS_LIBRARYFEATURES_HAS_CPP11_RANGE_FUNCTIONS            1
    #define BSLS_LIBRARYFEATURES_HAS_CPP14_RANGE_FUNCTIONS            1
    #define BSLS_LIBRARYFEATURES_HAS_CPP11_UNIQUE_PTR                 1
    #define BSLS_LIBRARYFEATURES_HAS_CPP17_PRECISE_BITWIDTH_ATOMICS   1
        // Note that 'cbegin()'/'cend()', 'rbegin()'/'rend()', and
        // 'crbegin()'/crend()' non-member functions are implemented in the
        // library for MSVC 2013 (version 1800), so the macro
        // 'BSLS_LIBRARYFEATURES_HAS_CPP14_RANGE_FUNCTIONS' is defined in this
        // section.  See https://msdn.microsoft.com/en-us/library/hh567368.aspx

    #if BSLS_PLATFORM_CMP_VERSION >= 1900  // Visual Studio 2015

        #define BSLS_LIBRARYFEATURES_HAS_C99_SNPRINTF                 1
        #define BSLS_LIBRARYFEATURES_HAS_CPP11_EXCEPTION_HANDLING     1
        #define BSLS_LIBRARYFEATURES_HAS_CPP11_GARBAGE_COLLECTION_API 1
        #define BSLS_LIBRARYFEATURES_HAS_CPP11_PAIR_PIECEWISE_CONSTRUCTOR 1
        #define BSLS_LIBRARYFEATURES_HAS_CPP11_PROGRAM_TERMINATION    1
        #define BSLS_LIBRARYFEATURES_HAS_CPP11_TUPLE                  1
            // Note that earlier verions have 'tuple' but this macro also
            // requires the definition of the
            // 'BSLS_COMPILER_FEATURES_HAS_VARIADIC_TEMPLATES' macro.
        #define BSLS_LIBRARYFEATURES_HAS_CPP14_BASELINE_LIBRARY       1
        #define BSLS_LIBRARYFEATURES_HAS_CPP17_BOOL_CONSTANT          1
            // Early access to C++17 feature!

        #undef BSLS_LIBRARYFEATURES_HAS_C90_GETS
    #endif

    // The search algorithm with functor has appeared Visual Studio 2017, but
    // the searchers (functors) have appeared in Visual Studio 2019.
    #if BSLS_COMPILERFEATURES_CPLUSPLUS >= 201402L
        #if BSLS_PLATFORM_CMP_VERSION >= 1910  // Visual Studio 2017
            #define BSLS_LIBRARYFEATURES_HAS_CPP17_SEARCH_OVERLOAD    1
        #endif
    #endif
    #if BSLS_COMPILERFEATURES_CPLUSPLUS > 201402L
        #if BSLS_PLATFORM_CMP_VERSION >= 1920  // Visual Studio 2019
            #define BSLS_LIBRARYFEATURES_HAS_CPP17_SEARCH_FUNCTORS    1
        #endif
    #endif

    // If _HAS_AUTO_PTR_ETC is defined, use its value as the deciding one for
    // whether the C++17 deprecated names are gone.
    #if defined _HAS_AUTO_PTR_ETC
        #if _HAS_AUTO_PTR_ETC
          # undef BSLS_LIBRARYFEATURES_HAS_CPP17_DEPRECATED_REMOVED
        #else
          #define BSLS_LIBRARYFEATURES_HAS_CPP17_DEPRECATED_REMOVED   1
        #endif
    #endif
#endif

// ============================================================================
//                     POST-DETECTION FIXUPS
// ----------------------------------------------------------------------------

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY)  &&             \
    defined(BSLS_COMPILERFEATURES_SUPPORT_VARIADIC_TEMPLATES) &&             \
    defined(BSLS_COMPILERFEATURES_SUPPORT_ALIAS_TEMPLATES)

    #define BSLS_LIBRARYFEATURES_HAS_CPP14_INTEGER_SEQUENCE           1
#endif


// Now, after detecting support, unconditionally undefine macros for features
// that have been removed from later standards.

#if BSLS_COMPILERFEATURES_CPLUSPLUS > 201103L
# undef BSLS_LIBRARYFEATURES_HAS_C90_GETS
    // 'gets' is removed immediately from C++14, so undefine for any standard
    // version identifier greater than that of C++11.
#endif

#if defined BSLS_LIBRARYFEATURES_HAS_CPP17_DEPRECATED_REMOVED
# undef BSLS_LIBRARYFEATURES_HAS_CPP98_AUTO_PTR
    // 'auto_ptr' is removed from C++17, so undefine for any standard version
    // identifier greater than that of C++14.
#endif

#endif // INCLUDED_BSLS_LIBRARYFEATURES

// ----------------------------------------------------------------------------
// Copyright 2019 Bloomberg Finance L.P.
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
