// bdempu_aggregate.h           -*-C++-*-
#ifndef INCLUDED_BDEMPU_AGGREGATE
#define INCLUDED_BDEMPU_AGGREGATE

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Convert 'bdem' lists and tables to/from text representation.
//
//@DEPRECATED: Use 'baexml_encoder' and 'baexml_decoder' instead.
//
//@CLASSES:
//  bdempu_Aggregate: namespace for list and table parsing functions
//
//@AUTHOR: John Lakos (jlakos)
//
//@DESCRIPTION: This component implements two types of functions: 'parse' and
// 'generate', which form the basic building blocks for top-down (and possibly
// recursive-descent) parsers.  The 'parse' functions initialize the resulting
// object with the value extracted from the input string.  The 'generate'
// functions append a text representation of the object to a user-specified
// 'char' buffer.
//
// The component provides a parse method for 'bdem_List' and for 'bdem_Table'.
// There is a 'generate' method for these two types as well.
//
// The parsing functions in this component *do* consume leading whitespace.
// For both lists and tables there are two kinds of generating functions.
// One allows producing whitespace (indentation) before producing the text
// representing a given value.  The other supports indentation *and* both
// removal of a possible (single) terminating null from the buffer prior to
// writing the value, and addition of a null to the buffer subsequent to
// writing that value.
//
// The representation produced by each 'generate' function is one that the
// corresponding parsing function can recognize.  The 'generate' functions
// operate on a 'bsl::vector<char>' buffer, which is used to maintain an
// efficient representation of the output.
//
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
//         {m, infinity}.  The plus (+) and star (*) operations are
//         equivalent to {1,} and {0,} respectively.
//
//   ( ... ) Parentheses are used for grouping.  An operator, for example, *,
//         +, {}, can work on a single character or on a regular expression
//         enclosed in parentheses.  For example, (a*(cb+)*)$.
//
//    \    Backslash can escape any of the symbols defined above, as well as
//         single characters that would be escaped in a C-style string.
//..
///PRODUCTION RULES FOR PARSING FUNCTIONS
///--------------------------------------
//..
// <WHITESPACE>  ::= any character for which isspace(char) returns 'true', or
//                   any legal C- or C++-style comment.
// <NONWHITESPACE> ::= any character for which isspace(char) returns 'false'.
// <EOS>          ::= End of string, or ASCII 0.
//
//                          --- list ---
// <LIST>         ::= <WHITESPACE>* '{' <WHITESPACE>+ <ELEMENTS>?
//                    <WHITESPACE>+ '}' | <NULL>
// <ELEMENTS>     ::= <ELEMENT> (<WHITESPACE>+ <ELEMENT>)*
// <ELEMENT>      ::=           "CHAR"           <CHAR>
//                  |          "SHORT"          <SHORT>
//                  |            "INT"            <INT>
//                  |          "INT64"          <INT64>
//                  |          "FLOAT"          <FLOAT>
//                  |         "DOUBLE"         <DOUBLE>
//                  |         "STRING"         <STRING>
//                  |       "DATETIME"       <DATETIME>
//                  |           "DATE"           <DATE>
//                  |           "TIME"           <TIME>
//                  |     "CHAR_ARRAY"     <CHAR_ARRAY>
//                  |    "SHORT_ARRAY"    <SHORT_ARRAY>
//                  |      "INT_ARRAY"      <INT_ARRAY>
//                  |    "INT64_ARRAY"    <INT64_ARRAY>
//                  |    "FLOAT_ARRAY"    <FLOAT_ARRAY>
//                  |   "DOUBLE_ARRAY"   <DOUBLE_ARRAY>
//                  |   "STRING_ARRAY"   <STRING_ARRAY>
//                  | "DATETIME_ARRAY" <DATETIME_ARRAY>
//                  |     "DATE_ARRAY"     <DATE_ARRAY>
//                  |     "TIME_ARRAY"     <TIME_ARRAY>
//                  |           "LIST"           <LIST>
//                  |          "TABLE"          <TABLE>
// <NULL>         ::= [Nn][Uu][Ll][Ll]
//
//                          --- table ---
// <TABLE>        ::= <WHITESPACE>* '{' <COLUMN_TYPES> <ROW>* '}' | <NULL>
// <COLUMN_TYPES> ::= '[' <ELEMENT_TYPES>? ']'
// <ROW>          ::= '{' <ELEMENT_VALUES>? '}' | <NULL>
// <NULL>         ::= [Nn][Uu][Ll][Ll]
// <ELEMENT_TYPES>::= <ELEMENT_TYPE> (<WHITESPACE>+ <ELEMENT_TYPE>)*
// <ELEMENT_TYPE> ::=     "CHAR"        |     "CHAR_ARRAY"
//                  |    "SHORT"        |    "SHORT_ARRAY"
//                  |      "INT"        |      "INT_ARRAY"
//                  |    "INT64"        |    "INT64_ARRAY"
//                  |    "FLOAT"        |    "FLOAT_ARRAY"
//                  |   "DOUBLE"        |   "DOUBLE_ARRAY"
//                  |   "STRING"        |   "STRING_ARRAY"
//                  | "DATETIME"        | "DATETIME_ARRAY"
//                  |     "DATE"        |     "DATE_ARRAY"
//                  |     "TIME"        |     "TIME_ARRAY"
//                  |     "LIST"        |          "TABLE"
// <ELEMENT_VALUES>  ::= <ELEMENT_VALUE> (<ELEMENT_VALUE>)*
// <ELEMENT_VALUE>   ::=     <CHAR>  |     <CHAR_ARRAY>
//                     |    <SHORT>  |    <SHORT_ARRAY>
//                     |      <INT>  |      <INT_ARRAY>
//                     |    <INT64>  |    <INT64_ARRAY>
//                     |    <FLOAT>  |    <FLOAT_ARRAY>
//                     |   <DOUBLE>  |   <DOUBLE_ARRAY>
//                     |   <STRING>  |   <STRING_ARRAY>
//                     | <DATETIME>  | <DATETIME_ARRAY>
//                     |     <DATE>  |     <DATE_ARRAY>
//                     |     <TIME>  |     <TIME_ARRAY>
//                     |     <LIST>  |    <TABLE_VALUE>
//
// <CHAR>, <CHAR_ARRAY>:         see 'bdepu_typesparser.h'
// <DOUBLE>, <DOUBLE_ARRAY>:     see 'bdepu_typesparser.h'
// <FLOAT>, <FLOAT_ARRAY>:       see 'bdepu_typesparser.h'
// <INT>, <INT_ARRAY>:           see 'bdepu_typesparser.h'
// <INT64>, <INT64_ARRAY>:       see 'bdepu_typesparser.h'
// <SHORT>, <SHORT_ARRAY>:       see 'bdepu_typesparser.h'
// <DATE>, <DATE_ARRAY>:         see 'bdepu_typesparser.h'
// <DATETIME>, <DATETIME_ARRAY>: see 'bdepu_typesparser.h'
// <TIME>, <TIME_ARRAY>:         see 'bdepu_typesparser.h'
// <STRING>, <STRING_ARRAY>:     see 'bdepu_typesparser.h'
//..
///EXAMPLE
///-------
// A list ('bdem_List') is a hierarchical value-semantic data structure
// containing a heterogeneous sequence of objects corresponding to the 22
// element types enumerated in 'bdem_elemtype'.  These 22 element types consist
// of ten scalar types ('char', 'short', 'int', 'Int64', 'float', 'double',
// 'bsl::string', bdet_DateTime', 'bdet_Date', and 'bdet_Time'), their ten
// corresponding array types ['bsl::vector<char>' ..
// 'bsl::vector<bdet_Time>'], 'bdem_List' itself, and 'bdem_Table' (described
// next).
//
// The following textual representation of a LIST value contains one instance
// of each of these 22 element types, separated by whitespace.  The first 12
// elements (indices 0 - 11) are of terminal types; the last two elements
// (indices 12 - 13) are of recursive types.
//..
//  {
//  /* Sample LIST  -- (note that embedded C-style comments are supported) */
//      CHAR     44                                    // element index 0
//      SHORT    1899                                  // element index 1
//      INT      314592653                             // element index 2
//      INT64    2718211821459045                      // element index 3
//      FLOAT    3.14 /* approximate */                // element index 4
//      DOUBLE   3.14 /* approximate */                // element index 5
//      STRING   "Happy New Year"                      // element index 6
//      DATETIME 2000/01/02 14:30:00.000               // element index 7
//      DATE     1913/10/30                            // element index 8
//      TIME     11:07:15.283                          // element index 9
//
//  // Note that embedded C++-style comments are supported.
//
//      CHAR_ARRAY [                                   // element index 10
//          55
//          44
//      ]
//      SHORT_ARRAY [ 55,  1899 ]                      // element index 11
//      INT_ARRAY [ 55,  314592653 ]                   // element index 12
//      INT64_ARRAY [ 55, 2718211821459045 ]           // element index 13
//      FLOAT_ARRAY [ 3.5 ]     /* exact! */           // element index 14
//      DOUBLE_ARRAY [ 3.5625 ] /* exact! */           // element index 15
//      STRING_ARRAY                                   // element index 16
//      [
//          "Hello\tthere"                                    // array index 0
//          "NULL" /* ordinary string - no special meaning */ // array index 1
//          NULL   /* default StringArray element = "~`~`~" */// array index 2
//          ""     /* empty StringArray element = "" */       // array index 3
//          0      /* null (0) StringArray element = 0 */     // array index 4
//      ]
//      DATETIME_ARRAY [ ]                             // element index 17
//      DATE_ARRAY [
//      ]                                              // element index 18
//      TIME_ARRAY NULL /* any default array = [ ] */  // element index 19
//
//      LIST {                                         // element index 20
//          INT NULL    // a default int = -2^31              // list index 0
//          DOUBLE nuLL // a default double =  1 - 2^48       // list index 1
//              // Note that "null" label is case-insensitive
//          INT_ARRAY [ 7 8 9 ]                               // list index 2
//          STRING ""  /* empty, *not* default; can't be 0 */ // list index 3
//          LIST {                                            // list index 4
//              STRING NULL // a default 'bsl::string' = "~`~`~"
//              DATE NULL   // a default 'bdet_Date' = 1/1/1
//              TIME NULL   // a default 'bdet_Time' = 24:00:00.000
//              LIST NULL   // a default (unconstrained) list = { }
//          }
//      }
//
//      TABLE {                                         // element index 21
//          /* col0 col1    col2    col3              col4 */
//          [ INT DOUBLE STRING LIST             TABLE ] // <-column types
//          { 0   1.5    "abc"  { INT 1 INT 2 } { [ ] } }           // row0
//          { NULL 1.111  "One"  { STRING "foo" } NULL }            // row1
//          { 2   NULL   "Two"  { INT 3 INT 4 } { [ INT ]  } }      // row2
//          { 3   3.333  NULL   { DOUBLE 1e10  } { [ ] { } } }      // row3
//          { 4   4.444  "Four" NULL             {[INT_ARRAY]{[]}}} // row4
//          { 5   5.555  "Five" { }              { [DATE]{ null }}} // row5
//          { 6   6.     "Six"  { INT NULL }     {[INT,INT]{1,2}} } // row6
//      }
//
//  } /* end of sample LIST value */
//..
// The first element (index 0) in the example LIST value above is of type CHAR
// with value 44, the ASCII code for comma (',').  Other 'bdem' types
// representing fundamental types follow at index positions 1 - 5.
//
// The DOUBLE (at index 5) has the value 3.14, which may not parse exactly.
// Note that a floating-point value will be approximate unless it can be
// represented exactly as the sum of integral (including negative) powers of 2
// (e.g., 5 = 2^2 + 2^0 and 2.75 = 2^1 + 2^-1 + 2^-2).
//
// The element at index 6 is a STRING whose value is "Happy New Year".  String
// values containing internal whitespace must be double-quoted.  Note that a
// 'bsl::string' cannot represent a C-style null string -- i.e., where the
// string address itself is 0.
//
// The next three elements (indices 7 - 9) represent values for DATETIME,
// DATE, and TIME elements.  1/1/1 happens to be the default value for a DATE
// element, and
//..
//     DATE 1/1/1
//..
// is always identical in meaning to
//..
//     DATE NULL
//..
// The same is true for the obscure (but valid) "default" values for the other
// scalar types.
//
// The CHAR_ARRAY element (index 10) of length 2 contains the integer values
// 55 and 44.  The DOUBLE_ARRAY element (index 15) of length 1 contains the
// value 3.5625, which can be parsed exactly since
// 3.5625 = 2^1 + 2^0 + 2-1 + 2^4.
//
// The STRING_ARRAY element (index 16) contains 5 sub-elements.  The first
// (array index 0) contains a string with character escape sequences for tab.
// Escape sequences other than '\0' all constitute valid string characters.
// Second (array index 1) is the literal string "NULL"; the meaning of the
// keyword is absent when hidden inside quotes.  Next (array index 2) is the
// NULL (or default) string, which has associated with it an obscure reserved
// value ("~`~`~") that is neither empty (array index 3) nor 0 (array index
// 4).  Note that a default element in a STRING_ARRAY has
// the same value as a default STRING ('bsl::string') element in a list or
// table.  Unlike a STRING, however, a STRING_ARRAY element can hold a C-style
// zero (0) address value.
//
// The next three elements (indices 17 - 19) represent empty (and therefore
// default) instances of DATETIME_ARRAY, DATE_ARRAY, and TIME_ARRAY,
// respectively.  Note that
//..
//     DATE_ARRAY [ ]
//..
// is always identical in meaning to
//..
//     DATE_ARRAY NULL
//..
// and similarly for the other five array types.
//
// The LIST element (index 20) contains five elements: (0) a default INT,
// (1) an default double (the 'NULL' keyword is case-insensitive), (2) an array
// of three integers, (3) an empty string, and (4) a heterogeneous sublist of
// length 4 containing a STRING, a DATE, a TIME, and a LIST, each having
// default value.  Note that
//..
//     LIST { }
//..
// is identical in meaning to
//..
//     LIST NULL
//..
// for unconstrained lists (see below for more on constraints).
//
// Lastly, the TABLE element (index 21) consists of 5 columns and 6 rows.
// While a LIST is a heterogeneous collection of elements defined in a single
// row ('bdem_Row'), a TABLE is an array of column types followed by a
// homogeneous sequence of zero or more rows, where the 'bdem_ElemType::Type'
// of the i'th element in each row matches that of the i'th column in the
// table.
//
// The array of ('bdem_ElemType::Type') column types, which in this example is
// '[ INT, DOUBLE, STRING, LIST, TABLE ]', appears first.  Any rows that
// follow must have the same length as the column-type array, and each
// row element must be parsable as its corresponding column type.
//
// The first row (row0) contains the integer 0 (col0), the (exact) double 1.5
// (col1), the string "abc" (col2), a list of two integers (col3) and a
// completely empty table (col4).  Note that an unconstrained table with 0
// columns and 0 rows
//..
//     TABLE { [ ] }                            // (row0, col4)
//..
// is identical in meaning to
//..
//     TABLE NULL                               // (row1, col4)
//..
// but a table with 1 column and 0 rows
//..
//     TABLE { [ INT ] }                        // (row2, col4)
//..
// or 0 columns and 1 row
//..
//     TABLE { [ ] { } }                        // (row3, col4)
//..
// is not.
//
// The entries for the first three columns of the subsequent rows are also
// straightforward.  Each value in the column is a textual representation of
// an instance of that type.  The last two columns are no different.  Note
// however that LIST (col3) and TABLE (col4) are unconstrained types.  The
// only requirement for values in column 3 is that each be parsable as a LIST
// -- there is no constraint on the particular structure of the list, which in
// this example varies from one row to the next.
//
// A column of type (unconstrained) TABLE is similar.  Each sub-table element
// value may vary in the number and types of its columns as well as the number
// of its of rows.  The syntax can also be somewhat complicated.  For
// example, the sub-table entry at row 4 and column 4 of the main table
// element (shown here fully indented)
//..
//  {
//      [
//          INT_ARRAY
//      ]
//      {
//          [ ]
//      }
//  }
//..
// represents a TABLE with one column of type INT_ARRAY and one row containing
// a single empty (and therefore default) instance of that array.  Note that,
// unlike a list, a row itself may not be null:
//..
//  {
//      [
//          INT_ARRAY
//      ]
//      NULL                                    // ILLEGAL SYNTAX
//  }
//..
// Both LIST and TABLE can serve as the root of a recursive data structure.
// Just as a LIST can be queried for the number of elements in its row, a
// TABLE can be queried for both the number of columns and number of rows.
// Every element in a row can be queried for its type and value.  It is not
// uncommon, however, to standardize on one or the other as a matter of
// policy, and clearly LIST is a simpler structure than TABLE.
//
// Finally, there are no names associated with the values contained in either
// of these aggregate data objects; each field is identified positionally, via
// its integer index.

#ifndef INCLUDED_BDESCM_VERSION
#include <bdescm_version.h>
#endif

#ifndef INCLUDED_BDEM_ELEMTYPE
#include <bdem_elemtype.h>
#endif

#ifndef INCLUDED_BDEPU_NTYPESPARSER
#include <bdepu_ntypesparser.h>
#endif

#ifndef INCLUDED_BSL_VECTOR
#include <bsl_vector.h>
#endif


namespace BloombergLP {

class bdem_List;
class bdem_Row;
class bdem_Table;

                        // =======================
                        // struct bdempu_Aggregate
                        // =======================

struct bdempu_Aggregate {
    // This struct provides a namespace for a suite of stateless procedures
    // that provide parsing functionality for 'bdem_List' and 'bdem_Table',
    // skipping past any leading whitespace before looking for the type in a
    // specified null-terminated character string.  Also provided are
    // stateless 'generate' procedures, each of which emit text that the
    // corresponding parsing function can recognize.  The 'generate' functions
    // operate on a 'bsl::vector<char>', which is used to maintain an efficient
    // representation of the output.

  private:
    // PRIVATE CLASS METHODS
    static int parseListElement(const char                 **endPos,
                                bdem_List                   *result,
                                const bdem_ElemType::Type&   elemType,
                                const char                  *inputString);
        // Parse the specified 'inputString' (ignoring any leading whitespace)
        // for the specified 'elemType' and append it to the specified
        // 'result'.  Store in the specified '*endPos' the address of the
        // non-modifiable character in 'inputString' immediately following the
        // successfully parsed text, or the position at which the parse
        // failure was detected.  Return zero on success, and a non-zero value
        // otherwise.  The value of '*result' is undefined if a parse failure
        // occurs.  The behavior is undefined if any address argument is 0.

    static void generateRowElement(bsl::vector<char>          *buffer,
                                   const bdem_Row&             value,
                                   int                         index,
                                   const bdem_ElemType::Type&  elemType,
                                   int                         level,
                                   int                         spacesPerLevel);
        // Format the item at specified position 'index' in specified row
        // 'value' as an item of type 'elemType' using the optionally
        // specified indentation 'level' and 'spacesPerLevel', and append the
        // result to the specified 'buffer' while maintaining (or adding if
        // not present) null-character termination.  When 'level' is positive,
        // indent the short by ('level' * 'spacesPerLevel') spaces.  A negative
        // 'level' suppresses indentation.  The behavior is undefined if
        // 0 == 'buffer' or 'spacesPerLevel' < 0.  Note that 'value' is
        // emitted in a form parsable by the corresponding 'parseListElement'
        // function.

  public:
    // CLASS METHODS
    static int parseList(const char **endPos,
                         bdem_List   *result,
                         const char  *inputString);
        // Parse the specified 'inputString' (ignoring any leading whitespace)
        // for a sequence of characters matching the production rule <LIST>,
        // and place into the specified 'result' the corresponding value.
        // Store in the specified '*endPos' the address of the non-modifiable
        // character in 'inputString' immediately following the successfully
        // parsed text, or the position at which the parse failure was
        // detected.  Return zero on success, and a non-zero value otherwise.
        // The value of '*result' is undefined if a parse failure occurs.  The
        // behavior is undefined if any argument is 0.
        //
        // A parse failure can occur for the following reasons:
        //..
        //   1. The first non-whitespace character in 'inputString' is not a
        //      left curly-brace  ('{').
        //   2. There is a typename in 'inputString' not followed by a value.
        //   3. There is a typename in 'inputString' followed by a sequence
        //      of characters not parsable as that type.
        //   4. There is a value in 'inputString' not preceded by the name
        //      of its type.
        //   5. There is no terminating right curly-brace ('}') in
        //      'inputString'.
        //..

    static int parseTable(const char **endPos,
                          bdem_Table  *result,
                          const char  *inputString);
        // Parse the specified 'inputString' (ignoring any leading whitespace)
        // for a sequence of characters matching the production rule <TABLE>
        // and place into the specified 'result' the corresponding value.
        // Store in the specified '*endPos' the address of the non-modifiable
        // character in 'inputString' immediately following the successfully
        // parsed text, or the position at which the parse failure was
        // detected.  Return zero on success, and a non-zero value otherwise.
        // The value of '*result' is undefined if a parse failure occurs.  The
        // behavior is undefined if any argument is 0.
        //
        // A parse failure can occur for the following reasons:
        //..
        //   1. The first non-whitespace character in 'inputString' is not a
        //      left curly-brace  ('{').
        //   2. The second non-whitespace character in 'inputString' is not a
        //      left square-bracket ('[').
        //   3. There is no terminating right square-bracket (']') in
        //      'inputString'.
        //   4. There is no terminating right curly-brace ('}') in
        //      'inputString'.
        //   5. The elements of some row in the table do not conform to the
        //      list of types described within the square brackets ('[' and
        //      ']').
        //..

    static void generateList(bsl::vector<char> *buffer,
                             const bdem_List&   value,
                             int                level = 0,
                             int                spacesPerLevel = 4);
        // Format the specified 'value' using the optionally specified
        // indentation 'level' and 'spacesPerLevel', and append the result to
        // the specified 'buffer' while maintaining (or adding if not present)
        // null-character termination.  When 'level' is positive, indent the
        // list by ('level' * 'spacesPerLevel') spaces.  A negative
        // 'level' suppresses indentation.  The behavior is undefined if
        // 0 == 'buffer' or 'spacesPerLevel' < 0.  Note that 'value' is
        // emitted in a form parsable by the corresponding 'parseList'
        // function.

    static void generateListRaw(bsl::vector<char> *buffer,
                                const bdem_List&   value,
                                int                level = 0,
                                int                spacesPerLevel = 4);
        // Format the specified 'value' using the optionally specified
        // indentation 'level' and 'spacesPerLevel', and append the result to
        // the specified 'buffer'.  When 'level' is positive, indent the
        // list by ('level' * 'spacesPerLevel') spaces.  A negative
        // 'level' suppresses indentation.  The behavior is undefined if
        // 0 == 'buffer' or 'spacesPerLevel' < 0.  Note that this method is
        // similar to 'generateList' except that no trailing null is ever
        // removed or supplied.

    static void generateTable(bsl::vector<char>  *buffer,
                              const bdem_Table&   value,
                              int                 level = 0,
                              int                 spacesPerLevel = 4);
        // Format the specified 'value' using the optionally specified
        // indentation 'level' and 'spacesPerLevel', and append the result to
        // the specified 'buffer' while maintaining (or adding if not present)
        // null-character termination.  When 'level' is positive, indent the
        // table by ('level' * 'spacesPerLevel') spaces.  A negative
        // 'level' suppresses indentation.  The behavior is undefined if
        // 0 == 'buffer' or 'spacesPerLevel' < 0.  Note that 'value' is
        // emitted in a form parsable by the corresponding 'parseTable'
        // function.

    static void generateTableRaw(bsl::vector<char>  *buffer,
                                 const bdem_Table&   value,
                                 int                 level = 0,
                                 int                 spacesPerLevel = 4);
        // Format the specified 'value' using the optionally specified
        // indentation 'level' and 'spacesPerLevel', and append the result to
        // the specified 'buffer'.  When 'level' is positive, indent the
        // table by ('level' * 'spacesPerLevel') spaces.  A negative
        // 'level' suppresses indentation.  The behavior is undefined if
        // 0 == 'buffer' or 'spacesPerLevel' < 0.  Note that this method is
        // similar to 'generateTable' except that no trailing null is ever
        // removed or supplied.

};

// ============================================================================
//                      INLINE FUNCTION DEFINITIONS
// ============================================================================

                        // -----------------------
                        // struct bdempu_Aggregate
                        // -----------------------

// CLASS METHODS
inline
void bdempu_Aggregate::generateList(bsl::vector<char> *buffer,
                                    const bdem_List&   value,
                                    int                level,
                                    int                spacesPerLevel)
{
    bdepu_NTypesParser::stripNull(buffer);
    generateListRaw(buffer, value, level, spacesPerLevel);
    buffer->push_back('\0');
}

inline
void bdempu_Aggregate::generateTable(bsl::vector<char>  *buffer,
                                     const bdem_Table&   value,
                                     int                 level,
                                     int                 spacesPerLevel)
{
    bdepu_NTypesParser::stripNull(buffer);
    generateTableRaw(buffer, value, level, spacesPerLevel);
    buffer->push_back('\0');
}

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2002
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
