// bslfmt_formatterbase.t.cpp                                         -*-C++-*-
#include <bslfmt_formatterbase.h>

#include <bslmf_isaccessiblebaseof.h>
#include <bslmf_issame.h>

#include <bsls_bsltestutil.h>

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP20_FORMAT)
#include <format>
#endif

#include <stdio.h>   // `printf`
#include <stdlib.h>  // `atoi`

using namespace BloombergLP;
using namespace bsl;

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

#define ASSERT BSLS_BSLTESTUTIL_ASSERT
#define ASSERTV BSLS_BSLTESTUTIL_ASSERTV

#define LOOP_ASSERT BSLS_BSLTESTUTIL_LOOP_ASSERT
#define LOOP0_ASSERT BSLS_BSLTESTUTIL_LOOP0_ASSERT
#define LOOP1_ASSERT BSLS_BSLTESTUTIL_LOOP1_ASSERT
#define LOOP2_ASSERT BSLS_BSLTESTUTIL_LOOP2_ASSERT
#define LOOP3_ASSERT BSLS_BSLTESTUTIL_LOOP3_ASSERT
#define LOOP4_ASSERT BSLS_BSLTESTUTIL_LOOP4_ASSERT
#define LOOP5_ASSERT BSLS_BSLTESTUTIL_LOOP5_ASSERT
#define LOOP6_ASSERT BSLS_BSLTESTUTIL_LOOP6_ASSERT

#define Q BSLS_BSLTESTUTIL_Q    // Quote identifier literally.
#define P BSLS_BSLTESTUTIL_P    // Print identifier and value.
#define P_ BSLS_BSLTESTUTIL_P_  // P(X) without '\n'.
#define T_ BSLS_BSLTESTUTIL_T_  // Print a tab (w/o newline).
#define L_ BSLS_BSLTESTUTIL_L_  // current Line number

#define RUN_EACH_TYPE BSLTF_TEMPLATETESTFACILITY_RUN_EACH_TYPE

// ============================================================================
//                  NEGATIVE-TEST MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT_SAFE_PASS(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS(EXPR)
#define ASSERT_SAFE_FAIL(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(EXPR)
#define ASSERT_PASS(EXPR) BSLS_ASSERTTEST_ASSERT_PASS(EXPR)
#define ASSERT_FAIL(EXPR) BSLS_ASSERTTEST_ASSERT_FAIL(EXPR)
#define ASSERT_OPT_PASS(EXPR) BSLS_ASSERTTEST_ASSERT_OPT_PASS(EXPR)
#define ASSERT_OPT_FAIL(EXPR) BSLS_ASSERTTEST_ASSERT_OPT_FAIL(EXPR)

#define ASSERT_SAFE_PASS_RAW(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS_RAW(EXPR)
#define ASSERT_SAFE_FAIL_RAW(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL_RAW(EXPR)
#define ASSERT_PASS_RAW(EXPR) BSLS_ASSERTTEST_ASSERT_PASS_RAW(EXPR)
#define ASSERT_FAIL_RAW(EXPR) BSLS_ASSERTTEST_ASSERT_FAIL_RAW(EXPR)
#define ASSERT_OPT_PASS_RAW(EXPR) BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(EXPR)
#define ASSERT_OPT_FAIL_RAW(EXPR) BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(EXPR)

// ============================================================================
//                        GLOBAL CLASSES FOR TESTING
// ----------------------------------------------------------------------------

namespace test_formatters {

                      // ==============================
                      // struct FormatterWithMacroTrait
                      // ==============================

/// This struct provides a test class that simulates a custom formatter with a
/// special trait that prevents this formatter from being promoted to the `std`
/// namespace declared using conditional macro.
struct FormatterWithMacroTrait {
    // TYPES
    BSL_FORMATTER_PREVENT_STD_DELEGATION_TRAIT_CPP20;
    typedef void ExistenceMarker;
};

                      // =================================
                      // struct FormatterWithExplicitTrait
                      // =================================

/// This struct provides a test class that simulates a custom formatter with an
/// explicitly declared trait that prevents this formatter from being promoted
/// to the `std` namespace.
struct FormatterWithExplicitTrait {
    // TYPES
    typedef void Formatter_PreventStdPromotion;
    typedef void ExistenceMarker;
};

                      // ============================
                      // struct FormatterWithoutTrait
                      // ============================

/// This struct provides a test class that simulates a custom formatter with a
/// missing trait preventing this formatter from being promoted to the `std`
/// namespace.
struct FormatterWithoutTrait {
    // TYPES
    typedef void ExistenceMarker;
};

}  // close namespace test_formatters

namespace {

                      // =========================
                      // struct TestTypeMacroTrait
                      // =========================

/// This struct provides a test class that simulates a custom user type to be
/// formatted by the `FormatterWithMacroTrait`.
struct TestTypeMacroTrait {
};

                      // ============================
                      // struct TestTypeExplicitTrait
                      // ============================

/// This struct provides a test class that simulates a custom user type to be
/// formatted by the `FormatterWithExplicitTrait`.
struct TestTypeExplicitTrait {
};

                      // ===========================
                      // struct TestTypeWithoutTrait
                      // ===========================

/// This struct provides a test class that simulates a custom user type to be
/// formatted by the `FormatterWithoutTrait`.
struct TestTypeWithoutTrait {
};

                      // ========================
                      // struct IsStructInherited
                      // ========================

/// This type exists to enable SFINAE-based detection of the presence or
/// absence of the `ExistenceMarker` type allowing us to determine whether the
/// formatter provided in the `std` namespace was inherited from the formatter
/// declared in the `bsl` namespace.  By default, we assume that the type is
/// not inherited.
template <class t_FORMATTER, class = void>
struct IsStructInherited : bsl::false_type {
};

/// This is a partial specialization of the `IsStructInherited` struct for
/// special test types in the `bsl` namespace, that declare special
/// `ExistenceMarker` alias.
template <class t_FORMATTER>
struct IsStructInherited<t_FORMATTER, typename t_FORMATTER::ExistenceMarker>
: bsl::true_type {
};

}  // close unnamed namespace

namespace bsl {

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP20_FORMAT)

/// Partial specialization of the `bsl::formatter` template for the test type
/// `TestTypeMacroTrait`.
template <class t_CHAR>
struct formatter<TestTypeMacroTrait, t_CHAR>
: test_formatters::FormatterWithMacroTrait {
};

/// Partial specialization of the `bsl::formatter` template for the test type
/// `TestTypeWithoutTrait`.
template <class t_CHAR>
struct formatter<TestTypeWithoutTrait, t_CHAR>
: test_formatters::FormatterWithoutTrait {
};

/// Partial specialization of the `bsl::formatter` template for the test type
/// `TestTypeExplicitTrait`.
template <class t_CHAR>
struct formatter<TestTypeExplicitTrait, t_CHAR>
: test_formatters::FormatterWithExplicitTrait {
};

/// Partial specialization of the `bsl::formatter` template for the type `int`.
/// This specialization carries absolutely no useful load and is declared only
/// to ensure that there is no ambiguity when formatter specializations for the
/// same type are declared simultaneously in both the `std` namespace and the
/// `bsl` namespace.
template <class t_CHAR>
struct formatter<int, t_CHAR>
: test_formatters::FormatterWithExplicitTrait {
};

#endif

}  // close namespace bsl


// ============================================================================
//                               USAGE EXAMPLE
// ----------------------------------------------------------------------------

///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Creating custom formatter for user type
/// - - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose we have a custom type representing a date. And we want to output it
// to the stream in different formats depending on the circumstances using
// `bsl::format` function. The following example shows how this can be done.
//
// First, we define our `Date` class:
// ```
    /// This class implements a complex-constrained, value-semantic type for
    /// representing dates.  Each object of this class *always* represents a
    /// *valid* date value in the range `[0001JAN01 .. 9999DEC31]` inclusive.
    class Date {
      private:
        // DATA
        int d_year;   // year
        int d_month;  // month
        int d_day;    // day

      public:
        // CREATORS

        /// Create an object having the value represented by the specified
        /// `year`, `month`, and `day`.
        Date(int year, int month, int day);

        // ACCESSORS

        /// Return the year of this date.
        int year() const;

        /// Return the month of this date.
        int month() const;

        /// Return the day of this date.
        int day() const;
    };
// ```
// Now, we define `formatter` specialization for the `Date` and in particular
// `parse()` and `format()` functions which will be called by `bsl::format`.
// Note that specialization must be defined in the namespace `bsl`.
// ```
    namespace bsl {

    template <class t_CHAR>
    struct formatter<Date, t_CHAR> {
        // MANIPULATORS

        /// Parse the specified `parseContext` and return an iterator, pointing
        /// to the beginning of the format string.
        template <class t_PARSE_CONTEXT>
        BSLS_KEYWORD_CONSTEXPR_CPP20 typename t_PARSE_CONTEXT::iterator parse(
                                                t_PARSE_CONTEXT& parseContext);

        // ACCESSORS

        /// Create string representation of the specified `value`, customized
        /// in accordance with the requested format and the specified
        /// `formatContext`, and copy it to the output that the output iterator
        /// of the `formatContext` points to.
        template <class t_FORMAT_CONTEXT>
        typename t_FORMAT_CONTEXT::iterator format(
                                        Date              value,
                                        t_FORMAT_CONTEXT& formatContext) const;
    };

    }  // close namespace bsl
// ```


                             // ===================
                             // struct DummyContext
                             // ===================

/// This test struct is used to verify the visibility of the
/// `bsl::formatter<Date, t_CHAR>` in the usage example.
struct DummyContext {
    // TYPES
    typedef void *iterator;

    // ACCESSORS
    /// Do nothing and return 0 implicitly cast to the iterator type.
    iterator out()
    {
        return 0;
    }
};

                                 // ----------
                                 // class Date
                                 // ----------

// CREATORS
inline
Date::Date(int year, int month, int day)
: d_year(year)
, d_month(month)
, d_day(day)
{
}

// ACCESSORS
inline
int Date::year() const
{
    return d_year;
}

inline
int Date::month() const
{
    return d_month;
}

/// Return the day of this date.
inline
int Date::day() const
{
    return d_day;
}

                      // -----------------------------
                      // class formatter<Date, t_CHAR>
                      // -----------------------------

// MANIPULATORS
template <class t_CHAR>
template <class t_PARSE_CONTEXT>
BSLS_KEYWORD_CONSTEXPR_CPP20 typename t_PARSE_CONTEXT::iterator
formatter<Date, t_CHAR>::parse(t_PARSE_CONTEXT& context)
{
    // This dummy implementation is used to verify the visibility of the
    // formatter.
    return context.out();
}

// ACCESSORS
template <class t_CHAR>
template <class t_FORMAT_CONTEXT>
typename t_FORMAT_CONTEXT::iterator formatter<Date, t_CHAR>::format(
                                               Date,
                                               t_FORMAT_CONTEXT& context) const
{
    // This dummy implementation is used to verify  the visibility of the
    // formatter.
    return context.out();
}

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------
int main(int argc, char **argv)
{
    const int  test    = argc > 1 ? atoi(argv[1]) : 0;
    const bool verbose = argc > 2;

    printf("TEST %s CASE %d \n", __FILE__, test);

    switch (test) {  case 0:
      case 2: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //
        // Concern:
        //: 1 Demonstrate the functioning of this component.
        //
        // Plan:
        //: 1 Use test contexts to format a single integer.
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) printf("USAGE EXAMPLE\n"
                            "=============\n");

// Unfortunately, due to the position of this component in the class hierarchy,
// a full-fledged example would require duplicating a huge amount of code. A
// full example of a custom formatter implementation can be found in
// the `bslfmt_format` component.

        // Here we have only dummy implementations to verify visibility of the
        // formatter.

        Date                       date(1, 2, 3);
        bsl::formatter<Date, char> formatter;
        DummyContext               context;

        formatter.parse(context);
        formatter.format(date, context);
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //   This case exercises (but does not fully test) basic functionality.
        //
        // Concerns:
        //: 1 `formatter` is declared in the `bsl` namespace.
        //:
        //: 2 Default type for the second template parameter of
        //:  `bsl::formatter` is `char`.
        //:
        //: 3 `bsl::formatter` specialization is promoted to the `std`
        //:   namespace only in absence of the special type declaration.
        //:
        //: 4 Special type declaration prevents compiler complain about
        //:   ambiguity in case of simultaneous declaration of formatter
        //:   specialization for the same type in the `bsl` and `std`
        //:   namespaces.
        //
        // Plan:
        //: 1 Using meta-functions check the presence of the `formatter`
        //:   template in the `bsl` namespace and its template parameter's
        //:   default value.  (C-1..2)
        //:
        //: 2 Using meta-functions check the propagation of the
        //:   `bsl::formatter` specializations to the `std` namespace depending
        //:   on the presence or lack of defined special type.  (C-3..4)
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose)
            printf("\nBREATHING TEST"
                   "\n==============\n");

        // Concerns:
        // - `formatter` is declared in the `bsl` namespace
        // - default type for the second template parameter is `char`

        ASSERT(false == (bsl::is_same<bsl::formatter<char, char>,
                                      bsl::formatter<int,  int > >::value));
        ASSERT(true  == (bsl::is_same<bsl::formatter<int,  char>,
                                      bsl::formatter<int,  char> >::value));
        ASSERT(true  == (bsl::is_same<bsl::formatter<int>,
                                      bsl::formatter<int,  char> >::value));

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP20_FORMAT)

        // Explicit trait definition.
        // Concern:
        // - `bsl::formatter` is **NOT** propagated to the `std` namespace
        //   under any circumstances

        ASSERT(false ==
               (bslfmt::FormatterBase_IsStdAliasingEnabled<
                   bsl::formatter<TestTypeExplicitTrait, char> >::value));
        ASSERT(false ==
               (IsStructInherited<
                   std::formatter<TestTypeExplicitTrait, char> >::value));

        // No trait definition.
        // Concern:
        // - `bsl::formatter` is propagated to the `std` namespace under any
        //   circumstances

        ASSERT(true ==
               (bslfmt::FormatterBase_IsStdAliasingEnabled<
                   bsl::formatter<TestTypeWithoutTrait, char> >::value));
        ASSERT(true ==
               (IsStructInherited<
                   std::formatter<TestTypeWithoutTrait, char> >::value));
        ASSERT(true ==
               (bslmf::IsAccessibleBaseOf<
                   bsl::formatter<TestTypeWithoutTrait, char>,
                   std::formatter<TestTypeWithoutTrait, char> >::value));

        // Trait is added to the formatter as a macro.
        // Concern:
        // - `bsl::formatter` is propagated to the `std` namespace depending on
        //   the version of the standard

#if BSLS_COMPILERFEATURES_CPLUSPLUS >= 202002L
        ASSERT(false ==
               (bslfmt::FormatterBase_IsStdAliasingEnabled<
                   bsl::formatter<TestTypeMacroTrait, char> >::value));
        ASSERT(false ==
               (IsStructInherited<
                   std::formatter<TestTypeMacroTrait, char> >::value));
#else
        ASSERT(true == (bslfmt::FormatterBase_IsStdAliasingEnabled<
                           bsl::formatter<TestTypeMacroTrait, char> >::value));
        ASSERT(true == (IsStructInherited<
                           std::formatter<TestTypeMacroTrait, char> >::value));
        ASSERT(true == (bslmf::IsAccessibleBaseOf<
                           bsl::formatter<TestTypeMacroTrait, char>,
                           std::formatter<TestTypeMacroTrait, char> >::value));
#endif

        // Formatters are defined in both `std` and `bsl` namespaces.
        // Concern:
        // - `bsl::formatter` is **NOT** propagated to the `std` namespace, but
        //   the standard implementation is used for formatting.

        ASSERT(false == (bslfmt::FormatterBase_IsStdAliasingEnabled<
                            bsl::formatter<int, char> >::value));
        ASSERT(false ==
               (IsStructInherited<std::formatter<int, char> >::value));
        ASSERT(false ==
               (bslmf::IsAccessibleBaseOf<bsl::formatter<int, char>,
                                          std::formatter<int, char> >::value));

        bsl::formatter<int, char> bslIntFormatter;
        (void) bslIntFormatter;  // suppress compiler warning

        std::formatter<int, char> stdIntFormatter;
        (void) stdIntFormatter;  // suppress compiler warning
#endif

      } break;
      default: {
        printf("WARNING: CASE `%d' NOT FOUND.\n", test);
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        printf("Error, non-zero test status = %d .\n", testStatus);
    }
    return testStatus;
}

// ----------------------------------------------------------------------------
// Copyright 2023 Bloomberg Finance L.P.
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
