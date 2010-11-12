// bdepcre_regex.t.cpp    -*-C++-*-

#include <bdepcre_regex.h>

#include <bslma_defaultallocatorguard.h>    // for testing only
#include <bslma_testallocator.h>            // for testing only

#include <bsl_cstdlib.h>
#include <bsl_iostream.h>
#include <bsl_string.h>
#include <bsl_utility.h>
#include <bsl_vector.h>

using namespace BloombergLP;
using namespace bsl;  // automatically added by script

//=============================================================================
//                                 TEST PLAN
//-----------------------------------------------------------------------------
//                                  Overview
//                                  --------
// The component under test is a mechanism that wraps the Perl Compatible
// Regular Expression (PCRE) C library.  In this test, we want to ensure that
// the PCRE library is "plugged in" correctly and that the component conforms
// to the documented interface.  We will not test the implementation details of
// the PCRE library.
//
// After breathing the component, we will test the basic mechanism of preparing
// and clearing a regular expression object.  We will then test that the
// matching functions are plugged in correctly.  Then we will test the various
// optional flags that can be passed when preparing a regular expression
// object.  We will then test that subpatterns are supported correctly and that
// the captured substrings are returned correctly.  Finally, we will test the
// usage example from the component's header file.
//-----------------------------------------------------------------------------
// CREATORS
// [ 2] bdepcre_RegEx(bslma_Allocator *basicAllocator = 0);
// [ 2] ~bdepcre_RegEx();

// MANIPULATORS
// [ 2] void clear();
// [ 2] int prepare(const char*, int, const char**, int*);
//
// ACCESSORS
// [4-6] int flags() const;
// [ 2] bool isPrepared() const;
// [ 3] int match(const char *subject, ...) const;
// [ 3] int match(bsl::pair<int, int> *result, ...) const;
// [3,7] int match(bsl::vector<bsl::pair<int, int> > *result, ...) const;
// [ 7] int numSubpatterns() const;
// [ 2] const bsl::string& pattern() const;
// [ 7] int subpatternIndex(const char *name) const;
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 4] BDEPCRE_FLAG_CASELESS
// [ 5] BDEPCRE_FLAG_MULTILINE
// [ 6] BDEPCRE_FLAG_UTF8
// [ 8] ALLOCATION PROPAGATION
// [ 9] USAGE EXAMPLE

//=============================================================================
//                      STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------
static int testStatus = 0;

static void aSsErT(int c, const char *s, int i)
{
    if (c) {
        cout << "Error " << __FILE__ << "(" << i << "): " << s
             << "    (failed)" << endl;
        if (0 <= testStatus && testStatus <= 100) ++testStatus;
    }
}

#define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }

//=============================================================================
//                       TEMPLATIZED OUTPUT FUNCTIONS
//=============================================================================

template <class T>
void printValue(ostream& out, const T& value)
{
    out << value;
}

static const char *printableCharacters[256]=
{
    "\\x00" , //   0   0
    "\\x01" , //   1   1
    "\\x02" , //   2   2
    "\\x03" , //   3   3
    "\\x04" , //   4   4
    "\\x05" , //   5   5
    "\\x06" , //   6   6
    "\\x07" , //   7   7
    "\\x08" , //   8   8 - BACKSPACE
    "\\t" ,   //   9   9 - TAB
    "\\n" ,   //  10   a - LF
    "\\x0b" , //  11   b
    "\\x0c" , //  12   c
    "\\r"   , //  13   d - CR
    "\\x0e" , //  14   e
    "\\x0f" , //  15   f
    "\\x10" , //  16  10
    "\\x11" , //  17  11
    "\\x12" , //  18  12
    "\\x13" , //  19  13
    "\\x14" , //  20  14
    "\\x15" , //  21  15
    "\\x16" , //  22  16
    "\\x17" , //  23  17
    "\\x18" , //  24  18
    "\\x19" , //  25  19
    "\\x1a" , //  26  1a
    "\\x1b" , //  27  1b
    "\\x1c" , //  28  1c
    "\\x1d" , //  29  1d
    "\\x1e" , //  30  1e
    "\\x1f" , //  31  1f
    " "     , //  32  20 - SPACE
    "!"     , //  33  21 - !
    "\\\""  , //  34  22 - "
    "#"     , //  35  23 - #
    "$"     , //  36  24 - $
    "%"     , //  37  25 - %
    "&"     , //  38  26 - &
    "'"     , //  39  27 - '
    "("     , //  40  28 - (
    ")"     , //  41  29 - )
    "*"     , //  42  2a - *
    "+"     , //  43  2b - +
    ","     , //  44  2c - ,
    "-"     , //  45  2d - -
    "."     , //  46  2e - .
    "/"     , //  47  2f - /
    "0"     , //  48  30 - 0
    "1"     , //  49  31 - 1
    "2"     , //  50  32 - 2
    "3"     , //  51  33 - 3
    "4"     , //  52  34 - 4
    "5"     , //  53  35 - 5
    "6"     , //  54  36 - 6
    "7"     , //  55  37 - 7
    "8"     , //  56  38 - 8
    "9"     , //  57  39 - 9
    ":"     , //  58  3a - :
    ";"     , //  59  3b - ;
    "<"     , //  60  3c - <
    "="     , //  61  3d - =
    ">"     , //  62  3e - >
    "?"     , //  63  3f - ?
    "@"     , //  64  40 - @
    "A"     , //  65  41 - A
    "B"     , //  66  42 - B
    "C"     , //  67  43 - C
    "D"     , //  68  44 - D
    "E"     , //  69  45 - E
    "F"     , //  70  46 - F
    "G"     , //  71  47 - G
    "H"     , //  72  48 - H
    "I"     , //  73  49 - I
    "J"     , //  74  4a - J
    "K"     , //  75  4b - K
    "L"     , //  76  4c - L
    "M"     , //  77  4d - M
    "N"     , //  78  4e - N
    "O"     , //  79  4f - O
    "P"     , //  80  50 - P
    "Q"     , //  81  51 - Q
    "R"     , //  82  52 - R
    "S"     , //  83  53 - S
    "T"     , //  84  54 - T
    "U"     , //  85  55 - U
    "V"     , //  86  56 - V
    "W"     , //  87  57 - W
    "X"     , //  88  58 - X
    "Y"     , //  89  59 - Y
    "Z"     , //  90  5a - Z
    "["     , //  91  5b - [
    "\\\\"  , //  92  5c - '\'
    "]"     , //  93  5d - ]
    "^"     , //  94  5e - ^
    "_"     , //  95  5f - _
    "`"     , //  96  60 - `
    "a"     , //  97  61 - a
    "b"     , //  98  62 - b
    "c"     , //  99  63 - c
    "d"     , // 100  64 - d
    "e"     , // 101  65 - e
    "f"     , // 102  66 - f
    "g"     , // 103  67 - g
    "h"     , // 104  68 - h
    "i"     , // 105  69 - i
    "j"     , // 106  6a - j
    "k"     , // 107  6b - k
    "l"     , // 108  6c - l
    "m"     , // 109  6d - m
    "n"     , // 110  6e - n
    "o"     , // 111  6f - o
    "p"     , // 112  70 - p
    "q"     , // 113  71 - q
    "r"     , // 114  72 - r
    "s"     , // 115  73 - s
    "t"     , // 116  74 - t
    "u"     , // 117  75 - u
    "v"     , // 118  76 - v
    "w"     , // 119  77 - w
    "x"     , // 120  78 - x
    "y"     , // 121  79 - y
    "z"     , // 122  7a - z
    "{"     , // 123  7b - {
    "|"     , // 124  7c - |
    "}"     , // 125  7d - }
    "~"     , // 126  7e - ~
    "\\x7f" , // 127  7f - DEL
    "\\x80" , // 128  80
    "\\x81" , // 129  81
    "\\x82" , // 130  82
    "\\x83" , // 131  83
    "\\x84" , // 132  84
    "\\x85" , // 133  85
    "\\x86" , // 134  86
    "\\x87" , // 135  87
    "\\x88" , // 136  88
    "\\x89" , // 137  89
    "\\x8a" , // 138  8a
    "\\x8b" , // 139  8b
    "\\x8c" , // 140  8c
    "\\x8d" , // 141  8d
    "\\x8e" , // 142  8e
    "\\x8f" , // 143  8f
    "\\x90" , // 144  90
    "\\x91" , // 145  91
    "\\x92" , // 146  92
    "\\x93" , // 147  93
    "\\x94" , // 148  94
    "\\x95" , // 149  95
    "\\x96" , // 150  96
    "\\x97" , // 151  97
    "\\x98" , // 152  98
    "\\x99" , // 153  99
    "\\x9a" , // 154  9a
    "\\x9b" , // 155  9b
    "\\x9c" , // 156  9c
    "\\x9d" , // 157  9d
    "\\x9e" , // 158  9e
    "\\x9f" , // 159  9f
    "\\xa0" , // 160  a0
    "\\xa1" , // 161  a1
    "\\xa2" , // 162  a2
    "\\xa3" , // 163  a3
    "\\xa4" , // 164  a4
    "\\xa5" , // 165  a5
    "\\xa6" , // 166  a6
    "\\xa7" , // 167  a7
    "\\xa8" , // 168  a8
    "\\xa9" , // 169  a9
    "\\xaa" , // 170  aa
    "\\xab" , // 171  ab
    "\\xac" , // 172  ac
    "\\xad" , // 173  ad
    "\\xae" , // 174  ae
    "\\xaf" , // 175  af
    "\\xb0" , // 176  b0
    "\\xb1" , // 177  b1
    "\\xb2" , // 178  b2
    "\\xb3" , // 179  b3
    "\\xb4" , // 180  b4
    "\\xb5" , // 181  b5
    "\\xb6" , // 182  b6
    "\\xb7" , // 183  b7
    "\\xb8" , // 184  b8
    "\\xb9" , // 185  b9
    "\\xba" , // 186  ba
    "\\xbb" , // 187  bb
    "\\xbc" , // 188  bc
    "\\xbd" , // 189  bd
    "\\xbe" , // 190  be
    "\\xbf" , // 191  bf
    "\\xc0" , // 192  c0
    "\\xc1" , // 193  c1
    "\\xc2" , // 194  c2
    "\\xc3" , // 195  c3
    "\\xc4" , // 196  c4
    "\\xc5" , // 197  c5
    "\\xc6" , // 198  c6
    "\\xc7" , // 199  c7
    "\\xc8" , // 200  c8
    "\\xc9" , // 201  c9
    "\\xca" , // 202  ca
    "\\xcb" , // 203  cb
    "\\xcc" , // 204  cc
    "\\xcd" , // 205  cd
    "\\xce" , // 206  ce
    "\\xcf" , // 207  cf
    "\\xd0" , // 208  d0
    "\\xd1" , // 209  d1
    "\\xd2" , // 210  d2
    "\\xd3" , // 211  d3
    "\\xd4" , // 212  d4
    "\\xd5" , // 213  d5
    "\\xd6" , // 214  d6
    "\\xd7" , // 215  d7
    "\\xd8" , // 216  d8
    "\\xd9" , // 217  d9
    "\\xda" , // 218  da
    "\\xdb" , // 219  db
    "\\xdc" , // 220  dc
    "\\xdd" , // 221  dd
    "\\xde" , // 222  de
    "\\xdf" , // 223  df
    "\\xe0" , // 224  e0
    "\\xe1" , // 225  e1
    "\\xe2" , // 226  e2
    "\\xe3" , // 227  e3
    "\\xe4" , // 228  e4
    "\\xe5" , // 229  e5
    "\\xe6" , // 230  e6
    "\\xe7" , // 231  e7
    "\\xe8" , // 232  e8
    "\\xe9" , // 233  e9
    "\\xea" , // 234  ea
    "\\xeb" , // 235  eb
    "\\xec" , // 236  ec
    "\\xed" , // 237  ed
    "\\xee" , // 238  ee
    "\\xef" , // 239  ef
    "\\xf0" , // 240  f0
    "\\xf1" , // 241  f1
    "\\xf2" , // 242  f2
    "\\xf3" , // 243  f3
    "\\xf4" , // 244  f4
    "\\xf5" , // 245  f5
    "\\xf6" , // 246  f6
    "\\xf7" , // 247  f7
    "\\xf8" , // 248  f8
    "\\xf9" , // 249  f9
    "\\xfa" , // 250  fa
    "\\xfb" , // 251  fb
    "\\xfc" , // 252  fc
    "\\xfd" , // 253  fd
    "\\xfe" , // 254  fe
    "\\xff"   // 255  ff
};

void printValue(ostream& out, const char* value)
    // Specialize for char*.  Need to expand \r, \n, \t and surround with
    // DQUOTE characters.
{
    out << '"';

    while (*value) {
        out << printableCharacters[(int)*value];
        ++value;
    }

    out << '"';
}

void printValue(ostream& out, const string& value)
    // Need to expand \r, \n, \t and surround with DQUOTE characters.
{
    printValue(out, value.c_str());
}

//=============================================================================
//                  STANDARD BDE LOOP-ASSERT TEST MACROS
//-----------------------------------------------------------------------------
#define LOOP_ASSERT(I,X) { \
   if (!(X)) { cout << #I << ": ";  printValue(cout, I);  cout << "\n";   \
               aSsErT(1, #X, __LINE__); } }

#define LOOP2_ASSERT(I,J,X) { \
   if (!(X)) { cout << #I << ": ";  printValue(cout, I);  cout << "\t";   \
               cout << #J << ": ";  printValue(cout, J);  cout << "\n";   \
               aSsErT(1, #X, __LINE__); } }

#define LOOP3_ASSERT(I,J,K,X) { \
   if (!(X)) { cout << #I << ": ";  printValue(cout, I);  cout << "\t";   \
               cout << #J << ": ";  printValue(cout, J);  cout << "\t";   \
               cout << #K << ": ";  printValue(cout, K);  cout << "\n";   \
               aSsErT(1, #X, __LINE__); } }

#define LOOP4_ASSERT(I,J,K,L,X) { \
   if (!(X)) { cout << #I << ": ";  printValue(cout, I);  cout << "\t";   \
               cout << #J << ": ";  printValue(cout, J);  cout << "\t";   \
               cout << #K << ": ";  printValue(cout, K);  cout << "\t";   \
               cout << #L << ": ";  printValue(cout, L);  cout << "\n";   \
               aSsErT(1, #X, __LINE__); } }

#define LOOP5_ASSERT(I,J,K,L,M,X) { \
   if (!(X)) { cout << #I << ": ";  printValue(cout, I);  cout << "\t";   \
               cout << #J << ": ";  printValue(cout, J);  cout << "\t";   \
               cout << #K << ": ";  printValue(cout, K);  cout << "\t";   \
               cout << #L << ": ";  printValue(cout, L);  cout << "\t";   \
               cout << #M << ": ";  printValue(cout, M);  cout << "\n";   \
               aSsErT(1, #X, __LINE__); } }

#define LOOP6_ASSERT(I,J,K,L,M,N,X) { \
   if (!(X)) { cout << #I << ": ";  printValue(cout, I);  cout << "\t";   \
               cout << #J << ": ";  printValue(cout, J);  cout << "\t";   \
               cout << #K << ": ";  printValue(cout, K);  cout << "\t";   \
               cout << #L << ": ";  printValue(cout, L);  cout << "\t";   \
               cout << #M << ": ";  printValue(cout, M);  cout << "\t";   \
               cout << #N << ": ";  printValue(cout, N);  cout << "\n";   \
               aSsErT(1, #X, __LINE__); } }

//=============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------
#define P(X) cout << #X " = "; printValue(cout, X); cout << endl;
                                                 // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = "; printValue(cout, X); cout << ", " << flush;
                                                           // P(X) without '\n'
#define L_ __LINE__                           // current Line number
#define T_ cout << "\t" << flush;             // Print tab w/o newline

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------
typedef bdepcre_RegEx Obj;

//=============================================================================
//                               USAGE EXAMPLE
//-----------------------------------------------------------------------------

///Usage
///-----
// The following snippets of code illustrate using this component to extract
// the text of the "Subject:" field from an Internet e-mail message (RFC822).
// The following 'parseSubject' function accepts an RFC822-compliant message of
// a specified length and returns the text of the message's subject in the
// 'result' "out" parameter:
//..
//  #include <bdepcre_regex.h>
//
//  using namespace BloombergLP;
//
    int parseSubject(bsl::string *result,
                     const char  *message,
                     int          messageLength)
//      // Parse the specified 'message' of the specified 'messageLength' for
//      // the "Subject:" field of 'message'.  Return 0 on success and load the
//      // specified 'result' with the text of the subject of 'message'; return
//      // a non-zero value otherwise with no effect on 'result'.
    {
//..
// The following is the regular expression that will be used to find the
// subject text of 'message'.  The "?P<subjectText>" syntax, borrowed from
// Python, allows us later to refer to a particular matched sub-pattern (i.e.,
// the text between the ':' and the '\r' in the "Subject:" field of the header)
// by the name "subjectText":
//..
        const char PATTERN[] = "^subject:(?P<subjectText>[^\r]*)";
//..
// We first need to compile 'PATTERN', using the 'prepare' method, in order to
// match subject strings against it.  In the event that 'prepare' fails, the
// first two arguments will be loaded with diagnostic information (an
// informational string and an index into the pattern at which the error
// occurred, respectively).  Two flags, 'bdepcre_RegEx::BDEPCRE_FLAG_CASELESS'
// and 'bdepcre_RegEx::BDEPCRE_FLAG_MULTILINE', are used in preparing the
// pattern since Internet message headers contain case-insensitive content as
// well as '\n' characters.  The 'prepare' method returns 0 on success, and a
// non-zero value otherwise:
//..
        bdepcre_RegEx regEx;
        bsl::string   errorMessage;
        int           errorOffset;

        int returnValue = regEx.prepare(&errorMessage,
                                        &errorOffset,
                                        PATTERN,
                                        bdepcre_RegEx::BDEPCRE_FLAG_CASELESS |
                                        bdepcre_RegEx::BDEPCRE_FLAG_MULTILINE);
        ASSERT(0 == returnValue);
//..
// Next we call 'match' supplying 'message' and its length.  The 'matchVector'
// will be populated with (offset, length) pairs describing substrings in
// 'message' that match the prepared 'PATTERN'.  All variants of the overloaded
// 'match' method return 0 if a match is found, and return a non-zero value
// otherwise:
//..
        bsl::vector<bsl::pair<int, int> > matchVector;
        returnValue = regEx.match(&matchVector, message, messageLength);

        if (0 != returnValue) {
            return returnValue;  // no match
        }
//..
// Next we pass "subjectText" to the 'subpatternIndex' method to obtain the
// index into 'matchVector' that describes how to locate the subject text
// within 'message'.  The text is then extracted from 'message' and assigned to
// the 'result' "out" parameter:
//..
        bsl::pair<int, int> capturedSubject =
                             matchVector[regEx.subpatternIndex("subjectText")];

        *result = bsl::string(&message[capturedSubject.first],
                              capturedSubject.second);

        return 0;
    }

//=============================================================================
//                  GLOBAL HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    int veryVerbose = argc > 3;
    int veryVeryVerbose = argc > 4;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;;

    bslma_TestAllocator testAllocator(veryVeryVerbose);

    switch (test) { case 0:  // Zero is always the leading case.
      case 10: {
        // --------------------------------------------------------------------
        // TESTING ?: non-matching grouping
        //
        // Concerns:
        //   That (?:  ) specifies grouping, without specifying a subpattern.
        //
        //   That a subpattern can be nested inside the grouping.
        // -------------------------------------------------------------------

        Obj mX(&testAllocator);
        int rc = mX.prepare(0,0,"XX(?:\\d\\d:)+ (\\S+)");
        ASSERT(0 == rc);
        bsl::vector<bsl::pair<int,int> > results;
        //                     0123456789012
        const char MATCH1[] = "XX12:23: WORD";
        const char MATCH2[] = "XX12:23:34WORD";
        ASSERT(0 == mX.match(&results, MATCH1, sizeof(MATCH1)-1));
        LOOP_ASSERT(results.size(), 2 == results.size());
        if (2 == results.size()) {
            LOOP_ASSERT(results[1].first, 9 == results[1].first);
        }

        rc = mX.prepare(0,0,"XX(?:\\d\\d:)+(?: |\\d\\d)(\\S+)");
        ASSERT(0 == mX.match(&results, MATCH1, sizeof(MATCH1)-1));
        LOOP_ASSERT(results.size(), 2 == results.size());
        if (2 == results.size()) {
            LOOP_ASSERT(results[1].first, 9 == results[1].first);
        }

        ASSERT(0 == mX.match(&results, MATCH2, sizeof(MATCH2)-1));
        LOOP_ASSERT(results.size(), 2 == results.size());
        if (2 == results.size()) {
            LOOP_ASSERT(results[1].first, 10 == results[1].first);
        }

        rc = mX.prepare(0,0,"XX(?:\\d\\d:)+(?: |(\\d\\d))(\\S+)");
        ASSERT(0 == mX.match(&results, MATCH1, sizeof(MATCH1)-1));
        // TBD...should have *ONE* subpattern match, but has two now
        LOOP_ASSERT(results.size(), 3 == results.size());
        if (3 == results.size()) {
            // TBD, this match probably shouldn't happen
            LOOP_ASSERT(results[1].first, -1 == results[1].first);
            // TBD...this should be the FIRST match, not second
            LOOP_ASSERT(results[2].first, 9 == results[2].first);
        }

        ASSERT(0 == mX.match(&results, MATCH2, sizeof(MATCH2)-1));
        LOOP_ASSERT(results.size(), 3 == results.size());
        if (3 == results.size()) {
            LOOP_ASSERT(results[1].first, 8 == results[1].first);
            LOOP_ASSERT(results[2].first, 10 == results[2].first);
        }
      } break;
          
      case 9: {
        // --------------------------------------------------------------------
        // TESTING USAGE EXAMPLE
        //   This will test the usage example provided in the component header
        //   file.
        //
        // Concerns:
        //   The usage example provided in the component header file must
        //   compile, link, and run on all platforms as shown.
        //
        // Plan:
        //   Run the usage example from the component's header file, replacing
        //   'assert' with 'ASSERT'.
        //
        // Testing:
        //   Usage example.
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting Usage Example"
                          << "\n=====================" << endl;

//..
// The following array contains the sample Internet e-mail message from which
// we will extract the subject:
//..
    const char RFC822_MESSAGE[] =
        "Received: ; Fri, 23 Apr 2004 14:30:00 -0400\r\n"
        "Message-ID: <12345@mailgate.bloomberg.net>\r\n"
        "Date: Fri, 23 Apr 2004 14:30:00 -0400\r\n"
        "From: <someone@bloomberg.net>\r\n"
        "To: <someone_else@bloomberg.net>\r\n"
        "Subject: This is the subject text\r\n"
        "MIME-Version: 1.0\r\n"
        "Content-Type: text/plain\r\n"
        "\r\n"
        "This is the message body.\r\n"
        ".\r\n";
//..
// Finally, the following 'main' function calls 'parseSubject' to extract the
// subject from 'RFC822_MESSAGE'.  The assertions verify that the subject of
// the message is correctly extracted and assigned to the local 'subject'
// variable:
//..
//  int main()
//  {
        bsl::string subject;
        const int   returnValue = parseSubject(&subject,
                                               RFC822_MESSAGE,
                                               sizeof(RFC822_MESSAGE) - 1);
        ASSERT(0 == returnValue);
        ASSERT(" This is the subject text" == subject);
//  }
//..

        if (veryVerbose) {
            T_ P(subject)
        }

        if (verbose) cout << "\nEnd of Usage Example Test." << endl;
      } break;
      case 8: {
        // --------------------------------------------------------------------
        // TESTING ALLOCATION PROPAGATION
        //
        // Concerns:
        //   We want to make sure that the allocator is propagated properly and
        //   that all memory allocation is done by the supplied allocator.
        //
        // Plan:
        //   Create a set of patterns using a test allocator, and use a default
        //   allocator guard to measure the amount of memory allocated via the
        //   default allocator.  Verify that all memory is allocated by the
        //   test allocator while exercising the pattern.
        //
        // Testing:
        //   ALLOCATION PROPAGATION
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting Allocation"
                          << "\n==================" << endl;

        bslma_TestAllocator allocator0(veryVeryVerbose);
        bslma_TestAllocator allocator1(veryVeryVerbose);
        bslma_TestAllocator allocator2(veryVeryVerbose);

        bslma_TestAllocator *Z0 = &allocator0;
        bslma_TestAllocator *Z1 = &allocator1;
        bslma_TestAllocator *Z2 = &allocator2;

        bslma_DefaultAllocatorGuard allocGuard(Z0);

        {
            Obj mX(Z1);  const Obj& X = mX;

            bsl::string errorMsg(Z2);          // do not use default allocator!
            int         errorOffset;

            const char PATTERN[]         = "(?P<pkgName>[a-z]+)_([A-Za-z]+)";
            const char SUBPATTERN_NAME[] = "pkgName";

            ASSERT(0 == mX.prepare(&errorMsg,
                                   &errorOffset,
                                   PATTERN,
                                   Obj::BDEPCRE_FLAG_MULTILINE));

            const char TEST_STRING[]   = "bbasm_SecurityCache\n"
                                         "tweut_StringRef\n";
            const int  TEST_STRING_LEN = sizeof(TEST_STRING) - 1;

            bsl::pair<int, int> match;
            ASSERT(0 == X.match(&match, TEST_STRING, TEST_STRING_LEN));

            {
                const char EXPECTED_MATCH[] = "bbasm_SecurityCache";
                bsl::string realMatch(&TEST_STRING[match.first],
                        match.second, Z2);     // do not use default allocator!
                LOOP_ASSERT(realMatch, EXPECTED_MATCH == realMatch);
            }

            const int startPosition = match.second;
            X.match(&match, TEST_STRING, TEST_STRING_LEN, startPosition);

            {
                const char EXPECTED_MATCH[] = "tweut_StringRef";
                bsl::string realMatch(&TEST_STRING[match.first],
                        match.second, Z2);     // do not use default allocator!
                LOOP_ASSERT(realMatch, EXPECTED_MATCH == realMatch);
            }

            bsl::vector<pair<int, int> > vMatch(Z2);
                                               // do not use default allocator!
            X.match(&vMatch, TEST_STRING, TEST_STRING_LEN);

            const int  LEN = 3;
            const char EXPECTED_MATCHES[LEN][30] = { "bbasm_SecurityCache",
                                                     "bbasm",
                                                     "SecurityCache" };

            LOOP_ASSERT(vMatch.size(), LEN == (int)vMatch.size());
            for (int i = 0; i < LEN; ++i) {
                bsl::string realMatch(&TEST_STRING[vMatch[i].first],
                        vMatch[i].second, Z2); // do not use default allocator!

                LOOP2_ASSERT(i, realMatch, EXPECTED_MATCHES[i] == realMatch);
            }
        }
        ASSERT(0 == Z0->numAllocations());
        ASSERT(0 != Z1->numAllocations());

      } break;
      case 7: {
        // --------------------------------------------------------------------
        // TESTING SUBPATTERNS
        //   This will test the 'numSubpatterns' and 'subpatternIndex'
        //   accessors.  It will also test the vector 'match' function to
        //   verify that it returns the captured substrings correctly.
        //
        // Concerns:
        //   We want to make sure that subpatterns are recognized correctly and
        //   also that captured substrings can be retrieved.
        //
        // Plan:
        //   Create a set of patterns, with increasing number of subpatterns.
        //   The pattern used will be a regular expression with the capability
        //   of splitting a BDE-style class name into package name & class
        //   name, and also split the class name into separate words.  For
        //   example, 'bbasm_SecurityDataChunkResolver' will be split into
        //   package name 'bbasm' and class name 'SecurityDataChunkResolver'.
        //   Further, the class name will be split into separate words
        //   'Security', 'Data', 'Chunk', and 'Resolver'.  Each word is
        //   represented by a sub-pattern in the regular expression.  Since the
        //   set contains patterns with increasing number of subpatterns, the
        //   number of words that a pattern can identify will also increase.
        //   The test data contains 'numWords' values from 0 to 4.  This also
        //   tests "nested" subpatterns (the 'words' sub-patterns are nested
        //   inside the 'class-name' sub-pattern).
        //
        // Testing:
        //   int match(bsl::vector<bsl::pair<int, int> >*, ...) const;
        //   int numSubpatterns() const;
        //   int subpatternIndex(const char*) const;
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting Subpatterns"
                          << "\n===================" << endl;

        static const struct {
            int         d_lineNum;   // source line number
            const char *d_pattern;   // pattern
            int         d_numWords;  // number of words recognized
        } PATTERNS[] = {
            //line  pattern                                        numWords
            //----  -------                                        --------
            { L_,   "(?P<pkg>[a-z]+)_"
                    "(?P<name>"
                    "([A-Z][a-z]*)+)",                             0        },
            { L_,   "(?P<pkg>[a-z]+)_"
                    "(?P<name>"
                    "(?P<word1>[A-Z][a-z]*)"
                    "([A-Z][a-z]*)*)",                             1        },
            { L_,   "(?P<pkg>[a-z]+)_"
                    "(?P<name>"
                    "(?P<word1>[A-Z][a-z]*)"
                    "(?P<word2>[A-Z][a-z]*)"
                    "([A-Z][a-z]*)*)",                             2        },
            { L_,   "(?P<pkg>[a-z]+)_"
                    "(?P<name>"
                    "(?P<word1>[A-Z][a-z]*)"
                    "(?P<word2>[A-Z][a-z]*)"
                    "(?P<word3>[A-Z][a-z]*)"
                    "([A-Z][a-z]*)*)",                             3        },
            { L_,   "(?P<pkg>[a-z]+)_"
                    "(?P<name>"
                    "(?P<word1>[A-Z][a-z]*)"
                    "(?P<word2>[A-Z][a-z]*)"
                    "(?P<word3>[A-Z][a-z]*)"
                    "(?P<word4>[A-Z][a-z]*)"
                    "([A-Z][a-z]*)*)",                             4        },
        };
        const int NUM_PATTERNS = sizeof PATTERNS / sizeof *PATTERNS;

        // subpattern names
        const char SPN_PKG[]      = "pkg";
        const char SPN_NAME[]     = "name";
        const char SPN_WORD[4][6] = { "word1", "word2", "word3", "word4" };

        // subject
        const char SUBJECT[]   = "bbasm_SecurityDataChunkResolver";
        const int  SUBJECT_LEN = sizeof(SUBJECT) - 1;

        // captured substrings
        const char CS_PKG[]       = "bbasm";
        const char CS_NAME[]      = "SecurityDataChunkResolver";
        const char CS_WORD[4][10] = { "Security",
                                      "Data",
                                      "Chunk",
                                      "Resolver" };

        if (verbose) {
            cout << "\nTesting with increasing number of subpatterns." << endl;
        }

        for (int i = 0; i < NUM_PATTERNS; ++i) {
            const int   LINE      = PATTERNS[i].d_lineNum;
            const char *PATTERN   = PATTERNS[i].d_pattern;
            const int   NUM_WORDS = PATTERNS[i].d_numWords;

            if (veryVerbose) {
                cout << "\n\tPreparing expression from "; P(LINE);
                cout << "\t  with "; P(PATTERN)
                cout << "\t  and "; P(NUM_WORDS)
            }

            Obj mX(&testAllocator);  const Obj& X = mX;

            bsl::string errorMsg;
            int         errorOffset;

            int retCode = mX.prepare(&errorMsg, &errorOffset, PATTERN, 0);
            LOOP3_ASSERT(LINE, errorMsg, errorOffset, 0 == retCode);

            const int NUM_SUBPATTERNS  = NUM_WORDS + 3;
            const int SP_PKG_INDEX     = 1;
            const int SP_NAME_INDEX    = 2;
            const int SP_WORD_INDEX[4] = { 3, 4, 5, 6 };

            if (veryVeryVerbose) {
                T_ T_
                P_(X.numSubpatterns())
                P_(X.subpatternIndex(SPN_PKG))
                P(X.subpatternIndex(SPN_NAME))
            }

            LOOP2_ASSERT(LINE,              X.numSubpatterns(),
                         NUM_SUBPATTERNS == X.numSubpatterns());
            LOOP2_ASSERT(LINE,            X.subpatternIndex(SPN_PKG),
                         SP_PKG_INDEX  == X.subpatternIndex(SPN_PKG));
            LOOP2_ASSERT(LINE,            X.subpatternIndex(SPN_NAME),
                         SP_NAME_INDEX == X.subpatternIndex(SPN_NAME));

            // Test names that do not identify sub-patterns.

            bsl::string BAD_SPN_NAME(SPN_NAME);  BAD_SPN_NAME += "xYz";
            bsl::string BAD_SPN_PKG(SPN_PKG);    BAD_SPN_PKG  += "AbC";

            LOOP_ASSERT(LINE, -1 == X.subpatternIndex(BAD_SPN_NAME.c_str()));
            LOOP_ASSERT(LINE, -1 == X.subpatternIndex(BAD_SPN_PKG.c_str()));

            int j;

            for (j = 0; j < NUM_WORDS; ++j) {
                const char *SPN_WORDJ      = SPN_WORD[j];
                const int   SP_WORD_INDEXJ = SP_WORD_INDEX[j];

                if (veryVeryVerbose) {
                    T_ T_ P_(j) P(X.subpatternIndex(SPN_WORDJ))
                }

                LOOP3_ASSERT(LINE, j,          X.subpatternIndex(SPN_WORDJ),
                             SP_WORD_INDEXJ == X.subpatternIndex(SPN_WORDJ));

                bsl::string BAD_SPN_WORDJ(SPN_WORDJ);  BAD_SPN_WORDJ += "#%$";

                LOOP_ASSERT(LINE,
                            -1 == X.subpatternIndex(BAD_SPN_WORDJ.c_str()));
            }

            if (veryVerbose) {
                cout << "\n\tMatching with "; P(SUBJECT);
            }

            vector<pair<int, int> > vMatch;

            if (NUM_WORDS%2) {
                // Grow the vector to make sure it shrinks back to exactly
                // 'NUM_SUBPATTERNS'+1.  Only do this for 1/2 the cases.  For
                // the other half, we check that the vector grows to exactly
                // 'NUM_SUBPATTERNS'+1.

                for (j = 0; j < NUM_SUBPATTERNS + 10; ++j) {
                    vMatch.push_back(make_pair(0, 0));
                }
            }

            retCode = X.match(&vMatch, SUBJECT, SUBJECT_LEN);
            LOOP_ASSERT(LINE, 0 == retCode);
            LOOP2_ASSERT(LINE,                vMatch.size(),
                         NUM_SUBPATTERNS+1 == (int)vMatch.size());

            // captured substrings
            const string csPkg(&SUBJECT[vMatch[SP_PKG_INDEX].first],
                               vMatch[SP_PKG_INDEX].second);
            const string csName(&SUBJECT[vMatch[SP_NAME_INDEX].first],
                                vMatch[SP_NAME_INDEX].second);

            LOOP2_ASSERT(LINE, csPkg,  CS_PKG  == csPkg);
            LOOP2_ASSERT(LINE, csName, CS_NAME == csName);

            if (veryVeryVerbose) {
                T_ T_ P_(csPkg) P(csName)
            }

            for (j = 0; j < NUM_WORDS; ++j) {
                const char *CS_WORDJ       = CS_WORD[j];
                const int   SP_WORD_INDEXJ = SP_WORD_INDEX[j];

                const string csWordj(&SUBJECT[vMatch[SP_WORD_INDEXJ].first],
                                     vMatch[SP_WORD_INDEXJ].second);

                if (veryVeryVerbose) {
                    T_ T_ P_(j) P(csWordj)
                }

                LOOP2_ASSERT(LINE, csWordj,  CS_WORDJ == csWordj);
            }

            if (4 == NUM_WORDS) {
                // If NUM_WORDS is 4, that means the last subpattern was not
                // matched (only 4 words in the subject).  So make sure that
                // the last element in the vector contains (as per doc):
                // pair<int, int>(-1, 0).

                const pair<int, int> NOT_FOUND(-1, 0);
                const pair<int, int> lastElement = vMatch[vMatch.size()-1];

                if (veryVeryVerbose) {
                    T_ T_ P_(lastElement.first) P(lastElement.second)
                }

                LOOP3_ASSERT(LINE, lastElement.first, lastElement.second,
                             NOT_FOUND == lastElement);
            }
            else {
                // Check that the last (unnamed) substring at the end was
                // captured correctly.  It should be the last word in the
                // subject (CS_WORD[3]).

                const pair<int, int> lastElement = vMatch[vMatch.size()-1];
                const string         csLastWord(&SUBJECT[lastElement.first],
                                                lastElement.second);

                if (veryVeryVerbose) {
                    T_ T_ P(csLastWord)
                }

                LOOP2_ASSERT(LINE, csLastWord, CS_WORD[3] == csLastWord);
            }
        }

        if (verbose) cout << "\nEnd of Subpatterns Test." << endl;
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // TESTING UTF8 FLAG
        //   This will test the 'BDEPCRE_FLAG_UTF8' option when compiling
        //   regular expressions.
        //
        // Concerns:
        //   We want to make sure that UTF8 byte sequences are treated as
        //   single UTF8 characters when this flag is specified.  We also want
        //   to make sure that the same UTF8 byte sequences are treated as
        //   regular characters when this flag is not specified.
        //
        // Plan:
        //   Create a set of UTF8 patterns.  For each pattern, create two
        //   regular expression objects - one with 'BDEPCRE_FLAG_UTF8'
        //   specified and another without 'BDEPCRE_FLAG_UTF8' specified.
        //
        //   For each object, exercise the match function using a UTF8 subject
        //   that matches the pattern being tested.  Verify that the object
        //   with 'BDEPCRE_FLAG_UTF8' succeeds and also that the object without
        //   'BDEPCRE_FLAG_UTF8' fails.  Next, exercise the match function
        //   using a non-UTF8 subject that matches the pattern being tested.
        //   Verify that the object with 'BDEPCRE_FLAG_UTF8' fails and also
        //   that the object without 'BDEPCRE_FLAG_UTF8' succeeds.
        //
        //   For simplicity, only 2-byte UTF8 characters will be used in the
        //   test.
        //
        // Testing:
        //   int flags() const;
        //   BDEPCRE_FLAG_UTF8
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting UTF8 Flag"
                          << "\n=================" << endl;

        static const struct {
            int            d_lineNum;         // source line number
            const char    *d_pattern;         // pattern string
            unsigned char  d_utf8Subject[2];  // utf8 subject
            unsigned char  d_regularSubject;  // regular subject
        } DATA[] = {
            //line   pattern       utf8Subject       regularSubject
            //----   -------       -----------       --------------
            { L_,    "\\xC0",      { 0xC3, 0x80 },   0xC0           },
            { L_,    "\\xC5",      { 0xC3, 0x85 },   0xC5           },
            { L_,    "\\xD3",      { 0xC3, 0x93 },   0xD3           },
            { L_,    "\\xFF",      { 0xC3, 0xBF },   0xFF           },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        if (verbose) {
            cout << "\nRunning test data." << endl;
        }

        for (int i = 0; i < NUM_DATA; ++i) {
            const int   LINE    = DATA[i].d_lineNum;
            const char *PATTERN = DATA[i].d_pattern;

            if (veryVerbose) {
                cout << "\tPreparing the regular expression objects.  ";
                P_(LINE) P(PATTERN)
            }

            Obj mX(&testAllocator);  const Obj& X = mX;
                                                   // has 'BDEPCRE_FLAG_UTF8'
            Obj mY(&testAllocator);  const Obj& Y = mY;
                                                   // !have 'BDEPCRE_FLAG_UTF8'

            bsl::string errorMsg;
            int         errorOffset;

            int retCode = mX.prepare(&errorMsg,
                                     &errorOffset,
                                     PATTERN,
                                     Obj::BDEPCRE_FLAG_UTF8);
            LOOP2_ASSERT(errorMsg, errorOffset, 0 == retCode);
            LOOP_ASSERT(X.flags(), Obj::BDEPCRE_FLAG_UTF8 == X.flags());

            retCode = mY.prepare(&errorMsg, &errorOffset, PATTERN, 0);
            LOOP2_ASSERT(errorMsg, errorOffset, 0 == retCode);
            LOOP_ASSERT(Y.flags(), 0 == Y.flags());

            {
                if (veryVeryVerbose) {
                    cout << "\t\tTesting with UTF8 subject." << endl;
                }

                const unsigned char *UTF8_SUBJECT = DATA[i].d_utf8Subject;

                retCode = X.match((const char*)UTF8_SUBJECT, 2);
                LOOP2_ASSERT(LINE, retCode, 0 == retCode);

                retCode = Y.match((const char*)UTF8_SUBJECT, 2);
                LOOP2_ASSERT(LINE, retCode, 0 != retCode);
            }

            {
                if (veryVeryVerbose) {
                    cout << "\t\tTesting with non-UTF8 subject." << endl;
                }

                const unsigned char REGULAR_SUBJECT = DATA[i].d_regularSubject;

                retCode = X.match((const char*)&REGULAR_SUBJECT, 1);
                LOOP2_ASSERT(LINE, retCode, 0 != retCode);

                retCode = Y.match((const char*)&REGULAR_SUBJECT, 1);
                LOOP2_ASSERT(LINE, retCode, 0 == retCode);
            }
        }

        if (verbose) cout << "\nEnd of UTF8 Flag Test." << endl;
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING MULTILINE FLAG
        //   This will test the 'BDEPCRE_FLAG_MULTILINE' option when compiling
        //   regular expressions.
        //
        // Concerns:
        //   We want to make sure that '^' and '$' match 'beginning-of-line'
        //   and 'end-of-line' respectively when this flag is specified.  We
        //   also want to make sure that '^' and '$' match
        //   'beginning-of-string' and 'end-of-string' respectively when this
        //   flag is not specified.
        //
        // Plan:
        //   For a given pattern string starting with '^' and ending with '$',
        //   create two regular expression objects - one with
        //   'BDEPCRE_FLAG_MULTILINE' specified and another without
        //   'BDEPCRE_FLAG_MULTILINE' specified.
        //
        //   For each object, exercise the match function using subjects of the
        //   form "<preamble>\n<pattern-match>\n<postamble>".  Select test data
        //   with increasing preamble/postamble length (from 0 to 3).  Verify
        //   that the object with 'BDEPCRE_FLAG_MULTILINE' always succeeds and
        //   also that the object without 'BDEPCRE_FLAG_MULTILINE' always
        //   fails.
        //
        //   Finally, exercise the match function using a subject that matches
        //   the pattern exactly on a single line (i.e.  without any
        //   preamble/postamble/newline characters).  Verify that both objects
        //   succeed.
        //
        // Testing:
        //   int flags() const;
        //   BDEPCRE_FLAG_MULTILINE
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting Multiline Flag"
                          << "\n======================" << endl;

        const char PATTERN[]             = "^bbasm_SecurityCache$";
        const char SUBJECT_SINGLE_LINE[] = "bbasm_SecurityCache";

        static const struct {
            int         d_lineNum;          // source line number
            const char *d_subject;          // subject string
            int         d_preambleLength;   // number of chars in preamble
            int         d_postambleLength;  // number of chars in postamble
        } DATA[] = {
            //line  subject                            preamble   postamble
            //----  -------                            --------   ---------
            { L_,   "\nbbasm_SecurityCache\n",         0,         0         },
            { L_,   "a\nbbasm_SecurityCache\n",        1,         0         },
            { L_,   "\nbbasm_SecurityCache\na",        0,         1         },
            { L_,   "a\nbbasm_SecurityCache\na",       1,         1         },
            { L_,   "ab\nbbasm_SecurityCache\n",       2,         0         },
            { L_,   "\nbbasm_SecurityCache\nab",       0,         2         },
            { L_,   "ab\nbbasm_SecurityCache\nab",     2,         2         },
            { L_,   "abc\nbbasm_SecurityCache\n",      3,         0         },
            { L_,   "\nbbasm_SecurityCache\nabc",      0,         3         },
            { L_,   "abc\nbbasm_SecurityCache\nabc",   3,         3         },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        Obj mX(&testAllocator);  const Obj& X = mX;
                                              // has 'BDEPCRE_FLAG_MULTILINE'
        Obj mY(&testAllocator);  const Obj& Y = mY;
                                              // !have 'BDEPCRE_FLAG_MULTILINE'

        if (verbose) {
            cout << "\nPreparing the regular expression objects." << endl;

            if (veryVerbose) {
                T_ P(PATTERN);
            }
        }

        bsl::string errorMsg;
        int         errorOffset;

        int retCode = mX.prepare(&errorMsg,
                                 &errorOffset,
                                 PATTERN,
                                 Obj::BDEPCRE_FLAG_MULTILINE);
        LOOP2_ASSERT(errorMsg, errorOffset, 0 == retCode);
        LOOP_ASSERT(X.flags(), Obj::BDEPCRE_FLAG_MULTILINE == X.flags());

        retCode = mY.prepare(&errorMsg, &errorOffset, PATTERN, 0);
        LOOP2_ASSERT(errorMsg, errorOffset, 0 == retCode);
        LOOP_ASSERT(Y.flags(), 0 == Y.flags());

        if (verbose) {
            cout << "\nTrying multiline subjects." << endl;
        }

        for (int i = 0; i < NUM_DATA; ++i) {
            const int   LINE          = DATA[i].d_lineNum;
            const char *SUBJECT       = DATA[i].d_subject;
            const int   PREAMBLE_LEN  = DATA[i].d_preambleLength;
            const int   POSTAMBLE_LEN = DATA[i].d_postambleLength;
            const int   SUBJECT_LEN   = strlen(SUBJECT);
            const int   MATCH_OFFSET  = PREAMBLE_LEN + 1;
            const int   MATCH_LENGTH  = SUBJECT_LEN
                                        - PREAMBLE_LEN
                                        - POSTAMBLE_LEN
                                        - 2;

            if (veryVerbose) {
                T_ P_(LINE) P_(SUBJECT) P_(SUBJECT_LEN)
                   P_(PREAMBLE_LEN) P(POSTAMBLE_LEN)
            }

            if (veryVeryVerbose) {
                cout << "\t\tUsing regular match function." << endl;
            }

            pair<int, int> match;

            retCode = X.match(&match, SUBJECT, SUBJECT_LEN);
            LOOP2_ASSERT(LINE, retCode,      0            == retCode);
            LOOP2_ASSERT(LINE, match.first,  MATCH_OFFSET == match.first);
            LOOP2_ASSERT(LINE, match.second, MATCH_LENGTH == match.second);

            retCode = Y.match(&match, SUBJECT, SUBJECT_LEN);
            LOOP2_ASSERT(LINE, retCode, 0 != retCode);

            if (veryVeryVerbose) {
                cout << "\t\tUsing vector match function." << endl;
            }

            vector<pair<int, int> > vMatch;

            retCode = X.match(&vMatch, SUBJECT, SUBJECT_LEN);
            LOOP2_ASSERT(LINE, retCode, 0 == retCode);
            LOOP2_ASSERT(LINE, vMatch[0].first,
                         MATCH_OFFSET == vMatch[0].first);
            LOOP2_ASSERT(LINE, vMatch[0].second,
                         MATCH_LENGTH == vMatch[0].second);

            retCode = Y.match(&vMatch, SUBJECT, SUBJECT_LEN);
            LOOP2_ASSERT(LINE, retCode, 0 != retCode);
        }

        if (verbose) {
            cout << "\nTrying single line subject." << endl;
        }

        {
            const char *SUBJECT     = SUBJECT_SINGLE_LINE;
            const int   SUBJECT_LEN = strlen(SUBJECT);

            if (veryVerbose) {
                T_ P_(SUBJECT) P(SUBJECT_LEN)
            }

            if (veryVeryVerbose) {
                cout << "\t\tUsing regular match function." << endl;
            }

            pair<int, int> match;

            retCode = X.match(&match, SUBJECT, SUBJECT_LEN);
            LOOP_ASSERT(retCode,      0           == retCode);
            LOOP_ASSERT(match.first,  0           == match.first);
            LOOP_ASSERT(match.second, SUBJECT_LEN == match.second);

            retCode = Y.match(&match, SUBJECT, SUBJECT_LEN);
            LOOP_ASSERT(retCode,      0           == retCode);
            LOOP_ASSERT(match.first,  0           == match.first);
            LOOP_ASSERT(match.second, SUBJECT_LEN == match.second);

            if (veryVeryVerbose) {
                cout << "\t\tUsing vector match function." << endl;
            }

            vector<pair<int, int> > vMatch;

            retCode = X.match(&vMatch, SUBJECT, SUBJECT_LEN);
            LOOP_ASSERT(retCode,          0           == retCode);
            LOOP_ASSERT(vMatch[0].first,  0           == vMatch[0].first);
            LOOP_ASSERT(vMatch[0].second, SUBJECT_LEN == vMatch[0].second);

            retCode = Y.match(&vMatch, SUBJECT, SUBJECT_LEN);
            LOOP_ASSERT(retCode,          0           == retCode);
            LOOP_ASSERT(vMatch[0].first,  0           == vMatch[0].first);
            LOOP_ASSERT(vMatch[0].second, SUBJECT_LEN == vMatch[0].second);
        }

        if (verbose) cout << "\nEnd of Multiline Flag Test." << endl;
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING CASELESS FLAG
        //   This will test the 'BDEPCRE_FLAG_CASELESS' option when compiling
        //   regular expressions.
        //
        // Concerns:
        //   We want to make sure that caseless matching is performed when this
        //   flag is specified, and also that caseless matching is *not*
        //   performed when this flag is not specified.
        //
        // Plan:
        //   For a given pattern string containing both upper-case and
        //   lower-case letters, create two regular expression objects - one
        //   with 'BDEPCRE_FLAG_CASELESS' specified and another without
        //   'BDEPCRE_FLAG_CASELESS' specified.
        //
        //   For each object, exercise the match function using subjects that
        //   use different cases from the pattern.  Verify that the object with
        //   'BDEPCRE_FLAG_CASELESS' always succeeds and also that the object
        //   without 'BDEPCRE_FLAG_CASELESS' always fails.
        //
        //   Finally, exercise the match function using a subject that uses the
        //   same case as the pattern.  Verify that both objects succeed.
        //
        // Testing:
        //   int flags() const;
        //   BDEPCRE_FLAG_CASELESS
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting Caseless Flag"
                          << "\n=====================" << endl;

        const char PATTERN[]           = "bbasm_SecurityCache";
        const char SUBJECT_SAME_CASE[] = "bbasm_SecurityCache";

        static const struct {
            int         d_lineNum;  // source line number
            const char *d_subject;  // subject string
        } DATA[] = {
            //line      subject
            //----      -------
            { L_,       "BBASM_SECURITYCACHE"                   },
            { L_,       "bbasm_securitycache"                   },
            { L_,       "bBaSm_sEcUrItYcAcHe"                   },
            { L_,       "BbAsM_SeCuRiTyCaChE"                   },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        Obj mX(&testAllocator);  const Obj& X = mX;
                                               // has 'BDEPCRE_FLAG_CASELESS'
        Obj mY(&testAllocator);  const Obj& Y = mY;
                                               // !have 'BDEPCRE_FLAG_CASELESS'

        if (verbose) {
            cout << "\nPreparing the regular expression objects." << endl;

            if (veryVerbose) {
                T_ P(PATTERN);
            }
        }

        bsl::string errorMsg;
        int         errorOffset;

        int retCode = mX.prepare(&errorMsg,
                                 &errorOffset,
                                 PATTERN,
                                 Obj::BDEPCRE_FLAG_CASELESS);
        LOOP2_ASSERT(errorMsg, errorOffset, 0 == retCode);
        LOOP_ASSERT(X.flags(), Obj::BDEPCRE_FLAG_CASELESS == X.flags());

        retCode = mY.prepare(&errorMsg, &errorOffset, PATTERN, 0);
        LOOP2_ASSERT(errorMsg, errorOffset, 0 == retCode);
        LOOP_ASSERT(Y.flags(), 0 == Y.flags());

        if (verbose) {
            cout << "\nTrying different cases." << endl;
        }

        for (int i = 0; i < NUM_DATA; ++i) {
            const int   LINE    = DATA[i].d_lineNum;
            const char *SUBJECT = DATA[i].d_subject;
            const int   LEN     = strlen(SUBJECT);

            if (veryVerbose) {
                T_ P_(LINE) P_(SUBJECT) P(LEN)
            }

            if (veryVeryVerbose) {
                cout << "\t\tUsing regular match function." << endl;
            }

            pair<int, int> match;

            retCode = X.match(&match, SUBJECT, LEN);
            LOOP2_ASSERT(LINE, retCode,      0   == retCode);
            LOOP2_ASSERT(LINE, match.first,  0   == match.first);
            LOOP2_ASSERT(LINE, match.second, LEN == match.second);

            retCode = Y.match(&match, SUBJECT, LEN);
            LOOP2_ASSERT(LINE, retCode, 0 != retCode);

            if (veryVeryVerbose) {
                cout << "\t\tUsing vector match function." << endl;
            }

            vector<pair<int, int> > vMatch;

            retCode = X.match(&vMatch, SUBJECT, LEN);
            LOOP2_ASSERT(LINE, retCode,          0   == retCode);
            LOOP2_ASSERT(LINE, vMatch[0].first,  0   == vMatch[0].first);
            LOOP2_ASSERT(LINE, vMatch[0].second, LEN == vMatch[0].second);

            retCode = Y.match(&vMatch, SUBJECT, LEN);
            LOOP2_ASSERT(LINE, retCode, 0 != retCode);
        }

        if (verbose) {
            cout << "\nTrying same case as pattern." << endl;
        }

        {
            const char *SUBJECT = SUBJECT_SAME_CASE;
            const int   LEN     = strlen(SUBJECT);

            if (veryVerbose) {
                T_ P_(SUBJECT) P(LEN)
            }

            if (veryVeryVerbose) {
                cout << "\t\tUsing regular match function." << endl;
            }

            pair<int, int> match;

            retCode = X.match(&match, SUBJECT, LEN);
            LOOP_ASSERT(retCode,      0   == retCode);
            LOOP_ASSERT(match.first,  0   == match.first);
            LOOP_ASSERT(match.second, LEN == match.second);

            retCode = Y.match(&match, SUBJECT, LEN);
            LOOP_ASSERT(retCode,      0   == retCode);
            LOOP_ASSERT(match.first,  0   == match.first);
            LOOP_ASSERT(match.second, LEN == match.second);

            if (veryVeryVerbose) {
                cout << "\t\tUsing vector match function." << endl;
            }

            vector<pair<int, int> > vMatch;

            retCode = X.match(&vMatch, SUBJECT, LEN);
            LOOP_ASSERT(retCode,          0   == retCode);
            LOOP_ASSERT(vMatch[0].first,  0   == vMatch[0].first);
            LOOP_ASSERT(vMatch[0].second, LEN == vMatch[0].second);

            retCode = Y.match(&vMatch, SUBJECT, LEN);
            LOOP_ASSERT(retCode,          0   == retCode);
            LOOP_ASSERT(vMatch[0].first,  0   == vMatch[0].first);
            LOOP_ASSERT(vMatch[0].second, LEN == vMatch[0].second);
        }

        if (verbose) cout << "\nEnd of Caseless Flag Test." << endl;
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING MATCHING METHODS
        //   This will test the 'match' methods.
        //
        // Concerns:
        //   Although we are not testing the implementation of the PCRE
        //   library, we want to check that the interface is plugged in
        //   correctly and works as documented.  In particular, we want to
        //   check that the 'subject', 'subjectLength', and 'subjectStart'
        //   arguments are passed to PCRE correctly and the resulting 'ovector'
        //   is copied correctly to the 'result'.  Also we want to make sure
        //   that a failure code is returned when the subject does not match
        //   the pattern.
        //
        // Plan:
        //   Prepare a regular expression object using a given 'PATTERN'
        //   string, create a set of subjects that contain a match for
        //   'PATTERN'.  The set should contain subjects of increasing length,
        //   and also increasing match offsets ('matchOffset').
        //
        //   Exercise the 'match' methods using 'subjectStart' values in the
        //   range [0..'subjectLength'].  Check that the methods succeed when
        //   'subjectStart' <= 'matchOffset' and they fail when
        //   'subjectStart' > 'matchOffset'.  For each successful call to
        //   'match', check that 'result' contains the correct offset and
        //   length for the captured string.  Note that captured substrings are
        //   tested in later test case.
        //
        //   Finally, exercise the special case where 'subjectLength' is 0.
        //
        // Testing:
        //   int match(const char *subject, ...) const;
        //   int match(bsl::pair<int, int>*, ...) const;
        //   int match(bsl::vector<bsl::pair<int, int> >*, ...) const;
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting Matching Methods"
                          << "\n========================" << endl;

        const char PATTERN[] = "(a(b(c)?)?)";  // matches 'a', 'ab', or 'abc'

        static const struct {
            int         d_lineNum;      // source line number
            const char *d_subject;      // subject string
            int         d_matchOffset;  // match offset
            int         d_matchLength;  // match length
        } DATA[] = {
            //line   subject             matchOffset   matchLength
            //----   -------             -----------   -----------

            // subject length = 1
            { L_,    "a",                0,            1            },

            // subject length = 2
            { L_,    "aZ",               0,            1            },
            { L_,    "ab",               0,            2            },
            { L_,    "Za",               1,            1            },

            // subject length = 3
            { L_,    "aZZ",              0,            1            },
            { L_,    "abZ",              0,            2            },
            { L_,    "abc",              0,            3            },
            { L_,    "ZaZ",              1,            1            },
            { L_,    "Zab",              1,            2            },
            { L_,    "ZZa",              2,            1            },

            // subject length = 4
            { L_,    "aZZZ",             0,            1            },
            { L_,    "abZZ",             0,            2            },
            { L_,    "abcZ",             0,            3            },
            { L_,    "ZaZZ",             1,            1            },
            { L_,    "ZabZ",             1,            2            },
            { L_,    "Zabc",             1,            3            },
            { L_,    "ZZaZ",             2,            1            },
            { L_,    "ZZab",             2,            2            },
            { L_,    "ZZZa",             3,            1            },

            // subject length = 5
            { L_,    "aZZZZ",            0,            1            },
            { L_,    "abZZZ",            0,            2            },
            { L_,    "abcZZ",            0,            3            },
            { L_,    "ZaZZZ",            1,            1            },
            { L_,    "ZabZZ",            1,            2            },
            { L_,    "ZabcZ",            1,            3            },
            { L_,    "ZZaZZ",            2,            1            },
            { L_,    "ZZabZ",            2,            2            },
            { L_,    "ZZabc",            2,            3            },
            { L_,    "ZZZaZ",            3,            1            },
            { L_,    "ZZZab",            3,            2            },
            { L_,    "ZZZZa",            4,            1            },

            // subject length = 6
            { L_,    "aZZZZZ",           0,            1            },
            { L_,    "abZZZZ",           0,            2            },
            { L_,    "abcZZZ",           0,            3            },
            { L_,    "ZaZZZZ",           1,            1            },
            { L_,    "ZabZZZ",           1,            2            },
            { L_,    "ZabcZZ",           1,            3            },
            { L_,    "ZZaZZZ",           2,            1            },
            { L_,    "ZZabZZ",           2,            2            },
            { L_,    "ZZabcZ",           2,            3            },
            { L_,    "ZZZaZZ",           3,            1            },
            { L_,    "ZZZabZ",           3,            2            },
            { L_,    "ZZZabc",           3,            3            },
            { L_,    "ZZZZaZ",           4,            1            },
            { L_,    "ZZZZab",           4,            2            },
            { L_,    "ZZZZZa",           5,            1            },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        Obj mX(&testAllocator);  const Obj& X = mX;

        bsl::string errorMsg;
        int         errorOffset;

        int retCode = mX.prepare(&errorMsg, &errorOffset, PATTERN, 0);
        LOOP2_ASSERT(errorMsg, errorOffset, 0 == retCode);

        if (verbose) {
            cout << "\nTesting non-empty subjects." << endl;

            if (veryVerbose) {
                T_ P(PATTERN)
            }
        }

        for (int i = 0; i < NUM_DATA; ++i) {
            const int   LINE         = DATA[i].d_lineNum;
            const char *SUBJECT      = DATA[i].d_subject;
            const int   MATCH_OFFSET = DATA[i].d_matchOffset;
            const int   MATCH_LENGTH = DATA[i].d_matchLength;
            const int   SUBJECT_LEN  = strlen(SUBJECT);

            if (veryVerbose) {
                T_ P_(LINE) P_(SUBJECT) P_(MATCH_OFFSET) P(MATCH_LENGTH)
            }

            pair<int, int>          match;
            vector<pair<int, int> > vMatch;

            for (int subjectStart = 0; subjectStart <= SUBJECT_LEN;
                                                              ++subjectStart) {
                if (veryVeryVerbose) {
                    T_ T_ P(subjectStart)
                }

                retCode = X.match(SUBJECT, SUBJECT_LEN, subjectStart);

                if (subjectStart <= MATCH_OFFSET) {
                    LOOP2_ASSERT(LINE, subjectStart, 0 == retCode);
                }
                else {
                    LOOP2_ASSERT(LINE, subjectStart, 0 != retCode);
                }

                retCode = X.match(&match, SUBJECT, SUBJECT_LEN, subjectStart);

                if (subjectStart <= MATCH_OFFSET) {
                    LOOP2_ASSERT(LINE, subjectStart, 0 == retCode);
                    LOOP3_ASSERT(LINE, subjectStart, match.first,
                                       MATCH_OFFSET == match.first);
                    LOOP3_ASSERT(LINE, subjectStart, match.second,
                                       MATCH_LENGTH == match.second);
                }
                else {
                    LOOP2_ASSERT(LINE, subjectStart, 0 != retCode);
                }

                retCode = X.match(&vMatch, SUBJECT, SUBJECT_LEN, subjectStart);

                if (subjectStart <= MATCH_OFFSET) {
                    LOOP2_ASSERT(LINE, subjectStart, 0 == retCode);
                    LOOP3_ASSERT(LINE, subjectStart, vMatch[0].first,
                                       MATCH_OFFSET == vMatch[0].first);
                    LOOP3_ASSERT(LINE, subjectStart, vMatch[0].second,
                                       MATCH_LENGTH == vMatch[0].second);
                }
                else {
                    LOOP2_ASSERT(LINE, subjectStart, 0 != retCode);
                }
            }
        }

        if (verbose) {
            cout << "\nTesting empty subjects." << endl;
        }

        {
            const char GOOD_PATTERN[]     = "(abc)*";
            const char NOT_GOOD_PATTERN[] = "(abc)+";

            Obj mGood(&testAllocator);     const Obj& GOOD     = mGood;
            Obj mNotGood(&testAllocator);  const Obj& NOT_GOOD = mNotGood;

            if (veryVerbose) {
                cout << "\tPreparing regular expression objects." << endl;
            }

            int retCode = mGood.prepare(&errorMsg,
                                        &errorOffset,
                                        GOOD_PATTERN,
                                        0);
            LOOP2_ASSERT(errorMsg, errorOffset, 0 == retCode);

            retCode = mNotGood.prepare(&errorMsg,
                                       &errorOffset,
                                       NOT_GOOD_PATTERN,
                                       0);
            LOOP2_ASSERT(errorMsg, errorOffset, 0 == retCode);

            if (veryVerbose) {
                cout << "\tTesting good pattern." << endl;
            }

            pair<int, int>          match;
            vector<pair<int, int> > vMatch;

            retCode = GOOD.match("", 0);
            ASSERT(0 == retCode);

            retCode = GOOD.match(&match, "", 0);
            ASSERT(0 == retCode);
            LOOP_ASSERT(match.first,   0 == match.first);
            LOOP_ASSERT(match.second , 0 == match.second);

            retCode = GOOD.match(&vMatch, "", 0);
            ASSERT(0 == retCode);
            LOOP_ASSERT(vMatch[0].first,   0 == vMatch[0].first);
            LOOP_ASSERT(vMatch[0].second , 0 == vMatch[0].second);

            if (veryVerbose) {
                cout << "\tTesting not so good pattern." << endl;
            }

            retCode = NOT_GOOD.match("", 0);
            ASSERT(0 != retCode);

            retCode = NOT_GOOD.match(&match, "", 0);
            ASSERT(0 != retCode);

            retCode = NOT_GOOD.match(&vMatch, "", 0);
            ASSERT(0 != retCode);
        }

        if (verbose) cout << "\nEnd of Matching Methods Test." << endl;
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING BASIC MECHANISM
        //   This will test the 'prepare' and 'clear' mechanism.
        //
        // Concerns:
        //   The 'prepare' method must compile a valid pattern and put the
        //   object in the "prepared" state.  Subsequent calls to 'prepare'
        //   must overwrite the previous pattern stored in the object.  If a
        //   'prepare' call fails, the object's state must not change.  Note
        //   that the concerns related to the 'flags' argument are addressed in
        //   separate test cases.
        //
        //   The 'clear' method must place the object in the "unprepared"
        //   state, regardless of what state the object was in when 'clear' was
        //   called.
        //
        // Plan:
        //   1. Create two regular expression objects ('mX' and 'mY').
        //   2. Verify that both objects are in the "unprepared" state.
        //   3. Call 'mX.prepare()' with a valid pattern ('pattern1') and
        //      verify that it is in the "prepared" state.  Also verify that
        //      'mX' matches subjects that conform to 'pattern1'.
        //   4. Call 'mY.prepare()' with an invalid pattern and verify that it
        //      remains in the "unprepared" state.
        //   5. Call 'mX.prepare()' with another valid pattern ('pattern2') and
        //      verify that it is in the "prepared" state.  Also verify that
        //      'mX' matches subjects that conform to 'pattern2', not
        //      'pattern1'.
        //   6. Call 'mX.prepare()' with an invalid pattern and verify that it
        //      goes into the "unprepared" state.
        //   7. Call 'mX.clear()' and 'mY.clear()' and verify that they remain
        //      in the "unprepared" state.
        //   8. Call 'mX.prepare()' again with a valid pattern ('pattern1') and
        //      verify that goes into the "prepared" state.  Also verify that
        //      'mX' matches subjects that conform to 'pattern1'.
        //   9. Call 'mX.clear()' and verify that it goes into the "unprepared"
        //      state.
        //
        //   Note that the destructor is tested when the objects go out of
        //   scope.
        //
        // Testing:
        //   bdepcre_RegEx(bslma_Allocator *basicAllocator);
        //   ~bdepcre_RegEx();
        //   void clear();
        //   int prepare(const char *pattern, int flags, ...);
        //   bool isPrepared() const;
        //   const bsl::string& pattern() const;
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting Basic Mechanism"
                          << "\n=======================" << endl;

        const char PATTERN1[]        = "(abc)+";
        const char PATTERN2[]        = "(def)+";
        const char INVALID_PATTERN[] = "(abcdef";

        const char SUBJECT1[] = "abcabcabc";
        const char SUBJECT2[] = "defdefdef";

        const int SUBJECT1_LEN = sizeof(SUBJECT1) - 1;
        const int SUBJECT2_LEN = sizeof(SUBJECT2) - 1;

        if (verbose) {
            cout << "\nCreating two regular expression objects "
                 << "('mX' and 'mY')." << endl;
        }

        Obj mX(&testAllocator);  const Obj& X = mX;
        Obj mY(&testAllocator);  const Obj& Y = mY;

        if (verbose) {
            cout << "\nVerifying that both objects are in the unprepared "
                 << "state." << endl;
        }

        ASSERT(false == X.isPrepared());
        ASSERT(false == Y.isPrepared());

        if (verbose) {
            cout << "\nCalling 'mX.prepare()' with valid 'pattern1'." << endl;
        }

        bsl::string errorMsg;
        int         errorOffset;

        int retCode = mX.prepare(&errorMsg, &errorOffset, PATTERN1, 0);
        LOOP2_ASSERT(errorMsg, errorOffset, 0 == retCode);
        ASSERT(true     == X.isPrepared());
        ASSERT(PATTERN1 == X.pattern());

        pair<int, int> match;

        retCode = X.match(&match, SUBJECT1, SUBJECT1_LEN);
        ASSERT(0 == retCode);
        LOOP_ASSERT(match.first,  0            == match.first);
        LOOP_ASSERT(match.second, SUBJECT1_LEN == match.second);

        retCode = X.match(&match, SUBJECT2, SUBJECT2_LEN);
        ASSERT(0 != retCode);

        if (verbose) {
            cout << "\nCalling 'mY.prepare()' with an invalid pattern."
                 << endl;
        }

        retCode = mY.prepare(&errorMsg, &errorOffset, INVALID_PATTERN, 0);
        ASSERT(0     != retCode);
        ASSERT(false == Y.isPrepared());

        if (verbose) {
            cout << "\nCalling 'mX.prepare()' with valid 'pattern2'." << endl;
        }

        retCode = mX.prepare(&errorMsg, &errorOffset, PATTERN2, 0);
        LOOP2_ASSERT(errorMsg, errorOffset, 0 == retCode);
        ASSERT(true     == X.isPrepared());
        ASSERT(PATTERN2 == X.pattern());

        retCode = X.match(&match, SUBJECT1, SUBJECT1_LEN);
        ASSERT(0 != retCode);

        retCode = X.match(&match, SUBJECT2, SUBJECT2_LEN);
        ASSERT(0 == retCode);
        LOOP_ASSERT(match.first,  0            == match.first);
        LOOP_ASSERT(match.second, SUBJECT2_LEN == match.second);

        if (verbose) {
            cout << "\nCalling 'mX.prepare()' with an invalid pattern."
                 << endl;
        }

        retCode = mX.prepare(&errorMsg, &errorOffset, INVALID_PATTERN, 0);
        ASSERT(0     != retCode);
        ASSERT(false == X.isPrepared());

        if (verbose) {
            cout << "\nCalling 'mX.clear()' and 'mY.clear()'." << endl;
        }

        mX.clear();
        ASSERT(false == X.isPrepared());

        mY.clear();
        ASSERT(false == Y.isPrepared());

        if (verbose) {
            cout << "\nCalling 'mX.prepare()' again with valid 'pattern1'."
                 << endl;
        }

        retCode = mX.prepare(&errorMsg, &errorOffset, PATTERN1, 0);
        LOOP2_ASSERT(errorMsg, errorOffset, 0 == retCode);
        ASSERT(true     == X.isPrepared());
        ASSERT(PATTERN1 == X.pattern());

        retCode = X.match(&match, SUBJECT1, SUBJECT1_LEN);
        ASSERT(0 == retCode);
        LOOP_ASSERT(match.first,  0            == match.first);
        LOOP_ASSERT(match.second, SUBJECT1_LEN == match.second);

        retCode = X.match(&match, SUBJECT2, SUBJECT2_LEN);
        ASSERT(0 != retCode);

        if (verbose) {
            cout << "\nCalling 'mX.clear()'." << endl;
        }

        mX.clear();
        ASSERT(false == X.isPrepared());

        if (verbose) cout << "\nEnd of Basic Mechanism Test." << endl;
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //   This test exercises basic functionality, but tests nothing.
        //
        // Concerns:
        //   We want to make sure that the PCRE library is plugged in
        //   correctly.
        //
        // Plan:
        //   1. Create a regular expression object.
        //   2. Verify that it is in the unprepared state.
        //   3. Prepare the object with a pattern.
        //   4. Verify that it is in the prepared state and that all the basic
        //      accessors return the correct values.
        //   5. Verify that the regular and the vector match routines work
        //      correctly, with a 0 start position and also a non-zero start
        //      position.
        //   6. Clear the object to free its resources.
        //   7. Verify that the object has gone back to the unprepared state.
        //
        // Testing:
        //   This test case exercises basic functionality.
        // --------------------------------------------------------------------

        if (verbose) cout << "\nBREATHING TEST"
                          << "\n==============" << endl;

        Obj mX(&testAllocator);  const Obj& X = mX;

        if (verbose) {
            cout << "\nVerifying unprepared state." << endl;
        }

        ASSERT(false == X.isPrepared());

        if (verbose) {
            cout << "\nPreparing the regular expression." << endl;
        }

        bsl::string errorMsg;
        int         errorOffset;

        const char PATTERN[]         = "(?P<pkgName>[a-z]+)_([A-Za-z]+)";
        const char SUBPATTERN_NAME[] = "pkgName";

        if (veryVerbose) {
            T_ P_(PATTERN) P(SUBPATTERN_NAME)
        }

        int retCode = mX.prepare(&errorMsg,
                                 &errorOffset,
                                 PATTERN,
                                 Obj::BDEPCRE_FLAG_MULTILINE);
        LOOP2_ASSERT(errorMsg, errorOffset, 0 == retCode);

        if (verbose) {
            cout << "\nVerifying prepared state." << endl;
        }

        ASSERT(true                        == X.isPrepared());
        ASSERT(Obj::BDEPCRE_FLAG_MULTILINE == X.flags());
        ASSERT(2                           == X.numSubpatterns());
        ASSERT(PATTERN                     == X.pattern());
        ASSERT(1                           ==
                                           X.subpatternIndex(SUBPATTERN_NAME));

        if (verbose) {
            cout << "\nVerifying regular match routine with 0 start "
                 << "position." << endl;
        }

        const char TEST_STRING[]   = "bbasm_SecurityCache\n"
                                     "tweut_StringRef\n";
        const int  TEST_STRING_LEN = sizeof(TEST_STRING) - 1;

        pair<int, int> match;

        {
            retCode = X.match(&match, TEST_STRING, TEST_STRING_LEN);
            ASSERT(0 == retCode);

            const char EXPECTED_MATCH[] = "bbasm_SecurityCache";

            string realMatch(&TEST_STRING[match.first], match.second);

            LOOP_ASSERT(realMatch, EXPECTED_MATCH == realMatch);
        }

        if (verbose) {
            cout << "\nVerifying regular match routine with non-zero start "
                 << "position." << endl;
        }

        {
            const int startPosition = match.second;

            retCode = X.match(&match,
                              TEST_STRING,
                              TEST_STRING_LEN,
                              startPosition);
            ASSERT(0 == retCode);

            const char EXPECTED_MATCH[] = "tweut_StringRef";

            string realMatch(&TEST_STRING[match.first], match.second);

            LOOP_ASSERT(realMatch, EXPECTED_MATCH == realMatch);
        }

        if (verbose) {
            cout << "\nVerifying vector match routine with 0 start "
                 << "position." << endl;
        }

        vector<pair<int, int> > vMatch;

        {
            retCode = X.match(&vMatch, TEST_STRING, TEST_STRING_LEN);
            ASSERT(0 == retCode);

            const int  LEN = 3;
            const char EXPECTED_MATCHES[LEN][30] = { "bbasm_SecurityCache",
                                                     "bbasm",
                                                     "SecurityCache" };

            LOOP_ASSERT(vMatch.size(), LEN == (int)vMatch.size());

            for (int i = 0; i < LEN; ++i) {
                string realMatch(&TEST_STRING[vMatch[i].first],
                                 vMatch[i].second);

                LOOP2_ASSERT(i, realMatch, EXPECTED_MATCHES[i] == realMatch);
            }
        }

        if (verbose) {
            cout << "\nVerifying vector match routine with non-zero start "
                 << "position." << endl;
        }

        {
            const int startPosition = vMatch[0].second;

            retCode = X.match(&vMatch,
                              TEST_STRING,
                              TEST_STRING_LEN,
                              startPosition);
            ASSERT(0 == retCode);

            const int  LEN = 3;
            const char EXPECTED_MATCHES[LEN][30] = { "tweut_StringRef",
                                                     "tweut",
                                                     "StringRef" };

            LOOP_ASSERT(vMatch.size(), LEN == (int)vMatch.size());

            for (int i = 0; i < LEN; ++i) {
                string realMatch(&TEST_STRING[vMatch[i].first],
                                 vMatch[i].second);

                LOOP2_ASSERT(i, realMatch, EXPECTED_MATCHES[i] == realMatch);
            }
        }

        if (verbose) {
            cout << "\nClearing regular expression." << endl;
        }

        mX.clear();

        if (verbose) {
            cout << "\nVerifying unprepared state." << endl;
        }

        ASSERT(false == X.isPrepared());

        if (verbose) cout << "\nEnd of Breathing Test." << endl;
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
