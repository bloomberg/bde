// bdlb_chartype.t.cpp                                                -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <bdlb_chartype.h>

#include <bslim_testutil.h>

#include <bsl_iostream.h>
#include <bsl_cstdlib.h>   // 'bsl::atoi'
#include <bsl_cstring.h>   // 'bsl::memset'
#include <bsl_cctype.h>    // 'bsl::isprint', 'bsl::toupper', etc.
#include <bsl_sstream.h>

using namespace BloombergLP;
using namespace bsl;  // automatically added by script

// ============================================================================
//                             TEST PLAN
// ----------------------------------------------------------------------------
//                              Overview
//                              --------
// Almost all of the behavior in this component is implemented via tables.
// Moreover, this component has no state, and the data it provides is
// delightfully finite.  Lastly, much of the functionality being implemented
// can be found in the standard C library: '#include <cctype>'.  Our general
// test approach will be to make liberal use of helper functions, oracles, and
// loop-based "area" testing to verify (exhaustively) that each entry in every
// table is correct.  Note that the redundant methods taking a category
// argument are all implemented in terms of separately assigned tables; hence,
// direct testing of each of these functions must be exhaustive as well.
// ----------------------------------------------------------------------------
// TYPES
// [ 3] enum Category;
// [ 3] enum { NUM_CATEGORIES };
//
// CLASS METHODS
// [ 5] bool isUpper(char character);
// [ 5] bool isLower(char character);
// [ 5] bool isAlpha(char character);
// [ 5] bool isDigit(char character);
// [ 5] bool isXdigit(char character);
// [ 5] bool isAlnum(char character);
// [ 5] bool isSpace(char character);
// [ 5] bool isPrint(char character);
// [ 5] bool isGraph(char character);
// [ 5] bool isPunct(char character);
// [ 5] bool isCntrl(char character);
// [ 5] bool isAscii(char character);
// [ 5] bool isIdent(char character);
// [ 5] bool isAlund(char character);
// [ 5] bool isAll(char character);
// [ 5] bool isNone(char character);
// [ 5] bool isCategory(char c, bdlb::CharType::Category category);
// [ 6] const char *stringUpper();
// [ 6] const char *stringLower();
// [ 6] const char *stringAlpha();
// [ 6] const char *stringDigit();
// [ 6] const char *stringXdigit();
// [ 6] const char *stringAlnum();
// [ 6] const char *stringSpace();
// [ 6] const char *stringPrint();
// [ 6] const char *stringGraph();
// [ 6] const char *stringPunct();
// [ 6] const char *stringCntrl();
// [ 6] const char *stringAscii();
// [ 6] const char *stringIdent();
// [ 6] const char *stringAlund();
// [ 6] const char *stringAll();
// [ 6] const char *stringNone();
// [ 6] int numUpper();
// [ 6] int numLower();
// [ 6] int numAlpha();
// [ 6] int numDigit();
// [ 6] int numXdigit();
// [ 6] int numAlnum();
// [ 6] int numSpace();
// [ 6] int numPrint();
// [ 6] int numGraph();
// [ 6] int numPunct();
// [ 6] int numCntrl();
// [ 6] int numAscii();
// [ 6] int numIdent();
// [ 6] int numAlund();
// [ 6] int numAll();
// [ 6] int numNone();
// [ 6] int numCategory(bdlb::CharType::Category category);
// [ 4] const char *toAscii(bdlb::CharType::Category category);
// [ 5] char toLower(char input);
// [ 5] char toUpper(char input);
//
// FREE OPERATORS
// [ 4] operator<<(bsl::ostream& out, bdlb::CharType::Category category);
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 7] USAGE EXAMPLE
// [ 2] Test helper functions match equivalent functions in '<cctype>'.
// [ 2] The detailed tabular documentation in the header is accurate.
// ----------------------------------------------------------------------------

// ============================================================================
//                     STANDARD BDE ASSERT TEST FUNCTION
// ----------------------------------------------------------------------------

namespace {

int testStatus = 0;

void aSsErT(bool condition, const char *message, int line)
{
    if (condition) {
        cout << "Error " __FILE__ "(" << line << "): " << message
             << "    (failed)" << endl;

        if (0 <= testStatus && testStatus <= 100) {
            ++testStatus;
        }
    }
}

}  // close unnamed namespace

// ============================================================================
//               STANDARD BDE TEST DRIVER MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT       BSLIM_TESTUTIL_ASSERT
#define ASSERTV      BSLIM_TESTUTIL_ASSERTV

#define LOOP_ASSERT  BSLIM_TESTUTIL_LOOP_ASSERT
#define LOOP0_ASSERT BSLIM_TESTUTIL_LOOP0_ASSERT
#define LOOP1_ASSERT BSLIM_TESTUTIL_LOOP1_ASSERT
#define LOOP2_ASSERT BSLIM_TESTUTIL_LOOP2_ASSERT
#define LOOP3_ASSERT BSLIM_TESTUTIL_LOOP3_ASSERT
#define LOOP4_ASSERT BSLIM_TESTUTIL_LOOP4_ASSERT
#define LOOP5_ASSERT BSLIM_TESTUTIL_LOOP5_ASSERT
#define LOOP6_ASSERT BSLIM_TESTUTIL_LOOP6_ASSERT

#define Q            BSLIM_TESTUTIL_Q   // Quote identifier literally.
#define P            BSLIM_TESTUTIL_P   // Print identifier and value.
#define P_           BSLIM_TESTUTIL_P_  // P(X) without '\n'.
#define T_           BSLIM_TESTUTIL_T_  // Print a tab (w/o newline).
#define L_           BSLIM_TESTUTIL_L_  // current Line number

// ============================================================================
//                       GLOBAL TYPEDEFS/CONSTANTS
// ----------------------------------------------------------------------------

// ============================================================================
//                         TEST HELPER FUNCTIONS
// ----------------------------------------------------------------------------
// The functions in this section provide piece-wise continuous representations
// of the tables used to implement the corresponding functions in the component
// under test.  These functions will be used to verify consistency with the
// standard library functions provided on the given platform (C locale).  Note
// that these functions will serve to detect a machine running in some other
// locale or one implementing the ASCII character set with a non-standard
// character encoding (e.g., EBCDIC).

static bool isUpper(char c) {
    return 'A' <= c && c <= 'Z';
}

static bool isLower(char c) {
    return 'a' <= c && c <= 'z';
}

static bool isAlpha(char c) {
    return 'A' <= c && c <= 'Z'
        || 'a' <= c && c <= 'z';
}

static bool isDigit(char c) {
    return '0' <= c && c <= '9';
}

static bool isXdigit(char c) {
    return '0' <= c && c <= '9'
        || 'A' <= c && c <= 'F'
        || 'a' <= c && c <= 'f';
}

static bool isAlnum(char c) {
    return '0' <= c && c <= '9'
        || 'A' <= c && c <= 'Z'
        || 'a' <= c && c <= 'z';
}

static bool isSpace(char c) {
    return ' '  == c
        || '\n' == c
        || '\t' == c
        || '\v' == c
        || '\r' == c
        || '\f' == c;
}

static bool isPrint(char c) {
    return ' ' <= c && c < '\177';
}

static bool isGraph(char c) {
    return isPrint(c) && ' ' != c;
}

static bool isPunct(char c) {
    return isGraph(c) && !isAlnum(c);
}

static bool isCntrl(char c) {
    return '\0' <= c && c < ' '
        || '\177' == c;
}

static bool isAscii(char c) {
    return !(0x80 & c);
}

static bool isIdent(char c) {
    return isAlnum(c) || '_' == c;
}

static bool isAlund(char c) {
    return isAlpha(c) || '_' == c;
}

static bool isAll(char) {
    return 1;
}

static bool isNone(char) {
    return 0;
}

static char toLower(char c) {
    return isUpper(c) ? c + 32 : c;
}

static char toUpper(char c) {
    return isLower(c) ? c - 32 : c;
}

// ============================================================================
//             DEFINITIONAL DATA TO VERIFY DOCUMENTATION IN HEADER
// ----------------------------------------------------------------------------
// The following table mirrors the one in the header used to document which
// characters belong to the respective categories.

static const char DOC_TABLE[128][bdlb::CharType::e_NONE + 1] =
{
  //             X
  // U  L  A  D  D  A  S  P  G  P  C  A  I  A
  // P  O  L  I  I  L  P  R  R  U  N  S  D  L     N
  // P  W  P  G  G  N  A  I  A  N  T  C  E  U  A  O      char/
  // E  E  H  I  I  U  C  N  P  C  R  I  N  N  L  N      Octal
  // R  R  A  T  T  M  E  T  H  T  L  I  T  D  L  E      Code
  // -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -     ------
   { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 0 },  // 000
   { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 0 },  // 001
   { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 0 },  // 002
   { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 0 },  // 003
   { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 0 },  // 004
   { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 0 },  // 005
   { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 0 },  // 006
   { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 0 },  // 007
   { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 0 },  // 010
   { 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 1, 0, 0, 1, 0 },  // 011
   { 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 1, 0, 0, 1, 0 },  // 012
   { 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 1, 0, 0, 1, 0 },  // 013
   { 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 1, 0, 0, 1, 0 },  // 014
   { 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 1, 0, 0, 1, 0 },  // 015
   { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 0 },  // 016
   { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 0 },  // 017
   { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 0 },  // 020
   { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 0 },  // 021
   { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 0 },  // 022
   { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 0 },  // 023
   { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 0 },  // 024
   { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 0 },  // 025
   { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 0 },  // 026
   { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 0 },  // 027
   { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 0 },  // 030
   { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 0 },  // 031
   { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 0 },  // 032
   { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 0 },  // 033
   { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 0 },  // 034
   { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 0 },  // 035
   { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 0 },  // 036
   { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 0 },  // 037
   { 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 1, 0, 0, 1, 0 },  //
   { 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 1, 0, 0, 1, 0 },  // !
   { 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 1, 0, 0, 1, 0 },  // "
   { 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 1, 0, 0, 1, 0 },  // #
   { 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 1, 0, 0, 1, 0 },  // $
   { 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 1, 0, 0, 1, 0 },  // %
   { 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 1, 0, 0, 1, 0 },  // &
   { 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 1, 0, 0, 1, 0 },  // '
   { 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 1, 0, 0, 1, 0 },  // (
   { 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 1, 0, 0, 1, 0 },  // )
   { 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 1, 0, 0, 1, 0 },  // *
   { 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 1, 0, 0, 1, 0 },  // +
   { 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 1, 0, 0, 1, 0 },  // ,
   { 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 1, 0, 0, 1, 0 },  // -
   { 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 1, 0, 0, 1, 0 },  // .
   { 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 1, 0, 0, 1, 0 },  // /
   { 0, 0, 0, 1, 1, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0 },  // 0
   { 0, 0, 0, 1, 1, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0 },  // 1
   { 0, 0, 0, 1, 1, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0 },  // 2
   { 0, 0, 0, 1, 1, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0 },  // 3
   { 0, 0, 0, 1, 1, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0 },  // 4
   { 0, 0, 0, 1, 1, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0 },  // 5
   { 0, 0, 0, 1, 1, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0 },  // 6
   { 0, 0, 0, 1, 1, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0 },  // 7
   { 0, 0, 0, 1, 1, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0 },  // 8
   { 0, 0, 0, 1, 1, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0 },  // 9
   { 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 1, 0, 0, 1, 0 },  // :
   { 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 1, 0, 0, 1, 0 },  // ;
   { 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 1, 0, 0, 1, 0 },  // <
   { 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 1, 0, 0, 1, 0 },  // =
   { 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 1, 0, 0, 1, 0 },  // >
   { 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 1, 0, 0, 1, 0 },  // ?
   { 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 1, 0, 0, 1, 0 },  // @
   { 1, 0, 1, 0, 1, 1, 0, 1, 1, 0, 0, 1, 1, 1, 1, 0 },  // A
   { 1, 0, 1, 0, 1, 1, 0, 1, 1, 0, 0, 1, 1, 1, 1, 0 },  // B
   { 1, 0, 1, 0, 1, 1, 0, 1, 1, 0, 0, 1, 1, 1, 1, 0 },  // C
   { 1, 0, 1, 0, 1, 1, 0, 1, 1, 0, 0, 1, 1, 1, 1, 0 },  // D
   { 1, 0, 1, 0, 1, 1, 0, 1, 1, 0, 0, 1, 1, 1, 1, 0 },  // E
   { 1, 0, 1, 0, 1, 1, 0, 1, 1, 0, 0, 1, 1, 1, 1, 0 },  // F
   { 1, 0, 1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 1, 1, 0 },  // G
   { 1, 0, 1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 1, 1, 0 },  // H
   { 1, 0, 1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 1, 1, 0 },  // I
   { 1, 0, 1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 1, 1, 0 },  // J
   { 1, 0, 1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 1, 1, 0 },  // K
   { 1, 0, 1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 1, 1, 0 },  // L
   { 1, 0, 1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 1, 1, 0 },  // M
   { 1, 0, 1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 1, 1, 0 },  // N
   { 1, 0, 1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 1, 1, 0 },  // O
   { 1, 0, 1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 1, 1, 0 },  // P
   { 1, 0, 1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 1, 1, 0 },  // Q
   { 1, 0, 1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 1, 1, 0 },  // R
   { 1, 0, 1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 1, 1, 0 },  // S
   { 1, 0, 1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 1, 1, 0 },  // T
   { 1, 0, 1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 1, 1, 0 },  // U
   { 1, 0, 1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 1, 1, 0 },  // V
   { 1, 0, 1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 1, 1, 0 },  // W
   { 1, 0, 1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 1, 1, 0 },  // X
   { 1, 0, 1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 1, 1, 0 },  // Y
   { 1, 0, 1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 1, 1, 0 },  // Z
   { 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 1, 0, 0, 1, 0 },  // [
   { 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 1, 0, 0, 1, 0 },  //
   { 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 1, 0, 0, 1, 0 },  // ]
   { 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 1, 0, 0, 1, 0 },  // ^
   { 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 1, 1, 1, 1, 0 },  // _
   { 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 1, 0, 0, 1, 0 },  // `
   { 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 0, 1, 1, 1, 1, 0 },  // a
   { 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 0, 1, 1, 1, 1, 0 },  // b
   { 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 0, 1, 1, 1, 1, 0 },  // c
   { 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 0, 1, 1, 1, 1, 0 },  // d
   { 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 0, 1, 1, 1, 1, 0 },  // e
   { 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 0, 1, 1, 1, 1, 0 },  // f
   { 0, 1, 1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 1, 1, 0 },  // g
   { 0, 1, 1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 1, 1, 0 },  // h
   { 0, 1, 1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 1, 1, 0 },  // i
   { 0, 1, 1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 1, 1, 0 },  // j
   { 0, 1, 1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 1, 1, 0 },  // k
   { 0, 1, 1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 1, 1, 0 },  // l
   { 0, 1, 1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 1, 1, 0 },  // m
   { 0, 1, 1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 1, 1, 0 },  // n
   { 0, 1, 1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 1, 1, 0 },  // o
   { 0, 1, 1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 1, 1, 0 },  // p
   { 0, 1, 1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 1, 1, 0 },  // q
   { 0, 1, 1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 1, 1, 0 },  // r
   { 0, 1, 1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 1, 1, 0 },  // s
   { 0, 1, 1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 1, 1, 0 },  // t
   { 0, 1, 1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 1, 1, 0 },  // u
   { 0, 1, 1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 1, 1, 0 },  // v
   { 0, 1, 1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 1, 1, 0 },  // w
   { 0, 1, 1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 1, 1, 0 },  // x
   { 0, 1, 1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 1, 1, 0 },  // y
   { 0, 1, 1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 1, 1, 0 },  // z
   { 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 1, 0, 0, 1, 0 },  // {
   { 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 1, 0, 0, 1, 0 },  // |
   { 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 1, 0, 0, 1, 0 },  // }
   { 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 1, 0, 0, 1, 0 },  // ~
   { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 0 },  // 177
};

// ============================================================================
//                       SUPPORT FOR USAGE EXAMPLE
// ----------------------------------------------------------------------------

///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Validating C-Style Identifiers
///- - - - - - - - - - - - - - - - - - - - -
// The character category extensions 'IDENT' and 'ALUND' are particularly
// useful for parsing C-style identifier names as described by the following
// regular expression:
//..
//  [A-Za-z_]([A-Za-z0-9_])*
//..
// The first character is required and must be in category 'ALUND'.  All
// subsequent characters are optional and must be in category 'IDENT':
//..
    bool isIdentifier(const char *token)
        // Return 'true' if the specified 'token' conforms to the requirements
        // of a C-style identifier, and 'false' otherwise.
    {
        ASSERT(token);

        if (!bdlb::CharType::isAlund(*token)) {
            return false; // bad required first character             // RETURN
        }

        for (const char *p = token + 1; *p; ++p) {
            if (!bdlb::CharType::isIdent(*p)) {
                return false; // bad optional subsequent character    // RETURN
            }
        }

        return true;
    }
//..

// ============================================================================
//                              MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int            test = argc > 1 ? atoi(argv[1]) : 0;
    int         verbose = argc > 2;
    int     veryVerbose = argc > 3;
    int veryVeryVerbose = argc > 4;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:  // Zero is always the leading case.
      case 7: {
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

        if (verbose) cout << endl
                          << "USAGE EXAMPLE" << endl
                          << "=============" << endl;

        if (verbose) cout << "\nTest 'isIdentifier' function." << endl;
        {
            static const struct {
                int         d_lineNum;   // source line number
                bool        d_expected;  // expected value
                const char *d_input_p;   // input string

            } DATA[] = {
                //lin  exp      input
                //---  ---      ----------------
                { L_,   0,      ""              },      // length 0

                { L_,   0,      " "             },      // length 1
                { L_,   0,      "1"             },
                { L_,   0,      "."             },
                { L_,   1,      "a"             },
                { L_,   1,      "A"             },
                { L_,   1,      "_"             },

                { L_,   0,      "a "            },      // length 2
                { L_,   0,      " A"            },
                { L_,   0,      "A."            },
                { L_,   0,      "9_"            },
                { L_,   1,      "_9"            },
                { L_,   1,      "a1"            },
                { L_,   1,      "aa"            },
                { L_,   1,      "aZ"            },

                { L_,   0,      "ab "           },      // length 3
                { L_,   0,      ".ab"           },
                { L_,   0,      "a.b"           },
                { L_,   0,      "ab."           },
                { L_,   1,      "___"           },
                { L_,   1,      "ab_"           },
                { L_,   1,      "_11"           },
                { L_,   1,      "ABC"           },

                { L_,   0,      "0123456789"    },      // length 10
                { L_,   0,      "A1.3456789"    },
                { L_,   0,      "A1234 6789"    },
                { L_,   0,      "A123456?89"    },
                { L_,   0,      "012345678+"    },
                { L_,   1,      "A123456789"    },
            };

            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            // MAIN TEST-TABLE LOOP

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int         LINE  = DATA[ti].d_lineNum;
                const bool        EXP   = DATA[ti].d_expected;
                const char *const INPUT = DATA[ti].d_input_p;

                if (veryVerbose) {
                    cout << "\n--------------------------------------" << endl;
                    P_(LINE) P_(EXP) P(INPUT)
                }

                int result = isIdentifier(INPUT);

                LOOP4_ASSERT(LINE, EXP, result, INPUT, EXP == result);

            }
        }

      } break;
      case 6: {
        // --------------------------------------------------------------------
        // VERIFY COUNT AND STRING FUNCTIONS
        //   Ensure 'numCategory', 'stringCategory', 'numUpper', etc. are
        //   correct.
        //
        // Concerns:
        //: 1 All character counts are consistent with the tables.
        //: 2 The strings characters are consistent with the tables.
        //: 3 Strings are ordered by increasing character encodings.
        //: 4 The character at the corresponding 'count' index is null.
        //: 5 The 'numCategory' and 'stringCategory' functions are tested
        //:   exhaustively.
        //: 6 Each 'num*' and 'string*' function are tested exhaustively.
        //: 7 The results of 'stringCategory' and string are identical.
        //
        // Plan:
        //   This test case uses the Area Data Selection Method and is
        //   implemented using the Brute Force and Loop-Based Implementation
        //   Techniques
        //
        //: 1 For each of the character sets:
        //:    1 Count up the number of 1 values returned and compare that
        //:      value with the number returned by the 'numCategory' function.
        //:      1 Also compare with individual num functions.
        //:
        //:    2 Initialize a table of 256 integers to 0; increment the array
        //:      element corresponding to the code for each character in the
        //:      'stringCategory' function.  Use the isCategory method to
        //:      ensure that each character in the string belongs (and occurs
        //:      once).
        //:      1 also use the 'num*' functions and 'memcmp' to ensure that
        //:        the corresponding strings returned are the same (and using
        //:        '==' are, in fact, identical) and that both strings are null
        //:        terminated (i.e., '0 == string...[num...]').
        //:
        //: 2 For each string of each category in 'stringCategory' walk the
        //:    string and make sure that each character code is strictly
        //:    greater than the previous by converting each 'char' to
        //:    to 'unsigned char' before assigning to a signed integer
        //:    'curr' (initial previous value 'prev = -1').
        //
        // Testing:
        //   int numUpper();
        //   int numLower();
        //   int numAlpha();
        //   int numDigit();
        //   int numXdigit();
        //   int numAlnum();
        //   int numSpace();
        //   int numPrint();
        //   int numGraph();
        //   int numPunct();
        //   int numCntrl();
        //   int numAscii();
        //   int numIdent();
        //   int numAlund();
        //   int numAll();
        //   int numNone();
        //   int numCategory(bdlb::CharType::Category category);
        //
        //   const char *stringUpper();
        //   const char *stringLower();
        //   const char *stringAlpha();
        //   const char *stringDigit();
        //   const char *stringXdigit();
        //   const char *stringAlnum();
        //   const char *stringSpace();
        //   const char *stringPrint();
        //   const char *stringGraph();
        //   const char *stringPunct();
        //   const char *stringCntrl();
        //   const char *stringAscii();
        //   const char *stringIdent();
        //   const char *stringAlund();
        //   const char *stringAll();
        //   const char *stringNone();
        //   int numCategory(bdlb::CharType::Category category);
        // --------------------------------------------------------------------

        if (verbose) cout << "\n" "VERIFY COUNT AND STRING FUNCTIONS" "\n"
                                  "=================================" "\n";

        typedef bdlb::CharType Util;     // *** Short Alias

        if (verbose) cout << "\nFor each character set, check count." << endl;
        {
            for (int ci = 0; ci < Util::k_NUM_CATEGORIES; ++ci) {
                Util::Category category = Util::Category(ci);

                // Count up the number of true values in the table and compare
                // it with the corresponding value returned from 'numCategory'.

                int count = 0;
                for (int i = 0; i < 256; ++i) {
                    count += Util::isCategory(i, category);
                }

                if (veryVerbose) { T_ P_(ci) P_(category) P(count) }

                int result = Util::numCategory(category);

                LOOP3_ASSERT(category, count, result, count == result);
            }

            if (verbose) cout <<
                "\tRepeat test with individual functions." << endl;

            ASSERT(Util::numCategory(Util::e_UPPER) == Util::numUpper());
            ASSERT(Util::numCategory(Util::e_LOWER) == Util::numLower());
            ASSERT(Util::numCategory(Util::e_ALPHA) == Util::numAlpha());
            ASSERT(Util::numCategory(Util::e_DIGIT) == Util::numDigit());

            ASSERT(Util::numCategory(Util::e_XDIGIT) == Util::numXdigit());
            ASSERT(Util::numCategory(Util::e_ALNUM) == Util::numAlnum());
            ASSERT(Util::numCategory(Util::e_SPACE) == Util::numSpace());
            ASSERT(Util::numCategory(Util::e_PRINT) == Util::numPrint());

            ASSERT(Util::numCategory(Util::e_GRAPH) == Util::numGraph());
            ASSERT(Util::numCategory(Util::e_PUNCT) == Util::numPunct());
            ASSERT(Util::numCategory(Util::e_CNTRL) == Util::numCntrl());
            ASSERT(Util::numCategory(Util::e_ASCII) == Util::numAscii());

            ASSERT(Util::numCategory(Util::e_IDENT) == Util::numIdent());
            ASSERT(Util::numCategory(Util::e_ALUND) == Util::numAlund());
            ASSERT(Util::numCategory(Util::e_ALL)   == Util::numAll());
            ASSERT(Util::numCategory(Util::e_NONE)  == Util::numNone());
        }

        if (verbose) cout << "\nFor each character set, check string." << endl;
        {
            for (int ci = 0; ci < Util::k_NUM_CATEGORIES; ++ci) {
                Util::Category category = Util::Category(ci);
                int count = Util::numCategory(category);
                if (veryVerbose) { T_ P_(ci) P_(category) P(count) }

                int n[256];
                memset(n, 0, sizeof n);

                const char *s = Util::stringCategory(category);
                {
                    for (int i = 0; i < count; ++i) {
                        ++n[(unsigned char)s[i]];
                    }
                }
                {
                    for (int i = 0; i < 256; ++i) {
                        const bool isMem = Util::isCategory(i, category);
                        if (veryVeryVerbose) { T_ T_ P_(i) P(isMem) }
                        LOOP4_ASSERT(category, i, isMem, n[i], isMem == n[i]);
                    }
                }
            }

            if (verbose) cout <<
                "\tCompare category with individual-function strings." << endl;

            int n; const char *r, *e;

                              n = Util::numUpper();
                           r = Util::stringUpper();     ASSERT(0 == r[n]);
            e = Util::stringCategory(Util::e_UPPER);      ASSERT(e == r);
            ASSERT(0 == memcmp(r, e, n + 1));           // redundant

                              n = Util::numLower();
                           r = Util::stringLower();     ASSERT(0 == r[n]);
            e = Util::stringCategory(Util::e_LOWER);      ASSERT(e == r);
            ASSERT(0 == memcmp(r, e, n + 1));           // redundant

                              n = Util::numAlpha();
                           r = Util::stringAlpha();     ASSERT(0 == r[n]);
            e = Util::stringCategory(Util::e_ALPHA);      ASSERT(e == r);
            ASSERT(0 == memcmp(r, e, n + 1));           // redundant

                              n = Util::numDigit();
                           r = Util::stringDigit();     ASSERT(0 == r[n]);
            e = Util::stringCategory(Util::e_DIGIT);      ASSERT(e == r);
            ASSERT(0 == memcmp(r, e, n + 1));           // redundant

                              n = Util::numXdigit();
                           r = Util::stringXdigit();    ASSERT(0 == r[n]);
            e = Util::stringCategory(Util::e_XDIGIT);     ASSERT(e == r);
            ASSERT(0 == memcmp(r, e, n + 1));           // redundant

                              n = Util::numAlnum();
                           r = Util::stringAlnum();     ASSERT(0 == r[n]);
            e = Util::stringCategory(Util::e_ALNUM);      ASSERT(e == r);
            ASSERT(0 == memcmp(r, e, n + 1));           // redundant

                              n = Util::numSpace();
                           r = Util::stringSpace();     ASSERT(0 == r[n]);
            e = Util::stringCategory(Util::e_SPACE);      ASSERT(e == r);
            ASSERT(0 == memcmp(r, e, n + 1));           // redundant

                              n = Util::numPrint();
                           r = Util::stringPrint();     ASSERT(0 == r[n]);
            e = Util::stringCategory(Util::e_PRINT);      ASSERT(e == r);
            ASSERT(0 == memcmp(r, e, n + 1));           // redundant

                              n = Util::numGraph();
                           r = Util::stringGraph();     ASSERT(0 == r[n]);
            e = Util::stringCategory(Util::e_GRAPH);      ASSERT(e == r);
            ASSERT(0 == memcmp(r, e, n + 1));           // redundant

                              n = Util::numPunct();
                           r = Util::stringPunct();     ASSERT(0 == r[n]);
            e = Util::stringCategory(Util::e_PUNCT);      ASSERT(e == r);
            ASSERT(0 == memcmp(r, e, n + 1));           // redundant

                              n = Util::numCntrl();
                           r = Util::stringCntrl();     ASSERT(0 == r[n]);
            e = Util::stringCategory(Util::e_CNTRL);      ASSERT(e == r);
            ASSERT(0 == memcmp(r, e, n + 1));           // redundant

                              n = Util::numAscii();
                           r = Util::stringAscii();     ASSERT(0 == r[n]);
            e = Util::stringCategory(Util::e_ASCII);      ASSERT(e == r);
            ASSERT(0 == memcmp(r, e, n + 1));           // redundant

                              n = Util::numIdent();
                           r = Util::stringIdent();     ASSERT(0 == r[n]);
            e = Util::stringCategory(Util::e_IDENT);      ASSERT(e == r);
            ASSERT(0 == memcmp(r, e, n + 1));           // redundant

                              n = Util::numAlund();
                           r = Util::stringAlund();     ASSERT(0 == r[n]);
            e = Util::stringCategory(Util::e_ALUND);      ASSERT(e == r);
            ASSERT(0 == memcmp(r, e, n + 1));           // redundant

                              n = Util::numAll();
                           r = Util::stringAll();       ASSERT(0 == r[n]);
            e = Util::stringCategory(Util::e_ALL);        ASSERT(e == r);
            ASSERT(0 == memcmp(r, e, n + 1));           // redundant

                              n = Util::numNone();
                           r = Util::stringNone();      ASSERT(0 == r[n]);
            e = Util::stringCategory(Util::e_NONE);       ASSERT(e == r);
            ASSERT(0 == memcmp(r, e, n + 1));           // redundant
        }

        if (verbose) cout << "\n"
      "For each character set, ensure character codes are strictly increasing."
                                                                          "\n";
        {
            for (int ci = 0; ci < Util::k_NUM_CATEGORIES; ++ci) {
                Util::Category category = Util::Category(ci);
                int count = Util::numCategory(category);
                if (veryVerbose) { T_ P_(ci) P_(category) P(count) }

                const char *s = Util::stringCategory(category);
                int prev = -1;

                for (int i = 0; i < count; ++i) {
                    int curr = (unsigned char) s[i];
                    if (veryVeryVerbose) { T_ T_ P_(i) P_(prev) P(curr) }
                    LOOP4_ASSERT(category, i, prev, curr, prev < curr);
                    prev = curr;
                }
            }
        }

      } break;
      case 5: {
        // --------------------------------------------------------------------
        // VERIFY IS MEMBER AND TOUPPER/TOLOWER FUNCTIONS
        //   Ensure 'isCategory', 'isUpper', 'isDigit', etc. are implemented
        //   properly.
        //
        // Concerns:
        //: 1 Every table entry is correct.
        //: 2 The 'isCategory' function is tested exhaustively.
        //
        // Plan: This test case uses the Area Data Selection Method and is
        // implemented using the Loop-Based Technique.
        //
        //: 1 For each of the table-based functions, verify that the test
        //:   helper produces the same results as the corresponding 'Utility'
        //:   function.
        //
        // Testing:
        //   bool isUpper(char character);
        //   bool isLower(char character);
        //   bool isAlpha(char character);
        //   bool isDigit(char character);
        //   bool isXdigit(char character);
        //   bool isAlnum(char character);
        //   bool isSpace(char character);
        //   bool isPrint(char character);
        //   bool isGraph(char character);
        //   bool isPunct(char character);
        //   bool isCntrl(char character);
        //   bool isAscii(char character);
        //   bool isIdent(char character);
        //   bool isAlund(char character);
        //   bool isAll(char character);
        //   bool isNone(char character);
        //
        //   bool isCategory(char c, bdlb::CharType::Category category);
        //
        //   char toLower(char input);
        //   char toUpper(char input);
        // --------------------------------------------------------------------

        if (verbose) cout << "\n"
                  "VERIFY IS MEMBER AND TOUPPER/TOLOWER FUNCTIONS" "\n"
                  "==============================================" "\n";

        typedef bdlb::CharType Util;     // *** Short Alias

        if (verbose) cout <<
                  "\nVerify isCategory, isAlpha, etc., toLower, toUpper." "\n";
        {
            for (int i = 0; i < 256; ++i) {
                if (veryVerbose) P(i);
                LOOP_ASSERT(i, isUpper(i)  == Util::isUpper(i));
                LOOP_ASSERT(i, isLower(i)  == Util::isLower(i));
                LOOP_ASSERT(i, isDigit(i)  == Util::isDigit(i));
                LOOP_ASSERT(i, isXdigit(i) == Util::isXdigit(i));

                LOOP_ASSERT(i, isAlpha(i)  == Util::isAlpha(i));
                LOOP_ASSERT(i, isAlnum(i)  == Util::isAlnum(i));
                LOOP_ASSERT(i, isSpace(i)  == Util::isSpace(i));
                LOOP_ASSERT(i, isPrint(i)  == Util::isPrint(i));

                LOOP_ASSERT(i, isGraph(i)  == Util::isGraph(i));
                LOOP_ASSERT(i, isPunct(i)  == Util::isPunct(i));
                LOOP_ASSERT(i, isCntrl(i)  == Util::isCntrl(i));
                LOOP_ASSERT(i, isAscii(i)  == Util::isAscii(i));

                LOOP_ASSERT(i, isIdent(i)  == Util::isIdent(i));
                LOOP_ASSERT(i, isAlund(i)  == Util::isAlund(i));
                LOOP_ASSERT(i, isAll(i)    == Util::isAll(i));
                LOOP_ASSERT(i, isNone(i)   == Util::isNone(i));

                typedef bdlb::CharType U;        // *** Very Short Alias

                LOOP_ASSERT(i, isUpper(i)  == Util::isCategory(i,
                                                               U::e_UPPER));
                LOOP_ASSERT(i, isLower(i)  == Util::isCategory(i,
                                                               U::e_LOWER));
                LOOP_ASSERT(i, isDigit(i)  == Util::isCategory(i,
                                                               U::e_DIGIT));
                LOOP_ASSERT(i, isXdigit(i) == Util::isCategory(i,
                                                              U::e_XDIGIT));

                LOOP_ASSERT(i, isAlpha(i)  == Util::isCategory(i,
                                                               U::e_ALPHA));
                LOOP_ASSERT(i, isAlnum(i)  == Util::isCategory(i,
                                                               U::e_ALNUM));
                LOOP_ASSERT(i, isSpace(i)  == Util::isCategory(i,
                                                               U::e_SPACE));
                LOOP_ASSERT(i, isPrint(i)  == Util::isCategory(i,
                                                               U::e_PRINT));

                LOOP_ASSERT(i, isGraph(i)  == Util::isCategory(i,
                                                               U::e_GRAPH));
                LOOP_ASSERT(i, isPunct(i)  == Util::isCategory(i,
                                                               U::e_PUNCT));
                LOOP_ASSERT(i, isCntrl(i)  == Util::isCategory(i,
                                                               U::e_CNTRL));
                LOOP_ASSERT(i, isAscii(i)  == Util::isCategory(i,
                                                               U::e_ASCII));

                LOOP_ASSERT(i, isIdent(i)  == Util::isCategory(i,
                                                               U::e_IDENT));
                LOOP_ASSERT(i, isAlund(i)  == Util::isCategory(i,
                                                               U::e_ALUND));
                LOOP_ASSERT(i, isAll(i)    == Util::isCategory(i,
                                                               U::e_ALL));
                LOOP_ASSERT(i, isNone(i)   == Util::isCategory(i,
                                                               U::e_NONE));

                LOOP_ASSERT(i, toLower(i)  == Util::toLower(i));
                LOOP_ASSERT(i, toUpper(i)  == Util::toUpper(i));
            }
        }

      } break;
      case 4: {
        // --------------------------------------------------------------------
        // VERIFY ENUM OUTPUT OPERATOR AND TO-ASCII METHOD
        //   Ensure enumerated types covert to readable values.
        //
        // Concerns:
        //: 1 Each enumerator converts to the appropriate string.
        //: 2 Each enumerator prints as the appropriate string.
        //
        // Plan: This test case uses the Area Test Data Selection Method and is
        // implemented using the Brute-Force and Table-Based Implementation
        // Techniques.
        //: 1 Assert the correct string value for each of these enumerators.
        //: 2 Write each value to a buffer and verify its contents.
        //
        // Testing:
        //   const char *toAscii(bdlb::CharType::Category category);
        //   operator<<(bsl::ostream& out, bdlb::CharType::Category category);
        // --------------------------------------------------------------------

        if (verbose) cout << "\n"
                        "VERIFY ENUM OUTPUT OPERATOR AND TO-ASCII METHOD" "\n"
                        "===============================================" "\n";

        typedef bdlb::CharType Util;     // *** Short Alias

        if (verbose) cout << "\nVerify bdlb::CharType::toAscii." << endl;
        {
            ASSERT(0 == strcmp("UPPER",  Util::toAscii(Util::e_UPPER)));
            ASSERT(0 == strcmp("LOWER",  Util::toAscii(Util::e_LOWER)));
            ASSERT(0 == strcmp("ALPHA",  Util::toAscii(Util::e_ALPHA)));
            ASSERT(0 == strcmp("DIGIT",  Util::toAscii(Util::e_DIGIT)));
            ASSERT(0 == strcmp("XDIGIT", Util::toAscii(Util::e_XDIGIT)));
            ASSERT(0 == strcmp("ALNUM",  Util::toAscii(Util::e_ALNUM)));
            ASSERT(0 == strcmp("SPACE",  Util::toAscii(Util::e_SPACE)));
            ASSERT(0 == strcmp("PRINT",  Util::toAscii(Util::e_PRINT)));
            ASSERT(0 == strcmp("GRAPH",  Util::toAscii(Util::e_GRAPH)));
            ASSERT(0 == strcmp("PUNCT",  Util::toAscii(Util::e_PUNCT)));
            ASSERT(0 == strcmp("CNTRL",  Util::toAscii(Util::e_CNTRL)));
            ASSERT(0 == strcmp("ASCII",  Util::toAscii(Util::e_ASCII)));
            ASSERT(0 == strcmp("IDENT",  Util::toAscii(Util::e_IDENT)));
            ASSERT(0 == strcmp("ALUND",  Util::toAscii(Util::e_ALUND)));
            ASSERT(0 == strcmp("ALL",    Util::toAscii(Util::e_ALL)));
            ASSERT(0 == strcmp("NONE",   Util::toAscii(Util::e_NONE)));
        }

        if (verbose) cout <<
               "\nVerify operator<<(bdlb::CharType::Category) (ostream)." "\n";
        {
            static const struct {
                int            d_lineNum;    // source line number
                Util::Category d_input;      // enumerator to be printed
                const char    *d_fmt_p;      // expected output format
            } DATA[] = {

                //      Input             Output
                //L#    Enumerator        Format
                //--    --------------    ------
                { L_,   Util::e_UPPER,    "UPPER"         },
                { L_,   Util::e_LOWER,    "LOWER"         },
                { L_,   Util::e_ALPHA,    "ALPHA"         },
                { L_,   Util::e_DIGIT,    "DIGIT"         },

                { L_,   Util::e_XDIGIT,   "XDIGIT"        },
                { L_,   Util::e_ALNUM,    "ALNUM"         },
                { L_,   Util::e_SPACE,    "SPACE"         },
                { L_,   Util::e_PRINT,    "PRINT"         },

                { L_,   Util::e_GRAPH,    "GRAPH"         },
                { L_,   Util::e_PUNCT,    "PUNCT"         },
                { L_,   Util::e_CNTRL,    "CNTRL"         },
                { L_,   Util::e_ASCII,    "ASCII"         },

                { L_,   Util::e_IDENT,    "IDENT"         },
                { L_,   Util::e_ALUND,    "ALUND"         },
                { L_,   Util::e_ALL,      "ALL"           },
                { L_,   Util::e_NONE,     "NONE"          },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            const int SIZE = 1000; // Must be big enough to hold output string.
            const char Z1 = (char) 0xff;  // Used to represent an unset char.
            const char Z2 = 0x00;  // Value 2 used to represent an unset char.

            char mCtrlBuf1[SIZE];  memset(mCtrlBuf1, Z1, SIZE);
            char mCtrlBuf2[SIZE];  memset(mCtrlBuf2, Z2, SIZE);
            const char *CTRL_BUF1 = mCtrlBuf1;
            const char *CTRL_BUF2 = mCtrlBuf2;

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int LINE             = DATA[ti].d_lineNum;
                const Util::Category INPUT = DATA[ti].d_input;
                const char *const FMT      = DATA[ti].d_fmt_p;

                if (veryVerbose) cout   // print format BEFORE output operation
                    << "EXPECTED FORMAT: ``" << FMT << "''" << endl;

                ostringstream out1(bsl::string(CTRL_BUF1, SIZE));
                ostringstream out2(bsl::string(CTRL_BUF2, SIZE));
                out1 << INPUT << ends;  // Ensure modifiable
                out2 << INPUT << ends;  // stream is returned.

                const int SZ = strlen(FMT) + 1;
                const int REST = SIZE - SZ;
                const bool failure = 0 != memcmp(out1.str().c_str(), FMT, SZ);

                if (!veryVerbose && failure) cout << // print AFTER if needed
                    "EXPECTED FORMAT: ``" << FMT << "''" << endl;
                if (veryVerbose || failure) cout <<  // print result if needed
                    "  ACTUAL FORMAT: ``" << out1.str() << "''" << endl;

                LOOP_ASSERT(LINE, SZ < SIZE);  // Check buffer is large enough.
                LOOP_ASSERT(LINE,
                            Z1 == out1.str()[SIZE - 1]);  // Check for overrun.
                LOOP_ASSERT(LINE,
                            Z2 == out2.str()[SIZE - 1]);  // Check for overrun.
                LOOP_ASSERT(LINE, 0 == memcmp(out1.str().c_str(), FMT, SZ));
                LOOP_ASSERT(LINE, 0 == memcmp(out2.str().c_str(), FMT, SZ));
                LOOP_ASSERT(LINE, 0 == memcmp(out1.str().c_str() + SZ,
                                              CTRL_BUF1 + SZ,
                                              REST));
                LOOP_ASSERT(LINE, 0 == memcmp(out2.str().c_str() + SZ,
                                              CTRL_BUF2 + SZ,
                                              REST));
            }
        }

      } break;
      case 3: {
        // --------------------------------------------------------------------
        // VERIFY CATEGORY ENUM AND NUM_CATEGORIES
        //   Ensure that enumerated types and values are correct.
        //
        // Concerns:
        //: 1 Each enumerator has the appropriate type.
        //: 2 Each enumerator has the appropriate value.
        //: 3 'BDEU_NUM_CATEGORIES' has the appropriate value.
        //
        // Plan: This test case uses Ad Hoc Test Data Selection and is
        // implemented using the Brute Force Technique.
        //: 1 Assert the correct value for each of these enumerators, after
        //:   assigning it to a variable of type Category where appropriate.
        //
        // Testing:
        //   enum Category;
        //   enum { BDEU_NUM_CATEGORIES };
        // --------------------------------------------------------------------

        if (verbose) cout << "\n"
                           "VERIFY CATEGORY ENUM AND BDEU_NUM_CATEGORIES" "\n"
                           "============================================" "\n";

        if (verbose) cout << "\nVerify bdlb::CharType::Category." << endl;
        {
            enum bdlb::CharType::Category c;

            c = bdlb::CharType::e_UPPER;   ASSERT(0 == c);
            c = bdlb::CharType::e_LOWER;   ASSERT(1 == c);
            c = bdlb::CharType::e_ALPHA;   ASSERT(2 == c);
            c = bdlb::CharType::e_DIGIT;   ASSERT(3 == c);
            c = bdlb::CharType::e_XDIGIT;  ASSERT(4 == c);
            c = bdlb::CharType::e_ALNUM;   ASSERT(5 == c);
            c = bdlb::CharType::e_SPACE;   ASSERT(6 == c);
            c = bdlb::CharType::e_PRINT;   ASSERT(7 == c);
            c = bdlb::CharType::e_GRAPH;   ASSERT(8 == c);
            c = bdlb::CharType::e_PUNCT;   ASSERT(9 == c);
            c = bdlb::CharType::e_CNTRL;   ASSERT(10 == c);
            c = bdlb::CharType::e_ASCII;   ASSERT(11 == c);
            c = bdlb::CharType::e_IDENT;   ASSERT(12 == c);
            c = bdlb::CharType::e_ALUND;   ASSERT(13 == c);
            c = bdlb::CharType::e_ALL;     ASSERT(14 == c);
            c = bdlb::CharType::e_NONE;    ASSERT(15 == c);
        }

        if (verbose) cout << "\nVerify bdlb::CharType::k_NUM_CATEGORIES."
                          << endl;
        {
            ASSERT(16 == bdlb::CharType::k_NUM_CATEGORIES);
        }

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // VERIFY TEST APPARATUS
        //   Ensure that supporting functionality is working properly.
        //
        // Concerns:
        //: 1 The current setting for locale on this platform is "C local".
        //: 2 Standard C-library functions from '<cctype>' give same results.
        //: 3 The table defining behavior for each category is correct.
        //: 4 The value returned is 0 or 1 (even though it is declared 'bool').
        //
        // Plan: This test case uses Area Datat Selection and is implemented
        // using the Loop-Based Technique.
        //: 1 For each of the table-based functions, verify that the test
        //:   helper produces the same results as the corresponding C-library
        //:   method and that the static table column for that entry is
        //:   correct.
        //
        // Testing:
        //   Test helper functions match equivalent functions in '<cctype>'.
        //   The detailed tabular documentation in the header is accurate.
        // --------------------------------------------------------------------

        if (verbose) cout << "\n" "VERIFY TEST APPARATUS" "\n"
                                  "=====================" "\n";

        if (verbose) cout << "\nCompare test helpers with <cctype>." << endl;
        {
            for (int i = 0; i < 256; ++i) {
                if (veryVerbose) P(i);
                LOOP_ASSERT(i, isUpper(i)  == !!isupper(i));
                LOOP_ASSERT(i, isLower(i)  == !!islower(i));
                LOOP_ASSERT(i, isDigit(i)  == !!isdigit(i));
                LOOP_ASSERT(i, isXdigit(i) == !!isXdigit(i));

                LOOP_ASSERT(i, isAlpha(i)  == !!isalpha(i));
                LOOP_ASSERT(i, isAlnum(i)  == !!isalnum(i));
                LOOP_ASSERT(i, isSpace(i)  == !!isspace(i));
                LOOP_ASSERT(i, isPrint(i)  == !!isprint(i));

                LOOP_ASSERT(i, isGraph(i)  == !!isgraph(i));
                LOOP_ASSERT(i, isPunct(i)  == !!ispunct(i));
                LOOP_ASSERT(i, isCntrl(i)  == !!iscntrl(i));
                LOOP_ASSERT(i, isAscii(i)  == !!isascii(i));

                LOOP_ASSERT(i, isIdent(i)  == isalnum(i) || '_');
                LOOP_ASSERT(i, isAlund(i)  == isalpha(i) || '_');
                LOOP_ASSERT(i, isAll(i)    == 1);
                LOOP_ASSERT(i, isNone(i)   == 0);
            }
        }

        typedef bdlb::CharType Util;     // *** Short Alias

        if (verbose) cout << "\nCompare test helpers with doc table." << endl;
        {
            for (int i = 0; i < 128; ++i) {
                if (veryVerbose) P(i);
                LOOP_ASSERT(i, isUpper(i)  == DOC_TABLE[i][Util::e_UPPER]);
                LOOP_ASSERT(i, isLower(i)  == DOC_TABLE[i][Util::e_LOWER]);
                LOOP_ASSERT(i, isDigit(i)  == DOC_TABLE[i][Util::e_DIGIT]);
                LOOP_ASSERT(i, isXdigit(i) == DOC_TABLE[i][Util::e_XDIGIT]);

                LOOP_ASSERT(i, isAlpha(i)  == DOC_TABLE[i][Util::e_ALPHA]);
                LOOP_ASSERT(i, isAlnum(i)  == DOC_TABLE[i][Util::e_ALNUM]);
                LOOP_ASSERT(i, isSpace(i)  == DOC_TABLE[i][Util::e_SPACE]);
                LOOP_ASSERT(i, isPrint(i)  == DOC_TABLE[i][Util::e_PRINT]);

                LOOP_ASSERT(i, isGraph(i)  == DOC_TABLE[i][Util::e_GRAPH]);
                LOOP_ASSERT(i, isPunct(i)  == DOC_TABLE[i][Util::e_PUNCT]);
                LOOP_ASSERT(i, isCntrl(i)  == DOC_TABLE[i][Util::e_CNTRL]);
                LOOP_ASSERT(i, isAscii(i)  == DOC_TABLE[i][Util::e_ASCII]);

                LOOP_ASSERT(i, isIdent(i)  == DOC_TABLE[i][Util::e_IDENT]);
                LOOP_ASSERT(i, isAlund(i)  == DOC_TABLE[i][Util::e_ALUND]);
                LOOP_ASSERT(i, isAll(i)    == DOC_TABLE[i][Util::e_ALL]);
                LOOP_ASSERT(i, isNone(i)   == DOC_TABLE[i][Util::e_NONE]);
            }
        }

        if (verbose) cout << "\nCompare toLower with <ctype>." << endl;
        {
            for (int i = 'A'; i <= 'Z'; ++i) {
                if (veryVerbose) P(char(i));
                LOOP_ASSERT(i, toLower(i) == tolower(i));
            }
        }

        if (verbose) cout << "\nCompare toUpper with <ctype>." << endl;
        {
            for (int i = 'a'; i <= 'z'; ++i) {
                if (veryVerbose) P(char(i));
                LOOP_ASSERT(i, toUpper(i) == toupper(i));
            }
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
        //: 1 Ad hoc.
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) cout << "\n" "BREATHING TEST" "\n"
                                  "==============" "\n";

        if (verbose) cout << "\nTry a few representative functions." << endl;

        if (verbose) cout << "\tisUpper" << endl;
        ASSERT(1 == bdlb::CharType::isUpper('A'));
        ASSERT(1 == bdlb::CharType::isUpper('Z'));
        ASSERT(0 == bdlb::CharType::isUpper('a'));
        ASSERT(0 == bdlb::CharType::isUpper('5'));
        ASSERT(0 == bdlb::CharType::isUpper('.'));

        if (verbose) cout << "\tnumDigit" << endl;
        ASSERT(10 == bdlb::CharType::numDigit());

        if (verbose) cout << "\tstringDigit" << endl;
        ASSERT('0' == bdlb::CharType::stringDigit()[0]);
        ASSERT('9' == bdlb::CharType::stringDigit()[9]);

        if (verbose) cout << "\ttoLower" << endl;
        ASSERT('a' == bdlb::CharType::toLower('A'));

        if (verbose) cout << "\ttoUpper" << endl;
        ASSERT('A' == bdlb::CharType::toUpper('a'));
        ASSERT('A' == bdlb::CharType::toUpper('A'));
        ASSERT('5' == bdlb::CharType::toUpper('5'));
        ASSERT('.' == bdlb::CharType::toUpper('.'));

        typedef bdlb::CharType Util;     // *** Short Alias

        if (verbose) cout << "\ttoAscii" << endl;
        ASSERT(0 == strcmp("ALPHA", Util::toAscii(Util::e_ALPHA)));

        if (verbose) cout << "\tisCategory" << endl;
        ASSERT(1 == Util::isCategory('A', Util::e_UPPER));
        ASSERT(0 == Util::isCategory('A', Util::e_DIGIT));
        ASSERT(1 == Util::isCategory('A', Util::e_ALUND));
        ASSERT(0 == Util::isCategory('A', Util::e_CNTRL));

        if (verbose) cout << "\tnumCategory" << endl;
        ASSERT(22 == Util::numCategory(Util::e_XDIGIT));

        if (verbose) cout << "\tstringCategory" << endl;
        ASSERT('0' == Util::stringCategory(Util::e_XDIGIT)[0]);
        ASSERT('9' == Util::stringCategory(Util::e_XDIGIT)[9]);
        ASSERT('F' == Util::stringCategory(Util::e_XDIGIT)[15]);
        ASSERT('f' == Util::stringCategory(Util::e_XDIGIT)[21]);

      } break;
      default: {
        cerr << "WARNING: CASE `" << test << "' NOT FOUND." << endl;
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        cerr << "Error, non-zero test status = " << testStatus << "." << endl;
    }

    return testStatus;
}

// ----------------------------------------------------------------------------
// Copyright 2015 Bloomberg Finance L.P.
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
