// bdempu_schemaaggregate.h     -*-C++-*-
#ifndef INCLUDED_BDEMPU_SCHEMAAGGREGATE
#define INCLUDED_BDEMPU_SCHEMAAGGREGATE

//@PURPOSE: Parse/generate a combined schema and the data it constrains.
//
//@CLASSES:
//  bdempu_SchemaAggregate: namespace for "parse" and "generate" functions
//
//@AUTHOR: John Lakos (jlakos)
//
//@DESCRIPTION: This component provides a suite of pure procedures for parsing
// and generating textual representations of lists and tables constrained by
// record definitions.  In particular, the scope mechanism provides a
// convenient and effective way to manage hierarchical name/value pairs that
// is especially useful for maintaining configuration files (see 'parseScope').
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
//   -     Within brackets the minus means through. For example, [a-z] is
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
///PRODUCTION RULES FOR PARSING FUNCTIONS
///--------------------------------------
//..
// <WHITESPACE>  ::= any character for which isspace(char) returns 'true', or
//                   any legal C- or C++-style comment.
// <NONWHITESPACE> ::= any character for which isspace(char) returns 'false'.
// <EOL>          ::= End of line, or ASCII 0.
// <SCOPE_VALUE>     ::= '{' <WHITESPACE>* <SCHEMA> <WHITESPACE>* <DATA>
//                           <WHITESPACE>* '}'
// <SCHEMA>          ::= "SCHEMA" <WHITESPACE>* <SCHEMA_VALUE>
// <DATA>            ::= "DATA" <WHITESPACE>* <CONSTRAINT> <WHITESPACE>*
//                       <COMPLIANT_RECORD>
// <CONSTRAINT>      ::=  '<' <WHITESPACE>* <LOCATOR> <WHITESPACE>* '>'
// <LOCATOR>         ::= <NAME>
//                     | <INTEGER_LOCATOR>
// <NAME>            ::= <IDENTIFIER>
//                     | <QUOTED_STRING>
// <INTEGER_LOCATOR> ::= '{' <WHITESPACE>* <INDEX> <WHITESPACE>* '}'
// <IDENTIFIER>      ::= C-style identifier: "[A-Za-z_][A-Za-z0-9_]*"
// <QUOTED_STRING>   ::= C-style string literal with escape sequences
// <INDEX>           ::= non-negative integer <= INT_MAX: "[0-9]+"
// <RECORD_VALUE>    ::= <COMPLIANT_RECORD>
//                      | <NULL>
// <COMPLIANT_RECORD ::= '{' <ASSOCIATIONS>? '}'
// <ASSOCIATIONS>    ::= ( <ASSOCIATION> <WHITESPACE>+ )*
// <ASSOCIATION>     ::= <FIELD_LOCATOR> <WHITESPACE>* '=' <WHITESPACE>*
//                        <VALUE>
// <FIELD_LOCATOR>   ::= <NAME>
//                      | <INTEGER_LOCATOR>
// <NAME>            ::= <IDENTIFIER>
//                      | <QUOTED_STRING>
// <INTEGER_LOCATOR> ::= '{' <WHITESPACE>* <INDEX> <WHITESPACE>* '}'
// <IDENTIFIER>      ::= C-style identifier: "[A-Za-z_][A-Za-z0-9_]*"
// <QUOTED_STRING>   ::= C-style string literal with escape sequences
// <INDEX>           ::= non-negative integer <= INT_MAX: "[0-9]+"
// <VALUE>           ::=     <CHAR_VALUE>  |     <CHAR_ARRAY_VALUE>
//                      |    <SHORT_VALUE>  |    <SHORT_ARRAY_VALUE>
//                      |      <INT_VALUE>  |      <INT_ARRAY_VALUE>
//                      |    <INT64_VALUE>  |    <INT64_ARRAY_VALUE>
//                      |    <FLOAT_VALUE>  |    <FLOAT_ARRAY_VALUE>
//                      |   <DOUBLE_VALUE>  |   <DOUBLE_ARRAY_VALUE>
//                      |   <STRING_VALUE>  |   <STRING_ARRAY_VALUE>
//                      | <DATETIME_VALUE>  | <DATETIME_ARRAY_VALUE>
//                      |     <DATE_VALUE>  |     <DATE_ARRAY_VALUE>
//                      |     <TIME_VALUE>  |     <TIME_ARRAY_VALUE>
//                      |     <LIST_VALUE>  |          <TABLE_VALUE>
//                      |   <RECORD_VALUE>  |   <RECORD_ARRAY_VALUE>
// <NULL>            ::= [Nn][Uu][Ll][Ll]
// <RECORD_ARRAY_VALUE>     ::= <COMPLIANT_RECORD_ARRAY>
//                            | <NULL>
// <COMPLIANT_RECORD_ARRAY> ::= '{' <WHITESPACE>* <COMPLIANT_RECORD>*
//                              <WHITESPACE>* '}'
//                            | '{' <WHITESPACE>* <COLUMN_FORMAT>
//                               <WHITESPACE>* '}'
// <COLUMN_FORMAT>   ::= <COLUMN_MAP> <WHITESPACE>* <DATA_ROW>*
// <COLUMN_MAP>      ::= '[' <WHITESPACE>* <FIELD_LOCATORS>? ']'
// <DATA_ROW>        ::= '{' <WHITESPACE>* <VALUES>? <WHITESPACE>* '}'
// <VALUES>          ::= ( <VALUE> <WHITESPACE>+ )*
// <FIELD_LOCATORS>  ::= ( <FIELD_LOCATOR> <WHITESPACE>* )*
//..
///EXAMPLE - Scope
///---------------
// The 'bdem_aggregate' component defines a set of three high-performance,
// hierarchical data objects, 'bdem_Row', 'bdem_List', and 'bdem_Table', each
// of which can be examined to determine the type and value of any of its
// contained elements.  (These data types are fully described in ../bdem/
// bdem_aggregate.h.)
//
// As container structures, both 'bdem_List' and 'bdem_Table' can hold
// heterogeneous collections of any legitimate 'bdem' type.  The items
// can be added to the collection in whatever order the user desires (except
// that table rows must conform to the regular, explicitly declared row
// structure, also defined by the user.)
//
// There are times when application data must be stored in a particular
// format, for example when two procedures need to exchange complex
// (structured) information and must adhere to a common representation.
// A schema ('bdem_Schema') is a value-semantic type that supports the
// definition of hierarchical record types that allow us to constrain the
// type and order of LIST and TABLE entries.  Constraining the
// number and sequence of element types in a LIST (or column types in a TABLE)
// enables the representation of data in a user-definable hierarchical type
// system.  (Schema are fully described in ../bdem/schema.h.)
//
// A scope is defined to be a triple consisting of a (non-empty) schema, a
// (non-negative) index indicating the record type within the schema that will
// be used as the top-level constraint, and a (possibly empty) LIST that
// conforms to that constraint (as defined by the 'isListConformant'
// function in 'bdem_schemaaggregateutil').
//..
// /* Sample scope containing 4 record types and 18 data fields */
//  {                                                           // ORDINATES
//      SCHEMA {                                                // Rec.field
//          RECORD Point {                                      // {0}
//              INT x;                                          // {0}.{0}
//              INT y;                                          // {0}.{1}
//              // DOUBLE weight;  /* might be added later */   // {0}.{2}
//          }
//          RECORD Rectangle {                                  // {1}
//              RECORD<Point> upperLeft;                        // {1}.{0}
//              RECORD<Point> lowerRight;                       // {1}.{1}
//              STRING        color;                            // {1}.{2}
//          }
//          RECORD Polygon {                                    // {2}
//              RECORD_ARRAY<Point> vertices;                   // {2}.{0}
//              STRING              color;                      // {2}.{1}
//              LIST                /* no field name */;        // {2}.{2}
//              TABLE               ;                           // {2}.{3}
//          }
//          RECORD Config {                                     // {3}
//              INT                   maxQueueLength;           // {3}.{0}
//              DOUBLE                timeoutPeriod;            // {3}.{1}
//              STRING                loginName;                // {3}.{2}
//              DATETIME              expiry;                   // {3}.{3}
//              DATE                  startDate;                // {3}.{4}
//              TIME                  nightlyBuildTime;         // {3}.{5}
//
//              INT_ARRAY             portNumbers;              // {3}.{6}
//
//              LIST                  arbitraryListData;        // {3}.{7}
//
//              TABLE                 arbitraryTableData;       // {3}.{8}
//
//              RECORD<Point>         originAlpha;              // {3}.{9}
//              RECORD<Point>         originBeta;               // {3}.{10}
//              RECORD<Point>         originGamma;              // {3}.{11}
//
//              RECORD<Rectangle>     redBox;                   // {3}.{12}
//              RECORD<Rectangle>     greenBox;                 // {3}.{13}
//
//              RECORD<Polygon>       shape1;                   // {3}.{14}
//              RECORD<Polygon>       shape2;                   // {3}.{15}
//              RECORD<Polygon>       shape3;                   // {3}.{16}
//
//              RECORD_ARRAY<Polygon> complexShape;             // {3}.{17}
//          }
//      } // SCHEMA
//      DATA<Config> {
//          maxQueueLength     = 16                             // field {0}
//
//          loginName          = "Mike Bloomberg"               // field {2}
//          timeoutPeriod      = 2.5 /* seconds */              // field {1}
//
//          {3}                = 2038/12/31 18:45:00.000        // field {3}
//          {5}                = 21:00:00.000                   // field {5}
//          {4}                = 1997/01/26                     // field {4}
//
//          portNumbers        = [ 2740  2741  2742  2743 ]     // field {6}
//
//          arbitraryListData  = { STRING "integer"  INT 123 }  // field {7}
//
//          arbitraryTableData = {                              // field {8}
//            [        INT           DOUBLE  STRING  INT_ARRAY  LIST          ]
//            {         16           123.50   "foo"  [ 2, 1 ]   { INT 123 }   }
//            {         32           345.75      ""  [ 0 ]      { DATE 1/1/1} }
//            {-2147483648 -281474976710655  "~`~`~" [ ]        { }           }
//            {       NULL           NULl     NUll   Null       null          }
//          }
//
//          originAlpha  = { "y" = 2 "x" = 5 }                  // field {9}
//          "originBeta" = { {1} = 2 {0} = 5 }                  // field {10}
//          originGamma  = { y = 1 x = 5  /* repeat */ y = 2 }  // field {11}
//
//          redBox = {                                          // field {12}
//              upperLeft = {
//                  x = 1
//                  y = NULL       // unneccessary -- omitting y entirely
//              }                  // would make its value NULL (the default)
//              lowerRight = {
//                  x = 65
//                  y = 80
//              }
//              color = "RED"
//          }
//          greenBox = {                                        // field {13}
//              lowerRight = {          // required -- Both x and y are unset.
//              }
//              upperLeft = NULL
//              color = "GREEN"
//          }
//
//          shape1 = {                                          // field {14}
//              vertices = {
//                  {                           // row 0
//                      x = 1
//                      y = 4
//                  }
//                  {                           // row 1
//                      x = 3
//                  }
//                  {                           // row 2
//                      {0} = 2
//                      {1} = 5
//                  }
//              }
//              color = "BLUE"
//          }
//          shape2 = {                                          // field {15}
//              {1} = "BLUE"
//              {0} = {
//                  [ x y ]
//                  { 1 4 }
//                  { 3 NULL }
//                  { 2 5 }
//              }
//          },
//          shape3 = {                                          // field {16}
//              {3} = NULL // unneccessary
//              {2} = NULL // unneccessary
//              color = "BLUE",
//              vertices = {
//                  [ y x {1} ]   // repeated 'y' field locator
//                  { 7 1 4 }
//                  { 9 3 NULL }
//                  { 8 2 5 }
//              }
//          },
//          complexShape = {                                    // field {17}
//            [                  {2} vertices                                ]
//            { { STRING "outside" } { [x y]{1 1}{1 4}{4 4}{4 1} }           }
//            { { STRING "tabs"    } { [ x ]{ 8 }{ 16 }{ 24 }{ 32 } }        }
//            { { STRING "odd rows"} { [ y ]{ 1 }{ 3 }{ 5 }{ 7 }{ 9 }}       }
//            { { STRING_ARRAY [0] } { { x = 1  y = 2 }{ x = 1 }{ y = 2 }{ }}}
//            { { DATE 9999/12/31  } { }                                     }
//            { { TIME 24:00:00.000} { [ ] }                                 }
//            { { TIME NULL        } NULL                                    }
//          }
//      } // DATA
//  } // SCOPE
//..
// Note that the spacing and tabbing in the scope text above is optional;
// the parser is completely whitespace-neutral.  For readability in
// configuration files, we do recommend aligning table columns and placing
// large list elements on separate rows.
//
// In the above example of a '<SCOPE_VALUE>' (as defined by the 'parseScope'
// function), the SCHEMA section describes four record types.
//..
//          RECORD Point {                                      // {0}
//              INT x;                                          // {0}.{0}
//              INT y;                                          // {0}.{1}
//              // DOUBLE weight;  /* might be added later */   // {0}.{2}
//          }
//..
// The record definition at index 0, named "Point", contains two integer
// fields named "x" and "y". A third field of type DOUBLE called "weight" is
// proposed.  Note that !appending! this new field (as shown) will have no
// effect on the rest of the scope except to enable subsequent use of the
// field.  !Inserting! the new field, say, between "x" and "y" would affect
// positional references to "y" (e.g., {1}), but not named references (e.g.,
// "y").  (Note that data lists whose constraints are not explicitly listed,
// such as { 1 2 } rather than { x = 1  y = 2 } are implicitly positional.)
// All references to field {0}, also referrable to as "x", would remain
// unaffected.
//..
//          RECORD Rectangle {                                  // {1}
//              RECORD<Point> upperLeft;                        // {1}.{0}
//              RECORD<Point> lowerRight;                       // {1}.{1}
//              STRING        color;                            // {1}.{2}
//          }
//..
// The record definition at index 1, named "Rectangle", contains two
// user-defined "Point" fields named, respectively, "upperLeft" and
// "lowerRight".  The data for each "Point" record will be represented by a
// LIST object whose elements are constrained to have the sequence of types
// defined in "Point".  A third field named "color" is of terminal type STRING.
// (Terminal means that it is not a LIST or a TABLE, i.e., it contains no
// internal substructure.)
//..
//          RECORD Polygon {                                    // {2}
//              RECORD_ARRAY<Point> vertices;                   // {2}.{0}
//              STRING              color;                      // {2}.{1}
//              LIST                /* no field name */;        // {2}.{2}
//              TABLE               ;                           // {2}.{3}
//          }
//..
// The record definition at index 2, named "Polygon", contains an
// array-of-"Point" fields named "vertices".  The "Point" record array data
// will be represented by a TABLE object whose columns are constrained to
// have the sequence of types defined "Point".  Also defined in "Polygon"
// is a STRING field named "color".  These two fields are followed by two
// unnamed fields of type (unconstrained) LIST and (unconstrained) TABLE,
// which (because they have no name) can be referred to only via their
// respective integer field locators, {2} and {3}.
//..
//          RECORD Config {                                     // {3}
//              INT                   maxQueueLength;           // {3}.{0}
//              DOUBLE                timeoutPeriod;            // {3}.{1}
//              STRING                loginName;                // {3}.{2}
//              DATETIME              expiry;                   // {3}.{3}
//              DATE                  startDate;                // {3}.{4}
//              TIME                  nightlyBuildTime;         // {3}.{5}
//
//              INT_ARRAY             portNumbers;              // {3}.{6}
//
//              LIST                  arbitraryListData;        // {3}.{7}
//              TABLE                 arbitraryTableData;       // {3}.{8}
//
//              RECORD<Point>         originAlpha;              // {3}.{9}
//              RECORD<Point>         originBeta;               // {3}.{10}
//              RECORD<Point>         originGamma;              // {3}.{11}
//              RECORD<Rectangle>     redBox;                   // {3}.{12}
//              RECORD<Rectangle>     greenBox;                 // {3}.{13}
//              RECORD<Polygon>       shape1;                   // {3}.{14}
//              RECORD<Polygon>       shape2;                   // {3}.{15}
//              RECORD<Polygon>       shape3;                   // {3}.{16}
//
//              RECORD_ARRAY<Polygon> complexShape;             // {3}.{17}
//          }
//..
// The record definition at index 3,  named "Config", defines a sequence of
// 18 fields.  For the purpose of example, these fields illustrate the proper
// syntax for various field types, including six common scalar types ({0} -
// {5}), a representative INT_ARRAY field ({6}), unconstrained LIST and TABLE
// fields ({7} - {8}), a variety of user-defined RECORD fields [i.e., a
// constrained list] ({9} - {16}), and a user-defined RECORD_ARRAY field
// [i.e., a constrained TABLE] ({17}).  This record will be selected to
// constrain the LIST ('bdem_List') object used to represent the entire DATA
// portion of this scope.
//..
//      DATA<Config> {
//..
// The DATA section contains the information requiring parsing, and must be
// constrained (either by name or index) to conform to some record type
// (!not! necessarily the last one) in the SCHEMA section.  In this example,
// the data is constrained by the record type whose name is "Config" (i.e.,
// the record definition at index 3).
//..
//          INT maxQueueLength;                                 // {3}.{0}
//
//          maxQueueLength = 16                                 // field {0}
//..
// The first association assigns the INT field named "maxQueueLength" (which is
// field {0} in the constraining record type) to the literal value 16.  Note
// that "type" is defined entirely by the constraining field and not the value
// used to populate it.  Had field {0} been of type DOUBLE, 16 would have
// been interpreted as a literal double value.
//..
//          DOUBLE timeoutPeriod;                               // {3}.{1}
//          STRING loginName;                                   // {3}.{2}
//
//          loginName = "Mike Bloomberg"                        // field {2}
//          timeoutPeriod = 2.5 /* seconds */                   // field {1}
//..
// The next two associations assign STRING field {2}, named "loginName", the
// value "Mike Bloomberg" and DOUBLE field {1}, named "timeoutPeriod", the
// value 2.5 (C-style comments are ignored).  Note that the order of
// association need not follow the order of field definition in the
// constraining record definition, in lists which explicitly contain the
// constraint as well as the data.
//..
//          DATETIME expiry;                                    // {3}.{3}
//          DATE startDate;                                     // {3}.{4}
//          TIME nightlyBuildTime;                              // {3}.{5}
//
//          {3} = 2038/12/31 18:45:00.000                       // field {3}
//          {5} = 21:00:00.000                                  // field {5}
//          {4} = 1997/01/26                                    // field {4}
//..
// The next three associations assign DATETIME, DATE, and TIME values,
// respectively.  For illustration purposes, we have identified the fields by
// index rather than by name.  Note that failing to set a field is equivalent
// to setting it to the "unset" value appropriate for its type.
//..
//          INT_ARRAY portNumbers;                              // {3}.{6}
//
//          portNumbers = [ 2740  2741  2742  2743 ]            // field {6}
//..
// The next association assigns field {6} of type INT_ARRAY (and named
// "portNumbers") the array value of length 4 shown above.  Literal values for
// each of the other five array types (e.g., STRING_ARRAY) are also represented
// as whitespace-separated lists of corresponding scalar literals (enclosed in
// square brackets indicating homogeneity) -- e.g., [ "2740", "2741", "2742" ]
// (double-quotes are optional for strings containing no embedded whitespace.)
//..
//          LIST arbitraryListData;                             // {3}.{7}
//
//          arbitraryListData  = { STRING "integer"  INT 123 }  // field {7}
//..
// The next association assigns to field {7} a literal LIST value of arbitrary
// structure, which in this case is of length two containing a STRING element
// whose value is the word "integer" and an INT element whose value is 123.
// Note that the sub-elements of an unconstrained list are not associations,
// (that is, not "<something> = <something else>" constructs,) but instead
// mimic the type/value structure of the LIST itself.  The nested elements
// in the literal LIST could have been of any of the 22 basic element
// types defined in 'bdem_ElemType' including LIST and TABLE.  There are,
// however, no names associated with these elements.  Moreover these elements
// cannot be constrained by record types such as "Point" described in the
// SCHEMA section.  In order for an element to be constrained, the enclosing
// structure (i.e., RECORD or RECORD_ARRAY and not LIST or TABLE) must also be
// constrained, as is illustrated in fields ({9} - {17}).
//..
//          TABLE arbitraryTableData;                           // {3}.{8}
//
//          arbitraryTableData = {                              // field {8}
//            [        INT           DOUBLE  STRING  INT_ARRAY  LIST          ]
//            {         16           123.50   "foo"  [ 2, 1 ]   { INT 123 }   }
//            {         32           345.75      ""  [ 0 ]      { DATE 1/1/1} }
//            {-2147483648 -281474976710655 "~`~`~"  [ ]        { }           }
//            {       NULL           NULl    NUll    Null       null          }
//          }
//..
// Similarly, the next association assigns to field {8} a literal TABLE value,
// again of arbitrary structure.  The format for a literal table consists of
// an array of column types followed by any number of rows where the literal
// value of the i'th element in each row is parsable as the i'th column type.
// In this instance the table has 5 columns and 4 rows.  The five column types
// are indicated by the type array '[ INT DOUBLE STRING INT_ARRAY LIST ]'.
// The first row (row 0) contains typical values, none of which is unset.
// The next row (row 1) also contains five non-"unset" values.  The value of
// the STRING element at row 1 and column 2 is the empty string ("") which is
// !not! considered "unset".  Although a STRING cannot hold a C-style null
// (0 address value), a STRING_ARRAY element can.  Both arrays and lists must
// be of length 0 to be considered "unset".  The specific date value 1/1/1 at
// index position 0 in the list value at row 1 and column 4 of this table
// value is reserved to indicate the "unset" DATE value, and is equivalent to
// the NULL keyword in this context.  The last two rows (rows 2 and 3) are
// identical to each other, each containing five "unset" values.  Row 2
// defines its "unset" elements using obscure-but-valid literal values that
// are reserved for indicating "unset" for each respective column type; row 3
// employs the case-insensitive keyword NULL to achieve the same purpose.
//..
//          RECORD Point {                                      // {0}
//              INT x;                                          // {0}.{0}
//              INT y;                                          // {0}.{1}
//              // DOUBLE weight;  /* might be added later */   // {0}.{2}
//          }
//
//          RECORD<Point> originAlpha;                          // {3}.{9}
//          RECORD<Point> originBeta;                           // {3}.{10}
//          RECORD<Point> originGamma;                          // {3}.{11}
//
//          originAlpha  = { "y" = 2  "x" = 5 }                 // field {9}
//          "originBeta" = { {1} = 2  {0} = 5 }                 // field {10}
//          originGamma  = { y = 1  x = 5  /* repeat */ y = 2 } // field {11}
//..
// The next three associations illustrate various ways of assigning the same
// user-defined "Point" value to three consecutive fields ({9} - {11}).
// Fields {9}, {10}, and {11} are each of type RECORD<Point>, which means
// the corresponding data will be held in a list ('bdem_List') whose
// element types are constrained by the field types in the "Point" record type
// defined in the SCHEMA section.  Field {9}, "originAlpha", is assigned the
// coordinates (5, 2) in reverse order using the field names provided in the
// "Point" record type.  Field {10}, "originBeta", is also assigned the
// coordinates (5, 2) in reverse order, but using integer locators instead of
// the (optional) field names provided in the record type.   Notice that
// quotes around field names (as well as record-type names) are optional,
// provided the name conforms to a valid C-style identifier (i.e., matching
// "[A-Za-z_][A-Za-z0-9_]*").  Field {11}, "originGamma", illustrates that
// name/value association for constrained lists (and for rows in constrained
// tables) may not be 1-to-1.  If a field is not associated with a value (as
// would be the case if the "weight" field were added to "Point"), that
// element defaults to the "unset" value appropriate for its field type.
// Repeating an association, on the other hand, will result in one of two
// implementation-dependent (but consistent) behaviors: either (1) the
// repeated association results in a parse failure for a given implementation,
// or (2) the value of the last association will prevail.  Notice that C-style
// comments are ignored.
//..
//          RECORD Rectangle {                                  // {1}
//              RECORD<Point> upperLeft;                        // {1}.{0}
//              RECORD<Point> lowerRight;                       // {1}.{1}
//              STRING        color;                            // {1}.{2}
//          }
//
//          RECORD<Rectangle> redBox;                           // {3}.{12}
//          RECORD<Rectangle> greenBox;                         // {3}.{13}
//
//          redBox = {                                          // field {12}
//              upperLeft = {
//                  x = 1
//                  y = NULL            // unneccessary - default value
//              }
//              lowerRight = {
//                  x = 65
//                  y = 80
//              }
//              color = "RED"
//          }
//          greenBox = {                                        // field {13}
//              lowerRight = {          // required -- Both x and y are unset.
//              }
//              upperLeft = NULL        // unneccessary - default value
//              color = "GREEN"
//          }
//..
// The next two associations illustrate how the values of hierarchical types
// are described, particularly when some of the information is unspecified
// (i.e, "unset").  Field {12}, "redBox", is of type RECORD<Rectangle>.  A
// rectangle consists of a "color" of type STRING and two "Point" records
// ("upperLeft" and "lowerRight") each of which holds two integer coordinates
// ("x" and "y").  Notice that the "upperLeft" corner of "redBox" is only
// partially specified, since the "y" coordinate is explicitly "unset".
// Omitting the redundant association does not alter the value assigned
// to "redBox" since the unspecified "y" field will automatically default
// to its corresponding unset value.
//
// Field {13}, "greenBox", emphasizes the difference between the empty
// association, { }, and the null association, NULL, for constrained lists.
// (This difference applies for tables as well.)  The "lowerRight" sub-list
// value will contain two "unset" integers, while the "upperLeft" sub-list
// will itself be "unset" (and therefore contain nothing at all).  Note that
// removing the association for "upperLeft" leaves the value of "greenBox"
// unchanged.
//..
//          RECORD Polygon {                                    // {2}
//              RECORD_ARRAY<Point> vertices;                   // {2}.{0}
//              STRING              color;                      // {2}.{1}
//              LIST                /* no field name */;        // {2}.{2}
//              TABLE               ;                           // {2}.{3}
//          }
//
//          RECORD<Polygon> shape1;                             // {3}.{14}
//          RECORD<Polygon> shape2;                             // {3}.{15}
//          RECORD<Polygon> shape3;                             // {3}.{16}
//
//          shape1 = {                                          // field {14}
//              vertices = {
//                  {                           // row 0
//                      x = 1
//                      y = 4
//                  }
//                  {                           // row 1
//                      x = 3
//                  }
//                  {                           // row 2
//                      {0} = 2
//                      {1} = 5
//                  }
//              }
//              color = "BLUE"
//          }
//          shape2 = {                                          // field {15}
//              {1} = "BLUE"
//              {0} = {
//                  [ x y ]                     // column order made explicit
//                  { 1 4 }
//                  { 3 NULL }
//                  { 2 5 }
//              }
//          shape3 = {                                          // field {16}
//              {3} = NULL
//              {2} = NULL
//              color = "BLUE"
//              vertices = {
//                  [ y x {1} ]                 // repeated 'y' field locator
//                  { 7 1 4 }
//                  { 9 3 NULL }
//                  { 8 2 5 }
//              }
//          }
//..
// The next three associations illustrate various ways of assigning the same
// user-defined "Polygon" value (containing tabular data) to three consecutive
// fields ({14} - {16}).  A polygon consists of a "color" STRING and a
// "vertices" table whose columns are constrained to conform to record type
// "Point".  Two additional unnamed fields of type LIST and TABLE are provided
// purely for illustration purposes.  Field {14} ("shape1") describes its
// vertices as a sequence of rows in which the association for each individual
// coordinate is explicit (unless it is "unset").  Row 0 refers to the "x" and
// "y" coordinates by name.  Row 1 refers only to coordinate "x" (again by
// name) allowing coordinate "y" to default to the "unset" value for type INT.
// Row 2 refers to both coordinates positionally.  The row-association format
// for constrained tables is useful when the table is sparse (and data appears
// in most columns).  Note that unlike a LIST, a row itself may not be "unset".
//
// Field {15} ("shape2") illustrates the other table data format,
// column-mapped, in which the field associations are made on a per-column
// basis initially (e.g., [ "x", "y" ] or equivalently [ {0}, {1} ]), and only
// those values (including "unset" values) corresponding to the mapped columns
// appear in each row.  Note, however, that adding even a single value to an
// unmapped column in this format will affect the textual representation of
// every row in the table.
//
// Field {16} ("shape3") serves to illustrate the implementation-dependent
// behavior of repeated field locators in a column map.  As with repeated
// associations in a constrained list, if the text does not result in a parse
// failure, the last (i.e., right-most bottom-most) column association
// prevails.  In particular, the "y" coordinate in row 1 of the "vertices"
// table will be "unset", not 9.  Note also that removing the redundant NULL
// associations for the unnamed LIST and TABLE fields will allow them to
// silently default to "unset" as they did in previous two fields, "shape1"
// and "shape2" (14-15), with no change in value.
//..
//          RECORD_ARRAY<Polygon> complexShape;                 // {3}.{17}
//
//          complexShape = {                                    // field {17}
//            [                  {2}, vertices                                ]
//            { { STRING "outside" }, { [x,y]{1,1}{1,4}{4,4}{4,1} }           }
//            { { STRING "tabs"    }, { [ x ]{ 8 }{ 16 }{ 24 }{ 32 } }        }
//            { { STRING "odd rows"}, { [ y ]{ 1 }{ 3 }{ 5 }{ 7 }{ 9 }}       }
//            { { STRING_ARRAY [0] }, { { x = 1, y = 2 }{ x = 1 }{ y = 2 }{ }}}
//            { { DATE 9999/12/31  }, { }                                     }
//            { { TIME 24:00:00.000}, { [ ] }                                 }
//            { { TIME NULL        }, NULL                                    }
//          }
//..
// Finally, field {17} ("complexShape") illustrates a concise representation of
// a table constrained by a record type that itself contains a constrained-
// table field.  In this example we use the mapped-column format to populate
// two of the four fields in the constraining "Polygon" type.  The first field
// (mapped column 0) is constrained by the unnamed, unconstrained LIST field,
// field {2}, and the second field (mapped column 1) is constrained by the
// "vertices" table field, field {0}, which is itself constrained by the record
// type "Point".  The column map [ {2} "vertices" ] defines the expected types
// of the literal values in the outermost tabular representation.  Notice that
// each unconstrained LIST value in mapped column 0 (which refers to field {2})
// has its own unique structure.  Of particular interest is the LIST entry at
// row 3, column 0, which has a STRING_ARRAY of length 1 as its only element.
// Notice that the literal value of this array element is 0, representing a
// C-style null string (i.e., a 0 address value) which could not have been
// represented by a LIST element of type STRING.
//
// Each table element in the "vertices" column (which maps to field {0} in the
// constraining "Polygon" record type) can be described in either table
// format.  The column-mapped format in row 0, maps both "x" and "y" fields
// and describes a rectangle with diagonally opposite corners at (1,1) and
// (4,4).  In row 1, only the "x" coordinates are mapped, leaving all of the
// "y" coordinates (and "weight" values when added to the "Point" record type)
// "unset".  Similarly, row 2, maps only "y" values leaving all others
// "unset".  Row 3 employs the other tabular format (row-association) in which
// each coordinate in each row is set only if needed.  The resulting vertices
// are (1, 2), (1, NULL), (NULL, 2), and (NULL, NULL).  Row 4 describes a
// table (in row-association format) with no rows (i.e., a "Polygon" with no
// vertices).  The table element itself (i.e., the "vertices" field) is not
// "unset" since the table is constrained to have (at least) two columns
// (i.e., "x" and "y").  Similarly, the table element described in row 5 (in
// mapped-column format) has no rows, yet inherits the two columns "x" and "y"
// from the constraining record type.  Note however that the meaning of NULL
// for the constrained table entry in row 6 is quite different; here the
// vertices entry itself is set to contain 0 columns and 0 rows, and hence is
// itself "unset".  Remember that for constrained lists and tables, { } and {
// [ ] } do not mean the same thing as NULL (except for the pathological case
// where the constraining record type contains no fields.
//
///USAGE
///-----
// The following sample program illustrates a utility function that validates
// the contents of a configuration file read from a stream and populates a
// 'bdem_List' object if the configuration file data is conformant with a
// well-known 'bdem_Schema' object.  If the data is not conformant with the
// schema, there is a parse error, and an appropriate diagnostic error message
// is written to 'stderr'.
//
// First, we assume the existence of a suite of stream and file reading and
// writing utilities such as 'my_StreamUtil':
//..
//    struct my_IoUtil {
//        // This class provides a namespace for a set of pure procedures on
//        // streams and files.  In all cases, the data buffer is a
//        // 'std::vector<char>' object.
//
//        // CLASS METHODS
//        static
//        int readFile(std::vector<char> *buffer, const char *file);
//            // Load into the specified 'buffer' the contents of the the
//            // specified 'file' and return the number of characters read
//            // into 'buffer'.
//
//        static
//        int readStream(std::vector<char> *buffer, std::istream& stream);
//            // Load into the specified 'buffer' the contents of the the
//            // specified 'stream' and return the number of characters read
//            // into 'buffer'.
//
//        static
//        int writeStream(std::ostream&            stream,
//                        const std::vector<char>& buffer);
//            // Write to the specified 'stream' the contents of the the
//            // specified 'buffer' and return the number of bytes written.
//
//        static
//        int writeStream(std::ostream&           stream,
//                        const std::vector<char>& buffer,
//                        int                     startIndex,
//                        int                     numChars);
//            // Write to the specified 'stream' the specified 'numChars' from
//            // the the specified 'buffer', starting at the specified
//            // 'startIndex, and return the number of bytes written.
//    };
//..
// We use the above I/O utilities and the 'parseConstrainedList' method to
// write an application that validates the input text, and reports a
// useful diagnostic error message to a specified 'errorStream' in case of
// error.
//..
//    // my_parseutil.h
//
//    struct my_ParseUtil {
//        // Namespace for pure procedures that parse and validate text
//        // representations of 'bdem' objects.
//
//        // CLASS METHODS
//        int
//        validateAndLoadConstrainedList(
//                                       bdem_List              *list,
//                                       std::ostream&           errorStream,
//                                       const std::vector<char>& inputBuffer,
//                                       const bdem_Schema&      schema,
//                                       int                     recordIndex);
//        // Parse the ASCII representation of a constrained list from the
//        // specified 'inputBuffer', using the record definition identified
//        // by the specified 'recordIndex' in the specified 'schema' as the
//        // constraint, and load the specified 'list' with the parsed value
//        // if there is no parse error.  Return 0 on successful parsing, or
//        // the line number of the first syntax error.  In the event of a
//        // parse error, format an error message containing the line number,
//        // text line, and an indication of the character causing the parse
//        // error to the specified 'errorStream'.  Note that the content of
//        // 'list' is undefined in the event of a parse error.
//    };
//
//    // my_parseutil.cpp
//
//    static
//    int getLineNumber(int                     *lineStartIndex,
//                      int                     *lineStopIndex,
//                      const std::vector<char>&  buffer,
//                      int                      index) {
//        // Load into the specified 'lineStartIndex' and 'lineStopIndex',
//        // respectively, the indices of the first and last characters in the
//        // line containing the character at the specified 'index'.  Return
//        // the line number of this line.  The behavior is undefined unless
//        // 0 <= index < buffer.length().  Note that the line number starts
//        // at 1, and that the newline character is the interior delimiter of
//        // lines of text, but that the specified 'buffer' need neither begin
//        // nor end with a newline.
//
//        assert(0 <= index);
//        assert(index < buffer.length());
//
//        int lineNumber = 1;
//        int length     = buffer.length();
//
//        *lineStartIndex = 0;
//        for (int i = 0; i <= index; ++i) {
//            if ('\n' == buffer[i]) {
//                ++lineNumber;
//                *lineStartIndex = i + 1;
//            }
//        }
//
//        // find end of the line containing the parse error
//        int eol = index;
//        while (eol < length && buffer[eol] != '\n' && buffer[eol] != '\0') {
//            ++eol;
//        }
//        *lineStopIndex = eol;
//
//        return lineNumber;
//    }
//
//    int my_ParseUtil::validateAndLoadConstrainedList(
//                                     bdem_List              *list,
//                                     std::ostream&           stream,
//                                     std::ostream&           errorStream,
//                                     const std::vector<char>& inputBuffer,
//                                     const bdem_Schema&      schema,
//                                     int                     recordIndex) {
//        assert(list);
//
//        const bdem_RecordDef *constraint = &schema.record(recordIndex);
//        const char           *inputText  = inputBuffer.data();
//        const char           *endPos;
//        int                   errorLineStart;
//        int                   errorLineStop;
//
//        // Parse the constrained list
//        int result = bdempu_SchemaAggregate::parseConstrainedList(
//                                      &endPos, list, inputText, *constraint);
//
//        // Display errors, if any
//        if(0 != result)
//        {
//            result = getLineNumber(&errorLineStart, &errorLineStop,
//                                   inputBuffer, endPos - inputText);
//
//            errorStream << "Parse error on line " << result << std::endl;
//            bdetoy_StreamUtil::writeStream(errorStream, inputBuffer,
//                             errorLineStart, errorLineStop - errorLineStart);
//            errorStream << std::endl;
//
//            int indicatorLength = endPos - inputText - errorLineStart + 1;
//            std::vector<char> indicator(indicatorLength, '.');
//            indicator[indicatorLength - 1] = '^';
//            bdetoy_StreamUtil::writeStream(errorStream, indicator);
//            errorStream << std::endl;
//
//            return result;
//        }
//        else {
//            return SUCCESS;
//        }
//    }
//..
// We can use the above utility code to populate a constrained list from a
// user-supplied configuration file containing the list data, but not the
// schema.  This convenient usage pattern assumes the existence of a well-known
// schema having the constraint (i.e., record definition) at a well-known index
// position.  We illustrate this pattern with a simple example.  We first read
// a textual representation of the schema from a well-known file (whose file
// name is hard-coded for simplicity).  The constraint index is obtained from
// a global 'int' value ('globalConstraintIndex').  (These values might better
// be obtained from command line arguments in a more sophisticated example).
// We parse the schema text using 'bdempu_Schema::parseSchema' to get a
// 'bdem_Schema' object 'schema', and then read the textual representation of
// the constrained-list data from 'stdin'.  We next use our
// 'validateAndLoadConstrainedList' utility function, along with 'schema' and
// 'constraintIndex', to validate the configuration-file data, and load a
// 'bdem_List' on success.  Finally, we can do something interesting with our
// valid constrained list value within the last 'if' block.
//..
//     using namespace std;
//     int main()
//     {
//         bdem_Schema       schema;
//         int               constraintIndex = globalConstraintIndex;
//         const char       *schemaFile      = "myWellKnownSchema";
//         std::vector<char>  schemaBuffer;
//
//         bdem_List         list;
//         std::vector<char>  listBuffer;
//
//         const char       *endPos;
//         int               status;
//
//         my_IoUtil::readFile(&schemaBuffer, schemaFile);
//         status = bdempu_Schema::parseSchema(&endPos, &schema,
//                                             schemaBuffer.data());
//
//         if (0 != status) {
//             cerr << "*** Error parsing schema." << endl;
//         }
//
//         if (0 == status) {
//             my_IoUtil::readStream(&listBuffer, cin);
//
//             status = my_ParseUtil::validateAndLoadConstrainedList(
//                          &list, cerr, listBuffer, schema, constraintIndex);
//         }
//
//         if (0 == status) {
//             // Do something useful with 'list'.
//         }
//
//         return status;
//     }
//..

#ifndef INCLUDED_BDESCM_VERSION
#include <bdescm_version.h>
#endif

#ifndef INCLUDED_BDEMPU_SCHEMA
#include <bdempu_schema.h>
#endif

#ifndef INCLUDED_BDEPU_PARSERIMPUTIL
#include <bdepu_parserimputil.h>
#endif

#ifndef INCLUDED_BDEPU_TYPESPARSER
#include <bdepu_typesparser.h>
#endif

#ifndef INCLUDED_BDEM_AGGREGATE
#include <bdem_aggregate.h>
#endif

#ifndef INCLUDED_BDEM_SCHEMA
#include <bdem_schema.h>
#endif

#ifndef INCLUDED_VECTOR
#include <vector>
#define INCLUDED_VECTOR
#endif


namespace BloombergLP {



                        // ======================
                        // bdempu_SchemaAggregate
                        // ======================

struct bdempu_SchemaAggregate {
    // This class provides a namespace for pure procedures that parse and
    // generate textual representations of objects of type 'bdem_List'
    // (RECORD) and 'bdem_Table' (RECORD_ARRAY), each of which is constrained
    // by a specified record definition in a given schema.  For convenience,
    // we provide a "parse" and "generate" function for a composite textual
    // representation known as a SCOPE (consisting of a schema, a constraint
    // identifier, and data in the form of a constrained list) is also
    // provided.  C- and C++-style comments along with character-string-
    // literal escape sequences are fully supported.  Whitespace, except
    // within quoted strings, is ignored.
    //
    // Note that the result of attempting to parse repeated
    // associations (e.g., '{ x = 1, y = 2, x = 3 }') or repeated fields in a
    // column map (e.g., '[ x, y, x ]') is implementation dependent.  If it is
    // not a parse failure, the last (i.e., right-most, bottom-most)
    // association prevails.

  private:
    static void generateConstrainedRowElement(
                                       std::vector<char>      *buffer,
                                       const bdem_Row&        value,
                                       const bdem_RecordDef&  constraint,
                                       int                    index,
                                       int                    level,
                                       int                    spacesPerLevel);
        // Format the element of specified 'value' as text, subject to the
        // specified 'constraint' record definition -----
        // 'index', constrained by the specified 'constraint' using the
        // specified indentation 'level' and append the result to the
        // specified 'buffer'.  Making 'level' negative suppresses indentation
        // for the first line only.  The behavior is undefined unless
        // 0 != buffer, 0 <= spacesPerLevel, index is not one of the elements
        // of the 'value' or the type of the element pointed to it does not
        // match the 'constraint'.

    static void generateConstrainedRow(
                                std::vector<char>     *buffer,
                                const bdem_Row&        value,
                                const bdem_RecordDef&  constraint,
                                int                    level = 0,
                                int                    spacesPerLevel = 4);
        // Format the specified row 'value' as text, subject to the specified
        // 'constraint' record definition, using the (absolute value of) the
        // optionally specified indentation 'level', and append the result
        // to the specified 'buffer while maintaining null-character
        // termination.  If 'level' is specified, optionally specify
        // 'spacesPerLevel', the non-negative number of spaces per
        // indentation level for this object.  Making 'level' negative
        // suppresses indentation for the first line only.  The behavior is
        // undefined unless 0 != buffer, 0 <= spacesPerLevel, and 'value' is
        // conformant to 'constraint' as defined by the 'isListConformant'
        // function in 'bdem_schemaaggregateutil'.  Note that 'value' is
        // output as a textual representation that is parsable by the
        // corresponding 'parseConstrainedList' function subject to (the same)
        // 'constraint'.

    static void generateConstrainedTable_h(
                                    std::vector<char>      *buffer,
                                    const bdem_Table&      value,
                                    const bdem_RecordDef&  constraint,
                                    int                    level = 0,
                                    int                    spacesPerLevel = 4);
        // Format the specified table 'value' as text, subject to the specified
        // 'constraint' record, using the (absolute value of) the optionally
        // specified indentation 'level', and append the result to the
        // specified 'buffer' while maintaining null-character termination.
        // If 'level' is specified, optionally specify 'spacesPerLevel', the
        // non-negative number of spaces per indentation level for this
        // object.  Making 'level' negative suppresses indentation for the
        // first line only.  The behavior is undefined unless 0 != buffer,
        // 0 <= spacesPerLevel, and 'value' is conformant to 'constraint'
        // as defined by the 'isTableConformant' function in
        // 'bdem_schemaaggregateutil'.  Note that 'value' is output as a
        // textual representation that is parsable by the corresponding
        // 'parseConstrainedTable' function subject to the same 'constraint'.

    static void initUnsetList(bdem_List             *result,
                              const bdem_RecordDef&  constraint);
        // Populate the specified list 'result' with the unset elements of
        // types specified in the specified 'constraint.  The behavior is
        // undefined unless 0 != result

    static int parseCompliantRecord(const char            **endPos,
                                    bdem_List              *result,
                                    const char             *inputString,
                                    const bdem_RecordDef&   constraint);
        // Parse the specified 'inputString' for a sequence of characters
        // matching the EBNF description of <COMPLIANT_RECORD> with the
        // specified 'constraint' and place in the specified 'result' the
        // corresponding value.  Store in the specified '*endPos' the address
        // of the non-modifiable character (in 'inputString') immediately
        // following the successfully parsed text, or the position at which
        // the parse failure was detected.  Return zero on success, and
        // a non-zero value otherwise.  The behavior is undefined if any
        // argument is 0.  The value of '*result' is undefined if a parse
        // failure occurs.


    static int parseNameOrIntegerLocator(const char            **endPos,
                                         int                    *index,
                                         const bdem_RecordDef&   constraint,
                                         const char             *inputString);

    static int parseNameOrIntegerLocator(const char         **endPos,
                                         int                 *index,
                                         const bdem_Schema&   constraint,
                                         const char          *inputString);
        // Parse the specified 'inputString' for a sequence of characters
        // matching the EBNF description of <STRING> <IDENTIFIER> or an
        // <INTEGER_LOCATOR>.  Find a field/record index matching the parsed
        // value in the specified 'constraint' and place it in the specified
        // 'index'.  Store in the specified '*endPos' the address of the
        // non-modifiable character (in 'inputString') immediately following
        // the successfully parsed text, or the position at which the parse
        // failure was detected.  Return zero on success, and a non-zero value
        // otherwise.  The behavior is undefined if any argument is 0.  The
        // value of '*index' is undefined if a parse failure occurs.

    static
    int parseTableRowElement(const char            **endPos,
                             bdem_List              *result,
                             int                     index,
                             const bdem_RecordDef&   constraint,
                             const char             *inputString);
    // Parse the specified 'inputString' for the element of type located at the
    // field at the specified 'index' in the specified 'constrain' and insert
    // it in the specified 'result' in place of the value residing at the
    // specified 'index'.  Return '0' on success and a non-zero value on
    // failure.  Place into the specified 'endPos' the address of the
    // character following the successfully parsed text or the place where
    // parse failure was identified.  The behavior is undefined unless
    // 0 != result or 0 != inputString.


                    // *** PARSE FUNCTIONS ***

  public:
    static int parseConstrainedList(const char            **endPos,
                                    bdem_List              *result,
                                    const char             *inputString,
                                    const bdem_RecordDef&   constraint);
        // Parse the specified 'inputString' for a sequence of characters
        // matching the EBNF description of <RECORD_VALUE> and place in the
        // specified 'result' the corresponding list value conforming to the
        // specified 'constraint'.  Store in the specified '*endPos' the
        // address of the non-modifiable character (in 'inputString')
        // immediately following the successfully parsed text, or the position
        // at which the parse failure was detected.  Return zero on success,
        // and a non-zero value otherwise.  The behavior is undefined if any
        // pointer argument is 0.  The value of '*result' is undefined if a
        // parse failure occurs.  Note that it is a parse failure unless (1)
        // each locator refers to a valid field within the constraining
        // record definition and (2) each data value can be parsed according
        // to its expected type. More generally, it is a parse failure unless
        // the parsed list (i.e., '*result') "conforms" to 'constraint' as
        // defined by the 'isListConformant' function in
        // 'bdem_schemaaggregateutil'.  It is also a parse failure unless
        // each integer value is within the range representable on the target
        // platform.  'double' and 'float' values will be invalid (even if
        // parsed correctly) if the parsed text indicates a floating point
        // value that cannot be represented on the target platform (e.g., the
        // absolute value is too large or too small).  Finally the result of
        // attempting to parse repeated associations (e.g.,
        // '{ x = 1, y = 2, x = 3 }') is implementation dependent.  If it is
        // not a parse failure, the last (i.e., right-most, bottom-most)
        // association prevails.
        //
        // Note also that each element value allows the (case-insensitive)
        // keyword NULL to mean the "unset" value.

    static int parseConstrainedTable(const char            **endPos,
                                     bdem_Table             *result,
                                     const char             *inputString,
                                     const bdem_RecordDef&   constraint);
        // Parse the specified 'inputString' for a sequence of characters
        // matching the EBNF description of <RECORD_ARRAY_VALUE> and
        // place in the specified 'result' the corresponding table value
        // conforming to the specified 'constraint'.  Store in the specified
        // '*endPos' the address of the non-modifiable character (in
        // 'inputString') immediately following the successfully parsed text,
        // or the position at which the parse failure was detected.  Return
        // zero on success, and a non-zero value otherwise.  The behavior is
        // undefined if any pointer argument is 0.  The value of '*result' is
        // undefined if a parse failure occurs.  Note that it is a parse
        // failure unless (1) each locator refers to a valid field within the
        // constraining record definition and (2) each value can be parsed
        // according to its expected type.  For tables in mapped-column
        // format, it is a parse failure unless the i'th element in each row
        // conforms to the record definition corresponding to the i'th field
        // locator in the column map.  More generally, it is a parse failure
        // unless the parsed table as a whole (i.e., '*result') "conforms"
        // to 'constraint' as defined by the 'isTableConformant' function in
        // 'bdem_schemaaggregateutil'.  It is also a parse failure unless each
        // integer value is within the range representable on the target
        // platform.  'double' and 'float' values will be invalid (even if
        // parsed correctly) if the parsed text indicates a floating point
        // value that cannot be represented on the target platform (e.g., the
        // absolute value is too large or too small).  Finally the result of
        // attempting to parse repeated associations (e.g.,
        // '{ x = 1, y = 2, x = 3 }') or repeated fields in a column map (e.g.,
        // '[ x, y, x ]') is implementation dependent.  If it is not a parse
        // failure, the last (i.e., right-most bottom-most) association
        // prevails.
        //
        // Note also that each element value allows the (case-insensitive)
        // keyword NULL to mean the "unset" value.


    static int parseScope(const char  **endPos,
                          bdem_Schema  *resultSchema,
                          int          *resultConstraintIndex,
                          bdem_List    *resultData,
                          const char   *inputString);
        // Parse the specified 'inputString' for a sequence of characters
        // matching the EBNF description of <SCOPE_VALUE> and place in the
        // specified 'result' the corresponding value.  Store in the specified
        // '*endPos' the address of the non-modifiable character (in
        // 'inputString') immediately following the successfully parsed text,
        // or the position at which the parse failure was detected.  Return
        // zero on success, and a non-zero value otherwise.  The behavior is
        // undefined if any argument is 0.  The values of '*resultSchema',
        // '*resultConstraintIndex', and '*resultData' are undefined if a
        // parse failure occurs.  Note that it is a parse failure unless the
        // data constraint identifies a valid record in the successfully
        // parsed schema.  It is also a parse failure unless each integer
        // value is within the range representable on the target platform.
        // 'double' and 'float' values will be invalid (even if parsed
        // correctly) if the parsed text indicates a floating point value that
        // cannot be represented on the target platform (e.g., the absolute
        // value is too large or too small).


                    // *** GENERATE FUNCTIONS ***

    static void generateConstrainedList(
                                 std::vector<char>      *buffer,
                                 const bdem_List&       value,
                                 const bdem_RecordDef&  constraint,
                                 int                    level = 0,
                                 int                    spacesPerLevel = 4);
        // Format the specified list 'value' as text, subject to the specified
        // 'constraint' record, using the (absolute value of) the optionally
        // specified indentation 'level', and append the result to the
        // specified 'buffer' while maintaining null-character termination.
        // If 'level' is specified, optionally specify 'spacesPerLevel', the
        // non-negative number of spaces per indentation level for this
        // object.  Making 'level' negative suppresses indentation for the
        // first line only.  The behavior is undefined unless 0 != buffer,
        // 0 <= spacesPerLevel, and 'value' is conformant to 'constraint'
        // as defined by the 'isListConformant' function in
        // 'bdem_schemaaggregateutil'.  Note that 'value' is output as a
        // textual representation that is parsable by the corresponding
        // 'parseConstrainedList' function subject to the same 'constraint'.

    static void generateConstrainedListRaw(
                                    std::vector<char>      *buffer,
                                    const bdem_List&       value,
                                    const bdem_RecordDef&  constraint,
                                    int                    level = 0,
                                    int                    spacesPerLevel = 4);
        // Format the specified list 'value' as text, subject to the specified
        // 'constraint' record definition, using the (absolute value of) the
        // optionally specified indentation 'level', and append the result
        // to the specified 'buffer while maintaining null-character
        // termination.  If 'level' is specified, optionally specify
        // 'spacesPerLevel', the non-negative number of spaces per indentation
        // level for this object.  Making 'level' negative suppresses
        // indentation for the first line only.  The behavior is undefined
        // unless 0 != buffer, 0 <= spacesPerLevel, and 'value' is
        // conformant to 'constraint' as defined by the 'isListConformant'
        // function in 'bdem_schemaaggregateutil'.  Note that this method is
        // similar to 'generateConstrainedList' - subject to (the same)
        // 'constraint' - except that no trailing null is ever removed or
        // supplied.

    static void generateConstrainedTable(
                                  std::vector<char>      *buffer,
                                  const bdem_Table&      value,
                                  const bdem_RecordDef&  constraint,
                                  int                    level = 0,
                                  int                    spacesPerLevel = 4);
        // Format the specified table 'value' as text, subject to the specified
        // 'constraint' record in the implicitly specified schema using the
        // (absolute value of) the optionally specified indentation 'level',
        // and append the result to the specified 'buffer' while maintaining
        // null-character termination.  If 'level' is specified, optionally
        // specify 'spacesPerLevel', the non-negative number of spaces per
        // indentation level for this object.  Making 'level' negative
        // suppresses indentation for the first line only.  The behavior is
        // undefined unless 0 != buffer, 0 <= spacesPerLevel, and 'value' is
        // conformant to 'constraint' as defined by the 'isTableConformant'
        // function in 'bdem_schemaaggregateutil'.  Note that 'value' is
        // output as a textual representation that is parsable by the
        // corresponding 'parseConstrainedTable' function subject to (the
        // same) 'constraint'.

    static void generateConstrainedTableRaw(
                                  std::vector<char>   *buffer,
                                  const bdem_Table&      value,
                                  const bdem_RecordDef&  constraint,
                                  int                    level = 0,
                                  int                    spacesPerLevel = 4);
        // Format the specified table 'value' as text, subject to the specified
        // 'constraint' record definition, using the (absolute value of) the
        // optionally specified indentation 'level', and append the result to
        // the specified 'buffer'.  If 'level' is specified, optionally
        // specify 'spacesPerLevel', the non-negative number of spaces per
        // indentation level for this object.  Making 'level' negative
        // suppresses indentation for the first line only.  The behavior is
        // undefined unless 0 != buffer, 0 <= spacesPerLevel, and 'value' is
        // conformant to 'constraint' as defined by the 'isTableConformant'
        // function in 'bdem_schemaaggregateutil'.  Note that this method is
        // similar to 'generateConstrainedTable' - subject to (the same)
        // 'constraint' - except that no trailing null is ever removed or
        // supplied.

   static void generateScope(std::vector<char>   *buffer,
                             const bdem_Schema&  schemaValue,
                             int                 constraintIndexValue,
                             const bdem_List&    dataValue,
                             int                 level = 0,
                             int                 spacesPerLevel = 4);
        // Format the specified 'schemaValue', 'constraintIndexValue', and
        //'dataValue' as a literal scope value using the (absolute value of)
        // the optionally specified indentation 'level' and append the result
        // to the specified 'buffer'termination.  If 'level' is specified,
        // optionally specify 'spacesPerLevel', the non-negative number of
        // spaces per indentation level for this object.  Making 'level'
        // negative suppresses indentation for the first line only.  The
        // behavior is undefined unless 0 <= constraintIndexValue,
        // 0 != buffer, 0 <= spacesPerLevel,
        // constraintIndexValue < schemValue.length(), and 'dataValue' is
        // conformant to the record definition at index
        // 'constraintIndexValue' in 'schemaValue' (as defined by the
        // 'isListConformant' function in 'bdem_schemaggregateutil').  Note
        // that 'value' is output as a textual representation that is
        // parsable by the corresponding 'parseScope' function.

    static void generateScopeRaw(std::vector<char>   *buffer,
                                 const bdem_Schema&  schemaValue,
                                 int                 constraintIndexValue,
                                 const bdem_List&    dataValue,
                                 int                 level = 0,
                                 int                 spacesPerLevel = 4);
        // Format the specified 'schemaValue', 'constraintIndexValue', and
        //'dataValue' as a literal scope value using the (absolute value of)
        // the optionally specified indentation 'level' and append the result
        // to the specified 'buffer'.  If 'level' is specified, optionally
        // specify 'spacesPerLevel', the non-negative number of spaces per
        // indentation level for this object.  Making 'level' negative
        // suppresses indentation for the first line only.  The behavior is
        // undefined unless 0 <= constraintIndexValue, 0 != buffer,
        // 0 <= spacesPerLevel, constraintIndexValue < schemValue.length(), and
        // 'dataValue' is conformant to the record type at index
        // 'constraintIndexValue' in 'schemaValue' (as defined by the
        // 'isListConformant' function in 'bdem_schemaggregateutil').  Note
        // that this method is similar to 'generateScope' except that no
        // trailing null is ever removed or supplied.

};

// ============================================================================
//                      INLINE FUNCTION DEFINITIONS
// ============================================================================

inline
int bdempu_SchemaAggregate::parseConstrainedList(
                                           const char            **endPos,
                                           bdem_List              *result,
                                           const char             *inputString,
                                           const bdem_RecordDef&   constraint)
{
    enum { SUCCESS = 0, FAILURE = 1 };

    result->removeAll();
    initUnsetList(result, constraint);

    bdepu_ParserImpUtil::skipWhiteSpace(&inputString, inputString);
    if (0 == bdepu_ParserImpUtil::parseNull(endPos, inputString)) {
        return SUCCESS;
    }
    return parseCompliantRecord(endPos, result, *endPos, constraint);
}

inline
int bdempu_SchemaAggregate::parseScope(const char  **endPos,
                                       bdem_Schema  *resultSchema,
                                       int          *resultConstraintIndex,
                                       bdem_List    *resultData,
                                       const char   *inputString)
{
    enum { SUCCESS = 0, FAILURE = 1 };

    bdepu_ParserImpUtil::skipWhiteSpace(endPos, inputString);
    if (bdepu_ParserImpUtil::skipRequiredToken(endPos, *endPos, '{')) {
        return FAILURE;
    }

    // parse <SCHEMA>
    bdepu_ParserImpUtil::skipWhiteSpace(endPos, *endPos);
    if (bdepu_ParserImpUtil::skipRequiredToken(endPos, *endPos, "SCHEMA")) {
        return FAILURE;
    }

    bdepu_ParserImpUtil::skipWhiteSpace(endPos, *endPos);
    if (bdempu_Schema::parseSchema(endPos, resultSchema, *endPos)) {
        return FAILURE;
    }

    // parse <DATA>
    bdepu_ParserImpUtil::skipWhiteSpace(endPos, *endPos);
    if (bdepu_ParserImpUtil::skipRequiredToken(endPos, *endPos, "DATA")) {
        return FAILURE;
    }

    bdepu_ParserImpUtil::skipWhiteSpace(endPos, *endPos);
    if (bdepu_ParserImpUtil::skipRequiredToken(endPos, *endPos, '<')) {
        return FAILURE;
    }
    if (parseNameOrIntegerLocator(endPos, resultConstraintIndex,
                                  *resultSchema, *endPos)) {
        return FAILURE;
    }

    bdepu_ParserImpUtil::skipWhiteSpace(endPos, *endPos);
    if (bdepu_ParserImpUtil::skipRequiredToken(endPos, *endPos, '>')) {
        return FAILURE;
    }

    bdepu_ParserImpUtil::skipWhiteSpace(endPos, *endPos);
    const bdem_RecordDef &constraint = resultSchema->record(
                                           *resultConstraintIndex);
    initUnsetList(resultData, constraint);

    bdepu_ParserImpUtil::skipWhiteSpace(endPos, *endPos);

    if (parseCompliantRecord(endPos, resultData, *endPos, constraint)) {
        return FAILURE;
    }

    bdepu_ParserImpUtil::skipWhiteSpace(endPos, *endPos);
    if (bdepu_ParserImpUtil::skipRequiredToken(endPos, *endPos, '}')) {
        return FAILURE;
    }

    return SUCCESS;
}

inline
void bdempu_SchemaAggregate::generateConstrainedList(
                                         std::vector<char>      *buffer,
                                         const bdem_List&       value,
                                         const bdem_RecordDef&  constraint,
                                         int                    level,
                                         int                    spacesPerLevel)
{
    bdepu_NTypesParser::stripNull(buffer);
    const int len = buffer->size();
    bdempu_SchemaAggregate::generateConstrainedRow(buffer, value.row(),
        constraint, level, spacesPerLevel);
    if (len == (int)buffer->size()) {
        // This is an empty list
        if (level > 0) {
            bdepu_ParserImpUtil::
                            generateIndentation(buffer, level, spacesPerLevel);
        }
        bdepu_ParserImpUtil::generateNull(buffer);
    }

    buffer->push_back('\0');
}

inline
void bdempu_SchemaAggregate::generateConstrainedTable(
                                        std::vector<char>      *buffer,
                                        const bdem_Table&      value,
                                        const bdem_RecordDef&  constraint,
                                        int                    level,
                                        int                    spacesPerLevel)
{
    bdepu_NTypesParser::stripNull(buffer);
    const int len = buffer->size();
    bdempu_SchemaAggregate::generateConstrainedTable_h(buffer, value,
        constraint, level, spacesPerLevel);
    if (len == (int)buffer->size()) {
        // This is an empty table
        if (level > 0) {
            bdepu_ParserImpUtil::
                            generateIndentation(buffer, level, spacesPerLevel);
        }
        bdepu_ParserImpUtil::generateNull(buffer);
    }
    buffer->push_back('\0');
}

inline
void bdempu_SchemaAggregate::generateScope(
                                      std::vector<char>   *buffer,
                                      const bdem_Schema&  schemaValue,
                                      int                 constraintIndexValue,
                                      const bdem_List&    dataValue,
                                      int                 level,
                                      int                 spacesPerLevel)
{
    bdepu_NTypesParser::stripNull(buffer);
    generateScopeRaw(buffer, schemaValue, constraintIndexValue,
                     dataValue, level, spacesPerLevel);
    buffer->push_back('\0');
}

inline
void bdempu_SchemaAggregate::generateConstrainedListRaw(
                                         std::vector<char>      *buffer,
                                         const bdem_List&       value,
                                         const bdem_RecordDef&  constraint,
                                         int                    level,
                                         int                    spacesPerLevel)
{
    const int len = buffer->size();
    bdempu_SchemaAggregate::generateConstrainedRow(buffer, value.row(),
        constraint, level, spacesPerLevel);
    if (len == (int)buffer->size()) {
        // This is an empty list
        if (level > 0) {
            bdepu_ParserImpUtil::
                            generateIndentation(buffer, level, spacesPerLevel);
        }
        bdepu_ParserImpUtil::generateNull(buffer);
    }

}

inline
void bdempu_SchemaAggregate::generateConstrainedTableRaw(
                                         std::vector<char>      *buffer,
                                         const bdem_Table&      value,
                                         const bdem_RecordDef&  constraint,
                                         int                    level,
                                         int                    spacesPerLevel)
{
    const int len = buffer->size();
    bdempu_SchemaAggregate::generateConstrainedTable_h(buffer, value,
        constraint, level, spacesPerLevel);
    if (len == (int)buffer->size()) {
        // This is an empty table
        if (level > 0) {
            bdepu_ParserImpUtil::
                            generateIndentation(buffer, level, spacesPerLevel);
        }
        bdepu_ParserImpUtil::generateNull(buffer);
    }
}

inline
void bdempu_SchemaAggregate::generateScopeRaw(
                                      std::vector<char>   *buffer,
                                      const bdem_Schema&  schemaValue,
                                      int                 constraintIndexValue,
                                      const bdem_List&    dataValue,
                                      int                 level,
                                      int                 spacesPerLevel)
{
    if (level > 0) {
        bdepu_ParserImpUtil::
                            generateIndentation(buffer, level, spacesPerLevel);
    }
    else {
        level *= -1;
    }
    int level1 = level + 1;

    buffer->push_back('{');
    buffer->push_back('\n');
    bdepu_ParserImpUtil::generateIndentation(buffer, level1, spacesPerLevel);

    buffer->push_back('S');
    buffer->push_back('C');
    buffer->push_back('H');
    buffer->push_back('E');
    buffer->push_back('M');
    buffer->push_back('A');
    buffer->push_back(' ');
    bdempu_Schema::generateSchemaRaw(buffer, schemaValue, level1 * -1,
                                     spacesPerLevel);
    buffer->push_back('\n');
    bdepu_ParserImpUtil::generateIndentation(buffer, level1, spacesPerLevel);
    buffer->push_back('D');
    buffer->push_back('A');
    buffer->push_back('T');
    buffer->push_back('A');
    buffer->push_back(' ');
    buffer->push_back('<');
    buffer->push_back(' ');
    bdepu_TypesParser::generateIntegerLocatorRaw(buffer,
        constraintIndexValue, 0, 0);
    buffer->push_back(' ');
    buffer->push_back('>');
    buffer->push_back(' ');
    generateConstrainedRow(buffer, dataValue.row(),
        schemaValue.record(constraintIndexValue), level1 * -1, spacesPerLevel);
    buffer->push_back('\n');
    bdepu_ParserImpUtil::generateIndentation(buffer, level, spacesPerLevel);
    buffer->push_back('}');
}

inline
void bdempu_SchemaAggregate::generateConstrainedRow(
                                         std::vector<char>      *buffer,
                                         const bdem_Row&        value,
                                         const bdem_RecordDef&  constraint,
                                         int                    level,
                                         int                    spacesPerLevel)
{
    int buflen0 = buffer->size();
    if (level > 0) {
        bdepu_ParserImpUtil::
                            generateIndentation(buffer, level, spacesPerLevel);
    }
    else {
        level *= -1;
    }
    int level1 = level + 1;

    buffer->push_back('{');
    buffer->push_back('\n');

    int buflen1 = buffer->size();

    // Look through the list and generate text representation for non-NULL
    // elements.
    int len = value.length();

    for (int i = 0; i < len; ++i) {
        generateConstrainedRowElement(buffer, value, constraint, i,
                                      level1, spacesPerLevel);
    }

    int buflen2 = buffer->size();
    if (buflen1 == buflen2) {
        // The list is empty, don't generate it.
        buffer->resize(buflen0);
        return;
    }

    //    buffer->remove(buflen2 - 1);

    bdepu_ParserImpUtil::generateIndentation(buffer, level, spacesPerLevel);
    buffer->push_back('}');
}

inline
void bdempu_SchemaAggregate::generateConstrainedTable_h(
                                         std::vector<char>      *buffer,
                                         const bdem_Table&      value,
                                         const bdem_RecordDef&  constraint,
                                         int                    level,
                                         int                    spacesPerLevel)
{
    int buflen0 = buffer->size();
    if (level > 0) {
        bdepu_ParserImpUtil::
                            generateIndentation(buffer, level, spacesPerLevel);
    }
    else {
        level *= -1;
    }
    int level1 = level + 1;

    buffer->push_back('{');
    buffer->push_back('\n');

    int buflen1 = buffer->size();

    // Look through the table and generate text representation for non-NULL
    // elements.
    int len = value.numRows();

    for (int i = 0; i < len; ++i) {
        generateConstrainedRow(buffer,
                               value[i],
                               constraint,
                               level1,
                               spacesPerLevel);
        buffer->push_back('\n');
    }

    int buflen2 = buffer->size();
    if (buflen1 == buflen2) {
        // The table is empty, don't generate it.
        buffer->resize(buflen0);
        return;
    }
    bdepu_ParserImpUtil::generateIndentation(buffer, level, spacesPerLevel);
    buffer->push_back('}');
}

inline
void bdempu_SchemaAggregate::initUnsetList(bdem_List             *result,
                                           const bdem_RecordDef&  constraint)
{
    // Initialize list with unset values
    int len = constraint.length();
    for (int i = 0; i < len; ++i) {
        result->appendUnsetElement(constraint.field(i).elemType());
    }
}

inline
int bdempu_SchemaAggregate::parseCompliantRecord(
                                           const char            **endPos,
                                           bdem_List              *result,
                                           const char             *inputString,
                                           const bdem_RecordDef&   constraint)
{
    enum { SUCCESS = 0, FAILURE = 1 };

    if (bdepu_ParserImpUtil::skipRequiredToken(endPos, inputString, '{')) {
        return FAILURE;
    }

    // Parse <COMPLIANT_RECORD>
    bdepu_ParserImpUtil::skipWhiteSpace(endPos, *endPos);
    if (0 == bdepu_ParserImpUtil::skipRequiredToken(endPos, *endPos, '}')) {
        // Empty list
        return SUCCESS;
    }

    while (1) {
        // Parse <ASSOCIATIONS>

        // Parse <FIELD_LOCATOR>
        int index;

        bdepu_ParserImpUtil::skipWhiteSpace(endPos, *endPos);

        if (parseNameOrIntegerLocator(endPos, &index, constraint,
                                      *endPos)) {
            return FAILURE;
        }

        bdepu_ParserImpUtil::skipWhiteSpace(endPos, *endPos);
        if (bdepu_ParserImpUtil::skipRequiredToken(endPos, *endPos, '=')) {
            return FAILURE;
        }
        bdepu_ParserImpUtil::skipWhiteSpace(endPos, *endPos);

        // Parse <VALUE> and insert it into the 'result'
        if (parseTableRowElement(endPos, result, index, constraint, *endPos)) {
            return FAILURE;
        }

        bdepu_ParserImpUtil::skipWhiteSpace(endPos, *endPos);
        if ('}' == **endPos) {
            ++*endPos;
            return SUCCESS;
        }
    }
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
