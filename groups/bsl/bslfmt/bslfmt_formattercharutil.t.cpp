// bslstl_formattercharutil.t.cpp                                     -*-C++-*-
#include <bslfmt_formattercharutil.h>

#include <bsls_bsltestutil.h>

#include <bslstl_string.h>

#include <stdio.h>    // `printf`
#include <stdlib.h>   // `atoi`
#include <string.h>   // `strlen`, `strcmp`, `memset`
#include <wchar.h>    // `wcscmp`

using namespace BloombergLP;
using namespace bslfmt;


// ============================================================================
//                             TEST PLAN
// ----------------------------------------------------------------------------
//                             Overview
//                             --------
// TBD
// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 2] USAGE EXAMPLE

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
//                       HELPER CLASSES FOR TESTING
// ----------------------------------------------------------------------------

                         // =====================
                         // struct SimpleIterator
                         // =====================

/// This class provides a primitive template iterator that iterates through the
/// sequence of objects of the (template parameter) type 't_TYPE'.
template <class t_TYPE>
class SimpleIterator {
  private:
    // DATA
    t_TYPE *d_value_p;  // address this iterator points to

  public:
    // TYPES
    typedef t_TYPE                   value_type;
    typedef t_TYPE *                 pointer;
    typedef t_TYPE&                  reference;
    typedef ptrdiff_t                difference_type;
    typedef bsl::output_iterator_tag iterator_category;

    // CREATORS

    /// Create an iterator pointing to the specified `value`.
    SimpleIterator(t_TYPE *value = 0)
    : d_value_p(value)
    {
    }

    // MANIPULATORS

    /// Increment to the next element and return a reference providing
    /// modifiable access to this iterator.
    SimpleIterator& operator++()
    {
        ++d_value_p;
        return *this;
    }

    /// Increment to the next element and return an iterator having
    /// pre-increment value of this iterator.
    SimpleIterator operator++(int)
    {
        SimpleIterator temp(d_value_p);
        ++d_value_p;
        return temp;
    }

    // ACCESSORS

    /// Return a reference to the modifiable `t_TYPE` to which this object
    /// refers.
    t_TYPE& operator*() const
    {
        return *d_value_p;
    }
};

// FREE OPERATORS

/// Return 'true' if the specified 'lhs' iterator has the same value as the
/// specified 'rhs' iterator, and 'false' otherwise.  Two iterators have the
/// same value if they refer to the same element.
template <class t_TYPE>
bool operator==(const SimpleIterator<t_TYPE>& lhs,
                const SimpleIterator<t_TYPE>& rhs)
{
    return &*lhs == &*rhs;
}

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------
int main(int argc, char **argv)
{
    const int  test    = argc > 1 ? atoi(argv[1]) : 0;
    const bool verbose = argc > 2;
    // const bool veryVerbose = argc > 3;

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

///Usage
///-----
// This section illustrates intended usage of this component.
//
///Example: Outputting a hexadecimal in upper case
///- - - - - - - - - - - - - - - - - - - - - - - -
// Suppose we need to output hexadecimal number to sink object presented by the
// output iterator (e.g. some character buffer).  Additionally, we are required
// to have the number displayed in uppercase.
// ```
    char         number[] = {'0', 'x', '1', '2', 'c', 'd', '\0'};
    const size_t sourceLength = std::strlen(number);
// ```
// First, we convert the number to uppercase in place and verify the result:
// ```
    bslfmt::FormatterCharUtil<char>::toUpper(number, number + sourceLength);
    const char *expectedUppercaseNumber = "0X12CD";
    ASSERT(0 == std::strcmp(number, expectedUppercaseNumber));
// ```
// Next, we output this uppercase number to the sink, using `outputFromChar`
// function.  Note that `SimpleIterator` in this example is just a primitive
// class that minimally satisfies the requirements of the output iterator. To
// reduce the code size and improve readability, we do not provide its
// implementation here.
// ```
    char charSink[8];
    std::memset(charSink, 0, sizeof(char) * 8);
    SimpleIterator<char> charIt(charSink);

    charIt = bslfmt::FormatterCharUtil<char>::outputFromChar(
                                                         number,
                                                         number + sourceLength,
                                                         charIt);

    ASSERT(&charSink[sourceLength] == &(*charIt));
    ASSERT(0 == std::strcmp(number, charSink));
// ```
// But the main purpose of these functions is to unify the output of values to
// character strings and wide character strings.  All we need to do is just
// change the template parameter:
// ```
    wchar_t wcharSink[8];
    std::memset(wcharSink, 0, sizeof(wchar_t) * 8);

    wchar_t wcharExpected[] = {'0', 'X', '1', '2', 'C', 'D', '\0'};

    SimpleIterator<wchar_t> wcharIt(wcharSink);
    wcharIt = bslfmt::FormatterCharUtil<wchar_t>::outputFromChar(
                                                         number,
                                                         number + sourceLength,
                                                         wcharIt);
    ASSERT(&wcharSink[sourceLength] == &(*wcharIt));
    ASSERT(0 == std::wcscmp(wcharExpected, wcharSink));
// ```

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //   This case exercises (but does not fully test) basic functionality.
        //
        // Concerns:
        //: 1 The class is sufficiently functional to enable comprehensive
        //:   testing in subsequent test cases.
        //
        // Plan:
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose)
            printf("\nBREATHING TEST"
                   "\n==============\n");

        const int  BUFFER_SIZE            = 6;
        const int  NUM_CHARACTERS_TO_COPY = BUFFER_SIZE - 1;

        const char SOURCE[]       = {'a', 'B', 'c', '1', '2', '#'};
        const char UPPER_SOURCE[] = {'A', 'B', 'C', '1', '2', '#'};

        // Testing `char` specialization.
        {
            char sink[BUFFER_SIZE];

            for (int i = 0; i < BUFFER_SIZE; ++i) {
                sink[i] = 0;
            }

            const char *sourceBegin = SOURCE;
            const char *sourceEnd   = SOURCE + NUM_CHARACTERS_TO_COPY;

            // Testing `outputFromChar(char *, char *, iterator)`.

            SimpleIterator<char> out(sink);
            out = FormatterCharUtil<char>::outputFromChar(sourceBegin,
                                                          sourceEnd,
                                                          out);

            ASSERTV(*out, &sink[NUM_CHARACTERS_TO_COPY] == &(*out));
            for (int i = 0; i < NUM_CHARACTERS_TO_COPY; ++i) {
                ASSERTV(SOURCE[i], sink[i], SOURCE[i] == sink[i]);
            }
            ASSERTV(sink[NUM_CHARACTERS_TO_COPY],
                    0 == sink[NUM_CHARACTERS_TO_COPY]);

            // Testing `outputFromChar(char, iterator)`.

            out = FormatterCharUtil<char>::outputFromChar(
                                                SOURCE[NUM_CHARACTERS_TO_COPY],
                                                out);

            ASSERTV(*out, &sink[BUFFER_SIZE] == &(*out));

            ASSERTV(SOURCE[NUM_CHARACTERS_TO_COPY],
                    sink[NUM_CHARACTERS_TO_COPY],
                    SOURCE[NUM_CHARACTERS_TO_COPY] ==
                        sink[NUM_CHARACTERS_TO_COPY]);

            // Testing `toUpper`.

            char       *sinkBegin = sink;
            char       *sinkEnd   = sink + BUFFER_SIZE;

            FormatterCharUtil<char>::toUpper(sinkBegin, sinkEnd);

            for (int i = 0; i < BUFFER_SIZE; ++i) {
                ASSERTV(UPPER_SOURCE[i], sink[i], UPPER_SOURCE[i] == sink[i]);
            }
        }

        // Testing `wchar_t` specialization.
        {
            wchar_t sink[BUFFER_SIZE];

            for (int i = 0; i < BUFFER_SIZE; ++i) {
                sink[i] = 0;
            }

            const char *sourceBegin = SOURCE;
            const char *sourceEnd   = SOURCE + NUM_CHARACTERS_TO_COPY;

            // Testing `outputFromChar(char *, char *, iterator)`.

            SimpleIterator<wchar_t> out(sink);
            out = FormatterCharUtil<wchar_t>::outputFromChar(sourceBegin,
                                                             sourceEnd,
                                                             out);

            ASSERTV(*out, &sink[NUM_CHARACTERS_TO_COPY] == &(*out));
            for (int i = 0; i < NUM_CHARACTERS_TO_COPY; ++i) {
                ASSERTV(SOURCE[i], sink[i], wchar_t(SOURCE[i]) == sink[i]);
            }
            ASSERTV(sink[NUM_CHARACTERS_TO_COPY],
                    0 == sink[NUM_CHARACTERS_TO_COPY]);

            // Testing `outputFromChar(char, iterator)`.

            out = FormatterCharUtil<wchar_t>::outputFromChar(
                                                SOURCE[NUM_CHARACTERS_TO_COPY],
                                                out);

            ASSERTV(*out, &sink[BUFFER_SIZE] == &(*out));

            ASSERTV(SOURCE[NUM_CHARACTERS_TO_COPY],
                    sink[NUM_CHARACTERS_TO_COPY],
                    wchar_t(SOURCE[NUM_CHARACTERS_TO_COPY]) ==
                        sink[NUM_CHARACTERS_TO_COPY]);

            // Testing `toUpper`.

            char  charSink[BUFFER_SIZE];
            char *sinkBegin      = charSink;
            char *sinkEnd        = charSink + BUFFER_SIZE;

            for (int i = 0; i < BUFFER_SIZE; ++i) {
                charSink[i] = SOURCE[i];
            }

            FormatterCharUtil<wchar_t>::toUpper(sinkBegin, sinkEnd);

            for (int i = 0; i < BUFFER_SIZE; ++i) {
                ASSERTV(UPPER_SOURCE[i], charSink[i],
                        UPPER_SOURCE[i] == charSink[i]);
            }
        }
      }; break;
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
// Copyright 2024 Bloomberg Finance L.P.
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
