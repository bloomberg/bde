// baexml_encoder.t.cpp    -*-C++-*-

#include <baexml_encoder.h>
#include <baexml_decoder.h>
#include <baexml_minireader.h>

#include <bdem_elemtype.h>
#include <bdem_list.h>
#include <bdem_row.h>
#include <bdem_schema.h>
#include <bdem_schemaaggregateutil.h>
#include <bdem_table.h>

#include <bcem_aggregate.h>
#include <bcema_sharedptr.h>

#include <bdeu_printmethods.h>  // for printing vector

#include <bdesb_memoutstreambuf.h>
#include <bdesb_fixedmeminstreambuf.h>

#include <bsl_iostream.h>
#include <bsl_sstream.h>

#include <bsl_cstdio.h>

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
//-----------------------------------------------------------------------------
//
// CREATORS
//
// MANIPULATORS
//
// ACCESSORS
//-----------------------------------------------------------------------------
//

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

typedef baexml_Encoder        Encoder;
typedef baexml_EncoderOptions EncoderOptions;
typedef baexml_EncodingStyle  EncodingStyle;

typedef baexml_Decoder           Decoder;
typedef baexml_DecoderOptions    DecoderOptions;

int verbose;
int veryVerbose;
int veryVeryVerbose;
int veryVeryVeryVerbose;

// test_mysequence.h   -*-C++-*-
#ifndef INCLUDED_TEST_MYSEQUENCE
#define INCLUDED_TEST_MYSEQUENCE

//@PURPOSE:
//  todo: provide purpose
//
//@CLASSES: MySequence
//
//@AUTHOR: Author Unknown
//
//@DESCRIPTION:
//  todo: provide annotation

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

#ifndef INCLUDED_BSL_IOSFWD
#include <bsl_iosfwd.h>
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

#ifndef INCLUDED_BSL_STRING
#include <bsl_string.h>
#endif

namespace BloombergLP {

class bslma_Allocator;

namespace test {

class MySequence {

  private:
    int d_attribute1; // todo: provide annotation
    bsl::string d_attribute2; // todo: provide annotation

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
        // Create an object of type 'MySequence' having the default value.
        // Use the optionally specified 'basicAllocator' to supply memory.
        // If 'basicAllocator' is 0, the currently installed default allocator
        // is used.

    MySequence(const MySequence&   original,
               bslma_Allocator    *basicAllocator = 0);
        // Create an object of type 'MySequence' having the value
        // of the specified 'original' object.  Use the optionally specified
        // 'basicAllocator' to supply memory.  If 'basicAllocator' is 0,
        // the currently installed default allocator is used.

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
        // Return a reference to the non-modifiable "Attribute1"
        // attribute of this object.

    const bsl::string& attribute2() const;
        // Return a reference to the non-modifiable "Attribute2"
        // attribute of this object.

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

// ============================================================================
//                      INLINE FUNCTION DEFINITIONS
// ============================================================================

// The following inlined functions are invoked from other inline functions.

inline
int MySequence::maxSupportedBdexVersion()
{
    return 1;  // versions start at 1.
}

// ---------------------------------------------------------------------------

// CREATORS
inline
MySequence::MySequence(bslma_Allocator *basicAllocator)
: d_attribute1()
, d_attribute2(bslma_Default::allocator(basicAllocator))
{
}

inline
MySequence::MySequence(
    const MySequence& original,
    bslma_Allocator *basicAllocator)
: d_attribute1(original.d_attribute1)
, d_attribute2(original.d_attribute2, bslma_Default::allocator(basicAllocator))
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
            bdex_InStreamFunctions::streamIn(
                  stream, d_attribute1, 1);
            bdex_InStreamFunctions::streamIn(
                  stream, d_attribute2, 1);
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
        return ret;                                                 // RETURN
    }

    ret = manipulator(&d_attribute2,
                      ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE2]);
    if (ret) {
        return ret;                                                 // RETURN
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
int MySequence::manipulateAttribute(MANIPULATOR&  manipulator,
                                     const char   *name,
                                     int           nameLength)
{
    enum { NOT_FOUND = -1 };

    const bdeat_AttributeInfo *attributeInfo =
           lookupAttributeInfo(name, nameLength);
    if (0 == attributeInfo) {
        return NOT_FOUND;                                           // RETURN
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
        return ret;                                                 // RETURN
    }

    ret = accessor(d_attribute2,
                   ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE2]);
    if (ret) {
        return ret;                                                 // RETURN
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
int MySequence::accessAttribute(ACCESSOR&   accessor,
                                 const char *name,
                                 int         nameLength) const
{
    enum { NOT_FOUND = -1 };

     const bdeat_AttributeInfo *attributeInfo =
           lookupAttributeInfo(name, nameLength);
     if (0 == attributeInfo) {
        return NOT_FOUND;                                           // RETURN
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

}  // close namespace test;

// TRAITS
BDEAT_DECL_SEQUENCE_WITH_ALLOCATOR_TRAITS(test::MySequence)

// FREE OPERATORS
inline
bool test::operator==(const test::MySequence& lhs, const test::MySequence& rhs)
{
    return  lhs.attribute1() == rhs.attribute1()
         && lhs.attribute2() == rhs.attribute2();
}

inline
bool test::operator!=(const test::MySequence& lhs, const test::MySequence& rhs)
{
    return  lhs.attribute1() != rhs.attribute1()
         || lhs.attribute2() != rhs.attribute2();
}

inline
bsl::ostream& test::operator<<(
                bsl::ostream&            stream,
                const test::MySequence&  rhs)
{
    return rhs.print(stream, 0, -1);
}

}  // close namespace BloombergLP;

#endif

// ---------------------------------------------------------------------------
// *End-of-File Block removed*
// ---------------------------------------------------------------------------

// test_mysequence.cpp  -*-C++-*-

#include <bsl_iostream.h>

#include <bdeu_print.h>
#include <bdeu_printmethods.h>
#include <bdeu_chartype.h>
#include <bdeat_formattingmode.h>

#include <bsl_string.h>

namespace BloombergLP {
namespace test {

                               // ---------
                               // CONSTANTS
                               // ---------

const char MySequence::CLASS_NAME[] = "MySequence";
    // the name of this class

const bdeat_AttributeInfo MySequence::ATTRIBUTE_INFO_ARRAY[] = {
    {
        ATTRIBUTE_ID_ATTRIBUTE1,
        "Attribute1",                     // name
        sizeof("Attribute1") - 1,         // name length
        "todo: provide annotation",// annotation
        bdeat_FormattingMode::BDEAT_DEC // formatting mode
    },
    {
        ATTRIBUTE_ID_ATTRIBUTE2,
        "Attribute2",                     // name
        sizeof("Attribute2") - 1,         // name length
        "todo: provide annotation",// annotation
        bdeat_FormattingMode::BDEAT_TEXT // formatting mode
    }
};

                               // -------------
                               // CLASS METHODS
                               // -------------

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
             && bdeu_CharType::toUpper(name[8])=='E') {
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

                                // --------
                                // CREATORS
                                // --------

                                // ------------
                                // MANIPULATORS
                                // ------------

                                // ---------
                                // ACCESSORS
                                // ---------

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

}  // close namespace test;
}  // close namespace BloombergLP;

// ---------------------------------------------------------------------------
// *End-of-File Block removed*
// ---------------------------------------------------------------------------

// test_mysequencewithnullables.h   -*-C++-*-
#ifndef INCLUDED_TEST_MYSEQUENCEWITHNULLABLES
#define INCLUDED_TEST_MYSEQUENCEWITHNULLABLES

//@PURPOSE:
//  todo: provide purpose
//
//@CLASSES: MySequenceWithNullables
//
//@AUTHOR: Author Unknown
//
//@DESCRIPTION:
//  todo: provide annotation

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

#ifndef INCLUDED_BSL_IOSFWD
#include <bsl_iosfwd.h>
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

namespace BloombergLP {

class bslma_Allocator;

namespace test {

class MySequenceWithNullables {

  private:
    bdeut_NullableValue<int> d_attribute1; // todo: provide annotation
    bdeut_NullableValue<bsl::string> d_attribute2; // todo: provide annotation
    bdeut_NullableValue<MySequence> d_attribute3; // todo: provide annotation

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

    MySequenceWithNullables(
                const MySequenceWithNullables&  original,
                bslma_Allocator                *basicAllocator = 0);
        // Create an object of type 'MySequenceWithNullables' having the value
        // of the specified 'original' object.  Use the optionally specified
        // 'basicAllocator' to supply memory.  If 'basicAllocator' is 0,
        // the currently installed default allocator is used.

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
        // Return a reference to the non-modifiable "Attribute1"
        // attribute of this object.

    const bdeut_NullableValue<bsl::string>& attribute2() const;
        // Return a reference to the non-modifiable "Attribute2"
        // attribute of this object.

    const bdeut_NullableValue<MySequence>& attribute3() const;
        // Return a reference to the non-modifiable "Attribute3"
        // attribute of this object.

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

// ============================================================================
//                      INLINE FUNCTION DEFINITIONS
// ============================================================================

// The following inlined functions are invoked from other inline functions.

inline
int MySequenceWithNullables::maxSupportedBdexVersion()
{
    return 1;  // versions start at 1.
}

// ---------------------------------------------------------------------------

// CREATORS
inline
MySequenceWithNullables::MySequenceWithNullables(
                bslma_Allocator *basicAllocator)
: d_attribute1()
, d_attribute2(bslma_Default::allocator(basicAllocator))
, d_attribute3(bslma_Default::allocator(basicAllocator))
{
}

inline
MySequenceWithNullables::MySequenceWithNullables(
    const MySequenceWithNullables& original,
    bslma_Allocator *basicAllocator)
: d_attribute1(original.d_attribute1)
, d_attribute2(original.d_attribute2, bslma_Default::allocator(basicAllocator))
, d_attribute3(original.d_attribute3, bslma_Default::allocator(basicAllocator))
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
            bdex_InStreamFunctions::streamIn(
                  stream, d_attribute1, 1);
            bdex_InStreamFunctions::streamIn(
                  stream, d_attribute2, 1);
            bdex_InStreamFunctions::streamIn(
                  stream, d_attribute3, 1);
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
        return ret;                                                 // RETURN
    }

    ret = manipulator(&d_attribute2,
                      ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE2]);
    if (ret) {
        return ret;                                                 // RETURN
    }

    ret = manipulator(&d_attribute3,
                      ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE3]);
    if (ret) {
        return ret;                                                 // RETURN
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
int MySequenceWithNullables::manipulateAttribute(MANIPULATOR&  manipulator,
                                     const char   *name,
                                     int           nameLength)
{
    enum { NOT_FOUND = -1 };

    const bdeat_AttributeInfo *attributeInfo =
           lookupAttributeInfo(name, nameLength);
    if (0 == attributeInfo) {
        return NOT_FOUND;                                           // RETURN
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
        return ret;                                                 // RETURN
    }

    ret = accessor(d_attribute2,
                   ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE2]);
    if (ret) {
        return ret;                                                 // RETURN
    }

    ret = accessor(d_attribute3,
                   ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE3]);
    if (ret) {
        return ret;                                                 // RETURN
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
int MySequenceWithNullables::accessAttribute(ACCESSOR&   accessor,
                                 const char *name,
                                 int         nameLength) const
{
    enum { NOT_FOUND = -1 };

     const bdeat_AttributeInfo *attributeInfo =
           lookupAttributeInfo(name, nameLength);
     if (0 == attributeInfo) {
        return NOT_FOUND;                                           // RETURN
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

}  // close namespace test;

// TRAITS
BDEAT_DECL_SEQUENCE_WITH_ALLOCATOR_TRAITS(test::MySequenceWithNullables)

// FREE OPERATORS
inline
bool test::operator==(const test::MySequenceWithNullables& lhs,
                      const test::MySequenceWithNullables& rhs)
{
    return  lhs.attribute1() == rhs.attribute1()
         && lhs.attribute2() == rhs.attribute2()
         && lhs.attribute3() == rhs.attribute3();
}

inline
bool test::operator!=(const test::MySequenceWithNullables& lhs,
                      const test::MySequenceWithNullables& rhs)
{
    return  lhs.attribute1() != rhs.attribute1()
         || lhs.attribute2() != rhs.attribute2()
         || lhs.attribute3() != rhs.attribute3();
}

inline
bsl::ostream& test::operator<<(bsl::ostream&                         stream,
                               const test::MySequenceWithNullables&  rhs)
{
    return rhs.print(stream, 0, -1);
}

}  // close namespace BloombergLP;

#endif

// ---------------------------------------------------------------------------
// *End-of-File Block removed*
// ---------------------------------------------------------------------------

// test_mysequencewithnullables.cpp  -*-C++-*-

#include <bsl_iostream.h>

#include <bdeu_print.h>
#include <bdeu_printmethods.h>
#include <bdeu_chartype.h>
#include <bdeat_formattingmode.h>

#include <bdeut_nullablevalue.h>
#include <bsl_string.h>

namespace BloombergLP {
namespace test {

                               // ---------
                               // CONSTANTS
                               // ---------

const char MySequenceWithNullables::CLASS_NAME[] = "MySequenceWithNullables";
    // the name of this class

const bdeat_AttributeInfo MySequenceWithNullables::ATTRIBUTE_INFO_ARRAY[] = {
    {
        ATTRIBUTE_ID_ATTRIBUTE1,
        "Attribute1",                     // name
        sizeof("Attribute1") - 1,         // name length
        "todo: provide annotation",// annotation
        bdeat_FormattingMode::BDEAT_DEC // formatting mode
    },
    {
        ATTRIBUTE_ID_ATTRIBUTE2,
        "Attribute2",                     // name
        sizeof("Attribute2") - 1,         // name length
        "todo: provide annotation",// annotation
        bdeat_FormattingMode::BDEAT_TEXT // formatting mode
    },
    {
        ATTRIBUTE_ID_ATTRIBUTE3,
        "Attribute3",                     // name
        sizeof("Attribute3") - 1,         // name length
        "todo: provide annotation",// annotation
        bdeat_FormattingMode::BDEAT_DEFAULT // formatting mode
    }
};

                               // -------------
                               // CLASS METHODS
                               // -------------

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
             && bdeu_CharType::toUpper(name[8])=='E') {
                switch(bdeu_CharType::toUpper(name[9])) {
                    case '1': {
                        return
                            &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE1];
                    } break;
                    case '2': {
                        return
                            &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE2];
                    } break;
                    case '3': {
                        return
                            &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE3];
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

                                // --------
                                // CREATORS
                                // --------

                                // ------------
                                // MANIPULATORS
                                // ------------

                                // ---------
                                // ACCESSORS
                                // ---------

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

}  // close namespace test;
}  // close namespace BloombergLP;

// ---------------------------------------------------------------------------
// *End-of-File Block removed*
// ---------------------------------------------------------------------------

// test_mychoice.h   -*-C++-*-
#ifndef INCLUDED_TEST_MYCHOICE
#define INCLUDED_TEST_MYCHOICE

//@PURPOSE:
//  todo: provide purpose
//
//@CLASSES: MyChoice
//
//@AUTHOR: Author Unknown
//
//@DESCRIPTION:
//  todo: provide annotation

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_BSLMA_DEFAULT
#include <bslma_default.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITS
#include <bslalg_typetraits.h>
#endif

#ifndef INCLUDED_BDEAT_SELECTIONINFO
#include <bdeat_selectioninfo.h>
#endif

#ifndef INCLUDED_BDEAT_TYPETRAITS
#include <bdeat_typetraits.h>
#endif

#ifndef INCLUDED_BDEAT_VALUETYPEFUNCTIONS
#include <bdeat_valuetypefunctions.h>
#endif

#ifndef INCLUDED_BSL_IOSFWD
#include <bsl_iosfwd.h>
#endif

#ifndef INCLUDED_BDEX_INSTREAMFUNCTIONS
#include <bdex_instreamfunctions.h>
#endif

#ifndef INCLUDED_BDEX_OUTSTREAMFUNCTIONS
#include <bdex_outstreamfunctions.h>
#endif

#ifndef INCLUDED_BSLS_OBJECTBUFFER
#include <bsls_objectbuffer.h>
#endif

#ifndef INCLUDED_BDEU_PRINTMETHODS
#include <bdeu_printmethods.h>
#endif

#ifndef INCLUDED_BSL_STRING
#include <bsl_string.h>
#endif

namespace BloombergLP {

class bslma_Allocator;

namespace test {

class MyChoice {

  private:
    union {
        bsls_ObjectBuffer< int > d_selection1;
            // todo: provide annotation
        bsls_ObjectBuffer< bsl::string > d_selection2;
            // todo: provide annotation
    };

    int                 d_selectionId;

    bslma_Allocator    *d_allocator_p;

  public:
    // TYPES
    enum {
        NUM_SELECTIONS = 2 // the number of selections in this class
    };

    enum {
        SELECTION_INDEX_SELECTION1 = 0,
            // index for "Selection1" selection
        SELECTION_INDEX_SELECTION2 = 1
            // index for "Selection2" selection
    };

    enum {
        SELECTION_ID_UNDEFINED = -1,

        SELECTION_ID_SELECTION1 = 0,
            // id for "Selection1" selection
        SELECTION_ID_SELECTION2 = 1
            // id for "Selection2" selection
    };

  public:
    // CONSTANTS
    static const char CLASS_NAME[];
        // the name of this class (i.e., "MyChoice")

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
    explicit MyChoice(bslma_Allocator *basicAllocator = 0);
        // Create an object of type 'MyChoice' having the default value.
        // Use the optionally specified 'basicAllocator' to supply memory.
        // If 'basicAllocator' is 0, the currently installed default allocator
        // is used.

    MyChoice(const MyChoice& original, bslma_Allocator *basicAllocator = 0);
        // Create an object of type 'MyChoice' having the value
        // of the specified 'original' object.  Use the optionally specified
        // 'basicAllocator' to supply memory.  If 'basicAllocator' is 0,
        // the currently installed default allocator is used.

    ~MyChoice();
        // Destroy this object.

    // MANIPULATORS
    MyChoice& operator=(const MyChoice& rhs);
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

    int makeSelection(int selectionId);
        // Set the value of this object to be the default for the selection
        // indicated by the specified 'selectionId'.  Return 0 on success, and
        // non-zero value otherwise (i.e., the selection is not found).

    int makeSelection(const char *name, int nameLength);
        // Set the value of this object to be the default for the selection
        // indicated by the specified 'name' of the specified 'nameLength'.
        // Return 0 on success, and non-zero value otherwise (i.e., the
        // selection is not found).

    void makeSelection1();
    void makeSelection1(int value);
        // Set the value of this object to be a "Selection1" value.
        // Optionally specify the 'value' of the "Selection1".  If
        // 'value' is not specified, the default "Selection1" value is
        // used.

    void makeSelection2();
    void makeSelection2(const bsl::string& value);
        // Set the value of this object to be a "Selection2" value.
        // Optionally specify the 'value' of the "Selection2".  If
        // 'value' is not specified, the default "Selection2" value is
        // used.

    template<class MANIPULATOR>
    int manipulateSelection(MANIPULATOR& manipulator);
        // Invoke the specified 'manipulator' on the address of the modifiable
        // selection, supplying 'manipulator' with the corresponding selection
        // information structure.  Return the value returned from the
        // invocation of 'manipulator' if this object has a defined selection,
        // and -1 otherwise.

    int& selection1();
        // Return a reference to the modifiable "Selection1" selection
        // of this object if "Selection1" is the current selection.
        // The behavior is undefined unless "Selection1" is the
        // selection of this object.

    bsl::string& selection2();
        // Return a reference to the modifiable "Selection2" selection
        // of this object if "Selection2" is the current selection.
        // The behavior is undefined unless "Selection2" is the
        // selection of this object.

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

    const int& selection1() const;
        // Return a reference to the non-modifiable "Selection1"
        // selection of this object if "Selection1" is the current
        // selection.  The behavior is undefined unless "Selection1"
        // is the selection of this object.

    const bsl::string& selection2() const;
        // Return a reference to the non-modifiable "Selection2"
        // selection of this object if "Selection2" is the current
        // selection.  The behavior is undefined unless "Selection2"
        // is the selection of this object.

};

// FREE OPERATORS
inline
bool operator==(const MyChoice& lhs, const MyChoice& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects have the same
    // value, and 'false' otherwise.  Two 'MyChoice' objects have the same
    // value if either the selections in both objects have the same ids and
    // the same values, or both selections are undefined.

inline
bool operator!=(const MyChoice& lhs, const MyChoice& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects do not have the
    // same values, as determined by 'operator==', and 'false' otherwise.

inline
bsl::ostream& operator<<(bsl::ostream& stream, const MyChoice& rhs);
    // Format the specified 'rhs' to the specified output 'stream' and
    // return a reference to the modifiable 'stream'.

// ============================================================================
//                      INLINE FUNCTION DEFINITIONS
// ============================================================================

// The following inlined functions are invoked from other inline functions.

inline
int MyChoice::maxSupportedBdexVersion()
{
    return 1;  // versions start at 1.
}

inline
void MyChoice::reset()
{
    switch (d_selectionId) {
      case SELECTION_ID_SELECTION1: {
        // no destruction required
      } break;
      case SELECTION_ID_SELECTION2: {
        typedef bsl::string Type;
        d_selection2.object().~Type();
      } break;
      default:
        BSLS_ASSERT_SAFE(SELECTION_ID_UNDEFINED == d_selectionId);
    }

    d_selectionId = SELECTION_ID_UNDEFINED;
}

inline
void MyChoice::makeSelection1()
{
    if (SELECTION_ID_SELECTION1 == d_selectionId) {
        bdeat_ValueTypeFunctions::reset(&d_selection1.object());
    }
    else {
        reset();
        new (d_selection1.buffer()) int;
        d_selectionId = SELECTION_ID_SELECTION1;
    }
}

inline
void MyChoice::makeSelection1(int value)
{
    if (SELECTION_ID_SELECTION1 == d_selectionId) {
        d_selection1.object() = value;
    }
    else {
        reset();
        new (d_selection1.buffer()) int(value);
        d_selectionId = SELECTION_ID_SELECTION1;
    }
}

inline
void MyChoice::makeSelection2()
{
    if (SELECTION_ID_SELECTION2 == d_selectionId) {
        bdeat_ValueTypeFunctions::reset(&d_selection2.object());
    }
    else {
        reset();
        new (d_selection2.buffer()) bsl::string(d_allocator_p);
        d_selectionId = SELECTION_ID_SELECTION2;
    }
}

inline
void MyChoice::makeSelection2(const bsl::string& value)
{
    if (SELECTION_ID_SELECTION2 == d_selectionId) {
        d_selection2.object() = value;
    }
    else {
        reset();
        new (d_selection2.buffer()) bsl::string(value, d_allocator_p);
        d_selectionId = SELECTION_ID_SELECTION2;
    }
}

// ---------------------------------------------------------------------------

// CREATORS
inline
MyChoice::MyChoice(bslma_Allocator *basicAllocator)
: d_selectionId(SELECTION_ID_UNDEFINED)
, d_allocator_p(bslma_Default::allocator(basicAllocator))
{
}

inline
MyChoice::MyChoice(
    const MyChoice& original,
    bslma_Allocator *basicAllocator)
: d_selectionId(original.d_selectionId)
, d_allocator_p(bslma_Default::allocator(basicAllocator))
{
    switch (d_selectionId) {
      case SELECTION_ID_SELECTION1: {
        new (d_selection1.buffer())
            int(original.d_selection1.object());
      } break;
      case SELECTION_ID_SELECTION2: {
        new (d_selection2.buffer())
            bsl::string(original.d_selection2.object(), d_allocator_p);
      } break;
      default:
        BSLS_ASSERT_SAFE(SELECTION_ID_UNDEFINED == d_selectionId);
    }
}

inline
MyChoice::~MyChoice()
{
    reset();
}

// MANIPULATORS
inline
MyChoice&
MyChoice::operator=(const MyChoice& rhs)
{
    if (this != &rhs) {
        switch (rhs.d_selectionId) {
          case SELECTION_ID_SELECTION1: {
            makeSelection1(rhs.d_selection1.object());
          } break;
          case SELECTION_ID_SELECTION2: {
            makeSelection2(rhs.d_selection2.object());
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
STREAM& MyChoice::bdexStreamIn(STREAM& stream, int version)
{
    if (stream) {
        switch (version) {  // Switch on the schema version (starting with 1).
          case 1: {
            short selectionId;
            stream.getInt16(selectionId);
            if (!stream) {
                return stream;                                      // RETURN
            }
            switch (selectionId) {
              case SELECTION_ID_SELECTION1: {
                makeSelection1();
                bdex_InStreamFunctions::streamIn(
                    stream, d_selection1.object(), 1);
              } break;
              case SELECTION_ID_SELECTION2: {
                makeSelection2();
                bdex_InStreamFunctions::streamIn(
                    stream, d_selection2.object(), 1);
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
int MyChoice::makeSelection(int selectionId)
{
    enum { NOT_FOUND = -1, SUCCESS = 0 };

    switch (selectionId) {
      case SELECTION_ID_SELECTION1: {
        makeSelection1();
      } break;
      case SELECTION_ID_SELECTION2: {
        makeSelection2();
      } break;
      case SELECTION_ID_UNDEFINED: {
        reset();
      } break;
      default:
        return NOT_FOUND;                                           // RETURN
    }
    return SUCCESS;
}

inline
int MyChoice::makeSelection(const char *name, int nameLength)
{
    enum { NOT_FOUND = -1 };

    const bdeat_SelectionInfo *selectionInfo =
           lookupSelectionInfo(name, nameLength);
    if (0 == selectionInfo) {
       return NOT_FOUND;                                            // RETURN
    }

    return makeSelection(selectionInfo->d_id);
}

template <class MANIPULATOR>
inline
int MyChoice::manipulateSelection(MANIPULATOR& manipulator)
{
    enum { FAILURE = -1, SUCCESS = 0 };

    switch (d_selectionId) {
      case MyChoice::SELECTION_ID_SELECTION1:
        return manipulator(&d_selection1.object(),
                           SELECTION_INFO_ARRAY[SELECTION_INDEX_SELECTION1]);
                                                                      // RETURN
      case MyChoice::SELECTION_ID_SELECTION2:
        return manipulator(&d_selection2.object(),
                           SELECTION_INFO_ARRAY[SELECTION_INDEX_SELECTION2]);
                                                                      // RETURN
      default:
        BSLS_ASSERT_SAFE(MyChoice::SELECTION_ID_UNDEFINED ==
                     d_selectionId);
        return FAILURE;
    }
}

inline
int& MyChoice::selection1()
{
    BSLS_ASSERT_SAFE(SELECTION_ID_SELECTION1 == d_selectionId);
    return d_selection1.object();
}

inline
bsl::string& MyChoice::selection2()
{
    BSLS_ASSERT_SAFE(SELECTION_ID_SELECTION2 == d_selectionId);
    return d_selection2.object();
}

// ACCESSORS
template <class STREAM>
inline
STREAM& MyChoice::bdexStreamOut(STREAM& stream, int version) const
{
    switch (version) {
      case 1: {
            stream.putInt16(d_selectionId);
            switch (d_selectionId) {
              case SELECTION_ID_SELECTION1: {
                bdex_OutStreamFunctions::streamOut(
                    stream, d_selection1.object(), 1);
              } break;
              case SELECTION_ID_SELECTION2: {
                bdex_OutStreamFunctions::streamOut(
                    stream, d_selection2.object(), 1);
              } break;
              default:
                BSLS_ASSERT_SAFE(SELECTION_ID_UNDEFINED == d_selectionId);
            }
      } break;
    }
    return stream;
}

inline
int MyChoice::selectionId() const
{
    return d_selectionId;
}

template <class ACCESSOR>
inline
int MyChoice::accessSelection(ACCESSOR& accessor) const
{
    enum { FAILURE = -1, SUCCESS = 0 };

    switch (d_selectionId) {
      case SELECTION_ID_SELECTION1:
        return accessor(d_selection1.object(),
                        SELECTION_INFO_ARRAY[SELECTION_INDEX_SELECTION1]);
                                                                      // RETURN
      case SELECTION_ID_SELECTION2:
        return accessor(d_selection2.object(),
                        SELECTION_INFO_ARRAY[SELECTION_INDEX_SELECTION2]);
                                                                      // RETURN
      default:
        BSLS_ASSERT_SAFE(SELECTION_ID_UNDEFINED == d_selectionId);
        return FAILURE;
    }
}

inline
const int& MyChoice::selection1() const
{
    BSLS_ASSERT_SAFE(SELECTION_ID_SELECTION1 == d_selectionId);
    return d_selection1.object();
}

inline
const bsl::string& MyChoice::selection2() const
{
    BSLS_ASSERT_SAFE(SELECTION_ID_SELECTION2 == d_selectionId);
    return d_selection2.object();
}

}  // close namespace test;

// TRAITS
BDEAT_DECL_CHOICE_WITH_ALLOCATOR_TRAITS(test::MyChoice)

// FREE OPERATORS
inline
bool test::operator==(const test::MyChoice& lhs,
                      const test::MyChoice& rhs)
{
    if (lhs.selectionId() == rhs.selectionId()) {
        switch (rhs.selectionId()) {
          case test::MyChoice::SELECTION_ID_SELECTION1:
            return lhs.selection1() == rhs.selection1();
                                                                    // RETURN
          case test::MyChoice::SELECTION_ID_SELECTION2:
            return lhs.selection2() == rhs.selection2();
                                                                    // RETURN
          default:
            BSLS_ASSERT_SAFE(test::MyChoice::SELECTION_ID_UNDEFINED
                            == rhs.selectionId());
            return true;                                            // RETURN
        }
    }
    else {
        return false;
   }
}

inline
bool test::operator!=(const test::MyChoice& lhs,
                      const test::MyChoice& rhs)
{
    return !(lhs == rhs);
}

inline
bsl::ostream& test::operator<<(bsl::ostream& stream, const test::MyChoice& rhs)
{
    return rhs.print(stream, 0, -1);
}

}  // close namespace BloombergLP;

#endif

// ---------------------------------------------------------------------------
// *End-of-File Block removed*
// ---------------------------------------------------------------------------

// test_mychoice.cpp  -*-C++-*-

#include <bsl_iostream.h>

#include <bdeu_print.h>
#include <bdeu_printmethods.h>
#include <bdeu_chartype.h>
#include <bdeat_formattingmode.h>

#include <bsl_string.h>

namespace BloombergLP {
namespace test {

                               // ---------
                               // CONSTANTS
                               // ---------

const char MyChoice::CLASS_NAME[] = "MyChoice";
    // the name of this class

const bdeat_SelectionInfo MyChoice::SELECTION_INFO_ARRAY[] = {
    {
        SELECTION_ID_SELECTION1,
        "Selection1",                         // name
        sizeof("Selection1") - 1,             // name length
        "todo: provide annotation",    // annotation
        bdeat_FormattingMode::BDEAT_DEC // formatting mode
    },
    {
        SELECTION_ID_SELECTION2,
        "Selection2",                         // name
        sizeof("Selection2") - 1,             // name length
        "todo: provide annotation",    // annotation
        bdeat_FormattingMode::BDEAT_TEXT // formatting mode
    }
};

                               // -------------
                               // CLASS METHODS
                               // -------------

const bdeat_SelectionInfo *MyChoice::lookupSelectionInfo(
        const char *name,
        int         nameLength)
{
    switch(nameLength) {
        case 10: {
            if (bdeu_CharType::toUpper(name[0])=='S'
             && bdeu_CharType::toUpper(name[1])=='E'
             && bdeu_CharType::toUpper(name[2])=='L'
             && bdeu_CharType::toUpper(name[3])=='E'
             && bdeu_CharType::toUpper(name[4])=='C'
             && bdeu_CharType::toUpper(name[5])=='T'
             && bdeu_CharType::toUpper(name[6])=='I'
             && bdeu_CharType::toUpper(name[7])=='O'
             && bdeu_CharType::toUpper(name[8])=='N') {
                switch(bdeu_CharType::toUpper(name[9])) {
                    case '1': {
                        return
                            &SELECTION_INFO_ARRAY[SELECTION_INDEX_SELECTION1];
                    } break;
                    case '2': {
                        return
                            &SELECTION_INFO_ARRAY[SELECTION_INDEX_SELECTION2];
                    } break;
                }
            }
        } break;
    }
    return 0;
}

const bdeat_SelectionInfo *MyChoice::lookupSelectionInfo(int id)
{
    switch (id) {
      case SELECTION_ID_SELECTION1:
        return &SELECTION_INFO_ARRAY[SELECTION_INDEX_SELECTION1];
      case SELECTION_ID_SELECTION2:
        return &SELECTION_INFO_ARRAY[SELECTION_INDEX_SELECTION2];
      default:
        return 0;
    }
}

                                // --------
                                // CREATORS
                                // --------

                                // ------------
                                // MANIPULATORS
                                // ------------

                                // ---------
                                // ACCESSORS
                                // ---------

bsl::ostream& MyChoice::print(
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
          case SELECTION_ID_SELECTION1: {
            stream << "Selection1 = ";
            bdeu_PrintMethods::print(stream, d_selection1.object(),
                                     -levelPlus1, spacesPerLevel);
          } break;
          case SELECTION_ID_SELECTION2: {
            stream << "Selection2 = ";
            bdeu_PrintMethods::print(stream, d_selection2.object(),
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
          case SELECTION_ID_SELECTION1: {
            stream << "Selection1 = ";
            bdeu_PrintMethods::print(stream, d_selection1.object(),
                                     -levelPlus1, spacesPerLevel);
          } break;
          case SELECTION_ID_SELECTION2: {
            stream << "Selection2 = ";
            bdeu_PrintMethods::print(stream, d_selection2.object(),
                                     -levelPlus1, spacesPerLevel);
          } break;
          default:
            stream << "SELECTION UNDEFINED";
        }

        stream << " ]";
    }

    return stream << bsl::flush;
}

}  // close namespace test;
}  // close namespace BloombergLP;

// ---------------------------------------------------------------------------
// *End-of-File Block removed*
// ---------------------------------------------------------------------------

// test_mysequencewitharrays.h   -*-C++-*-
#ifndef INCLUDED_TEST_MYSEQUENCEWITHARRAYS
#define INCLUDED_TEST_MYSEQUENCEWITHARRAYS

//@PURPOSE:
//  todo: provide purpose
//
//@CLASSES: MySequenceWithArrays
//
//@AUTHOR: Author Unknown
//
//@DESCRIPTION:
//  todo: provide annotation

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

#ifndef INCLUDED_BSL_IOSFWD
#include <bsl_iosfwd.h>
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

#ifndef INCLUDED_BSL_VECTOR
#include <bsl_vector.h>
#endif

namespace BloombergLP {

class bslma_Allocator;

namespace test {

class MySequenceWithArrays {

  private:
    bsl::vector<int> d_attribute1; // todo: provide annotation
    bsl::vector<char> d_attribute2; // todo: provide annotation
    bsl::vector<char> d_attribute3; // todo: provide annotation
    bsl::vector<char> d_attribute4; // todo: provide annotation
    bsl::vector<char> d_attribute5; // todo: provide annotation
    bsl::vector<char> d_attribute6; // todo: provide annotation
    bsl::vector<int> d_attribute7; // todo: provide annotation

  public:
    // TYPES
    enum {
        NUM_ATTRIBUTES = 7 // the number of attributes in this class
    };

    enum {
        ATTRIBUTE_INDEX_ATTRIBUTE1 = 0,
            // index for "Attribute1" attribute
        ATTRIBUTE_INDEX_ATTRIBUTE2 = 1,
            // index for "Attribute2" attribute
        ATTRIBUTE_INDEX_ATTRIBUTE3 = 2,
            // index for "Attribute3" attribute
        ATTRIBUTE_INDEX_ATTRIBUTE4 = 3,
            // index for "Attribute4" attribute
        ATTRIBUTE_INDEX_ATTRIBUTE5 = 4,
            // index for "Attribute5" attribute
        ATTRIBUTE_INDEX_ATTRIBUTE6 = 5,
            // index for "Attribute6" attribute
        ATTRIBUTE_INDEX_ATTRIBUTE7 = 6
            // index for "Attribute7" attribute
    };

    enum {
        ATTRIBUTE_ID_ATTRIBUTE1 = 0,
            // id for "Attribute1" attribute
        ATTRIBUTE_ID_ATTRIBUTE2 = 1,
            // id for "Attribute2" attribute
        ATTRIBUTE_ID_ATTRIBUTE3 = 2,
            // id for "Attribute3" attribute
        ATTRIBUTE_ID_ATTRIBUTE4 = 3,
            // id for "Attribute4" attribute
        ATTRIBUTE_ID_ATTRIBUTE5 = 4,
            // id for "Attribute5" attribute
        ATTRIBUTE_ID_ATTRIBUTE6 = 5,
            // id for "Attribute6" attribute
        ATTRIBUTE_ID_ATTRIBUTE7 = 6
            // id for "Attribute7" attribute
    };

  public:
    // CONSTANTS
    static const char CLASS_NAME[];
        // the name of this class (i.e., "MySequenceWithArrays")

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
    explicit MySequenceWithArrays(bslma_Allocator *basicAllocator = 0);
        // Create an object of type 'MySequenceWithArrays' having the default
        // value.  Use the optionally specified 'basicAllocator' to supply
        // memory.  If 'basicAllocator' is 0, the currently installed default
        // allocator is used.

    MySequenceWithArrays(const MySequenceWithArrays&  original,
                         bslma_Allocator             *basicAllocator = 0);
        // Create an object of type 'MySequenceWithArrays' having the value
        // of the specified 'original' object.  Use the optionally specified
        // 'basicAllocator' to supply memory.  If 'basicAllocator' is 0,
        // the currently installed default allocator is used.

    ~MySequenceWithArrays();
        // Destroy this object.

    // MANIPULATORS
    MySequenceWithArrays& operator=(const MySequenceWithArrays& rhs);
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

    bsl::vector<int>& attribute1();
        // Return a reference to the modifiable "Attribute1" attribute of this
        // object.

    bsl::vector<char>& attribute2();
        // Return a reference to the modifiable "Attribute2" attribute of this
        // object.

    bsl::vector<char>& attribute3();
        // Return a reference to the modifiable "Attribute3" attribute of this
        // object.

    bsl::vector<char>& attribute4();
        // Return a reference to the modifiable "Attribute4" attribute of this
        // object.

    bsl::vector<char>& attribute5();
        // Return a reference to the modifiable "Attribute5" attribute of this
        // object.

    bsl::vector<char>& attribute6();
        // Return a reference to the modifiable "Attribute6" attribute of this
        // object.

    bsl::vector<int>& attribute7();
        // Return a reference to the modifiable "Attribute7" attribute of this
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

    const bsl::vector<int>& attribute1() const;
        // Return a reference to the non-modifiable "Attribute1"
        // attribute of this object.

    const bsl::vector<char>& attribute2() const;
        // Return a reference to the non-modifiable "Attribute2"
        // attribute of this object.

    const bsl::vector<char>& attribute3() const;
        // Return a reference to the non-modifiable "Attribute3"
        // attribute of this object.

    const bsl::vector<char>& attribute4() const;
        // Return a reference to the non-modifiable "Attribute4"
        // attribute of this object.

    const bsl::vector<char>& attribute5() const;
        // Return a reference to the non-modifiable "Attribute5"
        // attribute of this object.

    const bsl::vector<char>& attribute6() const;
        // Return a reference to the non-modifiable "Attribute6"
        // attribute of this object.

    const bsl::vector<int>& attribute7() const;
        // Return a reference to the non-modifiable "Attribute7"
        // attribute of this object.

};

// FREE OPERATORS
inline
bool operator==(const MySequenceWithArrays& lhs,
                const MySequenceWithArrays& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects have
    // the same value, and 'false' otherwise.  Two attribute objects have the
    // same value if each respective attribute has the same value.

inline
bool operator!=(const MySequenceWithArrays& lhs,
                const MySequenceWithArrays& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects do not
    // have the same value, and 'false' otherwise.  Two attribute objects do
    // not have the same value if one or more respective attributes differ in
    // values.

inline
bsl::ostream& operator<<(bsl::ostream&                stream,
                         const MySequenceWithArrays&  rhs);
    // Format the specified 'rhs' to the specified output 'stream' and
    // return a reference to the modifiable 'stream'.

// ============================================================================
//                      INLINE FUNCTION DEFINITIONS
// ============================================================================

// The following inlined functions are invoked from other inline functions.

inline
int MySequenceWithArrays::maxSupportedBdexVersion()
{
    return 1;  // versions start at 1.
}

// ---------------------------------------------------------------------------

// CREATORS
inline
MySequenceWithArrays::MySequenceWithArrays(bslma_Allocator *basicAllocator)
: d_attribute1(bslma_Default::allocator(basicAllocator))
, d_attribute2(bslma_Default::allocator(basicAllocator))
, d_attribute3(bslma_Default::allocator(basicAllocator))
, d_attribute4(bslma_Default::allocator(basicAllocator))
, d_attribute5(bslma_Default::allocator(basicAllocator))
, d_attribute6()
, d_attribute7()
{
}

inline
MySequenceWithArrays::MySequenceWithArrays(
    const MySequenceWithArrays& original,
    bslma_Allocator *basicAllocator)
: d_attribute1(original.d_attribute1, bslma_Default::allocator(basicAllocator))
, d_attribute2(original.d_attribute2, bslma_Default::allocator(basicAllocator))
, d_attribute3(original.d_attribute3, bslma_Default::allocator(basicAllocator))
, d_attribute4(original.d_attribute4, bslma_Default::allocator(basicAllocator))
, d_attribute5(original.d_attribute5, bslma_Default::allocator(basicAllocator))
, d_attribute6(original.d_attribute6)
, d_attribute7(original.d_attribute7)
{
}

inline
MySequenceWithArrays::~MySequenceWithArrays()
{
}

// MANIPULATORS
inline
MySequenceWithArrays&
MySequenceWithArrays::operator=(const MySequenceWithArrays& rhs)
{
    if (this != &rhs) {
        d_attribute1 = rhs.d_attribute1;
        d_attribute2 = rhs.d_attribute2;
        d_attribute3 = rhs.d_attribute3;
        d_attribute4 = rhs.d_attribute4;
        d_attribute5 = rhs.d_attribute5;
        d_attribute6 = rhs.d_attribute6;
        d_attribute7 = rhs.d_attribute7;
    }
    return *this;
}

template <class STREAM>
inline
STREAM& MySequenceWithArrays::bdexStreamIn(STREAM& stream, int version)
{
    if (stream) {
        switch (version) {  // Switch on the schema version (starting with 1).
          case 1: {
            bdex_InStreamFunctions::streamIn(
                  stream, d_attribute1, 1);
            bdex_InStreamFunctions::streamIn(
                  stream, d_attribute2, 1);
            bdex_InStreamFunctions::streamIn(
                  stream, d_attribute3, 1);
            bdex_InStreamFunctions::streamIn(
                  stream, d_attribute4, 1);
            bdex_InStreamFunctions::streamIn(
                  stream, d_attribute5, 1);
            bdex_InStreamFunctions::streamIn(
                  stream, d_attribute6, 1);
            bdex_InStreamFunctions::streamIn(
                  stream, d_attribute7, 1);
          } break;
          default: {
            stream.invalidate();
          }
        }
    }
    return stream;
}

inline
void MySequenceWithArrays::reset()
{
    bdeat_ValueTypeFunctions::reset(&d_attribute1);
    bdeat_ValueTypeFunctions::reset(&d_attribute2);
    bdeat_ValueTypeFunctions::reset(&d_attribute3);
    bdeat_ValueTypeFunctions::reset(&d_attribute4);
    bdeat_ValueTypeFunctions::reset(&d_attribute5);
    bdeat_ValueTypeFunctions::reset(&d_attribute6);
    bdeat_ValueTypeFunctions::reset(&d_attribute7);
}

template <class MANIPULATOR>
inline
int MySequenceWithArrays::manipulateAttributes(MANIPULATOR& manipulator)
{
    int ret;

    ret = manipulator(&d_attribute1,
                      ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE1]);
    if (ret) {
        return ret;                                                 // RETURN
    }

    ret = manipulator(&d_attribute2,
                      ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE2]);
    if (ret) {
        return ret;                                                 // RETURN
    }

    ret = manipulator(&d_attribute3,
                      ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE3]);
    if (ret) {
        return ret;                                                 // RETURN
    }

    ret = manipulator(&d_attribute4,
                      ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE4]);
    if (ret) {
        return ret;                                                 // RETURN
    }

    ret = manipulator(&d_attribute5,
                      ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE5]);
    if (ret) {
        return ret;                                                 // RETURN
    }

    ret = manipulator(&d_attribute6,
                      ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE6]);
    if (ret) {
        return ret;                                                 // RETURN
    }

    ret = manipulator(&d_attribute7,
                      ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE7]);
    if (ret) {
        return ret;                                                 // RETURN
    }

    return ret;
}

template <class MANIPULATOR>
inline
int MySequenceWithArrays::manipulateAttribute(MANIPULATOR& manipulator, int id)
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
      case ATTRIBUTE_ID_ATTRIBUTE4: {
        return manipulator(&d_attribute4,
                           ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE4]);
                                                                    // RETURN
      } break;
      case ATTRIBUTE_ID_ATTRIBUTE5: {
        return manipulator(&d_attribute5,
                           ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE5]);
                                                                    // RETURN
      } break;
      case ATTRIBUTE_ID_ATTRIBUTE6: {
        return manipulator(&d_attribute6,
                           ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE6]);
                                                                    // RETURN
      } break;
      case ATTRIBUTE_ID_ATTRIBUTE7: {
        return manipulator(&d_attribute7,
                           ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE7]);
                                                                    // RETURN
      } break;
      default:
        return NOT_FOUND;
    }
}

template <class MANIPULATOR>
inline
int MySequenceWithArrays::manipulateAttribute(MANIPULATOR&  manipulator,
                                     const char   *name,
                                     int           nameLength)
{
    enum { NOT_FOUND = -1 };

    const bdeat_AttributeInfo *attributeInfo =
           lookupAttributeInfo(name, nameLength);
    if (0 == attributeInfo) {
        return NOT_FOUND;                                           // RETURN
    }

    return manipulateAttribute(manipulator, attributeInfo->d_id);
}

inline
bsl::vector<int>& MySequenceWithArrays::attribute1()
{
    return d_attribute1;
}

inline
bsl::vector<char>& MySequenceWithArrays::attribute2()
{
    return d_attribute2;
}

inline
bsl::vector<char>& MySequenceWithArrays::attribute3()
{
    return d_attribute3;
}

inline
bsl::vector<char>& MySequenceWithArrays::attribute4()
{
    return d_attribute4;
}

inline
bsl::vector<char>& MySequenceWithArrays::attribute5()
{
    return d_attribute5;
}

inline
bsl::vector<char>& MySequenceWithArrays::attribute6()
{
    return d_attribute6;
}

inline
bsl::vector<int>& MySequenceWithArrays::attribute7()
{
    return d_attribute7;
}

// ACCESSORS
template <class STREAM>
inline
STREAM& MySequenceWithArrays::bdexStreamOut(STREAM& stream, int version) const
{
    switch (version) {
      case 1: {
        bdex_OutStreamFunctions::streamOut(stream, d_attribute1, 1);
        bdex_OutStreamFunctions::streamOut(stream, d_attribute2, 1);
        bdex_OutStreamFunctions::streamOut(stream, d_attribute3, 1);
        bdex_OutStreamFunctions::streamOut(stream, d_attribute4, 1);
        bdex_OutStreamFunctions::streamOut(stream, d_attribute5, 1);
        bdex_OutStreamFunctions::streamOut(stream, d_attribute6, 1);
        bdex_OutStreamFunctions::streamOut(stream, d_attribute7, 1);
      } break;
    }
    return stream;
}

template <class ACCESSOR>
inline
int MySequenceWithArrays::accessAttributes(ACCESSOR& accessor) const
{
    int ret;

    ret = accessor(d_attribute1,
                   ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE1]);
    if (ret) {
        return ret;                                                 // RETURN
    }

    ret = accessor(d_attribute2,
                   ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE2]);
    if (ret) {
        return ret;                                                 // RETURN
    }

    ret = accessor(d_attribute3,
                   ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE3]);
    if (ret) {
        return ret;                                                 // RETURN
    }

    ret = accessor(d_attribute4,
                   ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE4]);
    if (ret) {
        return ret;                                                 // RETURN
    }

    ret = accessor(d_attribute5,
                   ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE5]);
    if (ret) {
        return ret;                                                 // RETURN
    }

    ret = accessor(d_attribute6,
                   ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE6]);
    if (ret) {
        return ret;                                                 // RETURN
    }

    ret = accessor(d_attribute7,
                   ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE7]);
    if (ret) {
        return ret;                                                 // RETURN
    }

    return ret;
}

template <class ACCESSOR>
inline
int MySequenceWithArrays::accessAttribute(ACCESSOR& accessor, int id) const
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
      case ATTRIBUTE_ID_ATTRIBUTE4: {
        return accessor(d_attribute4,
                        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE4]);
                                                                    // RETURN
      } break;
      case ATTRIBUTE_ID_ATTRIBUTE5: {
        return accessor(d_attribute5,
                        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE5]);
                                                                    // RETURN
      } break;
      case ATTRIBUTE_ID_ATTRIBUTE6: {
        return accessor(d_attribute6,
                        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE6]);
                                                                    // RETURN
      } break;
      case ATTRIBUTE_ID_ATTRIBUTE7: {
        return accessor(d_attribute7,
                        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE7]);
                                                                    // RETURN
      } break;
      default:
        return NOT_FOUND;
    }
}

template <class ACCESSOR>
inline
int MySequenceWithArrays::accessAttribute(ACCESSOR&   accessor,
                                 const char *name,
                                 int         nameLength) const
{
    enum { NOT_FOUND = -1 };

     const bdeat_AttributeInfo *attributeInfo =
           lookupAttributeInfo(name, nameLength);
     if (0 == attributeInfo) {
        return NOT_FOUND;                                           // RETURN
     }

     return accessAttribute(accessor, attributeInfo->d_id);
}

inline
const bsl::vector<int>& MySequenceWithArrays::attribute1() const
{
    return d_attribute1;
}

inline
const bsl::vector<char>& MySequenceWithArrays::attribute2() const
{
    return d_attribute2;
}

inline
const bsl::vector<char>& MySequenceWithArrays::attribute3() const
{
    return d_attribute3;
}

inline
const bsl::vector<char>& MySequenceWithArrays::attribute4() const
{
    return d_attribute4;
}

inline
const bsl::vector<char>& MySequenceWithArrays::attribute5() const
{
    return d_attribute5;
}

inline
const bsl::vector<char>& MySequenceWithArrays::attribute6() const
{
    return d_attribute6;
}

inline
const bsl::vector<int>& MySequenceWithArrays::attribute7() const
{
    return d_attribute7;
}

}  // close namespace test;

// TRAITS
BDEAT_DECL_SEQUENCE_WITH_ALLOCATOR_TRAITS(test::MySequenceWithArrays)

// FREE OPERATORS
inline
bool test::operator==(const test::MySequenceWithArrays& lhs,
                      const test::MySequenceWithArrays& rhs)
{
    return  lhs.attribute1() == rhs.attribute1()
         && lhs.attribute2() == rhs.attribute2()
         && lhs.attribute3() == rhs.attribute3()
         && lhs.attribute4() == rhs.attribute4()
         && lhs.attribute5() == rhs.attribute5()
         && lhs.attribute6() == rhs.attribute6()
         && lhs.attribute7() == rhs.attribute7();
}

inline
bool test::operator!=(const test::MySequenceWithArrays& lhs,
                      const test::MySequenceWithArrays& rhs)
{
    return  lhs.attribute1() != rhs.attribute1()
         || lhs.attribute2() != rhs.attribute2()
         || lhs.attribute3() != rhs.attribute3()
         || lhs.attribute4() != rhs.attribute4()
         || lhs.attribute5() != rhs.attribute5()
         || lhs.attribute6() != rhs.attribute6()
         || lhs.attribute7() != rhs.attribute7();
}

inline
bsl::ostream& test::operator<<(bsl::ostream&                      stream,
                               const test::MySequenceWithArrays&  rhs)
{
    return rhs.print(stream, 0, -1);
}

}  // close namespace BloombergLP;

#endif

// ---------------------------------------------------------------------------
// *End-of-File Block removed*
// ---------------------------------------------------------------------------

// test_mysequencewitharrays.cpp  -*-C++-*-

#include <bsl_iostream.h>

#include <bdeu_print.h>
#include <bdeu_printmethods.h>
#include <bdeu_chartype.h>
#include <bdeat_formattingmode.h>

#include <bsl_vector.h>

namespace BloombergLP {
namespace test {

                               // ---------
                               // CONSTANTS
                               // ---------

const char MySequenceWithArrays::CLASS_NAME[] = "MySequenceWithArrays";
    // the name of this class

const bdeat_AttributeInfo MySequenceWithArrays::ATTRIBUTE_INFO_ARRAY[] = {
    {
        ATTRIBUTE_ID_ATTRIBUTE1,
        "Attribute1",                     // name
        sizeof("Attribute1") - 1,         // name length
        "todo: provide annotation",// annotation
        bdeat_FormattingMode::BDEAT_DEC // formatting mode
    },
    {
        ATTRIBUTE_ID_ATTRIBUTE2,
        "Attribute2",                     // name
        sizeof("Attribute2") - 1,         // name length
        "todo: provide annotation",// annotation
        bdeat_FormattingMode::BDEAT_BASE64 // formatting mode
    },
    {
        ATTRIBUTE_ID_ATTRIBUTE3,
        "Attribute3",                     // name
        sizeof("Attribute3") - 1,         // name length
        "todo: provide annotation",// annotation
        bdeat_FormattingMode::BDEAT_HEX // formatting mode
    },
    {
        ATTRIBUTE_ID_ATTRIBUTE4,
        "Attribute4",                     // name
        sizeof("Attribute4") - 1,         // name length
        "todo: provide annotation",// annotation
        bdeat_FormattingMode::BDEAT_TEXT // formatting mode
    },
    {
        ATTRIBUTE_ID_ATTRIBUTE5,
        "Attribute5",                     // name
        sizeof("Attribute5") - 1,         // name length
        "todo: provide annotation",// annotation
        bdeat_FormattingMode::BDEAT_DEC // formatting mode
    },
    {
        ATTRIBUTE_ID_ATTRIBUTE6,
        "Attribute6",                     // name
        sizeof("Attribute6") - 1,         // name length
        "todo: provide annotation",// annotation
        bdeat_FormattingMode::BDEAT_LIST // formatting mode
    },
    {
        ATTRIBUTE_ID_ATTRIBUTE7,
        "Attribute7",                     // name
        sizeof("Attribute7") - 1,         // name length
        "todo: provide annotation",// annotation
        bdeat_FormattingMode::BDEAT_LIST // formatting mode
    }
};

                               // -------------
                               // CLASS METHODS
                               // -------------

const bdeat_AttributeInfo *MySequenceWithArrays::lookupAttributeInfo(
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
             && bdeu_CharType::toUpper(name[8])=='E') {
                switch(bdeu_CharType::toUpper(name[9])) {
                    case '1': {
                        return
                            &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE1];
                    } break;
                    case '2': {
                        return
                            &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE2];
                    } break;
                    case '3': {
                        return
                            &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE3];
                    } break;
                    case '4': {
                        return
                            &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE4];
                    } break;
                    case '5': {
                        return
                            &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE5];
                    } break;
                    case '6': {
                        return
                            &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE6];
                    } break;
                    case '7': {
                        return
                            &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE7];
                    } break;
                }
            }
        } break;
    }
    return 0;
}

const bdeat_AttributeInfo *MySequenceWithArrays::lookupAttributeInfo(int id)
{
    switch (id) {
      case ATTRIBUTE_ID_ATTRIBUTE1:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE1];
      case ATTRIBUTE_ID_ATTRIBUTE2:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE2];
      case ATTRIBUTE_ID_ATTRIBUTE3:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE3];
      case ATTRIBUTE_ID_ATTRIBUTE4:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE4];
      case ATTRIBUTE_ID_ATTRIBUTE5:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE5];
      case ATTRIBUTE_ID_ATTRIBUTE6:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE6];
      case ATTRIBUTE_ID_ATTRIBUTE7:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE7];
      default:
        return 0;
    }
}

                                // --------
                                // CREATORS
                                // --------

                                // ------------
                                // MANIPULATORS
                                // ------------

                                // ---------
                                // ACCESSORS
                                // ---------

bsl::ostream& MySequenceWithArrays::print(
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

        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "Attribute4 = ";
        bdeu_PrintMethods::print(stream, d_attribute4,
                             -levelPlus1, spacesPerLevel);

        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "Attribute5 = ";
        bdeu_PrintMethods::print(stream, d_attribute5,
                             -levelPlus1, spacesPerLevel);

        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "Attribute6 = ";
        bdeu_PrintMethods::print(stream, d_attribute6,
                             -levelPlus1, spacesPerLevel);

        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "Attribute7 = ";
        bdeu_PrintMethods::print(stream, d_attribute7,
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

        stream << ' ';
        stream << "Attribute4 = ";
        bdeu_PrintMethods::print(stream, d_attribute4,
                             -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "Attribute5 = ";
        bdeu_PrintMethods::print(stream, d_attribute5,
                             -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "Attribute6 = ";
        bdeu_PrintMethods::print(stream, d_attribute6,
                             -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "Attribute7 = ";
        bdeu_PrintMethods::print(stream, d_attribute7,
                             -levelPlus1, spacesPerLevel);

        stream << " ]";
    }

    return stream << bsl::flush;
}

}  // close namespace test;
}  // close namespace BloombergLP;

// ---------------------------------------------------------------------------
// *End-of-File Block removed*
// ---------------------------------------------------------------------------

// test_mysequencewithanonymouschoicechoice.h   -*-C++-*-
#ifndef INCLUDED_TEST_MYSEQUENCEWITHANONYMOUSCHOICECHOICE
#define INCLUDED_TEST_MYSEQUENCEWITHANONYMOUSCHOICECHOICE

//@PURPOSE:
//  todo: provide purpose
//
//@CLASSES: test::MySequenceWithAnonymousChoiceChoice
//
//@AUTHOR: Author Unknown
//
//@DESCRIPTION:
//  todo: provide annotation

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_BSLMA_DEFAULT
#include <bslma_default.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITS
#include <bslalg_typetraits.h>
#endif

#ifndef INCLUDED_BDEAT_SELECTIONINFO
#include <bdeat_selectioninfo.h>
#endif

#ifndef INCLUDED_BDEAT_TYPETRAITS
#include <bdeat_typetraits.h>
#endif

#ifndef INCLUDED_BDEAT_VALUETYPEFUNCTIONS
#include <bdeat_valuetypefunctions.h>
#endif

#ifndef INCLUDED_BSL_IOSFWD
#include <bsl_iosfwd.h>
#endif

#ifndef INCLUDED_BDEX_INSTREAMFUNCTIONS
#include <bdex_instreamfunctions.h>
#endif

#ifndef INCLUDED_BDEX_OUTSTREAMFUNCTIONS
#include <bdex_outstreamfunctions.h>
#endif

#ifndef INCLUDED_BSLS_OBJECTBUFFER
#include <bsls_objectbuffer.h>
#endif

#ifndef INCLUDED_BDEU_PRINTMETHODS
#include <bdeu_printmethods.h>
#endif

#ifndef INCLUDED_BSL_STRING
#include <bsl_string.h>
#endif

namespace BloombergLP {

class bslma_Allocator;

namespace test {

class MySequenceWithAnonymousChoiceChoice {

  private:
    union {
        bsls_ObjectBuffer< int > d_myChoice1;
            // todo: provide annotation
        bsls_ObjectBuffer< bsl::string > d_myChoice2;
            // todo: provide annotation
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
        // Reset this object to the default value (i.e., its value upon
        // default construction).

    int makeSelection(int selectionId);
        // Set the value of this object to be the default for the selection
        // indicated by the specified 'selectionId'.  Return 0 on success, and
        // non-zero value otherwise (i.e., the selection is not found).

    int makeSelection(const char *name, int nameLength);
        // Set the value of this object to be the default for the selection
        // indicated by the specified 'name' of the specified 'nameLength'.
        // Return 0 on success, and non-zero value otherwise (i.e., the
        // selection is not found).

    void makeMyChoice1();
    void makeMyChoice1(int value);
        // Set the value of this object to be a "MyChoice1" value.
        // Optionally specify the 'value' of the "MyChoice1".  If
        // 'value' is not specified, the default "MyChoice1" value is
        // used.

    void makeMyChoice2();
    void makeMyChoice2(const bsl::string& value);
        // Set the value of this object to be a "MyChoice2" value.
        // Optionally specify the 'value' of the "MyChoice2".  If
        // 'value' is not specified, the default "MyChoice2" value is
        // used.

    template<class MANIPULATOR>
    int manipulateSelection(MANIPULATOR& manipulator);
        // Invoke the specified 'manipulator' on the address of the modifiable
        // selection, supplying 'manipulator' with the corresponding selection
        // information structure.  Return the value returned from the
        // invocation of 'manipulator' if this object has a defined selection,
        // and -1 otherwise.

    int& myChoice1();
        // Return a reference to the modifiable "MyChoice1" selection
        // of this object if "MyChoice1" is the current selection.
        // The behavior is undefined unless "MyChoice1" is the
        // selection of this object.

    bsl::string& myChoice2();
        // Return a reference to the modifiable "MyChoice2" selection
        // of this object if "MyChoice2" is the current selection.
        // The behavior is undefined unless "MyChoice2" is the
        // selection of this object.

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
        // Return a reference to the non-modifiable "MyChoice1"
        // selection of this object if "MyChoice1" is the current
        // selection.  The behavior is undefined unless "MyChoice1"
        // is the selection of this object.

    const bsl::string& myChoice2() const;
        // Return a reference to the non-modifiable "MyChoice2"
        // selection of this object if "MyChoice2" is the current
        // selection.  The behavior is undefined unless "MyChoice2"
        // is the selection of this object.

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
bsl::ostream& operator<<(bsl::ostream&                               stream,
                         const MySequenceWithAnonymousChoiceChoice&  rhs);
    // Format the specified 'rhs' to the specified output 'stream' and
    // return a reference to the modifiable 'stream'.

// ============================================================================
//                      INLINE FUNCTION DEFINITIONS
// ============================================================================

// The following inlined functions are invoked from other inline functions.

inline
int MySequenceWithAnonymousChoiceChoice::maxSupportedBdexVersion()
{
    return 1;  // versions start at 1.
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
void MySequenceWithAnonymousChoiceChoice::makeMyChoice1()
{
    if (SELECTION_ID_MY_CHOICE1 == d_selectionId) {
        bdeat_ValueTypeFunctions::reset(&d_myChoice1.object());
    }
    else {
        reset();
        new (d_myChoice1.buffer()) int;
        d_selectionId = SELECTION_ID_MY_CHOICE1;
    }
}

inline
void MySequenceWithAnonymousChoiceChoice::makeMyChoice1(int value)
{
    if (SELECTION_ID_MY_CHOICE1 == d_selectionId) {
        d_myChoice1.object() = value;
    }
    else {
        reset();
        new (d_myChoice1.buffer()) int(value);
        d_selectionId = SELECTION_ID_MY_CHOICE1;
    }
}

inline
void MySequenceWithAnonymousChoiceChoice::makeMyChoice2()
{
    if (SELECTION_ID_MY_CHOICE2 == d_selectionId) {
        bdeat_ValueTypeFunctions::reset(&d_myChoice2.object());
    }
    else {
        reset();
        new (d_myChoice2.buffer()) bsl::string(d_allocator_p);
        d_selectionId = SELECTION_ID_MY_CHOICE2;
    }
}

inline
void MySequenceWithAnonymousChoiceChoice::makeMyChoice2(
                const bsl::string& value)
{
    if (SELECTION_ID_MY_CHOICE2 == d_selectionId) {
        d_myChoice2.object() = value;
    }
    else {
        reset();
        new (d_myChoice2.buffer()) bsl::string(value, d_allocator_p);
        d_selectionId = SELECTION_ID_MY_CHOICE2;
    }
}

// ---------------------------------------------------------------------------

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
            bsl::string(original.d_myChoice2.object(), d_allocator_p);
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
                STREAM&  stream,
                int      version)
{
    if (stream) {
        switch (version) {  // Switch on the schema version (starting with 1).
          case 1: {
            short selectionId;
            stream.getInt16(selectionId);
            if (!stream) {
                return stream;                                      // RETURN
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
        return NOT_FOUND;                                           // RETURN
    }
    return SUCCESS;
}

inline
int MySequenceWithAnonymousChoiceChoice::makeSelection(
            const char *name,
            int         nameLength)
{
    enum { NOT_FOUND = -1 };

    const bdeat_SelectionInfo *selectionInfo =
           lookupSelectionInfo(name, nameLength);
    if (0 == selectionInfo) {
       return NOT_FOUND;                                            // RETURN
    }

    return makeSelection(selectionInfo->d_id);
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
                           SELECTION_INFO_ARRAY[SELECTION_INDEX_MY_CHOICE1]);
                                                                      // RETURN
      case MySequenceWithAnonymousChoiceChoice::SELECTION_ID_MY_CHOICE2:
        return manipulator(&d_myChoice2.object(),
                           SELECTION_INFO_ARRAY[SELECTION_INDEX_MY_CHOICE2]);
                                                                      // RETURN
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
                STREAM&  stream,
                int      version) const
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
                        SELECTION_INFO_ARRAY[SELECTION_INDEX_MY_CHOICE1]);
                                                                      // RETURN
      case SELECTION_ID_MY_CHOICE2:
        return accessor(d_myChoice2.object(),
                        SELECTION_INFO_ARRAY[SELECTION_INDEX_MY_CHOICE2]);
                                                                      // RETURN
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

}  // close namespace test;

// TRAITS
BDEAT_DECL_CHOICE_WITH_ALLOCATOR_TRAITS(
                test::MySequenceWithAnonymousChoiceChoice)

// FREE OPERATORS
inline
bool test::operator==(const test::MySequenceWithAnonymousChoiceChoice& lhs,
                      const test::MySequenceWithAnonymousChoiceChoice& rhs)
{
    if (lhs.selectionId() == rhs.selectionId()) {
        switch (rhs.selectionId()) {
          case test::
               MySequenceWithAnonymousChoiceChoice::SELECTION_ID_MY_CHOICE1:
            return lhs.myChoice1() == rhs.myChoice1();
                                                                    // RETURN
          case test::
               MySequenceWithAnonymousChoiceChoice::SELECTION_ID_MY_CHOICE2:
            return lhs.myChoice2() == rhs.myChoice2();
                                                                    // RETURN
          default:
            BSLS_ASSERT_SAFE(
              test::MySequenceWithAnonymousChoiceChoice::SELECTION_ID_UNDEFINED
              == rhs.selectionId());
            return true;                                            // RETURN
        }
    }
    else {
        return false;
   }
}

inline
bool test::operator!=(const test::MySequenceWithAnonymousChoiceChoice& lhs,
                      const test::MySequenceWithAnonymousChoiceChoice& rhs)
{
    return !(lhs == rhs);
}

inline
bsl::ostream& test::operator<<(
                    bsl::ostream&                                     stream,
                    const test::MySequenceWithAnonymousChoiceChoice&  rhs)
{
    return rhs.print(stream, 0, -1);
}

}  // close namespace BloombergLP;

#endif

// ---------------------------------------------------------------------------
// *End-of-File Block removed*
// ---------------------------------------------------------------------------

// test_mysequencewithanonymouschoicechoice.cpp  -*-C++-*-

#include <bsl_iostream.h>

#include <bdeu_print.h>
#include <bdeu_printmethods.h>
#include <bdeu_chartype.h>
#include <bdeat_formattingmode.h>

#include <bsl_string.h>

namespace BloombergLP {
namespace test {

                               // ---------
                               // CONSTANTS
                               // ---------

const char MySequenceWithAnonymousChoiceChoice::CLASS_NAME[]
                = "MySequenceWithAnonymousChoiceChoice";
    // the name of this class

const bdeat_SelectionInfo
                MySequenceWithAnonymousChoiceChoice::SELECTION_INFO_ARRAY[] = {
    {
        SELECTION_ID_MY_CHOICE1,
        "MyChoice1",                         // name
        sizeof("MyChoice1") - 1,             // name length
        "todo: provide annotation",    // annotation
        bdeat_FormattingMode::BDEAT_DEC // formatting mode
    },
    {
        SELECTION_ID_MY_CHOICE2,
        "MyChoice2",                         // name
        sizeof("MyChoice2") - 1,             // name length
        "todo: provide annotation",    // annotation
        bdeat_FormattingMode::BDEAT_TEXT // formatting mode
    }
};

                               // -------------
                               // CLASS METHODS
                               // -------------

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
             && bdeu_CharType::toUpper(name[7])=='E') {
                switch(bdeu_CharType::toUpper(name[8])) {
                    case '1': {
                        return
                            &SELECTION_INFO_ARRAY[SELECTION_INDEX_MY_CHOICE1];
                    } break;
                    case '2': {
                        return
                            &SELECTION_INFO_ARRAY[SELECTION_INDEX_MY_CHOICE2];
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

                                // --------
                                // CREATORS
                                // --------

                                // ------------
                                // MANIPULATORS
                                // ------------

                                // ---------
                                // ACCESSORS
                                // ---------

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

}  // close namespace test;
}  // close namespace BloombergLP;

// ---------------------------------------------------------------------------
// *End-of-File Block removed*
// ---------------------------------------------------------------------------

// test_mysequencewithanonymouschoice.h   -*-C++-*-
#ifndef INCLUDED_TEST_MYSEQUENCEWITHANONYMOUSCHOICE
#define INCLUDED_TEST_MYSEQUENCEWITHANONYMOUSCHOICE

//@PURPOSE:
//  todo: provide purpose
//
//@CLASSES: test::MySequenceWithAnonymousChoice
//
//@AUTHOR: Author Unknown
//
//@DESCRIPTION:
//  todo: provide annotation

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

#ifndef INCLUDED_BSL_IOSFWD
#include <bsl_iosfwd.h>
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

#ifndef INCLUDED_BSL_STRING
#include <bsl_string.h>
#endif

namespace BloombergLP {

class bslma_Allocator;

namespace test {

class MySequenceWithAnonymousChoice {

  private:
    // TODO: Provide annotations.
    int d_attribute1;
    MySequenceWithAnonymousChoiceChoice d_theChoice;
    bsl::string d_attribute2;

  public:
    // TYPES
    enum {
        NUM_ATTRIBUTES = 3 // the number of attributes in this class
    };

    enum {
        ATTRIBUTE_INDEX_ATTRIBUTE1 = 0,
            // index for "Attribute1" attribute
        ATTRIBUTE_INDEX_THE_CHOICE = 1,
            // index for "TheChoice" attribute
        ATTRIBUTE_INDEX_ATTRIBUTE2 = 2
            // index for "Attribute2" attribute
    };

    enum {
        ATTRIBUTE_ID_ATTRIBUTE1 = 0,
            // id for "Attribute1" attribute
        ATTRIBUTE_ID_THE_CHOICE = 1,
            // id for "TheChoice" attribute
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
        // specified 'basicAllocator' to supply memory.  If 'basicAllocator'
        // is 0, the currently installed default allocator is used.

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

    int& attribute1();
        // Return a reference to the modifiable "Attribute1" attribute of this
        // object.

    MySequenceWithAnonymousChoiceChoice& theChoice();
        // Return a reference to the modifiable "TheChoice" attribute of this
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
        // Return a reference to the non-modifiable "Attribute1"
        // attribute of this object.

    const MySequenceWithAnonymousChoiceChoice& theChoice() const;
        // Return a reference to the non-modifiable "TheChoice"
        // attribute of this object.

    const bsl::string& attribute2() const;
        // Return a reference to the non-modifiable "Attribute2"
        // attribute of this object.

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

// ============================================================================
//                      INLINE FUNCTION DEFINITIONS
// ============================================================================

// The following inlined functions are invoked from other inline functions.

inline
int MySequenceWithAnonymousChoice::maxSupportedBdexVersion()
{
    return 1;  // versions start at 1.
}

// ---------------------------------------------------------------------------

// CREATORS
inline
MySequenceWithAnonymousChoice::MySequenceWithAnonymousChoice(
                bslma_Allocator *basicAllocator)
: d_attribute1()
, d_theChoice(bslma_Default::allocator(basicAllocator))
, d_attribute2(bslma_Default::allocator(basicAllocator))
{
}

inline
MySequenceWithAnonymousChoice::MySequenceWithAnonymousChoice(
        const MySequenceWithAnonymousChoice& original,
        bslma_Allocator *basicAllocator)
: d_attribute1(original.d_attribute1)
, d_theChoice(original.d_theChoice, bslma_Default::allocator(basicAllocator))
, d_attribute2(original.d_attribute2, bslma_Default::allocator(basicAllocator))
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
        d_theChoice = rhs.d_theChoice;
        d_attribute2 = rhs.d_attribute2;
    }
    return *this;
}

template <class STREAM>
inline
STREAM& MySequenceWithAnonymousChoice::bdexStreamIn(
                STREAM&     stream,
                int         version)
{
    if (stream) {
        switch (version) {  // Switch on the schema version (starting with 1).
          case 1: {
            bdex_InStreamFunctions::streamIn(
                  stream, d_attribute1, 1);
            bdex_InStreamFunctions::streamIn(
                  stream, d_theChoice, 1);
            bdex_InStreamFunctions::streamIn(
                  stream, d_attribute2, 1);
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
    bdeat_ValueTypeFunctions::reset(&d_theChoice);
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
        return ret;                                                 // RETURN
    }

    ret = manipulator(&d_theChoice,
                      ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_THE_CHOICE]);
    if (ret) {
        return ret;                                                 // RETURN
    }

    ret = manipulator(&d_attribute2,
                      ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE2]);
    if (ret) {
        return ret;                                                 // RETURN
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
      case ATTRIBUTE_ID_THE_CHOICE: {
        return manipulator(&d_theChoice,
                           ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_THE_CHOICE]);
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
        return NOT_FOUND;                                           // RETURN
    }

    return manipulateAttribute(manipulator, attributeInfo->d_id);
}

inline
int& MySequenceWithAnonymousChoice::attribute1()
{
    return d_attribute1;
}

inline
MySequenceWithAnonymousChoiceChoice& MySequenceWithAnonymousChoice::theChoice()
{
    return d_theChoice;
}

inline
bsl::string& MySequenceWithAnonymousChoice::attribute2()
{
    return d_attribute2;
}

// ACCESSORS
template <class STREAM>
inline
STREAM& MySequenceWithAnonymousChoice::bdexStreamOut(STREAM&  stream,
                                                     int      version) const
{
    switch (version) {
      case 1: {
        bdex_OutStreamFunctions::streamOut(stream, d_attribute1, 1);
        bdex_OutStreamFunctions::streamOut(stream, d_theChoice, 1);
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
        return ret;                                                 // RETURN
    }

    ret = accessor(d_theChoice,
                   ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_THE_CHOICE]);
    if (ret) {
        return ret;                                                 // RETURN
    }

    ret = accessor(d_attribute2,
                   ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE2]);
    if (ret) {
        return ret;                                                 // RETURN
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
      case ATTRIBUTE_ID_THE_CHOICE: {
        return accessor(d_theChoice,
                        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_THE_CHOICE]);
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
int MySequenceWithAnonymousChoice::accessAttribute(ACCESSOR&   accessor,
                                 const char *name,
                                 int         nameLength) const
{
    enum { NOT_FOUND = -1 };

     const bdeat_AttributeInfo *attributeInfo =
           lookupAttributeInfo(name, nameLength);
     if (0 == attributeInfo) {
        return NOT_FOUND;                                           // RETURN
     }

     return accessAttribute(accessor, attributeInfo->d_id);
}

inline
const int& MySequenceWithAnonymousChoice::attribute1() const
{
    return d_attribute1;
}

inline
const MySequenceWithAnonymousChoiceChoice&
                               MySequenceWithAnonymousChoice::theChoice() const
{
    return d_theChoice;
}

inline
const bsl::string& MySequenceWithAnonymousChoice::attribute2() const
{
    return d_attribute2;
}

}  // close namespace test;

// TRAITS
BDEAT_DECL_SEQUENCE_WITH_ALLOCATOR_TRAITS(test::MySequenceWithAnonymousChoice)

// FREE OPERATORS
inline
bool test::operator==(const test::MySequenceWithAnonymousChoice& lhs,
                      const test::MySequenceWithAnonymousChoice& rhs)
{
    return  lhs.attribute1() == rhs.attribute1()
         && lhs.theChoice() == rhs.theChoice()
         && lhs.attribute2() == rhs.attribute2();
}

inline
bool test::operator!=(const test::MySequenceWithAnonymousChoice& lhs,
                      const test::MySequenceWithAnonymousChoice& rhs)
{
    return  lhs.attribute1() != rhs.attribute1()
         || lhs.theChoice() != rhs.theChoice()
         || lhs.attribute2() != rhs.attribute2();
}

inline
bsl::ostream& test::operator<<(
                   bsl::ostream&                               stream,
                   const test::MySequenceWithAnonymousChoice&  rhs)
{
    return rhs.print(stream, 0, -1);
}

}  // close namespace BloombergLP;

#endif

// ---------------------------------------------------------------------------
// *End-of-File Block removed*
// ---------------------------------------------------------------------------

// test_mysequencewithanonymouschoice.cpp  -*-C++-*-

#include <bsl_iostream.h>

#include <bdeu_print.h>
#include <bdeu_printmethods.h>
#include <bdeu_chartype.h>
#include <bdeat_formattingmode.h>

#include <bsl_string.h>

namespace BloombergLP {
namespace test {

                               // ---------
                               // CONSTANTS
                               // ---------

const char MySequenceWithAnonymousChoice::CLASS_NAME[]
                = "MySequenceWithAnonymousChoice";
    // the name of this class

const bdeat_AttributeInfo
                      MySequenceWithAnonymousChoice::ATTRIBUTE_INFO_ARRAY[] = {
    {
        ATTRIBUTE_ID_ATTRIBUTE1,
        "Attribute1",                     // name
        sizeof("Attribute1") - 1,         // name length
        "todo: provide annotation",// annotation
        bdeat_FormattingMode::BDEAT_DEC // formatting mode
    },
    {
        ATTRIBUTE_ID_THE_CHOICE,
        "theChoice",                     // name
        sizeof("theChoice") - 1,         // name length
        "todo: provide annotation",// annotation
        bdeat_FormattingMode::BDEAT_DEFAULT // formatting mode
      | bdeat_FormattingMode::BDEAT_UNTAGGED
    },
    {
        ATTRIBUTE_ID_ATTRIBUTE2,
        "Attribute2",                     // name
        sizeof("Attribute2") - 1,         // name length
        "todo: provide annotation",// annotation
        bdeat_FormattingMode::BDEAT_TEXT // formatting mode
    }
};

                               // -------------
                               // CLASS METHODS
                               // -------------

const bdeat_AttributeInfo *MySequenceWithAnonymousChoice::lookupAttributeInfo(
        const char *name,
        int         nameLength)
{
    switch(nameLength) {
        case 9: {
            if (bdeu_CharType::toUpper(name[0])=='T'
             && bdeu_CharType::toUpper(name[1])=='H'
             && bdeu_CharType::toUpper(name[2])=='E'
             && bdeu_CharType::toUpper(name[3])=='C'
             && bdeu_CharType::toUpper(name[4])=='H'
             && bdeu_CharType::toUpper(name[5])=='O'
             && bdeu_CharType::toUpper(name[6])=='I'
             && bdeu_CharType::toUpper(name[7])=='C'
             && bdeu_CharType::toUpper(name[8])=='E') {
                return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_THE_CHOICE];
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
             && bdeu_CharType::toUpper(name[8])=='E') {
                switch(bdeu_CharType::toUpper(name[9])) {
                    case '1': {
                        return
                            &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE1];
                    } break;
                    case '2': {
                        return
                            &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE2];
                    } break;
                }
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
      case ATTRIBUTE_ID_THE_CHOICE:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_THE_CHOICE];
      case ATTRIBUTE_ID_ATTRIBUTE2:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE2];
      default:
        return 0;
    }
}

                                // --------
                                // CREATORS
                                // --------

                                // ------------
                                // MANIPULATORS
                                // ------------

                                // ---------
                                // ACCESSORS
                                // ---------

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
        stream << "TheChoice = ";
        bdeu_PrintMethods::print(stream, d_theChoice,
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
        stream << "TheChoice = ";
        bdeu_PrintMethods::print(stream, d_theChoice,
                             -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "Attribute2 = ";
        bdeu_PrintMethods::print(stream, d_attribute2,
                             -levelPlus1, spacesPerLevel);

        stream << " ]";
    }

    return stream << bsl::flush;
}

}  // close namespace test;
}  // close namespace BloombergLP;

// ---------------------------------------------------------------------------
// *End-of-File Block removed*
// ---------------------------------------------------------------------------

// test_mysequencewithattributes.h   -*-C++-*-
#ifndef INCLUDED_TEST_MYSEQUENCEWITHATTRIBUTES
#define INCLUDED_TEST_MYSEQUENCEWITHATTRIBUTES

//@PURPOSE:
//  todo: provide purpose
//
//@CLASSES: test::MySequenceWithAttributes
//
//@AUTHOR: Author Unknown
//
//@DESCRIPTION:
//  todo: provide annotation

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

#ifndef INCLUDED_BSL_IOSFWD
#include <bsl_iosfwd.h>
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

namespace BloombergLP {

class bslma_Allocator;

namespace test {

class MySequenceWithAttributes {

  private:
    int d_attribute1; // todo: provide annotation
    bdeut_NullableValue<bsl::string> d_attribute2; // todo: provide annotation
    int d_element1; // todo: provide annotation
    bsl::string d_element2; // todo: provide annotation

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

    MySequenceWithAttributes(
                const MySequenceWithAttributes&  original,
                bslma_Allocator                 *basicAllocator = 0);
        // Create an object of type 'MySequenceWithAttributes' having the value
        // of the specified 'original' object.  Use the optionally specified
        // 'basicAllocator' to supply memory.  If 'basicAllocator' is 0,
        // the currently installed default allocator is used.

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

    int& element1();
        // Return a reference to the modifiable "Element1" attribute of this
        // object.

    bsl::string& element2();
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
        // Return a reference to the non-modifiable "Attribute1"
        // attribute of this object.

    const bdeut_NullableValue<bsl::string>& attribute2() const;
        // Return a reference to the non-modifiable "Attribute2"
        // attribute of this object.

    const int& element1() const;
        // Return a reference to the non-modifiable "Element1"
        // attribute of this object.

    const bsl::string& element2() const;
        // Return a reference to the non-modifiable "Element2"
        // attribute of this object.

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

// ============================================================================
//                      INLINE FUNCTION DEFINITIONS
// ============================================================================

// The following inlined functions are invoked from other inline functions.

inline
int MySequenceWithAttributes::maxSupportedBdexVersion()
{
    return 1;  // versions start at 1.
}

// ---------------------------------------------------------------------------

// CREATORS
inline
MySequenceWithAttributes::MySequenceWithAttributes(
                bslma_Allocator *basicAllocator)
: d_attribute1()
, d_attribute2(bslma_Default::allocator(basicAllocator))
, d_element1()
, d_element2(bslma_Default::allocator(basicAllocator))
{
}

inline
MySequenceWithAttributes::MySequenceWithAttributes(
        const MySequenceWithAttributes& original,
        bslma_Allocator *basicAllocator)
: d_attribute1(original.d_attribute1)
, d_attribute2(original.d_attribute2, bslma_Default::allocator(basicAllocator))
, d_element1(original.d_element1)
, d_element2(original.d_element2, bslma_Default::allocator(basicAllocator))
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
            bdex_InStreamFunctions::streamIn(
                  stream, d_attribute1, 1);
            bdex_InStreamFunctions::streamIn(
                  stream, d_attribute2, 1);
            bdex_InStreamFunctions::streamIn(
                  stream, d_element1, 1);
            bdex_InStreamFunctions::streamIn(
                  stream, d_element2, 1);
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
        return ret;                                                 // RETURN
    }

    ret = manipulator(&d_attribute2,
                      ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE2]);
    if (ret) {
        return ret;                                                 // RETURN
    }

    ret = manipulator(&d_element1,
                      ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT1]);
    if (ret) {
        return ret;                                                 // RETURN
    }

    ret = manipulator(&d_element2,
                      ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT2]);
    if (ret) {
        return ret;                                                 // RETURN
    }

    return ret;
}

template <class MANIPULATOR>
inline
int MySequenceWithAttributes::manipulateAttribute(MANIPULATOR&  manipulator,
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
int MySequenceWithAttributes::manipulateAttribute(MANIPULATOR&  manipulator,
                                     const char   *name,
                                     int           nameLength)
{
    enum { NOT_FOUND = -1 };

    const bdeat_AttributeInfo *attributeInfo =
           lookupAttributeInfo(name, nameLength);
    if (0 == attributeInfo) {
        return NOT_FOUND;                                           // RETURN
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
int& MySequenceWithAttributes::element1()
{
    return d_element1;
}

inline
bsl::string& MySequenceWithAttributes::element2()
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
        return ret;                                                 // RETURN
    }

    ret = accessor(d_attribute2,
                   ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE2]);
    if (ret) {
        return ret;                                                 // RETURN
    }

    ret = accessor(d_element1,
                   ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT1]);
    if (ret) {
        return ret;                                                 // RETURN
    }

    ret = accessor(d_element2,
                   ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT2]);
    if (ret) {
        return ret;                                                 // RETURN
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
int MySequenceWithAttributes::accessAttribute(ACCESSOR&   accessor,
                                 const char *name,
                                 int         nameLength) const
{
    enum { NOT_FOUND = -1 };

     const bdeat_AttributeInfo *attributeInfo =
           lookupAttributeInfo(name, nameLength);
     if (0 == attributeInfo) {
        return NOT_FOUND;                                           // RETURN
     }

     return accessAttribute(accessor, attributeInfo->d_id);
}

inline
const int& MySequenceWithAttributes::attribute1() const
{
    return d_attribute1;
}

inline
const
 bdeut_NullableValue<bsl::string>& MySequenceWithAttributes::attribute2() const
{
    return d_attribute2;
}

inline
const int& MySequenceWithAttributes::element1() const
{
    return d_element1;
}

inline
const bsl::string& MySequenceWithAttributes::element2() const
{
    return d_element2;
}

}  // close namespace test;

// TRAITS
BDEAT_DECL_SEQUENCE_WITH_ALLOCATOR_TRAITS(test::MySequenceWithAttributes)

// FREE OPERATORS
inline
bool test::operator==(const test::MySequenceWithAttributes& lhs,
                      const test::MySequenceWithAttributes& rhs)
{
    return  lhs.attribute1() == rhs.attribute1()
         && lhs.attribute2() == rhs.attribute2()
         && lhs.element1() == rhs.element1()
         && lhs.element2() == rhs.element2();
}

inline
bool test::operator!=(const test::MySequenceWithAttributes& lhs,
                      const test::MySequenceWithAttributes& rhs)
{
    return  lhs.attribute1() != rhs.attribute1()
         || lhs.attribute2() != rhs.attribute2()
         || lhs.element1() != rhs.element1()
         || lhs.element2() != rhs.element2();
}

inline
bsl::ostream& test::operator<<(bsl::ostream&                          stream,
                               const test::MySequenceWithAttributes&  rhs)
{
    return rhs.print(stream, 0, -1);
}

}  // close namespace BloombergLP;

#endif

// ---------------------------------------------------------------------------
// *End-of-File Block removed*
// ---------------------------------------------------------------------------

// test_mysequencewithattributes.cpp  -*-C++-*-

#include <bsl_iostream.h>

#include <bdeu_print.h>
#include <bdeu_printmethods.h>
#include <bdeu_chartype.h>
#include <bdeat_formattingmode.h>

#include <bdeut_nullablevalue.h>
#include <bsl_string.h>

namespace BloombergLP {
namespace test {

                               // ---------
                               // CONSTANTS
                               // ---------

const char MySequenceWithAttributes::CLASS_NAME[] = "MySequenceWithAttributes";
    // the name of this class

const bdeat_AttributeInfo MySequenceWithAttributes::ATTRIBUTE_INFO_ARRAY[] = {
    {
        ATTRIBUTE_ID_ATTRIBUTE1,
        "Attribute1",                     // name
        sizeof("Attribute1") - 1,         // name length
        "todo: provide annotation",// annotation
        bdeat_FormattingMode::BDEAT_DEC // formatting mode
      | bdeat_FormattingMode::BDEAT_ATTRIBUTE
    },
    {
        ATTRIBUTE_ID_ATTRIBUTE2,
        "Attribute2",                     // name
        sizeof("Attribute2") - 1,         // name length
        "todo: provide annotation",// annotation
        bdeat_FormattingMode::BDEAT_TEXT // formatting mode
      | bdeat_FormattingMode::BDEAT_ATTRIBUTE
    },
    {
        ATTRIBUTE_ID_ELEMENT1,
        "Element1",                     // name
        sizeof("Element1") - 1,         // name length
        "todo: provide annotation",// annotation
        bdeat_FormattingMode::BDEAT_DEC // formatting mode
    },
    {
        ATTRIBUTE_ID_ELEMENT2,
        "Element2",                     // name
        sizeof("Element2") - 1,         // name length
        "todo: provide annotation",// annotation
        bdeat_FormattingMode::BDEAT_TEXT // formatting mode
    }
};

                               // -------------
                               // CLASS METHODS
                               // -------------

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
             && bdeu_CharType::toUpper(name[6])=='T') {
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
             && bdeu_CharType::toUpper(name[8])=='E') {
                switch(bdeu_CharType::toUpper(name[9])) {
                    case '1': {
                        return
                            &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE1];
                    } break;
                    case '2': {
                        return
                            &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE2];
                    } break;
                }
            }
        } break;
    }
    return 0;
}

const bdeat_AttributeInfo *
MySequenceWithAttributes::lookupAttributeInfo(int id)
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

                                // --------
                                // CREATORS
                                // --------

                                // ------------
                                // MANIPULATORS
                                // ------------

                                // ---------
                                // ACCESSORS
                                // ---------

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

}  // close namespace test;
}  // close namespace BloombergLP;

// ---------------------------------------------------------------------------
// *End-of-File Block removed*
// ---------------------------------------------------------------------------

// test_mysimplecontent.h   -*-C++-*-
#ifndef INCLUDED_TEST_MYSIMPLECONTENT
#define INCLUDED_TEST_MYSIMPLECONTENT

//@PURPOSE:
//  todo: provide purpose
//
//@CLASSES: test::MySimpleContent
//
//@AUTHOR: Author Unknown
//
//@DESCRIPTION:
//  todo: provide annotation

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

#ifndef INCLUDED_BSL_IOSFWD
#include <bsl_iosfwd.h>
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

namespace BloombergLP {

class bslma_Allocator;

namespace test {

class MySimpleContent {

  private:
    bdeut_NullableValue<bool> d_attribute1; // todo: provide annotation
    bdeut_NullableValue<bsl::string> d_attribute2; // todo: provide annotation
    bsl::string d_theContent; // todo: provide annotation

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
        // Use the optionally specified 'basicAllocator' to supply memory.
        // If 'basicAllocator' is 0, the currently installed default allocator
        // is used.

    MySimpleContent(const MySimpleContent&  original,
                    bslma_Allocator        *basicAllocator = 0);
        // Create an object of type 'MySimpleContent' having the value
        // of the specified 'original' object.  Use the optionally specified
        // 'basicAllocator' to supply memory.  If 'basicAllocator' is 0,
        // the currently installed default allocator is used.

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
        // Return a reference to the non-modifiable "Attribute1"
        // attribute of this object.

    const bdeut_NullableValue<bsl::string>& attribute2() const;
        // Return a reference to the non-modifiable "Attribute2"
        // attribute of this object.

    const bsl::string& theContent() const;
        // Return a reference to the non-modifiable "TheContent"
        // attribute of this object.

};

// FREE OPERATORS
inline
bool operator==(const MySimpleContent& lhs,
                const MySimpleContent& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects have
    // the same value, and 'false' otherwise.  Two attribute objects have the
    // same value if each respective attribute has the same value.

inline
bool operator!=(const MySimpleContent& lhs,
                const MySimpleContent& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects do not
    // have the same value, and 'false' otherwise.  Two attribute objects do
    // not have the same value if one or more respective attributes differ in
    // values.

inline
bsl::ostream& operator<<(bsl::ostream& stream, const MySimpleContent& rhs);
    // Format the specified 'rhs' to the specified output 'stream' and
    // return a reference to the modifiable 'stream'.

// ============================================================================
//                      INLINE FUNCTION DEFINITIONS
// ============================================================================

// The following inlined functions are invoked from other inline functions.

inline
int MySimpleContent::maxSupportedBdexVersion()
{
    return 1;  // versions start at 1.
}

// ---------------------------------------------------------------------------

// CREATORS
inline
MySimpleContent::MySimpleContent(bslma_Allocator *basicAllocator)
: d_attribute1()
, d_attribute2(bslma_Default::allocator(basicAllocator))
, d_theContent(bslma_Default::allocator(basicAllocator))
{
}

inline
MySimpleContent::MySimpleContent(
        const MySimpleContent& original,
        bslma_Allocator *basicAllocator)
: d_attribute1(original.d_attribute1)
, d_attribute2(original.d_attribute2, bslma_Default::allocator(basicAllocator))
, d_theContent(original.d_theContent, bslma_Default::allocator(basicAllocator))
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
            bdex_InStreamFunctions::streamIn(
                  stream, d_attribute1, 1);
            bdex_InStreamFunctions::streamIn(
                  stream, d_attribute2, 1);
            bdex_InStreamFunctions::streamIn(
                  stream, d_theContent, 1);
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
        return ret;                                                 // RETURN
    }

    ret = manipulator(&d_attribute2,
                      ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE2]);
    if (ret) {
        return ret;                                                 // RETURN
    }

    ret = manipulator(&d_theContent,
                      ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_THE_CONTENT]);
    if (ret) {
        return ret;                                                 // RETURN
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
int MySimpleContent::manipulateAttribute(MANIPULATOR&  manipulator,
                                     const char   *name,
                                     int           nameLength)
{
    enum { NOT_FOUND = -1 };

    const bdeat_AttributeInfo *attributeInfo =
           lookupAttributeInfo(name, nameLength);
    if (0 == attributeInfo) {
        return NOT_FOUND;                                           // RETURN
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
        return ret;                                                 // RETURN
    }

    ret = accessor(d_attribute2,
                   ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE2]);
    if (ret) {
        return ret;                                                 // RETURN
    }

    ret = accessor(d_theContent,
                   ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_THE_CONTENT]);
    if (ret) {
        return ret;                                                 // RETURN
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
int MySimpleContent::accessAttribute(ACCESSOR&   accessor,
                                 const char *name,
                                 int         nameLength) const
{
    enum { NOT_FOUND = -1 };

     const bdeat_AttributeInfo *attributeInfo =
           lookupAttributeInfo(name, nameLength);
     if (0 == attributeInfo) {
        return NOT_FOUND;                                           // RETURN
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

}  // close namespace test;

// TRAITS
BDEAT_DECL_SEQUENCE_WITH_ALLOCATOR_TRAITS(test::MySimpleContent)

// FREE OPERATORS
inline
bool test::operator==(const test::MySimpleContent& lhs,
                      const test::MySimpleContent& rhs)
{
    return  lhs.attribute1() == rhs.attribute1()
         && lhs.attribute2() == rhs.attribute2()
         && lhs.theContent() == rhs.theContent();
}

inline
bool test::operator!=(const test::MySimpleContent& lhs,
                      const test::MySimpleContent& rhs)
{
    return  lhs.attribute1() != rhs.attribute1()
         || lhs.attribute2() != rhs.attribute2()
         || lhs.theContent() != rhs.theContent();
}

inline
bsl::ostream& test::operator<<(bsl::ostream&                 stream,
                               const test::MySimpleContent&  rhs)
{
    return rhs.print(stream, 0, -1);
}

}  // close namespace BloombergLP;

#endif

// ---------------------------------------------------------------------------
// *End-of-File Block removed*
// ---------------------------------------------------------------------------

// test_mysimplecontent.cpp  -*-C++-*-

#include <bsl_iostream.h>

#include <bdeu_print.h>
#include <bdeu_printmethods.h>
#include <bdeu_chartype.h>
#include <bdeat_formattingmode.h>

#include <bdeut_nullablevalue.h>
#include <bsl_string.h>

namespace BloombergLP {
namespace test {

                               // ---------
                               // CONSTANTS
                               // ---------

const char MySimpleContent::CLASS_NAME[] = "MySimpleContent";
    // the name of this class

const bdeat_AttributeInfo MySimpleContent::ATTRIBUTE_INFO_ARRAY[] = {
    {
        ATTRIBUTE_ID_ATTRIBUTE1,
        "Attribute1",                     // name
        sizeof("Attribute1") - 1,         // name length
        "todo: provide annotation",// annotation
        bdeat_FormattingMode::BDEAT_TEXT // formatting mode
      | bdeat_FormattingMode::BDEAT_ATTRIBUTE
    },
    {
        ATTRIBUTE_ID_ATTRIBUTE2,
        "Attribute2",                     // name
        sizeof("Attribute2") - 1,         // name length
        "todo: provide annotation",// annotation
        bdeat_FormattingMode::BDEAT_TEXT // formatting mode
      | bdeat_FormattingMode::BDEAT_ATTRIBUTE
    },
    {
        ATTRIBUTE_ID_THE_CONTENT,
        "TheContent",                     // name
        sizeof("TheContent") - 1,         // name length
        "todo: provide annotation",// annotation
        bdeat_FormattingMode::BDEAT_TEXT // formatting mode
      | bdeat_FormattingMode::BDEAT_SIMPLE_CONTENT
    }
};

                               // -------------
                               // CLASS METHODS
                               // -------------

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
                     && bdeu_CharType::toUpper(name[8])=='E') {
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
                     && bdeu_CharType::toUpper(name[9])=='T') {
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

                                // --------
                                // CREATORS
                                // --------

                                // ------------
                                // MANIPULATORS
                                // ------------

                                // ---------
                                // ACCESSORS
                                // ---------

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

}  // close namespace test;
}  // close namespace BloombergLP;

// ---------------------------------------------------------------------------
// *End-of-File Block removed*
// ---------------------------------------------------------------------------

// test_mysimpleintcontent.h   -*-C++-*-
#ifndef INCLUDED_TEST_MYSIMPLEINTCONTENT
#define INCLUDED_TEST_MYSIMPLEINTCONTENT

//@PURPOSE:
//  todo: provide purpose
//
//@CLASSES: test::MySimpleIntContent
//
//@AUTHOR: Author Unknown
//
//@DESCRIPTION:
//  todo: provide annotation

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

#ifndef INCLUDED_BSL_IOSFWD
#include <bsl_iosfwd.h>
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

namespace BloombergLP {

class bslma_Allocator;

namespace test {

class MySimpleIntContent {

  private:
    bdeut_NullableValue<bool> d_attribute1; // todo: provide annotation
    bdeut_NullableValue<bsl::string> d_attribute2; // todo: provide annotation
    int d_theContent; // todo: provide annotation

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

    MySimpleIntContent(const MySimpleIntContent&  original,
                       bslma_Allocator           *basicAllocator = 0);
        // Create an object of type 'MySimpleIntContent' having the value
        // of the specified 'original' object.  Use the optionally specified
        // 'basicAllocator' to supply memory.  If 'basicAllocator' is 0,
        // the currently installed default allocator is used.

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
        // Return a reference to the non-modifiable "Attribute1"
        // attribute of this object.

    const bdeut_NullableValue<bsl::string>& attribute2() const;
        // Return a reference to the non-modifiable "Attribute2"
        // attribute of this object.

    const int& theContent() const;
        // Return a reference to the non-modifiable "TheContent"
        // attribute of this object.

};

// FREE OPERATORS
inline
bool operator==(const MySimpleIntContent& lhs,
                const MySimpleIntContent& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects have
    // the same value, and 'false' otherwise.  Two attribute objects have the
    // same value if each respective attribute has the same value.

inline
bool operator!=(const MySimpleIntContent& lhs,
                const MySimpleIntContent& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects do not
    // have the same value, and 'false' otherwise.  Two attribute objects do
    // not have the same value if one or more respective attributes differ in
    // values.

inline
bsl::ostream& operator<<(bsl::ostream&              stream,
                         const MySimpleIntContent&  rhs);
    // Format the specified 'rhs' to the specified output 'stream' and
    // return a reference to the modifiable 'stream'.

// ============================================================================
//                      INLINE FUNCTION DEFINITIONS
// ============================================================================

// The following inlined functions are invoked from other inline functions.

inline
int MySimpleIntContent::maxSupportedBdexVersion()
{
    return 1;  // versions start at 1.
}

// ---------------------------------------------------------------------------

// CREATORS
inline
MySimpleIntContent::MySimpleIntContent(bslma_Allocator *basicAllocator)
: d_attribute1()
, d_attribute2(bslma_Default::allocator(basicAllocator))
, d_theContent()
{
}

inline
MySimpleIntContent::MySimpleIntContent(
        const MySimpleIntContent& original,
        bslma_Allocator *basicAllocator)
: d_attribute1(original.d_attribute1)
, d_attribute2(original.d_attribute2, bslma_Default::allocator(basicAllocator))
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
            bdex_InStreamFunctions::streamIn(
                  stream, d_attribute1, 1);
            bdex_InStreamFunctions::streamIn(
                  stream, d_attribute2, 1);
            bdex_InStreamFunctions::streamIn(
                  stream, d_theContent, 1);
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
        return ret;                                                 // RETURN
    }

    ret = manipulator(&d_attribute2,
                      ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE2]);
    if (ret) {
        return ret;                                                 // RETURN
    }

    ret = manipulator(&d_theContent,
                      ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_THE_CONTENT]);
    if (ret) {
        return ret;                                                 // RETURN
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
int MySimpleIntContent::manipulateAttribute(MANIPULATOR&  manipulator,
                                     const char   *name,
                                     int           nameLength)
{
    enum { NOT_FOUND = -1 };

    const bdeat_AttributeInfo *attributeInfo =
           lookupAttributeInfo(name, nameLength);
    if (0 == attributeInfo) {
        return NOT_FOUND;                                           // RETURN
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
        return ret;                                                 // RETURN
    }

    ret = accessor(d_attribute2,
                   ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE2]);
    if (ret) {
        return ret;                                                 // RETURN
    }

    ret = accessor(d_theContent,
                   ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_THE_CONTENT]);
    if (ret) {
        return ret;                                                 // RETURN
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
int MySimpleIntContent::accessAttribute(ACCESSOR&   accessor,
                                 const char *name,
                                 int         nameLength) const
{
    enum { NOT_FOUND = -1 };

     const bdeat_AttributeInfo *attributeInfo =
           lookupAttributeInfo(name, nameLength);
     if (0 == attributeInfo) {
        return NOT_FOUND;                                           // RETURN
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

}  // close namespace test;

// TRAITS
BDEAT_DECL_SEQUENCE_WITH_ALLOCATOR_TRAITS(test::MySimpleIntContent)

// FREE OPERATORS
inline
bool test::operator==(const test::MySimpleIntContent& lhs,
                      const test::MySimpleIntContent& rhs)
{
    return  lhs.attribute1() == rhs.attribute1()
         && lhs.attribute2() == rhs.attribute2()
         && lhs.theContent() == rhs.theContent();
}

inline
bool test::operator!=(const test::MySimpleIntContent& lhs,
                      const test::MySimpleIntContent& rhs)
{
    return  lhs.attribute1() != rhs.attribute1()
         || lhs.attribute2() != rhs.attribute2()
         || lhs.theContent() != rhs.theContent();
}

inline
bsl::ostream& test::operator<<(bsl::ostream&                    stream,
                               const test::MySimpleIntContent&  rhs)
{
    return rhs.print(stream, 0, -1);
}

}  // close namespace BloombergLP;

#endif

// ---------------------------------------------------------------------------
// *End-of-File Block removed*
// ---------------------------------------------------------------------------

// test_mysimpleintcontent.cpp  -*-C++-*-

#include <bsl_iostream.h>

#include <bdeu_print.h>
#include <bdeu_printmethods.h>
#include <bdeu_chartype.h>
#include <bdeat_formattingmode.h>

#include <bdeut_nullablevalue.h>
#include <bsl_string.h>

namespace BloombergLP {
namespace test {

                               // ---------
                               // CONSTANTS
                               // ---------

const char MySimpleIntContent::CLASS_NAME[] = "MySimpleIntContent";
    // the name of this class

const bdeat_AttributeInfo MySimpleIntContent::ATTRIBUTE_INFO_ARRAY[] = {
    {
        ATTRIBUTE_ID_ATTRIBUTE1,
        "Attribute1",                     // name
        sizeof("Attribute1") - 1,         // name length
        "todo: provide annotation",// annotation
        bdeat_FormattingMode::BDEAT_TEXT // formatting mode
      | bdeat_FormattingMode::BDEAT_ATTRIBUTE
    },
    {
        ATTRIBUTE_ID_ATTRIBUTE2,
        "Attribute2",                     // name
        sizeof("Attribute2") - 1,         // name length
        "todo: provide annotation",// annotation
        bdeat_FormattingMode::BDEAT_TEXT // formatting mode
      | bdeat_FormattingMode::BDEAT_ATTRIBUTE
    },
    {
        ATTRIBUTE_ID_THE_CONTENT,
        "TheContent",                     // name
        sizeof("TheContent") - 1,         // name length
        "todo: provide annotation",// annotation
        bdeat_FormattingMode::BDEAT_DEC // formatting mode
      | bdeat_FormattingMode::BDEAT_SIMPLE_CONTENT
    }
};

                               // -------------
                               // CLASS METHODS
                               // -------------

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
                     && bdeu_CharType::toUpper(name[8])=='E') {
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
                     && bdeu_CharType::toUpper(name[9])=='T') {
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

                                // --------
                                // CREATORS
                                // --------

                                // ------------
                                // MANIPULATORS
                                // ------------

                                // ---------
                                // ACCESSORS
                                // ---------

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

}  // close namespace test;
}  // close namespace BloombergLP;

// ---------------------------------------------------------------------------
// *End-of-File Block removed*
// ---------------------------------------------------------------------------

// test_mysequencewithnillables.h   -*-C++-*-
#ifndef INCLUDED_TEST_MYSEQUENCEWITHNILLABLES
#define INCLUDED_TEST_MYSEQUENCEWITHNILLABLES

//@PURPOSE:
//  todo: provide purpose
//
//@CLASSES: test::MySequenceWithNillables
//
//@AUTHOR: Author Unknown
//
//@DESCRIPTION:
//  todo: provide annotation

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

#ifndef INCLUDED_BSL_IOSFWD
#include <bsl_iosfwd.h>
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

namespace BloombergLP {

class bslma_Allocator;

namespace test {

class MySequenceWithNillables {

  private:
    bdeut_NullableValue<int> d_attribute1; // todo: provide annotation
    bdeut_NullableValue<bsl::string> d_attribute2; // todo: provide annotation
    bdeut_NullableValue<MySequence> d_attribute3; // todo: provide annotation

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

    MySequenceWithNillables(
            const MySequenceWithNillables&   original,
            bslma_Allocator                 *basicAllocator = 0);
        // Create an object of type 'MySequenceWithNillables' having the value
        // of the specified 'original' object.  Use the optionally specified
        // 'basicAllocator' to supply memory.  If 'basicAllocator' is 0,
        // the currently installed default allocator is used.

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
        // Return a reference to the non-modifiable "Attribute1"
        // attribute of this object.

    const bdeut_NullableValue<bsl::string>& attribute2() const;
        // Return a reference to the non-modifiable "Attribute2"
        // attribute of this object.

    const bdeut_NullableValue<MySequence>& attribute3() const;
        // Return a reference to the non-modifiable "Attribute3"
        // attribute of this object.

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

// ============================================================================
//                      INLINE FUNCTION DEFINITIONS
// ============================================================================

// The following inlined functions are invoked from other inline functions.

inline
int MySequenceWithNillables::maxSupportedBdexVersion()
{
    return 1;  // versions start at 1.
}

// ---------------------------------------------------------------------------

// CREATORS
inline
MySequenceWithNillables::MySequenceWithNillables(
                bslma_Allocator *basicAllocator)
: d_attribute1()
, d_attribute2(bslma_Default::allocator(basicAllocator))
, d_attribute3(bslma_Default::allocator(basicAllocator))
{
}

inline
MySequenceWithNillables::MySequenceWithNillables(
        const MySequenceWithNillables& original,
        bslma_Allocator *basicAllocator)
: d_attribute1(original.d_attribute1)
, d_attribute2(original.d_attribute2, bslma_Default::allocator(basicAllocator))
, d_attribute3(original.d_attribute3, bslma_Default::allocator(basicAllocator))
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
            bdex_InStreamFunctions::streamIn(
                  stream, d_attribute1, 1);
            bdex_InStreamFunctions::streamIn(
                  stream, d_attribute2, 1);
            bdex_InStreamFunctions::streamIn(
                  stream, d_attribute3, 1);
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
        return ret;                                                 // RETURN
    }

    ret = manipulator(&d_attribute2,
                      ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE2]);
    if (ret) {
        return ret;                                                 // RETURN
    }

    ret = manipulator(&d_attribute3,
                      ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE3]);
    if (ret) {
        return ret;                                                 // RETURN
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
int MySequenceWithNillables::manipulateAttribute(MANIPULATOR&  manipulator,
                                     const char   *name,
                                     int           nameLength)
{
    enum { NOT_FOUND = -1 };

    const bdeat_AttributeInfo *attributeInfo =
           lookupAttributeInfo(name, nameLength);
    if (0 == attributeInfo) {
        return NOT_FOUND;                                           // RETURN
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
        return ret;                                                 // RETURN
    }

    ret = accessor(d_attribute2,
                   ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE2]);
    if (ret) {
        return ret;                                                 // RETURN
    }

    ret = accessor(d_attribute3,
                   ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE3]);
    if (ret) {
        return ret;                                                 // RETURN
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
int MySequenceWithNillables::accessAttribute(ACCESSOR&   accessor,
                                 const char *name,
                                 int         nameLength) const
{
    enum { NOT_FOUND = -1 };

     const bdeat_AttributeInfo *attributeInfo =
           lookupAttributeInfo(name, nameLength);
     if (0 == attributeInfo) {
        return NOT_FOUND;                                           // RETURN
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

}  // close namespace test;

// TRAITS
BDEAT_DECL_SEQUENCE_WITH_ALLOCATOR_TRAITS(test::MySequenceWithNillables)

// FREE OPERATORS
inline
bool test::operator==(const test::MySequenceWithNillables& lhs,
                      const test::MySequenceWithNillables& rhs)
{
    return  lhs.attribute1() == rhs.attribute1()
         && lhs.attribute2() == rhs.attribute2()
         && lhs.attribute3() == rhs.attribute3();
}

inline
bool test::operator!=(const test::MySequenceWithNillables& lhs,
                      const test::MySequenceWithNillables& rhs)
{
    return  lhs.attribute1() != rhs.attribute1()
         || lhs.attribute2() != rhs.attribute2()
         || lhs.attribute3() != rhs.attribute3();
}

inline
bsl::ostream& test::operator<<(bsl::ostream&                         stream,
                               const test::MySequenceWithNillables&  rhs)
{
    return rhs.print(stream, 0, -1);
}

}  // close namespace BloombergLP;

#endif

// ---------------------------------------------------------------------------
// *End-of-File Block removed*
// ---------------------------------------------------------------------------

// test_mysequencewithnillables.cpp  -*-C++-*-

#include <bsl_iostream.h>

#include <bdeu_print.h>
#include <bdeu_printmethods.h>
#include <bdeu_chartype.h>
#include <bdeat_formattingmode.h>

#include <bdeut_nullablevalue.h>
#include <bsl_string.h>

namespace BloombergLP {
namespace test {

                               // ---------
                               // CONSTANTS
                               // ---------

const char MySequenceWithNillables::CLASS_NAME[] = "MySequenceWithNillables";
    // the name of this class

const bdeat_AttributeInfo MySequenceWithNillables::ATTRIBUTE_INFO_ARRAY[] = {
    {
        ATTRIBUTE_ID_ATTRIBUTE1,
        "Attribute1",                     // name
        sizeof("Attribute1") - 1,         // name length
        "todo: provide annotation",// annotation
        bdeat_FormattingMode::BDEAT_DEC // formatting mode
      | bdeat_FormattingMode::BDEAT_NILLABLE
    },
    {
        ATTRIBUTE_ID_ATTRIBUTE2,
        "Attribute2",                     // name
        sizeof("Attribute2") - 1,         // name length
        "todo: provide annotation",// annotation
        bdeat_FormattingMode::BDEAT_TEXT // formatting mode
      | bdeat_FormattingMode::BDEAT_NILLABLE
    },
    {
        ATTRIBUTE_ID_ATTRIBUTE3,
        "Attribute3",                     // name
        sizeof("Attribute3") - 1,         // name length
        "todo: provide annotation",// annotation
        bdeat_FormattingMode::BDEAT_DEFAULT // formatting mode
      | bdeat_FormattingMode::BDEAT_NILLABLE
    }
};

                               // -------------
                               // CLASS METHODS
                               // -------------

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
             && bdeu_CharType::toUpper(name[8])=='E') {
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

                                // --------
                                // CREATORS
                                // --------

                                // ------------
                                // MANIPULATORS
                                // ------------

                                // ---------
                                // ACCESSORS
                                // ---------

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

// ---------------------------------------------------------------------------
// *End-of-File Block removed*
// ---------------------------------------------------------------------------

// test_address.h   -*-C++-*-
#ifndef INCLUDED_TEST_ADDRESS
#define INCLUDED_TEST_ADDRESS

//@PURPOSE:
//  todo: provide purpose
//
//@CLASSES: Address
//
//@AUTHOR: Author Unknown
//
//@DESCRIPTION:
//  todo: provide annotation

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

#ifndef INCLUDED_BSL_IOSFWD
#include <bsl_iosfwd.h>
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

#ifndef INCLUDED_BSL_STRING
#include <bsl_string.h>
#endif

namespace BloombergLP {

class bslma_Allocator;

namespace test {

class Address {

  private:
    bsl::string d_street; // todo: provide annotation
    bsl::string d_city; // todo: provide annotation
    bsl::string d_state; // todo: provide annotation

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
        // Create an object of type 'Address' having the default value.
        // Use the optionally specified 'basicAllocator' to supply memory.
        // If 'basicAllocator' is 0, the currently installed default allocator
        // is used.

    Address(const Address& original, bslma_Allocator *basicAllocator = 0);
        // Create an object of type 'Address' having the value
        // of the specified 'original' object.  Use the optionally specified
        // 'basicAllocator' to supply memory.  If 'basicAllocator' is 0,
        // the currently installed default allocator is used.

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
        // Return a reference to the non-modifiable "Street"
        // attribute of this object.

    const bsl::string& city() const;
        // Return a reference to the non-modifiable "City"
        // attribute of this object.

    const bsl::string& state() const;
        // Return a reference to the non-modifiable "State"
        // attribute of this object.

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

// ============================================================================
//                      INLINE FUNCTION DEFINITIONS
// ============================================================================

// The following inlined functions are invoked from other inline functions.

inline
int Address::maxSupportedBdexVersion()
{
    return 1;  // versions start at 1.
}

// ---------------------------------------------------------------------------

// CREATORS
inline
Address::Address(bslma_Allocator *basicAllocator)
: d_street(bslma_Default::allocator(basicAllocator))
, d_city(bslma_Default::allocator(basicAllocator))
, d_state(bslma_Default::allocator(basicAllocator))
{
}

inline
Address::Address(
    const Address& original,
    bslma_Allocator *basicAllocator)
: d_street(original.d_street, bslma_Default::allocator(basicAllocator))
, d_city(original.d_city, bslma_Default::allocator(basicAllocator))
, d_state(original.d_state, bslma_Default::allocator(basicAllocator))
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
            bdex_InStreamFunctions::streamIn(
                  stream, d_street, 1);
            bdex_InStreamFunctions::streamIn(
                  stream, d_city, 1);
            bdex_InStreamFunctions::streamIn(
                  stream, d_state, 1);
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

    ret = manipulator(&d_street,
                      ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_STREET]);
    if (ret) {
        return ret;                                                 // RETURN
    }

    ret = manipulator(&d_city,
                      ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CITY]);
    if (ret) {
        return ret;                                                 // RETURN
    }

    ret = manipulator(&d_state,
                      ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_STATE]);
    if (ret) {
        return ret;                                                 // RETURN
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
int Address::manipulateAttribute(MANIPULATOR&  manipulator,
                                     const char   *name,
                                     int           nameLength)
{
    enum { NOT_FOUND = -1 };

    const bdeat_AttributeInfo *attributeInfo =
           lookupAttributeInfo(name, nameLength);
    if (0 == attributeInfo) {
        return NOT_FOUND;                                           // RETURN
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
        return ret;                                                 // RETURN
    }

    ret = accessor(d_city, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CITY]);
    if (ret) {
        return ret;                                                 // RETURN
    }

    ret = accessor(d_state, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_STATE]);
    if (ret) {
        return ret;                                                 // RETURN
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
int Address::accessAttribute(ACCESSOR&   accessor,
                                 const char *name,
                                 int         nameLength) const
{
    enum { NOT_FOUND = -1 };

     const bdeat_AttributeInfo *attributeInfo =
           lookupAttributeInfo(name, nameLength);
     if (0 == attributeInfo) {
        return NOT_FOUND;                                           // RETURN
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

}  // close namespace test;

// TRAITS
BDEAT_DECL_SEQUENCE_WITH_ALLOCATOR_TRAITS(test::Address)

// FREE OPERATORS
inline
bool test::operator==(const test::Address& lhs, const test::Address& rhs)
{
    return  lhs.street() == rhs.street()
         && lhs.city() == rhs.city()
         && lhs.state() == rhs.state();
}

inline
bool test::operator!=(const test::Address& lhs, const test::Address& rhs)
{
    return  lhs.street() != rhs.street()
         || lhs.city() != rhs.city()
         || lhs.state() != rhs.state();
}

inline
bsl::ostream& test::operator<<(bsl::ostream& stream, const test::Address& rhs)
{
    return rhs.print(stream, 0, -1);
}

}  // close namespace BloombergLP;

#endif

// ---------------------------------------------------------------------------
// *End-of-File Block removed*
// ---------------------------------------------------------------------------

// test_address.cpp  -*-C++-*-

#include <bsl_iostream.h>

#include <bdeu_print.h>
#include <bdeu_printmethods.h>
#include <bdeu_chartype.h>
#include <bdeat_formattingmode.h>

#include <bsl_string.h>

namespace BloombergLP {
namespace test {

                               // ---------
                               // CONSTANTS
                               // ---------

const char Address::CLASS_NAME[] = "Address";
    // the name of this class

const bdeat_AttributeInfo Address::ATTRIBUTE_INFO_ARRAY[] = {
    {
        ATTRIBUTE_ID_STREET,
        "street",                     // name
        sizeof("street") - 1,         // name length
        "todo: provide annotation",// annotation
        bdeat_FormattingMode::BDEAT_TEXT // formatting mode
    },
    {
        ATTRIBUTE_ID_CITY,
        "city",                     // name
        sizeof("city") - 1,         // name length
        "todo: provide annotation",// annotation
        bdeat_FormattingMode::BDEAT_TEXT // formatting mode
    },
    {
        ATTRIBUTE_ID_STATE,
        "state",                     // name
        sizeof("state") - 1,         // name length
        "todo: provide annotation",// annotation
        bdeat_FormattingMode::BDEAT_TEXT // formatting mode
    }
};

                               // -------------
                               // CLASS METHODS
                               // -------------

const bdeat_AttributeInfo *Address::lookupAttributeInfo(
        const char *name,
        int         nameLength)
{
    switch(nameLength) {
        case 4: {
            if (bdeu_CharType::toUpper(name[0])=='C'
             && bdeu_CharType::toUpper(name[1])=='I'
             && bdeu_CharType::toUpper(name[2])=='T'
             && bdeu_CharType::toUpper(name[3])=='Y') {
                return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CITY];
            }
        } break;
        case 5: {
            if (bdeu_CharType::toUpper(name[0])=='S'
             && bdeu_CharType::toUpper(name[1])=='T'
             && bdeu_CharType::toUpper(name[2])=='A'
             && bdeu_CharType::toUpper(name[3])=='T'
             && bdeu_CharType::toUpper(name[4])=='E') {
                return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_STATE];
            }
        } break;
        case 6: {
            if (bdeu_CharType::toUpper(name[0])=='S'
             && bdeu_CharType::toUpper(name[1])=='T'
             && bdeu_CharType::toUpper(name[2])=='R'
             && bdeu_CharType::toUpper(name[3])=='E'
             && bdeu_CharType::toUpper(name[4])=='E'
             && bdeu_CharType::toUpper(name[5])=='T') {
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

                                // --------
                                // CREATORS
                                // --------

                                // ------------
                                // MANIPULATORS
                                // ------------

                                // ---------
                                // ACCESSORS
                                // ---------

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

}  // close namespace test;
}  // close namespace BloombergLP;

// ---------------------------------------------------------------------------
// *End-of-File Block removed*
// ---------------------------------------------------------------------------

// test_employee.h   -*-C++-*-
#ifndef INCLUDED_TEST_EMPLOYEE
#define INCLUDED_TEST_EMPLOYEE

//@PURPOSE:
//  todo: provide purpose
//
//@CLASSES: Employee
//
//@AUTHOR: Author Unknown
//
//@DESCRIPTION:
//  todo: provide annotation

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

#ifndef INCLUDED_BSL_IOSFWD
#include <bsl_iosfwd.h>
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

#ifndef INCLUDED_BSL_STRING
#include <bsl_string.h>
#endif

namespace BloombergLP {

class bslma_Allocator;

namespace test {

class Employee {

  private:
    bsl::string d_name; // todo: provide annotation
    Address d_homeAddress; // todo: provide annotation
    int d_age; // todo: provide annotation

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
        // Create an object of type 'Employee' having the default value.
        // Use the optionally specified 'basicAllocator' to supply memory.
        // If 'basicAllocator' is 0, the currently installed default allocator
        // is used.

    Employee(const Employee& original, bslma_Allocator *basicAllocator = 0);
        // Create an object of type 'Employee' having the value
        // of the specified 'original' object.  Use the optionally specified
        // 'basicAllocator' to supply memory.  If 'basicAllocator' is 0,
        // the currently installed default allocator is used.

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
        // Return a reference to the modifiable "HomeAddress" attribute of
        // this object.

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
        // Return a reference to the non-modifiable "Name"
        // attribute of this object.

    const Address& homeAddress() const;
        // Return a reference to the non-modifiable "HomeAddress"
        // attribute of this object.

    const int& age() const;
        // Return a reference to the non-modifiable "Age"
        // attribute of this object.

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

// ============================================================================
//                      INLINE FUNCTION DEFINITIONS
// ============================================================================

// The following inlined functions are invoked from other inline functions.

inline
int Employee::maxSupportedBdexVersion()
{
    return 1;  // versions start at 1.
}

// ---------------------------------------------------------------------------

// CREATORS
inline
Employee::Employee(bslma_Allocator *basicAllocator)
: d_name(bslma_Default::allocator(basicAllocator))
, d_homeAddress(bslma_Default::allocator(basicAllocator))
, d_age()
{
}

inline
Employee::Employee(
    const Employee& original,
    bslma_Allocator *basicAllocator)
: d_name(original.d_name, bslma_Default::allocator(basicAllocator))
, d_homeAddress(original.d_homeAddress,
                bslma_Default::allocator(basicAllocator))
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
            bdex_InStreamFunctions::streamIn(
                  stream, d_name, 1);
            bdex_InStreamFunctions::streamIn(
                  stream, d_homeAddress, 1);
            bdex_InStreamFunctions::streamIn(
                  stream, d_age, 1);
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

    ret = manipulator(&d_name,
                      ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_NAME]);
    if (ret) {
        return ret;                                                 // RETURN
    }

    ret = manipulator(&d_homeAddress,
                      ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_HOME_ADDRESS]);
    if (ret) {
        return ret;                                                 // RETURN
    }

    ret = manipulator(&d_age, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_AGE]);
    if (ret) {
        return ret;                                                 // RETURN
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
int Employee::manipulateAttribute(MANIPULATOR&  manipulator,
                                     const char   *name,
                                     int           nameLength)
{
    enum { NOT_FOUND = -1 };

    const bdeat_AttributeInfo *attributeInfo =
           lookupAttributeInfo(name, nameLength);
    if (0 == attributeInfo) {
        return NOT_FOUND;                                           // RETURN
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
        return ret;                                                 // RETURN
    }

    ret = accessor(d_homeAddress,
                   ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_HOME_ADDRESS]);
    if (ret) {
        return ret;                                                 // RETURN
    }

    ret = accessor(d_age, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_AGE]);
    if (ret) {
        return ret;                                                 // RETURN
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
int Employee::accessAttribute(ACCESSOR&   accessor,
                                 const char *name,
                                 int         nameLength) const
{
    enum { NOT_FOUND = -1 };

     const bdeat_AttributeInfo *attributeInfo =
           lookupAttributeInfo(name, nameLength);
     if (0 == attributeInfo) {
        return NOT_FOUND;                                           // RETURN
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

}  // close namespace test;

// TRAITS
BDEAT_DECL_SEQUENCE_WITH_ALLOCATOR_TRAITS(test::Employee)

// FREE OPERATORS
inline
bool test::operator==(const test::Employee& lhs, const test::Employee& rhs)
{
    return  lhs.name() == rhs.name()
         && lhs.homeAddress() == rhs.homeAddress()
         && lhs.age() == rhs.age();
}

inline
bool test::operator!=(const test::Employee& lhs, const test::Employee& rhs)
{
    return  lhs.name() != rhs.name()
         || lhs.homeAddress() != rhs.homeAddress()
         || lhs.age() != rhs.age();
}

inline
bsl::ostream& test::operator<<(bsl::ostream& stream, const test::Employee& rhs)
{
    return rhs.print(stream, 0, -1);
}

}  // close namespace BloombergLP;

#endif

// ---------------------------------------------------------------------------
// *End-of-File Block removed*
// ---------------------------------------------------------------------------

// test_employee.cpp  -*-C++-*-

#include <bsl_iostream.h>

#include <bdeu_print.h>
#include <bdeu_printmethods.h>
#include <bdeu_chartype.h>
#include <bdeat_formattingmode.h>

#include <bsl_string.h>

namespace BloombergLP {
namespace test {

                               // ---------
                               // CONSTANTS
                               // ---------

const char Employee::CLASS_NAME[] = "Employee";
    // the name of this class

const bdeat_AttributeInfo Employee::ATTRIBUTE_INFO_ARRAY[] = {
    {
        ATTRIBUTE_ID_NAME,
        "name",                     // name
        sizeof("name") - 1,         // name length
        "todo: provide annotation",// annotation
        bdeat_FormattingMode::BDEAT_TEXT // formatting mode
    },
    {
        ATTRIBUTE_ID_HOME_ADDRESS,
        "homeAddress",                     // name
        sizeof("homeAddress") - 1,         // name length
        "todo: provide annotation",// annotation
        bdeat_FormattingMode::BDEAT_DEFAULT // formatting mode
    },
    {
        ATTRIBUTE_ID_AGE,
        "age",                     // name
        sizeof("age") - 1,         // name length
        "todo: provide annotation",// annotation
        bdeat_FormattingMode::BDEAT_DEC // formatting mode
    }
};

                               // -------------
                               // CLASS METHODS
                               // -------------

const bdeat_AttributeInfo *Employee::lookupAttributeInfo(
        const char *name,
        int         nameLength)
{
    switch(nameLength) {
        case 3: {
            if (bdeu_CharType::toUpper(name[0])=='A'
             && bdeu_CharType::toUpper(name[1])=='G'
             && bdeu_CharType::toUpper(name[2])=='E') {
                return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_AGE];
            }
        } break;
        case 4: {
            if (bdeu_CharType::toUpper(name[0])=='N'
             && bdeu_CharType::toUpper(name[1])=='A'
             && bdeu_CharType::toUpper(name[2])=='M'
             && bdeu_CharType::toUpper(name[3])=='E') {
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
             && bdeu_CharType::toUpper(name[10])=='S') {
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

                                // --------
                                // CREATORS
                                // --------

                                // ------------
                                // MANIPULATORS
                                // ------------

                                // ---------
                                // ACCESSORS
                                // ---------

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

}  // close namespace test;
}  // close namespace BloombergLP;

// ---------------------------------------------------------------------------
// *End-of-File Block removed*
// ---------------------------------------------------------------------------

//=============================================================================
//                               USAGE EXAMPLE
//-----------------------------------------------------------------------------

// The following snippets of code illustrate the usage of this component.
// Suppose we have an XML schema inside a file called 'xsdfile.xsd':
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
//  </xs:schema>
//..
// Using the 'bde_xsdcc.pl' tool, we can generate C++ classes for this schema:
//..
//  $ bde_xsdcc.pl -g h -g cpp -p test xsdfile.xsd
//..
// This tool will generate the header and implementation files for the
// 'test_address' and 'test_employee' components in the current directory.
//
// Now suppose we wanted to encode information about a particular employee
// using XML encoding to the standard output, using the 'PRETTY' option for
// formatting the output.  The following function will do this:
//..
//  #include <test_employee.h>
//
//  #include <baexml_encoder.h>
//  #include <baexml_encodingstyle.h>
//
//  #include <iostream>
//
//  using namespace BloombergLP;
//
void usageExample()
{
    test::Employee bob;

    bob.name()                 = "Bob";
    bob.homeAddress().street() = "Some Street";
    bob.homeAddress().city()   = "Some City";
    bob.homeAddress().state()  = "Some State";
    bob.age()                  = 21;

    bsl::stringstream ss;

    baexml_EncoderOptions options;
    options.setEncodingStyle(baexml_EncodingStyle::BAEXML_PRETTY);

    baexml_Encoder encoder(&options, 0, 0);
    int rc = encoder.encodeToStream(ss, bob);

    ASSERT(0 == rc);

    if (veryVerbose) {
        bsl::cout << ss.str();
    }
}
//..
// When this function is invoked, the following text will be printed to the
// standard output:
//..
//  <?xml version="1.0" encoding="UTF-8" ?>
//  <Employee>
//      <name>Bob</name>
//      <homeAddress>
//          <street>Some Street</street>
//          <city>Some City</city>
//          <state>Some State</state>
//      </homeAddress>
//      <age>21</age>
//  </Employee>
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
      case 12: {
        // --------------------------------------------------------------------
        // TESTING USAGE EXAMPLE
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting Usage Example"
                          << "\n=====================" << endl;

        usageExample();

        if (verbose) cout << "\nEnd of Test." << endl;
      } break;
      case 11: {
        // --------------------------------------------------------------------
        // TESTING AGGREGATES
        //
        // Concerns:
        //   Test XML encoding of bcem_Aggregate in the presence of default
        //   values.
        //
        // Plan:
        //
        // Testing:
        // --------------------------------------------------------------------

        typedef bdem_ElemType ET;
        bdem_Schema schema;
        bdem_RecordDef *root = schema.createRecord("root");
        bdem_FieldDefAttributes boolAttr(ET::BDEM_BOOL);
        boolAttr.setIsNullable(true);
        root->appendField(boolAttr, "bool1");
        boolAttr.defaultValue().theModifiableBool() = true;
        root->appendField(boolAttr, "bool2");

        bcema_SharedPtr<bdem_Schema> schemaPtr(&schema,
                                               bcema_SharedPtrNilDeleter(),
                                               0);
        bcem_Aggregate agg(schemaPtr, "root");
        agg.setField("bool2", false);

        const char expectedXML[] =
            "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
            "<root>\n"
            "    <bool2>false</bool2>\n"
            "</root>\n";

        bsl::stringstream output;
        baexml_EncoderOptions options;
        options.setEncodingStyle(baexml_EncodingStyle::BAEXML_PRETTY);

        baexml_Encoder encoder(&options, 0, 0);
        int rc = encoder.encodeToStream(output, agg);

        ASSERT(0 == rc)
        ASSERT(output);
        LOOP2_ASSERT(expectedXML, output.str(), expectedXML == output.str());

        if (veryVerbose && 0 == testStatus) {
            P(expectedXML); P(output.str());
        }

        output.str("");
        encoder.encode(output, agg);

        ASSERT(output);
        LOOP2_ASSERT(expectedXML, output.str(), expectedXML == output.str());

        if (verbose) { P(output.str()); }

      } break;
      case 10: {
        // --------------------------------------------------------------------
        // TESTING ENCODING OF NILLABLES
        //   This will test encoding of nillables.
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting Encoding of Nillables"
                          << "\n=============================" << endl;

        static const char OBJ_NS[] =
            "http://bloomberg.com/schemas/baexml_encoder.t.xsd";
        static const char PRETTY_NS_ATTR[] = "\n    "
            "xmlns=\"http://bloomberg.com/schemas/baexml_encoder.t.xsd\"";
        static const char PRETTY_XSI_ATTR[] = "\n    "
            "xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\"";
        static const char COMPACT_NS_ATTR[] =
            " xmlns=\"http://bloomberg.com/schemas/baexml_encoder.t.xsd\"";
        static const char COMPACT_XSI_ATTR[] =
            " xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\"";
        static const char NIL_ATTR[]=" xsi:nil=\"true\"";

        typedef test::MySequenceWithNillables Type;

        const int         ATTRIBUTE1_VALUE = 123;
        const bsl::string ATTRIBUTE2_VALUE = "test string";
        test::MySequence  ATTRIBUTE3_VALUE;

        ATTRIBUTE3_VALUE.attribute1() = 987;
        ATTRIBUTE3_VALUE.attribute2() = "inner";

        for (int i = 0; i < Type::NUM_ATTRIBUTES; ++i) {

            // In each expected string, the first %s will be replaced by the
            // object namespace, the second %s will be replaced by the xsi
            // namespace, and the third %s will be replaced by the xsi:nil
            // attribute if the object namespace is used; otherwise, they will
            // be replaced by empty strings.
            static const char *PRETTY_RESULT[3] = {
                "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                "<MySequenceWithNillables%s%s>\n"
                "    <Attribute1%s/>\n"
                "    <Attribute2>test string</Attribute2>\n"
                "    <Attribute3>\n"
                "        <Attribute1>987</Attribute1>\n"
                "        <Attribute2>inner</Attribute2>\n"
                "    </Attribute3>\n"
                "</MySequenceWithNillables>\n",

                "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                "<MySequenceWithNillables%s%s>\n"
                "    <Attribute1>123</Attribute1>\n"
                "    <Attribute2%s/>\n"
                "    <Attribute3>\n"
                "        <Attribute1>987</Attribute1>\n"
                "        <Attribute2>inner</Attribute2>\n"
                "    </Attribute3>\n"
                "</MySequenceWithNillables>\n",

                "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                "<MySequenceWithNillables%s%s>\n"
                "    <Attribute1>123</Attribute1>\n"
                "    <Attribute2>test string</Attribute2>\n"
                "    <Attribute3%s/>\n"
                "</MySequenceWithNillables>\n",
            };

            Type mX;  const Type& X = mX;

            // Each attribute except attribute 'i' should be non-null:
            if (0 != i) {
                mX.attribute1().makeValue(ATTRIBUTE1_VALUE);
            }
            if (1 != i) {
                mX.attribute2().makeValue(ATTRIBUTE2_VALUE);
            }
            if (2 != i) {
                mX.attribute3().makeValue(ATTRIBUTE3_VALUE);
            }

            char expectedResult[1000];
            const char *EXPECTED_RESULT = expectedResult;

            if (verbose) cout << "PRETTY without object namespace" << endl;

            // Format expected result without object namespace and,
            // therefore, without 'xmlns', 'xmlns:xsi', and 'xsi:nil':
            bsl::sprintf(expectedResult, PRETTY_RESULT[i], "", "", "");

            if (veryVerbose) { T_ P_(i) P_(X) P(EXPECTED_RESULT) }

            bsl::stringstream result;

            EncoderOptions options;
            options.setEncodingStyle(EncodingStyle::BAEXML_PRETTY);

            baexml_Encoder encoder(&options, 0, 0);
            int rc = encoder.encodeToStream(result, X);

            LOOP_ASSERT(rc, 0 == rc);
            LOOP2_ASSERT(EXPECTED_RESULT,   result.str(),
                         EXPECTED_RESULT == result.str());

            if (verbose) cout << "PRETTY with object namespace" << endl;

            // Format expected result with object namespace and,
            // therefore, with 'xmlns', 'xmlns:xsi', and 'xsi:nil':
            bsl::sprintf(expectedResult, PRETTY_RESULT[i],
                         PRETTY_NS_ATTR, PRETTY_XSI_ATTR, NIL_ATTR);

            if (veryVerbose) { T_ P_(i) P_(X) P(EXPECTED_RESULT) }

            result.str("");

            options.setObjectNamespace(OBJ_NS);
            options.setEncodingStyle(EncodingStyle::BAEXML_PRETTY);

            rc = encoder.encodeToStream(result, X);

            LOOP_ASSERT(rc, 0 == rc);
            LOOP2_ASSERT(EXPECTED_RESULT,   result.str(),
                         EXPECTED_RESULT == result.str());

            static const char *COMPACT_RESULT[3] = {
                "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>"
                "<MySequenceWithNillables%s%s>"
                    "<Attribute1%s/>"
                    "<Attribute2>test string</Attribute2>"
                    "<Attribute3>"
                        "<Attribute1>987</Attribute1>"
                        "<Attribute2>inner</Attribute2>"
                    "</Attribute3>"
                "</MySequenceWithNillables>",

                "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>"
                "<MySequenceWithNillables%s%s>"
                    "<Attribute1>123</Attribute1>"
                    "<Attribute2%s/>"
                    "<Attribute3>"
                        "<Attribute1>987</Attribute1>"
                        "<Attribute2>inner</Attribute2>"
                    "</Attribute3>"
                "</MySequenceWithNillables>",

                "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>"
                "<MySequenceWithNillables%s%s>"
                    "<Attribute1>123</Attribute1>"
                    "<Attribute2>test string</Attribute2>"
                    "<Attribute3%s/>"
                "</MySequenceWithNillables>",
            };

            if (verbose) cout << "COMPACT without object namespace" << endl;

            // Format expected result without object namespace and,
            // therefore, without 'xmlns', 'xmlns:xsi', and 'xsi:nil':
            bsl::sprintf(expectedResult, COMPACT_RESULT[i], "", "", "");

            if (veryVerbose) { T_ P_(i) P_(X) P(EXPECTED_RESULT) }

            result.str("");

            options.setObjectNamespace("");
            options.setEncodingStyle(EncodingStyle::BAEXML_COMPACT);

            rc = encoder.encodeToStream(result, X);

            LOOP_ASSERT(rc, 0 == rc);
            LOOP2_ASSERT(EXPECTED_RESULT,   result.str(),
                         EXPECTED_RESULT == result.str());

            if (verbose) cout << "COMPACT with object namespace" << endl;

            // Format expected result with object namespace and,
            // therefore, with 'xmlns', 'xmlns:xsi', and 'xsi:nil':
            bsl::sprintf(expectedResult, COMPACT_RESULT[i],
                         COMPACT_NS_ATTR, COMPACT_XSI_ATTR, NIL_ATTR);

            if (veryVerbose) { T_ P_(i) P_(X) P(EXPECTED_RESULT) }

            result.str("");

            options.setObjectNamespace(OBJ_NS);
            options.setEncodingStyle(EncodingStyle::BAEXML_COMPACT);

            rc = encoder.encodeToStream(result, X);

            LOOP_ASSERT(rc, 0 == rc);
            LOOP2_ASSERT(EXPECTED_RESULT,   result.str(),
                         EXPECTED_RESULT == result.str());

        } // End for i

        if (verbose) cout << "\nEnd of Test." << endl;

      } break;
      case 9: {
        // --------------------------------------------------------------------
        // TESTING ENCODING OF SIMPLE CONTENT
        //   This will test encoding of simple content.
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting Encoding of Simple Content"
                          << "\n==================================" << endl;

        if (verbose) cout << "\nUsing 'MySimpleContent'." << endl;
        {
            typedef test::MySimpleContent Type;

            Type mX;  const Type& X = mX;

            mX.attribute1() = true;
            mX.attribute2() = "Hello World!";
            mX.theContent() = "  Some Stuff ";

            const char EXPECTED_RESULT[]
                             = "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                               "<MySimpleContent Attribute1=\"true\" "
                               "Attribute2=\"Hello World!\">"
                               "  Some Stuff "
                               "</MySimpleContent>\n";

            if (veryVerbose) {
                T_ P_(X) P(EXPECTED_RESULT)
            }

            bsl::stringstream result;

            EncoderOptions options;
            options.setEncodingStyle(EncodingStyle::BAEXML_PRETTY);

            baexml_Encoder encoder(&options, 0, 0);
            int rc = encoder.encodeToStream(result, X);

            LOOP_ASSERT(rc, 0 == rc);
            LOOP2_ASSERT(EXPECTED_RESULT,   result.str(),
                         EXPECTED_RESULT == result.str());
        }

        if (verbose) cout << "\nUsing 'MySimpleContent' with COMPACT." << endl;
        {
            typedef test::MySimpleContent Type;

            Type mX;  const Type& X = mX;

            mX.attribute1() = true;
            mX.attribute2() = "Hello World!";
            mX.theContent() = "  Some Stuff ";

            const char EXPECTED_RESULT[] =
                               "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>"
                               "<MySimpleContent Attribute1=\"true\" "
                               "Attribute2=\"Hello World!\">"
                               "  Some Stuff "
                               "</MySimpleContent>";

            if (veryVerbose) {
                T_ P_(X) P(EXPECTED_RESULT)
            }

            bsl::stringstream result;

            EncoderOptions options;
            options.setEncodingStyle(EncodingStyle::BAEXML_COMPACT);

            baexml_Encoder encoder(&options, 0, 0);
            int rc = encoder.encodeToStream(result, X);

            LOOP_ASSERT(rc, 0 == rc);
            LOOP2_ASSERT(EXPECTED_RESULT,   result.str(),
                         EXPECTED_RESULT == result.str());
        }

        if (verbose) cout << "\nUsing 'MySimpleIntContent'." << endl;
        {
            typedef test::MySimpleIntContent Type;

            Type mX;  const Type& X = mX;

            mX.attribute1() = false;
            mX.attribute2() = "Hello World!";
            mX.theContent() = 837;

            const char EXPECTED_RESULT[]
                             = "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                               "<MySimpleIntContent Attribute1=\"false\" "
                               "Attribute2=\"Hello World!\">"
                               "837"
                               "</MySimpleIntContent>\n";

            if (veryVerbose) {
                T_ P_(X) P(EXPECTED_RESULT)
            }

            bsl::stringstream result;

            EncoderOptions options;
            options.setEncodingStyle(EncodingStyle::BAEXML_PRETTY);

            baexml_Encoder encoder(&options, 0, 0);
            int rc = encoder.encodeToStream(result, X);

            LOOP_ASSERT(rc, 0 == rc);
            LOOP2_ASSERT(EXPECTED_RESULT,   result.str(),
                         EXPECTED_RESULT == result.str());
        }

        if (verbose) cout << "\nUsing 'MySimpleIntContent' with COMPACT."
                          << endl;
        {
            typedef test::MySimpleIntContent Type;

            Type mX;  const Type& X = mX;

            mX.attribute1() = false;
            mX.attribute2() = "Hello World!";
            mX.theContent() = 837;

            const char EXPECTED_RESULT[] =
                               "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>"
                               "<MySimpleIntContent Attribute1=\"false\" "
                               "Attribute2=\"Hello World!\">"
                               "837"
                               "</MySimpleIntContent>";

            if (veryVerbose) {
                T_ P_(X) P(EXPECTED_RESULT)
            }

            bsl::stringstream result;

            EncoderOptions options;
            options.setEncodingStyle(EncodingStyle::BAEXML_COMPACT);

            baexml_Encoder encoder(&options, 0, 0);
            int rc = encoder.encodeToStream(result, X);

            LOOP_ASSERT(rc, 0 == rc);
            LOOP2_ASSERT(EXPECTED_RESULT,   result.str(),
                         EXPECTED_RESULT == result.str());
        }

        if (verbose) cout << "\nEnd of Test." << endl;
      } break;
      case 8: {
        // --------------------------------------------------------------------
        // TESTING ENCODING OF XML ATTRIBUTES
        //   This will test encoding of XML attributes.
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting Encoding of XML Attributes"
                          << "\n==================================" << endl;

        if (verbose) cout << "\nUsing 'MySequenceWithAttributes' without "
                          << "attribute2." << endl;
        {
            typedef test::MySequenceWithAttributes Type;

            Type mX;  const Type& X = mX;

            mX.attribute1() = 34;
            mX.element1() = 45;
            mX.element2() = "Hello";

            const char EXPECTED_RESULT[] =
                               "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                               "<MySequenceWithAttributes Attribute1=\"34\">\n"
                               "    <Element1>45</Element1>\n"
                               "    <Element2>Hello</Element2>\n"
                               "</MySequenceWithAttributes>\n";

            if (veryVerbose) {
                T_ P_(X) P(EXPECTED_RESULT)
            }

            bsl::stringstream result;

            EncoderOptions options;
            options.setEncodingStyle(EncodingStyle::BAEXML_PRETTY);

            baexml_Encoder encoder(&options, 0, 0);
            int rc = encoder.encodeToStream(result, X);

            LOOP_ASSERT(rc, 0 == rc);
            LOOP2_ASSERT(EXPECTED_RESULT,   result.str(),
                         EXPECTED_RESULT == result.str());
        }

        if (verbose) cout << "\nUsing 'MySequenceWithAttributes' with "
                          << "attribute2." << endl;
        {
            typedef test::MySequenceWithAttributes Type;

            Type mX;  const Type& X = mX;

            mX.attribute1() = 34;
            mX.attribute2() = "World!";
            mX.element1() = 45;
            mX.element2() = "Hello";

            const char EXPECTED_RESULT[]
                             = "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                               "<MySequenceWithAttributes Attribute1=\"34\" "
                               "Attribute2=\"World!\">\n"
                               "    <Element1>45</Element1>\n"
                               "    <Element2>Hello</Element2>\n"
                               "</MySequenceWithAttributes>\n";

            if (veryVerbose) {
                T_ P_(X) P(EXPECTED_RESULT)
            }

            bsl::stringstream result;

            EncoderOptions options;
            options.setEncodingStyle(EncodingStyle::BAEXML_PRETTY);

            baexml_Encoder encoder(&options, 0, 0);
            int rc = encoder.encodeToStream(result, X);

            LOOP_ASSERT(rc, 0 == rc);
            LOOP2_ASSERT(EXPECTED_RESULT,   result.str(),
                         EXPECTED_RESULT == result.str());
        }

        if (verbose) cout << "\nUsing 'MySequenceWithAttributes' with "
                          << "attribute2 with COMPACT." << endl;
        {
            typedef test::MySequenceWithAttributes Type;

            Type mX;  const Type& X = mX;

            mX.attribute1() = 34;
            mX.attribute2() = "World!";
            mX.element1() = 45;
            mX.element2() = "Hello";

            const char EXPECTED_RESULT[] =
                                "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>"
                               "<MySequenceWithAttributes Attribute1=\"34\" "
                               "Attribute2=\"World!\">"
                               "<Element1>45</Element1>"
                               "<Element2>Hello</Element2>"
                               "</MySequenceWithAttributes>";

            if (veryVerbose) {
                T_ P_(X) P(EXPECTED_RESULT)
            }

            bsl::stringstream result;

            EncoderOptions options;
            options.setEncodingStyle(EncodingStyle::BAEXML_COMPACT);

            baexml_Encoder encoder(&options, 0, 0);
            int rc = encoder.encodeToStream(result, X);

            LOOP_ASSERT(rc, 0 == rc);
            LOOP2_ASSERT(EXPECTED_RESULT,   result.str(),
                         EXPECTED_RESULT == result.str());
        }

        if (verbose) cout << "\nEnd of Test." << endl;
      } break;
      case 7: {
        // --------------------------------------------------------------------
        // TESTING ENCODING OF ANONYMOUS CHOICE
        //   This will test encoding of anonymous choices.
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting Encoding of Anonymous Choice"
                          << "\n====================================" << endl;

        if (verbose) cout << "\nUsing 'MySequenceWithAnonymousChoice' with "
                          << "selection 1." << endl;
        {
            typedef test::MySequenceWithAnonymousChoice Type;

            Type mX;  const Type& X = mX;

            mX.attribute1() = 34;
            mX.theChoice().makeMyChoice1(67);
            mX.attribute2() = "Hello";

            const char EXPECTED_RESULT[]
                              = "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                                "<MySequenceWithAnonymousChoice>\n"
                                "    <Attribute1>34</Attribute1>\n"
                                "    <MyChoice1>67</MyChoice1>\n"
                                "    <Attribute2>Hello</Attribute2>\n"
                                "</MySequenceWithAnonymousChoice>\n";

            if (veryVerbose) {
                T_ P_(X) P(EXPECTED_RESULT)
            }

            bsl::stringstream result;

            EncoderOptions options;
            options.setEncodingStyle(EncodingStyle::BAEXML_PRETTY);

            baexml_Encoder encoder(&options, 0, 0);
            int rc = encoder.encodeToStream(result, X);

            LOOP_ASSERT(rc, 0 == rc);
            LOOP2_ASSERT(EXPECTED_RESULT,   result.str(),
                         EXPECTED_RESULT == result.str());
        }

        if (verbose) cout << "\nUsing 'MySequenceWithAnonymousChoice' with "
                          << "selection 2." << endl;
        {
            typedef test::MySequenceWithAnonymousChoice Type;

            Type mX;  const Type& X = mX;

            mX.attribute1() = 34;
            mX.theChoice().makeMyChoice2("World!");
            mX.attribute2() = "Hello";

            const char EXPECTED_RESULT[]
                              = "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                                "<MySequenceWithAnonymousChoice>\n"
                                "    <Attribute1>34</Attribute1>\n"
                                "    <MyChoice2>World!</MyChoice2>\n"
                                "    <Attribute2>Hello</Attribute2>\n"
                                "</MySequenceWithAnonymousChoice>\n";

            if (veryVerbose) {
                T_ P_(X) P(EXPECTED_RESULT)
            }

            bsl::stringstream result;

            EncoderOptions options;
            options.setEncodingStyle(EncodingStyle::BAEXML_PRETTY);

            baexml_Encoder encoder(&options, 0, 0);
            int rc = encoder.encodeToStream(result, X);

            LOOP_ASSERT(rc, 0 == rc);
            LOOP2_ASSERT(EXPECTED_RESULT,   result.str(),
                         EXPECTED_RESULT == result.str());
        }

        if (verbose) cout << "\nUsing 'MySequenceWithAnonymousChoice' with "
                          << "selection 2 with COMPACT." << endl;
        {
            typedef test::MySequenceWithAnonymousChoice Type;

            Type mX;  const Type& X = mX;

            mX.attribute1() = 34;
            mX.theChoice().makeMyChoice2("World!");
            mX.attribute2() = "Hello";

            const char EXPECTED_RESULT[] =
                                "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>"
                                "<MySequenceWithAnonymousChoice>"
                                "<Attribute1>34</Attribute1>"
                                "<MyChoice2>World!</MyChoice2>"
                                "<Attribute2>Hello</Attribute2>"
                                "</MySequenceWithAnonymousChoice>";

            if (veryVerbose) {
                T_ P_(X) P(EXPECTED_RESULT)
            }

            bsl::stringstream result;

            EncoderOptions options;
            options.setEncodingStyle(EncodingStyle::BAEXML_COMPACT);

            baexml_Encoder encoder(&options, 0, 0);
            int rc = encoder.encodeToStream(result, X);

            LOOP_ASSERT(rc, 0 == rc);
            LOOP2_ASSERT(EXPECTED_RESULT,   result.str(),
                         EXPECTED_RESULT == result.str());
        }

        if (verbose) cout << "\nEnd of Test." << endl;
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // TESTING ENCODING OF NULLABLES
        //   This will test encoding of nullables.
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting Encoding of Nullables"
                          << "\n=============================" << endl;

        if (verbose) cout << "\nUsing 'MySequenceWithNullables'." << endl;
        {
            typedef test::MySequenceWithNullables Type;

            const int         ATTRIBUTE1_VALUE = 123;
            const bsl::string ATTRIBUTE2_VALUE = "test string";
            test::MySequence  ATTRIBUTE3_VALUE;

            ATTRIBUTE3_VALUE.attribute1() = 987;
            ATTRIBUTE3_VALUE.attribute2() = "inner";

            const char *EXPECTED_RESULT[3]
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
                    T_ P_(i) P_(X) P(EXPECTED_RESULT[i])
                }

                bsl::stringstream result;

                EncoderOptions options;
                options.setEncodingStyle(EncodingStyle::BAEXML_PRETTY);

                baexml_Encoder encoder(&options, 0, 0);
                int rc = encoder.encodeToStream(result, X);

                LOOP_ASSERT(rc, 0 == rc);
                LOOP2_ASSERT(EXPECTED_RESULT[i],   result.str(),
                             EXPECTED_RESULT[i] == result.str());
            }
        }

        if (verbose) cout << "\nUsing 'MySequenceWithNullables' with COMPACT."
                          << endl;
        {
            typedef test::MySequenceWithNullables Type;

            const int         ATTRIBUTE1_VALUE = 123;
            const bsl::string ATTRIBUTE2_VALUE = "test string";
            test::MySequence  ATTRIBUTE3_VALUE;

            ATTRIBUTE3_VALUE.attribute1() = 987;
            ATTRIBUTE3_VALUE.attribute2() = "inner";

            const char *EXPECTED_RESULT[3]
                            = {
                                "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>"
                                "<MySequenceWithNullables>"
                                "<Attribute2>test string</Attribute2>"
                                "<Attribute3>"
                                "<Attribute1>987</Attribute1>"
                                "<Attribute2>inner</Attribute2>"
                                "</Attribute3>"
                                "</MySequenceWithNullables>",

                                "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>"
                                "<MySequenceWithNullables>"
                                "<Attribute1>123</Attribute1>"
                                "<Attribute3>"
                                "<Attribute1>987</Attribute1>"
                                "<Attribute2>inner</Attribute2>"
                                "</Attribute3>"
                                "</MySequenceWithNullables>",

                                "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>"
                                "<MySequenceWithNullables>"
                                "<Attribute1>123</Attribute1>"
                                "<Attribute2>test string</Attribute2>"
                                "</MySequenceWithNullables>",
                              };

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
                    T_ P_(i) P_(X) P(EXPECTED_RESULT[i])
                }

                bsl::stringstream result;

                EncoderOptions options;
                options.setEncodingStyle(EncodingStyle::BAEXML_COMPACT);

                baexml_Encoder encoder(&options, 0, 0);
                int rc = encoder.encodeToStream(result, X);

                LOOP_ASSERT(rc, 0 == rc);
                LOOP2_ASSERT(EXPECTED_RESULT[i],   result.str(),
                             EXPECTED_RESULT[i] == result.str());
            }
        }

        if (verbose) cout << "\nEnd of Test." << endl;
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING ENCODING OF ARRAYS
        //   This will test encoding of arrays.
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting Encoding of Arrays"
                          << "\n==========================" << endl;

        if (verbose) cout << "\nUsing 'MySequenceWithArrays'." << endl;
        {
            typedef test::MySequenceWithArrays Type;

            Type mX;  const Type& X = mX;

            mX.attribute1().push_back(123);  // int repetition
            mX.attribute1().push_back(456);
            mX.attribute1().push_back(927);

            mX.attribute2().push_back('a');  // base64
            mX.attribute2().push_back('b');
            mX.attribute2().push_back('c');
            mX.attribute2().push_back('d');

            mX.attribute3().push_back('a');  // hex
            mX.attribute3().push_back('b');
            mX.attribute3().push_back('c');
            mX.attribute3().push_back('d');

            mX.attribute4().push_back('a');  // text
            mX.attribute4().push_back('b');
            mX.attribute4().push_back('c');
            mX.attribute4().push_back('d');

            mX.attribute5().push_back('a');  // byte repetition
            mX.attribute5().push_back('b');
            mX.attribute5().push_back('c');
            mX.attribute5().push_back('d');

            mX.attribute6().push_back('a');  // byte list
            mX.attribute6().push_back('b');
            mX.attribute6().push_back('c');
            mX.attribute6().push_back('d');

            mX.attribute7().push_back(123);  // int list
            mX.attribute7().push_back(456);
            mX.attribute7().push_back(927);

            const char EXPECTED_RESULT[]
                              = "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                                "<MySequenceWithArrays>\n"
                                "    <Attribute1>123</Attribute1>\n"
                                "    <Attribute1>456</Attribute1>\n"
                                "    <Attribute1>927</Attribute1>\n"
                                "    <Attribute2>YWJjZA==</Attribute2>\n"
                                "    <Attribute3>61626364</Attribute3>\n"
                                "    <Attribute4>abcd</Attribute4>\n"
                                "    <Attribute5>97</Attribute5>\n"
                                "    <Attribute5>98</Attribute5>\n"
                                "    <Attribute5>99</Attribute5>\n"
                                "    <Attribute5>100</Attribute5>\n"
                                "    <Attribute6>97 98 99 100</Attribute6>\n"
                                "    <Attribute7>123 456 927</Attribute7>\n"
                                "</MySequenceWithArrays>\n";

            if (veryVerbose) {
                T_ P_(X) P(EXPECTED_RESULT)
            }

            bsl::stringstream result;

            EncoderOptions options;
            options.setEncodingStyle(EncodingStyle::BAEXML_PRETTY);

            baexml_Encoder encoder(&options, 0, 0);
            int rc = encoder.encodeToStream(result, X);

            LOOP_ASSERT(rc, 0 == rc);
            LOOP2_ASSERT(EXPECTED_RESULT,   result.str(),
                         EXPECTED_RESULT == result.str());

            result.str("");
            encoder.encode(result, X);
            LOOP2_ASSERT(EXPECTED_RESULT,   result.str(),
                         EXPECTED_RESULT == result.str());
        }

        if (verbose) cout << "\nUsing 'MySequenceWithArrays' with COMPACT."
                          << endl;
        {
            typedef test::MySequenceWithArrays Type;

            Type mX;  const Type& X = mX;

            mX.attribute1().push_back(123);  // int repetition
            mX.attribute1().push_back(456);
            mX.attribute1().push_back(927);

            mX.attribute2().push_back('a');  // base64
            mX.attribute2().push_back('b');
            mX.attribute2().push_back('c');
            mX.attribute2().push_back('d');

            mX.attribute3().push_back('a');  // hex
            mX.attribute3().push_back('b');
            mX.attribute3().push_back('c');
            mX.attribute3().push_back('d');

            mX.attribute4().push_back('a');  // text
            mX.attribute4().push_back('b');
            mX.attribute4().push_back('c');
            mX.attribute4().push_back('d');

            mX.attribute5().push_back('a');  // byte repetition
            mX.attribute5().push_back('b');
            mX.attribute5().push_back('c');
            mX.attribute5().push_back('d');

            mX.attribute6().push_back('a');  // byte list
            mX.attribute6().push_back('b');
            mX.attribute6().push_back('c');
            mX.attribute6().push_back('d');

            mX.attribute7().push_back(123);  // int list
            mX.attribute7().push_back(456);
            mX.attribute7().push_back(927);

            const char EXPECTED_RESULT[]=
                                "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>"
                                "<MySequenceWithArrays>"
                                "<Attribute1>123</Attribute1>"
                                "<Attribute1>456</Attribute1>"
                                "<Attribute1>927</Attribute1>"
                                "<Attribute2>YWJjZA==</Attribute2>"
                                "<Attribute3>61626364</Attribute3>"
                                "<Attribute4>abcd</Attribute4>"
                                "<Attribute5>97</Attribute5>"
                                "<Attribute5>98</Attribute5>"
                                "<Attribute5>99</Attribute5>"
                                "<Attribute5>100</Attribute5>"
                                "<Attribute6>97 98 99 100</Attribute6>"
                                "<Attribute7>123 456 927</Attribute7>"
                                "</MySequenceWithArrays>";

            if (veryVerbose) {
                T_ P_(X) P(EXPECTED_RESULT)
            }

            bsl::stringstream result;

            EncoderOptions options;
            options.setEncodingStyle(EncodingStyle::BAEXML_COMPACT);

            baexml_Encoder encoder(&options, 0, 0);
            int rc = encoder.encodeToStream(result, X);

            LOOP_ASSERT(rc, 0 == rc);
            LOOP2_ASSERT(EXPECTED_RESULT,   result.str(),
                         EXPECTED_RESULT == result.str());

            result.str("");
            encoder.encode(result, X);
            LOOP2_ASSERT(EXPECTED_RESULT,   result.str(),
                         EXPECTED_RESULT == result.str());
        }

        if (verbose) cout << "\nEnd of Test." << endl;
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING ENCODING OF CHOICE
        //   This will test encoding of choices.
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting Encoding of Choice"
                          << "\n==========================" << endl;

        if (verbose) cout << "\nUsing 'MyChoice' with selection 1." << endl;
        {
            typedef test::MyChoice Type;

            Type mX;  const Type& X = mX;

            mX.makeSelection1();
            mX.selection1() = 434;

            const char EXPECTED_RESULT[]
                              = "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                                "<MyChoice>\n"
                                "    <Selection1>434</Selection1>\n"
                                "</MyChoice>\n";

            if (veryVerbose) {
                T_ P_(X) P(EXPECTED_RESULT)
            }

            bsl::stringstream result;

            EncoderOptions options;
            options.setEncodingStyle(EncodingStyle::BAEXML_PRETTY);

            baexml_Encoder encoder(&options, 0, 0);
            int rc = encoder.encodeToStream(result, X);

            LOOP_ASSERT(rc, 0 == rc);
            LOOP2_ASSERT(EXPECTED_RESULT,   result.str(),
                         EXPECTED_RESULT == result.str());

            result.str("");
            encoder.encode(result, X);
            LOOP2_ASSERT(EXPECTED_RESULT,   result.str(),
                         EXPECTED_RESULT == result.str());
        }

        if (verbose) cout << "\nUsing 'MyChoice' with selection 2." << endl;
        {
            typedef test::MyChoice Type;

            Type mX;  const Type& X = mX;

            mX.makeSelection2();
            mX.selection2() = "test string";

            const char EXPECTED_RESULT[]
                              = "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                                "<MyChoice>\n"
                                "    <Selection2>test string</Selection2>\n"
                                "</MyChoice>\n";

            if (veryVerbose) {
                T_ P_(X) P(EXPECTED_RESULT)
            }

            bsl::stringstream result;

            EncoderOptions options;
            options.setEncodingStyle(EncodingStyle::BAEXML_PRETTY);

            baexml_Encoder encoder(&options, 0, 0);
            int rc = encoder.encodeToStream(result, X);

            LOOP_ASSERT(rc, 0 == rc);
            LOOP2_ASSERT(EXPECTED_RESULT,   result.str(),
                         EXPECTED_RESULT == result.str());

            result.str("");
            encoder.encode(result, X);
            LOOP2_ASSERT(EXPECTED_RESULT,   result.str(),
                         EXPECTED_RESULT == result.str());
        }

        if (verbose) cout << "\nEnd of Test." << endl;
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING ENCODING OF SEQUENCE
        //   This will test encoding of sequences.
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting Encoding of Sequence"
                          << "\n============================" << endl;

        if (verbose) cout << "\nUsing 'MySequence'." << endl;
        {
            typedef test::MySequence Type;

            Type mX;  const Type& X = mX;

            mX.attribute1() = 434;
            mX.attribute2() = "test string";

            const char EXPECTED_RESULT[]
                              = "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                                "<MySequence>\n"
                                "    <Attribute1>434</Attribute1>\n"
                                "    <Attribute2>test string</Attribute2>\n"
                                "</MySequence>\n";

            if (veryVerbose) {
                T_ P_(X) P(EXPECTED_RESULT)
            }

            bsl::stringstream result;

            EncoderOptions options;
            options.setEncodingStyle(EncodingStyle::BAEXML_PRETTY);

            baexml_Encoder encoder(&options, 0, 0);
            int rc = encoder.encodeToStream(result, X);

            LOOP_ASSERT(rc, 0 == rc);
            LOOP2_ASSERT(EXPECTED_RESULT,   result.str(),
                         EXPECTED_RESULT == result.str());

            result.str("");
            encoder.encode(result, X);
            LOOP2_ASSERT(EXPECTED_RESULT,   result.str(),
                         EXPECTED_RESULT == result.str());
        }

        if (verbose) cout << "\nUsing 'MySequence' with COMPACT." << endl;
        {
            typedef test::MySequence Type;

            Type mX;  const Type& X = mX;

            mX.attribute1() = 434;
            mX.attribute2() = "test string";

            const char EXPECTED_RESULT[]
                              = "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>"
                                "<MySequence>"
                                "<Attribute1>434</Attribute1>"
                                "<Attribute2>test string</Attribute2>"
                                "</MySequence>";

            if (veryVerbose) {
                T_ P_(X) P(EXPECTED_RESULT)
            }

            bsl::stringstream result;

            EncoderOptions options;
            options.setEncodingStyle(EncodingStyle::BAEXML_COMPACT);

            baexml_Encoder encoder(&options, 0, 0);
            int rc = encoder.encodeToStream(result, X);

            LOOP_ASSERT(rc, 0 == rc);
            LOOP2_ASSERT(EXPECTED_RESULT,   result.str(),
                         EXPECTED_RESULT == result.str());
            result.str("");
            encoder.encode(result, X);
            LOOP2_ASSERT(EXPECTED_RESULT,   result.str(),
                         EXPECTED_RESULT == result.str());
        }

        if (verbose) cout << "\nEnd of Test." << endl;
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING USAGE OF TypesPrinterUtil
        //   This will test the usage of 'TypesPrinterUtil' for handling basic
        //   XML types.
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting Usage of 'TypesPrinterUtil'"
                          << "\n===================================" << endl;

        if (verbose) cout << "\nUsing 'int' as parameterized 'TYPE'." << endl;
        {
            static const struct {
                int         d_lineNum;  // source line number
                int         d_value;    // input value
                const char *d_result;   // expected result
            } DATA[] = {
                //line  value  result
                //----  -----  ------
                { L_,   0,     "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                               "<int>0</int>\n"                              },
                { L_,   1,     "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                               "<int>1</int>\n"                              },
                { L_,   123,   "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                               "<int>123</int>\n"                            },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int   LINE            = DATA[i].d_lineNum;
                const int   VALUE           = DATA[i].d_value;
                const char *EXPECTED_RESULT = DATA[i].d_result;

                if (veryVerbose) {
                    T_ P_(LINE) P_(VALUE) P(EXPECTED_RESULT)
                }

                bsl::stringstream result;

                EncoderOptions options;
                options.setEncodingStyle(EncodingStyle::BAEXML_PRETTY);

                baexml_Encoder encoder(&options, 0, 0);
                int rc = encoder.encodeToStream(result, VALUE);

                LOOP_ASSERT(rc, 0 == rc);
                LOOP3_ASSERT(LINE, EXPECTED_RESULT,   result.str(),
                                   EXPECTED_RESULT == result.str());
            }
        }

        if (verbose) cout << "\nUsing 'bsl::string' as parameterized 'TYPE'."
                          << endl;
        {
            static const struct {
                int         d_lineNum;  // source line number
                const char *d_value;    // input value
                const char *d_result;   // expected result
            } DATA[] = {
                //line  value  result
                //----  -----  ------
                { L_,   "",    "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                               "<string></string>\n"                         },
                { L_,   "a",   "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                               "<string>a</string>\n"                        },
                { L_,   "abc", "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                               "<string>abc</string>\n"                      },
                { L_,   "\xc3\xb6" "abc",
                               "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                               "<string>\xc3\xb6" "abc</string>\n"           },
                { L_,   "abc\xc3\xb6" "def",
                               "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                               "<string>abc\xc3\xb6" "def</string>\n"        },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int          LINE            = DATA[i].d_lineNum;
                const bsl::string  VALUE           = DATA[i].d_value;
                const char        *EXPECTED_RESULT = DATA[i].d_result;

                if (veryVerbose) {
                    T_ P_(LINE) P_(VALUE) P(EXPECTED_RESULT)
                }

                bsl::stringstream result;

                EncoderOptions options;
                options.setEncodingStyle(EncodingStyle::BAEXML_PRETTY);

                baexml_Encoder encoder(&options, 0, 0);
                int rc = encoder.encodeToStream(result, VALUE);

                LOOP_ASSERT(rc, 0 == rc);
                LOOP3_ASSERT(LINE, EXPECTED_RESULT,   result.str(),
                                   EXPECTED_RESULT == result.str());
            }
        }

        if (verbose) cout << "\nUsing 'bsl::vector<char>' as parameterized "
                          << "'TYPE' using different formatting modes."
                          << endl;
        {
            enum FormattingMode {
                DEFAULT = bdeat_FormattingMode::BDEAT_DEFAULT,
                BASE64  = bdeat_FormattingMode::BDEAT_BASE64,
                HEX     = bdeat_FormattingMode::BDEAT_HEX,
                DEC     = bdeat_FormattingMode::BDEAT_DEC,
                TEXT    = bdeat_FormattingMode::BDEAT_TEXT,
                IS_LIST = bdeat_FormattingMode::BDEAT_LIST,
                LIST_OR_DEC = IS_LIST | DEC
            };

            const char VALUE[] = "abcd";

            static const struct {
                int             d_lineNum;  // source line number
                FormattingMode  d_mode;     // formatting mode
                const char     *d_result;   // expected result
            } DATA[] = {
                //line  mode       result
                //----  ----       ------
// TBD Uncomment
//                 { L_,   DEFAULT,   "<?xml version=\"1.0\" "
//                                    "encoding=\"UTF-8\" ?>\n"
//                                    "<byte>97</byte>\n"
//                                    "<byte>98</byte>\n"
//                                    "<byte>99</byte>\n"
//                                    "<byte>100</byte>\n" },
                { L_,   BASE64,    "<?xml version=\"1.0\" "
                                   "encoding=\"UTF-8\" ?>\n"
                                   "<base64Binary>YWJjZA==</base64Binary>\n" },
                { L_,   HEX,       "<?xml version=\"1.0\" "
                                   "encoding=\"UTF-8\" ?>\n"
                                   "<hexBinary>61626364</hexBinary>\n"       },
                { L_,   TEXT,      "<?xml version=\"1.0\" "
                                   "encoding=\"UTF-8\" ?>\n"
                                   "<string>abcd</string>\n"                 },
                { L_,   IS_LIST,   "<?xml version=\"1.0\" "
                                   "encoding=\"UTF-8\" ?>\n"
                                   "<list>97 98 99 100</list>\n"             },
                { L_,   LIST_OR_DEC,
                                   "<?xml version=\"1.0\" "
                                   "encoding=\"UTF-8\" ?>\n"
                                   "<list>97 98 99 100</list>\n"             },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int             LINE            = DATA[i].d_lineNum;
                const FormattingMode  FORMATTING_MODE = DATA[i].d_mode;
                const char           *EXPECTED_RESULT = DATA[i].d_result;

                if (veryVerbose) {
                    T_ P_(LINE) P_((int)FORMATTING_MODE) P(EXPECTED_RESULT)
                }

                bsl::stringstream result;

                bsl::vector<char>       OBJECT(VALUE,
                                               VALUE + sizeof(VALUE) - 1);
                baexml_EncoderOptions options;

                if (FORMATTING_MODE & IS_LIST) {
                    options.setTag("list");  // xmlName behavior is undefined
                                             // if formatting mode is IS_LIST
                }

                options.setFormattingMode(FORMATTING_MODE);
                options.setEncodingStyle(EncodingStyle::BAEXML_PRETTY);

                baexml_Encoder encoder(&options, 0, 0);
                int rc = encoder.encodeToStream(result, OBJECT);

                LOOP_ASSERT(rc, 0 == rc);
                LOOP3_ASSERT(LINE, EXPECTED_RESULT,   result.str(),
                                   EXPECTED_RESULT == result.str());
            }
        }

        if (verbose) cout << "\nUsing 'bsl::vector<char>' as parameterized "
                          << "'TYPE' using different formatting modes."
                          << endl;
        {
            enum FormattingMode {
                DEFAULT = bdeat_FormattingMode::BDEAT_DEFAULT,
                BASE64  = bdeat_FormattingMode::BDEAT_BASE64,
                HEX     = bdeat_FormattingMode::BDEAT_HEX,
                DEC     = bdeat_FormattingMode::BDEAT_DEC,
                TEXT    = bdeat_FormattingMode::BDEAT_TEXT,
                IS_LIST = bdeat_FormattingMode::BDEAT_LIST,
                LIST_OR_DEC = IS_LIST | DEC
            };

            const char VALUE[] =
               "\x04\x15'&\xFF\xFF\xB5T\xC0#\xDF\xA4\xAD+\xDC\xE9";

            static const struct {
                int             d_lineNum;  // source line number
                FormattingMode  d_mode;     // formatting mode
                const char     *d_result;   // expected result
            } DATA[] = {
                //line  mode       result
                //----  ----       ------
                { L_,   HEX,       "<?xml version=\"1.0\" "
                                   "encoding=\"UTF-8\" ?>\n"
                    "<hexBinary>04152726FFFFB554C023DFA4AD2BDCE9</hexBinary>\n"
                },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int             LINE            = DATA[i].d_lineNum;
                const FormattingMode  FORMATTING_MODE = DATA[i].d_mode;
                const char           *EXPECTED_RESULT = DATA[i].d_result;

                if (veryVerbose) {
                    T_ P_(LINE) P_((int)FORMATTING_MODE) P(EXPECTED_RESULT)
                }

                bsl::stringstream result;

                bsl::vector<char>       OBJECT(VALUE,
                                               VALUE + sizeof(VALUE) - 1);
                baexml_EncoderOptions options;

                if (FORMATTING_MODE & IS_LIST) {
                    options.setTag("list");  // xmlName behavior is undefined
                                             // if formatting mode is IS_LIST
                }

                options.setFormattingMode(FORMATTING_MODE);
                options.setEncodingStyle(EncodingStyle::BAEXML_PRETTY);

                baexml_Encoder encoder(&options, 0, 0);
                int rc = encoder.encodeToStream(result, OBJECT);

                LOOP_ASSERT(rc, 0 == rc);
                LOOP3_ASSERT(LINE, EXPECTED_RESULT,   result.str(),
                                   EXPECTED_RESULT == result.str());
            }
        }
        if (verbose) cout << "\nEnd of Test." << endl;
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) cout << "\nBREATHING TEST"
                          << "\n==============" << endl;

         baexml_EncoderOptions options;

         options.setObjectNamespace("http://bloomberg.com/schemas/apiy");

         bsl::stringstream result;

         const int OBJECT = 123;

         baexml_Encoder encoder(&options, 0, 0);
         int rc = encoder.encodeToStream(result, OBJECT);

         LOOP_ASSERT(rc, 0 == rc);
         if (veryVerbose) {
             T_ P(result.str())
         }
      } break;
      case -3: {
        bcema_SharedPtr<bdem_Schema> schemaPtr;  schemaPtr.createInplace();
        bdem_Schema&    schema = *schemaPtr;
        bdem_RecordDef *userInfoDef, *base64CollectionDef;

        base64CollectionDef = schema.createRecord("Blob");
        base64CollectionDef->appendField(bdem_ElemType::BDEM_CHAR_ARRAY,
                                         "BlobValue");

        userInfoDef = schema.createRecord("UserInfo");
        userInfoDef->appendField(bdem_ElemType::BDEM_DOUBLE,
                                 "MonthlyValueLimit");
        userInfoDef->appendField(bdem_ElemType::BDEM_LIST,
                                 base64CollectionDef, "TheBlob");

        typedef baexml_Encoder Encoder;

        bsl::cout << "\nTest 1" << bsl::endl;
        {
            bdem_List list;
            bdem_SchemaAggregateUtil::initListDeep(&list, *userInfoDef);
            list.theModifiableDouble(0) = 3.21;
            list.theModifiableList(1).theModifiableCharArray(0).push_back('a');
            list.theModifiableList(1).theModifiableCharArray(0).push_back('b');
            list.theModifiableList(1).theModifiableCharArray(0).push_back('c');
            list.theModifiableList(1).theModifiableCharArray(0).push_back('d');

            bcem_Aggregate aggregate(schemaPtr,
                                     "UserInfo",
                                     bdem_ElemType::BDEM_LIST);
            aggregate.setValue(list);

            P(aggregate);

            baexml_EncoderOptions encoderOptions;
            encoderOptions.setTag("UserInfo");

            baexml_Encoder encoder(&encoderOptions, 0, 0);
            encoder.encodeToStream(bsl::cout, aggregate);
        }
      } break;
#if 0
      case -2: {
        bdem_Schema     schema;
        bdem_RecordDef *userInfoDef, *dateCollectionDef;

        dateCollectionDef = schema.createRecord("DateCollection");
        dateCollectionDef->appendField(bdem_ElemType::BDEM_DATE_ARRAY, "date");

        userInfoDef = schema.createRecord("UserInfo");
        userInfoDef->appendField(bdem_ElemType::BDEM_DOUBLE,
                                 "MonthlyValueLimit");
        userInfoDef->appendField(bdem_ElemType::BDEM_LIST,
                                 dateCollectionDef, "DailyUsageDays");

        typedef baexml_Encoder Encoder;

        bsl::cout << "\nTest 1" << bsl::endl;
        {
            bdem_List list;

            bdem_SchemaAggregateUtil::initListDeep(&list, *userInfoDef);

            list.theDouble(0) = 3.21;
            list.theList(1).theDateArray(0).push_back(bdet_Date(2000, 12, 31));
            list.theList(1).theDateArray(0).push_back(bdet_Date(2000, 5, 31));
            list.theList(1).theDateArray(0).push_back(bdet_Date(2000, 2, 7));
            list.theList(1).theDateArray(0).push_back(bdet_Date(2000, 9, 21));

            P(list);

            bdem_ConstRowBinding binding(&list.row(), userInfoDef);

            bdem_SequenceAdapter<bdem_ConstRowBinding> adapter(&binding);

            baexml_EncoderOptions encoderOptions;

            encoderOptions.setTag("UserInfo");
            baexml_Encoder encoder(&encoderOptions, 0, 0);
            encoder.encodeToStream(bsl::cout, adapter);
        }
      } break;
#endif
      case -1: {
        bcema_SharedPtr<bdem_Schema> schemaPtr;  schemaPtr.createInplace();
        bdem_Schema&    schema = *schemaPtr;
        bdem_RecordDef *pointRecordDef, *circleRecordDef, *polygonRecordDef;

        pointRecordDef = schema.createRecord("PointRecord");
        pointRecordDef->appendField(bdem_ElemType::BDEM_DOUBLE, "x");
        pointRecordDef->appendField(bdem_ElemType::BDEM_DOUBLE, "y");

        circleRecordDef = schema.createRecord("CircleRecord");
        circleRecordDef->appendField(bdem_ElemType::BDEM_LIST,
                                     pointRecordDef, "center");
        circleRecordDef->appendField(bdem_ElemType::BDEM_DOUBLE, "radius");

        polygonRecordDef = schema.createRecord("PolygonRecord");
        polygonRecordDef->appendField(bdem_ElemType::BDEM_TABLE,
                                      pointRecordDef, "vertices");

        bdem_RecordDef *figureDef;

        figureDef = schema.createRecord("FigureChoice",
                                        bdem_RecordDef::BDEM_CHOICE_RECORD);
        figureDef->appendField(bdem_ElemType::BDEM_LIST, polygonRecordDef,
                               "Polygon");
        figureDef->appendField(bdem_ElemType::BDEM_LIST, circleRecordDef,
                               "Circle");
        figureDef->appendField(bdem_ElemType::BDEM_INT,
                               "IntSelection");
        figureDef->appendField(bdem_ElemType::BDEM_DOUBLE,
                               "DoubleSelection");

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

        schema.print(bsl::cout);

        typedef baexml_Encoder Encoder;

        bsl::cout << "\nTest 1" << bsl::endl;
        {
            bdem_List list;
            list.appendDouble(34.21);
            list.appendDouble(5.32);

            bcem_Aggregate aggregate(schemaPtr,
                                     "PointRecord",
                                     bdem_ElemType::BDEM_LIST);
            aggregate.setValue(list);

            P(aggregate);

            baexml_EncoderOptions encoderOptions;
            encoderOptions.setTag("Point");

            baexml_Encoder encoder(&encoderOptions, 0, 0);
            encoder.encodeToStream(bsl::cout, aggregate);
        }

        bsl::cout << "\nTest 2" << bsl::endl;
        {
            bdem_ElemType::Type cols[] = { bdem_ElemType::BDEM_DOUBLE,
                                           bdem_ElemType::BDEM_DOUBLE };

            bdem_Table table(cols, 2);

            bdem_List list;
            list.appendTable(table);
            bdem_Table& theTable = list.theModifiableTable(0);
            theTable.appendNullRows(3);
            bdem_Row& row0 = theTable.theModifiableRow(0);
            bdem_Row& row1 = theTable.theModifiableRow(1);
            bdem_Row& row2 = theTable.theModifiableRow(2);
            row0.theModifiableDouble(0) = 92.23;
            row0.theModifiableDouble(1) = 12.3;
            row1.theModifiableDouble(0) =  3.67;
            row1.theModifiableDouble(1) =  1.23;
            row2.theModifiableDouble(0) =  9.31;
            row2.theModifiableDouble(1) =  2.12;

            bsl::cout << bsl::endl << bsl::endl;

            bcem_Aggregate aggregate(schemaPtr,
                                     "PolygonRecord",
                                     bdem_ElemType::BDEM_LIST);
            aggregate.setValue(list);

            P(aggregate);

            baexml_EncoderOptions encoderOptions;
            encoderOptions.setTag("Polygon");

            baexml_Encoder encoder(&encoderOptions, 0, 0);
            encoder.encodeToStream(bsl::cout, aggregate);
        }

        bsl::cout << "\nTest 3" << bsl::endl;
        {
            bdem_List list;
            list.appendString("Shezan");
            list.appendDouble(97.32);
            list.appendNullChoice();

            bdem_SchemaAggregateUtil::initChoice(&list.theModifiableChoice(2),
                                                 *figureDef);

            list.theModifiableChoice(2).makeSelection(1);
            list.theModifiableChoice(2).theModifiableList().appendNullList();
            list.theModifiableChoice(2).theModifiableList().
                                    theModifiableList(0).appendDouble(8.2);
            list.theModifiableChoice(2).theModifiableList().
                                    theModifiableList(0).appendDouble(29.3);
            list.theModifiableChoice(2).theModifiableList().appendDouble(9.21);

            bsl::cout << bsl::endl << bsl::endl;

            bcem_Aggregate aggregate(schemaPtr,
                                     "PlayerRecord",
                                     bdem_ElemType::BDEM_LIST);
            aggregate.setValue(list);

            P(aggregate);

            baexml_EncoderOptions encoderOptions;
            encoderOptions.setTag("Player");

            baexml_Encoder encoder(&encoderOptions, 0, 0);
            encoder.encodeToStream(bsl::cout, aggregate);
        }

        bsl::cout << "\nTest 4" << bsl::endl;
        {
            bdem_List list;
            list.appendString("Shezan");
            list.appendDouble(3.45);
            list.appendNullChoiceArray();

            bdem_SchemaAggregateUtil::initChoiceArray(
                                             &list.theModifiableChoiceArray(2),
                                             *figureDef);

            bdem_ChoiceArray& choiceArray = list.theModifiableChoiceArray(2);
            choiceArray.appendNullItems(3);
            bdem_ChoiceArrayItem& item0 = choiceArray.theModifiableItem(0);
            bdem_ChoiceArrayItem& item1 = choiceArray.theModifiableItem(1);
            bdem_ChoiceArrayItem& item2 = choiceArray.theModifiableItem(2);
            item0.makeSelection(2);
            item0.theModifiableInt() = 45;
            item1.makeSelection(3);
            item1.theModifiableDouble() = 2.45;
            item2.makeSelection(1);
            item2.theModifiableList().appendNullList();
            item2.theModifiableList().theModifiableList(0).appendDouble(2.12);
            item2.theModifiableList().theModifiableList(0).appendDouble(4.34);
            item2.theModifiableList().appendDouble(92.34);

            bsl::cout << bsl::endl << bsl::endl;

            bcem_Aggregate aggregate(schemaPtr,
                                     "ComplexPlayerRecord",
                                     bdem_ElemType::BDEM_LIST);
            aggregate.setValue(list);

            P(aggregate);

            baexml_EncoderOptions encoderOptions;
            encoderOptions.setTag("ComplexPlayer");

            baexml_Encoder encoder(&encoderOptions, 0, 0);
            encoder.encodeToStream(bsl::cout, aggregate);
        }

        bsl::cout << "\nTest 5" << bsl::endl;
        {
            bdem_List list;
            list.appendString("Shezan");
            list.appendDouble(3.45);
            list.appendNullChoiceArray();

            bdem_SchemaAggregateUtil::initChoiceArray(
                                             &list.theModifiableChoiceArray(2),
                                             *figureDef);
            bdem_ChoiceArray& choiceArray = list.theModifiableChoiceArray(2);
            bdem_ChoiceArrayItem& item0 = choiceArray.theModifiableItem(0);
            bdem_ChoiceArrayItem& item1 = choiceArray.theModifiableItem(1);
            bdem_ChoiceArrayItem& item2 = choiceArray.theModifiableItem(2);
            choiceArray.appendNullItems(3);
            item0.makeSelection(2);
            item0.theModifiableInt() = 45;
            item1.makeSelection(3);
            item1.theModifiableDouble() = 2.45;
            item2.makeSelection(1);
            item2.theModifiableList().appendNullList();
            item2.theModifiableList().theModifiableList(0).appendDouble(2.12);
            item2.theModifiableList().theModifiableList(0).appendDouble(4.34);
            item2.theModifiableList().appendDouble(92.34);

            bsl::cout << bsl::endl << bsl::endl;

            bcem_Aggregate aggregate(schemaPtr,
                                     "ComplexPlayerRecord",
                                     bdem_ElemType::BDEM_LIST);
            aggregate.setValue(list);

            P(aggregate);

            bdesb_MemOutStreamBuf osb;
            Encoder encoder(0, &bsl::cerr, &bsl::cerr);
            int ret = encoder.encode(&osb, aggregate);
            bsl::cout << "Encode ret = " << ret << bsl::endl;

            const char *data = osb.data();
            int         len  = osb.length();

            bdeu_Print::hexDump(bsl::cout, data, len);

            bdesb_FixedMemInStreamBuf isb(data, len);

            bcema_SharedPtr<const bdem_RecordDef> recordDefPtr(
                                                       schemaPtr,
                                                       complexPlayerRecordDef);
            bcem_Aggregate aggregateOut(recordDefPtr);
            baexml_MiniReader reader;
            Decoder decoder(0, &reader, 0, &bsl::cerr, &bsl::cerr);
            ret = decoder.decode(&isb, &aggregateOut);
            bsl::cout << "Decode ret = " << ret << bsl::endl;
            P(aggregate);
        }
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
