// bsls_consteval.t.cpp                                               -*-C++-*-
#include <bsls_consteval.h>

#include <bsls_assert.h>
#include <bsls_bsltestutil.h>
#include <bsls_buildtarget.h>

#include <stdio.h>
#include <stdlib.h>  // `atoi`

// ============================================================================
//                                 TEST PLAN
// ----------------------------------------------------------------------------
//                                 Overview
//                                 --------
// This test driver serves as a framework for manually checking the macros
// defined in this component.
//
// The BREATHING TEST simply prints out the values of the macros in verbose
// mode.
//
// ----------------------------------------------------------------------------
// [ 2] USAGE EXAMPLE
// [ 1] BREATHING TEST
// ----------------------------------------------------------------------------

namespace bsls = BloombergLP::bsls;

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
//                              HELPER FUNCTIONS
// ----------------------------------------------------------------------------

namespace {

/// A statically-sized table for pointers to C strings for collecting
/// information about macros.
template <size_t t_MAX_SIZE>
class StaticStringsTable {

  public:
    // PUBLIC CONSTANTS
    static const size_t k_MAX_SIZE = t_MAX_SIZE;

  private:
    // DATA
    const char *d_name_p;          // Name for this table

    const char *d_table[k_MAX_SIZE];
    size_t      d_index;

  public:
    // CREATORS

    /// Create a new `StaticStringsTable` with the specified `name`.  The
    /// behavior is undefined unless `name` outlives the created object.
    explicit StaticStringsTable(const char *name)
    : d_name_p(name)
    , d_index(0)
    {
    }

    // MANIPULATORS

    /// If there are no more free locations in the table report that error
    /// to `stdout`.  Otherwise store the specified `string` to the next
    /// free location then increment the next free location.
    void pushBack(const char *string)
    {
        if (k_MAX_SIZE == d_index) {
            fputs(d_name_p, stdout);
            fputs(" table is full, could not store ", stdout);
            puts(string);
            aSsErT(true, "Could not store string.", __LINE__);
            return;                                                   // RETURN
        }

        d_table[d_index++] = string;
    }

    // ACCESSORS

    /// Return the string pointer at the specified `idx` position or return
    /// an error if `idx` is out of bounds.
    const char *operator[](size_t idx) const
    {
        ASSERT(idx < count());
        if (idx >= count()) {
            return "-<[* Out of Bound Access to String Table *]>-";   // RETURN
        }
        return d_table[idx];
    }

    /// Return the number of string pointers store in this object.
    size_t count() const
    {
        return d_index;
    }
};

}  // close unnamed namespace

/// Return `23` if the invocation is evaluated at compile time.  If the
/// invocation is not evaluated at compile time or if this cannot be
/// determined return `17`.
BSLS_CONSTEVAL_CONSTEXPR int testFunction()
{
#ifdef BSLS_CONSTEVAL_IS_CONSTANT_EVALUATED_IS_ACTIVE
    return (BSLS_CONSTEVAL_IS_CONSTANT_EVALUATED) ? 23 : 17;
#else
    return 17;
#endif
}

/// Print a diagnostic message to standard output if any of the preprocessor
/// flags of interest are defined, and their value if a value had been set.
/// An "Enter" and "Leave" message is printed unconditionally so there is
/// some report even if all of the flags are undefined.  Note that the macros
/// are organized by thematic section, and the sections are mostly sorted
/// alphanumerically except where comments indicate the reason for a different
/// ordering.
static void printFlags()
{
    StaticStringsTable<32> undefinedMacros("undefinedMacros");

/// Add the specified macro named by `X` to the list of macros to report as
/// not defined.
#define D_MACRO(X) undefinedMacros.pushBack(#X);

/// Print the name of the specified object-like macro named by `X`, and the
/// source it expands to.
#define P_MACRO(X) puts("\t  " #X ":\t" STRINGIFY(X));

    puts("printFlags: Enter");

    puts("\n  printFlags: bsls_consteval Macros");
    puts(  "  ---------------------------------");

#if defined(BSLS_CONSTEVAL_IS_CONSTANT_EVALUATED_IS_ACTIVE)
    P_MACRO(BSLS_CONSTEVAL_IS_CONSTANT_EVALUATED_IS_ACTIVE);
#else
    D_MACRO(BSLS_CONSTEVAL_IS_CONSTANT_EVALUATED_IS_ACTIVE);
#endif

#if defined(BSLS_CONSTEVAL_IS_CONSTANT_EVALUATED)
    P_MACRO(BSLS_CONSTEVAL_IS_CONSTANT_EVALUATED);
#else
    D_MACRO(BSLS_CONSTEVAL_IS_CONSTANT_EVALUATED);
#endif

#if defined(BSLS_CONSTEVAL_CONSTEXPR)
    P_MACRO(BSLS_CONSTEVAL_CONSTEXPR);
#else
    D_MACRO(BSLS_CONSTEVAL_CONSTEXPR);
#endif

#if defined(BSLS_CONSTEVAL_CONSTEXPR_MEMBER)
    P_MACRO(BSLS_CONSTEVAL_CONSTEXPR_MEMBER);
#else
    D_MACRO(BSLS_CONSTEVAL_CONSTEXPR_MEMBER);
#endif

    puts("\n  printFlags: bsls_consteval-referenced Macros");
    puts(  "  --------------------------------------------");

#if defined(BSLS_COMPILERFEATURES_SUPPORT_IS_CONSTANT_EVALUATED)
    P_MACRO(BSLS_COMPILERFEATURES_SUPPORT_IS_CONSTANT_EVALUATED);
#else
    D_MACRO(BSLS_COMPILERFEATURES_SUPPORT_IS_CONSTANT_EVALUATED);
#endif

    puts("\n\n  printFlags: UNDEFINED MACROS:");
    puts(    "  -----------------------------");

    for (size_t i = 0; i != undefinedMacros.count(); ++i) {
        fputs("\t  ", stdout);
        puts(undefinedMacros[i]);
    }

    puts("\n\nprintFlags: Leave");

        // Clean up locally scoped macros
#undef D_MACRO
#undef P_MACRO
}

namespace usage_example
{
// ============================================================================
//                                USAGE EXAMPLE
// ----------------------------------------------------------------------------
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: BSLS_CONSTEVAL_CONSTEXPR `compute` with output
///---------------------------------------------------------
// In this simple example, the macros are used to determine when it is
// permissible to log a message to `stdout`.
// ```

    /// Return `23` if the invocation is evaluated at compile time and that
    /// is detectable, otherwise print a diagnostic message to `stdout` and
    /// return `17`.
    BSLS_CONSTEVAL_CONSTEXPR int compute()
    {
    #ifdef BSLS_CONSTEVAL_IS_CONSTANT_EVALUATED_IS_ACTIVE
        if (BSLS_CONSTEVAL_IS_CONSTANT_EVALUATED) {
            return 23;                                                // RETURN
        }
    #endif
        printf("Computing value\n");
        return 17;                                                    // RETURN
    }
// ```
// Now we define a `test1` function to invoke `compute` in different contexts.
// This function can be evaluated both at runtime and at compile time without
// errors.  Below, this function is evaluated in both cases, and the difference
// in behavior is observed.
// ```

    /// Invoke `compute` in both a const and non-const initialization,
    /// verifying the expected results.
    void test1()
    {
                                        int i = compute();
        BSLS_CONSTEVAL_CONSTEXPR_MEMBER int j = compute();
    #ifdef BSLS_CONSTEVAL_IS_CONSTANT_EVALUATED_IS_ACTIVE
        ASSERT(17 == i);
        ASSERT(23 == j);
    #else
        ASSERT(17 == i);
        ASSERT(17 == j);
    #endif
    }
// ```
// When `17 == i` or `17 == j`, `compute` will write "Computing value/n" to
// `stdout`.  So, this message will be written once or twice depending on
// whether `BSLS_CONSTEVAL_IS_CONSTANT_EVALUATED_IS_ACTIVE` is defined.  The
// variable `j` will always be `const`, and on platforms where the `compute`
// function supports being `constexpr`, i.e. those where it can use
// `BSLS_CONSTEVAL_IS_CONSTANT_EVALUATED` to suppress its use of standard
// output, `j` will also be `constexpr` and its value will be computed at
// compile time.
//
///Example 2: Evolving a `constexpr` function
///------------------------------------------
// Consider the situation where there exist two implementations of the same
// algorithm, one of which satisfies the (stringent and pessimizing)
// requirements needed to be a C++11 `constexpr` function, the other of which
// takes advantage of runtime optimizations (such as hardware acceleration,
// exceptions, or non-`constexpr` third-party libraries) that are not available
// at compile time on any platform:
// ```
namespace usage_example_2a
{
    int runtimeCompute(int input);

    /// Return a complicated computed value based on the specified `input`.
    BSLS_KEYWORD_CONSTEXPR int compiletimeCompute(int input);
}  // close namespace usage_example_2a
// ```
// Assuming these functions were introduced long ago, it is likely they are
// heavily used wherever valid throughout a codebase.  The `compiletimeCompute`
// function is likely used to initialize many `const` and `constexpr`
// variables, but also potentially used in many runtime-only expressions, or as
// part of other `constexpr` functions that are themselves sometimes used at
// runtime.  The `runtimeCompute` function, similarly, is likely used in many
// contexts that could become `constexpr` or be evaluated at compile time, but
// is hindering that due to itself not being `constexpr`.
//
// We can begin to transform `compiletimeCompute` and `runtimeCompute` to both
// have improved performance wherever they might be used by moving their
// implementations to separate functions:
// ```
namespace usage_example_2b
{
    /// Return a complicated computed value based on the specified `input`.
    int runtimeComputeImpl(int input);

    /// Return a complicated computed value based on the specified `input`.
    BSLS_KEYWORD_CONSTEXPR int compiletimeComputeImpl(int input);
// ```
// Then, for `compiletimeCompute` we can provide a new implementation that will
// use the better runtime algorithm when possible, while remaining `constexpr`
// on all of the platforms where it previously was `constexpr` (i.e., without
// changing its declaration):
// ```

    /// Return a complicated computed value based on the specified `input`.
    BSLS_KEYWORD_CONSTEXPR int compiletimeCompute(int input)
    {
    #ifdef BSLS_CONSTEVAL_IS_CONSTANT_EVALUATED_IS_ACTIVE
        if (!BSLS_CONSTEVAL_IS_CONSTANT_EVALUATED) {
            return runtimeComputeImpl(input);                         // RETURN
        }
    #endif  // BSLS_CONSTEVAL_IS_CONSTANT_EVALUATED_IS_ACTIVE
        return compiletimeComputeImpl(input);
    }
// ```
// Now clients using `compiletimeCompute` at runtime, both within and outside
// of other `constexpr` functions,  will get the benefits of an improved
// algorithm without any need for change.
//
// Similarly, the implementation of `runtimeCompute` can be improved to be
// opportunistically `constexpr` by taking advantage of
// `BSLS_CONSTEVAL_CONSTEXPR`, potentially allowing some already existing
// expressions to be compile time evaluated on more modern platforms:
// ```

    /// Return a complicated computed value based on the specified `input`.
    BSLS_CONSTEVAL_CONSTEXPR int runtimeCompute(int input)
    {
    #ifdef BSLS_CONSTEVAL_IS_CONSTANT_EVALUATED_IS_ACTIVE
        if (BSLS_CONSTEVAL_IS_CONSTANT_EVALUATED) {
            return compiletimeComputeImpl(input);                     // RETURN
        }
    #endif  // BSLS_CONSTEVAL_IS_CONSTANT_EVALUATED_IS_ACTIVE
        return runtimeComputeImpl(input);
    }
// ```
// Clients of `runtimeCompute` can continue to use it at runtime with no
// changes, occasionally getting the benefits of the compile-time algorithm.
// When using `runtimeCompute` to initialize a variable, the compile-time
// behavior can be forced by annotating the variable with
// `BSLS_CONSTEVAL_CONSTEXPR_MEMBER`.
//
// With these changes, clients on older platforms can continue to take
// advantage of having optimal algorithms available at both compile time and
// runtime while getting the best available implementation on newer platforms
// that enable the detection of compile-time evaluation.
// ```
    int runtimeComputeImpl(int input)
    {
        int value = input;
        // do the same magic as the original `runtimeCompute`
        return value;
    }

    BSLS_KEYWORD_CONSTEXPR int compiletimeComputeImpl(int input)
    {
        // do the same magic as the original `compiletimeCompute`
        return input;
    }
}  // close namespace usage_example_2b
}  // close namespace usage_example

// ============================================================================
//                            MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char **argv)
{
    int                 test = argc > 1 ? atoi(argv[1]) : 0;
    bool             verbose = argc > 2;
    bool         veryVerbose = argc > 3;
    bool     veryVeryVerbose = argc > 4;

    (void)        veryVerbose;  // unused variable warning
    (void)    veryVeryVerbose;  // unused variable warning

    printf( "TEST %s CASE %d\n", __FILE__, test);

    switch (test) { case 0:
      case 2: {
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
        //    leading comment characters.
        //    (C-1)
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) printf("\nUSAGE EXAMPLE"
                            "\n=============\n");
        usage_example::test1();
        int i = usage_example::usage_example_2b::runtimeCompute(1729);
        BSLS_CONSTEVAL_CONSTEXPR_MEMBER int j =
            usage_example::usage_example_2b::compiletimeCompute(1729);
        (void)i;
        (void)j;
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //
        // Concerns:
        // 1. This test driver builds on all platforms.
        //
        // 2. That `BSLS_CONSTEVAL_IS_CONSTANT_EVALUATED` and
        //    `BSLS_CONSTEVAL_CONSTEXPR` are defined.
        //
        // Plan:
        // 1. Print out flags in veryVeryVerbose mode.
        //
        // 2. Generate a compilation error if either
        //    `BSLS_CONSTEVAL_IS_CONSTANT_EVALUATED` or
        //    `BSLS_CONSTEVAL_CONSTEXPR` is undefined.
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) printf("\nBREATHING TEST"
                            "\n==============\n");

        if (veryVeryVerbose) printFlags();

#ifndef BSLS_CONSTEVAL_IS_CONSTANT_EVALUATED
#error BSLS_CONSTEVAL_IS_CONSTANT_EVALUATED has not been defined
#endif
#ifndef BSLS_CONSTEVAL_CONSTEXPR
#error BSLS_CONSTEVAL_CONSTEXPR has not been defined
#endif

        const int i                    = testFunction();
        BSLS_CONSTEVAL_CONSTEXPR int j = testFunction();
        int k                          = testFunction();

#ifdef BSLS_CONSTEVAL_IS_CONSTANT_EVALUATED_IS_ACTIVE
        ASSERT(23 == i);
        ASSERT(23 == j);
        ASSERT(17 == k);
#else
        ASSERT(17 == i);
        ASSERT(17 == j);
        ASSERT(17 == k);
#endif
      } break;
      default: {
        fprintf( stderr, "WARNING: CASE `%d` NOT FOUND.\n" , test);
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        printFlags();
        fprintf( stderr, "Error, non-zero test status = %d.\n", testStatus );
    }

    return testStatus;
}

// ----------------------------------------------------------------------------
// Copyright 2022 Bloomberg Finance L.P.
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
