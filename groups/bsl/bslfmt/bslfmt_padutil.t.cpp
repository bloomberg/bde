// bslfmt_padutil.t.cpp                                               -*-C++-*-

#include <bslfmt_padutil.h>

#include <bslstl_string.h>

#include <bsls_asserttest.h>
#include <bsls_bsltestutil.h>
#include <bsls_libraryfeatures.h>

#include <stdio.h>
#include <string.h>

using namespace BloombergLP;
using std::size_t;

// ============================================================================
//                             TEST PLAN
// ----------------------------------------------------------------------------
//                             Overview
//                             --------
// `bslfmt::PadUtil` is a namespace class for the methods `computePadding` and
// `pad`, useful for padding output from bslfmt.
// ----------------------------------------------------------------------------
// CLASS METHOD
// [ 4] t_ITERATOR PadUtil<t_CHAR>::pad(t_ITERATOR, ptrdiff_t, char);
// [ 4] t_ITERATOR PadUtil<t_CHAR>::pad(t_ITERATOR, ptrdiff_t, wchar_t);
// [ 3] t_ITERATOR PadUtil<t_CHAR>::pad(t_ITERATOR, ptrdiff_t, string_view);
// [ 3] t_ITERATOR PadUtil<t_CHAR>::pad(t_ITERATOR, ptrdiff_t, wstring_view);
// [ 2] computePadding(size_t *, size_t *, WidthValue, size_t, Alignment);
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 5] USAGE EXAMPLE

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

#define Q  BSLS_BSLTESTUTIL_Q    // Quote identifier literally.
#define P  BSLS_BSLTESTUTIL_P    // Print identifier and value.
#define P_ BSLS_BSLTESTUTIL_P_   // P(X) without '\n'.
#define T_ BSLS_BSLTESTUTIL_T_   // Print a tab (w/o newline).
#define L_ BSLS_BSLTESTUTIL_L_   // current Line number

// ============================================================================
//                  NEGATIVE-TEST MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT_PASS(EXPR) BSLS_ASSERTTEST_ASSERT_PASS(EXPR)
#define ASSERT_FAIL(EXPR) BSLS_ASSERTTEST_ASSERT_FAIL(EXPR)

// ============================================================================
//                                USAGE EXAMPLE
// ----------------------------------------------------------------------------

namespace UsageExample {

///Usage
///-----
// Suppose we have a string and we want to be able to pad it with any desired
// filler string, and pad it to the left or right, or pad it on both sides, to
// reach a desired total width.
//
// First, we define a couple of typedef's to shorten some bslfmt package types:
// ```
    typedef bslfmt::FormatterSpecificationNumericValue     NumericValue;
    typedef bslfmt::FormatSpecificationParserEnums         ParserEnums;
    typedef bslfmt::PadUtil<char>                          PadUtil;
// ```
// Then, we are now able to define a function `paddedString` which will take a
// a string, `content`, and pad it to a given `fieldWidth` with the specified
// `filler`.  `pad` does not assume that `filler` is a single Unicode code
// point, though that will normally be the case.
// ```
    void paddedString(bsl::string             *result,
                      const bsl::string_view&  content,
                      unsigned                 fieldWidth,
                      ParserEnums::Alignment   alignment,
                      const bsl::string_view&  filler)
    {
        std::ptrdiff_t leftPadding, rightPadding;

        PadUtil::computePadding(&leftPadding,
                                &rightPadding,
                                NumericValue(NumericValue::e_VALUE,
                                             fieldWidth),
                                content.length(),
                                alignment,
                                ParserEnums::e_ALIGN_LEFT);

        result->resize(filler.length() * (leftPadding + rightPadding) +
                                                             content.length());

        bsl::string::iterator it = result->begin();
        it = PadUtil::pad(it, leftPadding,  filler);

        it = bsl::copy(content.begin(), content.end(), it);

        it = PadUtil::pad(it, rightPadding, filler);

        ASSERT(result->end() == it);
    }
// ```

}  // close namespace UsageExample

// ============================================================================
//                     ASSISTANCE TYPES AND FUNCTIONS
// ----------------------------------------------------------------------------

using   bslfmt::PadUtil;
typedef bslfmt::FormatterSpecificationNumericValue         NumericValue;
typedef NumericValue::Category                             Category;
typedef bslfmt::FormatSpecificationParserEnums             ParserEnums;
typedef ParserEnums::Alignment                             Alignment;

const Category  e_C_DEFAULT = NumericValue::e_DEFAULT;
const Category  e_VALUE     = NumericValue::e_VALUE;

const Alignment e_A_DEFAULT = ParserEnums::e_ALIGN_DEFAULT;
const Alignment e_LEFT      = ParserEnums::e_ALIGN_LEFT;
const Alignment e_MIDDLE    = ParserEnums::e_ALIGN_MIDDLE;
const Alignment e_RIGHT     = ParserEnums::e_ALIGN_RIGHT;

// ============================================================================
//                                 TEST CASE 3
// ----------------------------------------------------------------------------

namespace {
namespace u {

void padOracle(bsl::string      *result,
               std::ptrdiff_t    padWidth,
               bsl::string_view  filler)
{
    result->clear();
    while (0 < padWidth--) {
        *result += filler;
    }
}

void padOracle(bsl::string      *result,
               std::ptrdiff_t    padWidth,
               char              filler)
{
    result->clear();
    while (0 < padWidth--) {
        *result += filler;
    }
}

void widen(bsl::wstring            *out,
           const bsl::string_view&  in)
{
    out->assign(in.begin(), in.end());
}

/// This function is meant to be called by `ASSERTV` to print out
/// `wchar_t` - based strings.  Note that we can't return a `bsl::string` by
/// value because `ASSERTV`s at this level are `printf`-based and don't know
/// how to print out `bsl::string`s.
const char *narrow(bsl::string              *buffer,
                   const bsl::wstring_view&  in)
{
    buffer->clear();
    typedef bsl::wstring_view::iterator Iter;
    Iter end = in.end();
    for (Iter it = in.begin(); it < end; ++it) {
        *buffer += static_cast<char>(*it);
    }
    return buffer->c_str();
}

template <class t_ENUM_TYPE>
void incEnum(t_ENUM_TYPE *e)
{
    *e = static_cast<t_ENUM_TYPE>(*e + 1);
}

enum FillerType {
    k_FIRST_FILLER_TYPE,
    e_CONST_CHAR_STAR = k_FIRST_FILLER_TYPE,
    e_CHAR_SV,
    e_CHAR_BSL_STRING,
    e_CHAR_STD_STRING,
    e_CHAR_PMR_STRING,
    k_MAX_CHAR_TYPE = e_CHAR_PMR_STRING,
    e_CONST_WCHAR_T_STAR,
    e_WCHAR_T_SV,
    e_WCHAR_T_BSL_STRING,
    e_WCHAR_T_STD_STRING,
    e_WCHAR_T_PMR_STRING,
    k_NUM_FILLER_TYPES
};

}  // close namespace u
}  // close unnamed namespace

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char** argv)
{
    const int  test = argc > 1 ? atoi(argv[1]) : 0;
    const bool verbose = argc > 2;
    const bool veryVerbose = argc > 3;
    const bool veryVeryVerbose = argc > 4;        (void) veryVeryVerbose;
    const bool veryVeryVeryVerbose = argc > 5;    (void) veryVeryVeryVerbose;

    printf("TEST %s CASE %d \n", __FILE__, test);

    switch (test) { case 0:
      case 5: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) printf("USAGE EXAMPLE\n"
                            "=============\n");

    using namespace UsageExample;

// Next, we use our new `paddedString` to create a string with "John Brown"
// padded on either side to a total width of 20 bytes, padding with dashes:
// ```
    bsl::string s;
    paddedString(&s, "John Brown", 20, ParserEnums::e_ALIGN_MIDDLE, "-");
    ASSERT("-----John Brown-----" == s);
// ```
// Now, let's pad to right with Unicode `Euro` symbols to a total width of 20
// bytes:
// ```
    const char *euro = "\xe2\x82\xac";        // Unicode for the Euro symbol.
    paddedString(&s, "Seven Euros: ", 20, ParserEnums::e_ALIGN_LEFT, euro);
    ASSERT("Seven Euros: €€€€€€€" == s);
// ```
// Note that because `euro` is not a single byte, the output is longer than
// `fieldWidth`, which was 20:
// ```
    ASSERT(34 == s.length());
// ```
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING PAD ON SINGLE CHARS
        //
        // Concern:
        // 1. That `pad` works properly when `filler` is a single `char` or
        //    `wchar_t`.
        //
        // Plan:
        // 1. Have a table of different values of `char`s, at least one of them
        //    non-ascii.  Iterate through the values and assign the `char`
        //    value to another `wchar_t` variable.
        //
        // 2. Have a function `padOracle` that will produce the anticipated
        //    expected result of calling `pad`.
        //
        // 3. Iterate through values of `int padWidth` over `[ -1 .. 9 ]`.
        //
        // 4. Call `padOracle` to get a `bsl::string` of the expected result,
        //    and call `widen` (used in other examples) to translate that to a
        //    `bsl::wstring`.
        //
        // 5. Do a `char` to `char` padding and check the result.
        //
        // 6. Do a `wchar_t` to `char` padding.  If the pad char is ascii, the
        //    result should match the string provided by `padOracle`.  If not,
        //    call `pad` in an `ASSERT_FAIL` and expect failure.
        //
        // 7. Do a `wchar_t` to `wchar_t` padding and verify the result is as
        //    expected.
        //
        // 8. Do a `char` to `wchar_t` padding.  If the pad char is ascii, the
        //    result will be as expected.  If not, call `pad` in an
        //    `ASSERT_FAIL` and expect failure.
        //
        // Testing:
        //   t_ITERATOR PadUtil<t_CHAR>::pad(t_ITERATOR, ptrdiff_t, char);
        //   t_ITERATOR PadUtil<t_CHAR>::pad(t_ITERATOR, ptrdiff_t, wchar_t);
        // --------------------------------------------------------------------

        if (verbose) printf("TESTING PAD ON SINGLE CHARS\n"
                            "===========================\n");

        struct Data {
            int         d_line;
            char        d_char;
        } DATA[] = {
            { L_, 'a' },
            { L_, '*' },
            { L_, '%' },
            { L_, '\x82' } };
        enum { k_NUM_DATA = sizeof DATA / sizeof *DATA };

        char    cBuf[20];
        wchar_t wBuf[20];

        typedef bslfmt::PadUtil<char>       CPadUtil;
        typedef bslfmt::PadUtil<wchar_t>    WPadUtil;

        for (int ti = 0; ti < k_NUM_DATA; ++ti) {
            const Data&   data     = DATA[ti];
            const int     LINE     = data.d_line;
            const char    CHAR     = data.d_char;
            const wchar_t WCHAR    = CHAR;
            const bool    IS_ASCII = 0 == (0x80 & CHAR);

            for (std::ptrdiff_t padWidth = -1; padWidth < 10; ++padWidth) {
                bsl::string  exps;    const bsl::string&  EXPS = exps;

                u::padOracle(&exps, padWidth, CHAR);

                bsl::wstring expw;    const bsl::wstring& EXPW = expw;
                u::widen(&expw, EXPS);

                char    *pc;
                wchar_t *pw;

                ::memset(cBuf, 0xaf, sizeof(cBuf));

                pc = CPadUtil::pad(pc = cBuf, padWidth, CHAR);
                *pc = 0;
                ASSERTV(LINE, EXPS.c_str(), cBuf, EXPS == cBuf);

                ::memset(cBuf, 0xaf, sizeof(cBuf));

                if (IS_ASCII) {
                    pc = cBuf;
                    ASSERT_PASS(pc = CPadUtil::pad(pc, padWidth,WCHAR));
                    *pc = 0;
                    ASSERTV(LINE, EXPS.c_str(), cBuf, EXPS == cBuf);
                }
                else {
                    bsls::AssertTestHandlerGuard hG;

                    pc = cBuf;
                    ASSERT_FAIL(pc = CPadUtil::pad(pc, padWidth,WCHAR));
                    BSLS_ASSERT(cBuf == pc);
                }

                ::memset(wBuf, 0xaf, sizeof(wBuf));

                pw = wBuf;
                pw = WPadUtil::pad(pw, padWidth, WCHAR);
                *pw = 0;
                ASSERTV(LINE, EXPW == wBuf);

                ::memset(wBuf, 0xaf, sizeof(wBuf));

                if (IS_ASCII) {
                    pw = wBuf;
                    ASSERT_PASS(pw = WPadUtil::pad(pw, padWidth, CHAR));
                    *pw = 0;
                    ASSERTV(LINE, EXPS.c_str(), EXPW == wBuf);
                }
                else {
                    bsls::AssertTestHandlerGuard hG;

                    pw = wBuf;
                    ASSERT_FAIL(pw = WPadUtil::pad(pw, padWidth, CHAR));
                    BSLS_ASSERT(wBuf == pw);
                }
            }
        }
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING PAD ON BASIC_STRING_VIEW
        //
        // Concerns:
        // 1. That the `pad` function operates correctly.
        //
        // Plan:
        // 1. Create a `padOracle` function that will create the expected
        //    output of the `pad` function where `t_CHAR` is type `char`.
        //
        // 2. Create a `widen` function that will create a `bsl::wstring`
        //    version of a `bsl::string_view` passed into it.
        //
        // 3. Iterate through a table of strings of varying length to
        //    populate the `FILLER` `string_view` variable.
        //
        // 4. Create variables of every type to represent `FILLER`:
        //    - `const char *`
        //    - `bsl::string`
        //    - `bsl::string_view`
        //    - `std::string`
        //    - `std::pmr::string`
        //
        // 5. Widen `FILLER` and create every wide representation of that:
        //    - `bsl::wstring`
        //    - `bsl::wstring_view`
        //    - `std::wstring`
        //    - `std::pmr::wstring`
        //    - `const wchar_t *`
        //
        // 6. Have an enum `FillerType` enumerating all the representations of
        //    `FILLER`, both `char` and `wchar_t`-based.
        //
        // 7. In a nested loop, iterate `padWidth` from -1 to 9.
        //
        // 8. Call `padOracle` to generate `CEXP`, an `bsl::string` of the
        //    expected output with `t_CHAR` == `char`.  Call `widen` on `CEXP`
        //    to create `WEXP`, a `bsl::wstring` version of the same thing.
        //
        // 9. Iterate through the enumerations of `FillerType` and in that
        //    loop, do two switch statements:
        //    - the first, write to a `char` buffer from all possible types
        //      of `FillerType`
        //    - the second, write to a `wchar_t` buffer from all possible types
        //      of `FillerType`
        //    In both of these cases, care must be taken not to pad with a
        //    non-ascii filler where the filler and the buffer written to are
        //    different character types.
        //
        // 10. Negative Testing: for each `padWidth`, attempt translations to
        //     `char` and `wchar_t` buffers from `FILLER` and `WFILLER`.  If
        //     the pad is ascii or `padWidth <= 0` or the input and output
        //     character types match, it will always pass.  If `1 <= padWidth`
        //     and the pad is non-ascii and the input and output character
        //     types are different, expect failure.
        //
        // Testing:
        //   PadUtil<t_CHAR>::pad(t_ITERATOR, ptrdiff_t, string_view);
        //   PadUtil<t_CHAR>::pad(t_ITERATOR, ptrdiff_t, wstring_view);
        // --------------------------------------------------------------------

        if (verbose) printf("TESTING PAD ON BASIC_STRING_VIEW\n"
                            "================================\n");

        typedef bslfmt::PadUtil<char>    CPadUtil;
        typedef bslfmt::PadUtil<wchar_t> WPadUtil;

        static const struct Data {
            int         d_line;
            const char *d_filler;
        } DATA[] = {
            { L_, "" },
            { L_, " " },
            { L_, "a" },
            { L_, "woof" },
            { L_, "\xe2\x82\xac" },    // Euro
        };
        enum { k_NUM_DATA = sizeof DATA / sizeof *DATA };

        char    cBuf[256], *pc;
        wchar_t wBuf[256], *pw;

        for (int ti = 0; ti < k_NUM_DATA; ++ti) {
            int                      LINE     = DATA[ti].d_line;
            const char              *FILLER   = DATA[ti].d_filler;
            bool                     IS_ASCII = 0 == (*FILLER & 0x80);

            bsl::wstring             filler_swb;
            u::widen(&filler_swb, FILLER);

            const bsl::string        FILLER_SCB(FILLER);
            const bsl::string_view   FILLER_SCV(FILLER_SCB);
            const std::string        FILLER_SCS(FILLER_SCV);
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_PMR_STRING
            const std::pmr::string   FILLER_SCP(FILLER_SCV);
#endif

            const bsl::wstring&      FILLER_SWB = filler_swb;
            const bsl::wstring_view  FILLER_SWV(FILLER_SWB);
            const std::wstring       FILLER_SWS(FILLER_SWV);
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_PMR_STRING
            const std::pmr::wstring  FILLER_SWP(FILLER_SWV);
#endif
            const wchar_t           *WFILLER  = FILLER_SWB.c_str();

            if (veryVerbose) {
                printf("--------------------------------------------------\n");
                printf("FILLER: \"%s\"\n", FILLER);
            }

            for (std::ptrdiff_t padWidth = -1; padWidth < 10; ++padWidth) {
                bsl::string cExp;    const bsl::string& CEXP = cExp;
                u::padOracle(&cExp, padWidth, FILLER);

                bsl::wstring wExp;    const bsl::wstring& WEXP = wExp;
                u::widen(&wExp, cExp);

                if (veryVeryVerbose) {
                    printf("CEXP: \"%s\"\n", CEXP.c_str());
                    printf("t_CHAR: `char`\n");
                }

                for (u::FillerType ft = u::k_FIRST_FILLER_TYPE;
                                 ft < u::k_NUM_FILLER_TYPES; u::incEnum(&ft)) {

#ifndef BSLS_LIBRARYFEATURES_HAS_CPP17_PMR_STRING
                    if (u::e_CHAR_PMR_STRING == ft ||
                                               u::e_WCHAR_T_PMR_STRING == ft) {
                        continue;
                    }
#endif
                    ::memset(cBuf, 0xaf, sizeof(cBuf));
                    pc = cBuf;

                    if (veryVeryVeryVerbose) {
                        T_ P(ft);
                    }

                    if (IS_ASCII || ft <= u::k_MAX_CHAR_TYPE) {
                        switch (ft) {
                          case u::e_CONST_CHAR_STAR: {
                            pc = CPadUtil::pad(pc, padWidth, FILLER);
                          } break;
                          case u::e_CHAR_SV: {
                            pc = CPadUtil::pad(pc, padWidth, FILLER_SCV);
                          } break;
                          case u::e_CHAR_BSL_STRING: {
                            pc = CPadUtil::pad(pc, padWidth, FILLER_SCB);
                          } break;
                          case u::e_CHAR_STD_STRING: {
                            pc = CPadUtil::pad(pc, padWidth, FILLER_SCS);
                          } break;
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_PMR_STRING
                          case u::e_CHAR_PMR_STRING: {
                            pc = CPadUtil::pad(pc, padWidth, FILLER_SCP);
                          } break;
#endif
                          case u::e_CONST_WCHAR_T_STAR: {
                            pc = CPadUtil::pad(pc, padWidth, WFILLER);
                          } break;
                          case u::e_WCHAR_T_SV: {
                            pc = CPadUtil::pad(pc, padWidth, FILLER_SWV);
                          } break;
                          case u::e_WCHAR_T_BSL_STRING: {
                            pc = CPadUtil::pad(pc, padWidth, FILLER_SWB);
                          } break;
                          case u::e_WCHAR_T_STD_STRING: {
                            pc = CPadUtil::pad(pc, padWidth, FILLER_SWS);
                          } break;
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_PMR_STRING
                          case u::e_WCHAR_T_PMR_STRING: {
                            pc = CPadUtil::pad(pc, padWidth, FILLER_SWP);
                          } break;
#endif
                          default: {
                            ASSERTV(ft, 0);
                          }
                        }
                        ASSERTV(LINE, ft,
                                 pc - cBuf == static_cast<int>(CEXP.length()));
                        *pc = '\0';
                        ASSERTV(LINE, ft, CEXP == cBuf);
                    }

                    if (veryVeryVerbose) {
                        printf("t_CHAR: `wchar_t`\n");
                    }

                    ::memset(wBuf, 0xaf, sizeof(wBuf));
                    pw = wBuf;

                    if (IS_ASCII || u::k_MAX_CHAR_TYPE < ft) {
                        switch (ft) {
                          case u::e_CONST_CHAR_STAR: {
                            pw = WPadUtil::pad(pw, padWidth, FILLER);
                          } break;
                          case u::e_CHAR_SV: {
                            pw = WPadUtil::pad(pw, padWidth, FILLER_SCV);
                          } break;
                          case u::e_CHAR_BSL_STRING: {
                            pw = WPadUtil::pad(pw, padWidth, FILLER_SCB);
                          } break;
                          case u::e_CHAR_STD_STRING: {
                            pw = WPadUtil::pad(pw, padWidth, FILLER_SCS);
                          } break;
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_PMR_STRING
                          case u::e_CHAR_PMR_STRING: {
                            pw = WPadUtil::pad(pw, padWidth, FILLER_SCP);
                          } break;
#endif
                          case u::e_CONST_WCHAR_T_STAR: {
                            pw = WPadUtil::pad(pw, padWidth, WFILLER);
                          } break;
                          case u::e_WCHAR_T_SV: {
                            pw = WPadUtil::pad(pw, padWidth, FILLER_SWV);
                          } break;
                          case u::e_WCHAR_T_BSL_STRING: {
                            pw = WPadUtil::pad(pw, padWidth, FILLER_SWB);
                          } break;
                          case u::e_WCHAR_T_STD_STRING: {
                            pw = WPadUtil::pad(pw, padWidth, FILLER_SWS);
                          } break;
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_PMR_STRING
                          case u::e_WCHAR_T_PMR_STRING: {
                            pw = WPadUtil::pad(pw, padWidth, FILLER_SWP);
                          } break;
#endif
                          default: {
                            ASSERTV(ft, 0);
                          }
                        }
                        *pw = '\0';
                        bsl::string narrowStr;
                        ASSERTV(LINE, ft, pw - wBuf, WEXP.length(),
                                     CEXP.c_str(), u::narrow(&narrowStr, wBuf),
                                 pw - wBuf == static_cast<int>(WEXP.length()));
                        ASSERTV(LINE, ft, WEXP == wBuf);
                    }
                }

                {
                    // negative testing

                    bsls::AssertTestHandlerGuard hG;

                    ASSERT(0 < padWidth || CEXP.empty());
                    if (IS_ASCII) {
                        pc = cBuf;
                        ASSERT_PASS(pc = CPadUtil::pad(pc, padWidth, FILLER));
                        *pc = 0;
                        ASSERT(cBuf == CEXP);
                        pw = wBuf;
                        ASSERT_PASS(pw = WPadUtil::pad(pw, padWidth, FILLER));
                        *pw = 0;
                        ASSERT(wBuf == WEXP);
                        pc = cBuf;
                        ASSERT_PASS(pc = CPadUtil::pad(pc, padWidth, WFILLER));
                        *pc = 0;
                        ASSERT(cBuf == CEXP);
                        pw = wBuf;
                        ASSERT_PASS(pw = WPadUtil::pad(pw, padWidth, WFILLER));
                        *pw = 0;
                        ASSERT(wBuf == WEXP);
                    }
                    else {
                        pc = cBuf;
                        ASSERT_PASS(pc = CPadUtil::pad(pc, padWidth, FILLER));
                        *pc = 0;
                        ASSERT(cBuf == CEXP);
                        pw = wBuf;
                        ASSERT_PASS(pw = WPadUtil::pad(pw, padWidth, WFILLER));
                        *pw = 0;
                        ASSERT(wBuf == WEXP);
                        pw = wBuf;
                        ASSERT_FAIL(pw = WPadUtil::pad(pw,
                                                       padWidth,
                                                       FILLER));
                        BSLS_ASSERT(wBuf == pw);
                        pc = cBuf;
                        ASSERT_FAIL(pc = CPadUtil::pad(pc,
                                                       padWidth,
                                                       WFILLER));
                        BSLS_ASSERT(cBuf == pc);
                    }
                }
            }
        }
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING COMPUTEPADDING
        //
        // Concerns:
        // 1. That `computePadding` functions as designed.
        //
        // Plan:
        // 1. Populate a table with
        //    * `Category`
        //    * field width
        //    * `Alignment`
        //    * content width
        //    * expected left padding
        //    * expected right passing.
        //
        // 2. Combine `category` and `width` into a `NumericValue` value
        //    `widthValue`.
        //
        // 3. Call `computePadding`.
        //
        // 4. Verify that the return values for left and right padding match
        //    expected values from the table.
        //
        // Testing:
        //   computePadding(size_t *, size_t *, WidthValue, size_t, Alignment);
        // --------------------------------------------------------------------

        if (verbose) puts("TESTING COMPUTEPADDING\n"
                          "======================\n");

        static const struct Data {
            int               d_line;
            Category          d_category;
            int               d_width;
            Alignment         d_alignment;
            size_t            d_contentWidth;
            int               d_leftPadding;
            int               d_rightPadding;
        } DATA[] = {
            { L_, e_VALUE,     10, e_LEFT,       5, 0, 5 },
            { L_, e_VALUE,     10, e_A_DEFAULT,  5, 0, 5 },
            { L_, e_VALUE,     10, e_RIGHT,      5, 5, 0 },
            { L_, e_VALUE,     10, e_MIDDLE,     5, 2, 3 },
            { L_, e_C_DEFAULT, 10, e_LEFT,       5, 0, 0 },
            { L_, e_C_DEFAULT, 10, e_A_DEFAULT,  5, 0, 0 },
            { L_, e_C_DEFAULT, 10, e_RIGHT,      5, 0, 0 },
            { L_, e_C_DEFAULT, 10, e_MIDDLE,     5, 0, 0 },
            { L_, e_VALUE,     10, e_LEFT,      10, 0, 0 },
            { L_, e_VALUE,     10, e_A_DEFAULT, 10, 0, 0 },
            { L_, e_VALUE,     10, e_RIGHT,     10, 0, 0 },
            { L_, e_VALUE,     10, e_MIDDLE,    10, 0, 0 },
            { L_, e_VALUE,      5, e_LEFT,      10, 0, 0 },
            { L_, e_VALUE,      5, e_A_DEFAULT, 10, 0, 0 },
            { L_, e_VALUE,      5, e_RIGHT,     10, 0, 0 },
            { L_, e_VALUE,      5, e_MIDDLE,    10, 0, 0 },
        };
        enum { k_NUM_DATA = sizeof DATA / sizeof *DATA };

        for (int ti = 0; ti < 4 * k_NUM_DATA; ++ti) {
            const int       tm            = ti % k_NUM_DATA;
            const int       tk            = ti / k_NUM_DATA;
            const bool      doDefault     = tk & 1;
            const bool      doWchar_t     = tk & 2;
            const Data&     data          = DATA[tm];
            const int       LINE          = data.d_line;
            const Category  CATEGORY      = data.d_category;
            const int       WIDTH         = data.d_width;
            const Alignment ALIGNMENT     = data.d_alignment;
            const size_t    CONTENT_WIDTH = data.d_contentWidth;
            const int       LEFT_PADDING  = data.d_leftPadding;
            const int       RIGHT_PADDING = data.d_rightPadding;

            const char cat   = e_VALUE  == CATEGORY  ? 'V' : 'D';
            const char align = e_LEFT   == ALIGNMENT ? 'L'
                             : e_MIDDLE == ALIGNMENT ? 'M'
                             : e_RIGHT  == ALIGNMENT ? 'R'
                             :                         'D';

            if (veryVerbose) {
                printf("Cat: %c, Field Width: %d, Align: %c, Cont Width: %u,"
                                                       " Lpad: %u, Rpad: %u\n",
                                    cat, WIDTH, align, unsigned(CONTENT_WIDTH),
                              unsigned(LEFT_PADDING), unsigned(RIGHT_PADDING));
            }

            const NumericValue widthValue(CATEGORY, WIDTH);

            std::ptrdiff_t leftPad = 0, rightPad = 0;

            if (doDefault) {
                const Alignment DEFAULT_OVERRIDE = e_A_DEFAULT == ALIGNMENT
                                                 ? e_LEFT
                                                 : ALIGNMENT;

                if (doWchar_t) {
                    PadUtil<wchar_t>::computePadding(&leftPad,
                                                     &rightPad,
                                                     widthValue,
                                                     CONTENT_WIDTH,
                                                     e_A_DEFAULT,
                                                     DEFAULT_OVERRIDE);
                }
                else {
                    PadUtil<char>::computePadding(&leftPad,
                                                  &rightPad,
                                                  widthValue,
                                                  CONTENT_WIDTH,
                                                  e_A_DEFAULT,
                                                  DEFAULT_OVERRIDE);
                }
            }
            else {
                if (doWchar_t) {
                    PadUtil<wchar_t>::computePadding(&leftPad,
                                                     &rightPad,
                                                     widthValue,
                                                     CONTENT_WIDTH,
                                                     ALIGNMENT);
                }
                else {
                    PadUtil<char>::computePadding(&leftPad,
                                                  &rightPad,
                                                  widthValue,
                                                  CONTENT_WIDTH,
                                                  ALIGNMENT);
                }
            }

            ASSERTV(LINE, LEFT_PADDING  == leftPad);
            ASSERTV(LINE, RIGHT_PADDING == rightPad);

            {
                // negative testing

                bsls::AssertTestHandlerGuard hG;

                ASSERT_PASS(PadUtil<char>::computePadding(&leftPad,
                                                          &rightPad,
                                                          widthValue,
                                                          CONTENT_WIDTH,
                                                          e_A_DEFAULT,
                                                          e_LEFT));

                ASSERT_FAIL(PadUtil<char>::computePadding(&leftPad,
                                                          &rightPad,
                                                          widthValue,
                                                          CONTENT_WIDTH,
                                                          e_A_DEFAULT,
                                                          e_A_DEFAULT));

                ASSERT_FAIL(PadUtil<char>::computePadding(&leftPad,
                                                          &rightPad,
                                                          widthValue,
                                                          -1,
                                                          e_A_DEFAULT,
                                                          e_LEFT));
            }
        }
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //   This case exercises (but does not fully test) basic functionality.
        //
        // Concerns:
        // 1. The class is sufficiently functional to enable comprehensive
        //    testing in subsequent test cases.
        //
        // Plan:
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) puts("\nBREATHING TEST"
                          "\n==============");

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
