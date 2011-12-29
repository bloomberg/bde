// bdepu_typesparserimputil.h                                         -*-C++-*-
#ifndef INCLUDED_BDEPU_TYPESPARSERIMPUTIL
#define INCLUDED_BDEPU_TYPESPARSERIMPUTIL

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Convert fundamental and 'bdet' types to/from text representation.
//
//@CLASSES:
//  bdepu_TypesParserImpUtil: namespace for types parsing functions
//
//@AUTHOR: John Lakos (jlakos)
//
//@DESCRIPTION: The functions in this component fall into two categories --
// parsers and generators.
//
// Parsers convert textual representations of a type into an object of that
// type.  There is a parse method for each of the following types: 'char',
// 'short', 'int', 'bsls_PlatformUtil::Int64', 'float', 'double',
// 'long double', 'bdet_Date', 'bdet_DateTz', 'bdet_Time', 'bdet_TimeTz',
// 'bdet_Datetime', 'bdet_DatetimeTz', and several parse methods for
// 'bsl::string'.
//
// Generators convert objects into ASCII representations.  There is a
// 'generate' method for each type for which a 'parse' method exists.
//
// None of the parsing functions in this component consume leading whitespace.
// For a parse to succeed, the sought item must be found at the beginning of
// the input string.
//
// The various generating functions vary as to whether a null terminator is
// appended to the supplied buffer:
//..
//  o Generating functions that take a 'bsl::vector<char> *buffer' (to which
//    the generated text is appended) do not append a '\0' to 'buffer'.
//
//  o Generating functions that take a 'char *buffer' and an 'int bufferLength'
//    (indicating the length of 'buffer') append a '\0' to 'buffer' only if
//    there is sufficient capacity to do so.
//
//  o The "raw" generating functions, i.e., those which take a 'char *buffer'
//    but *no* corresponding 'int bufferLength', leave it unspecified as to
//    whether a '\0' is appended to 'buffer'.
//..
// The following two subsections describe the grammar defining the parsing
// rules.
//
///DEFINITION OF SYMBOLS USED IN PRODUCTION RULES
///----------------------------------------------
//
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
///GRAMMAR PRODUCTION RULES
///------------------------
//..
// <CHAR> ::= \'<CHAR_VAL>\'
// <CHAR_VAL> ::= [^\\] | <ESCAPE_SEQUENCE>
// <TZ> ::= (+|-) <DECIMAL_DIGIT>{4}
// <DATETIME_TZ> ::= <DATETIME><TZ>
// <DATETIME> ::= <DATE>(' ' | '_' )<TIME>
// <DATE_TZ> ::= <DATE><TZ>
// <DATE> ::= <YEAR>/<MONTH>/<DAY>
//      In addition to the correct ranges of <YEAR>, <MONTH>, and <DAY>, the
//      whole <DATE> must be valid as specified in 'bdet_Date'.
// <YEAR> ::= <DIGIT>{1,4} in range [1 .. 9999]
// <MONTH> ::= <DIGIT>{1,2} in range [1 .. 12]
// <DAY> ::= <DIGIT>{1,2} in range [1 .. 31]
// <TIME_TZ> ::= <TIME><TZ>
// <TIME> ::= <TIME1> | <TIME2> | <TIME3>
// <TIME1> ::= <HOUR>:<MINUTE>:<SECOND>.<SDECIMAL> (i.e., one to two digits
//      representing the hour followed by a colon and exactly two digits
//      representing the minute followed by a colon and exactly two digits
//      representing the second followed by a period and one to three digits
//      representing the decimal portion of a second.)  In addition to the
//      correct ranges of <HOUR>, <MINUTE>, <SECOND>, and <SDECIMAL>, the
//      whole <TIME> must be valid as specified in 'bdet_Time'.  Specifically,
//      the only valid time where <HOUR> equals 24 is when all other fields
//      equal zero.  (Note that time 24:00:00 is the internal BDE
//      representation for an unset 'bdet_Time', but is also computationally
//      equivalent to 00:00:00 and so can be "operated on" as though it
//      were simply another valid 'bdet_Time'.)
// <TIME2> ::= <HOUR>:<MINUTE>:<SECOND>  The same restrictions as for <TIME1>
//      apply here, excepting any related to <SDECIMAL>.
// <TIME3> ::= <HOUR>:<MINUTE>  The same restrictions as for <TIME2> apply
//      here, excepting any related to <SECOND>.
// <HOUR> ::= <DIGIT>{1,2}  in range [0 .. 24]
// <MINUTE> ::= <DIGIT><DIGIT>  in range [0 .. 59]
// <SECOND> ::= <DIGIT><DIGIT>  in range [0 .. 59]
// <SDECIMAL> ::= <DIGIT>{1,3}  This is the decimal portion of seconds.
// <NUMBER> ::= <OPTIONAL_SIGN><DIGIT>+
// <DECIMAL_NUMBER> ::= <OPTIONAL_SIGN><DECIMAL_DIGIT>+
// <OPTIONAL_SIGN> ::= (+|-)?
// <DIGIT> ::= depending on base can include characters 0-9 and case-
//      insensitive letters.  For example, octal digit is in the range
//      [0 .. 7].
// <DECIMAL_DIGIT> ::= [0123456789]
// <OCTAL_DIGIT> ::= [01234567]
// <HEX_DIGIT> ::= [0123456789abcdefABCDEF]
// <ESCAPE_SEQUENCE> ::= <SIMPLE_ESCAPE_SEQUENCE> | <OCTAL_ESCAPE_SEQUENCE> |
//                     <HEX_ESCAPE_SEQUENCE>
// <SIMPLE_ESCAPE_SEQUENCE> ::= [\a \b \f \n \r \t \v \\ \? \' \"]
// <OCTAL_ESCAPE_SEQUENCE> ::= \<OCTAL_DIGIT>{1,3}  The value of the numeric
//      part of the sequence must be in the range [0 .. UCHAR_MAX].
// <HEX_ESCAPE_SEQUENCE> ::= \x <HEX_DIGIT>{1,2}
// <SHORT> ::= <NUMBER>
//      <SHORT> must be in range [SHRT_MIN .. SHRT_MAX].
// <INT> ::= <NUMBER>
//      <INT> must be in range [INT_MIN .. INT_MAX].
// <INT64> ::= <NUMBER>
//      <INT64> must be in range
//                           [-0x8000000000000000uLL .. 0x7FFFFFFFFFFFFFFFuLL].
// <REAL> ::= <OPTIONAL_SIGN>
//            (<DECIMAL_DIGIT>+ .? <DECIMAL_DIGIT>* |
//                                         <DECIMAL_DIGIT>* . <DECIMAL_DIGIT>+)
//            (e|E <DECIMAL_NUMBER>)?
// <FLOAT> ::= <REAL>
//      <FLOAT> must be in range [FLT_MIN .. FLT_MAX].
// <DOUBLE> ::= <REAL>
//      <DOUBLE> must be in range [DBL_MIN .. DBL_MAX].
// <WHITESPACE> ::= any character for which 'isspace' returns 'true', or
//       any legal C- or C++-style comment.
// <NONWHITESPACE> ::= any character for which 'isspace' returns 'false'.
// <EOS> ::= End of line, or ASCII 0.
// <STRING> ::=  <QUOTED STRING> | <UNQUOTED STRING>
// <UNQUOTED STRING> ::= [^\"]* (<WHITESPACE> | <EOS>) (i.e., zero or
//       more characters followed by a mandatory whitespace or end-of-line
//       character on the right.  None of the characters may be double-quote
//       ('\"'), and the terminating whitespace or EOS is not consumed.
// <DELIMITED STRING> ::= <NONWHITESPACE>?<CHAR_VAL>*<NONWHITESPACE>.  Note
//                        that the left and right delimiters need not be the
//                        same character.
// <QUOTED STRING> ::=  \"<CHAR_VAL>*\" (i.e., zero or more characters or
//      escape sequences surrounded by double quotes.  A quoted string is
//      clearly a special case of a delimited string).
//..
///ENDING POSITION (endPos)
///------------------------
// Each parsing function returns as its first, modifiable argument the
// character position in the input string immediately following the text of
// a successfully parsed token, or the position at which the parse failure
// was detected.  The value returned for '*endPos' will be the address of the
// character following the maximal prefix for a valid instance of the requested
// token.  If that prefix is itself valid, the parse function loads its result
// and returns 0; otherwise, it returns a non-zero value with no effect on the
// result.
//
///AUXILIARY INTERFACE BEHAVIOR
///----------------------------
// One method of this component, 'parseCharRaw' (which parses a single
// character value) provides two overloaded variants: one that also accepts
// the address of a modifiable integer 'flags' argument that can provide
// additional information about the parsed character, and one that does not.
// 'flags' incorporates the value 'bdepu_TypesParserImpUtil::BDEPU_HAS_ESCAPE'
// if the parsed character value is backslash ('\\').  This information is made
// available as a performance-enhancing convenience, because the method has
// efficient access to the affirmative information, but never pays for
// providing the negative information.  Other methods needing the information
// can pass in an 'int*' and then inspect the value as needed, rather than
// doing their own (additional) comparison.
//
///FLOATING POINT VALUES
///---------------------
// Note that the conversion to and from text for values of type 'float' and
// 'double' is potentially lossy.   For example, the ASCII string "3.14159"
// is converted, on some platforms, to 3.1415899999999999.  (See the
// 'bdeimp_fuzzy' component for functions that compare floating point values
// for approximate equality.)
//
// Note also that the 'NaN' class of 'double' values is not uniform across all
// platforms and is therefore not supported by these parsers.  The textual
// representation of such a value, if generated, is left unspecified, and such
// values will NOT necessarily be restored to an equivalent state when parsed.
//
///Usage
///-----
// The following snippets of code illustrate how to generate an array of
// characters representing an integer and then parse the character array to
// obtain an integer equal to the initial integer.  First, create the initial
// integer:
//..
//      int initial = 37;
//..
// Second, create and populate a character array representing the integer
// 'initial':
//..
//      bsl::vector<char> buffer;
//      bdepu_TypesParserImpUtil::generateInt(&buffer, initial);
//      assert(0 == bsl::strncmp("37", buffer.data(), 2));
//      buffer.push_back('\0');
//..
// Finally, populate a new integer and ensure it is identical to the 'initial'
// integer.
//..
//      int final;
//      const char *pos;
//      bdepu_TypesParserImpUtil::parseInt(&pos, &final, buffer.data());
//      assert(buffer.data() + 2 == pos);
//      assert(initial == final);
//..

#ifndef INCLUDED_BDESCM_VERSION
#include <bdescm_version.h>
#endif

#ifndef INCLUDED_BDEPU_ESCAPECHAR
#include <bdepu_escapechar.h>
#endif

#ifndef INCLUDED_BDEPU_PARSERIMPUTIL
#include <bdepu_parserimputil.h>
#endif

#ifndef INCLUDED_BDET_DATE
#include <bdet_date.h>
#endif

#ifndef INCLUDED_BDET_DATETIME
#include <bdet_datetime.h>
#endif

#ifndef INCLUDED_BDET_DATETIMETZ
#include <bdet_datetimetz.h>
#endif

#ifndef INCLUDED_BDET_DATETZ
#include <bdet_datetz.h>
#endif

#ifndef INCLUDED_BDET_TIME
#include <bdet_time.h>
#endif

#ifndef INCLUDED_BDET_TIMETZ
#include <bdet_timetz.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_BSLS_PLATFORMUTIL
#include <bsls_platformutil.h>
#endif

#ifndef INCLUDED_BSL_LIMITS
#include <bsl_limits.h>
#endif

#ifndef INCLUDED_BSL_STRING
#include <bsl_string.h>
#endif

#ifndef INCLUDED_BSL_VECTOR
#include <bsl_vector.h>
#endif

#ifndef INCLUDED_BSL_CCTYPE
#include <bsl_cctype.h>      // for 'isspace'
#endif

namespace BloombergLP {

                        // ===============================
                        // struct bdepu_TypesParserImpUtil
                        // ===============================

struct bdepu_TypesParserImpUtil {
    // This 'struct' provides a namespace for a suite of stateless procedures
    // that perform low-level parsing functionality on fundamental and 'bdet'
    // vocabulary types.

  private:
    // CLASS DATA
    static const char    *s_sesStrings;  // special chars and escape sequences
    static const char    *s_charTypes;   // whether 'PRINT', 'HEX', or 'SES'
    static unsigned char  s_hex[16];     // to-hex conversion table

  public:
    // TYPES
    enum Flag {
        // This 'enum' provides flag values that may be incorporated into
        // optional 'int*' return arguments of 'parseCharRaw' to provide
        // enhanced performance in higher-level parse methods that use
        // 'parseCharRaw'.

        BDEPU_HAS_ESCAPE = 0x01  // If set in a returned 'int' 'flag', this
                                 // value indicates that the parsed character
                                 // contains an escape sequence.

#if !defined(BSL_LEGACY) || 1 == BSL_LEGACY
      , HAS_ESCAPE = BDEPU_HAS_ESCAPE
#endif
    };

    enum MaxBinaryStringLengths {
        // This 'enum' give upper bounds on the maximum string lengths storing
        // each scalar numerical type.  It is safe to use stack-allocated
        // buffers of these sizes for generating binary representations of
        // the corresponding type, including sign and terminating null
        // character.  Note that these lengths are also valid for any base, and
        // these constants are available with names without the suffix '2'.

        BDEPU_MAX_SHORT_STRLEN2  = 2 + sizeof(short) * 8,
        BDEPU_MAX_INT_STRLEN2    = 2 + sizeof(int) * 8,
        BDEPU_MAX_INT64_STRLEN2  = 66,
        BDEPU_MAX_SCALAR_STRLEN2 = BDEPU_MAX_INT64_STRLEN2,

        BDEPU_MAX_SHORT_STRLEN   = BDEPU_MAX_SHORT_STRLEN2,
        BDEPU_MAX_INT_STRLEN     = BDEPU_MAX_INT_STRLEN2,
        BDEPU_MAX_INT64_STRLEN   = BDEPU_MAX_INT64_STRLEN2,
        BDEPU_MAX_SCALAR_STRLEN  = BDEPU_MAX_SCALAR_STRLEN2

#if !defined(BSL_LEGACY) || 1 == BSL_LEGACY
      , MAX_SHORT_STRLEN2  = BDEPU_MAX_SHORT_STRLEN2
      , MAX_INT_STRLEN2    = BDEPU_MAX_INT_STRLEN2
      , MAX_INT64_STRLEN2  = BDEPU_MAX_INT64_STRLEN2
      , MAX_SCALAR_STRLEN2 = BDEPU_MAX_SCALAR_STRLEN2
      , MAX_SHORT_STRLEN   = BDEPU_MAX_SHORT_STRLEN
      , MAX_INT_STRLEN     = BDEPU_MAX_INT_STRLEN
      , MAX_INT64_STRLEN   = BDEPU_MAX_INT64_STRLEN
      , MAX_SCALAR_STRLEN  = BDEPU_MAX_SCALAR_STRLEN
#endif
    };

    enum MaxDecimalStringLengths{
        // This 'enum' give upper bounds on the maximum string lengths storing
        // each scalar numerical type.  It is safe to use stack-allocated
        // buffers of these sizes for generating decimal representations of the
        // corresponding type, including sign and terminating null character,
        // using the default precision of 6 significant digits for floating
        // point types.

        BDEPU_MAX_SHORT_STRLEN10      = 2 + sizeof(short) * 3,
        BDEPU_MAX_INT_STRLEN10        = 2 + sizeof(int) * 3,
        BDEPU_MAX_INT64_STRLEN10      = 26,
        BDEPU_MAX_FLOAT_STRLEN10      = 14,// sx.xxxxxxEsxx\0, s=+/-, x=[0-9]
        BDEPU_MAX_DOUBLE_STRLEN10     = 15,// sx.xxxxxxEsxxx\0, s=+/-, x=[0-9]
        BDEPU_MAX_LONGDOUBLE_STRLEN10 = 16,// sx.xxxxxxEsxxxx\0, s=+/-, x=[0-9]
        BDEPU_MAX_SCALAR_STRLEN10     = BDEPU_MAX_INT64_STRLEN10

#if !defined(BSL_LEGACY) || 1 == BSL_LEGACY
      , MAX_SHORT_STRLEN10      = BDEPU_MAX_SHORT_STRLEN10
      , MAX_INT_STRLEN10        = BDEPU_MAX_INT_STRLEN10
      , MAX_INT64_STRLEN10      = BDEPU_MAX_INT64_STRLEN10
      , MAX_FLOAT_STRLEN10      = BDEPU_MAX_FLOAT_STRLEN10
      , MAX_DOUBLE_STRLEN10     = BDEPU_MAX_DOUBLE_STRLEN10
      , MAX_LONGDOUBLE_STRLEN10 = BDEPU_MAX_LONGDOUBLE_STRLEN10
      , MAX_SCALAR_STRLEN10     = BDEPU_MAX_SCALAR_STRLEN10
#endif
    };

    enum MaxHexadecimalStringLengths {
        // This 'enum' give upper bounds on the maximum string lengths storing
        // each scalar numerical type.  It is safe to use stack-allocated
        // buffers of these sizes for generating hexadecimal representations of
        // the corresponding type, including sign and terminating null
        // character.  Note that the factor 2 equals 8 (bytes to bits
        // conversion) multiplied by 'log16(2) == 0.25' (bits to hexadecimal
        // conversion).

        BDEPU_MAX_SHORT_STRLEN16  = 2 + sizeof(short) * 2,
        BDEPU_MAX_INT_STRLEN16    = 2 + sizeof(int) * 2,
        BDEPU_MAX_INT64_STRLEN16  = 18,
        BDEPU_MAX_SCALAR_STRLEN16 = BDEPU_MAX_INT64_STRLEN

#if !defined(BSL_LEGACY) || 1 == BSL_LEGACY
      , MAX_SHORT_STRLEN16  = BDEPU_MAX_SHORT_STRLEN16
      , MAX_INT_STRLEN16    = BDEPU_MAX_INT_STRLEN16
      , MAX_INT64_STRLEN16  = BDEPU_MAX_INT64_STRLEN16
      , MAX_SCALAR_STRLEN16 = BDEPU_MAX_SCALAR_STRLEN16
#endif
    };

    // CLASS METHODS
                         // *** PARSING FUNCTIONS ***

    static int parseChar(const char **endPos,
                         char        *result,
                         const char  *inputString);
        // Parse the specified 'inputString' for a sequence of characters
        // matching the production rule <CHAR> (i.e., single-quoted characters)
        // and place into the specified 'result' the corresponding value.
        // Store in the specified '*endPos' the address of the non-modifiable
        // character in 'inputString' immediately following the successfully
        // parsed text, or the position at which the parse failure was
        // detected.  Return zero on success, and a non-zero value otherwise.
        // The value of '*result' is unchanged if a parse failure occurs.  The
        // behavior is undefined if any argument is 0.
        //
        // A parse failure can occur for the following reasons:
        //..
        //   1. 'inputString' does not begin with a single quote ('\'').
        //   2. The second and third characters in 'inputString' are "\x"
        //      but there is no <HEX_DIGIT> after the 'x'.
        //   3. The character is not an <ESCAPE_SEQUENCE>, but starts with
        //      backslash ('\\').
        //   4. The character is '\0', i.e., indicating the end of
        //      'inputString'.
        //   5. There is no closing single-quote ('\'') in 'inputString'.
        //..

    static int parseCharRaw(const char **endPos,
                            char        *result,
                            const char  *inputString);
    static int parseCharRaw(const char **endPos,
                            char        *result,
                            const char  *inputString,
                            int         *flags);
        // Parse the specified 'inputString' for a sequence of characters
        // matching the production rule <CHAR_VAL> (i.e., unquoted characters)
        // and place into the specified 'result' the corresponding value.
        // Store in the specified '*endPos' the address of the non-modifiable
        // character in 'inputString' immediately following the successfully
        // parsed text, or the position at which the parse failure was
        // detected.  If parsing succeeds, store
        // 'bdepu_TypesParserImpUtil::BDEPU_HAS_ESCAPE' in the optionally
        // specified 'flags' if the parse character was escaped and zero
        // otherwise.  If parsing fails, 'flags' is unchanged.  Return zero on
        // success, and a non-zero value otherwise.  The value of '*result' is
        // unchanged if a parse failure occurs.  The behavior is undefined if
        // any argument is 0.
        //
        // A parse failure can occur for the following reasons:
        //..
        //   1. The second and third characters in 'inputString' are "\x"
        //      but there is no <HEX_DIGIT> after the 'x'.
        //   2. The character is not an <ESCAPE_SEQUENCE>, but starts with
        //      backslash ('\\').
        //   3. The character is '\0'.
        //..

    static int parseDate(const char **endPos,
                         bdet_Date   *result,
                         const char  *inputString);
        // Parse the specified 'inputString' for a sequence of characters
        // matching the production rule <DATE> (i.e., one to four digits
        // representing the year followed by a slash and one to two digits
        // representing the month followed by a slash and one to two digits
        // representing the day, such as 2003/7/16), and place into the
        // specified 'result' the corresponding value.  Store in the specified
        // '*endPos' the address of the non-modifiable character in
        // 'inputString' immediately following the successfully parsed text,
        // or the position at which the parse failure was detected.  Return
        // zero on success, and a non-zero value otherwise.  The value of
        // '*result' is unchanged if a parse failure occurs.  The behavior is
        // undefined if any argument is 0.  Note that if parsing fails because
        // the day for the valid year and month is invalid, '*endPos' will be
        // set to point to the *second* digit of the day field.
        //
        // A parse failure can occur for the following reasons:
        //..
        //   1. The <YEAR> is not in range [1 .. 9999].
        //   2. The <MONTH> is not in the range [1 .. 12].
        //   3. The <DAY> is not in the range [1 .. 31].
        //   4. The number of digits in <YEAR> is not in range [1 .. 4].
        //   5. The number of digits in <MONTH> is not 1 or 2.
        //   6. The number of digits in <DAY> is not 1 or 2.
        //   7. The fields are not separated by a forward slash ('/')
        //      character.
        //   8. The <YEAR>, <MONTH>, <DAY> combination is invalid as
        //      defined in 'bdet_Date'.
        //..

    static int parseDatetime(const char    **endPos,
                             bdet_Datetime  *result,
                             const char     *inputString);
        // Parse the specified 'inputString' for a sequence of characters
        // matching the production rule <DATETIME> and place into the
        // specified 'result' the corresponding value.  Store in the specified
        // '*endPos' the address of the non-modifiable character in
        // 'inputString' immediately following the successfully parsed text,
        // or the position at which the parse failure was detected.  Return
        // zero on success, and a non-zero value otherwise.  The value of
        // '*result' is unchanged if a parse failure occurs.  The behavior is
        // undefined if any argument is 0.
        //
        // A parse failure can occur for the following reasons:
        //..
        //   1.  There is no initial prefix in 'inputString' that can
        //       successfully parse as a <DATE>.
        //   2.  The <DATE> and <TIME> portions of 'inputString' are not
        //       separated by either exactly one space (' ') or exactly one
        //       dash ('_').
        //   3.  The characters following the space (' ') or dash ('_')
        //       do not successfully parse as a <TIME>.
        //..

    static int parseDatetimeTz(const char      **endPos,
                               bdet_DatetimeTz  *result,
                               const char       *inputString);
        // Parse the specified 'inputString' for a sequence of characters
        // matching the production rule <DATETIME_TZ> and place into the
        // specified 'result' the corresponding value.  Store in the specified
        // '*endPos' the address of the non-modifiable character in
        // 'inputString' immediately following the successfully parsed text,
        // or the position at which the parse failure was detected.  Return
        // zero on success, and a non-zero value otherwise.  The value of
        // '*result' is unchanged if a parse failure occurs.  The behavior is
        // undefined if any argument is 0.
        //
        // A parse failure can occur for the following reasons:
        //..
        //   1.  There is no initial prefix in 'inputString' that can
        //       successfully parse as a <DATETIME>.
        //   2.  What follows the <DATETIME> prefix of 'inputString' is neither
        //       the plus ('+') or the minus ('-') characters.
        //   3.  The characters following the plus ('+') or minus ('-') do not
        // successfully parse as a decimal integer.
        //..

    static int parseDateTz(const char  **endPos,
                           bdet_DateTz  *result,
                           const char   *inputString);
        // Parse the specified 'inputString' for a sequence of characters
        // matching the production rule <DATE_TZ> (i.e., one to four digits
        // representing the year followed by a slash and one to two digits
        // representing the month followed by a slash and one to two digits
        // representing the day, such as 2003/7/16), and place into the
        // specified 'result' the corresponding value.  Store in the specified
        // '*endPos' the address of the non-modifiable character in
        // 'inputString' immediately following the successfully parsed text,
        // or the position at which the parse failure was detected.  Return
        // zero on success, and a non-zero value otherwise.  The value of
        // '*result' is unchanged if a parse failure occurs.  The behavior is
        // undefined if any argument is 0.  Note that if parsing fails because
        // the day for the valid year and month is invalid, '*endPos' will be
        // set to point to the *second* digit of the day field.
        //
        // A parse failure can occur for the following reasons:
        //..
        //   1.  There is no initial prefix in 'inputString' that can
        //       successfully parse as a <DATE>.
        //   2.  What follows the <DATE> prefix of 'inputString' is neither the
        // plus ('+') or the minus ('-') characters.
        //   3.  The characters following the plus ('+') or minus ('-') do not
        // successfully parse as a decimal integer.
        //..

    static int parseDouble(const char **endPos,
                           double      *result,
                           const char  *inputString);
        // Parse the specified 'inputString' for a sequence of characters
        // matching the production rule <DOUBLE> and place into the
        // specified 'result' the corresponding value.  Store in the specified
        // '*endPos' the address of the non-modifiable character in
        // 'inputString' immediately following the successfully parsed text,
        // or the position at which the parse failure was detected.  Return
        // zero on success, and a non-zero value otherwise.  The value of
        // '*result' is unchanged if a parse failure occurs.  The behavior is
        // undefined if any argument is 0.
        //
        // A parse failure can occur for the following reasons:
        //..
        //   1. The first character in 'inputString' is not '+', '-', or a
        //      decimal number.
        //   2. There is an exponent indicator (case-insensitive 'e') in
        //      'inputString' but the character following it is not a decimal
        //      number.
        //..

    static int parseFloat(const char **endPos,
                          float       *result,
                          const char  *inputString);
        // Parse the specified 'inputString' for a sequence of characters
        // matching the production rule <FLOAT> and place into the 'specified'
        // 'result' the corresponding value.  Store in the specified '*endPos'
        // the address of the non-modifiable character in 'inputString'
        // immediately following the successfully parsed text, or the position
        // at which the parse failure was detected.  Return zero on success,
        // and non-zero otherwise.  The value of '*result' is unchanged if a
        // parse failure occurs.  The behavior is undefined if any argument is
        // 0.
        //
        // A parse failure can occur for the following reasons:
        //..
        //   1. The first character in 'inputString' is not '+', '-', or a
        //      decimal number.
        //   2. There is an exponent indicator (case-insensitive 'e') in
        //      'inputString' but the character following it is not a decimal
        //      number.
        //..

    static int parseInt(const char **endPos,
                        int         *result,
                        const char  *inputString,
                        int          base = 10);
        // Parse the specified 'inputString' for the maximal sequence of
        // characters forming an <INT> in the optionally specified 'base' or in
        // base 10 if 'base' is not specified, and place into the specified
        // 'result' the corresponding value.  Store in the specified '*endPos'
        // the address of the non-modifiable character in 'inputString'
        // immediately following the successfully parsed text, or the position
        // at which the parse failure was detected.  Return zero on success,
        // and a non-zero value otherwise.  The value of '*result' is unchanged
        // if a parse failure occurs.  The behavior is undefined if any
        // argument is 0 and unless '2 <= base' and 'base <= 36' (i.e., bases
        // where digits are representable by characters in the range ['0'-'9'],
        // ['a'-'z'], or ['A'-'Z']).
        //
        // A parse failure can occur for the following reason:
        //..
        //  The first character of 'inputString' is not a valid digit in
        //  'base'.
        //..

    static int parseInt64(const char               **endPos,
                          bsls_PlatformUtil::Int64  *result,
                          const char                *inputString,
                          int                        base = 10);
        // Parse the specified 'inputString' for the maximal sequence of
        // characters forming a valid <INT64> in the optionally specified
        // 'base' or in base 10 if 'base' is not specified, and place into the
        // specified 'result' the corresponding value.  Store in the specified
        // '*endPos' the address of the non-modifiable character in
        // 'inputString' immediately following the successfully parsed text, or
        // the position at which the parse failure was detected.  Return zero
        // on success, and a non-zero value otherwise.  The value of '*result'
        // is unchanged if a parse failure occurs.  The behavior is undefined
        // if any argument is 0 and unless '2 <= base' and 'base <= 36' (i.e.,
        // bases where digits are representable by characters in the range
        // ['0'-'9'], ['a'-'z'], or ['A'-'Z']).
        //
        // A parse failure can occur for the following reason:
        //..
        //   The first character of 'inputString' is not a valid digit in
        //   'base'.
        //..

    static int parseShort(const char **endPos,
                          short       *result,
                          const char  *inputString,
                          int          base = 10);
        // Parse the specified 'inputString' for the maximal sequence of
        // characters forming a valid <SHORT> in the optionally specified
        // 'base' or in base 10 if 'base' is not specified, and place into the
        // specified 'result' the corresponding value.  Store in the specified
        // '*endPos' the address of the non-modifiable character in
        // 'inputString' immediately following the successfully parsed text, or
        // the position at which the parse failure was detected.  Return zero
        // on success, and a non-zero value otherwise.  The value of '*result'
        // is unchanged if a parse failure occurs.  The behavior is undefined
        // if any argument is 0, and unless '2 <= base' and 'base <= 36' (i.e.,
        // bases where digits are representable by characters in the range
        // ['0'-'9'], ['a'-'z'] or ['A'-'Z']).
        //
        // A parse failure can occur for the following reason:
        //..
        //   The first character of 'inputString' is not a valid digit in
        //   'base'.
        //..

    static int parseTime(const char **endPos,
                         bdet_Time   *result,
                         const char  *inputString);
        // Parse the specified 'inputString' for a sequence of characters
        // matching the production rule <TIME> and place into the specified
        // 'result' the corresponding value.  Store in the specified '*endPos'
        // the address of the non-modifiable character in 'inputString'
        // immediately following the successfully parsed text, or the
        // position at which the parse failure was detected.  Return zero on
        // success, and a non-zero value otherwise.  The value of '*result' is
        // unchanged if a parse failure occurs.  The behavior is undefined if
        // any argument is 0.
        //
        // A parse failure can occur for the following reasons:
        //..
        //   1.  <HOUR> is not in range [1 .. 24].
        //   2.  <MINUTE> is not in the range [0 .. 59].
        //   3.  <SECOND>, if present, is not in the range [0 .. 59].
        //   4.  <SDECIMAL>, if present, is not in the range [0 .. 999].
        //   5.  The number of digits in <HOUR> is not in range [1 .. 2].
        //   6.  The number of digits in <MINUTE> is not exactly two.
        //   7.  The number of digits in <SECOND>, if present, is not exactly
        //       two.
        //   8.  <HOUR> and <MINUTE> fields are not separated by ':'
        //       character.
        //   9.  There is a colon (':') immediately following the <MINUTE>
        //       field but no <SECOND> after the colon.
        //   10. There is a period ('.') following the <SECOND> field but no
        //       <SDECIMAL> after the period.
        //   11. The <HOUR> equals 24, but one or more of the other
        //       fields is not equal to zero.
        //..

    static int parseTimeTz(const char  **endPos,
                           bdet_TimeTz  *result,
                           const char   *inputString);
        // Parse the specified 'inputString' for a sequence of characters
        // matching the production rule <TIME_TZ> and place into the specified
        // 'result' the corresponding value.  Store in the specified '*endPos'
        // the address of the non-modifiable character in 'inputString'
        // immediately following the successfully parsed text, or the
        // position at which the parse failure was detected.  Return zero on
        // success, and a non-zero value otherwise.  The value of '*result' is
        // unchanged if a parse failure occurs.  The behavior is undefined if
        // any argument is 0.
        //
        // A parse failure can occur for the following reasons:
        //..
        //   1.  There is no initial prefix in 'inputString' that can
        //       successfully parse as a <TIME>.
        //   2.  What follows the <TIME> prefix of 'inputString' is neither the
        // plus ('+') or the minus ('-') characters.
        //   3.  The characters following the plus ('+') or minus ('-') do not
        // successfully parse as a decimal integer.
        //..

    static int parseTz(const char **endPos,
                       int         *result,
                       const char  *inputString);
        // Parse the specified 'inputString' for a sequence of characters
        // matching the production rule <TZ> and place into the specified
        // 'result' the corresponding value.  Store in the specified '*endPos'
        // the address of the non-modifiable character in 'inputString'
        // immediately following the successfully parsed text, or the position
        // at which the parse failure was detected.  Return zero on success,
        // and a non-zero value otherwise.  The value of '*result' is unchanged
        // if a parse failure occurs.  The behavior is undefined if any
        // argument is 0.
        //
        // A parse failure can occur for the following reasons:
        //..
        //   1.  The first character in 'inputString' is neither '+' nor '-'.
        //   2.  Following the '+' or '-' we don't have four decimal digits.
        //..

                         // *** STRING PARSING FUNCTIONS ***

    static int parseDelimitedString(const char  **endPos,
                                    bsl::string  *result,
                                    const char   *inputString,
                                    char          leftDelimiter,
                                    char          rightDelimiter);
        // Parse the specified 'inputString' for a sequence of characters
        // matching the production rule <DELIMITED STRING> using the specified
        // 'leftDelimiter' and 'rightDelimiter' as the <NONWHITESPACE>
        // delimiters and place into the specified 'result' the corresponding
        // value.  Store in the specified '*endPos' the address of the
        // non-modifiable character in 'inputString' immediately following the
        // successfully parsed text, or the position at which the parse failure
        // was detected.  Return zero on success, and a non-zero value
        // otherwise.  If 'leftDelimiter' is 0, do not require an initial
        // delimiter in the 'inputString'.  If 'rightDelimiter' is 0, parse
        // 'inputString' until a terminating <EOS> is found.  The value of
        // '*result' is unchanged if a parse failure occurs.  The behavior is
        // undefined if any pointer argument is 0.
        //
        // A parse failure can occur for the following reasons:
        //..
        //   1. A required delimiter has been specified, but is missing from
        //      'inputString'.
        //   2. Any of the characters before the right delimiter are invalid
        //       as defined in 'parseCharRaw'.
        //..
        // Note that there is potential for the right delimiter to be
        // misinterpreted as part of the desired text.  For example, using
        // capital A as a delimiter will lead to a misparse of the string
        // "A_some_other_stuff_and_then_\x05A" as terminating in number Hex-
        // 5-A, rather than Hex-5 and delimiter A.  The parse will then fail
        // because no right delimiter is located.  Precisely speaking, the
        // left-to-right scan implies that if a multibyte character (i.e., an
        // escape sequence) appears right before the terminating delimiter,
        // and the delimiter can legitimately be interpreted to belong to the
        // multibyte character, it will be consumed as part of that character,
        // leaving no recognizable delimiter to terminate the string itself.

    static int parseQuotedString(const char  **endPos,
                                 bsl::string  *result,
                                 const char   *inputString);
        // Parse the specified 'inputString' for a sequence of characters
        // matching the production rule '<QUOTED STRING>' and place into the
        // specified 'result' the corresponding value.  Store in the specified
        // '*endPos' the address of the non-modifiable character in
        // 'inputString' immediately following the successfully parsed text,
        // or the position at which the parse failure was detected.  Return
        // zero on success, and a non-zero value otherwise.  The value of
        // '*result' is unchanged if a parse failure occurs.  The behavior is
        // undefined if any argument is 0.
        //
        // A parse failure can occur for the following reasons:
        //..
        //   1. If the opening or closing double-quote ('\"') character is
        //      missing.
        //   2. If one of the characters in the string is invalid as
        //      defined in 'parseCharRaw'.
        //..

    static int parseSpaceDelimitedString(const char  **endPos,
                                         bsl::string  *result,
                                         const char   *inputString);
        // Parse the specified 'inputString' for a sequence of non-whitespace
        // characters, followed by at least one whitespace or <EOS>, and place
        // into the specified 'result' the corresponding value.  Store in the
        // specified '*endPos' the address of the non-modifiable character in
        // 'inputString' immediately following the successfully parsed text, or
        // the position at which the parse failure was detected.  Return zero
        // on success, and a non-zero value otherwise.  The value of '*result'
        // is unchanged if a parse failure occurs.  The behavior is undefined
        // if any argument is 0.  Note that this procedure is functionally
        // identical to 'parseUnquotedString' except for allowing double-quotes
        // ('\"') within the string.
        //
        // A parse failure can occur for the following reason:
        //..
        //   One of the characters in the string is invalid as defined in
        //   'parseCharRaw'.
        //..

    static int parseString(const char  **endPos,
                           bsl::string  *result,
                           const char   *inputString);
        // Parse the specified 'inputString' for a sequence of characters
        // matching the production rule <STRING> and place into the specified
        // 'result' the corresponding value.  Store in the specified '*endPos'
        // the address of the non-modifiable character in 'inputString'
        // immediately following the successfully parsed text, or the
        // position at which the parse failure was detected.  Return zero on
        // success, and a non-zero value otherwise.  The value of '*result'
        // is unchanged if a parse failure occurs.  The behavior is undefined
        // if any argument is 0.
        //
        // A parse failure can occur for the following reasons:
        //..
        //   1. There is an opening double-quote ('\"') character, but the
        //      closing double-quote ('\"') is missing.
        //   2. The first character is NOT a double-quote ('\"'), but a
        //      double-quote ('\"') character is subsequently found somewhere
        //      in 'inputString' prior to encountering the delimiting
        //      whitespace, or <EOS> ('\0').
        //   3. One of the characters in the string is invalid as defined
        //      in 'parseCharRaw'.
        //..

    static int parseUnquotedString(const char  **endPos,
                                   bsl::string  *result,
                                   const char   *inputString);
        // Parse the specified 'inputString' for a sequence of characters
        // matching the production rule '<UNQUOTED STRING>' and place into the
        // specified 'result' the corresponding value.  Store in the specified
        // '*endPos' the address of the first whitespace character or <EOS>
        // located after non-whitespace characters have been found, or the
        // position at which the parse failure was detected.  Return zero on
        // success, and a non-zero value otherwise.  The value of '*result' is
        // unchanged if a parse failure occurs.  The behavior is undefined if
        // any argument is 0.
        //
        // A parse failure can occur for the following reasons:
        //..
        //   1. A double-quote ('\"') character is found prior to encountering
        //      the delimiting whitespace, or <EOS> ('\0').
        //   2. One of the characters in the string is invalid as defined
        //      in 'parseCharRaw'.
        //..

                         // *** GENERATING FUNCTIONS ***

    static int generateShort     (char                     *buffer,
                                  short                     value,
                                  int                       bufferLength);
    static int generateShort     (char                     *buffer,
                                  short                     value,
                                  int                       bufferLength,
                                  int                       base);
    static int generateInt       (char                     *buffer,
                                  int                       value,
                                  int                       bufferLength);
    static int generateInt       (char                     *buffer,
                                  int                       value,
                                  int                       bufferLength,
                                  int                       base);
    static int generateInt64     (char                     *buffer,
                                  bsls_PlatformUtil::Int64  value,
                                  int                       bufferLength);
    static int generateInt64     (char                     *buffer,
                                  bsls_PlatformUtil::Int64  value,
                                  int                       bufferLength,
                                  int                       base);
        // Format the specified 'value' to the specified 'buffer' in the
        // optionally specified 'base', truncating (if necessary) to the
        // specified 'bufferLength'.  Return the length of the formatted string
        // before truncation (not including a null terminator).  If
        // 'bufferLength' is larger than necessary to contain the string
        // representation of 'value', then a null terminator is appended to the
        // output.  If 'base' is unspecified, decimal (i.e., 'base == 10') is
        // used.  The behavior is undefined unless '0 <= bufferLength' and
        // '2 <= base <= 36'.  Note that a buffer of size
        // 'BDEPU_MAX_SCALAR_STRLEN10 + 1' (resp.,
        // 'BDEPU_MAX_SCALAR_STRLEN16 + 1', or 'BDEPU_MAX_SCALAR_STRLEN + 1')
        // is large enough to hold any formatted decimal (resp., hexadecimal,
        // or other base) representation, including a null terminator.

    static int generateFloat     (char        *buffer,
                                  float        value,
                                  int          bufferLength);
    static int generateDouble    (char        *buffer,
                                  double       value,
                                  int          bufferLength);
    static int generateLongDouble(char        *buffer,
                                  long double  value,
                                  int          bufferLength);
        // Format the specified 'value' to the specified 'buffer', truncating
        // (if necessary) to the specified 'bufferLength'.  Return the length
        // of the formatted string before truncation (not including a null
        // terminator).  If 'bufferLength' is larger than necessary to contain
        // the string representation of 'value', then a null terminator is
        // appended to the output.  The behavior is undefined unless
        // '0 <= bufferLength'.  Note that the default precision (i.e., 6
        // significant digits) is used.  Also note that a buffer of size
        // 'BDEPU_MAX_LONGDOUBLE_STRLEN10 + 1' is large enough to hold any
        // formatted string, including a null terminator.

    static int generateShortRaw     (char                     *buffer,
                                     short                     value);
    static int generateShortRaw     (char                     *buffer,
                                     short                     value,
                                     int                       base);
    static int generateIntRaw       (char                     *buffer,
                                     int                       value);
    static int generateIntRaw       (char                     *buffer,
                                     int                       value,
                                     int                       base);
    static int generateInt64Raw     (char                     *buffer,
                                     bsls_PlatformUtil::Int64  value);
    static int generateInt64Raw     (char                     *buffer,
                                     bsls_PlatformUtil::Int64  value,
                                     int                       base);
        // Format the specified 'value' to the specified 'buffer' in the
        // optionally specified 'base'.  If 'base' is not specified, decimal
        // (i.e., 'base == 10') is used.  Return the number of characters
        // actually written.  It is unspecified whether the 'buffer' is
        // null-terminated on output.  The behavior is undefined unless
        // 'buffer' has sufficient capacity and '2 <= base <= 36'.  Note that a
        // buffer of size 'BDEPU_MAX_SCALAR_STRLEN10 + 1' (resp.,
        // 'BDEPU_MAX_SCALAR_STRLEN16 + 1' or 'BDEPU_MAX_SCALAR_STRLEN + 1') is
        // large enough to hold any formatted decimal (resp., hexadecimal, or
        // other base) representation, including a null terminator.

    static int generateFloatRaw     (char        *buffer,
                                     float        value);
    static int generateDoubleRaw    (char        *buffer,
                                     double       value);
    static int generateLongDoubleRaw(char        *buffer,
                                     long double  value);
        // Format the specified 'value' to the specified 'buffer'.  Return the
        // number of characters actually written.  It is unspecified whether
        // the 'buffer' is null-terminated on output.  The behavior is
        // undefined unless 'buffer' has sufficient capacity.  Note that a
        // buffer of size 'BDEPU_MAX_SCALAR_STRLEN10 + 1' is large enough to
        // hold any formatted string, including a null terminator.

    static void generateCharRaw(bsl::vector<char> *buffer, char value);
        // Format the specified 'value' and append the result to the specified
        // 'buffer' as follows: printable characters (as identified by the
        // 'isprint' library function) are generated as is; simple escape
        // sequences (see grammar rule <SIMPLE_ESCAPE_SEQUENCE>) are generated
        // as '\\' followed by the escape literal (e.g., '\a' is generated as
        // "\\a"); all other characters are generated as "\x" followed by the
        // the two digits denoting the character's value, in hexadecimal.  The
        // behavior is undefined if 'value' is '\0'.

    static void generateChar(bsl::vector<char> *buffer, char value);
        // Format the specified 'value' and append the result to the specified
        // 'buffer' with the following surrounded by single quotes: printable
        // characters (as identified by the 'isprint' library function) are
        // generated as is; simple escape sequences (see grammar rule
        // <SIMPLE_ESCAPE_SEQUENCE>) are generated as '\\' followed by the
        // escape literal (e.g., '\a' is generated as "\\a"); all other
        // characters are generated as "\x" followed by the the two digits
        // denoting the character's value, in hexadecimal.  The behavior is
        // undefined if 'value' is '\0'.

    static void generateDate(bsl::vector<char> *buffer,
                             const bdet_Date&   value);
        // Format the specified 'value' and append the result to the specified
        // 'buffer'.  The 'bdet_Date' is formatted as follows: four digits to
        // represent the year followed by a forward slash ('/') followed by
        // two digits to represent the month followed by a forward slash ('/')
        // followed by two digits to represent the day.  All values will have
        // as many leading zeros as necessary to reach the specified number
        // of digits.  Note that 'value' is emitted in a form parsable by the
        // corresponding 'parse' function.

    static void generateDatetime(bsl::vector<char>    *buffer,
                                 const bdet_Datetime&  value);
        // Format the specified 'value' and append the result to the specified
        // 'buffer'.  The 'bdet_Datetime' is formatted as a formatted
        // 'bdet_Date' followed by a single space followed by a formatted
        // 'bdet_Time'.  Note that 'value' is emitted in a form parsable by
        // the corresponding 'parse' function.

    static void generateDatetimeTz(bsl::vector<char>      *buffer,
                                   const bdet_DatetimeTz&  value);
        // Format the specified 'value' and append the result to the specified
        // 'buffer'.  The 'bdet_DatetimeTz' is formatted as a formatted
        // 'bdet_Date' followed by a single space followed by a formatted
        // 'bdet_Time'.  Note that 'value' is emitted in a form parsable by
        // the corresponding 'parse' function.

    static void generateDateTz(bsl::vector<char>  *buffer,
                               const bdet_DateTz&  value);
        // Format the specified 'value' and append the result to the specified
        // 'buffer'.  The 'bdet_DateTz' is formatted as follows: four digits to
        // represent the year followed by a forward slash ('/') followed by
        // two digits to represent the month followed by a forward slash ('/')
        // followed by two digits to represent the day.  All values will have
        // as many leading zeros as necessary to reach the specified number
        // of digits.  Note that 'value' is emitted in a form parsable by the
        // corresponding 'parse' function.

    static void generateDouble(bsl::vector<char> *buffer, double value);
        // Format the specified 'value' and append the result to the specified
        // 'buffer'.  Note that 'value' emitted in a form parsable by the
        // corresponding 'parse' function.
        //
        // Warning: The generated value might vary across platforms.

    static void generateFloat(bsl::vector<char> *buffer, float value);
        // Format the specified 'value' and append the result to the specified
        // 'buffer'.  Note that 'value' is emitted in a form parsable by the
        // corresponding 'parse' function.
        //
        // Warning: The generated value might vary across platforms.

    static void generateInt64(bsl::vector<char>        *buffer,
                              bsls_PlatformUtil::Int64  value,
                              int                       base = 10);
        // Format the specified 'value' in the optionally specified 'base' and
        // append the result to the specified 'buffer'.  Note that 'value' is
        // emitted in a form parsable by the corresponding 'parse' function.

    static void generateInt(bsl::vector<char> *buffer,
                            int                value,
                            int                base = 10);
        // Format the specified 'value' in the optionally specified 'base' and
        // append the result to the specified 'buffer'.  Note that 'value' is
        // emitted in a form parsable by the corresponding 'parse' function.

    static void generateShort(bsl::vector<char> *buffer,
                              short              value,
                              int                base = 10);
        // Format the specified 'value' in the optionally specified 'base' and
        // append the result to the specified 'buffer'.  Note that 'value' is
        // emitted in a form parsable by the corresponding 'parse' function.

    static void generateString(bsl::vector<char>  *buffer,
                               const char         *value);
    static void generateString(bsl::vector<char>  *buffer,
                               const bsl::string&  value);
        // Format the specified 'value' and append the result to the specified
        // 'buffer'.  The empty string, strings with embedded whitespace, and
        // string that start with ']', '//',  and '/*' are emitted surrounded
        // by double quotes ('"'); the internal characters are emitted in the
        // format defined by 'parseCharRaw'.

    static void generateTime(bsl::vector<char> *buffer,
                             const bdet_Time&   value);
        // Format the specified 'value' and append the result to the specified
        // 'buffer'.  The 'bdet_Time' is formatted as follows: two digits to
        // represent hours followed by a colon followed by two digits to
        // represent minutes followed by a colon followed by two digits to
        // represent seconds followed by a colon followed by three digits to
        // represent milliseconds.  All values will have leading zeros to
        // reach the stated number of digits.  Note that 'value' is emitted in
        // a form parsable by the corresponding 'parse' function.

    static void generateTimeTz(bsl::vector<char>  *buffer,
                               const bdet_TimeTz&  value);
        // Format the specified 'value' and append the result to the specified
        // 'buffer'.  The 'bdet_TimeTz' is formatted as follows: two digits to
        // represent hours followed by a colon followed by two digits to
        // represent minutes followed by a colon followed by two digits to
        // represent seconds followed by a colon followed by three digits to
        // represent milliseconds.  All values will have leading zeros to
        // reach the stated number of digits.  Note that 'value' is emitted in
        // a form parsable by the corresponding 'parse' function.

    static void generateTz(bsl::vector<char> *buffer, int offset);
        // Format the specified 'value' and append the result to the specified
        // 'buffer'.  The 'offset' is formatted as follows: TBD.
};

// ============================================================================
//                      INLINE FUNCTION DEFINITIONS
// ============================================================================

                        // -------------------------------
                        // struct bdepu_TypesParserImpUtil
                        // -------------------------------

// CLASS METHODS
inline
int bdepu_TypesParserImpUtil::parseQuotedString(const char  **endPos,
                                                bsl::string  *result,
                                                const char   *inputString)
{
    BSLS_ASSERT_SAFE(endPos);
    BSLS_ASSERT_SAFE(result);
    BSLS_ASSERT_SAFE(inputString);

    return bdepu_TypesParserImpUtil::parseDelimitedString(endPos,
                                                          result,
                                                          inputString,
                                                          '\"',
                                                          '\"');
}

inline
int bdepu_TypesParserImpUtil::parseString(const char  **endPos,
                                          bsl::string  *result,
                                          const char   *inputString)
{
    BSLS_ASSERT_SAFE(endPos);
    BSLS_ASSERT_SAFE(result);
    BSLS_ASSERT_SAFE(inputString);

    if (*inputString == '\"') {
        return parseQuotedString(endPos, result, inputString);
    }
    else {
        return parseUnquotedString(endPos, result, inputString);
    }
}

inline
int bdepu_TypesParserImpUtil::generateShort(char  *buffer,
                                            short  value,
                                            int    length)
{
    BSLS_ASSERT_SAFE(buffer);
    BSLS_ASSERT_SAFE(0 <= length);

    return generateInt(buffer, value, length);
}

inline
int bdepu_TypesParserImpUtil::generateShort(char  *buffer,
                                            short  value,
                                            int    length,
                                            int    base)
{
    BSLS_ASSERT_SAFE(buffer);
    BSLS_ASSERT_SAFE(0 <= length);
    BSLS_ASSERT_SAFE(2 <= base);
    BSLS_ASSERT_SAFE(     base <= 36);


    return generateInt(buffer, value, length, base);
}

inline
int bdepu_TypesParserImpUtil::generateShortRaw(char *buffer, short value)
{
    BSLS_ASSERT_SAFE(buffer);

    return generateIntRaw(buffer, value);
}

inline
int bdepu_TypesParserImpUtil::generateShortRaw(char  *buffer,
                                               short  value,
                                               int    base)
{
    BSLS_ASSERT_SAFE(buffer);
    BSLS_ASSERT_SAFE(2 <= base);
    BSLS_ASSERT_SAFE(     base <= 36);

    return generateIntRaw(buffer, value, base);
}

inline
void bdepu_TypesParserImpUtil::generateChar(bsl::vector<char> *buffer,
                                            char               value)
{
    BSLS_ASSERT_SAFE(buffer);

    buffer->push_back('\'');
    generateCharRaw(buffer, value);
    buffer->push_back('\'');
}

inline
void bdepu_TypesParserImpUtil::generateShort(bsl::vector<char> *buffer,
                                             short              value,
                                             int                base)
{
    BSLS_ASSERT_SAFE(buffer);
    BSLS_ASSERT_SAFE(2 <= base);
    BSLS_ASSERT_SAFE(     base <= 36);

    generateInt(buffer, value, base);
}

inline
void bdepu_TypesParserImpUtil::generateString(bsl::vector<char>  *buffer,
                                              const bsl::string&  value)
{
    BSLS_ASSERT_SAFE(buffer);

    generateString(buffer, value.data());
}

inline
void bdepu_TypesParserImpUtil::generateDatetime(bsl::vector<char>    *buffer,
                                                const bdet_Datetime&  value)
{
    BSLS_ASSERT_SAFE(buffer);

    generateDate(buffer, value.date());
    buffer->push_back(' ');
    generateTime(buffer, value.time());
}

inline
void bdepu_TypesParserImpUtil::generateDatetimeTz(
                                                bsl::vector<char>      *buffer,
                                                const bdet_DatetimeTz&  value)
{
    BSLS_ASSERT_SAFE(buffer);

    generateDatetime(buffer, value.localDatetime());
    generateTz(buffer, value.offset());
}

inline
void bdepu_TypesParserImpUtil::generateDateTz(bsl::vector<char>  *buffer,
                                              const bdet_DateTz&  value)
{
    BSLS_ASSERT_SAFE(buffer);

    generateDate(buffer, value.localDate());
    generateTz(buffer, value.offset());
}

inline
void bdepu_TypesParserImpUtil::generateTimeTz(bsl::vector<char>  *buffer,
                                              const bdet_TimeTz&  value)
{
    BSLS_ASSERT_SAFE(buffer);

    generateTime(buffer, value.localTime());
    generateTz(buffer, value.offset());
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
