// bsls_libraryfeatures.h                                             -*-C++-*-
#ifndef INCLUDED_BSLS_LIBRARYFEATURES
#define INCLUDED_BSLS_LIBRARYFEATURES

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
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
//  BSLS_LIBRARYFEATURES_HAS_CPP17_PRECISE_BITWIDTH_ATOMICS: bitwidth atomics
//  BSLS_LIBRARYFEATURES_HAS_CPP11_PROGRAM_TERMINATION: "program exit" provided
//  BSLS_LIBRARYFEATURES_HAS_CPP11_TUPLE: 'tuple' provided
//  BSLS_LIBRARYFEATURES_HAS_CPP11_UNIQUE_PTR: 'unique_ptr' provided
//  BSLS_LIBRARYFEATURES_HAS_CPP17_BOOL_CONSTANT: !NOT DEFINED! see below
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
//: o Supported by the compiler vendor's STL implementation
//:   o gcc 4.8
//:   o clang 3.0
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
//: o Supported by the compiler vendor's STL implementation
//:   o gcc 4.8
//:   o clang 3.0
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
// The C++11 standard library introduced several functions and types defined in
// headers available in earlier standards and implemented by all vendors of
// platform/compiler combination that supported C++11 features.  This macro is
// used to import symbols introduced in C++11 and defined in those headers into
// the 'bsl' namesapce via aliases to the native library implementation.  This
// macro is defined if *both* of the listed conditions are true:
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
//:   o Function defined in '<memory>'
//:     o 'addressof'
//:     o 'pointer_traits'
//:     o 'uninitialized_copy_n'
//:
//:   o Function defined in '<numeric>'
//:     o 'iota'
//:
//:   o Function defined in '<utility>'
//:     o 'declval'
//:     o 'forward'
//:     o 'move'
//:     o 'move_if_noexcept'
//:     o 'swap'
//:
//: o Supported by the compiler vendor's STL implementation
//:   o gcc 4.8
//:   o clang 3.0
//:   o MSVC 2013
//
// Notice that the above list does *not* include 'random_shuffle' as that is
// deprecated in C++14 and will be removed in C++17.
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
//: o Supported by the compiler vendor's STL implementation
//:   o gcc 4.8
//:   o clang 3.0
//:   o MSVC 2015
//
///'BSLS_LIBRARYFEATURES_HAS_CPP11_GARBAGE_COLLECTION_API'
///-------------------------------------------------
// The 'BSLS_LIBRARYFEATURES_HAS_CPP11_GARBAGE_COLLECTION_API' is defined if
// *both* of the listed conditions are true:
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
//: o Supported by the compiler vendor's STL implementation
//:   o gcc 6.0
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
//: o Supported by the compiler vendor's STL implementation
//:   o gcc 5.0
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
//: o Supported by the compiler vendor's STL implementation
//:   o gcc 4.8
//:   o clang 3.0
//:   o MSVC 2015
//
///'BSLS_LIBRARYFEATURES_HAS_CPP17_PRECISE_BITWIDTH_ATOMICS'
///---------------------------------------------------------
// The 'BSLS_LIBRARYFEATURES_HAS_CPP17_PRECISE_BITWIDTH_ATOMICS' macro is
// defined if *both* of the listed conditions are true:
// defined if the '<atomic>' header provided by the native standard library
// provides type aliases for all of the following precise bit-width atomic
// types:
//
//: o The 'BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY' macro is defined.
//:
//: o The following type aliases are provided by the native standard library in
//:   the '<atomic>' header:
//:   o atomic_int8_t;
//:   o atomic_int16_t;
//:   o atomic_int32_t;
//:   o atomic_int64_t;
//:   o atomic_uint8_t;
//:   o atomic_uint16_t;
//:   o atomic_uint32_t;
//:   o atomic_uint64_t;
//:   o atomic_intptr_t;
//:   o atomic_uintptr_t;
//
//: o Supported by the compiler vendor's STL implementation
//:   o MSVC 2013
//
///'BSLS_LIBRARYFEATURES_HAS_CPP11_PROGRAM_TERMINATION'
///----------------------------------------------------
// The 'BSLS_LIBRARYFEATURES_HAS_CPP11_PROGRAM_TERMINATION' is defined if
// if *both* of the listed conditions are true:
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
//: o Supported by the compiler vendor's STL implementation
//:   o gcc 5.4
//:   o MSVC 2015
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
//: o Supported by the compiler vendor's STL implementation
//:   o gcc 4.8
//:   o clang 3.0
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
//: o Supported by the compiler vendor's STL implementation
//:   o gcc 4.8
//:   o clang 3.0
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

#ifndef INCLUDED_BSLS_PLATFORM
#include <bsls_platform.h>
#endif

#if __cplusplus < 201402L
# define BSLS_LIBRARYFEATURES_HAS_C90_GETS
    // Set unconditionally for compilers supporting an earler standard than
    // C++14; this feature macro will be undefined for those platforms with
    // partial support for C++14, implementing the removal of this dangerous
    // function.
#endif

#define BSLS_LIBRARYFEATURES_HAS_CPP98_AUTO_PTR
    // Set unconditionally.  This features is found on each compiler
    // version/platform combination tested.  Assume universally available until
    // the day tool chains start removing this deprecated class template.

#if defined(BSLS_PLATFORM_CMP_GNU)
    #define BSLS_LIBRARYFEATURES_HAS_C99_FP_CLASSIFY
    #if (__cplusplus >= 201103L) ||                                           \
           (defined(__GXX_EXPERIMENTAL_CXX0X__) &&                            \
            BSLS_PLATFORM_CMP_VERSION >= 40800)
        // C99 functions are available in C++11 builds.

        #define BSLS_LIBRARYFEATURES_HAS_C99_LIBRARY
        #define BSLS_LIBRARYFEATURES_HAS_C99_SNPRINTF
    #endif
    #if defined(__GXX_EXPERIMENTAL_CXX0X__)
        #if BSLS_PLATFORM_CMP_VERSION >= 40800
            #define BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY
            #define BSLS_LIBRARYFEATURES_HAS_CPP11_EXCEPTION_HANDLING
            #define BSLS_LIBRARYFEATURES_HAS_CPP11_PAIR_PIECEWISE_CONSTRUCTOR
            #define BSLS_LIBRARYFEATURES_HAS_CPP11_TUPLE
            #define BSLS_LIBRARYFEATURES_HAS_CPP11_UNIQUE_PTR
        #endif

        #if BSLS_PLATFORM_CMP_VERSION >= 50000
            #define BSLS_LIBRARYFEATURES_HAS_CPP11_MISCELLANEOUS_UTILITIES
        #endif

        #if BSLS_PLATFORM_CMP_VERSION >= 50400 && \
            defined(_GLIBCXX_HAVE_AT_QUICK_EXIT) && \
            defined(_GLIBCXX_HAVE_QUICK_EXIT)
            // Have not confirmed these features are in gcc 5.4+, but they are
            // not available in gcc 5.3 (the most recent gcc installed on unix
            // development hosts).  Setting this to be enabled beyond 5.3 to
            // re-test this when it is appropriate.  (hversche 2017-03-06)
            // The features also depend on the underlying GLIBC implementation
            // (distributed separately from the library.  (hrosen4 2017-04-28)

            #define BSLS_LIBRARYFEATURES_HAS_CPP11_PROGRAM_TERMINATION
        #endif
        #if BSLS_PLATFORM_CMP_VERSION >= 60000
            #define BSLS_LIBRARYFEATURES_HAS_CPP11_GARBAGE_COLLECTION_API
        #endif
    #endif

    // #define BSLS_LIBRARYFEATURES_HAS_CPP17_PRECISE_BITWIDTH_ATOMICS
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
    // #define BSLS_LIBRARYFEATURES_HAS_CPP17_PRECISE_BITWIDTH_ATOMICS
    // #define BSLS_LIBRARYFEATURES_HAS_CPP11_PROGRAM_TERMINATION
    // #define BSLS_LIBRARYFEATURES_HAS_CPP11_TUPLE
    // #define BSLS_LIBRARYFEATURES_HAS_CPP11_UNIQUE_PTR
#endif

#if defined(BSLS_PLATFORM_CMP_SUN) && _cplusplus >= 201103L

    #if BSLS_PLATFORM_CMP_VERSION >= 0x5130
        #define BSLS_LIBRARYFEATURES_HAS_C99_FP_CLASSIFY
        #define BSLS_LIBRARYFEATURES_HAS_C99_LIBRARY
        #define BSLS_LIBRARYFEATURES_HAS_C99_SNPRINTF
        #define BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY
        #define BSLS_LIBRARYFEATURES_HAS_CPP11_EXCEPTION_HANDLING
        #define BSLS_LIBRARYFEATURES_HAS_CPP11_PAIR_PIECEWISE_CONSTRUCTOR
        #define BSLS_LIBRARYFEATURES_HAS_CPP11_TUPLE
        #define BSLS_LIBRARYFEATURES_HAS_CPP11_UNIQUE_PTR
    #endif

    // #define BSLS_LIBRARYFEATURES_HAS_CPP11_GARBAGE_COLLECTION_API
    // #define BSLS_LIBRARYFEATURES_HAS_CPP11_MISCELLANEOUS_UTILITIES
    // #define BSLS_LIBRARYFEATURES_HAS_CPP17_PRECISE_BITWIDTH_ATOMICS
    // #define BSLS_LIBRARYFEATURES_HAS_CPP11_PROGRAM_TERMINATION
#endif

#if defined(BSLS_PLATFORM_CMP_CLANG)

    #define BSLS_LIBRARYFEATURES_HAS_C99_FP_CLASSIFY

    #if defined(__APPLE_CC__) && \
        __APPLE_CC__ >= 6000  && \
        BSLS_PLATFORM_CMP_VERSION >= 70300

        #if defined(__GXX_EXPERIMENTAL_CXX0X__)
            #define BSLS_LIBRARYFEATURES_HAS_C99_LIBRARY
            #define BSLS_LIBRARYFEATURES_HAS_C99_SNPRINTF
            #define BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY
            #define BSLS_LIBRARYFEATURES_HAS_CPP11_EXCEPTION_HANDLING
            #define BSLS_LIBRARYFEATURES_HAS_CPP11_PAIR_PIECEWISE_CONSTRUCTOR
            #define BSLS_LIBRARYFEATURES_HAS_CPP11_TUPLE
            #define BSLS_LIBRARYFEATURES_HAS_CPP11_UNIQUE_PTR
        #endif

    #elif BSLS_PLATFORM_CMP_VERSION >= 30000

        #if defined(__GXX_EXPERIMENTAL_CXX0X__)
            #define BSLS_LIBRARYFEATURES_HAS_C99_LIBRARY
            #define BSLS_LIBRARYFEATURES_HAS_C99_SNPRINTF
            #define BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY
            #define BSLS_LIBRARYFEATURES_HAS_CPP11_EXCEPTION_HANDLING
            #define BSLS_LIBRARYFEATURES_HAS_CPP11_PAIR_PIECEWISE_CONSTRUCTOR
            #define BSLS_LIBRARYFEATURES_HAS_CPP11_TUPLE
            #define BSLS_LIBRARYFEATURES_HAS_CPP11_UNIQUE_PTR
        #endif
    #endif

    // #define BSLS_LIBRARYFEATURES_HAS_CPP11_GARBAGE_COLLECTION_API
    // #define BSLS_LIBRARYFEATURES_HAS_CPP11_MISCELLANEOUS_UTILITIES
    // #define BSLS_LIBRARYFEATURES_HAS_CPP17_PRECISE_BITWIDTH_ATOMICS
    // #define BSLS_LIBRARYFEATURES_HAS_CPP11_PROGRAM_TERMINATION
#endif

#if defined(BSLS_PLATFORM_CMP_MSVC)

    // Notice that BDE does not support C99 and C++11 library features prior to
    // Microsoft C++ compiler version 1800 (MSVC 2013)

    #if BSLS_PLATFORM_CMP_VERSION >= 1800  // Visual Studio 2013
        #define BSLS_LIBRARYFEATURES_HAS_C99_FP_CLASSIFY
        #define BSLS_LIBRARYFEATURES_HAS_C99_LIBRARY
        #define BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY
        #define BSLS_LIBRARYFEATURES_HAS_CPP11_MISCELLANEOUS_UTILITIES
        #define BSLS_LIBRARYFEATURES_HAS_CPP17_PRECISE_BITWIDTH_ATOMICS
        #define BSLS_LIBRARYFEATURES_HAS_CPP11_UNIQUE_PTR
            // Note that 'unique_ptr' appears as early as version 1600 (MSVC
            // 2010) 'BSLS_COMPILER_FEATURES_HAS_RVALUE_REFERENCES', also
            // required for this macro, is not defined until version 1800 (MSVC
            // 2013).
    #endif

    #if BSLS_PLATFORM_CMP_VERSION >= 1900  // Visual Studio 2015

        #define BSLS_LIBRARYFEATURES_HAS_C99_SNPRINTF
        #define BSLS_LIBRARYFEATURES_HAS_CPP11_EXCEPTION_HANDLING
        #define BSLS_LIBRARYFEATURES_HAS_CPP11_GARBAGE_COLLECTION_API
        #define BSLS_LIBRARYFEATURES_HAS_CPP11_PAIR_PIECEWISE_CONSTRUCTOR
        #define BSLS_LIBRARYFEATURES_HAS_CPP11_PROGRAM_TERMINATION
        #define BSLS_LIBRARYFEATURES_HAS_CPP11_TUPLE
            // Note that earlier verions have 'tuple' but this macro also
            // requires the definition of the
            // 'BSLS_COMPILER_FEATURES_HAS_VARIADIC_TEMPLATES' macro.

        #undef BSLS_LIBRARYFEATURES_HAS_C90_GETS
    #endif
#endif

#endif // INCLUDED_BSLS_LIBRARYFEATURES

// ----------------------------------------------------------------------------
// Copyright 2017 Bloomberg Finance L.P.
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
