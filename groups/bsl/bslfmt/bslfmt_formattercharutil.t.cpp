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

                         // ====================
                         // class OutputIterator
                         // ====================

/// This class provides a primitive template iterator that iterates through the
/// sequence of objects of the (template parameter) type 't_TYPE'.
template <class t_CHAR_TYPE>
class OutputIterator {
  private:
    // DATA
    t_CHAR_TYPE *d_value_p;  // address this iterator points to
    t_CHAR_TYPE *d_end_p;    // maximum value this iterator can reach

  public:
    // TYPES
    typedef t_CHAR_TYPE              value_type;
    typedef t_CHAR_TYPE *            pointer;
    typedef t_CHAR_TYPE&             reference;
    typedef ptrdiff_t                difference_type;
    typedef bsl::output_iterator_tag iterator_category;

    // CREATORS

    /// Create an iterator pointing to the specified `value` and having the
    /// specified `end` as the past-the-end element in the sequence to iterate
    /// through.
    OutputIterator(t_CHAR_TYPE *value, t_CHAR_TYPE *end)
    : d_value_p(value)
    , d_end_p(end)
    {
        BSLS_ASSERT(d_end_p > d_value_p);
    }

    /// Create an iterator having the specified `array` as a sequence to
    /// iterate through.
    template<size_t t_SIZE>
    OutputIterator(t_CHAR_TYPE (&array)[t_SIZE])
    : d_value_p(array)
    , d_end_p(array + sizeof(array) / sizeof(*array))
    {
    }

    // MANIPULATORS

    /// Increment to the next element and return a reference providing
    /// modifiable access to this iterator.  The behavior is undefined if
    /// iterator points to the past-the-end element of the sequence to iterate
    /// through.
    OutputIterator& operator++()
    {
        BSLS_ASSERT(d_end_p != d_value_p);
        ++d_value_p;
        return *this;
    }

    /// Increment to the next element and return an iterator having
    /// pre-increment value of this iterator.  The behavior is undefined if
    /// iterator points to the past-the-end element of the sequence to iterate
    /// through.
    OutputIterator operator++(int)
    {
        BSLS_ASSERT(d_end_p != d_value_p);
        OutputIterator temp(d_value_p, d_end_p);
        ++d_value_p;
        return temp;
    }

    // ACCESSORS

    /// Return a reference to the modifiable `t_CHAR_TYPE` to which this object
    /// refers.  The behavior is undefined if iterator points to the
    /// past-the-end element of the sequence to iterate through.
    reference operator*() const
    {
        BSLS_ASSERT(d_end_p != d_value_p);
        return *d_value_p;
    }

    /// Return a pointer to the non-modifiable `t_CHAR_TYPE` to which this
    /// object refers.  The behavior is undefined if iterator points to the
    /// past-the-end element of the sequence to iterate through.
    const t_CHAR_TYPE *ptr() const
    {
        BSLS_ASSERT(d_end_p != d_value_p);
        return d_value_p;
    }

    // HIDDEN FRIENDS

    /// Return 'true' if the specified 'lhs' iterator has the same value as the
    /// specified 'rhs' iterator, and 'false' otherwise.  Two iterators have
    /// the same value if they refer the same element and have the same
    /// past-the-end element.
    friend bool operator==(const OutputIterator& lhs,
                           const OutputIterator& rhs)
    {
        return (lhs.d_value_p == rhs.d_value_p && lhs.d_end_p == rhs.d_end_p);
    }
};

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

        if (verbose) printf("\nUSAGE EXAMPLE"
                            "\n=============\n");

///Usage
///-----
// This section illustrates intended use of this component.
//
///Example: Outputting a hexadecimal in upper case
///- - - - - - - - - - - - - - - - - - - - - - - -
// Suppose we need to output hexadecimal number to some object presented by the
// output iterator (e.g. some character buffer).  Additionally, we are required
// to have the number displayed in uppercase.
// ```
    char         number[]     = "0x12cd";
    const size_t sourceLength = sizeof(number) - 1;
// ```
// First, we convert the number to uppercase in place and verify the result:
// ```
    bslfmt::FormatterCharUtil<char>::toUpper(number, number + sourceLength);
    const char *expectedUppercaseNumber = "0X12CD";
    ASSERT(0 == std::strcmp(number, expectedUppercaseNumber));
// ```
// Next, we output this uppercase number to the destination, using
// `outputFromChar` function.  Note that `OutputIterator` in this example is
// just a primitive class that minimally satisfies the requirements of the
// output iterator. To reduce the code size and improve readability, we do not
// provide its implementation here.
// ```
    char destination[8];
    std::memset(destination, 0, sizeof(destination));
    OutputIterator<char> charIt(destination);

    charIt = bslfmt::FormatterCharUtil<char>::outputFromChar(
                                                         number,
                                                         number + sourceLength,
                                                         charIt);

    ASSERT(destination + sourceLength == charIt.ptr());
    ASSERT(0 == std::strcmp(number, destination));
// ```
// Finally we demonstrate the main purpose of these functions - to unify the
// output of values to character strings and wide character strings.  All we
// need to do is just change the template parameter:
// ```
    wchar_t wDestination[8];
    std::memset(wDestination, 0, sizeof(wchar_t) * 8);

    wchar_t wcharExpected[] = L"0X12CD";

    OutputIterator<wchar_t> wcharIt(wDestination);
    wcharIt = bslfmt::FormatterCharUtil<wchar_t>::outputFromChar(
                                                         number,
                                                         number + sourceLength,
                                                         wcharIt);
    ASSERT(wDestination + sourceLength == wcharIt.ptr());
    ASSERT(0 == std::wcscmp(wcharExpected, wDestination));
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
        //: 1 Create source `char` array containing some data.
        //:
        //: 2 For each utility specialization (`char` and `wchar_t`):
        //:
        //:   2.1 Create sink array and set it to some value different from the
        //:       value of the source array.
        //:
        //:   2.2 Output some part of the source array to the sink, using
        //:       sequence overload of the `outputFromChar` function.  Verify
        //:       the return value and the value of the sink array.
        //:
        //:   2.3 Output one character from the source array to the sink, using
        //:       single character overload of the `outputFromChar` function.
        //:       Verify the return value and the value of the sink array.
        //:
        //:   2.4 Modify the sink array using `toUpper` function and verify the
        //:       result.
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) printf("\nBREATHING TEST"
                            "\n==============\n");

        const char SOURCE[]       = "aBc12#";
        const char UPPER_SOURCE[] = "ABC12#";

        // We are going to perform two output operations.  The first for a
        // sequence of characters, the second for a single character.  However,
        // we don't want to get into an undefined behavior situation for our
        // output iterator.  So we leave one unchanged character in the sink
        // array.

        const size_t BUFFER_SIZE              = sizeof SOURCE / sizeof *SOURCE;
        const size_t NUM_CHARACTERS_TO_OUTPUT = BUFFER_SIZE - 2;

        // Testing `char` specialization.
        {
            char sink[BUFFER_SIZE];
            std::memset(sink, 0, sizeof(sink));

            const char *sourceBegin = SOURCE;
            const char *sourceEnd   = SOURCE + NUM_CHARACTERS_TO_OUTPUT;

            // Testing sequence overload.

            OutputIterator<char> out(sink);
            out = FormatterCharUtil<char>::outputFromChar(sourceBegin,
                                                          sourceEnd,
                                                          out);

            ASSERTV(sink + NUM_CHARACTERS_TO_OUTPUT == out.ptr());
            for (size_t i = 0; i < NUM_CHARACTERS_TO_OUTPUT; ++i) {
                ASSERTV(i, SOURCE[i], sink[i], SOURCE[i] == sink[i]);
            }

            // Verify that the rest of the output array remains unaffected.

            ASSERTV(sink[NUM_CHARACTERS_TO_OUTPUT],
                    0 == sink[NUM_CHARACTERS_TO_OUTPUT]);

            // Testing single character overload.

            out = FormatterCharUtil<char>::outputFromChar(
                                              SOURCE[NUM_CHARACTERS_TO_OUTPUT],
                                              out);

            ASSERTV(sink + NUM_CHARACTERS_TO_OUTPUT + 1,   out.ptr(),
                    sink + NUM_CHARACTERS_TO_OUTPUT + 1 == out.ptr());

            ASSERTV(SOURCE[NUM_CHARACTERS_TO_OUTPUT],
                    sink[NUM_CHARACTERS_TO_OUTPUT],
                    SOURCE[NUM_CHARACTERS_TO_OUTPUT] ==
                                               sink[NUM_CHARACTERS_TO_OUTPUT]);

            // Testing `toUpper`.

            char *sinkBegin = sink;
            char *sinkEnd   = sink + BUFFER_SIZE;

            FormatterCharUtil<char>::toUpper(sinkBegin, sinkEnd);

            for (size_t i = 0; i < BUFFER_SIZE; ++i) {
                ASSERTV(i, UPPER_SOURCE[i], sink[i],
                        UPPER_SOURCE[i] == sink[i]);
            }
        }

        // Testing `wchar_t` specialization.
        {
            wchar_t sink[BUFFER_SIZE];
            std::memset(sink, 0, sizeof(sink));

            const char *sourceBegin = SOURCE;
            const char *sourceEnd   = SOURCE + NUM_CHARACTERS_TO_OUTPUT;

            // Testing sequence overload.

            OutputIterator<wchar_t> out(sink);
            out = FormatterCharUtil<wchar_t>::outputFromChar(sourceBegin,
                                                             sourceEnd,
                                                             out);

            ASSERTV(sink + NUM_CHARACTERS_TO_OUTPUT == out.ptr());
            for (size_t i = 0; i < NUM_CHARACTERS_TO_OUTPUT; ++i) {
                ASSERTV(i, SOURCE[i], sink[i], wchar_t(SOURCE[i]) == sink[i]);
            }

            // Verify that the rest of the output array remains unaffected.

            ASSERTV(sink[NUM_CHARACTERS_TO_OUTPUT],
                    0 == sink[NUM_CHARACTERS_TO_OUTPUT]);

            // Testing single character overload.

            out = FormatterCharUtil<wchar_t>::outputFromChar(
                                              SOURCE[NUM_CHARACTERS_TO_OUTPUT],
                                              out);

            ASSERTV(sink + NUM_CHARACTERS_TO_OUTPUT + 1,   out.ptr(),
                    sink + NUM_CHARACTERS_TO_OUTPUT + 1 == out.ptr());

            ASSERTV(SOURCE[NUM_CHARACTERS_TO_OUTPUT],
                    sink[NUM_CHARACTERS_TO_OUTPUT],
                    wchar_t(SOURCE[NUM_CHARACTERS_TO_OUTPUT]) ==
                                               sink[NUM_CHARACTERS_TO_OUTPUT]);
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
