// bslstl_formattercharutil.t.cpp                                     -*-C++-*-
#include <bslfmt_formattercharutil.h>

#include <bsls_bsltestutil.h>

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
// The component under test implements a namespace for utility functions that
// convert characters (e.g. from `char` to `wchar_t` or lowercase characters to
// uppercase).  This test driver tests each implemented utility function
// independently.
// --------------------------------------------------------------------
// CLASS METHODS
// [ 3] t_ITER outputFromChar(const char *b, const char *e, t_ITER o);
// [ 3] t_ITER outputFromChar(const char value, t_ITER out);
// [ 2] void toUpper(char *begin, const char *end);
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 4] USAGE EXAMPLE

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
//                          GLOBAL TEST DATA
// ----------------------------------------------------------------------------

/// Data structure containing printable `char` values.
static const struct {
    int            d_line;        // source line number
    const char    *d_charStr_p;   // original string
    const char    *d_upperStr_p;  // expected uppercase result
    const wchar_t *d_wcharStr_p;  // expected `wchar_t` output
    size_t         d_length;      // string length
} DEFAULT_DATA[] = {
    //LINE  CHAR     UPPER    WCHAR     LENGTH
    //----  ------   ------   ------    ------
    // 0 characters
    { L_,   "",      "",      L"",      0      },

    // 1 character
    { L_,   "a",     "A",     L"a",     1      },
    { L_,   "0",     "0",     L"0",     1      },
    { L_,   " ",     " ",     L" ",     1      },

    // 2 characters
    { L_,   "az",    "AZ",    L"az",    2      },
    { L_,   "aZ",    "AZ",    L"aZ",    2      },
    { L_,   "Az",    "AZ",    L"Az",    2      },
    { L_,   "AZ",    "AZ",    L"AZ",    2      },

    { L_,   "by",    "BY",    L"by",    2      },
    { L_,   "b9",    "B9",    L"b9",    2      },
    { L_,   "0y",    "0Y",    L"0y",    2      },
    { L_,   "09",    "09",    L"09",    2      },

    { L_,   "cx",    "CX",    L"cx",    2      },
    { L_,   "c~",    "C~",    L"c~",    2      },
    { L_,   " x",    " X",    L" x",    2      },
    { L_,   " ~",    " ~",    L" ~",    2      },

    // 3 characters
    { L_,   "dmw",   "DMW",   L"dmw",   3      },
    { L_,   "dmW",   "DMW",   L"dmW",   3      },
    { L_,   "dMw",   "DMW",   L"dMw",   3      },
    { L_,   "dMW",   "DMW",   L"dMW",   3      },
    { L_,   "Dmw",   "DMW",   L"Dmw",   3      },
    { L_,   "DmW",   "DMW",   L"DmW",   3      },
    { L_,   "DMw",   "DMW",   L"DMw",   3      },
    { L_,   "DMW",   "DMW",   L"DMW",   3      },

    { L_,   "elv",   "ELV",   L"elv",   3      },
    { L_,   "el8",   "EL8",   L"el8",   3      },
    { L_,   "e5v",   "E5V",   L"e5v",   3      },
    { L_,   "e58",   "E58",   L"e58",   3      },
    { L_,   "1lv",   "1LV",   L"1lv",   3      },
    { L_,   "1l8",   "1L8",   L"1l8",   3      },
    { L_,   "15v",   "15V",   L"15v",   3      },
    { L_,   "158",   "158",   L"158",   3      },

    { L_,   "fnu",   "FNU",   L"fnu",   3      },
    { L_,   "fn}",   "FN}",   L"fn}",   3      },
    { L_,   "f?u",   "F?U",   L"f?u",   3      },
    { L_,   "f?}",   "F?}",   L"f?}",   3      },
    { L_,   "!nu",   "!NU",   L"!nu",   3      },
    { L_,   "!n}",   "!N}",   L"!n}",   3      },
    { L_,   "!?u",   "!?U",   L"!?u",   3      },
    { L_,   "!?}",   "!?}",   L"!?}",   3      },
};

const size_t NUM_DEFAULT_DATA = sizeof DEFAULT_DATA / sizeof *DEFAULT_DATA;

/// Data structure containing non-printable `char` values (including zero and
/// negative values).
static const struct {
    int           d_line;         // source line number
    const char    d_charStr[8];   // enumerator value
    const char    d_upperStr[8];  // expected result
    const wchar_t d_wcharStr[8];  // expected result
    size_t        d_length;       // expected result
} SPECIAL_DATA[] = {
    //LINE  CHAR                UPPER               WCHAR               LEN
    //----  -----------------   -----------------   -----------------   ---
    // 1 character
    { L_,   {  0            },  {  0            },  {  0            },  1   },
    { L_,   { -1            },  { -1            },  {  255          },  1   },
    { L_,   {  1            },  {  1            },  {  1            },  1   },
    { L_,   {  31           },  {  31           },  {  31           },  1   },
    { L_,   {  CHAR_MIN     },  { CHAR_MIN      },  {  128          },  1   },
    { L_,   {  CHAR_MAX     },  { CHAR_MAX      },  {  CHAR_MAX     },  1   },

    // 2 characters
    { L_,   {  0,   0       },  {  0,   0       },  {  0,   0       },  2   },
    { L_,   {  0,  'a'      },  {  0,  'A'      },  {  0,  'a'      },  2   },
    { L_,   { 'b',  0       },  { 'B',  0       },  { 'b',  0       },  2   },
    { L_,   { 'b', 'a'      },  { 'B', 'A'      },  { 'b', 'a'      },  2   },

    { L_,   { -1,  -2       },  { -1,  -2       },  { 255, 254      },  2   },
    { L_,   { -1,  'x'      },  { -1,  'X'      },  { 255, 'x'      },  2   },
    { L_,   { 'y', -2       },  { 'Y', -2       },  { 'y', 254      },  2   },

    { L_,   {  1,   2       },  {  1,   2       },  {  1,   2       },  2   },
    { L_,   {  1,  'k'      },  {  1,  'K'      },  {  1,  'k'      },  2   },
    { L_,   { 'l',  2       },  { 'L',  2       },  { 'l',  2       },  2   },

    // 3 characters
    { L_,   {  0,   0,   0  },  {  0,   0,   0  },  {  0,   0,   0  },  3   },
    { L_,   {  0,   0,  'a' },  {  0,   0,  'A' },  {  0,   0,  'a' },  3   },
    { L_,   {  0,  'b',  0  },  {  0,  'B',  0  },  {  0,  'b',  0  },  3   },
    { L_,   {  0,  'b', 'a' },  {  0,  'B', 'A' },  {  0,  'b', 'a' },  3   },
    { L_,   { 'c',  0,   0  },  { 'C',  0,   0  },  { 'c',  0,   0  },  3   },
    { L_,   { 'c',  0,  'a' },  { 'C',  0,  'A' },  { 'c',  0,  'a' },  3   },
    { L_,   { 'c', 'b',  0  },  { 'C', 'B',  0  },  { 'c', 'b',  0  },  3   },
    { L_,   { 'c', 'b', 'a' },  { 'C', 'B', 'A' },  { 'c', 'b', 'a' },  3   },

    { L_,   { -1,  -2,  -3  },  { -1,  -2,  -3  },  { 255, 254, 253 },  3   },
    { L_,   { -1,  -2,  'x' },  { -1,  -2,  'X' },  { 255, 254, 'x' },  3   },
    { L_,   { -1,  'y', -3  },  { -1,  'Y', -3  },  { 255, 'y', 253 },  3   },
    { L_,   { -1,  'y', 'x' },  { -1,  'Y', 'X' },  { 255, 'y', 'x' },  3   },
    { L_,   { 'z', -2,  -3  },  { 'Z', -2,  -3  },  { 'z', 254, 253 },  3   },
    { L_,   { 'z', -2,  'x' },  { 'Z', -2,  'X' },  { 'z', 254, 'x' },  3   },
    { L_,   { 'z', 'y', -3  },  { 'Z', 'Y', -3  },  { 'z', 'y', 253 },  3   },
    { L_,   { 'z', 'y', 'x' },  { 'Z', 'Y', 'X' },  { 'z', 'y', 'x' },  3   },

    { L_,   {  1,   2,   3  },  {  1,   2,   3  },  {  1,   2,   3  },  3   },
    { L_,   {  1,   2,  'k' },  {  1,   2,  'K' },  {  1,   2,  'k' },  3   },
    { L_,   {  1,  'l',  3  },  {  1,  'L',  3  },  {  1,  'l',  3  },  3   },
    { L_,   {  1,  'l', 'k' },  {  1,  'L', 'K' },  {  1,  'l', 'k' },  3   },
    { L_,   { 'm',  2,   3  },  { 'M',  2,   3  },  { 'm',  2,   3  },  3   },
    { L_,   { 'm',  2,  'k' },  { 'M',  2,  'K' },  { 'm',  2,  'k' },  3   },
    { L_,   { 'm', 'l',  3  },  { 'M', 'L',  3  },  { 'm', 'l',  3  },  3   },
    { L_,   { 'm', 'l', 'k' },  { 'M', 'L', 'K' },  { 'm', 'l', 'k' },  3   },
};

const size_t NUM_SPECIAL_DATA = sizeof SPECIAL_DATA / sizeof *SPECIAL_DATA;

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

    /// Set the  specified `array` as a sequence to iterate through.
    template<size_t t_SIZE>
    void reset(t_CHAR_TYPE (&array)[t_SIZE])
    {
        d_value_p = array;
        d_end_p = array + sizeof(array) / sizeof(*array);
    }

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
    const int  test        = argc > 1 ? atoi(argv[1]) : 0;
    const bool verbose     = argc > 2;
    const bool veryVerbose = argc > 3;

    printf("TEST %s CASE %d \n", __FILE__, test);

    switch (test) {  case 0:
      case 5: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //
        // Concern:
        //: 1 Demonstrate the functioning of this component.
        //
        // Plan:
        //: 1 Using `toUpper` function modify some buffer, containing character
        //:   sequence and output it to the different character array using
        //:   `outputFromChar`.
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
///Example: Outputting A Hexadecimal In Upper Case
///- - - - - - - - - - - - - - - - - - - - - - - -
// Suppose we need to output a hexadecimal number to some object (e.g. some
// character buffer) represented by the output iterator.  Additionally, we are
// required to have the number displayed in uppercase.
// ```
    char         number[]     = "0x12cd";
    const size_t sourceLength = sizeof(number) - 1;
// ```
// First, we convert the number to uppercase in place and verify the result:
// ```
    bslfmt::FormatterCharUtil<char>::toUpper(number, number + sourceLength);
    const char *expectedUppercaseNumber = "0X12CD";
    ASSERT(0 == strcmp(number, expectedUppercaseNumber));
// ```
// Next, we output this uppercase number to the destination, using
// `outputFromChar` function.  `OutputIterator` in this example is just a
// primitive class that minimally satisfies the requirements of the output
// iterator.
// ```
    char destination[8];
    memset(destination, 0, sizeof(destination));
    OutputIterator<char> charIt(destination);

    charIt = bslfmt::FormatterCharUtil<char>::outputFromChar(
                                                         number,
                                                         number + sourceLength,
                                                         charIt);

    ASSERT(destination + sourceLength == charIt.ptr());
    ASSERT(0 == strcmp(number, destination));
// ```
// Finally, we demonstrate the main purpose of these functions - to unify the
// output of values to character strings and wide character strings.  All we
// need to do is just change the template parameter:
// ```
    wchar_t wDestination[8];
    memset(wDestination, 0, sizeof(wchar_t) * 8);

    wchar_t wcharExpected[] = L"0X12CD";

    OutputIterator<wchar_t> wcharIt(wDestination);
    wcharIt = bslfmt::FormatterCharUtil<wchar_t>::outputFromChar(
                                                         number,
                                                         number + sourceLength,
                                                         wcharIt);
    ASSERT(wDestination + sourceLength == wcharIt.ptr());
    ASSERT(0 == wcscmp(wcharExpected, wDestination));
// ```
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING `outputFromChar`
        //
        // Concern:
        //: 1 All `outputFromChar` specializations and overloads successfully
        //:   outputs any `char` value (including zero, negative and control
        //:   ones).
        //:
        //: 2 All `outputFromChar` specializations and overloads return a copy
        //:   of the iterator passed to the function, shifted by the number of
        //:   characters output.
        //
        // Plan:
        //: 1 Using the table-driven technique, specify a set of distinct
        //:   strings (one per row) consisting of various characters.
        //:
        //: 2 For each row `R` in the table of P-1:
        //:
        //:   1 Output string value from `R` to the `char` buffer using both
        //:     `outputFromChar` overloads of the appropriate
        //:     `FormatterCharUtil` specialization.
        //:
        //:   2 Verify the value of the returned iterator and compare the
        //:     content of the output buffer with the expected predefined
        //:     result.
        //:
        //:   3 Output string value from `R` to the `wchar_t` buffer using both
        //:     `outputFromChar` overloads of the appropriate
        //:     `FormatterCharUtil` specialization.
        //:
        //:   4 Verify the value of the returned iterator and compare the
        //:     content of the output buffer with the expected predefined
        //:     result.  (C-1,2)
        //
        // Testing:
        //   t_ITER outputFromChar(const char *b, const char *e, t_ITER o);
        //   t_ITER outputFromChar(const char value, t_ITER out);
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING `outputFromChar`"
                            "\n========================\n");

        for (size_t i = 0; i < NUM_DEFAULT_DATA; ++i) {
            const int      LINE              = DEFAULT_DATA[i].d_line;
            const char    *CHAR_SOURCE_BEGIN = DEFAULT_DATA[i].d_charStr_p;
            const wchar_t *WCHAR_EXPECTED    = DEFAULT_DATA[i].d_wcharStr_p;
            const size_t   LENGTH            = DEFAULT_DATA[i].d_length;
            const char    *CHAR_SOURCE_END   = CHAR_SOURCE_BEGIN + LENGTH;

            if (veryVerbose) { T_ T_ P_(LINE) P(CHAR_SOURCE_BEGIN) }

            // Testing sequence overload for `char`

            char charDestination[8];
            memset(charDestination, 0, sizeof(charDestination));

            OutputIterator<char> charOut(charDestination);
            charOut = FormatterCharUtil<char>::outputFromChar(
                                                             CHAR_SOURCE_BEGIN,
                                                             CHAR_SOURCE_END,
                                                             charOut);

            ASSERTV(charDestination + LENGTH == charOut.ptr());
            ASSERTV(LINE, CHAR_SOURCE_BEGIN, charDestination,
                    0 == strcmp(CHAR_SOURCE_BEGIN, charDestination));

            // Testing one character overload for `char`

            memset(charDestination, 0, sizeof(charDestination));
            charOut.reset(charDestination);
            for (size_t j = 0; j < LENGTH; ++j)
            {
                const char CHARACTER = CHAR_SOURCE_BEGIN[j];
                charOut = FormatterCharUtil<char>::outputFromChar(CHARACTER,
                                                                  charOut);

                ASSERTV(charDestination + j + 1 == charOut.ptr());
                ASSERTV(LINE, CHAR_SOURCE_BEGIN[j], charDestination[j],
                        CHAR_SOURCE_BEGIN[j] == charDestination[j]);

            }

            // Testing sequence overload for `wchar_t`

            wchar_t wcharDestination[8];
            memset(wcharDestination, 0, sizeof(wcharDestination));

            OutputIterator<wchar_t> wcharOut(wcharDestination);
            wcharOut = FormatterCharUtil<wchar_t>::outputFromChar(
                                                             CHAR_SOURCE_BEGIN,
                                                             CHAR_SOURCE_END,
                                                             wcharOut);

            ASSERTV(wcharDestination + LENGTH == wcharOut.ptr());
            ASSERTV(LINE, CHAR_SOURCE_BEGIN, wcharDestination,
                    0 == wcscmp(WCHAR_EXPECTED, wcharDestination));

            // Testing one character overload for `wchar_t`

            memset(wcharDestination, 0, sizeof(wcharDestination));
            wcharOut.reset(wcharDestination);

            for (size_t j = 0; j < LENGTH; ++j)
            {
                const char CHARACTER = CHAR_SOURCE_BEGIN[j];
                wcharOut = FormatterCharUtil<wchar_t>::outputFromChar(
                                                                     CHARACTER,
                                                                     wcharOut);

                ASSERTV(wcharDestination + j + 1 == wcharOut.ptr());
                ASSERTV(LINE, WCHAR_EXPECTED[j], wcharDestination[j],
                        WCHAR_EXPECTED[j] == wcharDestination[j]);
            }

        }

        for (size_t i = 0; i < NUM_SPECIAL_DATA; ++i) {
            const int      LINE              = SPECIAL_DATA[i].d_line;
            const char    *CHAR_SOURCE_BEGIN = SPECIAL_DATA[i].d_charStr;
            const wchar_t *WCHAR_EXPECTED    = SPECIAL_DATA[i].d_wcharStr;
            const size_t   LENGTH            = SPECIAL_DATA[i].d_length;
            const char    *CHAR_SOURCE_END   = CHAR_SOURCE_BEGIN + LENGTH;

            if (veryVerbose) { T_ T_ P_(LINE) P(CHAR_SOURCE_BEGIN) }

            // Testing sequence overload for `char`

            char charDestination[8];
            memset(charDestination, 0, sizeof(charDestination));

            OutputIterator<char> charOut(charDestination);
            charOut = FormatterCharUtil<char>::outputFromChar(
                                                             CHAR_SOURCE_BEGIN,
                                                             CHAR_SOURCE_END,
                                                             charOut);

            ASSERTV(charDestination + LENGTH == charOut.ptr());
            ASSERTV(LINE, CHAR_SOURCE_BEGIN, charDestination,
                    0 == strncmp(CHAR_SOURCE_BEGIN, charDestination, LENGTH));

            // Testing one character overload for `char`

            memset(charDestination, 0, sizeof(charDestination));
            charOut.reset(charDestination);
            for (size_t j = 0; j < LENGTH; ++j)
            {
                const char CHARACTER = CHAR_SOURCE_BEGIN[j];
                charOut = FormatterCharUtil<char>::outputFromChar(CHARACTER,
                                                                  charOut);

                ASSERTV(charDestination + j + 1 == charOut.ptr());
                ASSERTV(LINE, CHAR_SOURCE_BEGIN[j], charDestination[j],
                        CHAR_SOURCE_BEGIN[j] == charDestination[j]);

            }

            // Testing sequence overload for `wchar_t`

            wchar_t wcharDestination[8];
            memset(wcharDestination, 0, sizeof(wcharDestination));

            OutputIterator<wchar_t> wcharOut(wcharDestination);
            wcharOut = FormatterCharUtil<wchar_t>::outputFromChar(
                                                             CHAR_SOURCE_BEGIN,
                                                             CHAR_SOURCE_END,
                                                             wcharOut);

            ASSERTV(wcharDestination + LENGTH == wcharOut.ptr());
            ASSERTV(LINE, CHAR_SOURCE_BEGIN, wcharDestination,
                    0 == wcsncmp(WCHAR_EXPECTED, wcharDestination, LENGTH));

            // Testing one character overload for `wchar_t`

            memset(wcharDestination, 0, sizeof(wcharDestination));
            wcharOut.reset(wcharDestination);

            for (size_t j = 0; j < LENGTH; ++j)
            {
                const char CHARACTER = CHAR_SOURCE_BEGIN[j];
                wcharOut = FormatterCharUtil<wchar_t>::outputFromChar(
                                                                     CHARACTER,
                                                                     wcharOut);

                ASSERTV(wcharDestination + j + 1 == wcharOut.ptr());
                ASSERTV(LINE, WCHAR_EXPECTED[j], wcharDestination[j],
                        WCHAR_EXPECTED[j] == wcharDestination[j]);
            }
        }
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING `toUpper`
        //
        // Concern:
        //: 1 `toUpper` capitalizes any lowercase alphabet character.
        //:
        //: 2 All non-alphabet characters remain unaffected.
        //:
        //: 3 Conversion occurs in-place.
        //
        // Plan:
        //: 1 Using the table-driven technique, specify a set of distinct
        //:   strings (one per row) consisting of various characters (alphabet
        //:   and non-alphabet) of both type cases (lowercase and uppercase).
        //:
        //: 2 For each row `R` in the table of P-1:
        //:
        //:   1 Copy string value from `R` to the `char` buffer.
        //:
        //:   2 Modify this buffer using `toUpper` function.
        //:
        //:   3 Compare the content of the buffer with the expected predefined
        //:     result.  (C-1..3)
        //
        // Testing:
        //   void toUpper(char *begin, const char *end);
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING `toUpper`"
                            "\n=================\n");

        char buffer[8];

        for (size_t i = 0; i < NUM_DEFAULT_DATA; ++i) {
            const int     LINE     = DEFAULT_DATA[i].d_line;
            const char   *ORIGINAL = DEFAULT_DATA[i].d_charStr_p;
            const char   *EXPECTED = DEFAULT_DATA[i].d_upperStr_p;
            const size_t  LENGTH   = DEFAULT_DATA[i].d_length;

            if (veryVerbose) { T_ T_ P_(LINE) P(ORIGINAL) }

            memset(buffer, 0, sizeof(buffer));
            strcpy(buffer, ORIGINAL);

            char *buffernBegin = buffer;
            char *bufferEnd    = buffer + LENGTH;

            FormatterCharUtil<char>::toUpper(buffernBegin, bufferEnd);

            ASSERTV(LINE, EXPECTED, buffer, 0 == strcmp(EXPECTED, buffer));
        }

        for (size_t i = 0; i < NUM_SPECIAL_DATA; ++i) {
            const int     LINE     = SPECIAL_DATA[i].d_line;
            const char   *ORIGINAL = SPECIAL_DATA[i].d_charStr;
            const char   *EXPECTED = SPECIAL_DATA[i].d_upperStr;
            const size_t  LENGTH   = SPECIAL_DATA[i].d_length;

            if (veryVerbose) { T_ T_ P_(LINE) P(ORIGINAL) }

            memset(buffer, 0, sizeof(buffer));
            strcpy(buffer, ORIGINAL);

            char *bufferBegin = buffer;
            char *bufferEnd   = buffer + LENGTH;

            FormatterCharUtil<char>::toUpper(bufferBegin, bufferEnd);

            ASSERTV(LINE, EXPECTED, buffer, 0 == strcmp(EXPECTED, buffer));
        }
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
        //: 1 Create source `char` array containing some data with trailing
        //:   markers.
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
        // output iterator.  So we leave one unchanged character in the
        // destination array.

        const size_t BUFFER_SIZE              = sizeof SOURCE;
        const size_t NUM_CHARACTERS_TO_OUTPUT = BUFFER_SIZE - 2;

        // Testing `char` specialization
        {
            char destination[BUFFER_SIZE];
            memset(destination, 0, sizeof(destination));

            const char *sourceBegin = SOURCE;
            const char *sourceEnd   = SOURCE + NUM_CHARACTERS_TO_OUTPUT;

            // Testing sequence overload

            OutputIterator<char> out(destination);
            out = FormatterCharUtil<char>::outputFromChar(sourceBegin,
                                                          sourceEnd,
                                                          out);

            ASSERTV(destination + NUM_CHARACTERS_TO_OUTPUT == out.ptr());
            for (size_t i = 0; i < NUM_CHARACTERS_TO_OUTPUT; ++i) {
                ASSERTV(i, SOURCE[i], destination[i],
                        SOURCE[i] == destination[i]);
            }

            // Verifying that the rest of the output array remains unaffected

            ASSERTV(destination[NUM_CHARACTERS_TO_OUTPUT],
                    0 == destination[NUM_CHARACTERS_TO_OUTPUT]);

            // Testing single character overload

            out = FormatterCharUtil<char>::outputFromChar(
                                              SOURCE[NUM_CHARACTERS_TO_OUTPUT],
                                              out);

            ASSERTV(destination + NUM_CHARACTERS_TO_OUTPUT + 1, out.ptr(),
                    destination + NUM_CHARACTERS_TO_OUTPUT + 1 == out.ptr());

            ASSERTV(SOURCE[NUM_CHARACTERS_TO_OUTPUT],
                    destination[NUM_CHARACTERS_TO_OUTPUT],
                    SOURCE[NUM_CHARACTERS_TO_OUTPUT] ==
                    destination[NUM_CHARACTERS_TO_OUTPUT]);

            // Testing `toUpper`

            char *destinationBegin = destination;
            char *destinationEnd   = destination + BUFFER_SIZE;

            FormatterCharUtil<char>::toUpper(destinationBegin, destinationEnd);

            for (size_t i = 0; i < BUFFER_SIZE; ++i) {
                ASSERTV(i, UPPER_SOURCE[i], destination[i],
                        UPPER_SOURCE[i] == destination[i]);
            }
        }

        // Testing `wchar_t` specialization
        {
            wchar_t destination[BUFFER_SIZE];
            memset(destination, 0, sizeof(destination));

            const char *sourceBegin = SOURCE;
            const char *sourceEnd   = SOURCE + NUM_CHARACTERS_TO_OUTPUT;

            // Testing sequence overload

            OutputIterator<wchar_t> out(destination);
            out = FormatterCharUtil<wchar_t>::outputFromChar(sourceBegin,
                                                             sourceEnd,
                                                             out);

            ASSERTV(destination + NUM_CHARACTERS_TO_OUTPUT == out.ptr());
            for (size_t i = 0; i < NUM_CHARACTERS_TO_OUTPUT; ++i) {
                ASSERTV(i, SOURCE[i], destination[i],
                        wchar_t(SOURCE[i]) == destination[i]);
            }

            // Verifying that the rest of the output array remains unaffected

            ASSERTV(destination[NUM_CHARACTERS_TO_OUTPUT],
                    0 == destination[NUM_CHARACTERS_TO_OUTPUT]);

            // Testing single character overload

            out = FormatterCharUtil<wchar_t>::outputFromChar(
                                              SOURCE[NUM_CHARACTERS_TO_OUTPUT],
                                              out);

            ASSERTV(destination + NUM_CHARACTERS_TO_OUTPUT + 1, out.ptr(),
                    destination + NUM_CHARACTERS_TO_OUTPUT + 1 == out.ptr());

            ASSERTV(SOURCE[NUM_CHARACTERS_TO_OUTPUT],
                    destination[NUM_CHARACTERS_TO_OUTPUT],
                    wchar_t(SOURCE[NUM_CHARACTERS_TO_OUTPUT]) ==
                                        destination[NUM_CHARACTERS_TO_OUTPUT]);
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
