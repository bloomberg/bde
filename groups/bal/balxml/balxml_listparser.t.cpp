// balxml_listparser.t.cpp                                            -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <balxml_listparser.h>

#include <bslim_testutil.h>

#include <bdlb_printmethods.h>

#include <bsl_cstdlib.h>
#include <bsl_cstring.h>
#include <bsl_iostream.h>
#include <bsl_istream.h>
#include <bsl_iterator.h>
#include <bsl_sstream.h>
#include <bsl_string.h>
#include <bsl_vector.h>

using namespace BloombergLP;
using namespace bsl;

// ============================================================================
//                             TEST PLAN
// ----------------------------------------------------------------------------
//                              Overview
//                              --------
// ----------------------------------------------------------------------------

// ============================================================================
//                     STANDARD BDE ASSERT TEST FUNCTION
// ----------------------------------------------------------------------------

namespace {

int testStatus = 0;

void aSsErT(bool condition, const char *message, int line)
{
    if (condition) {
        cout << "Error " __FILE__ "(" << line << "): " << message
             << "    (failed)" << endl;

        if (0 <= testStatus && testStatus <= 100) {
            ++testStatus;
        }
    }
}

}  // close unnamed namespace

// ============================================================================
//               STANDARD BDE TEST DRIVER MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT       BSLIM_TESTUTIL_ASSERT
#define ASSERTV      BSLIM_TESTUTIL_ASSERTV

#define LOOP_ASSERT  BSLIM_TESTUTIL_LOOP_ASSERT
#define LOOP0_ASSERT BSLIM_TESTUTIL_LOOP0_ASSERT
#define LOOP1_ASSERT BSLIM_TESTUTIL_LOOP1_ASSERT
#define LOOP2_ASSERT BSLIM_TESTUTIL_LOOP2_ASSERT
#define LOOP3_ASSERT BSLIM_TESTUTIL_LOOP3_ASSERT
#define LOOP4_ASSERT BSLIM_TESTUTIL_LOOP4_ASSERT
#define LOOP5_ASSERT BSLIM_TESTUTIL_LOOP5_ASSERT
#define LOOP6_ASSERT BSLIM_TESTUTIL_LOOP6_ASSERT

#define Q            BSLIM_TESTUTIL_Q   // Quote identifier literally.
#define P            BSLIM_TESTUTIL_P   // Print identifier and value.
#define P_           BSLIM_TESTUTIL_P_  // P(X) without '\n'.
#define T_           BSLIM_TESTUTIL_T_  // Print a tab (w/o newline).
#define L_           BSLIM_TESTUTIL_L_  // current Line number

// ============================================================================
//                       TEMPLATIZED OUTPUT FUNCTIONS
// ============================================================================

template <class T>
void printValue(bsl::ostream& out, const T& value)
{
    bdlb::PrintMethods::print(out, value, 0, -1);
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

void printValue(bsl::ostream& out, const char* value)
    // Specialize for char*.  Need to expand \r, \n, \t and surround with
    // DQUOTE characters.
{
    out << '"';

    while (*value) {
        out << printableCharacters[*value];
        ++value;
    }

    out << '"';
}

void printValue(bsl::ostream& out, const bsl::string& value)
    // Need to expand \r, \n, \t and surround with DQUOTE characters.
{
    printValue(out, value.c_str());
}

void printValue(bsl::ostream& out, const char& value)
    // Need to expand \r, \n, \t and surround with SQUOTE characters.
{
    out << '\'' << printableCharacters[value] << '\'';
}

// ============================================================================
//                   GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
// ----------------------------------------------------------------------------

// ============================================================================
//                       HELPER FUNCTIONS FOR TESTING
// ----------------------------------------------------------------------------

struct HelperFunctions {
    static int parse(int    *result, const char *data, int dataLength);
    static int parse(float  *result, const char *data, int dataLength);
    static int parse(double *result, const char *data, int dataLength);
};

int HelperFunctions::parse(int *result, const char *data, int dataLength)
{
    bsl::stringstream ss(bsl::string(data, dataLength));
    ss >> (*result);
    if (ss.fail()) return -1;                                         // RETURN
    return ss.eof() ? 0 : -1;
}

int HelperFunctions::parse(float *result, const char *data, int dataLength)
{
    bsl::stringstream ss(bsl::string(data, dataLength));
    ss >> (*result);
    if (ss.fail()) return -1;                                         // RETURN
    return ss.eof() ? 0 : -1;
}

int HelperFunctions::parse(double *result, const char *data, int dataLength)
{
    bsl::stringstream ss(bsl::string(data, dataLength));
    ss >> (*result);
    if (ss.fail()) return -1;                                         // RETURN
    return ss.eof() ? 0 : -1;
}

// ============================================================================
//                               USAGE EXAMPLE
// ----------------------------------------------------------------------------

// The following snippets of code illustrate the usage of this component.
// Suppose you had an input stream that contained a list of doubles.  The
// following 'loadDoublesFromListStream' function loads this data into an
// 'bsl::vector<double>':
//..
//  #include <balxml_listparser.h>
//
//  #include <bdlt_date.h>
//
//  #include <istream>
//  #include <iterator>
//  #include <vector>
//  #include <sstream>
//  #include <string>
//
//  using namespace BloombergLP;
//
int parseDouble(double *result, const char *data, int dataLength);

int loadDoublesFromListStream(bsl::vector<double> *result,
                              bsl::istream&        stream)
{
    enum { FAILURE = -1 };

    balxml::ListParser<bsl::vector<double> > parser(&parseDouble);

    if (0 != parser.beginParse(result)) {
        return FAILURE;                                               // RETURN
    }

    if (0 != parser.pushCharacters(bsl::istreambuf_iterator<char>(stream),
                                   bsl::istreambuf_iterator<char>())) {
        return FAILURE;                                               // RETURN
    }

    return parser.endParse();
}
//..
// The 'parseDouble' function is implemented as follows:
//..
int parseDouble(double *result, const char *data, int dataLength)
{
    bsl::stringstream ss(bsl::string(data, dataLength));
    ss >> (*result);
    return 0;
}
//..
// The following function demonstrates the 'loadDoublesFromListStream'
// function:
//..
void usageExample()
{
    const char INPUT[] = "1.5 2.0 3.8 1.0";

    bsl::vector<double> vec;
    bsl::istringstream  iss(INPUT);

    int result = loadDoublesFromListStream(&vec, iss);

    ASSERT(0   == result);
    ASSERT(4   == vec.size());
    ASSERT(1.5 == vec[0]);
    ASSERT(2.0 == vec[1]);
    ASSERT(3.8 == vec[2]);
    ASSERT(1.0 == vec[3]);
}
//..

// ============================================================================
//                               MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? bsl::atoi(argv[1]) : 0;
    int verbose = argc > 2;
    int veryVerbose = argc > 3;
    int veryVeryVerbose = argc > 4;

    bsl::cout << "TEST " << __FILE__ << " CASE " << test << bsl::endl;;

    switch (test) { case 0:  // Zero is always the leading case.
      case 3: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //
        // Concerns:
        //
        // Plan:
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "\nUSAGE EXAMPLE\n" << bsl::endl;

        usageExample();

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // THOROUGH TEST
        //   This will thoroughly test the 'balxml::ListParser' class.
        //
        // Concerns:
        //   The parser must reset the associated object when 'beginParse' is
        //   called.  It should decode incoming objects that may be passed
        //   through multiple calls of 'pushCharacters' and detect whitespace
        //   boundaries correctly.  The parsed object must be appended to the
        //   associated array.
        //
        // Plan:
        //   Use 'bsl::vector<int>' for the 'TYPE' parameter and use
        //   'HelperFunctions::parse(int*, const char*, int)' for the parse
        //   element callback.  For each item in a set of test data, create an
        //   'balxml::ListParser<TYPE>' object and associate it with an
        //   'bsl::vector<int>' object, 'mX', that has been filled with some
        //   arbitrary data.  Verify that 'mX' has been reset.
        //
        //   Push the characters from the dataset into the parser object and
        //   check that the return value is as expected.
        //
        //   Call 'endParse' and check that the return value is as expected.
        //   Finally, verify the contents of 'mX'.
        //
        // Testing:
        //   balxml::ListParser
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTHOROUGH TEST"
                          << "\n=============" << endl;

        const int MAX_NUM_PUSHES   = 10;
        const int MAX_NUM_ELEMENTS = 10;

        static const struct {
            int  d_lineNum;                         // source line number
            const struct {
                const char *d_chars;                    // characters to push
                bool        d_success;                  // true if push
                                                        // successful
            }    d_input[MAX_NUM_PUSHES];
            bool d_endSuccess;                      // true if 'endParse'
                                                    // successful
            int  d_resultData[MAX_NUM_ELEMENTS];    // expected result data
            int  d_resultLength;                    // result length
        } DATA[] = {
            //line  input                          endSuccess  result
            //----  -----                          ----------  ------
            //          chars      success                     resultLen
            //          -----      -------                     ---------

            { L_,   { { 0                  }  },   true,       {           },
                                                               0             },
            { L_,   { { "",        true    },
                      { 0                  }  },   true,       {           },
                                                               0             },
            { L_,   { { " ",       true    },
                      { 0                  }  },   true,       {           },
                                                               0             },
            { L_,   { { " ",       true    },
                      { " ",       true    },
                      { 0                  }  },   true,       {           },
                                                               0             },
            { L_,   { { " ",       true    },
                      { "  ",      true    },
                      { 0                  }  },   true,       {           },
                                                               0             },

            { L_,   { { "123",     true    },
                      { 0                  }  },   true,       { 123       },
                                                               1             },

            { L_,   { { "12 3",    true    },
                      { 0                  }  },   true,       { 12, 3     },
                                                               2             },
            { L_,   { { "12 3",    true    },
                      { "5",       true    },
                      { 0                  }  },   true,       { 12, 35    },
                                                               2             },
            { L_,   { { "12 3",    true    },
                      { "5",       true    },
                      { "9",       true    },
                      { 0                  }  },   true,       { 12, 359   },
                                                               2             },

            { L_,   { { "12 3",    true    },
                      { "5",       true    },
                      { "9 ",      true    },
                      { 0                  }  },   true,       { 12, 359   },
                                                               2             },

            { L_,   { { "12 3",    true    },
                      { "5 ",      true    },
                      { "9 ",      true    },
                      { 0                  }  },   true,       { 12, 35, 9 },
                                                               3             },

            { L_,   { { "12 3",    true    },
                      { "5",       true    },
                      { "A ",      false   }  }                              },

            { L_,   { { "12 3",    true    },
                      { "5",       true    },
                      { "A",       true    },
                      { 0                  }  },   false                     },


        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int i = 0; i < NUM_DATA; ++i) {
            const int LINE = DATA[i].d_lineNum;

            const int              INIT_DATA[]   = { 234, 211, 234, 21 };
            const int              NUM_INIT_DATA = sizeof INIT_DATA
                                                    / sizeof *INIT_DATA;
            const bsl::vector<int> INIT_VALUE(INIT_DATA,
                                              INIT_DATA + NUM_INIT_DATA);

            bsl::vector<int>        mX = INIT_VALUE;
            const bsl::vector<int>& X  = mX;

            typedef balxml::ListParser<bsl::vector<int> > Parser;

            Parser::ParseElementFunction temp = &HelperFunctions::parse;
            Parser                       parser(temp);
            int                          retCode;

            LOOP2_ASSERT(LINE, X.size(), 0 != X.size());

            retCode = parser.beginParse(&mX);
            LOOP2_ASSERT(LINE, retCode,  0 == retCode);
            LOOP2_ASSERT(LINE, X.size(), 0 == X.size());

            bool areAllPushesSuccessful = true;

            for (int j = 0; j < MAX_NUM_PUSHES; ++j) {
                const char *CHARS   = DATA[i].d_input[j].d_chars;
                const bool  SUCCESS = DATA[i].d_input[j].d_success;

                if (0 == CHARS) {
                    break;
                }

                if (veryVeryVerbose) {
                    T_ T_ P_(CHARS) P(SUCCESS)
                }

                const char *begin = CHARS;
                const char *end   = CHARS + bsl::strlen(CHARS);

                retCode = parser.pushCharacters(begin, end);

                if (!SUCCESS) {
                    areAllPushesSuccessful = false;

                    LOOP3_ASSERT(LINE, j, retCode, 0 != retCode);

                    break;
                }

                LOOP3_ASSERT(LINE, j, retCode, 0 == retCode);
            }

            if (!areAllPushesSuccessful) {
                continue;
            }

            const bool END_SUCCESS = DATA[i].d_endSuccess;

            retCode = parser.endParse();

            if (!END_SUCCESS) {
                if (veryVerbose) {
                    T_ P(END_SUCCESS)
                }

                LOOP2_ASSERT(LINE, retCode, 0 != retCode);

                continue;
            }

            LOOP2_ASSERT(LINE, retCode, 0 == retCode);

            const int *EXPECTED_RESULT_DATA   = DATA[i].d_resultData;
            const int  EXPECTED_RESULT_LENGTH = DATA[i].d_resultLength;

            if (veryVerbose) {
                T_ P_(END_SUCCESS)
                   P_(EXPECTED_RESULT_DATA)
                   P (EXPECTED_RESULT_LENGTH)
            }

            LOOP3_ASSERT(LINE, EXPECTED_RESULT_LENGTH,   X.size(),
                               EXPECTED_RESULT_LENGTH == X.size());

            for (int j = 0; j < X.size(); ++j) {
                LOOP4_ASSERT(LINE, j, EXPECTED_RESULT_DATA[j],   X[j],
                                      EXPECTED_RESULT_DATA[j] == X[j]);
            }
        }

        if (verbose) cout << "\nEnd of Test." << endl;
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //
        // Concerns:
        //
        // Plan:
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "\nBREATHING TEST\n" << bsl::endl;

        {
            typedef int                          ValueType;
            typedef bsl::vector<ValueType>       ArrayType;
            typedef balxml::ListParser<ArrayType> ListParser;

            ArrayType vec;

            const char INPUT[] = "  123 456    789";

            ListParser::ParseElementFunction temp2 = &HelperFunctions::parse;
            ListParser::ParseElementCallback temp(temp2);

            ListParser parser(temp);

            int        result;

            result = parser.beginParse(&vec);
            LOOP_ASSERT(result, 0 == result);

            result = parser.pushCharacters(INPUT, INPUT + sizeof(INPUT) - 1);
            LOOP_ASSERT(result, 0 == result);

            result = parser.endParse();
            LOOP_ASSERT(result, 0 == result);

            LOOP_ASSERT(vec.size(), 3 == vec.size());
            LOOP_ASSERT(vec[0],     123 == vec[0]);
            LOOP_ASSERT(vec[1],     456 == vec[1]);
            LOOP_ASSERT(vec[2],     789 == vec[2]);
        }

      } break;
      default: {
        bsl::cerr << "WARNING: CASE `" << test << "' NOT FOUND." << bsl::endl;
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        bsl::cerr << "Error, non-zero test status = "
                  << testStatus
                  << "."
                  << bsl::endl;
    }
    return testStatus;
}

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
