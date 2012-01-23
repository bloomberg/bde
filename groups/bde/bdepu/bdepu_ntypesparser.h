// bdepu_ntypesparser.h                                               -*-C++-*-
#ifndef INCLUDED_BDEPU_NTYPESPARSER
#define INCLUDED_BDEPU_NTYPESPARSER

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Convert scalars/arrays to/from text, understanding "NULL" keyword.
//
//@CLASSES:
//  bdepu_NTypesParser: namespace for scalar and array parsing functions
//
//@AUTHOR: John Lakos (jlakos)
//
//@DESCRIPTION: This component predominantly implements two types of functions:
// 'parse' and 'generate', which form the basic building blocks for top-down
// (and possibly recursive-descent) parsers.  The 'parse' functions initialize
// the resulting object with the value extracted from the input string.  The
// 'generate' functions append a text representation of the object to a
// user-specified 'char' buffer.
//
// The component provides a parse method for each of the following types:
// CHAR, SHORT, INT, INT64, FLOAT, DOUBLE, DATE, DATETZ, TIME, TIMETZ,
// DATETIME, DATETIMETZ, and several parse methods for STRING; it also provides
// parse methods for arrays of these types.  There is a 'generate' method for
// each type for which a 'parse' method exists.
//
// The parsing functions in this component *do* consume leading whitespace.
// For each type mentioned above, and its corresponding array, we have the
// 'Raw' version of the generating function: This allows producing whitespace
// (indentation) before producing the text representing a given value, and also
// between elements for arrays.  The other version supports indentation *and*
// removal of a possible (single) terminating null from the buffer prior to
// writing the value, and addition of a null to the buffer subsequent to
// writing that value.
//
// Although parse methods must parse *any* legal textual representation of an
// object of their type, the generators each produce the canonical
// representation by default.  Legal representations are white-space-neutral,
// however, and clients can direct generators to include more or fewer levels
// of indentation.
//
// Each of the "generate" functions in this class emits text that the
// corresponding parsing function can recognize.  The "generate" functions
// operate on a 'bsl::vector<char>' buffer, which is used to maintain an
// efficient, null-terminated-string representation of the output.  The result
// of generating into a 'bsl::vector<char>' buffer will always be
// null-terminated (whether or not the buffer came in that way).
//
// All functions permit the case-insensitive keyword "NULL" to be substituted
// to indicate the unset value for any of the 'bdet' types (see the
// 'bdetu_unset' component).
//
// The following two subsections describe the grammar defining the parsing
// rules and rules used in the implementation of this component.
//
///DEFINITION OF SYMBOLS USED IN REGULAR EXPRESSION DESCRIPTIONS
///-------------------------------------------------------------
//
//..
// The following grammar is used to specify regular expressions:
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
//   $     Matches the end of the string; \n matches a newline.
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
//   |     Logical OR between preceding and following expression.
//
//   {m} {m,} {m,u} Integer values enclosed in {} indicate the number of times
//         the preceding regular expression is to be applied.  The value m is
//         the minimum number and u is a number.  If only m is present (that
//         is, {m}), it indicates the exact number of times the regular
//         expression is to be applied.  The value {m,} is analogous to
//         {m, infinity}.  The plus (+) and star (*) operations are
//         equivalent to {1,} and {0,} respectively.
//
//   ( ... ) Parentheses are used for grouping.  An operator, for example, *,
//         +, {}, can work on a single character or a regular expression
//         enclosed in parentheses.  For example, (a*(cb+)*)$0.  By necessity,
//         all the above defined symbols are special.  They must, therefore,
//         be escaped with a \ (backslash) to be used as themselves.
//..
//
///DEFINITION OF TERMS USED IN REGULAR EXPRESSION DESCRIPTIONS
///-----------------------------------------------------------
//..
// <NULL> : [Nn][Uu][Ll][Ll]
// <WHITESPACE>: a combination of sequences as defined by 'bsl::isspace', C,
//      and C++ style comments.
// <DIGIT>: depending on base can include characters 0-9 and case-insensitive
//      letters.  For example, octal digit is in the range [0 .. 7].
// <NUMBER>: (+|-){0, 1} <DIGIT>+
// <DECIMAL_DIGIT>: [0123456789]
// <OCTAL_DIGIT>: [01234567]
// <HEX_DIGIT>: [0123456789abcdefABCDEF]
// <SIMPLE_ESCAPE_SEQUENCE>: [\a \b \f \n \r \t \v \\ \? \' \"]
// <OCTAL_ESCAPE_SEQUENCE>: \\<OCTAL_DIGIT>{1,3}
//      The numeric part of the value of the sequence must be in range
//      [0 .. UCHAR_MAX].  The sequence is considered complete when a first
//      character that is not an <OCTAL_DIGIT> is found.  (i.e., \1234 is not
//      a valid sequence, and \1239 is)
// <HEX_ESCAPE_SEQUENCE>: \(x|X) <HEX_DIGIT>{1,2}
//      The numeric part of the value of the sequence must be in range
//      [0 .. UCHAR_MAX].  The sequence is considered complete when a first
//      character, which is not a <HEX_DIGIT> is found.  (i.e., \9abc is not a
//      valid sequence, and \9ab_ is)
// <ESCAPE_SEQUENCE>: [<SIMPLE_ESCAPE_SEQUENCE>, <OCTAL_ESCAPE_SEQUENCE>,
//                     <HEX_ESCAPE_SEQUENCE>]
// <CHAR_VAL>: ([^\\] | <ESCAPE_SEQUENCE>)
// <CHAR>: \'<CHAR_VAL>\'
// <SHORT>: <NUMBER>
//      <SHORT> must be in range [SHRT_MIN .. SHRT_MAX].
// <INT>: <NUMBER>
//      <INT> must be in range [INT_MIN .. INT_MAX].
// <INT64>: <NUMBER>
//      <INT64> must be in range [LONG_MIN .. LONG_MAX].
// <REAL>: <NUMBER> {0, 1} (\.){0, 1} <DIGIT>* (e|E <NUMBER>) {0, 1}
//      The number of digits before the exponent part must be greater than 0.
// <FLOAT>: <REAL>
//      <FLOAT> must be in range [FLT_MIN .. FLT_MAX].
// <DOUBLE>: <REAL>
//      <DOUBLE> must be in range [DBL_MIN .. DBL_MAX].
// <STRING>: \"<CHAR_VAL>*\" (i.e., zero or more characters or escape sequences
//      surrounded by double quotes.)
// <YEAR>: <DIGIT>{1,4}  in range [1 .. 9999]
// <MONTH>: <DIGIT>{1,2}  in range [1 .. 12]
// <DAY>: <DIGIT>{1,2}  in range [1 .. 31]
// <DATE>: <YEAR>/<MONTH>/<DAY>
//      In addition to the correct ranges of <YEAR>, <MONTH>, and <DAY>, the
//      whole <DATE> must be valid as specified in 'bdet_Date'.
// <HOUR>: <DIGIT>{1,2}  in range [1 .. 24]
// <MINUTE>: <DIGIT>{1,2}  in range [0 .. 59]
// <SECOND>: <DIGIT>{1,2}  in range [0 .. 59]
// <MILLISECOND>: <DIGIT>{1,3}  in range [0 .. 999]
// <TIME>: <HOUR>:<MINUTE>:<SECOND>\.<MILLISECOND> (i.e., one to two digits
//      representing the hour followed by a colon and one to two digits
//      representing the minute followed by a colon and one to two digits
//      representing the second followed by a period and one to three digits
//      representing the millisecond.)  In addition to the correct ranges of
//      <HOUR>, <MINUTE>, <SECOND>, and <MILLISECOND>, the whole <TIME> must
//      be valid as specified in 'bdet_Time'.  Specifically, the only valid
//      time where <HOUR> equals '24' is when all other fields equal to zero.
// <DATETIME>: "<DATE> (<WHITESPACE>+|_) <TIME>" (i.e., <DATE> as defined in
//      the above function followed by at least one space or one underline
//      character followed by <TIME> as defined in the above function.)
// <NCHAR>: [<NULL><CHAR>] (if <NULL>, value is determined by 'bdetu_unset')
// <NSHORT>: [<NULL><SHORT>] (if <NULL>, value is determined by 'bdetu_unset')
// <NINT>: [<NULL><INT>] (if <NULL>, value is determined by 'bdetu_unset')
// <NINT64>: [<NULL><INT64>] (if <NULL>, value is determined by 'bdetu_unset')
// <NFLOAT>: [<NULL><FLOAT>] (if <NULL>, value is determined by 'bdetu_unset')
// <NDOUBLE>: [<NULL><DOUBLE>] (if <NULL>, value is determined by
//                              'bdetu_unset')
// <NSTRING>: [<NULL><STRING>] (if <NULL>, value is determined by
//                              'bdetu_unset')
// <NDATE>: [<NULL><DATE>] (if <NULL>, value is determined by 'bdetu_unset')
// <NTIME>: [<NULL><TIME>] (if <NULL>, value is determined by 'bdetu_unset')
// <NDATETIME>: [<NULL><DATETIME>] (if <NULL>, value is determined by
//      'bdetu_unset')
// <TZ>: (+|-) <DECIMAL_DIGIT>{4}
// <DATETZ>: <DATE><TZ>
// <TIMETZ>: <TIME><TZ>
// <DATETIMETZ>: <DATETIME><TZ>
// <NDATETZ>: [<NULL><DATETZ>] (if <NULL>, value is determined by
//      'bdetu_unset')
// <NTIMETZ>: [<NULL><TIMETZ>] (if <NULL>, value is determined by
//      'bdetu_unset')
// <NDATETIMETZ>: [<NULL><DATETIMETZ>] (if <NULL>, value is determined by
//      'bdetu_unset')
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
///USAGE 1 - PARSING A COMPOSITE OBJECT
///------------------------------------
// Suppose we have a 'Point' structure containing two 'double' values:
//..
//  struct Point {
//      double x,
//      double y;
//  };
//..
// Consider the following textual representation of an instance of this
// structure, representing the coordinate values x = 1.2 and y = 3.4.
//..
//  ( 1.2, 3.4 )
//..
// We can use the functions in the 'bdepu_typesparser' component to write a
// function 'parsePoint' that loads a 'Point' structure from a string
// containing a leading textual representation of a point as follows:
//..
//  #include <bdepu_typesparser.h>
//
//  enum { SUCCESS = 0, FAILURE };
//  #define FAIL_UNLESS(c) if (!(c)) { *endPos = s; return FAILURE; }
//
//  int parsePoint(const char **endPos, Point *point, const char *s)
//      // Parse the specified string 's' for a sequence of characters matching
//      // the pattern '(' <double> ',' <double> ')' ignoring white space, and
//      // place into the specified 'point' the corresponding value.  Store in
//      // the specified '*endPos' the address of the non-modifiable character
//      // immediately following the successfully parsed text, or the position
//      // at which the parse failure was detected.  Return zero on success,
//      // and a non-zero value otherwise.  The behavior is undefined if any
//      //  argument is 0.
//  {
//      FAIL_UNLESS(0 == bdepu_parserimputil::skipRequiredToken(&s, s, '('));
//      FAIL_UNLESS(0 == bdepu_typesparser::parseDouble(&s, &point.x, s));
//      FAIL_UNLESS(0 == bdepu_parserimputil::skipRequiredToken(&s, s, ','));
//      FAIL_UNLESS(0 == bdepu_typesparser::parseDouble(&s, &point.y, s));
//      FAIL_UNLESS(0 == bdepu_parserimputil::skipRequiredToken(&s, s, ')'));
//      *endPos = s;  // update end character position
//      return SUCCESS;
//  }
//
// #include <iostream>
//
//  int main(int argc, char *argv[])
//  {
//      if (2 != argc) return -1;  // wrong number of arguments
//
//      const char *pointText = argv[1];
//      const char *endPos;
//      Point point;
//
//      if (0 == parsePoint(&endPos, &point, pointText) {
//          bsl::cout << "x = " << point.x << ", y = " << point.y << bsl::endl;
//      }
//      else {
//          bsl::cerr << "Error at character " << endPos - pointText + 1
//                    << bsl::endl;
//      }
//
//      return 0;
//  }
//..
///USAGE 2 - OPTIONAL FIELDS
///-------------------------
// Sometimes a non-zero return status does not imply that the overall parse
// should fail.  Consider the implementation of the following function, which
// loads a 'bsl::string' from either a quoted string literal or C-style
// identifier.
//..
//  #include <bdepu_typesparser.h>
//
//  enum { SUCCESS = 0, FAILURE };
//  #define FAIL_UNLESS(c) if (!(c)) { *endPos = s; return FAILURE; }
//  typedef bdepu_typesparser TP;  // notational convenience
//  typedef bdepu_parserimputil PIU; // notational convenience
//
//  int parseStringOrIdent(const char  **endPos, bsl::string *result,
//                         const char  *s)
//      // Parse the specified string 's' for either a quoted string
//      // literal with standard escape sequences (e.g., "\"Foo\"\t\"Bar\"")
//      // or a C-style identifier (i.e., matching the regular expression
//      // "[A-Za-z_][A-Za-z0-9_]*").
//  {
//      const char *s2;
//
//      if (!(0 == TP::parseString(endPos, result, s))) {
//          FAIL_UNLESS(0 == PIU::parseIdentifier(endPos, result, s));
//      }
//
//    return SUCCESS;
//  }
//..
///USAGE 3 - VARIABLE FIELDS
///-------------------------
// Sometimes the length of the input is not fixed.  Consider a Comma-Separated
// List (CSL) representation of an integer array:
//..
//   [ 1, 4, 9, 16, 25 ]
//..
// The following is an implementation of a 'parseIntArray' function similar
// (but not identical) to the 'parseIntArray' found in this component.  This
// implementation accepts this CSL text representation and loads the
// corresponding value into a 'bsl::vector<int>'.
//..
//  #include <bdepu_parserimputil.h>
//  #include <bdepu_typesparser.h>
//  #include <assert.h>
//
//  #define SUCCEED { *endPos = s; return 0; }
//  #define FAIL_UNLESS(c) if (!(c)) { *endPos = s; return 1; }
//  typedef bdepu_NTypesParser TP;
//  typedef bdepu_ParserImpUtil PIU;
//
//  int parseIntArray(                          // (1) function name
//      const char    **endPos,
//      bsl::vector<int>  *result,                 // (2) type of array to load
//      const char     *s
//  )                                           // (3) function-level comment
//      // Parse the specified string 's' for a sequence of characters matching
//      // the pattern '[' (<int> (',' <int>)*)? ']' ignoring white space,
//      // and place into the specified 'result' the corresponding value.
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
//      result->setLength(0);
//
//      FAIL_UNLESS(0 == PIU::skipRequiredToken(&s, s, '['));
//
//      while (1) {
//          if (0 == PIU::skipRequiredToken(&s2, s, ']')) {
//              s = s2;
//              SUCCEED;
//          }
//
//          if (valueCount) {
//              FAIL_UNLESS(0 == PIU::skipRequiredToken(&s, s, ','));
//          }
//
//          int value;                          // (4) type of each element
//
//          FAIL_UNLESS(0 == parseInt(&s, &value, s));  // (5) element parser
//
//          result->append(value);
//
//          ++valueCount;
//      }
//      assert(!"Cannot possibly get here!");
//  }
//..
// Note that a similar function to load a 'bsl::vector<Point>' such as
//..
//   [ (.1, .01), (.2, .04), (0.3, 0.09), (0.4, 0.16), (0.5, 0.25) ]
//..
// is accomplished simply by replacing
//..
//   1. 'parseIntArray' with 'parsePointArray'
//   2. The array type 'bsl::vector<int>' with 'bsl::vector<MyPoint>'
//   3. The pattern in the function-level comment [ (<int> (, <int>)*)? ] with
//      [ (<Point> (, <Point>)*)? ]
//   4. The element type 'int' with 'Point'
//   5. The element parser function 'parseInt' with 'parsePoint'.
//..

#ifndef INCLUDED_BDESCM_VERSION
#include <bdescm_version.h>
#endif

#ifndef INCLUDED_BDEPU_PARSERIMPUTIL
#include <bdepu_parserimputil.h>
#endif

#ifndef INCLUDED_BDEPU_TYPESPARSERIMPUTIL
#include <bdepu_typesparserimputil.h>
#endif

#ifndef INCLUDED_BDETU_UNSET
#include <bdetu_unset.h>
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

#ifndef INCLUDED_BSL_CCTYPE
#include <bsl_cctype.h>        // 'isspace'
#endif

#ifndef INCLUDED_BSL_CSTDLIB
#include <bsl_cstdlib.h>       // 'bsl::size_t'
#endif

#ifndef INCLUDED_BSL_CSTRING
#include <bsl_cstring.h>       // 'strlen'
#endif

#ifndef INCLUDED_BSL_STRING
#include <bsl_string.h>
#endif

#ifndef INCLUDED_BSL_VECTOR
#include <bsl_vector.h>
#endif

namespace BloombergLP {

                        // =========================
                        // struct bdepu_NTypesParser
                        // =========================

struct bdepu_NTypesParser {
    // This struct provides a namespace for a suite of stateless procedures
    // that perform low-level parsing functionality of several vocabulary
    // types and arrays of these types.  This parsing framework recognizes
    // stream-based languages that ignore C- and C++-style comments and
    // whitespace between tokens.  In particular, every parsing function in
    // this struct calls 'skipWhiteSpace' initially to eliminate leading
    // whitespace (such as tabs, and newlines) as well as C- and C++-style
    // comments (i.e., "//...\n" and "/*...*/").  In all cases, these parsing
    // functions return 0 on success and non-zero otherwise.  The first
    // argument is always the address of a modifiable pointer to the
    // non-modifiable character immediately following the successfully parsed
    // text, or the position at which the parse failure was detected, if an
    // error occurred.
    //
    // Each of the "generate" functions in this struct emits text that the
    // corresponding parsing function can recognize.  The "generate" functions
    // operate on a 'bsl::vector<char>' buffer, which is used to maintain an
    // efficient, null-terminated-string representation of the output.  The
    // result of generating into a 'bsl::vector<char>' buffer will always be
    // null-terminated (whether or not the buffer came in that way).
    //
    // Both the parsers and generators recognize and use the keyword NULL to
    // refer to values of types defined in 'bdetu_Unset'.

  public:
    // CLASS METHODS

                       // *** PARSE FUNCTIONS ***

    static int parseChar(const char **endPos,
                         char        *result,
                         const char  *inputString);
        // Parse the specified 'inputString' for a sequence of characters
        // matching the regular expression <WHITESPACE>*<NCHAR> and place into
        // the specified 'result' the corresponding value.  Store in the
        // specified '*endPos' the address of the non-modifiable character (in
        // 'inputString') immediately following the successfully parsed text,
        // or the position at which the parse failure was detected.  Return
        // zero on success, and a non-zero value otherwise.  The value of
        // '*result' is unchanged if a parse failure occurs.  The behavior
        // is undefined if any argument is 0.
        //..
        // A parse failure can occur for the following reasons:
        //   1. If the char is an <OCTAL_ESCAPE_SEQUENCE>, then
        //      a. the number of digits in numeric part of the sequence must be
        //         in the range [1 .. 3].
        //      b. the numeric part of the value of the sequence must be in
        //         range [0 .. 255]
        //   2. If the char is a <HEX_ESCAPE_SEQUENCE>, the number of digits in
        //      numeric part of the sequence must be in the range [1 .. 2].
        //   3. If the char is not an <ESCAPE_SEQUENCE>, but starts with
        //      ('\\').
        //..

    static int parseShort(const char **endPos,
                          short       *result,
                          const char  *inputString,
                          int          base = 10);
        // Parse the specified 'inputString' for a sequence of characters
        // matching the regular expression <WHITESPACE>*<NSHORT>, and place
        // into the specified 'result' the corresponding value.  Store in the
        // specified '*endPos' the address of the non-modifiable character (in
        // 'inputString') immediately following the successfully parsed text,
        // or the position at which the parse failure was detected.  Return
        // zero on success, and a non-zero value otherwise.  The value of
        // '*result' is unchanged if a parse failure occurs.  The behavior
        // is undefined if any argument is 0 or if 'base' is not in the
        // range [2 .. 36]  (i.e., bases where digits are representable by
        // characters [0-9a-z] or [0-9A-Z]).
        //..
        // A parse failure can occur for the following reasons:
        //   1. The parsed string is not a <NUMBER>
        //   2. The parsed value is not in the range [SHRT_MIN .. SHRT_MAX]
        //      for the target platform.
        //..

    static int parseInt(const char **endPos,
                        int         *result,
                        const char  *inputString,
                        int          base = 10);
        // Parse the specified 'inputString' for a sequence of characters
        // matching the regular expression <WHITESPACE>*<NINT>, and place into
        // the specified 'result' the corresponding value.  Store in the
        // specified '*endPos' the address of the non-modifiable character (in
        // 'inputString') immediately following the successfully parsed text,
        // or the position at which the parse failure was detected.  Return
        // zero on success, and a non-zero value otherwise.  The value of
        // '*result' is unchanged if a parse failure occurs.  The behavior
        // is undefined if any argument is 0 or if 'base' is not in the
        // range [2 .. 36] (i.e., bases where digits are representable by
        // characters [0-9a-z] or [0-9A-Z]).
        //..
        // A parse failure can occur for the following reasons:
        //   1. The parsed string is not a <NUMBER>
        //   2. The parsed value is not in the range [INT_MIN .. INT_MAX]
        //      for the target platform.
        //..

    static int parseInt64(const char               **endPos,
                          bsls_PlatformUtil::Int64  *result,
                          const char                *inputString,
                          int                        base = 10);
        // Parse the specified 'inputString' for a sequence of characters
        // matching the regular expression <WHITESPACE>*<NINT64>, and place
        // into the specified 'result' the corresponding value.  Store in the
        // specified '*endPos' the address of the non-modifiable character (in
        // 'inputString') immediately following the successfully parsed text,
        // or the position at which the parse failure was detected.  Return
        // zero on success, and a non-zero value otherwise.  The value of
        // '*result' is unchanged if a parse failure occurs.  The behavior
        // is undefined if any argument is 0  if 'base' is not in the range
        // [2 .. 36] (i.e., bases where digits are representable by
        // characters [0-9a-z] or [0-9A-Z]).
        //..
        // A parse failure can occur for the following reasons:
        //   1. The parsed string is not a <NUMBER>
        //   2. The parsed value is not in the range [LONG_MIN .. LONG_MAX]
        //      for the target platform.
        //..

    static int parseFloat(const char **endPos,
                          float       *result,
                          const char  *inputString);
        // Parse the specified 'inputString' for a sequence of characters
        // matching the regular expression <WHITESPACE>*<NFLOAT> and place
        // into the specified 'result' the corresponding value.  Store in the
        // specified '*endPos' the address of the non-modifiable character (in
        // 'inputString') immediately following the successfully parsed text,
        // or the position at which the parse failure was detected.  Return
        // zero on success, and a non-zero value otherwise.  The value of
        // '*result' is unchanged if a parse failure occurs.  The behavior
        // is undefined if any argument is 0.
        //..
        // A parse failure can occur for the following reasons:
        //   1. syntactical errors in the input string (refer to a definition
        //      of <FLOAT>).
        //   2. number of digits before the exponent part is less than one
        //      parsed value is not in the range [FLT_MIN .. FLT_MAX] for the
        //      target platform.
        //..
        // Warning: this function might produce different values for the
        // specified 'result' from the same 'inputString'.  The value of
        // 'result' will be different on platforms, where the value of FLT_DIG
        // is smaller than the number of digits in the mantissa of the number
        // in the 'inputString'.

    static int parseDouble(const char **endPos,
                           double      *result,
                           const char  *inputString);
        // Parse the specified 'inputString' for a sequence of characters
        // matching the regular expression <WHITESPACE>*<NDOUBLE> and place
        // into the specified 'result' the corresponding value.  Store in the
        // specified '*endPos' the address of the non-modifiable character (in
        // 'inputString') immediately following the successfully parsed text,
        // or the position at which the parse failure was detected.  Return
        // zero on success, and a non-zero value otherwise.  The value of
        // '*result' is unchanged if a parse failure occurs.  The behavior
        // is undefined if any argument is 0.
        //..
        // A parse failure can occur for the following reasons:
        //   1. syntactical errors in the input string (refer to a definition
        //      of <DOUBLE>).
        //   2. number of digits before the exponent part is less than one
        //      parsed value is not in the range [DBL_MIN .. DBL_MAX] for the
        //      target platform.
        //..
        // Warning: this function might produce different values for the
        // specified 'result' from the same 'inputString'.  The value of
        // 'result' will be different on platforms, where the value of FLT_DIG
        // is smaller than the number of digits in the mantissa of the number
        // in the 'inputString'.

    static int parseString(const char  **endPos,
                           bsl::string  *result,
                           const char   *inputString);
        // Parse the specified 'inputString' for a sequence of characters
        // matching the regular expression <WHITESPACE>*<NSTRING> (i.e., zero
        // or more characters or escape sequences surrounded by double quotes)
        // and place into the specified 'result' the corresponding value.
        // Store in the specified '*endPos' the address of the non-modifiable
        // character in 'inputString' immediately following the successfully
        // parsed text, or the position at which the parse failure was
        // detected.  Return zero on success, and a non-zero value otherwise.
        // The value of '*result' is undefined if a parse failure occurs.
        // The behavior is undefined if any argument is 0.
        //..
        // A parse failure can occur for the following reasons:
        //   1. The opening or closing double-quote ('"') character is missing.
        //   2. One of the characters in the string is invalid i.e.:
        //      a. If the char is an <OCTAL_ESCAPE_SEQUENCE>, then
        //         1. the numeric part of the value of the sequence must have
        //            no more than three digits.
        //         2. the numeric part of the value of the sequence must be in
        //            range [0 .. UCHAR_MAX]
        //      b. If the char is a <HEX_ESCAPE_SEQUENCE>, then the numeric
        //         part of the value of the sequence must have no more than
        //         two digits.
        //      c. If the char is not an <ESCAPE_SEQUENCE>, but starts with
        //         ('\\').
        //..

    static int parseDelimitedString(const char  **endPos,
                                    bsl::string  *result,
                                    const char   *inputString,
                                    char          leftDelimiter,
                                    char          rightDelimiter);
        // Parse the specified 'inputString' for a sequence of characters
        // matching the regular expression <DELIMITED NSTRING> (i.e., zero or
        // more characters or escape sequences surrounded by a user-specifiable
        // left character and right character ) and place into the specified
        // 'result' the corresponding value.  Store in the specified
        // '*endPos' the address of the non-modifiable character in
        // 'inputString' immediately following the successfully parsed text,
        // or the position at which the parse failure was detected.  Return
        // zero on success, and a non-zero value otherwise.  The value of
        // '*result' is undefined if a parse failure occurs.  The behavior
        // is undefined if any argument is 0.
        //..
        // A parse failure can occur for the following reasons:
        //   1. If either delimiter is missing.
        //   2. If one of the characters in the string is invalid i.e.:
        //      a. If the char is an <OCTAL_ESCAPE_SEQUENCE>, then
        //         1. the numeric part of the value of the sequence must have
        //            no more than three digits.
        //         2. the numeric part of the value of the sequence must be in
        //            range [0 .. UCHAR_MAX]
        //      b. If the char is a <HEX_ESCAPE_SEQUENCE>, then the numeric
        //         part of the value of the sequence must have no more than
        //         two digits.
        //      c. If the char is not an <ESCAPE_SEQUENCE>, but starts with
        //         ('\\').
        //..

    static int parseQuotedString(const char  **endPos,
                                 bsl::string  *result,
                                 const char   *inputString);
        // Parse the specified 'inputString' for a sequence of characters
        // matching the regular expression <QUOTED STRING> (i.e., zero or more
        // characters or escape sequences surrounded by double quotes) and
        // place into the specified 'result' the corresponding value.  Store
        // in the specified '*endPos' the address of the non-modifiable
        // character in 'inputString' immediately following the successfully
        // parsed text, or the position at which the parse failure was
        // detected.  Return zero on success, and a non-zero value otherwise.
        // The value of '*result' is undefined if a parse failure occurs.
        // The behavior is undefined if any argument is 0.
        //..
        // A parse failure can occur for the following reasons:
        //   1. If the opening or closing double-quote ('"') character is
        //      missing.
        //   2. If one of the characters in the string is invalid i.e.:
        //      a. If the char is an <OCTAL_ESCAPE_SEQUENCE>, then
        //         1. the numeric part of the value of the sequence must have
        //            no more than three digits.
        //         2. the numeric part of the value of the sequence must be in
        //            range [0 .. UCHAR_MAX]
        //      b. If the char is a <HEX_ESCAPE_SEQUENCE>, then the numeric
        //         part of the value of the sequence must have no more than
        //         two digits.
        //      c. If the char is not an <ESCAPE_SEQUENCE>, but starts with
        //         ('\\').
        //..

    static int parseUnquotedString(const char  **endPos,
                                   bsl::string  *result,
                                   const char   *inputString);
        // Parse the specified 'inputString' for a sequence of characters
        // matching the regular expression <UNQUOTED STRING> (i.e., zero or
        // more characters or escape sequences followed by at least one
        // whitespace) and place into the specified 'result' the
        // corresponding value.  Store in the specified '*endPos' the address
        // of the first whitespace character located after non-whitespace
        // text has been accumulated, or the position at which the parse
        // failure was detected.  Return zero on success, and non-zero
        // otherwise.  The value of '*result' is undefined if a parse
        // failure occurs.  The behavior is undefined if any argument is 0.
        //..
        // A parse failure can occur for the following reasons:
        //   1. A double-quote ('"') character is found inside the string.
        //   2. If one of the characters in the string is invalid i.e.:
        //      a. If the char is an <OCTAL_ESCAPE_SEQUENCE>, then
        //         1. the numeric part of the value of the sequence must have
        //            no more than three digits.
        //         2. the numeric part of the value of the sequence must be in
        //            range [0 .. UCHAR_MAX]
        //      b. If the char is a <HEX_ESCAPE_SEQUENCE>, then the numeric
        //         part of the value of the sequence must have no more than
        //         two digits.
        //      c. If the char is not an <ESCAPE_SEQUENCE>, but starts with
        //         ('\\').
        //..

    static int parseSpaceDelimitedString(const char  **endPos,
                                         bsl::string  *result,
                                         const char   *inputString);
        // Parse the specified 'inputString' for a sequence of characters
        // matching the regular expression <SPACE-SEPARATED STRING> (i.e., zero
        // or more characters or escape sequences followed by at least one
        // whitespace) and place into the specified 'result' the
        // corresponding value.  Store in the specified '*endPos' the address
        // of the first whitespace character located after non-whitespace text
        // has been accumulated, or the position at which the parse failure was
        // detected.  Return zero on success, and non-zero otherwise.  The
        // value of '*result' is undefined if a parse failure occurs.  The
        // behavior is undefined if any argument is 0.

    static int parseDate(const char **endPos,
                         bdet_Date   *result,
                         const char  *inputString);
        // Parse the specified 'inputString' for a sequence of characters
        // matching the regular expression <WHITESPACE>*<NDATE> (i.e., one to
        // four digits representing the year followed by a slash and one to two
        // digits representing the month followed by a slash and one to two
        // digits representing the day, such as 2003/7/16), and place into the
        // specified 'result' the corresponding value.  Store in the specified
        // '*endPos' the address of the non-modifiable character in
        // 'inputString' immediately following the successfully parsed text, or
        // the position at which the parse failure was detected.  If the
        // parsing fails because the combination of <YEAR>, <MONTH>, and <DAY>
        // is invalid, the '*endPos' will point at the beginning of the <DAY>.
        // Return zero on success, and a non-zero value otherwise.  The value
        // of '*result' is unchanged if a parse failure occurs.  The behavior
        // is undefined if any argument is 0.
        //..
        // A parse failure can occur for the following reasons:
        //   1. <YEAR> is not in range [1 .. 9999]
        //   2. <MONTH> is not in the range [1 .. 12]
        //   3. <DAY> is not in the range [1 .. 31]
        //   4. number of digits in <YEAR> is not in range [1 .. 4]
        //   5. number of digits in <MONTH> is not in range [1 .. 2]
        //   6. number of digits in <DAY> is not in range [1 .. 2]
        //   7. fields are not separated by / character
        //   8. the <YEAR>, <MONTH>, <DATE> combination is invalid (as
        //      specified in 'bdet_Date'.
        //..

    static int parseTime(const char **endPos,
                         bdet_Time   *result,
                         const char  *inputString);
        // Parse the specified 'inputString' for a sequence of characters
        // matching the regular expression <WHITESPACE>*<NTIME> (i.e., one to
        // two digits representing the hour followed by a colon and one to two
        // digits representing the minute followed by a colon and one to two
        // digits representing the second followed by a colon and one to three
        // digits representing the millisecond, as in 3:12:15.234), and place
        // into the specified 'result' the corresponding value.  Store in the
        // specified '*endPos' the address of the non-modifiable character in
        // 'inputString' immediately following the successfully parsed text,
        // or the position at which the parse failure was detected.  Return
        // zero on success, and a non-zero value otherwise.  The value of
        // '*result' is unchanged if a parse failure occurs.  The behavior is
        // undefined if any argument is 0.
        //..
        // A parse failure can occur for the following reasons:
        //   1.  <HOUR> is not in range [1 .. 24]
        //   2.  <MINUTE> is not in the range [0 .. 59]
        //   3.  <SECOND> is not in the range [0 .. 59]
        //   4.  <MILLISECOND> is not in the range [0 .. 999]
        //   5.  number of digits in <HOUR> is not in range [1 .. 2]
        //   6.  number of digits in <MINUTE> is not in range [1 .. 2]
        //   7.  number of digits in <SECOND> is not in range [1 .. 2]
        //   8.  number of digits in <MILLISECOND> is not in range [1 .. 3]
        //   9.  <HOUR>, <MINUTE>, and <SECOND> fields are not separated by ':'
        //       character
        //   10. <SECOND> and <MILLISECOND> fields are not separated by '.'
        //       character
        //   11. when <HOUR> equals 24, the value of one or more of the other
        //       fields is not equals to zero.
        //..

    static int parseDatetime(const char    **endPos,
                             bdet_Datetime  *result,
                             const char     *inputString);
        // Parse the specified 'inputString' for a sequence of characters
        // matching the regular expression <WHITESPACE>*<NDATETIME> (i.e., one
        // to four digits representing the year followed by a slash and one to
        // two digits representing the month followed by a slash and one to
        // two digits representing the day followed by a space or \_ followed
        // by one to two digits representing the hour followed by a colon and
        // one to two digits representing the minute followed by a colon and
        // one to two digits representing the second followed by a colon and
        // one to three digits representing the millisecond, as in 2003/7/16
        // 3:12:15.234) and place into the specified 'result' the
        // corresponding value.  Store in the specified '*endPos' the address
        // of the non-modifiable character in 'inputString' immediately
        // following the successfully parsed text, or the position at which
        // the parse failure was detected.  Return zero on success, and
        // non-zero otherwise.  The value of '*result' is unchanged if a
        // parse failure occurs.  The behavior is undefined if any argument
        // is 0.
        //..
        // A parse failure can occur for the following reasons:
        //   1.  <YEAR> is not in range [1 .. 9999]
        //   2.  <MONTH> is not in the range [1 .. 12]
        //   3.  <DAY> is not in the range [1 .. 31]
        //   4.  number of digits in <YEAR> is not in range [1 .. 4]
        //   5.  number of digits in <MONTH> is not in range [1 .. 2]
        //   6.  number of digits in <DAY> is not in range [1 .. 2]
        //   7.  fields are not separated by '/' character
        //   8.  the <YEAR>, <MONTH>, <DATE> combination is invalid (as
        //       specified in 'bdet_Date'.
        //   9.  <DATE> and <HOUR> are not separated by ' ' or '_'
        //   10. <HOUR> is not in range [1 .. 24]
        //   11. <MINUTE> is not in the range [0 .. 59]
        //   12. <SECOND> is not in the range [0 .. 59]
        //   13. <MILLISECOND> is not in the range [0 .. 999]
        //   14. number of digits in <HOUR> is not in range [1 .. 2]
        //   15. number of digits in <MINUTE> is not in range [1 .. 2]
        //   16. number of digits in <SECOND> is not in range [1 .. 2]
        //   17. number of digits in <MILLISECOND> is not in range [1 .. 3]
        //   18. <HOUR>, <MINUTE>, and <SECOND> fields are not separated by \:
        //       character
        //   19. <SECOND> and <MILLISECOND> fields are not separated by '.'
        //       character
        //   21. when <HOUR> equals 24, the value of one or more of the other
        //       fields is not equal to zero.
        //..

    static int parseDateTz(const char  **endPos,
                           bdet_DateTz  *result,
                           const char   *inputString);
        // Parse the specified 'inputString' for a sequence of characters
        // matching the regular expression <WHITESPACE>*<NDATETZ> (i.e., one to
        // four digits representing the year followed by a slash and one to two
        // digits representing the month followed by a slash and one to two
        // digits representing the day, such as 2003/7/16), and place into the
        // specified 'result' the corresponding value.  Store in the specified
        // '*endPos' the address of the non-modifiable character in
        // 'inputString' immediately following the successfully parsed text, or
        // the position at which the parse failure was detected.  If the
        // parsing fails because the combination of <YEAR>, <MONTH>, and <DAY>
        // is invalid, the '*endPos' will point at the beginning of the <DAY>.
        // Return zero on success, and a non-zero value otherwise.  The value
        // of '*result' is unchanged if a parse failure occurs.  The behavior
        // is undefined if any argument is 0.
        //..
        // A parse failure can occur for the following reasons:
        //   1. <YEAR> is not in range [1 .. 9999]
        //   2. <MONTH> is not in the range [1 .. 12]
        //   3. <DAY> is not in the range [1 .. 31]
        //   4. number of digits in <YEAR> is not in range [1 .. 4]
        //   5. number of digits in <MONTH> is not in range [1 .. 2]
        //   6. number of digits in <DAY> is not in range [1 .. 2]
        //   7. fields are not separated by / character
        //   8. the <YEAR>, <MONTH>, <DATE> combination is invalid (as
        //      specified in 'bdet_Date'.
        //..

    static int parseTimeTz(const char  **endPos,
                           bdet_TimeTz  *result,
                           const char   *inputString);
        // Parse the specified 'inputString' for a sequence of characters
        // matching the regular expression <WHITESPACE>*<NTIMETZ> (i.e., one to
        // two digits representing the hour followed by a colon and one to two
        // digits representing the minute followed by a colon and one to two
        // digits representing the second followed by a colon and one to three
        // digits representing the millisecond, as in 3:12:15.234), and place
        // into the specified 'result' the corresponding value.  Store in the
        // specified '*endPos' the address of the non-modifiable character in
        // 'inputString' immediately following the successfully parsed text,
        // or the position at which the parse failure was detected.  Return
        // zero on success, and a non-zero value otherwise.  The value of
        // '*result' is unchanged if a parse failure occurs.  The behavior is
        // undefined if any argument is 0.
        //..
        // A parse failure can occur for the following reasons:
        //   1.  <HOUR> is not in range [1 .. 24]
        //   2.  <MINUTE> is not in the range [0 .. 59]
        //   3.  <SECOND> is not in the range [0 .. 59]
        //   4.  <MILLISECOND> is not in the range [0 .. 999]
        //   5.  number of digits in <HOUR> is not in range [1 .. 2]
        //   6.  number of digits in <MINUTE> is not in range [1 .. 2]
        //   7.  number of digits in <SECOND> is not in range [1 .. 2]
        //   8.  number of digits in <MILLISECOND> is not in range [1 .. 3]
        //   9.  <HOUR>, <MINUTE>, and <SECOND> fields are not separated by ':'
        //       character
        //   10. <SECOND> and <MILLISECOND> fields are not separated by '.'
        //       character
        //   11. when <HOUR> equals 24, the value of one or more of the other
        //       fields is not equals to zero.
        //..

    static int parseDatetimeTz(const char      **endPos,
                               bdet_DatetimeTz  *result,
                               const char       *inputString);
        // Parse the specified 'inputString' for a sequence of characters
        // matching the regular expression <WHITESPACE>*<NDATETIMETZ> (i.e.,
        // one to four digits representing the year followed by a slash and one
        // to two digits representing the month followed by a slash and one to
        // two digits representing the day followed by a space or \_ followed
        // by one to two digits representing the hour followed by a colon and
        // one to two digits representing the minute followed by a colon and
        // one to two digits representing the second followed by a colon and
        // one to three digits representing the millisecond, as in 2003/7/16
        // 3:12:15.234) and place into the specified 'result' the
        // corresponding value.  Store in the specified '*endPos' the address
        // of the non-modifiable character in 'inputString' immediately
        // following the successfully parsed text, or the position at which
        // the parse failure was detected.  Return zero on success, and
        // non-zero otherwise.  The value of '*result' is unchanged if a
        // parse failure occurs.  The behavior is undefined if any argument
        // is 0.
        //..
        // A parse failure can occur for the following reasons:
        //   1.  <YEAR> is not in range [1 .. 9999]
        //   2.  <MONTH> is not in the range [1 .. 12]
        //   3.  <DAY> is not in the range [1 .. 31]
        //   4.  number of digits in <YEAR> is not in range [1 .. 4]
        //   5.  number of digits in <MONTH> is not in range [1 .. 2]
        //   6.  number of digits in <DAY> is not in range [1 .. 2]
        //   7.  fields are not separated by '/' character
        //   8.  the <YEAR>, <MONTH>, <DATE> combination is invalid (as
        //       specified in 'bdet_Date'.
        //   9.  <DATE> and <HOUR> are not separated by ' ' or '_'
        //   10. <HOUR> is not in range [1 .. 24]
        //   11. <MINUTE> is not in the range [0 .. 59]
        //   12. <SECOND> is not in the range [0 .. 59]
        //   13. <MILLISECOND> is not in the range [0 .. 999]
        //   14. number of digits in <HOUR> is not in range [1 .. 2]
        //   15. number of digits in <MINUTE> is not in range [1 .. 2]
        //   16. number of digits in <SECOND> is not in range [1 .. 2]
        //   17. number of digits in <MILLISECOND> is not in range [1 .. 3]
        //   18. <HOUR>, <MINUTE>, and <SECOND> fields are not separated by \:
        //       character
        //   19. <SECOND> and <MILLISECOND> fields are not separated by '.'
        //       character
        //   21. when <HOUR> equals 24, the value of one or more of the other
        //       fields is not equal to zero.
        //..

    static int parseCharArray(const char        **endPos,
                              bsl::vector<char>  *result,
                              const char         *inputString);
        // Parse the specified 'inputString' for a sequence of characters
        // matching the regular expression <WHITESPACE>* [ <WHITESPACE>*
        // (<NCHAR> <WHITESPACE>*)* ] (i.e., a possibly empty list of
        // <NCHAR> surrounded by [ and ]) and place into the specified
        // 'result' the corresponding value.  Store in the specified
        // '*endPos' the address of the non-modifiable character in
        // 'inputString' immediately following the successfully parsed text,
        // or the position at which the parse failure was detected.  Return
        // zero on success, and a non-zero value otherwise.  The value of
        // '*result' is undefined if a parse failure occurs.  The behavior
        // is undefined if any argument is 0.
        //..
        // A parse failure can occur for the following reasons:
        //   1. The opening or closing square bracket is missing.
        //   2. Parsing of a particular array element fails.
        //..

    static int parseShortArray(const char         **endPos,
                               bsl::vector<short>  *result,
                               const char          *inputString);
        // Parse the specified 'inputString' for a sequence of characters
        // matching the regular expression <WHITESPACE>* [ <WHITESPACE>*
        // (<NSHORT> <WHITESPACE>*)* ], (i.e., a possibly empty list of
        // <NSHORT> surrounded by [ and ]) and place into the specified
        // 'result' the corresponding value.  Store in the specified
        // '*endPos' the address of the non-modifiable character in
        // 'inputString' immediately following the successfully parsed text,
        // or the position at which the parse failure was detected.  Return
        // zero on success, and a non-zero value otherwise.  The value of
        // '*result' is undefined if a parse failure occurs.  The behavior
        // is undefined if any argument is 0.
        //..
        // A parse failure can occur for the following reasons:
        //   1. The opening or closing square bracket is missing.
        //   2. Parsing of a particular array element fails.
        //..

    static int parseIntArray(const char       **endPos,
                             bsl::vector<int>  *result,
                             const char        *inputString);
        // Parse the specified 'inputString' for a sequence of characters
        // matching the regular expression <WHITESPACE>* [<WHITESPACE>*
        // (<NINT> <WHITESPACE>*)* ], (i.e., a possibly empty list of
        // <NINT> surrounded by [ and ]) and place into the specified
        // 'result' the corresponding value.  Store in the specified
        // '*endPos' the address of the non-modifiable character in
        // 'inputString' immediately following the successfully parsed text,
        // or the position at which the parse failure was detected.  Return
        // zero on success, and a non-zero value otherwise.  The value of
        // '*result' is undefined if a/ parse failure occurs.  The behavior
        // is undefined if any argument is 0.
        //..
        // A parse failure can occur for the following reasons:
        //   1. The opening or closing square bracket is missing.
        //   2. Parsing of a particular array element fails.
        //..

    static int parseInt64Array(
                          const char                            **endPos,
                          bsl::vector<bsls_PlatformUtil::Int64>  *result,
                          const char                             *inputString);
        // Parse the specified 'inputString' for a sequence of characters
        // matching the regular expression <WHITESPACE>* [<WHITESPACE>*
        // (<NINT64><WHITESPACE>*)], (i.e., a possibly empty list of
        // <NINT64> surrounded by [ and ]) and place into the specified
        // 'result' the corresponding value.  Store in the specified
        // '*endPos' the address of the non-modifiable character in
        // 'inputString' immediately following the successfully parsed text,
        // or the position at which the parse failure was detected.  Return
        // zero on success, and a non-zero value otherwise.  The value of
        // '*result' is undefined if a parse failure occurs.  The behavior
        // is undefined if any argument is 0.
        //..
        // A parse failure can occur for the following reasons:
        //   1. The opening or closing square bracket is missing.
        //   2. Parsing of a particular array element fails.
        //..

    static int parseFloatArray(const char         **endPos,
                               bsl::vector<float>  *result,
                               const char          *inputString);
        // Parse the specified 'inputString' for a sequence of characters
        // matching the regular expression <WHITESPACE>* [ <WHITESPACE>*
        // (<NFLOAT> <WHITESPACE>*)* ] (i.e., a possibly empty list of
        // <NFLOAT> surrounded by [ and ]) and place into the specified
        // 'result' the corresponding value.  Store in the specified
        // '*endPos' the address of the non-modifiable character in
        // 'inputString' immediately following the successfully parsed text,
        // or the position at which the parse failure was detected.  Return
        // zero on success, and a non-zero value otherwise.  The value of
        // '*result' is undefined if a parse failure occurs.  The behavior
        // is undefined if any argument is 0.
        //..
        // A parse failure can occur for the following reasons:
        //   1. The opening or closing square bracket is missing.
        //   2. Parsing of a particular array element fails.
        //..

    static int parseDoubleArray(const char          **endPos,
                                bsl::vector<double>  *result,
                                const char           *inputString);
        // Parse the specified 'inputString' for a sequence of characters
        // matching the regular expression <WHITESPACE>* [ <WHITESPACE>*
        // (<NDOUBLE> <WHITESPACE>*)* ] (i.e., a possibly empty list of
        // <NDOUBLE> surrounded by [ and ]) and place into the specified
        // 'result' the corresponding value.  Store in the specified
        // '*endPos' the address of the non-modifiable character in
        // 'inputString' immediately following the successfully parsed text,
        // or the position at which the parse failure was detected.  Return
        // zero on success, and a non-zero value otherwise.  The value of
        // '*result' is undefined if a parse failure occurs.  The behavior
        // is undefined if any argument is 0.
        //..
        // A parse failure can occur for the following reasons:
        //   1. The opening or closing square bracket is missing.
        //   2. Parsing of a particular array element fails.
        //..

    static int parseStringArray(const char               **endPos,
                                bsl::vector<bsl::string>  *result,
                                const char                *inputString);
        // Parse the specified 'inputString' for a sequence of characters
        // matching the regular expression <WHITESPACE>* [ <WHITESPACE>*
        // (<NSTRING> <WHITESPACE>*)* ] (i.e., a possibly empty list of
        // <NSTRING> surrounded by [ and ]) and place into the specified
        // 'result' the corresponding value.  Store in the specified
        // '*endPos' the address of the non-modifiable character in
        // 'inputString' immediately following the successfully parsed text,
        // or the position at which the parse failure was detected.  Return
        // zero on success, and a non-zero value otherwise.  The value of
        // '*result' is undefined if a parse failure occurs.  The behavior
        // is undefined if any argument is 0.  Note that strings containing
        // whitespace can be quoted in double quotes.
        //..
        // A parse failure can occur for the following reasons:
        //   1. The opening or closing square bracket is missing.
        //   2. Parsing of a particular array element fails.
        //..

    static int parseDateArray(const char             **endPos,
                              bsl::vector<bdet_Date>  *result,
                              const char              *inputString);
        // Parse the specified 'inputString' for a sequence of characters
        // matching the regular expression <WHITESPACE>* [ <WHITESPACE>*
        // (<NDATE> <WHITESPACE>*)* ] (i.e., a possibly empty delimited
        // list of <NDATE> surrounded by [ and ]) and place into the
        // specified 'result' the corresponding value.  Store in the specified
        // '*endPos' the address of the non-modifiable character in
        // 'inputString' immediately following the successfully parsed text,
        // or the position at which the parse failure was detected.  Return
        // zero on success, and a non-zero value otherwise.  The value of
        // '*result' is undefined if a parse failure occurs.  The behavior
        // is undefined if any argument is 0.
        //..
        // A parse failure can occur for the following reasons:
        //   1. The opening or closing square bracket is missing.
        //   2. Parsing of a particular array element fails.
        //..

    static int parseTimeArray(const char             **endPos,
                              bsl::vector<bdet_Time>  *result,
                              const char              *inputString);
        // Parse the specified 'inputString' for a sequence of characters
        // matching the regular expression <WHITESPACE>* [ <WHITESPACE>*
        // (<NTIME> <WHITESPACE>*)* ] (i.e., a possibly empty list of
        // <NTIME> surrounded by [ and ]) and place into the specified
        // 'result' the corresponding value.  Store in the specified
        // '*endPos' the address of the non-modifiable character in
        // 'inputString' immediately following the successfully parsed text,
        // or the position at which the parse failure was detected.  Return
        // zero on success, and a non-zero value otherwise.  The value of
        // '*result' is undefined if a parse failure occurs.  The behavior
        // is undefined if any argument is 0.
        //..
        // A parse failure can occur for the following reasons:
        //   1. The opening or closing square bracket is missing.
        //   2. Parsing of a particular array element fails.
        //..

    static int parseDatetimeArray(const char                 **endPos,
                                  bsl::vector<bdet_Datetime>  *result,
                                  const char                  *inputString);
        // Parse the specified 'inputString' for a sequence of characters
        // matching the regular expression <WHITESPACE>* [ <WHITESPACE>*
        // (<NDATETIME> <WHITESPACE>*)* <WHITESPACE>* ] (i.e., a possibly
        // empty list of <NDATETIME> surrounded by [ and ]) and place into
        // the specified 'result' the corresponding value.  Store
        // in the specified '*endPos' the address of the non-modifiable
        // character in 'inputString' immediately following the successfully
        // parsed text, or the position at which the parse failure was
        // detected.  Return zero on success, and a non-zero value otherwise.
        // The value of '*result' is undefined if a parse failure occurs.
        // The behavior is undefined if any argument is 0.
        //..
        // A parse failure can occur for the following reasons:
        //   1. The opening or closing square bracket is missing.
        //   2. Parsing of a particular array element fails.
        //..

    static int parseDateTzArray(const char               **endPos,
                                bsl::vector<bdet_DateTz>  *result,
                                const char                *inputString);
        // Parse the specified 'inputString' for a sequence of characters
        // matching the regular expression <WHITESPACE>* [ <WHITESPACE>*
        // (<NDATETZ> <WHITESPACE>*)* ] (i.e., a possibly empty delimited
        // list of <NDATETZ> surrounded by [ and ]) and place into the
        // specified 'result' the corresponding value.  Store in the specified
        // '*endPos' the address of the non-modifiable character in
        // 'inputString' immediately following the successfully parsed text,
        // or the position at which the parse failure was detected.  Return
        // zero on success, and a non-zero value otherwise.  The value of
        // '*result' is undefined if a parse failure occurs.  The behavior
        // is undefined if any argument is 0.
        //..
        // A parse failure can occur for the following reasons:
        //   1. The opening or closing square bracket is missing.
        //   2. Parsing of a particular array element fails.
        //..

    static int parseTimeTzArray(const char               **endPos,
                                bsl::vector<bdet_TimeTz>  *result,
                                const char                *inputString);
        // Parse the specified 'inputString' for a sequence of characters
        // matching the regular expression <WHITESPACE>* [ <WHITESPACE>*
        // (<NTIMETZ> <WHITESPACE>*)* ] (i.e., a possibly empty list of
        // <NTIMETZ> surrounded by [ and ]) and place into the specified
        // 'result' the corresponding value.  Store in the specified
        // '*endPos' the address of the non-modifiable character in
        // 'inputString' immediately following the successfully parsed text,
        // or the position at which the parse failure was detected.  Return
        // zero on success, and a non-zero value otherwise.  The value of
        // '*result' is undefined if a parse failure occurs.  The behavior
        // is undefined if any argument is 0.
        //..
        // A parse failure can occur for the following reasons:
        //   1. The opening or closing square bracket is missing.
        //   2. Parsing of a particular array element fails.
        //..

    static int parseDatetimeTzArray(
                                   const char                   **endPos,
                                   bsl::vector<bdet_DatetimeTz>  *result,
                                   const char                    *inputString);
        // Parse the specified 'inputString' for a sequence of characters
        // matching the regular expression <WHITESPACE>* [ <WHITESPACE>*
        // (<NDATETIMETZ> <WHITESPACE>*)* <WHITESPACE>* ] (i.e., a possibly
        // empty list of <NDATETIMETZ> surrounded by [ and ]) and place into
        // the specified 'result' the corresponding value.  Store
        // in the specified '*endPos' the address of the non-modifiable
        // character in 'inputString' immediately following the successfully
        // parsed text, or the position at which the parse failure was
        // detected.  Return zero on success, and a non-zero value otherwise.
        // The value of '*result' is undefined if a parse failure occurs.
        // The behavior is undefined if any argument is 0.
        //..
        // A parse failure can occur for the following reasons:
        //   1. The opening or closing square bracket is missing.
        //   2. Parsing of a particular array element fails.
        //..

                       // *** GENERATE FUNCTIONS ***

    static void generateCharRaw(bsl::vector<char> *buffer,
                                char               value,
                                int                level = 0,
                                int                spacesPerLevel = 4);
        // Format the specified 'value' using the optionally specified
        // indentation 'level' and append the result to the specified
        // 'buffer'.  If the specified 'value' matches the unset value defined
        // in 'bdetu_unset' it will be formatted as "NULL".  Otherwise, the
        // char is formatted such that it is always surrounded by single
        // quotes (').  Printable characters, as identified by the 'isprint'
        // library function, are generated as is.  Characters that are
        // identified as <SIMPLE_ESCAPE_SEQUENCE> are generated as '\\'
        // followed by the escape literal (i.e., '\a' is generated as '\\a').
        // Other characters are generated as a <HEX_ESCAPE_SEQUENCE> with
        // exactly two digits after the 'x' (i.e., character whose numeric
        // value is five will be generated as '\\x05').  If 'level' is
        // specified, optionally specify 'spacesPerLevel'.  The formatted
        // 'char' is indented by (level * spacesPerLevel) spaces.  (Making
        // 'level' negative suppresses indentation.)  The behavior is
        // undefined unless 0 != buffer and 0 <= spacesPerLevel.  Note that
        // 'value' is output as a textual representation that is parsable by
        // the corresponding 'parseChar' function.

    static void generateShortRaw(bsl::vector<char> *buffer,
                                 short              value,
                                 int                level = 0,
                                 int                spacesPerLevel = 4);
        // Format the specified 'value' using the optionally specified
        // indentation 'level' and append the result to the specified
        // 'buffer'.  If the specified 'value' matches the unset value defined
        // in 'bdetu_unset' it will be formatted as "NULL".  If 'level' is
        // specified, optionally specify 'spacesPerLevel'.  The formatted
        // short will be indented by (level * spacesPerLevel) spaces.  Making
        // 'level' negative suppresses indentation.  The behavior is undefined
        // if buffer == 0, or spacesPerLevel < 0.  Note that 'value' is output
        // as a textual representation that is parsable by the corresponding
        // 'parseShort' function.

    static void generateIntRaw(bsl::vector<char> *buffer,
                               int                value,
                               int                level = 0,
                               int                spacesPerLevel = 4);
        // Format the specified 'value' using the optionally specified
        // indentation 'level' and append the result to the specified
        // 'buffer'.  If the specified 'value' matches the unset value defined
        // in 'bdetu_unset' it will be formatted as "NULL".  If 'level' is
        // specified, optionally specify 'spacesPerLevel'.  The formatted int
        // will be indented by (level * spacesPerLevel) spaces.  Making
        // 'level' negative suppresses indentation.  The behavior is undefined
        // if buffer == 0, or spacesPerLevel < 0.  Note that 'value' is output
        // as a textual representation that is parsable by the corresponding
        // 'parseInt' function.

    static void generateInt64Raw(bsl::vector<char>        *buffer,
                                 bsls_PlatformUtil::Int64  value,
                                 int                       level = 0,
                                 int                       spacesPerLevel = 4);
        // Format the specified 'value' using the optionally specified
        // indentation 'level' and append the result to the specified
        // 'buffer'.  If the specified 'value' matches the unset value defined
        // in 'bdetu_unset' it will be formatted as "NULL".  If 'level' is
        // specified, optionally specify 'spacesPerLevel'.  The formatted
        // int64 will be indented by (level * spacesPerLevel) spaces.  Making
        // 'level' negative suppresses indentation.  The behavior is undefined
        // if buffer == 0, or spacesPerLevel < 0.  Note that 'value' is output
        // as a textual representation that is parsable by the corresponding
        // 'parseInt64' function.

    static void generateFloatRaw(bsl::vector<char> *buffer,
                                 float              value,
                                 int                level = 0,
                                 int                spacesPerLevel = 4);
        // Format the specified 'value' using the optionally specified
        // indentation 'level' and append the result to the specified
        // 'buffer'.  If the specified 'value' matches the unset value defined
        // in 'bdetu_unset' it will be formatted as "NULL".  If 'level' is
        // specified, optionally specify 'spacesPerLevel'.  The formatted
        // float will be indented by (level * spacesPerLevel) spaces.  Making
        // 'level' negative suppresses indentation.  The behavior is undefined
        // if buffer == 0, or spacesPerLevel < 0.  Note that 'value' is output
        // as a textual representation that is parsable by the corresponding
        // 'parseFloat' function.
        //
        // Warning: the generated value might be different from the specified
        // 'value', depending on the internal implementation of 'double' on
        // the given platform.

    static void generateDoubleRaw(bsl::vector<char> *buffer,
                                  double             value,
                                  int                level = 0,
                                  int                spacesPerLevel = 4);
        // Format the specified 'value' using the optionally specified
        // indentation 'level' and append the result to the specified
        // 'buffer'.  If the specified 'value' matches the unset value defined
        // in 'bdetu_unset' it will be formatted as "NULL".  If 'level' is
        // specified, optionally specify 'spacesPerLevel'.  The formatted
        // double will be indented by (level * spacesPerLevel) spaces.  Making
        // 'level' negative suppresses indentation.  The behavior is undefined
        // if buffer == 0, or spacesPerLevel < 0.  Note that 'value' is output
        // as a textual representation that is parsable by the corresponding
        // 'parseDouble' function.
        //
        // Warning: the generated value might be different from the specified
        // 'value', depending on the internal implementation of 'float' on the
        // given platform.

    static void generateStringRaw(bsl::vector<char> *buffer,
                                  const char        *value,
                                  int                level = 0,
                                  int                spacesPerLevel = 4);
        // Format the specified 'value' using the optionally specified
        // indentation 'level' and append the result to the specified
        // 'buffer'.  If the specified 'value' matches the unset value defined
        // in 'bdetu_unset' it will be formatted as "NULL".  Otherwise, the
        // string is formatted such that it is always surrounded by double
        // quotes ('"').  Printable characters, as identified by 'isprint'
        // function, are generated as is.  Characters that are identified as
        // <SIMPLE_ESCAPE_SEQUENCE> are generated as '\\' followed by the
        // escape literal.  (i.e., '\a' is generated as '\\a').  Other
        // characters are generated as a <HEX_ESCAPE_SEQUENCE> with two digits
        // after the 'x' (i.e., character whose numeric value is five will be
        // generated as '\\x05').  If 'level' is specified, optionally specify
        // 'spacesPerLevel'.  The formatted string will be indented by (level
        // * spacesPerLevel) spaces.  Making 'level' negative suppresses
        // indentation.  The behavior is undefined if buffer == 0, value == 0,
        // or spacesPerLevel < 0.  Note that 'value' is output as a textual
        // representation that is parsable by the corresponding 'parseString'
        // function.

    static void generateDateRaw(bsl::vector<char> *buffer,
                                const bdet_Date&   value,
                                int                level = 0,
                                int                spacesPerLevel = 4);
        // Format the specified 'value' using the optionally specified
        // indentation 'level' and append the result to the specified
        // 'buffer'.  If the specified 'value' matches the unset value defined
        // in 'bdetu_unset' it will be formatted as "NULL".  Otherwise, the
        // 'bdet_Date' is formatted as follows: four digits to represent the
        // year followed by a slash followed by two digits to represent the
        // month followed by a slash followed by to two digits to represent
        // the day.  All values will have leading zeros to reach the
        // proscribed number of digits.  If 'level' is specified, optionally
        // specify  'spacesPerLevel'.  The formatted 'bdet_Date' will be
        // indented by (level * spacesPerLevel) spaces.  Making 'level'
        // negative suppresses indentation.  The behavior is undefined if
        // 'buffer == 0', '&value == 0', or 'spacesPerLevel < 0'.  Note that
        // 'value' is output as a textual representation that is parsable by
        // the corresponding 'parseDate' function.

    static void generateDateTzRaw(bsl::vector<char>  *buffer,
                                  const bdet_DateTz&  value,
                                  int                 level = 0,
                                  int                 spacesPerLevel = 4);
        // Format the specified 'value' using the optionally specified
        // indentation 'level' and append the result to the specified
        // 'buffer'.  If the specified 'value' matches the unset value defined
        // in 'bdetu_unset' it will be formatted as "NULL".  Otherwise, the
        // 'bdet_DateTz' is formatted as follows: four digits to represent the
        // year followed by a slash followed by two digits to represent the
        // month followed by a slash followed by to two digits to represent
        // the day.  All values will have leading zeros to reach the
        // proscribed number of digits.  If 'level' is specified, optionally
        // specify  'spacesPerLevel'.  The formatted 'bdet_Date' will be
        // indented by (level * spacesPerLevel) spaces.  Making 'level'
        // negative suppresses indentation.  The behavior is undefined if
        // 'buffer == 0', '&value == 0', or 'spacesPerLevel < 0'.  Note that
        // 'value' is output as a textual representation that is parsable by
        // the corresponding 'parseDate' function.

    static void generateTimeRaw(bsl::vector<char> *buffer,
                                const bdet_Time&   value,
                                int                level = 0,
                                int                spacesPerLevel = 4);
        // Format the specified 'value' using the optionally specified
        // indentation 'level' and append the result to the specified
        // 'buffer'.  If the specified 'value' matches the unset value defined
        // in 'bdetu_unset' it will be formatted as "NULL".  Otherwise, the
        // 'bdet_Time' is formatted as follows: two digits to represent hours
        // followed by a colon followed by two digits to represent minutes
        // followed by a colon followed by two digits to represent seconds
        // followed by a colon followed by three digits to represent
        // milliseconds.  All values will have leading zeros to reach the
        // proscribed number of digits.  If 'level' is specified, optionally
        // specify 'spacesPerLevel'.  The formatted 'bdet_Time' will be
        // indented by (level * spacesPerLevel) spaces.  Making 'level'
        // negative suppresses indentation.  The behavior is undefined if
        // 'buffer == 0', '&value == 0', or 'spacesPerLevel < 0'.  Note that
        // 'value' is output as a textual representation that is parsable by
        // the corresponding 'parseTime' function.

    static void generateTimeTzRaw(bsl::vector<char>  *buffer,
                                  const bdet_TimeTz&  value,
                                  int                 level = 0,
                                  int                 spacesPerLevel = 4);
        // Format the specified 'value' using the optionally specified
        // indentation 'level' and append the result to the specified
        // 'buffer'.  If the specified 'value' matches the unset value defined
        // in 'bdetu_unset' it will be formatted as "NULL".  Otherwise, the
        // 'bdet_TimeTz' is formatted as follows: two digits to represent hours
        // followed by a colon followed by two digits to represent minutes
        // followed by a colon followed by two digits to represent seconds
        // followed by a colon followed by three digits to represent
        // milliseconds.  All values will have leading zeros to reach the
        // proscribed number of digits.  If 'level' is specified, optionally
        // specify 'spacesPerLevel'.  The formatted 'bdet_Time' will be
        // indented by (level * spacesPerLevel) spaces.  Making 'level'
        // negative suppresses indentation.  The behavior is undefined if
        // 'buffer == 0', '&value == 0', or 'spacesPerLevel < 0'.  Note that
        // 'value' is output as a textual representation that is parsable by
        // the corresponding 'parseTime' function.

    static void generateDatetimeRaw(bsl::vector<char>    *buffer,
                                    const bdet_Datetime&  value,
                                    int                   level = 0,
                                    int                   spacesPerLevel = 4);
        // Format the specified 'value' using the optionally specified
        // indentation 'level' and append the result to the specified
        // 'buffer'.  If the specified 'value' matches the unset value defined
        // in 'bdetu_unset' it will be formatted as "NULL".  Otherwise, the
        // 'bdet_Datetime' is formatted as a <DATE> (see the above function)
        // followed by a single space followed by a <TIME> (see the above
        // function).  If 'level' is specified, optionally specify
        // 'spacesPerLevel'.  The formatted 'bdet_Datetime will be indented by
        // (level * spacesPerLevel) spaces.  Making 'level' negative
        // suppresses indentation.  The behavior is undefined if 'buffer == 0',
        // '&value == 0', or 'spacesPerLevel < 0'.  Note that 'value' is
        // output as a textual representation that is parsable by the
        // corresponding 'parseTime' function.

    static void generateDatetimeTzRaw(
                                   bsl::vector<char>      *buffer,
                                   const bdet_DatetimeTz&  value,
                                   int                     level = 0,
                                   int                     spacesPerLevel = 4);
        // Format the specified 'value' using the optionally specified
        // indentation 'level' and append the result to the specified
        // 'buffer'.  If the specified 'value' matches the unset value defined
        // in 'bdetu_unset' it will be formatted as "NULL".  Otherwise, the
        // 'bdet_DatetimeTz' is formatted as a <DATE> (see the above function)
        // followed by a single space followed by a <TIME> (see the above
        // function).  If 'level' is specified, optionally specify
        // 'spacesPerLevel'.  The formatted 'bdet_Datetime will be indented by
        // (level * spacesPerLevel) spaces.  Making 'level' negative
        // suppresses indentation.  The behavior is undefined if 'buffer == 0',
        // '&value == 0', or 'spacesPerLevel < 0'.  Note that 'value' is output
        // as a textual representation that is parsable by the corresponding
        // 'parseTime' function.

    static void generateCharArrayRaw(
                                 bsl::vector<char>        *buffer,
                                 const bsl::vector<char>&  value,
                                 int                       level = 0,
                                 int                       spacesPerLevel = 4);
        // Format the specified 'value' using the optionally specified
        // indentation 'level' and append the result to the specified
        // 'buffer'.  The 'buffer' is formatted as a comma delimited list of
        // elements surrounded by '[' and ']'.  If the element matches the
        // unset value for the element type defined in 'bdetu_unset' it will be
        // formatted as "NULL".  Otherwise, it will be formatted as <CHAR>.
        //
        // If 'level' is specified, optionally specify 'spacesPerLevel'.  When
        // 'level' is positive, the indentation of both opening and closing
        // brackets is specified by (level * spacesPerLevel) spaces.  When
        // 'level' is negative, the opening bracket indentation is suppressed.
        // Each array element is generated on the new line and idented by
        // ((level + 1) * spacesPerLevel) spaces.  The behavior is undefined
        // if '0 == buffer', '0 == &value', or '0 > spacesPerLevel'.  Note that
        // 'value' is output as a textual representation that is parsable by
        // the corresponding 'parseArray' function.

    static void generateShortArrayRaw(
                                bsl::vector<char>         *buffer,
                                const bsl::vector<short>&  value,
                                int                        level = 0,
                                int                        spacesPerLevel = 4);
        // Format the specified 'value' using the optionally specified
        // indentation 'level' and append the result to the specified
        // 'buffer'.  The 'buffer' is formatted as a comma delimited list of
        // elements surrounded by '[' and ']'.  If the element matches the
        // unset value for the element type defined in 'bdetu_unset' it will be
        // formatted as "NULL".  Otherwise, it will be formatted as <CHAR>.
        //
        // If 'level' is specified, optionally specify 'spacesPerLevel'.  When
        // 'level' is positive, the indentation of both opening and closing
        // brackets is specified by (level * spacesPerLevel) spaces.  When
        // 'level' is negative, the opening bracket indentation is suppressed.
        // Each array element is generated on the new line and idented by
        // ((level + 1) * spacesPerLevel) spaces.  The behavior is undefined
        // if '0 == buffer', '0 == &value', or '0 > spacesPerLevel'.  Note that
        // 'value' is output as a textual representation that is parsable by
        // the corresponding 'parseArray' function.

    static void generateIntArrayRaw(
                                  bsl::vector<char>       *buffer,
                                  const bsl::vector<int>&  value,
                                  int                      level = 0,
                                  int                      spacesPerLevel = 4);
        // Format the specified 'value' using the optionally specified
        // indentation 'level' and append the result to the specified
        // 'buffer'.  The 'buffer' is formatted as a comma delimited list of
        // elements surrounded by '[' and ']'.  If the element matches the
        // unset value for the element type defined in 'bdetu_unset' it will be
        // formatted as "NULL".  Otherwise, it will be formatted as <CHAR>.
        //
        // If 'level' is specified, optionally specify 'spacesPerLevel'.  When
        // 'level' is positive, the indentation of both opening and closing
        // brackets is specified by (level * spacesPerLevel) spaces.  When
        // 'level' is negative, the opening bracket indentation is suppressed.
        // Each array element is generated on the new line and idented by
        // ((level + 1) * spacesPerLevel) spaces.  The behavior is undefined
        // if '0 == buffer', '0 == &value', or '0 > spacesPerLevel'.  Note that
        // 'value' is output as a textual representation that is parsable by
        // the corresponding 'parseArray' function.

    static void generateInt64ArrayRaw(
             bsl::vector<char>                            *buffer,
             const bsl::vector<bsls_PlatformUtil::Int64>&  value,
             int                                           level = 0,
             int                                           spacesPerLevel = 4);
        // Format the specified 'value' using the optionally specified
        // indentation 'level' and append the result to the specified
        // 'buffer'.  The 'buffer' is formatted as a comma delimited list of
        // elements surrounded by '[' and ']'.  If the element matches the
        // unset value for the element type defined in 'bdetu_unset' it will be
        // formatted as "NULL".  Otherwise, it will be formatted as <CHAR>.
        //
        // If 'level' is specified, optionally specify 'spacesPerLevel'.  When
        // 'level' is positive, the indentation of both opening and closing
        // brackets is specified by (level * spacesPerLevel) spaces.  When
        // 'level' is negative, the opening bracket indentation is suppressed.
        // Each array element is generated on the new line and idented by
        // ((level + 1) * spacesPerLevel) spaces.  The behavior is undefined
        // if '0 == buffer', '0 == &value', or '0 > spacesPerLevel'.  Note that
        // 'value' is output as a textual representation that is parsable by
        // the corresponding 'parseArray' function.

    static void generateFloatArrayRaw(
                                bsl::vector<char>         *buffer,
                                const bsl::vector<float>&  value,
                                int                        level = 0,
                                int                        spacesPerLevel = 4);
        // Format the specified 'value' using the optionally specified
        // indentation 'level' and append the result to the specified
        // 'buffer'.  The 'buffer' is formatted as a comma delimited list of
        // elements surrounded by '[' and ']'.  If the element matches the
        // unset value for the element type defined in 'bdetu_unset' it will be
        // formatted as "NULL".  Otherwise, it will be formatted as <CHAR>.
        //
        // If 'level' is specified, optionally specify 'spacesPerLevel'.  When
        // 'level' is positive, the indentation of both opening and closing
        // brackets is specified by (level * spacesPerLevel) spaces.  When
        // 'level' is negative, the opening bracket indentation is suppressed.
        // Each array element is generated on the new line and idented by
        // ((level + 1) * spacesPerLevel) spaces.  The behavior is undefined
        // if '0 == buffer', '0 == &value', or '0 > spacesPerLevel'.  Note that
        // 'value' is output as a textual representation that is parsable by
        // the corresponding 'parseArray' function.

    static void generateDoubleArrayRaw(
                               bsl::vector<char>          *buffer,
                               const bsl::vector<double>&  value,
                               int                         level = 0,
                               int                         spacesPerLevel = 4);
        // Format the specified 'value' using the optionally specified
        // indentation 'level' and append the result to the specified
        // 'buffer'.  The 'buffer' is formatted as a comma delimited list of
        // elements surrounded by '[' and ']'.  If the element matches the
        // unset value for the element type defined in 'bdetu_unset' it will be
        // formatted as "NULL".  Otherwise, it will be formatted as <CHAR>.
        //
        // If 'level' is specified, optionally specify 'spacesPerLevel'.  When
        // 'level' is positive, the indentation of both opening and closing
        // brackets is specified by (level * spacesPerLevel) spaces.  When
        // 'level' is negative, the opening bracket indentation is suppressed.
        // Each array element is generated on the new line and idented by
        // ((level + 1) * spacesPerLevel) spaces.  The behavior is undefined
        // if '0 == buffer', '0 == &value', or '0 > spacesPerLevel'.  Note that
        // 'value' is output as a textual representation that is parsable by
        // the corresponding 'parseArray' function.

    static void generateStringArrayRaw(
                          bsl::vector<char>               *buffer,
                          const bsl::vector<bsl::string>&  value,
                          int                              level = 0,
                          int                              spacesPerLevel = 4);
        // Format the specified 'value' using the optionally specified
        // indentation 'level' and append the result to the specified
        // 'buffer'.  The 'buffer' is formatted as a comma delimited list of
        // elements surrounded by '[' and ']'.  If the element matches the
        // unset value for the element type defined in 'bdetu_unset' it will be
        // formatted as "NULL".  Otherwise, it will be formatted as <CHAR>.
        //
        // If 'level' is specified, optionally specify 'spacesPerLevel'.  When
        // 'level' is positive, the indentation of both opening and closing
        // brackets is specified by (level * spacesPerLevel) spaces.  When
        // 'level' is negative, the opening bracket indentation is suppressed.
        // Each array element is generated on the new line and idented by
        // ((level + 1) * spacesPerLevel) spaces.  The behavior is undefined
        // if '0 == buffer', '0 == &value', or '0 > spacesPerLevel'.  Note that
        // 'value' is output as a textual representation that is parsable by
        // the corresponding 'parseArray' function.

    static void generateDateArrayRaw(
                            bsl::vector<char>             *buffer,
                            const bsl::vector<bdet_Date>&  value,
                            int                            level = 0,
                            int                            spacesPerLevel = 4);
        // Format the specified 'value' using the optionally specified
        // indentation 'level' and append the result to the specified
        // 'buffer'.  The 'buffer' is formatted as a comma delimited list of
        // elements surrounded by '[' and ']'.  If the element matches the
        // unset value for the element type defined in 'bdetu_unset' it will be
        // formatted as "NULL".  Otherwise, it will be formatted as <CHAR>.
        //
        // If 'level' is specified, optionally specify 'spacesPerLevel'.  When
        // 'level' is positive, the indentation of both opening and closing
        // brackets is specified by (level * spacesPerLevel) spaces.  When
        // 'level' is negative, the opening bracket indentation is suppressed.
        // Each array element is generated on the new line and idented by
        // ((level + 1) * spacesPerLevel) spaces.  The behavior is undefined
        // if '0 == buffer', '0 == &value', or '0 > spacesPerLevel'.  Note that
        // 'value' is output as a textual representation that is parsable by
        // the corresponding 'parseArray' function.

    static void generateDateTzArrayRaw(
                          bsl::vector<char>               *buffer,
                          const bsl::vector<bdet_DateTz>&  value,
                          int                              level = 0,
                          int                              spacesPerLevel = 4);
        // Format the specified 'value' using the optionally specified
        // indentation 'level' and append the result to the specified
        // 'buffer'.  The 'buffer' is formatted as a comma delimited list of
        // elements surrounded by '[' and ']'.  If the element matches the
        // unset value for the element type defined in 'bdetu_unset' it will be
        // formatted as "NULL".  Otherwise, it will be formatted as <CHAR>.
        //
        // If 'level' is specified, optionally specify 'spacesPerLevel'.  When
        // 'level' is positive, the indentation of both opening and closing
        // brackets is specified by (level * spacesPerLevel) spaces.  When
        // 'level' is negative, the opening bracket indentation is suppressed.
        // Each array element is generated on the new line and idented by
        // ((level + 1) * spacesPerLevel) spaces.  The behavior is undefined
        // if '0 == buffer', '0 == &value', or '0 > spacesPerLevel'.  Note that
        // 'value' is output as a textual representation that is parsable by
        // the corresponding 'parseArray' function.

    static void generateTimeArrayRaw(
                            bsl::vector<char>             *buffer,
                            const bsl::vector<bdet_Time>&  value,
                            int                            level = 0,
                            int                            spacesPerLevel = 4);
        // Format the specified 'value' using the optionally specified
        // indentation 'level' and append the result to the specified
        // 'buffer'.  The 'buffer' is formatted as a comma delimited list of
        // elements surrounded by '[' and ']'.  If the element matches the
        // unset value for the element type defined in 'bdetu_unset' it will be
        // formatted as "NULL".  Otherwise, it will be formatted as <CHAR>.
        //
        // If 'level' is specified, optionally specify 'spacesPerLevel'.  When
        // 'level' is positive, the indentation of both opening and closing
        // brackets is specified by (level * spacesPerLevel) spaces.  When
        // 'level' is negative, the opening bracket indentation is suppressed.
        // Each array element is generated on the new line and idented by
        // ((level + 1) * spacesPerLevel) spaces.  The behavior is undefined
        // or '0 == buffer', '0 == &value', or '0 > spacesPerLevel'.  Note that
        // 'value' is output as a textual representation that is parsable by
        // the corresponding 'parseArray' function.

    static void generateTimeTzArrayRaw(
                          bsl::vector<char>               *buffer,
                          const bsl::vector<bdet_TimeTz>&  value,
                          int                              level = 0,
                          int                              spacesPerLevel = 4);
        // Format the specified 'value' using the optionally specified
        // indentation 'level' and append the result to the specified
        // 'buffer'.  The 'buffer' is formatted as a comma delimited list of
        // elements surrounded by '[' and ']'.  If the element matches the
        // unset value for the element type defined in 'bdetu_unset' it will be
        // formatted as "NULL".  Otherwise, it will be formatted as <CHAR>.
        //
        // If 'level' is specified, optionally specify 'spacesPerLevel'.  When
        // 'level' is positive, the indentation of both opening and closing
        // brackets is specified by (level * spacesPerLevel) spaces.  When
        // 'level' is negative, the opening bracket indentation is suppressed.
        // Each array element is generated on the new line and idented by
        // ((level + 1) * spacesPerLevel) spaces.  The behavior is undefined
        // if '0 == buffer', '0 == &value', or '0 > spacesPerLevel'.  Note that
        // 'value' is output as a textual representation that is parsable by
        // the corresponding 'parseArray' function.

    static void generateDatetimeArrayRaw(
                        bsl::vector<char>                 *buffer,
                        const bsl::vector<bdet_Datetime>&  value,
                        int                                level = 0,
                        int                                spacesPerLevel = 4);
        // Format the specified 'value' using the optionally specified
        // indentation 'level' and append the result to the specified
        // 'buffer'.  The 'buffer' is formatted as a comma delimited list of
        // elements surrounded by '[' and ']'.  If the element matches the
        // unset value for the element type defined in 'bdetu_unset' it will be
        // formatted as "NULL".  Otherwise, it will be formatted as <CHAR>.
        //
        // If 'level' is specified, optionally specify 'spacesPerLevel'.  When
        // 'level' is positive, the indentation of both opening and closing
        // brackets is specified by (level * spacesPerLevel) spaces.  When
        // 'level' is negative, the opening bracket indentation is suppressed.
        // Each array element is generated on the new line and idented by
        // ((level + 1) * spacesPerLevel) spaces.  The behavior is undefined
        // if '0 == buffer', '0 == &value', or '0 > spacesPerLevel'.  Note that
        // 'value' is output as a textual representation that is parsable by
        // the corresponding 'parseArray' function.

    static void generateDatetimeTzArrayRaw(
                      bsl::vector<char>                   *buffer,
                      const bsl::vector<bdet_DatetimeTz>&  value,
                      int                                  level = 0,
                      int                                  spacesPerLevel = 4);
        // Format the specified 'value' using the optionally specified
        // indentation 'level' and append the result to the specified
        // 'buffer'.  The 'buffer' is formatted as a comma delimited list of
        // elements surrounded by '[' and ']'.  If the element matches the
        // unset value for the element type defined in 'bdetu_unset' it will be
        // formatted as "NULL".  Otherwise, it will be formatted as <CHAR>.
        //
        // If 'level' is specified, optionally specify 'spacesPerLevel'.  When
        // 'level' is positive, the indentation of both opening and closing
        // brackets is specified by (level * spacesPerLevel) spaces.  When
        // 'level' is negative, the opening bracket indentation is suppressed.
        // Each array element is generated on the new line and idented by
        // ((level + 1) * spacesPerLevel) spaces.  The behavior is undefined
        // if '0 == buffer', '0 == &value', or '0 > spacesPerLevel'.  Note that
        // 'value' is output as a textual representation that is parsable by
        // the corresponding 'parseArray' function.

    static void generateChar(bsl::vector<char> *buffer,
                             char               value,
                             int                level = 0,
                             int                spacesPerLevel = 4);
        // Format the specified 'value' using the optionally specified
        // indentation 'level' and append the result to the specified 'buffer'
        // while maintaining null-character termination.  If the specified
        // 'value' matches the unset value defined in 'bdetu_unset' it will be
        // formatted as "NULL".  Otherwise, the char is formatted such that it
        // is always surrounded by single quotes (').  Printable characters,
        // as identified by the 'isprint' library function, are generated as
        // is.  Characters that are identified as <SIMPLE_ESCAPE_SEQUENCE> are
        // generated as '\\' followed by the escape literal (i.e., '\a' is
        // generated as '\\a').  Other characters are generated as a
        // <HEX_ESCAPE_SEQUENCE> with exactly two digits after the 'x' (i.e.,
        // character whose numeric value is five will be generated as
        // '\\x05').  If 'level' is specified, optionally specify
        // 'spacesPerLevel'.  The formatted 'char' is indented by (level *
        // spacesPerLevel) spaces.  (Making 'level' negative suppresses
        // indentation.)  The behavior is undefined unless 0 != buffer and 0
        // <= spacesPerLevel.  Note that 'value' is output as a textual
        // representation that is parsable by the corresponding 'parseChar'
        // function.

    static void generateShort(bsl::vector<char> *buffer,
                              short              value,
                              int                level = 0,
                              int                spacesPerLevel = 4);
        // Format the specified 'value' using the optionally specified
        // indentation 'level' and append the result to the specified 'buffer'
        // while maintaining null-character termination.  If the specified
        // 'value' matches the unset value defined in 'bdetu_unset' it will be
        // formatted as "NULL".  If 'level' is specified, optionally specify
        // 'spacesPerLevel'.  The formatted short will be indented by (level *
        // spacesPerLevel) spaces.  Making 'level' negative suppresses
        // indentation.  The behavior is undefined if buffer == 0, or
        // spacesPerLevel < 0.  Note that 'value' is output as a textual
        // representation that is parsable by the corresponding 'parseShort'
        // function.

    static void generateInt(bsl::vector<char> *buffer,
                            int                value,
                            int                level = 0,
                            int                spacesPerLevel = 4);
        // Format the specified 'value' using the optionally specified
        // indentation 'level' and append the result to the specified 'buffer'
        // while maintaining null-character termination.  If the specified
        // 'value' matches the unset value defined in 'bdetu_unset' it will be
        // formatted as "NULL".  If 'level' is specified, optionally specify
        // 'spacesPerLevel'.  The formatted int will be indented by
        // (level * spacesPerLevel) spaces.  Making 'level' negative suppresses
        // indentation.  The behavior is undefined if buffer == 0, or
        // spacesPerLevel < 0.  Note that 'value' is output as a textual
        // representation that is parsable by the corresponding 'parseInt'
        // function.

    static void generateInt64(bsl::vector<char>        *buffer,
                              bsls_PlatformUtil::Int64  value,
                              int                       level = 0,
                              int                       spacesPerLevel = 4);
        // Format the specified 'value' using the optionally specified
        // indentation 'level' and append the result to the specified 'buffer'
        // while maintaining null-character termination.  If the specified
        // 'value' matches the unset value defined in 'bdetu_unset' it will be
        // formatted as "NULL".  If 'level' is specified, optionally specify
        // 'spacesPerLevel'.  The formatted int64 will be indented by
        // (level * spacesPerLevel) spaces.  Making 'level' negative suppresses
        // indentation.  The behavior is undefined if buffer == 0, or
        // spacesPerLevel < 0.  Note that 'value' is output as a textual
        // representation that is parsable by the corresponding 'parseInt64'
        // function.

    static void generateFloat(bsl::vector<char> *buffer,
                              float              value,
                              int                level = 0,
                              int                spacesPerLevel = 4);
        // Format the specified 'value' using the optionally specified
        // indentation 'level' and append the result to the specified 'buffer'
        // while maintaining null-character termination.  If the specified
        // 'value' matches the unset value defined in 'bdetu_unset' it will be
        // formatted as "NULL".  If 'level' is specified, optionally specify
        // 'spacesPerLevel'.  The formatted float will be indented by
        // (level * spacesPerLevel) spaces.  Making 'level' negative suppresses
        // indentation.  The behavior is undefined if buffer == 0, or
        // spacesPerLevel < 0.  Note that 'value' is output as a textual
        // representation that is parsable by the corresponding 'parseFloat'
        // function.
        //
        // Warning: the generated value might be different from the specified
        // 'value', depending on the internal implementation of 'double' on
        // the given platform.

    static void generateDouble(bsl::vector<char> *buffer,
                               double             value,
                               int                level = 0,
                               int                spacesPerLevel = 4);
        // Format the specified 'value' using the optionally specified
        // indentation 'level' and append the result to the specified 'buffer'
        // while maintaining null-character termination.  If the specified
        // 'value' matches the unset value defined in 'bdetu_unset' it will be
        // formatted as "NULL".  If 'level' is specified, optionally specify
        // 'spacesPerLevel'.  The formatted double will be indented by
        // (level * spacesPerLevel) spaces.  Making 'level' negative suppresses
        // indentation.  The behavior is undefined if buffer == 0, or
        // spacesPerLevel < 0.  Note that 'value' is output as a textual
        // representation that is parsable by the corresponding 'parseDouble'
        // function.
        //
        // Warning: the generated value might be different from the specified
        // 'value', depending on the internal implementation of 'float' on the
        // given platform.

    static void generateString(bsl::vector<char> *buffer,
                               const char        *value,
                               int                level = 0,
                               int                spacesPerLevel = 4);
        // Format the specified 'value' using the optionally specified
        // indentation 'level' and append the result to the specified 'buffer'
        // while maintaining null-character termination.  If the specified
        // 'value' matches the unset value defined in 'bdetu_unset' it will be
        // formatted as "NULL".  Otherwise, the string is formatted such that
        // it is always surrounded by double quotes ('"').  Printable
        // characters, as identified by 'isprint' function, are generated as
        // is.  Characters that are identified as <SIMPLE_ESCAPE_SEQUENCE> are
        // generated as '\\' followed by the escape literal.  (i.e., '\a' is
        // generated as '\\a').  Other characters are generated as a
        // <HEX_ESCAPE_SEQUENCE> with two digits after the 'x' (i.e.,
        // character whose numeric value is five will be generated as
        // '\\x05').  If 'level' is specified, optionally specify
        // 'spacesPerLevel'.  The formatted string will be indented by (level
        // * spacesPerLevel) spaces.  Making 'level' negative suppresses
        // indentation.  The behavior is undefined if buffer == 0, value == 0,
        // or spacesPerLevel < 0.  Note that 'value' is output as a textual
        // representation that is parsable by the corresponding 'parseString'
        // function.

    static void generateDate(bsl::vector<char> *buffer,
                             const bdet_Date&   value,
                             int                level = 0,
                             int                spacesPerLevel = 4);
        // Format the specified 'value' using the optionally specified
        // indentation 'level' and append the result to the specified 'buffer'
        // while maintaining null-character termination.  If the specified
        // 'value' matches the unset value defined in 'bdetu_unset' it will be
        // formatted as "NULL".  Otherwise, the 'bdet_Date' is formatted as
        // follows: four digits to represent the year followed by a slash
        // followed by two digits to represent the month followed by a slash
        // followed by to two digits to represent the day.  All values will
        // have leading zeros to reach the proscribed number of digits.  If
        // 'level' is specified, optionally specify  'spacesPerLevel'.  The
        // formatted 'bdet_Date' will be indented by (level * spacesPerLevel)
        // spaces.  Making 'level' negative suppresses indentation.  The
        // behavior is undefined if buffer == 0, value == 0, or spacesPerLevel
        // < 0.  Note that 'value' is output as a textual representation that
        // is parsable by the corresponding 'parseDate' function.

    static void generateDateTz(bsl::vector<char>  *buffer,
                               const bdet_DateTz&  value,
                               int                 level = 0,
                               int                 spacesPerLevel = 4);
        // Format the specified 'value' using the optionally specified
        // indentation 'level' and append the result to the specified 'buffer'
        // while maintaining null-character termination.  If the specified
        // 'value' matches the unset value defined in 'bdetu_unset' it will be
        // formatted as "NULL".  Otherwise, the 'bdet_DateTz' is formatted as
        // follows: four digits to represent the year followed by a slash
        // followed by two digits to represent the month followed by a slash
        // followed by to two digits to represent the day.  All values will
        // have leading zeros to reach the proscribed number of digits.  If
        // 'level' is specified, optionally specify  'spacesPerLevel'.  The
        // formatted 'bdet_Date' will be indented by (level * spacesPerLevel)
        // spaces.  Making 'level' negative suppresses indentation.  The
        // behavior is undefined if buffer == 0, value == 0, or spacesPerLevel
        // < 0.  Note that 'value' is output as a textual representation that
        // is parsable by the corresponding 'parseDate' function.

    static void generateTime(bsl::vector<char> *buffer,
                             const bdet_Time&   value,
                             int                level = 0,
                             int                spacesPerLevel = 4);
        // Format the specified 'value' using the optionally specified
        // indentation 'level' and append the result to the specified 'buffer'
        // while maintaining null-character termination.  If the specified
        // 'value' matches the unset value defined in 'bdetu_unset' it will be
        // formatted as "NULL".  Otherwise, the 'bdet_Time' is formatted as
        // follows: two digits to represent hours followed by a colon followed
        // by two digits to represent minutes followed by a colon followed by
        // two digits to represent seconds followed by a colon followed by
        // three digits to represent milliseconds.  All values will have
        // leading zeros to reach the proscribed number of digits.  If 'level'
        // is specified, optionally specify 'spacesPerLevel'.  The formatted
        // 'bdet_Time' will be indented by (level * spacesPerLevel) spaces.
        // Making 'level' negative suppresses indentation.  The behavior is
        // undefined if buffer == 0, value == 0, or spacesPerLevel < 0.  Note
        // that 'value' is output as a textual representation that is parsable
        // by the corresponding 'parseTime' function.

    static void generateTimeTz(bsl::vector<char>  *buffer,
                               const bdet_TimeTz&  value,
                               int                 level = 0,
                               int                 spacesPerLevel = 4);
        // Format the specified 'value' using the optionally specified
        // indentation 'level' and append the result to the specified 'buffer'
        // while maintaining null-character termination.  If the specified
        // 'value' matches the unset value defined in 'bdetu_unset' it will be
        // formatted as "NULL".  Otherwise, the 'bdet_TimeTz' is formatted as
        // follows: two digits to represent hours followed by a colon followed
        // by two digits to represent minutes followed by a colon followed by
        // two digits to represent seconds followed by a colon followed by
        // three digits to represent milliseconds.  All values will have
        // leading zeros to reach the proscribed number of digits.  If 'level'
        // is specified, optionally specify 'spacesPerLevel'.  The formatted
        // 'bdet_Time' will be indented by (level * spacesPerLevel) spaces.
        // Making 'level' negative suppresses indentation.  The behavior is
        // undefined if buffer == 0, value == 0, or spacesPerLevel < 0.  Note
        // that 'value' is output as a textual representation that is parsable
        // by the corresponding 'parseTime' function.

    static void generateDatetime(bsl::vector<char>    *buffer,
                                 const bdet_Datetime&  value,
                                 int                   level = 0,
                                 int                   spacesPerLevel = 4);
        // Format the specified 'value' using the optionally specified
        // indentation 'level' and append the result to the specified 'buffer'
        // while maintaining null-character termination.  If the specified
        // 'value' matches the unset value defined in 'bdetu_unset' it will be
        // formatted as "NULL".  Otherwise, the 'bdet_Datetime' is formatted
        // as a <DATE> (see the above function) followed by a single space
        // followed by a <TIME> (see the above function).  If 'level' is
        // specified, optionally specify 'spacesPerLevel'.  The formatted
        // 'bdet_Datetime will be indented by (level * spacesPerLevel) spaces.
        // Making 'level' negative suppresses indentation.  The behavior is
        // undefined if buffer == 0, value == 0, or spacesPerLevel < 0.  Note
        // that 'value' is output as a textual representation that is parsable
        // by the corresponding 'parseTime' function.

    static void generateDatetimeTz(bsl::vector<char>      *buffer,
                                   const bdet_DatetimeTz&  value,
                                   int                     level = 0,
                                   int                     spacesPerLevel = 4);
        // Format the specified 'value' using the optionally specified
        // indentation 'level' and append the result to the specified 'buffer'
        // while maintaining null-character termination.  If the specified
        // 'value' matches the unset value defined in 'bdetu_unset' it will be
        // formatted as "NULL".  Otherwise, the 'bdet_DatetimeTz' is formatted
        // as a <DATE> (see the above function) followed by a single space
        // followed by a <TIME> (see the above function).  If 'level' is
        // specified, optionally specify 'spacesPerLevel'.  The formatted
        // 'bdet_Datetime will be indented by (level * spacesPerLevel) spaces.
        // Making 'level' negative suppresses indentation.  The behavior is
        // undefined if buffer == 0, value == 0, or spacesPerLevel < 0.  Note
        // that 'value' is output as a textual representation that is parsable
        // by the corresponding 'parseTime' function.

    static void generateCharArray(
                                 bsl::vector<char>        *buffer,
                                 const bsl::vector<char>&  value,
                                 int                       level = 0,
                                 int                       spacesPerLevel = 4);
        // Format the specified 'value' using the optionally specified
        // indentation 'level' and append the result to the specified 'buffer'
        // while maintaining null-character termination.  The 'buffer' is
        // formatted as a comma delimited list of elements surrounded by '['
        // and ']'.  If the element matches the unset value for the element
        // type defined in 'bdetu_unset' it will be formatted as "NULL".
        // Otherwise, it will be formatted as <CHAR>.
        //
        // If 'level' is specified, optionally specify 'spacesPerLevel'.  When
        // 'level' is positive, the indentation of both opening and closing
        // brackets is specified by (level * spacesPerLevel) spaces.  When
        // 'level' is negative, the opening bracket indentation is suppressed.
        // Each array element is generated on the new line and idented by
        // ((level + 1) * spacesPerLevel) spaces.  The behavior is undefined
        // unless 0 == 'buffer', 0 == 'value', 0 > spacesPerLevel.  Note that
        // 'value' is output as a textual representation that is parsable by
        // the corresponding 'parseArray' function.

    static void generateShortArray(
                                bsl::vector<char>         *buffer,
                                const bsl::vector<short>&  value,
                                int                        level = 0,
                                int                        spacesPerLevel = 4);
        // Format the specified 'value' using the optionally specified
        // indentation 'level' and append the result to the specified 'buffer'
        // while maintaining null-character termination.  The 'buffer' is
        // formatted as a comma delimited list of elements surrounded by '['
        // and ']'.  If the element matches the unset value for the element
        // type defined in 'bdetu_unset' it will be formatted as "NULL".
        // Otherwise, it will be formatted as <CHAR>.
        //
        // If 'level' is specified, optionally specify 'spacesPerLevel'.  When
        // 'level' is positive, the indentation of both opening and closing
        // brackets is specified by (level * spacesPerLevel) spaces.  When
        // 'level' is negative, the opening bracket indentation is suppressed.
        // Each array element is generated on the new line and idented by
        // ((level + 1) * spacesPerLevel) spaces.  The behavior is undefined
        // unless 0 == 'buffer', 0 == 'value', 0 > spacesPerLevel.  Note that
        // 'value' is output as a textual representation that is parsable by
        // the corresponding 'parseArray' function.

    static void generateIntArray(bsl::vector<char>       *buffer,
                                 const bsl::vector<int>&  value,
                                 int                      level = 0,
                                 int                      spacesPerLevel = 4);
        // Format the specified 'value' using the optionally specified
        // indentation 'level' and append the result to the specified 'buffer'
        // while maintaining null-character termination.  The 'buffer' is
        // formatted as a comma delimited list of elements surrounded by '['
        // and ']'.  If the element matches the unset value for the element
        // type defined in 'bdetu_unset' it will be formatted as "NULL".
        // Otherwise, it will be formatted as <CHAR>.
        //
        // If 'level' is specified, optionally specify 'spacesPerLevel'.  When
        // 'level' is positive, the indentation of both opening and closing
        // brackets is specified by (level * spacesPerLevel) spaces.  When
        // 'level' is negative, the opening bracket indentation is suppressed.
        // Each array element is generated on the new line and idented by
        // ((level + 1) * spacesPerLevel) spaces.  The behavior is undefined
        // unless 0 == 'buffer', 0 == 'value', 0 > spacesPerLevel.  Note that
        // 'value' is output as a textual representation that is parsable by
        // the corresponding 'parseArray' function.

    static void generateInt64Array(
             bsl::vector<char>                            *buffer,
             const bsl::vector<bsls_PlatformUtil::Int64>&  value,
             int                                           level = 0,
             int                                           spacesPerLevel = 4);
        // Format the specified 'value' using the optionally specified
        // indentation 'level' and append the result to the specified 'buffer'
        // while maintaining null-character termination.  The 'buffer' is
        // formatted as a comma delimited list of elements surrounded by '['
        // and ']'.  If the element matches the unset value for the element
        // type defined in 'bdetu_unset' it will be formatted as "NULL".
        // Otherwise, it will be formatted as <CHAR>.
        //
        // If 'level' is specified, optionally specify 'spacesPerLevel'.  When
        // 'level' is positive, the indentation of both opening and closing
        // brackets is specified by (level * spacesPerLevel) spaces.  When
        // 'level' is negative, the opening bracket indentation is suppressed.
        // Each array element is generated on the new line and idented by
        // ((level + 1) * spacesPerLevel) spaces.  The behavior is undefined
        // unless 0 == 'buffer', 0 == 'value', 0 > spacesPerLevel.  Note that
        // 'value' is output as a textual representation that is parsable by
        // the corresponding 'parseArray' function.

    static void generateFloatArray(
                                bsl::vector<char>         *buffer,
                                const bsl::vector<float>&  value,
                                int                        level = 0,
                                int                        spacesPerLevel = 4);
        // Format the specified 'value' using the optionally specified
        // indentation 'level' and append the result to the specified 'buffer'
        // while maintaining null-character termination.  The 'buffer' is
        // formatted as a comma delimited list of elements surrounded by '['
        // and ']'.  If the element matches the unset value for the element
        // type defined in 'bdetu_unset' it will be formatted as "NULL".
        // Otherwise, it will be formatted as <CHAR>.
        //
        // If 'level' is specified, optionally specify 'spacesPerLevel'.  When
        // 'level' is positive, the indentation of both opening and closing
        // brackets is specified by (level * spacesPerLevel) spaces.  When
        // 'level' is negative, the opening bracket indentation is suppressed.
        // Each array element is generated on the new line and idented by
        // ((level + 1) * spacesPerLevel) spaces.  The behavior is undefined
        // unless 0 == 'buffer', 0 == 'value', 0 > spacesPerLevel.  Note that
        // 'value' is output as a textual representation that is parsable by
        // the corresponding 'parseArray' function.

    static void generateDoubleArray(
                               bsl::vector<char>          *buffer,
                               const bsl::vector<double>&  value,
                               int                         level = 0,
                               int                         spacesPerLevel = 4);
        // Format the specified 'value' using the optionally specified
        // indentation 'level' and append the result to the specified 'buffer'
        // while maintaining null-character termination.  The 'buffer' is
        // formatted as a comma delimited list of elements surrounded by '['
        // and ']'.  If the element matches the unset value for the element
        // type defined in 'bdetu_unset' it will be formatted as "NULL".
        // Otherwise, it will be formatted as <CHAR>.
        //
        // If 'level' is specified, optionally specify 'spacesPerLevel'.  When
        // 'level' is positive, the indentation of both opening and closing
        // brackets is specified by (level * spacesPerLevel) spaces.  When
        // 'level' is negative, the opening bracket indentation is suppressed.
        // Each array element is generated on the new line and idented by
        // ((level + 1) * spacesPerLevel) spaces.  The behavior is undefined
        // unless 0 == 'buffer', 0 == 'value', 0 > spacesPerLevel.  Note that
        // 'value' is output as a textual representation that is parsable by
        // the corresponding 'parseArray' function.

    static void generateStringArray(
                          bsl::vector<char>               *buffer,
                          const bsl::vector<bsl::string>&  value,
                          int                              level = 0,
                          int                              spacesPerLevel = 4);
        // Format the specified 'value' using the optionally specified
        // indentation 'level' and append the result to the specified 'buffer'
        // while maintaining null-character termination.  The 'buffer' is
        // formatted as a comma delimited list of elements surrounded by '['
        // and ']'.  If the element matches the unset value for the element
        // type defined in 'bdetu_unset' it will be formatted as "NULL".
        // Otherwise, it will be formatted as <CHAR>.
        //
        // If 'level' is specified, optionally specify 'spacesPerLevel'.  When
        // 'level' is positive, the indentation of both opening and closing
        // brackets is specified by (level * spacesPerLevel) spaces.  When
        // 'level' is negative, the opening bracket indentation is suppressed.
        // Each array element is generated on the new line and idented by
        // ((level + 1) * spacesPerLevel) spaces.  The behavior is undefined
        // unless 0 == 'buffer', 0 == 'value', 0 > spacesPerLevel.  Note that
        // 'value' is output as a textual representation that is parsable by
        // the corresponding 'parseArray' function.

    static void generateDateArray(
                            bsl::vector<char>             *buffer,
                            const bsl::vector<bdet_Date>&  value,
                            int                            level = 0,
                            int                            spacesPerLevel = 4);
        // Format the specified 'value' using the optionally specified
        // indentation 'level' and append the result to the specified 'buffer'
        // while maintaining null-character termination.  The 'buffer' is
        // formatted as a comma delimited list of elements surrounded by '['
        // and ']'.  If the element matches the unset value for the element
        // type defined in 'bdetu_unset' it will be formatted as "NULL".
        // Otherwise, it will be formatted as <CHAR>.
        //
        // If 'level' is specified, optionally specify 'spacesPerLevel'.  When
        // 'level' is positive, the indentation of both opening and closing
        // brackets is specified by (level * spacesPerLevel) spaces.  When
        // 'level' is negative, the opening bracket indentation is suppressed.
        // Each array element is generated on the new line and idented by
        // ((level + 1) * spacesPerLevel) spaces.  The behavior is undefined
        // unless 0 == 'buffer', 0 == 'value', 0 > spacesPerLevel.  Note that
        // 'value' is output as a textual representation that is parsable by
        // the corresponding 'parseArray' function.

    static void generateDateTzArray(
                          bsl::vector<char>               *buffer,
                          const bsl::vector<bdet_DateTz>&  value,
                          int                              level = 0,
                          int                              spacesPerLevel = 4);
        // Format the specified 'value' using the optionally specified
        // indentation 'level' and append the result to the specified 'buffer'
        // while maintaining null-character termination.  The 'buffer' is
        // formatted as a comma delimited list of elements surrounded by '['
        // and ']'.  If the element matches the unset value for the element
        // type defined in 'bdetu_unset' it will be formatted as "NULL".
        // Otherwise, it will be formatted as <CHAR>.
        //
        // If 'level' is specified, optionally specify 'spacesPerLevel'.  When
        // 'level' is positive, the indentation of both opening and closing
        // brackets is specified by (level * spacesPerLevel) spaces.  When
        // 'level' is negative, the opening bracket indentation is suppressed.
        // Each array element is generated on the new line and idented by
        // ((level + 1) * spacesPerLevel) spaces.  The behavior is undefined
        // unless 0 == 'buffer', 0 == 'value', 0 > spacesPerLevel.  Note that
        // 'value' is output as a textual representation that is parsable by
        // the corresponding 'parseArray' function.

    static void generateTimeArray(
                            bsl::vector<char>             *buffer,
                            const bsl::vector<bdet_Time>&  value,
                            int                            level = 0,
                            int                            spacesPerLevel = 4);
        // Format the specified 'value' using the optionally specified
        // indentation 'level' and append the result to the specified 'buffer'
        // while maintaining null-character termination.  The 'buffer' is
        // formatted as a comma delimited list of elements surrounded by '['
        // and ']'.  If the element matches the unset value for the element
        // type defined in 'bdetu_unset' it will be formatted as "NULL".
        // Otherwise, it will be formatted as <CHAR>.
        //
        // If 'level' is specified, optionally specify 'spacesPerLevel'.  When
        // 'level' is positive, the indentation of both opening and closing
        // brackets is specified by (level * spacesPerLevel) spaces.  When
        // 'level' is negative, the opening bracket indentation is suppressed.
        // Each array element is generated on the new line and idented by
        // ((level + 1) * spacesPerLevel) spaces.  The behavior is undefined
        // unless 0 == 'buffer', 0 == 'value', 0 > spacesPerLevel.  Note that
        // 'value' is output as a textual representation that is parsable by
        // the corresponding 'parseArray' function.

    static void generateTimeTzArray(
                          bsl::vector<char>               *buffer,
                          const bsl::vector<bdet_TimeTz>&  value,
                          int                              level = 0,
                          int                              spacesPerLevel = 4);
        // Format the specified 'value' using the optionally specified
        // indentation 'level' and append the result to the specified 'buffer'
        // while maintaining null-character termination.  The 'buffer' is
        // formatted as a comma delimited list of elements surrounded by '['
        // and ']'.  If the element matches the unset value for the element
        // type defined in 'bdetu_unset' it will be formatted as "NULL".
        // Otherwise, it will be formatted as <CHAR>.
        //
        // If 'level' is specified, optionally specify 'spacesPerLevel'.  When
        // 'level' is positive, the indentation of both opening and closing
        // brackets is specified by (level * spacesPerLevel) spaces.  When
        // 'level' is negative, the opening bracket indentation is suppressed.
        // Each array element is generated on the new line and idented by
        // ((level + 1) * spacesPerLevel) spaces.  The behavior is undefined
        // unless 0 == 'buffer', 0 == 'value', 0 > spacesPerLevel.  Note that
        // 'value' is output as a textual representation that is parsable by
        // the corresponding 'parseArray' function.

    static void generateDatetimeArray(
                        bsl::vector<char>                 *buffer,
                        const bsl::vector<bdet_Datetime>&  value,
                        int                                level = 0,
                        int                                spacesPerLevel = 4);
        // Format the specified 'value' using the optionally specified
        // indentation 'level' and append the result to the specified 'buffer'
        // while maintaining null-character termination.  The 'buffer' is
        // formatted as a comma delimited list of elements surrounded by '['
        // and ']'.  If the element matches the unset value for the element
        // type defined in 'bdetu_unset' it will be formatted as "NULL".
        // Otherwise, it will be formatted as <CHAR>.
        //
        // If 'level' is specified, optionally specify 'spacesPerLevel'.  When
        // 'level' is positive, the indentation of both opening and closing
        // brackets is specified by (level * spacesPerLevel) spaces.  When
        // 'level' is negative, the opening bracket indentation is suppressed.
        // Each array element is generated on the new line and idented by
        // ((level + 1) * spacesPerLevel) spaces.  The behavior is undefined
        // unless 0 == 'buffer', 0 == 'value', 0 > spacesPerLevel.  Note that
        // 'value' is output as a textual representation that is parsable by
        // the corresponding 'parseArray' function.

    static void generateDatetimeTzArray(
                      bsl::vector<char>                   *buffer,
                      const bsl::vector<bdet_DatetimeTz>&  value,
                      int                                  level = 0,
                      int                                  spacesPerLevel = 4);
        // Format the specified 'value' using the optionally specified
        // indentation 'level' and append the result to the specified 'buffer'
        // while maintaining null-character termination.  The 'buffer' is
        // formatted as a comma delimited list of elements surrounded by '['
        // and ']'.  If the element matches the unset value for the element
        // type defined in 'bdetu_unset' it will be formatted as "NULL".
        // Otherwise, it will be formatted as <CHAR>.
        //
        // If 'level' is specified, optionally specify 'spacesPerLevel'.  When
        // 'level' is positive, the indentation of both opening and closing
        // brackets is specified by (level * spacesPerLevel) spaces.  When
        // 'level' is negative, the opening bracket indentation is suppressed.
        // Each array element is generated on the new line and idented by
        // ((level + 1) * spacesPerLevel) spaces.  The behavior is undefined
        // unless 0 == 'buffer', 0 == 'value', 0 > spacesPerLevel.  Note that
        // 'value' is output as a textual representation that is parsable by
        // the corresponding 'parseArray' function.

    static void stripNull(bsl::vector<char> *buffer);
        // Remove the trailing '\0' from the specified 'buffer', if any.  This
        // method has no effect if the last element in 'buffer' is not '\0'.
};

// ============================================================================
//                      INLINE FUNCTION DEFINITIONS
// ============================================================================

                        // -------------------------
                        // struct bdepu_NTypesParser
                        // -------------------------

// CLASS METHODS
inline
int bdepu_NTypesParser::parseDelimitedString(const char  **endPos,
                                             bsl::string  *result,
                                             const char   *inputString,
                                             char          leftDelimiter,
                                             char          rightDelimiter)
{
    BSLS_ASSERT_SAFE(endPos);
    BSLS_ASSERT_SAFE(result);
    BSLS_ASSERT_SAFE(inputString);

    return bdepu_TypesParserImpUtil::parseDelimitedString(endPos,
                                                          result,
                                                          inputString,
                                                          leftDelimiter,
                                                          rightDelimiter);
}

// We reproduce 'stripNull' in this component though it is verbatim in
// 'bdepu_TypesParser' because calling it there would be the *only* dependency
// 'bdepu_ntypesparser' would have on 'bdepu_typesparser', so adding the
// function here reduces dependencies by one level.

inline
void bdepu_NTypesParser::stripNull(bsl::vector<char> *buffer)
{
    BSLS_ASSERT_SAFE(buffer);

    const bsl::size_t bufferLen = buffer->size();

    if (bufferLen && '\0' == (*buffer)[bufferLen - 1]) {
        buffer->pop_back();
    }
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
