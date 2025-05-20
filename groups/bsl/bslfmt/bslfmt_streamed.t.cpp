// bslstl_streamed.t.cpp                                              -*-C++-*-
#define BB_DEPRECATE_ENABLE_ALL_DEPRECATIONS_FOR_TESTING                      1

#include <bslfmt_streamed.h>

#include <bslfmt_format.h>

#include <bsls_bsltestutil.h>

#include <bslstl_iterator.h>
#include <bslstl_ostringstream.h>
#include <bslstl_string.h>

#include <limits.h>
#include <stdio.h>

#include <iostream>

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP20_FORMAT
  #include <format>
  #include <string>
#endif

using namespace BloombergLP;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
// Tested are the wrapper class template `bslfmt::Streamed<t_STREAMED>` as well
// as the wrapper-creator function template (mainly for C++03 compatibility)
// `bslfmt::streamed`.
//-----------------------------------------------------------------------------
// [ 2] Streamed_OutIterBuf
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 3] USAGE EXAMPLE
//-----------------------------------------------------------------------------

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

//=============================================================================
//                               TEST TYPES
//-----------------------------------------------------------------------------

struct CanStream {
    const char *d_content;

    // CREATORS
    explicit CanStream(const char *content)
    : d_content(content)
    {
    }

    // MANIPULATORS
    void setContent(const char *content) { d_content = content; }

    // ACCESSORS
    const char *content() const { return d_content; }
};

std::ostream& operator<<(std::ostream& os, const CanStream& obj)
{
    return os << obj.d_content;
}

                      // ================================
                      // OutputIteratorStreamBufferTester
                      // ================================

/// This type is used to test the content-limiting behavior of
/// `Streamed_OutIterBuf`.  It can print to stream up to 62 characters, and it
/// verifies the state of the stream after each print.  It saves after which
/// print has it encountered a bad state, in other words the position of the
/// first character it was unable to print to the stream.  Once the bad state
/// is encountered it won't print any more characters (as well behaved
/// operators would work).  The class is made so that the same instance is
/// reusable is it both has a `reset` method to reset the bad-position and a
/// method to update the number of characters printed.  The 62 characters
/// printed go from 0-9, then a-z, then A-Z.
class OutputIteratorStreamBufferTester {
    // DATA
    int         d_numCharsPrinted;
    mutable int d_badPos;

    // PRIVATE CLASS METHODS
    static char nthChar(int n)
    {
        if (n < 10) {
            return static_cast<char>('0' + n);
        }
        else if (n < 36) {
            return static_cast<char>('a' + (n - 10));
        }
        else {
            return static_cast<char>('A' + (n - 36));
        }
    }

  public:
    // CREATORS
    OutputIteratorStreamBufferTester(int numChars)
    : d_numCharsPrinted(numChars)
    , d_badPos(-1)
    {
        ASSERT(numChars >= 0);
        ASSERT(numChars <= 62);
    }

    // MANIPULATORS
    void reset() { d_badPos = -1; }

    void setNumChars(int numChars)
    {
        ASSERT(numChars >= 0);
        ASSERT(numChars <= 62);

        d_numCharsPrinted = numChars;
    }

    // ACCESSORS
    int badPos() const { return d_badPos; }

    std::ostream& streamInsert(std::ostream& os) const
    {
        for (int n = 0; n < d_numCharsPrinted; ++n) {
            if (os.bad()) {
                d_badPos = n;
                return os;                                            // RETURN
            }

            os << nthChar(n);
        }

        if (os.bad()) {
            d_badPos = d_numCharsPrinted;
        }

        return os;
    }
};

std::ostream& operator<<(std::ostream&                           os,
                         const OutputIteratorStreamBufferTester& obj)
{
    return obj.streamInsert(os);
}

//=============================================================================
//                              USAGE EXAMPLE
//-----------------------------------------------------------------------------

///Example 1: Formatting a Streamable Object Using the Function
/// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose we want to format an object that already supports streaming into an
// `ostream` using the insert `operator<<`.  When writing portable code that
// should work on compilers that do not support class template argument
// deduction we would use the wrapper-creator function `bslfmt::streamed` to
// avoid having to know and write the type of the object.
//
// First, for the sake of demonstration we create a type with an obscure and
// long name that we neither want to remember nor ever to write down, and which
// can be streamed out:
//
//```
    class ThisTypeHasLongAndObscureNameButStreamable {
    };

    std::ostream& operator<<(
                         std::ostream&                                     os,
                         const ThisTypeHasLongAndObscureNameButStreamable& )
    {
        return os << "The printout";
    }
//```

///Example 2: Formatting with CTAD support
///- - - - - - - - - - - - - - - - - - - -
// Suppose we want to format an object that already supports streaming into an
// `ostream` using the insert `operator<<` and we target only modern compilers.
// In such case the wrapper class template can be used directly, without the
// need for the function.
//
// First, for the sake of demonstration we create a type with an obscure and
// long name that we neither want to remember nor ever to write down, and which
// can be streamed out:
//
//```
//  class ThisTypeHasLongAndObscureNameButStreamable {
//  };
//
//  std::ostream& operator<<(
//                       std::ostream&                                     os,
//                       const ThisTypeHasLongAndObscureNameButStreamable& )
//  {
//      return os << "The printout";
//  }
//```

///Example 3: Format String Options
/// - - - - - - - - - - - - - - - -
// Suppose we want to format an object that already supports streaming into an
// `ostream` using the insert `operator<<` for an environment that requires the
// output to be padded, aligned, and/or truncation of the output.  In this
// example we will introduce the effects of the various possible format string
// specifications.
//
// First, for the sake of demonstration we create a type that prints a series
// of digits to help demonstrate the effects of the various formattings:
//
//```
    class Streamable {
    };

    std::ostream& operator<<(std::ostream& os, const Streamable&)
    {
        return os << "12345678";
    }
//```

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
      case 3: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //
        // Concern:
        //: 1 Demonstrate the functioning of this component.
        //
        // Plan:
        //: 1 Use test contexts to format a single string.
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) puts("\nUSAGE EXAMPLE"
                          "\n=============");

        {
///Example 1: Formatting a Streamable Object Using the Function
/// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose we want to format an object that already supports streaming into an
// `ostream` using the insert `operator<<`.  When writing portable code that
// should work on compilers that do not support class template argument
// deduction we would use the wrapper-creator function `bslfmt::streamed` to
// avoid having to know and write the type of the object.
//
// First, for the sake of demonstration we create a type with an obscure and
// long name that we neither want to remember nor ever to write down, and which
// can be streamed out:
//
//```
//  class ThisTypeHasLongAndObscureNameButStreamable {
//  };
//
//  std::ostream& operator<<(
//                       std::ostream&                                     os,
//                       const ThisTypeHasLongAndObscureNameButStreamable& )
//  {
//      return os << "The printout";
//  }
//```
//
// Then, we create an object of said type that we want to print out:
//
//```
    const ThisTypeHasLongAndObscureNameButStreamable obj;
//```
// Next, we format the "value" using `bsl::format` with the wrapper-creator
// function:
//
//```
    bsl::string s = bsl::format("{}", bslfmt::streamed(obj));
//```
// Finally, we verify the output is correct:
//
//```
    ASSERT(s == "The printout");
//```
        }

#ifdef BSLS_COMPILERFEATURES_SUPPORT_CTAD
        {
///Example 2: Formatting with CTAD support
///- - - - - - - - - - - - - - - - - - - -
// Suppose we want to format an object that already supports streaming into an
// `ostream` using the insert `operator<<` and we target only modern compilers.
// In such case the wrapper class template can be used directly, without the
// need for the function.
//
// First, for the sake of demonstration we create a type with an obscure and
// long name that we neither want to remember nor ever to write down, and which
// can be streamed out:
//
//```
//  class ThisTypeHasLongAndObscureNameButStreamable {
//  };
//
//  std::ostream& operator<<(
//                       std::ostream&                                     os,
//                       const ThisTypeHasLongAndObscureNameButStreamable& )
//  {
//      return os << "The printout";
//  }
//```
//
// Then, we create an object of said type that we want to print out:
//
//```
    const ThisTypeHasLongAndObscureNameButStreamable obj;
//```
// Next, we format the "value" using `bsl::format` with the wrapper class
// template, class template argument deduction takes care of the type:
//
//```
    bsl::string s = bsl::format("{}", bslfmt::Streamed(obj));
//```
// Finally, we verify the output is correct:
//
//```
    ASSERT(s == "The printout");
//```
        }
#endif  // BSLS_LIBRARYFEATURES_HAS_CPP20_FORMAT

        {
///Example 3: Format String Options
/// - - - - - - - - - - - - - - - -
// Suppose we want to format an object that already supports streaming into an
// `ostream` using the insert `operator<<` for an environment that requires the
// output to be padded, aligned, and/or truncation of the output.  In this
// example we will introduce the effects of the various possible format string
// specifications.
//
// First, for the sake of demonstration we create a type that prints a series
// of digits to help demonstrate the effects of the various formattings:
//
//```
//  class Streamable {
//  };
//
//  std::ostream& operator<<(std::ostream& os, const Streamable&)
//  {
//      return os << "12345678";
//  }
//```
//
// Then, we create an object of said type that we will format:
//
//```
    const Streamable obj;
//```
//
// Next, we format the "value" using many different format strings, starting
// with the default for completeness:
//```
    bsl::string s = bsl::format("{}", bslfmt::streamed(obj));
    ASSERT(s == "12345678");
//```
//
// Then, we format with specifying just a width:
//```
    s = bsl::format("{:10}", bslfmt::streamed(obj));
    ASSERT(s == "12345678  ");
//```
//
// Next, we format with specifying a width, and alignments:
//```
    s = bsl::format("{:<10}", bslfmt::streamed(obj));
    ASSERT(s == "12345678  ");

    s = bsl::format("{:^10}", bslfmt::streamed(obj));
    ASSERT(s == " 12345678 ");

    s = bsl::format("{:>10}", bslfmt::streamed(obj));
    ASSERT(s == "  12345678");
//```
//
// Finally, we demonstrate the truncation using a "precision" value:
//```
    s = bsl::format("{:.6}", bslfmt::streamed(obj));
    ASSERT(s == "123456");

    s = bsl::format("{:8.6}", bslfmt::streamed(obj));
    ASSERT(s == "123456  ");

    s = bsl::format("{:<8.6}", bslfmt::streamed(obj));
    ASSERT(s == "123456  ");

    s = bsl::format("{:^8.6}", bslfmt::streamed(obj));
    ASSERT(s == " 123456 ");

    s = bsl::format("{:>8.6}", bslfmt::streamed(obj));
    ASSERT(s == "  123456");
//```
        }
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // OUTPUT ITERATOR STREAM BUFFER
        //
        // Concern:
        //: 1 Demonstrate the functioning of this component.
        //
        // Plan:
        //: 1 Use test contexts to format a single string.
        //
        // Testing:
        //   Streamed_OutIterBuf
        // --------------------------------------------------------------------

        if (verbose) puts("\nOUTPUT ITERATOR STREAM BUFFER"
                          "\n=============================");

        // First a rudimentary test of the test machinery
        {
            OutputIteratorStreamBufferTester tester(10);
            bsl::ostringstream               os;

            os << tester;
            ASSERTV(os.str().c_str(), os.str() == "0123456789");
            ASSERTV(tester.badPos(), -1 == tester.badPos());
            ASSERT(os.good());

            os.str("");
            os.setstate(std::ios_base::badbit);
            os << tester;
            ASSERTV(os.str().c_str(), os.str() == "");
            ASSERTV(tester.badPos(), 0 == tester.badPos());
            ASSERT(os.bad());
            tester.reset();
            ASSERTV(tester.badPos(), -1 == tester.badPos());

            os.str("");
            os.clear();
            tester.setNumChars(16);
            os << tester;
            ASSERTV(os.str().c_str(), os.str() == "0123456789abcdef");
            ASSERTV(tester.badPos(), -1 == tester.badPos());
            ASSERT(os.good());

            os.str("");
            tester.setNumChars(37);
            os << tester;
            ASSERTV(os.str().c_str(),
                    os.str() == "0123456789abcdefghijklmnopqrstuvwxyzA");
            ASSERTV(tester.badPos(), -1 == tester.badPos());
            ASSERT(os.good());
        }

        // Now the actual test

        const int X = INT_MAX;  // X is no limit

        static struct {
            int         d_line;
            int         d_numChars;
            int         d_limit;
            int         d_badPos;
            const char *d_expected;
        } DATA[] = {
            { L_, 1, X, -1, "0" },
            { L_, 1, 1, -1, "0" },
            { L_, 1, 0,  1, ""  },

            { L_, 5, X, -1, "01234" },
            { L_, 5, 9, -1, "01234" },
            { L_, 5, 6, -1, "01234" },
            { L_, 5, 5, -1, "01234" },
            { L_, 5, 4,  5, "0123"  },
            { L_, 5, 3,  4, "012"   },
            { L_, 5, 2,  3, "01"    },
            { L_, 5, 1,  2, "0"     },
            { L_, 5, 0,  1, ""      },

            { L_, 16,  X, -1, "0123456789abcdef" },
            { L_, 16, 99, -1, "0123456789abcdef" },
            { L_, 16, 66, -1, "0123456789abcdef" },
            { L_, 16, 33, -1, "0123456789abcdef" },
            { L_, 16, 16, -1, "0123456789abcdef" },
            { L_, 16, 15, 16, "0123456789abcde"  },
            { L_, 16, 14, 15, "0123456789abcd"   },
            { L_, 16, 13, 14, "0123456789abc"    },
            { L_, 16, 12, 13, "0123456789ab"     },
            { L_, 16, 11, 12, "0123456789a"      },
            { L_, 16, 10, 11, "0123456789"       },
            { L_, 16,  9, 10, "012345678"        },
            { L_, 16,  8,  9, "01234567"         },
            { L_, 16,  7,  8, "0123456"          },
            { L_, 16,  6,  7, "012345"           },
            { L_, 16,  5,  6, "01234"            },
            { L_, 16,  4,  5, "0123"             },
            { L_, 16,  3,  4, "012"              },
            { L_, 16,  2,  3, "01"               },
            { L_, 16,  1,  2, "0"                },
            { L_, 16,  0,  1, ""                 },
        };

        const size_t NUM_DATA = sizeof DATA / sizeof *DATA;

        for (size_t i = 0; i < NUM_DATA; ++i) {
            const int          LINE      = DATA[i].d_line;
            const int          NUM_CHARS = DATA[i].d_numChars;
            const int          LIMIT     = DATA[i].d_limit;
            const int          BAD_POS   = DATA[i].d_badPos;
            const char *const  EXPECTED  = DATA[i].d_expected;

            if (veryVerbose) {
                P_(LINE) P_(NUM_CHARS) P_(LIMIT) P_(BAD_POS) P(EXPECTED);
            }

            const OutputIteratorStreamBufferTester tester(NUM_CHARS);

            typedef bsl::back_insert_iterator<bsl::string> Iter;

            bsl::string result;
            bslfmt::Streamed_OutIterBuf<Iter> buf(bsl::back_inserter(result),
                                                  LIMIT);
            std::ostream                      os(&buf);

            os << tester;

            ASSERTV(LINE, EXPECTED, result.c_str(), EXPECTED == result);
            ASSERTV(LINE, BAD_POS,  tester.badPos(), BAD_POS == tester.badPos());
        }
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //
        // Concerns:
        //: 1 That basic functionality appears to work as advertised before
        //:   before beginning testing in earnest:
        //:   - default and copy constructors
        //:   - assignment operator
        //:   - primary manipulators, basic accessors
        //:   - 'operator==', 'operator!='
        //
        // Plan:
        //: 1 Test all public methods mentioned in concerns.  (C-1)
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------
        if (verbose) puts("\nBREATHING TEST"
                          "\n==============");

        CanStream        mX("0123456789");
        const CanStream& X = mX;

#define TEST_LINE_BSL(fmt, res)                                               \
    result = bsl::format(fmt, bslfmt::Streamed<CanStream>(X));                \
    ASSERTV(result.c_str(), result == res);                                   \
                                                                              \
    result = bsl::format(fmt, bslfmt::streamed(X));                           \
    ASSERTV(result.c_str(), result == res)

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP20_FORMAT
  #define TEST_LINE_STD(fmt, res)                                             \
    stdResult = std::format(fmt, bslfmt::Streamed<CanStream>(X));             \
    ASSERTV(stdResult.c_str(), stdResult == res);                             \
                                                                              \
    stdResult = std::format(fmt, bslfmt::streamed(X));                        \
    ASSERTV(stdResult.c_str(), stdResult == res)
#else
  #define TEST_LINE_STD(fmt, res)
#endif

#define TEST_LINE(fmt, res)                                                   \
    TEST_LINE_BSL(fmt, res);                                                  \
    TEST_LINE_STD(fmt, res)

        bsl::string result;
        std::string stdResult;

        TEST_LINE("{}",     "0123456789"  );
        TEST_LINE("{:12}",  "0123456789  ");
        TEST_LINE("{:<12}", "0123456789  ");
        TEST_LINE("{:>12}", "  0123456789");
        TEST_LINE("{:^12}", " 0123456789 ");

        TEST_LINE("{:10.8}",  "01234567  ");
        TEST_LINE("{:<10.8}", "01234567  ");
        TEST_LINE("{:>10.8}", "  01234567");
        TEST_LINE("{:^10.8}", " 01234567 ");

        TEST_LINE("{:10.12}",  "0123456789");
        TEST_LINE("{:<10.12}", "0123456789");
        TEST_LINE("{:<10.12}", "0123456789");
        TEST_LINE("{:^10.12}", "0123456789");

        TEST_LINE("{:*<12}", "0123456789**");
        TEST_LINE("{:*>12}", "**0123456789");
        TEST_LINE("{:*^12}", "*0123456789*");

        TEST_LINE("{:*<10.7}", "0123456***");
        TEST_LINE("{:*>10.7}", "***0123456");
        TEST_LINE("{:*^10.7}", "*0123456**");

        mX.setContent(
          //           1         2         3         4         5         6
          //  12345678901234567890123456789012345678901234567890123456789012
             "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ");

        TEST_LINE(
             "{}",
             "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ");

        TEST_LINE(
             "{:62}",
             "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ");
        TEST_LINE(
            "{:63}",
            "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ ");
        TEST_LINE(
           "{:64}",
           "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ  ");
        TEST_LINE(
        "{:67}",
        "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ     ");
        TEST_LINE(
        "{:<67}",
        "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ     ");
        TEST_LINE(
        "{:^67}",
        "  0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ   ");
        TEST_LINE(
        "{:>67}",
        "     0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ");
        TEST_LINE(
        "{:-<67}",
        "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ-----");
        TEST_LINE(
        "{::^67}",
        "::0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ:::");
        TEST_LINE(
        "{:^>67}",
        "^^^^^0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ");
        TEST_LINE(
             "{:61}",
             "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ");
        TEST_LINE(
             "{:60}",
             "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ");
        TEST_LINE(
             "{:30}",
             "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ");
        TEST_LINE(
             "{:1}",
             "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ");

        TEST_LINE(
             "{:.100}",
             "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ");
        TEST_LINE(
             "{:.70}",
             "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ");
        TEST_LINE(
             "{:.63}",
             "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ");
        TEST_LINE(
             "{:.62}",
             "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ");
        TEST_LINE(
             "{:.61}",
             "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXY");
        TEST_LINE(
             "{:.60}",
             "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWX");
        TEST_LINE( "{:.36}", "0123456789abcdefghijklmnopqrstuvwxyz");
        TEST_LINE( "{:.10}", "0123456789");

        TEST_LINE("{:1.36}",    "0123456789abcdefghijklmnopqrstuvwxyz");
        TEST_LINE("{:10.36}",   "0123456789abcdefghijklmnopqrstuvwxyz");
        TEST_LINE("{:30.36}",   "0123456789abcdefghijklmnopqrstuvwxyz");
        TEST_LINE("{:36.36}",   "0123456789abcdefghijklmnopqrstuvwxyz");
        TEST_LINE("{:37.36}",   "0123456789abcdefghijklmnopqrstuvwxyz ");
        TEST_LINE("{:<37.36}",  "0123456789abcdefghijklmnopqrstuvwxyz ");
        TEST_LINE("{:^37.36}",  "0123456789abcdefghijklmnopqrstuvwxyz ");
        TEST_LINE("{:>37.36}",  " 0123456789abcdefghijklmnopqrstuvwxyz");
        TEST_LINE("{:<40.36}",  "0123456789abcdefghijklmnopqrstuvwxyz    ");
        TEST_LINE("{:^40.36}",  "  0123456789abcdefghijklmnopqrstuvwxyz  ");
        TEST_LINE("{:>40.36}",  "    0123456789abcdefghijklmnopqrstuvwxyz");
        TEST_LINE("{:|<40.36}", "0123456789abcdefghijklmnopqrstuvwxyz||||");
        TEST_LINE("{:+^40.36}", "++0123456789abcdefghijklmnopqrstuvwxyz++");
        TEST_LINE("{:.>40.36}", "....0123456789abcdefghijklmnopqrstuvwxyz");

//#define BSLFMT_STREAMED_DEMONSTRATE_FORMATTER_EXISTS_WARNING
#ifdef BSLFMT_STREAMED_DEMONSTRATE_FORMATTER_EXISTS_WARNING
        bsl::string s = bsl::format("{}", bslfmt::streamed(12));
#ifdef BSLS_COMPILERFEATURES_SUPPORT_CTAD
        s = bsl::format("{}", bslfmt::Streamed(12));
#endif  //BSLS_COMPILERFEATURES_SUPPORT_CTAD
#endif  // BSLFMT_STREAMED_DEMONSTRATE_FORMATTER_EXISTS_WARNING
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
// Copyright 2025 Bloomberg Finance L.P.
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
