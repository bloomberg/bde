// bdeut_strtokenrefiter.h                                            -*-C++-*-
#ifndef INCLUDED_BDEUT_STRTOKENREFITER
#define INCLUDED_BDEUT_STRTOKENREFITER

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide access to user-described tokens via string references.
//
//@CLASSES:
//   bdeut_StrTokenRefIter: token iterator for a non-modifiable string
//
//@SEE_ALSO: bdeut_stringref, bdeut_strtokeniter
//
//@AUTHOR: John Lakos (jlakos)
//
//@DESCRIPTION: This component implements a tokenizing iterator,
// 'bdeut_StrTokenRefIter', that provides non-destructive sequential access to
// the tokens in a given input character string specified as either a
// 'const char *' or 'const bdeut_StringRef&'.  The tokens are delimited by one
// or more user-described non-null characters in one or both of two delimiter
// sets: *soft* and *hard*.  In iterating through the input string, the
// iterator also keeps track of, and provides access to, the delimiters.
//
// Access to all tokens and delimiters is provided via 'bdeut_StringRef'.  The
// string references (i.e., instances of 'bdeut_StringRef') that are returned
// by the 'bdeut_StrTokenRefIter' accessors 'operator()', 'delimiter', and
// 'previousDelimiter' remain valid for as long as the input string supplied to
// the iterator upon construction (or 'reset') remains valid.  Contrast this
// behavior with that of the tokenizing iterator supplied by
// 'bdeut_strtokeniter' (namely 'bdeut_StrTokenIter') which expressly
// overwrites the contents of token and delimiter "buffers" each time an
// instance of that token iterator is advanced (via 'operator++').
//
///Definitions
///-----------
// A *delimiter* is a contiguous sequence of one or more delimiter characters
// that serves to separate or explicitly terminate user-described tokens.
//
// A *soft* *delimiter* is a contiguous sequence of soft delimiter characters
// occurring in the user-specified input string between a token character and
// (1) the beginning of the input (also called the leader), (2) the end of
// the input, or (3) another token character.
//
// A *hard* *delimiter* is a maximal sequence of remaining (i.e., not
// previously consumed) contiguous delimiter characters containing exactly
// one hard delimiter character occurring in the user-specified input.
//
// A *non-null* *token* is a maximal sequence of one or more non-delimiter
// characters occurring in the user-specified input.
//
// A *null* *token* is a zero-length sequence implied before a hard delimiter
// when that delimiter is not preceded by a token character in the
// user-specified input.
//
///Overview
///--------
// Soft delimiters are used in applications where consecutive delimiter
// characters need to be treated as one delimiter.  For example, declaring
// the space character (" ") as the soft delimiter character and parsing:
//..
//    "The   lazy dog"
//..
// should produce three tokens {"The", "lazy", "dog"} and not five tokens
// {"The", "", "", "lazy", "dog"}.  Hard delimiters are used in applications
// where consecutive delimiter characters imply null tokens and need to be
// treated as multiple delimiters.  For example, declaring forward-slash ("/")
// as the delimiter character and parsing the date "06//68" should produce
// three tokens {"06", "", "68"}, and not two tokens {"06", "68"}.  To clarify
// the issue further, consider the following string:
//..
//    "\t These/are /  /\nthe   good-old  / \tdays:: \n"
//..
// Assuming " \t\n" are soft delimiter characters and "/:" are hard delimiter
// characters, the following sequence of "previous delimiter", "token", and
// "current delimiter" values is produced:
//..
//    | Previous   | Token      | Delimiter |
//    +------------+------------+-----------+
//    | "\t "      | "These"    | "/"       |
//    | "/"        | "are"      | " /  "    |
//    | " /  "     | ""         | "/\n"     |
//    | "/\n"      | "the"      | "   "     |
//    | "   "      | "good-old" | "  / \t"  |
//    | "  / \t"   | "days"     | ":"       |
//    | ":"        | ""         | ": \n"    |
//    +------------+------------+-----------+
//..
// Note that a contiguous sequence of soft delimiter characters between two
// hard delimiter characters (e.g., " /  /\n" in the example above) is always
// part of the first delimiter and not the second (i.e., {" /  ", "/\n"}
// rather than {" /", "  /\n"}).  The following table provides additional
// examples of how input strings are parsed:
//..
//  Legend: " " = any soft delimiter character
//          "@" = any hard delimiter character
//          "x" = any non-delimiter character
//
//    Input     Iterator
//  | String  | State    | Previous | Token   | Delimiter | Initial Character |
//  +---------+----------+----------+---------+-----------+-------------------+
//  | ""      | invalid  | ""       | ""      | ""        | None              |
//  +---------+----------+----------+---------+-----------+-------------------+
//  | " "     | invalid  | " "      | ""      | ""        | Soft delimiter    |
//  +---------+----------+----------+---------+-----------|                   |
//  | "  "    | invalid  | "  "     | ""      | ""        | (upon             |
//  | " @"    | valid    | " "      | ""      | "@"       |  construction or  |
//  +---------+----------+----------+---------+-----------|  reset only)      |
//  | " x "   | valid    | " "      | "x"     | " "       |                   |
//  | "  x"   | valid    | "  "     | "x"     | ""        |                   |
//  | " xx"   | valid    | " "      | "xx"    | ""        |                   |
//  | " @x"   | valid    | " "      | ""      | "@"       |                   |
//  | " @ "   | valid    | " "      | ""      | "@ "      |                   |
//  | " @@"   | valid    | " "      | ""      | "@"       |                   |
//  +---------+----------+----------+---------+-----------+-------------------+
//  | "x"     | valid    | ""       | "x"     | ""        | Token             |
//  | "xx"    | valid    | ""       | "xx"    | ""        |                   |
//  +---------+----------+----------+---------+-----------|                   |
//  | "x x"   | valid    | ""       | "x"     | " "       |                   |
//  | "xx "   | valid    | ""       | "xx"    | " "       |                   |
//  | "x  "   | valid    | ""       | "x"     | "  "      |                   |
//  | "x@ "   | valid    | ""       | "x"     | "@ "      |                   |
//  | "x@x"   | valid    | ""       | "x"     | "@"       |                   |
//  | "x@@"   | valid    | ""       | "x"     | "@"       |                   |
//  +---------+----------+----------+---------+-----------|                   |
//  | "x  x"  | valid    | ""       | "x"     | "  "      |                   |
//  | "x @x"  | valid    | ""       | "x"     | " @"      |                   |
//  | "x@ x"  | valid    | ""       | "x"     | "@ "      |                   |
//  | "x@@x"  | valid    | ""       | "x"     | "@"       |                   |
//  +---------+----------+----------+---------+-----------|                   |
//  | "x @ x" | valid    | ""       | "x"     | " @ "     |                   |
//  | "x @ @" | valid    | ""       | "x"     | " @ "     |                   |
//  +---------+----------+----------+---------+-----------+-------------------+
//  | "@"     | valid    | ""       | ""      | "@"       | Hard delimiter    |
//  +---------+----------+----------+---------+-----------|                   |
//  | "@ "    | valid    | ""       | ""      | "@ "      |                   |
//  | "@@"    | valid    | ""       | ""      | "@"       |                   |
//  +---------+----------+----------+---------+-----------|                   |
//  | "@  "   | valid    | ""       | ""      | "@  "     |                   |
//  | "@ x"   | valid    | ""       | ""      | "@ "      |                   |
//  | "@ @"   | valid    | ""       | ""      | "@ "      |                   |
//  | "@x "   | valid    | ""       | ""      | "@"       |                   |
//  | "@xx"   | valid    | ""       | ""      | "@"       |                   |
//  | "@x@"   | valid    | ""       | ""      | "@"       |                   |
//  +---------+----------+----------+---------+-----------+-------------------+
//..
// Note that Token/Delimiter string pairs in subsequent iterations will never
// begin with a soft delimiter character.
//
// Tokens are retrieved with 'operator()'.  Corresponding (trailing) delimiter
// characters may be retrieved with the 'delimiter' method.  Leading soft
// delimiter characters preceding the first token or hard delimiter character
// (i.e., the "leader") are initially accumulated in the "previous" delimiter
// and, prior to the first invocation of the '++' operator, may be retrieved
// using the 'previousDelimiter' method.  The token and delimiter access
// methods each return 'bdeut_StringRef'.
//
// Each call to the '++' operator that does not result in an invalid iterator
// state updates the "token" and both the delimiters ("delimiter" and
// "previous").  (After advancing the iteration state with '++', the current
// delimiter becomes the previous delimiter.)  The 'isHard' and
// 'isPreviousHard' methods are used to query the delimiters for the presence
// of a hard delimiter character.  Similarly, the 'hasSoft' and
// 'hasPreviousSoft' methods are used to query the delimiters for the presence
// of any soft delimiter characters.  The 'reset' method allows the use of an
// existing token iterator for parsing multiple strings.
//
///Usage
///-----
// The following usage of 'bdeut_StrTokenRefIter' mimics the standard C library
// function 'strtok' on a non-modifiable 'const char *' string:
//..
//    void listTokens(const char *input, const char *delims, bool isVerbose)
//    {
//        for (bdeut_StrTokenRefIter it(input, delims); it; ++it) {
//            if (isVerbose) bsl::cout << it() << ':';
//        }
//        if (isVerbose) bsl::cout << bsl::endl;
//    }
//..
// Applying the 'listTokens' function to the string "\n Now\t\tis the time...",
// assuming a (soft) delimiter string of " \t\n", produces the following
// output:
//..
//    Now:is:the:time...:
//..
// To introduce the notion of hard delimiters, consider the standard (American)
// date format "mm/dd/yy" (albeit with an ambiguous 2-digit year) and the
// following 'parseDate' function:
//..
//    int parseDate(const char *dateString, bool isVerbose)
//    {
//        int numFields = 0;
//        for (bdeut_StrTokenRefIter it(dateString, " \t", "/"); it; ++it) {
//            ++numFields;
//            if (isVerbose) bsl::cout << it() << ':';
//        }
//        if (isVerbose) bsl::cout << bsl::endl;
//        return numFields;
//    }
//..
// Applying 'parseDate' to "3/8/59" produces "3:8:59:" and returns 3.  Note
// that the behavior is identical for each of the following strings:
//..
//    "3 8 59"
//    "3  8/59"
//    "3 / 8 / 59"
//    "\t\t  3\t\t8  /\t59 \t\t  "
//    "3/8/59/"                        // see 'checkDateFormat' below.
//..
// The following strings, however, produce different results:
//..
//    "3//8/59"               --> "3::8:59:",   return 4;
//    "/3/8/59"               --> ":3:8:59:",   return 4;
//    "3 8 / / 59"            --> "3:8::59:",   return 4;
//    "3 8 59// "             --> "3:8:59::",   return 4;
//
//..
// Had we desired, the entire input string could have been reconstructed by
// concatenating with the initial "previous delimiter", successive values of
// the current "token" and current (trailing) "delimiter" as follows:
//..
//    #include <bdeut_strtokenrefiter.h>
//    #include <cassert>
//    #include <cstring>             // 'strcmp'
//    #include <strstream>           // 'ostrstream'
//
//    void reconstruct(const char *input,
//                     const char *softDelims,
//                     const char *hardDelims)
//    {
//        bsl::ostrstream ostr;
//        bdeut_StrTokenRefIter it(input, softDelims, hardDelims);
//        ostr << it.previousDelimiter();    // only once per string
//        for (; it; ++it) {
//            ostr << it();                  // current token
//            ostr << it.delimiter();        // current (trailing) delimiter
//        }
//        ostr << bsl::ends;
//        assert(0 == bsl::strcmp(ostr.str(), input));
//        ostr.rdbuf()->freeze(0);           // return ownership of str to ostr
//    }
//..
// In the 'parseDate' function above, we allowed the format "mm/dd/yy/"
// (with a trailing hard delimiter) to go unnoticed.  If instead we wish
// to check that there are exactly 3 non-null fields and no leading or
// trailing hard delimiters, the following code fragment suffices:
//..
//    int checkDateFormat(const char *input)
//    {
//        enum { GOOD = 0, BAD = 1 };
//        bdeut_StrTokenRefIter it(input, " \t", "/");
//                                          // construct iterator outside 'for'
//        for (; it; ++it) {
//            if (!it().length()) return BAD;       // empty token
//        }
//
//        return it.isHard() ? BAD : GOOD;  // check last delimiter only
//    }
//..
// Applying 'checkDateFormat' to "3/8/59/" will return a 'BAD' status.  If we
// wish to ensure that there are no leading or trailing delimiters of any kind,
// we would add the line:
//..
//        if (it.previousDelimiter().length()) return BAD;
//..
// before the loop and replace the final 'return' statement in the code
// fragment above with the following:
//..
//        return it.delimiter().length() ? BAD : GOOD;
//..
// If we feel compelled to disallow soft delimiter characters entirely and
// require tokens to be separated only by a hard delimiter character, we also
// add the following line in the body of the loop:
//..
//            if (it.hasSoft()) return BAD; // disallow "11/3 /98", for example
//                                          //               ^
//..
// Finally, note that it is possible to nest iterators so that the output of
// one iterator is used as the input to another iterator.  For example, we
// could write:
//..
//      const char *string = "The quick brown fox\n"
//                           "jumped over the\n"
//                           "small lazy dog.\n";
//
//      for (bdeut_StrTokenRefIter it1(string, "\n"); it1; ++it1) {
//          bsl::cout << '@';
//          for (bdeut_StrTokenRefIter it2(it1(), " "); it2; ++it2) {
//              bsl::cout << it2() << ':';
//          }
//      }
//..
// which produces the following output:
//..
//    @The:quick:brown:fox:@jumped:over:the:@small:lazy:dog.:
//..

#ifndef INCLUDED_BDESCM_VERSION
#include <bdescm_version.h>
#endif

#ifndef INCLUDED_BDEUT_STRINGREF
#include <bdeut_stringref.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

namespace BloombergLP {

                        // ===========================
                        // class bdeut_StrTokenRefIter
                        // ===========================

class bdeut_StrTokenRefIter {
    // Provide read-only sequential access to the user-described tokens in a
    // 'const char *' string or 'bdeut_StringRef'.  Access to tokens, as well
    // as to previous and (trailing) delimiters, is provided efficiently via
    // 'bdeut_StringRef'.

    // PRIVATE TYPES
    enum { BDEUT_TABLE_SIZE = 256 };

    // DATA
    const char *d_cursor_p;                // cursor into original string
    const char *d_prevDelim_p;             // ptr to previous delimiter
    const char *d_token_p;                 // ptr to (current) token
    const char *d_postDelim_p;             // ptr to (trailing) delimiter
    const char *d_end_p;                   // ptr to one past end of string
          char  d_delimTable[BDEUT_TABLE_SIZE];  // table of delimiter
                                                 // characters

  private:
    // NOT IMPLEMENTED
    bdeut_StrTokenRefIter(const bdeut_StrTokenRefIter&);
    bdeut_StrTokenRefIter& operator=(const bdeut_StrTokenRefIter&);

    bool operator==(const bdeut_StrTokenRefIter&) const;
    bool operator!=(const bdeut_StrTokenRefIter&) const;

  public:
    // CREATORS
    bdeut_StrTokenRefIter(const char *input, const char *softDelims);
    bdeut_StrTokenRefIter(const char *input,
                          const char *softDelims,
                          const char *hardDelims);
    bdeut_StrTokenRefIter(const bdeut_StringRef&  input,
                          const char             *softDelims);
    bdeut_StrTokenRefIter(const bdeut_StringRef&  input,
                          const char             *softDelims,
                          const char             *hardDelims);
        // Create a token iterator for the specified 'input' string in which
        // each token is delimited by one or more (non-null) characters defined
        // in the specified 'softDelims' and the optionally-specified
        // 'hardDelims' delimiter strings; 'hardDelims' need not be specified
        // if no hard delimiters are required.  Any soft delimiter characters
        // between two hard delimiters are associated with the first hard
        // delimiter; leading soft delimiter characters (i.e., those preceding
        // the first "token" or hard delimiter character) are available
        // immediately after construction via the 'previousDelimiter' method.
        // Each of the three 'const char *' arguments may independently be
        // empty ("") or null (0).  Supplying a null 'input' creates an
        // "unbound" iterator in an invalid state.  If 'input' is empty or
        // contains only soft delimiter characters, the iterator immediately
        // reaches an invalid state, but the "previous" delimiter (if any) is
        // still accessible.  The behavior is undefined if 'input' is changed
        // while this iterator is bound to it, or if an 'input' supplied via
        // 'bdeut_StringRef' contains a null character.  Note that any
        // character specified as both a soft delimiter character and a hard
        // delimiter character is regarded as a *hard* delimiter character
        // (repeating a character in the same delimiter set is redundant and
        // has no additional effect).  Also note that an 'input' supplied via
        // 'bdeut_StringRef' need not be null-terminated.

    ~bdeut_StrTokenRefIter();
        // Destroy this token iterator.

    // MANIPULATORS
    void operator++();
        // Advance the state of the iteration of this token iterator.  When
        // this function returns with the iterator in a valid state, the
        // (current) delimiter becomes the previous delimiter and both the
        // "token" and "delimiter" references are updated to refer to the
        // respective new token and (trailing) delimiter values -- either of
        // which (but not both) could be the empty string.  The behavior is
        // undefined unless the iterator is initially in a valid state.

    void reset(const char *input);
    void reset(const bdeut_StringRef& input);
        // Reset this token iterator to refer to the specified 'input' string
        // (which may be null).  The state of the iterator following 'reset' is
        // *as if* it had been constructed with 'input' and the soft and hard
        // delimiter character sets currently in use by the iterator.  The
        // behavior is undefined if 'input' is changed while this iterator is
        // bound to it, or if an 'input' supplied via 'bdeut_StringRef'
        // contains a null character.  Note that an 'input' supplied via
        // 'bdeut_StringRef' need not be null-terminated.

    // ACCESSORS
    operator const void *() const;
        // Return non-zero if this token iterator is in a valid state, and 0
        // otherwise.

    bdeut_StringRef operator()() const;
        // Return a reference to the non-modifiable current token of this token
        // iterator.  The reference returned remains valid even after the state
        // of the iteration of this iterator is advanced, enabling the client
        // to avoid copying the token.  The behavior is undefined unless this
        // token iterator is in a valid state.

    bdeut_StringRef delimiter() const;
        // Return a reference to the non-modifiable current (trailing)
        // delimiter of this token iterator.  The reference returned remains
        // valid even after the state of the iteration of this iterator is
        // advanced, enabling the client to avoid copying the delimiter.

    bdeut_StringRef previousDelimiter() const;
        // Return a reference to the non-modifiable previous delimiter (i.e.,
        // the delimiter preceding the current token) of this token iterator.
        // The reference returned remains valid even after the state of the
        // iteration of this iterator is advanced, enabling the client to avoid
        // copying the previous delimiter.

    bool isHard() const;
        // Return 'true' if there is a hard delimiter character in the
        // (current) "delimiter" of this token iterator, and 'false' otherwise.

    bool isPreviousHard() const;
        // Return 'true' if there is a hard delimiter character in the
        // "previous" delimiter of this token iterator, and 'false' otherwise.

    bool hasSoft() const;
        // Return 'true' if there are any soft delimiter characters in the
        // (current) "delimiter" of this token iterator, and 'false' otherwise.

    bool hasPreviousSoft() const;
        // Return 'true' if there are any soft delimiter characters in the
        // "previous" delimiter of this token iterator, and 'false' otherwise.
};

// ===========================================================================
//                      INLINE FUNCTION DEFINITIONS
// ===========================================================================

                        // ---------------------------
                        // class bdeut_StrTokenRefIter
                        // ---------------------------

// ACCESSORS
inline
bdeut_StrTokenRefIter::operator const void *() const
{
    return (const void *)d_cursor_p;
}

inline
bdeut_StringRef bdeut_StrTokenRefIter::operator()() const
{
    BSLS_ASSERT_SAFE(d_cursor_p);

    return bdeut_StringRef(d_token_p, d_postDelim_p);
}

inline
bdeut_StringRef bdeut_StrTokenRefIter::delimiter() const
{
    return bdeut_StringRef(d_postDelim_p, d_cursor_p ? d_cursor_p : d_end_p);
}

inline
bdeut_StringRef bdeut_StrTokenRefIter::previousDelimiter() const
{
    return bdeut_StringRef(d_prevDelim_p, d_token_p);
}

}  // close namespace BloombergLP



#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2007
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
