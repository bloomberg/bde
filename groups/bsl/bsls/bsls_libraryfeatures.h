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
//  BSLS_LIBRARYFEATURES_HAS_AUTO_PTR:              'auto_ptr'       provided
//  BSLS_LIBRARYFEATURES_HAS_BOOL_CONSTANT:         !NOT DEFINED!   see below
//  BSLS_LIBRARYFEATURES_HAS_CPP11_ALGORITHMS:      C++11 algorithms provided
//  BSLS_LIBRARYFEATURES_HAS_PAIR_PIECEWISE_CONSTRUCTOR:   "piecewise" usable
//  BSLS_LIBRARYFEATURES_HAS_TUPLE:                 'tuple'          provided
//  BSLS_LIBRARYFEATURES_HAS_UNIQUE_PTR:            'unique_ptr'     provided
//  BSLS_LIBRARYFEATURES_HAS_UNORDERED_MAP:         'unordered_map'  provided
//
//@SEE_ALSO: bsls_platform, bsls_compilerfeatures, bsls_nativestd
//
//@AUTHOR: Steven Breitstein (sbreitstein)
//
//@DESCRIPTION: This component provides a suite of preprocessor macros that
// indicate the availability of library-specific support of features that may
// not be available in all standard library versions in use across an
// organization.  For example, the C++11 standard library introduced several
// algorithms that were not available in libraries for earlier standards.  The
// macro 'BSLS_LIBRARYFEATURES_HAS_CPP11_ALGORITHMS' can be used to
// conditionally compile BDE code depending on those algorithms when they are
// available, and exclude that code when those algorithms are not present,
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
///'BSLS_LIBRARYFEATURES_HAS_AUTO_PTR'
///-----------------------------------
// The 'BSLS_LIBRARYFEATURES_HAS_AUTO_PTR' macro is defined if the 'auto_ptr'
// class template (defined in '<memory>') is provided by the native standard
// library.   This macro is expected to be defined for all libaries/platforms
// at least until the introduction of C++17 to our build systems.
//
///'BSLS_LIBRARYFEATURES_HAS_BOOL_CONSTANT': !NOT DEFINED!
///-------------------------------------------------------
// The 'BSLS_LIBRARYFEATURES_HAS_BOOL_CONSTANT' macro is never defined and is
// mentioned in this component as documentation until its appearance in several
// BDE test drivers is removed.
//
// The 'BSLS_LIBRARYFEATURES_HAS_BOOL_CONSTANT' macro was intended to indicate
// whether or not the 'bool_constant' alias template (defined in
// '<type_traits>') is provided by the native standard library.  It has since
// been discovered that, as 'bsl::integral_constant' is actually a distinct
// class template derived from 'native_std::integral_constant', there must
// always be a distinct 'bsl' alias template, which will be provided in a
// future BDE release.  Given this new direction, the need for a macro for the
// native type is obviated so 'BSLS_LIBRARYFEATURES_HAS_BOOL_CONSTANT' is being
// removed.
//
///'BSLS_LIBRARYFEATURES_HAS_CPP11_ALGORITHMS'
///-------------------------------------------
// The 'BSLS_LIBRARYFEATURES_HAS_CPP11_ALGORITHMS' macro is defined if all of
// the following algorithms (defined in '<algorithm>', '<memory>', or
// '<numeric>') are implemented by the native standard library:
//: o 'all_of'
//: o 'any_of'
//: o 'copy_if'
//: o 'copy_n'
//: o 'find_if_not'
//: o 'iota'
//: o 'is_heap'
//: o 'is_heap_until'
//: o 'is_partitioned'
//: o 'is_permutation'
//: o 'is_sorted'
//: o 'is_sorted_until'
//: o 'minmax'
//: o 'minmax_element'
//: o 'move'
//: o 'move_backward'
//: o 'none_of'
//: o 'partition_copy'
//: o 'partition_point'
//: o 'shuffle'
//: o 'uninitialized_copy_n'
//
// Notice that the above list does *not* include 'random_shuffle' as that is
// deprecated in C++14 and will be removed in C++17.
//
///'BSLS_LIBRARYFEATURES_HAS_PAIR_PIECEWISE_CONSTRUCTOR'
///-----------------------------------------------------
// The 'BSLS_LIBRARYFEATURES_HAS_PAIR_PIECEWISE_CONSTRUCTOR' macro is defined
// if the 'pair' class template (defined in '<utility>') provided by the native
// library has a constructor with the signature:
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
//: o The 'BSLS_COMPILERFEATURES_SUPPORT_VARIADIC_TEMPLATES' macro is defined.
//:
//: o The 'BSLS_LIBRARYFEATURES_HAS_TUPLE' macro is defined.
//
///'BSLS_LIBRARYFEATURES_HAS_TUPLE'
///--------------------------------
// The 'BSLS_LIBRARYFEATURES_HAS_TUPLE' macro is defined if *both* of the
// listed conditions are true:
//
//: o The 'tuple' type template (defined in '<tuple>') is provided by the
//:   native standard library.
//:
//: o The 'BSLS_COMPILERFEATURES_SUPPORT_VARIADIC_TEMPLATES' macro is also
//:   defined.
//
///'BSLS_LIBRARYFEATURES_HAS_UNIQUE_PTR'
///-------------------------------------
// The 'BSLS_LIBRARYFEATURES_HAS_UNIQUE_PTR' macro is defined if *both* of the
// listed conditions are true:
//
//: o The 'unique_ptr' class template (defined in '<memory>') is provided by
//:   the native standard library.
//:
//: o The 'BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES' macro is also
//:   defined.
//
///'BSLS_LIBRARYFEATURES_HAS_UNORDERED_MAP'
///----------------------------------------
// The 'BSLS_LIBRARYFEATURES_HAS_UNORDERED_MAP' macro is defined if the
// 'unordered_map' type template (defined in '<unordered_map>') is provided by
// the native standard library.
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
//  #if defined(BSLS_LIBRARYFEATURES_HAS_TUPLE)
//  # ifndef INCLUDED_TUPLE
//  # include <tuple>
//  # define INCLUDED_TUPLE
//  # endif
//  #endif // BSLS_LIBRARYFEATURES_HAS_TUPLE
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
//  #ifdef BSLS_LIBRARYFEATURES_HAS_TUPLE
//      static native_std::tuple<int, double, double> getMedianMeanVariance(
//                                                            const int *begin,
//                                                            const int *end);
//          // Return the median, mean, and variance (in that order) of the
//          // sequence of values in the specified non-empty, semi-open range
//          // '[begin, end)'.  The behavior is undefined unless 'begin < end'.
//
//  #endif // BSLS_LIBRARYFEATURES_HAS_TUPLE
//
//  };
//..
// Finally, we find that our code compiles when we build our code against
// versions of the native standard library that provide a 'tuple' type, *and*
// those that do not.  Of course, in the later case the interface that returns

#ifndef INCLUDED_BSLS_PLATFORM
#include <bsls_platform.h>
#endif

#define BSLS_LIBRARYFEATURES_HAS_AUTO_PTR
    // Set unconditionally.  This features is found on each compiler
    // version/platform combination tested.  Assume universally available until
    // the day tool chains start removing this deprecated class template.

#if defined(BSLS_PLATFORM_CMP_GNU) && defined(__GXX_EXPERIMENTAL_CXX0X__)
    #if BSLS_PLATFORM_CMP_VERSION >= 40800
        #define BSLS_LIBRARYFEATURES_HAS_CPP11_ALGORITHMS
        #define BSLS_LIBRARYFEATURES_HAS_PAIR_PIECEWISE_CONSTRUCTOR
        #define BSLS_LIBRARYFEATURES_HAS_TUPLE
        #define BSLS_LIBRARYFEATURES_HAS_UNIQUE_PTR
        #define BSLS_LIBRARYFEATURES_HAS_UNORDERED_MAP
    #endif
#endif

#if defined(BSLS_PLATFORM_CMP_IBM)
    // #define __IBMCPP_TR1__ 1  // Required for 'type_traits' library that
                                 // is needed to test macros below.
    // #define BSLS_LIBRARYFEATURES_HAS_CPP11_ALGORITHMS
    // #define BSLS_LIBRARYFEATURES_HAS_PAIR_PIECEWISE_CONSTRUCTOR
    // #define BSLS_LIBRARYFEATURES_HAS_TUPLE
    // #define BSLS_LIBRARYFEATURES_HAS_UNIQUE_PTR
    // #define BSLS_LIBRARYFEATURES_HAS_UNORDERED_MAP
#endif

#if defined(BSLS_PLATFORM_CMP_SUN)
    // #define BSLS_LIBRARYFEATURES_HAS_CPP11_ALGORITHMS
    // #define BSLS_LIBRARYFEATURES_HAS_PAIR_PIECEWISE_CONSTRUCTOR
    // #define BSLS_LIBRARYFEATURES_HAS_TUPLE
    // #define BSLS_LIBRARYFEATURES_HAS_UNIQUE_PTR
    // #define BSLS_LIBRARYFEATURES_HAS_UNORDERED_MAP
#endif

#if defined(BSLS_PLATFORM_CMP_CLANG)
    #if defined(__APPLE_CC__) && \
        __APPLE_CC__ >= 6000  && \
        BSLS_PLATFORM_CMP_VERSION >= 70300

        #if defined(__GXX_EXPERIMENTAL_CXX0X__)
            #define BSLS_LIBRARYFEATURES_HAS_CPP11_ALGORITHMS
            #define BSLS_LIBRARYFEATURES_HAS_PAIR_PIECEWISE_CONSTRUCTOR
            #define BSLS_LIBRARYFEATURES_HAS_TUPLE
            #define BSLS_LIBRARYFEATURES_HAS_UNIQUE_PTR
        #endif
        #define BSLS_LIBRARYFEATURES_HAS_UNORDERED_MAP
    #endif
#endif

#if defined(BSLS_PLATFORM_CMP_MSVC)

    #if BSLS_PLATFORM_CMP_VERSION >= 1700  // Visual Studio 2012
        #define BSLS_LIBRARYFEATURES_HAS_UNORDERED_MAP
        #define BSLS_LIBRARYFEATURES_HAS_CPP11_ALGORITHMS
    #endif

    #if BSLS_PLATFORM_CMP_VERSION >= 1800  // Visual Studio 2013
        #define BSLS_LIBRARYFEATURES_HAS_UNIQUE_PTR
            // Note that 'unique_ptr' appears as early as version 1600 (MSVC
            // 2010) 'BSLS_COMPILER_FEATURES_HAS_RVALUE_REFERENCES', also
            // required for this macro, is not defined until version 1800 (MSVC
            // 2013).
    #endif

    #if BSLS_PLATFORM_CMP_VERSION >= 1900  // Visual Studio 2015

        #define BSLS_LIBRARYFEATURES_HAS_TUPLE
            // Note that earlier verions have 'tuple' but this macro also
            // requires the definition of the
            // 'BSLS_COMPILER_FEATURES_HAS_VARIADIC_TEMPLATES' macro.

        #define BSLS_LIBRARYFEATURES_HAS_PAIR_PIECEWISE_CONSTRUCTOR

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
