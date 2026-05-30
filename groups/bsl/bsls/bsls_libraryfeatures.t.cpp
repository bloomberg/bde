// bsls_libraryfeatures.t.cpp                                         -*-C++-*-
#include <bsls_libraryfeatures.h>

#include <bsls_bsltestutil.h>
#include <bsls_compilerfeatures.h>
#include <bsls_keyword.h>
#include <bsls_platform.h>

#include <algorithm>
#include <cctype>
#include <cmath>
#include <complex>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <exception>
#include <functional>
#include <iomanip>
#include <ios>
#include <iostream>
#include <iterator>
#include <locale>
#include <memory>
#include <numeric>
#include <ostream>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

#include <stddef.h>  // for `size_t`
#include <stdio.h>   // for `printf`, `puts`
#include <stdlib.h>  // for `atoi`
#include <string.h>  // for `strchr`

// Verify assumption that the BASELINE C++11 library includes all of the new
// library headers not covered by a more specific macro, and note all the other
// headers that are expected to be present in a C++11 library but are
// separately accounted for.
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY
    #include <array>
    #include <atomic>
    #include <cfenv>
    #include <chrono>
    #include <cinttypes>
    #include <codecvt>
    #include <condition_variable>
    #include <cstdint>
    #include <cuchar>
    #include <forward_list>
    #include <future>
//  #include <initializer_list>
                      // BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS
    #include <mutex>
    #include <random>
    #include <ratio>
    #include <regex>
    #include <scoped_allocator>
    #include <system_error>
    #include <thread>
//  #include <tuple>             // BSLS_LIBRARYFEATURES_HAS_CPP11_TUPLE
//  #include <type_traits>       // BSLS_COMPILERFEATURES_SUPPORT_TRAITS_HEADER
    #include <typeindex>
    #include <unordered_map>
    #include <unordered_set>
#endif

// Verify assumption that the BASELINE C++14 library includes all of the new
// library headers not covered by a more specific macro, and note all the other
// headers that are expected to be present in a C++14 library but are
// separately accounted for.
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP14_BASELINE_LIBRARY
    #include <shared_mutex>
#endif

// Verify assumption that the BASELINE C++17 library includes all of the new
// library headers not covered by a more specific macro, and note all the other
// headers that are expected to be present in a C++17 library but are
// separately accounted for.
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY
    #include <any>
//  #include <charconv>        // LIBRARYFEATURES_HAS_CPP17_CHARCONV
//  #include <execution>       // LIBRARYFEATURES_HAS_CPP17_PARALLEL_ALGORITHMS
//  #include <filesystem>      // LIBRARYFEATURES_HAS_CPP17_FILESYSTEM
//  #include <memory_resource> // LIBRARYFEATURES_HAS_CPP17_PMR
    #include <optional>
    #include <string_view>
    #include <variant>
#endif

// Verify assumption that <charconv> can be included.
#if defined(BSLS_LIBRARYFEATURES_HAS_CPP17_INT_CHARCONV)                      \
 || defined(BSLS_LIBRARYFEATURES_HAS_CPP17_CHARCONV)
    #include <charconv>
#endif

// Verify assumption that <filesystem> can be included.
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_FILESYSTEM
    #include <filesystem>
#endif

// Verify assumption that <execution> can be included.
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_PARALLEL_ALGORITHMS
    #include <execution>
#endif

// Verify assumption that <memory_resource> can be included.
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_PMR
    #include <memory_resource>
#endif

// Verify assumption that the BASELINE C++20 library includes all of the new
// library headers not covered by a more specific macro, and note all the other
// headers that are expected to be present in a C++20 library but are
// separately accounted for.
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP20_BASELINE_LIBRARY
    #include <barrier>
    #include <bit>
//  #include <compare>   // COMPILERFEATURES_SUPPORT_THREE_WAY_COMPARISON
//  #include <concepts>        // LIBRARYFEATURES_HAS_CPP20_CONCEPTS
//  #include <coroutine>       // BSLS_COMPILERFEATURES_SUPPORT_COROUTINE
//  #include <format>          // LIBRARYFEATURES_HAS_CPP20_FORMAT
    #include <latch>
    #include <numbers>
//  #include <ranges>          // LIBRARYFEATURES_HAS_CPP20_RANGES
    #include <semaphore>
//  #include <source_location> // LIBRARYFEATURES_HAS_CPP20_SOURCE_LOCATION
    #include <span>
    #include <stop_token>
//  #include <syncstream>      // LIBRARYFEATURES_HAS_CPP20_SYNCSTREAM
//  #include <version>         // LIBRARYFEATURES_HAS_CPP20_VERSION
#endif

// Verify assumption that <concepts> can be included.
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP20_CONCEPTS
    #include <concepts>
#endif

// Verify assumption that <format> can be included.
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP20_FORMAT
    #include <format>
#endif

// Verify assumption that <ranges> can be included.
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP20_RANGES
    #include <ranges>
#endif

// Verify assumption that <source_location> can be included.
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP20_SOURCE_LOCATION
    #include <source_location>
#endif

// Verify assumption that <syncstream> can be included.
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP20_SYNCSTREAM
    #include <syncstream>
#endif

// Verify assumption that <version> can be included.
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP20_VERSION
    #include <version>
#endif

// Verify assumption that the BASELINE C++23 library includes all of the new
// library headers not covered by a more specific macro, and note all the other
// headers that are expected to be present in a C++23 library but are
// separately accounted for.  Note that the C++23 baseline is defined
// purely in terms of features in existing headers; all new C++23 headers
// tracked by this component will have their own specific macros.  Note
// that several headers remain unaccounted for, pending BDE providing its
// own implementation of those headers.
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP23_BASELINE_LIBRARY
//  #include <expected>        // not tracked by this component
//  #include <flat_map>        // not tracked by this component
//  #include <flat_set>        // not tracked by this component
//  #include <generator>       // LIBRARYFEATURES_HAS_CPP23_GENERATOR
//  #include <mdspan>          // LIBRARYFEATURES_HAS_CPP23_MDSPAN
//  #include <print>           // LIBRARYFEATURES_HAS_CPP23_PRINT
//  #include <spanstream>      // LIBRARYFEATURES_HAS_CPP23_SPANSTREAM
//  #include <stacktrace>      // LIBRARYFEATURES_HAS_CPP23_STACKTRACE
//  #include <stdfloat>        // not tracked by this component
#endif

// Verify assumption that <generator> can be included.
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP23_GENERATOR
    #include <generator>
#endif

// Verify assumption that <mdspan> can be included.
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP23_MDSPAN
    #include <mdspan>
#endif

// Verify assumption that <print> can be included.
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP23_PRINT
    #include <print>
#endif

// Verify assumption that <spanstream> can be included.
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP23_SPANSTREAM
    #include <spanstream>
#endif

// Verify assumption that <stacktrace> can be included.
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP23_STACKTRACE
    #include <stacktrace>
#endif

// ============================================================================
//                             TEST PLAN
// ----------------------------------------------------------------------------
//                             Overview
//                             --------
// This component provides macros that indicate the presence (or not) of
// selected features (i.e., header-files/types) in the native standard library
// used in the current build.  In the current implementation, these flags are
// not set according to information directly from the library; rather, these
// features are inferred from platform and compiler information acquired from
// macros set in other components.  The role of this test driver is to provide
// a redundant check that those macros were correctly combined to produce the
// expected results.
//
// Atypically of BDE test drivers, the semantic tests are done implicitly are
// the point of usage in client components.  The test driver does a
// conditional, compile-time sanity check that the relevant header files can be
// included and, in some cases, that expected types are defined; however, there
// is also a *semantic* requirement associated with the macros defined in this
// component.  The macros, when defined, imply that certain features
// files/types exist *and* that their implementation is sufficient to support
// certain uses in other (higher) components.  The existence is tested by this
// test driver by minimal usage; however, comprehensive testing of the
// semantics is impractical.
//
// This test driver recognizes preprocessor flags that allow the developer to
// manually compile-time check for the absence of a feature when the associated
// macro test is *not* defined by the component.  Note that the component does
// not require that feature be absent when the macro is undefined.  See
// {`bsls_libraryfeatures`|Converse Logic}.  These tests are provided as a
// debugging aid and as an investigative tool to discover the presence or
// absence of these features in libraries that have not yet been evaluated.
//
// The compilation of code that uses a feature can be forced by defining (e.g.,
// on the command line) a macro consisting of the feature macro suffixed by
// `_FORCE`.  For example, to force a test for the presence of the `<tuple>`
// type specify `-DBSLS_LIBRARYFEATURES_HAS_CPP11_TUPLE_FORCE` on the command
// line.  The code associated with that feature will be exposed to the compiler
// even if `BSLS_LIBRARYFEATURES_HAS_CPP11_TUPLE_FORCE` was not defined by this
// component.
//
// Here, *failure* to compile this test driver is an indication that the
// feature is indeed absent from the library being evaluated.  Should the test
// driver (unexpectedly) compile, run the test case in very very verbose mode
// to determine whether or not the macro for the feature of interest is
// defined.
//
//  - If defined, then component was correct in defining that macro (and any
//    expectation of compiler failure was incorrect).
//
//  - If not defined, the component could be updated to recognize the build
//    configuration as providing the feature of interest.
//
// TBD Add tests for the new macros (and amendments to existing macros).  See
//     the macros without test-case numbers below.
// ----------------------------------------------------------------------------
// [  ] BSLS_LIBRARYFEATURES_HAS_C90_GETS
// [  ] BSLS_LIBRARYFEATURES_HAS_C99_FP_CLASSIFY
// [ 6] BSLS_LIBRARYFEATURES_HAS_C99_LIBRARY
// [ 6] BSLS_LIBRARYFEATURES_HAS_C99_SNPRINTF
// [ 1] BSLS_LIBRARYFEATURES_HAS_CPP98_AUTO_PTR
// [  ] BSLS_LIBRARYFEATURES_HAS_CPP98_BINDERS_API
// [ 2] BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY
// [  ] BSLS_LIBRARYFEATURES_HAS_CPP11_DYNAMIC_EXCEPTION_SPECS
// [  ] BSLS_LIBRARYFEATURES_HAS_CPP11_EXCEPTION_HANDLING
// [  ] BSLS_LIBRARYFEATURES_HAS_CPP11_GARBAGE_COLLECTION_API
// [  ] BSLS_LIBRARYFEATURES_HAS_CPP11_MISCELLANEOUS_UTILITIES
// [ 3] BSLS_LIBRARYFEATURES_HAS_CPP11_PAIR_PIECEWISE_CONSTRUCTOR
// [  ] BSLS_LIBRARYFEATURES_HAS_CPP11_PROGRAM_TERMINATION
// [12] BSLS_LIBRARYFEATURES_HAS_CPP11_RANGE_FUNCTIONS
// [ 2] BSLS_LIBRARYFEATURES_HAS_CPP11_SHORT_STRING
// [  ] BSLS_LIBRARYFEATURES_HAS_CPP11_STREAM_MOVE
// [ 4] BSLS_LIBRARYFEATURES_HAS_CPP11_TUPLE
// [ 5] BSLS_LIBRARYFEATURES_HAS_CPP11_UNIQUE_PTR
// [ 9] BSLS_LIBRARYFEATURES_HAS_CPP14_BASELINE_LIBRARY
// [11] BSLS_LIBRARYFEATURES_HAS_CPP14_INTEGER_SEQUENCE
// [  ] BSLS_LIBRARYFEATURES_HAS_CPP14_RANGE_FUNCTIONS
// [15] BSLS_LIBRARYFEATURES_HAS_CPP17_ALIGNED_ALLOC
// [14] BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY
// [18] BSLS_LIBRARYFEATURES_HAS_CPP17_CHARCONV
// [  ] BSLS_LIBRARYFEATURES_HAS_CPP17_DEPRECATED_REMOVED
// [  ] BSLS_LIBRARYFEATURES_HAS_CPP17_FILESYSTEM
// [18] BSLS_LIBRARYFEATURES_HAS_CPP17_INT_CHARCONV
// [  ] BSLS_LIBRARYFEATURES_HAS_CPP17_PARALLEL_ALGORITHMS
// [15] BSLS_LIBRARYFEATURES_HAS_CPP17_PMR
// [15] BSLS_LIBRARYFEATURES_HAS_CPP17_PMR_STRING
// [ 8] BSLS_LIBRARYFEATURES_HAS_CPP17_PRECISE_BITWIDTH_ATOMICS
// [16] BSLS_LIBRARYFEATURES_HAS_CPP17_RANGE_FUNCTIONS
// [13] BSLS_LIBRARYFEATURES_HAS_CPP17_SEARCH_FUNCTORS
// [13] BSLS_LIBRARYFEATURES_HAS_CPP17_SEARCH_OVERLOAD
// [  ] BSLS_LIBRARYFEATURES_HAS_CPP17_SPECIAL_MATH_FUNCTIONS
// [15] BSLS_LIBRARYFEATURES_HAS_CPP17_TIMESPEC_GET
// [19] BSLS_LIBRARYFEATURES_HAS_CPP20_ATOMIC_FLAG_TEST_FREE_FUNCTIONS
// [19] BSLS_LIBRARYFEATURES_HAS_CPP20_ATOMIC_LOCK_FREE_TYPE_ALIASES
// [19] BSLS_LIBRARYFEATURES_HAS_CPP20_ATOMIC_REF
// [19] BSLS_LIBRARYFEATURES_HAS_CPP20_ATOMIC_WAIT_FREE_FUNCTIONS
// [17] BSLS_LIBRARYFEATURES_HAS_CPP20_BASELINE_LIBRARY
// [19] BSLS_LIBRARYFEATURES_HAS_CPP20_CALENDAR
// [19] BSLS_LIBRARYFEATURES_HAS_CPP20_CHAR8_MB_CONV
// [19] BSLS_LIBRARYFEATURES_HAS_CPP20_CONCEPTS
// [  ] BSLS_LIBRARYFEATURES_HAS_CPP20_DEPRECATED_REMOVED
// [  ] BSLS_LIBRARYFEATURES_HAS_CPP20_FORMAT
// [23] BSLS_LIBRARYFEATURES_HAS_CPP20_IS_CORRESPONDING_MEMBER
// [21] BSLS_LIBRARYFEATURES_HAS_CPP20_IS_LAYOUT_COMPATIBLE
// [22] BSLS_LIBRARYFEATURES_HAS_CPP20_IS_POINTER_INTERCONVERTIBLE
// [24] BSLS_LIBRARYFEATURES_HAS_CPP20_JTHREAD
// [19] BSLS_LIBRARYFEATURES_HAS_CPP20_MAKE_UNIQUE_FOR_OVERWRITE
// [19] BSLS_LIBRARYFEATURES_HAS_CPP20_RANGES
// [19] BSLS_LIBRARYFEATURES_HAS_CPP20_SOURCE_LOCATION
// [19] BSLS_LIBRARYFEATURES_HAS_CPP20_SYNCSTREAM
// [20] BSLS_LIBRARYFEATURES_HAS_CPP20_TO_ARRAY
// [19] BSLS_LIBRARYFEATURES_HAS_CPP20_TIMEZONE
// [19] BSLS_LIBRARYFEATURES_HAS_CPP20_VERSION
// [26] BSLS_LIBRARYFEATURES_HAS_CPP23_ALLOCATE_AT_LEAST
// [25] BSLS_LIBRARYFEATURES_HAS_CPP23_BASELINE_LIBRARY
// [26] BSLS_LIBRARYFEATURES_HAS_CPP23_BIND_BACK
// [26] BSLS_LIBRARYFEATURES_HAS_CPP23_CONTAINERS_RANGES
// [26] BSLS_LIBRARYFEATURES_HAS_CPP23_FORWARD_LIKE
// [26] BSLS_LIBRARYFEATURES_HAS_CPP23_GENERATOR
// [26] BSLS_LIBRARYFEATURES_HAS_CPP23_INT_CHARCONV
// [26] BSLS_LIBRARYFEATURES_HAS_CPP23_IS_IMPLICIT_LIFETIME
// [26] BSLS_LIBRARYFEATURES_HAS_CPP23_MDSPAN
// [26] BSLS_LIBRARYFEATURES_HAS_CPP23_OUT_PTR
// [26] BSLS_LIBRARYFEATURES_HAS_CPP23_PRINT
// [26] BSLS_LIBRARYFEATURES_HAS_CPP23_RANGE_ADAPTOR_CLOSURE
// [  ] BSLS_LIBRARYFEATURES_HAS_CPP23_RANGE_FORMAT
// [26] BSLS_LIBRARYFEATURES_HAS_CPP23_RANGES_AS_CONST
// [26] BSLS_LIBRARYFEATURES_HAS_CPP23_RANGES_CARTESIAN_PRODUCT
// [26] BSLS_LIBRARYFEATURES_HAS_CPP23_RANGES_CHUNK
// [26] BSLS_LIBRARYFEATURES_HAS_CPP23_RANGES_CONTAINS
// [26] BSLS_LIBRARYFEATURES_HAS_CPP23_RANGES_ENUMERATE
// [26] BSLS_LIBRARYFEATURES_HAS_CPP23_RANGES_FIND_LAST
// [26] BSLS_LIBRARYFEATURES_HAS_CPP23_RANGES_FOLD
// [26] BSLS_LIBRARYFEATURES_HAS_CPP23_RANGES_IOTA
// [26] BSLS_LIBRARYFEATURES_HAS_CPP23_RANGES_JOIN_WITH
// [26] BSLS_LIBRARYFEATURES_HAS_CPP23_RANGES_SHIFT
// [26] BSLS_LIBRARYFEATURES_HAS_CPP23_RANGES_SLIDE
// [26] BSLS_LIBRARYFEATURES_HAS_CPP23_RANGES_STARTS_ENDS_WITH
// [26] BSLS_LIBRARYFEATURES_HAS_CPP23_RANGES_STRIDE
// [26] BSLS_LIBRARYFEATURES_HAS_CPP23_RANGES_TO_CONTAINER
// [26] BSLS_LIBRARYFEATURES_HAS_CPP23_RANGES_ZIP
// [26] BSLS_LIBRARYFEATURES_HAS_CPP23_REFERENCE_FROM_TEMPORARY
// [26] BSLS_LIBRARYFEATURES_HAS_CPP23_SPANSTREAM
// [  ] BSLS_LIBRARYFEATURES_HAS_CPP23_STACKTRACE
// [26] BSLS_LIBRARYFEATURES_HAS_CPP23_START_LIFETIME_AS
// [10] BSLS_LIBRARYFEATURES_STDCPP_GNU
// [10] BSLS_LIBRARYFEATURES_STDCPP_IBM
// [  ] BSLS_LIBRARYFEATURES_STDCPP_INTELLISENSE
// [10] BSLS_LIBRARYFEATURES_STDCPP_LIBCSTD
// [10] BSLS_LIBRARYFEATURES_STDCPP_LLVM
// [10] BSLS_LIBRARYFEATURES_STDCPP_MSVC
// [10] BSLS_LIBRARYFEATURES_STDCPP_STLPORT
// [ 7] int std::isblank(int);
// [ 7] bool std::isblank(char, const std::locale&);
// ----------------------------------------------------------------------------
// [25] USAGE EXAMPLE
// ----------------------------------------------------------------------------

// ============================================================================
//                     STANDARD BSL ASSERT TEST FUNCTION
// ----------------------------------------------------------------------------

namespace {

int testStatus = 0;

void aSsErT(bool condition, const char *message, int line)
{
    if (condition) {
        printf("Error " __FILE__ "(%d): %s    (failed)\n", line, message);
        fflush(stdout);

        if (0 <= testStatus && testStatus <= 100) {
            ++testStatus;
        }
    }
}

}  // close unnamed namespace

// ============================================================================
//               STANDARD BSL TEST DRIVER MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT       BSLS_BSLTESTUTIL_ASSERT
#define ASSERTV      BSLS_BSLTESTUTIL_ASSERTV

#define LOOP_ASSERT  BSLS_BSLTESTUTIL_LOOP_ASSERT
#define LOOP0_ASSERT BSLS_BSLTESTUTIL_LOOP0_ASSERT
#define LOOP1_ASSERT BSLS_BSLTESTUTIL_LOOP1_ASSERT
#define LOOP2_ASSERT BSLS_BSLTESTUTIL_LOOP2_ASSERT
#define LOOP3_ASSERT BSLS_BSLTESTUTIL_LOOP3_ASSERT
#define LOOP4_ASSERT BSLS_BSLTESTUTIL_LOOP4_ASSERT
#define LOOP5_ASSERT BSLS_BSLTESTUTIL_LOOP5_ASSERT
#define LOOP6_ASSERT BSLS_BSLTESTUTIL_LOOP6_ASSERT

#define Q            BSLS_BSLTESTUTIL_Q   // Quote identifier literally.
#define P            BSLS_BSLTESTUTIL_P   // Print identifier and value.
#define P_           BSLS_BSLTESTUTIL_P_  // P(X) without '\n'.
#define T_           BSLS_BSLTESTUTIL_T_  // Print a tab (w/o newline).
#define L_           BSLS_BSLTESTUTIL_L_  // current Line number

#define STRINGIFY2(...) "" #__VA_ARGS__
#define STRINGIFY(a) STRINGIFY2(a)

// ============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
// ----------------------------------------------------------------------------

                // Global constants for testing invariants

static const
bool   BSLS_COMPILERFEATURES_SUPPORT_ALIAS_TEMPLATES_defined =
#ifdef BSLS_COMPILERFEATURES_SUPPORT_ALIAS_TEMPLATES
                                                                          true;
#else
                                                                         false;
#endif

static const
bool   BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES_defined =
#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
                                                                          true;
#else
                                                                         false;
#endif

static const
bool   BSLS_COMPILERFEATURES_SUPPORT_VARIADIC_TEMPLATES_defined =
#ifdef BSLS_COMPILERFEATURES_SUPPORT_VARIADIC_TEMPLATES
                                                                          true;
#else
                                                                         false;
#endif

static const
bool   BSLS_LIBRARYFEATURES_HAS_CPP11_RANGE_FUNCTIONS_defined =
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_RANGE_FUNCTIONS
                                                                          true;
#else
                                                                         false;
#endif

static const
bool   BSLS_LIBRARYFEATURES_HAS_CPP14_INTEGER_SEQUENCE_defined =
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP14_INTEGER_SEQUENCE
                                                                          true;
#else
                                                                         false;
#endif

static const
bool   BSLS_LIBRARYFEATURES_HAS_CPP17_ALIGNED_ALLOC_defined =
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_ALIGNED_ALLOC
                                                                          true;
#else
                                                                         false;
#endif
static const
bool   BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY_defined =
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY
                                                                          true;
#else
                                                                         false;
#endif

static const
bool   BSLS_LIBRARYFEATURES_HAS_CPP17_CHARCONV_defined =
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_CHARCONV
                                                                          true;
#else
                                                                         false;
#endif

static const
bool   BSLS_LIBRARYFEATURES_HAS_CPP17_INT_CHARCONV_defined =
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_INT_CHARCONV
                                                                          true;
#else
                                                                         false;
#endif

static const
bool   BSLS_LIBRARYFEATURES_HAS_CPP17_PMR_defined =
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_PMR
                                                                          true;
#else
                                                                         false;
#endif

static const
bool   BSLS_LIBRARYFEATURES_HAS_CPP17_PMR_STRING_defined =
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_PMR_STRING
                                                                          true;
#else
                                                                         false;
#endif

static const
bool   BSLS_LIBRARYFEATURES_HAS_CPP17_RANGE_FUNCTIONS_defined =
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_RANGE_FUNCTIONS
                                                                          true;
#else
                                                                         false;
#endif

static const
bool   BSLS_LIBRARYFEATURES_HAS_CPP17_SEARCH_FUNCTORS_defined =
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_SEARCH_FUNCTORS
                                                                          true;
#else
                                                                         false;
#endif

static const
bool   BSLS_LIBRARYFEATURES_HAS_CPP17_SEARCH_OVERLOAD_defined =
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_SEARCH_OVERLOAD
                                                                          true;
#else
                                                                         false;
#endif

static const
bool   BSLS_LIBRARYFEATURES_HAS_CPP17_TIMESPEC_GET_defined =
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_TIMESPEC_GET
                                                                          true;
#else
                                                                         false;
#endif

static const
bool   BSLS_LIBRARYFEATURES_HAS_CPP20_ATOMIC_FLAG_TEST_FREE_FUNCTIONS_defined =
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP20_ATOMIC_FLAG_TEST_FREE_FUNCTIONS
                                                                          true;
#else
                                                                         false;
#endif

static const
bool   BSLS_LIBRARYFEATURES_HAS_CPP20_ATOMIC_LOCK_FREE_TYPE_ALIASES_defined =
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP20_ATOMIC_LOCK_FREE_TYPE_ALIASES
                                                                          true;
#else
                                                                         false;
#endif

static const
bool   BSLS_LIBRARYFEATURES_HAS_CPP20_ATOMIC_REF_defined =
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP20_ATOMIC_REF
                                                                          true;
#else
                                                                         false;
#endif

static const
bool   BSLS_LIBRARYFEATURES_HAS_CPP20_ATOMIC_WAIT_FREE_FUNCTIONS_defined =
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP20_ATOMIC_WAIT_FREE_FUNCTIONS
                                                                          true;
#else
                                                                         false;
#endif

static const
bool   BSLS_LIBRARYFEATURES_HAS_CPP20_BASELINE_LIBRARY_defined =
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP20_BASELINE_LIBRARY
                                                                          true;
#else
                                                                         false;
#endif

static const
bool   BSLS_LIBRARYFEATURES_HAS_CPP20_CALENDAR_defined =
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP20_CALENDAR
                                                                          true;
#else
                                                                         false;
#endif

static const
bool   BSLS_LIBRARYFEATURES_HAS_CPP20_CHAR8_MB_CONV_defined =
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP20_CHAR8_MB_CONV
                                                                          true;
#else
                                                                         false;
#endif

static const
bool   BSLS_LIBRARYFEATURES_HAS_CPP20_CONCEPTS_defined =
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP20_CONCEPTS
                                                                          true;
#else
                                                                         false;
#endif

static const
bool   BSLS_LIBRARYFEATURES_HAS_CPP20_FORMAT_defined =
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP20_FORMAT
                                                                          true;
#else
                                                                         false;
#endif

static const
bool   BSLS_LIBRARYFEATURES_HAS_CPP20_MAKE_UNIQUE_FOR_OVERWRITE_defined =
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP20_MAKE_UNIQUE_FOR_OVERWRITE
                                                                          true;
#else
                                                                         false;
#endif

static const
bool   BSLS_LIBRARYFEATURES_HAS_CPP20_RANGES_defined =
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP20_RANGES
                                                                          true;
#else
                                                                         false;
#endif
static const
bool   BSLS_LIBRARYFEATURES_HAS_CPP20_SOURCE_LOCATION_defined =
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP20_SOURCE_LOCATION
                                                                          true;
#else
                                                                         false;
#endif

static const
bool   BSLS_LIBRARYFEATURES_HAS_CPP20_SYNCSTREAM_defined =
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP20_SYNCSTREAM
                                                                          true;
#else
                                                                         false;
#endif

static const
bool   BSLS_LIBRARYFEATURES_HAS_CPP20_TIMEZONE_defined =
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP20_TIMEZONE
                                                                          true;
#else
                                                                         false;
#endif

static const
bool   BSLS_LIBRARYFEATURES_HAS_CPP20_TO_ARRAY_defined =
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP20_TO_ARRAY
                                                                          true;
#else
                                                                         false;
#endif

static const
bool   BSLS_LIBRARYFEATURES_HAS_CPP20_VERSION_defined =
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP20_VERSION
                                                                          true;
#else
                                                                         false;
#endif

static const
bool   BSLS_LIBRARYFEATURES_HAS_CPP23_ALLOCATE_AT_LEAST_defined =
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP23_ALLOCATE_AT_LEAST
                                                                          true;
#else
                                                                         false;
#endif

static const
bool   BSLS_LIBRARYFEATURES_HAS_CPP23_BASELINE_LIBRARY_defined =
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP23_BASELINE_LIBRARY
                                                                          true;
#else
                                                                         false;
#endif

static const
bool   BSLS_LIBRARYFEATURES_HAS_CPP23_BIND_BACK_defined =
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP23_BIND_BACK
                                                                          true;
#else
                                                                         false;
#endif

static const
bool   BSLS_LIBRARYFEATURES_HAS_CPP23_CONTAINERS_RANGES_defined =
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP23_CONTAINERS_RANGES
                                                                          true;
#else
                                                                         false;
#endif

static const
bool   BSLS_LIBRARYFEATURES_HAS_CPP23_FORWARD_LIKE_defined =
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP23_FORWARD_LIKE
                                                                          true;
#else
                                                                         false;
#endif

static const
bool   BSLS_LIBRARYFEATURES_HAS_CPP23_GENERATOR_defined =
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP23_GENERATOR
                                                                          true;
#else
                                                                         false;
#endif

static const
bool   BSLS_LIBRARYFEATURES_HAS_CPP23_INT_CHARCONV_defined =
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP23_INT_CHARCONV
                                                                          true;
#else
                                                                         false;
#endif

static const
bool   BSLS_LIBRARYFEATURES_HAS_CPP23_IS_IMPLICIT_LIFETIME_defined =
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP23_IS_IMPLICIT_LIFETIME
                                                                          true;
#else
                                                                         false;
#endif

static const
bool   BSLS_LIBRARYFEATURES_HAS_CPP23_MDSPAN_defined =
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP23_MDSPAN
                                                                          true;
#else
                                                                         false;
#endif

static const
bool   BSLS_LIBRARYFEATURES_HAS_CPP23_OUT_PTR_defined =
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP23_OUT_PTR
                                                                          true;
#else
                                                                         false;
#endif

static const
bool   BSLS_LIBRARYFEATURES_HAS_CPP23_PRINT_defined =
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP23_PRINT
                                                                          true;
#else
                                                                         false;
#endif

static const
bool   BSLS_LIBRARYFEATURES_HAS_CPP23_RANGE_ADAPTOR_CLOSURE_defined =
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP23_RANGE_ADAPTOR_CLOSURE
                                                                          true;
#else
                                                                         false;
#endif

static const
bool   BSLS_LIBRARYFEATURES_HAS_CPP23_RANGES_AS_CONST_defined =
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP23_RANGES_AS_CONST
                                                                          true;
#else
                                                                         false;
#endif

static const
bool   BSLS_LIBRARYFEATURES_HAS_CPP23_RANGES_CARTESIAN_PRODUCT_defined =
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP23_RANGES_CARTESIAN_PRODUCT
                                                                          true;
#else
                                                                         false;
#endif

static const
bool   BSLS_LIBRARYFEATURES_HAS_CPP23_RANGES_CHUNK_defined =
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP23_RANGES_CHUNK
                                                                          true;
#else
                                                                         false;
#endif

static const
bool   BSLS_LIBRARYFEATURES_HAS_CPP23_RANGES_CONTAINS_defined =
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP23_RANGES_CONTAINS
                                                                          true;
#else
                                                                         false;
#endif

static const
bool   BSLS_LIBRARYFEATURES_HAS_CPP23_RANGES_ENUMERATE_defined =
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP23_RANGES_ENUMERATE
                                                                          true;
#else
                                                                         false;
#endif

static const
bool   BSLS_LIBRARYFEATURES_HAS_CPP23_RANGES_FIND_LAST_defined =
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP23_RANGES_FIND_LAST
                                                                          true;
#else
                                                                         false;
#endif

static const
bool   BSLS_LIBRARYFEATURES_HAS_CPP23_RANGES_FOLD_defined =
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP23_RANGES_FOLD
                                                                          true;
#else
                                                                         false;
#endif

static const
bool   BSLS_LIBRARYFEATURES_HAS_CPP23_RANGES_IOTA_defined =
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP23_RANGES_IOTA
                                                                          true;
#else
                                                                         false;
#endif

static const
bool   BSLS_LIBRARYFEATURES_HAS_CPP23_RANGES_JOIN_WITH_defined =
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP23_RANGES_JOIN_WITH
                                                                          true;
#else
                                                                         false;
#endif

static const
bool   BSLS_LIBRARYFEATURES_HAS_CPP23_RANGES_SHIFT_defined =
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP23_RANGES_SHIFT
                                                                          true;
#else
                                                                         false;
#endif

static const
bool   BSLS_LIBRARYFEATURES_HAS_CPP23_RANGES_SLIDE_defined =
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP23_RANGES_SLIDE
                                                                          true;
#else
                                                                         false;
#endif

static const
bool   BSLS_LIBRARYFEATURES_HAS_CPP23_RANGES_STARTS_ENDS_WITH_defined =
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP23_RANGES_STARTS_ENDS_WITH
                                                                          true;
#else
                                                                         false;
#endif

static const
bool   BSLS_LIBRARYFEATURES_HAS_CPP23_RANGES_STRIDE_defined =
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP23_RANGES_STRIDE
                                                                          true;
#else
                                                                         false;
#endif

static const
bool   BSLS_LIBRARYFEATURES_HAS_CPP23_RANGES_TO_CONTAINER_defined =
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP23_RANGES_TO_CONTAINER
                                                                          true;
#else
                                                                         false;
#endif

static const
bool   BSLS_LIBRARYFEATURES_HAS_CPP23_RANGES_ZIP_defined =
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP23_RANGES_ZIP
                                                                          true;
#else
                                                                         false;
#endif

static const
bool   BSLS_LIBRARYFEATURES_HAS_CPP23_REFERENCE_FROM_TEMPORARY_defined =
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP23_REFERENCE_FROM_TEMPORARY
                                                                          true;
#else
                                                                         false;
#endif

static const
bool   BSLS_LIBRARYFEATURES_HAS_CPP23_SPANSTREAM_defined =
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP23_SPANSTREAM
                                                                          true;
#else
                                                                         false;
#endif

static const
bool   BSLS_LIBRARYFEATURES_HAS_CPP23_START_LIFETIME_AS_defined =
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP23_START_LIFETIME_AS
                                                                          true;
#else
                                                                         false;
#endif


                        // case 19

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP20_CONCEPTS
#include <vector> // for `concepts`
namespace case19 {
/// Return `true` if the specified `lhs` and `rhs` are equal.  This function
/// is used to conform concepts availability.
template <class TYPE>
requires std::equality_comparable<TYPE>
constexpr bool equal(const TYPE& lhs, const TYPE& rhs)
{
    return lhs == rhs;
}

}  // close namespace case19
#endif  // BSLS_LIBRARYFEATURES_HAS_CPP20_CONCEPTS

                    // case 13

#include <algorithm> // for `search`
#include <utility>   // for `pair`

namespace case13 {

struct SearcherNull {

    /// Return `std::pair<last, last>` ("needle not found")
    /// irrespective of the contents of `[first, last)`.  Note that the
    /// (default) constructor does not allow the specification of a
    /// "needle".
    std::pair<const char *, const char*> operator()(const char* first,
                                                    const char* last) const;
};

std::pair<const char *, const char*>
SearcherNull::operator()(const char *, const char *last) const
{
    return std::make_pair(last, last);
}

template<class ForwardIterator, class Searcher>
BSLS_KEYWORD_CONSTEXPR ForwardIterator search(ForwardIterator first,
                                              ForwardIterator last,
                                              const Searcher& searcher)
{
    return searcher(first, last).first;
}

}  // close namespace case13

                    // case 12

namespace case12 {
    // We need to portably test whether the begin/end free functions are part
    // of namespace `std` when the feature macro is NOT defined, as this will
    // cause failures when BDE code provides its own alternative.

struct TestType {
    typedef int * iterator;

    int d_data;

    int *begin() { return &d_data; }
    int *end() { return &d_data; }
};

/// Return `begin()` for the specified `c`.
template <class CONTAINER>
typename CONTAINER::iterator begin(CONTAINER & c)
{
    return c.begin();
}

/// Return `end()` for the specified `c`.
template <class CONTAINER>
typename CONTAINER::iterator end(CONTAINER & c)
{
    return c.end();
}

}  // close namespace case12

                    // case 11

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP14_BASELINE_LIBRARY) || \
    defined(BSLS_LIBRARYFEATURES_HAS_CPP14_BASELINE_LIBRARY_FORCE)

    #include <memory>
    using Obj1 = std::integer_sequence<int>;
    using Obj2 = std::index_sequence<1>;
    using Obj3 = std::make_integer_sequence<int, 1>;
    using Obj4 = std::make_index_sequence<1>;
    using Obj5 = std::index_sequence_for<int>;
#endif

                    // case 10

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP14_BASELINE_LIBRARY) || \
    defined(BSLS_LIBRARYFEATURES_HAS_CPP14_BASELINE_LIBRARY_FORCE)

    #include <chrono>
    #include <complex>
    #include <iomanip>
    #include <iterator>
    #include <functional>
    #include <memory>
    #include <type_traits>
    #include <utility>
    #include <vector>

    /// Use each of the function templates associated with the
    /// `BSLS_LIBRARYFEATURES_HAS_CPP14_BASELINE_LIBRARY` flag in a
    /// syntactically correct (and semantically meaningless) manner as a
    /// compile-time test that these templates are available.
    static void useCpp14Algorithms()
    {
        // Type defined in `<functional>`
        ASSERT(0x0F == std::bit_not<unsigned char>().operator()(0xF0));

        // Function defined in `<iterator>`
        std::vector<int> v;
        (void)std::make_reverse_iterator(v.end());

        // Function defined in `<iomanip>`
        (void)std::quoted("\"quotes\"");

        // Function defined in `<utility>`
        int X(0);
        int Y = std::exchange(X, 1);    (void)Y;

        { // UDLs for <complex>
            using namespace std::complex_literals;
            std::complex<double>      zi = 2i;
            (void)zi;
            std::complex<long double> zl = 2il;
            (void)zl;
            std::complex<float>       zf = 2if;
            (void)zf;
        }

        { // UDLs for <chrono>
            using namespace std::chrono_literals;
            std::chrono::duration<double> d(0);
            d = 1h;
            d = 2min;
            d = 3s;
            d = 4ms;
            d = 5ns;
            d = 6us;
        }

        { // Function defined in `<memory>`
            std::unique_ptr<int> up = std::make_unique<int>(0);
        }

        { // Functions defined in `<type_traits>`
            bool b = std::is_null_pointer<int *>::value;
            (void)b;

            class Foo final {};
            bool b2 = std::is_final<Foo>::value;
            (void)b2;
        }
    }
#endif

static const
bool   BSLS_LIBRARYFEATURES_HAS_CPP14_BASELINE_LIBRARY_defined =
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP14_BASELINE_LIBRARY
                                                                         true;
#else
                                                                         false;
#endif

                    // case 8

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP17_PRECISE_BITWIDTH_ATOMICS) || \
    defined(BSLS_LIBRARYFEATURES_HAS_CPP17_PRECISE_BITWIDTH_ATOMICS_FORCE)

    // We have already included `<atomic>` since C++11 baseline is present.

    /// Attempt to use all standard names that exist for the feature flag
    /// `BSLS_LIBRARYFEATURES_HAS_CPP17_PRECISE_BITWIDTH_ATOMICS` as a
    /// compile-time test that these specializations are available.  In
    /// addition, verify at run-time that these precise bit-size atomics
    /// have the expected (standard-mandated) `value_type`.
    static void useCpp11PreciseBitwidthAtomics()
    {
#define ASSERT_TYPEMATCH(type1, type2)       \
    ASSERT((std::is_same<type1, type2>::value))

        ASSERT_TYPEMATCH(std::atomic_int8_t::value_type,    std::int8_t   );
        ASSERT_TYPEMATCH(std::atomic_int16_t::value_type,   std::int16_t  );
        ASSERT_TYPEMATCH(std::atomic_int32_t::value_type,   std::int32_t  );
        ASSERT_TYPEMATCH(std::atomic_int64_t::value_type,   std::int64_t  );
        ASSERT_TYPEMATCH(std::atomic_uint8_t::value_type,   std::uint8_t  );
        ASSERT_TYPEMATCH(std::atomic_uint16_t::value_type,  std::uint16_t );
        ASSERT_TYPEMATCH(std::atomic_uint32_t::value_type,  std::uint32_t );
        ASSERT_TYPEMATCH(std::atomic_uint64_t::value_type,  std::uint64_t );
        ASSERT_TYPEMATCH(std::atomic_intptr_t::value_type,  std::intptr_t );
        ASSERT_TYPEMATCH(std::atomic_uintptr_t::value_type, std::uintptr_t);
#undef ASSERT_TYPEMATCH
    }
#endif

static const
bool   BSLS_LIBRARYFEATURES_HAS_CPP17_PRECISE_BITWIDTH_ATOMICS_defined =
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_PRECISE_BITWIDTH_ATOMICS
                                                                        true;
#else
                                                                        false;
#endif

                    // case 7

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY
     #include <cctype>
#endif

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_MISCELLANEOUS_UTILITIES
     #include <locale>
#endif

static const
bool   BSLS_LIBRARYFEATURES_HAS_CPP11_MISCELLANEOUS_UTILITIES_defined =
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_MISCELLANEOUS_UTILITIES
                                                                          true;
#else
                                                                         false;
#endif


                    // case 6

#ifdef BSLS_LIBRARYFEATURES_HAS_C99_LIBRARY
     #include <cstdlib>
     #include <cctype>
     #include <cmath>
#endif

#ifdef BSLS_LIBRARYFEATURES_HAS_C99_SNPRINTF
     #include <cstdio>
#endif

static const
bool   BSLS_LIBRARYFEATURES_HAS_C99_LIBRARY_defined =
#ifdef BSLS_LIBRARYFEATURES_HAS_C99_LIBRARY
                                                                          true;
#else
                                                                         false;
#endif

static const
bool   BSLS_LIBRARYFEATURES_HAS_C99_SNPRINTF_defined =
#ifdef BSLS_LIBRARYFEATURES_HAS_C99_SNPRINTF
                                                                          true;
#else
                                                                         false;
#endif

                    // case 5

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP11_UNIQUE_PTR) || \
    defined(BSLS_LIBRARYFEATURES_HAS_CPP11_UNIQUE_PTR_FORCE)

    #include <memory>
    std::unique_ptr<int> up;
#endif

static const
bool   BSLS_LIBRARYFEATURES_HAS_CPP11_UNIQUE_PTR_defined =
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_UNIQUE_PTR
                                                                          true;
#else
                                                                         false;
#endif

                        // case 4

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP11_TUPLE) || \
    defined(BSLS_LIBRARYFEATURES_HAS_CPP11_TUPLE_FORCE)

    #include <tuple>
    std::tuple<char, short, int, float, double> t4;

    #ifndef BSLS_COMPILERFEATURES_SUPPORT_VARIADIC_TEMPLATES
    #error "'BSLS_LIBRARYFEATURES_HAS_CPP11_TUPLE' requires \
            'BSLS_COMPILERFEATURES_SUPPORT_VARIADIC_TEMPLATES'"
    #endif

#endif

static const
bool   BSLS_LIBRARYFEATURES_HAS_CPP11_TUPLE_defined =
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_TUPLE
                                                                          true;
#else
                                                                         false;
#endif

                    // case 3

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP11_PAIR_PIECEWISE_CONSTRUCTOR) || \
    defined(BSLS_LIBRARYFEATURES_HAS_CPP11_PAIR_PIECEWISE_CONSTRUCTOR_FORCE)

    #include <tuple>
    #include <utility> // for `pair` and `piecewise_construct`

    std::pair<long, double> p(std::piecewise_construct,
                              std::tuple<int>(1),
                              std::tuple<int>(2));

    #ifndef BSLS_LIBRARYFEATURES_HAS_CPP11_TUPLE
    #error "'BSLS_LIBRARYFEATURES_HAS_CPP11_PAIR_PIECEWISE_CONSTRUCTOR' \
            requires 'BSLS_LIBRARYFEATURES_HAS_CPP11_TUPLE'"
    #endif

    #ifndef BSLS_COMPILERFEATURES_SUPPORT_VARIADIC_TEMPLATES
    #error "'BSLS_LIBRARYFEATURES_HAS_CPP11_PAIR_PIECEWISE_CONSTRUCTOR' \
            requires 'BSLS_COMPILERFEATURES_SUPPORT_VARIADIC_TEMPLATES'"
    #endif

#endif

static const
bool   BSLS_LIBRARYFEATURES_HAS_CPP11_PAIR_PIECEWISE_CONSTRUCTOR_defined =
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_PAIR_PIECEWISE_CONSTRUCTOR
                                                                          true;
#else
                                                                         false;
#endif
                    // case 2

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY) || \
    defined(BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY_FORCE)

                // ----------------------------------------
                // class SimpleUniformRandomNumberGenerator
                // ----------------------------------------

    /// This class defines a simple, easily tested uniform random number
    /// generator that can be validly used as the third argument to the
    /// `std::shuffle` function in the `useCpp11Algorithms()` test
    /// function.
    class SimpleUniformRandomNumberGenerator {

      public:
        // TYPES

        /// Alias for the type of the values returned by this class.
        typedef unsigned result_type;

        // CLASS METHODS

        /// Return 0, the smallest value returned by `operator()`.
        static BSLS_KEYWORD_CONSTEXPR unsigned min();

        /// Return the largest value *potentially* returned by `operator()`.
        /// The C++ standard requires that this returned value be greater
        /// than the value returned by the `min` method; consequentally,
        /// this method returns 1 even though `operator()(void)` always
        /// returns 0.
        static BSLS_KEYWORD_CONSTEXPR unsigned max();

        // CREATORS
        //! SimpleUniformRandomNumberGenerator() = default;
            // Create an `SimpleUniformRandomNumberGenerator` object.

        //! ~SimpleUniformRandomNumberGenerator() = default;
            // Destroy this `SimpleUniformRandomNumberGenerator` object.

        // ACCESSORS

        /// Return the next value from this `SimpleUniformNumberGenerator`
        /// object.  For this generator, that value is always 0.
        unsigned operator()(void) const;
    };

                // ========================================
                // class SimpleUniformRandomNumberGenerator
                // ========================================

    // CLASS METHODS
    BSLS_KEYWORD_CONSTEXPR unsigned SimpleUniformRandomNumberGenerator::min()
    {
        return 0;
    }
    BSLS_KEYWORD_CONSTEXPR unsigned SimpleUniformRandomNumberGenerator::max()
    {
        return 1;
    }

    unsigned SimpleUniformRandomNumberGenerator::operator()(void) const
    {
        return 0;
    }

    static void testSimpleUniformRandomNumberGenerator()
    {
        ASSERT(SimpleUniformRandomNumberGenerator::min()
             < SimpleUniformRandomNumberGenerator::max());

        ASSERT(0 == SimpleUniformRandomNumberGenerator::min());
        ASSERT(1 == SimpleUniformRandomNumberGenerator::max());

        SimpleUniformRandomNumberGenerator surng;

        ASSERT(0 == surng());
        ASSERT(0 == surng());
        ASSERT(0 == surng());
        ASSERT(0 == surng());
    };

    #include <algorithm>
    #include <ios>
    #include <memory>         // for `uninitialized_copy_n`
    #include <numeric>        // for `iota`
    #include <sstream>

    /// Return `true` irrespective of the (ignored) input argument.  Used to
    /// instantiate function templates in the `useCpp11Algorithms` test
    /// function.
    static bool unaryPredicate(int)
    {
        return true;
    }

    /// Use each of the function templates associated with the
    /// `BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY` flag in a
    /// syntactically correct (and semantically meaningless) manner as a
    /// compile-time test that these templates are available.
    static void useCpp11Algorithms()
    {
        const int    iarray[]     = { 0, 1, 2 };
        const size_t NUM_ELEMENTS = sizeof  iarray / sizeof *iarray;

        const int *inputFirst = &iarray[0];
        const int *inputLast  = &iarray[NUM_ELEMENTS];

        (void)std::all_of(inputFirst, inputLast, unaryPredicate);
        (void)std::any_of(inputFirst, inputLast, unaryPredicate);

        int  oarray[NUM_ELEMENTS];
        int *outputFirst = &oarray[0];
        int *outputLast  = &oarray[NUM_ELEMENTS];

        std::copy_if(inputFirst,
                     inputLast,
                     outputFirst,
                     unaryPredicate);

        std::copy_n (inputFirst, NUM_ELEMENTS, outputFirst);

        (void)std::find_if_not(inputFirst, inputLast, unaryPredicate);

        std::iota(outputFirst, outputLast, 0);

        (void)std::is_heap      (inputFirst, inputLast);
        (void)std::is_heap_until(inputFirst, inputLast);

        (void)std::is_partitioned (inputFirst, inputLast, unaryPredicate);
        (void)std::is_permutation (inputFirst, inputLast, inputFirst);
        (void)std::is_sorted      (inputFirst, inputLast);
        (void)std::is_sorted_until(inputFirst, inputLast);

        (void)std::minmax(0, 1);
        (void)std::minmax_element(inputFirst, inputLast);

        int  oarray2[NUM_ELEMENTS];
        int *output2First = &oarray2[0];
        int *output2Last  = &oarray2[NUM_ELEMENTS];

        std::move         (outputFirst, outputLast, output2First);
        std::move_backward(outputFirst, outputLast, output2Last);

        (void)std::none_of(inputFirst, inputLast, unaryPredicate);

        std::partition_copy(inputFirst,
                            inputLast,
                            outputFirst,
                            output2First,
                            unaryPredicate);

        (void)std::partition_point(inputFirst, inputLast, unaryPredicate);

        SimpleUniformRandomNumberGenerator surng;
        std::shuffle(outputFirst,
                     outputLast,
                     surng);

        std::uninitialized_copy_n(inputFirst,
                                  NUM_ELEMENTS,
                                  outputFirst);

        // test <ios> C++11 functions
        const std::error_category& errorCategory = std::iostream_category();
        (void)errorCategory;

        (void)std::make_error_code(std::io_errc::stream);
        (void)std::make_error_condition(std::io_errc::stream);

        ASSERT(true == std::is_error_code_enum<std::io_errc>::value);

        double f;
        std::istringstream("0x1P-1022") >> std::hexfloat >> f;
        std::istringstream("0.01") >> std::defaultfloat >> f;
    }

    static void useTypeIndex() {
        const std::type_info &info  = typeid(int);
        const std::type_index index = info;

        ASSERT(info.hash_code() == index.hash_code());
    }
#endif

static const
bool   BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY_defined =
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY
                                                                          true;
#else
                                                                         false;
#endif

                    // case 1

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP98_AUTO_PTR) || \
    defined(BSLS_LIBRARYFEATURES_HAS_CPP98_AUTO_PTR_FORCE)

#ifdef BSLS_PLATFORM_HAS_PRAGMA_GCC_DIAGNOSTIC
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#endif
    #include <memory>
    std::auto_ptr<int> ap;
#ifdef BSLS_PLATFORM_HAS_PRAGMA_GCC_DIAGNOSTIC
#pragma GCC diagnostic pop
#endif

#endif

static const
bool   BSLS_LIBRARYFEATURES_HAS_CPP98_AUTO_PTR_defined =
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP98_AUTO_PTR
                                                                          true;
#else
                                                                         false;
#endif

using namespace BloombergLP;

// ============================================================================
//                              USAGE EXAMPLE
// ----------------------------------------------------------------------------

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
// For example, the `tuple`-type is not available in older versions of the
// native standard library.  Suppose we have a utility component that returns
// an instance of a `tuple` of values *if* the underlying version of the
// standard library provides that type, and yet remain compilable otherwise.
//
// First, we conditionally include the header file we will need if we define an
// interface that returns a `std::tuple`.
// ```
    #if defined(BSLS_LIBRARYFEATURES_HAS_CPP11_TUPLE)
    # include <tuple>
    #endif // BSLS_LIBRARYFEATURES_HAS_CPP11_TUPLE
// ```
// Then, we declare the methods that will be unconditionally provided by our
// utility component:
// ```
    struct MyStatisticalUtil
    {
        /// Return the median (mean, variance) of the sequence of values in
        /// the specified non-empty, semi-open range `[begin, end)`.  The
        /// behavior is undefined unless `begin < end`.
        static double     mean(const int *begin, const int *end);
        static int      median(const int *begin, const int *end);
        static double variance(const int *begin, const int *end);
// ```
// Now, we conditionally define an interface that returns a `std::tuple`, if
// that type is available.  Note that, if all three values are needed, calling
// this interface is more efficient than calling the earlier three individually
// because the input need be traversed one time, not three, and if C++17 is
// enabled the result may be used to create a structured binding.
// ```
    #ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_TUPLE
        /// Return the median, mean, and variance (in that order) of the
        /// sequence of values in the specified non-empty, semi-open range
        /// `[begin, end)`.  The behavior is undefined unless `begin < end`.
        static std::tuple<int, double, double> getMedianMeanVariance(
                                                              const int *begin,
                                                              const int *end);

    #endif // BSLS_LIBRARYFEATURES_HAS_CPP11_TUPLE
    };
// ```
// Finally, we find that our code compiles when we build our code against
// versions of the native standard library that provide a `tuple` type, *and*
// those that do not.  Of course, in the later case the interface that returns
// a `tuple` is not defined for the `MyStatisticalUtil` `struct`.

// ============================================================================
//                              HELPER FUNCTIONS
// ----------------------------------------------------------------------------

/// Print a diagnostic message to standard output if any of the preprocessor
/// flags of interest are defined, and their value if a value had been set.
/// An "Enter" and "Leave" message is printed unconditionally so there is
/// some report even if all of the flags are undefined.
static void printFlags()
{
    puts("printFlags: Enter");
    puts("\t__cplusplus:\t" STRINGIFY(__cplusplus));

    puts("\nprintFlags: bsls_libraryfeatures Macros");

    printf("\tBSLS_LIBRARYFEATURES_DETECTION_IN_PROGRESS:\t");
#ifdef BSLS_LIBRARYFEATURES_DETECTION_IN_PROGRESS
    puts(STRINGIFY(BSLS_LIBRARYFEATURES_DETECTION_IN_PROGRESS));
#else
    puts("UNDEFINED");
#endif

    printf("\tBSLS_LIBRARYFEATURES_HAS_C90_GETS:\t");
#ifdef BSLS_LIBRARYFEATURES_HAS_C90_GETS
    puts(STRINGIFY(BSLS_LIBRARYFEATURES_HAS_C90_GETS));
#else
    puts("UNDEFINED");
#endif

    printf("\tBSLS_LIBRARYFEATURES_HAS_C99_FP_CLASSIFY:\t");
#ifdef BSLS_LIBRARYFEATURES_HAS_C99_FP_CLASSIFY
    puts(STRINGIFY(BSLS_LIBRARYFEATURES_HAS_C99_FP_CLASSIFY));
#else
    puts("UNDEFINED");
#endif

    printf("\tBSLS_LIBRARYFEATURES_HAS_C99_LIBRARY:\t");
#ifdef BSLS_LIBRARYFEATURES_HAS_C99_LIBRARY
    puts(STRINGIFY(BSLS_LIBRARYFEATURES_HAS_C99_LIBRARY));
#else
    puts("UNDEFINED");
#endif

    printf("\tBSLS_LIBRARYFEATURES_HAS_C99_SNPRINTF:\t");
#ifdef BSLS_LIBRARYFEATURES_HAS_C99_SNPRINTF
    puts(STRINGIFY(BSLS_LIBRARYFEATURES_HAS_C99_SNPRINTF));
#else
    puts("UNDEFINED");
#endif

    printf("\tBSLS_LIBRARYFEATURES_HAS_CPP98_AUTO_PTR:\t");
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP98_AUTO_PTR
    puts(STRINGIFY(BSLS_LIBRARYFEATURES_HAS_CPP98_AUTO_PTR));
#else
    puts("UNDEFINED");
#endif

    printf("\tBSLS_LIBRARYFEATURES_HAS_CPP98_BINDERS_API:\t");
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP98_BINDERS_API
    puts(STRINGIFY(BSLS_LIBRARYFEATURES_HAS_CPP98_BINDERS_API));
#else
    puts("UNDEFINED");
#endif

    printf("\tBSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY:\t");
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY
    puts(STRINGIFY(BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY));
#else
    puts("UNDEFINED");
#endif

    printf("\tBSLS_LIBRARYFEATURES_HAS_CPP11_DYNAMIC_EXCEPTION_SPECS:\t");
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_DYNAMIC_EXCEPTION_SPECS
    puts(STRINGIFY(BSLS_LIBRARYFEATURES_HAS_CPP11_DYNAMIC_EXCEPTION_SPECS));
#else
    puts("UNDEFINED");
#endif

    printf("\tBSLS_LIBRARYFEATURES_HAS_CPP11_EXCEPTION_HANDLING:\t");
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_EXCEPTION_HANDLING
    puts(STRINGIFY(BSLS_LIBRARYFEATURES_HAS_CPP11_EXCEPTION_HANDLING));
#else
    puts("UNDEFINED");
#endif

    printf("\tBSLS_LIBRARYFEATURES_HAS_CPP11_GARBAGE_COLLECTION_API:\t");
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_GARBAGE_COLLECTION_API
    puts(STRINGIFY(BSLS_LIBRARYFEATURES_HAS_CPP11_GARBAGE_COLLECTION_API));
#else
    puts("UNDEFINED");
#endif

    printf("\tBSLS_LIBRARYFEATURES_HAS_CPP11_MISCELLANEOUS_UTILITIES:\t");
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_MISCELLANEOUS_UTILITIES
    puts(STRINGIFY(BSLS_LIBRARYFEATURES_HAS_CPP11_MISCELLANEOUS_UTILITIES));
#else
    puts("UNDEFINED");
#endif

    printf("\tBSLS_LIBRARYFEATURES_HAS_CPP11_PAIR_PIECEWISE_CONSTRUCTOR:\t");
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_PAIR_PIECEWISE_CONSTRUCTOR
    puts(STRINGIFY(BSLS_LIBRARYFEATURES_HAS_CPP11_PAIR_PIECEWISE_CONSTRUCTOR));
#else
    puts("UNDEFINED");
#endif

    printf("\tBSLS_LIBRARYFEATURES_HAS_CPP11_PROGRAM_TERMINATION:\t");
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_PROGRAM_TERMINATION
    puts(STRINGIFY(BSLS_LIBRARYFEATURES_HAS_CPP11_PROGRAM_TERMINATION));
#else
    puts("UNDEFINED");
#endif

    printf("\tBSLS_LIBRARYFEATURES_HAS_CPP11_RANGE_FUNCTIONS:\t");
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_RANGE_FUNCTIONS
    puts(STRINGIFY(BSLS_LIBRARYFEATURES_HAS_CPP11_RANGE_FUNCTIONS));
#else
    puts("UNDEFINED");
#endif

    printf("\tBSLS_LIBRARYFEATURES_HAS_CPP11_SHORT_STRING:\t");
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_SHORT_STRING
    puts(STRINGIFY(BSLS_LIBRARYFEATURES_HAS_CPP11_SHORT_STRING));
#else
    puts("UNDEFINED");
#endif

    printf("\tBSLS_LIBRARYFEATURES_HAS_CPP11_STREAM_MOVE:\t");
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_STREAM_MOVE
    puts(STRINGIFY(BSLS_LIBRARYFEATURES_HAS_CPP11_STREAM_MOVE));
#else
    puts("UNDEFINED");
#endif

    printf("\tBSLS_LIBRARYFEATURES_HAS_CPP11_TUPLE:\t");
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_TUPLE
    puts(STRINGIFY(BSLS_LIBRARYFEATURES_HAS_CPP11_TUPLE));
#else
    puts("UNDEFINED");
#endif

    printf("\tBSLS_LIBRARYFEATURES_HAS_CPP11_UNIQUE_PTR:\t");
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_UNIQUE_PTR
    puts(STRINGIFY(BSLS_LIBRARYFEATURES_HAS_CPP11_UNIQUE_PTR));
#else
    puts("UNDEFINED");
#endif

    printf("\tBSLS_LIBRARYFEATURES_HAS_CPP14_BASELINE_LIBRARY:\t");
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP14_BASELINE_LIBRARY
    puts(STRINGIFY(BSLS_LIBRARYFEATURES_HAS_CPP14_BASELINE_LIBRARY));
#else
    puts("UNDEFINED");
#endif

    printf("\tBSLS_LIBRARYFEATURES_HAS_CPP14_INTEGER_SEQUENCE:\t");
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP14_INTEGER_SEQUENCE
    puts(STRINGIFY(BSLS_LIBRARYFEATURES_HAS_CPP14_INTEGER_SEQUENCE));
#else
    puts("UNDEFINED");
#endif

    printf("\tBSLS_LIBRARYFEATURES_HAS_CPP14_RANGE_FUNCTIONS:\t");
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP14_RANGE_FUNCTIONS
    puts(STRINGIFY(BSLS_LIBRARYFEATURES_HAS_CPP14_RANGE_FUNCTIONS));
#else
    puts("UNDEFINED");
#endif

    printf("\tBSLS_LIBRARYFEATURES_HAS_CPP17_ALIGNED_ALLOC:\t");
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_ALIGNED_ALLOC
    puts(STRINGIFY(BSLS_LIBRARYFEATURES_HAS_CPP17_ALIGNED_ALLOC));
#else
    puts("UNDEFINED");
#endif

    printf("\tBSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY:\t");
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY
    puts(STRINGIFY(BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY));
#else
    puts("UNDEFINED");
#endif

    printf("\tBSLS_LIBRARYFEATURES_HAS_CPP17_CHARCONV:\t");
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_CHARCONV
    puts(STRINGIFY(BSLS_LIBRARYFEATURES_HAS_CPP17_CHARCONV));
#else
    puts("UNDEFINED");
#endif

    printf("\tBSLS_LIBRARYFEATURES_HAS_CPP17_DEPRECATED_REMOVED:\t");
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_DEPRECATED_REMOVED
    puts(STRINGIFY(BSLS_LIBRARYFEATURES_HAS_CPP17_DEPRECATED_REMOVED));
#else
    puts("UNDEFINED");
#endif

    printf("\tBSLS_LIBRARYFEATURES_HAS_CPP17_FILESYSTEM:\t");
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_FILESYSTEM
    puts(STRINGIFY(BSLS_LIBRARYFEATURES_HAS_CPP17_FILESYSTEM));
#else
    puts("UNDEFINED");
#endif

    printf("\tBSLS_LIBRARYFEATURES_HAS_CPP17_INT_CHARCONV:\t");
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_INT_CHARCONV
    puts(STRINGIFY(BSLS_LIBRARYFEATURES_HAS_CPP17_INT_CHARCONV));
#else
    puts("UNDEFINED");
#endif

    printf("\tBSLS_LIBRARYFEATURES_HAS_CPP17_PARALLEL_ALGORITHMS:\t");
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_PARALLEL_ALGORITHMS
    puts(STRINGIFY(BSLS_LIBRARYFEATURES_HAS_CPP17_PARALLEL_ALGORITHMS));
#else
    puts("UNDEFINED");
#endif

    printf("\tBSLS_LIBRARYFEATURES_HAS_CPP17_PMR:\t");
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_PMR
    puts(STRINGIFY(BSLS_LIBRARYFEATURES_HAS_CPP17_PMR));
#else
    puts("UNDEFINED");
#endif

    printf("\tBSLS_LIBRARYFEATURES_HAS_CPP17_PMR_STRING:\t");
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_PMR_STRING
    puts(STRINGIFY(BSLS_LIBRARYFEATURES_HAS_CPP17_PMR_STRING));
#else
    puts("UNDEFINED");
#endif

    printf("\tBSLS_LIBRARYFEATURES_HAS_CPP17_PRECISE_BITWIDTH_ATOMICS:\t");
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_PRECISE_BITWIDTH_ATOMICS
    puts(STRINGIFY(BSLS_LIBRARYFEATURES_HAS_CPP17_PRECISE_BITWIDTH_ATOMICS));
#else
    puts("UNDEFINED");
#endif

    printf("\tBSLS_LIBRARYFEATURES_HAS_CPP17_RANGE_FUNCTIONS:\t");
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_RANGE_FUNCTIONS
    puts(STRINGIFY(BSLS_LIBRARYFEATURES_HAS_CPP17_RANGE_FUNCTIONS));
#else
    puts("UNDEFINED");
#endif

    printf("\tBSLS_LIBRARYFEATURES_HAS_CPP17_SEARCH_FUNCTORS:\t");
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_SEARCH_FUNCTORS
    puts(STRINGIFY(BSLS_LIBRARYFEATURES_HAS_CPP17_SEARCH_FUNCTORS));
#else
    puts("UNDEFINED");
#endif

    printf("\tBSLS_LIBRARYFEATURES_HAS_CPP17_SEARCH_OVERLOAD:\t");
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_SEARCH_OVERLOAD
    puts(STRINGIFY(BSLS_LIBRARYFEATURES_HAS_CPP17_SEARCH_OVERLOAD));
#else
    puts("UNDEFINED");
#endif

    printf("\tBSLS_LIBRARYFEATURES_HAS_CPP17_SPECIAL_MATH_FUNCTIONS:\t");
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_SPECIAL_MATH_FUNCTIONS
    puts(STRINGIFY(BSLS_LIBRARYFEATURES_HAS_CPP17_SPECIAL_MATH_FUNCTIONS));
#else
    puts("UNDEFINED");
#endif

    printf("\tBSLS_LIBRARYFEATURES_HAS_CPP17_TIMESPEC_GET:\t");
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_TIMESPEC_GET
    puts(STRINGIFY(BSLS_LIBRARYFEATURES_HAS_CPP17_TIMESPEC_GET));
#else
    puts("UNDEFINED");
#endif

    printf(
        "\tBSLS_LIBRARYFEATURES_HAS_CPP20_ATOMIC_FLAG_TEST_FREE_FUNCTIONS:\t");
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP20_ATOMIC_FLAG_TEST_FREE_FUNCTIONS
    puts(
    STRINGIFY(BSLS_LIBRARYFEATURES_HAS_CPP20_ATOMIC_FLAG_TEST_FREE_FUNCTIONS));
#else
    puts("UNDEFINED");
#endif

    printf(
          "\tBSLS_LIBRARYFEATURES_HAS_CPP20_ATOMIC_LOCK_FREE_TYPE_ALIASES:\t");
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP20_ATOMIC_LOCK_FREE_TYPE_ALIASES
    puts(
      STRINGIFY(BSLS_LIBRARYFEATURES_HAS_CPP20_ATOMIC_LOCK_FREE_TYPE_ALIASES));
#else
    puts("UNDEFINED");
#endif

    printf("\tBSLS_LIBRARYFEATURES_HAS_CPP20_ATOMIC_REF:\t");
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP20_ATOMIC_REF
    puts(STRINGIFY(BSLS_LIBRARYFEATURES_HAS_CPP20_ATOMIC_REF));
#else
    puts("UNDEFINED");
#endif

    printf("\tBSLS_LIBRARYFEATURES_HAS_CPP20_ATOMIC_WAIT_FREE_FUNCTIONS:\t");
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP20_ATOMIC_WAIT_FREE_FUNCTIONS
    puts(STRINGIFY(BSLS_LIBRARYFEATURES_HAS_CPP20_ATOMIC_WAIT_FREE_FUNCTIONS));
#else
    puts("UNDEFINED");
#endif

    printf("\tBSLS_LIBRARYFEATURES_HAS_CPP20_BASELINE_LIBRARY:\t");
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP20_BASELINE_LIBRARY
    puts(STRINGIFY(BSLS_LIBRARYFEATURES_HAS_CPP20_BASELINE_LIBRARY));
#else
    puts("UNDEFINED");
#endif

    printf("\tBSLS_LIBRARYFEATURES_HAS_CPP20_CALENDAR:\t");
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP20_CALENDAR
    puts(STRINGIFY(BSLS_LIBRARYFEATURES_HAS_CPP20_CALENDAR));
#else
    puts("UNDEFINED");
#endif

    printf("\tBSLS_LIBRARYFEATURES_HAS_CPP20_CHAR8_MB_CONV:\t");
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP20_CHAR8_MB_CONV
    puts(STRINGIFY(BSLS_LIBRARYFEATURES_HAS_CPP20_CHAR8_MB_CONV));
#else
    puts("UNDEFINED");
#endif

    printf("\tBSLS_LIBRARYFEATURES_HAS_CPP20_CONCEPTS:\t");
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP20_CONCEPTS
    puts(STRINGIFY(BSLS_LIBRARYFEATURES_HAS_CPP20_CONCEPTS));
#else
    puts("UNDEFINED");
#endif

    printf("\tBSLS_LIBRARYFEATURES_HAS_CPP20_DEPRECATED_REMOVED:\t");
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP20_DEPRECATED_REMOVED
    puts(STRINGIFY(BSLS_LIBRARYFEATURES_HAS_CPP20_DEPRECATED_REMOVED));
#else
    puts("UNDEFINED");
#endif

    printf("\tBSLS_LIBRARYFEATURES_HAS_CPP20_FORMAT:\t");
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP20_FORMAT
    puts(STRINGIFY(BSLS_LIBRARYFEATURES_HAS_CPP20_FORMAT));
#else
    puts("UNDEFINED");
#endif

    printf("\tBSLS_LIBRARYFEATURES_HAS_CPP20_IS_CORRESPONDING_MEMBER:\t");
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP20_IS_CORRESPONDING_MEMBER
    puts(STRINGIFY(BSLS_LIBRARYFEATURES_HAS_CPP20_IS_CORRESPONDING_MEMBER));
#else
    puts("UNDEFINED");
#endif

    printf("\tBSLS_LIBRARYFEATURES_HAS_CPP20_IS_LAYOUT_COMPATIBLE:\t");
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP20_IS_LAYOUT_COMPATIBLE
    puts(STRINGIFY(BSLS_LIBRARYFEATURES_HAS_CPP20_IS_LAYOUT_COMPATIBLE));
#else
    puts("UNDEFINED");
#endif

    printf("\tBSLS_LIBRARYFEATURES_HAS_CPP20_IS_POINTER_INTERCONVERTIBLE:\t");
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP20_IS_POINTER_INTERCONVERTIBLE
    puts(
        STRINGIFY(BSLS_LIBRARYFEATURES_HAS_CPP20_IS_POINTER_INTERCONVERTIBLE));
#else
    puts("UNDEFINED");
#endif

    printf("\tBSLS_LIBRARYFEATURES_HAS_CPP20_JTHREAD:\t");
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP20_JTHREAD
    puts(STRINGIFY(BSLS_LIBRARYFEATURES_HAS_CPP20_JTHREAD));
#else
    puts("UNDEFINED");
#endif

    printf("\tBSLS_LIBRARYFEATURES_HAS_CPP20_MAKE_UNIQUE_FOR_OVERWRITE:\t");
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP20_MAKE_UNIQUE_FOR_OVERWRITE
    puts(STRINGIFY(BSLS_LIBRARYFEATURES_HAS_CPP20_MAKE_UNIQUE_FOR_OVERWRITE));
#else
    puts("UNDEFINED");
#endif

    printf("\tBSLS_LIBRARYFEATURES_HAS_CPP20_RANGES:\t");
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP20_RANGES
    puts(STRINGIFY(BSLS_LIBRARYFEATURES_HAS_CPP20_RANGES));
#else
    puts("UNDEFINED");
#endif

    printf("\tBSLS_LIBRARYFEATURES_HAS_CPP20_SOURCE_LOCATION:\t");
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP20_SOURCE_LOCATION
    puts(STRINGIFY(BSLS_LIBRARYFEATURES_HAS_CPP20_SOURCE_LOCATION));
#else
    puts("UNDEFINED");
#endif

    printf("\tBSLS_LIBRARYFEATURES_HAS_CPP20_SYNCSTREAM:\t");
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP20_SYNCSTREAM
    puts(STRINGIFY(BSLS_LIBRARYFEATURES_HAS_CPP20_SYNCSTREAM));
#else
    puts("UNDEFINED");
#endif

    printf("\tBSLS_LIBRARYFEATURES_HAS_CPP20_TO_ARRAY:\t");
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP20_TO_ARRAY
    puts(STRINGIFY(BSLS_LIBRARYFEATURES_HAS_CPP20_TO_ARRAY));
#else
    puts("UNDEFINED");
#endif

    printf("\tBSLS_LIBRARYFEATURES_HAS_CPP20_TIMEZONE:\t");
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP20_TIMEZONE
    puts(STRINGIFY(BSLS_LIBRARYFEATURES_HAS_CPP20_TIMEZONE));
#else
    puts("UNDEFINED");
#endif

    printf("\tBSLS_LIBRARYFEATURES_HAS_CPP20_VERSION:\t");
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP20_VERSION
    puts(STRINGIFY(BSLS_LIBRARYFEATURES_HAS_CPP20_VERSION));
#else
    puts("UNDEFINED");
#endif

    printf("\tBSLS_LIBRARYFEATURES_HAS_CPP23_ALLOCATE_AT_LEAST:\t");
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP23_ALLOCATE_AT_LEAST
    puts(STRINGIFY(BSLS_LIBRARYFEATURES_HAS_CPP23_ALLOCATE_AT_LEAST));
#else
    puts("UNDEFINED");
#endif

    printf("\tBSLS_LIBRARYFEATURES_HAS_CPP23_BASELINE_LIBRARY:\t");
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP23_BASELINE_LIBRARY
    puts(STRINGIFY(BSLS_LIBRARYFEATURES_HAS_CPP23_BASELINE_LIBRARY));
#else
    puts("UNDEFINED");
#endif

    printf("\tBSLS_LIBRARYFEATURES_HAS_CPP23_BIND_BACK:\t");
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP23_BIND_BACK
    puts(STRINGIFY(BSLS_LIBRARYFEATURES_HAS_CPP23_BIND_BACK));
#else
    puts("UNDEFINED");
#endif

    printf("\tBSLS_LIBRARYFEATURES_HAS_CPP23_CONTAINERS_RANGES:\t");
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP23_CONTAINERS_RANGES
    puts(STRINGIFY(BSLS_LIBRARYFEATURES_HAS_CPP23_CONTAINERS_RANGES));
#else
    puts("UNDEFINED");
#endif

    printf("\tBSLS_LIBRARYFEATURES_HAS_CPP23_FORWARD_LIKE:\t");
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP23_FORWARD_LIKE
    puts(STRINGIFY(BSLS_LIBRARYFEATURES_HAS_CPP23_FORWARD_LIKE));
#else
    puts("UNDEFINED");
#endif

    printf("\tBSLS_LIBRARYFEATURES_HAS_CPP23_GENERATOR:\t");
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP23_GENERATOR
    puts(STRINGIFY(BSLS_LIBRARYFEATURES_HAS_CPP23_GENERATOR));
#else
    puts("UNDEFINED");
#endif

    printf("\tBSLS_LIBRARYFEATURES_HAS_CPP23_INT_CHARCONV:\t");
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP23_INT_CHARCONV
    puts(STRINGIFY(BSLS_LIBRARYFEATURES_HAS_CPP23_INT_CHARCONV));
#else
    puts("UNDEFINED");
#endif

    printf("\tBSLS_LIBRARYFEATURES_HAS_CPP23_IS_IMPLICIT_LIFETIME:\t");
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP23_IS_IMPLICIT_LIFETIME
    puts(STRINGIFY(BSLS_LIBRARYFEATURES_HAS_CPP23_IS_IMPLICIT_LIFETIME));
#else
    puts("UNDEFINED");
#endif

    printf("\tBSLS_LIBRARYFEATURES_HAS_CPP23_MDSPAN:\t");
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP23_MDSPAN
    puts(STRINGIFY(BSLS_LIBRARYFEATURES_HAS_CPP23_MDSPAN));
#else
    puts("UNDEFINED");
#endif

    printf("\tBSLS_LIBRARYFEATURES_HAS_CPP23_OUT_PTR:\t");
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP23_OUT_PTR
    puts(STRINGIFY(BSLS_LIBRARYFEATURES_HAS_CPP23_OUT_PTR));
#else
    puts("UNDEFINED");
#endif

    printf("\tBSLS_LIBRARYFEATURES_HAS_CPP23_PRINT:\t");
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP23_PRINT
    puts(STRINGIFY(BSLS_LIBRARYFEATURES_HAS_CPP23_PRINT));
#else
    puts("UNDEFINED");
#endif

    printf("\tBSLS_LIBRARYFEATURES_HAS_CPP23_RANGE_ADAPTOR_CLOSURE:\t");
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP23_RANGE_ADAPTOR_CLOSURE
    puts(STRINGIFY(BSLS_LIBRARYFEATURES_HAS_CPP23_RANGE_ADAPTOR_CLOSURE));
#else
    puts("UNDEFINED");
#endif

    printf("\tBSLS_LIBRARYFEATURES_HAS_CPP23_RANGE_FORMAT:\t");
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP23_RANGE_FORMAT
    puts(STRINGIFY(BSLS_LIBRARYFEATURES_HAS_CPP23_RANGE_FORMAT));
#else
    puts("UNDEFINED");
#endif

    printf("\tBSLS_LIBRARYFEATURES_HAS_CPP23_RANGES_AS_CONST:\t");
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP23_RANGES_IOTA
    puts(STRINGIFY(BSLS_LIBRARYFEATURES_HAS_CPP23_RANGES_AS_CONST));
#else
    puts("UNDEFINED");
#endif

    printf("\tBSLS_LIBRARYFEATURES_HAS_CPP23_RANGES_CARTESIAN_PRODUCT:\t");
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP23_RANGES_CARTESIAN_PRODUCT
    puts(STRINGIFY(BSLS_LIBRARYFEATURES_HAS_CPP23_RANGES_CARTESIAN_PRODUCT));
#else
    puts("UNDEFINED");
#endif

    printf("\tBSLS_LIBRARYFEATURES_HAS_CPP23_RANGES_CHUNK:\t");
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP23_RANGES_CHUNK
    puts(STRINGIFY(BSLS_LIBRARYFEATURES_HAS_CPP23_RANGES_CHUNK));
#else
    puts("UNDEFINED");
#endif

    printf("\tBSLS_LIBRARYFEATURES_HAS_CPP23_RANGES_CONTAINS:\t");
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP23_RANGES_CONTAINS
    puts(STRINGIFY(BSLS_LIBRARYFEATURES_HAS_CPP23_RANGES_CONTAINS));
#else
    puts("UNDEFINED");
#endif

    printf("\tBSLS_LIBRARYFEATURES_HAS_CPP23_RANGES_ENUMERATE:\t");
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP23_RANGES_ENUMERATE
    puts(STRINGIFY(BSLS_LIBRARYFEATURES_HAS_CPP23_RANGES_ENUMERATE));
#else
    puts("UNDEFINED");
#endif

    printf("\tBSLS_LIBRARYFEATURES_HAS_CPP23_RANGES_FIND_LAST:\t");
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP23_RANGES_FIND_LAST
    puts(STRINGIFY(BSLS_LIBRARYFEATURES_HAS_CPP23_RANGES_FIND_LAST));
#else
    puts("UNDEFINED");
#endif

    printf("\tBSLS_LIBRARYFEATURES_HAS_CPP23_RANGES_FOLD:\t");
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP23_RANGES_FOLD
    puts(STRINGIFY(BSLS_LIBRARYFEATURES_HAS_CPP23_RANGES_FOLD));
#else
    puts("UNDEFINED");
#endif

    printf("\tBSLS_LIBRARYFEATURES_HAS_CPP23_RANGES_IOTA:\t");
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP23_RANGES_IOTA
    puts(STRINGIFY(BSLS_LIBRARYFEATURES_HAS_CPP23_RANGES_IOTA));
#else
    puts("UNDEFINED");
#endif

    printf("\tBSLS_LIBRARYFEATURES_HAS_CPP23_RANGES_JOIN_WITH:\t");
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP23_RANGES_JOIN_WITH
    puts(STRINGIFY(BSLS_LIBRARYFEATURES_HAS_CPP23_RANGES_JOIN_WITH));
#else
    puts("UNDEFINED");
#endif

    printf("\tBSLS_LIBRARYFEATURES_HAS_CPP23_RANGES_SHIFT:\t");
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP23_RANGES_SHIFT
    puts(STRINGIFY(BSLS_LIBRARYFEATURES_HAS_CPP23_RANGES_SHIFT));
#else
    puts("UNDEFINED");
#endif

    printf("\tBSLS_LIBRARYFEATURES_HAS_CPP23_RANGES_SLIDE:\t");
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP23_RANGES_SLIDE
    puts(STRINGIFY(BSLS_LIBRARYFEATURES_HAS_CPP23_RANGES_SLIDE));
#else
    puts("UNDEFINED");
#endif

    printf("\tBSLS_LIBRARYFEATURES_HAS_CPP23_RANGES_STARTS_ENDS_WITH:\t");
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP23_RANGES_STARTS_ENDS_WITH
    puts(STRINGIFY(BSLS_LIBRARYFEATURES_HAS_CPP23_RANGES_STARTS_ENDS_WITH));
#else
    puts("UNDEFINED");
#endif

    printf("\tBSLS_LIBRARYFEATURES_HAS_CPP23_RANGES_STRIDE:\t");
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP23_RANGES_STRIDE
    puts(STRINGIFY(BSLS_LIBRARYFEATURES_HAS_CPP23_RANGES_STRIDE));
#else
    puts("UNDEFINED");
#endif

    printf("\tBSLS_LIBRARYFEATURES_HAS_CPP23_RANGES_TO_CONTAINER:\t");
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP23_RANGES_TO_CONTAINER
    puts(STRINGIFY(BSLS_LIBRARYFEATURES_HAS_CPP23_RANGES_TO_CONTAINER));
#else
    puts("UNDEFINED");
#endif

    printf("\tBSLS_LIBRARYFEATURES_HAS_CPP23_RANGES_ZIP:\t");
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP23_RANGES_ZIP
    puts(STRINGIFY(BSLS_LIBRARYFEATURES_HAS_CPP23_RANGES_ZIP));
#else
    puts("UNDEFINED");
#endif

    printf("\tBSLS_LIBRARYFEATURES_HAS_CPP23_REFERENCE_FROM_TEMPORARY:\t");
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP23_REFERENCE_FROM_TEMPORARY
    puts(STRINGIFY(BSLS_LIBRARYFEATURES_HAS_CPP23_REFERENCE_FROM_TEMPORARY));
#else
    puts("UNDEFINED");
#endif

    printf("\tBSLS_LIBRARYFEATURES_HAS_CPP23_SPANSTREAM:\t");
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP23_SPANSTREAM
    puts(STRINGIFY(BSLS_LIBRARYFEATURES_HAS_CPP23_SPANSTREAM));
#else
    puts("UNDEFINED");
#endif

    printf("\tBSLS_LIBRARYFEATURES_HAS_CPP23_STACKTRACE:\t");
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP23_STACKTRACE
    puts(STRINGIFY(BSLS_LIBRARYFEATURES_HAS_CPP23_STACKTRACE));
#else
    puts("UNDEFINED");
#endif

    printf("\tBSLS_LIBRARYFEATURES_HAS_CPP23_START_LIFETIME_AS:\t");
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP23_START_LIFETIME_AS
    puts(STRINGIFY(BSLS_LIBRARYFEATURES_HAS_CPP23_START_LIFETIME_AS));
#else
    puts("UNDEFINED");
#endif

    printf("\tBSLS_LIBRARYFEATURES_STDCPP_GNU:\t");
#ifdef BSLS_LIBRARYFEATURES_STDCPP_GNU
    puts(STRINGIFY(BSLS_LIBRARYFEATURES_STDCPP_GNU));
#else
    puts("UNDEFINED");
#endif

    printf("\tBSLS_LIBRARYFEATURES_STDCPP_IBM:\t");
#ifdef BSLS_LIBRARYFEATURES_STDCPP_IBM
    puts(STRINGIFY(BSLS_LIBRARYFEATURES_STDCPP_IBM));
#else
    puts("UNDEFINED");
#endif

    printf("\tBSLS_LIBRARYFEATURES_STDCPP_INTELLISENSE:\t");
#ifdef BSLS_LIBRARYFEATURES_STDCPP_INTELLISENSE
    puts(STRINGIFY(BSLS_LIBRARYFEATURES_STDCPP_INTELLISENSE));
#else
    puts("UNDEFINED");
#endif

    printf("\tBSLS_LIBRARYFEATURES_STDCPP_LIBCSTD:\t");
#ifdef BSLS_LIBRARYFEATURES_STDCPP_LIBCSTD
    puts(STRINGIFY(BSLS_LIBRARYFEATURES_STDCPP_LIBCSTD));
#else
    puts("UNDEFINED");
#endif

    printf("\tBSLS_LIBRARYFEATURES_STDCPP_LLVM:\t");
#ifdef BSLS_LIBRARYFEATURES_STDCPP_LLVM
    puts(STRINGIFY(BSLS_LIBRARYFEATURES_STDCPP_LLVM));
#else
    puts("UNDEFINED");
#endif

    printf("\tBSLS_LIBRARYFEATURES_STDCPP_MSVC:\t");
#ifdef BSLS_LIBRARYFEATURES_STDCPP_MSVC
    puts(STRINGIFY(BSLS_LIBRARYFEATURES_STDCPP_MSVC));
#else
    puts("UNDEFINED");
#endif

    printf("\tBSLS_LIBRARYFEATURES_STDCPP_STLPORT:\t");
#ifdef BSLS_LIBRARYFEATURES_STDCPP_STLPORT
    puts(STRINGIFY(BSLS_LIBRARYFEATURES_STDCPP_STLPORT));
#else
    puts("UNDEFINED");
#endif

    printf("\tBSLS_LIBRARYFEATURES_SUSPECT_CLANG_WITH_GLIBCPP:\t");
#ifdef BSLS_LIBRARYFEATURES_SUSPECT_CLANG_WITH_GLIBCPP
    puts(STRINGIFY(BSLS_LIBRARYFEATURES_SUSPECT_CLANG_WITH_GLIBCPP));
#else
    puts("UNDEFINED");
#endif

    puts("\nprintFlags: bsls_libraryfeatures Referenced Macros");

    printf("\tBSLS_COMPILERFEATURES_SUPPORT_ALIAS_TEMPLATES:\t");
#ifdef BSLS_COMPILERFEATURES_SUPPORT_ALIAS_TEMPLATES
    puts(STRINGIFY(BSLS_COMPILERFEATURES_SUPPORT_ALIAS_TEMPLATES));
#else
    puts("UNDEFINED");
#endif

    printf("\tBSLS_COMPILERFEATURES_SUPPORT_HAS_INCLUDE:\t");
#ifdef BSLS_COMPILERFEATURES_SUPPORT_HAS_INCLUDE
    puts(STRINGIFY(BSLS_COMPILERFEATURES_SUPPORT_HAS_INCLUDE));
#else
    puts("UNDEFINED");
#endif

    printf("\tBSLS_COMPILERFEATURES_SUPPORT_VARIADIC_TEMPLATES:\t");
#ifdef BSLS_COMPILERFEATURES_SUPPORT_VARIADIC_TEMPLATES
    puts(STRINGIFY(BSLS_COMPILERFEATURES_SUPPORT_VARIADIC_TEMPLATES));
#else
    puts("UNDEFINED");
#endif

    printf("\tBSLS_PLATFORM_CMP_CLANG:\t");
#ifdef BSLS_PLATFORM_CMP_CLANG
    puts(STRINGIFY(BSLS_PLATFORM_CMP_CLANG));
#else
    puts("UNDEFINED");
#endif

    printf("\tBSLS_PLATFORM_CMP_GNU:\t");
#ifdef BSLS_PLATFORM_CMP_GNU
    puts(STRINGIFY(BSLS_PLATFORM_CMP_GNU));
#else
    puts("UNDEFINED");
#endif

    printf("\tBSLS_PLATFORM_CMP_IBM:\t");
#ifdef BSLS_PLATFORM_CMP_IBM
    puts(STRINGIFY(BSLS_PLATFORM_CMP_IBM));
#else
    puts("UNDEFINED");
#endif

    printf("\tBSLS_PLATFORM_CMP_MSVC:\t");
#ifdef BSLS_PLATFORM_CMP_MSVC
    puts(STRINGIFY(BSLS_PLATFORM_CMP_MSVC));
#else
    puts("UNDEFINED");
#endif

    printf("\tBSLS_PLATFORM_CMP_SUN:\t");
#ifdef BSLS_PLATFORM_CMP_SUN
    puts(STRINGIFY(BSLS_PLATFORM_CMP_SUN));
#else
    puts("UNDEFINED");
#endif

    puts("\tBSLS_PLATFORM_CMP_VERSION:\t"
         STRINGIFY(BSLS_PLATFORM_CMP_VERSION));

    printf("\t_CPPLIB_VER:\t");
#ifdef _CPPLIB_VER
    puts(STRINGIFY(_CPPLIB_VER));
#else
    puts("UNDEFINED");
#endif

    puts("\nprintFlags: Platform Detection Macros");

    printf("\t_GLIBCXX_HAVE_AT_QUICK_EXIT:\t");
#ifdef _GLIBCXX_HAVE_AT_QUICK_EXIT
    puts(STRINGIFY(_GLIBCXX_HAVE_AT_QUICK_EXIT));
#else
    puts("UNDEFINED");
#endif

    printf("\t_GLIBCXX_HAVE_QUICK_EXIT:\t");
#ifdef _GLIBCXX_HAVE_QUICK_EXIT
    puts(STRINGIFY(_GLIBCXX_HAVE_QUICK_EXIT));
#else
    puts("UNDEFINED");
#endif

    printf("\t_LIBCPP_VERSION:\t");
#ifdef _LIBCPP_VERSION
    puts(STRINGIFY(_LIBCPP_VERSION));
#else
    puts("UNDEFINED");
#endif

    printf("\t_RWSTD_VER:\t");
#ifdef _RWSTD_VER
    puts(STRINGIFY(_RWSTD_VER));
#else
    puts("UNDEFINED");
#endif

    printf("\t_YVALS:\t");
#ifdef _YVALS
    puts(STRINGIFY(_YVALS));
#else
    puts("UNDEFINED");
#endif

    printf("\t__APPLE_CC__:\t");
#ifdef __APPLE_CC__
    puts(STRINGIFY(__APPLE_CC__));
#else
    puts("UNDEFINED");
#endif

    printf("\t__apple_build_version__:\t");
#ifdef __apple_build_version__
    puts(STRINGIFY(__apple_build_version__));
#else
    puts("UNDEFINED");
#endif

    printf("\t__GLIBC__:\t");
#ifdef __GLIBC__
    puts(STRINGIFY(__GLIBC__));
#else
    puts("UNDEFINED");
#endif

    printf("\t__GLIBCPP__:\t");
#ifdef __GLIBCPP__
    puts(STRINGIFY(__GLIBCPP__));
#else
    puts("UNDEFINED");
#endif

    printf("\t__GLIBCXX__:\t");
#ifdef __GLIBCXX__
    puts(STRINGIFY(__GLIBCXX__));
#else
    puts("UNDEFINED");
#endif

    printf("\t__GXX_EXPERIMENTAL_CXX0X__:\t");
#ifdef __GXX_EXPERIMENTAL_CXX0X__
    puts(STRINGIFY(__GXX_EXPERIMENTAL_CXX0X__));
#else
    puts("UNDEFINED");
#endif

    printf("\t__IBMCPP__:\t");
#ifdef __IBMCPP__
    puts(STRINGIFY(__IBMCPP__));
#else
    puts("UNDEFINED");
#endif

    printf("\t__INTELLISENSE__:\t");
#ifdef __INTELLISENSE__
    puts(STRINGIFY(__INTELLISENSE__));
#else
    puts("UNDEFINED");
#endif

    printf("\t__SGI_STL_PORT:\t");
#ifdef __SGI_STL_PORT
    puts(STRINGIFY(__SGI_STL_PORT));
#else
    puts("UNDEFINED");
#endif

    printf("\t__STD_RWCOMPILER_H__:\t");
#ifdef __STD_RWCOMPILER_H__
    puts(STRINGIFY(__STD_RWCOMPILER_H__));
#else
    puts("UNDEFINED");
#endif

    printf("\t__STLPORT_VERSION:\t");
#ifdef __STLPORT_VERSION
    puts(STRINGIFY(__STLPORT_VERSION));
#else
    puts("UNDEFINED");
#endif

    printf("\t__cpp_lib_atomic_is_always_lock_free:\t");
#ifdef __cpp_lib_atomic_is_always_lock_free
    puts(STRINGIFY(__cpp_lib_atomic_is_always_lock_free));
#else
    puts("UNDEFINED");
#endif

    puts("\nprintFlags: Leave");
}

// ============================================================================
//                           COMPILE TIME TESTS
// ----------------------------------------------------------------------------

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP23_RANGE_FORMAT
// This code verifies that we can disable range formatting for  range-looking
// type by specializing the `std::format_kind` variable template.

struct LooksLikeRange {
    char *begin();
    char *end();

    const char *begin() const;
    const char *end() const;
};

template <>
inline
constexpr std::range_format std::format_kind<LooksLikeRange> =
                                                   std::range_format::disabled;

static_assert(std::ranges::range<LooksLikeRange>);

#endif  // BSLS_LIBRARYFEATURES_HAS_CPP23_RANGE_FORMAT

// ============================================================================
//                              MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int                 test = argc > 1 ? atoi(argv[1]) : 0;
    bool             verbose = argc > 2;
    bool         veryVerbose = argc > 3;
    bool     veryVeryVerbose = argc > 4;

    (void)       veryVerbose;  // unused variable warning
    (void)   veryVeryVerbose;  // unused variable warning

    setbuf(stdout, NULL);    // Use unbuffered output

    printf("TEST " __FILE__ " CASE %d\n", test);

    if (veryVeryVerbose) {
        printFlags();
    }

    switch (test) { case 0:
      case 27: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //   Extracted from component header file.
        //
        // Concerns:
        // 1. The usage example provided in the component header file compiles,
        //    links, and runs as shown.
        //
        // Plan:
        // 1. Incorporate usage example from header into test driver, remove
        //    leading comment characters, and replace `assert` with `ASSERT`.
        //    (C-1)
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) puts("\nUSAGE EXAMPLE"
                          "\n=============");
      } break;
      case 26: {
        // --------------------------------------------------------------------
        // `BSLS_LIBRARYFEATURES_HAS_CPP23_*` MISCELLANY
        //
        // Concerns:
        // 1. If `BSLS_LIBRARYFEATURES_HAS_CPP23_BIND_BACK` is defined, the
        //    `std::bind_back` function is available.
        //
        // 2. If `BSLS_LIBRARYFEATURES_HAS_CPP23_FORWARD_LIKE` is defined, the
        //    `std::forward_like` function is available.
        //
        // 3. If `BSLS_LIBRARYFEATURES_HAS_CPP23_RANGES_IOTA` is defined, the
        //    `std::ranges::iota` function, the `bsl::ranges::iota_result`
        //    class and the `bsl::ranges::out_value_result` class are
        //    available.
        //
        // 4. If `BSLS_LIBRARYFEATURES_HAS_CPP23_RANGES_AS_CONST` is defined,
        //    the following names are available:
        //     - `std::const_iterator`
        //     - `std::const_sentinel`
        //     - `std::basic_const_iterator`
        //     - `std::make_const_iterator`
        //     - `std::make_const_sentinel`
        //     - `std::iter_const_reference_t`
        //     - `std::ranges::const_iterator_t`
        //     - `std::ranges::const_sentinel_t`
        //     - `std::ranges::range_const_reference_t`
        //     - `std::ranges::constant_range`
        //     - `std::ranges::as_const_view`
        //     - `std::views::as_const`
        //
        // 5. If `BSLS_LIBRARYFEATURES_HAS_CPP23_RANGES_SHIFT` is defined, the
        //    `std::ranges::shift_left` and `std::ranges::shift_right`
        //    functions are available.
        //
        // 6. If `BSLS_LIBRARYFEATURES_HAS_CPP23_RANGES_STARTS_ENDS_WITH` is
        //    defined, the `std::ranges::starts_with` and
        //    `std::ranges::ends_with` functions are available.
        //
        // 7. If `BSLS_LIBRARYFEATURES_HAS_CPP23_RANGES_FIND_LAST` is defined,
        //    the `std::ranges::find_last`, `std::ranges::find_last_if` and
        //    `std::ranges::find_last_if_not` functions are available.
        //
        // 8. If `BSLS_LIBRARYFEATURES_HAS_CPP23_RANGES_CONTAINS` is defined,
        //    the `std::ranges::contains` and `std::ranges::contains_subrange`
        //    functions are available.
        //
        // 9. If `BSLS_LIBRARYFEATURES_HAS_CPP23_RANGES_FOLD` is defined, the
        //    `std::ranges::fold_*` family of functions and the
        //    `std::ranges::in_value_result` class are available.
        //
        //10. If `BSLS_LIBRARYFEATURES_HAS_CPP23_IS_IMPLICIT_LIFETIME` is
        //    defined, the `std::is_implicit_lifetime` and
        //    `std::is_implicit_lifetime_v` traits are available.
        //
        //11. If `BSLS_LIBRARYFEATURES_HAS_CPP23_REFERENCE_FROM_TEMPORARY` is
        //    defined, the `std::reference_constructs_from_temporary`,
        //    `std::reference_constructs_from_temporary_v`,
        //    `std::reference_converts_from_temporary` and
        //    `std::reference_converts_from_temporary` traits are available.
        //
        //12. If `BSLS_LIBRARYFEATURES_HAS_CPP23_CONTAINERS_RANGES` is defined,
        //    the `std::from_range_t` type and the `std::from_range` value are
        //    available.
        //
        //13. If `BSLS_LIBRARYFEATURES_HAS_CPP23_RANGES_TO_CONTAINER` is
        //    defined, the `std::ranges::to` function is available.
        //
        //14. If `BSLS_LIBRARYFEATURES_HAS_CPP23_RANGE_ADAPTOR_CLOSURE` is
        //    defined, the `std::ranges::range_adaptor_closure` class is
        //    available.
        //
        //15. If `BSLS_LIBRARYFEATURES_HAS_CPP23_RANGES_CARTESIAN_PRODUCT` is
        //    defined, the `std::views::cartesian_product` view and the
        //    `std::ranges::cartesian_product_view` class are available.
        //
        //16. If `BSLS_LIBRARYFEATURES_HAS_CPP23_RANGES_CHUNK` is defined,
        //    the `std::views::chunk` view and the `std::ranges::chunk_view`
        //    class are available.
        //
        //17. If `BSLS_LIBRARYFEATURES_HAS_CPP23_RANGES_ENUMERATE` is defined,
        //    the `std::views::enumerate` view and the
        //    `std::ranges::enumerate_view` class are available.
        //
        //18. If `BSLS_LIBRARYFEATURES_HAS_CPP23_RANGES_JOIN_WITH` is defined,
        //    the `std::views::join_with` view and the
        //    `std::ranges::join_with_view` class are available.
        //
        //19. If `BSLS_LIBRARYFEATURES_HAS_CPP23_RANGES_SLIDE` is defined, the
        //    `std::views::slide` view and the `std::ranges::slide_view` class
        //    are available.
        //
        //20. If `BSLS_LIBRARYFEATURES_HAS_CPP23_RANGES_STRIDE` is defined, the
        //    `std::views::stride` view and the `std::ranges::stride_view`
        //    class are available.
        //
        //21. If `BSLS_LIBRARYFEATURES_HAS_CPP23_RANGES_ZIP` is defined, the
        //    following names are available:
        //     - `std::views::adjacent`, `std::ranges::adjacent_view`
        //     - `std::views::adjacent_transform',
        //       `std::ranges::adjacent_transform_view`
        //     - `std::views::zip`, `std::ranges::zip_view`
        //     - `std::views::zip_transform`, `std::ranges::zip_transform_view`
        //
        //22. If `BSLS_LIBRARYFEATURES_HAS_CPP23_GENERATOR` is defined, the
        //    following names are available:
        //     - `std::ranges::elements_of`
        //     - `std::generator`
        //
        //23. If `BSLS_LIBRARYFEATURES_HAS_CPP23_ALLOCATE_AT_LEAST` is defined,
        //    the following names are available:
        //     - `std::allocator::allocate_at_least`
        //     - `std::allocator_traits::allocate_at_least`
        //     - `std::allocation_result`
        //
        //24. If `BSLS_LIBRARYFEATURES_HAS_CPP23_OUT_PTR` is defined, the
        //    following names are available:
        //     - `std::out_ptr`, `std::out_ptr_t`
        //     - `std::inout_ptr`, `std::inout_ptr_t`
        //
        //25. If `BSLS_LIBRARYFEATURES_HAS_CPP23_START_LIFETIME_AS` is defined,
        //    the `std::start_lifetime_as` and `std::start_lifetime_as_array`
        //    functions are available.
        //
        //26. If `BSLS_LIBRARYFEATURES_HAS_CPP23_MDSPAN` is defined, the
        //    following names are available:
        //     - `std::default_accessor`
        //     - `std::dextents`
        //     - `std::extents`
        //     - `std::layout_left`
        //     - `std::layout_right`
        //     - `std::layout_stride`
        //     - `std::mdspan`
        //
        //27. If `BSLS_LIBRARYFEATURES_HAS_CPP23_SPANSTREAM` is defined, the
        //    following names are available:
        //     - `std::basic_ispanstream`
        //     - `std::basic_ospanstream`
        //     - `std::basic_spanbuf`
        //     - `std::basic_spanstream`
        //     - `std::ispanstream`
        //     - `std::ospanstream`
        //     - `std::spanbuf`
        //     - `std::spanstream`
        //     - `std::wspanbuf`
        //     - `std::wispanstream`
        //     - `std::wospanstream`
        //     - `std::wspanstream`
        //
        //28. If `BSLS_LIBRARYFEATURES_HAS_CPP23_PRINT` is defined, the
        //    following names are available:
        //     - `std::print`
        //     - `std::println` (incl. no-args version)
        //     - `std::vprint_nonunicode`
        //     - `std::vprint_nonunicode_buffered`
        //     - `std::vprint_unicode`
        //     - `std::vprint_unicode_buffered`
        //
        //29. If `BSLS_LIBRARYFEATURES_HAS_CPP23_RANGE_FORMAT` is defined,
        //    `std::format` can format a range.
        //
        //30. If `BSLS_LIBRARYFEATURES_HAS_CPP23_INT_CHARCONV` is defined, the
        //    `std::to_chars` and `std::from_chars` functions for integers in
        //    `<charconv>` are `constexpr`.
        //
        //31. The corresponding standard feature test macros are defined and
        //    have values in the expected range.
        //
        // Plan:
        // 1. When these macros are defined include the appropriate headers and
        //    use the expected names.
        //
        // 2. Verify the corresponding standard feature test macro if exists.
        //
        // Testing:
        //   BSLS_LIBRARYFEATURES_HAS_CPP23_BIND_BACK
        //   BSLS_LIBRARYFEATURES_HAS_CPP23_FORWARD_LIKE
        //   BSLS_LIBRARYFEATURES_HAS_CPP23_RANGES_IOTA
        //   BSLS_LIBRARYFEATURES_HAS_CPP23_RANGES_AS_CONST
        //   BSLS_LIBRARYFEATURES_HAS_CPP23_RANGES_SHIFT
        //   BSLS_LIBRARYFEATURES_HAS_CPP23_RANGES_STARTS_ENDS_WITH
        //   BSLS_LIBRARYFEATURES_HAS_CPP23_RANGES_FIND_LAST
        //   BSLS_LIBRARYFEATURES_HAS_CPP23_RANGES_CONTAINS
        //   BSLS_LIBRARYFEATURES_HAS_CPP23_RANGES_FOLD
        //   BSLS_LIBRARYFEATURES_HAS_CPP23_IS_IMPLICIT_LIFETIME
        //   BSLS_LIBRARYFEATURES_HAS_CPP23_REFERENCE_FROM_TEMPORARY
        //   BSLS_LIBRARYFEATURES_HAS_CPP23_CONTAINERS_RANGES
        //   BSLS_LIBRARYFEATURES_HAS_CPP23_RANGES_TO_CONTAINER
        //   BSLS_LIBRARYFEATURES_HAS_CPP23_RANGE_ADAPTOR_CLOSURE
        //   BSLS_LIBRARYFEATURES_HAS_CPP23_RANGES_CARTESIAN_PRODUCT
        //   BSLS_LIBRARYFEATURES_HAS_CPP23_RANGES_CHUNK
        //   BSLS_LIBRARYFEATURES_HAS_CPP23_RANGES_ENUMERATE
        //   BSLS_LIBRARYFEATURES_HAS_CPP23_RANGES_JOIN_WITH
        //   BSLS_LIBRARYFEATURES_HAS_CPP23_RANGES_SLIDE
        //   BSLS_LIBRARYFEATURES_HAS_CPP23_RANGES_STRIDE
        //   BSLS_LIBRARYFEATURES_HAS_CPP23_RANGES_ZIP
        //   BSLS_LIBRARYFEATURES_HAS_CPP23_GENERATOR
        //   BSLS_LIBRARYFEATURES_HAS_CPP23_ALLOCATE_AT_LEAST
        //   BSLS_LIBRARYFEATURES_HAS_CPP23_OUT_PTR
        //   BSLS_LIBRARYFEATURES_HAS_CPP23_START_LIFETIME_AS
        //   BSLS_LIBRARYFEATURES_HAS_CPP23_MDSPAN
        //   BSLS_LIBRARYFEATURES_HAS_CPP23_SPANSTREAM
        //   BSLS_LIBRARYFEATURES_HAS_CPP23_PRINT
        //   BSLS_LIBRARYFEATURES_HAS_CPP23_RANGE_FORMAT
        //   BSLS_LIBRARYFEATURES_HAS_CPP23_INT_CHARCONV
        // --------------------------------------------------------------------

        if (verbose)
            puts("\n'`BSLS_LIBRARYFEATURES_HAS_CPP23_*` MISCELLANY'"
                 "\n===============================================");

        if (verbose) {
            P(BSLS_LIBRARYFEATURES_HAS_CPP23_BIND_BACK_defined)
            P(BSLS_LIBRARYFEATURES_HAS_CPP23_FORWARD_LIKE_defined)
            P(BSLS_LIBRARYFEATURES_HAS_CPP23_RANGES_IOTA_defined)
            P(BSLS_LIBRARYFEATURES_HAS_CPP23_RANGES_AS_CONST_defined)
            P(BSLS_LIBRARYFEATURES_HAS_CPP23_RANGES_SHIFT_defined)
            P(BSLS_LIBRARYFEATURES_HAS_CPP23_RANGES_STARTS_ENDS_WITH_defined)
            P(BSLS_LIBRARYFEATURES_HAS_CPP23_RANGES_FIND_LAST_defined)
            P(BSLS_LIBRARYFEATURES_HAS_CPP23_RANGES_CONTAINS_defined)
            P(BSLS_LIBRARYFEATURES_HAS_CPP23_RANGES_FOLD_defined)
            P(BSLS_LIBRARYFEATURES_HAS_CPP23_IS_IMPLICIT_LIFETIME_defined)
            P(BSLS_LIBRARYFEATURES_HAS_CPP23_REFERENCE_FROM_TEMPORARY_defined)
            P(BSLS_LIBRARYFEATURES_HAS_CPP23_CONTAINERS_RANGES_defined)
            P(BSLS_LIBRARYFEATURES_HAS_CPP23_RANGES_TO_CONTAINER_defined)
            P(BSLS_LIBRARYFEATURES_HAS_CPP23_RANGE_ADAPTOR_CLOSURE_defined)
            P(BSLS_LIBRARYFEATURES_HAS_CPP23_RANGES_CARTESIAN_PRODUCT_defined)
            P(BSLS_LIBRARYFEATURES_HAS_CPP23_RANGES_CHUNK_defined)
            P(BSLS_LIBRARYFEATURES_HAS_CPP23_RANGES_ENUMERATE_defined)
            P(BSLS_LIBRARYFEATURES_HAS_CPP23_RANGES_JOIN_WITH_defined)
            P(BSLS_LIBRARYFEATURES_HAS_CPP23_RANGES_SLIDE_defined)
            P(BSLS_LIBRARYFEATURES_HAS_CPP23_RANGES_STRIDE_defined)
            P(BSLS_LIBRARYFEATURES_HAS_CPP23_RANGES_ZIP_defined)
            P(BSLS_LIBRARYFEATURES_HAS_CPP23_GENERATOR_defined)
            P(BSLS_LIBRARYFEATURES_HAS_CPP23_ALLOCATE_AT_LEAST_defined)
            P(BSLS_LIBRARYFEATURES_HAS_CPP23_OUT_PTR_defined)
            P(BSLS_LIBRARYFEATURES_HAS_CPP23_START_LIFETIME_AS_defined)
            P(BSLS_LIBRARYFEATURES_HAS_CPP23_MDSPAN_defined)
            P(BSLS_LIBRARYFEATURES_HAS_CPP23_SPANSTREAM_defined)
            P(BSLS_LIBRARYFEATURES_HAS_CPP23_PRINT_defined)
            P(BSLS_LIBRARYFEATURES_HAS_CPP23_INT_CHARCONV_defined)
        }

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP23_BIND_BACK
        {
            ASSERT(__cpp_lib_bind_back >= 202202L);
            const auto f = std::bind_back([](double a1, int a2, char a3) {
                                            ASSERT(a1 == 0.5);
                                            ASSERT(a2 == 1);
                                            ASSERT(a3 == 'A');
                                          },
                                          1,
                                          'A');
            f(0.5);
        }
#endif
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP23_FORWARD_LIKE
        {
            ASSERT(__cpp_lib_forward_like >= 202207L);
            class C {};
            int mem;
            ASSERT((std::is_same_v<decltype(std::forward_like<const C&>(mem)),
                                   const int &>));
            ASSERT((std::is_same_v<decltype(std::forward_like<const C&&>(mem)),
                                   const int &&>));
            ASSERT((std::is_same_v<decltype(std::forward_like<const C>(mem)),
                                   const int &&>));
            ASSERT((std::is_same_v<decltype(std::forward_like<C&>(mem)),
                                   int &>));
            ASSERT((std::is_same_v<decltype(std::forward_like<C&&>(mem)),
                                   int &&>));
            ASSERT((std::is_same_v<decltype(std::forward_like<C>(mem)),
                                   int &&>));
        }
#endif
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP23_RANGES_IOTA
        {
            ASSERT(__cpp_lib_ranges_iota >= 202202L);
            int buf[2] = {};
            std::ranges::iota_result<int *, int> r =
                          std::ranges::iota(std::begin(buf), std::end(buf), 1);
            ASSERT(r.out == std::end(buf));
            ASSERT(r.value == 3);
            ASSERT(buf[0] == 1);
            ASSERT(buf[1] == 2);

            buf[0] = buf[1] = 0;
            r = std::ranges::iota(buf, 1);
            ASSERT(r.out == std::end(buf));
            ASSERT(r.value == 3);
            ASSERT(buf[0] == 1);
            ASSERT(buf[1] == 2);

            [[maybe_unused]]
            std::ranges::out_value_result<int *, int> rr = r;
        }
#endif
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP23_RANGES_AS_CONST
        ASSERT(__cpp_lib_ranges_as_const >= 202207L);
        {
            char ch = 'A';
            std::basic_const_iterator<char *> it{&ch};
            ASSERT(*it == ch);
        }
        {
            char ch = 'A';
            std::const_iterator<char *> it = std::make_const_iterator(&ch);
            ASSERT(*it == ch);
        }
        {
            char ch = 'A';
            std::const_sentinel<char *> end = std::make_const_sentinel(&ch);
            (void) end;
        }
        {
            char ch = 'A';
            std::iter_const_reference_t<char *> ref = ch;
            ASSERT(ref == ch);
        }
        {
            char str[] = {'A', 'B'};
            std::ranges::const_iterator_t<decltype(str)> it{str};
            ASSERT(*it == str[0]);
        }
        {
            char str[] = {'A', 'B'};
            std::ranges::const_sentinel_t<decltype(str)> end{str};
            (void) end;
        }
        {
            char str[] = {'A', 'B'};
            std::ranges::range_const_reference_t<decltype(str)> ref = str[0];
            ASSERT(ref == str[0]);
        }
        {
            ASSERT(std::ranges::constant_range<const char[2]>);
        }
        {
            char str[] = {'A', 'B'};
            std::ranges::as_const_view v{std::views::as_const(str)};
            (void) v;
        }
#endif
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP23_RANGES_SHIFT
        {
            ASSERT(__cpp_lib_shift >= 202202L);
            int nums[] = {1, 2, 3};
            std::ranges::shift_left(nums, 1);
            ASSERT(std::ranges::equal(nums, std::initializer_list{2, 3, 3}));

            std::ranges::shift_right(nums, 1);
            ASSERT(std::ranges::equal(nums, std::initializer_list{2, 2, 3}));
        }
#endif
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP23_RANGES_STARTS_ENDS_WITH
        {
            ASSERT(__cpp_lib_ranges_starts_ends_with >= 202106L);
            const int nums[] = {1, 2, 3, 4};
            ASSERT( std::ranges::starts_with(nums,
                                            std::initializer_list{1, 2}));
            ASSERT(!std::ranges::starts_with(nums,
                                            std::initializer_list{1, 1}));
            ASSERT( std::ranges::ends_with(nums, std::initializer_list{3, 4}));
            ASSERT(!std::ranges::ends_with(nums, std::initializer_list{3, 1}));
        }
#endif
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP23_RANGES_FIND_LAST
        {
            ASSERT(__cpp_lib_ranges_find_last >= 202207L);
            const int nums[] = {1, 2, 3, 4, 1, 6};

            auto l1 = std::ranges::find_last(nums, 1);
            ASSERT(std::distance(nums, l1.begin()) == 4);

            const auto is_1 = [](auto v) { return v == 1; };
            auto l2 = std::ranges::find_last_if(nums, is_1);
            ASSERT(std::distance(nums, l2.begin()) == 4);

            const auto more_than_1 = [](auto v) { return v > 1; };
            auto l3 = std::ranges::find_last_if_not(nums, more_than_1);
            ASSERT(std::distance(nums, l3.begin()) == 4);
        }
#endif
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP23_RANGES_CONTAINS
        {
            ASSERT(__cpp_lib_ranges_contains >= 202207L);
            const int nums[] = {1, 2, 3, 4};
            ASSERT( std::ranges::contains(nums, 2));
            ASSERT(!std::ranges::contains(nums, 5));
            ASSERT( std::ranges::contains_subrange(nums,
                                                 std::initializer_list{2, 3}));
            ASSERT(!std::ranges::contains_subrange(nums,
                                                 std::initializer_list{4, 5}));
        }
#endif
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP23_RANGES_FOLD
        {
            ASSERT(__cpp_lib_ranges_fold >= 202207L);
            const int nums[] = {1, 2, 3, 4};
            ASSERT(std::ranges::fold_left(nums, 0, std::plus<>{}) == 10);
            ASSERT(std::ranges::fold_left_first(nums, std::plus<>{}) == 10);
            ASSERT(std::ranges::fold_right(nums, 0, std::plus<>{}) == 10);
            ASSERT(std::ranges::fold_right_last(nums, std::plus<>{}) == 10);

            std::ranges::fold_left_with_iter_result<const int *, int>
                 r1 = std::ranges::fold_left_with_iter(nums, 0, std::plus<>{});
            ASSERT(r1.value == 10);
            ASSERT(r1.in == std::end(nums));

            std::ranges::fold_left_first_with_iter_result<const int *,
                                                          std::optional<int>>
                 r2 = std::ranges::fold_left_first_with_iter(nums,
                                                             std::plus<>{});
            ASSERT(r2.value == 10);
            ASSERT(r2.in == std::end(nums));

            [[maybe_unused]]
            std::ranges::in_value_result<const int *, int> r11 = r1;
            [[maybe_unused]]
            std::ranges::in_value_result<const int *, std::optional<int>>
                                                                      r22 = r2;
        }
#endif
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP23_IS_IMPLICIT_LIFETIME
        {
            ASSERT(__cpp_lib_is_implicit_lifetime >= 202302L);
            ASSERT(std::is_implicit_lifetime<int>::value);
            ASSERT(std::is_implicit_lifetime_v<int>);
        }
#endif
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP23_REFERENCE_FROM_TEMPORARY
        {
            ASSERT(__cpp_lib_reference_from_temporary >= 202202L);
            ASSERT((std::reference_constructs_from_temporary<int&&,
                                                             int>::value));
            ASSERT((std::reference_constructs_from_temporary_v<int&&,int>));
            ASSERT((std::reference_converts_from_temporary<int&&,int>::value));
            ASSERT((std::reference_converts_from_temporary_v<int&&,int>));
        }
#endif
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP23_CONTAINERS_RANGES
        {
            ASSERT(__cpp_lib_containers_ranges >= 202202L);
            std::from_range_t v{std::from_range};
            (void) v;
        }
#endif
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP23_RANGES_TO_CONTAINER
        {
            ASSERT(__cpp_lib_ranges_to_container >= 202202L);
            const int arr[] = {1, 2};
            auto v = std::ranges::to<std::vector>(arr);
            ASSERT(v.size() == 2);
        }
#endif
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP23_RANGE_ADAPTOR_CLOSURE
        {
            ASSERT(__cpp_lib_ranges >= 202202L);
            struct FirstChar : std::ranges::range_adaptor_closure<FirstChar> {
                std::string_view operator()(std::string_view s) const
                {
                    if (s.empty()) return {};
                    return s.substr(0, 1);
                }
            } firstChar;
            std::string_view char1 = std::string_view{"abc"} | firstChar;
            ASSERT(char1 == "a");
        }
#endif
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP23_RANGES_CARTESIAN_PRODUCT
        {
            ASSERT(__cpp_lib_ranges_cartesian_product >= 202207L);
            const int nums[] = {1, 2, 3};
            const char chars[] = {'a', 'b'};
            std::ranges::cartesian_product_view res{
                                   std::views::cartesian_product(nums, chars)};
            const std::pair<int, char> expected[] = {
                {1, 'a'}, {1, 'b'},
                {2, 'a'}, {2, 'b'},
                {3, 'a'}, {3, 'b'}
            };
#if defined(_GLIBCXX_RELEASE) && _GLIBCXX_RELEASE == 13
            ASSERT(std::ranges::equal(res, expected,
                                       std::equal_to<std::pair<int, char>>{}));
#else
            ASSERT(std::ranges::equal(res, expected));
#endif
        }
#endif
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP23_RANGES_CHUNK
        {
            ASSERT(__cpp_lib_ranges_chunk >= 202202L);
            const int nums[] = {1, 2, 3, 4, 5, 6};
            std::ranges::chunk_view view{std::views::chunk(nums, 2)};
            std::initializer_list<std::initializer_list<int>> expected =
                {{1, 2}, {3, 4}, {5, 6}};
            ASSERT(std::ranges::equal(view, expected, std::ranges::equal));
        }
#endif
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP23_RANGES_ENUMERATE
        {
            ASSERT(__cpp_lib_ranges_enumerate >= 202302L);
            const char chars[] = {'a', 'b', 'c'};
            std::ranges::enumerate_view view{std::views::enumerate(chars)};
            const std::tuple<int, char> expected[] =
                {{0, 'a'}, {1, 'b'}, {2, 'c'}};
            ASSERT(std::ranges::equal(view, expected));
        }
#endif
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP23_RANGES_JOIN_WITH
        {
            ASSERT(__cpp_lib_ranges_join_with >= 202202L);
            const std::string_view words[] = {"w1", "w2", "w3"};
            std::ranges::join_with_view view{std::views::join_with(words,' ')};
            ASSERT(std::ranges::equal(view, std::string_view{"w1 w2 w3"}));
        }
#endif
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP23_RANGES_SLIDE
        {
            ASSERT(__cpp_lib_ranges_slide >= 202202L);
            const int nums[] = {1, 2, 3, 4, 5};
            std::ranges::slide_view view{std::views::slide(nums, 3)};
            std::initializer_list<std::initializer_list<int>> expected =
                {{1, 2, 3}, {2, 3, 4}, {3, 4, 5}};
            ASSERT(std::ranges::equal(view, expected, std::ranges::equal));
        }
#endif
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP23_RANGES_STRIDE
        {
            ASSERT(__cpp_lib_ranges_stride >= 202207L);
            const int nums[] = {1, 2, 3, 4, 5, 6, 7, 8, 9};
            std::ranges::stride_view view{std::views::stride(nums, 3)};
            const int expected[] = {1, 4, 7};
            ASSERT(std::ranges::equal(view, expected));
        }
#endif
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP23_RANGES_ZIP
        {
            ASSERT(__cpp_lib_ranges_zip >= 202110L);
            const int nums[] = {1, 2, 3, 4, 5};
            const char chars[] = {'a', 'b', 'c'};
            {
                std::ranges::zip_view view{std::views::zip(nums, chars)};
                const std::pair<int, char> expected[] =
                    {{1, 'a'}, {2, 'b'}, {3, 'c'}};
#if defined(_GLIBCXX_RELEASE) && _GLIBCXX_RELEASE == 13
                ASSERT(std::ranges::equal(view, expected,
                                       std::equal_to<std::pair<int, char>>{}));
#else
                ASSERT(std::ranges::equal(view, expected));
#endif
            }
            {
                const auto concat = [](int n, char c) {
                    return std::to_string(n) + c;
                };
                std::ranges::zip_transform_view view{
                               std::views::zip_transform(concat, nums, chars)};
                const std::string_view expected[] = {"1a", "2b", "3c"};
                ASSERT(std::ranges::equal(view, expected));
            }
            {
                std::ranges::adjacent_view view{std::views::adjacent<3>(nums)};
                const std::tuple<int, int, int> expected[] =
                    {{1, 2, 3}, {2, 3, 4}, {3, 4, 5}};
                ASSERT(std::ranges::equal(view, expected));
            }
            {
                const auto sum = [](auto... nums) { return (0 + ... + nums); };
                std::ranges::adjacent_transform_view view{
                                 std::views::adjacent_transform<3>(nums, sum)};
                const int expected[] = {6, 9, 12};
                ASSERT(std::ranges::equal(view, expected));
            }
        }
#endif
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP23_GENERATOR
        {
            ASSERT(__cpp_lib_generator >= 202207L);
            const auto chars = [](std::string_view s) -> std::generator<char> {
                for(char c : s) {
                    co_yield c;
                }
            };
            const auto gen = [&](std::string_view s) -> std::generator<char> {
                co_yield std::ranges::elements_of(chars(s));
            };
            const std::string_view s{"str"};
            const char expected[] = {'s', 't', 'r'};
            ASSERT(std::ranges::equal(gen(s), expected));
        }
#endif
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP23_ALLOCATE_AT_LEAST
        {
            ASSERT(__cpp_lib_allocate_at_least >= 202302L);
            (void)[] {
                std::allocator<int> alloc;
                std::allocation_result<int *, std::size_t> res1 =
                                                   alloc.allocate_at_least(1U);
                using A = std::allocator_traits<std::allocator<int>>;
                std::allocation_result<int *, std::size_t> res2 =
                                               A::allocate_at_least(alloc, 1U);
            };
        }
#endif
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP23_OUT_PTR
        {
            ASSERT(__cpp_lib_out_ptr >= 202106L);
            {
                const auto func = [](int **pp) { *pp = new int{}; };
                std::unique_ptr<int> p;
                func(std::out_ptr(p));
                ASSERT(p);
                ASSERT((std::is_same_v<decltype(std::out_ptr(p)),
                                std::out_ptr_t<std::unique_ptr<int>, int *>>));
            }
            {
                const auto func = [](int **) {};
                auto p = std::make_unique<int>();
                func(std::inout_ptr(p));
                ASSERT(p);
                ASSERT((std::is_same_v<decltype(std::inout_ptr(p)),
                              std::inout_ptr_t<std::unique_ptr<int>, int *>>));
            }
        }
#endif
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP23_START_LIFETIME_AS
        {
            ASSERT(__cpp_lib_start_lifetime_as >= 202207L);
            {
                alignas(unsigned) unsigned char buf[sizeof(unsigned)] = {};
                unsigned *uint_p = std::start_lifetime_as<unsigned>(buf);
                ASSERT(*uint_p == 0U);
            }
            {
                const std::size_t arrSize = 4;
                alignas(unsigned) unsigned char buf[
                                              arrSize * sizeof(unsigned)] = {};
                unsigned *uint_arr =
                          std::start_lifetime_as_array<unsigned>(buf, arrSize);
                ASSERT(std::count(uint_arr, uint_arr + arrSize, 0U)== arrSize);
            }
        }
#endif
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP23_MDSPAN
        ASSERT(__cpp_lib_mdspan >= 202207L);
        {
            int data[2][3] = {{1, 2, 3}, {4, 5, 6}};
            std::mdspan<int, std::extents<size_t, 2, 3> > view(&data[0][0]);
        }
        {
            using LeftMD =
                  std::mdspan<int, std::dextents<size_t, 2>, std::layout_left>;
            using RightMD = std::mdspan<int, std::dextents<size_t, 2>,
                                std::layout_right, std::default_accessor<int>>;

            int data[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12};
            LeftMD  view_l(data, 3, 4);
            RightMD view_r(data, 2, 6);
        }
#endif
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP23_SPANSTREAM
        ASSERT(__cpp_lib_spanstream >= 202106L);
        {
            int a = 0;
            char dataBuffer[50] = {};
            std::span<char> dataSpan(+dataBuffer, 50);
            std::spanbuf sbufOut(dataSpan, std::ios::out);
            std::ostream os(&sbufOut);
            os << "13 1 2";

            std::ispanstream inputStream(dataSpan);
            inputStream >> a;
            ASSERT(13 == a);

            a = 42;
            std::spanstream dataStream(dataSpan);
            dataStream >> a;
            ASSERT(13 == a);

            std::ospanstream outputStream(dataSpan);
            const auto message = "I have something to say.";
            outputStream << message;
            ASSERT(!strcmp(+dataBuffer, +message));
        }
        {
            ASSERT((std::is_same_v<std::ispanstream,
                                   std::basic_ispanstream<char>>));
            ASSERT((std::is_same_v<std::ospanstream,
                                   std::basic_ospanstream<char>>));
            ASSERT((std::is_same_v<std::spanbuf,
                                   std::basic_spanbuf<char>>));
            ASSERT((std::is_same_v<std::spanstream,
                                   std::basic_spanstream<char>>));
            ASSERT((std::is_same_v<std::wspanbuf,
                                   std::basic_spanbuf<wchar_t>>));
            ASSERT((std::is_same_v<std::wispanstream,
                                   std::basic_ispanstream<wchar_t>>));
            ASSERT((std::is_same_v<std::wospanstream,
                                   std::basic_ospanstream<wchar_t>>));
            ASSERT((std::is_same_v<std::wspanstream,
                                   std::basic_spanstream<wchar_t>>));
        }
#endif
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP23_PRINT
        (void) []{
            // <print>
            std::print("{}", 1);
            std::print(stdout, "{}", 1);
            std::println("{}", 1);
            std::println(stdout, "{}", 1);
            std::println();
            std::println(stdout);

            const int one = 1;
            //std::vprint_nonunicode("{}", std::make_format_args(one));
            std::vprint_nonunicode(stdout, "{}", std::make_format_args(one));
            std::vprint_nonunicode_buffered(stdout,
                                            "{}",
                                            std::make_format_args(one));
            //std::vprint_unicode("{}", std::make_format_args(1));
            std::vprint_unicode(stdout, "{}", std::make_format_args(one));
            std::vprint_unicode_buffered(stdout,
                                         "{}",
                                         std::make_format_args(one));

            // <ostream>
            std::print(std::cout, "{}", 1);
            std::println(std::cout, "{}", 1);
            std::println(std::cout);

            std::vprint_nonunicode(std::cout,
                                   "{}",
                                   std::make_format_args(one));
            std::vprint_unicode(std::cout, "{}", std::make_format_args(one));
        };
#endif

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP23_RANGE_FORMAT
        if (veryVerbose) puts("\tTesting `BSLS_LIBRARYFEATURES_HAS_CPP23_RANGE_FORMAT`");
        {
            const int aRange[] = {1, 2, 3, 4};
            (void) std::format("{}", aRange);
        }
#endif

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP23_INT_CHARCONV
        if (veryVerbose) puts("\tTesting `BSLS_LIBRARYFEATURES_HAS_CPP23_INT_CHARCONV`");
        {
            ASSERT(__cpp_lib_constexpr_charconv >= 202207L);
            static_assert([]{
                const auto my_assert = [](bool c) { if (!c) throw 0; };
                const int VALUE = 1;
                char buf[8] = {};
                {
                    auto r = std::to_chars(buf, buf + sizeof(buf), VALUE);
                    my_assert(r.ec == std::errc{});
                    my_assert(r.ptr == buf + 1);
                    my_assert(buf[0] == '1');
                }
                {
                    int res = 0;
                    auto r = std::from_chars(buf, buf + 1, res);
                    my_assert(r.ec == std::errc{});
                    my_assert(res == VALUE);
                }
                return true;
            }());
        }
#endif
      } break;
      case 25: {
        // --------------------------------------------------------------------
        // `BSLS_LIBRARYFEATURES_HAS_CPP23_BASELINE_LIBRARY`
        //
        // Concerns:
        // 1. `BSLS_LIBRARYFEATURES_HAS_CPP23_BASELINE_LIBRARY` is defined only
        //    when the native standard library provides a baseline of C++23
        //    library features, including:
        //    - `std::byteswap`
        //    - `std::invoke_r`
        //    - `std::ios_base::noreplace`
        //    - `std::views::as_rvalue`, `std::ranges::as_rvalue_view'
        //    - `std::views::chunk_by`, `std::ranges::chunk_view'
        //    - `std::views::repeat`, `std::ranges::repeat_view'
        //    - `std::is_scoped_enum`, `std::is_scoped_enum_v`
        //    - `std::to_underlying`
        //    - `std::unreachable`
        //    - `constexpr` `std::unique_ptr`.
        //    - `constexpr` `std::type_info::operator==`.
        //    - Random access `std::move_iterator<T*>`.
        //
        // 2. The corresponding standard feature test macro is defined and has
        //    a value in the expected range.
        //
        // Plan:
        // 1. When `BSLS_LIBRARYFEATURES_HAS_CPP23_BASELINE_LIBRARY` is
        //    defined include the appropriate headers and use the expected
        //    names.
        //
        // 2. Write a simple test for each component when possible.
        //
        // Testing:
        //   BSLS_LIBRARYFEATURES_HAS_CPP23_BASELINE_LIBRARY
        // --------------------------------------------------------------------

        if (verbose)
            puts("\n'BSLS_LIBRARYFEATURES_HAS_CPP23_BASELINE_LIBRARY'"
                 "\n=================================================");

        if (verbose) {
            P(BSLS_LIBRARYFEATURES_HAS_CPP23_BASELINE_LIBRARY_defined)
        }

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP23_BASELINE_LIBRARY
        {
            ASSERT(__cpp_lib_byteswap >= 202110L);
            std::uint32_t value = 0x01020304U;
            value = std::byteswap(value);
            ASSERT(value == 0x04030201U);
        }
        {
            ASSERT(__cpp_lib_invoke_r >= 202106L);
            bool called = false;
            std::invoke_r<void>(
                [&](int arg){
                    ASSERT(arg == 3);
                    called = true;
                },
                3);
            ASSERT(called);
        }
        {
            ASSERT(__cpp_lib_ios_noreplace >= 202207L);
            (void) std::ios_base::noreplace;
        }
        {
            ASSERT(__cpp_lib_ranges_as_rvalue >= 202207L);
            std::unique_ptr<int> values[] = {
                std::make_unique<int>(1),
                std::make_unique<int>(2)
            };
            std::ranges::as_rvalue_view view{values | std::views::as_rvalue};
            for (auto v : view) {
                auto vv = std::move(v);
            }
            for (auto& p : values) {
                ASSERT(!p);
            }
        }
        {
            ASSERT(__cpp_lib_ranges_chunk_by >= 202202L);
            const int values[] = {-1, 1, 2, -2, 3, 4};
            const auto is_equal = [](const auto&                range,
                                     std::initializer_list<int> list) {
                return std::ranges::equal(range, list);
            };
            int i = 0;
            const auto pred = [](int a, int b) { return a > 0 && b > 0; };
            std::ranges::chunk_by_view view{ values
                                           | std::views::chunk_by(pred)};
            for(auto r : view) {
                switch(i)
                {
                  case 0: ASSERT((is_equal(r, {-1}  ))); break;
                  case 1: ASSERT((is_equal(r, {1, 2}))); break;
                  case 2: ASSERT((is_equal(r, {-2}  ))); break;
                  case 3: ASSERT((is_equal(r, {3, 4}))); break;
                }
                ++i;
            }
            ASSERT(i == 4);
        }
        {
            ASSERT(__cpp_lib_ranges_repeat >= 202207L);
            int value = 1;
            int count = 0;
            const int COUNT = 3;
            std::ranges::repeat_view view{std::views::repeat(value, COUNT)};
            for(auto v : view) {
                ASSERT(v == value);
                ++count;
            }
            ASSERT(count == COUNT);
        }
        {
            ASSERT(__cpp_lib_is_scoped_enum >= 202011L);
            enum E1 { E1_enumerator1 };
            enum class E2 { enumerator1 };

            ASSERT(!std::is_scoped_enum<E1>::value);
            ASSERT(!std::is_scoped_enum_v<E1>);

            ASSERT(std::is_scoped_enum<E2>::value);
            ASSERT(std::is_scoped_enum_v<E2>);
        }
        {
            ASSERT(__cpp_lib_to_underlying >= 202102L);
            enum Enum : char { enumerator1 = 5 };
            char underlyingValue{std::to_underlying(enumerator1)};
            ASSERT(underlyingValue == 5);
        }
        for(;;) {
            ASSERT(__cpp_lib_unreachable >= 202202L);
            break;
            std::unreachable();
        }
        {
            ASSERT(__cpp_lib_constexpr_memory >= 202202L);
            static_assert([]{
                const auto my_assert = [](bool c) { if (!c) throw 0; };
                const int VALUE = 5;
                auto p = std::make_unique<int>(VALUE);
                my_assert(p != nullptr);
                my_assert(*p == VALUE);
                return true;
            }());
        }
        {
            ASSERT(__cpp_lib_constexpr_typeinfo >= 202106L);
            static_assert([]{
                return typeid(0) == typeid(int);
            }());
        }
        {
            ASSERT(__cpp_lib_move_iterator_concept >= 202207L);
            ASSERT(std::random_access_iterator<std::move_iterator<int*>>);
        }
#endif
        if (veryVeryVerbose) P(BSLS_PLATFORM_CMP_VERSION);
      } break;
      case 24: {
        // --------------------------------------------------------------------
        // TESTING `BSLS_LIBRARYFEATURES_HAS_CPP20_JTHREAD`
        //
        // Concerns:
        // 1. If `BSLS_LIBRARYFEATURES_HAS_CPP20_JTHREAD` is defined, the
        //    `std::jthread` class is available.
        //
        // 2. The corresponding standard feature test macro is defined and has
        //    a value in the expected range.
        //
        // Plan:
        // 1. Make simple use of the `std::jthread` class name and the member
        //    type `id` to verify its reasonable availability when
        //    `BSLS_LIBRARYFEATURES_HAS_CPP20_JTHREAD` is defined.
        //
        // Testing:
        //   BSLS_LIBRARYFEATURES_HAS_CPP20_JTHREAD
        // --------------------------------------------------------------------

        if (verbose)
            printf("TESTING `BSLS_LIBRARYFEATURES_HAS_CPP20_JTHREAD`\n"
                   "================================================\n");

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP20_IS_CORRESPONDING_MEMBER)

        ASSERTV("__cpp_lib_jthread >= 201911L check", __cpp_lib_jthread,
                __cpp_lib_jthread >= 201911L);

        ASSERT((std::is_same_v<std::jthread::id, std::thread::id>));
#else
        if (veryVerbose) {
            puts("SKIPPED: `BSLS_LIBRARYFEATURES_HAS_CPP20_JTHREAD` is not "
                 "defined.");
        }
#endif
      } break;
      case 23: {
        // --------------------------------------------------------------------
        // TESTING `BSLS_LIBRARYFEATURES_HAS_CPP20_IS_CORRESPONDING_MEMBER`
        //
        // Concerns:
        // 1. If `BSLS_LIBRARYFEATURES_HAS_CPP20_IS_CORRESPONDING_MEMBER` is
        //    defined, the `std::is_corresponding_member` meta function is
        //    available.
        //
        // 2. The corresponding standard feature test macro is defined and has
        //    a value in the expected range.
        //
        // Plan:
        // 1. Make simple use of the `std::is_corresponding_member` function
        //    template to verify its availability when
        //    `BSLS_LIBRARYFEATURES_HAS_CPP20_IS_CORRESPONDING_MEMBER` is
        //    defined.
        //
        // Testing:
        //   BSLS_LIBRARYFEATURES_HAS_CPP20_IS_CORRESPONDING_MEMBER
        // --------------------------------------------------------------------

        if (verbose) printf(
         "TESTING `BSLS_LIBRARYFEATURES_HAS_CPP20_IS_CORRESPONDING_MEMBER`\n"
         "================================================================\n");

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP20_IS_CORRESPONDING_MEMBER)

        ASSERTV("__cpp_lib_is_layout_compatible >= 201907L check",
                 __cpp_lib_is_layout_compatible,
                 __cpp_lib_is_layout_compatible >= 201907L);

        struct Foo { int x; };
        struct Bar { int y; double z; };

        const bool result = std::is_corresponding_member(&Foo::x, &Bar::y);
        ASSERT(true == result);
#else
        if (veryVerbose) {
            printf("SKIPPED: "
                   "`BSLS_LIBRARYFEATURES_HAS_CPP20_IS_CORRESPONDING_MEMBER` "
                   "undefined.\n");
        }
#endif
      } break;
      case 22: {
        // --------------------------------------------------------------------
        // TESTING `BSLS_LIBRARYFEATURES_HAS_CPP20_IS_POINTER_INTERCONVERTIBLE`
        //
        // Concerns:
        // 1. If `BSLS_LIBRARYFEATURES_HAS_CPP20_IS_POINTER_INTERCONVERTIBLE`
        //    is defined, the `std::is_pointer_interconvertible_base_of` and
        //    `std::is_pointer_interconvertible_with_class` function templates
        //    are available.
        //
        // Plan:
        // 1. Make simple use of the `std::is_pointer_interconvertible_base_of`
        //    and `std::is_pointer_interconvertible_with_class` function
        //    templates to verify their availability when
        //    `BSLS_LIBRARYFEATURES_HAS_CPP20_IS_POINTER_INTERCONVERTIBLE` is
        //    defined.
        //
        // Testing:
        //   BSLS_LIBRARYFEATURES_HAS_CPP20_IS_POINTER_INTERCONVERTIBLE
        // --------------------------------------------------------------------

        if (verbose) printf(
     "TESTING `BSLS_LIBRARYFEATURES_HAS_CPP20_IS_POINTER_INTERCONVERTIBLE`\n"
     "====================================================================\n");

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP20_IS_POINTER_INTERCONVERTIBLE)

        ASSERTV("__cpp_lib_is_pointer_interconvertible >= 201907L check",
                 __cpp_lib_is_pointer_interconvertible,
                 __cpp_lib_is_pointer_interconvertible >= 201907L);
        {
            struct Foo {};
            struct Bar {};
            class Baz : Foo, public Bar {
                int x;
            };

            {
                // Test the first base class.  It's private, but the trait
                // should still be true.
                const bool result =
                     std::is_pointer_interconvertible_base_of<Foo, Baz>::value;
                const bool result_v =
                     std::is_pointer_interconvertible_base_of_v<Foo, Baz>;

                ASSERT(true == result);
                ASSERT(true == result_v);
            }
            {
                // Test the second base class.  MSVC doesn't give the right
                // answer for ABI reasons.
                const bool result =
                     std::is_pointer_interconvertible_base_of<Bar, Baz>::value;
                const bool result_v =
                     std::is_pointer_interconvertible_base_of_v<Bar, Baz>;

#if defined(BSLS_PLATFORM_CMP_MSVC)
                const bool expected = false;
#else
                const bool expected = true;
#endif

                ASSERT(expected == result);
                ASSERT(expected == result_v);
            }
        }
        {
            struct Foo { int x; };
            struct Bar { int y; };
            struct Baz : Foo, Bar {}; // not standard-layout

            const bool result = std::is_pointer_interconvertible_with_class(
                                             static_cast<int Baz::*>(&Foo::x));
            ASSERT(false == result);
        }
#else
        if (veryVerbose) {
            printf(
                 "SKIPPED: "
                 "BSLS_LIBRARYFEATURES_HAS_CPP20_IS_POINTER_INTERCONVERTIBLE' "
                 "undefined.\n");
        }
#endif
      } break;
      case 21: {
        // --------------------------------------------------------------------
        // TESTING `BSLS_LIBRARYFEATURES_HAS_CPP20_IS_LAYOUT_COMPATIBLE`
        //
        // Concerns:
        // 1. If `BSLS_LIBRARYFEATURES_HAS_CPP20_IS_LAYOUT_COMPATIBLE`
        //    is defined, the `std::is_layout_compatible`
        //    function template is available.
        //
        // 2. The corresponding standard feature test macro is defined and has
        //    a value in the expected range.
        //
        // Plan:
        // 1. Make simple use of the `std::is_layout_compatible` function
        //    template to verify its availability when
        //    `BSLS_LIBRARYFEATURES_HAS_CPP20_IS_LAYOUT_COMPATIBLE` is defined.
        //
        // Testing:
        //   BSLS_LIBRARYFEATURES_HAS_CPP20_IS_LAYOUT_COMPATIBLE
        // --------------------------------------------------------------------

        if (verbose) {
            printf(
            "TESTING `BSLS_LIBRARYFEATURES_HAS_CPP20_IS_LAYOUT_COMPATIBLE`\n"
            "=============================================================\n");

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP20_IS_LAYOUT_COMPATIBLE)

        ASSERTV("__cpp_lib_is_layout_compatible >= 201907L check",
                 __cpp_lib_is_layout_compatible,
                 __cpp_lib_is_layout_compatible >= 201907L);

        struct Foo{
            int x;
            char y;
        };

        class Bar
        {
            const int u = 42;
            volatile char v = '*';
        };

        const bool result   = std::is_layout_compatible  <Foo, Bar>::value;
        const bool result_v = std::is_layout_compatible_v<Foo, Bar>;

        ASSERTV(result,   true == result);
        ASSERTV(result_v, true == result_v);
#else
        if (veryVerbose) {
            printf(
         "SKIPPED: "
         "`BSLS_LIBRARYFEATURES_HAS_CPP20_IS_LAYOUT_COMPATIBLE` undefined.\n");
        }
#endif
        }
      } break;
      case 20: {
        // --------------------------------------------------------------------
        // TESTING `BSLS_LIBRARYFEATURES_HAS_CPP20_TO_ARRAY`
        //
        // Concerns:
        // 1. If `BSLS_LIBRARYFEATURES_HAS_CPP20_TO_ARRAY` is defined,
        //    `std::to_array` is available.
        //
        // Plan:
        // 1. Make simple use of `std::to_array` to verify its availability
        //    if `BSLS_LIBRARYFEATURES_HAS_CPP20_TO_ARRAY` is defined.
        //
        // Testing:
        //   BSLS_LIBRARYFEATURES_HAS_CPP20_TO_ARRAY
        // --------------------------------------------------------------------

        if (verbose) {
            printf("TESTING `BSLS_LIBRARYFEATURES_HAS_CPP20_TO_ARRAY`\n"
                   "=================================================\n");
        }

        if (verbose) {
            P(BSLS_LIBRARYFEATURES_HAS_CPP20_TO_ARRAY_defined);
        }

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP20_TO_ARRAY)
        // String is there since the macro will get replaced by value.
        ASSERTV("__cpp_lib_to_array >= 201907L check",
                __cpp_lib_to_array,
                __cpp_lib_to_array >= 201907L);


        if (veryVerbose) printf("... testing to_array(T&[N])\n");
        {
            int src[5] = {1, 2, 3, 4, 5};
            std::array<int, 5> dest = std::to_array(src);
            for (std::size_t i = 0; i < sizeof(src)/sizeof(*src); ++i) {
                ASSERTV(i, src[i], dest[i], src[i] == dest[i]);
            }
        }


        if (veryVerbose) printf("... testing to_array(T&&[N])\n");
        {
            int src[5] = {1, 2, 3, 4, 5};
            int check[5] = {1, 2, 3, 4, 5};
            std::array<int, 5> dest = std::to_array(std::move(src));
            for (std::size_t i = 0; i < sizeof(check)/sizeof(*check); ++i) {
                ASSERTV(i, dest[i], check[i], dest[i] == check[i]);
            }
        }
#endif
      } break;
      case 19: {
        // --------------------------------------------------------------------
        // `BSLS_LIBRARYFEATURES_HAS_CPP20_*` MISCELLANY
        //
        // Concerns:
        //  1. `BSLS_LIBRARYFEATURES_HAS_CPP20_VERSION` is defined only when
        //     the native standard library provides it.
        //
        //  2. `BSLS_LIBRARYFEATURES_HAS_CPP20_CONCEPTS` is defined only when
        //     the native standard library provides it.
        //
        //  3. `BSLS_LIBRARYFEATURES_HAS_CPP20_RANGES` is defined only when the
        //     native standard library provides it.
        //
        //  3. `BSLS_LIBRARYFEATURES_HAS_CPP20_SOURCE_LOCATION` is defined only
        //     when the native standard library provides it.
        //
        //  4. `BSLS_LIBRARYFEATURES_HAS_CPP20_ATOMIC_REF` is
        //     defined only when the native standard library provides it.
        //
        //  5. `BSLS_LIBRARYFEATURES_HAS_CPP20_ATOMIC_LOCK_FREE_TYPE_ALIASES`
        //     is defined only when the native standard library provides it.
        //
        //  6. `BSLS_LIBRARYFEATURES_HAS_CPP20_ATOMIC_WAIT_FREE_FUNCTIONS` is
        //     defined only when the native standard library provides it.
        //
        //  7. `BSLS_LIBRARYFEATURES_HAS_CPP20_ATOMIC_FLAG_TEST_FREE_FUNCTIONS`
        //     is defined only when the native standard library provides it.
        //
        //  8. `BSLS_LIBRARYFEATURES_HAS_CPP20_MAKE_UNIQUE_FOR_OVERWRITE` is
        //     defined only when the native standard library provides it.
        //
        //  9. `BSLS_LIBRARYFEATURES_HAS_CPP20_CALENDAR` is defined only when
        //     the native standard library provides it.
        //
        // 10. `BSLS_LIBRARYFEATURES_HAS_CPP20_TIMEZONE` is defined only when
        //     the native standard library provides it.
        //
        // 11. `BSLS_LIBRARYFEATURES_HAS_CPP20_CHAR8_MB_CONV` is defined only
        //     when the native standard library provides it.
        //
        // 12. `BSLS_LIBRARYFEATURES_HAS_CPP20_FORMAT` is defined only
        //    w hen the native standard library provides it.
        //
        // Plan:
        // 1. When these macros are defined include the appropriate headers and
        //    use the expected names.
        //
        // Testing:
        //   BSLS_LIBRARYFEATURES_HAS_CPP20_VERSION
        //   BSLS_LIBRARYFEATURES_HAS_CPP20_CONCEPTS
        //   BSLS_LIBRARYFEATURES_HAS_CPP20_RANGES
        //   BSLS_LIBRARYFEATURES_HAS_CPP20_SOURCE_LOCATION
        //   BSLS_LIBRARYFEATURES_HAS_CPP20_ATOMIC_REF
        //   BSLS_LIBRARYFEATURES_HAS_CPP20_ATOMIC_LOCK_FREE_TYPE_ALIASES
        //   BSLS_LIBRARYFEATURES_HAS_CPP20_ATOMIC_WAIT_FREE_FUNCTIONS
        //   BSLS_LIBRARYFEATURES_HAS_CPP20_ATOMIC_FLAG_TEST_FREE_FUNCTIONS
        //   BSLS_LIBRARYFEATURES_HAS_CPP20_MAKE_UNIQUE_FOR_OVERWRITE
        //   BSLS_LIBRARYFEATURES_HAS_CPP20_CALENDAR
        //   BSLS_LIBRARYFEATURES_HAS_CPP20_CHAR8_MB_CONV
        //   BSLS_LIBRARYFEATURES_HAS_CPP20_FORMAT
        //   BSLS_LIBRARYFEATURES_HAS_CPP20_TIMEZONE
        // --------------------------------------------------------------------

        if (verbose) puts("\n'BSLS_LIBRARYFEATURES_HAS_CPP20_*' MISCELLANY"
                          "\n=============================================");

        if (verbose) {
/// Print Macro Defined
#define PMD(macro_name) P(macro_name##_defined)

            PMD(BSLS_LIBRARYFEATURES_HAS_CPP20_ATOMIC_FLAG_TEST_FREE_FUNCTIONS);
            PMD(BSLS_LIBRARYFEATURES_HAS_CPP20_ATOMIC_LOCK_FREE_TYPE_ALIASES);
            PMD(BSLS_LIBRARYFEATURES_HAS_CPP20_ATOMIC_REF);
            PMD(BSLS_LIBRARYFEATURES_HAS_CPP20_ATOMIC_WAIT_FREE_FUNCTIONS);
            PMD(BSLS_LIBRARYFEATURES_HAS_CPP20_CALENDAR);
            PMD(BSLS_LIBRARYFEATURES_HAS_CPP20_CHAR8_MB_CONV);
            PMD(BSLS_LIBRARYFEATURES_HAS_CPP20_CONCEPTS);
            PMD(BSLS_LIBRARYFEATURES_HAS_CPP20_FORMAT);
            PMD(BSLS_LIBRARYFEATURES_HAS_CPP20_MAKE_UNIQUE_FOR_OVERWRITE);
            PMD(BSLS_LIBRARYFEATURES_HAS_CPP20_RANGES);
            PMD(BSLS_LIBRARYFEATURES_HAS_CPP20_SOURCE_LOCATION);
            PMD(BSLS_LIBRARYFEATURES_HAS_CPP20_SYNCSTREAM);
            PMD(BSLS_LIBRARYFEATURES_HAS_CPP20_TIMEZONE);
            PMD(BSLS_LIBRARYFEATURES_HAS_CPP20_VERSION);
#undef PMD
        }

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP20_CONCEPTS
        ASSERT( case19::equal(2,2));
        ASSERT(!case19::equal(1,2));
#endif

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP20_RANGES
        {
            std::vector<int> v = {1,2,3,4,5};

            (void)std::ranges::data(v);
            std::ranges::take_view   tv(v, 3);
            (void)tv;
            std::ranges::owning_view ov(std::move(v));
            (void)ov;
        }
#endif

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP20_SOURCE_LOCATION
        {
            const std::source_location s = std::source_location::current();
            (void) s;
        }
#endif

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP20_ATOMIC_REF
        {
            int dummy;
            int &dummy2 = dummy;
            (void)std::atomic_ref{dummy2};
        }
#endif

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP20_ATOMIC_LOCK_FREE_TYPE_ALIASES
        {
            (void)std::atomic_signed_lock_free{};
            (void)std::atomic_unsigned_lock_free{};
        }
#endif

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP20_ATOMIC_WAIT_FREE_FUNCTIONS
        (void)[](std::atomic_flag *ptr) {
            std::atomic_flag_wait(ptr, true);
            std::atomic_flag_wait_explicit(ptr,
                                           true,
                                           std::memory_order::relaxed);
            std::atomic_flag_notify_one(ptr);
            std::atomic_flag_notify_all(ptr);
        };
#endif

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP20_ATOMIC_FLAG_TEST_FREE_FUNCTIONS
        (void)[](std::atomic_flag *ptr) {
            (void)std::atomic_flag_test(ptr);
            (void)std::atomic_flag_test_explicit(ptr,
                                                  std::memory_order::relaxed);
        };
#endif

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP20_MAKE_UNIQUE_FOR_OVERWRITE
        (void)std::make_unique_for_overwrite<int>();
        (void)std::make_unique_for_overwrite<int[]>(4);
#endif

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP20_CALENDAR
        (void)std::chrono::day{1};
        (void)std::chrono::month{1};
        (void)std::chrono::year{2000};

        using namespace std::chrono_literals;
        using std::chrono::May;
        (void)(1d/May/2000y);
#endif

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP20_TIMEZONE
        (void)[]{
            const std::chrono::tzdb& tz = std::chrono::get_tzdb();
            (void)tz;
        };
#endif

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP20_CHAR8_MB_CONV
        (void)[](char8_t *out, std::mbstate_t *st) {
            (void)std::mbrtoc8(out, "", 0U, st);
        };
        (void)[](char *out, std::mbstate_t *st) {
            (void)std::c8rtomb(out, {}, st);
        };
#endif

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP20_FORMAT
        {
            (void) std::format("{}", 42);
        }
#endif

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP23_RANGE_FORMAT
        {
            const int aRange[] = {1, 2, 3, 4};
            (void) std::format("{}", aRange);
        }
#endif
      } break;
      case 18: {
        // --------------------------------------------------------------------
        // CHARCONV RELATED MACROS
        //
        // Concerns:
        // 1. If `BSLS_LIBRARYFEATURES_HAS_CPP17_CHARCONV` is defined
        //    `BSLS_LIBRARYFEATURES_HAS_CPP17_INT_CHARCONV` is also defined as
        //    they represent levels of support.
        //
        // 3. When `BSLS_LIBRARYFEATURES_HAS_CPP17_INT_CHARCONV` is defined
        //    both `std::to_chars` and `std::from_chars` overloads exist for
        //    all signed and unsigned standard integer types and `char`, as
        //    well as the `std::to_chars_result` and `std::from_chars_result`
        //    types are defined.
        //
        // 4. When `BSLS_LIBRARYFEATURES_HAS_CPP17_CHARCONV` is defined the
        //    `std::chars_format` type, as well as `std::from_chars` and
        //    `std::to_chars` overloads exists for all 3 standard floating
        //    point types (`float`, `double`, `long double`).
        //
        // Plan:
        // 1. Macro dependency tests (C-1) use the `u_*_defined` variables.
        // 2. Presence of functions is tested by defining calling lambdas.
        // 3. Presence of result types is tested by declarations.
        // 4. `std::chars_format` is used as argument in the float lambdas.
        //
        // Testing:
        //   BSLS_LIBRARYFEATURES_HAS_CPP17_INT_CHARCONV
        //   BSLS_LIBRARYFEATURES_HAS_CPP17_CHARCONV
        // --------------------------------------------------------------------

        if (verbose) puts("\nCHARCONV RELATED MACROS"
                          "\n=======================");

        if (verbose) {
            P(BSLS_LIBRARYFEATURES_HAS_CPP17_INT_CHARCONV_defined);
            P(BSLS_LIBRARYFEATURES_HAS_CPP17_CHARCONV_defined);
        }

        // C-1
        ASSERT(!BSLS_LIBRARYFEATURES_HAS_CPP17_CHARCONV_defined ||
               BSLS_LIBRARYFEATURES_HAS_CPP17_INT_CHARCONV_defined);

        // C-2
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_INT_CHARCONV
        // `std::to_chars` for singed/unsigned integers plus `char`
        {
            char *FIRST = nullptr;
            char *LAST  = nullptr;

            std::to_chars_result r1, r2;

            (void)[FIRST, LAST, &r1, &r2](char v) {
                r1 = std::to_chars(FIRST, LAST, v, 16);
                r2 = std::to_chars(FIRST, LAST, v);
            };

            (void)[FIRST, LAST, &r1, &r2](signed char v) {
                r1 = std::to_chars(FIRST, LAST, v, 16);
                r2 = std::to_chars(FIRST, LAST, v);
            };

            (void)[FIRST, LAST, &r1, &r2](unsigned char v) {
                r1 = std::to_chars(FIRST, LAST, v, 16);
                r2 = std::to_chars(FIRST, LAST, v);
            };

            (void)[FIRST, LAST, &r1, &r2](signed short v) {
                r1 = std::to_chars(FIRST, LAST, v, 16);
                r2 = std::to_chars(FIRST, LAST, v);
            };

            (void)[FIRST, LAST, &r1, &r2](unsigned short v) {
                r1 = std::to_chars(FIRST, LAST, v, 16);
                r2 = std::to_chars(FIRST, LAST, v);
            };

            (void)[FIRST, LAST, &r1, &r2](signed int v) {
                r1 = std::to_chars(FIRST, LAST, v, 16);
                r2 = std::to_chars(FIRST, LAST, v);
            };

            (void)[FIRST, LAST, &r1, &r2](unsigned int v) {
                r1 = std::to_chars(FIRST, LAST, v, 16);
                r2 = std::to_chars(FIRST, LAST, v);
            };

            (void)[FIRST, LAST, &r1, &r2](signed long v) {
                r1 = std::to_chars(FIRST, LAST, v, 16);
                r2 = std::to_chars(FIRST, LAST, v);
            };

            (void)[FIRST, LAST, &r1, &r2](unsigned long v) {
                r1 = std::to_chars(FIRST, LAST, v, 16);
                r2 = std::to_chars(FIRST, LAST, v);
            };

            (void)[FIRST, LAST, &r1, &r2](signed long long v) {
                r1 = std::to_chars(FIRST, LAST, v, 16);
                r2 = std::to_chars(FIRST, LAST, v);
            };

            (void)[FIRST, LAST, &r1, &r2](unsigned long long v) {
                r1 = std::to_chars(FIRST, LAST, v, 16);
                r2 = std::to_chars(FIRST, LAST, v);
            };
        }

        // `std::from_chars` for singed/unsigned integers plus `char`
        {
            const char *FIRST = nullptr;
            const char *LAST  = nullptr;

            std::from_chars_result r1, r2;

            (void)[FIRST, LAST, &r1, &r2](char v) {
                r1 = std::from_chars(FIRST, LAST, v, 16);
                r2 = std::from_chars(FIRST, LAST, v);
                return v;
            };

            (void)[FIRST, LAST, &r1, &r2](signed char v) {
                r1 = std::from_chars(FIRST, LAST, v, 16);
                r2 = std::from_chars(FIRST, LAST, v);
                return v;
            };

            (void)[FIRST, LAST, &r1, &r2](unsigned char v) {
                r1 = std::from_chars(FIRST, LAST, v, 16);
                r2 = std::from_chars(FIRST, LAST, v);
                return v;
            };

            (void)[FIRST, LAST, &r1, &r2](signed short v) {
                r1 = std::from_chars(FIRST, LAST, v, 16);
                r2 = std::from_chars(FIRST, LAST, v);
                return v;
            };

            (void)[FIRST, LAST, &r1, &r2](unsigned short v) {
                r1 = std::from_chars(FIRST, LAST, v, 16);
                r2 = std::from_chars(FIRST, LAST, v);
                return v;
            };

            (void)[FIRST, LAST, &r1, &r2](signed int v) {
                r1 = std::from_chars(FIRST, LAST, v, 16);
                r2 = std::from_chars(FIRST, LAST, v);
                return v;
            };

            (void)[FIRST, LAST, &r1, &r2](unsigned int v) {
                r1 = std::from_chars(FIRST, LAST, v, 16);
                r2 = std::from_chars(FIRST, LAST, v);
                return v;
            };

            (void)[FIRST, LAST, &r1, &r2](signed long v) {
                r1 = std::from_chars(FIRST, LAST, v, 16);
                r2 = std::from_chars(FIRST, LAST, v);
                return v;
            };

            (void)[FIRST, LAST, &r1, &r2](unsigned long v) {
                r1 = std::from_chars(FIRST, LAST, v, 16);
                r2 = std::from_chars(FIRST, LAST, v);
                return v;
            };

            (void)[FIRST, LAST, &r1, &r2](signed long long v) {
                r1 = std::from_chars(FIRST, LAST, v, 16);
                r2 = std::from_chars(FIRST, LAST, v);
                return v;
            };

            (void)[FIRST, LAST, &r1, &r2](unsigned long long v) {
                r1 = std::from_chars(FIRST, LAST, v, 16);
                r2 = std::from_chars(FIRST, LAST, v);
                return v;
            };
        }
#endif // BSLS_LIBRARYFEATURES_HAS_CPP17_INT_CHARCONV

        // C-3
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_CHARCONV
        // `std::to_chars` - for `float`, `double`, and `long double`
        {
            char *FIRST = nullptr;
            char *LAST  = nullptr;

            (void)[FIRST, LAST](float v) {
                std::to_chars(FIRST, LAST, v, std::chars_format::hex);
                std::to_chars(FIRST, LAST, v);
            };

            (void)[FIRST, LAST](double v) {
                std::to_chars(FIRST, LAST, v, std::chars_format::hex);
                std::to_chars(FIRST, LAST, v);
            };

            (void)[FIRST, LAST](long double v) {
                std::to_chars(FIRST, LAST, v, std::chars_format::hex);
                std::to_chars(FIRST, LAST, v);
            };
        }

        // `std::from_chars` - for `float`, `double`, and `long double`
        {
            const char *FIRST = nullptr;
            const char *LAST  = nullptr;

            (void)[FIRST, LAST](float v) {
                std::from_chars(FIRST, LAST, v, std::chars_format::hex);
                std::from_chars(FIRST, LAST, v);
                return v;
            };

            (void)[FIRST, LAST](double v) {
                std::from_chars(FIRST, LAST, v, std::chars_format::hex);
                std::from_chars(FIRST, LAST, v);
                return v;
            };

            (void)[FIRST, LAST](long double v) {
                std::from_chars(FIRST, LAST, v, std::chars_format::hex);
                std::from_chars(FIRST, LAST, v);
                return v;
            };
        }
#endif  // BSLS_LIBRARYFEATURES_HAS_CPP17_CHARCONV
      } break;
      case 17: {
        // --------------------------------------------------------------------
        // `BSLS_LIBRARYFEATURES_HAS_CPP20_BASELINE_LIBRARY`
        //
        // Concerns:
        // 1. `BSLS_LIBRARYFEATURES_HAS_CPP20_BASELINE_LIBRARY` is defined only
        //    when the native standard library provides a baseline of C++20
        //    library features, including:
        //    - `span`
        //    - `barrier`
        //    - `latch`
        //    - `countingSemaphore`
        //    - `to_array`
        //    - `remove_cvref`
        //    - `type_identity`
        //
        // Plan:
        // 1. When `BSLS_LIBRARYFEATURES_HAS_CPP20_BASELINE_LIBRARY` is
        //    defined include the appropriate headers and use the expected
        //    typenames.
        //
        // Testing:
        //   BSLS_LIBRARYFEATURES_HAS_CPP20_BASELINE_LIBRARY
        // --------------------------------------------------------------------

        if (verbose)
            puts("\n'BSLS_LIBRARYFEATURES_HAS_CPP20_BASELINE_LIBRARY'"
                 "\n=================================================");

        if (verbose) {
            P(BSLS_LIBRARYFEATURES_HAS_CPP20_BASELINE_LIBRARY_defined)
        }

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP20_BASELINE_LIBRARY
        {
            std::span<int> x;
            (void)x;
        }

        {
            std::barrier barrier(1);
            (void)barrier;
        }

        {
            std::latch latch(0);
            (void)latch;
        }

        {
            std::counting_semaphore countingSemaphore(0);
            std::binary_semaphore binarySemaphore(0);

            (void)countingSemaphore;
            (void)binarySemaphore;
        }

        {
            // String is there since the macro will get replaced by value.
            ASSERTV("__cpp_lib_to_array >= 201907L check",
                    __cpp_lib_to_array,
                    __cpp_lib_to_array >= 201907L);

            if (veryVerbose) printf("... testing to_array(T&[N])\n");
            {
                int src[5] = {1, 2, 3, 4, 5};
                std::array<int, 5> dest = std::to_array(src);
                for (std::size_t i = 0; i < sizeof(src)/sizeof(*src); ++i) {
                    ASSERTV(i, src[i], dest[i], src[i] == dest[i]);
                }
            }

            if (veryVerbose) printf("... testing to_array(T&&[N])\n");
            {
                int src[5] = {1, 2, 3, 4, 5};
                int check[5] = {1, 2, 3, 4, 5};
                std::array<int, 5> dest = std::to_array(std::move(src));
                for (std::size_t i = 0;
                     i < sizeof(check)/sizeof(*check);
                     ++i) {
                    ASSERTV(i, dest[i], check[i], dest[i] == check[i]);
                }
            }
        }

        {
            typedef int T;

            // Types defined in `<type_traits>`
            typedef std::remove_cvref  <T>::type TypeSansCvref;
            typedef std::remove_cvref_t<T>       TypeSansCvref_t;

            TypeSansCvref   x; (void)x;
            TypeSansCvref_t y; (void)y;
        }
        {
            typedef int T;

            // Types defined in `<type_traits>`
            typedef std::type_identity  <T>::type TypeIdentity;
            typedef std::type_identity_t<T>       TypeIdentity_t;

            TypeIdentity   x; (void)x;
            TypeIdentity_t y; (void)y;
        }

#endif // BSLS_LIBRARYFEATURES_HAS_CPP20_BASELINE_LIBRARY

        if (veryVeryVerbose) P(BSLS_PLATFORM_CMP_VERSION);
      } break;
      case 16: {
        // --------------------------------------------------------------------
        // `BSLS_LIBRARYFEATURES_HAS_CPP17_RANGE_FUNCTIONS`
        //
        // Concerns:
        // 1. `BSLS_LIBRARYFEATURES_HAS_CPP17_RANGE_FUNCTIONS` is defined only
        //    when the native standard library provides a baseline of C++17
        //    library features (empty, data, size).
        //
        // Plan:
        // 1. When `BSLS_LIBRARYFEATURES_HAS_CPP17_RANGE_FUNCTIONS` is
        //    defined include the appropriate headers and use the expected
        //    typenames.
        //
        // Testing:
        //   BSLS_LIBRARYFEATURES_HAS_CPP17_RANGE_FUNCTIONS
        // --------------------------------------------------------------------

        if (verbose)
            puts("\n'BSLS_LIBRARYFEATURES_HAS_CPP17_RANGE_FUNCTIONS'"
                 "\n================================================");

        if (verbose) {
            P(BSLS_LIBRARYFEATURES_HAS_CPP17_RANGE_FUNCTIONS_defined);
        }

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_RANGE_FUNCTIONS
        std::string s;
        (void)std::empty(s);
        (void)std::data(s);
        (void)std::size(s);
#endif
        if (veryVeryVerbose) P(BSLS_PLATFORM_CMP_VERSION);
      } break;
      case 15: {
        // --------------------------------------------------------------------
        // `BSLS_LIBRARYFEATURES_HAS_CPP17_*` MISCELLANY
        //
        // Concerns:
        // 1. `BSLS_LIBRARYFEATURES_HAS_CPP17_ALIGNED_ALLOC` and
        //    `BSLS_LIBRARYFEATURES_HAS_CPP17_TIMESPEC_GET` are defined only
        //    when the native standard library provides them.
        //
        // Plan:
        // 1. When `BSLS_LIBRARYFEATURES_HAS_CPP17_ALIGNED_ALLOC` and/or
        //    `BSLS_LIBRARYFEATURES_HAS_CPP17_TIMESPEC_GET` are defined
        //    include the appropriate headers and use the expected calls.
        //
        // Testing:
        //   BSLS_LIBRARYFEATURES_HAS_CPP17_ALIGNED_ALLOC
        //   BSLS_LIBRARYFEATURES_HAS_CPP17_TIMESPEC_GET
        //   BSLS_LIBRARYFEATURES_HAS_CPP17_PMR
        //   BSLS_LIBRARYFEATURES_HAS_CPP17_PMR_STRING
        // --------------------------------------------------------------------

        if (verbose) puts("\n'BSLS_LIBRARYFEATURES_HAS_CPP17_*' MISCELLANY"
                          "\n=============================================");

        if (verbose) {
            P(BSLS_LIBRARYFEATURES_HAS_CPP17_ALIGNED_ALLOC_defined);
            P(BSLS_LIBRARYFEATURES_HAS_CPP17_TIMESPEC_GET_defined);
            P(BSLS_LIBRARYFEATURES_HAS_CPP17_PMR_defined);
            P(BSLS_LIBRARYFEATURES_HAS_CPP17_PMR_STRING_defined);
        }

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_ALIGNED_ALLOC
        {
            std::free(std::aligned_alloc(1024, 1024));
        }
#endif

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_TIMESPEC_GET
        {
            std::timespec ts;
            (void)std::timespec_get(&ts, TIME_UTC);
        }
#endif

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_PMR
        {
            (void)(std::pmr::polymorphic_allocator<char> *)0;
            (void)(std::pmr::memory_resource *)0;
            (void)(std::pmr::pool_options *)0;
            (void)(std::pmr::synchronized_pool_resource *)0;
            (void)(std::pmr::unsynchronized_pool_resource *)0;
            (void)(std::pmr::monotonic_buffer_resource *)0;

            (void)std::pmr::new_delete_resource();
            (void)std::pmr::null_memory_resource();
            (void)std::pmr::get_default_resource();
            (void)[](std::pmr::memory_resource *r) {
                (void)std::pmr::set_default_resource(r);
            };
        }
#endif

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_PMR_STRING
        {
            (void)(std::pmr::basic_string<char> *)0;
            (void)(std::pmr::string *)0;
            (void)(std::pmr::wstring *)0;
            (void)(std::pmr::u16string *)0;
            (void)(std::pmr::u32string *)0;
        }
#endif
      } break;
      case 14: {
        // --------------------------------------------------------------------
        // `BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY`
        //
        // Concerns:
        // 1. `BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY` is defined only
        //    when the native standard library provides a baseline of C++17
        //    library features (any, optional, variant, string_view).
        //
        // Plan:
        // 1. When `BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY` is
        //    defined include the appropriate headers and use the expected
        //    typenames.
        //
        // Testing:
        //   BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY
        // --------------------------------------------------------------------

        if (verbose)
            puts("\n'BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY'"
                 "\n=================================================");

        if (verbose) {
            P(BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY_defined);
        }

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY
        {
            std::any x;
            (void)x;
        }
        {
            std::optional<int> x;
            (void)x;
        }
        {
            std::string_view x;
            (void)x;
        }
        {
            std::variant<int> x;
            (void)x;
        }
#endif
        if (veryVeryVerbose) P(BSLS_PLATFORM_CMP_VERSION);
      } break;
      case 13: {
        // --------------------------------------------------------------------
        // `BSLS_LIBRARYFEATURES_HAS_CPP17_SEARCH_*` MACROS
        //
        // Concerns:
        // 1. The `BSLS_LIBRARYFEATURES_HAS_CPP17_SEARCH_OVERLOAD` macro is
        //    defined when the native library provides a definition for an
        //    overload of the `search` function template that accepts a
        //    searcher object, and not otherwise.
        //
        // 2. The `BSLS_LIBRARYFEATURES_HAS_CPP17_SEARCH_FUNCTORS` macro is
        //    defined when the native library provides the definition for all
        //    of the `search` functor templates in <functional>, and not
        //    otherwise.
        //
        // 3. The `BSLS_LIBRARYFEATURES_HAS_CPP17_SEARCH_FUNCTORS` macro is
        //    only defined when the
        //    `BSLS_LIBRARYFEATURES_HAS_CPP17_SEARCH_OVERLOAD` macro is also
        //    defined.  This is a santy check as we know that no implementation
        //    exists that would defined the functors but not the algorithm.
        //
        // Plan:
        // 1. In namespace `case13`, define `SearcherNull`, a class that is
        //    compatible with the "searcher" concept and an independent
        //    definition of the `search` overload under test.
        //
        // 2. If `BSLS_LIBRARYFEATURES_HAS_CPP17_SEARCH_OVERLOAD` is set,
        //    confirm that the overload exists, that it accepts the searcher
        //    object, and returns the expected result.
        //
        // 3. If `BSLS_LIBRARYFEATURES_HAS_CPP17_SEARCH_FUNCTORS` is set,
        //    confirm that the three native types for searcher objects exist.
        //
        // 4. If `BSLS_LIBRARYFEATURES_HAS_CPP17_SEARCH_OVERLOAD` is *not* set,
        //    specify `using` directives to search for definitions in both the
        //    `std` and the `case13` namespaces.  Then define an
        //    an expression using the namespace-unqualified name `search`.  If
        //    there is a definition in the `std` namespace in addition
        //    to the one we planted in namespace `case13`, the test driver will
        //    fail to compile (ambiguity error).
        //
        // 5. If `BSLS_LIBRARYFEATURES_HAS_CPP17_SEARCH_FUNCTORS` is *not* set
        //    set, we do not test for the possible presence of the `std`
        //    functors, because only one of them may be missing, which we
        //    cannot test for, or they may be faulty and not indicated for that
        //    reason.
        //
        // Testing:
        //   BSLS_LIBRARYFEATURES_HAS_CPP17_SEARCH_OVERLOAD
        //   BSLS_LIBRARYFEATURES_HAS_CPP17_SEARCH_FUNCTORS
        // --------------------------------------------------------------------

        if (verbose)
            puts("\n'BSLS_LIBRARYFEATURES_HAS_CPP17_SEARCH_*' MACROS"
                 "\n================================================");

        if (verbose) {
            P(BSLS_LIBRARYFEATURES_HAS_CPP17_SEARCH_OVERLOAD_defined);
            P(BSLS_LIBRARYFEATURES_HAS_CPP17_SEARCH_FUNCTORS_defined);
        }

        case13::SearcherNull searcher;

        const char  needle[]      = "world";
        const char *needleFirst   = needle;
        const char *needleLast    = needle + sizeof needle - 1;

        const char  haystack[]    = "Hello, world!";
        const char *haystackFirst = haystack;
        const char *haystackLast  = haystack + sizeof haystack - 1;

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_SEARCH_OVERLOAD

        const char *result = std::search(haystackFirst,
                                         haystackLast,
                                         searcher);
        ASSERT(haystackLast == result);
#else
        using namespace std;
        using namespace case13;

        const char *result = search(haystackFirst, haystackLast, searcher);
        ASSERT(haystackLast == result);
#endif

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_SEARCH_FUNCTORS
        std::default_searcher              dftSearcher(needleFirst,
                                                       needleLast);
        std::boyer_moore_searcher           bmSearcher(needleFirst,
                                                       needleLast);
        std::boyer_moore_horspool_searcher bmhSearcher(needleFirst,
                                                       needleLast);
#else
        (void)needleFirst;
        (void)needleLast;
#endif
        if (veryVeryVerbose) P(BSLS_PLATFORM_CMP_VERSION);
      } break;
      case 12: {
        // --------------------------------------------------------------------
        // `BSLS_LIBRARYFEATURES_HAS_CPP11_RANGE_FUNCTIONS`
        //
        // Concerns:
        // 1. The `BSLS_LIBRARYFEATURES_HAS_CPP11_RANGE_FUNCTIONS` flag is
        //    defined when the `begin` and `end` function templates are
        //    provided by the native standard library.
        //
        // 2. The `BSLS_LIBRARYFEATURES_HAS_CPP11_RANGE_FUNCTIONS` flag is not
        //    defined unless the `begin` and `end` function templates are
        //    provided by the native standard library.
        //
        // Plan:
        // 1. Write a test type, `case13::TestType`, that has members `begin`
        //    and `end`, returning a correspondingly defined `iterator_type`.
        //
        // 2. If the feature macro is defined, explicitly call `std::begin` and
        //    `std::end` to confirm they exist with a compatible signature.
        //
        // 3. If the feature macro is NOT defined, apply a `using namespace` to
        //    both namespaces `std` and `case13`, and then call `begin` and
        //    `end`.  The call will be ambiguous and force a compile-time error
        //    only if the `std` functions are also available, indicating that
        //    the feature macro should be defined.
        //
        // Testing:
        //   BSLS_LIBRARYFEATURES_HAS_CPP11_RANGE_FUNCTIONS
        // --------------------------------------------------------------------

        if (verbose)
            puts("\n'BSLS_LIBRARYFEATURES_HAS_CPP11_RANGE_FUNCTIONS'"
                 "\n================================================");

        if (verbose) {
            P(BSLS_LIBRARYFEATURES_HAS_CPP11_RANGE_FUNCTIONS_defined);
        }

        case12::TestType mX = { 12 };
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_RANGE_FUNCTIONS
        {
            int *from = std::begin(mX);    (void)from;
            int *to   = std::end(mX);      (void)to;
        }
#else
        {
            // This will produce ambiguities if `begin` and `end` are defined
            // in both namespaces.

            using namespace std;
            using namespace case12;

            int *from = begin(mX);         (void)from;
            int *to   = end(mX);           (void)to;
        }
#endif
        if (veryVeryVerbose) P(BSLS_PLATFORM_CMP_VERSION);
      } break;
      case 11: {
        // --------------------------------------------------------------------
        // `BSLS_LIBRARYFEATURES_HAS_CPP14_INTEGER_SEQUENCE`
        //
        // Concerns:
        // 1. The `BSLS_LIBRARYFEATURES_HAS_CPP14_INTEGER_SEQUENCE` flag is
        //    defined when related macros
        //    `BSLS_LIBRARYFEATURES_HAS_CPP11_BASE_LINE`,
        //    `BSLS_COMPILERFEATURES_SUPPORT_VARIADIC_TEMPLATES` and
        //    `BSLS_COMPILERFEATURES_SUPPORT_ALIAS_TEMPLATES` are also defined.
        //
        // Plan:
        // 1. Confirm the expected relationship between
        //    `BSLS_LIBRARYFEATURES_HAS_CPP14_INTEGER_SEQUENCE` and its related
        //    macros `BSLS_LIBRARYFEATURES_HAS_CPP11_BASE_LINE`,
        //    `BSLS_COMPILERFEATURES_SUPPORT_VARIADIC_TEMPLATES` and
        //    `BSLS_COMPILERFEATURES_SUPPORT_ALIAS_TEMPLATES` using the
        //    associated conditionally initialized global variables.  See
        //    "Global constants for testing invariants" above.
        //
        // Testing:
        //   BSLS_LIBRARYFEATURES_HAS_CPP14_INTEGER_SEQUENCE
        // --------------------------------------------------------------------

        if (verbose)
            puts("\n'BSLS_LIBRARYFEATURES_HAS_CPP14_INTEGER_SEQUENCE'"
                 "\n=================================================");

        if (verbose) {
            P(BSLS_LIBRARYFEATURES_HAS_CPP14_INTEGER_SEQUENCE_defined);
        }

        if (BSLS_LIBRARYFEATURES_HAS_CPP14_INTEGER_SEQUENCE_defined)
        {
            ASSERT(BSLS_COMPILERFEATURES_SUPPORT_ALIAS_TEMPLATES_defined);
            ASSERT(BSLS_COMPILERFEATURES_SUPPORT_VARIADIC_TEMPLATES_defined);
        }

        if (veryVeryVerbose) P(BSLS_PLATFORM_CMP_VERSION);
      } break;
      case 10: {
        // --------------------------------------------------------------------
        // STANDARD LIBRARY IMPLEMENTATION DETECTION
        //
        // Concerns:
        // 1. We detect the expected libraries for the compilers
        //
        // 2. We detect the right library implementation
        //
        // Plan:
        // 1. Verify that the compiler type is one of the expected ones for the
        //    detected standard library implementation.
        //
        // 2. Try to include header files that exist only in the given standard
        //    library implementation to verify that we actually have that
        //    implementation.  This is done in the .cpp file so that it the
        //    sanity check is always done.  To find it, search for
        //    `Standard library implementation detection verification` in the
        //    implementation (.cpp) file.
        //
        // Testing:
        //  BSLS_LIBRARYFEATURES_STDCPP_GNU
        //  BSLS_LIBRARYFEATURES_STDCPP_IBM
        //  BSLS_LIBRARYFEATURES_STDCPP_LLVM
        //  BSLS_LIBRARYFEATURES_STDCPP_MSVC
        //  BSLS_LIBRARYFEATURES_STDCPP_LIBCSTD
        //  BSLS_LIBRARYFEATURES_STDCPP_STLPORT
        // --------------------------------------------------------------------

        if (verbose) puts("\nSTANDARD LIBRARY IMPLEMENTATION DETECTION"
                          "\n=========================================");

        if (veryVeryVerbose) P(BSLS_PLATFORM_CMP_VERSION);

#ifdef BSLS_LIBRARYFEATURES_STDCPP_GNU
# ifdef BSLS_PLATFORM_CMP_GNU
# elif  BSLS_PLATFORM_CMP_CLANG
# elif  BSLS_PLATFORM_CMP_SUN && BSLS_PLATFORM_CMP_VERSION >= 0x5130
# else
#   error Unexpected compiler for GNU LibStdC++.
# endif
#elif defined(BSLS_LIBRARYFEATURES_STDCPP_MSVC)
# ifdef BSLS_PLATFORM_CMP_MSVC
# else
#   error Unexpected compiler for Microsoft STL.
# endif
#elif defined(BSLS_LIBRARYFEATURES_STDCPP_LLVM)
# ifdef BSLS_PLATFORM_CMP_CLANG
# else
#   error Unexpected compiler for LLVM LibC++.
# endif
#elif defined(BSLS_LIBRARYFEATURES_STDCPP_LIBCSTD)
# ifdef BSLS_PLATFORM_CMP_SUN
# else
#   error Unexpected compiler for RogueWave STL.
# endif
#elif defined(BSLS_LIBRARYFEATURES_STDCPP_STLPORT)
# ifdef BSLS_PLATFORM_CMP_SUN
# else
#   error Unexpected compiler for STLPort.
# endif
#elif defined(BSLS_LIBRARYFEATURES_STDCPP_IBM)
# ifdef BSLS_PLATFORM_CMP_IBM
# else
#   error Unexpected compiler for IBM STL.
# endif
#else
# error Unexpected standard library implementation.  Please update test driver.
#endif
      } break;
      case 9: {
        // --------------------------------------------------------------------
        // `BSLS_LIBRARYFEATURES_HAS_CPP14_BASELINE_LIBRARY`
        //
        // Concerns:
        // 1. The `BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY` flag is
        //    defined when the native standard library defines the following
        //    functions and types:
        //
        //    - Type defined in `<functional>`
        //      - bit_not
        //
        //    - Function defined in `<iterator>`
        //      - make_reverse_iterator
        //
        //    - Function defined in `<iomanip>`
        //      - quoted
        //
        //    - UDLs support for `<chrono>`
        //
        //    - UDLs support for `<complex>`
        //
        // Plan:
        // 1. When `BSLS_LIBRARYFEATURES_HAS_CPP14_BASELINE_LIBRARY` is defined
        //    conditionally compile code that includes `<functional>`,
        //    `<iomanip>`, `<iterator>`, `<chrono>`, `<complex>` and uses each
        //    of the listed function templates at least once.
        //
        // Testing:
        //   BSLS_LIBRARYFEATURES_HAS_CPP14_BASELINE_LIBRARY
        // --------------------------------------------------------------------

        if (verbose)
            puts("\n'BSLS_LIBRARYFEATURES_HAS_CPP14_BASELINE_LIBRARY'"
                 "\n=================================================");

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP14_BASELINE_LIBRARY
        useCpp14Algorithms();
#endif

        if (verbose) {
            P(BSLS_LIBRARYFEATURES_HAS_CPP14_BASELINE_LIBRARY_defined);
        }

        if (veryVeryVerbose) P(BSLS_PLATFORM_CMP_VERSION);
      } break;
      case 8: {
        // --------------------------------------------------------------------
        // `BSLS_LIBRARYFEATURES_HAS_CPP17_PRECISE_BITWIDTH_ATOMICS`
        //
        // Concerns:
        // 1. The `BSLS_LIBRARYFEATURES_HAS_CPP17_PRECISE_BITWIDTH_ATOMICS`
        //    flag is defined when the native standard library defines the
        //    following types:
        //
        //    - Types defined in `<atomic>`
        //
        //      - atomic class template and specializations for precise
        //        bitwidth integral types
        //
        //      - atomic class template and specializations for pointer types
        //
        // Plan:
        // 1. When `BSLS_LIBRARYFEATURES_HAS_CPP17_PRECISE_BITWIDTH_ATOMICS` is
        //    defined conditionally compile code that includes `<atomic>`, and
        //    uses each of the listed types at least once.  (C-1)
        //
        // Testing:
        //   BSLS_LIBRARYFEATURES_HAS_CPP17_PRECISE_BITWIDTH_ATOMICS
        // --------------------------------------------------------------------

        if (verbose)
           puts("\n'BSLS_LIBRARYFEATURES_HAS_CPP17_PRECISE_BITWIDTH_ATOMICS'"
                "\n=========================================================");

        if (verbose) {
          P(BSLS_LIBRARYFEATURES_HAS_CPP17_PRECISE_BITWIDTH_ATOMICS_defined);
        }

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_PRECISE_BITWIDTH_ATOMICS
        useCpp11PreciseBitwidthAtomics();
#endif
        if (veryVeryVerbose) P(BSLS_PLATFORM_CMP_VERSION);
      } break;
      case 7: {
        // --------------------------------------------------------------------
        // `isblank`
        //
        // Concerns:
        // 1. The one-argument form of `isblank` is available when
        //    `BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY` is defined.
        //
        // 2. The two-argument locale form of `isblank` is available when
        //    `BSLS_LIBRARYFEATURES_HAS_CPP11_MISCELLANEOUS_UTILITIES` is
        //    defined.
        //
        // Plan:
        // 1. When `BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY` is defined
        //    compile code that uses the one-argument `isblank`.
        //
        // 2. When `BSLS_LIBRARYFEATURES_HAS_CPP11_MISCELLANEOUS_UTILITIES` is
        //    defined compile code that uses the two-argument `isblank`.
        //
        // Testing:
        //   int std::isblank(int);
        //   bool std::isblank(char, const std::locale&);
        // --------------------------------------------------------------------

        if (verbose) puts("\n'isblank'"
                          "\n=========");

        if (verbose) {
           P(BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY_defined);
           P(BSLS_LIBRARYFEATURES_HAS_CPP11_MISCELLANEOUS_UTILITIES_defined);
        }

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY
        if (verbose) {
            printf("Expecting `isblank` from <cctype>\n");
        }
        (void)static_cast<int (*)(int)>(&std::isblank);
#else
        if (verbose) {
            printf("Not expecting `isblank` from <cctype>\n");
        }
#endif
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_MISCELLANEOUS_UTILITIES
        if (verbose) {
            printf("Expecting `isblank` from <locale>\n");
        }
        (void)static_cast<bool (*)(char, const std::locale&)>(&std::isblank);
#else
        if (verbose) {
            printf("Not expecting `isblank` from <locale>\n");
        }
#endif
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // `BSLS_LIBRARYFEATURES_HAS_C99_*`
        //
        // Concerns:
        // 1. `BSLS_LIBRARYFEATURES_HAS_C99_LIBRARY` is defined only when the
        //    native standard library provides C99 features.
        //
        // 2. `BSLS_LIBRARYFEATURES_HAS_C99_SNPRINTF` is defined only when the
        //    native standard library provides C99 `snprintf`.
        //
        //
        // Plan:
        // 1. When `BSLS_LIBRARYFEATURES_HAS_C99_LIBRARY` is defined compile
        //    code that uses C99 library functions.
        //
        // 2. If `BSLS_LIBRARYFEATURES_HAS_C99_SNPRINTF` is defined compile
        //    code using `snprintf`.
        //
        // Testing:
        //   BSLS_LIBRARYFEATURES_HAS_C99_LIBRARY
        //   BSLS_LIBRARYFEATURES_HAS_C99_SNPRINTF
        // --------------------------------------------------------------------

        if (verbose) puts("\n'BSLS_LIBRARYFEATURES_HAS_C99_*'"
                          "\n================================");

        if (verbose) {
            P(BSLS_LIBRARYFEATURES_HAS_C99_LIBRARY_defined);
            P(BSLS_LIBRARYFEATURES_HAS_C99_SNPRINTF_defined);
        }

        if (BSLS_LIBRARYFEATURES_HAS_CPP11_UNIQUE_PTR_defined) {
            ASSERT(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES_defined);
        }

#ifdef BSLS_LIBRARYFEATURES_HAS_C99_LIBRARY
        // Test a subset of C99 features

        // cmath
        {
            typedef int (*FuncPtrType)(double);
            FuncPtrType funcPtr = &std::fpclassify;
            (void)funcPtr;  // suppress unused variable warning
        }

        // cstdlib
        {
            typedef std::lldiv_t dummy;
            dummy x;  // suppress unused typedef warning
            (void)x;  // suppress unused variable warning
        }

        // cctype
        {
            typedef int (*FuncPtrType)(int);
            FuncPtrType funcPtr = &std::isblank;
            (void)funcPtr;  // suppress unused variable warning

        }
#endif

#ifdef BSLS_LIBRARYFEATURES_HAS_C99_SNPRINTF
        (void)&std::snprintf;
#endif
        if (veryVeryVerbose) P(BSLS_PLATFORM_CMP_VERSION);
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // `BSLS_LIBRARYFEATURES_HAS_CPP11_UNIQUE_PTR`
        //
        // Concerns:
        // 1. `BSLS_LIBRARYFEATURES_HAS_CPP11_UNIQUE_PTR` is defined only when
        //    the native standard library defines the `unique_ptr` class
        //    template (in `<memory>`).
        //
        // 2. If `BSLS_LIBRARYFEATURES_HAS_CPP11_UNIQUE_PTR` is defined then
        //    the related macro
        //    `BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES` is also
        //    defined.
        //
        // Plan:
        // 1. When `BSLS_LIBRARYFEATURES_HAS_CPP11_UNIQUE_PTR` is defined
        //    compile code that includes `<memory>` and constructs a
        //    `unique_ptr` object to an `int`.
        //
        // 2. If `BSLS_LIBRARYFEATURES_HAS_CPP11_UNIQUE_PTR` is defined confirm
        //    that `BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES` is also
        //    defined using the associated conditionally initialized global
        //    variables.  See "Global constants for testing invariants" above.
        //
        // Testing:
        //   BSLS_LIBRARYFEATURES_HAS_CPP11_UNIQUE_PTR
        // --------------------------------------------------------------------

        if (verbose) puts("\n'BSLS_LIBRARYFEATURES_HAS_CPP11_UNIQUE_PTR'"
                          "\n===========================================");

        if (verbose) {
            P(BSLS_LIBRARYFEATURES_HAS_CPP11_UNIQUE_PTR_defined);
        }

        if (BSLS_LIBRARYFEATURES_HAS_CPP11_UNIQUE_PTR_defined) {
            ASSERT(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES_defined);
        }

        if (veryVeryVerbose) P(BSLS_PLATFORM_CMP_VERSION);
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // `BSLS_LIBRARYFEATURES_HAS_CPP11_TUPLE`
        //
        // Concerns:
        // 1. `BSLS_LIBRARYFEATURES_HAS_CPP11_TUPLE` is defined only when the
        //    native standard library defines the `<tuple>` class template.
        //
        // 2. If `BSLS_LIBRARYFEATURES_HAS_CPP11_TUPLE` is defined then the
        //    related macro `BSLS_COMPILERFEATURES_SUPPORT_VARIADIC_TEMPLATES`
        //    is also defined.
        //
        // Plan:
        // 1. When `BSLS_LIBRARYFEATURES_HAS_CPP11_TUPLE` is defined compile
        //    code that includes `<tuple>` and constructs a `tuple` object.
        //
        // 2. If `BSLS_LIBRARYFEATURES_HAS_CPP11_TUPLE` is defined confirm that
        //    `BSLS_COMPILERFEATURES_SUPPORT_VARIADIC_TEMPLATES` is also
        //    defined using the associated conditionally initialized global
        //    variables.  See "Global constants for testing invariants" above.
        //
        // Testing:
        //   BSLS_LIBRARYFEATURES_HAS_CPP11_TUPLE
        // --------------------------------------------------------------------

        if (verbose) puts("\n'BSLS_LIBRARYFEATURES_HAS_CPP11_TUPLE'"
                          "\n======================================");

        if (verbose) {
            P(BSLS_LIBRARYFEATURES_HAS_CPP11_TUPLE_defined);
        }

        if (BSLS_LIBRARYFEATURES_HAS_CPP11_TUPLE_defined) {
            ASSERT(BSLS_COMPILERFEATURES_SUPPORT_VARIADIC_TEMPLATES_defined);
        }

        if (veryVeryVerbose) P(BSLS_PLATFORM_CMP_VERSION);
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // `BSLS_LIBRARYFEATURES_HAS_CPP11_PAIR_PIECEWISE_CONSTRUCTOR`
        //
        // Concerns:
        // 1. The `BSLS_LIBRARYFEATURES_HAS_CPP11_PAIR_PIECEWISE_CONSTRUCTOR`
        //    flag is defined when the native standard library defines for its
        //    `pair` class template (defined in `<utility>`) a constructor that
        //    accepts as arguments `std::piecewise_construct` (also
        //    defined in `<utility>` followed by two `tuple` arguments.
        //
        // 2. If `BSLS_LIBRARYFEATURES_HAS_CPP11_PAIR_PIECEWISE_CONSTRUCTOR` is
        //    defined then related macros
        //    `BSLS_LIBRARYFEATURES_HAS_CPP11_TUPLE`
        //    `BSLS_COMPILERFEATURES_SUPPORT_VARIADIC_TEMPLATES` are also
        //    defined.
        //
        // Plan:
        // 1. When `BSLS_LIBRARYFEATURES_HAS_CPP11_PAIR_PIECEWISE_CONSTRUCTOR`
        //    is defined conditionally compile code that includes `<utility>`,
        //    `<tuple>` and creates an object using the constructor specified
        //    in C-1.
        //
        // 2. Confirm the expected relationship between
        //    `BSLS_LIBRARYFEATURES_HAS_CPP11_PAIR_PIECEWISE_CONSTRUCTOR` and
        //    its related macros `BSLS_LIBRARYFEATURES_HAS_CPP11_TUPLE`
        //    `BSLS_LIBRARYFEATURES_HAS_CPP98_AUTO_PTR` using the associated
        //    conditionally initialized global variables.  See "Global
        //    constants for testing invariants" above.
        //
        // Testing:
        //   BSLS_LIBRARYFEATURES_HAS_CPP11_PAIR_PIECEWISE_CONSTRUCTOR
        // --------------------------------------------------------------------

        if (verbose) puts(
              "\n'BSLS_LIBRARYFEATURES_HAS_CPP11_PAIR_PIECEWISE_CONSTRUCTOR'"
              "\n===========================================================");

        if (verbose) {
          P(BSLS_LIBRARYFEATURES_HAS_CPP11_PAIR_PIECEWISE_CONSTRUCTOR_defined);
        }

        if (BSLS_LIBRARYFEATURES_HAS_CPP11_PAIR_PIECEWISE_CONSTRUCTOR_defined){
            ASSERT(BSLS_LIBRARYFEATURES_HAS_CPP11_TUPLE_defined);
            ASSERT(BSLS_COMPILERFEATURES_SUPPORT_VARIADIC_TEMPLATES_defined);
        }

        if (veryVeryVerbose) P(BSLS_PLATFORM_CMP_VERSION);
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // `BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY`
        //
        // Concerns:
        // 1. The `BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY` flag is
        //    defined when the native standard library defines the following
        //    functions and types:
        //
        //    - Functions defined in `<algorithm>`
        //      - `all_of`
        //      - `any_of`
        //      - `copy_if`
        //      - `copy_n`
        //      - `find_if_not`
        //      - `is_heap`
        //      - `is_heap_until`
        //      - `is_partitioned`
        //      - `is_permutation`
        //      - `is_sorted`
        //      - `is_sorted_until`
        //      - `minmax`
        //      - `minmax_element`
        //      - `move`
        //      - `move_backward`
        //      - `none_of`
        //      - `partition_copy`
        //      - `partition_point`
        //      - `shuffle`
        //
        //    - Types defined in `<atomic>`
        //      - atomic class template and specializations for integral types
        //
        //    - Functions defined in `<functional>`
        //      - `bind`
        //      - `bit_and`
        //      - `bit_or`
        //      - `bit_xor`
        //      - `cref`
        //      - `is_bind_expression`
        //      - `is_placeholder`
        //      - `mem_fn`
        //      - `ref`
        //      - `reference_wrapper`
        //
        //    - Functions defined in `<iomanip>`
        //      - `get_money`
        //      - `put_money`
        //
        //    - Functions and types defined in `<ios>`
        //      - `io_errc`
        //      - `iostream_category`
        //      - `is_error_code_enum`
        //      - `make_error_code`
        //      - `make_error_condition`
        //      - `hexfloat`
        //      - `defaultfloat`
        //
        //    - Functions defined in `<iterator>`
        //      - `begin`
        //      - `end`
        //      - `move_iterator`
        //      - `make_move_iterator`
        //      - `next`
        //      - `prev`
        //
        //    - Function defined in `<locale>`
        //      - `isblank`
        //
        //    - Function defined in `<memory>`
        //      - `addressof`;
        //      - `uninitialized_copy_n`
        //
        //    - Function defined in `<numeric>`
        //      - `iota`
        //
        //    - Member function defined in `<typeinfo>`
        //      - `type_info::hash_code`
        //
        //    - Function defined in `<utility>`
        //      - `swap`
        //
        // Plan:
        // 1. When `BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY` is defined
        //    run the `testSimpleUniformRandomNumberGenerator` function to
        //    confirm that the helper class
        //    `SimpleUniformRandomNumberGenerator` -- used in the test of the
        //    `std::shuffle` function -- works as expected.
        //
        // 2. When `BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY` is defined
        //    conditionally compile code that includes `<algorithm>`,
        //    `<atomic>`, `<functional>`, `<iomanip>`, `<iterator>`,
        //    `<locale>`, `<memory>`, `<numeric>`, `<utility>` and uses each of
        //    the listed function templates at least once.
        //
        // 3. When `BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY` is defined
        //    conditionally compile code that includes `<typeindex>` and verify
        //    the `hash_code` method is available in both `type_info` and
        //    `type_index`.
        //
        // Testing:
        //   BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY
        //   BSLS_LIBRARYFEATURES_HAS_CPP11_SHORT_STRING
        // --------------------------------------------------------------------

        if (verbose)
            puts("\n'BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY'"
                 "\n=================================================");

        if (verbose) {
            P(BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY_defined);
        }

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY
        testSimpleUniformRandomNumberGenerator();
        useCpp11Algorithms();
        useTypeIndex();

        {
            int x;
            std::reference_wrapper<int>       mX = std::ref(x);
            std::reference_wrapper<const int> X  = std::cref(x);
            (void)mX;
            (void)X;
        }
#endif

        {
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_SHORT_STRING
            // Confirm that `a` and `b` do not alias the same string.
            // If the implementation uses the CoW optimization then the
            // two strings will share the same buffer until a non-`const`
            // operation is called on either of them --- hence we declare
            // the copied string as `const` to ensure we call the `const`
            // qualified `data()` function.  Note that this test is
            // carefully crafted to avoid undefined behavior associated
            // with iterator invalidation, otherwise we might have looked
            // into two calls to `begin()` returning different values for
            // the same `string`, before and after overwriting the character
            // at index 0.
            std::string a = "short";    const std::string& A = a;
            const void *pA = A.data();

            const std::string b = a;
            const void *pB = b.data();

            ASSERTV(pA, pB, pA != pB);

            a[0] = 'S';
            const void *pA2 = A.data();

            ASSERTV(pA, pA2, pA == pA2);
#else       // Confirm the CoW string semantics instead
            std::string a = "copy on Write";    const std::string& A = a;
            const void *pA = A.data();

            const std::string b = a;
            const void *pB = b.data();

            ASSERTV(pA, pB, pA == pB);

            a[0] = 'C';
            const void *pA2 = A.data();
            const void *pB2 = b.data();

            ASSERTV(pA2, pB2, pA2 != pB2);
            ASSERTV(pA,  pA2, pA  != pA2);
            ASSERTV(pB,  pB2, pB  == pB2);
#endif
        }
        if (veryVeryVerbose) P(BSLS_PLATFORM_CMP_VERSION);
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // `BSLS_LIBRARYFEATURES_HAS_CPP98_AUTO_PTR`
        //
        // Concerns:
        // 1. The `BSLS_LIBRARYFEATURES_HAS_CPP98_AUTO_PTR` flag is defined
        //    when the native standard library defines type
        //    `std::auto_ptr` template in `<memory>`.
        //
        // 2. The `BSLS_LIBRARYFEATURES_HAS_CPP98_AUTO_PTR` macro is set on all
        //    platforms (until C++17).
        //
        // Plan:
        // 1. When `BSLS_LIBRARYFEATURES_HAS_CPP98_AUTO_PTR` is defined
        //    conditionally compile code that includes `<memory>` and
        //    constructs `std::auto_ptr` object for `int`.
        //
        // 2. Confirm the value of the conditionally compiled global variable
        //    `BSLS_LIBRARYFEATURES_HAS_CPP98_AUTO_PTR_defined` is `true`.
        //
        // Testing:
        //   BSLS_LIBRARYFEATURES_HAS_CPP98_AUTO_PTR
        // --------------------------------------------------------------------

        if (verbose) puts("\n'BSLS_LIBRARYFEATURES_HAS_CPP98_AUTO_PTR'"
                          "\n=========================================");

        if (verbose) {
            P(BSLS_LIBRARYFEATURES_HAS_CPP98_AUTO_PTR_defined);
        }

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP98_AUTO_PTR
    #ifdef BSLS_PLATFORM_HAS_PRAGMA_GCC_DIAGNOSTIC
        #pragma GCC diagnostic push
        #pragma GCC diagnostic ignored "-Wdeprecated-declarations"
    #endif
        std::auto_ptr<int> x(new int);
        *x = 42;
        std::auto_ptr<int> y = x;
        ASSERT(!x.get());
        ASSERT(42 == *y);
    #ifdef BSLS_PLATFORM_HAS_PRAGMA_GCC_DIAGNOSTIC
        #pragma GCC diagnostic pop
    #endif
#endif
        if (veryVeryVerbose) P(BSLS_PLATFORM_CMP_VERSION);
      } break;
      default: {
        fprintf(stderr, "WARNING: CASE `%d' NOT FOUND.\n", test);
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        fprintf(stderr, "Error, non-zero test status = %d.\n", testStatus);
    }
    return testStatus;
}

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
