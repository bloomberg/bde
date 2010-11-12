// baexml_decoder.t.cpp    -*-C++-*-

// Working around Linux compiler problem (GCC 3.4.4)
// =================================================
// The reason we are not including <baexml_decoder.h> as the first
// included file in this test driver is because we are working around a problem
// with this compiler.  The only known workaround (currently) is to change to
// order of the include files.  Note that the 'baexml_decoder.cpp' file
// *does* include <baexml_decoder.h> and compiles successfully on all
// supported compilers.  The problem only occurs in this test driver because of
// the nature of the tests we are doing.
//
// The following snippets of code illustrate the problem:
//..
//  struct A { void a() { } };
//  struct B { void b() { } };
//
//  // SECTION 1
//
//  namespace funcNamespace {
//      template <typename TYPE, typename OTHER_TYPE>
//      void func(TYPE object, OTHER_TYPE other)
//      {
//          object.a();
//      }
//  }
//
//  // SECTION 2
//
//  template <typename TYPE>
//  void invokeFunc(TYPE object)
//  {
//      int other;
//
//      funcNamespace::func(object, other);
//  }
//
//  // SECTION 3
//
//  namespace funcNamespace {
//      template <typename OTHER_TYPE>
//      void func(B object, OTHER_TYPE other)
//      {
//          object.b();
//      }
//  }
//
//  // SECTION 4
//
//  int main()
//  {
//      B object;
//      invokeFunc(object);     // <-- Point of instantiation
//  }
//..
// In the above program, 'SECTION 1' defines a function parameterized with 2
// template arguments.  'SECTION 3' defines an *overloaded* (not specialized)
// function parameterized with 1 template argument.  Both functions are inside
// 'funcNamespace'.
//
// 'SECTION 2' defines an 'invoker' function template that is instantiated in
// 'SECTION 4'.  Since 'object' is of type 'B', the function in 'SECTION 3'
// should be selected, however GCC 3.4.4 selects the function in 'SECTION 1',
// causing a compilation error:
//..
//  test.cpp: In function `void funcNamespace::func(TYPE, OTHER_TYPE) [
//                                                     with TYPE       = B,
//                                                          OTHER_TYPE = int]':
//  test.cpp:21:   instantiated from `void invokeFunc(TYPE) [with TYPE = B]'
//  test.cpp:39:   instantiated from here
//  test.cpp:10: error: 'struct B' has no member named 'a'
//..
// If 'SECTION 3' is placed above 'SECTION 2', the program compiles.
//
// To work around this problem in this test driver, we need to rearrange the
// order of the included files.

#include <bdeat_sequencefunctions.h>
#include <bdeat_choicefunctions.h>
#include <bdeat_attributeinfo.h>
#include <bdeat_selectioninfo.h>
#include <bslma_testallocator.h>
#include <bslmf_issame.h>
#include <bdeu_printmethods.h>  // for printing vector

#include <bdem_list.h>              //  from xml)
#include <bdem_row.h>
#include <bdem_schema.h>
#include <bdem_table.h>
#include <bdem_schemaaggregateutil.h>
#include <bcem_aggregate.h>
#include <bdesb_fixedmeminstreambuf.h>

#include <bcema_sharedptr.h>

#include <baexml_schemaparser.h>
#include <baexml_minireader.h>

#include <bsl_typeinfo.h>
#include <bsl_fstream.h>
#include <bsl_iostream.h>
#include <bsl_sstream.h>
#include <bsl_list.h>

using namespace BloombergLP;

using bsl::cout;
using bsl::cerr;
using bsl::endl;
using bsl::atoi;
using bsl::flush;

//=============================================================================
//                                 TEST PLAN
//-----------------------------------------------------------------------------
//                                  Overview
//                                  --------
// The component under test is a decoder that decodes XML input to produce a
// value for a parameterized 'TYPE'.  Each 'TYPE' is represented by a context
// class that implements the 'Decoder_ElementContext' protocol.  This
// component provides context classes for the following types:
//
//     - 'bsl::string' (parsed as UTF8).
//     - 'bsl::vector<char>' (parsed as Base64).
//     - other 'bsl::vector's.
//     - types that support 'bdeat_SequenceFunctions'.
//     - types that support 'bdeat_ChoiceFunctions'.
//     - simple types that can be parsed by 'baexml_typesparserutil'.
//
// After breathing the component [1], we will first test the internal 'Parser'
// class through the 'Decoder' interface [2].  The purpose of
// this test will be to establish that the parser uses the
// 'Decoder_ElementContext' protocol correctly.  A customized 'TestContext'
// will be used for this purpose.  Then we will test the
// 'Decoder_SelectContext' meta-function [3] to check that, given a
// particular 'TYPE', the meta-function returns an appropriate context type
// that can be used by the parser to parse that 'TYPE'.
//
// Once we have established that the parser is working correctly, we can start
// testing the context types defined in this component.  The
// 'Decoder_UTF8Context' [4] and 'Decoder_Base64Context' [5] context
// types are tested first, because they are simple and non-templated.  Next,
// the 'Decoder_SimpleContext<TYPE>' [6] template is tested.
//
// The 'Decoder_SequenceContext<TYPE>' [8] and the
// 'Decoder_ChoiceContext<TYPE>' [9] templates make use of the
// 'Decoder_PrepareSubContext' [7] function class.  Therefore, this
// function class must be tested prior to these two class templates.
//
// The 'Decoder_VectorContext<TYPE>' [10] test makes use of the
// 'Decoder_SequenceContext<TYPE>' template, so it must be tested after the
// sequence test.
//
// At this point, the main functionality of this component has been thoroughly
// tested.  Now we need to test the 4 'decode' functions [11] in the
// 'Decoder' namespace.  These tests are trivial and only involve testing
// that the arguments and return values are passed correctly and that the input
// streams are invalidated if there is an error.
//
// Finally, we will test the usage example from the component-level
// documentation to check that it compiles and runs as expected.
//
// Note that the 'Decoder_ErrorReporter' and 'Decoder_ElementContext'
// protocol classes are tested implicitly in all test cases.
//-----------------------------------------------------------------------------
// [11] int baexml_Decoder::decode(sbuf*, TYPE, b_A*);
// [11] int baexml_Decoder::decode(sbuf*, TYPE, ostrm&, ostrm&, b_A*);
// [11] int baexml_Decoder::decode(istrm&, TYPE, b_A*);
// [11] int baexml_Decoder::decode(istrm&, TYPE, ostrm&, ostrm&, b_A*);
// [ 3] baexml_Decoder_SelectContext
// [ 2] baexml_Decoder_ParserUtil
// [ 5] baexml_Decoder_Base64Context
// [ 9] baexml_Decoder_ChoiceContext<TYPE>
// [ 8] baexml_Decoder_SequenceContext<TYPE>
// [ 6] baexml_Decoder_SimpleContext<TYPE>
// [ 4] baexml_Decoder_UTF8Context
// [10] baexml_Decoder_VectorContext<TYPE>
// [ 7] baexml_Decoder_PrepareSubContext
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [16] USAGE EXAMPLES
// [15] XML SCHEMA PARSING AND BDEM BINDING ADAPTOR DECODING
//-----------------------------------------------------------------------------

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
void printValue(bsl::ostream& out, const T& value)
{
    bdeu_PrintMethods::print(out, value, 0, -1);
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
        out << printableCharacters[(unsigned char)*value];
        ++value;
    }

    out << '"';
}

void printValue(bsl::ostream& out, const bsl::string& value)
    // Need to expand \r, \n, \t and surround with DQUOTE characters.
{
    printValue(out, value.c_str());
}

#if 0

void printValue(bsl::ostream& out, const bdeut_StringRef& value)
    // Need to expand \r, \n, \t and surround with DQUOTE characters.
{
    out << '"';

    const char *begin = value.begin();
    const char *end   = value.end();

    while (begin != end) {
        out << printableCharacters[*begin];
        ++begin;
    }

    out << '"';
}

#endif

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

int verbose;
int veryVerbose;
int veryVeryVerbose;
int veryVeryVeryVerbose;

//=============================================================================
//                        GLOBAL CLASSES FOR TESTING
//-----------------------------------------------------------------------------

namespace baexml_Decoder_TestNamespace {

                            // ====================
                            // struct TestSequence0
                            // ====================

struct TestSequence0 {
    // A test sequence with 0 attributes.
};

                            // ====================
                            // struct TestSequence1
                            // ====================

struct TestSequence1 {
    // A test sequence with 1 attributes.

    static bdeat_AttributeInfo ATTRIBUTE_INFO_ARRAY[];

    static const int DEFAULT_ELEMENT1;

    int d_element1;

    TestSequence1()
    : d_element1(DEFAULT_ELEMENT1)
    {
    }

    explicit TestSequence1(int element1)
    : d_element1(element1)
    {
    }
};

bool operator==(const TestSequence1& lhs, const TestSequence1& rhs)
{
    return lhs.d_element1 == rhs.d_element1;
}

bsl::ostream& operator<<(bsl::ostream& stream, const TestSequence1& rhs)
{
    printValue(stream, rhs.d_element1);
    return stream;
}

bdeat_AttributeInfo TestSequence1::ATTRIBUTE_INFO_ARRAY[] = {
    { 1, "E1", 2, "Element 1" },
};

const int TestSequence1::DEFAULT_ELEMENT1 = 0x71DEFA17;

                            // ====================
                            // struct TestSequence2
                            // ====================

struct TestSequence2 {
    // A test sequence with 2 attributes.

    static bdeat_AttributeInfo ATTRIBUTE_INFO_ARRAY[];

    // We need default values for elements in 'TestSequence2' because this
    // class is used when testing the 'Decoder::decode' functions and we
    // need to test that elements are reset when these functions are called.

    static const int         DEFAULT_ELEMENT1;
    static const bsl::string DEFAULT_ELEMENT2;

    int         d_element1;
    bsl::string d_element2;

    TestSequence2()
    : d_element1(DEFAULT_ELEMENT1)
    , d_element2(DEFAULT_ELEMENT2)
    {
    }

    TestSequence2(int                element1,
                  const bsl::string& element2)
    : d_element1(element1)
    , d_element2(element2)
    {
    }
};

bool operator==(const TestSequence2& lhs, const TestSequence2& rhs)
{
    return lhs.d_element1 == rhs.d_element1
        && lhs.d_element2 == rhs.d_element2;
}

bsl::ostream& operator<<(bsl::ostream& stream, const TestSequence2& rhs)
{
    printValue(stream, rhs.d_element1);
    stream << ", ";
    printValue(stream, rhs.d_element2);
    return stream;
}

bdeat_AttributeInfo TestSequence2::ATTRIBUTE_INFO_ARRAY[] = {
    { 1, "E1", 2, "Element 1" },
    { 2, "E2", 2, "Element 2" },
};

const int         TestSequence2::DEFAULT_ELEMENT1
                                           = 0x72DEFA17;
const bsl::string TestSequence2::DEFAULT_ELEMENT2
                                           = "TestSequence2::DEFAULT_ELEMENT2";

                             // ==================
                             // struct TestChoice0
                             // ==================

struct TestChoice0 {
    // A test choice with 0 selections.
};

                             // ==================
                             // struct TestChoice1
                             // ==================

struct TestChoice1 {
    // A test choice with 1 selection.

    static const bdeat_SelectionInfo SELECTION_INFO_ARRAY[];

    int d_choice;  // -1 indicates nothing selected

    int d_selection1;

    TestChoice1()
    : d_choice(-1)
    {
    }

    explicit TestChoice1(int selection1)
    : d_choice(0)
    , d_selection1(selection1)
    {
    }
};

bool operator==(const TestChoice1& lhs, const TestChoice1& rhs)
{
    if (lhs.d_choice != rhs.d_choice) {
        return 0;
    }

    switch (rhs.d_choice) {
      case 0: {
        if (lhs.d_selection1 != rhs.d_selection1) {
            return 0;
        }
      } break;
      default: {
        ASSERT(-1 == rhs.d_choice);
      } break;
    }

    return 1;
}

bsl::ostream& operator<<(bsl::ostream& stream, const TestChoice1& rhs)
{
    switch (rhs.d_choice) {
      case 0: {
        printValue(stream, rhs.d_selection1);
      } break;
      default: {
        ASSERT(-1 == rhs.d_choice);
        printValue(stream, "(* NULL *)");
      } break;
    }

    return stream;
}

const bdeat_SelectionInfo TestChoice1::SELECTION_INFO_ARRAY[] = {
    { 1, "S1", 2, "Selection 1" },
};

                             // ==================
                             // struct TestChoice2
                             // ==================

struct TestChoice2 {
    // A test choice with 2 selections.

    static const bdeat_SelectionInfo SELECTION_INFO_ARRAY[];

    int d_choice;  // -1 indicates nothing selected

    int         d_selection1;
    bsl::string d_selection2;

    TestChoice2()
    : d_choice(-1)
    {
    }

    explicit TestChoice2(int selection1)
    : d_choice(0)
    , d_selection1(selection1)
    {
    }

    explicit TestChoice2(const bsl::string& selection2)
    : d_choice(1)
    , d_selection2(selection2)
    {
    }
};

bool operator==(const TestChoice2& lhs, const TestChoice2& rhs)
{
    if (lhs.d_choice != rhs.d_choice) {
        return 0;
    }

    switch (rhs.d_choice) {
      case 0: {
        if (lhs.d_selection1 != rhs.d_selection1) {
            return 0;
        }
      } break;
      case 1: {
        if (lhs.d_selection2 != rhs.d_selection2) {
            return 0;
        }
      } break;
      default: {
        ASSERT(-1 == rhs.d_choice);
      } break;
    }

    return 1;
}

bsl::ostream& operator<<(bsl::ostream& stream, const TestChoice2& rhs)
{
    switch (rhs.d_choice) {
      case 0: {
        printValue(stream, rhs.d_selection1);
      } break;
      case 1: {
        printValue(stream, rhs.d_selection2);
      } break;
      default: {
        ASSERT(-1 == rhs.d_choice);
        printValue(stream, "(* NULL *)");
      } break;
    }

    return stream;
}

const bdeat_SelectionInfo TestChoice2::SELECTION_INFO_ARRAY[] = {
    { 1, "S1", 2, "Selection 1" },
    { 2, "S2", 2, "Selection 2" },
};

                          // ========================
                          // class TestVectorElemType
                          // ========================

class TestVectorElemType {
    // This class will be used as the element type for the vector inside
    // 'TestSequenceWithVector' (see below).

    // PRIVATE DATA MEMBERS
    bsl::string d_callSequence;  // log of call sequence for this element

  public:
    // CREATORS
    // Generated by compiler:
    // TestVectorElemType();
    // TestVectorElemType(const TestVectorElemType&);
    // ~TestVectorElemType();

    // MANIPULATORS
    // Generated by compiler:
    // TestVectorElemType& operator=(TestVectorElemType&);

    void addCall(const bsl::string& call);
        // Add the specified 'call' to the call sequence for this element.

    // ACCESSORS
    const bsl::string& callSequence() const;
        // Return a reference to the non-modifiable call sequence string for
        // this element.
};

// FREE OPERATORS
bool operator==(const TestVectorElemType& lhs, const TestVectorElemType& rhs);
    // Return 'true' if the specified 'lhs' has the same call sequence as the
    // specified 'rhs'.

bsl::ostream& operator<<(bsl::ostream& stream, const TestVectorElemType& rhs);
    // Output the specified 'rhs' to the specified 'stream'.

                          // ------------------------
                          // class TestVectorElemType
                          // ------------------------

// MANIPULATORS

void TestVectorElemType::addCall(const bsl::string& call)
{
    d_callSequence.append(call);
}

// ACCESSORS

const bsl::string& TestVectorElemType::callSequence() const
{
    return d_callSequence;
}

// FREE OPERATORS

bool operator==(const TestVectorElemType& lhs, const TestVectorElemType& rhs)
{
    return lhs.callSequence() == rhs.callSequence();
}

bsl::ostream& operator<<(bsl::ostream& stream, const TestVectorElemType& rhs)
{
    printValue(stream, rhs.callSequence());
    return stream;
}

                       // =============================
                       // struct TestSequenceWithVector
                       // =============================

struct TestSequenceWithVector {
    // A test sequence with 1 vector attribute.

    static bdeat_AttributeInfo ATTRIBUTE_INFO_ARRAY[];

    bsl::vector<TestVectorElemType> d_vector;
};

bool operator==(const TestSequenceWithVector& lhs,
                const TestSequenceWithVector& rhs)
{
    return lhs.d_vector == rhs.d_vector;
}

bsl::ostream& operator<<(bsl::ostream&                 stream,
                         const TestSequenceWithVector& rhs)
{
    printValue(stream, rhs.d_vector);
    return stream;
}

bdeat_AttributeInfo TestSequenceWithVector::ATTRIBUTE_INFO_ARRAY[] = {
    { 1, "Elem", 4, "Vector Element" },
};

}  // close namespace baexml_Decoder_TestNamespace

using baexml_Decoder_TestNamespace::TestSequence0;
using baexml_Decoder_TestNamespace::TestSequence1;
using baexml_Decoder_TestNamespace::TestSequence2;
using baexml_Decoder_TestNamespace::TestChoice0;
using baexml_Decoder_TestNamespace::TestChoice1;
using baexml_Decoder_TestNamespace::TestChoice2;
using baexml_Decoder_TestNamespace::TestVectorElemType;
using baexml_Decoder_TestNamespace::TestSequenceWithVector;

            // ===================================================
            // bdeat_SequenceFunctions Overrides For TestSequence0
            // ===================================================

namespace BloombergLP {

namespace bdeat_SequenceFunctions {
    template <>
    struct IsSequence<TestSequence0> : bslmf_MetaInt<1> {
    };

    // MANIPULATORS
    template <typename MANIPULATOR>
    int manipulateAttribute(TestSequence0 *object,
                            MANIPULATOR&   manipulator,
                            const char    *attributeName,
                            int            attributeNameLength)
    {
        return -1;
    }

    template <typename MANIPULATOR>
    int manipulateAttribute(TestSequence0 *object,
                            MANIPULATOR&   manipulator,
                            int            attributeId)
    {
        return -1;
    }

    template <typename MANIPULATOR>
    int manipulateAttributes(TestSequence0 *object,
                             MANIPULATOR&   manipulator)
    {
        return -1;
    }

    // ACCESSORS
    template <typename ACCESSOR>
    int accessAttribute(const TestSequence0&  object,
                        ACCESSOR&             accessor,
                        const char           *attributeName,
                        int                   attributeNameLength)
    {
        return -1;
    }

    template <typename ACCESSOR>
    int accessAttribute(const TestSequence0& object,
                        ACCESSOR&            accessor,
                        int                  attributeId)
    {
        return -1;
    }

    template <typename ACCESSOR>
    int accessAttributes(const TestSequence0& object,
                         ACCESSOR&            accessor)
    {
        return -1;
    }

    bool hasAttribute(const TestSequence0&  object,
                      const char           *attributeName,
                      int                   attributeNameLength)
    {
        return false;
    }

    bool hasAttribute(const TestSequence0& object,
                      int                  attributeId)
    {
        return false;
    }

}  // close namespace bdeat_SequenceFunctions
}  // close namespace BloombergLP

            // ===================================================
            // bdeat_SequenceFunctions Overrides For TestSequence1
            // ===================================================

namespace BloombergLP {

namespace bdeat_SequenceFunctions {
    template <>
    struct IsSequence<TestSequence1> : bslmf_MetaInt<1> {
    };

    // MANIPULATORS
    template <typename MANIPULATOR>
    int manipulateAttribute(TestSequence1 *object,
                            MANIPULATOR&   manipulator,
                            const char    *attributeName,
                            int            attributeNameLength)
    {
        if (bsl::string(attributeName, attributeNameLength)
                          == TestSequence1::ATTRIBUTE_INFO_ARRAY[0].d_name_p) {
            return manipulator(&object->d_element1,
                               TestSequence1::ATTRIBUTE_INFO_ARRAY[0]);
        }

        return -1;
    }

    template <typename MANIPULATOR>
    int manipulateAttribute(TestSequence1 *object,
                            MANIPULATOR&   manipulator,
                            int            attributeId)
    {
        if (attributeId == TestSequence1::ATTRIBUTE_INFO_ARRAY[0].d_id) {
            return manipulator(&object->d_element1,
                               TestSequence1::ATTRIBUTE_INFO_ARRAY[0]);
        }

        return -1;
    }

    template <typename MANIPULATOR>
    int manipulateAttributes(TestSequence1 *object,
                             MANIPULATOR&   manipulator)
    {
        if (0 == manipulator(&object->d_element1,
                             TestSequence1::ATTRIBUTE_INFO_ARRAY[0])) {
            return 0;
        }

        return -1;
    }

    // ACCESSORS
    template <typename ACCESSOR>
    int accessAttribute(const TestSequence1&  object,
                        ACCESSOR&             accessor,
                        const char           *attributeName,
                        int                   attributeNameLength)
    {
        if (bsl::string(attributeName, attributeNameLength)
                          == TestSequence1::ATTRIBUTE_INFO_ARRAY[0].d_name_p) {
            return accessor(object.d_element1,
                            TestSequence1::ATTRIBUTE_INFO_ARRAY[0]);
        }

        return -1;
    }

    template <typename ACCESSOR>
    int accessAttribute(const TestSequence1& object,
                        ACCESSOR&            accessor,
                        int                  attributeId)
    {
        if (attributeId == TestSequence1::ATTRIBUTE_INFO_ARRAY[0].d_id) {
            return accessor(object.d_element1,
                            TestSequence1::ATTRIBUTE_INFO_ARRAY[0]);
        }

        return -1;
    }

    template <typename ACCESSOR>
    int accessAttributes(const TestSequence1& object,
                         ACCESSOR&            accessor)
    {
        if (0 == accessor(object.d_element1,
                          TestSequence1::ATTRIBUTE_INFO_ARRAY[0])) {
            return 0;
        }

        return -1;
    }

    bool hasAttribute(const TestSequence1&  object,
                      const char           *attributeName,
                      int                   attributeNameLength)
    {
        if (bsl::string(attributeName, attributeNameLength)
                          == TestSequence1::ATTRIBUTE_INFO_ARRAY[0].d_name_p) {
            return true;
        }
        return false;
    }

    bool hasAttribute(const TestSequence1& object,
                      int                  attributeId)
    {
        if (attributeId == TestSequence1::ATTRIBUTE_INFO_ARRAY[0].d_id) {
            return true;
        }
        return false;
    }

}  // close namespace bdeat_SequenceFunctions
}  // close namespace BloombergLP

            // ===================================================
            // bdeat_SequenceFunctions Overrides For TestSequence2
            // ===================================================

namespace BloombergLP {

namespace bdeat_SequenceFunctions {
    template <>
    struct IsSequence<TestSequence2> : bslmf_MetaInt<1> {
    };

    // MANIPULATORS
    template <typename MANIPULATOR>
    int manipulateAttribute(TestSequence2 *object,
                            MANIPULATOR&   manipulator,
                            const char    *attributeName,
                            int            attributeNameLength)
    {
        if (bsl::string(attributeName, attributeNameLength)
                          == TestSequence2::ATTRIBUTE_INFO_ARRAY[0].d_name_p) {
            return manipulator(&object->d_element1,
                               TestSequence2::ATTRIBUTE_INFO_ARRAY[0]);
        }
        if (bsl::string(attributeName, attributeNameLength)
                          == TestSequence2::ATTRIBUTE_INFO_ARRAY[1].d_name_p) {
            return manipulator(&object->d_element2,
                               TestSequence2::ATTRIBUTE_INFO_ARRAY[1]);
        }

        return -1;
    }

    template <typename MANIPULATOR>
    int manipulateAttribute(TestSequence2 *object,
                            MANIPULATOR&   manipulator,
                            int            attributeId)
    {
        if (attributeId == TestSequence2::ATTRIBUTE_INFO_ARRAY[0].d_id) {
            return manipulator(&object->d_element1,
                               TestSequence2::ATTRIBUTE_INFO_ARRAY[0]);
        }
        if (attributeId == TestSequence2::ATTRIBUTE_INFO_ARRAY[1].d_id) {
            return manipulator(&object->d_element2,
                               TestSequence2::ATTRIBUTE_INFO_ARRAY[1]);
        }

        return -1;
    }

    template <typename MANIPULATOR>
    int manipulateAttributes(TestSequence2 *object,
                             MANIPULATOR&   manipulator)
    {
        if (0 == manipulator(&object->d_element1,
                             TestSequence2::ATTRIBUTE_INFO_ARRAY[0])
         && 0 == manipulator(&object->d_element2,
                             TestSequence2::ATTRIBUTE_INFO_ARRAY[1])) {
            return 0;
        }

        return -1;
    }

    // ACCESSORS
    template <typename ACCESSOR>
    int accessAttribute(const TestSequence2&  object,
                        ACCESSOR&             accessor,
                        const char           *attributeName,
                        int                   attributeNameLength)
    {
        if (bsl::string(attributeName, attributeNameLength)
                          == TestSequence2::ATTRIBUTE_INFO_ARRAY[0].d_name_p) {
            return accessor(object.d_element1,
                            TestSequence2::ATTRIBUTE_INFO_ARRAY[0]);
        }
        if (bsl::string(attributeName, attributeNameLength)
                          == TestSequence2::ATTRIBUTE_INFO_ARRAY[1].d_name_p) {
            return accessor(object.d_element2,
                            TestSequence2::ATTRIBUTE_INFO_ARRAY[1]);
        }

        return -1;
    }

    template <typename ACCESSOR>
    int accessAttribute(const TestSequence2& object,
                        ACCESSOR&            accessor,
                        int                  attributeId)
    {
        if (attributeId == TestSequence2::ATTRIBUTE_INFO_ARRAY[0].d_id) {
            return accessor(object.d_element1,
                            TestSequence2::ATTRIBUTE_INFO_ARRAY[0]);
        }
        if (attributeId == TestSequence2::ATTRIBUTE_INFO_ARRAY[1].d_id) {
            return accessor(object.d_element2,
                            TestSequence2::ATTRIBUTE_INFO_ARRAY[1]);
        }

        return -1;
    }

    template <typename ACCESSOR>
    int accessAttributes(const TestSequence2& object,
                         ACCESSOR&            accessor)
    {
        if (0 == accessor(object.d_element1,
                          TestSequence2::ATTRIBUTE_INFO_ARRAY[0])
         && 0 == accessor(object.d_element2,
                          TestSequence2::ATTRIBUTE_INFO_ARRAY[1])) {
            return 0;
        }

        return -1;
    }

    bool hasAttribute(const TestSequence2&  object,
                      const char           *attributeName,
                      int                   attributeNameLength)
    {
        if (bsl::string(attributeName, attributeNameLength)
                          == TestSequence2::ATTRIBUTE_INFO_ARRAY[0].d_name_p) {
            return true;
        }
        if (bsl::string(attributeName, attributeNameLength)
                          == TestSequence2::ATTRIBUTE_INFO_ARRAY[1].d_name_p) {
            return true;
        }
        return false;
    }

    bool hasAttribute(const TestSequence2& object,
                      int                  attributeId)
    {
        if (attributeId == TestSequence2::ATTRIBUTE_INFO_ARRAY[0].d_id) {
            return true;
        }
        if (attributeId == TestSequence2::ATTRIBUTE_INFO_ARRAY[1].d_id) {
            return true;
        }
        return false;
    }
}  // close namespace bdeat_SequenceFunctions
}  // close namespace BloombergLP

              // ===============================================
              // bdeat_ChoiceFunctions Overrides For TestChoice0
              // ===============================================

namespace BloombergLP {

namespace bdeat_ChoiceFunctions {
    template <>
    struct IsChoice<TestChoice0> : bslmf_MetaInt<1> {
    };

    // MANIPULATORS
    int makeSelection(TestChoice0 *object, int selectionId)
    {
        return -1;
    }

    int makeSelection(TestChoice0 *object, const char *name, int nameLength)
    {
        return -1;
    }

    template <typename MANIPULATOR>
    int manipulateSelection(TestChoice0 *object, MANIPULATOR& manipulator)
    {
        return -1;
    }

    // ACCESSORS
    template <typename ACCESSOR>
    int accessSelection(const TestChoice0& object, ACCESSOR& accessor)
    {
        return -1;
    }

    int selectionId(const TestChoice0& object)
    {
        return -1;
    }

    bool hasSelection(const TestChoice0& object, const char *name, int length)
    {
        return false;
    }

    bool hasSelection(const TestChoice0& object, int id)
    {
        return false;
    }

}  // close namespace bdeat_ChoiceFunctions
}  // close namespace BloombergLP

              // ===============================================
              // bdeat_ChoiceFunctions Overrides For TestChoice1
              // ===============================================

namespace BloombergLP {

namespace bdeat_ChoiceFunctions {
    template <>
    struct IsChoice<TestChoice1> : bslmf_MetaInt<1> {
    };

    // MANIPULATORS
    int makeSelection(TestChoice1 *object, int selectionId)
    {
        ASSERT(1 == selectionId);

        object->d_selection1 = 0;
        object->d_choice = selectionId - 1;

        return 0;
    }

    int makeSelection(TestChoice1 *object, const char *name, int nameLength)
    {
        if ("S1" == bsl::string(name, nameLength)) {
            object->d_selection1 = 0;
            object->d_choice = 0;
            return 0;
        }

        return -1;
    }

    template <typename MANIPULATOR>
    int manipulateSelection(TestChoice1 *object, MANIPULATOR& manipulator)
    {
        if (0 == object->d_choice) {
            return manipulator(&object->d_selection1,
                               TestChoice1::SELECTION_INFO_ARRAY[0]);
        }

        return -1;
    }

    // ACCESSORS
    template <typename ACCESSOR>
    int accessSelection(const TestChoice1& object, ACCESSOR& accessor)
    {
        if (0 == object.d_choice) {
            return accessor(object.d_selection1,
                            TestChoice1::SELECTION_INFO_ARRAY[0]);
        }

        return -1;
    }

    int selectionId(const TestChoice1& object)
    {
        return object.d_choice;
    }

    bool hasSelection(const TestChoice1& object, const char *name,
                                                 int nameLength)
    {
        if ("S1" == bsl::string(name, nameLength)) {
            return true;
        }
        return false;
    }

    bool hasSelection(const TestChoice1& object, int id)
    {
        if (0 == id) {
            return true;
        }
        return false;
    }

}  // close namespace bdeat_ChoiceFunctions
}  // close namespace BloombergLP

              // ===============================================
              // bdeat_ChoiceFunctions Overrides For TestChoice2
              // ===============================================

namespace BloombergLP {

namespace bdeat_ChoiceFunctions {
    template <>
    struct IsChoice<TestChoice2> : bslmf_MetaInt<1> {
    };

    // MANIPULATORS
    int makeSelection(TestChoice2 *object, int selectionId)
    {
        ASSERT(1 == selectionId || 2 == selectionId);

        object->d_selection1 = 0;
        object->d_selection2 = "";
        object->d_choice = selectionId - 1;

        return 0;
    }

    int makeSelection(TestChoice2 *object, const char *name, int nameLength)
    {
        if ("S1" == bsl::string(name, nameLength)) {
            object->d_selection1 = 0;
            object->d_choice = 0;
            return 0;
        }
        else if ("S2" == bsl::string(name, nameLength)) {
            object->d_selection2 = "";
            object->d_choice = 1;
            return 0;
        }

        return -1;
    }

    template <typename MANIPULATOR>
    int manipulateSelection(TestChoice2 *object, MANIPULATOR& manipulator)
    {
        if (0 == object->d_choice) {
            return manipulator(&object->d_selection1,
                               TestChoice2::SELECTION_INFO_ARRAY[0]);
        }
        if (1 == object->d_choice) {
            return manipulator(&object->d_selection2,
                               TestChoice2::SELECTION_INFO_ARRAY[1]);
        }

        return -1;
    }

    // ACCESSORS
    template <typename ACCESSOR>
    int accessSelection(const TestChoice2& object, ACCESSOR& accessor)
    {
        if (0 == object.d_choice) {
            return accessor(object.d_selection1,
                            TestChoice2::SELECTION_INFO_ARRAY[0]);
        }
        if (1 == object.d_choice) {
            return accessor(object.d_selection2,
                            TestChoice2::SELECTION_INFO_ARRAY[1]);
        }

        return -1;
    }

    int selectionId(const TestChoice2& object)
    {
        return object.d_choice;
    }

    bool hasSelection(const TestChoice2& object, const char *name,
                                                 int nameLength)
    {
        if ("S1" == bsl::string(name, nameLength)) {
            return true;
        }
        if ("S2" == bsl::string(name, nameLength)) {
            return true;
        }
        return false;
    }

    bool hasSelection(const TestChoice2& object, int id)
    {
        if (0 == id) {
            return true;
        }
        if (1 == id) {
            return true;
        }
        return false;
    }

}  // close namespace bdeat_ChoiceFunctions
}  // close namespace BloombergLP

        // ============================================================
        // bdeat_SequenceFunctions Overrides For TestSequenceWithVector
        // ============================================================

namespace BloombergLP {

namespace bdeat_SequenceFunctions {
    template <>
    struct IsSequence<TestSequenceWithVector> : bslmf_MetaInt<1> {
    };

    // MANIPULATORS
    template <typename MANIPULATOR>
    int manipulateAttribute(TestSequenceWithVector *object,
                            MANIPULATOR&            manipulator,
                            const char             *attributeName,
                            int                     attributeNameLength)
    {
        if (bsl::string(attributeName, attributeNameLength)
                 == TestSequenceWithVector::ATTRIBUTE_INFO_ARRAY[0].d_name_p) {
            return manipulator(
                              &object->d_vector,
                              TestSequenceWithVector::ATTRIBUTE_INFO_ARRAY[0]);
        }

        return -1;
    }

    template <typename MANIPULATOR>
    int manipulateAttribute(TestSequenceWithVector *object,
                            MANIPULATOR&            manipulator,
                            int                     attributeId)
    {
        if (attributeId
                     == TestSequenceWithVector::ATTRIBUTE_INFO_ARRAY[0].d_id) {
            return manipulator(
                              &object->d_vector,
                              TestSequenceWithVector::ATTRIBUTE_INFO_ARRAY[0]);
        }

        return -1;
    }

    template <typename MANIPULATOR>
    int manipulateAttributes(TestSequenceWithVector *object,
                             MANIPULATOR&            manipulator)
    {
        if (0 == manipulator(
                            &object->d_vector,
                            TestSequenceWithVector::ATTRIBUTE_INFO_ARRAY[0])) {
            return 0;
        }

        return -1;
    }

    // ACCESSORS
    template <typename ACCESSOR>
    int accessAttribute(const TestSequenceWithVector&  object,
                        ACCESSOR&                      accessor,
                        const char                    *attributeName,
                        int                            attributeNameLength)
    {
        if (bsl::string(attributeName, attributeNameLength)
                 == TestSequenceWithVector::ATTRIBUTE_INFO_ARRAY[0].d_name_p) {
            return accessor(object.d_vector,
                            TestSequenceWithVector::ATTRIBUTE_INFO_ARRAY[0]);
        }

        return -1;
    }

    template <typename ACCESSOR>
    int accessAttribute(const TestSequenceWithVector& object,
                        ACCESSOR&                     accessor,
                        int                           attributeId)
    {
        if (attributeId
                     == TestSequenceWithVector::ATTRIBUTE_INFO_ARRAY[0].d_id) {
            return accessor(object.d_vector,
                            TestSequenceWithVector::ATTRIBUTE_INFO_ARRAY[0]);
        }

        return -1;
    }

    template <typename ACCESSOR>
    int accessAttributes(const TestSequenceWithVector& object,
                         ACCESSOR&                     accessor)
    {
        if (0 == accessor(object.d_vector,
                          TestSequenceWithVector::ATTRIBUTE_INFO_ARRAY[0])) {
            return 0;
        }

        return -1;
    }

}  // close namespace bdeat_SequenceFunctions
}  // close namespace BloombergLP

#include <baexml_decoder.h>

namespace baexml_Decoder_TestNamespace {

                             // =================
                             // class TestContext
                             // =================

class TestContext : public baexml_Decoder_ElementContext {
    // This class implements the 'Decoder_ElementContext' protocol and is
    // used to test the correct usage of this protocol by the parser.  When a
    // method from the protocol is called, the method call is recorded by
    // appending the method's name to the 'd_callSequence' member, along with
    // the arguments passed (if any).  This test context also checks that the
    // error stream and warning stream returned by the
    // 'Decoder_ErrorReporter' object is the same error stream and warning
    // stream that was passed to the 'Decoder_ParserUtil::parse' function.

    // PRIVATE DATA MEMBERS
    bsl::ostream&    d_callSequence;     // log of the call sequence
    bsl::string      d_elementName;      // element name
    bool             d_isInsideElement;  // true if we are inside
                                         // 'startElement' and 'endElement'
                                         // pair
    bslma_Allocator *d_allocator_p;      // allocator for supplying memory

  private:
    // NOT IMPLEMENTED
    TestContext(const TestContext&);
    TestContext& operator=(const TestContext&);

  public:
    // CREATORS
    TestContext(bsl::ostream&       callSequence,
                const char         *elementName,
                bslma_Allocator    *basicAllocator = 0);
        // TBD: doc

    virtual ~TestContext();
        // TBD: doc

    // CALLBACKS
    virtual int startElement(baexml_Decoder *decoder);
        // Behavior is undefined if a previous successful call to
        // 'startElement' was not ended with a successful call to 'endElement'.

    virtual int endElement(baexml_Decoder *decoder);
        // Behavior is undefined unless the most recent call to 'startElement'
        // was successful and it was not already ended with a successful call
        // to 'endElement'.

    virtual int addCharacters(const char * chars,
                              unsigned int length,
                              baexml_Decoder *decoder);
        // TBD: doc
        // Behavior is undefined unless the most recent call to 'startElement'
        // was successful and it was not already ended with a successful call
        // to 'endElement'.

    virtual int parseAttribute(const char * name,
                               const char * value,
                               size_t       lenValue,
                               baexml_Decoder *decoder);
        // TBD: doc

    virtual int parseSubElement(const char                 *elementName,
                                baexml_Decoder *decoder);
        // TBD: doc
        // Behavior is undefined unless the most recent call to 'startElement'
        // was successful and it was not already ended with a successful call
        // to 'endElement'.
};

                             // -----------------
                             // class TestContext
                             // -----------------

// CREATORS

TestContext::TestContext(bsl::ostream&       callSequence,
                         const char         *elementName,
                         bslma_Allocator    *basicAllocator)
: d_callSequence(callSequence)
, d_elementName(elementName, basicAllocator)
, d_isInsideElement(false)
, d_allocator_p(bslma_Default::allocator(basicAllocator))
{
}

TestContext::~TestContext()
{
    ASSERT(!d_isInsideElement);
}

// CALLBACKS

int TestContext::startElement(baexml_Decoder *decoder)
{
    ASSERT(!d_isInsideElement);

    d_callSequence << d_elementName << ":startElement(...)";

    d_isInsideElement = true;

    if ("failOnStart" == d_elementName) {
        return -1;  // trigger failure in parser
    }

    return 0;
}

int TestContext::endElement(baexml_Decoder *decoder)
{
    ASSERT(d_isInsideElement);

    d_callSequence << d_elementName << ":endElement(...)";

    d_isInsideElement = false;

    if ("failOnEnd" == d_elementName) {
        return -1;  // trigger failure in parser
    }

    return 0;
}

int TestContext::addCharacters(const char                 *chars,
                               unsigned int                length,
                               baexml_Decoder *decoder)
{
    ASSERT(d_isInsideElement);

    bsl::string strChars(chars, length);

    d_callSequence << d_elementName << ":addCharacters("
        << strChars << ", ...)";

    if ("failHere" == strChars) {
        return -1;  // trigger failure in parser
    }

    return 0;
}

int TestContext::parseAttribute(const char * name,
                                const char * value,
                                size_t       lenValue,
                                baexml_Decoder *decoder)
{
    ASSERT(d_isInsideElement);

    bsl::string strVal (value, lenValue);

    d_callSequence << d_elementName << ":parseAttribute("
                                    << name << ", " << strVal
                                    << ", ...)";

    return 0;
}

int TestContext::parseSubElement(const char            *elementName,
                                 baexml_Decoder        *decoder)
{
    ASSERT(d_isInsideElement);

    d_callSequence << d_elementName << ":parseSubElement("
                                    << elementName
                                    << ", ...)";

    if (bsl::strcmp("failHere" , elementName) == 0)
    {
        return -1;  // trigger failure in parser
    }

    TestContext subContext(d_callSequence, elementName, d_allocator_p);

    return subContext.beginParse(decoder);
}

                      // ===============================
                      // class TestVectorElemTypeContext
                      // ===============================

class TestVectorElemTypeContext : public baexml_Decoder_ElementContext {
    // This class implements the 'Decoder_ElementContext' protocol and is
    // used as the context for the 'TestVectorElemType' class.  Each call back
    // is recorded in the element object using the object's 'addCall' method.

    // PRIVATE DATA MEMBERS
    int                 d_currentDepth;  // used to keep track of depth so that
                                         // we can trigger an error inside
                                         // 'startElement' and 'endElement'
    TestVectorElemType *d_object_p;

  private:
    // NOT IMPLEMENTED
    TestVectorElemTypeContext(const TestVectorElemTypeContext&);
    TestVectorElemTypeContext& operator=(const TestVectorElemTypeContext&);

  public:
    // CLASS MEMBERS
    static bsl::ostream *s_loggingStream;  // stream used to verify logger
                                           // was passed correctly

    // CREATORS
    TestVectorElemTypeContext(TestVectorElemType *object,
                              bslma_Allocator    *basicAllocator = 0);
        // TBD: doc

    virtual ~TestVectorElemTypeContext();
        // TBD: doc

    // MANIPULATORS
    void reassociate(TestVectorElemType *object);
        // Reassociate this context with the specified 'object'.

    // CALLBACKS
    virtual int startElement(baexml_Decoder *decoder);
        // Behavior is undefined if a previous successful call to
        // 'startElement' was not ended with a successful call to 'endElement'.

    virtual int endElement(baexml_Decoder *decoder);
        // Behavior is undefined unless the most recent call to 'startElement'
        // was successful and it was not already ended with a successful call
        // to 'endElement'.

    virtual int addCharacters(const char                 *chars,
                              unsigned int                length,
                              baexml_Decoder *decoder);

    virtual int parseAttribute(const char * name,
                               const char * value,
                               size_t       lenValue,
                               baexml_Decoder *decoder);

    virtual int parseSubElement(const char                 *elementName,
                                baexml_Decoder *decoder);
        // TBD: doc
        // Behavior is undefined unless the most recent call to 'startElement'
        // was successful and it was not already ended with a successful call
        // to 'endElement'.

    baexml_Decoder_ElementContext* createSubContext(
                                  const char                 *elementName,
                                  baexml_Decoder *decoder);
        // TBD: doc
        // Behavior is undefined unless the most recent call to 'startElement'
        // was successful and it was not already ended with a successful call
        // to 'endElement'.
};

                      // -------------------------------
                      // class TestVectorElemTypeContext
                      // -------------------------------

// CLASS MEMBERS

bsl::ostream *TestVectorElemTypeContext::s_loggingStream = 0;

// CREATORS

TestVectorElemTypeContext::TestVectorElemTypeContext(
                                            TestVectorElemType *object,
                                            bslma_Allocator    *basicAllocator)
: d_currentDepth(0)
, d_object_p(object)
{
}

TestVectorElemTypeContext::~TestVectorElemTypeContext()
{
}

// MANIPULATORS

void TestVectorElemTypeContext::reassociate(TestVectorElemType *object)
{
    d_object_p = object;
}

// CALLBACKS

int TestVectorElemTypeContext::startElement(
                                          baexml_Decoder *decoder)
{
    d_object_p->addCall("startElement(...)");

    if (3 == d_currentDepth) {
        return -1;  // trigger failure in parser
    }

    ++d_currentDepth;

    return 0;
}

int TestVectorElemTypeContext::endElement(baexml_Decoder *decoder)
{
    d_object_p->addCall("endElement(...)");

    --d_currentDepth;

    if (2 == d_currentDepth) {
        return -1;  // trigger failure in parser
    }

    return 0;
}

int TestVectorElemTypeContext::addCharacters(const char * chars,
                                             unsigned int length,
                                             baexml_Decoder *decoder)
{
    bsl::string strChars(chars, length);

    d_object_p->addCall("addCharacters(" + strChars + ", ...)");

    if ("failHere" == strChars) {
        return -1;  // trigger failure in parser
    }

    return 0;
}

int
TestVectorElemTypeContext::parseAttribute(const char * name,
                                const char * value,
                                size_t       lenValue,
                                baexml_Decoder *decoder)
{
    bsl::string strName (name);
    bsl::string strValue (value, lenValue);

    d_object_p->addCall("parseAttribute(" +
                        strName +
                        "," +
                        strValue +
                        ")");

    return 0;
}

int
TestVectorElemTypeContext::parseSubElement(const char       *elementName,
                                 baexml_Decoder *decoder)
{
    bsl::string strElementName (elementName);

    d_object_p->addCall("purseSubElement(" + strElementName + ", ...)");

    if ("failHere" == strElementName) {
        return -1;  // trigger failure in parser
    }

    return 0;
}

baexml_Decoder_ElementContext*
TestVectorElemTypeContext::createSubContext(const char      *elementName,
                                 baexml_Decoder *decoder)
{
    bsl::string strElementName (elementName);

    d_object_p->addCall("createSubContext(" + strElementName + ", ...)");

    if ("failHere" == strElementName) {
        return 0;  // trigger failure in parser
    }

    return this;
}

}  // close namespace baexml_Decoder_TestNamespace

using baexml_Decoder_TestNamespace::TestContext;
using baexml_Decoder_TestNamespace::TestVectorElemTypeContext;

      // ===============================================================
      // Associate 'TestVectorElemType' With 'TestVectorElemTypeContext'
      // ===============================================================

namespace BloombergLP {

    template <>
    struct baexml_Decoder_SelectContext<TestVectorElemType> {
        typedef TestVectorElemTypeContext Type;
    };

}  // close namespace BloombergLP

                              // ================
                              // DummyEnumeration
                              // ================

struct DummyEnumeration {
    enum Value {
        VALUE1, VALUE2, VALUE3
    };
};

namespace BloombergLP {

namespace bdeat_EnumFunctions {

    template <>
    struct IsEnumeration<DummyEnumeration::Value> {
        enum { VALUE = 1 };
    };

}  // close namespace bdeat_EnumFunctions
}  // close namespace BloombergLP

                            // ===================
                            // DummyCustomizedType
                            // ===================

class DummyCustomizedType {
    int  d_i;
public:
    typedef int BaseType;

    DummyCustomizedType () :  d_i(0) {}

    int fromInt (const int& i) { d_i = i;  return 0; }
    const int& toInt() { return d_i; }
};

namespace BloombergLP {

namespace bdeat_CustomizedTypeFunctions {

    template <>
    struct IsCustomizedType<DummyCustomizedType> {
        enum { VALUE = 1 };
    };

    template  <>
    struct BaseType<DummyCustomizedType> {
        typedef int Type;
    };

}  // close namespace bdeat_CustomizedTypeFunctions
}  // close namespace BloombergLP

// To test the facilities in this component, we generate a number of types
// using the command 'bas_codegen.pl test.xsd -m msg -p test'.  The input
// schema, in 'test.xsd' is as follows:
//..
//  <?xml version='1.0' encoding='UTF-8'?>
//  <xs:schema xmlns:xs='http://www.w3.org/2001/XMLSchema'
//             xmlns:bdem='http://bloomberg.com/schemas/bdem'
//             xmlns='http://bloomberg.com/schemas/test'
//             targetNamespace='http://bloomberg.com/schemas/test'
//             elementFormDefault='qualified'>
//
//      <xs:complexType name='MySequence'>
//          <xs:sequence>
//              <xs:element name='Attribute1' type='xs:int'/>
//              <xs:element name='Attribute2' type='xs:string'/>
//          </xs:sequence>
//      </xs:complexType>
//
//      <xs:complexType name='MySequenceWithNullables'>
//          <xs:sequence>
//              <xs:element name='Attribute1' type='xs:int' minOccurs='0'/>
//              <xs:element name='Attribute2' type='xs:string' minOccurs='0'/>
//              <xs:element name='Attribute3' type='MySequence' minOccurs='0'/>
//          </xs:sequence>
//      </xs:complexType>
//
//      <xs:complexType name='MySequenceWithAnonymousChoice'>
//          <xs:sequence>
//              <xs:element name='Attribute1' type='xs:int' minOccurs='0'/>
//              <xs:choice>
//                  <xs:element name='MyChoice1' type='xs:int'/>
//                  <xs:element name='MyChoice2' type='xs:string'/>
//              </xs:choice>
//              <xs:element name='Attribute2' type='xs:string' minOccurs='0'/>
//          </xs:sequence>
//      </xs:complexType>
//
//      <xs:complexType name='MySequenceWithAttributes'>
//          <xs:attribute name='Attribute1' type='xs:int' use='required'/>
//          <xs:attribute name='Attribute2' type='xs:string'/>
//          <xs:sequence>
//              <xs:element name='Element1' type='xs:int' minOccurs='0'/>
//              <xs:element name='Element2' type='xs:string' minOccurs='0'/>
//          </xs:sequence>
//      </xs:complexType>
//
//      <xs:complexType name='MySimpleContent'>
//          <xs:simpleContent>
//              <xs:extension base='xs:string'>
//                  <xs:attribute name='Attribute1' type='xs:boolean'/>
//                  <xs:attribute name='Attribute2' type='xs:string'/>
//              </xs:extension>
//          </xs:simpleContent>
//      </xs:complexType>
//
//      <xs:complexType name='MySimpleIntContent'>
//          <xs:simpleContent>
//              <xs:extension base='xs:int'>
//                  <xs:attribute name='Attribute1' type='xs:boolean'/>
//                  <xs:attribute name='Attribute2' type='xs:string'/>
//              </xs:extension>
//          </xs:simpleContent>
//      </xs:complexType>
//
//      <xs:complexType name='MySequenceWithNillables'>
//          <xs:sequence>
//              <xs:element name='Attribute1' type='xs:int' nillable='true'/>
//              <xs:element name='Attribute2' type='xs:string'
//                   nillable='true'/>
//              <xs:element name='Attribute3' type='MySequence'
//                   nillable='true'/>
//          </xs:sequence>
//      </xs:complexType>
//
//      <xs:complexType name='Address'>
//          <xs:sequence>
//              <xs:element name='street' type='xs:string'/>
//              <xs:element name='city' type='xs:string'/>
//              <xs:element name='state' type='xs:string'/>
//          </xs:sequence>
//      </xs:complexType>
//
//      <xs:complexType name='Employee'>
//          <xs:sequence>
//              <xs:element name='name' type='xs:string'/>
//              <xs:element name='homeAddress' type='Address'/>
//              <xs:element name='age' type='xs:int'/>
//          </xs:sequence>
//      </xs:complexType>
//
//      <xs:element name='MySequence' type='MySequence'/>
//      <xs:element name='MySequenceWithNullables'
//                  type='MySequenceWithNullables'/>
//      <xs:element name='MySequenceWithAnonymousChoice'
//                  type='MySequenceWithAnonymousChoice'/>
//      <xs:element name='MySequenceWithAttributes'
//                  type='MySequenceWithAttributes'/>
//      <xs:element name='MySimpleContent' type='MySimpleContent'/>
//      <xs:element name='MySimpleIntContent' type='MySimpleIntContent'/>
//      <xs:element name='MySequenceWithNillables'
//                  type='MySequenceWithNillables'/>
//      <xs:element name='Address' type='Address'/>
//      <xs:element name='Employee' type='Employee'/>
//
//  </xs:schema>
//..

// ***** START OF GENERATED CODE ****

// test_messages.h   -*-C++-*-
#ifndef INCLUDED_TEST_MESSAGES
#define INCLUDED_TEST_MESSAGES

//@PURPOSE: TBD: Provide purpose
//
//@CLASSES:
// test::MySequenceWithAttributes: TBD: Provide purpose
// test::Address: TBD: Provide purpose
// test::MySequence: TBD: Provide purpose
// test::MySimpleContent: TBD: Provide purpose
// test::MySimpleIntContent: TBD: Provide purpose
// test::MySequenceWithAnonymousChoiceChoice: TBD: Provide purpose
// test::MySequenceWithNullables: TBD: Provide purpose
// test::Employee: TBD: Provide purpose
// test::MySequenceWithAnonymousChoice: TBD: Provide purpose
// test::MySequenceWithNillables: TBD: Provide purpose
//
//@AUTHOR: Author Unknown (Unix login: phalpern)
//
//@DESCRIPTION:

#ifndef INCLUDED_BSLMA_DEFAULT
#include <bslma_default.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITS
#include <bslalg_typetraits.h>
#endif

#ifndef INCLUDED_BDEAT_ATTRIBUTEINFO
#include <bdeat_attributeinfo.h>
#endif

#ifndef INCLUDED_BDEAT_TYPETRAITS
#include <bdeat_typetraits.h>
#endif

#ifndef INCLUDED_BDEAT_VALUETYPEFUNCTIONS
#include <bdeat_valuetypefunctions.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_BDEX_INSTREAMFUNCTIONS
#include <bdex_instreamfunctions.h>
#endif

#ifndef INCLUDED_BDEX_OUTSTREAMFUNCTIONS
#include <bdex_outstreamfunctions.h>
#endif

#ifndef INCLUDED_BDEU_PRINTMETHODS
#include <bdeu_printmethods.h>
#endif

#ifndef INCLUDED_BDEUT_NULLABLEVALUE
#include <bdeut_nullablevalue.h>
#endif

#ifndef INCLUDED_BSL_STRING
#include <bsl_string.h>
#endif

#ifndef INCLUDED_BSL_IOSFWD
#include <bsl_iosfwd.h>
#endif

namespace BloombergLP {

class bslma_Allocator;

namespace test { class MySequenceWithAttributes; }
namespace test { class Address; }
namespace test { class MySequence; }
namespace test { class MySimpleContent; }
namespace test { class MySimpleIntContent; }
namespace test { class MySequenceWithAnonymousChoiceChoice; }
namespace test { class MySequenceWithNullables; }
namespace test { class Employee; }
namespace test { class MySequenceWithAnonymousChoice; }
namespace test { class MySequenceWithNillables; }
namespace test {

                       // ==============================
                       // class MySequenceWithAttributes
                       // ==============================

class MySequenceWithAttributes {
    // MySequenceWithAttributes: TBD: Provide annotation

  private:
    int d_attribute1;
        // Attribute1: TBD: Provide annotation
    bdeut_NullableValue<bsl::string> d_attribute2;
        // Attribute2: TBD: Provide annotation
    bdeut_NullableValue<int> d_element1;
        // Element1: TBD: Provide annotation
    bdeut_NullableValue<bsl::string> d_element2;
        // Element2: TBD: Provide annotation

  public:
    // TYPES
    enum {
        NUM_ATTRIBUTES = 4 // the number of attributes in this class
    };

    enum {
        ATTRIBUTE_INDEX_ATTRIBUTE1 = 0,
            // index for "Attribute1" attribute
        ATTRIBUTE_INDEX_ATTRIBUTE2 = 1,
            // index for "Attribute2" attribute
        ATTRIBUTE_INDEX_ELEMENT1 = 2,
            // index for "Element1" attribute
        ATTRIBUTE_INDEX_ELEMENT2 = 3
            // index for "Element2" attribute
    };

    enum {
        ATTRIBUTE_ID_ATTRIBUTE1 = 0,
            // id for "Attribute1" attribute
        ATTRIBUTE_ID_ATTRIBUTE2 = 1,
            // id for "Attribute2" attribute
        ATTRIBUTE_ID_ELEMENT1 = 2,
            // id for "Element1" attribute
        ATTRIBUTE_ID_ELEMENT2 = 3
            // id for "Element2" attribute
    };

  public:
    // CONSTANTS
    static const char CLASS_NAME[];
        // the name of this class (i.e., "MySequenceWithAttributes")

    static const bdeat_AttributeInfo ATTRIBUTE_INFO_ARRAY[];
        // attribute information for each attribute

  public:
    // CLASS METHODS
    static int maxSupportedBdexVersion();
        // Return the most current 'bdex' streaming version number supported by
        // this class.  See the 'bdex' package-level documentation for more
        // information on 'bdex' streaming of value-semantic types and
        // containers.

    static const bdeat_AttributeInfo *lookupAttributeInfo(int id);
        // Return attribute information for the attribute indicated by the
        // specified 'id' if the attribute exists, and 0 otherwise.

    static const bdeat_AttributeInfo *lookupAttributeInfo(
                                                    const char *name,
                                                    int         nameLength);
        // Return attribute information for the attribute indicated by the
        // specified 'name' of the specified 'nameLength' if the attribute
        // exists, and 0 otherwise.

    // CREATORS
    explicit MySequenceWithAttributes(bslma_Allocator *basicAllocator = 0);
        // Create an object of type 'MySequenceWithAttributes' having the
        // default value.  Use the optionally specified 'basicAllocator' to
        // supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.

    MySequenceWithAttributes(const MySequenceWithAttributes& original,
                             bslma_Allocator *basicAllocator = 0);
        // Create an object of type 'MySequenceWithAttributes' having the value
        // of the specified 'original' object.  Use the optionally specified
        // 'basicAllocator' to supply memory.  If 'basicAllocator' is 0, the
        // currently installed default allocator is used.

    ~MySequenceWithAttributes();
        // Destroy this object.

    // MANIPULATORS
    MySequenceWithAttributes& operator=(const MySequenceWithAttributes& rhs);
        // Assign to this object the value of the specified 'rhs' object.

    template <class STREAM>
    STREAM& bdexStreamIn(STREAM& stream, int version);
        // Assign to this object the value read from the specified input
        // 'stream' using the specified 'version' format and return a reference
        // to the modifiable 'stream'.  If 'stream' is initially invalid, this
        // operation has no effect.  If 'stream' becomes invalid during this
        // operation, this object is valid, but its value is undefined.  If
        // 'version' is not supported, 'stream' is marked invalid and this
        // object is unaltered.  Note that no version is read from 'stream'.
        // See the 'bdex' package-level documentation for more information on
        // 'bdex' streaming of value-semantic types and containers.

    void reset();
        // Reset this object to the default value (i.e., its value upon
        // default construction).

    template<class MANIPULATOR>
    int manipulateAttributes(MANIPULATOR& manipulator);
        // Invoke the specified 'manipulator' sequentially on the address of
        // each (modifiable) attribute of this object, supplying 'manipulator'
        // with the corresponding attribute information structure until such
        // invocation returns a non-zero value.  Return the value from the
        // last invocation of 'manipulator' (i.e., the invocation that
        // terminated the sequence).

    template<class MANIPULATOR>
    int manipulateAttribute(MANIPULATOR& manipulator, int id);
        // Invoke the specified 'manipulator' on the address of
        // the (modifiable) attribute indicated by the specified 'id',
        // supplying 'manipulator' with the corresponding attribute
        // information structure.  Return the value returned from the
        // invocation of 'manipulator' if 'id' identifies an attribute of this
        // class, and -1 otherwise.

    template<class MANIPULATOR>
    int manipulateAttribute(MANIPULATOR&  manipulator,
                            const char   *name,
                            int           nameLength);
        // Invoke the specified 'manipulator' on the address of
        // the (modifiable) attribute indicated by the specified 'name' of the
        // specified 'nameLength', supplying 'manipulator' with the
        // corresponding attribute information structure.  Return the value
        // returned from the invocation of 'manipulator' if 'name' identifies
        // an attribute of this class, and -1 otherwise.

    int& attribute1();
        // Return a reference to the modifiable "Attribute1" attribute of this
        // object.

    bdeut_NullableValue<bsl::string>& attribute2();
        // Return a reference to the modifiable "Attribute2" attribute of this
        // object.

    bdeut_NullableValue<int>& element1();
        // Return a reference to the modifiable "Element1" attribute of this
        // object.

    bdeut_NullableValue<bsl::string>& element2();
        // Return a reference to the modifiable "Element2" attribute of this
        // object.

    // ACCESSORS
    bsl::ostream& print(bsl::ostream& stream,
                        int           level = 0,
                        int           spacesPerLevel = 4) const;
        // Format this object to the specified output 'stream' at the
        // optionally specified indentation 'level' and return a reference to
        // the modifiable 'stream'.  If 'level' is specified, optionally
        // specify 'spacesPerLevel', the number of spaces per indentation level
        // for this and all of its nested objects.  Each line is indented by
        // the absolute value of 'level * spacesPerLevel'.  If 'level' is
        // negative, suppress indentation of the first line.  If
        // 'spacesPerLevel' is negative, suppress line breaks and format the
        // entire output on one line.  If 'stream' is initially invalid, this
        // operation has no effect.  Note that a trailing newline is provided
        // in multiline mode only.

    template <class STREAM>
    STREAM& bdexStreamOut(STREAM& stream, int version) const;
        // Write the value of this object to the specified output 'stream'
        // using the specified 'version' format and return a reference to the
        // modifiable 'stream'.  If 'version' is not supported, 'stream' is
        // unmodified.  Note that 'version' is not written to 'stream'.
        // See the 'bdex' package-level documentation for more information
        // on 'bdex' streaming of value-semantic types and containers.

    template<class ACCESSOR>
    int accessAttributes(ACCESSOR& accessor) const;
        // Invoke the specified 'accessor' sequentially on each
        // (non-modifiable) attribute of this object, supplying 'accessor'
        // with the corresponding attribute information structure until such
        // invocation returns a non-zero value.  Return the value from the
        // last invocation of 'accessor' (i.e., the invocation that terminated
        // the sequence).

    template<class ACCESSOR>
    int accessAttribute(ACCESSOR& accessor, int id) const;
        // Invoke the specified 'accessor' on the (non-modifiable) attribute
        // of this object indicated by the specified 'id', supplying 'accessor'
        // with the corresponding attribute information structure.  Return the
        // value returned from the invocation of 'accessor' if 'id' identifies
        // an attribute of this class, and -1 otherwise.

    template<class ACCESSOR>
    int accessAttribute(ACCESSOR&   accessor,
                        const char *name,
                        int         nameLength) const;
        // Invoke the specified 'accessor' on the (non-modifiable) attribute
        // of this object indicated by the specified 'name' of the specified
        // 'nameLength', supplying 'accessor' with the corresponding attribute
        // information structure.  Return the value returned from the
        // invocation of 'accessor' if 'name' identifies an attribute of this
        // class, and -1 otherwise.

    const int& attribute1() const;
        // Return a reference to the non-modifiable "Attribute1" attribute of
        // this object.

    const bdeut_NullableValue<bsl::string>& attribute2() const;
        // Return a reference to the non-modifiable "Attribute2" attribute of
        // this object.

    const bdeut_NullableValue<int>& element1() const;
        // Return a reference to the non-modifiable "Element1" attribute of
        // this object.

    const bdeut_NullableValue<bsl::string>& element2() const;
        // Return a reference to the non-modifiable "Element2" attribute of
        // this object.
};

// FREE OPERATORS
inline
bool operator==(const MySequenceWithAttributes& lhs,
                const MySequenceWithAttributes& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects have
    // the same value, and 'false' otherwise.  Two attribute objects have the
    // same value if each respective attribute has the same value.

inline
bool operator!=(const MySequenceWithAttributes& lhs,
                const MySequenceWithAttributes& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects do not
    // have the same value, and 'false' otherwise.  Two attribute objects do
    // not have the same value if one or more respective attributes differ in
    // values.

inline
bsl::ostream& operator<<(bsl::ostream&                    stream,
                         const MySequenceWithAttributes&  rhs);
    // Format the specified 'rhs' to the specified output 'stream' and
    // return a reference to the modifiable 'stream'.

}  // close namespace test

// TRAITS

BDEAT_DECL_SEQUENCE_WITH_ALLOCATOR_TRAITS(test::MySequenceWithAttributes)

namespace test {

                               // =============
                               // class Address
                               // =============

class Address {
    // Address: TBD: Provide annotation

  private:
    bsl::string d_street;
        // street: TBD: Provide annotation
    bsl::string d_city;
        // city: TBD: Provide annotation
    bsl::string d_state;
        // state: TBD: Provide annotation

  public:
    // TYPES
    enum {
        NUM_ATTRIBUTES = 3 // the number of attributes in this class
    };

    enum {
        ATTRIBUTE_INDEX_STREET = 0,
            // index for "Street" attribute
        ATTRIBUTE_INDEX_CITY = 1,
            // index for "City" attribute
        ATTRIBUTE_INDEX_STATE = 2
            // index for "State" attribute
    };

    enum {
        ATTRIBUTE_ID_STREET = 0,
            // id for "Street" attribute
        ATTRIBUTE_ID_CITY = 1,
            // id for "City" attribute
        ATTRIBUTE_ID_STATE = 2
            // id for "State" attribute
    };

  public:
    // CONSTANTS
    static const char CLASS_NAME[];
        // the name of this class (i.e., "Address")

    static const bdeat_AttributeInfo ATTRIBUTE_INFO_ARRAY[];
        // attribute information for each attribute

  public:
    // CLASS METHODS
    static int maxSupportedBdexVersion();
        // Return the most current 'bdex' streaming version number supported by
        // this class.  See the 'bdex' package-level documentation for more
        // information on 'bdex' streaming of value-semantic types and
        // containers.

    static const bdeat_AttributeInfo *lookupAttributeInfo(int id);
        // Return attribute information for the attribute indicated by the
        // specified 'id' if the attribute exists, and 0 otherwise.

    static const bdeat_AttributeInfo *lookupAttributeInfo(
                                                    const char *name,
                                                    int         nameLength);
        // Return attribute information for the attribute indicated by the
        // specified 'name' of the specified 'nameLength' if the attribute
        // exists, and 0 otherwise.

    // CREATORS
    explicit Address(bslma_Allocator *basicAllocator = 0);
        // Create an object of type 'Address' having the default value.  Use
        // the optionally specified 'basicAllocator' to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.

    Address(const Address& original,
            bslma_Allocator *basicAllocator = 0);
        // Create an object of type 'Address' having the value of the specified
        // 'original' object.  Use the optionally specified 'basicAllocator' to
        // supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.

    ~Address();
        // Destroy this object.

    // MANIPULATORS
    Address& operator=(const Address& rhs);
        // Assign to this object the value of the specified 'rhs' object.

    template <class STREAM>
    STREAM& bdexStreamIn(STREAM& stream, int version);
        // Assign to this object the value read from the specified input
        // 'stream' using the specified 'version' format and return a reference
        // to the modifiable 'stream'.  If 'stream' is initially invalid, this
        // operation has no effect.  If 'stream' becomes invalid during this
        // operation, this object is valid, but its value is undefined.  If
        // 'version' is not supported, 'stream' is marked invalid and this
        // object is unaltered.  Note that no version is read from 'stream'.
        // See the 'bdex' package-level documentation for more information on
        // 'bdex' streaming of value-semantic types and containers.

    void reset();
        // Reset this object to the default value (i.e., its value upon
        // default construction).

    template<class MANIPULATOR>
    int manipulateAttributes(MANIPULATOR& manipulator);
        // Invoke the specified 'manipulator' sequentially on the address of
        // each (modifiable) attribute of this object, supplying 'manipulator'
        // with the corresponding attribute information structure until such
        // invocation returns a non-zero value.  Return the value from the
        // last invocation of 'manipulator' (i.e., the invocation that
        // terminated the sequence).

    template<class MANIPULATOR>
    int manipulateAttribute(MANIPULATOR& manipulator, int id);
        // Invoke the specified 'manipulator' on the address of
        // the (modifiable) attribute indicated by the specified 'id',
        // supplying 'manipulator' with the corresponding attribute
        // information structure.  Return the value returned from the
        // invocation of 'manipulator' if 'id' identifies an attribute of this
        // class, and -1 otherwise.

    template<class MANIPULATOR>
    int manipulateAttribute(MANIPULATOR&  manipulator,
                            const char   *name,
                            int           nameLength);
        // Invoke the specified 'manipulator' on the address of
        // the (modifiable) attribute indicated by the specified 'name' of the
        // specified 'nameLength', supplying 'manipulator' with the
        // corresponding attribute information structure.  Return the value
        // returned from the invocation of 'manipulator' if 'name' identifies
        // an attribute of this class, and -1 otherwise.

    bsl::string& street();
        // Return a reference to the modifiable "Street" attribute of this
        // object.

    bsl::string& city();
        // Return a reference to the modifiable "City" attribute of this
        // object.

    bsl::string& state();
        // Return a reference to the modifiable "State" attribute of this
        // object.

    // ACCESSORS
    bsl::ostream& print(bsl::ostream& stream,
                        int           level = 0,
                        int           spacesPerLevel = 4) const;
        // Format this object to the specified output 'stream' at the
        // optionally specified indentation 'level' and return a reference to
        // the modifiable 'stream'.  If 'level' is specified, optionally
        // specify 'spacesPerLevel', the number of spaces per indentation level
        // for this and all of its nested objects.  Each line is indented by
        // the absolute value of 'level * spacesPerLevel'.  If 'level' is
        // negative, suppress indentation of the first line.  If
        // 'spacesPerLevel' is negative, suppress line breaks and format the
        // entire output on one line.  If 'stream' is initially invalid, this
        // operation has no effect.  Note that a trailing newline is provided
        // in multiline mode only.

    template <class STREAM>
    STREAM& bdexStreamOut(STREAM& stream, int version) const;
        // Write the value of this object to the specified output 'stream'
        // using the specified 'version' format and return a reference to the
        // modifiable 'stream'.  If 'version' is not supported, 'stream' is
        // unmodified.  Note that 'version' is not written to 'stream'.
        // See the 'bdex' package-level documentation for more information
        // on 'bdex' streaming of value-semantic types and containers.

    template<class ACCESSOR>
    int accessAttributes(ACCESSOR& accessor) const;
        // Invoke the specified 'accessor' sequentially on each
        // (non-modifiable) attribute of this object, supplying 'accessor'
        // with the corresponding attribute information structure until such
        // invocation returns a non-zero value.  Return the value from the
        // last invocation of 'accessor' (i.e., the invocation that terminated
        // the sequence).

    template<class ACCESSOR>
    int accessAttribute(ACCESSOR& accessor, int id) const;
        // Invoke the specified 'accessor' on the (non-modifiable) attribute
        // of this object indicated by the specified 'id', supplying 'accessor'
        // with the corresponding attribute information structure.  Return the
        // value returned from the invocation of 'accessor' if 'id' identifies
        // an attribute of this class, and -1 otherwise.

    template<class ACCESSOR>
    int accessAttribute(ACCESSOR&   accessor,
                        const char *name,
                        int         nameLength) const;
        // Invoke the specified 'accessor' on the (non-modifiable) attribute
        // of this object indicated by the specified 'name' of the specified
        // 'nameLength', supplying 'accessor' with the corresponding attribute
        // information structure.  Return the value returned from the
        // invocation of 'accessor' if 'name' identifies an attribute of this
        // class, and -1 otherwise.

    const bsl::string& street() const;
        // Return a reference to the non-modifiable "Street" attribute of this
        // object.

    const bsl::string& city() const;
        // Return a reference to the non-modifiable "City" attribute of this
        // object.

    const bsl::string& state() const;
        // Return a reference to the non-modifiable "State" attribute of this
        // object.
};

// FREE OPERATORS
inline
bool operator==(const Address& lhs, const Address& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects have
    // the same value, and 'false' otherwise.  Two attribute objects have the
    // same value if each respective attribute has the same value.

inline
bool operator!=(const Address& lhs, const Address& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects do not
    // have the same value, and 'false' otherwise.  Two attribute objects do
    // not have the same value if one or more respective attributes differ in
    // values.

inline
bsl::ostream& operator<<(bsl::ostream& stream, const Address& rhs);
    // Format the specified 'rhs' to the specified output 'stream' and
    // return a reference to the modifiable 'stream'.

}  // close namespace test

// TRAITS

BDEAT_DECL_SEQUENCE_WITH_ALLOCATOR_TRAITS(test::Address)

namespace test {

                              // ================
                              // class MySequence
                              // ================

class MySequence {
    // MySequence: TBD: Provide annotation

  private:
    int d_attribute1;
        // Attribute1: TBD: Provide annotation
    bsl::string d_attribute2;
        // Attribute2: TBD: Provide annotation

  public:
    // TYPES
    enum {
        NUM_ATTRIBUTES = 2 // the number of attributes in this class
    };

    enum {
        ATTRIBUTE_INDEX_ATTRIBUTE1 = 0,
            // index for "Attribute1" attribute
        ATTRIBUTE_INDEX_ATTRIBUTE2 = 1
            // index for "Attribute2" attribute
    };

    enum {
        ATTRIBUTE_ID_ATTRIBUTE1 = 0,
            // id for "Attribute1" attribute
        ATTRIBUTE_ID_ATTRIBUTE2 = 1
            // id for "Attribute2" attribute
    };

  public:
    // CONSTANTS
    static const char CLASS_NAME[];
        // the name of this class (i.e., "MySequence")

    static const bdeat_AttributeInfo ATTRIBUTE_INFO_ARRAY[];
        // attribute information for each attribute

  public:
    // CLASS METHODS
    static int maxSupportedBdexVersion();
        // Return the most current 'bdex' streaming version number supported by
        // this class.  See the 'bdex' package-level documentation for more
        // information on 'bdex' streaming of value-semantic types and
        // containers.

    static const bdeat_AttributeInfo *lookupAttributeInfo(int id);
        // Return attribute information for the attribute indicated by the
        // specified 'id' if the attribute exists, and 0 otherwise.

    static const bdeat_AttributeInfo *lookupAttributeInfo(
                                                    const char *name,
                                                    int         nameLength);
        // Return attribute information for the attribute indicated by the
        // specified 'name' of the specified 'nameLength' if the attribute
        // exists, and 0 otherwise.

    // CREATORS
    explicit MySequence(bslma_Allocator *basicAllocator = 0);
        // Create an object of type 'MySequence' having the default value.  Use
        // the optionally specified 'basicAllocator' to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.

    MySequence(const MySequence& original,
               bslma_Allocator *basicAllocator = 0);
        // Create an object of type 'MySequence' having the value of the
        // specified 'original' object.  Use the optionally specified
        // 'basicAllocator' to supply memory.  If 'basicAllocator' is 0, the
        // currently installed default allocator is used.

    ~MySequence();
        // Destroy this object.

    // MANIPULATORS
    MySequence& operator=(const MySequence& rhs);
        // Assign to this object the value of the specified 'rhs' object.

    template <class STREAM>
    STREAM& bdexStreamIn(STREAM& stream, int version);
        // Assign to this object the value read from the specified input
        // 'stream' using the specified 'version' format and return a reference
        // to the modifiable 'stream'.  If 'stream' is initially invalid, this
        // operation has no effect.  If 'stream' becomes invalid during this
        // operation, this object is valid, but its value is undefined.  If
        // 'version' is not supported, 'stream' is marked invalid and this
        // object is unaltered.  Note that no version is read from 'stream'.
        // See the 'bdex' package-level documentation for more information on
        // 'bdex' streaming of value-semantic types and containers.

    void reset();
        // Reset this object to the default value (i.e., its value upon
        // default construction).

    template<class MANIPULATOR>
    int manipulateAttributes(MANIPULATOR& manipulator);
        // Invoke the specified 'manipulator' sequentially on the address of
        // each (modifiable) attribute of this object, supplying 'manipulator'
        // with the corresponding attribute information structure until such
        // invocation returns a non-zero value.  Return the value from the
        // last invocation of 'manipulator' (i.e., the invocation that
        // terminated the sequence).

    template<class MANIPULATOR>
    int manipulateAttribute(MANIPULATOR& manipulator, int id);
        // Invoke the specified 'manipulator' on the address of
        // the (modifiable) attribute indicated by the specified 'id',
        // supplying 'manipulator' with the corresponding attribute
        // information structure.  Return the value returned from the
        // invocation of 'manipulator' if 'id' identifies an attribute of this
        // class, and -1 otherwise.

    template<class MANIPULATOR>
    int manipulateAttribute(MANIPULATOR&  manipulator,
                            const char   *name,
                            int           nameLength);
        // Invoke the specified 'manipulator' on the address of
        // the (modifiable) attribute indicated by the specified 'name' of the
        // specified 'nameLength', supplying 'manipulator' with the
        // corresponding attribute information structure.  Return the value
        // returned from the invocation of 'manipulator' if 'name' identifies
        // an attribute of this class, and -1 otherwise.

    int& attribute1();
        // Return a reference to the modifiable "Attribute1" attribute of this
        // object.

    bsl::string& attribute2();
        // Return a reference to the modifiable "Attribute2" attribute of this
        // object.

    // ACCESSORS
    bsl::ostream& print(bsl::ostream& stream,
                        int           level = 0,
                        int           spacesPerLevel = 4) const;
        // Format this object to the specified output 'stream' at the
        // optionally specified indentation 'level' and return a reference to
        // the modifiable 'stream'.  If 'level' is specified, optionally
        // specify 'spacesPerLevel', the number of spaces per indentation level
        // for this and all of its nested objects.  Each line is indented by
        // the absolute value of 'level * spacesPerLevel'.  If 'level' is
        // negative, suppress indentation of the first line.  If
        // 'spacesPerLevel' is negative, suppress line breaks and format the
        // entire output on one line.  If 'stream' is initially invalid, this
        // operation has no effect.  Note that a trailing newline is provided
        // in multiline mode only.

    template <class STREAM>
    STREAM& bdexStreamOut(STREAM& stream, int version) const;
        // Write the value of this object to the specified output 'stream'
        // using the specified 'version' format and return a reference to the
        // modifiable 'stream'.  If 'version' is not supported, 'stream' is
        // unmodified.  Note that 'version' is not written to 'stream'.
        // See the 'bdex' package-level documentation for more information
        // on 'bdex' streaming of value-semantic types and containers.

    template<class ACCESSOR>
    int accessAttributes(ACCESSOR& accessor) const;
        // Invoke the specified 'accessor' sequentially on each
        // (non-modifiable) attribute of this object, supplying 'accessor'
        // with the corresponding attribute information structure until such
        // invocation returns a non-zero value.  Return the value from the
        // last invocation of 'accessor' (i.e., the invocation that terminated
        // the sequence).

    template<class ACCESSOR>
    int accessAttribute(ACCESSOR& accessor, int id) const;
        // Invoke the specified 'accessor' on the (non-modifiable) attribute
        // of this object indicated by the specified 'id', supplying 'accessor'
        // with the corresponding attribute information structure.  Return the
        // value returned from the invocation of 'accessor' if 'id' identifies
        // an attribute of this class, and -1 otherwise.

    template<class ACCESSOR>
    int accessAttribute(ACCESSOR&   accessor,
                        const char *name,
                        int         nameLength) const;
        // Invoke the specified 'accessor' on the (non-modifiable) attribute
        // of this object indicated by the specified 'name' of the specified
        // 'nameLength', supplying 'accessor' with the corresponding attribute
        // information structure.  Return the value returned from the
        // invocation of 'accessor' if 'name' identifies an attribute of this
        // class, and -1 otherwise.

    const int& attribute1() const;
        // Return a reference to the non-modifiable "Attribute1" attribute of
        // this object.

    const bsl::string& attribute2() const;
        // Return a reference to the non-modifiable "Attribute2" attribute of
        // this object.
};

// FREE OPERATORS
inline
bool operator==(const MySequence& lhs, const MySequence& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects have
    // the same value, and 'false' otherwise.  Two attribute objects have the
    // same value if each respective attribute has the same value.

inline
bool operator!=(const MySequence& lhs, const MySequence& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects do not
    // have the same value, and 'false' otherwise.  Two attribute objects do
    // not have the same value if one or more respective attributes differ in
    // values.

inline
bsl::ostream& operator<<(bsl::ostream& stream, const MySequence& rhs);
    // Format the specified 'rhs' to the specified output 'stream' and
    // return a reference to the modifiable 'stream'.

}  // close namespace test

// TRAITS

BDEAT_DECL_SEQUENCE_WITH_ALLOCATOR_TRAITS(test::MySequence)

namespace test {

                           // =====================
                           // class MySimpleContent
                           // =====================

class MySimpleContent {
    // MySimpleContent: TBD: Provide annotation

  private:
    bdeut_NullableValue<bool> d_attribute1;
        // Attribute1: TBD: Provide annotation
    bdeut_NullableValue<bsl::string> d_attribute2;
        // Attribute2: TBD: Provide annotation
    bsl::string d_theContent;
        // TheContent: TBD: Provide annotation

  public:
    // TYPES
    enum {
        NUM_ATTRIBUTES = 3 // the number of attributes in this class
    };

    enum {
        ATTRIBUTE_INDEX_ATTRIBUTE1 = 0,
            // index for "Attribute1" attribute
        ATTRIBUTE_INDEX_ATTRIBUTE2 = 1,
            // index for "Attribute2" attribute
        ATTRIBUTE_INDEX_THE_CONTENT = 2
            // index for "TheContent" attribute
    };

    enum {
        ATTRIBUTE_ID_ATTRIBUTE1 = 0,
            // id for "Attribute1" attribute
        ATTRIBUTE_ID_ATTRIBUTE2 = 1,
            // id for "Attribute2" attribute
        ATTRIBUTE_ID_THE_CONTENT = 2
            // id for "TheContent" attribute
    };

  public:
    // CONSTANTS
    static const char CLASS_NAME[];
        // the name of this class (i.e., "MySimpleContent")

    static const bdeat_AttributeInfo ATTRIBUTE_INFO_ARRAY[];
        // attribute information for each attribute

  public:
    // CLASS METHODS
    static int maxSupportedBdexVersion();
        // Return the most current 'bdex' streaming version number supported by
        // this class.  See the 'bdex' package-level documentation for more
        // information on 'bdex' streaming of value-semantic types and
        // containers.

    static const bdeat_AttributeInfo *lookupAttributeInfo(int id);
        // Return attribute information for the attribute indicated by the
        // specified 'id' if the attribute exists, and 0 otherwise.

    static const bdeat_AttributeInfo *lookupAttributeInfo(
                                                    const char *name,
                                                    int         nameLength);
        // Return attribute information for the attribute indicated by the
        // specified 'name' of the specified 'nameLength' if the attribute
        // exists, and 0 otherwise.

    // CREATORS
    explicit MySimpleContent(bslma_Allocator *basicAllocator = 0);
        // Create an object of type 'MySimpleContent' having the default value.
        //  Use the optionally specified 'basicAllocator' to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.

    MySimpleContent(const MySimpleContent& original,
                    bslma_Allocator *basicAllocator = 0);
        // Create an object of type 'MySimpleContent' having the value of the
        // specified 'original' object.  Use the optionally specified
        // 'basicAllocator' to supply memory.  If 'basicAllocator' is 0, the
        // currently installed default allocator is used.

    ~MySimpleContent();
        // Destroy this object.

    // MANIPULATORS
    MySimpleContent& operator=(const MySimpleContent& rhs);
        // Assign to this object the value of the specified 'rhs' object.

    template <class STREAM>
    STREAM& bdexStreamIn(STREAM& stream, int version);
        // Assign to this object the value read from the specified input
        // 'stream' using the specified 'version' format and return a reference
        // to the modifiable 'stream'.  If 'stream' is initially invalid, this
        // operation has no effect.  If 'stream' becomes invalid during this
        // operation, this object is valid, but its value is undefined.  If
        // 'version' is not supported, 'stream' is marked invalid and this
        // object is unaltered.  Note that no version is read from 'stream'.
        // See the 'bdex' package-level documentation for more information on
        // 'bdex' streaming of value-semantic types and containers.

    void reset();
        // Reset this object to the default value (i.e., its value upon
        // default construction).

    template<class MANIPULATOR>
    int manipulateAttributes(MANIPULATOR& manipulator);
        // Invoke the specified 'manipulator' sequentially on the address of
        // each (modifiable) attribute of this object, supplying 'manipulator'
        // with the corresponding attribute information structure until such
        // invocation returns a non-zero value.  Return the value from the
        // last invocation of 'manipulator' (i.e., the invocation that
        // terminated the sequence).

    template<class MANIPULATOR>
    int manipulateAttribute(MANIPULATOR& manipulator, int id);
        // Invoke the specified 'manipulator' on the address of
        // the (modifiable) attribute indicated by the specified 'id',
        // supplying 'manipulator' with the corresponding attribute
        // information structure.  Return the value returned from the
        // invocation of 'manipulator' if 'id' identifies an attribute of this
        // class, and -1 otherwise.

    template<class MANIPULATOR>
    int manipulateAttribute(MANIPULATOR&  manipulator,
                            const char   *name,
                            int           nameLength);
        // Invoke the specified 'manipulator' on the address of
        // the (modifiable) attribute indicated by the specified 'name' of the
        // specified 'nameLength', supplying 'manipulator' with the
        // corresponding attribute information structure.  Return the value
        // returned from the invocation of 'manipulator' if 'name' identifies
        // an attribute of this class, and -1 otherwise.

    bdeut_NullableValue<bool>& attribute1();
        // Return a reference to the modifiable "Attribute1" attribute of this
        // object.

    bdeut_NullableValue<bsl::string>& attribute2();
        // Return a reference to the modifiable "Attribute2" attribute of this
        // object.

    bsl::string& theContent();
        // Return a reference to the modifiable "TheContent" attribute of this
        // object.

    // ACCESSORS
    bsl::ostream& print(bsl::ostream& stream,
                        int           level = 0,
                        int           spacesPerLevel = 4) const;
        // Format this object to the specified output 'stream' at the
        // optionally specified indentation 'level' and return a reference to
        // the modifiable 'stream'.  If 'level' is specified, optionally
        // specify 'spacesPerLevel', the number of spaces per indentation level
        // for this and all of its nested objects.  Each line is indented by
        // the absolute value of 'level * spacesPerLevel'.  If 'level' is
        // negative, suppress indentation of the first line.  If
        // 'spacesPerLevel' is negative, suppress line breaks and format the
        // entire output on one line.  If 'stream' is initially invalid, this
        // operation has no effect.  Note that a trailing newline is provided
        // in multiline mode only.

    template <class STREAM>
    STREAM& bdexStreamOut(STREAM& stream, int version) const;
        // Write the value of this object to the specified output 'stream'
        // using the specified 'version' format and return a reference to the
        // modifiable 'stream'.  If 'version' is not supported, 'stream' is
        // unmodified.  Note that 'version' is not written to 'stream'.
        // See the 'bdex' package-level documentation for more information
        // on 'bdex' streaming of value-semantic types and containers.

    template<class ACCESSOR>
    int accessAttributes(ACCESSOR& accessor) const;
        // Invoke the specified 'accessor' sequentially on each
        // (non-modifiable) attribute of this object, supplying 'accessor'
        // with the corresponding attribute information structure until such
        // invocation returns a non-zero value.  Return the value from the
        // last invocation of 'accessor' (i.e., the invocation that terminated
        // the sequence).

    template<class ACCESSOR>
    int accessAttribute(ACCESSOR& accessor, int id) const;
        // Invoke the specified 'accessor' on the (non-modifiable) attribute
        // of this object indicated by the specified 'id', supplying 'accessor'
        // with the corresponding attribute information structure.  Return the
        // value returned from the invocation of 'accessor' if 'id' identifies
        // an attribute of this class, and -1 otherwise.

    template<class ACCESSOR>
    int accessAttribute(ACCESSOR&   accessor,
                        const char *name,
                        int         nameLength) const;
        // Invoke the specified 'accessor' on the (non-modifiable) attribute
        // of this object indicated by the specified 'name' of the specified
        // 'nameLength', supplying 'accessor' with the corresponding attribute
        // information structure.  Return the value returned from the
        // invocation of 'accessor' if 'name' identifies an attribute of this
        // class, and -1 otherwise.

    const bdeut_NullableValue<bool>& attribute1() const;
        // Return a reference to the non-modifiable "Attribute1" attribute of
        // this object.

    const bdeut_NullableValue<bsl::string>& attribute2() const;
        // Return a reference to the non-modifiable "Attribute2" attribute of
        // this object.

    const bsl::string& theContent() const;
        // Return a reference to the non-modifiable "TheContent" attribute of
        // this object.
};

// FREE OPERATORS
inline
bool operator==(const MySimpleContent& lhs, const MySimpleContent& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects have
    // the same value, and 'false' otherwise.  Two attribute objects have the
    // same value if each respective attribute has the same value.

inline
bool operator!=(const MySimpleContent& lhs, const MySimpleContent& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects do not
    // have the same value, and 'false' otherwise.  Two attribute objects do
    // not have the same value if one or more respective attributes differ in
    // values.

inline
bsl::ostream& operator<<(bsl::ostream& stream, const MySimpleContent& rhs);
    // Format the specified 'rhs' to the specified output 'stream' and
    // return a reference to the modifiable 'stream'.

}  // close namespace test

// TRAITS

BDEAT_DECL_SEQUENCE_WITH_ALLOCATOR_TRAITS(test::MySimpleContent)

namespace test {

                          // ========================
                          // class MySimpleIntContent
                          // ========================

class MySimpleIntContent {
    // MySimpleIntContent: TBD: Provide annotation

  private:
    bdeut_NullableValue<bool> d_attribute1;
        // Attribute1: TBD: Provide annotation
    bdeut_NullableValue<bsl::string> d_attribute2;
        // Attribute2: TBD: Provide annotation
    int d_theContent;
        // TheContent: TBD: Provide annotation

  public:
    // TYPES
    enum {
        NUM_ATTRIBUTES = 3 // the number of attributes in this class
    };

    enum {
        ATTRIBUTE_INDEX_ATTRIBUTE1 = 0,
            // index for "Attribute1" attribute
        ATTRIBUTE_INDEX_ATTRIBUTE2 = 1,
            // index for "Attribute2" attribute
        ATTRIBUTE_INDEX_THE_CONTENT = 2
            // index for "TheContent" attribute
    };

    enum {
        ATTRIBUTE_ID_ATTRIBUTE1 = 0,
            // id for "Attribute1" attribute
        ATTRIBUTE_ID_ATTRIBUTE2 = 1,
            // id for "Attribute2" attribute
        ATTRIBUTE_ID_THE_CONTENT = 2
            // id for "TheContent" attribute
    };

  public:
    // CONSTANTS
    static const char CLASS_NAME[];
        // the name of this class (i.e., "MySimpleIntContent")

    static const bdeat_AttributeInfo ATTRIBUTE_INFO_ARRAY[];
        // attribute information for each attribute

  public:
    // CLASS METHODS
    static int maxSupportedBdexVersion();
        // Return the most current 'bdex' streaming version number supported by
        // this class.  See the 'bdex' package-level documentation for more
        // information on 'bdex' streaming of value-semantic types and
        // containers.

    static const bdeat_AttributeInfo *lookupAttributeInfo(int id);
        // Return attribute information for the attribute indicated by the
        // specified 'id' if the attribute exists, and 0 otherwise.

    static const bdeat_AttributeInfo *lookupAttributeInfo(
                                                    const char *name,
                                                    int         nameLength);
        // Return attribute information for the attribute indicated by the
        // specified 'name' of the specified 'nameLength' if the attribute
        // exists, and 0 otherwise.

    // CREATORS
    explicit MySimpleIntContent(bslma_Allocator *basicAllocator = 0);
        // Create an object of type 'MySimpleIntContent' having the default
        // value.  Use the optionally specified 'basicAllocator' to supply
        // memory.  If 'basicAllocator' is 0, the currently installed default
        // allocator is used.

    MySimpleIntContent(const MySimpleIntContent& original,
                       bslma_Allocator *basicAllocator = 0);
        // Create an object of type 'MySimpleIntContent' having the value of
        // the specified 'original' object.  Use the optionally specified
        // 'basicAllocator' to supply memory.  If 'basicAllocator' is 0, the
        // currently installed default allocator is used.

    ~MySimpleIntContent();
        // Destroy this object.

    // MANIPULATORS
    MySimpleIntContent& operator=(const MySimpleIntContent& rhs);
        // Assign to this object the value of the specified 'rhs' object.

    template <class STREAM>
    STREAM& bdexStreamIn(STREAM& stream, int version);
        // Assign to this object the value read from the specified input
        // 'stream' using the specified 'version' format and return a reference
        // to the modifiable 'stream'.  If 'stream' is initially invalid, this
        // operation has no effect.  If 'stream' becomes invalid during this
        // operation, this object is valid, but its value is undefined.  If
        // 'version' is not supported, 'stream' is marked invalid and this
        // object is unaltered.  Note that no version is read from 'stream'.
        // See the 'bdex' package-level documentation for more information on
        // 'bdex' streaming of value-semantic types and containers.

    void reset();
        // Reset this object to the default value (i.e., its value upon
        // default construction).

    template<class MANIPULATOR>
    int manipulateAttributes(MANIPULATOR& manipulator);
        // Invoke the specified 'manipulator' sequentially on the address of
        // each (modifiable) attribute of this object, supplying 'manipulator'
        // with the corresponding attribute information structure until such
        // invocation returns a non-zero value.  Return the value from the
        // last invocation of 'manipulator' (i.e., the invocation that
        // terminated the sequence).

    template<class MANIPULATOR>
    int manipulateAttribute(MANIPULATOR& manipulator, int id);
        // Invoke the specified 'manipulator' on the address of
        // the (modifiable) attribute indicated by the specified 'id',
        // supplying 'manipulator' with the corresponding attribute
        // information structure.  Return the value returned from the
        // invocation of 'manipulator' if 'id' identifies an attribute of this
        // class, and -1 otherwise.

    template<class MANIPULATOR>
    int manipulateAttribute(MANIPULATOR&  manipulator,
                            const char   *name,
                            int           nameLength);
        // Invoke the specified 'manipulator' on the address of
        // the (modifiable) attribute indicated by the specified 'name' of the
        // specified 'nameLength', supplying 'manipulator' with the
        // corresponding attribute information structure.  Return the value
        // returned from the invocation of 'manipulator' if 'name' identifies
        // an attribute of this class, and -1 otherwise.

    bdeut_NullableValue<bool>& attribute1();
        // Return a reference to the modifiable "Attribute1" attribute of this
        // object.

    bdeut_NullableValue<bsl::string>& attribute2();
        // Return a reference to the modifiable "Attribute2" attribute of this
        // object.

    int& theContent();
        // Return a reference to the modifiable "TheContent" attribute of this
        // object.

    // ACCESSORS
    bsl::ostream& print(bsl::ostream& stream,
                        int           level = 0,
                        int           spacesPerLevel = 4) const;
        // Format this object to the specified output 'stream' at the
        // optionally specified indentation 'level' and return a reference to
        // the modifiable 'stream'.  If 'level' is specified, optionally
        // specify 'spacesPerLevel', the number of spaces per indentation level
        // for this and all of its nested objects.  Each line is indented by
        // the absolute value of 'level * spacesPerLevel'.  If 'level' is
        // negative, suppress indentation of the first line.  If
        // 'spacesPerLevel' is negative, suppress line breaks and format the
        // entire output on one line.  If 'stream' is initially invalid, this
        // operation has no effect.  Note that a trailing newline is provided
        // in multiline mode only.

    template <class STREAM>
    STREAM& bdexStreamOut(STREAM& stream, int version) const;
        // Write the value of this object to the specified output 'stream'
        // using the specified 'version' format and return a reference to the
        // modifiable 'stream'.  If 'version' is not supported, 'stream' is
        // unmodified.  Note that 'version' is not written to 'stream'.
        // See the 'bdex' package-level documentation for more information
        // on 'bdex' streaming of value-semantic types and containers.

    template<class ACCESSOR>
    int accessAttributes(ACCESSOR& accessor) const;
        // Invoke the specified 'accessor' sequentially on each
        // (non-modifiable) attribute of this object, supplying 'accessor'
        // with the corresponding attribute information structure until such
        // invocation returns a non-zero value.  Return the value from the
        // last invocation of 'accessor' (i.e., the invocation that terminated
        // the sequence).

    template<class ACCESSOR>
    int accessAttribute(ACCESSOR& accessor, int id) const;
        // Invoke the specified 'accessor' on the (non-modifiable) attribute
        // of this object indicated by the specified 'id', supplying 'accessor'
        // with the corresponding attribute information structure.  Return the
        // value returned from the invocation of 'accessor' if 'id' identifies
        // an attribute of this class, and -1 otherwise.

    template<class ACCESSOR>
    int accessAttribute(ACCESSOR&   accessor,
                        const char *name,
                        int         nameLength) const;
        // Invoke the specified 'accessor' on the (non-modifiable) attribute
        // of this object indicated by the specified 'name' of the specified
        // 'nameLength', supplying 'accessor' with the corresponding attribute
        // information structure.  Return the value returned from the
        // invocation of 'accessor' if 'name' identifies an attribute of this
        // class, and -1 otherwise.

    const bdeut_NullableValue<bool>& attribute1() const;
        // Return a reference to the non-modifiable "Attribute1" attribute of
        // this object.

    const bdeut_NullableValue<bsl::string>& attribute2() const;
        // Return a reference to the non-modifiable "Attribute2" attribute of
        // this object.

    const int& theContent() const;
        // Return a reference to the non-modifiable "TheContent" attribute of
        // this object.
};

// FREE OPERATORS
inline
bool operator==(const MySimpleIntContent& lhs, const MySimpleIntContent& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects have
    // the same value, and 'false' otherwise.  Two attribute objects have the
    // same value if each respective attribute has the same value.

inline
bool operator!=(const MySimpleIntContent& lhs, const MySimpleIntContent& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects do not
    // have the same value, and 'false' otherwise.  Two attribute objects do
    // not have the same value if one or more respective attributes differ in
    // values.

inline
bsl::ostream& operator<<(bsl::ostream& stream, const MySimpleIntContent& rhs);
    // Format the specified 'rhs' to the specified output 'stream' and
    // return a reference to the modifiable 'stream'.

}  // close namespace test

// TRAITS

BDEAT_DECL_SEQUENCE_WITH_ALLOCATOR_TRAITS(test::MySimpleIntContent)

namespace test {

                 // =========================================
                 // class MySequenceWithAnonymousChoiceChoice
                 // =========================================

class MySequenceWithAnonymousChoiceChoice {
    // MySequenceWithAnonymousChoiceChoice: TBD: Provide annotation

  private:
    union {
        bsls_ObjectBuffer< int > d_myChoice1;
            // MyChoice1: TBD: Provide annotation
        bsls_ObjectBuffer< bsl::string > d_myChoice2;
            // MyChoice2: TBD: Provide annotation
    };

    int                 d_selectionId;

    bslma_Allocator    *d_allocator_p;

  public:
    // TYPES
    enum {
        NUM_SELECTIONS = 2 // the number of selections in this class
    };

    enum {
        SELECTION_INDEX_MY_CHOICE1 = 0,
            // index for "MyChoice1" selection
        SELECTION_INDEX_MY_CHOICE2 = 1
            // index for "MyChoice2" selection
    };

    enum {
        SELECTION_ID_UNDEFINED = -1,

        SELECTION_ID_MY_CHOICE1 = 0,
            // id for "MyChoice1" selection
        SELECTION_ID_MY_CHOICE2 = 1
            // id for "MyChoice2" selection
    };

  public:
    // CONSTANTS
    static const char CLASS_NAME[];
        // the name of this class (i.e., "MySequenceWithAnonymousChoiceChoice")

    static const bdeat_SelectionInfo SELECTION_INFO_ARRAY[];
        // selection information for each selection

  public:
    // CLASS METHODS
    static int maxSupportedBdexVersion();
        // Return the most current 'bdex' streaming version number supported by
        // this class.  See the 'bdex' package-level documentation for more
        // information on 'bdex' streaming of value-semantic types and
        // containers.

    static const bdeat_SelectionInfo *lookupSelectionInfo(int id);
        // Return selection information for the selection indicated by the
        // specified 'id' if the selection exists, and 0 otherwise.

    static const bdeat_SelectionInfo *lookupSelectionInfo(
                                                    const char *name,
                                                    int         nameLength);
        // Return selection information for the selection indicated by the
        // specified 'name' of the specified 'nameLength' if the selection
        // exists, and 0 otherwise.

    // CREATORS
    explicit MySequenceWithAnonymousChoiceChoice(
                    bslma_Allocator *basicAllocator = 0);
        // Create an object of type 'MySequenceWithAnonymousChoiceChoice'
        // having the default value.  Use the optionally specified
        // 'basicAllocator' to supply memory.  If 'basicAllocator' is 0, the
        // currently installed default allocator is used.

    MySequenceWithAnonymousChoiceChoice(
            const MySequenceWithAnonymousChoiceChoice&  original,
            bslma_Allocator                            *basicAllocator = 0);
        // Create an object of type 'MySequenceWithAnonymousChoiceChoice'
        // having the value of the specified 'original' object.  Use the
        // optionally specified 'basicAllocator' to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.

    ~MySequenceWithAnonymousChoiceChoice();
        // Destroy this object.

    // MANIPULATORS
    MySequenceWithAnonymousChoiceChoice& operator=(
                    const MySequenceWithAnonymousChoiceChoice& rhs);
        // Assign to this object the value of the specified 'rhs' object.

    template <class STREAM>
    STREAM& bdexStreamIn(STREAM& stream, int version);
        // Assign to this object the value read from the specified input
        // 'stream' using the specified 'version' format and return a reference
        // to the modifiable 'stream'.  If 'stream' is initially invalid, this
        // operation has no effect.  If 'stream' becomes invalid during this
        // operation, this object is valid, but its value is undefined.  If
        // 'version' is not supported, 'stream' is marked invalid and this
        // object is unaltered.  Note that no version is read from 'stream'.
        // See the 'bdex' package-level documentation for more information on
        // 'bdex' streaming of value-semantic types and containers.

    void reset();
        // Reset this object to the default value (i.e., its value upon default
        // construction).

    int makeSelection(int selectionId);
        // Set the value of this object to be the default for the selection
        // indicated by the specified 'selectionId'.  Return 0 on success, and
        // non-zero value otherwise (i.e., the selection is not found).

    int makeSelection(const char *name, int nameLength);
        // Set the value of this object to be the default for the selection
        // indicated by the specified 'name' of the specified 'nameLength'.
        // Return 0 on success, and non-zero value otherwise (i.e., the
        // selection is not found).

    int& makeMyChoice1();
    int& makeMyChoice1(int value);
        // Set the value of this object to be a "MyChoice1" value.  Optionally
        // specify the 'value' of the "MyChoice1".  If 'value' is not
        // specified, the default "MyChoice1" value is used.

    bsl::string& makeMyChoice2();
    bsl::string& makeMyChoice2(const bsl::string& value);
        // Set the value of this object to be a "MyChoice2" value.  Optionally
        // specify the 'value' of the "MyChoice2".  If 'value' is not
        // specified, the default "MyChoice2" value is used.

    template<class MANIPULATOR>
    int manipulateSelection(MANIPULATOR& manipulator);
        // Invoke the specified 'manipulator' on the address of the modifiable
        // selection, supplying 'manipulator' with the corresponding selection
        // information structure.  Return the value returned from the
        // invocation of 'manipulator' if this object has a defined selection,
        // and -1 otherwise.

    int& myChoice1();
        // Return a reference to the modifiable "MyChoice1" selection of this
        // object if "MyChoice1" is the current selection.  The behavior is
        // undefined unless "MyChoice1" is the selection of this object.

    bsl::string& myChoice2();
        // Return a reference to the modifiable "MyChoice2" selection of this
        // object if "MyChoice2" is the current selection.  The behavior is
        // undefined unless "MyChoice2" is the selection of this object.

    // ACCESSORS
    bsl::ostream& print(bsl::ostream& stream,
                        int           level = 0,
                        int           spacesPerLevel = 4) const;
        // Format this object to the specified output 'stream' at the
        // optionally specified indentation 'level' and return a reference to
        // the modifiable 'stream'.  If 'level' is specified, optionally
        // specify 'spacesPerLevel', the number of spaces per indentation level
        // for this and all of its nested objects.  Each line is indented by
        // the absolute value of 'level * spacesPerLevel'.  If 'level' is
        // negative, suppress indentation of the first line.  If
        // 'spacesPerLevel' is negative, suppress line breaks and format the
        // entire output on one line.  If 'stream' is initially invalid, this
        // operation has no effect.  Note that a trailing newline is provided
        // in multiline mode only.

    template <class STREAM>
    STREAM& bdexStreamOut(STREAM& stream, int version) const;
        // Write the value of this object to the specified output 'stream'
        // using the specified 'version' format and return a reference to the
        // modifiable 'stream'.  If 'version' is not supported, 'stream' is
        // unmodified.  Note that 'version' is not written to 'stream'.
        // See the 'bdex' package-level documentation for more information
        // on 'bdex' streaming of value-semantic types and containers.

    int selectionId() const;
        // Return the id of the current selection if the selection is defined,
        // and -1 otherwise.

    template<class ACCESSOR>
    int accessSelection(ACCESSOR& accessor) const;
        // Invoke the specified 'accessor' on the non-modifiable selection,
        // supplying 'accessor' with the corresponding selection information
        // structure.  Return the value returned from the invocation of
        // 'accessor' if this object has a defined selection, and -1 otherwise.

    const int& myChoice1() const;
        // Return a reference to the non-modifiable "MyChoice1" selection of
        // this object if "MyChoice1" is the current selection.  The behavior
        // is undefined unless "MyChoice1" is the selection of this object.

    const bsl::string& myChoice2() const;
        // Return a reference to the non-modifiable "MyChoice2" selection of
        // this object if "MyChoice2" is the current selection.  The behavior
        // is undefined unless "MyChoice2" is the selection of this object.

    bool isMyChoice1Value() const;
        // Return 'true' if the value of this object is a "MyChoice1" value,
        // and return 'false' otherwise.

    bool isMyChoice2Value() const;
        // Return 'true' if the value of this object is a "MyChoice2" value,
        // and return 'false' otherwise.

    bool isUndefinedValue() const;
        // Return 'true' if the value of this object is undefined, and 'false'
        // otherwise.
};

// FREE OPERATORS
inline
bool operator==(const MySequenceWithAnonymousChoiceChoice& lhs,
                const MySequenceWithAnonymousChoiceChoice& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects have the same
    // value, and 'false' otherwise.  Two
    // 'MySequenceWithAnonymousChoiceChoice' objects have the same value if
    // either the selections in both objects have the same ids and the same
    // values, or both selections are undefined.

inline
bool operator!=(const MySequenceWithAnonymousChoiceChoice& lhs,
                const MySequenceWithAnonymousChoiceChoice& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects do not have the
    // same values, as determined by 'operator==', and 'false' otherwise.

inline
bsl::ostream& operator<<(bsl::ostream& stream,
                         const MySequenceWithAnonymousChoiceChoice& rhs);
    // Format the specified 'rhs' to the specified output 'stream' and
    // return a reference to the modifiable 'stream'.

}  // close namespace test

// TRAITS

BDEAT_DECL_CHOICE_WITH_ALLOCATOR_TRAITS(
                test::MySequenceWithAnonymousChoiceChoice)

namespace test {

                       // =============================
                       // class MySequenceWithNullables
                       // =============================

class MySequenceWithNullables {
    // MySequenceWithNullables: TBD: Provide annotation

  private:
    bdeut_NullableValue<int> d_attribute1;
        // Attribute1: TBD: Provide annotation
    bdeut_NullableValue<bsl::string> d_attribute2;
        // Attribute2: TBD: Provide annotation
    bdeut_NullableValue<MySequence> d_attribute3;
        // Attribute3: TBD: Provide annotation

  public:
    // TYPES
    enum {
        NUM_ATTRIBUTES = 3 // the number of attributes in this class
    };

    enum {
        ATTRIBUTE_INDEX_ATTRIBUTE1 = 0,
            // index for "Attribute1" attribute
        ATTRIBUTE_INDEX_ATTRIBUTE2 = 1,
            // index for "Attribute2" attribute
        ATTRIBUTE_INDEX_ATTRIBUTE3 = 2
            // index for "Attribute3" attribute
    };

    enum {
        ATTRIBUTE_ID_ATTRIBUTE1 = 0,
            // id for "Attribute1" attribute
        ATTRIBUTE_ID_ATTRIBUTE2 = 1,
            // id for "Attribute2" attribute
        ATTRIBUTE_ID_ATTRIBUTE3 = 2
            // id for "Attribute3" attribute
    };

  public:
    // CONSTANTS
    static const char CLASS_NAME[];
        // the name of this class (i.e., "MySequenceWithNullables")

    static const bdeat_AttributeInfo ATTRIBUTE_INFO_ARRAY[];
        // attribute information for each attribute

  public:
    // CLASS METHODS
    static int maxSupportedBdexVersion();
        // Return the most current 'bdex' streaming version number supported by
        // this class.  See the 'bdex' package-level documentation for more
        // information on 'bdex' streaming of value-semantic types and
        // containers.

    static const bdeat_AttributeInfo *lookupAttributeInfo(int id);
        // Return attribute information for the attribute indicated by the
        // specified 'id' if the attribute exists, and 0 otherwise.

    static const bdeat_AttributeInfo *lookupAttributeInfo(
                                                    const char *name,
                                                    int         nameLength);
        // Return attribute information for the attribute indicated by the
        // specified 'name' of the specified 'nameLength' if the attribute
        // exists, and 0 otherwise.

    // CREATORS
    explicit MySequenceWithNullables(bslma_Allocator *basicAllocator = 0);
        // Create an object of type 'MySequenceWithNullables' having the
        // default value.  Use the optionally specified 'basicAllocator' to
        // supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.

    MySequenceWithNullables(const MySequenceWithNullables& original,
                            bslma_Allocator *basicAllocator = 0);
        // Create an object of type 'MySequenceWithNullables' having the value
        // of the specified 'original' object.  Use the optionally specified
        // 'basicAllocator' to supply memory.  If 'basicAllocator' is 0, the
        // currently installed default allocator is used.

    ~MySequenceWithNullables();
        // Destroy this object.

    // MANIPULATORS
    MySequenceWithNullables& operator=(const MySequenceWithNullables& rhs);
        // Assign to this object the value of the specified 'rhs' object.

    template <class STREAM>
    STREAM& bdexStreamIn(STREAM& stream, int version);
        // Assign to this object the value read from the specified input
        // 'stream' using the specified 'version' format and return a reference
        // to the modifiable 'stream'.  If 'stream' is initially invalid, this
        // operation has no effect.  If 'stream' becomes invalid during this
        // operation, this object is valid, but its value is undefined.  If
        // 'version' is not supported, 'stream' is marked invalid and this
        // object is unaltered.  Note that no version is read from 'stream'.
        // See the 'bdex' package-level documentation for more information on
        // 'bdex' streaming of value-semantic types and containers.

    void reset();
        // Reset this object to the default value (i.e., its value upon
        // default construction).

    template<class MANIPULATOR>
    int manipulateAttributes(MANIPULATOR& manipulator);
        // Invoke the specified 'manipulator' sequentially on the address of
        // each (modifiable) attribute of this object, supplying 'manipulator'
        // with the corresponding attribute information structure until such
        // invocation returns a non-zero value.  Return the value from the
        // last invocation of 'manipulator' (i.e., the invocation that
        // terminated the sequence).

    template<class MANIPULATOR>
    int manipulateAttribute(MANIPULATOR& manipulator, int id);
        // Invoke the specified 'manipulator' on the address of
        // the (modifiable) attribute indicated by the specified 'id',
        // supplying 'manipulator' with the corresponding attribute
        // information structure.  Return the value returned from the
        // invocation of 'manipulator' if 'id' identifies an attribute of this
        // class, and -1 otherwise.

    template<class MANIPULATOR>
    int manipulateAttribute(MANIPULATOR&  manipulator,
                            const char   *name,
                            int           nameLength);
        // Invoke the specified 'manipulator' on the address of
        // the (modifiable) attribute indicated by the specified 'name' of the
        // specified 'nameLength', supplying 'manipulator' with the
        // corresponding attribute information structure.  Return the value
        // returned from the invocation of 'manipulator' if 'name' identifies
        // an attribute of this class, and -1 otherwise.

    bdeut_NullableValue<int>& attribute1();
        // Return a reference to the modifiable "Attribute1" attribute of this
        // object.

    bdeut_NullableValue<bsl::string>& attribute2();
        // Return a reference to the modifiable "Attribute2" attribute of this
        // object.

    bdeut_NullableValue<MySequence>& attribute3();
        // Return a reference to the modifiable "Attribute3" attribute of this
        // object.

    // ACCESSORS
    bsl::ostream& print(bsl::ostream& stream,
                        int           level = 0,
                        int           spacesPerLevel = 4) const;
        // Format this object to the specified output 'stream' at the
        // optionally specified indentation 'level' and return a reference to
        // the modifiable 'stream'.  If 'level' is specified, optionally
        // specify 'spacesPerLevel', the number of spaces per indentation level
        // for this and all of its nested objects.  Each line is indented by
        // the absolute value of 'level * spacesPerLevel'.  If 'level' is
        // negative, suppress indentation of the first line.  If
        // 'spacesPerLevel' is negative, suppress line breaks and format the
        // entire output on one line.  If 'stream' is initially invalid, this
        // operation has no effect.  Note that a trailing newline is provided
        // in multiline mode only.

    template <class STREAM>
    STREAM& bdexStreamOut(STREAM& stream, int version) const;
        // Write the value of this object to the specified output 'stream'
        // using the specified 'version' format and return a reference to the
        // modifiable 'stream'.  If 'version' is not supported, 'stream' is
        // unmodified.  Note that 'version' is not written to 'stream'.
        // See the 'bdex' package-level documentation for more information
        // on 'bdex' streaming of value-semantic types and containers.

    template<class ACCESSOR>
    int accessAttributes(ACCESSOR& accessor) const;
        // Invoke the specified 'accessor' sequentially on each
        // (non-modifiable) attribute of this object, supplying 'accessor'
        // with the corresponding attribute information structure until such
        // invocation returns a non-zero value.  Return the value from the
        // last invocation of 'accessor' (i.e., the invocation that terminated
        // the sequence).

    template<class ACCESSOR>
    int accessAttribute(ACCESSOR& accessor, int id) const;
        // Invoke the specified 'accessor' on the (non-modifiable) attribute
        // of this object indicated by the specified 'id', supplying 'accessor'
        // with the corresponding attribute information structure.  Return the
        // value returned from the invocation of 'accessor' if 'id' identifies
        // an attribute of this class, and -1 otherwise.

    template<class ACCESSOR>
    int accessAttribute(ACCESSOR&   accessor,
                        const char *name,
                        int         nameLength) const;
        // Invoke the specified 'accessor' on the (non-modifiable) attribute
        // of this object indicated by the specified 'name' of the specified
        // 'nameLength', supplying 'accessor' with the corresponding attribute
        // information structure.  Return the value returned from the
        // invocation of 'accessor' if 'name' identifies an attribute of this
        // class, and -1 otherwise.

    const bdeut_NullableValue<int>& attribute1() const;
        // Return a reference to the non-modifiable "Attribute1" attribute of
        // this object.

    const bdeut_NullableValue<bsl::string>& attribute2() const;
        // Return a reference to the non-modifiable "Attribute2" attribute of
        // this object.

    const bdeut_NullableValue<MySequence>& attribute3() const;
        // Return a reference to the non-modifiable "Attribute3" attribute of
        // this object.
};

// FREE OPERATORS
inline
bool operator==(const MySequenceWithNullables& lhs,
                const MySequenceWithNullables& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects have
    // the same value, and 'false' otherwise.  Two attribute objects have the
    // same value if each respective attribute has the same value.

inline
bool operator!=(const MySequenceWithNullables& lhs,
                const MySequenceWithNullables& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects do not
    // have the same value, and 'false' otherwise.  Two attribute objects do
    // not have the same value if one or more respective attributes differ in
    // values.

inline
bsl::ostream& operator<<(bsl::ostream&                   stream,
                         const MySequenceWithNullables&  rhs);
    // Format the specified 'rhs' to the specified output 'stream' and
    // return a reference to the modifiable 'stream'.

}  // close namespace test

// TRAITS

BDEAT_DECL_SEQUENCE_WITH_ALLOCATOR_TRAITS(test::MySequenceWithNullables)

namespace test {

                               // ==============
                               // class Employee
                               // ==============

class Employee {
    // Employee: TBD: Provide annotation

  private:
    bsl::string d_name;
        // name: TBD: Provide annotation
    Address d_homeAddress;
        // homeAddress: TBD: Provide annotation
    int d_age;
        // age: TBD: Provide annotation

  public:
    // TYPES
    enum {
        NUM_ATTRIBUTES = 3 // the number of attributes in this class
    };

    enum {
        ATTRIBUTE_INDEX_NAME = 0,
            // index for "Name" attribute
        ATTRIBUTE_INDEX_HOME_ADDRESS = 1,
            // index for "HomeAddress" attribute
        ATTRIBUTE_INDEX_AGE = 2
            // index for "Age" attribute
    };

    enum {
        ATTRIBUTE_ID_NAME = 0,
            // id for "Name" attribute
        ATTRIBUTE_ID_HOME_ADDRESS = 1,
            // id for "HomeAddress" attribute
        ATTRIBUTE_ID_AGE = 2
            // id for "Age" attribute
    };

  public:
    // CONSTANTS
    static const char CLASS_NAME[];
        // the name of this class (i.e., "Employee")

    static const bdeat_AttributeInfo ATTRIBUTE_INFO_ARRAY[];
        // attribute information for each attribute

  public:
    // CLASS METHODS
    static int maxSupportedBdexVersion();
        // Return the most current 'bdex' streaming version number supported by
        // this class.  See the 'bdex' package-level documentation for more
        // information on 'bdex' streaming of value-semantic types and
        // containers.

    static const bdeat_AttributeInfo *lookupAttributeInfo(int id);
        // Return attribute information for the attribute indicated by the
        // specified 'id' if the attribute exists, and 0 otherwise.

    static const bdeat_AttributeInfo *lookupAttributeInfo(
                                                    const char *name,
                                                    int         nameLength);
        // Return attribute information for the attribute indicated by the
        // specified 'name' of the specified 'nameLength' if the attribute
        // exists, and 0 otherwise.

    // CREATORS
    explicit Employee(bslma_Allocator *basicAllocator = 0);
        // Create an object of type 'Employee' having the default value.  Use
        // the optionally specified 'basicAllocator' to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.

    Employee(const Employee& original,
             bslma_Allocator *basicAllocator = 0);
        // Create an object of type 'Employee' having the value of the
        // specified 'original' object.  Use the optionally specified
        // 'basicAllocator' to supply memory.  If 'basicAllocator' is 0, the
        // currently installed default allocator is used.

    ~Employee();
        // Destroy this object.

    // MANIPULATORS
    Employee& operator=(const Employee& rhs);
        // Assign to this object the value of the specified 'rhs' object.

    template <class STREAM>
    STREAM& bdexStreamIn(STREAM& stream, int version);
        // Assign to this object the value read from the specified input
        // 'stream' using the specified 'version' format and return a reference
        // to the modifiable 'stream'.  If 'stream' is initially invalid, this
        // operation has no effect.  If 'stream' becomes invalid during this
        // operation, this object is valid, but its value is undefined.  If
        // 'version' is not supported, 'stream' is marked invalid and this
        // object is unaltered.  Note that no version is read from 'stream'.
        // See the 'bdex' package-level documentation for more information on
        // 'bdex' streaming of value-semantic types and containers.

    void reset();
        // Reset this object to the default value (i.e., its value upon
        // default construction).

    template<class MANIPULATOR>
    int manipulateAttributes(MANIPULATOR& manipulator);
        // Invoke the specified 'manipulator' sequentially on the address of
        // each (modifiable) attribute of this object, supplying 'manipulator'
        // with the corresponding attribute information structure until such
        // invocation returns a non-zero value.  Return the value from the
        // last invocation of 'manipulator' (i.e., the invocation that
        // terminated the sequence).

    template<class MANIPULATOR>
    int manipulateAttribute(MANIPULATOR& manipulator, int id);
        // Invoke the specified 'manipulator' on the address of
        // the (modifiable) attribute indicated by the specified 'id',
        // supplying 'manipulator' with the corresponding attribute
        // information structure.  Return the value returned from the
        // invocation of 'manipulator' if 'id' identifies an attribute of this
        // class, and -1 otherwise.

    template<class MANIPULATOR>
    int manipulateAttribute(MANIPULATOR&  manipulator,
                            const char   *name,
                            int           nameLength);
        // Invoke the specified 'manipulator' on the address of
        // the (modifiable) attribute indicated by the specified 'name' of the
        // specified 'nameLength', supplying 'manipulator' with the
        // corresponding attribute information structure.  Return the value
        // returned from the invocation of 'manipulator' if 'name' identifies
        // an attribute of this class, and -1 otherwise.

    bsl::string& name();
        // Return a reference to the modifiable "Name" attribute of this
        // object.

    Address& homeAddress();
        // Return a reference to the modifiable "HomeAddress" attribute of this
        // object.

    int& age();
        // Return a reference to the modifiable "Age" attribute of this object.

    // ACCESSORS
    bsl::ostream& print(bsl::ostream& stream,
                        int           level = 0,
                        int           spacesPerLevel = 4) const;
        // Format this object to the specified output 'stream' at the
        // optionally specified indentation 'level' and return a reference to
        // the modifiable 'stream'.  If 'level' is specified, optionally
        // specify 'spacesPerLevel', the number of spaces per indentation level
        // for this and all of its nested objects.  Each line is indented by
        // the absolute value of 'level * spacesPerLevel'.  If 'level' is
        // negative, suppress indentation of the first line.  If
        // 'spacesPerLevel' is negative, suppress line breaks and format the
        // entire output on one line.  If 'stream' is initially invalid, this
        // operation has no effect.  Note that a trailing newline is provided
        // in multiline mode only.

    template <class STREAM>
    STREAM& bdexStreamOut(STREAM& stream, int version) const;
        // Write the value of this object to the specified output 'stream'
        // using the specified 'version' format and return a reference to the
        // modifiable 'stream'.  If 'version' is not supported, 'stream' is
        // unmodified.  Note that 'version' is not written to 'stream'.
        // See the 'bdex' package-level documentation for more information
        // on 'bdex' streaming of value-semantic types and containers.

    template<class ACCESSOR>
    int accessAttributes(ACCESSOR& accessor) const;
        // Invoke the specified 'accessor' sequentially on each
        // (non-modifiable) attribute of this object, supplying 'accessor'
        // with the corresponding attribute information structure until such
        // invocation returns a non-zero value.  Return the value from the
        // last invocation of 'accessor' (i.e., the invocation that terminated
        // the sequence).

    template<class ACCESSOR>
    int accessAttribute(ACCESSOR& accessor, int id) const;
        // Invoke the specified 'accessor' on the (non-modifiable) attribute
        // of this object indicated by the specified 'id', supplying 'accessor'
        // with the corresponding attribute information structure.  Return the
        // value returned from the invocation of 'accessor' if 'id' identifies
        // an attribute of this class, and -1 otherwise.

    template<class ACCESSOR>
    int accessAttribute(ACCESSOR&   accessor,
                        const char *name,
                        int         nameLength) const;
        // Invoke the specified 'accessor' on the (non-modifiable) attribute
        // of this object indicated by the specified 'name' of the specified
        // 'nameLength', supplying 'accessor' with the corresponding attribute
        // information structure.  Return the value returned from the
        // invocation of 'accessor' if 'name' identifies an attribute of this
        // class, and -1 otherwise.

    const bsl::string& name() const;
        // Return a reference to the non-modifiable "Name" attribute of this
        // object.

    const Address& homeAddress() const;
        // Return a reference to the non-modifiable "HomeAddress" attribute of
        // this object.

    const int& age() const;
        // Return a reference to the non-modifiable "Age" attribute of this
        // object.
};

// FREE OPERATORS
inline
bool operator==(const Employee& lhs, const Employee& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects have
    // the same value, and 'false' otherwise.  Two attribute objects have the
    // same value if each respective attribute has the same value.

inline
bool operator!=(const Employee& lhs, const Employee& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects do not
    // have the same value, and 'false' otherwise.  Two attribute objects do
    // not have the same value if one or more respective attributes differ in
    // values.

inline
bsl::ostream& operator<<(bsl::ostream& stream, const Employee& rhs);
    // Format the specified 'rhs' to the specified output 'stream' and
    // return a reference to the modifiable 'stream'.

}  // close namespace test

// TRAITS

BDEAT_DECL_SEQUENCE_WITH_ALLOCATOR_TRAITS(test::Employee)

namespace test {

                    // ===================================
                    // class MySequenceWithAnonymousChoice
                    // ===================================

class MySequenceWithAnonymousChoice {
    // MySequenceWithAnonymousChoice: TBD: Provide annotation

  private:
    bdeut_NullableValue<int> d_attribute1;
        // Attribute1: TBD: Provide annotation
    MySequenceWithAnonymousChoiceChoice d_mySequenceWithAnonymousChoiceChoice;
        // MySequenceWithAnonymousChoiceChoice: TBD: Provide annotation
    bdeut_NullableValue<bsl::string> d_attribute2;
        // Attribute2: TBD: Provide annotation

  public:
    // TYPES
    enum {
        NUM_ATTRIBUTES = 3 // the number of attributes in this class
    };

    enum {
        ATTRIBUTE_INDEX_ATTRIBUTE1 = 0,
            // index for "Attribute1" attribute
        ATTRIBUTE_INDEX_MY_SEQUENCE_WITH_ANONYMOUS_CHOICE_CHOICE = 1,
            // index for "MySequenceWithAnonymousChoiceChoice" attribute
        ATTRIBUTE_INDEX_ATTRIBUTE2 = 2
            // index for "Attribute2" attribute
    };

    enum {
        ATTRIBUTE_ID_ATTRIBUTE1 = 0,
            // id for "Attribute1" attribute
        ATTRIBUTE_ID_MY_SEQUENCE_WITH_ANONYMOUS_CHOICE_CHOICE = 1,
            // id for "MySequenceWithAnonymousChoiceChoice" attribute
        ATTRIBUTE_ID_ATTRIBUTE2 = 2
            // id for "Attribute2" attribute
    };

  public:
    // CONSTANTS
    static const char CLASS_NAME[];
        // the name of this class (i.e., "MySequenceWithAnonymousChoice")

    static const bdeat_AttributeInfo ATTRIBUTE_INFO_ARRAY[];
        // attribute information for each attribute

  public:
    // CLASS METHODS
    static int maxSupportedBdexVersion();
        // Return the most current 'bdex' streaming version number supported by
        // this class.  See the 'bdex' package-level documentation for more
        // information on 'bdex' streaming of value-semantic types and
        // containers.

    static const bdeat_AttributeInfo *lookupAttributeInfo(int id);
        // Return attribute information for the attribute indicated by the
        // specified 'id' if the attribute exists, and 0 otherwise.

    static const bdeat_AttributeInfo *lookupAttributeInfo(
                                                    const char *name,
                                                    int         nameLength);
        // Return attribute information for the attribute indicated by the
        // specified 'name' of the specified 'nameLength' if the attribute
        // exists, and 0 otherwise.

    // CREATORS
    explicit MySequenceWithAnonymousChoice(
                    bslma_Allocator *basicAllocator = 0);
        // Create an object of type 'MySequenceWithAnonymousChoice' having the
        // default value.  Use the optionally specified 'basicAllocator' to
        // supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.

    MySequenceWithAnonymousChoice(
                    const MySequenceWithAnonymousChoice&  original,
                    bslma_Allocator                      *basicAllocator = 0);
        // Create an object of type 'MySequenceWithAnonymousChoice' having the
        // value of the specified 'original' object.  Use the optionally
        // specified 'basicAllocator' to supply memory.  If 'basicAllocator' is
        // 0, the currently installed default allocator is used.

    ~MySequenceWithAnonymousChoice();
        // Destroy this object.

    // MANIPULATORS
    MySequenceWithAnonymousChoice& operator=(
                    const MySequenceWithAnonymousChoice& rhs);
        // Assign to this object the value of the specified 'rhs' object.

    template <class STREAM>
    STREAM& bdexStreamIn(STREAM& stream, int version);
        // Assign to this object the value read from the specified input
        // 'stream' using the specified 'version' format and return a reference
        // to the modifiable 'stream'.  If 'stream' is initially invalid, this
        // operation has no effect.  If 'stream' becomes invalid during this
        // operation, this object is valid, but its value is undefined.  If
        // 'version' is not supported, 'stream' is marked invalid and this
        // object is unaltered.  Note that no version is read from 'stream'.
        // See the 'bdex' package-level documentation for more information on
        // 'bdex' streaming of value-semantic types and containers.

    void reset();
        // Reset this object to the default value (i.e., its value upon
        // default construction).

    template<class MANIPULATOR>
    int manipulateAttributes(MANIPULATOR& manipulator);
        // Invoke the specified 'manipulator' sequentially on the address of
        // each (modifiable) attribute of this object, supplying 'manipulator'
        // with the corresponding attribute information structure until such
        // invocation returns a non-zero value.  Return the value from the
        // last invocation of 'manipulator' (i.e., the invocation that
        // terminated the sequence).

    template<class MANIPULATOR>
    int manipulateAttribute(MANIPULATOR& manipulator, int id);
        // Invoke the specified 'manipulator' on the address of
        // the (modifiable) attribute indicated by the specified 'id',
        // supplying 'manipulator' with the corresponding attribute
        // information structure.  Return the value returned from the
        // invocation of 'manipulator' if 'id' identifies an attribute of this
        // class, and -1 otherwise.

    template<class MANIPULATOR>
    int manipulateAttribute(MANIPULATOR&  manipulator,
                            const char   *name,
                            int           nameLength);
        // Invoke the specified 'manipulator' on the address of
        // the (modifiable) attribute indicated by the specified 'name' of the
        // specified 'nameLength', supplying 'manipulator' with the
        // corresponding attribute information structure.  Return the value
        // returned from the invocation of 'manipulator' if 'name' identifies
        // an attribute of this class, and -1 otherwise.

    bdeut_NullableValue<int>& attribute1();
        // Return a reference to the modifiable "Attribute1" attribute of this
        // object.

    MySequenceWithAnonymousChoiceChoice& mySequenceWithAnonymousChoiceChoice();
        // Return a reference to the modifiable
        // "MySequenceWithAnonymousChoiceChoice" attribute of this object.

    bdeut_NullableValue<bsl::string>& attribute2();
        // Return a reference to the modifiable "Attribute2" attribute of this
        // object.

    // ACCESSORS
    bsl::ostream& print(bsl::ostream& stream,
                        int           level = 0,
                        int           spacesPerLevel = 4) const;
        // Format this object to the specified output 'stream' at the
        // optionally specified indentation 'level' and return a reference to
        // the modifiable 'stream'.  If 'level' is specified, optionally
        // specify 'spacesPerLevel', the number of spaces per indentation level
        // for this and all of its nested objects.  Each line is indented by
        // the absolute value of 'level * spacesPerLevel'.  If 'level' is
        // negative, suppress indentation of the first line.  If
        // 'spacesPerLevel' is negative, suppress line breaks and format the
        // entire output on one line.  If 'stream' is initially invalid, this
        // operation has no effect.  Note that a trailing newline is provided
        // in multiline mode only.

    template <class STREAM>
    STREAM& bdexStreamOut(STREAM& stream, int version) const;
        // Write the value of this object to the specified output 'stream'
        // using the specified 'version' format and return a reference to the
        // modifiable 'stream'.  If 'version' is not supported, 'stream' is
        // unmodified.  Note that 'version' is not written to 'stream'.
        // See the 'bdex' package-level documentation for more information
        // on 'bdex' streaming of value-semantic types and containers.

    template<class ACCESSOR>
    int accessAttributes(ACCESSOR& accessor) const;
        // Invoke the specified 'accessor' sequentially on each
        // (non-modifiable) attribute of this object, supplying 'accessor'
        // with the corresponding attribute information structure until such
        // invocation returns a non-zero value.  Return the value from the
        // last invocation of 'accessor' (i.e., the invocation that terminated
        // the sequence).

    template<class ACCESSOR>
    int accessAttribute(ACCESSOR& accessor, int id) const;
        // Invoke the specified 'accessor' on the (non-modifiable) attribute
        // of this object indicated by the specified 'id', supplying 'accessor'
        // with the corresponding attribute information structure.  Return the
        // value returned from the invocation of 'accessor' if 'id' identifies
        // an attribute of this class, and -1 otherwise.

    template<class ACCESSOR>
    int accessAttribute(ACCESSOR&   accessor,
                        const char *name,
                        int         nameLength) const;
        // Invoke the specified 'accessor' on the (non-modifiable) attribute
        // of this object indicated by the specified 'name' of the specified
        // 'nameLength', supplying 'accessor' with the corresponding attribute
        // information structure.  Return the value returned from the
        // invocation of 'accessor' if 'name' identifies an attribute of this
        // class, and -1 otherwise.

    const bdeut_NullableValue<int>& attribute1() const;
        // Return a reference to the non-modifiable "Attribute1" attribute of
        // this object.

    const MySequenceWithAnonymousChoiceChoice&
                                   mySequenceWithAnonymousChoiceChoice() const;
        // Return a reference to the non-modifiable
        // "MySequenceWithAnonymousChoiceChoice" attribute of this object.

    const bdeut_NullableValue<bsl::string>& attribute2() const;
        // Return a reference to the non-modifiable "Attribute2" attribute of
        // this object.
};

// FREE OPERATORS
inline
bool operator==(const MySequenceWithAnonymousChoice& lhs,
                const MySequenceWithAnonymousChoice& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects have
    // the same value, and 'false' otherwise.  Two attribute objects have the
    // same value if each respective attribute has the same value.

inline
bool operator!=(const MySequenceWithAnonymousChoice& lhs,
                const MySequenceWithAnonymousChoice& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects do not
    // have the same value, and 'false' otherwise.  Two attribute objects do
    // not have the same value if one or more respective attributes differ in
    // values.

inline
bsl::ostream& operator<<(bsl::ostream&                         stream,
                         const MySequenceWithAnonymousChoice&  rhs);
    // Format the specified 'rhs' to the specified output 'stream' and
    // return a reference to the modifiable 'stream'.

}  // close namespace test

// TRAITS

BDEAT_DECL_SEQUENCE_WITH_ALLOCATOR_TRAITS(test::MySequenceWithAnonymousChoice)

namespace test {

                       // =============================
                       // class MySequenceWithNillables
                       // =============================

class MySequenceWithNillables {
    // MySequenceWithNillables: TBD: Provide annotation

  private:
    bdeut_NullableValue<int> d_attribute1;
        // Attribute1: TBD: Provide annotation
    bdeut_NullableValue<bsl::string> d_attribute2;
        // Attribute2: TBD: Provide annotation
    bdeut_NullableValue<MySequence> d_attribute3;
        // Attribute3: TBD: Provide annotation

  public:
    // TYPES
    enum {
        NUM_ATTRIBUTES = 3 // the number of attributes in this class
    };

    enum {
        ATTRIBUTE_INDEX_ATTRIBUTE1 = 0,
            // index for "Attribute1" attribute
        ATTRIBUTE_INDEX_ATTRIBUTE2 = 1,
            // index for "Attribute2" attribute
        ATTRIBUTE_INDEX_ATTRIBUTE3 = 2
            // index for "Attribute3" attribute
    };

    enum {
        ATTRIBUTE_ID_ATTRIBUTE1 = 0,
            // id for "Attribute1" attribute
        ATTRIBUTE_ID_ATTRIBUTE2 = 1,
            // id for "Attribute2" attribute
        ATTRIBUTE_ID_ATTRIBUTE3 = 2
            // id for "Attribute3" attribute
    };

  public:
    // CONSTANTS
    static const char CLASS_NAME[];
        // the name of this class (i.e., "MySequenceWithNillables")

    static const bdeat_AttributeInfo ATTRIBUTE_INFO_ARRAY[];
        // attribute information for each attribute

  public:
    // CLASS METHODS
    static int maxSupportedBdexVersion();
        // Return the most current 'bdex' streaming version number supported by
        // this class.  See the 'bdex' package-level documentation for more
        // information on 'bdex' streaming of value-semantic types and
        // containers.

    static const bdeat_AttributeInfo *lookupAttributeInfo(int id);
        // Return attribute information for the attribute indicated by the
        // specified 'id' if the attribute exists, and 0 otherwise.

    static const bdeat_AttributeInfo *lookupAttributeInfo(
                                                    const char *name,
                                                    int         nameLength);
        // Return attribute information for the attribute indicated by the
        // specified 'name' of the specified 'nameLength' if the attribute
        // exists, and 0 otherwise.

    // CREATORS
    explicit MySequenceWithNillables(bslma_Allocator *basicAllocator = 0);
        // Create an object of type 'MySequenceWithNillables' having the
        // default value.  Use the optionally specified 'basicAllocator' to
        // supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.

    MySequenceWithNillables(const MySequenceWithNillables& original,
                            bslma_Allocator *basicAllocator = 0);
        // Create an object of type 'MySequenceWithNillables' having the value
        // of the specified 'original' object.  Use the optionally specified
        // 'basicAllocator' to supply memory.  If 'basicAllocator' is 0, the
        // currently installed default allocator is used.

    ~MySequenceWithNillables();
        // Destroy this object.

    // MANIPULATORS
    MySequenceWithNillables& operator=(const MySequenceWithNillables& rhs);
        // Assign to this object the value of the specified 'rhs' object.

    template <class STREAM>
    STREAM& bdexStreamIn(STREAM& stream, int version);
        // Assign to this object the value read from the specified input
        // 'stream' using the specified 'version' format and return a reference
        // to the modifiable 'stream'.  If 'stream' is initially invalid, this
        // operation has no effect.  If 'stream' becomes invalid during this
        // operation, this object is valid, but its value is undefined.  If
        // 'version' is not supported, 'stream' is marked invalid and this
        // object is unaltered.  Note that no version is read from 'stream'.
        // See the 'bdex' package-level documentation for more information on
        // 'bdex' streaming of value-semantic types and containers.

    void reset();
        // Reset this object to the default value (i.e., its value upon
        // default construction).

    template<class MANIPULATOR>
    int manipulateAttributes(MANIPULATOR& manipulator);
        // Invoke the specified 'manipulator' sequentially on the address of
        // each (modifiable) attribute of this object, supplying 'manipulator'
        // with the corresponding attribute information structure until such
        // invocation returns a non-zero value.  Return the value from the
        // last invocation of 'manipulator' (i.e., the invocation that
        // terminated the sequence).

    template<class MANIPULATOR>
    int manipulateAttribute(MANIPULATOR& manipulator, int id);
        // Invoke the specified 'manipulator' on the address of
        // the (modifiable) attribute indicated by the specified 'id',
        // supplying 'manipulator' with the corresponding attribute
        // information structure.  Return the value returned from the
        // invocation of 'manipulator' if 'id' identifies an attribute of this
        // class, and -1 otherwise.

    template<class MANIPULATOR>
    int manipulateAttribute(MANIPULATOR&  manipulator,
                            const char   *name,
                            int           nameLength);
        // Invoke the specified 'manipulator' on the address of
        // the (modifiable) attribute indicated by the specified 'name' of the
        // specified 'nameLength', supplying 'manipulator' with the
        // corresponding attribute information structure.  Return the value
        // returned from the invocation of 'manipulator' if 'name' identifies
        // an attribute of this class, and -1 otherwise.

    bdeut_NullableValue<int>& attribute1();
        // Return a reference to the modifiable "Attribute1" attribute of this
        // object.

    bdeut_NullableValue<bsl::string>& attribute2();
        // Return a reference to the modifiable "Attribute2" attribute of this
        // object.

    bdeut_NullableValue<MySequence>& attribute3();
        // Return a reference to the modifiable "Attribute3" attribute of this
        // object.

    // ACCESSORS
    bsl::ostream& print(bsl::ostream& stream,
                        int           level = 0,
                        int           spacesPerLevel = 4) const;
        // Format this object to the specified output 'stream' at the
        // optionally specified indentation 'level' and return a reference to
        // the modifiable 'stream'.  If 'level' is specified, optionally
        // specify 'spacesPerLevel', the number of spaces per indentation level
        // for this and all of its nested objects.  Each line is indented by
        // the absolute value of 'level * spacesPerLevel'.  If 'level' is
        // negative, suppress indentation of the first line.  If
        // 'spacesPerLevel' is negative, suppress line breaks and format the
        // entire output on one line.  If 'stream' is initially invalid, this
        // operation has no effect.  Note that a trailing newline is provided
        // in multiline mode only.

    template <class STREAM>
    STREAM& bdexStreamOut(STREAM& stream, int version) const;
        // Write the value of this object to the specified output 'stream'
        // using the specified 'version' format and return a reference to the
        // modifiable 'stream'.  If 'version' is not supported, 'stream' is
        // unmodified.  Note that 'version' is not written to 'stream'.
        // See the 'bdex' package-level documentation for more information
        // on 'bdex' streaming of value-semantic types and containers.

    template<class ACCESSOR>
    int accessAttributes(ACCESSOR& accessor) const;
        // Invoke the specified 'accessor' sequentially on each
        // (non-modifiable) attribute of this object, supplying 'accessor'
        // with the corresponding attribute information structure until such
        // invocation returns a non-zero value.  Return the value from the
        // last invocation of 'accessor' (i.e., the invocation that terminated
        // the sequence).

    template<class ACCESSOR>
    int accessAttribute(ACCESSOR& accessor, int id) const;
        // Invoke the specified 'accessor' on the (non-modifiable) attribute
        // of this object indicated by the specified 'id', supplying 'accessor'
        // with the corresponding attribute information structure.  Return the
        // value returned from the invocation of 'accessor' if 'id' identifies
        // an attribute of this class, and -1 otherwise.

    template<class ACCESSOR>
    int accessAttribute(ACCESSOR&   accessor,
                        const char *name,
                        int         nameLength) const;
        // Invoke the specified 'accessor' on the (non-modifiable) attribute
        // of this object indicated by the specified 'name' of the specified
        // 'nameLength', supplying 'accessor' with the corresponding attribute
        // information structure.  Return the value returned from the
        // invocation of 'accessor' if 'name' identifies an attribute of this
        // class, and -1 otherwise.

    const bdeut_NullableValue<int>& attribute1() const;
        // Return a reference to the non-modifiable "Attribute1" attribute of
        // this object.

    const bdeut_NullableValue<bsl::string>& attribute2() const;
        // Return a reference to the non-modifiable "Attribute2" attribute of
        // this object.

    const bdeut_NullableValue<MySequence>& attribute3() const;
        // Return a reference to the non-modifiable "Attribute3" attribute of
        // this object.
};

// FREE OPERATORS
inline
bool operator==(const MySequenceWithNillables& lhs,
                const MySequenceWithNillables& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects have
    // the same value, and 'false' otherwise.  Two attribute objects have the
    // same value if each respective attribute has the same value.

inline
bool operator!=(const MySequenceWithNillables& lhs,
                const MySequenceWithNillables& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects do not
    // have the same value, and 'false' otherwise.  Two attribute objects do
    // not have the same value if one or more respective attributes differ in
    // values.

inline
bsl::ostream& operator<<(bsl::ostream&                   stream,
                         const MySequenceWithNillables&  rhs);
    // Format the specified 'rhs' to the specified output 'stream' and
    // return a reference to the modifiable 'stream'.

}  // close namespace test

// TRAITS

BDEAT_DECL_SEQUENCE_WITH_ALLOCATOR_TRAITS(test::MySequenceWithNillables)

namespace test {

                               // ==============
                               // class Messages
                               // ==============

struct Messages {
    // This class serves as a place holder to reserve a type having the same
    // name as this component.  Doing so ensures that such a type cannot be
    // defined outside of this component in the current namespace.
};

}  // close namespace test

// ============================================================================
//                      INLINE FUNCTION DEFINITIONS
// ============================================================================

namespace test {

                       // ------------------------------
                       // class MySequenceWithAttributes
                       // ------------------------------

// CLASS METHODS
inline
int MySequenceWithAttributes::maxSupportedBdexVersion()
{
    return 1;  // versions start at 1.
}

// CREATORS
inline
MySequenceWithAttributes::MySequenceWithAttributes(
                bslma_Allocator *basicAllocator)
: d_attribute1()
, d_attribute2(basicAllocator)
, d_element1()
, d_element2(basicAllocator)
{
}

inline
MySequenceWithAttributes::MySequenceWithAttributes(
        const MySequenceWithAttributes& original,
        bslma_Allocator *basicAllocator)
: d_attribute1(original.d_attribute1)
, d_attribute2(original.d_attribute2, basicAllocator)
, d_element1(original.d_element1)
, d_element2(original.d_element2, basicAllocator)
{
}

inline
MySequenceWithAttributes::~MySequenceWithAttributes()
{
}

// MANIPULATORS
inline
MySequenceWithAttributes&
MySequenceWithAttributes::operator=(const MySequenceWithAttributes& rhs)
{
    if (this != &rhs) {
        d_attribute1 = rhs.d_attribute1;
        d_attribute2 = rhs.d_attribute2;
        d_element1 = rhs.d_element1;
        d_element2 = rhs.d_element2;
    }
    return *this;
}

template <class STREAM>
inline
STREAM& MySequenceWithAttributes::bdexStreamIn(STREAM& stream, int version)
{
    if (stream) {
        switch (version) {  // Switch on the schema version (starting with 1).
          case 1: {
            bdex_InStreamFunctions::streamIn(stream, d_attribute1, 1);
            bdex_InStreamFunctions::streamIn(stream, d_attribute2, 1);
            bdex_InStreamFunctions::streamIn(stream, d_element1, 1);
            bdex_InStreamFunctions::streamIn(stream, d_element2, 1);
          } break;
          default: {
            stream.invalidate();
          }
        }
    }
    return stream;
}

inline
void MySequenceWithAttributes::reset()
{
    bdeat_ValueTypeFunctions::reset(&d_attribute1);
    bdeat_ValueTypeFunctions::reset(&d_attribute2);
    bdeat_ValueTypeFunctions::reset(&d_element1);
    bdeat_ValueTypeFunctions::reset(&d_element2);
}

template <class MANIPULATOR>
inline
int MySequenceWithAttributes::manipulateAttributes(MANIPULATOR& manipulator)
{
    int ret;

    ret = manipulator(&d_attribute1,
                      ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE1]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = manipulator(&d_attribute2,
                      ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE2]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = manipulator(&d_element1,
                      ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT1]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = manipulator(&d_element2,
                      ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT2]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    return ret;
}

template <class MANIPULATOR>
inline
int MySequenceWithAttributes::manipulateAttribute(MANIPULATOR& manipulator,
                                                  int          id)
{
    enum { NOT_FOUND = -1 };

    switch (id) {
      case ATTRIBUTE_ID_ATTRIBUTE1: {
        return manipulator(&d_attribute1,
                           ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE1]);
                                                                      // RETURN
      } break;
      case ATTRIBUTE_ID_ATTRIBUTE2: {
        return manipulator(&d_attribute2,
                           ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE2]);
                                                                      // RETURN
      } break;
      case ATTRIBUTE_ID_ELEMENT1: {
        return manipulator(&d_element1,
                           ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT1]);
                                                                      // RETURN
      } break;
      case ATTRIBUTE_ID_ELEMENT2: {
        return manipulator(&d_element2,
                           ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT2]);
                                                                      // RETURN
      } break;
      default:
        return NOT_FOUND;
    }
}

template <class MANIPULATOR>
inline
int MySequenceWithAttributes::manipulateAttribute(
        MANIPULATOR&  manipulator,
        const char   *name,
        int           nameLength)
{
    enum { NOT_FOUND = -1 };

    const bdeat_AttributeInfo *attributeInfo =
           lookupAttributeInfo(name, nameLength);
    if (0 == attributeInfo) {
        return NOT_FOUND;                                             // RETURN
    }

    return manipulateAttribute(manipulator, attributeInfo->d_id);
}

inline
int& MySequenceWithAttributes::attribute1()
{
    return d_attribute1;
}

inline
bdeut_NullableValue<bsl::string>& MySequenceWithAttributes::attribute2()
{
    return d_attribute2;
}

inline
bdeut_NullableValue<int>& MySequenceWithAttributes::element1()
{
    return d_element1;
}

inline
bdeut_NullableValue<bsl::string>& MySequenceWithAttributes::element2()
{
    return d_element2;
}

// ACCESSORS
template <class STREAM>
inline
STREAM& MySequenceWithAttributes::bdexStreamOut(STREAM&  stream,
                                                int      version) const
{
    switch (version) {
      case 1: {
        bdex_OutStreamFunctions::streamOut(stream, d_attribute1, 1);
        bdex_OutStreamFunctions::streamOut(stream, d_attribute2, 1);
        bdex_OutStreamFunctions::streamOut(stream, d_element1, 1);
        bdex_OutStreamFunctions::streamOut(stream, d_element2, 1);
      } break;
    }
    return stream;
}

template <class ACCESSOR>
inline
int MySequenceWithAttributes::accessAttributes(ACCESSOR& accessor) const
{
    int ret;

    ret = accessor(d_attribute1,
                   ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE1]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = accessor(d_attribute2,
                   ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE2]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = accessor(d_element1, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT1]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = accessor(d_element2, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT2]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    return ret;
}

template <class ACCESSOR>
inline
int MySequenceWithAttributes::accessAttribute(ACCESSOR& accessor, int id) const
{
    enum { NOT_FOUND = -1 };

    switch (id) {
      case ATTRIBUTE_ID_ATTRIBUTE1: {
        return accessor(d_attribute1,
                        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE1]);
                                                                      // RETURN
      } break;
      case ATTRIBUTE_ID_ATTRIBUTE2: {
        return accessor(d_attribute2,
                        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE2]);
                                                                      // RETURN
      } break;
      case ATTRIBUTE_ID_ELEMENT1: {
        return accessor(d_element1,
                        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT1]);
                                                                      // RETURN
      } break;
      case ATTRIBUTE_ID_ELEMENT2: {
        return accessor(d_element2,
                        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT2]);
                                                                      // RETURN
      } break;
      default:
        return NOT_FOUND;
    }
}

template <class ACCESSOR>
inline
int MySequenceWithAttributes::accessAttribute(
        ACCESSOR&   accessor,
        const char *name,
        int         nameLength) const
{
    enum { NOT_FOUND = -1 };

    const bdeat_AttributeInfo *attributeInfo =
          lookupAttributeInfo(name, nameLength);
    if (0 == attributeInfo) {
       return NOT_FOUND;                                              // RETURN
    }

    return accessAttribute(accessor, attributeInfo->d_id);
}

inline
const int& MySequenceWithAttributes::attribute1() const
{
    return d_attribute1;
}

inline
const bdeut_NullableValue<bsl::string>&
                                   MySequenceWithAttributes::attribute2() const
{
    return d_attribute2;
}

inline
const bdeut_NullableValue<int>& MySequenceWithAttributes::element1() const
{
    return d_element1;
}

inline
const bdeut_NullableValue<bsl::string>&
                                     MySequenceWithAttributes::element2() const
{
    return d_element2;
}

                               // -------------
                               // class Address
                               // -------------

// CLASS METHODS
inline
int Address::maxSupportedBdexVersion()
{
    return 1;  // versions start at 1.
}

// CREATORS
inline
Address::Address(bslma_Allocator *basicAllocator)
: d_street(basicAllocator)
, d_city(basicAllocator)
, d_state(basicAllocator)
{
}

inline
Address::Address(
        const Address& original,
        bslma_Allocator *basicAllocator)
: d_street(original.d_street, basicAllocator)
, d_city(original.d_city, basicAllocator)
, d_state(original.d_state, basicAllocator)
{
}

inline
Address::~Address()
{
}

// MANIPULATORS
inline
Address&
Address::operator=(const Address& rhs)
{
    if (this != &rhs) {
        d_street = rhs.d_street;
        d_city = rhs.d_city;
        d_state = rhs.d_state;
    }
    return *this;
}

template <class STREAM>
inline
STREAM& Address::bdexStreamIn(STREAM& stream, int version)
{
    if (stream) {
        switch (version) {  // Switch on the schema version (starting with 1).
          case 1: {
            bdex_InStreamFunctions::streamIn(stream, d_street, 1);
            bdex_InStreamFunctions::streamIn(stream, d_city, 1);
            bdex_InStreamFunctions::streamIn(stream, d_state, 1);
          } break;
          default: {
            stream.invalidate();
          }
        }
    }
    return stream;
}

inline
void Address::reset()
{
    bdeat_ValueTypeFunctions::reset(&d_street);
    bdeat_ValueTypeFunctions::reset(&d_city);
    bdeat_ValueTypeFunctions::reset(&d_state);
}

template <class MANIPULATOR>
inline
int Address::manipulateAttributes(MANIPULATOR& manipulator)
{
    int ret;

    ret = manipulator(&d_street, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_STREET]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = manipulator(&d_city, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CITY]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = manipulator(&d_state, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_STATE]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    return ret;
}

template <class MANIPULATOR>
inline
int Address::manipulateAttribute(MANIPULATOR& manipulator, int id)
{
    enum { NOT_FOUND = -1 };

    switch (id) {
      case ATTRIBUTE_ID_STREET: {
        return manipulator(&d_street,
                           ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_STREET]);
                                                                      // RETURN
      } break;
      case ATTRIBUTE_ID_CITY: {
        return manipulator(&d_city,
                           ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CITY]);
                                                                      // RETURN
      } break;
      case ATTRIBUTE_ID_STATE: {
        return manipulator(&d_state,
                           ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_STATE]);
                                                                      // RETURN
      } break;
      default:
        return NOT_FOUND;
    }
}

template <class MANIPULATOR>
inline
int Address::manipulateAttribute(
        MANIPULATOR&  manipulator,
        const char   *name,
        int           nameLength)
{
    enum { NOT_FOUND = -1 };

    const bdeat_AttributeInfo *attributeInfo =
           lookupAttributeInfo(name, nameLength);
    if (0 == attributeInfo) {
        return NOT_FOUND;                                             // RETURN
    }

    return manipulateAttribute(manipulator, attributeInfo->d_id);
}

inline
bsl::string& Address::street()
{
    return d_street;
}

inline
bsl::string& Address::city()
{
    return d_city;
}

inline
bsl::string& Address::state()
{
    return d_state;
}

// ACCESSORS
template <class STREAM>
inline
STREAM& Address::bdexStreamOut(STREAM& stream, int version) const
{
    switch (version) {
      case 1: {
        bdex_OutStreamFunctions::streamOut(stream, d_street, 1);
        bdex_OutStreamFunctions::streamOut(stream, d_city, 1);
        bdex_OutStreamFunctions::streamOut(stream, d_state, 1);
      } break;
    }
    return stream;
}

template <class ACCESSOR>
inline
int Address::accessAttributes(ACCESSOR& accessor) const
{
    int ret;

    ret = accessor(d_street, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_STREET]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = accessor(d_city, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CITY]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = accessor(d_state, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_STATE]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    return ret;
}

template <class ACCESSOR>
inline
int Address::accessAttribute(ACCESSOR& accessor, int id) const
{
    enum { NOT_FOUND = -1 };

    switch (id) {
      case ATTRIBUTE_ID_STREET: {
        return accessor(d_street,
                        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_STREET]);
                                                                      // RETURN
      } break;
      case ATTRIBUTE_ID_CITY: {
        return accessor(d_city, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CITY]);
                                                                      // RETURN
      } break;
      case ATTRIBUTE_ID_STATE: {
        return accessor(d_state, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_STATE]);
                                                                      // RETURN
      } break;
      default:
        return NOT_FOUND;
    }
}

template <class ACCESSOR>
inline
int Address::accessAttribute(
        ACCESSOR&   accessor,
        const char *name,
        int         nameLength) const
{
    enum { NOT_FOUND = -1 };

    const bdeat_AttributeInfo *attributeInfo =
          lookupAttributeInfo(name, nameLength);
    if (0 == attributeInfo) {
       return NOT_FOUND;                                              // RETURN
    }

    return accessAttribute(accessor, attributeInfo->d_id);
}

inline
const bsl::string& Address::street() const
{
    return d_street;
}

inline
const bsl::string& Address::city() const
{
    return d_city;
}

inline
const bsl::string& Address::state() const
{
    return d_state;
}

                              // ----------------
                              // class MySequence
                              // ----------------

// CLASS METHODS
inline
int MySequence::maxSupportedBdexVersion()
{
    return 1;  // versions start at 1.
}

// CREATORS
inline
MySequence::MySequence(bslma_Allocator *basicAllocator)
: d_attribute1()
, d_attribute2(basicAllocator)
{
}

inline
MySequence::MySequence(
        const MySequence& original,
        bslma_Allocator *basicAllocator)
: d_attribute1(original.d_attribute1)
, d_attribute2(original.d_attribute2, basicAllocator)
{
}

inline
MySequence::~MySequence()
{
}

// MANIPULATORS
inline
MySequence&
MySequence::operator=(const MySequence& rhs)
{
    if (this != &rhs) {
        d_attribute1 = rhs.d_attribute1;
        d_attribute2 = rhs.d_attribute2;
    }
    return *this;
}

template <class STREAM>
inline
STREAM& MySequence::bdexStreamIn(STREAM& stream, int version)
{
    if (stream) {
        switch (version) {  // Switch on the schema version (starting with 1).
          case 1: {
            bdex_InStreamFunctions::streamIn(stream, d_attribute1, 1);
            bdex_InStreamFunctions::streamIn(stream, d_attribute2, 1);
          } break;
          default: {
            stream.invalidate();
          }
        }
    }
    return stream;
}

inline
void MySequence::reset()
{
    bdeat_ValueTypeFunctions::reset(&d_attribute1);
    bdeat_ValueTypeFunctions::reset(&d_attribute2);
}

template <class MANIPULATOR>
inline
int MySequence::manipulateAttributes(MANIPULATOR& manipulator)
{
    int ret;

    ret = manipulator(&d_attribute1,
                      ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE1]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = manipulator(&d_attribute2,
                      ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE2]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    return ret;
}

template <class MANIPULATOR>
inline
int MySequence::manipulateAttribute(MANIPULATOR& manipulator, int id)
{
    enum { NOT_FOUND = -1 };

    switch (id) {
      case ATTRIBUTE_ID_ATTRIBUTE1: {
        return manipulator(&d_attribute1,
                           ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE1]);
                                                                      // RETURN
      } break;
      case ATTRIBUTE_ID_ATTRIBUTE2: {
        return manipulator(&d_attribute2,
                           ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE2]);
                                                                      // RETURN
      } break;
      default:
        return NOT_FOUND;
    }
}

template <class MANIPULATOR>
inline
int MySequence::manipulateAttribute(
        MANIPULATOR&  manipulator,
        const char   *name,
        int           nameLength)
{
    enum { NOT_FOUND = -1 };

    const bdeat_AttributeInfo *attributeInfo =
           lookupAttributeInfo(name, nameLength);
    if (0 == attributeInfo) {
        return NOT_FOUND;                                             // RETURN
    }

    return manipulateAttribute(manipulator, attributeInfo->d_id);
}

inline
int& MySequence::attribute1()
{
    return d_attribute1;
}

inline
bsl::string& MySequence::attribute2()
{
    return d_attribute2;
}

// ACCESSORS
template <class STREAM>
inline
STREAM& MySequence::bdexStreamOut(STREAM& stream, int version) const
{
    switch (version) {
      case 1: {
        bdex_OutStreamFunctions::streamOut(stream, d_attribute1, 1);
        bdex_OutStreamFunctions::streamOut(stream, d_attribute2, 1);
      } break;
    }
    return stream;
}

template <class ACCESSOR>
inline
int MySequence::accessAttributes(ACCESSOR& accessor) const
{
    int ret;

    ret = accessor(d_attribute1,
                   ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE1]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = accessor(d_attribute2,
                   ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE2]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    return ret;
}

template <class ACCESSOR>
inline
int MySequence::accessAttribute(ACCESSOR& accessor, int id) const
{
    enum { NOT_FOUND = -1 };

    switch (id) {
      case ATTRIBUTE_ID_ATTRIBUTE1: {
        return accessor(d_attribute1,
                        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE1]);
                                                                      // RETURN
      } break;
      case ATTRIBUTE_ID_ATTRIBUTE2: {
        return accessor(d_attribute2,
                        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE2]);
                                                                      // RETURN
      } break;
      default:
        return NOT_FOUND;
    }
}

template <class ACCESSOR>
inline
int MySequence::accessAttribute(
        ACCESSOR&   accessor,
        const char *name,
        int         nameLength) const
{
    enum { NOT_FOUND = -1 };

    const bdeat_AttributeInfo *attributeInfo =
          lookupAttributeInfo(name, nameLength);
    if (0 == attributeInfo) {
       return NOT_FOUND;                                              // RETURN
    }

    return accessAttribute(accessor, attributeInfo->d_id);
}

inline
const int& MySequence::attribute1() const
{
    return d_attribute1;
}

inline
const bsl::string& MySequence::attribute2() const
{
    return d_attribute2;
}

                           // ---------------------
                           // class MySimpleContent
                           // ---------------------

// CLASS METHODS
inline
int MySimpleContent::maxSupportedBdexVersion()
{
    return 1;  // versions start at 1.
}

// CREATORS
inline
MySimpleContent::MySimpleContent(bslma_Allocator *basicAllocator)
: d_attribute1()
, d_attribute2(basicAllocator)
, d_theContent(basicAllocator)
{
}

inline
MySimpleContent::MySimpleContent(
        const MySimpleContent& original,
        bslma_Allocator *basicAllocator)
: d_attribute1(original.d_attribute1)
, d_attribute2(original.d_attribute2, basicAllocator)
, d_theContent(original.d_theContent, basicAllocator)
{
}

inline
MySimpleContent::~MySimpleContent()
{
}

// MANIPULATORS
inline
MySimpleContent&
MySimpleContent::operator=(const MySimpleContent& rhs)
{
    if (this != &rhs) {
        d_attribute1 = rhs.d_attribute1;
        d_attribute2 = rhs.d_attribute2;
        d_theContent = rhs.d_theContent;
    }
    return *this;
}

template <class STREAM>
inline
STREAM& MySimpleContent::bdexStreamIn(STREAM& stream, int version)
{
    if (stream) {
        switch (version) {  // Switch on the schema version (starting with 1).
          case 1: {
            bdex_InStreamFunctions::streamIn(stream, d_attribute1, 1);
            bdex_InStreamFunctions::streamIn(stream, d_attribute2, 1);
            bdex_InStreamFunctions::streamIn(stream, d_theContent, 1);
          } break;
          default: {
            stream.invalidate();
          }
        }
    }
    return stream;
}

inline
void MySimpleContent::reset()
{
    bdeat_ValueTypeFunctions::reset(&d_attribute1);
    bdeat_ValueTypeFunctions::reset(&d_attribute2);
    bdeat_ValueTypeFunctions::reset(&d_theContent);
}

template <class MANIPULATOR>
inline
int MySimpleContent::manipulateAttributes(MANIPULATOR& manipulator)
{
    int ret;

    ret = manipulator(&d_attribute1,
                      ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE1]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = manipulator(&d_attribute2,
                      ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE2]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = manipulator(&d_theContent,
                      ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_THE_CONTENT]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    return ret;
}

template <class MANIPULATOR>
inline
int MySimpleContent::manipulateAttribute(MANIPULATOR& manipulator, int id)
{
    enum { NOT_FOUND = -1 };

    switch (id) {
      case ATTRIBUTE_ID_ATTRIBUTE1: {
        return manipulator(&d_attribute1,
                           ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE1]);
                                                                      // RETURN
      } break;
      case ATTRIBUTE_ID_ATTRIBUTE2: {
        return manipulator(&d_attribute2,
                           ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE2]);
                                                                      // RETURN
      } break;
      case ATTRIBUTE_ID_THE_CONTENT: {
        return manipulator(&d_theContent,
                           ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_THE_CONTENT]);
                                                                      // RETURN
      } break;
      default:
        return NOT_FOUND;
    }
}

template <class MANIPULATOR>
inline
int MySimpleContent::manipulateAttribute(
        MANIPULATOR&  manipulator,
        const char   *name,
        int           nameLength)
{
    enum { NOT_FOUND = -1 };

    const bdeat_AttributeInfo *attributeInfo =
           lookupAttributeInfo(name, nameLength);
    if (0 == attributeInfo) {
        return NOT_FOUND;                                             // RETURN
    }

    return manipulateAttribute(manipulator, attributeInfo->d_id);
}

inline
bdeut_NullableValue<bool>& MySimpleContent::attribute1()
{
    return d_attribute1;
}

inline
bdeut_NullableValue<bsl::string>& MySimpleContent::attribute2()
{
    return d_attribute2;
}

inline
bsl::string& MySimpleContent::theContent()
{
    return d_theContent;
}

// ACCESSORS
template <class STREAM>
inline
STREAM& MySimpleContent::bdexStreamOut(STREAM& stream, int version) const
{
    switch (version) {
      case 1: {
        bdex_OutStreamFunctions::streamOut(stream, d_attribute1, 1);
        bdex_OutStreamFunctions::streamOut(stream, d_attribute2, 1);
        bdex_OutStreamFunctions::streamOut(stream, d_theContent, 1);
      } break;
    }
    return stream;
}

template <class ACCESSOR>
inline
int MySimpleContent::accessAttributes(ACCESSOR& accessor) const
{
    int ret;

    ret = accessor(d_attribute1,
                   ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE1]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = accessor(d_attribute2,
                   ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE2]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = accessor(d_theContent,
                   ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_THE_CONTENT]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    return ret;
}

template <class ACCESSOR>
inline
int MySimpleContent::accessAttribute(ACCESSOR& accessor, int id) const
{
    enum { NOT_FOUND = -1 };

    switch (id) {
      case ATTRIBUTE_ID_ATTRIBUTE1: {
        return accessor(d_attribute1,
                        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE1]);
                                                                      // RETURN
      } break;
      case ATTRIBUTE_ID_ATTRIBUTE2: {
        return accessor(d_attribute2,
                        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE2]);
                                                                      // RETURN
      } break;
      case ATTRIBUTE_ID_THE_CONTENT: {
        return accessor(d_theContent,
                        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_THE_CONTENT]);
                                                                      // RETURN
      } break;
      default:
        return NOT_FOUND;
    }
}

template <class ACCESSOR>
inline
int MySimpleContent::accessAttribute(
        ACCESSOR&   accessor,
        const char *name,
        int         nameLength) const
{
    enum { NOT_FOUND = -1 };

    const bdeat_AttributeInfo *attributeInfo =
          lookupAttributeInfo(name, nameLength);
    if (0 == attributeInfo) {
       return NOT_FOUND;                                              // RETURN
    }

    return accessAttribute(accessor, attributeInfo->d_id);
}

inline
const bdeut_NullableValue<bool>& MySimpleContent::attribute1() const
{
    return d_attribute1;
}

inline
const bdeut_NullableValue<bsl::string>& MySimpleContent::attribute2() const
{
    return d_attribute2;
}

inline
const bsl::string& MySimpleContent::theContent() const
{
    return d_theContent;
}

                          // ------------------------
                          // class MySimpleIntContent
                          // ------------------------

// CLASS METHODS
inline
int MySimpleIntContent::maxSupportedBdexVersion()
{
    return 1;  // versions start at 1.
}

// CREATORS
inline
MySimpleIntContent::MySimpleIntContent(bslma_Allocator *basicAllocator)
: d_attribute1()
, d_attribute2(basicAllocator)
, d_theContent()
{
}

inline
MySimpleIntContent::MySimpleIntContent(
        const MySimpleIntContent& original,
        bslma_Allocator *basicAllocator)
: d_attribute1(original.d_attribute1)
, d_attribute2(original.d_attribute2, basicAllocator)
, d_theContent(original.d_theContent)
{
}

inline
MySimpleIntContent::~MySimpleIntContent()
{
}

// MANIPULATORS
inline
MySimpleIntContent&
MySimpleIntContent::operator=(const MySimpleIntContent& rhs)
{
    if (this != &rhs) {
        d_attribute1 = rhs.d_attribute1;
        d_attribute2 = rhs.d_attribute2;
        d_theContent = rhs.d_theContent;
    }
    return *this;
}

template <class STREAM>
inline
STREAM& MySimpleIntContent::bdexStreamIn(STREAM& stream, int version)
{
    if (stream) {
        switch (version) {  // Switch on the schema version (starting with 1).
          case 1: {
            bdex_InStreamFunctions::streamIn(stream, d_attribute1, 1);
            bdex_InStreamFunctions::streamIn(stream, d_attribute2, 1);
            bdex_InStreamFunctions::streamIn(stream, d_theContent, 1);
          } break;
          default: {
            stream.invalidate();
          }
        }
    }
    return stream;
}

inline
void MySimpleIntContent::reset()
{
    bdeat_ValueTypeFunctions::reset(&d_attribute1);
    bdeat_ValueTypeFunctions::reset(&d_attribute2);
    bdeat_ValueTypeFunctions::reset(&d_theContent);
}

template <class MANIPULATOR>
inline
int MySimpleIntContent::manipulateAttributes(MANIPULATOR& manipulator)
{
    int ret;

    ret = manipulator(&d_attribute1,
                      ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE1]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = manipulator(&d_attribute2,
                      ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE2]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = manipulator(&d_theContent,
                      ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_THE_CONTENT]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    return ret;
}

template <class MANIPULATOR>
inline
int MySimpleIntContent::manipulateAttribute(MANIPULATOR& manipulator, int id)
{
    enum { NOT_FOUND = -1 };

    switch (id) {
      case ATTRIBUTE_ID_ATTRIBUTE1: {
        return manipulator(&d_attribute1,
                           ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE1]);
                                                                      // RETURN
      } break;
      case ATTRIBUTE_ID_ATTRIBUTE2: {
        return manipulator(&d_attribute2,
                           ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE2]);
                                                                      // RETURN
      } break;
      case ATTRIBUTE_ID_THE_CONTENT: {
        return manipulator(&d_theContent,
                           ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_THE_CONTENT]);
                                                                      // RETURN
      } break;
      default:
        return NOT_FOUND;
    }
}

template <class MANIPULATOR>
inline
int MySimpleIntContent::manipulateAttribute(
        MANIPULATOR&  manipulator,
        const char   *name,
        int           nameLength)
{
    enum { NOT_FOUND = -1 };

    const bdeat_AttributeInfo *attributeInfo =
           lookupAttributeInfo(name, nameLength);
    if (0 == attributeInfo) {
        return NOT_FOUND;                                             // RETURN
    }

    return manipulateAttribute(manipulator, attributeInfo->d_id);
}

inline
bdeut_NullableValue<bool>& MySimpleIntContent::attribute1()
{
    return d_attribute1;
}

inline
bdeut_NullableValue<bsl::string>& MySimpleIntContent::attribute2()
{
    return d_attribute2;
}

inline
int& MySimpleIntContent::theContent()
{
    return d_theContent;
}

// ACCESSORS
template <class STREAM>
inline
STREAM& MySimpleIntContent::bdexStreamOut(STREAM& stream, int version) const
{
    switch (version) {
      case 1: {
        bdex_OutStreamFunctions::streamOut(stream, d_attribute1, 1);
        bdex_OutStreamFunctions::streamOut(stream, d_attribute2, 1);
        bdex_OutStreamFunctions::streamOut(stream, d_theContent, 1);
      } break;
    }
    return stream;
}

template <class ACCESSOR>
inline
int MySimpleIntContent::accessAttributes(ACCESSOR& accessor) const
{
    int ret;

    ret = accessor(d_attribute1,
                   ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE1]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = accessor(d_attribute2,
                   ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE2]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = accessor(d_theContent,
                   ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_THE_CONTENT]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    return ret;
}

template <class ACCESSOR>
inline
int MySimpleIntContent::accessAttribute(ACCESSOR& accessor, int id) const
{
    enum { NOT_FOUND = -1 };

    switch (id) {
      case ATTRIBUTE_ID_ATTRIBUTE1: {
        return accessor(d_attribute1,
                        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE1]);
                                                                      // RETURN
      } break;
      case ATTRIBUTE_ID_ATTRIBUTE2: {
        return accessor(d_attribute2,
                        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE2]);
                                                                      // RETURN
      } break;
      case ATTRIBUTE_ID_THE_CONTENT: {
        return accessor(d_theContent,
                        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_THE_CONTENT]);
                                                                      // RETURN
      } break;
      default:
        return NOT_FOUND;
    }
}

template <class ACCESSOR>
inline
int MySimpleIntContent::accessAttribute(
        ACCESSOR&   accessor,
        const char *name,
        int         nameLength) const
{
    enum { NOT_FOUND = -1 };

    const bdeat_AttributeInfo *attributeInfo =
          lookupAttributeInfo(name, nameLength);
    if (0 == attributeInfo) {
       return NOT_FOUND;                                              // RETURN
    }

    return accessAttribute(accessor, attributeInfo->d_id);
}

inline
const bdeut_NullableValue<bool>& MySimpleIntContent::attribute1() const
{
    return d_attribute1;
}

inline
const bdeut_NullableValue<bsl::string>& MySimpleIntContent::attribute2() const
{
    return d_attribute2;
}

inline
const int& MySimpleIntContent::theContent() const
{
    return d_theContent;
}

                 // -----------------------------------------
                 // class MySequenceWithAnonymousChoiceChoice
                 // -----------------------------------------

// CLASS METHODS
inline
int MySequenceWithAnonymousChoiceChoice::maxSupportedBdexVersion()
{
    return 1;  // versions start at 1.
}

// CREATORS
inline
MySequenceWithAnonymousChoiceChoice::MySequenceWithAnonymousChoiceChoice(
                bslma_Allocator *basicAllocator)
: d_selectionId(SELECTION_ID_UNDEFINED)
, d_allocator_p(bslma_Default::allocator(basicAllocator))
{
}

inline
MySequenceWithAnonymousChoiceChoice::MySequenceWithAnonymousChoiceChoice(
    const MySequenceWithAnonymousChoiceChoice& original,
    bslma_Allocator *basicAllocator)
: d_selectionId(original.d_selectionId)
, d_allocator_p(bslma_Default::allocator(basicAllocator))
{
    switch (d_selectionId) {
      case SELECTION_ID_MY_CHOICE1: {
        new (d_myChoice1.buffer())
            int(original.d_myChoice1.object());
      } break;
      case SELECTION_ID_MY_CHOICE2: {
        new (d_myChoice2.buffer())
            bsl::string(
                original.d_myChoice2.object(), d_allocator_p);
      } break;
      default:
        BSLS_ASSERT_SAFE(SELECTION_ID_UNDEFINED == d_selectionId);
    }
}

inline
MySequenceWithAnonymousChoiceChoice::~MySequenceWithAnonymousChoiceChoice()
{
    reset();
}

// MANIPULATORS
inline
MySequenceWithAnonymousChoiceChoice&
MySequenceWithAnonymousChoiceChoice::operator=(
                const MySequenceWithAnonymousChoiceChoice& rhs)
{
    if (this != &rhs) {
        switch (rhs.d_selectionId) {
          case SELECTION_ID_MY_CHOICE1: {
            makeMyChoice1(rhs.d_myChoice1.object());
          } break;
          case SELECTION_ID_MY_CHOICE2: {
            makeMyChoice2(rhs.d_myChoice2.object());
          } break;
          default:
            BSLS_ASSERT_SAFE(SELECTION_ID_UNDEFINED == rhs.d_selectionId);
            reset();
        }
    }
    return *this;
}

template <class STREAM>
inline
STREAM& MySequenceWithAnonymousChoiceChoice::bdexStreamIn(
                STREAM&     stream,
                int         version)
{
    if (stream) {
        switch (version) {  // Switch on the schema version (starting with 1).
          case 1: {
            short selectionId;
            stream.getInt16(selectionId);
            if (!stream) {
                return stream;                                        // RETURN
            }
            switch (selectionId) {
              case SELECTION_ID_MY_CHOICE1: {
                makeMyChoice1();
                bdex_InStreamFunctions::streamIn(
                    stream, d_myChoice1.object(), 1);
              } break;
              case SELECTION_ID_MY_CHOICE2: {
                makeMyChoice2();
                bdex_InStreamFunctions::streamIn(
                    stream, d_myChoice2.object(), 1);
              } break;
              case SELECTION_ID_UNDEFINED: {
                reset();
              } break;
              default:
                stream.invalidate();
            }
          } break;
          default: {
            stream.invalidate();
          }
        }
    }
    return stream;
}

inline
void MySequenceWithAnonymousChoiceChoice::reset()
{
    switch (d_selectionId) {
      case SELECTION_ID_MY_CHOICE1: {
        // no destruction required
      } break;
      case SELECTION_ID_MY_CHOICE2: {
        typedef bsl::string Type;
        d_myChoice2.object().~Type();
      } break;
      default:
        BSLS_ASSERT_SAFE(SELECTION_ID_UNDEFINED == d_selectionId);
    }

    d_selectionId = SELECTION_ID_UNDEFINED;
}

inline
int MySequenceWithAnonymousChoiceChoice::makeSelection(int selectionId)
{
    enum { NOT_FOUND = -1, SUCCESS = 0 };

    switch (selectionId) {
      case SELECTION_ID_MY_CHOICE1: {
        makeMyChoice1();
      } break;
      case SELECTION_ID_MY_CHOICE2: {
        makeMyChoice2();
      } break;
      case SELECTION_ID_UNDEFINED: {
        reset();
      } break;
      default:
        return NOT_FOUND;                                             // RETURN
    }
    return SUCCESS;
}

inline
int MySequenceWithAnonymousChoiceChoice::makeSelection(
                const char  *name,
                int          nameLength)
{
    enum { NOT_FOUND = -1 };

    const bdeat_SelectionInfo *selectionInfo =
           lookupSelectionInfo(name, nameLength);
    if (0 == selectionInfo) {
       return NOT_FOUND;                                              // RETURN
    }

    return makeSelection(selectionInfo->d_id);
}

inline
int& MySequenceWithAnonymousChoiceChoice::makeMyChoice1()
{
    if (SELECTION_ID_MY_CHOICE1 == d_selectionId) {
        bdeat_ValueTypeFunctions::reset(&d_myChoice1.object());
    }
    else {
        reset();
        new (d_myChoice1.buffer())
            int();

        d_selectionId = SELECTION_ID_MY_CHOICE1;
    }

    return d_myChoice1.object();
}

inline
int& MySequenceWithAnonymousChoiceChoice::makeMyChoice1(int value)
{
    if (SELECTION_ID_MY_CHOICE1 == d_selectionId) {
        d_myChoice1.object() = value;
    }
    else {
        reset();
        new (d_myChoice1.buffer())
                int(value);
        d_selectionId = SELECTION_ID_MY_CHOICE1;
    }

    return d_myChoice1.object();
}

inline
bsl::string& MySequenceWithAnonymousChoiceChoice::makeMyChoice2()
{
    if (SELECTION_ID_MY_CHOICE2 == d_selectionId) {
        bdeat_ValueTypeFunctions::reset(&d_myChoice2.object());
    }
    else {
        reset();
        new (d_myChoice2.buffer())
                bsl::string(d_allocator_p);

        d_selectionId = SELECTION_ID_MY_CHOICE2;
    }

    return d_myChoice2.object();
}

inline
bsl::string& MySequenceWithAnonymousChoiceChoice::makeMyChoice2(
                const bsl::string& value)
{
    if (SELECTION_ID_MY_CHOICE2 == d_selectionId) {
        d_myChoice2.object() = value;
    }
    else {
        reset();
        new (d_myChoice2.buffer())
                bsl::string(value, d_allocator_p);
        d_selectionId = SELECTION_ID_MY_CHOICE2;
    }

    return d_myChoice2.object();
}

template <class MANIPULATOR>
inline
int MySequenceWithAnonymousChoiceChoice::manipulateSelection(
                MANIPULATOR& manipulator)
{
    enum { FAILURE = -1, SUCCESS = 0 };

    switch (d_selectionId) {
      case MySequenceWithAnonymousChoiceChoice::SELECTION_ID_MY_CHOICE1:
        return manipulator(&d_myChoice1.object(),
                SELECTION_INFO_ARRAY[SELECTION_INDEX_MY_CHOICE1]);    // RETURN
      case MySequenceWithAnonymousChoiceChoice::SELECTION_ID_MY_CHOICE2:
        return manipulator(&d_myChoice2.object(),
                SELECTION_INFO_ARRAY[SELECTION_INDEX_MY_CHOICE2]);    // RETURN
      default:
        BSLS_ASSERT_SAFE(
                MySequenceWithAnonymousChoiceChoice::SELECTION_ID_UNDEFINED
                == d_selectionId);
        return FAILURE;
    }
}

inline
int& MySequenceWithAnonymousChoiceChoice::myChoice1()
{
    BSLS_ASSERT_SAFE(SELECTION_ID_MY_CHOICE1 == d_selectionId);
    return d_myChoice1.object();
}

inline
bsl::string& MySequenceWithAnonymousChoiceChoice::myChoice2()
{
    BSLS_ASSERT_SAFE(SELECTION_ID_MY_CHOICE2 == d_selectionId);
    return d_myChoice2.object();
}

// ACCESSORS
template <class STREAM>
inline
STREAM& MySequenceWithAnonymousChoiceChoice::bdexStreamOut(
                STREAM&     stream,
                int         version) const
{
    switch (version) {
      case 1: {
            stream.putInt16(d_selectionId);
            switch (d_selectionId) {
              case SELECTION_ID_MY_CHOICE1: {
                bdex_OutStreamFunctions::streamOut(
                    stream, d_myChoice1.object(), 1);
              } break;
              case SELECTION_ID_MY_CHOICE2: {
                bdex_OutStreamFunctions::streamOut(
                    stream, d_myChoice2.object(), 1);
              } break;
              default:
                BSLS_ASSERT_SAFE(SELECTION_ID_UNDEFINED == d_selectionId);
            }
      } break;
    }
    return stream;
}

inline
int MySequenceWithAnonymousChoiceChoice::selectionId() const
{
    return d_selectionId;
}

template <class ACCESSOR>
inline
int MySequenceWithAnonymousChoiceChoice::accessSelection(
                ACCESSOR& accessor) const
{
    enum { FAILURE = -1, SUCCESS = 0 };

    switch (d_selectionId) {
      case SELECTION_ID_MY_CHOICE1:
        return accessor(d_myChoice1.object(),
                SELECTION_INFO_ARRAY[SELECTION_INDEX_MY_CHOICE1]);    // RETURN
      case SELECTION_ID_MY_CHOICE2:
        return accessor(d_myChoice2.object(),
                SELECTION_INFO_ARRAY[SELECTION_INDEX_MY_CHOICE2]);    // RETURN
      default:
        BSLS_ASSERT_SAFE(SELECTION_ID_UNDEFINED == d_selectionId);
        return FAILURE;
    }
}

inline
const int& MySequenceWithAnonymousChoiceChoice::myChoice1() const
{
    BSLS_ASSERT_SAFE(SELECTION_ID_MY_CHOICE1 == d_selectionId);
    return d_myChoice1.object();
}

inline
const bsl::string& MySequenceWithAnonymousChoiceChoice::myChoice2() const
{
    BSLS_ASSERT_SAFE(SELECTION_ID_MY_CHOICE2 == d_selectionId);
    return d_myChoice2.object();
}

inline
bool MySequenceWithAnonymousChoiceChoice::isMyChoice1Value() const
{
    return SELECTION_ID_MY_CHOICE1 == d_selectionId;
}

inline
bool MySequenceWithAnonymousChoiceChoice::isMyChoice2Value() const
{
    return SELECTION_ID_MY_CHOICE2 == d_selectionId;
}

inline
bool MySequenceWithAnonymousChoiceChoice::isUndefinedValue() const
{
    return SELECTION_ID_UNDEFINED == d_selectionId;
}

                       // -----------------------------
                       // class MySequenceWithNullables
                       // -----------------------------

// CLASS METHODS
inline
int MySequenceWithNullables::maxSupportedBdexVersion()
{
    return 1;  // versions start at 1.
}

// CREATORS
inline
MySequenceWithNullables::MySequenceWithNullables(
                bslma_Allocator *basicAllocator)
: d_attribute1()
, d_attribute2(basicAllocator)
, d_attribute3(basicAllocator)
{
}

inline
MySequenceWithNullables::MySequenceWithNullables(
        const MySequenceWithNullables& original,
        bslma_Allocator *basicAllocator)
: d_attribute1(original.d_attribute1)
, d_attribute2(original.d_attribute2, basicAllocator)
, d_attribute3(original.d_attribute3, basicAllocator)
{
}

inline
MySequenceWithNullables::~MySequenceWithNullables()
{
}

// MANIPULATORS
inline
MySequenceWithNullables&
MySequenceWithNullables::operator=(const MySequenceWithNullables& rhs)
{
    if (this != &rhs) {
        d_attribute1 = rhs.d_attribute1;
        d_attribute2 = rhs.d_attribute2;
        d_attribute3 = rhs.d_attribute3;
    }
    return *this;
}

template <class STREAM>
inline
STREAM& MySequenceWithNullables::bdexStreamIn(STREAM& stream, int version)
{
    if (stream) {
        switch (version) {  // Switch on the schema version (starting with 1).
          case 1: {
            bdex_InStreamFunctions::streamIn(stream, d_attribute1, 1);
            bdex_InStreamFunctions::streamIn(stream, d_attribute2, 1);
            bdex_InStreamFunctions::streamIn(stream, d_attribute3, 1);
          } break;
          default: {
            stream.invalidate();
          }
        }
    }
    return stream;
}

inline
void MySequenceWithNullables::reset()
{
    bdeat_ValueTypeFunctions::reset(&d_attribute1);
    bdeat_ValueTypeFunctions::reset(&d_attribute2);
    bdeat_ValueTypeFunctions::reset(&d_attribute3);
}

template <class MANIPULATOR>
inline
int MySequenceWithNullables::manipulateAttributes(MANIPULATOR& manipulator)
{
    int ret;

    ret = manipulator(&d_attribute1,
                      ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE1]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = manipulator(&d_attribute2,
                      ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE2]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = manipulator(&d_attribute3,
                      ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE3]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    return ret;
}

template <class MANIPULATOR>
inline
int MySequenceWithNullables::manipulateAttribute(MANIPULATOR&  manipulator,
                                                 int           id)
{
    enum { NOT_FOUND = -1 };

    switch (id) {
      case ATTRIBUTE_ID_ATTRIBUTE1: {
        return manipulator(&d_attribute1,
                           ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE1]);
                                                                      // RETURN
      } break;
      case ATTRIBUTE_ID_ATTRIBUTE2: {
        return manipulator(&d_attribute2,
                           ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE2]);
                                                                      // RETURN
      } break;
      case ATTRIBUTE_ID_ATTRIBUTE3: {
        return manipulator(&d_attribute3,
                           ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE3]);
                                                                      // RETURN
      } break;
      default:
        return NOT_FOUND;
    }
}

template <class MANIPULATOR>
inline
int MySequenceWithNullables::manipulateAttribute(
        MANIPULATOR&  manipulator,
        const char   *name,
        int           nameLength)
{
    enum { NOT_FOUND = -1 };

    const bdeat_AttributeInfo *attributeInfo =
           lookupAttributeInfo(name, nameLength);
    if (0 == attributeInfo) {
        return NOT_FOUND;                                             // RETURN
    }

    return manipulateAttribute(manipulator, attributeInfo->d_id);
}

inline
bdeut_NullableValue<int>& MySequenceWithNullables::attribute1()
{
    return d_attribute1;
}

inline
bdeut_NullableValue<bsl::string>& MySequenceWithNullables::attribute2()
{
    return d_attribute2;
}

inline
bdeut_NullableValue<MySequence>& MySequenceWithNullables::attribute3()
{
    return d_attribute3;
}

// ACCESSORS
template <class STREAM>
inline
STREAM& MySequenceWithNullables::bdexStreamOut(STREAM&  stream,
                                               int      version) const
{
    switch (version) {
      case 1: {
        bdex_OutStreamFunctions::streamOut(stream, d_attribute1, 1);
        bdex_OutStreamFunctions::streamOut(stream, d_attribute2, 1);
        bdex_OutStreamFunctions::streamOut(stream, d_attribute3, 1);
      } break;
    }
    return stream;
}

template <class ACCESSOR>
inline
int MySequenceWithNullables::accessAttributes(ACCESSOR& accessor) const
{
    int ret;

    ret = accessor(d_attribute1,
                   ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE1]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = accessor(d_attribute2,
                   ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE2]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = accessor(d_attribute3,
                   ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE3]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    return ret;
}

template <class ACCESSOR>
inline
int MySequenceWithNullables::accessAttribute(ACCESSOR& accessor, int id) const
{
    enum { NOT_FOUND = -1 };

    switch (id) {
      case ATTRIBUTE_ID_ATTRIBUTE1: {
        return accessor(d_attribute1,
                        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE1]);
                                                                      // RETURN
      } break;
      case ATTRIBUTE_ID_ATTRIBUTE2: {
        return accessor(d_attribute2,
                        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE2]);
                                                                      // RETURN
      } break;
      case ATTRIBUTE_ID_ATTRIBUTE3: {
        return accessor(d_attribute3,
                        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE3]);
                                                                      // RETURN
      } break;
      default:
        return NOT_FOUND;
    }
}

template <class ACCESSOR>
inline
int MySequenceWithNullables::accessAttribute(
        ACCESSOR&   accessor,
        const char *name,
        int         nameLength) const
{
    enum { NOT_FOUND = -1 };

    const bdeat_AttributeInfo *attributeInfo =
          lookupAttributeInfo(name, nameLength);
    if (0 == attributeInfo) {
       return NOT_FOUND;                                              // RETURN
    }

    return accessAttribute(accessor, attributeInfo->d_id);
}

inline
const bdeut_NullableValue<int>& MySequenceWithNullables::attribute1() const
{
    return d_attribute1;
}

inline
const bdeut_NullableValue<bsl::string>&
                                    MySequenceWithNullables::attribute2() const
{
    return d_attribute2;
}

inline
const bdeut_NullableValue<MySequence>&
                                    MySequenceWithNullables::attribute3() const
{
    return d_attribute3;
}

                               // --------------
                               // class Employee
                               // --------------

// CLASS METHODS
inline
int Employee::maxSupportedBdexVersion()
{
    return 1;  // versions start at 1.
}

// CREATORS
inline
Employee::Employee(bslma_Allocator *basicAllocator)
: d_name(basicAllocator)
, d_homeAddress(basicAllocator)
, d_age()
{
}

inline
Employee::Employee(
        const Employee& original,
        bslma_Allocator *basicAllocator)
: d_name(original.d_name, basicAllocator)
, d_homeAddress(original.d_homeAddress, basicAllocator)
, d_age(original.d_age)
{
}

inline
Employee::~Employee()
{
}

// MANIPULATORS
inline
Employee&
Employee::operator=(const Employee& rhs)
{
    if (this != &rhs) {
        d_name = rhs.d_name;
        d_homeAddress = rhs.d_homeAddress;
        d_age = rhs.d_age;
    }
    return *this;
}

template <class STREAM>
inline
STREAM& Employee::bdexStreamIn(STREAM& stream, int version)
{
    if (stream) {
        switch (version) {  // Switch on the schema version (starting with 1).
          case 1: {
            bdex_InStreamFunctions::streamIn(stream, d_name, 1);
            bdex_InStreamFunctions::streamIn(stream, d_homeAddress, 1);
            bdex_InStreamFunctions::streamIn(stream, d_age, 1);
          } break;
          default: {
            stream.invalidate();
          }
        }
    }
    return stream;
}

inline
void Employee::reset()
{
    bdeat_ValueTypeFunctions::reset(&d_name);
    bdeat_ValueTypeFunctions::reset(&d_homeAddress);
    bdeat_ValueTypeFunctions::reset(&d_age);
}

template <class MANIPULATOR>
inline
int Employee::manipulateAttributes(MANIPULATOR& manipulator)
{
    int ret;

    ret = manipulator(&d_name, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_NAME]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = manipulator(&d_homeAddress,
                      ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_HOME_ADDRESS]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = manipulator(&d_age, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_AGE]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    return ret;
}

template <class MANIPULATOR>
inline
int Employee::manipulateAttribute(MANIPULATOR& manipulator, int id)
{
    enum { NOT_FOUND = -1 };

    switch (id) {
      case ATTRIBUTE_ID_NAME: {
        return manipulator(&d_name,
                           ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_NAME]);
                                                                      // RETURN
      } break;
      case ATTRIBUTE_ID_HOME_ADDRESS: {
        return manipulator(&d_homeAddress,
                           ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_HOME_ADDRESS]);
                                                                      // RETURN
      } break;
      case ATTRIBUTE_ID_AGE: {
        return manipulator(&d_age, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_AGE]);
                                                                      // RETURN
      } break;
      default:
        return NOT_FOUND;
    }
}

template <class MANIPULATOR>
inline
int Employee::manipulateAttribute(
        MANIPULATOR&  manipulator,
        const char   *name,
        int           nameLength)
{
    enum { NOT_FOUND = -1 };

    const bdeat_AttributeInfo *attributeInfo =
           lookupAttributeInfo(name, nameLength);
    if (0 == attributeInfo) {
        return NOT_FOUND;                                             // RETURN
    }

    return manipulateAttribute(manipulator, attributeInfo->d_id);
}

inline
bsl::string& Employee::name()
{
    return d_name;
}

inline
Address& Employee::homeAddress()
{
    return d_homeAddress;
}

inline
int& Employee::age()
{
    return d_age;
}

// ACCESSORS
template <class STREAM>
inline
STREAM& Employee::bdexStreamOut(STREAM& stream, int version) const
{
    switch (version) {
      case 1: {
        bdex_OutStreamFunctions::streamOut(stream, d_name, 1);
        bdex_OutStreamFunctions::streamOut(stream, d_homeAddress, 1);
        bdex_OutStreamFunctions::streamOut(stream, d_age, 1);
      } break;
    }
    return stream;
}

template <class ACCESSOR>
inline
int Employee::accessAttributes(ACCESSOR& accessor) const
{
    int ret;

    ret = accessor(d_name, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_NAME]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = accessor(d_homeAddress,
                   ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_HOME_ADDRESS]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = accessor(d_age, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_AGE]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    return ret;
}

template <class ACCESSOR>
inline
int Employee::accessAttribute(ACCESSOR& accessor, int id) const
{
    enum { NOT_FOUND = -1 };

    switch (id) {
      case ATTRIBUTE_ID_NAME: {
        return accessor(d_name, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_NAME]);
                                                                      // RETURN
      } break;
      case ATTRIBUTE_ID_HOME_ADDRESS: {
        return accessor(d_homeAddress,
                        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_HOME_ADDRESS]);
                                                                      // RETURN
      } break;
      case ATTRIBUTE_ID_AGE: {
        return accessor(d_age, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_AGE]);
                                                                      // RETURN
      } break;
      default:
        return NOT_FOUND;
    }
}

template <class ACCESSOR>
inline
int Employee::accessAttribute(
        ACCESSOR&   accessor,
        const char *name,
        int         nameLength) const
{
    enum { NOT_FOUND = -1 };

    const bdeat_AttributeInfo *attributeInfo =
          lookupAttributeInfo(name, nameLength);
    if (0 == attributeInfo) {
       return NOT_FOUND;                                              // RETURN
    }

    return accessAttribute(accessor, attributeInfo->d_id);
}

inline
const bsl::string& Employee::name() const
{
    return d_name;
}

inline
const Address& Employee::homeAddress() const
{
    return d_homeAddress;
}

inline
const int& Employee::age() const
{
    return d_age;
}

                    // -----------------------------------
                    // class MySequenceWithAnonymousChoice
                    // -----------------------------------

// CLASS METHODS
inline
int MySequenceWithAnonymousChoice::maxSupportedBdexVersion()
{
    return 1;  // versions start at 1.
}

// CREATORS
inline
MySequenceWithAnonymousChoice::MySequenceWithAnonymousChoice(
                bslma_Allocator *basicAllocator)
: d_attribute1()
, d_mySequenceWithAnonymousChoiceChoice(basicAllocator)
, d_attribute2(basicAllocator)
{
}

inline
MySequenceWithAnonymousChoice::MySequenceWithAnonymousChoice(
        const MySequenceWithAnonymousChoice& original,
        bslma_Allocator *basicAllocator)
: d_attribute1(original.d_attribute1)
, d_mySequenceWithAnonymousChoiceChoice(
                original.d_mySequenceWithAnonymousChoiceChoice,
                basicAllocator)
, d_attribute2(original.d_attribute2, basicAllocator)
{
}

inline
MySequenceWithAnonymousChoice::~MySequenceWithAnonymousChoice()
{
}

// MANIPULATORS
inline
MySequenceWithAnonymousChoice&
MySequenceWithAnonymousChoice::operator=(
                const MySequenceWithAnonymousChoice& rhs)
{
    if (this != &rhs) {
        d_attribute1 = rhs.d_attribute1;
        d_mySequenceWithAnonymousChoiceChoice
                     = rhs.d_mySequenceWithAnonymousChoiceChoice;
        d_attribute2 = rhs.d_attribute2;
    }
    return *this;
}

template <class STREAM>
inline
STREAM& MySequenceWithAnonymousChoice::bdexStreamIn(STREAM&  stream,
                                                    int      version)
{
    if (stream) {
        switch (version) {  // Switch on the schema version (starting with 1).
          case 1: {
            bdex_InStreamFunctions::streamIn(stream, d_attribute1, 1);
            bdex_InStreamFunctions::streamIn(
                                        stream,
                                        d_mySequenceWithAnonymousChoiceChoice,
                                        1);
            bdex_InStreamFunctions::streamIn(stream, d_attribute2, 1);
          } break;
          default: {
            stream.invalidate();
          }
        }
    }
    return stream;
}

inline
void MySequenceWithAnonymousChoice::reset()
{
    bdeat_ValueTypeFunctions::reset(&d_attribute1);
    bdeat_ValueTypeFunctions::reset(&d_mySequenceWithAnonymousChoiceChoice);
    bdeat_ValueTypeFunctions::reset(&d_attribute2);
}

template <class MANIPULATOR>
inline
int MySequenceWithAnonymousChoice::manipulateAttributes(
                MANIPULATOR& manipulator)
{
    int ret;

    ret = manipulator(&d_attribute1,
                      ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE1]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = manipulator(&d_mySequenceWithAnonymousChoiceChoice,
                  ATTRIBUTE_INFO_ARRAY[
                      ATTRIBUTE_INDEX_MY_SEQUENCE_WITH_ANONYMOUS_CHOICE_CHOICE
                  ]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = manipulator(&d_attribute2,
                      ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE2]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    return ret;
}

template <class MANIPULATOR>
inline
int MySequenceWithAnonymousChoice::manipulateAttribute(
                MANIPULATOR&  manipulator,
                int           id)
{
    enum { NOT_FOUND = -1 };

    switch (id) {
      case ATTRIBUTE_ID_ATTRIBUTE1: {
        return manipulator(&d_attribute1,
                           ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE1]);
                                                                      // RETURN
      } break;
      case ATTRIBUTE_ID_MY_SEQUENCE_WITH_ANONYMOUS_CHOICE_CHOICE: {
        return manipulator(&d_mySequenceWithAnonymousChoiceChoice,
                  ATTRIBUTE_INFO_ARRAY[
                      ATTRIBUTE_INDEX_MY_SEQUENCE_WITH_ANONYMOUS_CHOICE_CHOICE
                  ]);
                                                                      // RETURN
      } break;
      case ATTRIBUTE_ID_ATTRIBUTE2: {
        return manipulator(&d_attribute2,
                           ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE2]);
                                                                      // RETURN
      } break;
      default:
        return NOT_FOUND;
    }
}

template <class MANIPULATOR>
inline
int MySequenceWithAnonymousChoice::manipulateAttribute(
        MANIPULATOR&  manipulator,
        const char   *name,
        int           nameLength)
{
    enum { NOT_FOUND = -1 };

    const bdeat_AttributeInfo *attributeInfo =
           lookupAttributeInfo(name, nameLength);
    if (0 == attributeInfo) {
        return NOT_FOUND;                                             // RETURN
    }

    return manipulateAttribute(manipulator, attributeInfo->d_id);
}

inline
bdeut_NullableValue<int>& MySequenceWithAnonymousChoice::attribute1()
{
    return d_attribute1;
}

inline
MySequenceWithAnonymousChoiceChoice&
           MySequenceWithAnonymousChoice::mySequenceWithAnonymousChoiceChoice()
{
    return d_mySequenceWithAnonymousChoiceChoice;
}

inline
bdeut_NullableValue<bsl::string>& MySequenceWithAnonymousChoice::attribute2()
{
    return d_attribute2;
}

// ACCESSORS
template <class STREAM>
inline
STREAM& MySequenceWithAnonymousChoice::bdexStreamOut(
            STREAM&  stream,
            int      version) const
{
    switch (version) {
      case 1: {
        bdex_OutStreamFunctions::streamOut(stream, d_attribute1, 1);
        bdex_OutStreamFunctions::streamOut(
                                        stream,
                                        d_mySequenceWithAnonymousChoiceChoice,
                                        1);
        bdex_OutStreamFunctions::streamOut(stream, d_attribute2, 1);
      } break;
    }
    return stream;
}

template <class ACCESSOR>
inline
int MySequenceWithAnonymousChoice::accessAttributes(ACCESSOR& accessor) const
{
    int ret;

    ret = accessor(d_attribute1,
                   ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE1]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = accessor(d_mySequenceWithAnonymousChoiceChoice,
                   ATTRIBUTE_INFO_ARRAY[
                      ATTRIBUTE_INDEX_MY_SEQUENCE_WITH_ANONYMOUS_CHOICE_CHOICE
                   ]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = accessor(d_attribute2,
                   ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE2]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    return ret;
}

template <class ACCESSOR>
inline
int MySequenceWithAnonymousChoice::accessAttribute(ACCESSOR&  accessor,
                                                   int        id) const
{
    enum { NOT_FOUND = -1 };

    switch (id) {
      case ATTRIBUTE_ID_ATTRIBUTE1: {
        return accessor(d_attribute1,
                        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE1]);
                                                                      // RETURN
      } break;
      case ATTRIBUTE_ID_MY_SEQUENCE_WITH_ANONYMOUS_CHOICE_CHOICE: {
        return accessor(d_mySequenceWithAnonymousChoiceChoice,
              ATTRIBUTE_INFO_ARRAY[
                  ATTRIBUTE_INDEX_MY_SEQUENCE_WITH_ANONYMOUS_CHOICE_CHOICE
              ]);
                                                                      // RETURN
      } break;
      case ATTRIBUTE_ID_ATTRIBUTE2: {
        return accessor(d_attribute2,
                        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE2]);
                                                                      // RETURN
      } break;
      default:
        return NOT_FOUND;
    }
}

template <class ACCESSOR>
inline
int MySequenceWithAnonymousChoice::accessAttribute(
        ACCESSOR&   accessor,
        const char *name,
        int         nameLength) const
{
    enum { NOT_FOUND = -1 };

    const bdeat_AttributeInfo *attributeInfo =
          lookupAttributeInfo(name, nameLength);
    if (0 == attributeInfo) {
       return NOT_FOUND;                                              // RETURN
    }

    return accessAttribute(accessor, attributeInfo->d_id);
}

inline
const bdeut_NullableValue<int>&
                              MySequenceWithAnonymousChoice::attribute1() const
{
    return d_attribute1;
}

inline
const MySequenceWithAnonymousChoiceChoice&
     MySequenceWithAnonymousChoice::mySequenceWithAnonymousChoiceChoice() const
{
    return d_mySequenceWithAnonymousChoiceChoice;
}

inline
const bdeut_NullableValue<bsl::string>&
                              MySequenceWithAnonymousChoice::attribute2() const
{
    return d_attribute2;
}

                       // -----------------------------
                       // class MySequenceWithNillables
                       // -----------------------------

// CLASS METHODS
inline
int MySequenceWithNillables::maxSupportedBdexVersion()
{
    return 1;  // versions start at 1.
}

// CREATORS
inline
MySequenceWithNillables::MySequenceWithNillables(
                bslma_Allocator *basicAllocator)
: d_attribute1()
, d_attribute2(basicAllocator)
, d_attribute3(basicAllocator)
{
}

inline
MySequenceWithNillables::MySequenceWithNillables(
        const MySequenceWithNillables& original,
        bslma_Allocator *basicAllocator)
: d_attribute1(original.d_attribute1)
, d_attribute2(original.d_attribute2, basicAllocator)
, d_attribute3(original.d_attribute3, basicAllocator)
{
}

inline
MySequenceWithNillables::~MySequenceWithNillables()
{
}

// MANIPULATORS
inline
MySequenceWithNillables&
MySequenceWithNillables::operator=(const MySequenceWithNillables& rhs)
{
    if (this != &rhs) {
        d_attribute1 = rhs.d_attribute1;
        d_attribute2 = rhs.d_attribute2;
        d_attribute3 = rhs.d_attribute3;
    }
    return *this;
}

template <class STREAM>
inline
STREAM& MySequenceWithNillables::bdexStreamIn(STREAM& stream, int version)
{
    if (stream) {
        switch (version) {  // Switch on the schema version (starting with 1).
          case 1: {
            bdex_InStreamFunctions::streamIn(stream, d_attribute1, 1);
            bdex_InStreamFunctions::streamIn(stream, d_attribute2, 1);
            bdex_InStreamFunctions::streamIn(stream, d_attribute3, 1);
          } break;
          default: {
            stream.invalidate();
          }
        }
    }
    return stream;
}

inline
void MySequenceWithNillables::reset()
{
    bdeat_ValueTypeFunctions::reset(&d_attribute1);
    bdeat_ValueTypeFunctions::reset(&d_attribute2);
    bdeat_ValueTypeFunctions::reset(&d_attribute3);
}

template <class MANIPULATOR>
inline
int MySequenceWithNillables::manipulateAttributes(MANIPULATOR& manipulator)
{
    int ret;

    ret = manipulator(&d_attribute1,
                      ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE1]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = manipulator(&d_attribute2,
                      ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE2]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = manipulator(&d_attribute3,
                      ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE3]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    return ret;
}

template <class MANIPULATOR>
inline
int MySequenceWithNillables::manipulateAttribute(MANIPULATOR&  manipulator,
                                                 int           id)
{
    enum { NOT_FOUND = -1 };

    switch (id) {
      case ATTRIBUTE_ID_ATTRIBUTE1: {
        return manipulator(&d_attribute1,
                           ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE1]);
                                                                      // RETURN
      } break;
      case ATTRIBUTE_ID_ATTRIBUTE2: {
        return manipulator(&d_attribute2,
                           ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE2]);
                                                                      // RETURN
      } break;
      case ATTRIBUTE_ID_ATTRIBUTE3: {
        return manipulator(&d_attribute3,
                           ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE3]);
                                                                      // RETURN
      } break;
      default:
        return NOT_FOUND;
    }
}

template <class MANIPULATOR>
inline
int MySequenceWithNillables::manipulateAttribute(
        MANIPULATOR&  manipulator,
        const char   *name,
        int           nameLength)
{
    enum { NOT_FOUND = -1 };

    const bdeat_AttributeInfo *attributeInfo =
           lookupAttributeInfo(name, nameLength);
    if (0 == attributeInfo) {
        return NOT_FOUND;                                             // RETURN
    }

    return manipulateAttribute(manipulator, attributeInfo->d_id);
}

inline
bdeut_NullableValue<int>& MySequenceWithNillables::attribute1()
{
    return d_attribute1;
}

inline
bdeut_NullableValue<bsl::string>& MySequenceWithNillables::attribute2()
{
    return d_attribute2;
}

inline
bdeut_NullableValue<MySequence>& MySequenceWithNillables::attribute3()
{
    return d_attribute3;
}

// ACCESSORS
template <class STREAM>
inline
STREAM& MySequenceWithNillables::bdexStreamOut(STREAM&  stream,
                                               int      version) const
{
    switch (version) {
      case 1: {
        bdex_OutStreamFunctions::streamOut(stream, d_attribute1, 1);
        bdex_OutStreamFunctions::streamOut(stream, d_attribute2, 1);
        bdex_OutStreamFunctions::streamOut(stream, d_attribute3, 1);
      } break;
    }
    return stream;
}

template <class ACCESSOR>
inline
int MySequenceWithNillables::accessAttributes(ACCESSOR& accessor) const
{
    int ret;

    ret = accessor(d_attribute1,
                   ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE1]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = accessor(d_attribute2,
                   ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE2]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = accessor(d_attribute3,
                   ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE3]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    return ret;
}

template <class ACCESSOR>
inline
int MySequenceWithNillables::accessAttribute(ACCESSOR& accessor, int id) const
{
    enum { NOT_FOUND = -1 };

    switch (id) {
      case ATTRIBUTE_ID_ATTRIBUTE1: {
        return accessor(d_attribute1,
                        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE1]);
                                                                      // RETURN
      } break;
      case ATTRIBUTE_ID_ATTRIBUTE2: {
        return accessor(d_attribute2,
                        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE2]);
                                                                      // RETURN
      } break;
      case ATTRIBUTE_ID_ATTRIBUTE3: {
        return accessor(d_attribute3,
                        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE3]);
                                                                      // RETURN
      } break;
      default:
        return NOT_FOUND;
    }
}

template <class ACCESSOR>
inline
int MySequenceWithNillables::accessAttribute(
        ACCESSOR&   accessor,
        const char *name,
        int         nameLength) const
{
    enum { NOT_FOUND = -1 };

    const bdeat_AttributeInfo *attributeInfo =
          lookupAttributeInfo(name, nameLength);
    if (0 == attributeInfo) {
       return NOT_FOUND;                                              // RETURN
    }

    return accessAttribute(accessor, attributeInfo->d_id);
}

inline
const bdeut_NullableValue<int>& MySequenceWithNillables::attribute1() const
{
    return d_attribute1;
}

inline
const bdeut_NullableValue<bsl::string>&
                                    MySequenceWithNillables::attribute2() const
{
    return d_attribute2;
}

inline
const bdeut_NullableValue<MySequence>&
                                    MySequenceWithNillables::attribute3() const
{
    return d_attribute3;
}

}  // close namespace test

// FREE FUNCTIONS

inline
bool test::operator==(
        const test::MySequenceWithAttributes& lhs,
        const test::MySequenceWithAttributes& rhs)
{
    return  lhs.attribute1() == rhs.attribute1()
         && lhs.attribute2() == rhs.attribute2()
         && lhs.element1() == rhs.element1()
         && lhs.element2() == rhs.element2();
}

inline
bool test::operator!=(
        const test::MySequenceWithAttributes& lhs,
        const test::MySequenceWithAttributes& rhs)
{
    return  lhs.attribute1() != rhs.attribute1()
         || lhs.attribute2() != rhs.attribute2()
         || lhs.element1() != rhs.element1()
         || lhs.element2() != rhs.element2();
}

inline
bsl::ostream& test::operator<<(
        bsl::ostream& stream,
        const test::MySequenceWithAttributes& rhs)
{
    return rhs.print(stream, 0, -1);
}

inline
bool test::operator==(
        const test::Address& lhs,
        const test::Address& rhs)
{
    return  lhs.street() == rhs.street()
         && lhs.city() == rhs.city()
         && lhs.state() == rhs.state();
}

inline
bool test::operator!=(
        const test::Address& lhs,
        const test::Address& rhs)
{
    return  lhs.street() != rhs.street()
         || lhs.city() != rhs.city()
         || lhs.state() != rhs.state();
}

inline
bsl::ostream& test::operator<<(
        bsl::ostream& stream,
        const test::Address& rhs)
{
    return rhs.print(stream, 0, -1);
}

inline
bool test::operator==(
        const test::MySequence& lhs,
        const test::MySequence& rhs)
{
    return  lhs.attribute1() == rhs.attribute1()
         && lhs.attribute2() == rhs.attribute2();
}

inline
bool test::operator!=(
        const test::MySequence& lhs,
        const test::MySequence& rhs)
{
    return  lhs.attribute1() != rhs.attribute1()
         || lhs.attribute2() != rhs.attribute2();
}

inline
bsl::ostream& test::operator<<(
        bsl::ostream& stream,
        const test::MySequence& rhs)
{
    return rhs.print(stream, 0, -1);
}

inline
bool test::operator==(
        const test::MySimpleContent& lhs,
        const test::MySimpleContent& rhs)
{
    return  lhs.attribute1() == rhs.attribute1()
         && lhs.attribute2() == rhs.attribute2()
         && lhs.theContent() == rhs.theContent();
}

inline
bool test::operator!=(
        const test::MySimpleContent& lhs,
        const test::MySimpleContent& rhs)
{
    return  lhs.attribute1() != rhs.attribute1()
         || lhs.attribute2() != rhs.attribute2()
         || lhs.theContent() != rhs.theContent();
}

inline
bsl::ostream& test::operator<<(
        bsl::ostream& stream,
        const test::MySimpleContent& rhs)
{
    return rhs.print(stream, 0, -1);
}

inline
bool test::operator==(
        const test::MySimpleIntContent& lhs,
        const test::MySimpleIntContent& rhs)
{
    return  lhs.attribute1() == rhs.attribute1()
         && lhs.attribute2() == rhs.attribute2()
         && lhs.theContent() == rhs.theContent();
}

inline
bool test::operator!=(
        const test::MySimpleIntContent& lhs,
        const test::MySimpleIntContent& rhs)
{
    return  lhs.attribute1() != rhs.attribute1()
         || lhs.attribute2() != rhs.attribute2()
         || lhs.theContent() != rhs.theContent();
}

inline
bsl::ostream& test::operator<<(
        bsl::ostream& stream,
        const test::MySimpleIntContent& rhs)
{
    return rhs.print(stream, 0, -1);
}

inline
bool test::operator==(
        const test::MySequenceWithAnonymousChoiceChoice& lhs,
        const test::MySequenceWithAnonymousChoiceChoice& rhs)
{
    typedef test::MySequenceWithAnonymousChoiceChoice Class;
    if (lhs.selectionId() == rhs.selectionId()) {
        switch (rhs.selectionId()) {
          case Class::SELECTION_ID_MY_CHOICE1:
            return lhs.myChoice1() == rhs.myChoice1();
                                                                    // RETURN
          case Class::SELECTION_ID_MY_CHOICE2:
            return lhs.myChoice2() == rhs.myChoice2();
                                                                    // RETURN
          default:
            BSLS_ASSERT_SAFE(Class::SELECTION_ID_UNDEFINED
                              == rhs.selectionId());
            return true;                                            // RETURN
        }
    }
    else {
        return false;
   }
}

inline
bool test::operator!=(
        const test::MySequenceWithAnonymousChoiceChoice& lhs,
        const test::MySequenceWithAnonymousChoiceChoice& rhs)
{
    return !(lhs == rhs);
}

inline
bsl::ostream& test::operator<<(
        bsl::ostream& stream,
        const test::MySequenceWithAnonymousChoiceChoice& rhs)
{
    return rhs.print(stream, 0, -1);
}

inline
bool test::operator==(
        const test::MySequenceWithNullables& lhs,
        const test::MySequenceWithNullables& rhs)
{
    return  lhs.attribute1() == rhs.attribute1()
         && lhs.attribute2() == rhs.attribute2()
         && lhs.attribute3() == rhs.attribute3();
}

inline
bool test::operator!=(
        const test::MySequenceWithNullables& lhs,
        const test::MySequenceWithNullables& rhs)
{
    return  lhs.attribute1() != rhs.attribute1()
         || lhs.attribute2() != rhs.attribute2()
         || lhs.attribute3() != rhs.attribute3();
}

inline
bsl::ostream& test::operator<<(
        bsl::ostream& stream,
        const test::MySequenceWithNullables& rhs)
{
    return rhs.print(stream, 0, -1);
}

inline
bool test::operator==(
        const test::Employee& lhs,
        const test::Employee& rhs)
{
    return  lhs.name() == rhs.name()
         && lhs.homeAddress() == rhs.homeAddress()
         && lhs.age() == rhs.age();
}

inline
bool test::operator!=(
        const test::Employee& lhs,
        const test::Employee& rhs)
{
    return  lhs.name() != rhs.name()
         || lhs.homeAddress() != rhs.homeAddress()
         || lhs.age() != rhs.age();
}

inline
bsl::ostream& test::operator<<(
        bsl::ostream& stream,
        const test::Employee& rhs)
{
    return rhs.print(stream, 0, -1);
}

inline
bool test::operator==(
        const test::MySequenceWithAnonymousChoice& lhs,
        const test::MySequenceWithAnonymousChoice& rhs)
{
    return  lhs.attribute1() == rhs.attribute1()
         &&    lhs.mySequenceWithAnonymousChoiceChoice()
            == rhs.mySequenceWithAnonymousChoiceChoice()
         && lhs.attribute2() == rhs.attribute2();
}

inline
bool test::operator!=(
        const test::MySequenceWithAnonymousChoice& lhs,
        const test::MySequenceWithAnonymousChoice& rhs)
{
    return  lhs.attribute1() != rhs.attribute1()
         ||     lhs.mySequenceWithAnonymousChoiceChoice()
             != rhs.mySequenceWithAnonymousChoiceChoice()
         || lhs.attribute2() != rhs.attribute2();
}

inline
bsl::ostream& test::operator<<(
        bsl::ostream& stream,
        const test::MySequenceWithAnonymousChoice& rhs)
{
    return rhs.print(stream, 0, -1);
}

inline
bool test::operator==(
        const test::MySequenceWithNillables& lhs,
        const test::MySequenceWithNillables& rhs)
{
    return  lhs.attribute1() == rhs.attribute1()
         && lhs.attribute2() == rhs.attribute2()
         && lhs.attribute3() == rhs.attribute3();
}

inline
bool test::operator!=(
        const test::MySequenceWithNillables& lhs,
        const test::MySequenceWithNillables& rhs)
{
    return  lhs.attribute1() != rhs.attribute1()
         || lhs.attribute2() != rhs.attribute2()
         || lhs.attribute3() != rhs.attribute3();
}

inline
bsl::ostream& test::operator<<(
        bsl::ostream& stream,
        const test::MySequenceWithNillables& rhs)
{
    return rhs.print(stream, 0, -1);
}

}  // close namespace BloombergLP;
#endif

// GENERATED BY BLP_BAS_CODEGEN_2.0.2_DEV_LATEST Thu Dec 28 20:58:29 2006
// ----------------------------------------------------------------------------
// *End-of-file block removed*
// ----------------------------------------------------------------------------

// test_messages.cpp   -*-C++-*-

// #include <test_messages.h>
#include <bdeut_nullablevalue.h>
#include <bsl_string.h>

#include <bdeat_formattingmode.h>

#include <bsls_assert.h>
#include <bdeu_chartype.h>
#include <bdeu_print.h>
#include <bdeu_printmethods.h>
#include <bdeu_string.h>

#include <bsl_ostream.h>
#include <bsl_iomanip.h>

namespace BloombergLP {
namespace test {

                       // ------------------------------
                       // class MySequenceWithAttributes
                       // ------------------------------

// CONSTANTS

const char MySequenceWithAttributes::CLASS_NAME[] = "MySequenceWithAttributes";
    // the name of this class

const bdeat_AttributeInfo MySequenceWithAttributes::ATTRIBUTE_INFO_ARRAY[] = {
    {
        ATTRIBUTE_ID_ATTRIBUTE1,
        "Attribute1",                 // name
        sizeof("Attribute1") - 1,     // name length
        "Attribute1: TBD: Provide annotation",  // annotation
        bdeat_FormattingMode::BDEAT_DEC // formatting mode
      | bdeat_FormattingMode::BDEAT_ATTRIBUTE
    },
    {
        ATTRIBUTE_ID_ATTRIBUTE2,
        "Attribute2",                 // name
        sizeof("Attribute2") - 1,     // name length
        "Attribute2: TBD: Provide annotation",  // annotation
        bdeat_FormattingMode::BDEAT_TEXT // formatting mode
      | bdeat_FormattingMode::BDEAT_ATTRIBUTE
    },
    {
        ATTRIBUTE_ID_ELEMENT1,
        "Element1",                 // name
        sizeof("Element1") - 1,     // name length
        "Element1: TBD: Provide annotation",  // annotation
        bdeat_FormattingMode::BDEAT_DEC // formatting mode
    },
    {
        ATTRIBUTE_ID_ELEMENT2,
        "Element2",                 // name
        sizeof("Element2") - 1,     // name length
        "Element2: TBD: Provide annotation",  // annotation
        bdeat_FormattingMode::BDEAT_TEXT // formatting mode
    }
};

// CLASS METHODS

const bdeat_AttributeInfo *MySequenceWithAttributes::lookupAttributeInfo(
        const char *name,
        int         nameLength)
{
    switch(nameLength) {
        case 8: {
            if (bdeu_CharType::toUpper(name[0])=='E'
             && bdeu_CharType::toUpper(name[1])=='L'
             && bdeu_CharType::toUpper(name[2])=='E'
             && bdeu_CharType::toUpper(name[3])=='M'
             && bdeu_CharType::toUpper(name[4])=='E'
             && bdeu_CharType::toUpper(name[5])=='N'
             && bdeu_CharType::toUpper(name[6])=='T')
            {
                switch(bdeu_CharType::toUpper(name[7])) {
                    case '1': {
                        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT1];
                    } break;
                    case '2': {
                        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT2];
                    } break;
                }
            }
        } break;
        case 10: {
            if (bdeu_CharType::toUpper(name[0])=='A'
             && bdeu_CharType::toUpper(name[1])=='T'
             && bdeu_CharType::toUpper(name[2])=='T'
             && bdeu_CharType::toUpper(name[3])=='R'
             && bdeu_CharType::toUpper(name[4])=='I'
             && bdeu_CharType::toUpper(name[5])=='B'
             && bdeu_CharType::toUpper(name[6])=='U'
             && bdeu_CharType::toUpper(name[7])=='T'
             && bdeu_CharType::toUpper(name[8])=='E')
            {
                switch(bdeu_CharType::toUpper(name[9])) {
                    case '1': {
                        return &ATTRIBUTE_INFO_ARRAY[
                                    ATTRIBUTE_INDEX_ATTRIBUTE1
                               ];
                    } break;
                    case '2': {
                        return &ATTRIBUTE_INFO_ARRAY[
                                    ATTRIBUTE_INDEX_ATTRIBUTE2
                               ];
                    } break;
                }
            }
        } break;
    }
    return 0;
}

const bdeat_AttributeInfo *MySequenceWithAttributes::lookupAttributeInfo(
                int id)
{
    switch (id) {
      case ATTRIBUTE_ID_ATTRIBUTE1:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE1];
      case ATTRIBUTE_ID_ATTRIBUTE2:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE2];
      case ATTRIBUTE_ID_ELEMENT1:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT1];
      case ATTRIBUTE_ID_ELEMENT2:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT2];
      default:
        return 0;
    }
}

// ACCESSORS

bsl::ostream& MySequenceWithAttributes::print(
    bsl::ostream& stream,
    int           level,
    int           spacesPerLevel) const
{
    if (level < 0) {
        level = -level;
    }
    else {
        bdeu_Print::indent(stream, level, spacesPerLevel);
    }

    int levelPlus1 = level + 1;

    if (0 <= spacesPerLevel) {
        // multiline

        stream << "[\n";

        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "Attribute1 = ";
        bdeu_PrintMethods::print(stream, d_attribute1,
                                 -levelPlus1, spacesPerLevel);

        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "Attribute2 = ";
        bdeu_PrintMethods::print(stream, d_attribute2,
                                 -levelPlus1, spacesPerLevel);

        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "Element1 = ";
        bdeu_PrintMethods::print(stream, d_element1,
                                 -levelPlus1, spacesPerLevel);

        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "Element2 = ";
        bdeu_PrintMethods::print(stream, d_element2,
                                 -levelPlus1, spacesPerLevel);

        bdeu_Print::indent(stream, level, spacesPerLevel);
        stream << "]\n";
    }
    else {
        // single line

        stream << '[';

        stream << ' ';
        stream << "Attribute1 = ";
        bdeu_PrintMethods::print(stream, d_attribute1,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "Attribute2 = ";
        bdeu_PrintMethods::print(stream, d_attribute2,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "Element1 = ";
        bdeu_PrintMethods::print(stream, d_element1,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "Element2 = ";
        bdeu_PrintMethods::print(stream, d_element2,
                                 -levelPlus1, spacesPerLevel);

        stream << " ]";
    }

    return stream << bsl::flush;
}

                               // -------------
                               // class Address
                               // -------------

// CONSTANTS

const char Address::CLASS_NAME[] = "Address";
    // the name of this class

const bdeat_AttributeInfo Address::ATTRIBUTE_INFO_ARRAY[] = {
    {
        ATTRIBUTE_ID_STREET,
        "street",                 // name
        sizeof("street") - 1,     // name length
        "street: TBD: Provide annotation",  // annotation
        bdeat_FormattingMode::BDEAT_TEXT // formatting mode
    },
    {
        ATTRIBUTE_ID_CITY,
        "city",                 // name
        sizeof("city") - 1,     // name length
        "city: TBD: Provide annotation",  // annotation
        bdeat_FormattingMode::BDEAT_TEXT // formatting mode
    },
    {
        ATTRIBUTE_ID_STATE,
        "state",                 // name
        sizeof("state") - 1,     // name length
        "state: TBD: Provide annotation",  // annotation
        bdeat_FormattingMode::BDEAT_TEXT // formatting mode
    }
};

// CLASS METHODS

const bdeat_AttributeInfo *Address::lookupAttributeInfo(
        const char *name,
        int         nameLength)
{
    switch(nameLength) {
        case 4: {
            if (bdeu_CharType::toUpper(name[0])=='C'
             && bdeu_CharType::toUpper(name[1])=='I'
             && bdeu_CharType::toUpper(name[2])=='T'
             && bdeu_CharType::toUpper(name[3])=='Y')
            {
                return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CITY];
            }
        } break;
        case 5: {
            if (bdeu_CharType::toUpper(name[0])=='S'
             && bdeu_CharType::toUpper(name[1])=='T'
             && bdeu_CharType::toUpper(name[2])=='A'
             && bdeu_CharType::toUpper(name[3])=='T'
             && bdeu_CharType::toUpper(name[4])=='E')
            {
                return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_STATE];
            }
        } break;
        case 6: {
            if (bdeu_CharType::toUpper(name[0])=='S'
             && bdeu_CharType::toUpper(name[1])=='T'
             && bdeu_CharType::toUpper(name[2])=='R'
             && bdeu_CharType::toUpper(name[3])=='E'
             && bdeu_CharType::toUpper(name[4])=='E'
             && bdeu_CharType::toUpper(name[5])=='T')
            {
                return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_STREET];
            }
        } break;
    }
    return 0;
}

const bdeat_AttributeInfo *Address::lookupAttributeInfo(int id)
{
    switch (id) {
      case ATTRIBUTE_ID_STREET:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_STREET];
      case ATTRIBUTE_ID_CITY:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CITY];
      case ATTRIBUTE_ID_STATE:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_STATE];
      default:
        return 0;
    }
}

// ACCESSORS

bsl::ostream& Address::print(
    bsl::ostream& stream,
    int           level,
    int           spacesPerLevel) const
{
    if (level < 0) {
        level = -level;
    }
    else {
        bdeu_Print::indent(stream, level, spacesPerLevel);
    }

    int levelPlus1 = level + 1;

    if (0 <= spacesPerLevel) {
        // multiline

        stream << "[\n";

        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "Street = ";
        bdeu_PrintMethods::print(stream, d_street,
                                 -levelPlus1, spacesPerLevel);

        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "City = ";
        bdeu_PrintMethods::print(stream, d_city,
                                 -levelPlus1, spacesPerLevel);

        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "State = ";
        bdeu_PrintMethods::print(stream, d_state,
                                 -levelPlus1, spacesPerLevel);

        bdeu_Print::indent(stream, level, spacesPerLevel);
        stream << "]\n";
    }
    else {
        // single line

        stream << '[';

        stream << ' ';
        stream << "Street = ";
        bdeu_PrintMethods::print(stream, d_street,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "City = ";
        bdeu_PrintMethods::print(stream, d_city,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "State = ";
        bdeu_PrintMethods::print(stream, d_state,
                                 -levelPlus1, spacesPerLevel);

        stream << " ]";
    }

    return stream << bsl::flush;
}

                              // ----------------
                              // class MySequence
                              // ----------------

// CONSTANTS

const char MySequence::CLASS_NAME[] = "MySequence";
    // the name of this class

const bdeat_AttributeInfo MySequence::ATTRIBUTE_INFO_ARRAY[] = {
    {
        ATTRIBUTE_ID_ATTRIBUTE1,
        "Attribute1",                 // name
        sizeof("Attribute1") - 1,     // name length
        "Attribute1: TBD: Provide annotation",  // annotation
        bdeat_FormattingMode::BDEAT_DEC // formatting mode
    },
    {
        ATTRIBUTE_ID_ATTRIBUTE2,
        "Attribute2",                 // name
        sizeof("Attribute2") - 1,     // name length
        "Attribute2: TBD: Provide annotation",  // annotation
        bdeat_FormattingMode::BDEAT_TEXT // formatting mode
    }
};

// CLASS METHODS

const bdeat_AttributeInfo *MySequence::lookupAttributeInfo(
        const char *name,
        int         nameLength)
{
    switch(nameLength) {
        case 10: {
            if (bdeu_CharType::toUpper(name[0])=='A'
             && bdeu_CharType::toUpper(name[1])=='T'
             && bdeu_CharType::toUpper(name[2])=='T'
             && bdeu_CharType::toUpper(name[3])=='R'
             && bdeu_CharType::toUpper(name[4])=='I'
             && bdeu_CharType::toUpper(name[5])=='B'
             && bdeu_CharType::toUpper(name[6])=='U'
             && bdeu_CharType::toUpper(name[7])=='T'
             && bdeu_CharType::toUpper(name[8])=='E')
            {
                switch(bdeu_CharType::toUpper(name[9])) {
                    case '1': {
                        return &ATTRIBUTE_INFO_ARRAY[
                            ATTRIBUTE_INDEX_ATTRIBUTE1
                        ];
                    } break;
                    case '2': {
                        return &ATTRIBUTE_INFO_ARRAY[
                            ATTRIBUTE_INDEX_ATTRIBUTE2
                        ];
                    } break;
                }
            }
        } break;
    }
    return 0;
}

const bdeat_AttributeInfo *MySequence::lookupAttributeInfo(int id)
{
    switch (id) {
      case ATTRIBUTE_ID_ATTRIBUTE1:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE1];
      case ATTRIBUTE_ID_ATTRIBUTE2:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE2];
      default:
        return 0;
    }
}

// ACCESSORS

bsl::ostream& MySequence::print(
    bsl::ostream& stream,
    int           level,
    int           spacesPerLevel) const
{
    if (level < 0) {
        level = -level;
    }
    else {
        bdeu_Print::indent(stream, level, spacesPerLevel);
    }

    int levelPlus1 = level + 1;

    if (0 <= spacesPerLevel) {
        // multiline

        stream << "[\n";

        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "Attribute1 = ";
        bdeu_PrintMethods::print(stream, d_attribute1,
                                 -levelPlus1, spacesPerLevel);

        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "Attribute2 = ";
        bdeu_PrintMethods::print(stream, d_attribute2,
                                 -levelPlus1, spacesPerLevel);

        bdeu_Print::indent(stream, level, spacesPerLevel);
        stream << "]\n";
    }
    else {
        // single line

        stream << '[';

        stream << ' ';
        stream << "Attribute1 = ";
        bdeu_PrintMethods::print(stream, d_attribute1,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "Attribute2 = ";
        bdeu_PrintMethods::print(stream, d_attribute2,
                                 -levelPlus1, spacesPerLevel);

        stream << " ]";
    }

    return stream << bsl::flush;
}

                           // ---------------------
                           // class MySimpleContent
                           // ---------------------

// CONSTANTS

const char MySimpleContent::CLASS_NAME[] = "MySimpleContent";
    // the name of this class

const bdeat_AttributeInfo MySimpleContent::ATTRIBUTE_INFO_ARRAY[] = {
    {
        ATTRIBUTE_ID_ATTRIBUTE1,
        "Attribute1",                 // name
        sizeof("Attribute1") - 1,     // name length
        "Attribute1: TBD: Provide annotation",  // annotation
        bdeat_FormattingMode::BDEAT_TEXT // formatting mode
      | bdeat_FormattingMode::BDEAT_ATTRIBUTE
    },
    {
        ATTRIBUTE_ID_ATTRIBUTE2,
        "Attribute2",                 // name
        sizeof("Attribute2") - 1,     // name length
        "Attribute2: TBD: Provide annotation",  // annotation
        bdeat_FormattingMode::BDEAT_TEXT // formatting mode
      | bdeat_FormattingMode::BDEAT_ATTRIBUTE
    },
    {
        ATTRIBUTE_ID_THE_CONTENT,
        "TheContent",                 // name
        sizeof("TheContent") - 1,     // name length
        "TheContent: TBD: Provide annotation",  // annotation
        bdeat_FormattingMode::BDEAT_TEXT // formatting mode
      | bdeat_FormattingMode::BDEAT_SIMPLE_CONTENT
    }
};

// CLASS METHODS

const bdeat_AttributeInfo *MySimpleContent::lookupAttributeInfo(
        const char *name,
        int         nameLength)
{
    switch(nameLength) {
        case 10: {
            switch(bdeu_CharType::toUpper(name[0])) {
                case 'A': {
                    if (bdeu_CharType::toUpper(name[1])=='T'
                     && bdeu_CharType::toUpper(name[2])=='T'
                     && bdeu_CharType::toUpper(name[3])=='R'
                     && bdeu_CharType::toUpper(name[4])=='I'
                     && bdeu_CharType::toUpper(name[5])=='B'
                     && bdeu_CharType::toUpper(name[6])=='U'
                     && bdeu_CharType::toUpper(name[7])=='T'
                     && bdeu_CharType::toUpper(name[8])=='E')
                    {
                        switch(bdeu_CharType::toUpper(name[9])) {
                            case '1': {
                                return &ATTRIBUTE_INFO_ARRAY[
                                            ATTRIBUTE_INDEX_ATTRIBUTE1
                                       ];
                            } break;
                            case '2': {
                                return &ATTRIBUTE_INFO_ARRAY[
                                            ATTRIBUTE_INDEX_ATTRIBUTE2
                                       ];
                            } break;
                        }
                    }
                } break;
                case 'T': {
                    if (bdeu_CharType::toUpper(name[1])=='H'
                     && bdeu_CharType::toUpper(name[2])=='E'
                     && bdeu_CharType::toUpper(name[3])=='C'
                     && bdeu_CharType::toUpper(name[4])=='O'
                     && bdeu_CharType::toUpper(name[5])=='N'
                     && bdeu_CharType::toUpper(name[6])=='T'
                     && bdeu_CharType::toUpper(name[7])=='E'
                     && bdeu_CharType::toUpper(name[8])=='N'
                     && bdeu_CharType::toUpper(name[9])=='T')
                    {
                        return &ATTRIBUTE_INFO_ARRAY[
                                    ATTRIBUTE_INDEX_THE_CONTENT
                               ];
                    }
                } break;
            }
        } break;
    }
    return 0;
}

const bdeat_AttributeInfo *MySimpleContent::lookupAttributeInfo(int id)
{
    switch (id) {
      case ATTRIBUTE_ID_ATTRIBUTE1:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE1];
      case ATTRIBUTE_ID_ATTRIBUTE2:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE2];
      case ATTRIBUTE_ID_THE_CONTENT:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_THE_CONTENT];
      default:
        return 0;
    }
}

// ACCESSORS

bsl::ostream& MySimpleContent::print(
    bsl::ostream& stream,
    int           level,
    int           spacesPerLevel) const
{
    if (level < 0) {
        level = -level;
    }
    else {
        bdeu_Print::indent(stream, level, spacesPerLevel);
    }

    int levelPlus1 = level + 1;

    if (0 <= spacesPerLevel) {
        // multiline

        stream << "[\n";

        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "Attribute1 = ";
        bdeu_PrintMethods::print(stream, d_attribute1,
                                 -levelPlus1, spacesPerLevel);

        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "Attribute2 = ";
        bdeu_PrintMethods::print(stream, d_attribute2,
                                 -levelPlus1, spacesPerLevel);

        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "TheContent = ";
        bdeu_PrintMethods::print(stream, d_theContent,
                                 -levelPlus1, spacesPerLevel);

        bdeu_Print::indent(stream, level, spacesPerLevel);
        stream << "]\n";
    }
    else {
        // single line

        stream << '[';

        stream << ' ';
        stream << "Attribute1 = ";
        bdeu_PrintMethods::print(stream, d_attribute1,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "Attribute2 = ";
        bdeu_PrintMethods::print(stream, d_attribute2,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "TheContent = ";
        bdeu_PrintMethods::print(stream, d_theContent,
                                 -levelPlus1, spacesPerLevel);

        stream << " ]";
    }

    return stream << bsl::flush;
}

                          // ------------------------
                          // class MySimpleIntContent
                          // ------------------------

// CONSTANTS

const char MySimpleIntContent::CLASS_NAME[] = "MySimpleIntContent";
    // the name of this class

const bdeat_AttributeInfo MySimpleIntContent::ATTRIBUTE_INFO_ARRAY[] = {
    {
        ATTRIBUTE_ID_ATTRIBUTE1,
        "Attribute1",                 // name
        sizeof("Attribute1") - 1,     // name length
        "Attribute1: TBD: Provide annotation",  // annotation
        bdeat_FormattingMode::BDEAT_TEXT // formatting mode
      | bdeat_FormattingMode::BDEAT_ATTRIBUTE
    },
    {
        ATTRIBUTE_ID_ATTRIBUTE2,
        "Attribute2",                 // name
        sizeof("Attribute2") - 1,     // name length
        "Attribute2: TBD: Provide annotation",  // annotation
        bdeat_FormattingMode::BDEAT_TEXT // formatting mode
      | bdeat_FormattingMode::BDEAT_ATTRIBUTE
    },
    {
        ATTRIBUTE_ID_THE_CONTENT,
        "TheContent",                 // name
        sizeof("TheContent") - 1,     // name length
        "TheContent: TBD: Provide annotation",  // annotation
        bdeat_FormattingMode::BDEAT_DEC // formatting mode
      | bdeat_FormattingMode::BDEAT_SIMPLE_CONTENT
    }
};

// CLASS METHODS

const bdeat_AttributeInfo *MySimpleIntContent::lookupAttributeInfo(
        const char *name,
        int         nameLength)
{
    switch(nameLength) {
        case 10: {
            switch(bdeu_CharType::toUpper(name[0])) {
                case 'A': {
                    if (bdeu_CharType::toUpper(name[1])=='T'
                     && bdeu_CharType::toUpper(name[2])=='T'
                     && bdeu_CharType::toUpper(name[3])=='R'
                     && bdeu_CharType::toUpper(name[4])=='I'
                     && bdeu_CharType::toUpper(name[5])=='B'
                     && bdeu_CharType::toUpper(name[6])=='U'
                     && bdeu_CharType::toUpper(name[7])=='T'
                     && bdeu_CharType::toUpper(name[8])=='E')
                    {
                        switch(bdeu_CharType::toUpper(name[9])) {
                            case '1': {
                                return &ATTRIBUTE_INFO_ARRAY[
                                            ATTRIBUTE_INDEX_ATTRIBUTE1
                                       ];
                            } break;
                            case '2': {
                                return &ATTRIBUTE_INFO_ARRAY[
                                            ATTRIBUTE_INDEX_ATTRIBUTE2
                                       ];
                            } break;
                        }
                    }
                } break;
                case 'T': {
                    if (bdeu_CharType::toUpper(name[1])=='H'
                     && bdeu_CharType::toUpper(name[2])=='E'
                     && bdeu_CharType::toUpper(name[3])=='C'
                     && bdeu_CharType::toUpper(name[4])=='O'
                     && bdeu_CharType::toUpper(name[5])=='N'
                     && bdeu_CharType::toUpper(name[6])=='T'
                     && bdeu_CharType::toUpper(name[7])=='E'
                     && bdeu_CharType::toUpper(name[8])=='N'
                     && bdeu_CharType::toUpper(name[9])=='T')
                    {
                        return &ATTRIBUTE_INFO_ARRAY[
                                    ATTRIBUTE_INDEX_THE_CONTENT
                               ];
                    }
                } break;
            }
        } break;
    }
    return 0;
}

const bdeat_AttributeInfo *MySimpleIntContent::lookupAttributeInfo(int id)
{
    switch (id) {
      case ATTRIBUTE_ID_ATTRIBUTE1:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE1];
      case ATTRIBUTE_ID_ATTRIBUTE2:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE2];
      case ATTRIBUTE_ID_THE_CONTENT:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_THE_CONTENT];
      default:
        return 0;
    }
}

// ACCESSORS

bsl::ostream& MySimpleIntContent::print(
    bsl::ostream& stream,
    int           level,
    int           spacesPerLevel) const
{
    if (level < 0) {
        level = -level;
    }
    else {
        bdeu_Print::indent(stream, level, spacesPerLevel);
    }

    int levelPlus1 = level + 1;

    if (0 <= spacesPerLevel) {
        // multiline

        stream << "[\n";

        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "Attribute1 = ";
        bdeu_PrintMethods::print(stream, d_attribute1,
                                 -levelPlus1, spacesPerLevel);

        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "Attribute2 = ";
        bdeu_PrintMethods::print(stream, d_attribute2,
                                 -levelPlus1, spacesPerLevel);

        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "TheContent = ";
        bdeu_PrintMethods::print(stream, d_theContent,
                                 -levelPlus1, spacesPerLevel);

        bdeu_Print::indent(stream, level, spacesPerLevel);
        stream << "]\n";
    }
    else {
        // single line

        stream << '[';

        stream << ' ';
        stream << "Attribute1 = ";
        bdeu_PrintMethods::print(stream, d_attribute1,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "Attribute2 = ";
        bdeu_PrintMethods::print(stream, d_attribute2,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "TheContent = ";
        bdeu_PrintMethods::print(stream, d_theContent,
                                 -levelPlus1, spacesPerLevel);

        stream << " ]";
    }

    return stream << bsl::flush;
}

                 // -----------------------------------------
                 // class MySequenceWithAnonymousChoiceChoice
                 // -----------------------------------------

// CONSTANTS

const char MySequenceWithAnonymousChoiceChoice::CLASS_NAME[]
                    = "MySequenceWithAnonymousChoiceChoice";
    // the name of this class

const bdeat_SelectionInfo
                MySequenceWithAnonymousChoiceChoice::SELECTION_INFO_ARRAY[] = {
    {
        SELECTION_ID_MY_CHOICE1,
        "MyChoice1",                   // name
        sizeof("MyChoice1") - 1,       // name length
        "MyChoice1: TBD: Provide annotation",  // annotation
        bdeat_FormattingMode::BDEAT_DEC // formatting mode
    },
    {
        SELECTION_ID_MY_CHOICE2,
        "MyChoice2",                   // name
        sizeof("MyChoice2") - 1,       // name length
        "MyChoice2: TBD: Provide annotation",  // annotation
        bdeat_FormattingMode::BDEAT_TEXT // formatting mode
    }
};

// CLASS METHODS

const bdeat_SelectionInfo *
MySequenceWithAnonymousChoiceChoice::lookupSelectionInfo(
        const char *name,
        int         nameLength)
{
    switch(nameLength) {
        case 9: {
            if (bdeu_CharType::toUpper(name[0])=='M'
             && bdeu_CharType::toUpper(name[1])=='Y'
             && bdeu_CharType::toUpper(name[2])=='C'
             && bdeu_CharType::toUpper(name[3])=='H'
             && bdeu_CharType::toUpper(name[4])=='O'
             && bdeu_CharType::toUpper(name[5])=='I'
             && bdeu_CharType::toUpper(name[6])=='C'
             && bdeu_CharType::toUpper(name[7])=='E')
            {
                switch(bdeu_CharType::toUpper(name[8])) {
                    case '1': {
                        return &SELECTION_INFO_ARRAY[
                                    SELECTION_INDEX_MY_CHOICE1
                               ];
                    } break;
                    case '2': {
                        return &SELECTION_INFO_ARRAY[
                                    SELECTION_INDEX_MY_CHOICE2
                               ];
                    } break;
                }
            }
        } break;
    }
    return 0;
}

const bdeat_SelectionInfo *
MySequenceWithAnonymousChoiceChoice::lookupSelectionInfo(int id)
{
    switch (id) {
      case SELECTION_ID_MY_CHOICE1:
        return &SELECTION_INFO_ARRAY[SELECTION_INDEX_MY_CHOICE1];
      case SELECTION_ID_MY_CHOICE2:
        return &SELECTION_INFO_ARRAY[SELECTION_INDEX_MY_CHOICE2];
      default:
        return 0;
    }
}

// ACCESSORS

bsl::ostream& MySequenceWithAnonymousChoiceChoice::print(
    bsl::ostream& stream,
    int           level,
    int           spacesPerLevel) const
{
    if (level < 0) {
        level = -level;
    }
    else {
        bdeu_Print::indent(stream, level, spacesPerLevel);
    }

    int levelPlus1 = level + 1;

    if (0 <= spacesPerLevel) {
        // multiline

        stream << "[\n";
        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);

        switch (d_selectionId) {
          case SELECTION_ID_MY_CHOICE1: {
            stream << "MyChoice1 = ";
            bdeu_PrintMethods::print(stream, d_myChoice1.object(),
                                     -levelPlus1, spacesPerLevel);
          } break;
          case SELECTION_ID_MY_CHOICE2: {
            stream << "MyChoice2 = ";
            bdeu_PrintMethods::print(stream, d_myChoice2.object(),
                                     -levelPlus1, spacesPerLevel);
          } break;
          default:
            stream << "SELECTION UNDEFINED\n";
        }
        bdeu_Print::indent(stream, level, spacesPerLevel);
        stream << "]\n";
    }
    else {
        // single line

        stream << "[ ";

        switch (d_selectionId) {
          case SELECTION_ID_MY_CHOICE1: {
            stream << "MyChoice1 = ";
            bdeu_PrintMethods::print(stream, d_myChoice1.object(),
                                     -levelPlus1, spacesPerLevel);
          } break;
          case SELECTION_ID_MY_CHOICE2: {
            stream << "MyChoice2 = ";
            bdeu_PrintMethods::print(stream, d_myChoice2.object(),
                                     -levelPlus1, spacesPerLevel);
          } break;
          default:
            stream << "SELECTION UNDEFINED";
        }

        stream << " ]";
    }

    return stream << bsl::flush;
}

                       // -----------------------------
                       // class MySequenceWithNullables
                       // -----------------------------

// CONSTANTS

const char MySequenceWithNullables::CLASS_NAME[] = "MySequenceWithNullables";
    // the name of this class

const bdeat_AttributeInfo MySequenceWithNullables::ATTRIBUTE_INFO_ARRAY[] = {
    {
        ATTRIBUTE_ID_ATTRIBUTE1,
        "Attribute1",                 // name
        sizeof("Attribute1") - 1,     // name length
        "Attribute1: TBD: Provide annotation",  // annotation
        bdeat_FormattingMode::BDEAT_DEC // formatting mode
    },
    {
        ATTRIBUTE_ID_ATTRIBUTE2,
        "Attribute2",                 // name
        sizeof("Attribute2") - 1,     // name length
        "Attribute2: TBD: Provide annotation",  // annotation
        bdeat_FormattingMode::BDEAT_TEXT // formatting mode
    },
    {
        ATTRIBUTE_ID_ATTRIBUTE3,
        "Attribute3",                 // name
        sizeof("Attribute3") - 1,     // name length
        "Attribute3: TBD: Provide annotation",  // annotation
        bdeat_FormattingMode::BDEAT_DEFAULT // formatting mode
    }
};

// CLASS METHODS

const bdeat_AttributeInfo *MySequenceWithNullables::lookupAttributeInfo(
        const char *name,
        int         nameLength)
{
    switch(nameLength) {
        case 10: {
            if (bdeu_CharType::toUpper(name[0])=='A'
             && bdeu_CharType::toUpper(name[1])=='T'
             && bdeu_CharType::toUpper(name[2])=='T'
             && bdeu_CharType::toUpper(name[3])=='R'
             && bdeu_CharType::toUpper(name[4])=='I'
             && bdeu_CharType::toUpper(name[5])=='B'
             && bdeu_CharType::toUpper(name[6])=='U'
             && bdeu_CharType::toUpper(name[7])=='T'
             && bdeu_CharType::toUpper(name[8])=='E')
            {
                switch(bdeu_CharType::toUpper(name[9])) {
                    case '1': {
                        return &ATTRIBUTE_INFO_ARRAY[
                                    ATTRIBUTE_INDEX_ATTRIBUTE1
                               ];
                    } break;
                    case '2': {
                        return &ATTRIBUTE_INFO_ARRAY[
                                    ATTRIBUTE_INDEX_ATTRIBUTE2
                               ];
                    } break;
                    case '3': {
                        return &ATTRIBUTE_INFO_ARRAY[
                                    ATTRIBUTE_INDEX_ATTRIBUTE3
                               ];
                    } break;
                }
            }
        } break;
    }
    return 0;
}

const bdeat_AttributeInfo *MySequenceWithNullables::lookupAttributeInfo(int id)
{
    switch (id) {
      case ATTRIBUTE_ID_ATTRIBUTE1:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE1];
      case ATTRIBUTE_ID_ATTRIBUTE2:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE2];
      case ATTRIBUTE_ID_ATTRIBUTE3:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE3];
      default:
        return 0;
    }
}

// ACCESSORS

bsl::ostream& MySequenceWithNullables::print(
    bsl::ostream& stream,
    int           level,
    int           spacesPerLevel) const
{
    if (level < 0) {
        level = -level;
    }
    else {
        bdeu_Print::indent(stream, level, spacesPerLevel);
    }

    int levelPlus1 = level + 1;

    if (0 <= spacesPerLevel) {
        // multiline

        stream << "[\n";

        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "Attribute1 = ";
        bdeu_PrintMethods::print(stream, d_attribute1,
                                 -levelPlus1, spacesPerLevel);

        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "Attribute2 = ";
        bdeu_PrintMethods::print(stream, d_attribute2,
                                 -levelPlus1, spacesPerLevel);

        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "Attribute3 = ";
        bdeu_PrintMethods::print(stream, d_attribute3,
                                 -levelPlus1, spacesPerLevel);

        bdeu_Print::indent(stream, level, spacesPerLevel);
        stream << "]\n";
    }
    else {
        // single line

        stream << '[';

        stream << ' ';
        stream << "Attribute1 = ";
        bdeu_PrintMethods::print(stream, d_attribute1,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "Attribute2 = ";
        bdeu_PrintMethods::print(stream, d_attribute2,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "Attribute3 = ";
        bdeu_PrintMethods::print(stream, d_attribute3,
                                 -levelPlus1, spacesPerLevel);

        stream << " ]";
    }

    return stream << bsl::flush;
}

                               // --------------
                               // class Employee
                               // --------------

// CONSTANTS

const char Employee::CLASS_NAME[] = "Employee";
    // the name of this class

const bdeat_AttributeInfo Employee::ATTRIBUTE_INFO_ARRAY[] = {
    {
        ATTRIBUTE_ID_NAME,
        "name",                 // name
        sizeof("name") - 1,     // name length
        "name: TBD: Provide annotation",  // annotation
        bdeat_FormattingMode::BDEAT_TEXT // formatting mode
    },
    {
        ATTRIBUTE_ID_HOME_ADDRESS,
        "homeAddress",                 // name
        sizeof("homeAddress") - 1,     // name length
        "homeAddress: TBD: Provide annotation",  // annotation
        bdeat_FormattingMode::BDEAT_DEFAULT // formatting mode
    },
    {
        ATTRIBUTE_ID_AGE,
        "age",                 // name
        sizeof("age") - 1,     // name length
        "age: TBD: Provide annotation",  // annotation
        bdeat_FormattingMode::BDEAT_DEC // formatting mode
    }
};

// CLASS METHODS

const bdeat_AttributeInfo *Employee::lookupAttributeInfo(
        const char *name,
        int         nameLength)
{
    switch(nameLength) {
        case 3: {
            if (bdeu_CharType::toUpper(name[0])=='A'
             && bdeu_CharType::toUpper(name[1])=='G'
             && bdeu_CharType::toUpper(name[2])=='E')
            {
                return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_AGE];
            }
        } break;
        case 4: {
            if (bdeu_CharType::toUpper(name[0])=='N'
             && bdeu_CharType::toUpper(name[1])=='A'
             && bdeu_CharType::toUpper(name[2])=='M'
             && bdeu_CharType::toUpper(name[3])=='E')
            {
                return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_NAME];
            }
        } break;
        case 11: {
            if (bdeu_CharType::toUpper(name[0])=='H'
             && bdeu_CharType::toUpper(name[1])=='O'
             && bdeu_CharType::toUpper(name[2])=='M'
             && bdeu_CharType::toUpper(name[3])=='E'
             && bdeu_CharType::toUpper(name[4])=='A'
             && bdeu_CharType::toUpper(name[5])=='D'
             && bdeu_CharType::toUpper(name[6])=='D'
             && bdeu_CharType::toUpper(name[7])=='R'
             && bdeu_CharType::toUpper(name[8])=='E'
             && bdeu_CharType::toUpper(name[9])=='S'
             && bdeu_CharType::toUpper(name[10])=='S')
            {
                return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_HOME_ADDRESS];
            }
        } break;
    }
    return 0;
}

const bdeat_AttributeInfo *Employee::lookupAttributeInfo(int id)
{
    switch (id) {
      case ATTRIBUTE_ID_NAME:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_NAME];
      case ATTRIBUTE_ID_HOME_ADDRESS:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_HOME_ADDRESS];
      case ATTRIBUTE_ID_AGE:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_AGE];
      default:
        return 0;
    }
}

// ACCESSORS

bsl::ostream& Employee::print(
    bsl::ostream& stream,
    int           level,
    int           spacesPerLevel) const
{
    if (level < 0) {
        level = -level;
    }
    else {
        bdeu_Print::indent(stream, level, spacesPerLevel);
    }

    int levelPlus1 = level + 1;

    if (0 <= spacesPerLevel) {
        // multiline

        stream << "[\n";

        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "Name = ";
        bdeu_PrintMethods::print(stream, d_name,
                                 -levelPlus1, spacesPerLevel);

        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "HomeAddress = ";
        bdeu_PrintMethods::print(stream, d_homeAddress,
                                 -levelPlus1, spacesPerLevel);

        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "Age = ";
        bdeu_PrintMethods::print(stream, d_age,
                                 -levelPlus1, spacesPerLevel);

        bdeu_Print::indent(stream, level, spacesPerLevel);
        stream << "]\n";
    }
    else {
        // single line

        stream << '[';

        stream << ' ';
        stream << "Name = ";
        bdeu_PrintMethods::print(stream, d_name,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "HomeAddress = ";
        bdeu_PrintMethods::print(stream, d_homeAddress,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "Age = ";
        bdeu_PrintMethods::print(stream, d_age,
                                 -levelPlus1, spacesPerLevel);

        stream << " ]";
    }

    return stream << bsl::flush;
}

                    // -----------------------------------
                    // class MySequenceWithAnonymousChoice
                    // -----------------------------------

// CONSTANTS

const char MySequenceWithAnonymousChoice::CLASS_NAME[]
            = "MySequenceWithAnonymousChoice";
    // the name of this class

const bdeat_AttributeInfo
                      MySequenceWithAnonymousChoice::ATTRIBUTE_INFO_ARRAY[] = {
    {
        ATTRIBUTE_ID_ATTRIBUTE1,
        "Attribute1",                 // name
        sizeof("Attribute1") - 1,     // name length
        "Attribute1: TBD: Provide annotation",  // annotation
        bdeat_FormattingMode::BDEAT_DEC // formatting mode
    },
    {
        ATTRIBUTE_ID_MY_SEQUENCE_WITH_ANONYMOUS_CHOICE_CHOICE,
        "MySequenceWithAnonymousChoiceChoice",                 // name
        sizeof("MySequenceWithAnonymousChoiceChoice") - 1,     // name length
        "MySequenceWithAnonymousChoiceChoice: TBD: Provide annotation",
                                                               // annotation
        bdeat_FormattingMode::BDEAT_DEFAULT // formatting mode
      | bdeat_FormattingMode::BDEAT_UNTAGGED
    },
    {
        ATTRIBUTE_ID_ATTRIBUTE2,
        "Attribute2",                 // name
        sizeof("Attribute2") - 1,     // name length
        "Attribute2: TBD: Provide annotation",  // annotation
        bdeat_FormattingMode::BDEAT_TEXT // formatting mode
    }
};

// CLASS METHODS

const bdeat_AttributeInfo *MySequenceWithAnonymousChoice::lookupAttributeInfo(
        const char *name,
        int         nameLength)
{
    if (bdeu_String::areEqualCaseless("MyChoice1", name, nameLength)) {
        return &ATTRIBUTE_INFO_ARRAY[
                    ATTRIBUTE_INDEX_MY_SEQUENCE_WITH_ANONYMOUS_CHOICE_CHOICE
               ];
    }

    if (bdeu_String::areEqualCaseless("MyChoice2", name, nameLength)) {
        return &ATTRIBUTE_INFO_ARRAY[
                    ATTRIBUTE_INDEX_MY_SEQUENCE_WITH_ANONYMOUS_CHOICE_CHOICE
               ];
    }

    switch(nameLength) {
        case 10: {
            if (bdeu_CharType::toUpper(name[0])=='A'
             && bdeu_CharType::toUpper(name[1])=='T'
             && bdeu_CharType::toUpper(name[2])=='T'
             && bdeu_CharType::toUpper(name[3])=='R'
             && bdeu_CharType::toUpper(name[4])=='I'
             && bdeu_CharType::toUpper(name[5])=='B'
             && bdeu_CharType::toUpper(name[6])=='U'
             && bdeu_CharType::toUpper(name[7])=='T'
             && bdeu_CharType::toUpper(name[8])=='E')
            {
                switch(bdeu_CharType::toUpper(name[9])) {
                    case '1': {
                        return &ATTRIBUTE_INFO_ARRAY[
                                    ATTRIBUTE_INDEX_ATTRIBUTE1
                               ];
                    } break;
                    case '2': {
                        return &ATTRIBUTE_INFO_ARRAY[
                                    ATTRIBUTE_INDEX_ATTRIBUTE2
                               ];
                    } break;
                }
            }
        } break;
        case 35: {
            if (bdeu_CharType::toUpper(name[0])=='M'
             && bdeu_CharType::toUpper(name[1])=='Y'
             && bdeu_CharType::toUpper(name[2])=='S'
             && bdeu_CharType::toUpper(name[3])=='E'
             && bdeu_CharType::toUpper(name[4])=='Q'
             && bdeu_CharType::toUpper(name[5])=='U'
             && bdeu_CharType::toUpper(name[6])=='E'
             && bdeu_CharType::toUpper(name[7])=='N'
             && bdeu_CharType::toUpper(name[8])=='C'
             && bdeu_CharType::toUpper(name[9])=='E'
             && bdeu_CharType::toUpper(name[10])=='W'
             && bdeu_CharType::toUpper(name[11])=='I'
             && bdeu_CharType::toUpper(name[12])=='T'
             && bdeu_CharType::toUpper(name[13])=='H'
             && bdeu_CharType::toUpper(name[14])=='A'
             && bdeu_CharType::toUpper(name[15])=='N'
             && bdeu_CharType::toUpper(name[16])=='O'
             && bdeu_CharType::toUpper(name[17])=='N'
             && bdeu_CharType::toUpper(name[18])=='Y'
             && bdeu_CharType::toUpper(name[19])=='M'
             && bdeu_CharType::toUpper(name[20])=='O'
             && bdeu_CharType::toUpper(name[21])=='U'
             && bdeu_CharType::toUpper(name[22])=='S'
             && bdeu_CharType::toUpper(name[23])=='C'
             && bdeu_CharType::toUpper(name[24])=='H'
             && bdeu_CharType::toUpper(name[25])=='O'
             && bdeu_CharType::toUpper(name[26])=='I'
             && bdeu_CharType::toUpper(name[27])=='C'
             && bdeu_CharType::toUpper(name[28])=='E'
             && bdeu_CharType::toUpper(name[29])=='C'
             && bdeu_CharType::toUpper(name[30])=='H'
             && bdeu_CharType::toUpper(name[31])=='O'
             && bdeu_CharType::toUpper(name[32])=='I'
             && bdeu_CharType::toUpper(name[33])=='C'
             && bdeu_CharType::toUpper(name[34])=='E')
            {
                return
                    &ATTRIBUTE_INFO_ARRAY[
                       ATTRIBUTE_INDEX_MY_SEQUENCE_WITH_ANONYMOUS_CHOICE_CHOICE
                    ];
            }
        } break;
    }
    return 0;
}

const bdeat_AttributeInfo *
MySequenceWithAnonymousChoice::lookupAttributeInfo(int id)
{
    switch (id) {
      case ATTRIBUTE_ID_ATTRIBUTE1:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE1];
      case ATTRIBUTE_ID_MY_SEQUENCE_WITH_ANONYMOUS_CHOICE_CHOICE:
        return &ATTRIBUTE_INFO_ARRAY[
                    ATTRIBUTE_INDEX_MY_SEQUENCE_WITH_ANONYMOUS_CHOICE_CHOICE
               ];
      case ATTRIBUTE_ID_ATTRIBUTE2:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE2];
      default:
        return 0;
    }
}

// ACCESSORS

bsl::ostream& MySequenceWithAnonymousChoice::print(
    bsl::ostream& stream,
    int           level,
    int           spacesPerLevel) const
{
    if (level < 0) {
        level = -level;
    }
    else {
        bdeu_Print::indent(stream, level, spacesPerLevel);
    }

    int levelPlus1 = level + 1;

    if (0 <= spacesPerLevel) {
        // multiline

        stream << "[\n";

        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "Attribute1 = ";
        bdeu_PrintMethods::print(stream, d_attribute1,
                                 -levelPlus1, spacesPerLevel);

        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "MySequenceWithAnonymousChoiceChoice = ";
        bdeu_PrintMethods::print(stream, d_mySequenceWithAnonymousChoiceChoice,
                                 -levelPlus1, spacesPerLevel);

        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "Attribute2 = ";
        bdeu_PrintMethods::print(stream, d_attribute2,
                                 -levelPlus1, spacesPerLevel);

        bdeu_Print::indent(stream, level, spacesPerLevel);
        stream << "]\n";
    }
    else {
        // single line

        stream << '[';

        stream << ' ';
        stream << "Attribute1 = ";
        bdeu_PrintMethods::print(stream, d_attribute1,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "MySequenceWithAnonymousChoiceChoice = ";
        bdeu_PrintMethods::print(stream, d_mySequenceWithAnonymousChoiceChoice,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "Attribute2 = ";
        bdeu_PrintMethods::print(stream, d_attribute2,
                                 -levelPlus1, spacesPerLevel);

        stream << " ]";
    }

    return stream << bsl::flush;
}

                       // -----------------------------
                       // class MySequenceWithNillables
                       // -----------------------------

// CONSTANTS

const char MySequenceWithNillables::CLASS_NAME[] = "MySequenceWithNillables";
    // the name of this class

const bdeat_AttributeInfo MySequenceWithNillables::ATTRIBUTE_INFO_ARRAY[] = {
    {
        ATTRIBUTE_ID_ATTRIBUTE1,
        "Attribute1",                 // name
        sizeof("Attribute1") - 1,     // name length
        "Attribute1: TBD: Provide annotation",  // annotation
        bdeat_FormattingMode::BDEAT_DEC // formatting mode
      | bdeat_FormattingMode::BDEAT_NILLABLE
    },
    {
        ATTRIBUTE_ID_ATTRIBUTE2,
        "Attribute2",                 // name
        sizeof("Attribute2") - 1,     // name length
        "Attribute2: TBD: Provide annotation",  // annotation
        bdeat_FormattingMode::BDEAT_TEXT // formatting mode
      | bdeat_FormattingMode::BDEAT_NILLABLE
    },
    {
        ATTRIBUTE_ID_ATTRIBUTE3,
        "Attribute3",                 // name
        sizeof("Attribute3") - 1,     // name length
        "Attribute3: TBD: Provide annotation",  // annotation
        bdeat_FormattingMode::BDEAT_DEFAULT // formatting mode
      | bdeat_FormattingMode::BDEAT_NILLABLE
    }
};

// CLASS METHODS

const bdeat_AttributeInfo *MySequenceWithNillables::lookupAttributeInfo(
        const char *name,
        int         nameLength)
{
    switch(nameLength) {
        case 10: {
            if (bdeu_CharType::toUpper(name[0])=='A'
             && bdeu_CharType::toUpper(name[1])=='T'
             && bdeu_CharType::toUpper(name[2])=='T'
             && bdeu_CharType::toUpper(name[3])=='R'
             && bdeu_CharType::toUpper(name[4])=='I'
             && bdeu_CharType::toUpper(name[5])=='B'
             && bdeu_CharType::toUpper(name[6])=='U'
             && bdeu_CharType::toUpper(name[7])=='T'
             && bdeu_CharType::toUpper(name[8])=='E')
            {
                switch(bdeu_CharType::toUpper(name[9])) {
                    case '1': {
                        return &ATTRIBUTE_INFO_ARRAY[
                                    ATTRIBUTE_INDEX_ATTRIBUTE1
                               ];
                    } break;
                    case '2': {
                        return &ATTRIBUTE_INFO_ARRAY[
                                    ATTRIBUTE_INDEX_ATTRIBUTE2
                               ];
                    } break;
                    case '3': {
                        return &ATTRIBUTE_INFO_ARRAY[
                                    ATTRIBUTE_INDEX_ATTRIBUTE3
                               ];
                    } break;
                }
            }
        } break;
    }
    return 0;
}

const bdeat_AttributeInfo *MySequenceWithNillables::lookupAttributeInfo(int id)
{
    switch (id) {
      case ATTRIBUTE_ID_ATTRIBUTE1:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE1];
      case ATTRIBUTE_ID_ATTRIBUTE2:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE2];
      case ATTRIBUTE_ID_ATTRIBUTE3:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE3];
      default:
        return 0;
    }
}

// ACCESSORS

bsl::ostream& MySequenceWithNillables::print(
    bsl::ostream& stream,
    int           level,
    int           spacesPerLevel) const
{
    if (level < 0) {
        level = -level;
    }
    else {
        bdeu_Print::indent(stream, level, spacesPerLevel);
    }

    int levelPlus1 = level + 1;

    if (0 <= spacesPerLevel) {
        // multiline

        stream << "[\n";

        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "Attribute1 = ";
        bdeu_PrintMethods::print(stream, d_attribute1,
                                 -levelPlus1, spacesPerLevel);

        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "Attribute2 = ";
        bdeu_PrintMethods::print(stream, d_attribute2,
                                 -levelPlus1, spacesPerLevel);

        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "Attribute3 = ";
        bdeu_PrintMethods::print(stream, d_attribute3,
                                 -levelPlus1, spacesPerLevel);

        bdeu_Print::indent(stream, level, spacesPerLevel);
        stream << "]\n";
    }
    else {
        // single line

        stream << '[';

        stream << ' ';
        stream << "Attribute1 = ";
        bdeu_PrintMethods::print(stream, d_attribute1,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "Attribute2 = ";
        bdeu_PrintMethods::print(stream, d_attribute2,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "Attribute3 = ";
        bdeu_PrintMethods::print(stream, d_attribute3,
                                 -levelPlus1, spacesPerLevel);

        stream << " ]";
    }

    return stream << bsl::flush;
}

}  // close namespace test;
}  // close namespace BloombergLP;

// GENERATED BY BLP_BAS_CODEGEN_2.0.2_DEV_LATEST Thu Dec 28 20:58:29 2006
// ----------------------------------------------------------------------------
// *End-of-file block removed*
// ----------------------------------------------------------------------------

// ***** END OF GENERATED CODE ****

//=============================================================================
//                               USAGE EXAMPLE
//-----------------------------------------------------------------------------

///Usage Example 1
///---------------
// Suppose we have the following XML schema inside a file called
// 'employee.xsd':
//..
//  <?xml version='1.0' encoding='UTF-8'?>
//  <xs:schema xmlns:xs='http://www.w3.org/2001/XMLSchema'
//             xmlns:test='http://bloomberg.com/schemas/test'
//             targetNamespace='http://bloomberg.com/schemas/test'
//             elementFormDefault='unqualified'>
//
//      <xs:complexType name='Address'>
//          <xs:sequence>
//              <xs:element name='street' type='xs:string'/>
//              <xs:element name='city'   type='xs:string'/>
//              <xs:element name='state'  type='xs:string'/>
//          </xs:sequence>
//      </xs:complexType>
//
//      <xs:complexType name='Employee'>
//          <xs:sequence>
//              <xs:element name='name'        type='xs:string'/>
//              <xs:element name='homeAddress' type='test:Address'/>
//              <xs:element name='age'         type='xs:int'/>
//          </xs:sequence>
//      </xs:complexType>
//
//      <xs:element name='Address' type='test:Address'/>
//      <xs:element name='Employee' type='test:Employee'/>
//
//  </xs:schema>
//..
// Using the 'bde_xsdcc.pl' tool, we can generate C++ classes for this schema:
//..
//  $ bde_xsdcc.pl -g h -g cpp -p test xsdfile.xsd
//..
// This tool will generate the header and implementation files for the
// 'test_address' and 'test_employee' components in the current directory.
//
// The following function decodes an XML string into a 'test::Employee' object
// and verifies the results:
//..
//  #include <test_employee.h>

    #include <baexml_decoder.h>
    #include <baexml_decoderoptions.h>
    #include <baexml_errorinfo.h>
    #include <baexml_minireader.h>
    #include <bsl_sstream.h>

    using namespace BloombergLP;

    void usageExample1()
    {
        const char INPUT[] = "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                             "<Employee>\n"
                             "    <name>Bob</name>\n"
                             "    <homeAddress>\n"
                             "        <street>Some Street</street>\n"
                             "        <city>Some City</city>\n"
                             "        <state>Some State</state>\n"
                             "    </homeAddress>\n"
                             "    <age>21</age>\n"
                             "</Employee>\n";

        bsl::stringstream ss(INPUT);

        test::Employee bob;

        baexml_DecoderOptions options;
        baexml_MiniReader     reader;
        baexml_ErrorInfo      errInfo;

        baexml_Decoder decoder(&options, &reader, &errInfo);

        decoder.decode(ss, &bob);

        ASSERT(ss);
        ASSERT("Bob"         == bob.name());
        ASSERT("Some Street" == bob.homeAddress().street());
        ASSERT("Some City"   == bob.homeAddress().city());
        ASSERT("Some State"  == bob.homeAddress().state());
        ASSERT(21            == bob.age());
    }
//..
//
///Usage Example 2
///---------------
// The following snippets of code illustrate how to pass an error stream and
// warning stream to the 'decode' function.  We will use the same
// 'test_employee' component from the previous usage example.  Note that the
// input XML string contains an error.  (The 'homeAddress' object has an
// element called 'country', which does not exist in the schema.):
//..
//  #include <test_employee.h>

    #include <baexml_decoder.h>
    #include <baexml_decoderoptions.h>
    #include <baexml_errorinfo.h>
    #include <baexml_minireader.h>
    #include <bsl_sstream.h>

    using namespace BloombergLP;

    int usageExample2()
    {
        const char INPUT[] = "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                             "<Employee>\n"
                             "    <name>Bob</name>\n"
                             "    <homeAddress>\n"
                             "        <street>Some Street</street>\n"
                             "        <city>Some City</city>\n"
                             "        <state>Some State</state>\n"
                             "        <country>Some Country</country>\n"
                             "    </homeAddress>\n"
                             "    <age>21</age>\n"
                             "</Employee>\n";

        bsl::stringstream ss(INPUT);

        test::Employee bob;

        baexml_DecoderOptions options;
        baexml_MiniReader     reader;
        baexml_ErrorInfo      errInfo;

        options.setSkipUnknownElements(false);
        baexml_Decoder decoder(&options, &reader, &errInfo,
                               &bsl::cerr, &bsl::cerr);
        decoder.decode(ss, &bob, "employee.xml");

        ASSERT(!ss);

        return 0;
    }
//..
// Note that the input stream is invalidated to indicate that an error
// occurred.  Also note that the following error message will be printed on
// 'bsl::cerr':
//..
//  employee.xml:8.18: Error: Unable to decode sub-element 'country'.\n"
//  employee.xml:8.18: Error: Unable to decode sub-element 'homeAddress'.\n";
//..
// The following snippets of code illustrate how to open decoder and read the
// first node before calling 'decode':
//..
    int usageExample3()
    {
        const char INPUT[] =
            "<?xml version='1.0' encoding='UTF-8' ?>\n"
            "<Employee xmlns='http://www.bde.com/bdem_test'>\n"
            "    <name>Bob</name>\n"
            "    <homeAddress>\n"
            "        <street>Some Street</street>\n"
            "        <state>Some State</state>\n"
            "        <city>Some City</city>\n"
            "        <country>Some Country</country>\n"
            "    </homeAddress>\n"
            "    <age>21</age>\n"
            "</Employee>\n";

        baexml_MiniReader     reader;
        baexml_ErrorInfo      errInfo;
        baexml_DecoderOptions options;

        baexml_Decoder decoder(&options, &reader, &errInfo,
                               &bsl::cerr, &bsl::cerr);

//..
// Now we open the document, but we don't begin decoding yet:
//..
        int rc = decoder.open(INPUT, sizeof(INPUT) - 1);
        ASSERT(0 == rc);
//..
// Depending on the value of the first node, we can now determine whether the
// document is an 'Address' object or an 'Employee' object, and construct the
// target object accordingly:
//..
        if (0 == bsl::strcmp(reader.nodeLocalName(), "Address")) {
            test::Address addr;
            rc = decoder.decode(&addr);
            bsl::cout << addr;
        }
        else {
            test::Employee bob;
            rc = decoder.decode(&bob);
            bsl::cout << bob;
        }

        ASSERT(0 == rc);
//..
// When decoding is complete, we must close the decoder object:
//..
        decoder.close();
        return 0;
    }
//..

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    verbose = argc > 2;
    veryVerbose = argc > 3;
    veryVeryVerbose = argc > 4;
    veryVeryVeryVerbose = argc > 5;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;;

    switch (test) { case 0:  // Zero is always the leading case.
      case 16: {
        // --------------------------------------------------------------------
        // TESTING USAGE EXAMPLES
        //
        // Concerns:
        //   The usage examples must compile and run as expected.
        //
        // Plan:
        //   Copy the usage examples from the component-level documentation,
        //   replace 'assert' with 'ASSERT', and check that they compile and
        //   run as expected.
        //
        // Testing:
        //   USAGE EXAMPLES
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting Usage Examples"
                          << "\n======================" << endl;

        usageExample1();

        const char USAGE2_ERRORS[] =
         "employee.xml:8.18: Error: Unable to decode sub-element 'country'.\n"
         "employee.xml:8.18: Error: Unable to decode sub-element"
            " 'homeAddress'.\n";

        // Redirect standard error to 'errorStream'.
        bsl::stringstream errorStream;
        bsl::streambuf *cerrBuf = bsl::cerr.rdbuf();
        bsl::cerr.rdbuf(errorStream.rdbuf());
        usageExample2();
        bsl::cerr.rdbuf(cerrBuf);
        LOOP_ASSERT(errorStream.str(), USAGE2_ERRORS == errorStream.str());
        if (verbose) bsl::cout << errorStream.str();

        // Redirect standard output to 'outStream' and standard error to
        // 'errorStream'.
        bsl::stringstream outStream;
        bsl::streambuf *coutBuf = bsl::cout.rdbuf();
        errorStream.str("");
        bsl::cout.rdbuf(outStream.rdbuf());
        bsl::cerr.rdbuf(errorStream.rdbuf());
        usageExample3();
        bsl::cerr.rdbuf(cerrBuf);
        bsl::cout.rdbuf(coutBuf);
        LOOP_ASSERT(errorStream.str(), errorStream.str().empty());
        if (verbose) bsl::cout << outStream.str() << bsl::endl;

      } break;
      case 15: {
        // --------------------------------------------------------------------
        // TESTING XML SCHEMA PARSING AND BDEM BINDING ADAPTOR DECODING
        //
        // Concerns:
        //   The schema should be correctly parsed and response should be
        //   properly decoded.
        //
        // Plan:
        //   Parse a given schema and decode an XML element corresponding to
        //   that schema, use 'baexml_SchemaParser' to parse the schema and
        //   use a binding adaptor with 'baexml_Decoder' to decode the
        //   element.
        //
        // Testing:
        //   static bsl::istream& decode(istream&,
        //                               TYPE,
        //                               ostream&,
        //                               ostream&)
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "\nTESTING XML SCHEMA PARSING AND "
                                  "BDEM BINDING ADAPTOR DECODING"
                                  "\n-------------------------------"
                                  "-----------------------------" << bsl::endl;

        const char SCHEMA_STR[] =
         "<?xml version='1.0'?>\n"
          "<schema xmlns='http://www.w3.org/2001/XMLSchema'\n"
            "xmlns:bdem='http://bloomberg.com/schemas/bdem'\n"
            "xmlns:fxc='http://bloomberg.com/schemas/fxc'\n"
            "targetNamespace='http://bloomberg.com/schemas/fxc'\n"
            "bdem:requestType='Request'\n"
            "bdem:responseType='Response'\n"
            "bdem:serviceName='fxrlsvc'\n"
            "elementFormDefault='qualified'>\n"
         "<complexType name='DocumentListResponse'>\n"
         "<sequence>\n"
             "<element name='TotalCount' type='int' />\n"
             "<element name='Name' type='string' />\n"
         "</sequence>\n"
         "</complexType>\n"
         "<complexType name='Request'>\n"
         "<sequence>\n"
               "<element name='Test' type='string' />\n"
         "</sequence>\n"
         "</complexType>\n"
         "<complexType name='Response'>\n"
         " <choice>\n"
         "  <element name='Error' type='string'/>\n"
         "  <element name='DocumentListResponse' "
                                         "type='fxc:DocumentListResponse' />\n"
         " </choice>\n"
         "</complexType>\n"
         "<element name='Request' type='fxc:Request' />\n"
         "<element name='Response' type='fxc:Response' />\n"
         "</schema>";

        const char DATA[] =
         "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
         "<Response xmlns=\"http://bloomberg.com/schemas/fxc\">\n"
         "<DocumentListResponse>\n"
         "<TotalCount>4</TotalCount>\n"
         "<Name>Economie Internationale</Name>\n"
         "</DocumentListResponse>\n"
         "</Response>\n";

        bsl::istringstream xmlStream(SCHEMA_STR);

        baexml_MiniReader reader;
        baexml_ErrorInfo errInfo;
        baexml_SchemaParser parser(&reader, &errInfo);
        bcema_SharedPtr<bdem_Schema> schemaPtr;  schemaPtr.createInplace();
        bsl::string targetNamespace;

        if (veryVerbose) {
            parser.setVerboseStream(&bsl::cout);
        }

        // parse the schema
        int result = parser.parse(xmlStream,
                                  schemaPtr.ptr(),
                                  &targetNamespace);

        ASSERT(0 == result);
        if (veryVerbose) { bsl::cout << errInfo << bsl::endl; }

        bdesb_FixedMemInStreamBuf dataStreamBuf(DATA, sizeof(DATA));

        bcem_Aggregate aggregate(schemaPtr,
                                 "Response",
                                 bdem_ElemType::BDEM_CHOICE);

        reader.close();
        errInfo.reset();

        baexml_DecoderOptions options;

        baexml_Decoder decoder(&options, &reader, &errInfo,
                               &bsl::cerr, &bsl::cerr);

        decoder.decode(&dataStreamBuf, &aggregate);

        if (verbose) cout << "\nEnd of Test." << endl;

      } break;
      case 14: {
        // --------------------------------------------------------------------
        // TESTING 'decode' FUNCTIONS
        //   This will test the 'decode' functions in the 'Decoder'
        //   namespace.
        //
        // Concerns:
        //   The main functionality has already been tested in previous test
        //   cases.  In this test, we only have the following concerns:
        //
        //       - the value passed in is reset before parsing.
        //       - the 'Decoder_SelectContext' meta-function is used
        //         correctly.
        //       - the XML name for the object is correctly passed to the
        //         'Decoder_ParserUtil::parse' function.
        //       - the versions that use 'istream' instead of 'streambuf' for
        //         input should invalidate the stream if there is an error.
        //
        // Plan:
        //   Use 'TestSequence2' for the 'TYPE' parameter.  Exercise each
        //   function with valid and invalid input.  Check that the return
        //   value is as expected and, if successful, check that the object
        //   contains the expected value.  For the versions that use 'istream',
        //   check that the input stream is invalidated if there is an error.
        //
        // Testing:
        //   int baexml_Decoder::decode(sbuf*, TYPE, b_A*);
        //   int baexml_Decoder::decode(sbuf*, TYPE, ostrm&, ostrm&, b_A*);
        //   int baexml_Decoder::decode(istrm&, TYPE, b_A*);
        //   int baexml_Decoder::decode(istrm&, TYPE, ostrm&, ostrm&, b_A*);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting 'decode' Functions"
                          << "\n==========================" << endl;

        typedef TestSequence2 TS;  // shorthand

        static const struct {
            int         d_lineNum;  // source line number
            const char *d_input;    // input string
            int         d_retCode;  // expected ret code
            TS          d_result;   // expected result
        } DATA[] = {
            ///line  input                  retCode  result
            ///----  -----                  -------  ------
            { L_,   "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                    "<TestSequence2>\n"
                    "</TestSequence2>\n",  0,       TS()                     },

            { L_,   "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                    "<TestSequence2>\n"
                    "  <E1>123</E1>\n"
                    "</TestSequence2>\n",  0,       TS(123,
                                                       TS::DEFAULT_ELEMENT2) },

            { L_,   "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                    "<TestSequence2>\n"
                    "  <E2>abc</E2>\n"
                    "</TestSequence2>\n",  0,       TS(TS::DEFAULT_ELEMENT1,
                                                       "abc")                },

            { L_,   "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                    "<TestSequence2>\n"
                    "  <E1>123</E1>\n"
                    "  <E2>abc</E2>\n"
                    "</TestSequence2>\n",  0,       TS(123, "abc")           },

            { L_,   "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                    "<TestSequence2>\n"
                    "  <E1>123</E1>\n"
                    "  <E2_Wrong>123</E2_Wrong>\n"
                    "</TestSequence2>\n",  1,       TS(123,
                                                       TS::DEFAULT_ELEMENT2) },

            { L_,   "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                    "<TestSequence2>\n"
                    "  <E2>abc</E2>\n"
                    "  <E1>blah</E1>\n"
                    "</TestSequence2>\n",  1,       TS(TS::DEFAULT_ELEMENT1,
                                                       "abc")                },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        const TS INIT_VALUE(9876, "AA");

        if (verbose) cout << "\nTesting 'Decoder::decode(streambuf*, "
                          << "TYPE*)'." << endl;
        for (int i = 0; i < NUM_DATA; ++i) {
            const int   LINE              = DATA[i].d_lineNum;
            const char *INPUT             = DATA[i].d_input;
            const int   EXPECTED_RET_CODE = DATA[i].d_retCode;
            const TS    EXPECTED_RESULT   = DATA[i].d_result;

            if (veryVerbose) {
                T_ P(LINE)

                if (veryVeryVerbose) {
                    T_ T_ P(INPUT)
                    T_ T_ P_(EXPECTED_RET_CODE) P(EXPECTED_RESULT)
                }
            }

            bsl::stringstream input(INPUT);
            TS                result = INIT_VALUE;
            baexml_DecoderOptions    options;
            options.setSkipUnknownElements(false);

            baexml_MiniReader reader;
            baexml_ErrorInfo  errInfo;

            baexml_Decoder decoder(&options, &reader, &errInfo);

            int retCode = decoder.decode(input.rdbuf(), &result);

            if (0 == EXPECTED_RET_CODE) {
                LOOP2_ASSERT(LINE, retCode, 0 == retCode);
                LOOP3_ASSERT(LINE, EXPECTED_RESULT,   result,
                                   EXPECTED_RESULT == result);
            }
            else {
                LOOP2_ASSERT(LINE, retCode, 0 != retCode);
            }
        }

        if (verbose) cout << "\nTesting 'Decoder::decode(streambuf*, "
                          << "TYPE*, ostream&, ostream&)'."
                          << endl;
        for (int i = 0; i < NUM_DATA; ++i) {
            const int   LINE              = DATA[i].d_lineNum;
            const char *INPUT             = DATA[i].d_input;
            const int   EXPECTED_RET_CODE = DATA[i].d_retCode;
            const TS    EXPECTED_RESULT   = DATA[i].d_result;

            if (veryVerbose) {
                T_ P(LINE)

                if (veryVeryVerbose) {
                    T_ T_ P(INPUT)
                    T_ T_ P_(EXPECTED_RET_CODE) P(EXPECTED_RESULT)
                }
            }

            bsl::stringstream input(INPUT);
            TS                result = INIT_VALUE;
            bsl::ostream      nullStream(0);
            baexml_DecoderOptions    options;
            options.setSkipUnknownElements(false);

            // display error messages on 'bsl::cerr' only if errors are not
            // expected or if very very very verbose
            bsl::ostream& outStream
                              = (0 == EXPECTED_RET_CODE || veryVeryVeryVerbose)
                                ? bsl::cerr
                                : nullStream;

            baexml_MiniReader reader;
            baexml_ErrorInfo  errInfo;

            baexml_Decoder decoder(&options, &reader, &errInfo,
                                   &outStream, &outStream);

            int retCode = decoder.decode(input.rdbuf(), &result);

            if (0 == EXPECTED_RET_CODE) {
                LOOP2_ASSERT(LINE, retCode, 0 == retCode);
                LOOP3_ASSERT(LINE, EXPECTED_RESULT,   result,
                                   EXPECTED_RESULT == result);
            }
            else {
                LOOP2_ASSERT(LINE, retCode, 0 != retCode);
            }
        }

        if (verbose) cout << "\nTesting 'Decoder::decode(istream&, "
                          << "TYPE*)'." << endl;
        for (int i = 0; i < NUM_DATA; ++i) {
            const int   LINE              = DATA[i].d_lineNum;
            const char *INPUT             = DATA[i].d_input;
            const int   EXPECTED_RET_CODE = DATA[i].d_retCode;
            const TS    EXPECTED_RESULT   = DATA[i].d_result;

            if (veryVerbose) {
                T_ P(LINE)

                if (veryVeryVerbose) {
                    T_ T_ P(INPUT)
                    T_ T_ P_(EXPECTED_RET_CODE) P(EXPECTED_RESULT)
                }
            }

            bsl::stringstream input(INPUT);
            TS                result = INIT_VALUE;
            baexml_DecoderOptions    options;
            options.setSkipUnknownElements(false);

            LOOP_ASSERT(LINE, input.good());

            baexml_MiniReader reader;
            baexml_ErrorInfo  errInfo;

            baexml_Decoder decoder(&options, &reader, &errInfo);

            decoder.decode(input, &result);

            if (0 == EXPECTED_RET_CODE) {
                LOOP_ASSERT(LINE, !input.fail());
                LOOP3_ASSERT(LINE, EXPECTED_RESULT,   result,
                                   EXPECTED_RESULT == result);
            }
            else {
                LOOP_ASSERT(LINE, input.fail());
            }
        }

        if (verbose) cout << "\nTesting 'Decoder::decode(istream&, "
                          << "TYPE*, ostream&, ostream&)'."
                          << endl;
        for (int i = 0; i < NUM_DATA; ++i) {
            const int   LINE              = DATA[i].d_lineNum;
            const char *INPUT             = DATA[i].d_input;
            const int   EXPECTED_RET_CODE = DATA[i].d_retCode;
            const TS    EXPECTED_RESULT   = DATA[i].d_result;

            if (veryVerbose) {
                T_ P(LINE)

                if (veryVeryVerbose) {
                    T_ T_ P(INPUT)
                    T_ T_ P_(EXPECTED_RET_CODE) P(EXPECTED_RESULT)
                }
            }

            bsl::stringstream input(INPUT);
            TS                result = INIT_VALUE;
            bsl::ostream      nullStream(0);
            baexml_DecoderOptions    options;
            options.setSkipUnknownElements(false);

            // display error messages on 'bsl::cerr' only if errors are not
            // expected or if very very very verbose
            bsl::ostream& outStream
                              = (0 == EXPECTED_RET_CODE || veryVeryVeryVerbose)
                                ? bsl::cerr
                                : nullStream;

            LOOP_ASSERT(LINE, input.good());

            baexml_MiniReader reader;
            baexml_ErrorInfo  errInfo;
            baexml_Decoder  decoder(&options, &reader, &errInfo,
                                    &outStream, &outStream);
            decoder.decode(input, &result);

            if (0 == EXPECTED_RET_CODE) {
                LOOP_ASSERT(LINE, !input.fail());
                LOOP3_ASSERT(LINE, EXPECTED_RESULT,   result,
                                   EXPECTED_RESULT == result);
            }
            else {
                LOOP_ASSERT(LINE, input.fail());
            }
        }

        if (verbose) cout << "\nEnd of 'decode' Functions Test." << endl;
      } break;
      case 13: {
        // --------------------------------------------------------------------
        // TESTING DECODING OF SIMPLE CONTENT
        //   This will test decoding of simple content.
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting Decoding of Simple Content"
                          << "\n==================================" << endl;

        if (verbose) cout << "\nUsing 'MySimpleContent'." << endl;
        {
            typedef test::MySimpleContent Type;

            Type EXPECTED_RESULT[3];

            EXPECTED_RESULT[0].attribute1() = true;
            EXPECTED_RESULT[0].attribute2() = "Hello World!";
            EXPECTED_RESULT[0].theContent() = "";

            EXPECTED_RESULT[1].attribute1() = false;
            EXPECTED_RESULT[1].attribute2() = "Hello World!";
            EXPECTED_RESULT[1].theContent() = "Some Stuff";

            EXPECTED_RESULT[2].attribute1() = true;
            EXPECTED_RESULT[2].attribute2() = "Hello World!";
            EXPECTED_RESULT[2].theContent() = "  Some Stuff ";

            const char *INPUT[3]
                            = {
                              "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                              "<MySimpleContent Attribute1=\"true\" "
                              "Attribute2=\"Hello World!\">"
                              ""
                              "</MySimpleContent>\n",

                              "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                              "<MySimpleContent Attribute1=\"false\" "
                              "Attribute2=\"Hello World!\">"
                              "Some Stuff"
                              "</MySimpleContent>\n",

                              "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                              "<MySimpleContent Attribute1=\"true\" "
                              "Attribute2=\"Hello World!\">"
                              "  Some Stuff "
                              "</MySimpleContent>\n",
                              };
            const int NUM_INPUT = sizeof INPUT / sizeof *INPUT;

            for (int i = 0; i < NUM_INPUT; ++i) {
                Type mX;  const Type& X = mX;

                const Type& Y = EXPECTED_RESULT[i];

                if (veryVerbose) {
                    T_ P_(i) P_(Y) P(INPUT[i])
                }

                LOOP3_ASSERT(i, X, Y, X != Y);

                bsl::stringstream input(INPUT[i]);

                baexml_MiniReader reader;
                baexml_ErrorInfo  errInfo;
                baexml_DecoderOptions    options;

                baexml_Decoder decoder(&options, &reader, &errInfo,
                                       &bsl::cerr, &bsl::cerr);

                decoder.decode(input, &mX);

                LOOP_ASSERT(i, input);
                LOOP3_ASSERT(i, X, Y, X == Y);
            }
        }

        if (verbose) cout << "\nUsing 'MySimpleIntContent'." << endl;
        {
            typedef test::MySimpleIntContent Type;

            Type EXPECTED_RESULT[2];

            EXPECTED_RESULT[0].attribute1() = true;
            EXPECTED_RESULT[0].attribute2() = "Hello World!";
            EXPECTED_RESULT[0].theContent() = 34;

            EXPECTED_RESULT[1].attribute1() = false;
            EXPECTED_RESULT[1].attribute2() = "Hello World!";
            EXPECTED_RESULT[1].theContent() = 34;

            const char *INPUT[2]
                            = {
                              "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                              "<MySimpleIntContent Attribute1=\"true\" "
                              "Attribute2=\"Hello World!\">"
                              "34"
                              "</MySimpleIntContent>\n",

                              "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                              "<MySimpleIntContent Attribute1=\"false\" "
                              "Attribute2=\"Hello World!\">"
                              "  34 "
                              "</MySimpleIntContent>\n",
                              };
            const int NUM_INPUT = sizeof INPUT / sizeof *INPUT;

            for (int i = 0; i < NUM_INPUT; ++i) {
                Type mX;  const Type& X = mX;

                const Type& Y = EXPECTED_RESULT[i];

                if (veryVerbose) {
                    T_ P_(i) P_(Y) P(INPUT[i])
                }

                LOOP3_ASSERT(i, X, Y, X != Y);

                bsl::stringstream input(INPUT[i]);

                baexml_MiniReader reader;
                baexml_ErrorInfo  errInfo;
                baexml_DecoderOptions    options;

                baexml_Decoder decoder(&options, &reader, &errInfo,
                                       &bsl::cerr, &bsl::cerr);

                decoder.decode(input, &mX);

                LOOP_ASSERT(i, input);
                LOOP3_ASSERT(i, X, Y, X == Y);
            }
        }

        if (verbose) cout << "\nEnd of Test." << endl;
      } break;
      case 12: {
        // --------------------------------------------------------------------
        // TESTING DECODING OF XML ATTRIBUTES
        //   This will test decoding of XML attributes.
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting Decoding of XML Attributes"
                          << "\n==================================" << endl;

        if (verbose) cout << "\nUsing 'MySequenceWithAttributes'." << endl;
        {
            typedef test::MySequenceWithAttributes Type;

            Type EXPECTED_RESULT[6];

            EXPECTED_RESULT[0].attribute1() = 34;
            EXPECTED_RESULT[0].element1() = 45;
            EXPECTED_RESULT[0].element2() = "Hello";

            EXPECTED_RESULT[1].attribute1() = 34;
            EXPECTED_RESULT[1].attribute2() = "World!";
            EXPECTED_RESULT[1].element1() = 45;
            EXPECTED_RESULT[1].element2() = "Hello";

            EXPECTED_RESULT[2].attribute1() = 34;
            EXPECTED_RESULT[2].attribute2() = "  World ! ";
            EXPECTED_RESULT[2].element1() = 45;
            EXPECTED_RESULT[2].element2() = "Hello";

            EXPECTED_RESULT[3].attribute1() = 34;
            EXPECTED_RESULT[3].attribute2() = "  World ! ";

            EXPECTED_RESULT[4].attribute1() = 34;

            EXPECTED_RESULT[5].attribute1() = 34;

            const char *INPUT[6]
                            = {
                              "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                              "<MySequenceWithAttributes Attribute1=\"34\">\n"
                              "    <Element1>45</Element1>\n"
                              "    <Element2>Hello</Element2>\n"
                              "</MySequenceWithAttributes>\n",

                              "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                              "<MySequenceWithAttributes Attribute1=\"34\" "
                              "Attribute2=\"World!\">\n"
                              "    <Element1>45</Element1>\n"
                              "    <Element2>Hello</Element2>\n"
                              "</MySequenceWithAttributes>\n",

                              "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                              "<MySequenceWithAttributes Attribute1=\"34\" "
                              "Attribute2=\"  World ! \">\n"
                              "    <Element1>45</Element1>\n"
                              "    <Element2>Hello</Element2>\n"
                              "</MySequenceWithAttributes>\n",

                              "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                              "<MySequenceWithAttributes Attribute1=\"34\" "
                              "Attribute2=\"  World ! \">\n"
                              "</MySequenceWithAttributes>\n",

                              "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                              "<MySequenceWithAttributes Attribute1=\"34\">\n"
                              "</MySequenceWithAttributes>\n",

                              "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                              "<MySequenceWithAttributes Attribute1=\"34\"/>\n"
                              };
            const int NUM_INPUT = sizeof INPUT / sizeof *INPUT;

            for (int i = 0; i < NUM_INPUT; ++i) {
                Type mX;  const Type& X = mX;

                const Type& Y = EXPECTED_RESULT[i];

                if (veryVerbose) {
                    T_ P_(i) P_(Y) P(INPUT[i])
                }

                LOOP3_ASSERT(i, X, Y, X != Y);

                bsl::stringstream input(INPUT[i]);

                baexml_MiniReader reader;
                baexml_ErrorInfo  errInfo;
                baexml_DecoderOptions    options;

                baexml_Decoder decoder(&options, &reader, &errInfo,
                                       &bsl::cerr, &bsl::cerr);

                decoder.decode(input, &mX);

                LOOP_ASSERT(i, input);
                LOOP3_ASSERT(i, X, Y, X == Y);
            }
        }

        if (verbose) cout << "\nEnd of Test." << endl;
      } break;
      case 11: {
        // --------------------------------------------------------------------
        // TESTING DECODING OF ANONYMOUS CHOICE
        //   This will test decoding of anonymous choices.
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting Decoding of Anonymous Choice"
                          << "\n====================================" << endl;

        if (verbose) cout << "\nUsing 'MySequenceWithAnonymousChoice'."
                          << endl;
        {
            typedef test::MySequenceWithAnonymousChoice Type;

            Type EXPECTED_RESULT[4];

            EXPECTED_RESULT[0].attribute1() = 34;
            EXPECTED_RESULT[0].mySequenceWithAnonymousChoiceChoice().
                makeMyChoice1(67);
            EXPECTED_RESULT[0].attribute2() = "Hello";

            EXPECTED_RESULT[1].attribute1() = 34;
            EXPECTED_RESULT[1].mySequenceWithAnonymousChoiceChoice().
                makeMyChoice1(67);
            EXPECTED_RESULT[1].attribute2() = "Hello";

            EXPECTED_RESULT[2].attribute1() = 34;
            EXPECTED_RESULT[2].mySequenceWithAnonymousChoiceChoice().
                makeMyChoice2("World!");
            EXPECTED_RESULT[2].attribute2() = "Hello";

            EXPECTED_RESULT[3].attribute1() = 34;
            EXPECTED_RESULT[3].mySequenceWithAnonymousChoiceChoice().
                makeMyChoice2("  World! ");
            EXPECTED_RESULT[3].attribute2() = "Hello";

            const char *INPUT[4]
                            = {
                                "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                                "<MySequenceWithAnonymousChoice>\n"
                                "    <Attribute1>34</Attribute1>\n"
                                "    <MyChoice1>67</MyChoice1>\n"
                                "    <Attribute2>Hello</Attribute2>\n"
                                "</MySequenceWithAnonymousChoice>\n",

                                "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                                "<MySequenceWithAnonymousChoice>\n"
                                "    <Attribute1>34</Attribute1>\n"
                                "    <MyChoice1>  67 </MyChoice1>\n"
                                "    <Attribute2>Hello</Attribute2>\n"
                                "</MySequenceWithAnonymousChoice>\n",

                                "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                                "<MySequenceWithAnonymousChoice>\n"
                                "    <Attribute1>34</Attribute1>\n"
                                "    <MyChoice2>World!</MyChoice2>\n"
                                "    <Attribute2>Hello</Attribute2>\n"
                                "</MySequenceWithAnonymousChoice>\n",

                                "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                                "<MySequenceWithAnonymousChoice>\n"
                                "    <Attribute1>34</Attribute1>\n"
                                "    <MyChoice2>  World! </MyChoice2>\n"
                                "    <Attribute2>Hello</Attribute2>\n"
                                "</MySequenceWithAnonymousChoice>\n",
                              };
            const int NUM_INPUT = sizeof INPUT / sizeof *INPUT;

            for (int i = 0; i < NUM_INPUT; ++i) {
                Type mX;  const Type& X = mX;

                const Type& Y = EXPECTED_RESULT[i];

                if (veryVerbose) {
                    T_ P_(i) P_(Y) P(INPUT[i])
                }

                LOOP3_ASSERT(i, X, Y, X != Y);

                bsl::stringstream input(INPUT[i]);

                baexml_MiniReader reader;
                baexml_ErrorInfo  errInfo;
                baexml_DecoderOptions    options;

                baexml_Decoder decoder(&options, &reader, &errInfo,
                                       &bsl::cerr, &bsl::cerr);

                decoder.decode(input, &mX);

                LOOP_ASSERT(i, input);
                LOOP3_ASSERT(i, X, Y, X == Y);
            }
        }

        if (verbose) cout << "\nEnd of Test." << endl;
      } break;
      case 10: {
        // --------------------------------------------------------------------
        // TESTING baexml_Decoder_StdVectorCharContext
        //   This will test the 'Decoder_StdVectorCharContext' class
        //   template.
        //
        // Concerns:
        //   This context must be able to select and initialize an appropriate
        //   "implementation" context, based on the formatting mode.  It should
        //   also forward all calls to this "implementation" context
        //   correctly.
        //
        // Plan:
        //   For each item in a set of test data, create an instance of the
        //   'Decoder_StdVectorCharContext' class and associate it with an
        //   'bsl::vector<char>' object that has been set to some arbitrary
        //   initial value, and a formatting mode from the test vector.  Call
        //   the 'Decoder_ParserUtil::parse' function using the context and
        //   ensure that the associated 'bsl::vector<char>' has the expected
        //   value.
        //
        // Testing:
        //   baexml_Decoder_StdVectorCharContext
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting baexml_Decoder_StdVectorCharContext"
                          << "\n========================================"
                          << endl;

        const bsl::string EXPECTED_RESULT_DATA = "abcd";

        enum FormattingMode {
            DEFAULT = bdeat_FormattingMode::BDEAT_DEFAULT,
            BASE64  = bdeat_FormattingMode::BDEAT_BASE64,
            HEX     = bdeat_FormattingMode::BDEAT_HEX,
            IS_LIST = bdeat_FormattingMode::BDEAT_LIST,
            TEXT    = bdeat_FormattingMode::BDEAT_TEXT,
            LIST_OR_DEC = IS_LIST | bdeat_FormattingMode::BDEAT_DEC
        };

        static const struct {
            int             d_lineNum;  // source line number
            const char     *d_input;    // input string
            FormattingMode  d_mode;     // formatting mode
        } DATA[] = {
            //line  input                                 mode
            //----  -----                                 ----

            { L_,   "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                    "<Value>YWJjZA==</Value>\n",          DEFAULT,  },
            { L_,   "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                    "<Value>YWJjZA==</Value>\n",          BASE64,   },
            { L_,   "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                    "<Value>61626364</Value>\n",          HEX,      },
            { L_,   "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                    "<Value>97 98 99 100</Value>\n",      IS_LIST,     },
            { L_,   "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                    "<Value>abcd</Value>\n",              TEXT,     },
            { L_,   "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                    "<Value>97 98 99 100</Value>\n",      LIST_OR_DEC, },
        };
        const int NUM_DATA  = sizeof DATA / sizeof *DATA;
        const int MAX_DEPTH = 5;

        for (int i = 0; i < NUM_DATA; ++i) {
            const int             LINE            = DATA[i].d_lineNum;
            const char           *INPUT           = DATA[i].d_input;
            const FormattingMode  FORMATTING_MODE = DATA[i].d_mode;

            if (veryVerbose) {
                T_ P(LINE)

                if (veryVeryVerbose) {
                    T_ T_ P_(INPUT) P((int)FORMATTING_MODE)
                }
            }

            const bsl::string INIT_VALUE = "qwer";

            bsl::stringstream ss(INPUT);
            bsl::vector<char> result1(INIT_VALUE.data(),
                                      INIT_VALUE.data() + INIT_VALUE.size());
            bsl::ostream      nullStream(0);

            // display error messages on 'bsl::cerr' only if errors are not
            // expected or if very very very verbose
            bsl::ostream& outStream = (veryVeryVeryVerbose)
                                      ? bsl::cerr
                                      : nullStream;

            baexml_DecoderOptions options;
            options.setMaxDepth(MAX_DEPTH);

            baexml_Decoder_StdVectorCharContext context(&result1,
                                                     FORMATTING_MODE);

            baexml_MiniReader reader;
            baexml_ErrorInfo  errInfo;
            baexml_Decoder    decoder(&options, &reader, &errInfo,
                 &outStream, &outStream);

            decoder.open(ss.rdbuf());
            int retCode = context.beginParse(&decoder);;

            const bsl::vector<char> EXPECTED_RESULT(
                                                EXPECTED_RESULT_DATA.data(),
                                                EXPECTED_RESULT_DATA.data()
                                                + EXPECTED_RESULT_DATA.size());

            LOOP2_ASSERT(LINE, retCode, 0 == retCode);
            LOOP3_ASSERT(LINE, EXPECTED_RESULT,   result1,
                               EXPECTED_RESULT == result1);
        }

        if (verbose)
            cout << "\nEnd of baexml_Decoder_StdVectorCharContext Test."
                 << endl;
      } break;
      case 9: {
        // --------------------------------------------------------------------
        // TESTING baexml_Decoder_StdStringContext
        //   This will test the 'Decoder_StdStringContext' class template.
        //
        // Concerns:
        //   This context must be able to select and initialize an appropriate
        //   "implementation" context, based on the formatting mode.  It should
        //   also forward all calls to this "implementation" context
        //   correctly.
        //
        // Plan:
        //   For each item in a set of test data, create an instance of the
        //   'Decoder_StdStringContext' class and associate it with an
        //   'bsl::string' object that has been set to some arbitrary initial
        //   value, and a formatting mode from the test vector.  Call the
        //   'Decoder_ParserUtil::parse' function using the context and
        //   ensure that the associated 'bsl::string' has the expected value.
        //
        // Testing:
        //   baexml_Decoder_StdStringContext
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting baexml_Decoder_StdStringContext"
                          << "\n===================================="
                          << endl;

        const bsl::string EXPECTED_RESULT = "abcd";

        enum FormattingMode {
            DEFAULT = bdeat_FormattingMode::BDEAT_DEFAULT,
            TEXT    = bdeat_FormattingMode::BDEAT_TEXT,
            BASE64  = bdeat_FormattingMode::BDEAT_BASE64,
            HEX     = bdeat_FormattingMode::BDEAT_HEX
        };

        static const struct {
            int             d_lineNum;  // source line number
            const char     *d_input;    // input string
            FormattingMode  d_mode;     // formatting mode
        } DATA[] = {
            //line  input                                 mode
            //----  -----                                 ----

            { L_,   "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                    "<Value>abcd</Value>\n",              DEFAULT,  },
            { L_,   "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                    "<Value>abcd</Value>\n",              TEXT,     },
            { L_,   "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                    "<Value>YWJjZA==</Value>\n",          BASE64,   },
            { L_,   "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                    "<Value>61626364</Value>\n",          HEX,      },
        };
        const int NUM_DATA  = sizeof DATA / sizeof *DATA;
        const int MAX_DEPTH = 5;

        for (int i = 0; i < NUM_DATA; ++i) {
            const int             LINE            = DATA[i].d_lineNum;
            const char           *INPUT           = DATA[i].d_input;
            const FormattingMode  FORMATTING_MODE = DATA[i].d_mode;

            if (veryVerbose) {
                T_ P(LINE)

                if (veryVeryVerbose) {
                    T_ T_ P_(INPUT) P((int)FORMATTING_MODE)
                }
            }

            const bsl::string INIT_VALUE = "qwer";

            bsl::stringstream ss(INPUT);
            bsl::string       result1 = INIT_VALUE;
            bsl::ostream      nullStream(0);

            // display error messages on 'bsl::cerr' only if errors are not
            // expected or if very very very verbose
            bsl::ostream& outStream = (veryVeryVeryVerbose)
                                      ? bsl::cerr
                                      : nullStream;

            baexml_DecoderOptions options;
            options.setMaxDepth(MAX_DEPTH);

            baexml_Decoder_StdStringContext context(&result1,
                                                 FORMATTING_MODE);

            baexml_MiniReader reader;
            baexml_ErrorInfo  errInfo;
            baexml_Decoder    decoder(&options, &reader, &errInfo,
                 &outStream, &outStream);

            decoder.open(ss.rdbuf());
            int retCode = context.beginParse(&decoder);;

            LOOP2_ASSERT(LINE, retCode, 0 == retCode);
            LOOP3_ASSERT(LINE, EXPECTED_RESULT,   result1,
                               EXPECTED_RESULT == result1);
        }

        if (verbose) cout << "\nEnd of baexml_Decoder_StdStringContext Test."
                          << endl;
      } break;
      case 8: {
        // --------------------------------------------------------------------
        // TESTING baexml_Decoder_ChoiceContext<TYPE>
        //   This will test the 'Decoder_ChoiceContext' class template.
        //
        // Concerns:
        //   This context must be able to handle choice types with varying
        //   number of selections (fixed at compile-time).  It should ensure
        //   that one and only one choice is selected.  It should also detect
        //   other invalid data such as non-whitespace characters and invalid
        //   sub-element names.  It should be able to create an appropriate
        //   sub-context for the selected choice so that the underlying parser
        //   can successfully parse the selected choice.
        //
        // Plan:
        //   Use the following types for the 'TYPE' template parameter:
        //       - TestChoice0
        //       - TestChoice1
        //       - TestChoice2
        //   Each of these types (defined above) represents a struct that
        //   supports the 'bdeat_ChoiceFunctions'.  The number at the end of
        //   the struct name indicates the number of selections in the
        //   sequence.
        //
        //   For each choice type, create a set of XML test input data.  For
        //   each item in this set of test data, create an instance of the
        //   corresponding 'Decoder_ChoiceContext<TYPE>' class and
        //   associate it with a choice object that has been set to some
        //   arbitrary initial value.  Call the
        //   'Decoder_ParserUtil::parse' function using the context and
        //   ensure that the associated choice object has the expected value.
        //
        // Testing:
        //   baexml_Decoder_ChoiceContext<TYPE>
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting baexml_Decoder_ChoiceContext<TYPE>"
                          << "\n======================================="
                          << endl;

        if (verbose) cout << "\nUsing TestChoice0." << endl;
        {
            // Here we will test the 'Decoder_ChoiceContext' class
            // template using a choice with 0 selections.  We cannot really
            // test much here.  We can only test the following errors:
            //     - invalid characters (i.e., non-whitespace).
            //     - invalid selections.
            // Note that this is a very trivial test.  We cannot test the value
            // of the decoded object (since there are no selections).  Our
            // primary concern in this test is that the 'NUM_SELECTION_RECORDS'
            // constant inside the context class is set correctly to 1 instead
            // of 0 so that the declaration of the 'd_selectionRecords' array
            // does not fail to compile.

            static const struct {
                int         d_lineNum;  // source line number
                const char *d_input;    // input string
                int         d_retCode;  // expected return code
            } DATA[] = {
                //line  input                                           retCode
                //----  -----                                           -------
                { L_,   "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                        "<RE>\n"
                        "</RE>\n",                                        2  },

                { L_,   "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                        "<RE>\n"
                        "    blah"
                        "</RE>\n",                                        4  },

                { L_,   "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                        "<RE>\n"
                        "    <S1>\n"
                        "    </S1>\n"
                        "</RE>\n",                                        3  },
            };
            const int NUM_DATA  = sizeof DATA / sizeof *DATA;
            const int MAX_DEPTH = 5;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int   LINE              = DATA[i].d_lineNum;
                const char *INPUT             = DATA[i].d_input;
                const int   EXPECTED_RET_CODE = DATA[i].d_retCode;

                if (veryVerbose) {
                    T_ P(LINE)

                    if (veryVeryVerbose) {
                        T_ T_ P(INPUT)
                        T_ T_ P(EXPECTED_RET_CODE)
                    }
                }

                bsl::stringstream ss(INPUT);
                TestChoice0       result;

                baexml_DecoderOptions options;
                options.setMaxDepth(MAX_DEPTH);
                options.setSkipUnknownElements(false);

                baexml_Decoder_ChoiceContext<TestChoice0>
                                  context(&result,
                                          bdeat_FormattingMode::BDEAT_DEFAULT);

                bsl::ostream nullStream(0);

                // display error messages on 'bsl::cerr' only if errors are not
                // expected or if very very very verbose
                bsl::ostream& outStream
                              = (0 == EXPECTED_RET_CODE || veryVeryVeryVerbose)
                                ? bsl::cerr
                                : nullStream;

                baexml_MiniReader reader;
                baexml_ErrorInfo  errInfo;
                baexml_Decoder    decoder(&options, &reader, &errInfo,
                     &outStream, &outStream);

                decoder.open(ss.rdbuf());
                int retCode = context.beginParse(&decoder);;

                if (0 == EXPECTED_RET_CODE) {
                    LOOP3_ASSERT(LINE, EXPECTED_RET_CODE,   retCode,
                                       EXPECTED_RET_CODE == retCode);
                }
                else {
                    LOOP2_ASSERT(LINE, retCode, 0 != retCode);
                }
            }
        }

        if (verbose) cout << "\nUsing TestChoice1." << endl;
        {
            // Here we will test the 'Decoder_ChoiceContext' class
            // template using a choice with 1 selection.  We will test the
            // following errors:
            //     - no choices made
            //     - more then 1 choice made
            //     - invalid characters
            //       (i.e., non-whitespace characters in non-leaf elements).
            //     - invalid selection
            //       (i.e., selection name is not found inside choice).
            //     - failure to decode selection.
            // We will also test that, if there are no errors, the decoded
            // object has the expected value.

            typedef TestChoice1 TC;  // shorthand for test choice type

            static const struct {
                int         d_lineNum;  // source line number
                const char *d_input;    // input string
                int         d_retCode;  // expected return code
                TC          d_result;   // expected result
            } DATA[] = {
                //line   input         retCode   result
                //----   -----         -------   ------
                { L_,    "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                         "<RE>\n"
                         "</RE>\n",    2                                     },

                { L_,    "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                         "<RE>\n"
                         "  <S1>123</S1>\n"
                         "</RE>\n",    0,        TC(123)                     },

                { L_,    "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                         "<RE>\n"
                         "  <S1/>\n"
                         "</RE>\n",    0,        TC(0)                       },

                { L_,    "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                         "<RE>\n"
                         "  <S1>123</S1>\n"
                         "  <S1>456</S1>\n"
                         "</RE>\n",    0,        TC(456)                     },

                { L_,    "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                         "<RE>\n"
                         "  <S1>123</S1>\n"
                         "  <S1>456</S1>\n"
                         "  <S1>789</S1>\n"
                         "</RE>\n",    0,        TC(789)                     },

                { L_,    "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                         "<RE>\n"
                         "  <S1_Wrong>123</S1_Wrong>\n"
                         "</RE>\n",    3                                     },

                { L_,    "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                         "<RE>\n"
                         "  <S1>blah</S1>\n"
                         "</RE>\n",    4                                     },

                { L_,    "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                         "<RE>\n"
                         "  blah\n"
                         "</RE>\n",    4                                     },
            };
            const int NUM_DATA  = sizeof DATA / sizeof *DATA;
            const int MAX_DEPTH = 5;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int   LINE              = DATA[i].d_lineNum;
                const char *INPUT             = DATA[i].d_input;
                const int   EXPECTED_RET_CODE = DATA[i].d_retCode;
                const TC    EXPECTED_RESULT   = DATA[i].d_result;

                if (veryVerbose) {
                    T_ P(LINE)

                    if (veryVeryVerbose) {
                        T_ T_ P(INPUT)

                        if (0 == EXPECTED_RET_CODE) {
                            T_ T_ P_(EXPECTED_RET_CODE) P(EXPECTED_RESULT)
                        }
                        else {
                            T_ T_ P(EXPECTED_RET_CODE)
                        }
                    }
                }

                const TC INIT_VALUE = TC(9876);

                bsl::stringstream ss(INPUT);
                TC                result1 = INIT_VALUE;

                baexml_DecoderOptions options;
                options.setMaxDepth(MAX_DEPTH);
                options.setSkipUnknownElements(false);

                baexml_Decoder_ChoiceContext<TC>
                                  context(&result1,
                                          bdeat_FormattingMode::BDEAT_DEFAULT);

                bsl::ostream nullStream(0);

                // display error messages on 'bsl::cerr' only if errors are not
                // expected or if very very very verbose
                bsl::ostream& outStream
                              = (0 == EXPECTED_RET_CODE || veryVeryVeryVerbose)
                                ? bsl::cerr
                                : nullStream;

                baexml_MiniReader reader;
                baexml_ErrorInfo  errInfo;
                baexml_Decoder    decoder(&options, &reader, &errInfo,
                     &outStream, &outStream);

                decoder.open(ss.rdbuf());
                int retCode = context.beginParse(&decoder);;

                if (0 == EXPECTED_RET_CODE) {
                    LOOP3_ASSERT(LINE, EXPECTED_RET_CODE,   retCode,
                                       EXPECTED_RET_CODE == retCode);
                }
                else {
                    LOOP2_ASSERT(LINE, retCode, 0 != retCode);
                }

                if (0 == retCode) {
                    LOOP3_ASSERT(LINE, EXPECTED_RESULT,   result1,
                                       EXPECTED_RESULT == result1);
                }
            }
        }

        if (verbose) cout << "\nUsing TestChoice2." << endl;
        {
            // Here we will test the 'Decoder_ChoiceContext' class
            // template using a choice with 2 selections.  We will test the
            // following errors:
            //     - no choices made
            //     - more then 1 choice made
            //     - invalid characters
            //       (i.e., non-whitespace characters in non-leaf elements).
            //     - invalid selection
            //       (i.e., selection name is not found inside choice).
            //     - failure to decode selection.
            // We will also test that, if there are no errors, the decoded
            // object has the expected value.  Note that this test is very
            // similar to the test using a choice with 1 selection.

            typedef TestChoice2 TC;  // shorthand for test choice type

            static const struct {
                int         d_lineNum;  // source line number
                const char *d_input;    // input string
                int         d_retCode;  // expected return code
                TC          d_result;   // expected result
            } DATA[] = {
                //line   input         retCode   result
                //----   -----         -------   ------
                { L_,    "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                         "<RE>\n"
                         "</RE>\n",    2                                     },

                { L_,    "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                         "<RE>\n"
                         "  <S1>123</S1>\n"
                         "</RE>\n",    0,        TC(123)                     },

                { L_,    "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                         "<RE>\n"
                         "  <S2>abc</S2>\n"
                         "</RE>\n",    0,        TC("abc")                   },

                { L_,    "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                         "<RE>\n"
                         "  <S1/>\n"
                         "</RE>\n",    0,        TC(0)                       },

                { L_,    "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                         "<RE>\n"
                         "  <S2/>\n"
                         "</RE>\n",    0,        TC("")                      },

                { L_,    "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                         "<RE>\n"
                         "  <S1>123</S1>\n"
                         "  <S1>456</S1>\n"
                         "</RE>\n",    0,        TC(456)                     },

                { L_,    "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                         "<RE>\n"
                         "  <S1>123</S1>\n"
                         "  <S2>abc</S2>\n"
                         "</RE>\n",    3                                     },

                { L_,    "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                         "<RE>\n"
                         "  <S2>abc</S2>\n"
                         "  <S1>123</S1>\n"
                         "</RE>\n",    3                                     },

                { L_,    "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                         "<RE>\n"
                         "  <S1>123</S1>\n"
                         "  <S1>456</S1>\n"
                         "  <S1>789</S1>\n"
                         "</RE>\n",    0,        TC(789)                     },

                { L_,    "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                         "<RE>\n"
                         "  <S2>abc</S2>\n"
                         "  <S2>def</S2>\n"
                         "  <S2>ghi</S2>\n"
                         "</RE>\n",    0,        TC("ghi")                   },

                { L_,    "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                         "<RE>\n"
                         "  <S1>123</S1>\n"
                         "  <S2>abc</S2>\n"
                         "  <S1>456</S1>\n"
                         "</RE>\n",    3                                     },

                { L_,    "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                         "<RE>\n"
                         "  <S2>abc</S2>\n"
                         "  <S1>123</S1>\n"
                         "  <S2>def</S2>\n"
                         "</RE>\n",    3                                     },

                { L_,    "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                         "<RE>\n"
                         "  <S1_Wrong>123</S1_Wrong>\n"
                         "</RE>\n",    3                                     },

                { L_,    "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                         "<RE>\n"
                         "  <S2_Wrong>abc</S2_Wrong>\n"
                         "</RE>\n",    3                                     },

                { L_,    "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                         "<RE>\n"
                         "  <S1>blah</S1>\n"
                         "</RE>\n",    4                                     },

                { L_,    "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                         "<RE>\n"
                         "  blah\n"
                         "</RE>\n",    4                                     },
            };
            const int NUM_DATA  = sizeof DATA / sizeof *DATA;
            const int MAX_DEPTH = 5;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int   LINE              = DATA[i].d_lineNum;
                const char *INPUT             = DATA[i].d_input;
                const int   EXPECTED_RET_CODE = DATA[i].d_retCode;
                const TC    EXPECTED_RESULT   = DATA[i].d_result;

                if (veryVerbose) {
                    T_ P(LINE)

                    if (veryVeryVerbose) {
                        T_ T_ P(INPUT)

                        if (0 == EXPECTED_RET_CODE) {
                            T_ T_ P_(EXPECTED_RET_CODE) P(EXPECTED_RESULT)
                        }
                        else {
                            T_ T_ P(EXPECTED_RET_CODE)
                        }
                    }
                }

                const TC INIT_VALUE = TC(9876);

                bsl::stringstream ss(INPUT);
                TC                result1 = INIT_VALUE;

                baexml_DecoderOptions options;
                options.setMaxDepth(MAX_DEPTH);
                options.setSkipUnknownElements(false);

                baexml_Decoder_ChoiceContext<TC>
                                  context(&result1,
                                          bdeat_FormattingMode::BDEAT_DEFAULT);

                bsl::ostream nullStream(0);

                // display error messages on 'bsl::cerr' only if errors are not
                // expected or if very very very verbose
                bsl::ostream& outStream
                              = (0 == EXPECTED_RET_CODE || veryVeryVeryVerbose)
                                ? bsl::cerr
                                : nullStream;

                baexml_MiniReader reader;
                baexml_ErrorInfo  errInfo;
                baexml_Decoder    decoder(&options, &reader, &errInfo,
                     &outStream, &outStream);

                decoder.open(ss.rdbuf());
                int retCode = context.beginParse(&decoder);;

                if (0 == EXPECTED_RET_CODE) {
                    LOOP3_ASSERT(LINE, EXPECTED_RET_CODE,   retCode,
                                       EXPECTED_RET_CODE == retCode);
                }
                else {
                    LOOP2_ASSERT(LINE, retCode, 0 != retCode);
                }

                if (0 == retCode) {
                    LOOP3_ASSERT(LINE, EXPECTED_RESULT,   result1,
                                       EXPECTED_RESULT == result1);
                }
            }
        }

        if (verbose) cout << "\nEnd of baexml_Decoder_ChoiceContext<TYPE> "
                          << "Test." << endl;
      } break;
      case 7: {
        // --------------------------------------------------------------------
        // TESTING baexml_Decoder_SequenceContext<TYPE>
        //   This will test the 'Decoder_SequenceContext' class
        //   template.
        //
        // Concerns:
        //   This context must be able to handle sequence types with varying
        //   number of attributes (fixed at compile-time).  It should handle
        //   min/max occurrences constraints correctly.  It should also detect
        //   other invalid data such as non-whitespace characters and invalid
        //   sub-element names.  It should be able to create appropriate
        //   sub-contexts for the sub-elements inside a sequence so that the
        //   underlying parser can successfully parse these sub-elements.
        //
        // Plan:
        //   Use the following types for the 'TYPE' template parameter:
        //       - TestSequence0
        //       - TestSequence1
        //       - TestSequence2
        //   Each of these types (defined above) represents a struct that
        //   supports the 'bdeat_SequenceFunctions'.  The number at the end of
        //   the struct name indicates the number of attributes in the
        //   sequence.
        //
        //   For each sequence type, create a set of XML test input data.  For
        //   each item in this set of test data, create an instance of the
        //   corresponding 'Decoder_SequenceContext<TYPE>' class and
        //   associate it with a sequence object that has been set to some
        //   arbitrary initial value.  Call the
        //   'Decoder_ParserUtil::parse' function using the context and
        //   ensure that the associated sequence object has the expected value.
        //
        // Testing:
        //   baexml_Decoder_SequenceContext<TYPE>
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting baexml_Decoder_SequenceContext<TYPE>"
                          << "\n========================================="
                          << endl;

        if (verbose) cout << "\nUsing TestSequence0." << endl;
        {
            // Here we will test the 'Decoder_SequenceContext' class
            // template using a sequence with 0 attributes.  We cannot really
            // test min/max occurrences here.  We can only test the following
            // errors:
            //     - invalid characters (i.e., non-whitespace).
            //     - invalid sub-elements.
            // Note that this is a very trivial test.  We cannot test the value
            // of the decoded object (since there are no sub-elements).  Our
            // primary concern in this test is that the 'NUM_ATTRIBUTE_RECORDS'
            // constant inside the context class is set correctly to 1 instead
            // of 0 so that the declaration of the 'd_attributeRecords' array
            // does not fail to compile.

            static const struct {
                int         d_lineNum;  // source line number
                const char *d_input;    // input string
                int         d_retCode;  // expected return code
            } DATA[] = {
                //line  input                                           retCode
                //----  -----                                           -------
                { L_,   "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                        "<RE>\n"
                        "</RE>\n",                                        1  },

                { L_,   "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                        "<RE>\n"
                        "    blah"
                        "</RE>\n",                                        3  },

                { L_,   "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                        "<RE>\n"
                        "    <E1>\n"
                        "    </E1>\n"
                        "</RE>\n",                                        3  },
            };
            const int NUM_DATA  = sizeof DATA / sizeof *DATA;
            const int MAX_DEPTH = 5;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int   LINE              = DATA[i].d_lineNum;
                const char *INPUT             = DATA[i].d_input;
                const int   EXPECTED_RET_CODE = DATA[i].d_retCode;

                if (veryVerbose) {
                    T_ P(LINE)

                    if (veryVeryVerbose) {
                        T_ T_ P(INPUT)
                        T_ T_ P(EXPECTED_RET_CODE)
                    }
                }

                bsl::stringstream ss(INPUT);
                TestSequence0     result;

                baexml_DecoderOptions options;
                options.setMaxDepth(MAX_DEPTH);
                options.setSkipUnknownElements(false);

                baexml_Decoder_SequenceContext<TestSequence0>
                                  context(&result,
                                          bdeat_FormattingMode::BDEAT_DEFAULT);

                bsl::ostream nullStream(0);

                // display error messages on 'bsl::cerr' only if errors are not
                // expected or if very very very verbose
                bsl::ostream& outStream
                              = (0 == EXPECTED_RET_CODE || veryVeryVeryVerbose)
                                ? bsl::cerr
                                : nullStream;

                baexml_MiniReader reader;
                baexml_ErrorInfo  errInfo;
                baexml_Decoder    decoder(&options, &reader, &errInfo,
                     &outStream, &outStream);

                decoder.open(ss.rdbuf());
                int retCode = context.beginParse(&decoder);;

                if (0 == EXPECTED_RET_CODE) {
                    LOOP3_ASSERT(LINE, EXPECTED_RET_CODE,   retCode,
                                       EXPECTED_RET_CODE == retCode);
                }
                else {
                    LOOP2_ASSERT(LINE, retCode, 0 != retCode);
                }
            }
        }

        if (verbose) cout << "\nUsing TestSequence1." << endl;
        {
            // Here we will test the 'Decoder_SequenceContext' class
            // template using a sequence with 1 attribute.  We will test the
            // following errors:
            //     - insufficient occurrences
            //       (i.e., num occurrences < min occurrences).
            //     - excessive occurrences
            //       (i.e., num occurrences > max occurrences).
            //     - invalid characters
            //       (i.e., non-whitespace characters in non-leaf elements).
            //     - invalid sub-elements
            //       (i.e., sub-element name is not found inside sequence).
            //     - failure to decode sub-element.
            // We will also test that, if there are no errors, the decoded
            // object has the expected value.

            typedef TestSequence1 TS;  // shorthand for test sequence type

            const int INIT1 = 9876;  // initial (default) value for element 1

            static const struct {
                int         d_lineNum;  // source line number
                const char *d_input;    // input string
                int         d_min1;     // min occurrences for element 1
                int         d_max1;     // max occurrences for element 1
                int         d_retCode;  // expected return code
                TS          d_result;   // expected result
            } DATA[] = {
                //line   input
                //----   -----
                //       min1   max1   retCode   result
                //       ----   ----   -------   ------
                { L_,    "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                         "<RE>\n"
                         "</RE>\n",
                         0,     0,     0,        TS(INIT1)                   },

                { L_,    "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                         "<RE>\n"
                         "  <E1>123</E1>\n"
                         "</RE>\n",
                         0,     1,     0,        TS(123)                     },

                { L_,    "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                         "<RE>\n"
                         "  <E1/>\n"
                         "</RE>\n",
                         0,     1,     0,        TS(9876)                    },

                { L_,    "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                         "<RE>\n"
                         "  <E1>123</E1>\n"
                         "</RE>\n",
                         1,     1,     0,        TS(123)                     },

                { L_,    "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                         "<RE>\n"
                         "  <E1_Wrong>123</E1_Wrong>\n"
                         "</RE>\n",
                         0,     1,     2                                     },

                { L_,    "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                         "<RE>\n"
                         "  <E1>blah</E1>\n"
                         "</RE>\n",
                         0,     1,     4                                     },

                { L_,    "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                         "<RE>\n"
                         "  blah\n"
                         "</RE>\n",
                         0,     0,     2                                     },
            };
            const int NUM_DATA  = sizeof DATA / sizeof *DATA;
            const int MAX_DEPTH = 5;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int   LINE              = DATA[i].d_lineNum;
                const char *INPUT             = DATA[i].d_input;
                const int   EXPECTED_RET_CODE = DATA[i].d_retCode;
                const TS    EXPECTED_RESULT   = DATA[i].d_result;

                if (veryVerbose) {
                    T_ P(LINE)

                    if (veryVeryVerbose) {
                        T_ T_ P(INPUT)

                        if (0 == EXPECTED_RET_CODE) {
                            T_ T_ P_(EXPECTED_RET_CODE) P(EXPECTED_RESULT)
                        }
                        else {
                            T_ T_ P(EXPECTED_RET_CODE)
                        }
                    }
                }

                const TS INIT_VALUE = TS(INIT1);

                bsl::stringstream ss(INPUT);
                TS                result1 = INIT_VALUE;

                baexml_DecoderOptions options;
                options.setMaxDepth(MAX_DEPTH);
                options.setSkipUnknownElements(false);

                baexml_Decoder_SequenceContext<TS>
                                  context(&result1,
                                          bdeat_FormattingMode::BDEAT_DEFAULT);

                bsl::ostream nullStream(0);

                // display error messages on 'bsl::cerr' only if errors are not
                // expected or if very very very verbose
                bsl::ostream& outStream
                              = (0 == EXPECTED_RET_CODE || veryVeryVeryVerbose)
                                ? bsl::cerr
                                : nullStream;

                baexml_MiniReader reader;
                baexml_ErrorInfo  errInfo;
                baexml_Decoder    decoder(&options, &reader, &errInfo,
                     &outStream, &outStream);

                decoder.open(ss.rdbuf());
                int retCode = context.beginParse(&decoder);;

                if (0 == EXPECTED_RET_CODE) {
                    LOOP3_ASSERT(LINE, EXPECTED_RET_CODE,   retCode,
                                       EXPECTED_RET_CODE == retCode);
                }
                else {
                    LOOP2_ASSERT(LINE, retCode, 0 != retCode);
                }

                if (0 == retCode) {
                    LOOP3_ASSERT(LINE, EXPECTED_RESULT,   result1,
                                       EXPECTED_RESULT == result1);
                }
            }
        }

        if (verbose) cout << "\nUsing TestSequence2." << endl;
        {
            // Here we will test the 'Decoder_SequenceContext' class
            // template using a sequence with 2 attributes.  We will test the
            // following errors:
            //     - insufficient occurrences
            //       (i.e., num occurrences < min occurrences).
            //     - excessive occurrences
            //       (i.e., num occurrences > max occurrences).
            //     - invalid characters
            //       (i.e., non-whitespace characters in non-leaf elements).
            //     - invalid sub-elements
            //       (i.e., sub-element name is not found inside sequence).
            //     - failure to decode sub-element.
            // We will also test that, if there are no errors, the decoded
            // object has the expected value.  Note that this test is very
            // similar to the test using a sequence with 1 attribute.  Our main
            // concern in this test is that the context indexes the
            // 'd_attributeRecords' array correctly.

            typedef TestSequence2 TS;  // shorthand for test sequence type

            const int         INIT1 = 9876;  // initial (default) value for
                                             // element 1
            const bsl::string INIT2 = "AA";  // initial (default) value for
                                             // element 1

            static const struct {
                int         d_lineNum;  // source line number
                const char *d_input;    // input string
                int         d_min1;     // min occurrences for element 1
                int         d_max1;     // max occurrences for element 1
                int         d_min2;     // min occurrences for element 2
                int         d_max2;     // max occurrences for element 2
                int         d_retCode;  // expected return code
                TS          d_result;   // expected result
            } DATA[] = {
                //line   input
                //----   -----
                /////////min1  max1  min2  max2  retCode  result
                /////////----  ----  ----  ----  -------  ------
                { L_,    "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                         "<RE>\n"
                         "</RE>\n",
                         0,    0,    0,    0,    0,       TS(INIT1, INIT2)   },

                { L_,    "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                         "<RE>\n"
                         "  <E1>123</E1>\n"
                         "</RE>\n",
                         0,    1,    0,    0,    0,       TS(123, INIT2)     },

                { L_,    "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                         "<RE>\n"
                         "  <E2>abc</E2>\n"
                         "</RE>\n",
                         0,    0,    0,    1,    0,       TS(INIT1, "abc")   },

                { L_,    "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                         "<RE>\n"
                         "  <E1>123</E1>\n"
                         "</RE>\n",
                         1,    1,    0,    0,    0,       TS(123, INIT2)     },

                { L_,    "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                         "<RE>\n"
                         "  <E2>abc</E2>\n"
                         "</RE>\n",
                         0,    0,    1,    1,    0,       TS(INIT1, "abc")   },

                { L_,    "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                         "<RE>\n"
                         "</RE>\n",
                         1,    1,    0,    0,    0,      TS(INIT1, INIT2)    },

                { L_,    "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                         "<RE>\n"
                         "</RE>\n",
                         0,    0,    1,    1,    0,      TS(INIT1, INIT2)    },

                { L_,    "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                         "<RE>\n"
                         "  <E2>abc</E2>\n"
                         "</RE>\n",
                         1,    1,    0,    1,    0,      TS(INIT1, "abc")    },

                { L_,    "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                         "<RE>\n"
                         "  <E1>123</E1>\n"
                         "</RE>\n",
                         0,    1,    1,    1,    0,      TS(123, INIT2)      },

                { L_,    "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                         "<RE>\n"
                         "  <E1>123</E1>\n"
                         "</RE>\n",
                         0,    0,    0,    0,    0,      TS(123, INIT2)      },

                { L_,    "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                         "<RE>\n"
                         "  <E2>abc</E2>\n"
                         "</RE>\n",
                         0,    0,    0,    0,    0,      TS(INIT1, "abc")    },

                { L_,    "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                         "<RE>\n"
                         "  <E1>123</E1>\n"
                         "  <E2>abc</E2>\n"
                         "</RE>\n",
                         0,    0,    0,    0,    0,      TS(123, "abc")      },

                { L_,    "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                         "<RE>\n"
                         "  <E1/>\n"
                         "  <E2>abc</E2>\n"
                         "</RE>\n",
                         0,    0,    0,    0,    0,      TS(INIT1, "abc")    },

                { L_,    "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                         "<RE>\n"
                         "  <E1>123</E1>\n"
                         "  <E2/>\n"
                         "</RE>\n",
                         0,    0,    0,    0,    0,      TS(123, INIT2)      },

                { L_,    "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                         "<RE>\n"
                         "  <E1_Wrong>123</E1_Wrong>\n"
                         "</RE>\n",
                         0,    1,    0,    0,    2                           },

                { L_,    "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                         "<RE>\n"
                         "  <E2_Wrong>abc</E2_Wrong>\n"
                         "</RE>\n",
                         0,    0,    0,    1,    2                           },

                { L_,    "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                         "<RE>\n"
                         "  <E1>blah</E1>\n"
                         "</RE>\n",
                         0,    1,    0,    0,    4                           },

                { L_,    "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                         "<RE>\n"
                         "  blah\n"
                         "</RE>\n",
                         0,    0,    0,    0,    2                           },
            };
            const int NUM_DATA  = sizeof DATA / sizeof *DATA;
            const int MAX_DEPTH = 5;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int   LINE              = DATA[i].d_lineNum;
                const char *INPUT             = DATA[i].d_input;
                const int   EXPECTED_RET_CODE = DATA[i].d_retCode;
                const TS    EXPECTED_RESULT   = DATA[i].d_result;

                if (veryVerbose) {
                    T_ P(LINE)

                    if (veryVeryVerbose) {
                        T_ T_ P(INPUT)

                        if (0 == EXPECTED_RET_CODE) {
                            T_ T_ P_(EXPECTED_RET_CODE) P(EXPECTED_RESULT)
                        }
                        else {
                            T_ T_ P(EXPECTED_RET_CODE)
                        }
                    }
                }

                const TS INIT_VALUE = TS(INIT1, INIT2);

                bsl::stringstream ss(INPUT);
                TS                result1 = INIT_VALUE;

                baexml_DecoderOptions options;
                options.setMaxDepth(MAX_DEPTH);
                options.setSkipUnknownElements(false);

                baexml_Decoder_SequenceContext<TS>
                                  context(&result1,
                                          bdeat_FormattingMode::BDEAT_DEFAULT);

                bsl::ostream nullStream(0);

                // display error messages on 'bsl::cerr' only if errors are not
                // expected or if very very very verbose
                bsl::ostream& outStream
                              = (0 == EXPECTED_RET_CODE || veryVeryVeryVerbose)
                                ? bsl::cerr
                                : nullStream;

                baexml_MiniReader reader;
                baexml_ErrorInfo  errInfo;
                baexml_Decoder    decoder(&options, &reader, &errInfo,
                     &outStream, &outStream);

                decoder.open(ss.rdbuf());
                int retCode = context.beginParse(&decoder);;

                if (0 == EXPECTED_RET_CODE) {
                    LOOP3_ASSERT(LINE, EXPECTED_RET_CODE,   retCode,
                                       EXPECTED_RET_CODE == retCode);
                }
                else {
                    LOOP2_ASSERT(LINE, retCode, 0 != retCode);
                }

                if (0 == EXPECTED_RET_CODE) {
                    LOOP3_ASSERT(LINE, EXPECTED_RESULT,   result1,
                                       EXPECTED_RESULT == result1);
                }
            }
        }

        if (verbose) cout << "\nUsing 'MySequenceWithNullables'." << endl;
        {
            typedef test::MySequenceWithNullables Type;

            const int         ATTRIBUTE1_VALUE = 123;
            const bsl::string ATTRIBUTE2_VALUE = "test string";
            test::MySequence  ATTRIBUTE3_VALUE;

            ATTRIBUTE3_VALUE.attribute1() = 987;
            ATTRIBUTE3_VALUE.attribute2() = "inner";

            const char *INPUT[3]
                            = {
                                "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                                "<MySequenceWithNullables>\n"
                                "    <Attribute2>test string</Attribute2>\n"
                                "    <Attribute3>\n"
                                "        <Attribute1>987</Attribute1>\n"
                                "        <Attribute2>inner</Attribute2>\n"
                                "    </Attribute3>\n"
                                "</MySequenceWithNullables>\n",

                                "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                                "<MySequenceWithNullables>\n"
                                "    <Attribute1>123</Attribute1>\n"
                                "    <Attribute3>\n"
                                "        <Attribute1>987</Attribute1>\n"
                                "        <Attribute2>inner</Attribute2>\n"
                                "    </Attribute3>\n"
                                "</MySequenceWithNullables>\n",

                                "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                                "<MySequenceWithNullables>\n"
                                "    <Attribute1>123</Attribute1>\n"
                                "    <Attribute2>test string</Attribute2>\n"
                                "</MySequenceWithNullables>\n",
                              };
            const int NUM_INPUT = sizeof INPUT / sizeof *INPUT;

            for (int i = 0; i < Type::NUM_ATTRIBUTES; ++i) {
                Type mX;  const Type& X = mX;

                for (int j = 0; j < Type::NUM_ATTRIBUTES; ++j) {
                    if (j != i) {
                        if (0 == j) {
                            mX.attribute1().makeValue(ATTRIBUTE1_VALUE);
                        }
                        else if (1 == j) {
                            mX.attribute2().makeValue(ATTRIBUTE2_VALUE);
                        }
                        else if (2 == j) {
                            mX.attribute3().makeValue(ATTRIBUTE3_VALUE);
                        }
                    }
                }

                if (veryVerbose) {
                    T_ P_(i) P_(X) P(INPUT[i])
                }

                bsl::stringstream input(INPUT[i]);

                Type mY;  const Type& Y = mY;
                LOOP3_ASSERT(i, X, Y, X != Y);

                baexml_MiniReader reader;
                baexml_ErrorInfo  errInfo;
                baexml_DecoderOptions    options;

                baexml_Decoder decoder(&options, &reader, &errInfo,
                                       &bsl::cerr, &bsl::cerr);

                decoder.decode(input, &mY);

                LOOP_ASSERT(i, input);
                LOOP3_ASSERT(i, X, Y, X == Y);
            }
        }

        if (verbose) cout << "\nUsing 'MySequenceWithNillables'." << endl;
        {
            typedef test::MySequenceWithNillables Type;

            const int         ATTRIBUTE1_VALUE = 123;
            const bsl::string ATTRIBUTE2_VALUE = "test string";
            test::MySequence  ATTRIBUTE3_VALUE;

            ATTRIBUTE3_VALUE.attribute1() = 987;
            ATTRIBUTE3_VALUE.attribute2() = "inner";

            const char *INPUT[3]
                            = {
                                "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                                "<MySequenceWithNillables>\n"
                                "    <Attribute1/>\n"
                                "    <Attribute2>test string</Attribute2>\n"
                                "    <Attribute3>\n"
                                "        <Attribute1>987</Attribute1>\n"
                                "        <Attribute2>inner</Attribute2>\n"
                                "    </Attribute3>\n"
                                "</MySequenceWithNillables>\n",

                                "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                                "<MySequenceWithNillables>\n"
                                "    <Attribute1>123</Attribute1>\n"
                                "    <Attribute2/>\n"
                                "    <Attribute3>\n"
                                "        <Attribute1>987</Attribute1>\n"
                                "        <Attribute2>inner</Attribute2>\n"
                                "    </Attribute3>\n"
                                "</MySequenceWithNillables>\n",

                                "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                                "<MySequenceWithNillables>\n"
                                "    <Attribute1>123</Attribute1>\n"
                                "    <Attribute2>test string</Attribute2>\n"
                                "    <Attribute3/>\n"
                                "</MySequenceWithNillables>\n",
                              };
            const int NUM_INPUT = sizeof INPUT / sizeof *INPUT;

            for (int i = 0; i < Type::NUM_ATTRIBUTES; ++i) {
                Type mX;  const Type& X = mX;

                for (int j = 0; j < Type::NUM_ATTRIBUTES; ++j) {
                    if (j != i) {
                        if (0 == j) {
                            mX.attribute1().makeValue(ATTRIBUTE1_VALUE);
                        }
                        else if (1 == j) {
                            mX.attribute2().makeValue(ATTRIBUTE2_VALUE);
                        }
                        else if (2 == j) {
                            mX.attribute3().makeValue(ATTRIBUTE3_VALUE);
                        }
                    }
                }

                if (veryVerbose) {
                    T_ P_(i) P_(X) P(INPUT[i])
                }

                bsl::stringstream input(INPUT[i]);

                Type mY;  const Type& Y = mY;
                LOOP3_ASSERT(i, X, Y, X != Y);

                baexml_MiniReader reader;
                baexml_ErrorInfo  errInfo;
                baexml_DecoderOptions    options;

                baexml_Decoder decoder(&options, &reader, &errInfo,
                                       &bsl::cerr, &bsl::cerr);

                decoder.decode(input, &mY);

                LOOP_ASSERT(i, input);
                LOOP3_ASSERT(i, X, Y, X == Y);
            }
        }

        if (verbose) cout << "\nEnd of baexml_Decoder_SequenceContext<TYPE> "
                          << "Test." << endl;
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // TESTING baexml_Decoder_ParseObject
        //   This will test the 'Decoder_ParseObject' function class.
        //
        // Concerns:
        //   When the function object is called, the correct 'execute' method
        //   must be called (i.e., for 'TYPE's that fall into the
        //   'bdeat_ArrayCategory' (except 'bsl::vector<char>'), the object
        //   must grow by one and the function object must be called
        //   recursively for the newly added element.  For other 'TYPE's, the
        //   'execute' method must create an appropriate context for the
        //   element and call the 'parse' method for the parser).
        //
        // Plan:
        //   Construct a 'Decoder_ParseObject' function object using a
        //   pointer to a 'TestParser' object.  Execute the function object
        //   using 'int' and 'bsl::vector<char>' for the parameterized 'TYPE'.
        //   These types are used because they use the
        //   'Decoder_SimpleContext' and the
        //   'Decoder_Base64Context' contexts, which have already been
        //   tested thoroughly.  After executing the function object, check
        //   that the values stored in the objects are as expected.
        //
        //   Next, exercise the function object using a 'bsl::vector<int>'
        //   object.  Check that each time the function object is executed, the
        //   vector grows by one and the element at the end has the expected
        //   value.
        //
        //   Since the 'INFO_TYPE' parameter is ignored, we can just use a
        //   dummy.
        //
        // Testing:
        //   baexml_Decoder_ParseObject
        // --------------------------------------------------------------------

        //if (verbose) cout << "\nTesting 'Decoder_ParseObject'"
        //                  << "\n=================================" << endl;

        //bsl::ostream            nullStream(0);
        //bsl::string             nullElementName;
        //TestErrorReporter       errorReporter(nullStream, nullStream);
        //TestParser              parser(&errorReporter);

        //baexml_Decoder_DecoderContext decoderContext;
        //decoderContext.d_reporter_p = &errorReporter;

        //baexml_Decoder_ParseObject parseObject(&parser,
        //                                    &decoderContext,
        //                                    nullElementName.c_str(),
        //                                    nullElementName.length());

        //if (verbose) cout << "\nUsing 'int'." << endl;
        //{
        //    int object = 987;

        //    parser.setCharsToAdd("123");
        //    parseObject.execute(&object,
        //                        bdeat_FormattingMode::BDEAT_DEFAULT);

        //    LOOP_ASSERT(object, 123 == object);
        //}

        //if (verbose) cout << "\nUsing 'bdeut_NullableValue<int>'." << endl;
        //{
        //    bdeut_NullableValue<int> object;

        //    parser.setCharsToAdd("123");
        //    parseObject.execute(&object,
        //                        bdeat_FormattingMode::BDEAT_DEFAULT);

        //    LOOP_ASSERT(object, 123 == object.value());
        //}
        //{
        //    bdeut_NullableValue<int> object;

        //    object.makeValue(432);

        //    parser.setCharsToAdd("123");
        //    parseObject.execute(&object,
        //                        bdeat_FormattingMode::BDEAT_DEFAULT);

        //    LOOP_ASSERT(object, 123 == object.value());
        //}

        //if (verbose) cout << "\nUsing 'bsl::vector<char>'." << endl;
        //{
        //    const char INIT[] = "InIt VaLuE";

        //    bsl::vector<char> object(INIT, INIT + sizeof(INIT)-1);

        //    parser.setCharsToAdd("YWJjZA==");
        //    parseObject.execute(&object,
        //                        bdeat_FormattingMode::BDEAT_DEFAULT);

        //    bsl::string value(&object[0], object.size());

        //    LOOP_ASSERT(object, "abcd" == value);
        //}

        //if (verbose) cout << "\nUsing 'bsl::vector<int>'." << endl;
        //{
        //    bsl::vector<int> object;

        //    parser.setCharsToAdd("123");
        //    parseObject.execute(&object, bdeat_FormattingMode::BDEAT_DEC);

        //    LOOP_ASSERT(object.size(), 1   == object.size());
        //    LOOP_ASSERT(object[0],     123 == object[0]);

        //    parser.setCharsToAdd("456");
        //    parseObject.execute(&object, bdeat_FormattingMode::BDEAT_DEC);

        //    LOOP_ASSERT(object.size(), 2   == object.size());
        //    LOOP_ASSERT(object[0],     123 == object[0]);
        //    LOOP_ASSERT(object[1],     456 == object[1]);

        //    parser.setCharsToAdd("789");
        //    parseObject.execute(&object, bdeat_FormattingMode::BDEAT_DEC);

        //    LOOP_ASSERT(object.size(), 3   == object.size());
        //    LOOP_ASSERT(object[0],     123 == object[0]);
        //    LOOP_ASSERT(object[1],     456 == object[1]);
        //    LOOP_ASSERT(object[2],     789 == object[2]);
        //}

        //if (verbose) cout << "\nUsing 'bsl::vector<int>' (list)." << endl;
        //{
        //    bsl::vector<int> object;

        //    parser.setCharsToAdd("  123 456   789");
        //    parseObject.execute(&object, bdeat_FormattingMode::BDEAT_LIST);

        //    LOOP_ASSERT(object.size(), 3   == object.size());
        //    LOOP_ASSERT(object[0],     123 == object[0]);
        //    LOOP_ASSERT(object[1],     456 == object[1]);
        //    LOOP_ASSERT(object[2],     789 == object[2]);
        //}

        //if (verbose) cout << "\nEnd of 'Decoder_ParseObject' Test."
        //                  << endl;
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING baexml_Decoder_SimpleContext<TYPE>
        //   This will test the 'Decoder_SimpleContext' class template.
        //
        // Concerns:
        //   This context must clear the accumulated characters at the start of
        //   the element and append any subsequent characters in the element.
        //   At the end of the element, the accumulated characters should be
        //   parsed to set the value of the associated object.
        //
        // Plan:
        //   Use 'int' for the 'TYPE' template parameter.  For each item in a
        //   set of test data, create an instance of the
        //   'Decoder_SimpleContext<int>' class and associate it with
        //   an 'int' object that has been set to some arbitrary initial value.
        //   Call the 'Decoder_ParserUtil::parse' function using the
        //   context and ensure that the associated 'int' has the expected
        //   value.
        //
        // Testing:
        //   baexml_Decoder_SimpleContext<TYPE>
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting baexml_Decoder_SimpleContext<TYPE>"
                          << "\n======================================="
                          << endl;

        static const struct {
            int         d_lineNum;  // source line number
            const char *d_input;    // input string
            int         d_retCode;  // expected ret code
            int         d_result;   // expected result
        } DATA[] = {
            //line  input                              retCode   result
            //----  -----                              -------   ------

            // INT_MIN, -1, 0, 1, INT_MAX
            { L_,   "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                    "<Value>-2147483648</Value>\n",    0,      -2147483647-1 },
            { L_,   "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                    "<Value>-1</Value>\n",             0,        -1          },
            { L_,   "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                    "<Value>0</Value>\n",              0,        0           },
            { L_,   "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                    "<Value>1</Value>\n",              0,        1           },
            { L_,   "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                    "<Value>2147483647</Value>\n",     0,        2147483647  },

            // arbitrary values
            { L_,   "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                    "<Value>123</Value>\n",            0,        123         },
            { L_,   "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                    "<Value>-4567</Value>\n",          0,        -4567       },

            // arbitrary values with surrounding whitespace
            { L_,   "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                    "<Value> \n 123  </Value>\n",      0,        123         },
            { L_,   "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                    "<Value> -4567 \n\t </Value>\n",   0,        -4567       },

            // invalid input
            { L_,   "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                    "<Value>123<Bad></Bad></Value>\n", 3                     },
            { L_,   "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                    "<Value>123 567</Value>\n",        2                     },
            { L_,   "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                    "<Value>blah</Value>\n",           2                     },
        };
        const int NUM_DATA  = sizeof DATA / sizeof *DATA;
        const int MAX_DEPTH = 5;

        for (int i = 0; i < NUM_DATA; ++i) {
            const int   LINE              = DATA[i].d_lineNum;
            const char *INPUT             = DATA[i].d_input;
            const int   EXPECTED_RET_CODE = DATA[i].d_retCode;
            const int   EXPECTED_RESULT   = DATA[i].d_result;

            if (veryVerbose) {
                T_ P(LINE)

                if (veryVeryVerbose) {
                    T_ T_ P(INPUT)

                    if (0 == EXPECTED_RET_CODE) {
                        T_ T_ P_(EXPECTED_RET_CODE) P(EXPECTED_RESULT)
                    }
                    else {
                        T_ T_ P(EXPECTED_RET_CODE)
                    }
                }
            }

            const int INIT_VALUE = 9876;

            baexml_DecoderOptions options;
            options.setMaxDepth(MAX_DEPTH);

            bsl::stringstream ss(INPUT);
            int               result1 = INIT_VALUE;
            bsl::ostream      nullStream(0);

            // display error messages on 'bsl::cerr' only if errors are not
            // expected or if very very very verbose
            bsl::ostream& outStream
                              = (0 == EXPECTED_RET_CODE || veryVeryVeryVerbose)
                                ? bsl::cerr
                                : nullStream;

            baexml_Decoder_SimpleContext<int> context(
                                          &result1,
                                          bdeat_FormattingMode::BDEAT_DEFAULT);

            baexml_MiniReader reader;
            baexml_ErrorInfo  errInfo;
            baexml_Decoder    decoder(&options, &reader, &errInfo,
                    &outStream, &outStream);

            decoder.open(ss.rdbuf());
            int retCode = context.beginParse(&decoder);;

            if (0 == EXPECTED_RET_CODE) {
                LOOP3_ASSERT(LINE, EXPECTED_RET_CODE,   retCode,
                                    EXPECTED_RET_CODE == retCode);
            }
            else {
                LOOP2_ASSERT(LINE, retCode, 0 != retCode);
            }

            if (0 == EXPECTED_RET_CODE) {
                LOOP3_ASSERT(LINE, EXPECTED_RESULT,   result1,
                                   EXPECTED_RESULT == result1);
            }
        }

        if (verbose) cout << "\nEnd of baexml_Decoder_SimpleContext<TYPE> "
                          << "Test." << endl;
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING baexml_Decoder_UTF8Context
        //   This will test the 'Decoder_UTF8Context' class.
        //
        // Concerns:
        //   This context class must clear the string at the start of the
        //   element and append any subsequent characters in the element.
        //
        // Plan:
        //   For each item in a set of test data, create an instance of the
        //   'Decoder_UTF8Context' class and associate it with a string
        //   object that has been set to some arbitrary initial value.  Call
        //   the 'Decoder_ParserUtil::parse' function using the context
        //   and ensure that the associated string has the expected value.
        //
        // Testing:
        //   baexml_Decoder_UTF8Context
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting baexml_Decoder_UTF8Context"
                          << "\n===============================" << endl;

        static const struct {
            int         d_lineNum;  // source line number
            const char *d_input;    // input string
            int         d_retCode;  // expected ret code
            const char *d_result;   // expected result
        } DATA[] = {
            //line  input                                retCode     result
            //----  -----                                -------     ------
            { L_,   "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                    "<Value></Value>\n",                 0,          ""      },

            { L_,   "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                    "<Value> </Value>\n",                0,          " "     },
            { L_,   "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                    "<Value>\n</Value>\n",               0,          "\n"    },
            { L_,   "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                    "<Value>  </Value>\n",               0,          "  "    },
            { L_,   "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                    "<Value></Value>\n",                 0,          ""      },

            { L_,   "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                    "<Value>abc</Value>\n",              0,          "abc"   },
            { L_,   "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                    "<Value>ab cd</Value>\n",            0,          "ab cd" },

            { L_,   "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                    "<Value>A&amp;B</Value>\n",          0,          "A&B"   },
            { L_,   "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                    "<Value>A&lt;B</Value>\n",           0,          "A<B"   },
            { L_,   "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                    "<Value>A&gt;B</Value>\n",           0,          "A>B"   },
            { L_,   "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                    "<Value>A&apos;B</Value>\n",         0,          "A\'B"  },
            { L_,   "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                    "<Value>A&quot;B</Value>\n",         0,          "A\"B"  },
            { L_,   "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                    "<Value>A" "\xC3\xA4" "B</Value>\n", 0,
                                                         "A" "\xC3\xA4" "B"  },

            { L_,   "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                    "<Value>abc<Bad></Bad></Value>\n",   3                   },
        };
        const int NUM_DATA  = sizeof DATA / sizeof *DATA;
        const int MAX_DEPTH = 5;

        for (int i = 0; i < NUM_DATA; ++i) {
            const int   LINE              = DATA[i].d_lineNum;
            const char *INPUT             = DATA[i].d_input;
            const int   INPUT_LENGTH      = bsl::strlen(INPUT);
            const int   EXPECTED_RET_CODE = DATA[i].d_retCode;
            const char *EXPECTED_DATA     = DATA[i].d_result;

            if (veryVerbose) {
                T_ P(LINE)

                if (veryVeryVerbose) {
                    T_ T_ P(INPUT)

                    if (0 == EXPECTED_RET_CODE) {
                        T_ T_ P_(EXPECTED_RET_CODE) P(EXPECTED_DATA)
                    }
                    else {
                        T_ T_ P(EXPECTED_RET_CODE)
                    }
                }
            }

            const char INIT_VALUE[]      = "InIt VaLuE";
            const int  INIT_VALUE_LENGTH = sizeof (INIT_VALUE) - 1;

            if (veryVerbose) cout << "\tUsing bsl::string." << endl;
            {
                typedef bsl::string Type;

                bsl::stringstream ss(INPUT);
                Type              result1(INIT_VALUE,
                                          INIT_VALUE + INIT_VALUE_LENGTH);
                bsl::ostream      nullStream(0);

                baexml_DecoderOptions options;
                options.setMaxDepth(MAX_DEPTH);

                // display error messages on 'bsl::cerr' only if errors are not
                // expected or if very very very verbose
                bsl::ostream& outStream
                              = (0 == EXPECTED_RET_CODE || veryVeryVeryVerbose)
                                ? bsl::cerr
                                : nullStream;

                baexml_Decoder_UTF8Context<Type> context(
                                          &result1,
                                          bdeat_FormattingMode::BDEAT_DEFAULT);
                baexml_MiniReader reader;
                baexml_ErrorInfo  errInfo;
                baexml_Decoder    decoder(&options, &reader, &errInfo,
                        &outStream, &outStream);

                decoder.open(ss.rdbuf());
                int retCode = context.beginParse(&decoder);;

                if (0 == EXPECTED_RET_CODE) {
                    LOOP3_ASSERT(LINE, EXPECTED_RET_CODE,   retCode,
                                       EXPECTED_RET_CODE == retCode);
                }
                else {
                    LOOP2_ASSERT(LINE, retCode, 0 != retCode);
                }

                if (0 == EXPECTED_RET_CODE) {
                    const int EXPECTED_DATA_LENGTH = bsl::strlen(
                                                                EXPECTED_DATA);

                    Type EXPECTED_RESULT(EXPECTED_DATA,
                                         EXPECTED_DATA + EXPECTED_DATA_LENGTH);

                    LOOP3_ASSERT(LINE, EXPECTED_RESULT,   result1,
                                       EXPECTED_RESULT == result1);
                }
            }

            if (veryVerbose) cout << "\tUsing bsl::vector<char>." << endl;
            {
                typedef bsl::vector<char> Type;

                bsl::stringstream ss(INPUT);
                Type              result1(INIT_VALUE,
                                          INIT_VALUE + INIT_VALUE_LENGTH);
                bsl::ostream      nullStream(0);

                // display error messages on 'bsl::cerr' only if errors are not
                // expected or if very very very verbose
                bsl::ostream& outStream
                              = (0 == EXPECTED_RET_CODE || veryVeryVeryVerbose)
                                ? bsl::cerr
                                : nullStream;

                baexml_DecoderOptions options;
                options.setMaxDepth(MAX_DEPTH);

                baexml_Decoder_UTF8Context<Type> context(
                                             &result1,
                                             bdeat_FormattingMode::BDEAT_TEXT);

                baexml_MiniReader reader;
                baexml_ErrorInfo  errInfo;
                baexml_Decoder    decoder(&options, &reader, &errInfo,
                        &outStream, &outStream);

                decoder.open(ss.rdbuf());
                int retCode = context.beginParse(&decoder);;

                if (0 == EXPECTED_RET_CODE) {
                    LOOP3_ASSERT(LINE, EXPECTED_RET_CODE,   retCode,
                                       EXPECTED_RET_CODE == retCode);
                }
                else {
                    LOOP2_ASSERT(LINE, retCode, 0 != retCode);
                }

                if (0 == EXPECTED_RET_CODE) {
                    const int EXPECTED_DATA_LENGTH = bsl::strlen(
                                                                EXPECTED_DATA);

                    Type EXPECTED_RESULT(EXPECTED_DATA,
                                         EXPECTED_DATA + EXPECTED_DATA_LENGTH);

                    LOOP3_ASSERT(LINE, EXPECTED_RESULT,   result1,
                                       EXPECTED_RESULT == result1);
                }
            }
        }

        if (verbose) cout << "\nEnd of baexml_Decoder_UTF8Context Test."
                          << endl;
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING baexml_Decoder_SelectContext
        //   This will test the 'Decoder_SelectContext' meta-function.
        //
        // Concerns:
        //   This meta-function should give the correct context type.
        //
        // Plan:
        //   For a set of types, exercise the 'Decoder_SelectContext'
        //   meta-function and check that the returned 'Type' is as expected.
        //
        // Testing:
        //   baexml_Decoder_SelectContext
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting baexml_Decoder_SelectContext"
                          << "\n=================================" << endl;

        if (verbose) cout << "\nUsing sequence types." << endl;
        {
            {
                typedef TestSequence0                     TestType;
                typedef
                baexml_Decoder_SelectContext<TestType>::Type Result;
                typedef
                baexml_Decoder_SequenceContext<TestType>     ExpectedResult;

                ASSERT((bslmf_IsSame<Result, ExpectedResult>::VALUE));
            }
            {
                typedef TestSequence1                     TestType;
                typedef
                baexml_Decoder_SelectContext<TestType>::Type Result;
                typedef
                baexml_Decoder_SequenceContext<TestType>     ExpectedResult;

                ASSERT((bslmf_IsSame<Result, ExpectedResult>::VALUE));
            }
            {
                typedef TestSequence2                     TestType;
                typedef
                baexml_Decoder_SelectContext<TestType>::Type Result;
                typedef
                baexml_Decoder_SequenceContext<TestType>     ExpectedResult;

                ASSERT((bslmf_IsSame<Result, ExpectedResult>::VALUE));
            }
        }

        if (verbose) cout << "\nUsing choice types." << endl;
        {
            {
                typedef TestChoice0                       TestType;
                typedef
                baexml_Decoder_SelectContext<TestType>::Type Result;
                typedef
                baexml_Decoder_ChoiceContext<TestType>       ExpectedResult;

                ASSERT((bslmf_IsSame<Result, ExpectedResult>::VALUE));
            }
            {
                typedef TestChoice1                       TestType;
                typedef
                baexml_Decoder_SelectContext<TestType>::Type Result;
                typedef
                baexml_Decoder_ChoiceContext<TestType>       ExpectedResult;

                ASSERT((bslmf_IsSame<Result, ExpectedResult>::VALUE));
            }
            {
                typedef TestChoice2                       TestType;
                typedef
                baexml_Decoder_SelectContext<TestType>::Type Result;
                typedef
                baexml_Decoder_ChoiceContext<TestType>       ExpectedResult;

                ASSERT((bslmf_IsSame<Result, ExpectedResult>::VALUE));
            }
        }

        if (verbose) cout << "\nUsing simple types." << endl;
        {
            {
                typedef int                               TestType;
                typedef
                baexml_Decoder_SelectContext<TestType>::Type Result;
                typedef
                baexml_Decoder_SimpleContext<TestType>       ExpectedResult;

                ASSERT((bslmf_IsSame<Result, ExpectedResult>::VALUE));
            }
            {
                typedef short                             TestType;
                typedef
                baexml_Decoder_SelectContext<TestType>::Type Result;
                typedef
                baexml_Decoder_SimpleContext<TestType>       ExpectedResult;

                ASSERT((bslmf_IsSame<Result, ExpectedResult>::VALUE));
            }
            {
                typedef double                            TestType;
                typedef
                baexml_Decoder_SelectContext<TestType>::Type Result;
                typedef
                baexml_Decoder_SimpleContext<TestType>       ExpectedResult;

                ASSERT((bslmf_IsSame<Result, ExpectedResult>::VALUE));
            }
            {
                typedef float                             TestType;
                typedef
                baexml_Decoder_SelectContext<TestType>::Type Result;
                typedef
                baexml_Decoder_SimpleContext<TestType>       ExpectedResult;

                ASSERT((bslmf_IsSame<Result, ExpectedResult>::VALUE));
            }
            {
                typedef long long                         TestType;
                typedef
                baexml_Decoder_SelectContext<TestType>::Type Result;
                typedef
                baexml_Decoder_SimpleContext<TestType>       ExpectedResult;

                ASSERT((bslmf_IsSame<Result, ExpectedResult>::VALUE));
            }
            {
                typedef DummyEnumeration::Value           TestType;
                typedef
                baexml_Decoder_SelectContext<TestType>::Type Result;
                typedef
                baexml_Decoder_SimpleContext<TestType>       ExpectedResult;

                ASSERT((bslmf_IsSame<Result, ExpectedResult>::VALUE));
            }
            {
                typedef DummyCustomizedType               TestType;
                typedef
                baexml_Decoder_SelectContext<TestType>::Type Result;
                typedef
                baexml_Decoder_CustomizedContext<TestType>   ExpectedResult;

                ASSERT((bslmf_IsSame<Result, ExpectedResult>::VALUE));
            }
        }

        if (verbose) cout << "\nUsing array types." << endl;
        {
            {
                typedef bsl::vector<int>                        TestType;
                typedef
                baexml_Decoder_SelectContext<TestType>::Type       Result;
                typedef
                baexml_Decoder_PushParserContext<
                         TestType,
                         baexml_Decoder_ListParser<TestType> > ExpectedResult;

                ASSERT((bslmf_IsSame<Result, ExpectedResult>::VALUE));
            }
            {
                typedef bsl::vector<short>                      TestType;
                typedef
                baexml_Decoder_SelectContext<TestType>::Type       Result;
                typedef
                baexml_Decoder_PushParserContext<
                         TestType,
                         baexml_Decoder_ListParser<TestType> > ExpectedResult;

                ASSERT((bslmf_IsSame<Result, ExpectedResult>::VALUE));
            }
            {
                typedef bsl::vector<double>                     TestType;
                typedef
                baexml_Decoder_SelectContext<TestType>::Type       Result;
                typedef
                baexml_Decoder_PushParserContext<
                         TestType,
                         baexml_Decoder_ListParser<TestType> > ExpectedResult;

                ASSERT((bslmf_IsSame<Result, ExpectedResult>::VALUE));
            }
        }

        if (verbose) cout << "\nUsing 'bsl::string'." << endl;
        {
            {
                typedef bsl::string                       TestType;
                typedef
                baexml_Decoder_SelectContext<TestType>::Type Result;
                typedef baexml_Decoder_StdStringContext      ExpectedResult;

                ASSERT((bslmf_IsSame<Result, ExpectedResult>::VALUE));
            }
        }

        if (verbose) cout << "\nUsing 'bsl::vector<char>'." << endl;
        {
            {
                typedef bsl::vector<char>                 TestType;
                typedef
                baexml_Decoder_SelectContext<TestType>::Type Result;
                typedef baexml_Decoder_StdVectorCharContext  ExpectedResult;

                ASSERT((bslmf_IsSame<Result, ExpectedResult>::VALUE));
            }
        }

        if (verbose) cout << "\nEnd of baexml_Decoder_SelectContext Test."
                          << endl;
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING PARSER UTIL
        //   This will test the 'Decoder_ParserUtil' struct and also
        //   the underlying 'Parser' class in the .cpp file.
        //
        // Concerns:
        //   The parser should use the 'Decoder_ElementContext'
        //   protocol correctly.  Note that we are not testing the classes that
        //   derive from 'Decoder_ElementContext' in the header file.
        //   We are only concerned that, given a particular input, the parser
        //   will call the appropriate methods in the
        //   'Decoder_ElementContext' protocol with the correct
        //   arguments and in the correct order.  Once we have established that
        //   the parser is using the protocol correctly, we can then thoroughly
        //   test the classes that derive from the
        //   'Decoder_ElementContext' protocol in subsequent test
        //   cases.
        //
        // Plan:
        //   The 'TestContext' class will be used as a test implementation of
        //   the 'Decoder_ElementContext' protocol.
        //
        //   For each string in a set of input strings, create an independent
        //   'TestContext' object.  Also prepare the appropriate 'input',
        //   'errorStream', and 'warningStream' arguments.  Use these arguments
        //   to call the 'Decoder_ParserUtil::parse' function.
        //
        //   When the function returns, check that the correct methods in the
        //   'TestContext' object were called - in the correct order and with
        //   the correct arguments.
        //
        // Testing:
        //   baexml_Decoder_ParserUtil
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting Parser Util"
                          << "\n===================" << endl;

        const char XML_NAME[] = "RE";  // xml name for root element

        static const struct {
            int         d_lineNum;       // source line number
            const char *d_input;         // input string
            int         d_maxDepth;      // maximum depth
            bool        d_success;       // parser succeed
            const char *d_callSequence;  // expected call sequence
        } DATA[] = {
            //line  input
            //----  -----
            //      maxDepth       success   call sequence
            //      --------       -------   -------------
            { L_,   "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                    "<RE></RE>\n",
                    5,             true,     "RE:startElement(...)"
                                             "RE:endElement(...)"            },

            { L_,   "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                    "<RE>abc</RE>\n",
                    5,             true,     "RE:startElement(...)"
                                             "RE:addCharacters(abc, ...)"
                                             "RE:endElement(...)"            },

            { L_,   "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                    "<RE>abc"
                    "<E1></E1>"
                    "def</RE>\n",
                    5,             true,     "RE:startElement(...)"
                                             "RE:addCharacters(abc, ...)"
                                             "RE:parseSubElement(E1, ...)"
                                             "E1:startElement(...)"
                                             "E1:endElement(...)"
                                             "RE:addCharacters(def, ...)"
                                             "RE:endElement(...)"            },

            { L_,   "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                    "<RE>abc"
                    "<E1> sub "
                    "<E1.1>sub-sub</E1.1>"
                    "</E1>"
                    "def</RE>\n",
                    5,             true,     "RE:startElement(...)"
                                             "RE:addCharacters(abc, ...)"
                                             "RE:parseSubElement(E1, ...)"
                                             "E1:startElement(...)"
                                             "E1:addCharacters( sub , ...)"
                                             "E1:parseSubElement(E1.1, ...)"
                                             "E1.1:startElement(...)"
                                             "E1.1:addCharacters(sub-sub, ...)"
                                             "E1.1:endElement(...)"
                                             "E1:endElement(...)"
                                             "RE:addCharacters(def, ...)"
                                             "RE:endElement(...)"            },

            { L_,   "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                    "<RE>abc"
                    "<E1> sub "
                    "<E1.1>sub-sub</E1.1>"
                    "</E1>"
                    "<E2>next-sub</E2>"
                    "def</RE>\n",
                    5,             true,     "RE:startElement(...)"
                                             "RE:addCharacters(abc, ...)"
                                             "RE:parseSubElement(E1, ...)"
                                             "E1:startElement(...)"
                                             "E1:addCharacters( sub , ...)"
                                             "E1:parseSubElement(E1.1, ...)"
                                             "E1.1:startElement(...)"
                                             "E1.1:addCharacters(sub-sub, ...)"
                                             "E1.1:endElement(...)"
                                             "E1:endElement(...)"
                                             "RE:parseSubElement(E2, ...)"
                                             "E2:startElement(...)"
                                             "E2:addCharacters(next-sub, ...)"
                                             "E2:endElement(...)"
                                             "RE:addCharacters(def, ...)"
                                             "RE:endElement(...)"            },

            { L_,   "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                    "<RE>abc"
                    "<E1> sub "
                    "<E1.1>sub-sub</E1.1>"
                    "<E1.2>blah</E1.2>"
                    "</E1>"
                    "<E2>next-sub</E2>"
                    "def</RE>\n",
                    5,             true,     "RE:startElement(...)"
                                             "RE:addCharacters(abc, ...)"
                                             "RE:parseSubElement(E1, ...)"
                                             "E1:startElement(...)"
                                             "E1:addCharacters( sub , ...)"
                                             "E1:parseSubElement(E1.1, ...)"
                                             "E1.1:startElement(...)"
                                             "E1.1:addCharacters(sub-sub, ...)"
                                             "E1.1:endElement(...)"
                                             "E1:parseSubElement(E1.2, ...)"
                                             "E1.2:startElement(...)"
                                             "E1.2:addCharacters(blah, ...)"
                                             "E1.2:endElement(...)"
                                             "E1:endElement(...)"
                                             "RE:parseSubElement(E2, ...)"
                                             "E2:startElement(...)"
                                             "E2:addCharacters(next-sub, ...)"
                                             "E2:endElement(...)"
                                             "RE:addCharacters(def, ...)"
                                             "RE:endElement(...)"            },

            // If the element name is 'failOnStart', then the 'TestContext'
            // class will return a failure code when it receives the
            // 'startElement' callback.  We need to check that this error is
            // propagated back to the parser correctly.

            { L_,   "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                    "<RE>abc"
                    "<failOnStart></failOnStart>"
                    "def</RE>\n",
                    5,             false,    "RE:startElement(...)"
                                             "RE:addCharacters(abc, ...)"
                                             "RE:parseSubElement(failOnStart, "
                                                                 "...)"
                                             "failOnStart:startElement(...)"
                                             "failOnStart:endElement(...)"
                                             "RE:endElement(...)"            },

            { L_,   "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                    "<RE>abc"
                    "<E1><failOnStart></failOnStart></E1>"
                    "def</RE>\n",
                    5,             false,    "RE:startElement(...)"
                                             "RE:addCharacters(abc, ...)"
                                             "RE:parseSubElement(E1, ...)"
                                             "E1:startElement(...)"
                                             "E1:parseSubElement(failOnStart,"
                                                                 " ...)"
                                             "failOnStart:startElement(...)"
                                             "failOnStart:endElement(...)"
                                             "E1:endElement(...)"
                                             "RE:endElement(...)"            },

            // If the element name is 'failOnEnd', then the 'TestContext' class
            // will return a failure code when it receives the 'endElement'
            // callback.  We need to check that this error is propagated back
            // to the parser correctly.

            { L_,   "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                    "<RE>abc"
                    "<failOnEnd></failOnEnd>"
                    "def</RE>\n",
                    5,             false,    "RE:startElement(...)"
                                             "RE:addCharacters(abc, ...)"
                                             "RE:parseSubElement(failOnEnd, "
                                                                 "...)"
                                             "failOnEnd:startElement(...)"
                                             "failOnEnd:endElement(...)"
                                             "RE:endElement(...)"            },

            { L_,   "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                    "<RE>abc"
                    "<E1><failOnEnd></failOnEnd></E1>"
                    "def</RE>\n",
                    5,             false,    "RE:startElement(...)"
                                             "RE:addCharacters(abc, ...)"
                                             "RE:parseSubElement(E1, ...)"
                                             "E1:startElement(...)"
                                             "E1:parseSubElement(failOnEnd, "
                                                                 "...)"
                                             "failOnEnd:startElement(...)"
                                             "failOnEnd:endElement(...)"
                                             "E1:endElement(...)"
                                             "RE:endElement(...)"            },

            // If 'addCharacters(failHere, ...)' is called, then the
            // 'TestContext' class will return a failure code.  We need to
            // check that this error is propagated back to the parser
            // correctly.

            { L_,   "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                    "<RE>abc"
                    "<E1> sub "
                    "<E1.1>failHere</E1.1>"
                    "</E1>"
                    "def</RE>\n",
                    5,             false,    "RE:startElement(...)"
                                             "RE:addCharacters(abc, ...)"
                                             "RE:parseSubElement(E1, ...)"
                                             "E1:startElement(...)"
                                             "E1:addCharacters( sub , ...)"
                                             "E1:parseSubElement(E1.1, ...)"
                                             "E1.1:startElement(...)"
                                             "E1.1:addCharacters(failHere, "
                                                                 "...)"
                                             "E1.1:endElement(...)"
                                             "E1:endElement(...)"
                                             // "RE:addCharacters(def, ...)"
                                             "RE:endElement(...)"            },

            // If 'createSubContext(failHere, ...)' is called, then the
            // 'TestContext' class will return a failure code.  We need to
            // check that this error is propagated back to the parser
            // correctly.

            { L_,   "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                    "<RE>abc"
                    "<failHere></failHere>"
                    "def</RE>\n",
                    5,             false,    "RE:startElement(...)"
                                             "RE:addCharacters(abc, ...)"
                                             "RE:parseSubElement(failHere, "
                                                                 "...)"
                                             "RE:endElement(...)"            },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int i = 0; i < NUM_DATA; ++i) {
            const int   LINE                   = DATA[i].d_lineNum;
            const char *INPUT                  = DATA[i].d_input;
            const int   MAX_DEPTH              = DATA[i].d_maxDepth;
            const bool  EXPECTED_SUCCESS       = DATA[i].d_success;
            const char *EXPECTED_CALL_SEQUENCE = DATA[i].d_callSequence;

            if (veryVerbose) {
                T_ P(LINE)

                if (veryVeryVerbose) {
                    T_ T_ P(INPUT)
                    T_ T_ P(EXPECTED_SUCCESS)
                    T_ T_ P(EXPECTED_CALL_SEQUENCE)
                }
            }

            bsl::stringstream ss(INPUT);
            bsl::stringstream callSequence;
            bsl::ostream      errorStream(0);
            bsl::ostream      warningStream(0);

            bslma_TestAllocator testAllocator;

            baexml_DecoderOptions options;
            options.setMaxDepth(MAX_DEPTH);

            TestContext context(callSequence, XML_NAME, &testAllocator);

            baexml_MiniReader reader;
            baexml_ErrorInfo  errInfo;
            baexml_Decoder    decoder(&options, &reader, &errInfo,
                    &errorStream, &warningStream);

            decoder.open(ss.rdbuf());
            int retCode = context.beginParse(&decoder);;

            bool success = (0 == retCode);

            LOOP3_ASSERT(LINE, EXPECTED_SUCCESS,   success,
                               EXPECTED_SUCCESS == success);
            LOOP3_ASSERT(LINE,
                         EXPECTED_CALL_SEQUENCE,   callSequence.str(),
                         EXPECTED_CALL_SEQUENCE == callSequence.str());
        }

        if (verbose) cout << "\nEnd of Parser Util Test." << endl;
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //   This breathing test exercises basic functionality, but tests
        //   nothing.
        //
        // Concerns:
        //   The interface should work as expected.
        //
        // Plan:
        //   Exercise the 'decode' functions using the following types:
        //
        //       - int.
        //       - bsl::string.
        //       - TestSequence2.
        //       - TestChoice2.
        //
        //   For each type, check that the value of the object when decoded is
        //   as expected.  For each type, use a different version of the
        //   'decode' function.
        //
        // Testing:
        //   This exercises basic functionality, but tests nothing.
        // --------------------------------------------------------------------

        if (verbose) cout << "\nBREATHING TEST"
                          << "\n==============" << endl;

        enum {
            MAX_DEPTH = 30
        };

        if (verbose) cout << "\nTesting int." << endl;
        {
            bsl::string INPUT = "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                                "<Value>\n"
                                "    123\n"
                                "</Value>\n";
            bsl::stringstream ss(INPUT);

            if (veryVerbose) {
                T_ P(INPUT)
            }

            int i;

            baexml_MiniReader reader;
            baexml_ErrorInfo  errInfo;
            baexml_DecoderOptions    options;

            baexml_Decoder decoder(&options, &reader, &errInfo,
                                   &bsl::cerr, &bsl::cerr);

           int ret = decoder.decode(ss.rdbuf(), &i);

            LOOP_ASSERT(ret, 0 == ret);
            LOOP_ASSERT(i, 123 == i);
        }

        if (verbose) cout << "\nTesting bsl::string." << endl;
        {
            bsl::string INPUT = "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                                "<Value>\n"
                                "    abc\n"
                                "</Value>\n";
            bsl::stringstream ss(INPUT);

            if (veryVerbose) {
                T_ P(INPUT)
            }

            bsl::string s;

            baexml_MiniReader reader;
            baexml_ErrorInfo  errInfo;
            baexml_DecoderOptions    options;

            baexml_Decoder decoder(&options, &reader, &errInfo,
                                   &bsl::cerr, &bsl::cerr);

            int ret = decoder.decode(ss.rdbuf(), &s);

            LOOP_ASSERT(ret, 0 == ret);
            LOOP_ASSERT(s, "\n    abc\n" == s);
        }

        if (verbose) cout << "\nTesting TestSequence2." << endl;
        {
            bsl::string INPUT = "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                                "<TestSequence2>\n"
                                "    <E1>123</E1>\n"
                                "    <E2>abc</E2>\n"
                                "</TestSequence2>\n";
            bsl::stringstream ss(INPUT);

            if (veryVerbose) {
                T_ P(INPUT)
            }

            TestSequence2 ts;

            baexml_MiniReader reader;
            baexml_ErrorInfo  errInfo;
            baexml_DecoderOptions    options;

            baexml_Decoder decoder(&options, &reader, &errInfo,
                                   &bsl::cerr, &bsl::cerr);

            decoder.decode(ss, &ts);

            LOOP_ASSERT(ss.fail(), !ss.fail());
            LOOP_ASSERT(ts, TestSequence2(123, "abc") == ts);
        }

        if (verbose) cout << "\nTesting TestChoice2." << endl;
        {
            bsl::string INPUT = "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                                "<TestChoice2>\n"
                                "    <S1>123</S1>\n"
                                "</TestChoice2>\n";
            bsl::stringstream ss(INPUT);

            if (veryVerbose) {
                T_ P(INPUT)
            }

            TestChoice2 tc;

            baexml_MiniReader reader;
            baexml_ErrorInfo  errInfo;
            baexml_DecoderOptions    options;

            baexml_Decoder decoder(&options, &reader, &errInfo,
                                   &bsl::cerr, &bsl::cerr);

            decoder.decode(ss, &tc);

            LOOP_ASSERT(ss.fail(), !ss.fail());
            LOOP_ASSERT(tc, TestChoice2(123) == tc);
        }

        if (verbose) cout << "\nEnd of Breathing Test." << endl;
      } break;
      case -1: {
        bcema_SharedPtr<bdem_Schema> schemaPtr;  schemaPtr.createInplace();
        bdem_Schema&    schema = *schemaPtr;
        bdem_RecordDef *pointRecordDef, *circleRecordDef, *polygonRecordDef;

        pointRecordDef = schema.createRecord("PointRecord");
        pointRecordDef->appendField(bdem_ElemType::BDEM_DOUBLE, "x");
        pointRecordDef->appendField(bdem_ElemType::BDEM_DOUBLE, "y");

        circleRecordDef = schema.createRecord("CircleRecord");
        circleRecordDef->appendField(bdem_ElemType::BDEM_LIST, pointRecordDef,
                                     "center");
        circleRecordDef->appendField(bdem_ElemType::BDEM_DOUBLE, "radius");

        polygonRecordDef = schema.createRecord("PolygonRecord");
        polygonRecordDef->appendField(bdem_ElemType::BDEM_TABLE,
                                      pointRecordDef,
                                      "vertices");

        bdem_RecordDef *figureDef;

        figureDef = schema.createRecord("FigureChoice",
                                        bdem_RecordDef::BDEM_CHOICE_RECORD);
        figureDef->appendField(bdem_ElemType::BDEM_LIST, polygonRecordDef,
                               "Polygon");
        figureDef->appendField(bdem_ElemType::BDEM_LIST, circleRecordDef,
                               "Circle");
        figureDef->appendField(bdem_ElemType::BDEM_INT,     "IntSelection");
        figureDef->appendField(bdem_ElemType::BDEM_DOUBLE,  "DoubleSelection");

        bdem_RecordDef *playerRecordDef, *complexPlayerRecordDef;

        playerRecordDef = schema.createRecord("PlayerRecord");
        playerRecordDef->appendField(bdem_ElemType::BDEM_STRING, "name");
        playerRecordDef->appendField(bdem_ElemType::BDEM_DOUBLE, "health");
        playerRecordDef->appendField(bdem_ElemType::BDEM_CHOICE,
                                     figureDef,
                                     "figure");

        complexPlayerRecordDef = schema.createRecord("ComplexPlayerRecord");
        complexPlayerRecordDef->appendField(bdem_ElemType::BDEM_STRING,
                                            "name");
        complexPlayerRecordDef->appendField(bdem_ElemType::BDEM_DOUBLE,
                                            "health");
        complexPlayerRecordDef->appendField(bdem_ElemType::BDEM_CHOICE_ARRAY,
                                            figureDef,
                                            "figures");

        bdem_RecordDef *arrayOfPlayerDef = schema.createRecord(
                                                       "ArrayOfPlayersRecord");
        arrayOfPlayerDef->appendField(bdem_ElemType::BDEM_TABLE,
                                      playerRecordDef,
                                      "player");

        bdem_RecordDef *topLevelChoiceDef;

        topLevelChoiceDef = schema.createRecord(
                                           "TopLevelChoiceRecord",
                                           bdem_RecordDef::BDEM_CHOICE_RECORD);
        topLevelChoiceDef->appendField(bdem_ElemType::BDEM_LIST,
                                       playerRecordDef, "player");
        topLevelChoiceDef->appendField(bdem_ElemType::BDEM_LIST,
                                       complexPlayerRecordDef,
                                       "complexPlayer");
        topLevelChoiceDef->appendField(bdem_ElemType::BDEM_LIST,
                                       arrayOfPlayerDef,
                                       "arrayOfPlayers");

        bsl::cout << "\nTest 1" << bsl::endl;
        {
            const char INPUT[] =
                "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                "<Player>\n"
                "    <health>97.32</health>\n"
                "    <figure>\n"
                "        <Circle>\n"
                "            <center>\n"
                "                <y>29.3</y>\n"
                "                <x>8.2</x>\n"
                "            </center>\n"
                "            <radius>9.21</radius>\n"
                "        </Circle>\n"
                "    </figure>\n"
                "    <name>Shezan</name>\n"
                "</Player>\n";
            bsl::stringstream ss(INPUT);

            bsl::cout << "INPUT = \n" << INPUT;
            bsl::cout << bsl::endl << bsl::endl;

            bcem_Aggregate aggregate(schemaPtr,
                                     "PlayerRecord",
                                     bdem_ElemType::BDEM_LIST);

            bsl::cout << "Decoding..." << bsl::endl;

            baexml_MiniReader     reader;
            baexml_ErrorInfo      errInfo;
            baexml_DecoderOptions options;

            baexml_Decoder decoder(&options, &reader, &errInfo,
                                   &bsl::cerr, &bsl::cerr);

            decoder.decode(ss, &aggregate);
            ASSERT(ss);

            P(aggregate);
        }

        bsl::cout << "\nTest 2" << bsl::endl;
        {
            const char INPUT[] =
                "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                "<ComplexPlayer>\n"
                "    <name>Shezan</name>\n"
                "    <health>3.45</health>\n"
                "    <figures>\n"
                "        <IntSelection>45</IntSelection>\n"
                "    </figures>\n"
                "    <figures>\n"
                "        <DoubleSelection>2.45</DoubleSelection>\n"
                "    </figures>\n"
                "    <figures>\n"
                "        <Circle>\n"
                "            <center>\n"
                "                <x>2.12</x>\n"
                "                <y>4.34</y>\n"
                "            </center>\n"
                "            <radius>92.34</radius>\n"
                "        </Circle>\n"
                "    </figures>\n"
                "</ComplexPlayer>\n";

            bsl::stringstream ss(INPUT);

            bsl::cout << "INPUT = \n" << INPUT;
            bsl::cout << bsl::endl << bsl::endl;

            bcem_Aggregate aggregate(schemaPtr,
                                     "ComplexPlayerRecord",
                                     bdem_ElemType::BDEM_LIST);

            baexml_DecoderOptions decoderOptions;

            bsl::cout << "Decoding..." << bsl::endl;

            baexml_MiniReader     reader;
            baexml_ErrorInfo      errInfo;
            baexml_DecoderOptions options;

            baexml_Decoder decoder(&options, &reader, &errInfo,
                                   &bsl::cerr, &bsl::cerr);

            decoder.decode(ss, &aggregate);
            ASSERT(ss);

            P(aggregate);
        }

        bsl::cout << "\nTest 3" << bsl::endl;
        {
            const char INPUT[] =
                    "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                    "<TopLevelChoice>\n"
                    "    <player>\n"
                    "        <health>97.32</health>\n"
                    "        <figure>\n"
                    "            <Circle>\n"
                    "                <center>\n"
                    "                    <y>29.3</y>\n"
                    "                    <x>8.2</x>\n"
                    "                </center>\n"
                    "                <radius>9.21</radius>\n"
                    "            </Circle>\n"
                    "        </figure>\n"
                    "        <name>Shezan</name>\n"
                    "    </player>\n"
                    "</TopLevelChoice>\n";
            bsl::stringstream ss(INPUT);

            bsl::cout << "INPUT = \n" << INPUT;
            bsl::cout << bsl::endl << bsl::endl;

            bcem_Aggregate aggregate(schemaPtr,
                                     "TopLevelChoiceRecord",
                                     bdem_ElemType::BDEM_CHOICE);

            bsl::cout << "Decoding..." << bsl::endl;

            baexml_MiniReader     reader;
            baexml_ErrorInfo      errInfo;
            baexml_DecoderOptions options;

            baexml_Decoder decoder(&options, &reader, &errInfo,
                                   &bsl::cerr, &bsl::cerr);

            decoder.decode(ss, &aggregate);
            ASSERT(ss);

            P(aggregate);
        }

        bsl::cout << "\nTest 4" << bsl::endl;
        {
            const char INPUT[] =
                    "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                    "<TopLevelChoice>\n"
                    "    <arrayOfPlayers>\n"
                    "        <player>\n"
                    "            <health>97.32</health>\n"
                    "            <figure>\n"
                    "                <Circle>\n"
                    "                    <center>\n"
                    "                        <y>29.3</y>\n"
                    "                        <x>8.2</x>\n"
                    "                    </center>\n"
                    "                    <radius>9.21</radius>\n"
                    "                </Circle>\n"
                    "            </figure>\n"
                    "            <name>Shezan</name>\n"
                    "        </player>\n"
                    "    </arrayOfPlayers>\n"
                    "</TopLevelChoice>\n";
            bsl::stringstream ss(INPUT);

            bsl::cout << "INPUT = \n" << INPUT;
            bsl::cout << bsl::endl << bsl::endl;

            bcem_Aggregate aggregate(schemaPtr,
                                     "TopLevelChoiceRecord",
                                     bdem_ElemType::BDEM_CHOICE);

            bsl::cout << "Decoding..." << bsl::endl;

            baexml_MiniReader     reader;
            baexml_ErrorInfo      errInfo;
            baexml_DecoderOptions options;

            baexml_Decoder decoder(&options, &reader, &errInfo,
                                   &bsl::cerr, &bsl::cerr);

            decoder.decode(ss, &aggregate);

            ASSERT(ss);

            P(aggregate);
        }
      } break;
      case -2: {
        bcema_SharedPtr<bdem_Schema> schemaPtr;  schemaPtr.createInplace();

        bdem_RecordDef *usageDef = schemaPtr.ptr()->createRecord("Usage");
        usageDef->appendField(bdem_ElemType::BDEM_INT, "Sid");
        usageDef->appendField(bdem_ElemType::BDEM_STRING, "UserType");
        usageDef->appendField(bdem_ElemType::BDEM_DATE, "Date");

        bdem_RecordDef *requestDef = schemaPtr.ptr()->createRecord("Request");
        requestDef->appendField(bdem_ElemType::BDEM_LIST, usageDef,
                                "DailyUsage");

        {
            const char INPUT[] =
                "<?xml version='1.0' encoding='UTF-8'?>\n"
                "  <Request\n"
                "   xmlns:xsi='http://www.w3.org/2001/XMLSchema-instance'\n"
                "   xsi:schemaLocation='http://bloomberg.com/schemas/apiy "
                                                               "apiysvc.xsd'\n"
                "   xmlns='http://bloomberg.com/schemas/apiy'>\n"
                "   <DailyUsage>\n"
                "   <Sid>101226</Sid>\n"
                "   <UserType>DTC</UserType>\n"
                "   <Date>2005-04-07</Date>\n"
                "   </DailyUsage>\n"
                "  </Request>\n";

            bsl::stringstream ss(INPUT);

            bsl::cout << "INPUT = \n" << INPUT;
            bsl::cout << bsl::endl << bsl::endl;

            bcem_Aggregate aggregate(schemaPtr,
                                     "Request",
                                     bdem_ElemType::BDEM_LIST);

            baexml_MiniReader     reader;
            baexml_ErrorInfo      errInfo;
            baexml_DecoderOptions options;

            baexml_Decoder decoder(&options, &reader, &errInfo,
                                   &bsl::cerr, &bsl::cerr);

            decoder.decode(ss, &aggregate);
            ASSERT(ss);

            aggregate.asElemRef().theList().print(bsl::cout);
        }
      } break;
      case -3: {
          // ------------------------------------------------------------------
          // INTERACTIVE TEST
          //
          // Usage baexml_decoderutil.t -3 schemafile datafile root
          // ------------------------------------------------------------------

          if (argc < 5) {
              cerr << "Usage: " << argv[0]
                   << " -3 schemafile datafile rootElement\n";
              ++testStatus;
              break;
          }

          const char* schemaFileName = argv[2];
          const char* dataFileName = argv[3];
          const char* rootElement = argv[4];

          verbose = argc > 5;
          veryVerbose = argc > 6;
          veryVeryVerbose = argc > 7;
          veryVeryVeryVerbose = argc > 8;

          bsl::ifstream schemaFile(schemaFileName);
          ASSERT(schemaFile);
          if (! schemaFile) break;

          bsl::ifstream dataFile(dataFileName);
          ASSERT(dataFile);
          if (! dataFile) break;

          bcema_SharedPtr<bdem_Schema> schema;
          schema.createInplace();
          baexml_MiniReader reader;
          baexml_ErrorInfo errorInfo;
          baexml_SchemaParser schemaParser(&reader, &errorInfo,
                                           (veryVeryVerbose ? &bsl::cerr : 0));
          bsl::string targetNamespace;
          int result = schemaParser.parse(schemaFile, &*schema,
                                          &targetNamespace, schemaFileName);
          ASSERT(0 == result);
          if (result) {
              cerr << "Error parsing schema " << errorInfo.source() << ":"
                   << errorInfo.lineNumber() << '('
                   << errorInfo.columnNumber() << ") :"
                   << errorInfo.message() << bsl::endl;
              break;
          }
          if (verbose) cout << "Target namespace = " << targetNamespace
                            << bsl::endl;
          if (veryVerbose) P(*schema);

          bcem_Aggregate aggregate(schema, rootElement);
          ASSERT(! aggregate.isError());
          if (aggregate.isError()) {
              cerr << "Cannot create aggregate: " << aggregate.errorMessage()
                   << endl;
              break;
          }

          baexml_DecoderOptions decoderOptions;
          decoderOptions.setSkipUnknownElements(false);

          bsl::cout << "Decoding..." << bsl::endl;
          baexml_Decoder decoder(&decoderOptions, &reader, &errorInfo,
                                 &bsl::cerr, &bsl::cerr);
//          schemaFile.seekg(0);
//          reader.addSchema(schemaFile.rdbuf());
          decoder.decode(dataFile, &aggregate);
          ASSERT(! dataFile.fail());
          if (result) {
              cerr << "Error parsing schema " << errorInfo.source() << ":"
                   << errorInfo.lineNumber() << '('
                   << errorInfo.columnNumber() << ") :"
                   << errorInfo.message() << bsl::endl;
              break;
          }

          P(aggregate);

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
//      Copyright (C) Bloomberg L.P., 2005
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
