// bsls_libraryfeatures.t.cpp                                         -*-C++-*-
#include <bsls_libraryfeatures.h>

#include <bsls_bsltestutil.h>
#include <bsls_buildtarget.h>
#include <bsls_compilerfeatures.h>
#include <bsls_keyword.h>
#include <bsls_platform.h>

#include <stddef.h>  // for 'size_t'
#include <stdio.h>   // for 'printf', 'puts'
#include <stdlib.h>  // for 'atoi'

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_INT_CHARCONV
    #include <string.h>  // for 'strchr'
#endif

// Verify assumption that the BASELINE C++11 library includes all of the new
// library headers not covered by a more specific macro.  Note that we actively
// #include each header to check for errors, though this could switch to using
// '__has_include(<header>)' now that we no longer mess with standard include
// files with intercept headers.
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY
    #include <array>
    #include <atomic>
    #include <chrono>
    #include <codecvt>
    #include <condition_variable>
    #include <forward_list>
    #include <future>
//  #include <initializer_list>
                      // BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS
    #include <mutex>
    #include <random>
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
// library headers not covered by a more specific macro.
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP14_BASELINE_LIBRARY
    #include <shared_mutex>
#endif

// Verify assumption that <cstdlib> can be included.
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_ALIGNED_ALLOC
    #include <cstdlib>
#endif

// Verify assumption that the BASELINE C++17 library includes all of the new
// library headers not covered by a more specific macro.
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

// Verify assumption that <execution> can be included.
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_PARALLEL_ALGORITHMS
    #include <execution>
#endif

// Verify assumption that <filesystem> can be included.
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_FILESYSTEM
    #include <filesystem>
#endif

// Verify assumption that <memory_resource> can be included.
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_PMR
    #include <memory_resource>
#endif

// Verify assumption that <ctime> can be included.
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_TIMESPEC_GET
    #include <ctime>
#endif

// Verify assumption that the BASELINE C++20 library includes all of the new
// library headers not covered by a more specific macro.

// Verify assumption that <concepts> can be included.
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP20_RANGES
    #include <concepts>
#endif

// Verify assumption that <ranges> can be included.
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP20_RANGES
    #include <ranges>
#endif

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP20_BASELINE_LIBRARY
    #include <barrier>
    #include <latch>
    #include <semaphore>
    #include <span>
    #include <stop_token>
#endif

// Verify assumption that <version> can be included.
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP20_VERSION
    #include <version>
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
// {'bsls_libraryfeatures'|Converse Logic}.  These tests are provided as a
// debugging aid and as an investigative tool to discover the presence or
// absence of these features in libraries that have not yet been evaluated.
//
// The compilation of code that uses a feature can be forced by defining (e.g.,
// on the command line) a macro consisting of the feature macro suffixed by
// '_FORCE'.  For example, to force a test for the presence of the '<tuple>'
// type specify '-DBSLS_LIBRARYFEATURES_HAS_CPP11_TUPLE_FORCE' on the command
// line.  The code associated with that feature will be exposed to the compiler
// even if 'BSLS_LIBRARYFEATURES_HAS_CPP11_TUPLE_FORCE' was not defined by this
// component.
//
// Here, *failure* to compile this test driver is an indication that the
// feature is indeed absent from the library being evaluated.  Should the test
// driver (unexpectedly) compile, run the test case in very very verbose mode
// to determine whether or not the macro for the feature of interest is
// defined.
//
//: o If defined, then component was correct in defining that macro (and any
//:   expectation of compiler failure was incorrect).
//:
//: o If not defined, the component could be updated to recognize the build
//:   configuration as providing the feature of interest.
//
// TBD Add tests for the new macros (and amendments to existing macros).  See
//     the macros without test-case numbers below.
// ----------------------------------------------------------------------------
// [  ] BSLS_LIBRARYFEATURES_HAS_C90_GETS
// [  ] BSLS_LIBRARYFEATURES_HAS_C99_FP_CLASSIFY
// [ 6] BSLS_LIBRARYFEATURES_HAS_C99_LIBRARY
// [ 6] BSLS_LIBRARYFEATURES_HAS_C99_SNPRINTF
// [ 1] BSLS_LIBRARYFEATURES_HAS_CPP98_AUTO_PTR
// [ 2] BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY
// [  ] BSLS_LIBRARYFEATURES_HAS_CPP11_EXCEPTION_HANDLING
// [  ] BSLS_LIBRARYFEATURES_HAS_CPP11_GARBAGE_COLLECTION_API
// [  ] BSLS_LIBRARYFEATURES_HAS_CPP11_MISCELLANEOUS_UTILITIES
// [ 3] BSLS_LIBRARYFEATURES_HAS_CPP11_PAIR_PIECEWISE_CONSTRUCTOR
// [  ] BSLS_LIBRARYFEATURES_HAS_CPP11_PROGRAM_TERMINATION
// [12] BSLS_LIBRARYFEATURES_HAS_CPP11_RANGE_FUNCTIONS
// [  ] BSLS_LIBRARYFEATURES_HAS_CPP11_STREAM_MOVE
// [ 4] BSLS_LIBRARYFEATURES_HAS_CPP11_TUPLE
// [ 5] BSLS_LIBRARYFEATURES_HAS_CPP11_UNIQUE_PTR
// [ 9] BSLS_LIBRARYFEATURES_HAS_CPP14_BASELINE_LIBRARY
// [11] BSLS_LIBRARYFEATURES_HAS_CPP14_INTEGER_SEQUENCE
// [  ] BSLS_LIBRARYFEATURES_HAS_CPP14_RANGE_FUNCTIONS
// [15] BSLS_LIBRARYFEATURES_HAS_CPP17_ALIGNED_ALLOC
// [14] BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY
// [  ] BSLS_LIBRARYFEATURES_HAS_CPP17_BOOL_CONSTANT
// [16] BSLS_LIBRARYFEATURES_HAS_CPP17_RANGE_FUNCTIONS
// [ 8] BSLS_LIBRARYFEATURES_HAS_CPP17_PRECISE_BITWIDTH_ATOMICS
// [13] BSLS_LIBRARYFEATURES_HAS_CPP17_SEARCH_OVERLOAD
// [13] BSLS_LIBRARYFEATURES_HAS_CPP17_SEARCH_FUNCTORS
// [15] BSLS_LIBRARYFEATURES_HAS_CPP17_TIMESPEC_GET
// [18] BSLS_LIBRARYFEATURES_HAS_CPP17_INT_CHARCONV
// [18] BSLS_LIBRARYFEATURES_HAS_CPP17_CHARCONV
// [17] BSLS_LIBRARYFEATURES_HAS_CPP20_BASELINE_LIBRARY
// [19] BSLS_LIBRARYFEATURES_HAS_CPP20_VERSION
// [19] BSLS_LIBRARYFEATURES_HAS_CPP20_CONCEPTS
// [19] BSLS_LIBRARYFEATURES_HAS_CPP20_RANGES
// [19] BSLS_LIBRARYFEATURES_HAS_CPP20_ATOMIC_LOCK_FREE_TYPE_ALIASES
// [19] BSLS_LIBRARYFEATURES_HAS_CPP20_ATOMIC_WAIT_FREE_FUNCTIONS
// [19] BSLS_LIBRARYFEATURES_HAS_CPP20_ATOMIC_FLAG_TEST_FREE_FUNCTIONS
// [19] BSLS_LIBRARYFEATURES_HAS_CPP20_MAKE_UNIQUE_FOR_OVERWRITE
// [19] BSLS_LIBRARYFEATURES_HAS_CPP20_CALENDAR
// [19] BSLS_LIBRARYFEATURES_HAS_CPP20_CHAR8_MB_CONV
// [10] BSLS_LIBRARYFEATURES_STDCPP_GNU
// [10] BSLS_LIBRARYFEATURES_STDCPP_IBM
// [  ] BSLS_LIBRARYFEATURES_STDCPP_INTELLISENSE
// [10] BSLS_LIBRARYFEATURES_STDCPP_LLVM
// [10] BSLS_LIBRARYFEATURES_STDCPP_MSVC
// [10] BSLS_LIBRARYFEATURES_STDCPP_LIBCSTD
// [10] BSLS_LIBRARYFEATURES_STDCPP_STLPORT
// [ 7] int std::isblank(int);
// [ 7] bool std::isblank(char, const std::locale&);
// ----------------------------------------------------------------------------
// [20] USAGE EXAMPLE
// [-1] BSLS_LIBRARYFEATURES_HAS_CPP17_BOOL_CONSTANT: obsolescent: not defined
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
bool   BSLS_COMPILERFEATURES_SUPPORT_VARIADIC_TEMPLATES_defined =
#ifdef BSLS_COMPILERFEATURES_SUPPORT_VARIADIC_TEMPLATES
                                                                          true;
#else
                                                                         false;
#endif

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
bool   BSLS_LIBRARYFEATURES_HAS_CPP17_BOOL_CONSTANT_defined =
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_BOOL_CONSTANT
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
bool   BSLS_LIBRARYFEATURES_HAS_CPP17_SEARCH_OVERLOAD_defined =
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_SEARCH_OVERLOAD
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
bool   BSLS_LIBRARYFEATURES_HAS_CPP17_RANGE_FUNCTIONS_defined =
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_RANGE_FUNCTIONS
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
bool   BSLS_LIBRARYFEATURES_HAS_CPP17_INT_CHARCONV_defined =
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_INT_CHARCONV
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
bool   BSLS_LIBRARYFEATURES_HAS_CPP20_BASELINE_LIBRARY_defined =
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP20_BASELINE_LIBRARY
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
bool   BSLS_LIBRARYFEATURES_HAS_CPP20_CONCEPTS_defined =
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP20_CONCEPTS
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
bool   BSLS_LIBRARYFEATURES_HAS_CPP20_ATOMIC_LOCK_FREE_TYPE_ALIASES_defined =
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP20_ATOMIC_LOCK_FREE_TYPE_ALIASES
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
bool   BSLS_LIBRARYFEATURES_HAS_CPP20_ATOMIC_FLAG_TEST_FREE_FUNCTIONS_defined =
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP20_ATOMIC_FLAG_TEST_FREE_FUNCTIONS
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

                        // case 19

#include <vector> // for 'ranges'
namespace case19 {
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP20_CONCEPTS
template <class TYPE>
requires std::equality_comparable<TYPE>
constexpr bool equal(const TYPE& lhs, const TYPE& rhs)
    // Return 'true' if the specified 'lhs' and 'rhs' are equal.  This function
    // is used to conform concepts availability.
{
    return lhs == rhs;
}
#endif

}  // close namespace case19

                    // case 13

#include <algorithm> // for 'search'
#include <utility>   // for 'pair'

namespace case13 {

struct SearcherNull {

    std::pair<const char *, const char*> operator()(const char* first,
                                                    const char* last) const;
        // Return 'std::pair<last, last>' ("needle not found")
        // irrespective of the contents of '[first, last)'.  Note that the
        // (default) constructor does not allow the specification of a
        // "needle".
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
    // of namespace 'std' when the feature macro is NOT defined, as this will
    // cause failures when BDE code provides its own alternative.

struct TestType {
    typedef int * iterator;

    int d_data;

    int *begin() { return &d_data; }
    int *end() { return &d_data; }
};

template <class CONTAINER>
typename CONTAINER::iterator begin(CONTAINER & c)
    // Return 'begin()' for the specified 'c'.
{
    return c.begin();
}

template <class CONTAINER>
typename CONTAINER::iterator end(CONTAINER & c)
    // Return 'end()' for the specified 'c'.
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

    static void useCpp14Algorithms()
        // Use each of the function templates associated with the
        // 'BSLS_LIBRARYFEATURES_HAS_CPP14_BASELINE_LIBRARY' flag in a
        // syntactically correct (and semantically meaningless) manner as a
        // compile-time test that these templates are available.
    {
        // Type defined in '<functional>'
        ASSERT(0x0F == std::bit_not<unsigned char>().operator()(0xF0));

        // Function defined in '<iterator>'
        std::vector<int> v;
        (void)std::make_reverse_iterator(v.end());

        // Function defined in '<iomanip>'
        (void)std::quoted("\"quotes\"");

        // Function defined in '<utility>'
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

        { // Function defined in '<memory>'
            std::unique_ptr<int> up = std::make_unique<int>(0);
        }

        { // Functions defined in '<type_traits>'
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

    #include <atomic>

    static void useCpp11PreciseBitwidthAtomics()
        // Declare variables with each of the 'typedef's associated with the
        // 'BSLS_LIBRARYFEATURES_HAS_CPP17_PRECISE_BITWIDTH_ATOMICS' flag as a
        // compile-time test that these 'typedef's are available.
    {
        ASSERT(0 < sizeof(std::atomic_int8_t));
        ASSERT(0 < sizeof(std::atomic_int16_t));
        ASSERT(0 < sizeof(std::atomic_int32_t));
        ASSERT(0 < sizeof(std::atomic_int64_t));
        ASSERT(0 < sizeof(std::atomic_uint8_t));
        ASSERT(0 < sizeof(std::atomic_uint16_t));
        ASSERT(0 < sizeof(std::atomic_uint32_t));
        ASSERT(0 < sizeof(std::atomic_uint64_t));
        ASSERT(0 < sizeof(std::atomic_intptr_t));
        ASSERT(0 < sizeof(std::atomic_uintptr_t));
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
    #include <utility> // for 'pair' and 'piecewise_construct'

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

    class SimpleUniformRandomNumberGenerator {
        // This class defines a simple, easily tested uniform random number
        // generator that can be validly used as the third argument to the
        // 'std::shuffle' function in the 'useCpp11Algorithms()' test
        // function.

      public:
        // TYPES
        typedef unsigned result_type;
            // Alias for the type of the values returned by this class.

        // CLASS METHODS
        static BSLS_KEYWORD_CONSTEXPR unsigned min();
            // Return 0, the smallest value returned by 'operator()'.

        static BSLS_KEYWORD_CONSTEXPR unsigned max();
            // Return the largest value *potentially* returned by 'operator()'.
            // The C++ standard requires that this returned value be greater
            // than the value returned by the 'min' method; consequentally,
            // this method returns 1 even though 'operator()(void)' always
            // returns 0.

        // CREATORS
        //! SimpleUniformRandomNumberGenerator() = default;
            // Create an 'SimpleUniformRandomNumberGenerator' object.

        //! ~SimpleUniformRandomNumberGenerator() = default;
            // Destroy this 'SimpleUniformRandomNumberGenerator' object.

        // ACCESSORS
        unsigned operator()(void) const;
            // Return the next value from this 'SimpleUniformNumberGenerator'
            // object.  For this generator, that value is always 0.
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
    #include <memory>         // for 'uninitialized_copy_n'
    #include <numeric>        // for 'iota'
    #include <sstream>

    static bool unaryPredicate(int)
        // Return 'true' irrespective of the (ignored) input argument.  Used to
        // instantiate function templates in the 'useCpp11Algorithms' test
        // function.
    {
        return true;
    }

    static void useCpp11Algorithms()
        // Use each of the function templates associated with the
        // 'BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY' flag in a
        // syntactically correct (and semantically meaningless) manner as a
        // compile-time test that these templates are available.
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
// When building software across multiple platforms may have to deal with
// different versions of the native standard library, some providing features
// that the others do not.  The macros defined in this component can be used
// make features visible only if the required native standard library features
// are present.
//
// For example, the 'tuple'-type is not available in older versions of the
// native standard library.  Suppose we have a utility component that returns
// an instance of a 'tuple' of values *if* the underlying version of the
// standard library provides that type, and yet remain compilable otherwise.
//
// First, we conditionally include the header file we will need if we define an
// interface that returns a 'std::tuple'.
//..
    #if defined(BSLS_LIBRARYFEATURES_HAS_CPP11_TUPLE)
    # ifndef INCLUDED_TUPLE
    # include <tuple>
    # define INCLUDED_TUPLE
    # endif
    #endif // BSLS_LIBRARYFEATURES_HAS_CPP11_TUPLE
//..
// Then, we declare the methods that will be unconditionally provided by our
// utility component:
//..
    struct MyStatisticalUtil
    {
        static double     mean(const int *begin, const int *end);
        static int      median(const int *begin, const int *end);
        static double variance(const int *begin, const int *end);
            // Return the median (mean, variance) of the sequence of values in
            // the specified non-empty, semi-open range '[begin, end)'.  The
            // behavior is undefined unless 'begin < end'.
//..
// Now, we conditionally define an interface that returns a 'bsl::type', if
// that type is available.  Note that, if all three values are needed, calling
// this interface is more efficient than calling the earlier three individually
// because the input need be traversed one time, not three.
//..
    #ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_TUPLE
        static std::tuple<int, double, double> getMedianMeanVariance(
                                                              const int *begin,
                                                              const int *end);
            // Return the median, mean, and variance (in that order) of the
            // sequence of values in the specified non-empty, semi-open range
            // '[begin, end)'.  The behavior is undefined unless 'begin < end'.

    #endif // BSLS_LIBRARYFEATURES_HAS_CPP11_TUPLE

    };
//..
// Finally, we find that our code compiles when we build our code against
// versions of the native standard library that provide a 'tuple' type, *and*
// those that do not.  Of course, in the later case the interface that returns
// a 'tuple' is not defined for the 'MyStatisticalUtil' 'struct'.

// ============================================================================
//                              HELPER FUNCTIONS
// ----------------------------------------------------------------------------

static void printFlags()
    // Print a diagnostic message to standard output if any of the preprocessor
    // flags of interest are defined, and their value if a value had been set.
    // An "Enter" and "Leave" message is printed unconditionally so there is
    // some report even if all of the flags are undefined.
{
    printf("printFlags: Enter\n");

    printf("\n  BSLS_PLATFORM_CMP_VERSION: %s\n",
                                         STRINGIFY(BSLS_PLATFORM_CMP_VERSION));
    printf("\n  __cplusplus: %s\n", STRINGIFY(__cplusplus));

    printf("\n  printFlags: bsls_libraryfeatures Macros\n");

    printf("\n  BSLS_LIBRARYFEATURES_DETECTION_IN_PROGRESS: ");
#ifdef BSLS_LIBRARYFEATURES_DETECTION_IN_PROGRESS
    printf("%s\n", STRINGIFY(BSLS_LIBRARYFEATURES_DETECTION_IN_PROGRESS) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_LIBRARYFEATURES_HAS_C90_GETS: ");
#ifdef BSLS_LIBRARYFEATURES_HAS_C90_GETS
    printf("%s\n", STRINGIFY(BSLS_LIBRARYFEATURES_HAS_C90_GETS) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_LIBRARYFEATURES_HAS_C99_FP_CLASSIFY: ");
#ifdef BSLS_LIBRARYFEATURES_HAS_C99_FP_CLASSIFY
    printf("%s\n", STRINGIFY(BSLS_LIBRARYFEATURES_HAS_C99_FP_CLASSIFY) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_LIBRARYFEATURES_HAS_C99_LIBRARY: ");
#ifdef BSLS_LIBRARYFEATURES_HAS_C99_LIBRARY
    printf("%s\n", STRINGIFY(BSLS_LIBRARYFEATURES_HAS_C99_LIBRARY) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_LIBRARYFEATURES_HAS_C99_SNPRINTF: ");
#ifdef BSLS_LIBRARYFEATURES_HAS_C99_SNPRINTF
    printf("%s\n", STRINGIFY(BSLS_LIBRARYFEATURES_HAS_C99_SNPRINTF) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY: ");
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY
    printf("%s\n",
                  STRINGIFY(BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_LIBRARYFEATURES_HAS_CPP11_EXCEPTION_HANDLING: ");
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_EXCEPTION_HANDLING
    printf("%s\n",
                STRINGIFY(BSLS_LIBRARYFEATURES_HAS_CPP11_EXCEPTION_HANDLING) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_LIBRARYFEATURES_HAS_CPP11_GARBAGE_COLLECTION_API: ");
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_GARBAGE_COLLECTION_API
    printf("%s\n",
            STRINGIFY(BSLS_LIBRARYFEATURES_HAS_CPP11_GARBAGE_COLLECTION_API) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_LIBRARYFEATURES_HAS_CPP11_MISCELLANEOUS_UTILITIES: ");
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_MISCELLANEOUS_UTILITIES
    printf("%s\n",
           STRINGIFY(BSLS_LIBRARYFEATURES_HAS_CPP11_MISCELLANEOUS_UTILITIES) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_LIBRARYFEATURES_HAS_CPP11_PAIR_PIECEWISE_CONSTRUCTOR: ");
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_PAIR_PIECEWISE_CONSTRUCTOR
    printf("%s\n",
        STRINGIFY(BSLS_LIBRARYFEATURES_HAS_CPP11_PAIR_PIECEWISE_CONSTRUCTOR) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_LIBRARYFEATURES_HAS_CPP11_PROGRAM_TERMINATION: ");
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_PROGRAM_TERMINATION
    printf("%s\n",
               STRINGIFY(BSLS_LIBRARYFEATURES_HAS_CPP11_PROGRAM_TERMINATION) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_LIBRARYFEATURES_HAS_CPP11_RANGE_FUNCTIONS: ");
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_RANGE_FUNCTIONS
    printf("%s\n",
                   STRINGIFY(BSLS_LIBRARYFEATURES_HAS_CPP11_RANGE_FUNCTIONS) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_LIBRARYFEATURES_HAS_CPP11_STREAM_MOVE: ");
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_STREAM_MOVE
    printf("%s\n", STRINGIFY(BSLS_LIBRARYFEATURES_HAS_CPP11_STREAM_MOVE) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_LIBRARYFEATURES_HAS_CPP11_TUPLE: ");
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_TUPLE
    printf("%s\n", STRINGIFY(BSLS_LIBRARYFEATURES_HAS_CPP11_TUPLE) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_LIBRARYFEATURES_HAS_CPP11_UNIQUE_PTR: ");
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_UNIQUE_PTR
    printf("%s\n", STRINGIFY(BSLS_LIBRARYFEATURES_HAS_CPP11_UNIQUE_PTR) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_LIBRARYFEATURES_HAS_CPP14_BASELINE_LIBRARY: ");
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP14_BASELINE_LIBRARY
    printf("%s\n",
                  STRINGIFY(BSLS_LIBRARYFEATURES_HAS_CPP14_BASELINE_LIBRARY) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_LIBRARYFEATURES_HAS_CPP14_INTEGER_SEQUENCE: ");
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP14_INTEGER_SEQUENCE
    printf("%s\n",
                  STRINGIFY(BSLS_LIBRARYFEATURES_HAS_CPP14_INTEGER_SEQUENCE) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_LIBRARYFEATURES_HAS_CPP14_RANGE_FUNCTIONS: ");
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP14_RANGE_FUNCTIONS
    printf("%s\n",
                   STRINGIFY(BSLS_LIBRARYFEATURES_HAS_CPP14_RANGE_FUNCTIONS) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY: ");
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY
    printf("%s\n",
                  STRINGIFY(BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_LIBRARYFEATURES_HAS_CPP17_PRECISE_BITWIDTH_ATOMICS: ");
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_PRECISE_BITWIDTH_ATOMICS
    printf("%s\n",
          STRINGIFY(BSLS_LIBRARYFEATURES_HAS_CPP17_PRECISE_BITWIDTH_ATOMICS) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_LIBRARYFEATURES_HAS_CPP17_RANGE_FUNCTIONS: ");
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_RANGE_FUNCTIONS
    printf("%s\n",
                   STRINGIFY(BSLS_LIBRARYFEATURES_HAS_CPP17_RANGE_FUNCTIONS) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_LIBRARYFEATURES_HAS_CPP98_AUTO_PTR: ");
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP98_AUTO_PTR
    printf("%s\n", STRINGIFY(BSLS_LIBRARYFEATURES_HAS_CPP98_AUTO_PTR) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_LIBRARYFEATURES_STDCPP_GNU: ");
#ifdef BSLS_LIBRARYFEATURES_STDCPP_GNU
    printf("%s\n", STRINGIFY(BSLS_LIBRARYFEATURES_STDCPP_GNU) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_LIBRARYFEATURES_STDCPP_IBM: ");
#ifdef BSLS_LIBRARYFEATURES_STDCPP_IBM
    printf("%s\n", STRINGIFY(BSLS_LIBRARYFEATURES_STDCPP_IBM) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_LIBRARYFEATURES_STDCPP_INTELLISENSE: ");
#ifdef BSLS_LIBRARYFEATURES_STDCPP_INTELLISENSE
    printf("%s\n", STRINGIFY(BSLS_LIBRARYFEATURES_STDCPP_INTELLISENSE) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_LIBRARYFEATURES_STDCPP_LIBCSTD: ");
#ifdef BSLS_LIBRARYFEATURES_STDCPP_LIBCSTD
    printf("%s\n", STRINGIFY(BSLS_LIBRARYFEATURES_STDCPP_LIBCSTD) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_LIBRARYFEATURES_STDCPP_LLVM: ");
#ifdef BSLS_LIBRARYFEATURES_STDCPP_LLVM
    printf("%s\n", STRINGIFY(BSLS_LIBRARYFEATURES_STDCPP_LLVM) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_LIBRARYFEATURES_STDCPP_MSVC: ");
#ifdef BSLS_LIBRARYFEATURES_STDCPP_MSVC
    printf("%s\n", STRINGIFY(BSLS_LIBRARYFEATURES_STDCPP_MSVC) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_LIBRARYFEATURES_STDCPP_STLPORT: ");
#ifdef BSLS_LIBRARYFEATURES_STDCPP_STLPORT
    printf("%s\n", STRINGIFY(BSLS_LIBRARYFEATURES_STDCPP_STLPORT) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_LIBRARYFEATURES_HAS_CPP17_INT_CHARCONV: ");
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_INT_CHARCONV
    printf("%s\n", STRINGIFY(BSLS_LIBRARYFEATURES_HAS_CPP17_INT_CHARCONV) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_LIBRARYFEATURES_HAS_CPP17_CHARCONV: ");
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_CHARCONV
    printf("%s\n", STRINGIFY(BSLS_LIBRARYFEATURES_HAS_CPP17_CHARCONV) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_LIBRARYFEATURES_HAS_CPP17_PARALLEL_ALGORITHMS: ");
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_PARALLEL_ALGORITHMS
    printf("%s\n",
           STRINGIFY(BSLS_LIBRARYFEATURES_HAS_CPP17_PARALLEL_ALGORITHMS) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_LIBRARYFEATURES_HAS_CPP17_FILESYSTEM: ");
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_FILESYSTEM
    printf("%s\n", STRINGIFY(BSLS_LIBRARYFEATURES_HAS_CPP17_FILESYSTEM) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_LIBRARYFEATURES_HAS_CPP17_PMR: ");
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_PMR
    printf("%s\n", STRINGIFY(BSLS_LIBRARYFEATURES_HAS_CPP17_PMR) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_LIBRARYFEATURES_HAS_CPP20_BASELINE_LIBRARY: ");
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP20_BASELINE_LIBRARY
    printf("%s\n",
                  STRINGIFY(BSLS_LIBRARYFEATURES_HAS_CPP20_BASELINE_LIBRARY) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_LIBRARYFEATURES_HAS_CPP20_VERSION: ");
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP20_VERSION
    printf("%s\n", STRINGIFY(BSLS_LIBRARYFEATURES_HAS_CPP20_VERSION) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_LIBRARYFEATURES_HAS_CPP20_CONCEPTS: ");
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP20_CONCEPTS
    printf("%s\n", STRINGIFY(BSLS_LIBRARYFEATURES_HAS_CPP20_CONCEPTS) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_LIBRARYFEATURES_HAS_CPP20_RANGES: ");
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP20_RANGES
    printf("%s\n", STRINGIFY(BSLS_LIBRARYFEATURES_HAS_CPP20_RANGES) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_LIBRARYFEATURES_SUSPECT_CLANG_WITH_GLIBCPP: ");
#ifdef BSLS_LIBRARYFEATURES_SUSPECT_CLANG_WITH_GLIBCPP
    printf("%s\n",
                  STRINGIFY(BSLS_LIBRARYFEATURES_SUSPECT_CLANG_WITH_GLIBCPP) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n\n  printFlags: bsls_libraryfeatures Referenced Macros\n");

    printf("\n  BSLS_COMPILERFEATURES_SUPPORT_ALIAS_TEMPLATES: ");
#ifdef BSLS_COMPILERFEATURES_SUPPORT_ALIAS_TEMPLATES
    printf("%s\n", STRINGIFY(BSLS_COMPILERFEATURES_SUPPORT_ALIAS_TEMPLATES) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_COMPILERFEATURES_SUPPORT_HAS_INCLUDE: ");
#ifdef BSLS_COMPILERFEATURES_SUPPORT_HAS_INCLUDE
    printf("%s\n", STRINGIFY(BSLS_COMPILERFEATURES_SUPPORT_HAS_INCLUDE) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_COMPILERFEATURES_SUPPORT_VARIADIC_TEMPLATES: ");
#ifdef BSLS_COMPILERFEATURES_SUPPORT_VARIADIC_TEMPLATES
    printf("%s\n",
                 STRINGIFY(BSLS_COMPILERFEATURES_SUPPORT_VARIADIC_TEMPLATES) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY: ");
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY
    printf("%s\n",
                  STRINGIFY(BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_LIBRARYFEATURES_HAS_CPP11_TUPLE: ");
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_TUPLE
    printf("%s\n", STRINGIFY(BSLS_LIBRARYFEATURES_HAS_CPP11_TUPLE) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_LIBRARYFEATURES_STDCPP_STLPORT: ");
#ifdef BSLS_LIBRARYFEATURES_STDCPP_STLPORT
    printf("%s\n", STRINGIFY(BSLS_LIBRARYFEATURES_STDCPP_STLPORT) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_LIBRARYFEATURES_SUSPECT_CLANG_WITH_GLIBCPP: ");
#ifdef BSLS_LIBRARYFEATURES_SUSPECT_CLANG_WITH_GLIBCPP
    printf("%s\n",
                  STRINGIFY(BSLS_LIBRARYFEATURES_SUSPECT_CLANG_WITH_GLIBCPP) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_PLATFORM_CMP_CLANG: ");
#ifdef BSLS_PLATFORM_CMP_CLANG
    printf("%s\n", STRINGIFY(BSLS_PLATFORM_CMP_CLANG) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_PLATFORM_CMP_GNU: ");
#ifdef BSLS_PLATFORM_CMP_GNU
    printf("%s\n", STRINGIFY(BSLS_PLATFORM_CMP_GNU) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_PLATFORM_CMP_IBM: ");
#ifdef BSLS_PLATFORM_CMP_IBM
    printf("%s\n", STRINGIFY(BSLS_PLATFORM_CMP_IBM) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_PLATFORM_CMP_MSVC: ");
#ifdef BSLS_PLATFORM_CMP_MSVC
    printf("%s\n", STRINGIFY(BSLS_PLATFORM_CMP_MSVC) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_PLATFORM_CMP_SUN: ");
#ifdef BSLS_PLATFORM_CMP_SUN
    printf("%s\n", STRINGIFY(BSLS_PLATFORM_CMP_SUN) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  _CPPLIB_VER: ");
#ifdef _CPPLIB_VER
    printf("%s\n", STRINGIFY(_CPPLIB_VER) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  _GLIBCXX_HAVE_AT_QUICK_EXIT: ");
#ifdef _GLIBCXX_HAVE_AT_QUICK_EXIT
    printf("%s\n", STRINGIFY(_GLIBCXX_HAVE_AT_QUICK_EXIT) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  _GLIBCXX_HAVE_QUICK_EXIT: ");
#ifdef _GLIBCXX_HAVE_QUICK_EXIT
    printf("%s\n", STRINGIFY(_GLIBCXX_HAVE_QUICK_EXIT) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  _LIBCPP_VERSION: ");
#ifdef _LIBCPP_VERSION
    printf("%s\n", STRINGIFY(_LIBCPP_VERSION) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  _RWSTD_VER: ");
#ifdef _RWSTD_VER
    printf("%s\n", STRINGIFY(_RWSTD_VER) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  _YVALS: ");
#ifdef _YVALS
    printf("%s\n", STRINGIFY(_YVALS) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  __APPLE_CC__: ");
#ifdef __APPLE_CC__
    printf("%s\n", STRINGIFY(__APPLE_CC__) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  __GLIBCPP__: ");
#ifdef __GLIBCPP__
    printf("%s\n", STRINGIFY(__GLIBCPP__) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  __GLIBCXX__: ");
#ifdef __GLIBCXX__
    printf("%s\n", STRINGIFY(__GLIBCXX__) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  __GXX_EXPERIMENTAL_CXX0X__: ");
#ifdef __GXX_EXPERIMENTAL_CXX0X__
    printf("%s\n", STRINGIFY(__GXX_EXPERIMENTAL_CXX0X__) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  __IBMCPP__: ");
#ifdef __IBMCPP__
    printf("%s\n", STRINGIFY(__IBMCPP__) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  __INTELLISENSE__: ");
#ifdef __INTELLISENSE__
    printf("%s\n", STRINGIFY(__INTELLISENSE__) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  __SGI_STL_PORT: ");
#ifdef __SGI_STL_PORT
    printf("%s\n", STRINGIFY(__SGI_STL_PORT) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  __STD_RWCOMPILER_H__: ");
#ifdef __STD_RWCOMPILER_H__
    printf("%s\n", STRINGIFY(__STD_RWCOMPILER_H__) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  __STLPORT_VERSION: ");
#ifdef __STLPORT_VERSION
    printf("%s\n", STRINGIFY(__STLPORT_VERSION) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  __cpp_lib_atomic_is_always_lock_free: ");
#ifdef __cpp_lib_atomic_is_always_lock_free
    printf("%s\n", STRINGIFY(__cpp_lib_atomic_is_always_lock_free) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n\nprintFlags: Leave\n");
}

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
      case 20: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //   Extracted from component header file.
        //
        // Concerns:
        //: 1 The usage example provided in the component header file compiles,
        //:   links, and runs as shown.
        //
        // Plan:
        //: 1 Incorporate usage example from header into test driver, remove
        //:   leading comment characters, and replace 'assert' with 'ASSERT'.
        //:   (C-1)
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) puts("\nUSAGE EXAMPLE"
                          "\n=============");
      } break;
      case 19: {
        // --------------------------------------------------------------------
        // 'BSLS_LIBRARYFEATURES_HAS_CPP20_*' MISCELLANY
        //
        // Concerns:
        //: 1 'BSLS_LIBRARYFEATURES_HAS_CPP20_VERSION' is defined only when the
        //:   native standard library provides it.
        //:
        //: 2 'BSLS_LIBRARYFEATURES_HAS_CPP20_CONCEPTS' is defined only when
        //:   the native standard library provides it.
        //:
        //: 3 'BSLS_LIBRARYFEATURES_HAS_CPP20_RANGES' is defined only when the
        //:   native standard library provides it.
        //:
        //: 4 'BSLS_LIBRARYFEATURES_HAS_CPP20_ATOMIC_LOCK_FREE_TYPE_ALIASES' is
        //:   defined only when the native standard library provides it.
        //:
        //: 5 'BSLS_LIBRARYFEATURES_HAS_CPP20_ATOMIC_WAIT_FREE_FUNCTIONS' is
        //:   defined only when the native standard library provides it.
        //:
        //: 6 'BSLS_LIBRARYFEATURES_HAS_CPP20_ATOMIC_FLAG_TEST_FREE_FUNCTIONS'
        //:   is defined only when the native standard library provides it.
        //:
        //: 7 'BSLS_LIBRARYFEATURES_HAS_CPP20_MAKE_UNIQUE_FOR_OVERWRITE' is
        //:   defined only when the native standard library provides it.
        //:
        //: 8 'BSLS_LIBRARYFEATURES_HAS_CPP20_CALENDAR' is defined only when
        //:   the native standard library provides it.
        //:
        //: 9 'BSLS_LIBRARYFEATURES_HAS_CPP20_CHAR8_MB_CONV' is defined only
        //:   when the native standard library provides it.
        //
        // Plan:
        //: 1 When these macros are defined include the appropriate headers and
        //:   use the expected calls.
        //
        // Testing:
        //   BSLS_LIBRARYFEATURES_HAS_CPP20_VERSION
        //   BSLS_LIBRARYFEATURES_HAS_CPP20_CONCEPTS
        //   BSLS_LIBRARYFEATURES_HAS_CPP20_RANGES
        //   BSLS_LIBRARYFEATURES_HAS_CPP20_ATOMIC_LOCK_FREE_TYPE_ALIASES
        //   BSLS_LIBRARYFEATURES_HAS_CPP20_ATOMIC_WAIT_FREE_FUNCTIONS
        //   BSLS_LIBRARYFEATURES_HAS_CPP20_ATOMIC_FLAG_TEST_FREE_FUNCTIONS
        //   BSLS_LIBRARYFEATURES_HAS_CPP20_MAKE_UNIQUE_FOR_OVERWRITE
        //   BSLS_LIBRARYFEATURES_HAS_CPP20_CALENDAR
        //   BSLS_LIBRARYFEATURES_HAS_CPP20_CHAR8_MB_CONV
        // --------------------------------------------------------------------

        if (verbose) puts("\n'BSLS_LIBRARYFEATURES_HAS_CPP20_*' MISCELLANY"
                          "\n=============================================");

        if (verbose) {
#define PMD(macro_name) P(macro_name##_defined)
    // Print Macro Defined

            PMD(BSLS_LIBRARYFEATURES_HAS_CPP20_VERSION);
            PMD(BSLS_LIBRARYFEATURES_HAS_CPP20_CONCEPTS);
            PMD(BSLS_LIBRARYFEATURES_HAS_CPP20_RANGES);
            PMD(BSLS_LIBRARYFEATURES_HAS_CPP20_ATOMIC_LOCK_FREE_TYPE_ALIASES);
            PMD(BSLS_LIBRARYFEATURES_HAS_CPP20_ATOMIC_WAIT_FREE_FUNCTIONS);
           PMD(BSLS_LIBRARYFEATURES_HAS_CPP20_ATOMIC_FLAG_TEST_FREE_FUNCTIONS);
            PMD(BSLS_LIBRARYFEATURES_HAS_CPP20_MAKE_UNIQUE_FOR_OVERWRITE);
            PMD(BSLS_LIBRARYFEATURES_HAS_CPP20_CALENDAR);
            PMD(BSLS_LIBRARYFEATURES_HAS_CPP20_CHAR8_MB_CONV);
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
            std::ranges::owning_view ov(std::move(v));
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

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP20_CHAR8_MB_CONV
        (void)[](char8_t *out, std::mbstate_t *st) {
            (void)std::mbrtoc8(out, "", 0U, st);
        };
        (void)[](char *out, std::mbstate_t *st) {
            (void)std::c8rtomb(out, {}, st);
        };
#endif
      } break;
      case 18: {
        // --------------------------------------------------------------------
        // CHARCONV RELATED MACROS
        //
        // Concerns:
        //: 1 If 'BSLS_LIBRARYFEATURES_HAS_CPP17_CHARCONV' is defined
        //:   'BSLS_LIBRARYFEATURES_HAS_CPP17_INT_CHARCONV' is also defined as
        //:   they represent levels of support.
        //:
        //: 3 When 'BSLS_LIBRARYFEATURES_HAS_CPP17_INT_CHARCONV' is defined
        //:   both 'std::to_chars' and 'std::from_chars' overloads exist for
        //:   all signed and unsigned standard integer types and 'char', as
        //:   well as the 'std::to_chars_result' and 'std::from_chars_result'
        //:   types are defined.
        //:
        //: 4 When 'BSLS_LIBRARYFEATURES_HAS_CPP17_CHARCONV' is defined the
        //:   'std::chars_format' type, as well as 'std::from_chars' and
        //:   'std::to_chars' overloads exists for all 3 standard floating
        //:   point types ('float', 'double', 'long double').
        //
        // Plan:
        //: 1 Macro dependency tests (C-1) use the 'u_*_defined' variables.
        //: 2 Presence of functions is tested by defining calling lambdas.
        //: 3 Presence of result types is tested by declarations.
        //: 4 'std::chars_format' is used as argument in the float lambdas.
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
        // 'std::to_chars' for singed/unsigned integers plus 'char'
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

        // 'std::from_chars' for singed/unsigned integers plus 'char'
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
        // 'std::to_chars' - for 'float', 'double', and 'long double'
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

        // 'std::from_chars' - for 'float', 'double', and 'long double'
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
        // 'BSLS_LIBRARYFEATURES_HAS_CPP20_BASELINE_LIBRARY'
        //
        // Concerns:
        //: 1 'BSLS_LIBRARYFEATURES_HAS_CPP20_BASELINE_LIBRARY' is defined only
        //:   when the native standard library provides a baseline of C++20
        //:   library features, including:
        //:       o 'span'
        //:       o 'barrier'
        //:       o 'latch'
        //:       o 'countingSemaphore'
        //:       o 'nostopstate'
        //:       o 'to_array'
        //
        // Plan:
        //: 1 When 'BSLS_LIBRARYFEATURES_HAS_CPP20_BASELINE_LIBRARY' is
        //:   defined include the appropriate headers and use the expected
        //:   typenames.
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
            (void)std::nostopstate;
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
#endif
        if (veryVeryVerbose) P(BSLS_PLATFORM_CMP_VERSION);
      } break;
      case 16: {
        // --------------------------------------------------------------------
        // 'BSLS_LIBRARYFEATURES_HAS_CPP17_RANGE_FUNCTIONS'
        //
        // Concerns:
        //: 1 'BSLS_LIBRARYFEATURES_HAS_CPP17_RANGE_FUNCTIONS' is defined only
        //:   when the native standard library provides a baseline of C++17
        //:   library features (empty, data, size).
        //
        // Plan:
        //: 1 When 'BSLS_LIBRARYFEATURES_HAS_CPP17_RANGE_FUNCTIONS' is
        //:   defined include the appropriate headers and use the expected
        //:   typenames.
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
        // 'BSLS_LIBRARYFEATURES_HAS_CPP17_*' MISCELLANY
        //
        // Concerns:
        //: 1 'BSLS_LIBRARYFEATURES_HAS_CPP17_ALIGNED_ALLOC' and
        //:   'BSLS_LIBRARYFEATURES_HAS_CPP17_TIMESPEC_GET' are defined only
        //:   when the native standard library provides them.
        //
        // Plan:
        //: 1 When 'BSLS_LIBRARYFEATURES_HAS_CPP17_ALIGNED_ALLOC' and/or
        //:   'BSLS_LIBRARYFEATURES_HAS_CPP17_TIMESPEC_GET' are defined
        //:   include the appropriate headers and use the expected calls.
        //
        // Testing:
        //   BSLS_LIBRARYFEATURES_HAS_CPP17_ALIGNED_ALLOC
        //   BSLS_LIBRARYFEATURES_HAS_CPP17_TIMESPEC_GET
        // --------------------------------------------------------------------

        if (verbose) puts("\n'BSLS_LIBRARYFEATURES_HAS_CPP17_*' MISCELLANY"
                          "\n=============================================");

        if (verbose) {
            P(BSLS_LIBRARYFEATURES_HAS_CPP17_ALIGNED_ALLOC_defined);
            P(BSLS_LIBRARYFEATURES_HAS_CPP17_TIMESPEC_GET_defined);
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
      } break;
      case 14: {
        // --------------------------------------------------------------------
        // 'BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY'
        //
        // Concerns:
        //: 1 'BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY' is defined only
        //:   when the native standard library provides a baseline of C++17
        //:   library features (any, optional, variant, string_view).
        //
        // Plan:
        //: 1 When 'BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY' is
        //:   defined include the appropriate headers and use the expected
        //:   typenames.
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
        // 'BSLS_LIBRARYFEATURES_HAS_CPP17_SEARCH_*' MACROS
        //
        // Concerns:
        //: 1 The 'BSLS_LIBRARYFEATURES_HAS_CPP17_SEARCH_OVERLOAD' macro is
        //:   defined when the native library provides a definition for an
        //:   overload of the 'search' function template that accepts a
        //:   searcher object, and not otherwise.
        //:
        //: 2 The 'BSLS_LIBRARYFEATURES_HAS_CPP17_SEARCH_FUNCTORS' macro is
        //:   defined when the native library provides the definition for all
        //:   of the 'search' functor templates in <functional>, and not
        //:   otherwise.
        //:
        //: 3 The 'BSLS_LIBRARYFEATURES_HAS_CPP17_SEARCH_FUNCTORS' macro is
        //:   only defined when the
        //:   'BSLS_LIBRARYFEATURES_HAS_CPP17_SEARCH_OVERLOAD' macro is also
        //:   defined.  This is a santy check as we know that no implementation
        //:   exists that would defined the functors but not the algorithm.
        //
        // Plan:
        //: 1 In namespace 'case13', define 'SearcherNull', a class that is
        //:   compatible with the "searcher" concept and an independent
        //:   definition of the 'search' overload under test.
        //:
        //: 2 If 'BSLS_LIBRARYFEATURES_HAS_CPP17_SEARCH_OVERLOAD' is set,
        //:   confirm that the overload exists, that it accepts the searcher
        //:   object, and returns the expected result.
        //:
        //: 3 If 'BSLS_LIBRARYFEATURES_HAS_CPP17_SEARCH_FUNCTORS' is set,
        //:   confirm that the three native types for searcher objects exist.
        //:
        //: 4 If 'BSLS_LIBRARYFEATURES_HAS_CPP17_SEARCH_OVERLOAD' is *not* set,
        //:   specify 'using' directives to search for definitions in both the
        //:   'std' and the 'case13' namespaces.  Then define an
        //:   an expression using the namespace-unqualified name 'search'.  If
        //:   there is a definition in the 'std' namespace in addition
        //:   to the one we planted in namespace 'case13', the test driver will
        //:   fail to compile (ambiguity error).
        //:
        //: 5 If 'BSLS_LIBRARYFEATURES_HAS_CPP17_SEARCH_FUNCTORS' is *not* set
        //:   set, we do not test for the possible presence of the 'std'
        //:   functors, because only one of them may be missing, which we
        //:   cannot test for, or they may be faulty and not indicated for that
        //:   reason.
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
        // 'BSLS_LIBRARYFEATURES_HAS_CPP11_RANGE_FUNCTIONS'
        //
        // Concerns:
        //: 1 The 'BSLS_LIBRARYFEATURES_HAS_CPP11_RANGE_FUNCTIONS' flag is
        //:   defined when the 'begin' and 'end' function templates are
        //:   provided by the native standard library.
        //:
        //: 2 The 'BSLS_LIBRARYFEATURES_HAS_CPP11_RANGE_FUNCTIONS' flag is not
        //:   defined unless the 'begin' and 'end' function templates are
        //:   provided by the native standard library.
        //
        // Plan:
        //: 1 Write a test type, 'case13::TestType', that has members 'begin'
        //:   and 'end', returning a correspondingly defined 'iterator_type'.
        //:
        //: 2 If the feature macro is defined, explicitly call 'std::begin' and
        //:   'std::end' to confirm they exist with a compatible signature.
        //:
        //: 3 If the feature macro is NOT defined, apply a 'using namespace' to
        //:   both namespaces 'std' and 'case13', and then call 'begin' and
        //:   'end'.  The call will be ambiguous and force a compile-time error
        //:   only if the 'std' functions are also available, indicating that
        //:   the feature macro should be defined.
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
            // This will produce ambiguities if 'begin' and 'end' are defined
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
        // 'BSLS_LIBRARYFEATURES_HAS_CPP14_INTEGER_SEQUENCE'
        //
        // Concerns:
        //: 1 The 'BSLS_LIBRARYFEATURES_HAS_CPP14_INTEGER_SEQUENCE' flag is
        //:   defined when related macros
        //:   'BSLS_LIBRARYFEATURES_HAS_CPP11_BASE_LINE',
        //:   'BSLS_COMPILERFEATURES_SUPPORT_VARIADIC_TEMPLATES' and
        //:   'BSLS_COMPILERFEATURES_SUPPORT_ALIAS_TEMPLATES' are also defined.
        //
        // Plan:
        //: 1 Confirm the expected relationship between
        //:   'BSLS_LIBRARYFEATURES_HAS_CPP14_INTEGER_SEQUENCE' and its related
        //:   macros 'BSLS_LIBRARYFEATURES_HAS_CPP11_BASE_LINE',
        //:   'BSLS_COMPILERFEATURES_SUPPORT_VARIADIC_TEMPLATES' and
        //:   'BSLS_COMPILERFEATURES_SUPPORT_ALIAS_TEMPLATES' using the
        //:   associated conditionally initialized global variables.  See
        //:   "Global constants for testing invariants" above.
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
        //: 1 We detect the expected libraries for the compilers
        //:
        //: 2 We detect the right library implementation
        //
        // Plan:
        //: 1 Verify that the compiler type is one of the expected ones for the
        //:   detected standard library implementation.
        //:
        //: 2 Try to include header files that exist only in the given standard
        //:   library implementation to verify that we actually have that
        //:   implementation.  This is done in the .cpp file so that it the
        //:   sanity check is always done.  To find it, search for
        //:   'Standard library implementation detection verification' in the
        //:   implementation (.cpp) file.
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
#ifdef BSLS_PLATFORM_CMP_GNU
#elif  BSLS_PLATFORM_CMP_CLANG
#elif  BSLS_PLATFORM_CMP_SUN && BSLS_PLATFORM_CMP_VERSION >= 0x5130
#else
#error Unexpected compiler for GNU LibStdC++.
#endif
#elif defined(BSLS_LIBRARYFEATURES_STDCPP_MSVC)
#ifdef BSLS_PLATFORM_CMP_MSVC
#else
#error Unexpected compiler for Microsoft STL.
#endif
#elif defined(BSLS_LIBRARYFEATURES_STDCPP_LLVM)
#ifdef BSLS_PLATFORM_CMP_CLANG
#else
#error Unexpected compiler for LLVM LibC++.
#endif
#elif defined(BSLS_LIBRARYFEATURES_STDCPP_LIBCSTD)
#ifdef BSLS_PLATFORM_CMP_SUN
#else
#error Unexpected compiler for RogueWave STL.
#endif
#elif defined(BSLS_LIBRARYFEATURES_STDCPP_STLPORT)
#ifdef BSLS_PLATFORM_CMP_SUN
#else
#error Unexpected compiler for STLPort.
#endif
#elif defined(BSLS_LIBRARYFEATURES_STDCPP_IBM)
#ifdef BSLS_PLATFORM_CMP_IBM
#else
#error Unexpected compiler for IBM STL.
#endif
#else
#error Unexpected standard library implementation.  Please update test driver.
#endif
      } break;
      case 9: {
        // --------------------------------------------------------------------
        // 'BSLS_LIBRARYFEATURES_HAS_CPP14_BASELINE_LIBRARY'
        //
        // Concerns:
        //: 1 The 'BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY' flag is
        //:   defined when the native standard library defines the following
        //:   functions and types:
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
        //:   o UDLs support for '<chrono>'
        //:
        //:   o UDLs support for '<complex>'
        //
        // Plan:
        //: 1 When 'BSLS_LIBRARYFEATURES_HAS_CPP14_BASELINE_LIBRARY' is defined
        //:   conditionally compile code that includes '<functional>',
        //:   '<iomanip>', '<iterator>', '<chrono>', '<complex>' and uses each
        //:   of the listed function templates at least once.
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
        // 'BSLS_LIBRARYFEATURES_HAS_CPP17_PRECISE_BITWIDTH_ATOMICS'
        //
        // Concerns:
        //: 1 The 'BSLS_LIBRARYFEATURES_HAS_CPP17_PRECISE_BITWIDTH_ATOMICS'
        //:   flag is defined when the native standard library defines the
        //:   following types:
        //:
        //:   o Types defined in '<atomic>'
        //:
        //:     o atomic class template and specializations for precise
        //:       bitwidth integral types
        //:
        //:     o atomic class template and specializations for pointer types
        //
        // Plan:
        //: 1 When 'BSLS_LIBRARYFEATURES_HAS_CPP17_PRECISE_BITWIDTH_ATOMICS' is
        //:   defined conditionally compile code that includes '<atomic>', and
        //:   uses each of the listed types at least once.  (C-1)
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
        // 'isblank'
        //
        // Concerns:
        //: 1 The one-argument form of 'isblank' is available when
        //:   'BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY' is defined.
        //:
        //: 2 The two-argument locale form of 'isblank' is available when
        //:   'BSLS_LIBRARYFEATURES_HAS_CPP11_MISCELLANEOUS_UTILITIES' is
        //:   defined.
        //
        // Plan:
        //: 1 When 'BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY' is defined
        //:   compile code that uses the one-argument 'isblank'.
        //:
        //: 2 When 'BSLS_LIBRARYFEATURES_HAS_CPP11_MISCELLANEOUS_UTILITIES' is
        //:   defined compile code that uses the two-argument 'isblank'.
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
            printf("Expecting 'isblank' from <cctype>\n");
        }
        (void)static_cast<int (*)(int)>(&std::isblank);
#else
        if (verbose) {
            printf("Not expecting 'isblank' from <cctype>\n");
        }
#endif
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_MISCELLANEOUS_UTILITIES
        if (verbose) {
            printf("Expecting 'isblank' from <locale>\n");
        }
        (void)static_cast<bool (*)(char, const std::locale&)>(&std::isblank);
#else
        if (verbose) {
            printf("Not expecting 'isblank' from <locale>\n");
        }
#endif
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // 'BSLS_LIBRARYFEATURES_HAS_C99_*'
        //
        // Concerns:
        //: 1 'BSLS_LIBRARYFEATURES_HAS_C99_LIBRARY' is defined only when the
        //:   native standard library provides C99 features.
        //:
        //: 2 'BSLS_LIBRARYFEATURES_HAS_C99_SNPRINTF' is defined only when the
        //:   native standard library provides C99 'snprintf'.
        //
        //
        // Plan:
        //: 1 When 'BSLS_LIBRARYFEATURES_HAS_C99_LIBRARY' is defined compile
        //:   code that uses C99 library functions.
        //:
        //: 2 If 'BSLS_LIBRARYFEATURES_HAS_C99_SNPRINTF' is defined compile
        //:   code using 'snprintf'.
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
        // 'BSLS_LIBRARYFEATURES_HAS_CPP11_UNIQUE_PTR'
        //
        // Concerns:
        //: 1 'BSLS_LIBRARYFEATURES_HAS_CPP11_UNIQUE_PTR' is defined only when
        //:   the native standard library defines the 'unique_ptr' class
        //:   template (in '<memory>').
        //
        //: 2 If 'BSLS_LIBRARYFEATURES_HAS_CPP11_UNIQUE_PTR' is defined then
        //:   the related macro
        //:   'BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES' is also
        //:   defined.
        //
        // Plan:
        //: 1 When 'BSLS_LIBRARYFEATURES_HAS_CPP11_UNIQUE_PTR' is defined
        //:   compile code that includes '<memory>' and constructs a
        //:   'unique_ptr' object to an 'int'.
        //:
        //: 2 If 'BSLS_LIBRARYFEATURES_HAS_CPP11_UNIQUE_PTR' is defined confirm
        //:   that 'BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES' is also
        //:   defined using the associated conditionally initialized global
        //:   variables.  See "Global constants for testing invariants" above.
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
        // 'BSLS_LIBRARYFEATURES_HAS_CPP11_TUPLE'
        //
        // Concerns:
        //: 1 'BSLS_LIBRARYFEATURES_HAS_CPP11_TUPLE' is defined only when the
        //:   native standard library defines the '<tuple>' class template.
        //:
        //: 2 If 'BSLS_LIBRARYFEATURES_HAS_CPP11_TUPLE' is defined then the
        //:   related macro 'BSLS_COMPILERFEATURES_SUPPORT_VARIADIC_TEMPLATES'
        //:   is also defined.
        //
        // Plan:
        //: 1 When 'BSLS_LIBRARYFEATURES_HAS_CPP11_TUPLE' is defined compile
        //:   code that includes '<tuple>' and constructs a 'tuple' object.
        //:
        //: 2 If 'BSLS_LIBRARYFEATURES_HAS_CPP11_TUPLE' is defined confirm that
        //:   'BSLS_COMPILERFEATURES_SUPPORT_VARIADIC_TEMPLATES' is also
        //:   defined using the associated conditionally initialized global
        //:   variables.  See "Global constants for testing invariants" above.
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
        // 'BSLS_LIBRARYFEATURES_HAS_CPP11_PAIR_PIECEWISE_CONSTRUCTOR'
        //
        // Concerns:
        //: 1 The 'BSLS_LIBRARYFEATURES_HAS_CPP11_PAIR_PIECEWISE_CONSTRUCTOR'
        //:   flag is defined when the native standard library defines for its
        //:   'pair' class template (defined in '<utility>') a constructor that
        //:   accepts as arguments 'std::piecewise_construct' (also
        //:   defined in '<utility>' followed by two 'tuple' arguments.
        //:
        //: 2 If 'BSLS_LIBRARYFEATURES_HAS_CPP11_PAIR_PIECEWISE_CONSTRUCTOR' is
        //:   defined then related macros
        //:   'BSLS_LIBRARYFEATURES_HAS_CPP11_TUPLE'
        //:   'BSLS_COMPILERFEATURES_SUPPORT_VARIADIC_TEMPLATES' are also
        //:   defined.
        //
        // Plan:
        //: 1 When 'BSLS_LIBRARYFEATURES_HAS_CPP11_PAIR_PIECEWISE_CONSTRUCTOR'
        //:   is defined conditionally compile code that includes '<utility>',
        //:   '<tuple>' and creates an object using the constructor specified
        //:   in C-1.
        //:
        //: 2 Confirm the expected relationship between
        //:   'BSLS_LIBRARYFEATURES_HAS_CPP11_PAIR_PIECEWISE_CONSTRUCTOR' and
        //:   its related macros 'BSLS_LIBRARYFEATURES_HAS_CPP11_TUPLE'
        //:   'BSLS_LIBRARYFEATURES_HAS_CPP98_AUTO_PTR' using the associated
        //:   conditionally initialized global variables.  See "Global
        //:   constants for testing invariants" above.
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
        // 'BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY'
        //
        // Concerns:
        //: 1 The 'BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY' flag is
        //:   defined when the native standard library defines the following
        //:   functions and types:
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
        //:   o Functions defined in '<functional>'
        //:     o 'bind'
        //:     o 'bit_and'
        //:     o 'bit_or'
        //:     o 'bit_xor'
        //:     o 'cref'
        //:     o 'is_bind_expression'
        //:     o 'is_placeholder'
        //:     o 'mem_fn'
        //:     o 'ref'
        //:     o 'reference_wrapper'
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
        //:     o 'addressof';
        //:     o 'uninitialized_copy_n'
        //:
        //:   o Function defined in '<numeric>'
        //:     o 'iota'
        //:
        //:   o Member function defined in '<typeinfo>'
        //:     o 'type_info::hash_code'
        //:
        //:   o Function defined in '<utility>'
        //:     o 'swap'
        //
        // Plan:
        //: 1 When 'BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY' is defined
        //:   run the 'testSimpleUniformRandomNumberGenerator' function to
        //:   confirm that the helper class
        //:   'SimpleUniformRandomNumberGenerator' -- used in the test of the
        //:   'std::shuffle' function -- works as expected.
        //:
        //: 2 When 'BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY' is defined
        //:   conditionally compile code that includes '<algorithm>',
        //:   '<atomic>', '<functional>', '<iomanip>', '<iterator>',
        //:   '<locale>', '<memory>', '<numeric>', '<utility>' and uses each of
        //:   the listed function templates at least once.
        //:
        //: 3 When 'BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY' is defined
        //:   conditionally compile code that includes '<typeindex>' and verify
        //:   the 'hash_code' method is availble in both 'type_info' and
        //:   'type_index'.
        //
        // Testing:
        //   BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY
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
        if (veryVeryVerbose) P(BSLS_PLATFORM_CMP_VERSION);
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // 'BSLS_LIBRARYFEATURES_HAS_CPP98_AUTO_PTR'
        //
        // Concerns:
        //: 1 The 'BSLS_LIBRARYFEATURES_HAS_CPP98_AUTO_PTR' flag is defined
        //:   when the native standard library defines type
        //:   'std::auto_ptr' template in '<memory>'.
        //:
        //: 2 The 'BSLS_LIBRARYFEATURES_HAS_CPP98_AUTO_PTR' macro is set on all
        //:   platforms (until C++17).
        //
        // Plan:
        //: 1 When 'BSLS_LIBRARYFEATURES_HAS_CPP98_AUTO_PTR' is defined
        //:   conditionally compile code that includes '<memory>' and
        //:   constructs 'std::auto_ptr' object for 'int'.
        //:
        //: 2 Confirm the value of the conditionally compiled global variable
        //:   'BSLS_LIBRARYFEATURES_HAS_CPP98_AUTO_PTR_defined' is 'true'.
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
      case -1: {
        // --------------------------------------------------------------------
        // 'BSLS_LIBRARYFEATURES_HAS_CPP17_BOOL_CONSTANT'
        //
        // Concerns:
        //: 1 The 'BSLS_LIBRARYFEATURES_HAS_CPP17_BOOL_CONSTANT' must never be
        //:   defined.
        //
        // Plan:
        //: 1 Confirm the value of the conditionally compiled global variable
        //:   'BSLS_LIBRARYFEATURES_HAS_CPP17_BOOL_CONSTANT_defined' is
        //:   'false'.
        //
        // Testing:
        //   BSLS_LIBRARYFEATURES_HAS_CPP17_BOOL_CONSTANT: obsolescent: never
        //   defined
        // --------------------------------------------------------------------

        if (verbose) puts("\n'BSLS_LIBRARYFEATURES_HAS_CPP17_BOOL_CONSTANT'"
                          "\n==============================================");

        if (verbose) {
            P(BSLS_LIBRARYFEATURES_HAS_CPP17_BOOL_CONSTANT_defined);
        }

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_BOOL_CONSTANT
        ASSERT(true == std::bool_constant<true>());
#endif

        ASSERT(!BSLS_LIBRARYFEATURES_HAS_CPP17_BOOL_CONSTANT_defined);

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
