// bdepu_arrayparserimputil.h       -*-C++-*-
#ifndef INCLUDED_BDEPU_ARRAYPARSERIMPUTIL
#define INCLUDED_BDEPU_ARRAYPARSERIMPUTIL

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Convert arrays to and from text representation.
//
//@CLASSES:
//  bdepu_ArrayParserImpUtil: namespace for array parsing functions
//
//@AUTHOR: Lea Fester (lfester)
//
//@DESCRIPTION: The utility functions in this component fall into two
// categories -- parsers and generators.  Parsers convert textual
// representations of a type into an object of that type.  Generators convert
// objects into ASCII representations.  This component provides a parser and a
// generator for arrays (vectors) of each of the following types:
//..
//   char
//   short
//   int
//   bsls_PlatformUtil::Int64
//   float
//   double
//   bsl::string
//   bdet_Date
//   bdet_DateTz
//   bdet_Datetime
//   bdet_DatetimeTz
//   bdet_Time
//   bdet_TimeTz
//..
// For efficiency, all parsers allow the result array to be modified regardless
// of whether parsing succeeds or fails.
//
// Although parse methods must parse *any* legal textual representation of
// an object of their type, the generators each produce a simple, canonical
// representation.  For methods in this component, legal representations are
// internal-white-space-neutral; leading white-space is not allowed.
//
// Each of the 'generate' functions in this component emits text that the
// corresponding parsing function can recognize.  The 'generate' functions
// operate on a 'bsl::vector<char>' buffer, which is used to maintain an
// efficient representation of the output.
//
// The following two subsections describe the grammar defining the parsing
// rules and rules used in the implementation of this component.  Note that
// when parsing arrays, the *shortest* acceptable string will be parsed and
// only if there is not any legal string will the parsing fail.  The shortest
// acceptable string corresponds to the string delimited by '[' and the first
// ']'.
//
///DEFINITION OF SYMBOLS USED IN REGULAR EXPRESSION DESCRIPTIONS
///-------------------------------------------------------------
//
// The following grammar is used to specify regular expressions:
//..
//   .     A period (.) is a one-character RE (regular expression) that matches
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
//   ^     Matches the beginning of the string.
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
//   ?     A regular expression followed by ? means zero or one time.
//
//   |     Logical OR between two expressions means one must be present.
//
//   {m} {m,} {m,u} Integer values enclosed in {} indicate the number of times
//         the preceding regular expression is to be applied.  The value m is
//         the minimum number and u is the max.  If only m is present (that
//         is, {m}), it indicates the exact number of times the regular
//         expression is to be applied.  The value {m,} is analogous to
//         {m,infinity}.  The plus (+) and star (*) operations are
//         equivalent to {1,} and {0,} respectively.
//
//   ( ... ) Parentheses are used for grouping.  An operator, for example, *,
//         +, {}, can work on a single character or a regular expression
//         enclosed in parentheses.  For example, (a*(cb+)*)$0.
//
//    \    Backslash can escape any of the symbols defined above, where
//         meaning is otherwise possibly ambiguous.
//..
//
///DEFINITION OF TERMS USED IN REGULAR EXPRESSION DESCRIPTIONS
///-----------------------------------------------------------
//..
// <DIGIT>: depending on base can include characters 0-9 and case-insensitive
//      letters.  For example, octal digit is in the range [0 .. 7].
// <NUMBER>: (+|-){0, 1} <DIGIT>+
// <OCTAL_DIGIT>: [01234567]
// <HEX_DIGIT>: [0123456789abcdefABCDEF]
// <SIMPLE_ESCAPE_SEQUENCE>: [\a \b \f \n \r \t \v \\ \? \' \"]
// <OCTAL_ESCAPE_SEQUENCE>: \\<OCTAL_DIGIT>{1,3}
//      The numeric part of the value of the sequence must be in range
//      [0 .. UCHAR_MAX].  The sequence is considered complete when a first
//      character that is not an <OCTAL_DIGIT> is found.  (I.e., \1234 is not
//      a valid sequence, but \1239 is.)
// <HEX_ESCAPE_SEQUENCE>: \(x|X) <HEX_DIGIT>{1,2}
//      The numeric part of the value of the sequence must be in range
//      [0 .. UCHAR_MAX].  The sequence is considered complete when a first
//      character, which is not a <HEX_DIGIT> is found.  (I.e., \9abc is not a
//      valid sequence, but \9ab_ is.)
// <ESCAPE_SEQUENCE>: <SIMPLE_ESCAPE_SEQUENCE> | <OCTAL_ESCAPE_SEQUENCE> |
//                     <HEX_ESCAPE_SEQUENCE>
// <CHAR_VAL>: [^\\] | <ESCAPE_SEQUENCE>
// <CHAR>: \'<CHAR_VAL>\'
// <SHORT>: <NUMBER>
//      <SHORT> must be in range [SHRT_MIN .. SHRT_MAX].
// <INT>: <NUMBER>
//      <INT> must be in range [INT_MIN .. INT_MAX].
// <INT64>: <NUMBER>
//      <INT64> must be in range
//                           [-0x8000000000000000uLL .. 0x7FFFFFFFFFFFFFFFuLL].
// <REAL>: <NUMBER> {0, 1} \. {0, 1} <DIGIT>* (e|E <NUMBER>) {0, 1}
//      The number of digits before the exponent part must be greater than 0.
// <FLOAT>: <REAL>
//      <FLOAT> must be in range [FLT_MIN .. FLT_MAX].
// <DOUBLE>: <REAL>
//      <DOUBLE> must be in range [DBL_MIN .. DBL_MAX].
// <WHITESPACE> : any character for which 'bsl::isspace' returns 'true', or
//       any legal C- or C++-style comment.
// <NONWHITESPACE> : any character for which 'bsl::isspace' returns 'false'
// <EOL> : End of line, or ASCII 0.
// <UNQUOTED STRING>: <WHITESPACE>*[^"]* (<WHITESPACE> | <EOL>) (i.e., zero or
//       more characters bounded by a mandatory whitespace or end-of-line
//       character on the right, and optional whitespace characters on the
//       left).  None of the characters may be double-quote.
// <DELIMITED STRING> : <NONWHITESPACE>?<CHAR_VAL>*<NONWHITESPACE>.  Note that
//                      the left and right delimiters need not be the same
//                      character.
// <QUOTED STRING>: \"<CHAR_VAL>*\" (i.e., zero or more characters or escape
//      sequences surrounded by double quotes.  Obviously a special case of a
//      delimited string).
// <STRING>: <QUOTED STRING> | <UNQUOTED STRING>
// <YEAR>: <DIGIT>{1,4}  in range [1 .. 9999]
// <MONTH>: <DIGIT>{1,2}  in range [1 .. 12]
// <DAY>: <DIGIT>{1,2}  in range [1 .. 31]
// <DATE>: <YEAR>/<MONTH>/<DAY>
//      In addition to the correct ranges of <YEAR>, <MONTH>, and <DAY>, the
//      whole <DATE> must be valid as specified in 'bdet_Date'.
// <HOUR>: <DIGIT>{1,2}  in range [1 .. 24]
// <MINUTE>: <DIGIT><DIGIT>  in range [0 .. 59]
// <SECOND>: <DIGIT><DIGIT>  in range [0 .. 59]
// <MILLISECOND>: <DIGIT>{1,3}  in range [0 .. 999]
// <TIME1>: <HOUR>:<MINUTE>:<SECOND>\.<MILLISECOND> (i.e., one to two digits
//      representing the hour followed by a colon and one to two digits
//      representing the minute followed by a colon and one to two digits
//      representing the second followed by a period and one to three digits
//      representing the millisecond.)  In addition to the correct ranges of
//      <HOUR>, <MINUTE>, <SECOND>, and <MILLISECOND>, the whole <TIME> must
//      be valid as specified in 'bdet_Time'.  Specifically, the only valid
//      time where <HOUR> equals '24' is when all other fields equal to zero.
// <TIME2>: <HOUR>:<MINUTE>:<SECOND>
// <TIME3>: <HOUR>:<MINUTE>
// <TIME>: <TIME1> | <TIME2> | <TIME3>
// <DATETIME>: <DATE>(' ' | _ )<TIME> (I.e., <DATE> as defined above
//      followed by one space or one underline character followed by
//      <TIME> as defined above.)
// <DATE_TZ>: <DATE><TZ>
// <DATETIME_TZ>: <DATETIME><TZ>
// <TIME_TZ>: <TIME><TZ>
// <TZ>: (+|-) <DECIMAL_DIGIT>{4}
//..
///NOTES ABOUT FLOATING POINT VALUES
///---------------------------------
// Note that the conversion to and from text for values of type 'float' and
// 'double' is potentially lossy.   For example, the ASCII string "3.14159" is
// converted, on some platforms, to 3.1415899999999999.  (See the
// 'bdeimp_fuzzy' component for functions that compare floating point values
// for approximate equality.)
//
// Note also that the NaN class of 'double' values is not uniform across all
// platforms and is therefore not supported by these parsers.  The textual
// representation of such a value, if generated, is left unspecified, and such
// values will NOT necessarily be restored to an equivalent state when parsed.
//
///NOTES ABOUT STANDARD ARRAY FORMAT
///---------------------------------
// The standard array format is [<WHITESPACE>*(<ITEM><WHITESPACE>+)*] where the
// array contains things using the parse rule <ITEM>.  The array is delimited
// by '[' and ']' and does not allow for leading white-space; the first parsed
// character must be '['.  The white-space after the '[' is optional and then
// a list of white-space terminated values is allowed.  This representation
// allows empty arrays to be represented as "[]".
//
///Usage
///-----
// The following snippets of code illustrate how to generate an array of
// characters representing an integer array and then parse the character array
// to obtain an integer array equal to the initial integer array.  First,
// create the initial array:
//..
//      bsl::vector<int> initial;
//      initial.push_back(1);
//      initial.push_back(2);
//      initial.push_back(3);
//      initial.push_back(4);
//..
// Second, create and populate a character array representing the integer array
// 'initial':
//..
//      bsl::vector<char> buffer;
//      bdepu_ArrayParserImpUtil::generateIntArray(&buffer, initial);
//      assert(0 == bsl::strncmp("[ 1 2 3 4 ]", buffer.data(), 11));
//..
// Finally, populate a new integer array and ensure it has the same value as
// the 'initial' integer array.
//..
//      bsl::vector<int> final;
//      const char *pos;
//      bdepu_ArrayParserImpUtil::parseIntArray(&pos, &final, buffer.data());
//      assert(buffer.data() + 11 == pos);
//      assert(initial == final);
//..

#ifndef INCLUDED_BDESCM_VERSION
#include <bdescm_version.h>
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

                          // ===============================
                          // struct bdepu_ArrayParserImpUtil
                          // ===============================

struct bdepu_ArrayParserImpUtil {
    // This struct provides a namespace for a suite of low-level stateless
    // procedures that perform parsing and generating functionality for arrays
    // of character, numeric, and bdet vocabulary types.  These procedures all
    // follow a certain pattern of operation:
    //
    // Each of the 'parse' functions calls 'skipWhiteSpace' to eliminate
    // internal whitespace (such as tabs, and newlines) as well as C- and
    // C++-style comments (i.e., "//...\n" and "/*...*/").  Note that leading
    // whitespace is not permitted by the parsing methods.  In all cases, these
    // parsing functions fill a vector of the type being parsed, and return 0
    // on success and non-zero otherwise.  The first argument is always the
    // address of a modifiable pointer to the non-modifiable character
    // immediately following the successfully parsed text, or the position at
    // which the parse failure was detected, if an error occurred.
    //
    // Each of the 'generate' functions emits text that the corresponding
    // parsing function can recognize.  The 'generate' functions operate on a
    // 'bsl::vector<char>', which is used to maintain an efficient
    // representation of the output.

  public:
    // CLASS METHODS
    static void generateCharArray(bsl::vector<char>        *buffer,
                                  const bsl::vector<char>&  value);
        // Append the specified 'value' to the specified 'buffer'.  The
        // 'buffer' is formatted as a space-terminated list of <CHAR> (as
        // defined above) surrounded by "[ " and ']'.  Note that 'value' is
        // output as a textual representation that is parsable by the
        // corresponding 'parseArray' function.

    static void generateDateArray(bsl::vector<char>             *buffer,
                                  const bsl::vector<bdet_Date>&  value);
        // Append the specified 'value' to the specified 'buffer'.  The
        // 'buffer' is formatted as a space-terminated list of <DATE> (as
        // defined above) surrounded by "[ " and ']'.  Note that 'value' is
        // output as a textual representation that is parsable by the
        // corresponding 'parseArray' function.

    static void generateDatetimeArray(bsl::vector<char>                *buffer,
                                      const bsl::vector<bdet_Datetime>& value);
        // Append the specified 'value' to the specified 'buffer'.  The
        // 'buffer' is formatted as a space-terminated list of <DATETIME> (as
        // defined above) surrounded by "[ " and ']'.  Note that 'value' is
        // output as a textual representation that is parsable by the
        // corresponding 'parseArray' function.

    static void generateDatetimeTzArray(
                                bsl::vector<char>                   *buffer,
                                const bsl::vector<bdet_DatetimeTz>&  value);
        // Append the specified 'value' to the specified 'buffer'.  The
        // 'buffer' is formatted as a space-terminated list of <DATETIME_TZ>
        // (as defined above) surrounded by "[ " and ']'.  Note that 'value' is
        // output as a textual representation that is parsable by the
        // corresponding 'parseArray' function.

    static void generateDateTzArray(bsl::vector<char>             *buffer,
                                  const bsl::vector<bdet_DateTz>&  value);
        // Append the specified 'value' to the specified 'buffer'.  The
        // 'buffer' is formatted as a space-terminated list of <DATE_TZ> (as
        // defined above) surrounded by "[ " and ']'.  Note that 'value' is
        // output as a textual representation that is parsable by the

    static void generateDoubleArray(bsl::vector<char>          *buffer,
                                    const bsl::vector<double>&  value);
        // Append the specified 'value' to the specified 'buffer'.  The
        // 'buffer' is formatted as a space-terminated list of <DOUBLE> (as
        // defined above) surrounded by "[ " and ']'.  Note that 'value' is
        // output as a textual representation that is parsable by the
        // corresponding 'parseArray' function.

    static void generateFloatArray(bsl::vector<char>         *buffer,
                                   const bsl::vector<float>&  value);
        // Append the specified 'value' to the specified 'buffer'.  The
        // 'buffer' is formatted as a space-terminated list of <FLOAT> (as
        // defined above) surrounded by "[ " and ']'.  Note that 'value' is
        // output as a textual representation that is parsable by the
        // corresponding 'parseArray' function.

    static void generateInt64Array(
                          bsl::vector<char>                            *buffer,
                          const bsl::vector<bsls_PlatformUtil::Int64>&  value);
        // Append the specified 'value' to the specified 'buffer'.  The
        // 'buffer' is formatted as a space-terminated list of <INT64> (as
        // defined above) surrounded by "[ " and ']'.  Note that 'value' is
        // output as a textual representation that is parsable by the
        // corresponding 'parseArray' function.

    static void generateIntArray(bsl::vector<char>       *buffer,
                                 const bsl::vector<int>&  value);
        // Append the specified 'value' to the specified 'buffer'.  The
        // 'buffer' is formatted as a space-terminated list of <INT> (as
        // defined above) surrounded by "[ " and ']'.  Note that 'value' is
        // output as a textual representation that is parsable by the
        // corresponding 'parseArray' function.

    static void generateShortArray(bsl::vector<char>         *buffer,
                                   const bsl::vector<short>&  value);
        // Append the specified 'value' to the specified 'buffer'.  The
        // 'buffer' is formatted as a space-terminated list of <SHORT> (as
        // defined above) surrounded by "[ " and ']'.  Note that 'value' is
        // output as a textual representation that is parsable by the
        // corresponding 'parseArray' function.

    static void generateStringArray(bsl::vector<char>               *buffer,
                                    const bsl::vector<bsl::string>&  value);
        // Append the specified 'value' to the specified 'buffer'.  The
        // 'buffer' is formatted as a space-terminated list of <STRING> (as
        // defined above) surrounded by "[ " and ']'.  Note that 'value' is
        // output as a textual representation that is parsable by the
        // corresponding 'parseArray' function.

    static void generateTimeArray(bsl::vector<char>             *buffer,
                                  const bsl::vector<bdet_Time>&  value);
        // Append the specified 'value' to the specified 'buffer'.  The
        // 'buffer' is formatted as a space-terminated list of <TIME> (as
        // defined above) surrounded by "[ " and ']'.  Note that 'value' is
        // output as a textual representation that is parsable by the
        // corresponding 'parseArray' function.

    static void generateTimeTzArray(bsl::vector<char>               *buffer,
                                    const bsl::vector<bdet_TimeTz>&  value);
        // Append the specified 'value' to the specified 'buffer'.  The
        // 'buffer' is formatted as a space-terminated list of <TIME_TZ> (as
        // defined above) surrounded by "[ " and ']'.  Note that 'value' is
        // output as a textual representation that is parsable by the
        // corresponding 'parseArray' function.

    static int parseCharArray(const char        **endPos,
                              bsl::vector<char>  *result,
                              const char         *inputString);
        // Parse the specified 'inputString' for a sequence of elements
        // matching the regular expression [<WS>*(<CHAR><WS>+)*], where <WS> is
        // short for <WHITESPACE> (see above) and <CHAR> represents a value
        // parsable by 'bdepu_TypesParserImpUtil::parseChar' (i.e., a possibly
        // empty whitespace-terminated list of <CHAR> surrounded by [ and ]),
        // and place into the specified 'result' the corresponding value.
        // Store in the specified '*endPos' the address of the non-modifiable
        // character in 'inputString' immediately following the successfully
        // parsed text, or the position at which the parse failure was
        // detected.  The value of '*result' is the array of elements parsed
        // until failure.  Return zero on success, and non-zero otherwise.  The
        // behavior is undefined unless all arguments are non-zero.
        //..
        // A parse failure can occur for the following reasons:
        //   1. opening or closing bracket is missing
        //   2. elements of the array are not whitespace-terminated
        //   3. parsing of a particular array element failed
        //..

    static int parseDateArray(const char             **endPos,
                              bsl::vector<bdet_Date>  *result,
                              const char              *inputString);
        // Parse the specified 'inputString' for a sequence of elements
        // matching the regular expression [<WS>*(<DATE><WS>+)*], where <WS> is
        // short for <WHITESPACE> (see above) and <DATE> represents a value
        // parsable by 'bdepu_TypesParserImpUtil::parseDate' (i.e., a possibly
        // empty whitespace-terminated list of <DATE> surrounded by [ and ]),
        // and place into the specified 'result' the corresponding value.
        // Store in the specified '*endPos' the address of the non-modifiable
        // character in 'inputString' immediately following the successfully
        // parsed text, or the position at which the parse failure was
        // detected.  The value of '*result' is the array of elements parsed
        // until failure.  Return zero on success, and non-zero otherwise.  The
        // behavior is undefined unless all arguments are non-zero.
        //..
        // A parse failure can occur for the following reasons:
        //   1. opening or closing bracket is missing
        //   2. elements of the array are not whitespace-terminated
        //   3. parsing of a particular array element failed
        //..

    static int parseDatetimeArray(const char                 **endPos,
                                  bsl::vector<bdet_Datetime>  *result,
                                  const char                  *inputString);
        // Parse the specified 'inputString' for a sequence of elements
        // matching the regular expression [<WS>*(<DATETIME><WS>+)*], where
        // <WS> is short for <WHITESPACE> (see above) and <DATETIME>
        // represents a value parsable by
        // 'bdepu_TypesParserImpUtil::parseDatetime' (i.e., a possibly empty
        // whitespace-terminated list of <DATETIME> surrounded by [ and ]), and
        // place into the specified 'result' the corresponding value.  Store in
        // the specified '*endPos' the address of the non-modifiable character
        // in 'inputString' immediately following the successfully parsed text,
        // or the position at which the parse failure was detected.  The value
        // of '*result' is the array of elements parsed until failure.  Return
        // zero on success, and non-zero otherwise.  The behavior is undefined
        // unless all arguments are non-zero.
        //..
        // A parse failure can occur for the following reasons:
        //   1. opening or closing bracket is missing
        //   2. elements of the array are not whitespace-terminated
        //   3. parsing of a particular array element failed
        //..

    static int parseDatetimeTzArray(
                                 const char                   **endPos,
                                 bsl::vector<bdet_DatetimeTz>  *result,
                                 const char                    *inputString);
        // Parse the specified 'inputString' for a sequence of elements
        // matching the regular expression [<WS>*(<DATETIME_TZ><WS>+)*], where
        // <WS> is short for <WHITESPACE> (see above) and <DATETIME_TZ>
        // represents a value parsable by
        // 'bdepu_TypesParserImpUtil::parseDatetimeTz' (i.e., a possibly empty
        // whitespace-terminated list of <DATETIME_TZ> surrounded by [ and ]),
        // and place into the specified 'result' the corresponding value.
        // Store in the specified '*endPos' the address of the non-modifiable
        // character in 'inputString' immediately following the successfully
        // parsed text, or the position at which the parse failure was
        // detected.  The value of '*result' is the array of elements parsed
        // until failure.  Return zero on success, and non-zero otherwise.  The
        // behavior is undefined unless all arguments are non-zero.
        //..
        // A parse failure can occur for the following reasons:
        //   1. opening or closing bracket is missing
        //   2. elements of the array are not whitespace-terminated
        //   3. parsing of a particular array element failed
        //..

    static int parseDateTzArray(const char               **endPos,
                                bsl::vector<bdet_DateTz>  *result,
                                const char                *inputString);
        // Parse the specified 'inputString' for a sequence of elements
        // matching the regular expression [<WS>*(<DATE_TZ><WS>+)*], where <WS>
        // is short for <WHITESPACE> (see above) and <DATE_TZ> represents a
        // value parsable by 'bdepu_TypesParserImpUtil::parseDateTz' (i.e., a
        // possibly empty whitespace-terminated list of <DATE_TZ> surrounded by
        // [ and ]), and place into the specified 'result' the corresponding
        // value.  Store in the specified '*endPos' the address of the
        // non-modifiable character in 'inputString' immediately following the
        // successfully parsed text, or the position at which the parse failure
        // was detected.  The value of '*result' is the array of elements
        // parsed until failure.  Return zero on success, and non-zero
        // otherwise.  The behavior is undefined unless all arguments are
        // non-zero.
        //..
        // A parse failure can occur for the following reasons:
        //   1. opening or closing bracket is missing
        //   2. elements of the array are not whitespace-terminated
        //   3. parsing of a particular array element failed
        //..

    static int parseDoubleArray(const char          **endPos,
                                bsl::vector<double>  *result,
                                const char           *inputString);
        // Parse the specified 'inputString' for a sequence of elements
        // matching the regular expression [<WS>*(<DOUBLE><WS>+)*], where <WS>
        // is short for <WHITESPACE> (see above) and <DOUBLE> represents a
        // value parsable by 'bdepu_TypesParserImpUtil::parseDouble' (i.e., a
        // possibly empty whitespace-terminated list of <DOUBLE> surrounded by
        // [ and ]), and place into the specified 'result' the corresponding
        // value.  Store in the specified '*endPos' the address of the
        // non-modifiable character in 'inputString' immediately following the
        // successfully parsed text, or the position at which the parse failure
        // was detected.  The value of '*result' is the array of elements
        // parsed until failure.  Return zero on success, and non-zero
        // otherwise.  The behavior is undefined unless all arguments are
        // non-zero.
        //..
        // A parse failure can occur for the following reasons:
        //   1. opening or closing bracket is missing
        //   2. elements of the array are not whitespace-terminated
        //   3. parsing of a particular array element failed
        //..

    static int parseFloatArray(const char         **endPos,
                               bsl::vector<float>  *result,
                               const char          *inputString);
        // Parse the specified 'inputString' for a sequence of elements
        // matching the regular expression [<WS>*(<FLOAT><WS>+)*], where <WS>
        // is short for <WHITESPACE> (see above) and <FLOAT> represents a value
        // parsable by 'bdepu_TypesParserImpUtil::parseFloat' (i.e., a possibly
        // empty whitespace-terminated list of <FLOAT> surrounded by [ and ]),
        // and place into the specified 'result' the corresponding value.
        // Store in the specified '*endPos' the address of the non-modifiable
        // character in 'inputString' immediately following the successfully
        // parsed text, or the position at which the parse failure was
        // detected.  The value of '*result' is the array of elements parsed
        // until failure.  Return zero on success, and non-zero otherwise.  The
        // behavior is undefined unless all arguments are non-zero.
        //..
        // A parse failure can occur for the following reasons:
        //   1. opening or closing bracket is missing
        //   2. elements of the array are not whitespace-terminated
        //   3. parsing of a particular array element failed
        //..

    static int parseInt64Array(
                         const char                            **endPos,
                         bsl::vector<bsls_PlatformUtil::Int64>  *result,
                         const char                             *inputString);
        // Parse the specified 'inputString' for a sequence of elements
        // matching the regular expression [<WS>*(<INT64><WS>+)*], where <WS>
        // is short for <WHITESPACE> (see above) and <INT64> represents a value
        // parsable by 'bdepu_TypesParserImpUtil::parseInt64' (i.e., a possibly
        // empty whitespace-terminated list of <INT64> surrounded by [ and ]),
        // and place into the specified 'result' the corresponding value.
        // Store in the specified '*endPos' the address of the non-modifiable
        // character in 'inputString' immediately following the successfully
        // parsed text, or the position at which the parse failure was
        // detected.  The value of '*result' is the array of elements parsed
        // until failure.  Return zero on success, and non-zero otherwise.  The
        // behavior is undefined unless all arguments are non-zero.
        //..
        // A parse failure can occur for the following reasons:
        //   1. opening or closing bracket is missing
        //   2. elements of the array are not whitespace-terminated
        //   3. parsing of a particular array element failed
        //..

    static int parseIntArray(const char       **endPos,
                             bsl::vector<int>  *result,
                             const char        *inputString);
        // Parse the specified 'inputString' for a sequence of elements
        // matching the regular expression [<WS>*(<INT><WS>+)*], where <WS> is
        // short for <WHITESPACE> (see above) and <INT> represents a value
        // parsable by 'bdepu_TypesParserImpUtil::parseInt' (i.e., a possibly
        // empty whitespace-terminated list of <INT> surrounded by [ and ]),
        // and place into the specified 'result' the corresponding value.
        // Store in the specified '*endPos' the address of the non-modifiable
        // character in 'inputString' immediately following the successfully
        // parsed text, or the position at which the parse failure was
        // detected.  The value of '*result' is the array of elements parsed
        // until failure.  Return zero on success, and non-zero otherwise.  The
        // behavior is undefined unless all arguments are non-zero.
        //..
        // A parse failure can occur for the following reasons:
        //   1. opening or closing bracket is missing
        //   2. elements of the array are not whitespace-terminated
        //   3. parsing of a particular array element failed
        //..

    static int parseShortArray(const char         **endPos,
                               bsl::vector<short>  *result,
                               const char          *inputString);
        // Parse the specified 'inputString' for a sequence of elements
        // matching the regular expression [<WS>*(<SHORT><WS>+)*], where <WS>
        // is short for <WHITESPACE> (see above) and <SHORT> represents a value
        // parsable by 'bdepu_TypesParserImpUtil::parseShort' (i.e., a possibly
        // empty whitespace-terminated list of <SHORT> surrounded by [ and ]),
        // and place into the specified 'result' the corresponding value.
        // Store in the specified '*endPos' the address of the non-modifiable
        // character in 'inputString' immediately following the successfully
        // parsed text, or the position at which the parse failure was
        // detected.  The value of '*result' is the array of elements parsed
        // until failure.  Return zero on success, and non-zero otherwise.  The
        // behavior is undefined unless all arguments are non-zero.
        //..
        // A parse failure can occur for the following reasons:
        //   1. opening or closing bracket is missing
        //   2. elements of the array are not whitespace-terminated
        //   3. parsing of a particular array element failed
        //..

    static int parseStringArray(const char               **endPos,
                                bsl::vector<bsl::string>  *result,
                                const char                *inputString);
        // Parse the specified 'inputString' for a sequence of elements
        // matching the regular expression [<WS>*(<STRING><WS>+)*], where <WS>
        // is short for <WHITESPACE> (see above) and <STRING> represents a
        // value parsable by 'bdepu_TypesParserImpUtil::parseString' (i.e., a
        // possibly empty whitespace-terminated list of <STRING> surrounded by
        // [ and ]), and place into the specified 'result' the corresponding
        // value.  Store in the specified '*endPos' the address of the
        // non-modifiable character in 'inputString' immediately following the
        // successfully parsed text, or the position at which the parse failure
        // was detected.  The value of '*result' is the array of elements
        // parsed until failure.  Return zero on success, and non-zero
        // otherwise.  The behavior is undefined unless all arguments are
        // non-zero.
        //..
        // A parse failure can occur for the following reasons:
        //   1. opening or closing bracket is missing
        //   2. elements of the array are not whitespace-terminated
        //   3. parsing of a particular array element failed
        //..

    static int parseTimeArray(const char             **endPos,
                              bsl::vector<bdet_Time>  *result,
                              const char              *inputString);
        // Parse the specified 'inputString' for a sequence of elements
        // matching the regular expression [<WS>*(<TIME><WS>+)*], where <WS> is
        // short for <WHITESPACE> (see above) and <TIME> represents a value
        // parsable by 'bdepu_TypesParserImpUtil::parseTime' (i.e., a possibly
        // empty whitespace-terminated list of <TIME> surrounded by [ and ]),
        // and place into the specified 'result' the corresponding value.
        // Store in the specified '*endPos' the address of the non-modifiable
        // character in 'inputString' immediately following the successfully
        // parsed text, or the position at which the parse failure was
        // detected.  The value of '*result' is the array of elements parsed
        // until failure.  Return zero on success, and non-zero otherwise.  The
        // behavior is undefined unless all arguments are non-zero.
        //..
        // A parse failure can occur for the following reasons:
        //   1. opening or closing bracket is missing
        //   2. elements of the array are not whitespace-terminated
        //   3. parsing of a particular array element failed
        //..

    static int parseTimeTzArray(const char               **endPos,
                                bsl::vector<bdet_TimeTz>  *result,
                                const char                *inputString);
        // Parse the specified 'inputString' for a sequence of elements
        // matching the regular expression [<WS>*(<TIME_TZ><WS>+)*], where <WS>
        // is short for <WHITESPACE> (see above) and <TIME_TZ> represents a
        // value parsable by 'bdepu_TypesParserImpUtil::parseTime' (i.e., a
        // possibly empty whitespace-terminated list of <TIME> surrounded by [
        // and ]), and place into the specified 'result' the corresponding
        // value.  Store in the specified '*endPos' the address of the
        // non-modifiable character in 'inputString' immediately following the
        // successfully parsed text, or the position at which the parse failure
        // was detected.  The value of '*result' is the array of elements
        // parsed until failure.  Return zero on success, and non-zero
        // otherwise.  The behavior is undefined unless all arguments are
        // non-zero.
        //..
        // A parse failure can occur for the following reasons:
        //   1. opening or closing bracket is missing
        //   2. elements of the array are not whitespace-terminated
        //   3. parsing of a particular array element failed
        //..
};

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
