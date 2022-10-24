// bdlb_tokenizer.h                                                   -*-C++-*-
#ifndef INCLUDED_BDLB_TOKENIZER
#define INCLUDED_BDLB_TOKENIZER

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide access to user-described tokens via string references.
//
//@CLASSES:
//  bdlb::Tokenizer: lexer for tokens defined via hard and/or soft delimiters
//  bdlb::TokenizerIterator: input iterator for delimited tokens in a string
//
//@SEE_ALSO: bslstl_stringref
//
//@DESCRIPTION: This component defines a mechanism, 'bdlb::Tokenizer', that
// provides non-destructive sequential (read-only) access to tokens in a given
// input string as characterized by two disjoint sets of user-specified
// delimiter characters, each of which is supplied at construction via either a
// 'const bsl::string_view&' or (for efficiency, when only the leading
// characters of the input string may need to be parsed) a 'const char *'.
// Note that each character (including '\0') that is not explicitly designated
// as a delimiter character is assumed to be *token* character.
//
///Soft versus Hard Delimiters
///---------------------------
// The tokenizer recognizes two distinct kinds of delimiter characters, *soft*
// and *hard*.
//
// A *soft* *delimiter* is a maximal (non-empty) sequence of soft-delimiter
// characters.  Soft delimiters, typically whitespace characters, are used to
// separate (rather than terminate) tokens, and thus never result in an empty
// token.
//
// A *hard* *delimiter* is a maximal (non-empty) sequence of delimiter
// characters consisting of exactly one hard-delimiter character.  Hard
// delimiters, typically printable punctuation characters such ('/') or colon
// (':' ), are used to terminate (rather than just separate) tokens, and thus a
// hard delimiter that is not preceded by a token character results in an empty
// token.
//
// Soft delimiters are used in applications where multiple consecutive
// delimiter characters are to be treated as just a single delimiter.  For
// example, if we want the input string '"Sticks  and stones"' to parse into a
// sequence of three non-empty tokens ["Sticks", "and", "stones"], rather than
// the four-token sequence ["Sticks", "", "and", "stones"], we would make the
// space (' ') a soft-delimiter character.
//
// Hard delimiters are used in applications where consecutive delimiter
// characters are to be treated as separate delimiters, giving rise to the
// possibility of empty tokens.  Making the slash ('/') in the standard date
// format a hard delimiter for the input string "15//9" yields the three-token
// sequence ["15", "", "9"], rather than the two-token one ["15", "9"] had it
// been made soft.
//
// All members within each respective character set are considered equivalent
// with respect to tokenization.  For example, making '/' and ':' *soft*
// *delimiter* characters on the questionably formatted date "2015/:10:/31"
// would yield the token sequence ["2015", "10", "31"], whereas making '/' and
// ':' *hard* *delimiter* characters would result in the token sequence
// ["2015", "", "10", "", "31"].  Making either of these two delimiter
// characters hard and the other soft would, in this example, yield the former
// (shorter) sequence of tokens.  The details of how soft and hard delimiters
// interact is illustrated in more detail in the following section (but also
// see, later on, the section on "Comprehensive Detailed Parsing
// Specification").
//
///The Input String to be Tokenized
///--------------------------------
// Each input string consists of an optional leading sequence of soft-delimiter
// characters called the *leader*, followed by an alternating sequence of
// tokens and delimiters (the final delimiter being optional):
//..
//  Input String:
//  +--------+---------+-------------+---...---+---------+-------------+
//  | leader | token_1 | delimiter_1 |         | token_N | delimiter_N |
//  +--------+---------+-------------+---...---+---------+-------------+
//  (optional)                                              (optional)
//..
// The tokenization of a string can also be expressed as pseudo-Posix regular
// expression notation:
//..
//   delimiter = [[:soft:]]+ | [[:soft:]]* [[:hard:]] [[:soft:]]*
//   token     = [^[:soft:][:hard:]]*
//   string    = [[:soft:]]* (token delimiter)* token?
//..
// Parsing is from left to right and is *greedy* -- i.e., the longest sequence
// satisfying the regular expression is the one that matches.  For example, let
// 's' represent the start of a soft delimiter, 'd' the start of a hard
// delimiter, '^" the start of a token, and '~' the continuation of that same
// delimiter or token.  Using '.' as a soft delimiter and "/" as a hard one,
// the string
//..
//         s~   h~  h~~  h~     s~   hh  s  h~h    h~~~        Delimiters
//
//        "..One/.if./.by./land,..two//if.by/./sea!./.."
//
//           ^~~  ^~   ^~  ^~~~   ^~~ ^^~ ^~  ^^~~             Tokens
//                                    |       |
//                                 (empty)  (empty)
//..
// yields the tokenization
//..
//     [One]  [if]   [by]  [land,]  [two] [] [if] [by]  [] [sea]       Tokens
//
//  (..)   (/.)  (./.)  (./)     (..)   (/)(/)  (.)  (/.)(/)   (./..)  Delims
//..
// Notice that in pair of hard delimiters "/./" before the token "sea", the
// soft token character between the two hard ones binds to the earlier
// delimiter.
//
///Iterating using a 'TokenizerIterator' object (ACCESS TO TOKENS ONLY)
///--------------------------------------------------------------------
// This component provides two separate mechanisms by which a user may iterate
// over a sequence of tokens.  The first mechanism is as a *token* *range*,
// exposed by the 'TokenizerIterator' objects returned by the 'begin' and 'end'
// methods on a 'Tokenizer' object.  A 'TokenizerIterator' supports the concept
// of a standard *input* *iterator*, returning each successive token as a
// 'bslstl::StringRef', making it suitable for generic use -- e.g., in a
// range-based 'for' loop:
//..
//  void parse_1(bsl::ostream& output, const char *input)
//      // Print, to the specified 'output' stream, each whitespace-delimited
//      // token in the specified 'input; string on a separate line following
//      // a vertical bar ('|') and a hard space (' ').
//  {
//      const char softDelimiters[] = " \t\n";  // whitespace
//
//      for (bslstl::StringRef token : bdlb::Tokenizer(input, softDelimiters)) {
//          bsl::cout << "| " << token << bsl::endl;
//      }
//  }
//..
// The 'parse_1' function above produces each (non-whitespace) token in the
// supplied input string on a separate line.  So, were 'parse_1' to be given a
// reference to 'bsl::cout' and the input string
//..
//  " Times  like \tthese\n  try \n \t men's\t \tsouls.\n"
//..
// we would expect
//..
//  | Times
//  | like
//  | these
//  | try
//  | men's
//  | souls.
//..
// to be displayed on 'bsl::cout'.  Note that there is no way to access the
// delimiters from a 'TokenizerIterator' directly, for that we will need to
// use the 'tokenizer' as a non-standard "iterator" directly.
//
///Iterating using a 'Tokenizer' object (ACCESS TO TOKENS AND DELIMITERS)
///----------------------------------------------------------------------
// The second mechanism, not intended for generic use, provides direct access
// to the previous and current (trailing) delimiters as well as the current
// token:
//..
//  void parse_2(bsl::ostream& output, const char *input)
//      // Print, to the specified 'output' stream the leader of the specified
//      // 'input', on a singly line, followed by subsequent current token and
//      // (trailing) delimiter pairs on successive lines, each line beginning
//      // with a vertical bar ('|') followed by a tab ('\t') character.
//  {
//      const char softDelimiters[] = " ";
//      const char hardDelimiters[] = ":/";
//
//      bdlb::Tokenizer it(input, softDelimiters, hardDelimiters);
//      output << "| " << '"' << it.previousDelimiter() << '"' << "\n";
//
//      for (; it.isValid(); ++it) {
//          output << "|\t"
//                 << '"' << it.token() << '"'
//                 << "\t"
//                 << '"' << it.trailingDelimiter() << '"'
//                 << "\n";
//      }
//  }
//..
// The parse_2 function above produces the *leader* on the first line,
// followed by each *token* along with its current (trailing) delimiter on
// successive lines.  So, were 'parse_2' to be given a reference to
// 'bsl::cout' and the input string
//..
//  " I've :been: a : :bad:/ boy! / "
//..
// we would expect
//..
//  |       " "
//  |       "I've"  " :"
//  |       "been"  ": "
//  |       "a :"   " : "
//  |       ""      ":"
//  |       "bad"   ":"
//  |       ""      "/ "
//  |       "boy!"  " / "
//..
// to be displayed on 'bsl::cout'.
//
///Token and Delimiter Lifetimes
///-----------------------------
// All tokens and delimiters are returned efficiently by value as
// 'bslstl::StringRef' objects, which naturally remain valid so long as the
// underlying input string remains unchanged -- irrespective of the validity
// of the 'tokenizer' or any of its dispensed token iterators.  Note, however,
// that all such token iterators are invalidated if the parent tokenizer object
// is destroyed or reset.  Note also the previous delimiter field remains
// accessible from a 'tokenizer' object even after it has reached the end of
// its input.  Also note that the *leader* is accessible, using the
// 'previousDelimiter' method prior to advancing the iteration state of the
// 'Tokenizer'.
//
///Comprehensive Detailed Parsing Specification
///--------------------------------------------
// This section provides a comprehensive (length-ordered) enumeration of how
// the 'bdlb::Tokenizer' performs, according to its three (non-null) character
// types:
//..
//  '.' = any *soft* delimiter character
//  '#' = any *hard* delimiter character
//  'T' = any token character
//..
// Here's how iteration progresses for various input strings.  Note that input
// strings having consecutive characters of the same category that naturally
// coalesce (i.e., behave as if they were a single character of that category)
// -- namely soft-delimiter or token characters -- are labeled with '(%)'.
// For example, consider the input ".." at the top of the [length 2] section
// below.  The table indicates, with a (%) in the first column, that the input
// acts the same as if it were a single (soft-delimiter) character (i.e., ".").
// There is only one line in this row of the table because, upon construction,
// the iterator is immediately invalid (as indicated by the right-most column).
// Now consider the "##" entry near the bottom of [length 2].  These
// (hard-delimiter) tokens do not coalesce.  What's more, the iterator on
// construction is valid and produces a empty leader and empty first token.
// after advancing the tokenizer, the second line of that row shows the
// current state of iteration with the previous delimiter being a '#' as well
// as the current one.  The current token is again shown as empty.  After
// advancing the tokenizer again, we now see that the iterator is invalid, yet
// the previous delimiter (still accessible) is a '#').
//..
//  (%) = repeat   Previous   Current   Current   Iterator
//  Input String   Delimiter   Token   Delimiter   Status
//  ============   =========  =======  =========  ========  [length 0]
//  ""             ""         na       na         invalid
//
//  ============   =========  =======  =========  ========  [length 1]
//  "."            "."        na       na         invalid
//  ------------   ---------  -------  ---------  --------
//  "#"            ""         ""       "#"        valid
//                 "#"        na       na         invalid
//  ------------   ---------  -------  ---------  --------
//  "T"            ""         "T"      ""         valid
//                 ""         na       na         invalid
//
//  ============   =========  =======  =========  ========  [length 2]
//  ".."     (%)   ".."       na       na         invalid
//  ------------   ---------  -------  ---------  --------
//  ".#"           "."        ""       "#"        valid
//                 "#"        na       na         invalid
//  ------------   ---------  -------  ---------  --------
//  ".T"           "."        "T"      ""         valid
//                 ""         na       na         invalid
//
//  ------------   ---------  -------  ---------  --------
//  "#."           ""         ""       "#"        valid
//                 "#"        na       na         invalid
//  ------------   ---------  -------  ---------  --------
//  "##"           ""         ""       "#"        valid
//                 "#"        ""       "#"        valid
//                 "#"        na       na         invalid
//  ------------   ---------  -------  ---------  --------
//  "#T"           ""         ""       "#"        valid
//                 "#"        "T"      ""         valid
//                 ""         na       na         invalid
//
//  ------------   ---------  -------  ---------  --------
//  "T."           ""         "T"      "."        valid
//                 "."        na       na         invalid
//  ------------   ---------  -------  ---------  --------
//  "T#"           ""         "T"      "#"        valid
//                 "#"        na       na         invalid
//  ------------   ---------  -------  ---------  --------
//  "TT"     (%)   ""         "TT"     ""         valid
//                 ""         na       na         invalid
//
//  ============   =========  =======  =========  ========  [length 3]
//  "..."    (%)   "..."      na       na         invalid
//  ------------   ---------  -------  ---------  --------
//  "..#"    (%)   ".."       ""       "#"        valid
//                   "#"      na       na         invalid
//  ------------   ---------  -------  ---------  --------
//  "..T"    (%)   ".."       "T"      ""         valid
//                 ".."       na       na         invalid
//  ------------   ---------  -------  ---------  --------
//  ".#."          "."        ""       "#."       valid
//                 "#."       na       na         invalid
//  ------------   ---------  -------  ---------  --------
//  ".##"          "."        ""       "#"        valid
//                 "#"        ""       "#"        valid
//                 "#"        na       na         invalid
//  ------------   ---------  -------  ---------  --------
//  ".#T"          "."        ""       "#"        valid
//                 "#"        "T"      ""         valid
//                 ""         na       na         invalid
//  ------------   ---------  -------  ---------  --------
//  ".T."          "."        "T"      "."        valid
//                 "."        na       na         invalid
//  ------------   ---------  -------  ---------  --------
//  ".T#"          "."        "T"      "#"        valid
//                 "#"        na       na         invalid
//  ------------   ---------  -------  ---------  --------
//  ".TT"    (%)   "."        "TT"     ""         valid
//                 ""         na       na         invalid
//
//  ------------   ---------  -------  ---------  --------
//  "#.."    (%)   ""         ""       "#.."      invalid
//                 "#.."      na       na         invalid
//  ------------   ---------  -------  ---------  --------
//  "#.#"          ""         ""       "#."       valid
//                 "#."       ""       "#"        valid
//                 "#"        na       na         invalid
//  ------------   ---------  -------  ---------  --------
//  "#.T"          ""         ""       "#."       valid
//                 "#."       "T"      ""         valid
//                 ""         na       na         invalid
//  ------------   ---------  -------  ---------  --------
//  "##."          ""         ""       "#"        valid
//                 "#"        ""       "#."       valid
//                 "#."       na       na         invalid
//  ------------   ---------  -------  ---------  --------
//  "###"          ""         ""       "#"        valid
//                 "#"        ""       "#"        valid
//                 "#"        ""       "#"        valid
//                 "#"        na       na         invalid
//  ------------   ---------  -------  ---------  --------
//  "##T"          ""         ""       "#"        valid
//                 "#"        ""       "#"        valid
//                 "#"        "T"      ""         valid
//                 ""         na       na         invalid
//  ------------   ---------  -------  ---------  --------
//  "#T."          ""         ""       "#"        valid
//                 "#"        "T"      "."        valid
//                 "."        na       na         invalid
//  ------------   ---------  -------  ---------  --------
//  "#T#"          ""         ""       "#"        valid
//                 "#"        "T"      "#"        valid
//                 "#"        na       na         invalid
//  ------------   ---------  -------  ---------  --------
//  "#TT"    (%)   ""         ""       "#"        valid
//                 "#"        "TT"     ""         valid
//                 ""         na       na         invalid
//
//  ------------   ---------  -------  ---------  --------
//  "T.."    (%)   ""         "T"      ".."       valid
//                 ".."       na       na         invalid
//  ------------   ---------  -------  ---------  --------
//  "T.#"          ""         "T"      ".#"       valid
//                 ".#"       na       na         invalid
//  ------------   ---------  -------  ---------  --------
//  "T.T"          ""         "T"      "."        valid
//                 "."        "T"      ""         valid
//                 ""         na       na         invalid
//  ------------   ---------  -------  ---------  --------
//  "T#."          ""         "T"      "#."       valid
//                 "#."       na       na         invalid
//  ------------   ---------  -------  ---------  --------
//  "T##"          ""         "T"      "#"        valid
//                 "#"        ""       "#"        valid
//                 "#"        ""       "#"        valid
//                 "#"        na       na         invalid
//  ------------   ---------  -------  ---------  --------
//  "T#T"          ""         "T"      "#"        valid
//                 "#"        "T"      "#"        valid
//                 ""         na       na         invalid
//  ------------   ---------  -------  ---------  --------
//  "TT."    (%)   ""         "TT"     "."        valid
//                 "."        na       na         invalid
//  ------------   ---------  -------  ---------  --------
//  "TT#"    (%)   ""         "TT"     "#"        valid
//                 "#"        na       na         invalid
//  ------------   ---------  -------  ---------  --------
//  "TTT"    (%)   "#"        "TTT"    ""         valid
//                 ""         na       na         invalid
//  ------------   ---------  -------  ---------  --------
//..
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Iterating Over Tokens Using Just *Soft* Delimiters
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// This example illustrates the process of splitting the input string into a
// sequence of tokens using just soft delimiters.
//
// Suppose, we have a text where words are separated with a variable number of
// spaces and we want to remove all duplicated spaces.
//
// First, we create an example character array:
//..
//  const char text1[] = "   This  is    a test.";
//..
// Then, we create a 'Tokenizer' that uses " "(space) as a soft delimiter:
//..
//  bdlb::Tokenizer tokenizer1(text1, " ");
//..
// Note, that the tokenizer skips the leading soft delimiters upon
// initialization.  Next, we iterate the input character array and build the
// string without duplicated spaces:
//..
//  bsl::string result1;
//  if (tokenizer1.isValid()) {
//      result1 += tokenizer1.token();
//      ++tokenizer1;
//  }
//  while (tokenizer1.isValid()) {
//      result1 += " ";
//      result1 += tokenizer1.token();
//      ++tokenizer1;
//  }
//..
// Finally, we verify that the resulting string contains the expected result:
//..
//  const bsl::string EXPECTED1("This is a test.");
//  assert(EXPECTED1 == result1);
//..
//
///Example 2: Iterating Over Tokens Using Just *Hard* Delimiters
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// This example illustrates the process of splitting the input string into a
// sequence of tokens using just hard delimiters.
//
// Suppose, we want to reformat comma-separated-value file and insert the
// default value of '0' into missing columns.
//
// First, we create an example CSV line:
//..
//  const char text2[] = "Col1,Col2,Col3\n111,,133\n,222,\n311,322,\n";
//..
// Then, we create a 'Tokenizer' that uses ","(comma) and "\n"(new-line) as
// hard delimiters:
//..
//  bdlb::Tokenizer tokenizer2(text2, "", ",\n");
//..
// We use the 'trailingDelimiter' accessor to insert correct delimiter into the
// output string.  Next, we iterate the input line and insert the default
// value:
//..
//  string result2;
//  while (tokenizer2.isValid()) {
//      if (tokenizer2.token() != "") {
//          result2 += tokenizer2.token();
//      } else {
//          result2 += "0";
//      }
//      result2 += tokenizer2.trailingDelimiter();
//      ++tokenizer2;
//  }
//..
// Finally, we verify that the resulting string contains the expected result:
//..
//  const string EXPECTED2("Col1,Col2,Col3\n111,0,133\n0,222,0\n311,322,0\n");
//  assert(EXPECTED2 == result2);
//..
//
///Example 3: Iterating Over Tokens Using Both *Hard* and *Soft* Delimiters
/// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// This example illustrates the process of splitting the input string into a
// sequence of tokens using both soft and hard delimiters.
//
// Suppose, we want to extract the tokens from a file, where the fields are
// separated with a "$"(dollar-sign), but can have leading or trailing spaces.
//
// First, we create an example line:
//..
//  const char text3[] = " This $is    $   a$ test.      ";
//..
// Then, we create a 'Tokenizer' that uses "$"(dollar-sign) as a hard delimiter
// and " "(space) as a soft delimiter:
//..
//  bdlb::Tokenizer tokenizer3(text3, " ", "$");
//..
// In this example we only extracting the tokens, and can use the iterator
// provided by the tokenizer.
//
// Next, we create an iterator and iterate over the input, extracting the
// tokens into the result string:
//..
//  string result3;
//
//  bdlb::Tokenizer::iterator it3 = tokenizer3.begin();
//
//  if (it3 != tokenizer3.end()) {
//      result3 += *it3;
//  }
//  ++it3;
//
//  while (it3 != tokenizer3.end()) {
//      result3 += " ";
//      result3 += *it3;
//      ++it3;
//  }
//..
// Finally, we verify that the resulting string contains the expected result:
//..
//  const string EXPECTED3("This is a test.");
//  assert(EXPECTED3 == result3);
//..

#include <bdlscm_version.h>

#include <bsl_string.h>

#include <bsls_assert.h>
#include <bsls_compilerfeatures.h>
#include <bsls_keyword.h>
#include <bsls_libraryfeatures.h>
#include <bsls_platform.h>
#include <bsls_review.h>

#include <bsl_iterator.h>

namespace BloombergLP {

namespace bdlb {
                        // ============================
                        // private class Tokenizer_Data
                        // ============================

class Tokenizer_Data {
    // This component-private class is used to hold delimiter information.
    // Each 'Tokenizer' object will have, as a private data member, an object
    // of this class,  and will pass the address of that member to the
    // (private) constructor of each 'TokenizerIterator' object it issues:
    //..
    //  +--------------------------------------+
    //  |   ,--------------.                   |
    //  |  ( Tokenizer_Data )                  |
    //  |   `--------------'\                  |
    //  |          |         \                 |
    //  |          |     ,----*------------.   |
    //  |          |    ( TokenizerIterator )  |
    //  |          |    /`-----------------'   |
    //  |          |   /                       |
    //  |     ,----*--o-.                      |
    //  |    ( Tokenizer )                     |
    //  |     `---------'                      |
    //  +--------------------------------------+
    //  bdlb_tokenizer
    //..

    enum {
        k_MAX_CHARS = 256  // maximum # of unique values for an 8-bit 'char'
    };

    char d_charTypes[k_MAX_CHARS];  // table of SOFT / HARD / TOKEN characters

  private:
    // NOT IMPLEMENTED
    Tokenizer_Data(const Tokenizer_Data&) BSLS_KEYWORD_DELETED;
    Tokenizer_Data& operator=(const Tokenizer_Data&) BSLS_KEYWORD_DELETED;

  public:
    // CREATORS
    explicit Tokenizer_Data(const bsl::string_view& softDelimiters);
    Tokenizer_Data(const bsl::string_view& softDelimiters,
                   const bsl::string_view& hardDelimiters);
        // Create a 'Tokenizer_Data' object and load the 'd_charTypes' data
        // member such that it has the same value *as* *if* this (overly
        // prescriptive) algorithm were used: (I) initialize each entry in
        // 'd_charTypes' array to a value indicating that the character having
        // that 'index' as its (e.g., ASCII) representation is a *token*
        // character; (II) then, for each character in the specified
        // 'softDelimiters' sequence, overwrite the element at the
        // corresponding index in 'd_charTypes' with a value that indicates
        // that the character is a *soft* delimiter character; (III) finally,
        // for each character in the specified 'hardDelimiters' sequence,
        // overwrite the element at the corresponding index with a distinct
        // value that indicates the character is a *hard* delimiter* character.
        // Note that duplicate delimiter characters in the respective inputs
        // are naturally ignored, and that a character that appears in both
        // sets would naturally be considered *hard*.  Also note that it is
        // entirely reasonable to state, in any public interface, that the
        // behavior is undefined unless the characters in the union of the two
        // delimiter sequences are unique.

    // ACCESSORS
    int inputType(char character) const;
        // Return the input type of the specified 'character': 0 for token,
        // 1 for soft delimiter, 2 for hard delimiter.
};

                        // =====================
                        // class Tokenizer_Proxy
                        // =====================

class Tokenizer_Proxy {
    // This class provides a proxy holder of a reference to a
    // 'TokernizerIterator' object, allowing correct return of 'operator->'.

    // DATA
    bslstl::StringRef d_obj; // The object

  private:
    // NOT IMPLEMENTED
    Tokenizer_Proxy& operator=(const Tokenizer_Proxy&) BSLS_KEYWORD_DELETED;

  public:
    // CREATORS
    Tokenizer_Proxy(const bsl::string_view &obj);
        // Create a 'ProxyHolder' object with a copy the specified 'obj'.

#ifdef BSLS_COMPILERFEATURES_SUPPORT_DEFAULTED_FUNCTIONS
    Tokenizer_Proxy(const Tokenizer_Proxy& original) = default;
        // Create a 'Tokenizer_Proxy' object having the same value as the
        // specified 'original' object.  Note that this copy constructor is
        // generated by the compiler.

    ~Tokenizer_Proxy() = default;
        // Destroy this object.
#endif

    // OPERATORS
    const bslstl::StringRef *operator->() const;
        // Return a pointer to the object contained by the 'Tokenizer_Proxy'.
};

                        // =======================
                        // class TokenizerIterator
                        // =======================

class TokenizerIterator
#if defined(BSLS_LIBRARYFEATURES_STDCPP_LIBCSTD)
// Sun CC workaround: iterators must be derived from 'std::iterator' to work
// with the native std library algorithms.  However, 'std::iterator' is
// deprecated in C++17, so do not rely on derivation unless required, to avoid
// deprecation warnings on modern compilers.
    : public bsl::iterator<bsl::input_iterator_tag,
                           bslstl::StringRef,
                           int,
                           Tokenizer_Proxy,
                           const bslstl::StringRef>
#endif  // BSLS_LIBRARYFEATURES_STDCPP_LIBCSTD
{
    // This class provides a C++-standards-conforming input iterator over the
    // tokens in the input string suppled at construction (along with the
    // designation of *soft* and *hard* delimiter characters) to a 'Tokenizer'
    // object.  Tokens are returned, using a 'bslstl::StringRef' -- by value --
    // that means the iterated references remain valid until the underlying
    // input string itself is modified or destroyed.  Note that all iterators
    // are invalidated whenever the input string in the parent 'Tokenizer'
    // change.


    // DATA
    const Tokenizer_Data *d_sharedData_p;  // (address of) character categories
    const char           *d_cursor_p;      // tail of parsed input
    const char           *d_token_p;       // current token
    const char           *d_postDelim_p;   // current (trailing) delimiter
    const char           *d_end_p;         // one past input; 0 for '(char *)'
    bool                  d_endFlag;       // set 'true' when at end of input

    // FRIENDS
    friend class Tokenizer;
    friend bool operator==(const TokenizerIterator&, const TokenizerIterator&);
    friend bool operator!=(const TokenizerIterator&, const TokenizerIterator&);

    // PRIVATE CREATORS
    TokenizerIterator(const char           *input,
                      const char           *end,
                      const Tokenizer_Data *sharedData);
        // Create a 'TokenizerIterator' object bound to the specified sequence
        // of 'input' characters ending at the specified 'end' and the
        // specified delimiter and token mapper 'sharedData'.

  public:
    // TYPES
    typedef bslstl::StringRef        value_type;
    typedef int                      difference_type;
    typedef Tokenizer_Proxy          pointer;
    typedef const bslstl::StringRef  reference;
    typedef bsl::input_iterator_tag  iterator_category;
        // Defines a type alias for the tag type that represents the iterator
        // concept this class models.

    // CREATORS
    TokenizerIterator();
    TokenizerIterator(const TokenizerIterator& origin);
        // Create a 'TokenizerIterator' object having the value of the
        // specified 'origin' iterator.

    // MANIPULATORS
    TokenizerIterator& operator=(const TokenizerIterator& rhs);
        // Assign to this object the value of the specified 'rhs' iterator, and
        // return a reference providing modifiable access to this object.

    TokenizerIterator& operator++();
        // Advance the iteration state of this object to refer to the next
        // token in the underlying input sequence, and return a reference
        // providing modifiable access to this object.  The behavior is
        // undefined unless the iteration state of this object is initially
        // valid, or if the underlying input has been modified or destroyed
        // since this object was created.

    // ACCESSORS
    const bslstl::StringRef operator*() const;
        // Return a reference to the non-modifiable current token (i.e.,
        // maximal sequence of non-delimiter characters) in the input string.
        // The returned reference remains valid so long as the underlying input
        // is not modified or destroyed -- irrespective of the state (or
        // existence) of this object.  The behavior is undefined unless the
        // iteration state of this object is initially valid, or if the
        // underlying input has been modified or destroyed since this object
        // was created.

    Tokenizer_Proxy operator->() const;
        // Return a proxy object containing the non-modifiable current token
        // (i.e., maximal sequence of non-delimiter characters) in the input
        // string.  The returned proxy remains valid so long as the underlying
        // input is not modified or destroyed -- irrespective of the state (or
        // existence) of this object.  The behavior is undefined unless the
        // iteration state of this object is initially valid, or if the
        // underlying input has been modified or destroyed since this object
        // was created.
};

// FREE OPERATORS
bool operator==(const TokenizerIterator& lhs, const TokenizerIterator& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects have the same
    // value, and 'false' otherwise.  Two 'TokenizerIterator' objects have the
    // same value if both of them are pointing to the same token within the
    // same tokenized string or if they both point past the tokenized string.
    // The behaviour is undefined unless the iterators returned by the same
    // 'Tokenizer' object, or if the underlying input has been modified or
    // destroyed since any of those objects were created.

bool operator!=(const TokenizerIterator& lhs, const TokenizerIterator& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects do not have the
    // same value, and 'false' otherwise.  The behaviour is undefined unless
    // the iterators returned by the same 'Tokenizer' object, or if the
    // underlying input has been modified or destroyed since any of those
    // objects were created.

const TokenizerIterator operator++(TokenizerIterator& object, int);
    // Advance the iteration state of the specified 'object' to refer to the
    // next token in the underlying input sequence, and return a copy of this
    // object prior advancing the iteration state.  The behavior is undefined
    // unless the iteration state of this object is initially valid, or if the
    // underlying input has been modified or destroyed since this object was
    // created.


                            // ===============
                            // class Tokenizer
                            // ===============

class Tokenizer {
    // This class provides (read-only) sequential access to tokens delimited by
    // two user-supplied character sets consisting, respectively, of *soft* and
    // *hard* delimiters characters.  Access to the previous and current
    // (trailing) delimiter, as well as to the current token itself, is
    // provided efficiently via 'bslstl::StringRef'.

    // DATA
    Tokenizer_Data  d_sharedData;  // delimiter/token character categories
    const char     *d_input_p;     // original input
    const char     *d_cursor_p;    // tail of parsed input
    const char     *d_prevDelim_p; // previous delimiter
    const char     *d_token_p;     // current token
    const char     *d_postDelim_p; // current (trailing) delimiter
    const char     *d_end_p;       // one past end of input; 0 for '(char *)'
    bool            d_endFlag;     // set 'true' when cursor at end of input

  private:
    // PRIVATE MANIPULATORS
    void resetImpl(const char *input, const char *endOfInput);
        // Rebind this object to refer to the specified sequence of 'input'
        // characters ending at the specified 'endOfInput' pointer.  The state
        // of the tokenizer following this call is *as* *if* it had been
        // constructed with 'input' and its current sets of *soft* and *hard*
        // delimiter characters.  Note that the behavior is undefined if this
        // object is used in any way (other than to reset or destroy it) after
        // its underlying 'input' string is modified.

  private:
    // NOT IMPLEMENTED
    Tokenizer(const Tokenizer&) BSLS_KEYWORD_DELETED;
    Tokenizer& operator=(const Tokenizer&) BSLS_KEYWORD_DELETED;
    Tokenizer& operator++(int) BSLS_KEYWORD_DELETED;

  public:
    // TYPES
    typedef TokenizerIterator iterator;

    // CREATORS
    Tokenizer(const char              *input, const bsl::string_view& soft);
    Tokenizer(const bsl::string_view&  input, const bsl::string_view& soft);
    Tokenizer(const char              *input,
              const bsl::string_view&  soft,
              const bsl::string_view&  hard);
    Tokenizer(const bsl::string_view&  input,
              const bsl::string_view&  soft,
              const bsl::string_view&  hard);
        // Create a 'Tokenizer' object bound to the specified sequence of
        // 'input' characters having the specified set of (unique) 'soft'
        // delimiter characters to be used to separate *tokens* (i.e., maximal
        // sequence of non-delimiter characters) in 'input'.  Optionally
        // specify a disjoint set of (unique) 'hard' delimiter characters to be
        // used to explicitly terminate tokens.  Delimiters within 'input'
        // consist of a maximal sequence of one or more delimiter characters,
        // at most one of which may be *hard*; when there is a contiguous
        // sequence of delimiter characters containing two or more *hard*
        // delimiter characters in 'input', any intervening *soft* delimiter
        // characters are associated with the previous (*hard*) delimiter.  Any
        // leading soft delimiter characters -- i.e., those preceding the first
        // *token* or *hard* delimiter character (referred to as the *leader*)
        // -- are available immediately after construction via the
        // 'previousDelimiter' method.  The behavior is undefined unless all
        // supplied delimiter characters are unique.  Note that the behavior is
        // also undefined if this object is used in any way (other than to
        // reset or destroy it) after its underlying 'input' string is
        // modified.  Also note that the current token and (trailing) delimiter
        // may be accessed only while this object is in the valid state;
        // however, the previous delimiter (or *leader*) is always accessible.
        // Also note that all token and delimiter strings are returned as
        // references into the underlying 'input' string, and hence remain
        // valid so long as that string is not modified or destroyed --
        // irrespective of the state (or even the existence) of this object.
        // Finally note that supplying a default constructed 'string_view' is
        // equivalent to supplying an empty c-string (i.e., "").

    ~Tokenizer();
        // Destroy this object.

    // MANIPULATORS
    Tokenizer& operator++();
        // Advance the iteration state of this object to refer to the next
        // sequence of previous delimiter, current token, and current
        // (trailing) delimiter in the underlying input sequence, and return a
        // reference providing modifiable access to this object.  The current
        // delimiter reference becomes the previous one.  If there is another
        // token remaining in the input, the current token and delimiter are
        // updated to refer to the respective new token and (trailing)
        // delimiter values -- either of which (but not both) might be empty.
        // If there are no tokens remaining in the input, the iteration state
        // of this object becomes invalid.  The behavior is undefined unless
        // the iteration state of this object is initially valid, or if the
        // underlying input has been modified or destroyed since this object
        // was most recently reset (or created).

    void reset(const char *input);
    void reset(const bsl::string_view& input);
        // Rebind this object to refer to the specified sequence of 'input'
        // characters.  The state of the tokenizer following this call is *as*
        // *if* it had been constructed with 'input' and its current sets of
        // *soft* and *hard* delimiter characters.  The behavior is
        // undefined if this object is used in any way (other than to reset or
        // destroy it) after its underlying 'input' string is modified.  Note
        // that supplying a default constructed 'string_view' is equivalent to
        // supplying an empty c-string (i.e., "").

    // ACCESSORS
    bool hasPreviousSoft() const;
        // Return 'true' if the previous delimiter (or *leader*) contains a
        // *soft* delimiter character, and 'false' otherwise.  The behavior is
        // undefined if the underlying input itself has been modified or
        // destroyed since this object was most recently reset (or created).

    bool hasTrailingSoft() const;
        // Return 'true' if the current (trailing) delimiter contains a *soft*
        // delimiter character, and 'false' otherwise.  The behavior is
        // undefined if the iteration state of this object is initially
        // invalid, or if the underlying input itself has been modified or
        // destroyed since this object was most recently reset (or created).

    bool isPreviousHard() const;
        // Return 'true' if the previous delimiter contains a *hard-delimiter*
        // character, and 'false' otherwise.  The behavior is undefined if the
        // underlying input itself has been modified or destroyed since this
        // object was most recently reset (or created).

    bool isTrailingHard() const;
        // Return 'true' if the current (trailing) delimiter contains a *hard*
        // delimiter character, and 'false' otherwise.  The behavior is
        // undefined if the iteration state of this object is initially
        // invalid, or if the underlying input itself has been modified or
        // destroyed since this object was most recently reset (or created).

    bool isValid() const;
        // Return 'true' if the iteration state of this object is valid, and
        // 'false' otherwise.  Note that the behavior of advancing the
        // iteration state as well as accessing the current token or (trailing)
        // delimiter is undefined unless the current iteration state of this
        // object is valid.

    bslstl::StringRef previousDelimiter() const;
        // Return a reference to the non-modifiable previous delimiter (or
        // *leader*) in the input string.  The behavior is undefined if the
        // underlying input has been modified or destroyed since this object
        // was most recently reset (or created).

    bslstl::StringRef token() const;
        // Return a reference to the non-modifiable current token (i.e.,
        // maximal sequence of non-delimiter characters) in the input string.
        // The returned reference remains valid so long as the underlying input
        // is not modified or destroyed -- irrespective of the state (or
        // existence) of this object.  The behavior is undefined unless the
        // iteration state of this object is initially valid, or if the
        // underlying input has been modified or destroyed since this object
        // was most recently reset (or created).


    bslstl::StringRef trailingDelimiter() const;
        // Return a reference to the non-modifiable current (trailing)
        // delimiter (maximal sequence of one or more delimiter characters
        // containing at most one *hard* delimiter character) in the input
        // string.  The returned reference remains valid so long as the
        // underlying input is not modified or destroyed -- irrespective of the
        // state (or existence) of this object.  The behavior is undefined
        // unless the iteration state of this object is initially valid, or if
        // the underlying input has been modified or destroyed since this
        // object was most recently reset (or created).


                         // iterators
    iterator begin() const;
        // Return an iterator referring to the first token in this object's
        // input string (the past-the-end iterator if this object iteration
        // state is initially invalid).  This reference remains valid as long
        // as the underlying input has not been modified or destroyed since
        // this object was most recently reset (or created).

    iterator end() const;
        // Return an iterator referring to position beyond the last token in
        // this object's input string.  This reference remains valid as long as
        // the underlying input has not been modified or destroyed since this
        // object was most recently reset (or created).
};

// FREE OPERATORS
const Tokenizer operator++(Tokenizer& object, int);

// ============================================================================
//                               INLINE DEFINITIONS
// ============================================================================

                        // --------------------------
                        // class bdlb::Tokenizer_Data
                        // --------------------------
// ACCESSORS
inline
int Tokenizer_Data::inputType(char character) const
{
    return d_charTypes[static_cast<unsigned char>(character)];
}

                        // ---------------------------
                        // class bdlb::Tokenizer_Proxy
                        // ---------------------------
// CREATORS
inline
bdlb::Tokenizer_Proxy::Tokenizer_Proxy(const bsl::string_view &obj)
: d_obj(obj)
{
}

// OPERATORS
inline
const bslstl::StringRef *bdlb::Tokenizer_Proxy::operator->() const
{
    return &d_obj;
}

                        // -----------------------------
                        // class bdlb::TokenizerIterator
                        // -----------------------------
// ACCESSORS
inline
const bslstl::StringRef TokenizerIterator::operator*() const
{
    // Called on invalid iterator
    BSLS_REVIEW(!d_endFlag);
    return bslstl::StringRef(d_token_p, d_postDelim_p);
}

inline
Tokenizer_Proxy TokenizerIterator::operator->() const
{
    // Called on invalid iterator
    BSLS_REVIEW(!d_endFlag);
    return Tokenizer_Proxy(this->operator*());
}

                        // ---------------------
                        // class bdlb::Tokenizer
                        // ---------------------
// ACCESSORS
inline
bool Tokenizer::isValid() const
{
    return !d_endFlag;
}

inline
bslstl::StringRef Tokenizer::previousDelimiter() const
{
    return bslstl::StringRef(d_prevDelim_p, d_token_p);
}

inline
bslstl::StringRef Tokenizer::token() const
{
    // Called on invalid tokenizer
    BSLS_REVIEW(!d_endFlag);
    return bslstl::StringRef(d_token_p, d_postDelim_p);
}

inline
bslstl::StringRef Tokenizer::trailingDelimiter() const
{
    // Called on invalid tokenizer
    BSLS_REVIEW(!d_endFlag);
    return bslstl::StringRef(d_postDelim_p, d_cursor_p);
}

}  // close package namespace

// FREE OPERATORS
inline
bool bdlb::operator==(const bdlb::TokenizerIterator& lhs,
                      const bdlb::TokenizerIterator& rhs)
{
    // Fast path decision
    if (lhs.d_endFlag != rhs.d_endFlag) {
        return false;                                                 // RETURN
    }

    // Comparing end iterators
    if (lhs.d_endFlag && rhs.d_endFlag) {
        return true;                                                  // RETURN
    }

    return  lhs.d_token_p == rhs.d_token_p;
}

inline
bool bdlb::operator!=(const bdlb::TokenizerIterator& lhs,
                      const bdlb::TokenizerIterator& rhs)
{
    return !(lhs == rhs);
}

inline
const bdlb::TokenizerIterator bdlb::operator++(bdlb::TokenizerIterator& object,
                                               int)
{
    bdlb::TokenizerIterator tmp(object);
    ++object;
    return tmp;
}

}  // close enterprise namespace

#endif

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
