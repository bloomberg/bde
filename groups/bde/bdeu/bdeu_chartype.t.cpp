// bdeu_chartype.t.cpp      -*-C++-*-

#include <bdeu_chartype.h>

#include <bdex_testinstream.h>
#include <bdex_testinstreamexception.h>
#include <bdex_testoutstream.h>

#include <bdex_outstreamfunctions.h>
#include <bdex_instreamfunctions.h>

#include <bsl_iostream.h>
#include <bsl_cstdlib.h>   // atoi()
#include <bsl_cstring.h>   // memset()
#include <bsl_cctype.h>    // isprint(), toupper(), etc.
#include <bsl_strstream.h> // ostream

using namespace BloombergLP;
using namespace bsl;  // automatically added by script

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
// Almost all of the behavior in this component is implemented via tables.
// Moreover, this component has no state, and the data it provides is
// delightfully finite.  Lastly, much of the functionality being implemented
// can be found in the standard C library: 'include' '<cctype>'.  Our general
// test approach will be to make liberal use of helper functions, oracles, and
// loop-based "area" testing to verify (exhaustively) that each entry in every
// table is correct.  Note that the redundant methods taking a category
// argument are all implemented in terms of separately assigned tables; hence,
// direct testing of each of these functions must be exhaustive as well.
//-----------------------------------------------------------------------------
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
// [ 5] bool isCategory(char character, bdeu_CharType::Category category);
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
// [ 6] int numCategory(bdeu_CharType::Category category);
// [ 4] const char *toAscii(bdeu_CharType::Category category);
// [ 5] char toLower(char input);
// [ 5] char toUpper(char input);
//
// FREE OPERATORS
// [ 4] operator<<(bsl::ostream& out, bdeu_CharType::Category category);
// [*7] TBD ..  bdex streaming of ENUM ...TBD
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST -- (developer's sandbox)
// [ 8] USAGE EXAMPLE
// [ 2] Test helper functions match equivalent functions in <cctype>.
// [ 2] The detailed tabular documentation in the header is accurate.
//-----------------------------------------------------------------------------

//==========================================================================
//                  STANDARD BDE ASSERT TEST MACRO
//--------------------------------------------------------------------------
static int testStatus = 0;

static void aSsErT(int c, const char *s, int i) {
    if (c) {
        cout << "Error " << __FILE__ << "(" << i << "): " << s
             << "    (failed)" << endl;
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}

# define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }
//--------------------------------------------------------------------------
#define LOOP_ASSERT(I,X) { \
    if (!(X)) { cout << #I << ": " << I << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP2_ASSERT(I,J,X) { \
    if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " \
        << J << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP3_ASSERT(I,J,K,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" \
              << #K << ": " << K << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP4_ASSERT(I,J,K,L,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" << \
       #K << ": " << K << "\t" << #L << ": " << L << "\n"; \
       aSsErT(1, #X, __LINE__); } }

#define LOOP5_ASSERT(I,J,K,L,M,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" << \
       #K << ": " << K << "\t" << #L << ": " << L << "\t" << \
       #M << ": " << M << "\n"; \
       aSsErT(1, #X, __LINE__); } }

#define LOOP6_ASSERT(I,J,K,L,M,N,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" << \
       #K << ": " << K << "\t" << #L << ": " << L << "\t" << \
       #M << ": " << M << "\t" << #N << ": " << N << "\n"; \
       aSsErT(1, #X, __LINE__); } }

//=============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------
#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", " << flush; // P(X) without '\n'
#define L_ __LINE__                           // current Line number
#define T_ cout << "\t" << flush;             // Print a tab (w/o newline)

//=============================================================================
//                       GLOBAL TYPEDEFS/CONSTANTS
//-----------------------------------------------------------------------------

//=============================================================================
//                         TEST HELPER FUNCTIONS
//-----------------------------------------------------------------------------
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

//=============================================================================
//             DEFINITIONAL DATA TO VERIFY DOCUMENTATION IN HEADER
//-----------------------------------------------------------------------------
// The following table mirrors the one in the header used to document which
// characters belong to the respective categories.

static const char DOC_TABLE[128][bdeu_CharType::BDEU_NONE + 1] =
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

//=============================================================================
//                       SUPPORT FOR USAGE EXAMPLE
//-----------------------------------------------------------------------------
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

        if (!bdeu_CharType::isAlund(*token)) {
            return false; // bad required first character
        }

        for (const char *p = token + 1; *p; ++p) {
            if (!bdeu_CharType::isIdent(*p)) {
                return false; // bad optional subsequent character
            }
        }

        return true;
    }
//..

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    int veryVerbose = argc > 3;
    int veryVeryVerbose = argc > 4;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:  // Zero is always the leading case.
      case 8: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //   Ensure that the function (implemented above) works as advertized.
        //
        // Concerns:
        //   - That the function isIdentifier works as advertized.
        //
        // Plan:
        //   - Try a few examples ordered by length.
        //      + consider boundary conditions
        //      + ident/non-ident characters
        //
        // Tactics:
        //   - Ad Hoc Data Selection Method
        //   - Table-Based Implementation Technique
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
                int d_lineNum;           // source line number
                bool d_expected;         // expected value
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
                const int LINE          = DATA[ti].d_lineNum;
                const bool EXP          = DATA[ti].d_expected;
                const char *const INPUT = DATA[ti].d_input_p;

                if (veryVerbose) {
                    cout << "\n--------------------------------------" << endl;
                    P_(LINE) P_(EXP) P(INPUT)
                }

                int result = isIdentifier(INPUT);

                LOOP4_ASSERT(LINE, EXP, result, INPUT, EXP == result);

            } // end for ti
        } // end block

      } break;
      case 7: {
        // --------------------------------------------------------------------
        // VERIFY BDEX STREAMING OF CATEGORY ENUM
        //   The 'bdex' streaming concerns for this 'enum' component are
        //   standard.  We thoroughly test "normal" functionality using the
        //   overloaded '<<' and '>>' free operators.  We next step through
        //   the sequence of possible "abnormal" stream states (empty, invalid,
        //   incomplete, and corrupted), appropriately selecting data sets as
        //   described below.  In all cases, exception neutrality is confirmed
        //   using the specially instrumented 'bdex_TestInStream' and a pair of
        //   standard macros, 'BEGIN_BDEX_EXCEPTION_TEST' and
        //   'END_BDEX_EXCEPTION_TEST', which configure the
        //   'bdex_TestInStream' object appropriately in a loop.
        //
        // Plan:
        //   Let L represents the number of valid enumerator values.
        //   Let S represent the set of consecutive integers { 1 .. L }
        //   Let T represent the set of consecutive integers { 0 .. L + 1 }
        //
        //   VALID STREAMS
        //     Verify that each valid enumerator value in S can be written to
        //     and successfully read from a valid 'bdex' data stream into an
        //     instance of the enumeration with any initial value in T leaving
        //     the stream in a valid state.
        //
        //   EMPTY AND INVALID STREAMS
        //     For each valid and invalid initial enumerator value in T,
        //     create an instance of the enumeration and attempt to stream
        //     into it from an empty and then invalid stream.  Verify that the
        //     instance has its initial value, and that the stream is invalid.
        //
        //   INCOMPLETE (BUT OTHERWISE VALID) DATA
        //     Write 3 identical valid enumerator values to an output stream
        //     buffer, which will then be of total length N.  For each partial
        //     stream length from 0 to N - 1, construct an input stream and
        //     attempt to read into enumerator instances initialized with 3
        //     other identical values.  Verify values of instances that are
        //     successfully modified, partially modified (and therefore reset
        //     to the default value), or left entirely unmodified.  Also verify
        //     that the stream becomes invalid immediately after the first
        //     incomplete read.
        //
        //   CORRUPTED DATA
        //     Use the underlying stream package to simulate an instance of a
        //     typical valid (control) stream and verify that it can be
        //     streamed in successfully.  Then for each of the two data fields
        //     in the stream (beginning with the version number), provide two
        //     similar tests with the data field corrupted ("too small" and
        //     "too large").  After each test, verify the instance has the
        //     default value, and that the input stream has gone invalid.
        //
        // Testing:
        //   operator>>(bdex_InStream&, bdem_ElemType::Type& rhs);
        //   operator<<(bdex_OutStream&, bdem_ElemType::Type rhs);
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "Testing Streaming Operators" << endl
                                  << "===========================" << endl;

        const int NUM_ENUMS = bdeu_CharType::BDEU_NUM_CATEGORIES;
        typedef bdeu_CharType::Category Enum;

        // --------------------------------------------------------------------

        if (verbose) cout <<
            "\nTesting ('<<') and ('>>') on valid streams and data." << endl;
        if (verbose) cout << "\tFor normal (correct) conditions." << endl;
        {
            for (int i = 0; i < NUM_ENUMS; ++i) {
                const Enum X = Enum(i);  if (veryVerbose) { P_(i);  P(X); }
                bdex_TestOutStream out;
                const int VERSION = 1;
                out.putVersion(VERSION);
                bdex_OutStreamFunctions::streamOut(out, X, VERSION);
                const char *const OD  = out.data();
                const int         LOD = out.length();

                // Verify that each new value overwrites every old value
                // and that the input stream is emptied, but remains valid.

                for (int j = 1; j <= NUM_ENUMS; ++j) {
                    bdex_TestInStream in(OD, LOD);
                    bdex_TestInStream &testInStream = in;
                    LOOP2_ASSERT(i, j, in);  LOOP2_ASSERT(i, j, !in.isEmpty());

                    Enum t = Enum(j);
                  BEGIN_BDEX_EXCEPTION_TEST { in.reset();
                    LOOP2_ASSERT(i, j, X == t == (i == j));
                    int version;
                    in.getVersion(version);
                    ASSERT(VERSION == version);
                    bdex_InStreamFunctions::streamIn(in, t, version);
                  } END_BDEX_EXCEPTION_TEST
                    LOOP2_ASSERT(i, j, X == t);
                    LOOP2_ASSERT(i, j, in);  LOOP2_ASSERT(i, j, in.isEmpty());
                }
            }
        }

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout << "\tOn empty and invalid streams." << endl;
        {
            bdex_TestOutStream out;
            const char *const  OD = out.data();
            const int         LOD = out.length();
            ASSERT(0 == LOD);

            for (int i = 0; i <= NUM_ENUMS + 1; ++i) {
                bdex_TestInStream in(OD, LOD);
                bdex_TestInStream &testInStream = in;
                LOOP_ASSERT(i, in);  LOOP_ASSERT(i, in.isEmpty());

                // Ensure that reading from an empty or invalid input stream
                // leaves the stream invalid and the target object unchanged.

                const int VERSION = 1;
                const Enum X = Enum(i);  Enum t(X);  LOOP_ASSERT(i, X == t);
              BEGIN_BDEX_EXCEPTION_TEST { in.reset();
                bdex_InStreamFunctions::streamIn(in, t, VERSION);
                LOOP_ASSERT(i, !in);     LOOP_ASSERT(i, X == t);
                bdex_InStreamFunctions::streamIn(in, t, VERSION);
                LOOP_ASSERT(i, !in);     LOOP_ASSERT(i, X == t);
              } END_BDEX_EXCEPTION_TEST
            }
        }

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout <<
             "\tOn incomplete (but otherwise valid) data." << endl;
        {
            const Enum W1 = Enum(0), X1 = Enum(1), Y1 = Enum(2);
            const Enum W2 = Enum(3), X2 = Enum(4), Y2 = Enum(5);
            const Enum W3 = Enum(6), X3 = Enum(7), Y3 = Enum(8);

            const int VERSION = 1;
            bdex_TestOutStream out;
            bdex_OutStreamFunctions::streamOut(out, X1, VERSION);
            const int LOD1 = out.length();
            bdex_OutStreamFunctions::streamOut(out, X2, VERSION);
            const int LOD2 = out.length();
            bdex_OutStreamFunctions::streamOut(out, X3, VERSION);
            const int LOD  = out.length();
            const char *const OD = out.data();

            for (int i = 0; i < LOD; ++i) {
                bdex_TestInStream in(OD, i);
                bdex_TestInStream &testInStream = in;
                in.setSuppressVersionCheck(1);
              BEGIN_BDEX_EXCEPTION_TEST { in.reset();
                LOOP_ASSERT(i, in); LOOP_ASSERT(i, !i == in.isEmpty());
                Enum t1(W1), t2(W2), t3(W3);

                if (i < LOD1) {
                    bdex_InStreamFunctions::streamIn(in, t1, VERSION);
                    LOOP_ASSERT(i, !in);
                    if (0 == i) LOOP_ASSERT(i, W1 == t1);
                    bdex_InStreamFunctions::streamIn(in, t2, VERSION);
                    LOOP_ASSERT(i, !in);  LOOP_ASSERT(i, W2 == t2);
                    bdex_InStreamFunctions::streamIn(in, t3, VERSION);
                    LOOP_ASSERT(i, !in);  LOOP_ASSERT(i, W3 == t3);
                }
                else if (i < LOD2) {
                    bdex_InStreamFunctions::streamIn(in, t1, VERSION);
                    LOOP_ASSERT(i,  in);  LOOP_ASSERT(i, X1 == t1);
                    bdex_InStreamFunctions::streamIn(in, t2, VERSION);
                    LOOP_ASSERT(i, !in);
                    if (LOD1 == i) LOOP_ASSERT(i, W2 == t2);
                    bdex_InStreamFunctions::streamIn(in, t3, VERSION);
                    LOOP_ASSERT(i, !in);  LOOP_ASSERT(i, W3 == t3);
                }
                else {
                    bdex_InStreamFunctions::streamIn(in, t1, VERSION);
                    LOOP_ASSERT(i,  in);  LOOP_ASSERT(i, X1 == t1);
                    bdex_InStreamFunctions::streamIn(in, t2, VERSION);
                    LOOP_ASSERT(i,  in);
                    bdex_InStreamFunctions::streamIn(in, t3, VERSION);
                    LOOP_ASSERT(i, !in);  LOOP_ASSERT(i, W3 == t3);
                    if (LOD2 == i) LOOP_ASSERT(i, W3 == t3);
                }

                                LOOP_ASSERT(i, Y1 != t1);
                t1 = Y1;        LOOP_ASSERT(i, Y1 == t1);

                                LOOP_ASSERT(i, Y2 != t2);
                t2 = Y2;        LOOP_ASSERT(i, Y2 == t2);

                                LOOP_ASSERT(i, Y3 != t3);
                t3 = Y3;        LOOP_ASSERT(i, Y3 == t3);
              } END_BDEX_EXCEPTION_TEST
            }
        }

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout << "\tOn corrupted data." << endl;

        const Enum W = Enum(0), X = Enum(1), Y = Enum(2);
        ASSERT(NUM_ENUMS > Y);
        // If only two enumerators, use Y = X = 1 and remove "ASSERT(Y != t)"s.

        // -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -

        if (verbose) cout << "\t\tGood stream (for control)." << endl;

        {
            const int  VERSION    = 1;
            const char enumerator = char(Y);

            bdex_TestOutStream out;
            out.putVersion(VERSION);
            bdex_OutStreamFunctions::streamOut(out, enumerator, VERSION);
            const char *const OD  = out.data();
            const int         LOD = out.length();

            Enum t(X);          ASSERT(W != t); ASSERT(X == t); ASSERT(Y != t);
            bdex_TestInStream in(OD, LOD);      ASSERT(in);

            int version;
            in.getVersion(version);
            ASSERT(version == VERSION);
            bdex_InStreamFunctions::streamIn(in, t, version);
            ASSERT(in);
            ASSERT(W != t); ASSERT(X != t); ASSERT(Y == t);
         }

        // -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -

        if (verbose) cout << "\t\tBad version number." << endl;

        {
            const int  VERSION    = -1;          // BAD: too small
            const char enumerator = char(Y);

            bdex_TestOutStream out;
            bdex_OutStreamFunctions::streamOut(out, enumerator, VERSION);
            const char *const OD  = out.data();
            const int         LOD = out.length();

            Enum t(X);          ASSERT(W != t); ASSERT(X == t); ASSERT(Y != t);
            bdex_TestInStream in(OD, LOD);     ASSERT(in);
            in.setQuiet(!veryVerbose);

            bdex_InStreamFunctions::streamIn(in, t, VERSION);
            ASSERT(!in);
            ASSERT(W != t); ASSERT(X == t); ASSERT(Y != t);
        }
        {
            const int  VERSION    = 5;           // BAD: too large
            const char enumerator = char(Y);

            bdex_TestOutStream out;
            out.putVersion(VERSION);
            bdex_OutStreamFunctions::streamOut(out, enumerator, VERSION);
            const char *const OD  = out.data();
            const int         LOD = out.length();

            Enum t(X);          ASSERT(W != t); ASSERT(X == t); ASSERT(Y != t);
            bdex_TestInStream in(OD, LOD);     ASSERT(in);
            in.setQuiet(!veryVerbose);

            int version;
            in.getVersion(version);
            ASSERT(version == VERSION);
            bdex_InStreamFunctions::streamIn(in, t, version);
            ASSERT(!in);
            ASSERT(W != t); ASSERT(X == t); ASSERT(Y != t);
        }

        // -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -

        if (verbose) cout << "\t\tBad enumerator value." << endl;

        {
            const int  VERSION    = 1;
            const char enumerator = char(-1);     // BAD: too small

            bdex_TestOutStream out;
            out.putVersion(VERSION);
            bdex_OutStreamFunctions::streamOut(out, enumerator, VERSION);
            const char *const OD =  out.data();
            const int         LOD = out.length();

            Enum t(X);          ASSERT(W != t); ASSERT(X == t); ASSERT(Y != t);
            bdex_TestInStream in(OD, LOD);      ASSERT(in);

            int version;
            in.getVersion(version);
            ASSERT(version == VERSION);
            bdex_InStreamFunctions::streamIn(in, t, version);
            ASSERT(!in);
            ASSERT(W != t); ASSERT(X == t); ASSERT(Y != t);
        }
        {
            const int  VERSION    = 1;
            const char enumerator = char(NUM_ENUMS);  // BAD: too large

            bdex_TestOutStream out;
            out.putVersion(VERSION);
            bdex_OutStreamFunctions::streamOut(out, enumerator, VERSION);
            const char *const OD  = out.data();
            const int         LOD = out.length();

            Enum t(X);          ASSERT(W != t); ASSERT(X == t); ASSERT(Y != t);
            bdex_TestInStream in(OD, LOD);      ASSERT(in);
            int version;
            in.getVersion(version);
            ASSERT(version == VERSION);
            bdex_InStreamFunctions::streamIn(in, t, version);
            ASSERT(!in);
            ASSERT(W != t); ASSERT(X == t); ASSERT(Y != t);
        }
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // VERIFY COUNT AND STRING FUNCTIONS
        //   Ensure numCategory, stringCategory, numUpper, etc are correct.
        //
        // Concerns:
        //   - That all character counts are consistent with the tables.
        //   - That the strings characters are consistent with the tables.
        //   - That strings are ordered by increasing character encodings.
        //   - That the character at the corresponding 'count' index is null.
        //   - That we test numCategory and stringCategory exhaustively.
        //   - That we test each num and string function exhaustively.
        //   - That the results of stringCategory and string are identical.
        //
        // Plan:
        //   - For each of the character sets,
        //      + count up the number of 1 values returned and compare that
        //        value with the number returned by the 'numCategory' function.
        //        - also compare with individual num functions.
        //      + Initialize a table of 256 integers to 0; increment the array
        //        element corresponding to the code for each character in the
        //        'stringCategory' function.  Use the isCategory method to
        //        ensure that each character in the string belongs (and occurs
        //        once).
        //        - also use the num functions and 'memcmp' to ensure that
        //          the corresponding strings returned are the same (and using
        //          == are, in fact, identical) and that both strings are null
        //          terminated (i.e., 0 == string...[num...]).
        //   - for each string of each category in stringCategory walk the
        //      string and make sure that each character code is strictly
        //      greater than the previous by converting each 'char' to
        //      to 'unsigned' 'char' before assigning to a signed integer
        //      'curr' (initial previous value 'prev' = -1).
        //
        // Tactics:
        //   - Area Data Selection Method
        //   - Brute Force and Loop-Based Implementation Techniques
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
        //   int numCategory(bdeu_CharType::Category category);
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
        //   int numCategory(bdeu_CharType::Category category);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                  << "VERIFY COUNT AND STRING FUNCTIONS" << endl
                  << "=================================" << endl;

        typedef bdeu_CharType Util;     // *** Short Alias

        if (verbose) cout << "\nFor each character set, check count." << endl;
        {
            for (int ci = 0; ci < Util::BDEU_NUM_CATEGORIES; ++ci) {
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

            ASSERT(Util::numCategory(Util::BDEU_UPPER) == Util::numUpper());
            ASSERT(Util::numCategory(Util::BDEU_LOWER) == Util::numLower());
            ASSERT(Util::numCategory(Util::BDEU_ALPHA) == Util::numAlpha());
            ASSERT(Util::numCategory(Util::BDEU_DIGIT) == Util::numDigit());

            ASSERT(Util::numCategory(Util::BDEU_XDIGIT) == Util::numXdigit());
            ASSERT(Util::numCategory(Util::BDEU_ALNUM) == Util::numAlnum());
            ASSERT(Util::numCategory(Util::BDEU_SPACE) == Util::numSpace());
            ASSERT(Util::numCategory(Util::BDEU_PRINT) == Util::numPrint());

            ASSERT(Util::numCategory(Util::BDEU_GRAPH) == Util::numGraph());
            ASSERT(Util::numCategory(Util::BDEU_PUNCT) == Util::numPunct());
            ASSERT(Util::numCategory(Util::BDEU_CNTRL) == Util::numCntrl());
            ASSERT(Util::numCategory(Util::BDEU_ASCII) == Util::numAscii());

            ASSERT(Util::numCategory(Util::BDEU_IDENT) == Util::numIdent());
            ASSERT(Util::numCategory(Util::BDEU_ALUND) == Util::numAlund());
            ASSERT(Util::numCategory(Util::BDEU_ALL)   == Util::numAll());
            ASSERT(Util::numCategory(Util::BDEU_NONE)  == Util::numNone());
        }

        if (verbose) cout << "\nFor each character set, check string." << endl;
        {
            for (int ci = 0; ci < Util::BDEU_NUM_CATEGORIES; ++ci) {
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
            e = Util::stringCategory(Util::BDEU_UPPER);      ASSERT(e == r);
            ASSERT(0 == memcmp(r, e, n + 1));           // redundant

                              n = Util::numLower();
                           r = Util::stringLower();     ASSERT(0 == r[n]);
            e = Util::stringCategory(Util::BDEU_LOWER);      ASSERT(e == r);
            ASSERT(0 == memcmp(r, e, n + 1));           // redundant

                              n = Util::numAlpha();
                           r = Util::stringAlpha();     ASSERT(0 == r[n]);
            e = Util::stringCategory(Util::BDEU_ALPHA);      ASSERT(e == r);
            ASSERT(0 == memcmp(r, e, n + 1));           // redundant

                              n = Util::numDigit();
                           r = Util::stringDigit();     ASSERT(0 == r[n]);
            e = Util::stringCategory(Util::BDEU_DIGIT);      ASSERT(e == r);
            ASSERT(0 == memcmp(r, e, n + 1));           // redundant

                              n = Util::numXdigit();
                           r = Util::stringXdigit();    ASSERT(0 == r[n]);
            e = Util::stringCategory(Util::BDEU_XDIGIT);     ASSERT(e == r);
            ASSERT(0 == memcmp(r, e, n + 1));           // redundant

                              n = Util::numAlnum();
                           r = Util::stringAlnum();     ASSERT(0 == r[n]);
            e = Util::stringCategory(Util::BDEU_ALNUM);      ASSERT(e == r);
            ASSERT(0 == memcmp(r, e, n + 1));           // redundant

                              n = Util::numSpace();
                           r = Util::stringSpace();     ASSERT(0 == r[n]);
            e = Util::stringCategory(Util::BDEU_SPACE);      ASSERT(e == r);
            ASSERT(0 == memcmp(r, e, n + 1));           // redundant

                              n = Util::numPrint();
                           r = Util::stringPrint();     ASSERT(0 == r[n]);
            e = Util::stringCategory(Util::BDEU_PRINT);      ASSERT(e == r);
            ASSERT(0 == memcmp(r, e, n + 1));           // redundant

                              n = Util::numGraph();
                           r = Util::stringGraph();     ASSERT(0 == r[n]);
            e = Util::stringCategory(Util::BDEU_GRAPH);      ASSERT(e == r);
            ASSERT(0 == memcmp(r, e, n + 1));           // redundant

                              n = Util::numPunct();
                           r = Util::stringPunct();     ASSERT(0 == r[n]);
            e = Util::stringCategory(Util::BDEU_PUNCT);      ASSERT(e == r);
            ASSERT(0 == memcmp(r, e, n + 1));           // redundant

                              n = Util::numCntrl();
                           r = Util::stringCntrl();     ASSERT(0 == r[n]);
            e = Util::stringCategory(Util::BDEU_CNTRL);      ASSERT(e == r);
            ASSERT(0 == memcmp(r, e, n + 1));           // redundant

                              n = Util::numAscii();
                           r = Util::stringAscii();     ASSERT(0 == r[n]);
            e = Util::stringCategory(Util::BDEU_ASCII);      ASSERT(e == r);
            ASSERT(0 == memcmp(r, e, n + 1));           // redundant

                              n = Util::numIdent();
                           r = Util::stringIdent();     ASSERT(0 == r[n]);
            e = Util::stringCategory(Util::BDEU_IDENT);      ASSERT(e == r);
            ASSERT(0 == memcmp(r, e, n + 1));           // redundant

                              n = Util::numAlund();
                           r = Util::stringAlund();     ASSERT(0 == r[n]);
            e = Util::stringCategory(Util::BDEU_ALUND);      ASSERT(e == r);
            ASSERT(0 == memcmp(r, e, n + 1));           // redundant

                              n = Util::numAll();
                           r = Util::stringAll();       ASSERT(0 == r[n]);
            e = Util::stringCategory(Util::BDEU_ALL);        ASSERT(e == r);
            ASSERT(0 == memcmp(r, e, n + 1));           // redundant

                              n = Util::numNone();
                           r = Util::stringNone();      ASSERT(0 == r[n]);
            e = Util::stringCategory(Util::BDEU_NONE);       ASSERT(e == r);
            ASSERT(0 == memcmp(r, e, n + 1));           // redundant
        }

        if (verbose) cout <<
    "\nFor each character set, ensure character codes are strictly increasing."
                                                                       << endl;
        {
            for (int ci = 0; ci < Util::BDEU_NUM_CATEGORIES; ++ci) {
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
        //   Ensure isCategory, isUpper, isDigit etc are implemented properly.
        //
        // Concerns:
        //   - that every table entry is correct.
        //   - that we test isCategory exhaustively.
        //
        // Plan:
        //   For each of the table-based functions, verify that the test helper
        //   produces the same results as the corresponding Utility function.
        //
        // Tactics:
        //   - Area Data Selection Method
        //   - Loop-Based Implementation Technique
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
        //   bool isCategory(char character, bdeu_CharType::Category category);
        //
        //   char toLower(char input);
        //   char toUpper(char input);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                  << "VERIFY IS MEMBER AND TOUPPER/TOLOWER FUNCTIONS" << endl
                  << "==============================================" << endl;

        typedef bdeu_CharType Util;     // *** Short Alias

        if (verbose) cout <<
               "\nVerify isCategory, isAlpha, etc., toLower, toUpper." << endl;
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

                typedef bdeu_CharType U;        // *** Very Short Alias

                LOOP_ASSERT(i, isUpper(i)  == Util::isCategory(i,
                                                               U::BDEU_UPPER));
                LOOP_ASSERT(i, isLower(i)  == Util::isCategory(i,
                                                               U::BDEU_LOWER));
                LOOP_ASSERT(i, isDigit(i)  == Util::isCategory(i,
                                                               U::BDEU_DIGIT));
                LOOP_ASSERT(i, isXdigit(i) == Util::isCategory(i,
                                                              U::BDEU_XDIGIT));

                LOOP_ASSERT(i, isAlpha(i)  == Util::isCategory(i,
                                                               U::BDEU_ALPHA));
                LOOP_ASSERT(i, isAlnum(i)  == Util::isCategory(i,
                                                               U::BDEU_ALNUM));
                LOOP_ASSERT(i, isSpace(i)  == Util::isCategory(i,
                                                               U::BDEU_SPACE));
                LOOP_ASSERT(i, isPrint(i)  == Util::isCategory(i,
                                                               U::BDEU_PRINT));

                LOOP_ASSERT(i, isGraph(i)  == Util::isCategory(i,
                                                               U::BDEU_GRAPH));
                LOOP_ASSERT(i, isPunct(i)  == Util::isCategory(i,
                                                               U::BDEU_PUNCT));
                LOOP_ASSERT(i, isCntrl(i)  == Util::isCategory(i,
                                                               U::BDEU_CNTRL));
                LOOP_ASSERT(i, isAscii(i)  == Util::isCategory(i,
                                                               U::BDEU_ASCII));

                LOOP_ASSERT(i, isIdent(i)  == Util::isCategory(i,
                                                               U::BDEU_IDENT));
                LOOP_ASSERT(i, isAlund(i)  == Util::isCategory(i,
                                                               U::BDEU_ALUND));
                LOOP_ASSERT(i, isAll(i)    == Util::isCategory(i,
                                                               U::BDEU_ALL));
                LOOP_ASSERT(i, isNone(i)   == Util::isCategory(i,
                                                               U::BDEU_NONE));

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
        //   - That each enumerator converts to the appropriate string.
        //   - That each enumerator prints as the appropriate string
        //
        // Plan:
        //   - Assert the correct string value for each of these enumerators.
        //   - Write each value to a buffer and verify its contents.
        //
        // Tactics:
        //   - Area Test Data Selection Method
        //   - Brute-Force and Table-Based Implementation Techniques
        //
        // Testing:
        //   const char *toAscii(bdeu_CharType::Category category);
        //   operator<<(bsl::ostream& out, bdeu_CharType::Category category);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                  << "VERIFY ENUM OUTPUT OPERATOR AND TO-ASCII METHOD" << endl
                  << "===============================================" << endl;

        typedef bdeu_CharType Util;     // *** Short Alias

        if (verbose) cout << "\nVerify bdeu_CharType::toAscii." << endl;
        {
            ASSERT(0 == strcmp("UPPER", Util::toAscii(Util::BDEU_UPPER)));
            ASSERT(0 == strcmp("LOWER", Util::toAscii(Util::BDEU_LOWER)));
            ASSERT(0 == strcmp("ALPHA", Util::toAscii(Util::BDEU_ALPHA)));
            ASSERT(0 == strcmp("DIGIT", Util::toAscii(Util::BDEU_DIGIT)));
            ASSERT(0 == strcmp("XDIGIT", Util::toAscii(Util::BDEU_XDIGIT)));
            ASSERT(0 == strcmp("ALNUM", Util::toAscii(Util::BDEU_ALNUM)));
            ASSERT(0 == strcmp("SPACE", Util::toAscii(Util::BDEU_SPACE)));
            ASSERT(0 == strcmp("PRINT", Util::toAscii(Util::BDEU_PRINT)));
            ASSERT(0 == strcmp("GRAPH", Util::toAscii(Util::BDEU_GRAPH)));
            ASSERT(0 == strcmp("PUNCT", Util::toAscii(Util::BDEU_PUNCT)));
            ASSERT(0 == strcmp("CNTRL", Util::toAscii(Util::BDEU_CNTRL)));
            ASSERT(0 == strcmp("ASCII", Util::toAscii(Util::BDEU_ASCII)));
            ASSERT(0 == strcmp("IDENT", Util::toAscii(Util::BDEU_IDENT)));
            ASSERT(0 == strcmp("ALUND", Util::toAscii(Util::BDEU_ALUND)));
            ASSERT(0 == strcmp("ALL", Util::toAscii(Util::BDEU_ALL)));
            ASSERT(0 == strcmp("NONE", Util::toAscii(Util::BDEU_NONE)));
        }

        if (verbose) cout <<
            "\nVerify operator<<(bdeu_CharType::Category) (ostream)." << endl;
        {
            static const struct {
                int            d_lineNum;    // source line number
                Util::Category d_input;       // enumerator to be printed
                const char    *d_fmt_p;      // expected output format
            } DATA[] = {

                //        Input              Output
                //L#    Enumerator           Format
                //--    ----------           ------
                { L_,   Util::BDEU_UPPER,    "UPPER"         },
                { L_,   Util::BDEU_LOWER,    "LOWER"         },
                { L_,   Util::BDEU_ALPHA,    "ALPHA"         },
                { L_,   Util::BDEU_DIGIT,    "DIGIT"         },

                { L_,   Util::BDEU_XDIGIT,   "XDIGIT"        },
                { L_,   Util::BDEU_ALNUM,    "ALNUM"         },
                { L_,   Util::BDEU_SPACE,    "SPACE"         },
                { L_,   Util::BDEU_PRINT,    "PRINT"         },

                { L_,   Util::BDEU_GRAPH,    "GRAPH"         },
                { L_,   Util::BDEU_PUNCT,    "PUNCT"         },
                { L_,   Util::BDEU_CNTRL,    "CNTRL"         },
                { L_,   Util::BDEU_ASCII,    "ASCII"         },

                { L_,   Util::BDEU_IDENT,    "IDENT"         },
                { L_,   Util::BDEU_ALUND,    "ALUND"         },
                { L_,   Util::BDEU_ALL,      "ALL"           },
                { L_,   Util::BDEU_NONE,     "NONE"          },
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

                char buf1[SIZE], buf2[SIZE];
                memcpy(buf1, CTRL_BUF1, SIZE); // Preset buf1 to Z1 values.
                memcpy(buf2, CTRL_BUF2, SIZE); // Preset buf2 to Z2 values.

                if (veryVerbose) cout   // print format BEFORE output operation
                    << "EXPECTED FORMAT: ``" << FMT << "''" << endl;

                ostrstream out1(buf1, SIZE), out2(buf2, SIZE);
                out1 << INPUT << ends;  // Ensure modifiable
                out2 << INPUT << ends;  // stream is returned.

                const int SZ = strlen(FMT) + 1;
                const int REST = SIZE - SZ;
                const bool failure = 0 != memcmp(buf1, FMT, SZ);

                if (!veryVerbose && failure) cout << // print AFTER if needed
                    "EXPECTED FORMAT: ``" << FMT << "''" << endl;
                if (veryVerbose || failure) cout <<  // print result if needed
                    "  ACTUAL FORMAT: ``" << buf1 << "''" << endl;

                LOOP_ASSERT(LINE, SZ < SIZE);  // Check buffer is large enough.
                LOOP_ASSERT(LINE, Z1 == buf1[SIZE - 1]);  // Check for overrun.
                LOOP_ASSERT(LINE, Z2 == buf2[SIZE - 1]);  // Check for overrun.
                LOOP_ASSERT(LINE, 0 == memcmp(buf1, FMT, SZ));
                LOOP_ASSERT(LINE, 0 == memcmp(buf2, FMT, SZ));
                LOOP_ASSERT(LINE, 0 == memcmp(buf1 + SZ, CTRL_BUF1 + SZ,REST));
                LOOP_ASSERT(LINE, 0 == memcmp(buf2 + SZ, CTRL_BUF2 + SZ,REST));
            }
        }

      } break;
      case 3: {
        // --------------------------------------------------------------------
        // VERIFY CATEGORY ENUM AND NUM_CATEGORIES
        //   Ensure that enumerated types and values are correct.
        //
        // Concerns:
        //   - That each enumerator has the appropriate type.
        //   - That each enumerator has the appropriate value.
        //   - that BDEU_NUM_CATEGORIES has the appropriate value.
        //
        // Plan:
        //   - Assert the correct value for each of these enumerators, after
        //      assigning it to a variable of type Category where appropriate.
        //
        // Tactics:
        //   - Ad Hoc Test Data Selection Method
        //   - Brute Force Implementation Technique
        //
        // Testing:
        //   enum Category;
        //   enum { BDEU_NUM_CATEGORIES };
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "VERIFY CATEGORY ENUM AND BDEU_NUM_CATEGORIES"
                          << endl
                          << "============================================"
                          << endl;

        if (verbose) cout << "\nVerify bdeu_CharType::Category." << endl;
        {
            enum bdeu_CharType::Category c;

            c = bdeu_CharType::BDEU_UPPER;   ASSERT(0 == c);
            c = bdeu_CharType::BDEU_LOWER;   ASSERT(1 == c);
            c = bdeu_CharType::BDEU_ALPHA;   ASSERT(2 == c);
            c = bdeu_CharType::BDEU_DIGIT;   ASSERT(3 == c);
            c = bdeu_CharType::BDEU_XDIGIT;  ASSERT(4 == c);
            c = bdeu_CharType::BDEU_ALNUM;   ASSERT(5 == c);
            c = bdeu_CharType::BDEU_SPACE;   ASSERT(6 == c);
            c = bdeu_CharType::BDEU_PRINT;   ASSERT(7 == c);
            c = bdeu_CharType::BDEU_GRAPH;   ASSERT(8 == c);
            c = bdeu_CharType::BDEU_PUNCT;   ASSERT(9 == c);
            c = bdeu_CharType::BDEU_CNTRL;   ASSERT(10 == c);
            c = bdeu_CharType::BDEU_ASCII;   ASSERT(11 == c);
            c = bdeu_CharType::BDEU_IDENT;   ASSERT(12 == c);
            c = bdeu_CharType::BDEU_ALUND;   ASSERT(13 == c);
            c = bdeu_CharType::BDEU_ALL;     ASSERT(14 == c);
            c = bdeu_CharType::BDEU_NONE;    ASSERT(15 == c);
        }

        if (verbose) cout << "\nVerify bdeu_CharType::BDEU_NUM_CATEGORIES."
                          << endl;
        {
            ASSERT(16 == bdeu_CharType::BDEU_NUM_CATEGORIES);
        }

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // VERIFY TEST APPARATUS
        //   Ensure that supporting functionality is working properly.
        //
        // Concerns:
        //   - the current setting for locale on this platform is "C local"
        //   - std C-library functions from <cctype> give same results
        //   - the table defining behavior for each category is correct
        //   - that the value returned is 0 or 1 (even though it is dec bool)
        //
        // Plan:
        //   For each of the table-based functions, verify that the test helper
        //   produces the same results as the corresponding C-library method
        //   and that the static table column for that entry is correct.
        //
        // Tactics:
        //   - Area Data Selection Method
        //   - Loop-Based Implementation Technique
        //
        // Testing:
        //   Test helper functions match equivalent functions in <cctype>.
        //   The detailed tabular documentation in the header is accurate.
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "VERIFY TEST APPARATUS" << endl
                          << "=====================" << endl;

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

        typedef bdeu_CharType Util;     // *** Short Alias

        if (verbose) cout << "\nCompare test helpers with doc table." << endl;
        {
            for (int i = 0; i < 128; ++i) {
                if (veryVerbose) P(i);
                LOOP_ASSERT(i, isUpper(i)  == DOC_TABLE[i][Util::BDEU_UPPER]);
                LOOP_ASSERT(i, isLower(i)  == DOC_TABLE[i][Util::BDEU_LOWER]);
                LOOP_ASSERT(i, isDigit(i)  == DOC_TABLE[i][Util::BDEU_DIGIT]);
                LOOP_ASSERT(i, isXdigit(i) == DOC_TABLE[i][Util::BDEU_XDIGIT]);

                LOOP_ASSERT(i, isAlpha(i)  == DOC_TABLE[i][Util::BDEU_ALPHA]);
                LOOP_ASSERT(i, isAlnum(i)  == DOC_TABLE[i][Util::BDEU_ALNUM]);
                LOOP_ASSERT(i, isSpace(i)  == DOC_TABLE[i][Util::BDEU_SPACE]);
                LOOP_ASSERT(i, isPrint(i)  == DOC_TABLE[i][Util::BDEU_PRINT]);

                LOOP_ASSERT(i, isGraph(i)  == DOC_TABLE[i][Util::BDEU_GRAPH]);
                LOOP_ASSERT(i, isPunct(i)  == DOC_TABLE[i][Util::BDEU_PUNCT]);
                LOOP_ASSERT(i, isCntrl(i)  == DOC_TABLE[i][Util::BDEU_CNTRL]);
                LOOP_ASSERT(i, isAscii(i)  == DOC_TABLE[i][Util::BDEU_ASCII]);

                LOOP_ASSERT(i, isIdent(i)  == DOC_TABLE[i][Util::BDEU_IDENT]);
                LOOP_ASSERT(i, isAlund(i)  == DOC_TABLE[i][Util::BDEU_ALUND]);
                LOOP_ASSERT(i, isAll(i)    == DOC_TABLE[i][Util::BDEU_ALL]);
                LOOP_ASSERT(i, isNone(i)   == DOC_TABLE[i][Util::BDEU_NONE]);
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
        //   This case is available to be used as a developers sandbox.
        //
        // Concerns:
        //    None.
        //
        // Plan:
        //    Ad hoc.
        //
        // Tactics:
        //    - Ad Hoc Data Selection Method
        //    - Brute Force Implementation Technique
        //
        // Testing:
        //     BREATHING TEST -- (developer's sandbox)
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "BREATHING TEST" << endl
                          << "==============" << endl;

        if (verbose) cout << "\nTry a few representative functions." << endl;

        if (verbose) cout << "\tisUpper" << endl;
        ASSERT(1 == bdeu_CharType::isUpper('A'));
        ASSERT(1 == bdeu_CharType::isUpper('Z'));
        ASSERT(0 == bdeu_CharType::isUpper('a'));
        ASSERT(0 == bdeu_CharType::isUpper('5'));
        ASSERT(0 == bdeu_CharType::isUpper('.'));

        if (verbose) cout << "\tnumDigit" << endl;
        ASSERT(10 == bdeu_CharType::numDigit());

        if (verbose) cout << "\tstringDigit" << endl;
        ASSERT('0' == bdeu_CharType::stringDigit()[0]);
        ASSERT('9' == bdeu_CharType::stringDigit()[9]);

        if (verbose) cout << "\ttoLower" << endl;
        ASSERT('a' == bdeu_CharType::toLower('A'));

        if (verbose) cout << "\ttoUpper" << endl;
        ASSERT('A' == bdeu_CharType::toUpper('a'));
        ASSERT('A' == bdeu_CharType::toUpper('A'));
        ASSERT('5' == bdeu_CharType::toUpper('5'));
        ASSERT('.' == bdeu_CharType::toUpper('.'));

        typedef bdeu_CharType Util;     // *** Short Alias

        if (verbose) cout << "\ttoAscii" << endl;
        ASSERT(0 == strcmp("ALPHA", Util::toAscii(Util::BDEU_ALPHA)));

        if (verbose) cout << "\tisCategory" << endl;
        ASSERT(1 == Util::isCategory('A', Util::BDEU_UPPER));
        ASSERT(0 == Util::isCategory('A', Util::BDEU_DIGIT));
        ASSERT(1 == Util::isCategory('A', Util::BDEU_ALUND));
        ASSERT(0 == Util::isCategory('A', Util::BDEU_CNTRL));

        if (verbose) cout << "\tnumCategory" << endl;
        ASSERT(22 == Util::numCategory(Util::BDEU_XDIGIT));

        if (verbose) cout << "\tstringCategory" << endl;
        ASSERT('0' == Util::stringCategory(Util::BDEU_XDIGIT)[0]);
        ASSERT('9' == Util::stringCategory(Util::BDEU_XDIGIT)[9]);
        ASSERT('F' == Util::stringCategory(Util::BDEU_XDIGIT)[15]);
        ASSERT('f' == Util::stringCategory(Util::BDEU_XDIGIT)[21]);

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

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2004
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
