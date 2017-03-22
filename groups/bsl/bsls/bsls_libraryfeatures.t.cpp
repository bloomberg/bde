// bsls_libraryfeatures.t.cpp                                         -*-C++-*-
#include <bsls_libraryfeatures.h>

#include <bsls_bsltestutil.h>
#include <bsls_compilerfeatures.h>
#include <bsls_cpp11.h>
#include <bsls_nativestd.h>

#include <stddef.h>  // for 'size_t'
#include <stdio.h>   // for 'printf'
#include <stdlib.h>  // for 'atoi'

// ============================================================================
//                             TEST PLAN
// ----------------------------------------------------------------------------
//                             Overview
//                             --------
// This component provides macros that indicate the presence (or not) of
// selected features (i.e., header-files/types) in the native standard library
// used in the current build.  In the current implementation, the these flags
// are not set according to information directly from the library; rather,
// these features are inferred from platform and compiler information acquired
// from macros set in other components.  The role of this test driver is to
// provide a redundant check that those macros were correctly combined to
// produce the expected results.
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
// ----------------------------------------------------------------------------
// [ 1] BSLS_LIBRARYFEATURES_HAS_CPP98_AUTO_PTR
// [ 2] BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY
// [ 3] BSLS_LIBRARYFEATURES_HAS_CPP11_PAIR_PIECEWISE_CONSTRUCTOR
// [ 4] BSLS_LIBRARYFEATURES_HAS_CPP11_TUPLE
// [ 5] BSLS_LIBRARYFEATURES_HAS_CPP11_UNIQUE_PTR
// [ 6] BSLS_LIBRARYFEATURES_HAS_C99_LIBRARY
// [ 6] BSLS_LIBRARYFEATURES_HAS_C99_SNPRINTF
// [ 7] int native_std::isblank(int);
// [ 7] bool native_std::isblank(char, const native_std::locale&);
// [ 8] BSLS_LIBRARYFEATURES_HAS_CPP17_PRECISE_BITWIDTH_ATOMICS
// ----------------------------------------------------------------------------
// [ 9] USAGE EXAMPLE
// [-1] BSLS_LIBRARYFEATURES_HAS_CPP17_BOOL_CONSTANT: obsolescent: not defined
// ----------------------------------------------------------------------------
// TBD Add tests for the new macros (and amendments to existing macros):
//
//: o BSLS_LIBRARYFEATURES_HAS_CPP11_EXCEPTION_HANDLING
//: o BSLS_LIBRARYFEATURES_HAS_CPP11_GARBAGE_COLLECTION_API
//: o BSLS_LIBRARYFEATURES_HAS_CPP11_MISCELLANEOUS_UTILITIES
//: o BSLS_LIBRARYFEATURES_HAS_CPP11_PROGRAM_TERMINATION

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

// ============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
// ----------------------------------------------------------------------------

                     // Global constants for testing invariants

static const bool u_BSLS_COMPILERFEATURES_SUPPORT_VARIADIC_TEMPLATES_defined =
#if         defined(BSLS_COMPILERFEATURES_SUPPORT_VARIADIC_TEMPLATES)
                                                                          true;
#else
                                                                         false;
#endif

static const bool u_BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES_defined =
#if         defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)
                                                                          true;
#else
                                                                         false;
#endif

static const bool u_BSLS_LIBRARYFEATURES_HAS_CPP17_BOOL_CONSTANT_defined =
#if         defined(BSLS_LIBRARYFEATURES_HAS_CPP17_BOOL_CONSTANT)
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
        ASSERT(0 < sizeof(native_std::atomic_int8_t));
        ASSERT(0 < sizeof(native_std::atomic_int16_t));
        ASSERT(0 < sizeof(native_std::atomic_int32_t));
        ASSERT(0 < sizeof(native_std::atomic_int64_t));
        ASSERT(0 < sizeof(native_std::atomic_uint8_t));
        ASSERT(0 < sizeof(native_std::atomic_uint16_t));
        ASSERT(0 < sizeof(native_std::atomic_uint32_t));
        ASSERT(0 < sizeof(native_std::atomic_uint64_t));
        ASSERT(0 < sizeof(native_std::atomic_intptr_t));
        ASSERT(0 < sizeof(native_std::atomic_uintptr_t));
    }
#endif

static const bool
    u_BSLS_LIBRARYFEATURES_HAS_CPP17_PRECISE_BITWIDTH_ATOMICS_defined =
#if         defined(BSLS_LIBRARYFEATURES_HAS_CPP17_PRECISE_BITWIDTH_ATOMICS)
                                                                        true;
#else
                                                                        false;
#endif

                    // case 7

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY)
     #include <cctype>
#endif

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP11_MISCELLANEOUS_UTILITIES)
     #include <locale>
#endif

static const bool
    u_BSLS_LIBRARYFEATURES_HAS_CPP11_MISCELLANEOUS_UTILITIES_defined =
#if defined(BSLS_LIBRARYFEATURES_HAS_CPP11_MISCELLANEOUS_UTILITIES)
                                                                          true;
#else
                                                                         false;
#endif


                    // case 6

#if defined(BSLS_LIBRARYFEATURES_HAS_C99_LIBRARY)
     #include <cstdlib>
     #include <cctype>
     #include <cmath>
#endif

#if defined(BSLS_LIBRARYFEATURES_HAS_C99_SNPRINTF)
     #include <cstdio>
#endif

static const bool u_BSLS_LIBRARYFEATURES_HAS_C99_LIBRARY_defined =
#if         defined(BSLS_LIBRARYFEATURES_HAS_C99_LIBRARY)
                                                                  true;
#else
                                                                  false;
#endif

static const bool u_BSLS_LIBRARYFEATURES_HAS_C99_SNPRINTF_defined =
#if         defined(BSLS_LIBRARYFEATURES_HAS_C99_SNPRINTF)
                                                                  true;
#else
                                                                  false;
#endif


                    // case 5

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP11_UNIQUE_PTR) || \
    defined(BSLS_LIBRARYFEATURES_HAS_CPP11_UNIQUE_PTR_FORCE)

    #include <memory>
    native_std::unique_ptr<int> up;
#endif

static const bool u_BSLS_LIBRARYFEATURES_HAS_CPP11_UNIQUE_PTR_defined =
#if         defined(BSLS_LIBRARYFEATURES_HAS_CPP11_UNIQUE_PTR)
                                                                  true;
#else
                                                                  false;
#endif
                    // case 4

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP11_TUPLE) || \
    defined(BSLS_LIBRARYFEATURES_HAS_CPP11_TUPLE_FORCE)

    #include <tuple>
    native_std::tuple<char, short, int, float, double> t4;

    #ifndef BSLS_COMPILERFEATURES_SUPPORT_VARIADIC_TEMPLATES
    #error "'BSLS_LIBRARYFEATURES_HAS_CPP11_TUPLE' requires \
            'BSLS_COMPILERFEATURES_SUPPORT_VARIADIC_TEMPLATES'"
    #endif

#endif

static const bool u_BSLS_LIBRARYFEATURES_HAS_CPP11_TUPLE_defined =
#if         defined(BSLS_LIBRARYFEATURES_HAS_CPP11_TUPLE)
                                                       true;
#else
                                                       false;
#endif

                    // case 3

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP11_PAIR_PIECEWISE_CONSTRUCTOR) || \
    defined(BSLS_LIBRARYFEATURES_HAS_CPP11_PAIR_PIECEWISE_CONSTRUCTOR_FORCE)

    #include <tuple>
    #include <utility> // for 'pair' and 'piecewise_construct'

    native_std::pair<long, double> p(native_std::piecewise_construct,
                                     native_std::tuple<int>(1),
                                     native_std::tuple<int>(2));

    #ifndef BSLS_LIBRARYFEATURES_HAS_CPP11_TUPLE
    #error "'BSLS_LIBRARYFEATURES_HAS_CPP11_PAIR_PIECEWISE_CONSTRUCTOR' \
            requires 'BSLS_LIBRARYFEATURES_HAS_CPP11_TUPLE'"
    #endif

    #ifndef BSLS_COMPILERFEATURES_SUPPORT_VARIADIC_TEMPLATES
    #error "'BSLS_LIBRARYFEATURES_HAS_CPP11_PAIR_PIECEWISE_CONSTRUCTOR' \
            requires 'BSLS_COMPILERFEATURES_SUPPORT_VARIADIC_TEMPLATES'"
    #endif

#endif

static const bool
            u_BSLS_LIBRARYFEATURES_HAS_CPP11_PAIR_PIECEWISE_CONSTRUCTOR_defined
                                                                              =
#if   defined(BSLS_LIBRARYFEATURES_HAS_CPP11_PAIR_PIECEWISE_CONSTRUCTOR)
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
        // 'native_std::shuffle' function in in the 'useCpp11Algorithms()' test
        // function.

      public:
        // TYPES
        typedef unsigned result_type;
            // Alias for the type of the values returned by this class.

        // CLASS METHODS
        static BSLS_CPP11_CONSTEXPR unsigned min();
            // Return 0, the smallest value returned by 'operator()'.

        static BSLS_CPP11_CONSTEXPR unsigned max();
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
    BSLS_CPP11_CONSTEXPR unsigned SimpleUniformRandomNumberGenerator::min()
    {
        return 0;
    }
    BSLS_CPP11_CONSTEXPR unsigned SimpleUniformRandomNumberGenerator::max()
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
    #include <memory>    // for 'uninitialized_copy_n'
    #include <numeric>   // for 'iota'

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

        native_std::all_of(inputFirst, inputLast, unaryPredicate);
        native_std::any_of(inputFirst, inputLast, unaryPredicate);

        int  oarray[NUM_ELEMENTS];
        int *outputFirst = &oarray[0];
        int *outputLast  = &oarray[NUM_ELEMENTS];

        native_std::copy_if(inputFirst,
                            inputLast,
                            outputFirst,
                            unaryPredicate);

        native_std::copy_n (inputFirst, NUM_ELEMENTS, outputFirst);

        native_std::find_if_not(inputFirst, inputLast, unaryPredicate);

        native_std::iota(outputFirst, outputLast, 0);

        native_std::is_heap      (inputFirst, inputLast);
        native_std::is_heap_until(inputFirst, inputLast);

        native_std::is_partitioned (inputFirst, inputLast, unaryPredicate);
        native_std::is_permutation (inputFirst, inputLast, inputFirst);
        native_std::is_sorted      (inputFirst, inputLast);
        native_std::is_sorted_until(inputFirst, inputLast);

        native_std::minmax(0, 1);
        native_std::minmax_element(inputFirst, inputLast);

        int  oarray2[NUM_ELEMENTS];
        int *output2First = &oarray2[0];
        int *output2Last  = &oarray2[NUM_ELEMENTS];

        native_std::move         (outputFirst, outputLast, output2First);
        native_std::move_backward(outputFirst, outputLast, output2Last);

        native_std::none_of(inputFirst, inputLast, unaryPredicate);

        native_std::partition_copy(inputFirst,
                                   inputLast,
                                   outputFirst,
                                   output2First,
                                   unaryPredicate);

        native_std::partition_point(inputFirst, inputLast, unaryPredicate);

        SimpleUniformRandomNumberGenerator surng;
        native_std::shuffle(outputFirst,
                            outputLast,
                            surng);

        native_std::uninitialized_copy_n(inputFirst,
                                         NUM_ELEMENTS,
                                         outputFirst);
    }
#endif

static const bool u_BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY_defined =
#if         defined(BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY)
                                                                        true;
#else
                                                                        false;
#endif

                    // case 1

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP98_AUTO_PTR) ||\
    defined(BSLS_LIBRARYFEATURES_HAS_CPP98_AUTO_PTR_FORCE)

    #include <memory>
    native_std::auto_ptr<int> ap;

#endif

static const bool u_BSLS_LIBRARYFEATURES_HAS_CPP98_AUTO_PTR_defined =
#if         defined(BSLS_LIBRARYFEATURES_HAS_CPP98_AUTO_PTR)
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
// interface that returns a 'native_std::tuple'.
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
        static native_std::tuple<int, double, double> getMedianMeanVariance(
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
    Q(printFlags: Enter);
    Q(printFlags: component-defined macros);
#if defined(BSLS_LIBRARYFEATURES_HAS_CPP98_AUTO_PTR)
          Q(BSLS_LIBRARYFEATURES_HAS_CPP98_AUTO_PTR);
#endif
#if defined(BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY)
          Q(BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY);
#endif
#if defined(BSLS_LIBRARYFEATURES_HAS_CPP11_TUPLE)
          Q(BSLS_LIBRARYFEATURES_HAS_CPP11_TUPLE);
#endif
#if defined(BSLS_LIBRARYFEATURES_HAS_CPP11_UNIQUE_PTR)
          Q(BSLS_LIBRARYFEATURES_HAS_CPP11_UNIQUE_PTR);
#endif
#if defined(BSLS_LIBRARYFEATURES_HAS_CPP11_PAIR_PIECEWISE_CONSTRUCTOR)
          Q(BSLS_LIBRARYFEATURES_HAS_CPP11_PAIR_PIECEWISE_CONSTRUCTOR);
#endif

    Q(printFlags: imported macros);

#if defined(BSLS_COMPILERFEATURES_SUPPORT_VARIADIC_TEMPLATES)
          Q(BSLS_COMPILERFEATURES_SUPPORT_VARIADIC_TEMPLATES);
#endif
#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)
          Q(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES);
#endif
#if defined(BSLS_PLATFORM_OS_SUNOS)
          Q(BSLS_PLATFORM_OS_SUNOS)
#endif
#if defined(BSLS_PLATFORM_OS_SOLARIS)
          Q(BSLS_PLATFORM_OS_SOLARIS)
#endif
#if defined(BSLS_PLATFORM_OS_LINUX)
          Q(BSLS_PLATFORM_OS_LINUX)
#endif
#if defined(BSLS_PLATFORM_CMP_CLANG)
          P(BSLS_PLATFORM_CMP_CLANG)
#endif
#if defined(BSLS_PLATFORM_CMP_EDG)
          P(BSLS_PLATFORM_CMP_EDG)
#endif
#if defined(BSLS_PLATFORM_CMP_GNU)
          P(BSLS_PLATFORM_CMP_GNU)
#endif
#if defined(BSLS_PLATFORM_CMP_HP)
          P(BSLS_PLATFORM_CMP_HP)
#endif
#if defined(BSLS_PLATFORM_CMP_IBM)
          P(BSLS_PLATFORM_CMP_IBM)
#endif
#if defined(BSLS_PLATFORM_CMP_MSVC)
          P(BSLS_PLATFORM_CMP_MSVC)
#endif
#if defined(BSLS_PLATFORM_CMP_SUN)
          P(BSLS_PLATFORM_CMP_SUN)
#endif
#if defined(__GXX_EXPERIMENTAL_CXX0X__)
          P(__GXX_EXPERIMENTAL_CXX0X__)
#endif
#if defined(__APPLE_CC__)
          P(__APPLE_CC__)
#endif
#if defined(BSLS_PLATFORM_CMP_VERSION)
          P(BSLS_PLATFORM_CMP_VERSION)
#endif

    Q(printFlags: Leave);
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
    bool veryVeryVeryVerbose = argc > 5;

    (void)        veryVerbose;  // unused variable warning
    (void)veryVeryVeryVerbose;  // unused variable warning

    setbuf(stdout, NULL);    // Use unbuffered output

    printf("TEST " __FILE__ " CASE %d\n", test);

    if (veryVeryVerbose) {
        printFlags();
    }

    switch (test) { case 0:
      case 9: {
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

        if (verbose) printf("USAGE EXAMPLE\n"
                            "=============\n");
      } break;
      case 8: {
        // --------------------------------------------------------------------
        // TESTING 'BSLS_LIBRARYFEATURES_HAS_CPP17_PRECISE_BITWIDTH_ATOMICS'
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
        //:Plan:
        //: 1 When 'BSLS_LIBRARYFEATURES_HAS_CPP17_PRECISE_BITWIDTH_ATOMICS' is
        //:   defined conditionally compile code that includes '<atomic>', and
        //:   uses each of the listed types at least once.  (C-1)
        //
        // Testing:
        //   BSLS_LIBRARYFEATURES_HAS_CPP17_PRECISE_BITWIDTH_ATOMICS
        // --------------------------------------------------------------------

        if (verbose) printf(
        "TESTING 'BSLS_LIBRARYFEATURES_HAS_CPP17_PRECISE_BITWIDTH_ATOMICS'\n"
        "=================================================================\n");

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP17_PRECISE_BITWIDTH_ATOMICS)
        useCpp11PreciseBitwidthAtomics();
#endif

        if (verbose) {
            P(
             u_BSLS_LIBRARYFEATURES_HAS_CPP17_PRECISE_BITWIDTH_ATOMICS_defined)
        }

        if (veryVeryVerbose) P(BSLS_PLATFORM_CMP_VERSION);

      } break;
      case 7: {
        // --------------------------------------------------------------------
        // TESTING 'isblank'
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
        //   int native_std::isblank(int);
        //   bool native_std::isblank(char, const native_std::locale&);
        // --------------------------------------------------------------------

        if (verbose) printf("TESTING 'isblank'\n"
                            "=================\n");

        if (verbose) {
            P(u_BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY_defined)
            P(u_BSLS_LIBRARYFEATURES_HAS_CPP11_MISCELLANEOUS_UTILITIES_defined)
        }

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY
        if (verbose) {
            printf("Expecting 'isblank' from <cctype>\n");
        }
        int (*isblankc)(int) = &native_std::isblank;
        (void)isblankc;
#else
        if (verbose) {
            printf("Not expecting 'isblank' from <cctype>\n");
        }
#endif
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_MISCELLANEOUS_UTILITIES
        if (verbose) {
            printf("Expecting 'isblank' from <locale>\n");
        }
        bool (*isblankl)(char, const native_std::locale&) =
            &native_std::isblank;
        (void)isblankl;
#else
        if (verbose) {
            printf("Not expecting 'isblank' from <locale>\n");
        }
#endif
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // TESTING 'BSLS_LIBRARYFEATURES_HAS_C99_*'
        //
        // Concerns:
        //: 1 'BSLS_LIBRARYFEATURES_HAS_C99_LIBRARY' is defined only when
        //:   the native standard library provides C99 features.
        //:
        //: 2 'BSLS_LIBRARYFEATURES_HAS_C99_SNPRINTF' is defined only when
        //:   the native standard library provides C99 snprintf.
        //
        //
        // Plan:
        //: 1 When 'BSLS_LIBRARYFEATURES_HAS_C99_LIBRARY' is defined
        //:   compile code that uses C99 library functions.
        //:
        //: 2 If 'BSLS_LIBRARYFEATURES_HAS_C99_SNPRINTF' is defined compile
        //:   code using 'snprintf'.
        //
        // Testing:
        //   BSLS_LIBRARYFEATURES_HAS_C99_LIBRARY
        //   BSLS_LIBRARYFEATURES_HAS_C99_SNPRINTF
        // --------------------------------------------------------------------

        if (verbose) printf(
                      "TESTING 'BSLS_LIBRARYFEATURES_HAS_C99_*'\n"
                      "========================================\n");

        if (verbose) {
            P(u_BSLS_LIBRARYFEATURES_HAS_C99_LIBRARY_defined);
            P(u_BSLS_LIBRARYFEATURES_HAS_C99_SNPRINTF_defined);
        }

        if (u_BSLS_LIBRARYFEATURES_HAS_CPP11_UNIQUE_PTR_defined) {
            ASSERT(true ==
                    u_BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES_defined);
        }

#ifdef BSLS_LIBRARYFEATURES_HAS_C99_LIBRARY
        // Test a subset of C99 features

        // cmath
        {
            typedef int (*FuncPtrType)(double);
            FuncPtrType funcPtr = &native_std::fpclassify;
            (void)funcPtr;  // suppress unused variable warning
        }

        // cstdlib
        {
            typedef native_std::lldiv_t dummy;
            dummy x;  // suppress unused typedef warning
            (void)x;  // suppress unused variable warning
        }

        // cctype
        {
            typedef int (*FuncPtrType)(int);
            FuncPtrType funcPtr = &native_std::isblank;
            (void)funcPtr;  // suppress unused variable warning

        }
#endif

#ifdef BSLS_LIBRARYFEATURES_HAS_C99_SNPRINTF
        (void)&native_std::snprintf;
#endif

        if (veryVeryVerbose) P(BSLS_PLATFORM_CMP_VERSION);

      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING 'BSLS_LIBRARYFEATURES_HAS_CPP11_UNIQUE_PTR'
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
        //:   variables. See "Global constants for testing invariants" above.
        //
        // Testing:
        //   BSLS_LIBRARYFEATURES_HAS_CPP11_UNIQUE_PTR
        // --------------------------------------------------------------------

        if (verbose) printf(
                      "TESTING 'BSLS_LIBRARYFEATURES_HAS_CPP11_UNIQUE_PTR'\n"
                      "===================================================\n");

        if (verbose) {
            P(u_BSLS_LIBRARYFEATURES_HAS_CPP11_UNIQUE_PTR_defined);
        }

        if (u_BSLS_LIBRARYFEATURES_HAS_CPP11_UNIQUE_PTR_defined) {
            ASSERT(true ==
                    u_BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES_defined);
        }

        if (veryVeryVerbose) P(BSLS_PLATFORM_CMP_VERSION);

      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING 'BSLS_LIBRARYFEATURES_HAS_CPP11_TUPLE'
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

        if (verbose) printf(
                           "TESTING 'BSLS_LIBRARYFEATURES_HAS_CPP11_TUPLE'\n"
                           "==============================================\n");

        if (verbose) {
            P(u_BSLS_LIBRARYFEATURES_HAS_CPP11_TUPLE_defined);
        }

        if (u_BSLS_LIBRARYFEATURES_HAS_CPP11_TUPLE_defined) {
            ASSERT(true ==
                   u_BSLS_COMPILERFEATURES_SUPPORT_VARIADIC_TEMPLATES_defined);
        }

        if (veryVeryVerbose) P(BSLS_PLATFORM_CMP_VERSION);

      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING 'BSLS_LIBRARYFEATURES_HAS_CPP11_PAIR_PIECEWISE_CONSTRUCTOR'
        //
        // Concerns:
        //: 1 The 'BSLS_LIBRARYFEATURES_HAS_CPP11_PAIR_PIECEWISE_CONSTRUCTOR'
        //:   flag is defined when the native standard library defines for its
        //:   'pair' class template (defined in '<utility>') a constructor that
        //:   accepts as arguments 'native_std::piecewise_construct' (also
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

        if (verbose) printf(
      "TESTING 'BSLS_LIBRARYFEATURES_HAS_CPP11_PAIR_PIECEWISE_CONSTRUCTOR'\n"
      "===================================================================\n");

        if (verbose) {
         P(u_BSLS_LIBRARYFEATURES_HAS_CPP11_PAIR_PIECEWISE_CONSTRUCTOR_defined)
        }

        if(u_BSLS_LIBRARYFEATURES_HAS_CPP11_PAIR_PIECEWISE_CONSTRUCTOR_defined)
        {
            ASSERT(true == u_BSLS_LIBRARYFEATURES_HAS_CPP11_TUPLE_defined);
            ASSERT(true ==
                   u_BSLS_COMPILERFEATURES_SUPPORT_VARIADIC_TEMPLATES_defined);
        }

        if (veryVeryVerbose) P(BSLS_PLATFORM_CMP_VERSION);

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING 'BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY'
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
        //:     o addressof;
        //:     o 'uninitialized_copy_n'
        //:
        //:   o Function defined in '<numeric>'
        //:     o 'iota'
        //:
        //:   o Function defined in '<utility>'
        //:     o 'swap'
        //
        // Plan:
        //: 1 When 'BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY' is defined
        //:   run the 'testSimpleUniformRandomNumberGenerator' function to
        //:   confirm that the helper class
        //:   'SimpleUniformRandomNumberGenerator' -- used in the test of the
        //:   'native_std::shuffle' function -- works as expected.
        //
        //: 2 When 'BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY' is defined
        //:   conditionally compile code that includes '<algorithm>',
        //:   '<atomic>', '<functional>', '<iomanip>', '<iterator>',
        //:   '<locale>', '<memory>', '<numeric>', '<utility>' and uses each of
        //:   the listed function templates at least once.
        //
        // Testing:
        //   BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY
        // --------------------------------------------------------------------

        if (verbose) printf(
                "TESTING 'BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY'\n"
                "=========================================================\n");

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY)
        testSimpleUniformRandomNumberGenerator();
        useCpp11Algorithms();
#endif

        if (verbose) {
            P(u_BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY_defined)
        }

        if (veryVeryVerbose) P(BSLS_PLATFORM_CMP_VERSION);

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // TESTING 'BSLS_LIBRARYFEATURES_HAS_CPP98_AUTO_PTR'
        //
        // Concerns:
        //: 1 The 'BSLS_LIBRARYFEATURES_HAS_CPP98_AUTO_PTR' flag is defined
        //:   when the native standard library defines type
        //:   'native_std::auto_ptr' template in '<memory>'.
        //:
        //: 2 The 'BSLS_LIBRARYFEATURES_HAS_CPP98_AUTO_PTR' macro is set on all
        //:   platforms (until C++17).
        //
        // Plan:
        //: 1 When 'BSLS_LIBRARYFEATURES_HAS_CPP98_AUTO_PTR' is defined
        //:   conditionally compile code that includes '<memory>' and
        //:   constructs 'native_std::auto_ptr' object for 'int'.
        //:
        //: 2 Confirm the value of the conditionally compiled global variable
        //:   'u_BSLS_LIBRARYFEATURES_HAS_CPP98_AUTO_PTR_defined' is 'true'.
        //
        // Testing:
        //   BSLS_LIBRARYFEATURES_HAS_CPP98_AUTO_PTR
        // --------------------------------------------------------------------

        if (verbose) printf(
                        "TESTING 'BSLS_LIBRARYFEATURES_HAS_CPP98_AUTO_PTR'\n"
                        "=================================================\n");

        if (verbose) {
            P(u_BSLS_LIBRARYFEATURES_HAS_CPP98_AUTO_PTR_defined)
        }

        // This macro should be defined on all platforms until C++17.

        ASSERT(true == u_BSLS_LIBRARYFEATURES_HAS_CPP98_AUTO_PTR_defined);

        if (veryVeryVerbose) P(BSLS_PLATFORM_CMP_VERSION);

      } break;
      case -1: {
        // --------------------------------------------------------------------
        // TESTING 'BSLS_LIBRARYFEATURES_HAS_CPP17_BOOL_CONSTANT'
        //
        // Concerns:
        //: 1 The 'BSLS_LIBRARYFEATURES_HAS_CPP17_BOOL_CONSTANT' must never be
        //:   defined.
        //
        // Plan:
        //: 1 Confirm the value of the conditionally compiled global variable
        //:   'u_BSLS_LIBRARYFEATURES_HAS_CPP17_BOOL_CONSTANT_defined' is
        //:   'false'.
        //
        // Testing:
        //   BSLS_LIBRARYFEATURES_HAS_CPP17_BOOL_CONSTANT: obsolescent: never
        //   defined
        // --------------------------------------------------------------------

        if (verbose) printf(
                   "TESTING 'BSLS_LIBRARYFEATURES_HAS_CPP17_BOOL_CONSTANT'\n"
                   "======================================================\n");

        if (verbose) {
            P(u_BSLS_LIBRARYFEATURES_HAS_CPP17_BOOL_CONSTANT_defined);
        }

        ASSERT(false ==
                       u_BSLS_LIBRARYFEATURES_HAS_CPP17_BOOL_CONSTANT_defined);

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
