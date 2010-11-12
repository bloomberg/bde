// bdepu_parserimputil.h       -*-C++-*-
#ifndef INCLUDED_BDEPU_PARSERIMPUTIL
#define INCLUDED_BDEPU_PARSERIMPUTIL

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide miscellaneous low-level parsing functions.
//
//@CLASSES:
//   bdepu_ParserImpUtil: namespace for primitive utility parsing functions
//
//@AUTHOR: John Lakos (jlakos)
//
//@DESCRIPTION: This component predominantly implements three types of utility
// functions: 'parse', 'generate', and 'skip', which form the basic building
// blocks for top-down (and possibly recursive descent) parsers.  The 'parse'
// functions initialize the resulting object with the value extracted from
// the input string.  The 'generate' functions append a text representation of
// the object to a user-specified 'char' buffer.  The 'skip' functions
// position a pointer just after a specified token at the start of an input
// string.  Additional utilities include functions to convert characters to
// numbers, to replace an escape sequence in a string with the single
// character denoted by that sequence, and to determine whether some string is
// a legal C-identifier.
//
// None of the parsing functions in *this* component consume leading
// whitespace.  For a parse to succeed, the sought item must be found at the
// beginning of the input string.
//
// None of the generating functions in *this* component remove a terminating
// null character from, or add one to, the buffer into which they place the
// emitted text.
//
// The following two subsections describe the grammar defining the parsing
// rules.
//
///DEFINITION OF SYMBOLS USED IN PRODUCTION RULES
///----------------------------------------------
// The following grammar is used to specify regular expressions:
//..
//   .     A period (.) is a one-character RE (Regular Expression) that matches
//         any character except new-line.
//
//   []    A non-empty string of characters enclosed in square brackets ([]) is
//         a one-character RE that matches any single character in that string.
//         If, however, the first character of the string is a circumflex (^),
//         the one-character RE matches any character *except* new-line and the
//         remaining characters in the string.  The ^ has this special meaning
//         only if it occurs first in the string.  The minus (-) may be used
//         to indicate a range of consecutive characters; for example, [0-9]
//         is equivalent to [0123456789].  The - loses this special meaning if
//         it occurs first (after an initial ^, if any) or last in the string.
//         The right square bracket (]) does not terminate such a string when
//         it is the first character within it (after an initial ^, if any);
//         for example, []a-f] matches either a right square bracket (]) or
//         one of the ASCII letters a through f inclusive.
//
//   ^     Matches the beginning of the string.  See the paragraph above for
//         special interpretation of the '^' when it is inside a "[]".
//
//   $     Matches the end of the string.
//
//   \n    Matches a newline.
//
//   -     Within brackets the minus means through.  For example, [a-z] is
//         equivalent to [abcd...xyz].  The - can appear as itself only if used
//         as the first or last character.  For example, the character class
//         expression []-] matches the characters ] and -.
//
//   *     A regular expression followed by * means zero or more times.
//
//   +     A regular expression followed by + means one or more times.  For
//         example, [0-9]+ is equivalent to [0-9][0-9]*.
//
//   ?     A regular expression followed by ? means zero times or one time.
//
//   |     Logical OR between two expressions means one must be present.
//
//   {m} {m,} {m,u} Integer values enclosed in {} indicate the number of times
//         the preceding regular expression is to be applied.  The value m is
//         the minimum number and u is the maximum.  If only m is present (that
//         is, {m}), it indicates the exact number of times the regular
//         expression is to be applied.  The value {m,} is analogous to
//         {m,infinity}.  The plus (+) and star (*) operations are
//         equivalent to {1,} and {0,} respectively.
//
//   ( ... ) Parentheses are used for grouping.  An operator, for example, *,
//         +, {}, can work on a single character or on a regular expression
//         enclosed in parentheses.  For example, (a*(cb+)*)$.
//
//    \    Backslash can escape any of the symbols defined above, as well as
//         single characters that would be escaped in a C-style string.
//..
//
///PRODUCTION RULES FOR PARSING FUNCTIONS
///--------------------------------------
//..
// <ESCAPE_SEQUENCE> ::= <SIMPLE_ESCAPE_SEQUENCE> | <OCTAL_ESCAPE_SEQUENCE> |
//                     <HEX_ESCAPE_SEQUENCE>
// <SIMPLE_ESCAPE_SEQUENCE> ::= [\a \b \f \n \r \t \v \\ \? \' \"]
// <OCTAL_ESCAPE_SEQUENCE> ::= \<OCTAL_DIGIT>{1,3}
//      The numeric part of the value of the sequence must be in range
//      [0 .. UCHAR_MAX].  The sequence is considered complete when a first
//      character that is not an <OCTAL_DIGIT> is found, three octal digits
//      have been read, or the value obtained after parsing the next character
//      would cause the value of be more than UCHAR_MAX.
// <OCTAL_DIGIT> ::= [01234567]
// <HEX_ESCAPE_SEQUENCE>: \(x|X) <HEX_DIGIT>{1,2}
//      The numeric part of the value of the sequence must be in range
//      [0 .. UCHAR_MAX].  The sequence is considered complete when a first
//      character that is not a <HEX_DIGIT> is found, two hex digits have been
//      read or the value obtained after parsing the next character would cause
//      the value of be more than UCHAR_MAX.
// <HEX_DIGIT> ::= [0123456789abcdefABCDEF]
// <DECIMAL_DIGIT> ::= [0123456789]
// <WHITESPACE> ::= Any sequence for which 'isspace' returns true, or any
//               sequence meeting the definition of a C or C++ comment.
// <IDENTIFIER> ::= C-style identifier, i.e., [A-Za-z_][A-Za-z0-9_]*
// <NULL> ::= [Nn][Uu][Ll][Ll]
// <NUMBER> ::= <OPTIONAL_SIGN><DIGIT>+
// <POSITIVE_NUMBER> ::= <DIGIT>+
// <OPTIONAL_SIGN> ::= (+|-)?
// <DIGIT> ::= Depending on base, can include characters 0-9 and case-
//      insensitive letters.  For example, octal digit is in the range
//      [0 .. 7].
//..
///Usage
///-----
// Sometimes the length of an input is not fixed.  Consider a Comma-Separated
// List (CSL) representation of a 64-bit integer array, which can contain an
// arbitrary number of elements (note that the BDE representation of an array
// does !not! contain commas):
//..
//   [ 1, 4, 9, 16, 25 ]
//..
// A 64-bit integer is defined by the following production rules:
//..
// <DIGIT> ::= depending on base can include characters 0-9 and case-
//      insensitive letters.  For example, octal digit is in the range
//      [0 .. 7].
// <NUMBER> ::= <OPTIONAL_SIGN><DIGIT>+
// <INT64> ::= <NUMBER>
//      <INT64> must be in range
//                           [-0x8000000000000000uLL .. 0x7FFFFFFFFFFFFFFFuLL].
//..
// The following is an implementation of a 'parseInt64Array' function which
// accepts this CSL representation and loads the corresponding value into a
// 'bsl::vector<int>'.
//..
//  #include <bdepu_parserimputil.h>
//  #include <assert.h>
//
//  #define SUCCEED { *endPos = s; return 0; }
//  #define FAIL_UNLESS(c) if (!(c)) { *endPos = s; return 1; }
//
//  int parseInt64Array(const char                           **endPos,
//                      bsl::vector<bsls_PlatformUtil::Int64   *result,
//                      const char                            *s)
//      // Parse the specified string 's' for a sequence of integers matching
//      // the pattern [ (<INT64> (, <INT64>)*)? ] ignoring internal
//      // whitespace, and load into the specified 'result'  those values.
//      // Store in the specified '*endPos' the address of the non-modifiable
//      // character immediately following the successfully parsed text, or the
//      // position at which the parse failure was detected.  Return zero on
//      // success, and a non-zero value otherwise.  The behavior is undefined
//      // if any argument is 0.
//  {
//      assert(endPos);
//      assert(result);
//      assert(s);
//
//      const char *s2;         // temporary for possible next position
//      int valueCount = 0;     // number of values seen so far
//
//      result->resize(0);
//
//      FAIL_UNLESS(0 == bdepu_ParserImpUtil::skipRequiredToken(&s, s, '['));
//      bdepu_ParserImpUtil::skipWhiteSpace(&s, s);
//
//      while (1) {
//          if (0 == bdepu_ParserImpUtil::skipRequiredToken(&s2, s, ']')) {
//              s = s2;
//              SUCCEED;
//          }
//
//          if (valueCount) {
//              FAIL_UNLESS(0 == bdepu_ParserImpUtil::
//                                              skipRequiredToken(&s, s, ','));
//              bdepu_ParserImpUtil::skipWhiteSpace(&s, s);
//          }
//
//          bsls_PlatformUtil::Int64 value;
//          FAIL_UNLESS(0 == bdepu_ParserImpUtil::
//                          parseSignedInteger(&s, &value, s, 10, 0xFFFFFFFF));
//          bdepu_ParserImpUtil::skipWhiteSpace(&s, s);
//          result->push_back(value);
//          ++valueCount;
//      }
//      assert(!"Cannot possibly get here!");
//  }
//..

#ifndef INCLUDED_BDESCM_VERSION
#include <bdescm_version.h>
#endif

#ifndef INCLUDED_BDEU_CHARTYPE
#include <bdeu_chartype.h>
#endif

#ifndef INCLUDED_BSLS_PLATFORMUTIL
#include <bsls_platformutil.h>
#endif

#ifndef INCLUDED_BSL_STRING
#include <bsl_string.h>
#endif

#ifndef INCLUDED_BSL_VECTOR
#include <bsl_vector.h>
#endif

namespace BloombergLP {

                        // ==========================
                        // struct bdepu_ParserImpUtil
                        // ==========================

struct bdepu_ParserImpUtil {
    // This 'struct' provides a namespace for a set of utility functions
    // that perform low-level parsing functionality used to implement top-down
    // (including recursive descent) parsers.

  private:
    // NOT IMPLEMENTED
    static int skipRequiredCaselessToken(const char **, const char *, int);
    static int skipRequiredToken(const char **, const char *, int);
        // Detect and suppress coercion of 'int' to 'char' at compile time.

  public:
    // CLASS METHODS
    static int characterToDigit(char character, int base);
        // Determine whether the specified 'character' represents a digit
        // in the specified 'base'; return the numeric equivalent if so,
        // and -1 otherwise.

    static void compactMultibyteChars(bsl::string *inOutString);
        // Modify the contents of the specified 'inOutString' in-place so
        // that each occurrence of a multibyte escape sequence is replaced
        // with the corresponding single-byte ASCII character value.
        // Note that this compaction is done without leaving gaps, by moving
        // the remaining characters in the string leftward as needed.

    static bool isIdentifier(const bsl::string& inString);
        // Return 'true' if the specified 'inString' holds a character sequence
        // that meets the requirements for a C-style identifier, (i.e.,
        // matches the definition of '<IDENTIFIER>'), and 'false' otherwise.

                        // *** PARSE FUNCTIONS ***

    static int parseEscapeSequence(const char **endPos,
                                   char        *result,
                                   const char  *inputString);
        // Parse the specified 'inputString' for a sequence of characters
        // matching the trailing part (all but the initial backslash ('\\'))
        // of an '<ESCAPE_SEQUENCE>' and place into the specified 'result' the
        // corresponding value.  Store in the specified '*endPos' the address
        // of the non-modifiable character in 'inputString' immediately
        // following the successfully parsed text, or the position at which
        // the parse failure was detected.  Return 0 on success, and a
        // non-zero value otherwise.  The value of '*result' is unchanged if
        // a parse failure occurs.  The behavior is undefined if any argument
        // is 0.  Note that if the first three characters are octal digits
        // forming a number whose value exceeds 'UCHAR_MAX', the function will
        // consume only the initial two and return their value as the result
        // of the (successful) parse.
        //
        // A parse failure can occur for the following reason:
        //..
        //   If the escape sequence is a '<HEX_ESCAPE_SEQUENCE>', but there
        //   are no '<HEX_DIGITS>' following '\x'.
        //..

    static int parseIdentifier(const char  **endPos,
                               bsl::string  *result,
                               const char   *inputString);
        // Parse the specified 'inputString' for a sequence of characters
        // matching the production rule '<IDENTIFIER>' and place into
        // the specified 'result' the corresponding value.  Store in the
        // specified '*endPos' the address of the non-modifiable character in
        // 'inputString' immediately following the successfully parsed text,
        // or the position at which the parse failure was detected.  Return
        // 0 on success, and a non-zero value otherwise.  The value of
        // '*result' is unchanged if a parse failure occurs.  The behavior
        // is undefined if any argument is 0.

    static int parseNull(const char **endPos,
                         const char  *inputString);
        // Parse the specified 'inputString' for a sequence of characters
        // matching the production rule '<NULL>'.  Store in the specified
        // '*endPos' the address of the non-modifiable character in
        // 'inputString' immediately following the successfully parsed text,
        // or the position at which the parse failure was detected.  Return
        // 0 on success, and a non-zero value otherwise.  The behavior
        // is undefined if any argument is 0.

    static int parseSignedInteger(
                              const char                     **endPos,
                              bsls_PlatformUtil::Int64        *result,
                              const char                      *inputString,
                              int                              base,
                              const bsls_PlatformUtil::Int64   minValue,
                              const bsls_PlatformUtil::Int64   maxValue);
        // Parse the specified 'inputString' for an optional sign followed by a
        // sequence of characters representing digits in the specified 'base',
        // consuming the maximum that will form a number whose value is less
        // than or equal to the specified 'maxValue' and greater than or equal
        // to the specified 'minValue'.  'maxValue' must itself be a positive
        // integer, and 'minValue' negative (this is required to allow for
        // efficient implementation).  Place into the specified 'result' the
        // extracted value, and store in the specified '*endPos' the address
        // of the non-modifiable character in 'inputString' immediately
        // following the successfully parsed text, or the position at which
        // the parse failure was detected.  Return 0 on success, and a
        // non-zero value otherwise.  The value of '*result' is unchanged if
        // a parse failure occurs.  The behavior is undefined if any argument
        // is 0, and unless '2 <= base' and 'base <= 36', (i.e., bases where
        // digits are representable by characters '[ 0 .. 9 ]', '[ a .. z ]' or
        // '[ A .. Z ]').
        //
        // A parse failure can occur for the following reasons:
        //..
        //   1. The parsed string is not a '<NUMBER>', i.e., does not contain
        //      an optional sign followed by at least one digit.
        //   2. The first digit in 'inputString' is larger than 'maxValue' or
        //      smaller than 'minValue'.
        //   3. The first digit is not a valid number for the 'base'.
        //..

    static int parseUnsignedInteger(
                   const char                      **endPos,
                   bsls_PlatformUtil::Uint64        *result,
                   const char                       *inputString,
                   int                               base,
                   const bsls_PlatformUtil::Uint64   maxValue);
    static int parseUnsignedInteger(
                   const char                      **endPos,
                   bsls_PlatformUtil::Uint64        *result,
                   const char                       *inputString,
                   int                               base,
                   const bsls_PlatformUtil::Uint64   maxValue,
                   int                               maxNumDigits);
        // Parse the specified 'inputString' for a sequence of characters
        // representing digits in the specified 'base', consuming the maximum
        // up to the optionally specified 'maxNumDigits' that form a number
        // whose value does not exceed the specified 'maxValue'.  Place into
        // the specified 'result' the extracted value, and store in the
        // specified '*endPos' the address of the non-modifiable character in
        // 'inputString' immediately following the successfully parsed text,
        // or the position at which the parse failure was detected.  Return
        // 0 on success, and a non-zero value otherwise.  The value of
        // '*result' is unchanged if a parse failure occurs.  If
        // 'maxNumDigits' is not specified, it defaults to a number larger
        // than the number of possible digits in an unsigned 64-bit integer.
        // The behavior is undefined if any argument is 0, and unless
        // '2 < = base' and  'base <= 36' (i.e., bases where digits are
        // representable by characters '[ 0 .. 9 ]', '[ a .. z ]' or
        // '[ A .. Z ]').
        //
        // A parse failure can occur for the following reasons:
        //..
        //   1. The 'inputString' is not a '<POSITIVE_NUMBER>', i.e., does
        //      not begin with a digit.
        //   2. The first digit in 'inputString' is larger than 'maxValue'.
        //   3. The first digit is not a valid number for the 'base'.
        //..

                        // *** SKIP FUNCTIONS ***

    static int skipRequiredCaselessToken(const char **endPos,
                                         const char  *inputString,
                                         char         token);
    static int skipRequiredCaselessToken(const char **endPos,
                                         const char  *inputString,
                                         const char  *token);
        // Skip past the value of the specified 'token' in the specified
        // 'inputString', ignoring case.  Store in the specified '*endPos' the
        // address of the non-modifiable character in 'inputString'
        // immediately following the successfully matched text, or the position
        // at which the match failure was detected.  Return 0 if 'token' is
        // found, and a non-zero value otherwise.  The behavior is undefined if
        // any argument is 0.  Note that the character-token version of this
        // function may be significantly faster than the string-token version.
        //
        // A parse failure can occur for the following reason:
        //..
        //   The 'token' is not at the beginning of 'inputString'.
        //..

    static int skipRequiredToken(const char **endPos,
                                 const char  *inputString,
                                 char         token);
    static int skipRequiredToken(const char **endPos,
                                 const char  *inputString,
                                 const char  *token);
        // Skip past the value of the specified 'token' in the specified
        // 'inputString'.  Store in the specified '*endPos' the address
        // of the non-modifiable character in 'inputString' immediately
        // following the successfully matched text, or the position at
        // which the match failure was detected.  Return 0 if 'token'
        // is found, and a non-zero value otherwise.  The behavior is
        // undefined if any argument is 0.  Note that the character-token
        // version of this function may be significantly faster than the
        // string-token version.
        //
        // A parse failure can occur for the following reason:
        //..
        //   The 'token' is not at the beginning of the 'inputString'.
        //..

    static int skipWhiteSpace(const char **endPos,
                              const char  *inputString);
        // Skip over any combination of C-style comments, C++-style comments,
        // and characters for which 'isspace' returns true in the specified
        // 'inputString'.  Store in the specified '*endPos' the address of
        // the non-modifiable character in 'inputString' immediately
        // following the successfully matched text, or the position at which
        // the match failure was detected.  Return 0 on success and
        // a non-zero value otherwise.  If a C++-style comment terminates with
        // '\0', 'endPos' will point *at* the '\0' and not past it.  The
        // behavior is undefined if either argument is 0.
        //
        // A parse failure can occur for the following reason:
        //..
        //   '\0' is found before a C-style comment is terminated with '*/'.
        //..

                        // *** GENERATE FUNCTIONS

    static void generateIdentifier(bsl::vector<char> *buffer,
                                   const char        *value,
                                   int                level = 0,
                                   int                spacesPerLevel = 4);
        // Format the specified 'value' using the optionally specified
        // indentation 'level' and append the result to the specified 'buffer'.
        // If 'level' is specified, optionally specify 'spacesPerLevel'.  The
        // formatted identifier will be indented by (level * spacesPerLevel)
        // spaces.  Making 'level' negative suppresses indentation.  The
        // behavior is undefined if 'buffer' or 'value' is 0, and unless
        // '0 <= spacesPerLevel'.  Note that this method does not check the
        // validity of the 'value', i.e., does not ensure that 'value' conforms
        // to the definition of '<IDENTIFIER>'.  A string generated for an
        // invalid identifier will fail to re-parse as an identifier should
        // that be attempted, though it might be parsable as an ordinary
        // '<STRING>' (see 'bdepu_typesparserimputil').

    static void generateIndentation(bsl::vector<char> *buffer,
                                    int                level = 0,
                                    int                spacesPerLevel = 4);
        // Append to the specified 'buffer' a number of spaces equal to the
        // product of 'level' and 'spacesPerLevel'.  The behavior is
        // undefined if 'buffer' is 0, and unless '0 <= level' and
        // '0 <= spacesPerLevel'.

    static void generateNull(bsl::vector<char> *buffer);
        // Append the string "NULL" to the specified 'buffer'.  The behavior is
        // undefined if 'buffer' is 0.
};

// ============================================================================
//                      INLINE FUNCTION DEFINITIONS
// ============================================================================

                        // --------------------------
                        // struct bdepu_ParserImpUtil
                        // --------------------------

// CLASS METHODS
inline
int bdepu_ParserImpUtil::skipRequiredCaselessToken(const char **endPos,
                                                   const char  *inputString,
                                                   char         token)
{
    enum { BDEPU_SUCCESS = 0, BDEPU_FAILURE = 1 };

    int result = BDEPU_FAILURE;
    *endPos = inputString;
    if (bdeu_CharType::toUpper(token) == bdeu_CharType::toUpper(**endPos)) {
        ++*endPos;
        result = BDEPU_SUCCESS;
    }

    return result;
}

inline
int bdepu_ParserImpUtil::skipRequiredToken(const char **endPos,
                                           const char  *inputString,
                                           char         token)
{
    enum { BDEPU_SUCCESS = 0, BDEPU_FAILURE = 1 };

    int result = BDEPU_FAILURE;
    *endPos = inputString;
    if (token == **endPos) {
        ++*endPos;
        result = BDEPU_SUCCESS;
    }

    return result;
}

inline
void bdepu_ParserImpUtil::generateIndentation(bsl::vector<char> *buffer,
                                              int               level,
                                              int               spacesPerLevel)
{
    buffer->resize(buffer->size() + level * spacesPerLevel, ' ');
}

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2003
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
