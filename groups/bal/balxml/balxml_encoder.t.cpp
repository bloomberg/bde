// balxml_encoder.t.cpp                                               -*-C++-*-

// ----------------------------------------------------------------------------
//                                  ATTENTION
//
// This test driver requires much system memory to build on IBM/AIX using xlC.
// If you get an out of memory error during the compilation of this file please
// search for COMPILER_RESOURCE_LIMITATIONS in this source file, and uncomment
// the '#define' you find, and remove the comment text right above it.  Then
// search for the '#undef' for the same macro and uncomment that, too.  Finally
// remove this comment block as well.
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <balxml_encoder.h>

#include <s_baltst_address.h>
#include <s_baltst_employee.h>
#include <s_baltst_generatetestarray.h>
#include <s_baltst_generatetestchoice.h>
#include <s_baltst_generatetestcustomizedtype.h>
#include <s_baltst_generatetestdynamictype.h>
#include <s_baltst_generatetestenumeration.h>
#include <s_baltst_generatetestnullablevalue.h>
#include <s_baltst_generatetestsequence.h>
#include <s_baltst_generatetesttaggedvalue.h>
#include <s_baltst_mychoice.h>
#include <s_baltst_myenumeration.h>
#include <s_baltst_mysequence.h>
#include <s_baltst_mysequencewithanonymouschoice.h>
#include <s_baltst_mysequencewithattributes.h>
#include <s_baltst_mysequencewithdecimalattribute.h>
#include <s_baltst_mysequencewithdoubleattribute.h>
#include <s_baltst_mysequencewithprecisiondecimalattribute.h>
#include <s_baltst_mysequencewithnillables.h>
#include <s_baltst_mysequencewithnullables.h>
#include <s_baltst_mysimplecontent.h>
#include <s_baltst_mysimpleintcontent.h>
#include <s_baltst_testchoice.h>
#include <s_baltst_testcustomizedtype.h>
#include <s_baltst_testdynamictype.h>
#include <s_baltst_testenumeration.h>
#include <s_baltst_testnilvalue.h>
#include <s_baltst_testplaceholder.h>
#include <s_baltst_testselection.h>
#include <s_baltst_testsequence.h>
#include <s_baltst_testtaggedvalue.h>

#include <bslim_testutil.h>

#include <bdlat_attributeinfo.h>
#include <bdlat_formattingmode.h>
#include <bdlat_selectioninfo.h>
#include <bdlat_typetraits.h>
#include <bdlat_valuetypefunctions.h>
#include <bdlb_chartype.h>
#include <bdlb_nullablevalue.h>
#include <bdlb_variant.h>
#include <bdlb_print.h>
#include <bdlb_printmethods.h>
#include <bdldfp_decimal.h>

#include <bdlsb_memoutstreambuf.h>

#include <bslalg_constructorproxy.h>
#include <bslalg_typetraits.h>

#include <bslma_allocator.h>
#include <bslma_default.h>
#include <bslma_managedptr.h>

#include <bslmf_assert.h>

#include <bsl_algorithm.h>
#include <bsl_cstdio.h>
#include <bsl_cstdlib.h>
#include <bsl_iosfwd.h>
#include <bsl_limits.h>
#include <bsl_iostream.h>
#include <bsl_memory.h>
#include <bsl_sstream.h>
#include <bsl_string.h>
#include <bsl_string_view.h>
#include <bsl_vector.h>

#include <bsls_assert.h>
#include <bsls_buildtarget.h>
#include <bsls_objectbuffer.h>
#include <bsls_platform.h>
#include <bsls_review.h>

using namespace BloombergLP;
using bsl::cout;
using bsl::cerr;
using bsl::endl;
using bsl::flush;
using bsl::string_view;
namespace test = BloombergLP::s_baltst;

// ============================================================================
//                                 TEST PLAN
// ----------------------------------------------------------------------------
//                                  Overview
//                                  --------
// ----------------------------------------------------------------------------
//
// CREATORS
//
// MANIPULATORS
//
// ACCESSORS
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [19] USAGE EXAMPLE
// [18] REPRODUCE SCENARIO FROM DRQS 169438741

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

void printValue(bsl::ostream& out, const string_view& value)
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

// ============================================================================
//                   GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
// ----------------------------------------------------------------------------

typedef balxml::Encoder        Encoder;
typedef balxml::EncoderOptions EncoderOptions;
typedef balxml::EncodingStyle  EncodingStyle;

#define XSI " xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\""

int verbose;
int veryVerbose;
int veryVeryVerbose;
int veryVeryVeryVerbose;

// ----------------------------------------------------------------------------
//                        *End-of-File Block removed*
// ----------------------------------------------------------------------------

// test_mysequencewitharrays.h   -*-C++-*-

//@PURPOSE: todo: provide purpose.
//
//@CLASSES:
//  MySequenceWithArrays: a sequence with arrays
//
//@DESCRIPTION:
//  todo: provide annotation for 'MySequenceWithArrays'

namespace BloombergLP {
namespace s_baltst {

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

    static const bdlat_AttributeInfo ATTRIBUTE_INFO_ARRAY[];
        // attribute information for each attribute

  public:
    // CLASS METHODS
    static const bdlat_AttributeInfo *lookupAttributeInfo(int id);
        // Return attribute information for the attribute indicated by the
        // specified 'id' if the attribute exists, and 0 otherwise.

    static const bdlat_AttributeInfo *lookupAttributeInfo(
                                                    const char *name,
                                                    int         nameLength);
        // Return attribute information for the attribute indicated by the
        // specified 'name' of the specified 'nameLength' if the attribute
        // exists, and 0 otherwise.

    // CREATORS
    explicit MySequenceWithArrays(bslma::Allocator  *basicAllocator = 0);
        // Create an object of type 'MySequenceWithArrays' having the default
        // value.  Optionally specify a 'basicAllocator' used to supply memory.
        // If 'basicAllocator' is 0, the currently installed default allocator
        // is used.

    MySequenceWithArrays(const MySequenceWithArrays&  original,
                         bslma::Allocator            *basicAllocator = 0);
        // Create an object of type 'MySequenceWithArrays' having the value of
        // the specified 'original' object.  Optionally specify a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0,
        // the currently installed default allocator is used.

    ~MySequenceWithArrays();
        // Destroy this object.

    // MANIPULATORS
    MySequenceWithArrays& operator=(const MySequenceWithArrays& rhs);
        // Assign to this object the value of the specified 'rhs' object.

    void reset();
        // Reset this object to the default value (i.e., its value upon default
        // construction).

    template<class MANIPULATOR>
    int manipulateAttributes(MANIPULATOR& manipulator);
        // Invoke the specified 'manipulator' sequentially on the address of
        // each (modifiable) attribute of this object, supplying 'manipulator'
        // with the corresponding attribute information structure until such
        // invocation returns a non-zero value.  Return the value from the last
        // invocation of 'manipulator' (i.e., the invocation that terminated
        // the sequence).

    template<class MANIPULATOR>
    int manipulateAttribute(MANIPULATOR& manipulator, int id);
        // Invoke the specified 'manipulator' on the address of the
        // (modifiable) attribute indicated by the specified 'id', supplying
        // 'manipulator' with the corresponding attribute information
        // structure.  Return the value returned from the invocation of
        // 'manipulator' if 'id' identifies an attribute of this class, and -1
        // otherwise.

    template<class MANIPULATOR>
    int manipulateAttribute(MANIPULATOR&  manipulator,
                            const char   *name,
                            int           nameLength);
        // Invoke the specified 'manipulator' on the address of the
        // (modifiable) attribute indicated by the specified 'name' of the
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
        // Return a reference to the non-modifiable "Attribute1" attribute of
        // this object.

    const bsl::vector<char>& attribute2() const;
        // Return a reference to the non-modifiable "Attribute2" attribute of
        // this object.

    const bsl::vector<char>& attribute3() const;
        // Return a reference to the non-modifiable "Attribute3" attribute of
        // this object.

    const bsl::vector<char>& attribute4() const;
        // Return a reference to the non-modifiable "Attribute4" attribute of
        // this object.

    const bsl::vector<char>& attribute5() const;
        // Return a reference to the non-modifiable "Attribute5" attribute of
        // this object.

    const bsl::vector<char>& attribute6() const;
        // Return a reference to the non-modifiable "Attribute6" attribute of
        // this object.

    const bsl::vector<int>& attribute7() const;
        // Return a reference to the non-modifiable "Attribute7" attribute of
        // this object.

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
    // Format the specified 'rhs' to the specified output 'stream' and return a
    // reference to the modifiable 'stream'.

// ============================================================================
//                        INLINE FUNCTION DEFINITIONS
// ============================================================================

// CREATORS
inline
MySequenceWithArrays::MySequenceWithArrays(bslma::Allocator *basicAllocator)
: d_attribute1(bslma::Default::allocator(basicAllocator))
, d_attribute2(bslma::Default::allocator(basicAllocator))
, d_attribute3(bslma::Default::allocator(basicAllocator))
, d_attribute4(bslma::Default::allocator(basicAllocator))
, d_attribute5(bslma::Default::allocator(basicAllocator))
, d_attribute6()
, d_attribute7()
{
}

inline
MySequenceWithArrays::MySequenceWithArrays(
                                   const MySequenceWithArrays&  original,
                                   bslma::Allocator            *basicAllocator)
: d_attribute1(original.d_attribute1,
               bslma::Default::allocator(basicAllocator))
, d_attribute2(original.d_attribute2,
               bslma::Default::allocator(basicAllocator))
, d_attribute3(original.d_attribute3,
               bslma::Default::allocator(basicAllocator))
, d_attribute4(original.d_attribute4,
               bslma::Default::allocator(basicAllocator))
, d_attribute5(original.d_attribute5,
               bslma::Default::allocator(basicAllocator))
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

inline
void MySequenceWithArrays::reset()
{
    bdlat_ValueTypeFunctions::reset(&d_attribute1);
    bdlat_ValueTypeFunctions::reset(&d_attribute2);
    bdlat_ValueTypeFunctions::reset(&d_attribute3);
    bdlat_ValueTypeFunctions::reset(&d_attribute4);
    bdlat_ValueTypeFunctions::reset(&d_attribute5);
    bdlat_ValueTypeFunctions::reset(&d_attribute6);
    bdlat_ValueTypeFunctions::reset(&d_attribute7);
}

template <class MANIPULATOR>
inline
int MySequenceWithArrays::manipulateAttributes(MANIPULATOR& manipulator)
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

    ret = manipulator(&d_attribute4,
                      ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE4]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = manipulator(&d_attribute5,
                      ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE5]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = manipulator(&d_attribute6,
                      ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE6]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = manipulator(&d_attribute7,
                      ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE7]);
    if (ret) {
        return ret;                                                   // RETURN
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
        return NOT_FOUND;                                             // RETURN
    }
}

template <class MANIPULATOR>
inline
int MySequenceWithArrays::manipulateAttribute(MANIPULATOR&  manipulator,
                                              const char   *name,
                                              int           nameLength)
{
    enum { NOT_FOUND = -1 };

    const bdlat_AttributeInfo *attributeInfo =
           lookupAttributeInfo(name, nameLength);
    if (0 == attributeInfo) {
        return NOT_FOUND;                                             // RETURN
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
template <class ACCESSOR>
inline
int MySequenceWithArrays::accessAttributes(ACCESSOR& accessor) const
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

    ret = accessor(d_attribute4,
                   ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE4]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = accessor(d_attribute5,
                   ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE5]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = accessor(d_attribute6,
                   ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE6]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = accessor(d_attribute7,
                   ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE7]);
    if (ret) {
        return ret;                                                   // RETURN
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
        return NOT_FOUND;                                             // RETURN
    }
}

template <class ACCESSOR>
inline
int MySequenceWithArrays::accessAttribute(ACCESSOR&   accessor,
                                          const char *name,
                                          int         nameLength) const
{
    enum { NOT_FOUND = -1 };

     const bdlat_AttributeInfo *attributeInfo =
           lookupAttributeInfo(name, nameLength);
     if (0 == attributeInfo) {
        return NOT_FOUND;                                             // RETURN
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

}  // close namespace s_baltst

// TRAITS
BDLAT_DECL_SEQUENCE_WITH_ALLOCATOR_TRAITS(test::MySequenceWithArrays)

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

}  // close enterprise namespace

// ----------------------------------------------------------------------------
//                        *End-of-File Block removed*
// ----------------------------------------------------------------------------

// test_mysequencewitharrays.cpp  -*-C++-*-

namespace BloombergLP {
namespace s_baltst {

                               // ---------
                               // CONSTANTS
                               // ---------

const char MySequenceWithArrays::CLASS_NAME[] = "MySequenceWithArrays";
    // the name of this class

const bdlat_AttributeInfo MySequenceWithArrays::ATTRIBUTE_INFO_ARRAY[] = {
    {
        ATTRIBUTE_ID_ATTRIBUTE1,
        "attribute1",                     // name
        sizeof("attribute1") - 1,         // name length
        "todo: provide annotation",// annotation
        bdlat_FormattingMode::e_DEC // formatting mode
    },
    {
        ATTRIBUTE_ID_ATTRIBUTE2,
        "attribute2",                     // name
        sizeof("attribute2") - 1,         // name length
        "todo: provide annotation",// annotation
        bdlat_FormattingMode::e_BASE64 // formatting mode
    },
    {
        ATTRIBUTE_ID_ATTRIBUTE3,
        "attribute3",                     // name
        sizeof("attribute3") - 1,         // name length
        "todo: provide annotation",// annotation
        bdlat_FormattingMode::e_HEX // formatting mode
    },
    {
        ATTRIBUTE_ID_ATTRIBUTE4,
        "attribute4",                     // name
        sizeof("attribute4") - 1,         // name length
        "todo: provide annotation",// annotation
        bdlat_FormattingMode::e_TEXT // formatting mode
    },
    {
        ATTRIBUTE_ID_ATTRIBUTE5,
        "attribute5",                     // name
        sizeof("attribute5") - 1,         // name length
        "todo: provide annotation",// annotation
        bdlat_FormattingMode::e_DEC // formatting mode
    },
    {
        ATTRIBUTE_ID_ATTRIBUTE6,
        "attribute6",                     // name
        sizeof("attribute6") - 1,         // name length
        "todo: provide annotation",// annotation
        bdlat_FormattingMode::e_LIST // formatting mode
    },
    {
        ATTRIBUTE_ID_ATTRIBUTE7,
        "attribute7",                     // name
        sizeof("attribute7") - 1,         // name length
        "todo: provide annotation",// annotation
        bdlat_FormattingMode::e_LIST // formatting mode
    }
};

                               // -------------
                               // CLASS METHODS
                               // -------------

const bdlat_AttributeInfo *MySequenceWithArrays::lookupAttributeInfo(
        const char *name,
        int         nameLength)
{
    switch(nameLength) {
        case 10: {
            if (bdlb::CharType::toUpper(name[0])=='A'
             && bdlb::CharType::toUpper(name[1])=='T'
             && bdlb::CharType::toUpper(name[2])=='T'
             && bdlb::CharType::toUpper(name[3])=='R'
             && bdlb::CharType::toUpper(name[4])=='I'
             && bdlb::CharType::toUpper(name[5])=='B'
             && bdlb::CharType::toUpper(name[6])=='U'
             && bdlb::CharType::toUpper(name[7])=='T'
             && bdlb::CharType::toUpper(name[8])=='E') {
                switch(bdlb::CharType::toUpper(name[9])) {
                    case '1': {
                        return
                            &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE1];
                                                                      // RETURN
                    } break;
                    case '2': {
                        return
                            &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE2];
                                                                      // RETURN
                    } break;
                    case '3': {
                        return
                            &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE3];
                                                                      // RETURN
                    } break;
                    case '4': {
                        return
                            &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE4];
                                                                      // RETURN
                    } break;
                    case '5': {
                        return
                            &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE5];
                                                                      // RETURN
                    } break;
                    case '6': {
                        return
                            &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE6];
                                                                      // RETURN
                    } break;
                    case '7': {
                        return
                            &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE7];
                                                                      // RETURN
                    } break;
                }
            }
        } break;
    }
    return 0;
}

const bdlat_AttributeInfo *MySequenceWithArrays::lookupAttributeInfo(int id)
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
        bdlb::Print::indent(stream, level, spacesPerLevel);
    }

    int levelPlus1 = level + 1;

    if (0 <= spacesPerLevel) {
        // multiline

        stream << "[\n";

        bdlb::Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "Attribute1 = ";
        bdlb::PrintMethods::print(stream, d_attribute1,
                             -levelPlus1, spacesPerLevel);

        bdlb::Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "Attribute2 = ";
        bdlb::PrintMethods::print(stream, d_attribute2,
                             -levelPlus1, spacesPerLevel);

        bdlb::Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "Attribute3 = ";
        bdlb::PrintMethods::print(stream, d_attribute3,
                             -levelPlus1, spacesPerLevel);

        bdlb::Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "Attribute4 = ";
        bdlb::PrintMethods::print(stream, d_attribute4,
                             -levelPlus1, spacesPerLevel);

        bdlb::Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "Attribute5 = ";
        bdlb::PrintMethods::print(stream, d_attribute5,
                             -levelPlus1, spacesPerLevel);

        bdlb::Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "Attribute6 = ";
        bdlb::PrintMethods::print(stream, d_attribute6,
                             -levelPlus1, spacesPerLevel);

        bdlb::Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "Attribute7 = ";
        bdlb::PrintMethods::print(stream, d_attribute7,
                             -levelPlus1, spacesPerLevel);

        bdlb::Print::indent(stream, level, spacesPerLevel);
        stream << "]\n";
    }
    else {
        // single line

        stream << '[';

        stream << ' ';
        stream << "Attribute1 = ";
        bdlb::PrintMethods::print(stream, d_attribute1,
                             -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "Attribute2 = ";
        bdlb::PrintMethods::print(stream, d_attribute2,
                             -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "Attribute3 = ";
        bdlb::PrintMethods::print(stream, d_attribute3,
                             -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "Attribute4 = ";
        bdlb::PrintMethods::print(stream, d_attribute4,
                             -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "Attribute5 = ";
        bdlb::PrintMethods::print(stream, d_attribute5,
                             -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "Attribute6 = ";
        bdlb::PrintMethods::print(stream, d_attribute6,
                             -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "Attribute7 = ";
        bdlb::PrintMethods::print(stream, d_attribute7,
                             -levelPlus1, spacesPerLevel);

        stream << " ]";
    }

    return stream << bsl::flush;
}

}  // close namespace s_baltst
}  // close enterprise namespace

namespace DECIMALTEST {

using namespace BloombergLP;
using test::Address;

class Contractor {

  private:
    bsl::string d_name; // todo: provide annotation
    Address d_homeAddress; // todo: provide annotation
    bdldfp::Decimal64 d_hourlyRate; // todo: provide annotation

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
        ATTRIBUTE_INDEX_HOURLY_RATE = 2
            // index for "HourlyRate" attribute
    };

    enum {
        ATTRIBUTE_ID_NAME = 0,
            // id for "Name" attribute
        ATTRIBUTE_ID_HOME_ADDRESS = 1,
            // id for "HomeAddress" attribute
        ATTRIBUTE_ID_HOURLY_RATE = 2
            // id for "HourlyRate" attribute
    };

  public:
    // CONSTANTS
    static const char CLASS_NAME[];
        // the name of this class (i.e., "Contractor")

    static const bdlat_AttributeInfo ATTRIBUTE_INFO_ARRAY[];
        // attribute information for each attribute

  public:
    // CLASS METHODS
    static const bdlat_AttributeInfo *lookupAttributeInfo(int id);
        // Return attribute information for the attribute indicated by the
        // specified 'id' if the attribute exists, and 0 otherwise.

    static const bdlat_AttributeInfo *lookupAttributeInfo(
                                                    const char *name,
                                                    int         nameLength);
        // Return attribute information for the attribute indicated by the
        // specified 'name' of the specified 'nameLength' if the attribute
        // exists, and 0 otherwise.

    // CREATORS
    explicit Contractor(bslma::Allocator *basicAllocator = 0);
        // Create an object of type 'Contractor' having the default value.
        // Optionally specify a 'basicAllocator' used to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.

    Contractor(const Contractor&  original,
               bslma::Allocator  *basicAllocator = 0);
        // Create an object of type 'Contractor' having the value of the
        // specified 'original' object.  Optionally specify a 'basicAllocator'
        // used to supply memory.  If 'basicAllocator' is 0, the currently
        // installed default allocator is used.

    ~Contractor();
        // Destroy this object.

    // MANIPULATORS
    Contractor& operator=(const Contractor& rhs);
        // Assign to this object the value of the specified 'rhs' object.

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
        // Invoke the specified 'manipulator' on the address of the
        // (modifiable) attribute indicated by the specified 'id', supplying
        // 'manipulator' with the corresponding attribute information
        // structure.  Return the value returned from the invocation of
        // 'manipulator' if 'id' identifies an attribute of this class, and -1
        // otherwise.

    template<class MANIPULATOR>
    int manipulateAttribute(MANIPULATOR&  manipulator,
                            const char   *name,
                            int           nameLength);
        // Invoke the specified 'manipulator' on the address of the
        // (modifiable) attribute indicated by the specified 'name' of the
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

    bdldfp::Decimal64& hourlyRate();
        // Return a reference to the modifiable "HourlyRate" attribute of this
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

    const bdldfp::Decimal64& hourlyRate() const;
        // Return a reference to the non-modifiable "HourlyRate" attribute of
        // this object.

};

// FREE OPERATORS
inline
bool operator==(const Contractor& lhs, const Contractor& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects have
    // the same value, and 'false' otherwise.  Two attribute objects have the
    // same value if each respective attribute has the same value.

inline
bool operator!=(const Contractor& lhs, const Contractor& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects do not
    // have the same value, and 'false' otherwise.  Two attribute objects do
    // not have the same value if one or more respective attributes differ in
    // values.

inline
bsl::ostream& operator<<(bsl::ostream& stream, const Contractor& rhs);
    // Format the specified 'rhs' to the specified output 'stream' and return a
    // reference to the modifiable 'stream'.

// ============================================================================
//                        INLINE FUNCTION DEFINITIONS
// ============================================================================

// CREATORS
inline
Contractor::Contractor(bslma::Allocator *basicAllocator)
: d_name(bslma::Default::allocator(basicAllocator))
, d_homeAddress(bslma::Default::allocator(basicAllocator))
, d_hourlyRate()
{
}

inline
Contractor::Contractor(
    const Contractor&   original,
    bslma::Allocator *basicAllocator)
: d_name(original.d_name, bslma::Default::allocator(basicAllocator))
, d_homeAddress(original.d_homeAddress,
                bslma::Default::allocator(basicAllocator))
, d_hourlyRate(original.d_hourlyRate)
{
}

inline
Contractor::~Contractor()
{
}

// MANIPULATORS
inline
Contractor&
Contractor::operator=(const Contractor& rhs)
{
    if (this != &rhs) {
        d_name = rhs.d_name;
        d_homeAddress = rhs.d_homeAddress;
        d_hourlyRate = rhs.d_hourlyRate;
    }
    return *this;
}

inline
void Contractor::reset()
{
    bdlat_ValueTypeFunctions::reset(&d_name);
    bdlat_ValueTypeFunctions::reset(&d_homeAddress);
    bdlat_ValueTypeFunctions::reset(&d_hourlyRate);
}

template <class MANIPULATOR>
inline
int Contractor::manipulateAttributes(MANIPULATOR& manipulator)
{
    int ret;

    ret = manipulator(&d_name,
                      ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_NAME]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = manipulator(&d_homeAddress,
                      ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_HOME_ADDRESS]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = manipulator(&d_hourlyRate,
                      ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_HOURLY_RATE]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    return ret;
}

template <class MANIPULATOR>
inline
int Contractor::manipulateAttribute(MANIPULATOR& manipulator, int id)
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
      case ATTRIBUTE_ID_HOURLY_RATE: {
        return manipulator(&d_hourlyRate,
                           ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_HOURLY_RATE]);
                                                                      // RETURN
      } break;
      default:
        return NOT_FOUND;                                             // RETURN
    }
}

template <class MANIPULATOR>
inline
int Contractor::manipulateAttribute(MANIPULATOR&  manipulator,
                                  const char   *name,
                                  int           nameLength)
{
    enum { NOT_FOUND = -1 };

    const bdlat_AttributeInfo *attributeInfo =
           lookupAttributeInfo(name, nameLength);
    if (0 == attributeInfo) {
        return NOT_FOUND;                                             // RETURN
    }

    return manipulateAttribute(manipulator, attributeInfo->d_id);
}

inline
bsl::string& Contractor::name()
{
    return d_name;
}

inline
Address& Contractor::homeAddress()
{
    return d_homeAddress;
}

inline
bdldfp::Decimal64& Contractor::hourlyRate()
{
    return d_hourlyRate;
}

// ACCESSORS
template <class ACCESSOR>
inline
int Contractor::accessAttributes(ACCESSOR& accessor) const
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

    ret = accessor(d_hourlyRate,
                   ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_HOURLY_RATE]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    return ret;
}

template <class ACCESSOR>
inline
int Contractor::accessAttribute(ACCESSOR& accessor, int id) const
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
      case ATTRIBUTE_ID_HOURLY_RATE: {
        return accessor(d_hourlyRate,
                        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_HOURLY_RATE]);
                                                                      // RETURN
      } break;
      default:
        return NOT_FOUND;                                             // RETURN
    }
}

template <class ACCESSOR>
inline
int Contractor::accessAttribute(ACCESSOR&   accessor,
                              const char *name,
                              int         nameLength) const
{
    enum { NOT_FOUND = -1 };

     const bdlat_AttributeInfo *attributeInfo =
           lookupAttributeInfo(name, nameLength);
     if (0 == attributeInfo) {
        return NOT_FOUND;                                             // RETURN
     }

     return accessAttribute(accessor, attributeInfo->d_id);
}

inline
const bsl::string& Contractor::name() const
{
    return d_name;
}

inline
const Address& Contractor::homeAddress() const
{
    return d_homeAddress;
}

inline
const bdldfp::Decimal64& Contractor::hourlyRate() const
{
    return d_hourlyRate;
}

                               // ---------
                               // CONSTANTS
                               // ---------

const char Contractor::CLASS_NAME[] = "Contractor";
    // the name of this class

const bdlat_AttributeInfo Contractor::ATTRIBUTE_INFO_ARRAY[] = {
    {
        ATTRIBUTE_ID_NAME,
        "name",                     // name
        sizeof("name") - 1,         // name length
        "todo: provide annotation",// annotation
        bdlat_FormattingMode::e_TEXT // formatting mode
    },
    {
        ATTRIBUTE_ID_HOME_ADDRESS,
        "homeAddress",                     // name
        sizeof("homeAddress") - 1,         // name length
        "todo: provide annotation",// annotation
        bdlat_FormattingMode::e_DEFAULT // formatting mode
    },
    {
        ATTRIBUTE_ID_HOURLY_RATE,
        "hourlyRate",                     // name
        sizeof("hourlyRate") - 1,         // name length
        "todo: provide annotation",// annotation
        bdlat_FormattingMode::e_DEC // formatting mode
    }
};

                               // -------------
                               // CLASS METHODS
                               // -------------

const bdlat_AttributeInfo *Contractor::lookupAttributeInfo(
        const char *name,
        int         nameLength)
{
    switch(nameLength) {
        case 4: {
            if (bdlb::CharType::toUpper(name[0])=='N'
             && bdlb::CharType::toUpper(name[1])=='A'
             && bdlb::CharType::toUpper(name[2])=='M'
             && bdlb::CharType::toUpper(name[3])=='E') {
                return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_NAME];   // RETURN
            }
        } break;
        case 10: {
            if (bdlb::CharType::toUpper(name[0])=='H'
             && bdlb::CharType::toUpper(name[1])=='O'
             && bdlb::CharType::toUpper(name[2])=='U'
             && bdlb::CharType::toUpper(name[3])=='R'
             && bdlb::CharType::toUpper(name[4])=='L'
             && bdlb::CharType::toUpper(name[5])=='Y'
             && bdlb::CharType::toUpper(name[6])=='R'
             && bdlb::CharType::toUpper(name[7])=='A'
             && bdlb::CharType::toUpper(name[8])=='T'
             && bdlb::CharType::toUpper(name[9])=='E') {
                return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_HOURLY_RATE];
                                                                      // RETURN
            }
        } break;
        case 11: {
            if (bdlb::CharType::toUpper(name[0])=='H'
             && bdlb::CharType::toUpper(name[1])=='O'
             && bdlb::CharType::toUpper(name[2])=='M'
             && bdlb::CharType::toUpper(name[3])=='E'
             && bdlb::CharType::toUpper(name[4])=='A'
             && bdlb::CharType::toUpper(name[5])=='D'
             && bdlb::CharType::toUpper(name[6])=='D'
             && bdlb::CharType::toUpper(name[7])=='R'
             && bdlb::CharType::toUpper(name[8])=='E'
             && bdlb::CharType::toUpper(name[9])=='S'
             && bdlb::CharType::toUpper(name[10])=='S') {
                return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_HOME_ADDRESS];
                                                                      // RETURN
            }
        } break;
    }
    return 0;
}

const bdlat_AttributeInfo *Contractor::lookupAttributeInfo(int id)
{
    switch (id) {
      case ATTRIBUTE_ID_NAME:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_NAME];
      case ATTRIBUTE_ID_HOME_ADDRESS:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_HOME_ADDRESS];
      case ATTRIBUTE_ID_HOURLY_RATE:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_HOURLY_RATE];
      default:
        return 0;
    }
}

// FREE OPERATORS
inline
bool operator==(const Contractor& lhs, const Contractor& rhs)
{
    return  lhs.name() == rhs.name()
         && lhs.homeAddress() == rhs.homeAddress()
         && lhs.hourlyRate() == rhs.hourlyRate();
}

inline
bool operator!=(const Contractor& lhs, const Contractor& rhs)
{
    return  lhs.name() != rhs.name()
         || lhs.homeAddress() != rhs.homeAddress()
         || lhs.hourlyRate() != rhs.hourlyRate();
}

inline
bsl::ostream& operator<<(bsl::ostream& stream, const Contractor& rhs)
{
    return rhs.print(stream, 0, -1);
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

bsl::ostream& Contractor::print(
    bsl::ostream& stream,
    int           level,
    int           spacesPerLevel) const
{
    if (level < 0) {
        level = -level;
    }
    else {
        bdlb::Print::indent(stream, level, spacesPerLevel);
    }

    int levelPlus1 = level + 1;

    if (0 <= spacesPerLevel) {
        // multiline

        stream << "[\n";

        bdlb::Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "Name = ";
        bdlb::PrintMethods::print(stream, d_name,
                             -levelPlus1, spacesPerLevel);

        bdlb::Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "HomeAddress = ";
        bdlb::PrintMethods::print(stream, d_homeAddress,
                             -levelPlus1, spacesPerLevel);

        bdlb::Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "Age = ";
        bdlb::PrintMethods::print(stream, d_hourlyRate,
                             -levelPlus1, spacesPerLevel);

        bdlb::Print::indent(stream, level, spacesPerLevel);
        stream << "]\n";
    }
    else {
        // single line

        stream << '[';

        stream << ' ';
        stream << "Name = ";
        bdlb::PrintMethods::print(stream, d_name,
                             -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "HomeAddress = ";
        bdlb::PrintMethods::print(stream, d_homeAddress,
                             -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "HourlyRate = ";
        bdlb::PrintMethods::print(stream, d_hourlyRate,
                             -levelPlus1, spacesPerLevel);

        stream << " ]";
    }

    return stream << bsl::flush;
}

}  // close namespace DECIMALTEST

namespace BloombergLP {

// TRAITS
BDLAT_DECL_SEQUENCE_WITH_ALLOCATOR_TRAITS(DECIMALTEST::Contractor)

}  // close enterprise namespace

// ----------------------------------------------------------------------------
//                        *End-of-File Block removed*
// ----------------------------------------------------------------------------

namespace bsl {

                             // =================
                             // class bsl::vector
                             // =================

// FREE FUNCTIONS
ostream& operator<<(ostream& stream, const vector<int>& object)
{
    stream << "(";

    typedef vector<int>::const_iterator ConstIterator;
    for (ConstIterator it = object.begin(); it != object.end(); ++it) {
        const int& element = *it;
        if (object.begin() == it) {
            stream << element;
        }
        else {
            stream << ", " << element;
        }
    }

    return stream << ")";
}

}  // close namespace bsl

// ============================================================================
//                       BEGIN XML GENERATION APPARATUS
// ----------------------------------------------------------------------------

                            // ====================
                            // class TestXmlElement
                            // ====================

class TestXmlElement {
    // This in-core value-semantic type provides a structural representation of
    // a subset of valid XML.  This class is intended to be used as an
    // intermediate representation for generating (potentially large) XML texts
    // using a set of factory functions.

  public:
    // TYPES
    typedef class bsl::pair<bsl::string, bsl::string>  Attribute;
    typedef class bsl::vector<Attribute>               Attributes;
    typedef class TestXmlElement                       Element;
    typedef class bsl::vector<Element>                 Elements;
    typedef class bdlb::Variant<Elements, bsl::string> Content;

  private:
    // DATA
    bsl::string d_name;        // name of the tag
    Attributes  d_attributes;  // sequence of key-value pairs
    Content     d_content;     // textual or sequence-of-children content

  public:
    // CREATORS
    TestXmlElement()
    : d_name()
    , d_attributes()
    , d_content(Elements())
    {
    }

    explicit TestXmlElement(bslma::Allocator *basicAllocator)
    : d_name(basicAllocator)
    , d_attributes(basicAllocator)
    , d_content(Elements(), basicAllocator)
    {
    }

    TestXmlElement(const string_view&  name,
                   bslma::Allocator   *basicAllocator = 0)
    : d_name(name, basicAllocator)
    , d_attributes(basicAllocator)
    , d_content(Elements(), basicAllocator)
    {
    }

    TestXmlElement(const string_view&  name,
                   const Attributes&   attributes,
                   bslma::Allocator   *basicAllocator = 0)
    : d_name(name, basicAllocator)
    , d_attributes(attributes, basicAllocator)
    , d_content(Elements(), basicAllocator)
    {
    }

    TestXmlElement(const string_view&  name,
                   const Content&      content,
                   bslma::Allocator   *basicAllocator = 0)
    : d_name(name, basicAllocator)
    , d_attributes(basicAllocator)
    , d_content(content, basicAllocator)
    {
    }

    TestXmlElement(const string_view&  name,
                   const Attributes&   attributes,
                   const Content&      content,
                   bslma::Allocator   *basicAllocator = 0)
    : d_name(name, basicAllocator)
    , d_attributes(attributes, basicAllocator)
    , d_content(content, basicAllocator)
    {
    }

    TestXmlElement(const TestXmlElement&  original,
                   bslma::Allocator      *basicAllocator = 0)
    : d_name(original.d_name, basicAllocator)
    , d_attributes(original.d_attributes, basicAllocator)
    , d_content(original.d_content, basicAllocator)
    {
    }

    // MANIPULATORS
    TestXmlElement& operator=(const TestXmlElement& original)
    {
        d_name       = original.d_name;
        d_attributes = original.d_attributes;
        d_content    = original.d_content;
        return *this;
    }

    // ACCESSORS
    bsl::ostream& print(bsl::ostream& stream) const
        // Write the value of this object to the specified output 'stream' in
        // the XML format and return a reference to 'stream'.
    {
        stream << "<" << d_name;

        for (Attributes::const_iterator it = d_attributes.begin();
             it != d_attributes.end();
             ++it) {
            stream << " " << it->first << "='" << it->second << "'";
        }

        switch (d_content.typeIndex()) {
          case 1: {
            const Elements& content = d_content.the<Elements>();

            if (content.empty()) {
                return stream << "/>";                                // RETURN
            }

            stream << ">";

            for (Elements::const_iterator it = content.begin();
                 it != content.end();
                 ++it) {
                it->print(stream);
            }

            return stream << "</" << d_name << ">";                   // RETURN
          } break;
          case 2: {
            const bsl::string& text = d_content.the<bsl::string>();
            return stream << ">" << text << "</" << d_name << ">";    // RETURN
          } break;
        }

        BSLS_ASSERT(false && "Illegal path");
        return stream;
    }
};

// FREE FUNCTIONS
bsl::ostream& operator<<(bsl::ostream& stream, const TestXmlElement& object)
{
    return object.print(stream);
}

                             // ==================
                             // struct TestXmlUtil
                             // ==================

struct TestXmlUtil {
    // This utility 'struct' provides a namespace for a suite of functions that
    // construct 'TestXmlElement' objects.

    // TYPES
    typedef TestXmlElement::Attribute Attribute;
    typedef TestXmlElement::Content   Content;

    // CLASS METHODS
    static bsl::vector<Attribute> generateAttributes(const string_view& key,
                                                     const string_view& value)
    {
        bsl::vector<Attribute> result;
        result.push_back(Attribute(bsl::string(key), bsl::string(value)));
        return result;
    }

    static bsl::vector<Attribute> generateAttributes(const string_view& key0,
                                                     const string_view& value0,
                                                     const string_view& key1,
                                                     const string_view& value1)
    {
        bsl::vector<Attribute> result;
        result.push_back(Attribute(bsl::string(key0), bsl::string(value0)));
        result.push_back(Attribute(bsl::string(key1), bsl::string(value1)));
        return result;
    }

    static Content generateContent(const string_view& content)
    {
        return Content(bsl::string(content));
    }

    static Content generateContent(const bsl::vector<TestXmlElement>& children)
    {
        return Content(children);
    }

    static Content generateContent(const TestXmlElement& element)
    {
        bsl::vector<TestXmlElement> result;
        result.push_back(element);
        return generateContent(result);
    }

    static Content generateContent(const TestXmlElement& element0,
                                   const TestXmlElement& element1)
    {
        bsl::vector<TestXmlElement> result;
        result.push_back(element0);
        result.push_back(element1);
        return generateContent(result);
    }

    static TestXmlElement generateElement(const string_view& name)
    {
        return TestXmlElement(name);
    }

    static TestXmlElement generateElement(const string_view& name,
                                          const string_view& attributeKey,
                                          const string_view& attributeValue)
    {
        return TestXmlElement(
            name, generateAttributes(attributeKey, attributeValue));
    }

    static TestXmlElement generateElement(const string_view& name,
                                          const string_view& attributeKey0,
                                          const string_view& attributeValue0,
                                          const string_view& attributeKey1,
                                          const string_view& attributeValue1)
    {
        return TestXmlElement(name,
                              generateAttributes(attributeKey0,
                                                 attributeValue0,
                                                 attributeKey1,
                                                 attributeValue1));
    }

    static TestXmlElement generateElement(const string_view& name,
                                          const string_view& content)
    {
        return TestXmlElement(name, generateContent(content));
    }

    static TestXmlElement generateElement(const string_view& name,
                                          const TestXmlElement&    child)
    {
        return TestXmlElement(name, generateContent(child));
    }

    static TestXmlElement generateElement(const string_view& name,
                                          const TestXmlElement&    child0,
                                          const TestXmlElement&    child1)
    {
        return TestXmlElement(name, generateContent(child0, child1));
    }

    static TestXmlElement generateElement(const string_view& name,
                                          const string_view& attributeKey,
                                          const string_view& attributeValue,
                                          const TestXmlElement&    child)
    {
        return TestXmlElement(name,
                              generateAttributes(attributeKey, attributeValue),
                              generateContent(child));
    }
};

                          // ========================
                          // class GenerateXmlElement
                          // ========================

class GenerateXmlElement {
    // This in-core value-semantic class provides a function object whose
    // function call operator can be used to generate 'TestXmlElement' objects.

  public:
    // TYPES
    typedef TestXmlUtil Util;

    // CREATORS
    GenerateXmlElement()
    {
    }

    // ACCESSORS
    TestXmlElement operator()(const string_view& name) const
    {
        return Util::generateElement(name);
    }

    TestXmlElement operator()(const string_view& name,
                              const string_view& attributeKey,
                              const string_view& attributeValue) const
    {
        return Util::generateElement(name, attributeKey, attributeValue);
    }

    TestXmlElement operator()(const string_view& name,
                              const string_view& attributeKey0,
                              const string_view& attributeValue0,
                              const string_view& attributeKey1,
                              const string_view& attributeValue1) const
    {
        return Util::generateElement(name,
                                     attributeKey0,
                                     attributeValue0,
                                     attributeKey1,
                                     attributeValue1);
    }

    TestXmlElement operator()(const string_view& name,
                              const string_view& content) const
    {
        return Util::generateElement(name, content);
    }

    TestXmlElement operator()(const string_view& name,
                              const TestXmlElement&    child) const
    {
        return Util::generateElement(name, child);
    }

    TestXmlElement operator()(const string_view& name,
                              const TestXmlElement&    child0,
                              const TestXmlElement&    child1) const
    {
        return Util::generateElement(name, child0, child1);
    }

    TestXmlElement operator()(const string_view& name,
                              const string_view& attributeKey,
                              const string_view& attributeValue,
                              const TestXmlElement&    child) const
    {
        return Util::generateElement(
            name, attributeKey, attributeValue, child);
    }
};

// ============================================================================
//                        END XML GENERATION APPARATUS
// ----------------------------------------------------------------------------

// ============================================================================
//                            BEGIN TEST APPARATUS
// ----------------------------------------------------------------------------

                  // ========================================
                  // class MakeMySequenceWithDecimalAttribute
                  // ========================================

class MakeMySequenceWithDecimalAttribute {
  public:
    // CREATORS
    MakeMySequenceWithDecimalAttribute() {}

    // ACCESSORS
    s_baltst::MySequenceWithDecimalAttribute operator()(double value) const
    {
        s_baltst::MySequenceWithDecimalAttribute result;
        result.attribute1() = value;
        return result;
    }
};

                  // =======================================
                  // class MakeMySequenceWithDoubleAttribute
                  // =======================================

class MakeMySequenceWithDoubleAttribute {
  public:
    // CREATORS
    MakeMySequenceWithDoubleAttribute() {}

    // ACCESSORS
    s_baltst::MySequenceWithDoubleAttribute operator()(double value) const
    {
        s_baltst::MySequenceWithDoubleAttribute result;
        result.attribute1() = value;
        return result;
    }
};

             // =================================================
             // class MakeMySequenceWithPrecisionDecimalAttribute
             // =================================================

class MakeMySequenceWithPrecisionDecimalAttribute {
  public:
    // CREATORS
    MakeMySequenceWithPrecisionDecimalAttribute() {}

    // ACCESSORS
    s_baltst::MySequenceWithPrecisionDecimalAttribute operator()(
                                          const bdldfp::Decimal64& value) const
    {
        s_baltst::MySequenceWithPrecisionDecimalAttribute result;
        result.attribute1() = value;
        return result;
    }
};

                            // ====================
                            // class TestCase17Test
                            // ====================

class TestCase17Test {
  public:
    // CREATORS
    TestCase17Test() {}

    // ACCESSORS
    template <class TYPE>
    void operator()(int                             line,
                    const bdlb::NullableValue<int>& maxDecimalTotalDigits,
                    const bdlb::NullableValue<int>& maxDecimalFractionDigits,
                    const TYPE&                     object,
                    const bsl::string&              expectedResult) const
    {
        balxml::EncoderOptions options;
        options.setTag("tag");
        options.setMaxDecimalTotalDigits(maxDecimalTotalDigits);
        options.setMaxDecimalFractionDigits(maxDecimalFractionDigits);
        options.setOutputXMLHeader(false);
        options.setOutputXSIAlias(false);

        balxml::Encoder encoder(&options);

        bdlsb::MemOutStreamBuf streambuf;

        int rc = encoder.encode(&streambuf, object);
        ASSERTV(line, rc, 0 == rc);
        if (0 != rc) {
            return;                                                   // RETURN
        }

        const bsl::string_view result(streambuf.data(), streambuf.length());

        ASSERTV(line, expectedResult, result, expectedResult == result);
    }

    template <class TYPE>
    void operator()(
                 int                             line,
                 const bdlb::NullableValue<int>& maxDecimalTotalDigits,
                 const bdlb::NullableValue<int>& maxDecimalFractionDigits,
                 const TYPE&                     object,
                 const bsl::string&              expectedResult,
                 const bsl::string&              alternateExpectedResult) const
    {
        balxml::EncoderOptions options;
        options.setTag("tag");
        options.setMaxDecimalTotalDigits(maxDecimalTotalDigits);
        options.setMaxDecimalFractionDigits(maxDecimalFractionDigits);
        options.setOutputXMLHeader(false);
        options.setOutputXSIAlias(false);

        balxml::Encoder encoder(&options);

        bdlsb::MemOutStreamBuf streambuf;

        int rc = encoder.encode(&streambuf, object);
        ASSERTV(line, rc, 0 == rc);
        if (0 != rc) {
            return;                                                   // RETURN
        }

        const bsl::string_view result(streambuf.data(), streambuf.length());
        ASSERTV(line,
                expectedResult,
                alternateExpectedResult,
                result,
                expectedResult == result || alternateExpectedResult == result);
    }
};

                       // ==============================
                       // class TestCase14RowProtocolImp
                       // ==============================

class TestCase14RowProtocol {
  public:
    // CREATORS
    virtual ~TestCase14RowProtocol() {}

    // ACCESSORS
    virtual bslma::ManagedPtr<TestCase14RowProtocol> clone(
                                        bslma::Allocator *allocator) const = 0;
    virtual void runTest() const                                           = 0;
};

                       // ==============================
                       // class TestCase14RowProtocolImp
                       // ==============================

template <class VALUE_TYPE>
class TestCase14RowProtocolImp : public TestCase14RowProtocol {
  public:
    // TYPES
    typedef VALUE_TYPE Value;

    // CLASS DATA
    enum {
        e_ENCODING_FAILS                  = 0,
        e_ENCODING_SUCCEEDS               = 1,
        e_ENCODING_YIELDS_DIFFERENT_VALUE = 2
    };

  private:
    // DATA
    int                             d_line;
    bslalg::ConstructorProxy<Value> d_value;
    int                             d_encodingStatus;
    int                             d_decodingStatus;
    TestXmlElement                    d_xml;

    // NOT IMPLEMENTED
    TestCase14RowProtocolImp(const TestCase14RowProtocolImp&);
    TestCase14RowProtocolImp& operator=(const TestCase14RowProtocolImp&);

  public:
    // CREATORS
    explicit TestCase14RowProtocolImp(int                  line,
                                      const Value&         value,
                                      int                  encodingStatus,
                                      int                  decodingStatus,
                                      const TestXmlElement&  xml,
                                      bslma::Allocator    *basicAllocator = 0)
    : d_line(line)
    , d_value(value, bslma::Default::allocator(basicAllocator))
    , d_encodingStatus(encodingStatus)
    , d_decodingStatus(decodingStatus)
    , d_xml(xml)
    {
    }

    ~TestCase14RowProtocolImp() BSLS_KEYWORD_OVERRIDE {}

    // ACCESSORS
    bslma::ManagedPtr<TestCase14RowProtocol> clone(
                       bslma::Allocator *allocator) const BSLS_KEYWORD_OVERRIDE
    {
        bslma::Allocator *const basicAllocator = bslma::Default::allocator(
                                                                    allocator);

        return bslma::ManagedPtr<TestCase14RowProtocol>(
            new (*basicAllocator) TestCase14RowProtocolImp(d_line,
                                                           d_value.object(),
                                                           d_encodingStatus,
                                                           d_decodingStatus,
                                                           d_xml,
                                                           basicAllocator),
            basicAllocator);
    }

    void runTest() const BSLS_KEYWORD_OVERRIDE
    {
        const int           LINE            = d_line;
        const Value&        VALUE           = d_value.object();
        const int           ENCODING_STATUS = d_encodingStatus;
        const TestXmlElement& XML             = d_xml;

        bdlsb::MemOutStreamBuf expectedXmlStreamBuf;
        bsl::ostream expectedXmlStream(&expectedXmlStreamBuf);
        expectedXmlStream << XML;

        const string_view EXPECTED_OUTPUT(expectedXmlStreamBuf.data(),
                                          expectedXmlStreamBuf.length());

        balxml::EncoderOptions options;
        options.setOutputXMLHeader(false);
        options.setOutputXSIAlias(false);

        bdlsb::MemOutStreamBuf errorStreamBuf;
        bsl::ostream errorStream(&errorStreamBuf);

        bdlsb::MemOutStreamBuf warningStreamBuf;
        bsl::ostream warningStream(&warningStreamBuf);

        balxml::Encoder mX(&options,
                           &errorStream,
                           &warningStream);

        bdlsb::MemOutStreamBuf xmlStreamBuf;

        int rc = mX.encode(&xmlStreamBuf, VALUE);

        const string_view OUTPUT(xmlStreamBuf.data(),
                                 xmlStreamBuf.length());

        const string_view ERRORS(errorStreamBuf.data(),
                                 errorStreamBuf.length());

        const string_view WARNINGS(warningStreamBuf.data(),
                                   warningStreamBuf.length());
        switch (ENCODING_STATUS) {
          case e_ENCODING_FAILS: {
              ASSERTV(LINE, rc, 0 != rc);

              if (verbose && 0 == rc) {
                  P(EXPECTED_OUTPUT);
                  P(OUTPUT);
              }
          } break;
          case e_ENCODING_SUCCEEDS: {
              ASSERTV(LINE, rc, 0 == rc);
              if (verbose && 0 != rc) {
                  P(mX.loggedMessages());
                  P(ERRORS);
                  P(EXPECTED_OUTPUT);
                  P(OUTPUT);

                  if (veryVerbose) {
                      P(WARNINGS);
                  }
              }

              ASSERTV(LINE, EXPECTED_OUTPUT, OUTPUT,
                      EXPECTED_OUTPUT == OUTPUT);
          } break;
          case e_ENCODING_YIELDS_DIFFERENT_VALUE: {
              ASSERTV(LINE, rc, 0 == rc);
              if (verbose && 0 != rc) {
                  P(mX.loggedMessages());
                  P(ERRORS);
                  P(EXPECTED_OUTPUT);
                  P(OUTPUT);

                  if (veryVerbose) {
                      P(WARNINGS);
                  }
              }

              ASSERTV(LINE, EXPECTED_OUTPUT, OUTPUT,
                      EXPECTED_OUTPUT != OUTPUT);
          } break;
        }
    }
};

                            // ===================
                            // class TestCase14Row
                            // ===================

class TestCase14Row {
    // DATA
    bslma::Allocator                         *d_allocator_p;
    bslma::ManagedPtr<TestCase14RowProtocol>  d_imp;

  public:
    // CREATORS
    template <class VALUE_TYPE>
    TestCase14Row(int                  line,
                  const VALUE_TYPE&    value,
                  int                  encodingStatus,
                  int                  decodingStatus,
                  const TestXmlElement&  xml,
                  bslma::Allocator    *basicAllocator = 0)
    : d_allocator_p(bslma::Default::allocator(basicAllocator))
    , d_imp(new (*d_allocator_p)
                TestCase14RowProtocolImp<VALUE_TYPE>(line,
                                                     value,
                                                     encodingStatus,
                                                     decodingStatus,
                                                     xml,
                                                     d_allocator_p),
            d_allocator_p)
    {
    }

    TestCase14Row(const TestCase14Row&  original,
                  bslma::Allocator     *basicAllocator = 0)
    : d_allocator_p(bslma::Default::allocator(basicAllocator))
    , d_imp(original.d_imp->clone(d_allocator_p))
    {
    }

    // MANIPULATORS
    TestCase14Row& operator=(const TestCase14Row& original)
    {
        d_imp = original.d_imp->clone(d_allocator_p);
        return *this;
    }

    // ACCESSORS
    void runTest() const { d_imp->runTest(); }
};

// DATA
extern const char attribute0Name[] = "attribute0";
extern const char attribute1Name[] = "attribute1";

extern const char enumerator0String[] = "enumerator0";
extern const char enumerator1String[] = "enumerator1";

extern const char selection0Name[] = "selection0";
extern const char selection1Name[] = "selection1";

// ============================================================================
//                             END TEST APPARATUS
// ----------------------------------------------------------------------------

// ============================================================================
//                              BEGIN TEST CASES
// ----------------------------------------------------------------------------
// Some test cases have been moved into separate functions to stop AIX xlC from
// trying to optimize them and run out of memory.

void runTestCase17()
{
    //-------------------------------------------------------------------------
    // TEST CASE DOCUMENTATION IS REPEATED HERE SO IT IS WITH THE CODE.  It is
    // indented wrong so it does not have to be reformatted here if it needs a
    // change.  Make sure that anything you change here is also changed in
    // 'main' and vice versa.
    //---+
        // --------------------------------------------------------------------
        // TESTING DECIMAL ATTRIBUTE ENCODING
        //   This case tests that the "MaxDecimalTotalDigits" and
        //   "MaxDecimalFractionDigits" options apply when
        //   encoding attributes of sequence types that are represented by
        //   'double's and have the 'bdlat_FormattingMode::e_DEC' formatting
        //   mode.
        //
        // Concerns:
        //: 1 If neither option is set, decimal-formatted, 'double'-valued
        //:   attributes are encoded in decimal notation with the minimum
        //:   fractional digits necessary to read back the same binary 'double'
        //:   value by a proper reader (round-trip property).
        //:
        //: 2 If neither option is set, decimal-formatted, 'double'-valued
        //:   attributes are encoded as if the unset option
        //:   "MaxDecimalTotalDigits" were 16 if "MaxDecimalFractionDigits" was
        //:   set, *or* "MaxDecimalFractionDigits" were 15 if
        //:   "MaxDecimalTotalDigits" was set.
        //:
        //: 3 Neither option affects the number of digits in the encoding of
        //:   the non-fractional component of such an attribute (i.e. neither
        //:   option affects the number of digits that can appear to the left
        //:   of the decimal point.)
        //:
        //: 4 The encoding of such an attribute always includes at least 1
        //:   fractional digit, even if the values of the options would
        //:   otherwise prohibit it.
        //:
        //: 5 The total number of fractional digits in the encoding of
        //:   such an attribute is exactly equal to the difference between
        //:   "MaxDecimalTotalDigits" and "MaxDecimalFractionDigits", or 1,
        //:   whichever is greater.
        //:
        //: 6 If the value of the fractional component of the attribute can be
        //:   represented in fewer than the number of digits alloted, the
        //:   remaining digits are filled with zeros.
        //:
        //: 7 Neither option affects the encoding of 'double'-valued attributes
        //:   with the 'bdlat_FormattingMode::e_DEFAULT' formatting mode.
        //:
        //: 8 Neither option affects the encoding of 'bdldfp::Decimal64'-valued
        //:   attributes with any formatting mode.
        //:
        // Plan:
        //: 1 Define a set N of numbers that can be exactly represented by
        //:   the IEEE 754-2008 64-bit binary floating-point format, and that
        //:   have different numbers of digits in their decimal representation.
        //:   (Selecting positive and negative powers of 2 works.)
        //:
        //: 2 For each number n in N, enumerate several permutations of values
        //:   for "MaxDecimalTotalDigits" and "MaxDecimalFractionDigits" and
        //:   perform the following:
        //:
        //:   1 Encode an object having a 'double'-valued, decimal-formatted
        //:     attribute with the value n.
        //:
        //:   2 Verify that the encoding of the attribute satisfies the
        //:     constraints defined in the concerns.
        //:
        //:   3 Encode an object having a 'double'-valued, default-formatted
        //:     attribute with the value n.
        //:
        //:   4 Verify that the encoding of the attribute is not affected by
        //:     the value of "MaxDecimalTotalDigits" or
        //:     "MaxDecimalFractionDigits".  (Notice that the encoding of the
        //:     attribute satisfies the constraints defined in the W3C XML
        //:     specification for 'double' values.)
        //:
        //: 3 Define a set P of numbers that can be exactly represented by the
        //:   IEEE 754-2008 64-bit *decimal* floating-point format, and that
        //:   have different numbers of digits in their decimal representation.
        //:   (Selecting positive and negative powers of 10 with different
        //:   amounts of precision works.)
        //:
        //: 4 For each number p in P, enumerate several permutations of
        //:   values for "MaxDecimalTotalDigits" and "MaxDecimalFractionDigits"
        //:   and perform the following:
        //:
        //:   5 Encode an object having a 'bdldfp::Decimal64'-valued,
        //:     default-formatted attribute with the value p.
        //:
        //:   6 Verify that the encoding of the attribute is not affected
        //:     by the value of "MaxDecimalTotalDigits" or
        //:     "MaxDecimalFractionDigits".  (Notice that the encoding of the
        //:     attribute satisfies the constraints defined in the W3C XML
        //:     specification for 'precisionDecimal' values.)
        // --------------------------------------------------------------------

    const MakeMySequenceWithDecimalAttribute          de;  // 'double' DECIMAL
    const MakeMySequenceWithDoubleAttribute           du;  // 'double' DEFAULT
    const MakeMySequenceWithPrecisionDecimalAttribute pd;  // 'bdldfp' PRECDEC

    const TestCase17Test t;

    const bdlb::NullableValue<int> N;

    typedef bsl::numeric_limits<double> Limits;

#define AS(X) "<tag attribute1=\"" X "\"/>"
#define A(X) AS(#X)

#define TLN(maxtotal, maxfractional, objtoencode, expected)                   \
    t(L_, maxtotal, maxfractional, objtoencode, A(expected))
    // (T)est (L)i(N)e

#define TLL(maxtotal, maxfractional, objtoencode, expected)                   \
    t(L_, maxtotal, maxfractional, objtoencode, AS(expected))
    // (T)est (L)ine (L)ong -- 'expected' is already a string literal

#define TLA(maxtotal, maxfractional, objtoencode, expected, altexpected)      \
    t(L_, maxtotal, maxfractional, objtoencode, A(expected), A(altexpected))
    // (T)est (L)ine with (A)lternative expected result

//   MAX DECIMAL TOTAL DIGITS  --------.   If both are N we use the 'bslalg::'
//  .------------------------           |-- 'NumericFormatterUtil::toChar' in
//  |     MAX DECIMAL FRACTION DIGITS -'   minimal form (shortest round trip).
//  |    .---------------------------
//  |   /  -=: OBJECT TO ENCODE :=-      --==:: EXPECTED XML OUTPUT ::==--
//- -- -- --------------------------- ---------------------------------------
if (veryVerbose) cout << "Testing 'double' decimal format\n";
TLN( N, N, de(1.52587890625e-05     ),                   0.0000152587890625);
TLN( N, N, de(3.0517578125e-05      ),                   0.000030517578125 );
TLN( N, N, de(6.103515625e-05       ),                   0.00006103515625  );
TLN( N, N, de(1.220703125e-04       ),                   0.0001220703125   );
TLN( N, N, de(2.44140625e-04        ),                   0.000244140625    );
TLN( N, N, de(4.8828125e-04         ),                   0.00048828125     );
TLN( N, N, de(9.765625e-04          ),                   0.0009765625      );
TLN( N, N, de(1.953125e-03          ),                   0.001953125       );
TLN( N, N, de(3.90625e-03           ),                   0.00390625        );
TLN( N, N, de(7.8125e-03            ),                   0.0078125         );
TLN( N, N, de(1.5625e-02            ),                   0.015625          );
TLN( N, N, de(3.125e-02             ),                   0.03125           );
TLN( N, N, de(6.25e-02              ),                   0.0625            );
TLN( N, N, de(1.25e-01              ),                   0.125             );
TLN( N, N, de(2.5e-1                ),                   0.25              );
TLN( N, N, de(5e-1                  ),                   0.5               );
TLN( N, N, de(                   1.0),                   1                 );
TLN( N, N, de(                   8.0),                   8                 );
TLN( N, N, de(                  64.0),                  64                 );
TLN( N, N, de(                 128.0),                 128                 );
TLN( N, N, de(                1024.0),                1024                 );
TLN( N, N, de(               16384.0),               16384                 );
TLN( N, N, de(              131072.0),              131072                 );
TLN( N, N, de(             1048576.0),             1048576                 );
TLN( N, N, de(            16777216.0),            16777216                 );
TLN( N, N, de(           134217728.0),           134217728                 );
TLN( N, N, de(          1073741824.0),          1073741824                 );
TLN( N, N, de(         17179869184.0),         17179869184                 );
TLN( N, N, de(        137438953472.0),        137438953472                 );
TLN( N, N, de(       1099511627776.0),       1099511627776                 );
TLN( N, N, de(      17592186044416.0),      17592186044416                 );
TLN( N, N, de(     140737488355328.0),     140737488355328                 );
TLN( N, N, de(    1125899906842624.0),    1125899906842624                 );
TLN( N, N, de(   18014398509481984.0),   18014398509481984                 );
TLN( N, N, de(  144115188075855870.0),  144115188075855872                 );

TLN( N, 0, de(1.52587890625e-05     ),                   0.0               );
TLN( N, 0, de(3.0517578125e-05      ),                   0.0               );
TLN( N, 0, de(6.103515625e-05       ),                   0.0               );
TLN( N, 0, de(1.220703125e-04       ),                   0.0               );
TLN( N, 0, de(2.44140625e-04        ),                   0.0               );
TLN( N, 0, de(4.8828125e-04         ),                   0.0               );
TLN( N, 0, de(9.765625e-04          ),                   0.0               );
TLN( N, 0, de(1.953125e-03          ),                   0.0               );
TLN( N, 0, de(3.90625e-03           ),                   0.0               );
TLN( N, 0, de(7.8125e-03            ),                   0.0               );
TLN( N, 0, de(1.5625e-02            ),                   0.0               );
TLN( N, 0, de(3.125e-02             ),                   0.0               );
TLN( N, 0, de(6.25e-02              ),                   0.1               );
TLN( N, 0, de(1.25e-01              ),                   0.1               );
TLA( N, 0, de(2.5e-1                ),                   0.2
                                     ,                   0.3               );
TLN( N, 0, de(5e-1                  ),                   0.5               );
TLN( N, 0, de(                   1.0),                   1.0               );
TLN( N, 0, de(                   8.0),                   8.0               );
TLN( N, 0, de(                  64.0),                  64.0               );
TLN( N, 0, de(                 128.0),                 128.0               );
TLN( N, 0, de(                1024.0),                1024.0               );
TLN( N, 0, de(               16384.0),               16384.0               );
TLN( N, 0, de(              131072.0),              131072.0               );
TLN( N, 0, de(             1048576.0),             1048576.0               );
TLN( N, 0, de(            16777216.0),            16777216.0               );
TLN( N, 0, de(           134217728.0),           134217728.0               );
TLN( N, 0, de(          1073741824.0),          1073741824.0               );
TLN( N, 0, de(         17179869184.0),         17179869184.0               );
TLN( N, 0, de(        137438953472.0),        137438953472.0               );
TLN( N, 0, de(       1099511627776.0),       1099511627776.0               );
TLN( N, 0, de(      17592186044416.0),      17592186044416.0               );
TLN( N, 0, de(     140737488355328.0),     140737488355328.0               );
TLN( N, 0, de(    1125899906842624.0),    1125899906842624.0               );
TLN( N, 0, de(   18014398509481984.0),   18014398509481984.0               );
TLN( N, 0, de(  144115188075855870.0),  144115188075855872.0               );

// {DRQS 165472076} regression and other limit values
TLL( N, N, de(Limits::max()),
    "179769313486231570814527423731704356798070567525844996598917"       //  60
    "476803157260780028538760589558632766878171540458953514382464"       // 120
    "234321326889464182768467546703537516986049910576551282076245"       // 180
    "490090389328944075868508455133942304583236903222948165808559"       // 240
    "332123348274797826204144723168738177180919299881250404026184"       // 300
    "124858368");                                                        // 309

#ifndef BSLS_PLATFORM_OS_AIX
TLL( N, 0, de(Limits::max()),
    "179769313486231570814527423731704356798070567525844996598917"       //  60
    "476803157260780028538760589558632766878171540458953514382464"       // 120
    "234321326889464182768467546703537516986049910576551282076245"       // 180
    "490090389328944075868508455133942304583236903222948165808559"       // 240
    "332123348274797826204144723168738177180919299881250404026184"       // 300
    "124858368.0");                                                      // 311
#else
    // AIX 'sprintf' is "lazy"
TLL(N, 0, de(Limits::max()),
    "179769313486231570814527423731704356800000000000000000000000"       //  60
    "000000000000000000000000000000000000000000000000000000000000"       // 120
    "000000000000000000000000000000000000000000000000000000000000"       // 180
    "000000000000000000000000000000000000000000000000000000000000"       // 240
    "000000000000000000000000000000000000000000000000000000000000"       // 300
    "000000000.0");                                                      // 311
#endif

#ifndef BSLS_PLATFORM_OS_AIX
TLL(327, 17, de(Limits::max()),
    "179769313486231570814527423731704356798070567525844996598917"       //  60
    "476803157260780028538760589558632766878171540458953514382464"       // 120
    "234321326889464182768467546703537516986049910576551282076245"       // 180
    "490090389328944075868508455133942304583236903222948165808559"       // 240
    "332123348274797826204144723168738177180919299881250404026184"       // 300
    "124858368.00000000000000000");                                      // 327
#else
    // AIX 'sprintf' is "lazy"
TLL(327, 17, de(Limits::max()),
    "179769313486231570814527423731704356800000000000000000000000"       //  60
    "000000000000000000000000000000000000000000000000000000000000"       // 120
    "000000000000000000000000000000000000000000000000000000000000"       // 180
    "000000000000000000000000000000000000000000000000000000000000"       // 240
    "000000000000000000000000000000000000000000000000000000000000"       // 300
    "000000000.00000000000000000");                                      // 327
#endif

TLL(N, N, de(Limits::min()), "0."                                    //     + 2
    "000000000000000000000000000000000000000000000000000000000000"   //  60 + 2
    "000000000000000000000000000000000000000000000000000000000000"   // 120 + 2
    "000000000000000000000000000000000000000000000000000000000000"   // 180 + 2
    "000000000000000000000000000000000000000000000000000000000000"   // 240 + 2
    "000000000000000000000000000000000000000000000000000000000000"   // 300 + 2
    "000000022250738585072014");                                     // 324 + 2

TLL(325, 324, de(Limits::min()), "0."                                //     + 2
    "000000000000000000000000000000000000000000000000000000000000"   //  60 + 2
    "000000000000000000000000000000000000000000000000000000000000"   // 120 + 2
    "000000000000000000000000000000000000000000000000000000000000"   // 180 + 2
    "000000000000000000000000000000000000000000000000000000000000"   // 240 + 2
    "000000000000000000000000000000000000000000000000000000000000"   // 300 + 2
    "000000022250738585072014");                                     // 324 + 2

TLN( N, 0, de(Limits::min()), 0.0);

TLL( N, N, de(-Limits::max()), "-"                                   //     + 1
    "179769313486231570814527423731704356798070567525844996598917"   //  60 + 1
    "476803157260780028538760589558632766878171540458953514382464"   // 120 + 1
    "234321326889464182768467546703537516986049910576551282076245"   // 180 + 1
    "490090389328944075868508455133942304583236903222948165808559"   // 240 + 1
    "332123348274797826204144723168738177180919299881250404026184"   // 300 + 1
    "124858368");                                                    // 309 + 1

#ifndef BSLS_PLATFORM_OS_AIX
TLL(N, 0, de(-Limits::max()), "-"                                    //     + 1
    "179769313486231570814527423731704356798070567525844996598917"   //  60 + 1
    "476803157260780028538760589558632766878171540458953514382464"   // 120 + 1
    "234321326889464182768467546703537516986049910576551282076245"   // 180 + 1
    "490090389328944075868508455133942304583236903222948165808559"   // 240 + 1
    "332123348274797826204144723168738177180919299881250404026184"   // 300 + 1
    "124858368.0");                                                  // 311 + 1
#else
    // AIX 'sprintf' is "lazy"
TLL(N, 0, de(-Limits::max()), "-"                                    //     + 1
    "179769313486231570814527423731704356800000000000000000000000"   //  60 + 1
    "000000000000000000000000000000000000000000000000000000000000"   // 120 + 1
    "000000000000000000000000000000000000000000000000000000000000"   // 180 + 1
    "000000000000000000000000000000000000000000000000000000000000"   // 240 + 1
    "000000000000000000000000000000000000000000000000000000000000"   // 300 + 1
    "000000000.0");                                                  // 311 + 1
#endif

#ifndef BSLS_PLATFORM_OS_AIX
TLL(326, 17, de(-Limits::max()), "-"                                 //     + 1
    "179769313486231570814527423731704356798070567525844996598917"   //  60 + 1
    "476803157260780028538760589558632766878171540458953514382464"   // 120 + 1
    "234321326889464182768467546703537516986049910576551282076245"   // 180 + 1
    "490090389328944075868508455133942304583236903222948165808559"   // 240 + 1
    "332123348274797826204144723168738177180919299881250404026184"   // 300 + 1
    "124858368.00000000000000000");                                  // 327 + 1
#else
    // AIX 'sprintf' is "lazy"
TLL(326, 17, de(-Limits::max()), "-"                                 //     + 1
    "179769313486231570814527423731704356800000000000000000000000"   //  60 + 1
    "000000000000000000000000000000000000000000000000000000000000"   // 120 + 1
    "000000000000000000000000000000000000000000000000000000000000"   // 180 + 1
    "000000000000000000000000000000000000000000000000000000000000"   // 240 + 1
    "000000000000000000000000000000000000000000000000000000000000"   // 300 + 1
    "000000000.00000000000000000");                                  // 327 + 1
#endif

TLL(N, N, de(-Limits::min()), "-0."                                  //     + 3
    "000000000000000000000000000000000000000000000000000000000000"   //  60 + 3
    "000000000000000000000000000000000000000000000000000000000000"   // 120 + 3
    "000000000000000000000000000000000000000000000000000000000000"   // 180 + 3
    "000000000000000000000000000000000000000000000000000000000000"   // 240 + 3
    "000000000000000000000000000000000000000000000000000000000000"   // 300 + 3
    "000000022250738585072014");                                     // 324 + 3

TLL(325, 324, de(-Limits::min()), "-0."                              //     + 3
    "000000000000000000000000000000000000000000000000000000000000"   //  60 + 3
    "000000000000000000000000000000000000000000000000000000000000"   // 120 + 3
    "000000000000000000000000000000000000000000000000000000000000"   // 180 + 3
    "000000000000000000000000000000000000000000000000000000000000"   // 240 + 3
    "000000000000000000000000000000000000000000000000000000000000"   // 300 + 3
    "000000022250738585072014");                                     // 324 + 3

TLN( N, 1, de(1.52587890625e-05     ),                   0.0               );
TLN( N, 1, de(3.0517578125e-05      ),                   0.0               );
TLN( N, 1, de(6.103515625e-05       ),                   0.0               );
TLN( N, 1, de(1.220703125e-04       ),                   0.0               );
TLN( N, 1, de(2.44140625e-04        ),                   0.0               );
TLN( N, 1, de(4.8828125e-04         ),                   0.0               );
TLN( N, 1, de(9.765625e-04          ),                   0.0               );
TLN( N, 1, de(1.953125e-03          ),                   0.0               );
TLN( N, 1, de(3.90625e-03           ),                   0.0               );
TLN( N, 1, de(7.8125e-03            ),                   0.0               );
TLN( N, 1, de(1.5625e-02            ),                   0.0               );
TLN( N, 1, de(3.125e-02             ),                   0.0               );
TLN( N, 1, de(6.25e-02              ),                   0.1               );
TLN( N, 1, de(1.25e-01              ),                   0.1               );
TLA( N, 1, de(2.5e-1                ),                   0.2
                                     ,                   0.3               );
TLN( N, 1, de(5e-1                  ),                   0.5               );
TLN( N, 1, de(                   1.0),                   1.0               );
TLN( N, 1, de(                   8.0),                   8.0               );
TLN( N, 1, de(                  64.0),                  64.0               );
TLN( N, 1, de(                 128.0),                 128.0               );
TLN( N, 1, de(                1024.0),                1024.0               );
TLN( N, 1, de(               16384.0),               16384.0               );
TLN( N, 1, de(              131072.0),              131072.0               );
TLN( N, 1, de(             1048576.0),             1048576.0               );
TLN( N, 1, de(            16777216.0),            16777216.0               );
TLN( N, 1, de(           134217728.0),           134217728.0               );
TLN( N, 1, de(          1073741824.0),          1073741824.0               );
TLN( N, 1, de(         17179869184.0),         17179869184.0               );
TLN( N, 1, de(        137438953472.0),        137438953472.0               );
TLN( N, 1, de(       1099511627776.0),       1099511627776.0               );
TLN( N, 1, de(      17592186044416.0),      17592186044416.0               );
TLN( N, 1, de(     140737488355328.0),     140737488355328.0               );
TLN( N, 1, de(    1125899906842624.0),    1125899906842624.0               );
TLN( N, 1, de(   18014398509481984.0),   18014398509481984.0               );
TLN( N, 1, de(  144115188075855870.0),  144115188075855872.0               );

TLN(12, 8, de(1.52587890625e-05     ),                   0.00001526        );
TLN(12, 8, de(3.0517578125e-05      ),                   0.00003052        );
TLN(12, 8, de(6.103515625e-05       ),                   0.00006104        );
TLN(12, 8, de(1.220703125e-04       ),                   0.00012207        );
TLN(12, 8, de(2.44140625e-04        ),                   0.00024414        );
TLN(12, 8, de(4.8828125e-04         ),                   0.00048828        );
TLN(12, 8, de(9.765625e-04          ),                   0.00097656        );
TLA(12, 8, de(1.953125e-03          ),                   0.00195312
                                     ,                   0.00195313        );
TLN(12, 8, de(3.90625e-03           ),                   0.00390625        );
TLN(12, 8, de(7.8125e-03            ),                   0.00781250        );
TLN(12, 8, de(1.5625e-02            ),                   0.01562500        );
TLN(12, 8, de(3.125e-02             ),                   0.03125000        );
TLN(12, 8, de(6.25e-02              ),                   0.06250000        );
TLN(12, 8, de(1.25e-01              ),                   0.12500000        );
TLN(12, 8, de(2.5e-1                ),                   0.25000000        );
TLN(12, 8, de(5e-1                  ),                   0.50000000        );
TLN(12, 8, de(                   1.0),                   1.00000000        );
TLN(12, 8, de(                   8.0),                   8.00000000        );
TLN(12, 8, de(                  64.0),                  64.00000000        );
TLN(12, 8, de(                 128.0),                 128.00000000        );
TLN(12, 8, de(                1024.0),                1024.00000000        );
TLN(12, 8, de(               16384.0),               16384.0000000         );
TLN(12, 8, de(              131072.0),              131072.000000          );
TLN(12, 8, de(             1048576.0),             1048576.00000           );
TLN(12, 8, de(            16777216.0),            16777216.0000            );
TLN(12, 8, de(           134217728.0),           134217728.000             );
TLN(12, 8, de(          1073741824.0),          1073741824.00              );
TLN(12, 8, de(         17179869184.0),         17179869184.0               );
TLN(12, 8, de(        137438953472.0),        137438953472.0               );
TLN(12, 8, de(       1099511627776.0),       1099511627776.0               );
TLN(12, 8, de(      17592186044416.0),      17592186044416.0               );
TLN(12, 8, de(     140737488355328.0),     140737488355328.0               );
TLN(12, 8, de(    1125899906842624.0),    1125899906842624.0               );
TLN(12, 8, de(   18014398509481984.0),   18014398509481984.0               );
TLN(12, 8, de(  144115188075855870.0),  144115188075855872.0               );

if (veryVerbose) cout << "Testing 'double' default format\n";
{
static bdlb::NullableValue<int> TOTAL_DIGITS_DATA[] = { N, 6, 12 };
const int NUM_TOTAL_DIGITS =
                          sizeof TOTAL_DIGITS_DATA / sizeof *TOTAL_DIGITS_DATA;

static bdlb::NullableValue<int> FRACTION_DIGITS_DATA[] = { N, 0, 1, 8 };
const int NUM_FRACTION_DIGITS =
                    sizeof FRACTION_DIGITS_DATA / sizeof *FRACTION_DIGITS_DATA;

for (int j = 0; j < NUM_TOTAL_DIGITS; ++j) {
    for (int k = 0; k < NUM_FRACTION_DIGITS; ++k) {
        const bdlb::NullableValue<int> TOTAL = TOTAL_DIGITS_DATA[j];
        const bdlb::NullableValue<int> FRACT = FRACTION_DIGITS_DATA[k];

        // Verify that decimal format options are *ignored* by the default
        // format printing.  Passing decimal encoding options should not change
        // the resulting XML.

        TLN( TOTAL, FRACT, du(1.1920928955078125e-07), 1.1920928955078125e-07);
        TLN( TOTAL, FRACT, du(2.384185791015625e-07 ), 2.384185791015625e-07 );
        TLN( TOTAL, FRACT, du(1.52587890625e-05     ), 1.52587890625e-05     );
        TLN( TOTAL, FRACT, du(2.44140625e-04        ), 0.000244140625        );
        TLN( TOTAL, FRACT, du(3.90625e-03           ), 0.00390625            );
        TLN( TOTAL, FRACT, du(6.25e-02              ), 0.0625                );
        TLN( TOTAL, FRACT, du(5e-1                  ), 0.5                   );
        TLN( TOTAL, FRACT, du(                   1.0),                 1     );
        TLN( TOTAL, FRACT, du(                1024.0),              1024     );
        TLN( TOTAL, FRACT, du(            16777216.0),          16777216     );
        TLN( TOTAL, FRACT, du(        137438953472.0),      137438953472     );
        TLN( TOTAL, FRACT, du(    1125899906842624.0),  1125899906842624     );
        TLN( TOTAL, FRACT, du(   18014398509481984.0), 18014398509481984     );
        TLN( TOTAL, FRACT, du(  144115188075855870.0), 144115188075855872    );
    }
}}  // end 'double' default format testing

if (veryVerbose) cout << "Testing 'Decimal64' precision-decimal format\n";
{
static bdlb::NullableValue<int> TOTAL_DIGITS_DATA[] = { N, 3 };
const int NUM_TOTAL_DIGITS =
                          sizeof TOTAL_DIGITS_DATA / sizeof *TOTAL_DIGITS_DATA;

static bdlb::NullableValue<int> FRACTION_DIGITS_DATA[] = { N, 0, 1, 2 };
const int NUM_FRACTION_DIGITS =
                    sizeof FRACTION_DIGITS_DATA / sizeof *FRACTION_DIGITS_DATA;

for (int j = 0; j < NUM_TOTAL_DIGITS; ++j) {
    for (int k = 0; k < NUM_FRACTION_DIGITS; ++k) {
        const bdlb::NullableValue<int> TOTAL = TOTAL_DIGITS_DATA[j];
        const bdlb::NullableValue<int> FRACT = FRACTION_DIGITS_DATA[k];

        // Verify that decimal format options are *ignored* by the precision-
        // decimal format printing, it always prints using the precision stored
        // in the 'bdldfp::Decimal64'.  Passing decimal encoding options should
        // not change the resulting XML.

#define DD BDLDFP_DECIMAL_DD
            TLN(TOTAL, FRACT, pd(DD(0.001)), 0.001);
            TLN(TOTAL, FRACT, pd(DD(0.01 )), 0.01 );
            TLN(TOTAL, FRACT, pd(DD(0.1  )), 0.1  );
            TLN(TOTAL, FRACT, pd(DD(1.   )), 1    );
            TLN(TOTAL, FRACT, pd(DD(1.0  )), 1.0  );
            TLN(TOTAL, FRACT, pd(DD(1.00 )), 1.00 );
            TLN(TOTAL, FRACT, pd(DD(1.000)), 1.000);
#undef DD
    }
}}  // end 'Decmal64' precision-default format testing

#undef TLA
#undef TLL
#undef TLN
#undef A
#undef AS
}

#if defined(BSLS_PLATFORM_CMP_GNU) &&                                         \
    defined(BDE_BUILD_TARGET_OPT) && defined(BDE_BUILD_TARGET_DBG)
// g++ gives an unhelpful warning (note) for this function about not being able
// to track all variables for the debug info, probably due to loop unrolling
__attribute__((optimize("no-var-tracking-assignments")))
#endif
void runTestCase14()
{
    //-------------------------------------------------------------------------
    // TEST CASE DOCUMENTATION IS REPEATED HERE SO IT IS WITH THE CODE.  It is
    // indented wrong so it does not have to be reformatted here if it needs a
    // change.  Make sure that anything you change here is also changed in
    // 'main' and vice versa.
    //---+
        // --------------------------------------------------------------------
        // TESTING NILLABLE ELEMENT ENCODING
        //   This case tests the 'balxml::Encoder::encode' operation when
        //   encoding objects that may or may not be "nullable", and may or may
        //   not have the "nillable" formatting mode applied.  In general, it
        //   is expected that null values encode to nothing in XML, without
        //   regard to the value of their "nillable" formatting mode.
        //
        // Concerns:
        //: 1 Attributes of sequence types and selections of choice types
        //:   that do not have the "nillable" formatting mode and are not null
        //:   encode to XML representations of their value.
        //:
        //: 2 Attributes of sequence types and selections of choice types that
        //:   do not have the "nillable" formatting mode and are null do not
        //:   appear in the XML representations of their value.
        //:
        //: 3 Attributes of sequence types and selections of choice types
        //:   that have the "nillable" formatting mode and are not null
        //:   encode to XML representations of their value.
        //:
        //: 4 Attributes of sequence types and selections of choice types
        //:   that have the "nillable" formatting mode and are null do not
        //:   appear in the XML representations of their value.
        //:
        //: 5 Objects of nullable types that do not have the null value encode
        //:   to XML representations of the underlying value.
        //:
        //: 6 Objects of nullable types that have the null value do not appear
        //:   in the XML representations of their value.
        //:
        //: 7 The above 6 properties hold for arbitrary nesting and
        //:   permutation of 'bdlat' concept implementations.  For example,
        //:   these properties should not only hold for a sequence of two
        //:   integers, but also a sequence of an enumeration and a choice
        //:   between an integer and a string.
        //
        // Plan:
        //: 1 Create objects enumerating all 8 'bdlat' attribute type concepts,
        //:   and where applicable, recursively up to a depth of 2.
        //:
        //: 2 For each of the above objects, create a variant that is non-null
        //:   but not nillable, a variant that is null but not nillable, a
        //:   variant that is non-null and nillable, and a variant that is null
        //:   and nillable.
        //:
        //: 3 For each of the above objects, verify that their XML encodings
        //:   created by 'balxml::Encoder::encode' satisfy the 6 properties
        //:   defined in the "Concerns".
        //
        // Testing:
        //   int encode(bsl::streambuf *buffer, const TYPE& object);
        // --------------------------------------------------------------------

    // Abbreviations for the names of 'bdlat' concept test implementations,
    // which will become the tag names of the XML they generate.
    const string_view S  = "MySequence";
    const string_view C  = "MyChoice";
    const string_view CT = "MyCustomizedType";
    const string_view D  = "MyDynamicType";
    const string_view E  = "MyEnumeration";

    // Abbreviations for attribute and selection names.
    const string_view A0 = "attribute0";
    const string_view A1 = "attribute1";
    const string_view E0 = "enumerator0";
    const string_view E1 = "enumerator1";
    const string_view S0 = "selection0";
    const string_view S1 = "selection1";

    // Abbreviations for some test values.
    const int i0 = 0;
    const int i1 = 1;
    const double d0 = 1.5;

    // Abbreviations for XML-encoded representations of some test values.
    const string_view V0 = "0";
    const string_view V1 = "1";
    const string_view D0 = "1.5";

    // Abbreviations for function objects used to generate objects that
    // implement various 'bdlat' attribute type concepts.
    const s_baltst::GenerateTestArray          a;
    const s_baltst::GenerateTestChoice         c;
    const s_baltst::GenerateTestCustomizedType ct;
    const s_baltst::GenerateTestDynamicType    d;
    const s_baltst::GenerateTestEnumeration    e;
    const s_baltst::GenerateTestNullableValue  n;
    const s_baltst::GenerateTestSequence       s;

    // Abbreviations for some sequence attributes.
    typedef s_baltst::TestAttribute<0, attribute0Name> Attribute0;
    const Attribute0                                   a0;

    typedef s_baltst::TestAttribute<1, attribute1Name> Attribute1;
    const Attribute1                                   a1;

    typedef s_baltst::TestAttribute<
        0,
        attribute0Name,
        s_baltst::TestAttributeDefaults::k_DEFAULT_ANNOTATION,
        bdlat_FormattingMode::e_NILLABLE>
                             NillableAttribute0;
    const NillableAttribute0 na0;

    typedef s_baltst::TestAttribute<
        1,
        attribute1Name,
        s_baltst::TestAttributeDefaults::k_DEFAULT_ANNOTATION,
        bdlat_FormattingMode::e_NILLABLE>
                             NillableAttribute1;
    const NillableAttribute1 na1;

    // Abbreviations for some enumeration enumerators.
    typedef s_baltst::TestEnumerator<0, enumerator0String> Enumerator0;
    const Enumerator0                                      e0;

    typedef s_baltst::TestEnumerator<1, enumerator1String> Enumerator1;
    const Enumerator1                                      e1;

    // Abbreviations for some choice selections.
    typedef s_baltst::TestSelection<0, selection0Name> Selection0;
    const Selection0                                   s0;

    typedef s_baltst::TestSelection<1, selection1Name> Selection1;
    const Selection1                                   s1;

    typedef s_baltst::TestSelection<
        0,
        selection0Name,
        s_baltst::TestSelectionDefaults::k_DEFAULT_ANNOTATION,
        bdlat_FormattingMode::e_NILLABLE>
                             NillableSelection0;
    const NillableSelection0 ns0;

    typedef s_baltst::TestSelection<
        1,
        selection1Name,
        s_baltst::TestSelectionDefaults::k_DEFAULT_ANNOTATION,
        bdlat_FormattingMode::e_NILLABLE>
                             NillableSelection1;
    const NillableSelection1 ns1;

    // Abbreviation for a function object used to generate XML document
    // structures for printing.
    const GenerateXmlElement x;

    // Abbreviations for some XML attribute keys and values.
    const string_view Nil = "xsi:nil";
    const string_view T   = "true";

    // Abbreviations for function objects used to generate placeholders.
    const s_baltst::TestPlaceHolder<int>               i_;
    const s_baltst::TestPlaceHolder<double>            f_;
    const s_baltst::GenerateTestArrayPlaceHolder       a_;
    const s_baltst::GenerateTestChoicePlaceHolder      c_;
    const s_baltst::GenerateTestDynamicPlaceHolder     d_;
    const s_baltst::GenerateTestEnumerationPlaceHolder e_;
    const s_baltst::GenerateTestNullablePlaceHolder    n_;
    const s_baltst::GenerateTestSequencePlaceHolder    s_;

    // Abbreviations for possible results of a decoding operation.
    enum {
        f = false,  // 0, (en/de)coding fails
        t = true,   // 1, (en/de)coding succeeds
        _ = 2       // 2, (en/de)coding succeeds, but gives different value
    };

    // An abbreviation for an XML structure that will not be used when testing
    // a particular row of the below test table.  The name is short for "Not
    // Applicable."
    const TestXmlElement NA("NA");

    // An abbreviation for the name of the type used to represent one row in
    // this table-based test.
    typedef TestCase14Row R;

    // A macro that is conditionally defined if compiling on platforms where
    // compilation is known to run into resource limitations (e.g. running out
    // of memory on IBM.)
#ifdef BSLS_PLATFORM_CMP_IBM
// Code restructuring made it possible to build this test driver on AIX/IBM xlC
// again.  Should out of memory errors start happening again while compiling
// this file on IBM/AIX using xlC please uncomment the following line, and the
// '#undef' at the end of this function.
#define U_SKIP_DUE_TO_COMPILER_RESOURCE_LIMITATIONS
#endif

    ///Implementation Note
    ///-------------------
    // The following test table shares its structure with the table in case 19
    // of the 'balxml_decoder' component test driver.  These two test cases
    // share an identical test table structure in order to verify that,
    // abstractly, the encoding and decoding operations they perform are
    // "consistent".  Note that the "decoding result" is unused in this test
    // driver.
    //
    // Test case rows labeled with an asterisk "*" verify that different
    // encodings of null values that may be produced by the encoder are treated
    // as representing the same value (null) by the decoder.  In particular,
    // lines with a "1" after the asterisk verify the nullness of decoded
    // values of omitted tags, lines with a "2" verify the nullness of decoded
    // values of self-closing tags, and lines with a "3" verify the nullness of
    // decoded values of self-closing tags with an 'xsi:nil="true"' attribute.

    static const TestCase14Row DATA[] = {
//v---------^                       ENCODING RESULT
//                                 /  DECODING RESULT
//LINE    BDLAT-AWARE OBJECT      /  /         XML STRUCTURE
//---- ------------------------- -- -- -------------------------------
// Arrays.  Top-level arrays are not currently supported.
//R(L_,  a(i_)                    , f, t, NA                           ),
//R(L_,  a(i0)                    , f, _, NA                           ),
// Single-selection choices.
#ifndef U_SKIP_DUE_TO_COMPILER_RESOURCE_LIMITATIONS
R(L_,  c( s0,          i0   )   , t, t, x(C,x(S0,V0      )   )       ),
R(L_,  c( s0, n(       i_  ))   , t, t, x(C                  )       ), // * 1
R(L_,  c( s0, n(       i0  ))   , _, t, x(C,x(S0         )   )       ), // * 2
R(L_,  c( s0, n(       i0  ))   , _, t, x(C,x(S0,Nil,T   )   )       ), // * 3
R(L_,  c( s0, n(       i0  ))   , t, t, x(C,x(S0,V0      )   )       ),
R(L_,  c( s0,    a(i_     ) )   , t, t, x(C                  )       ),
R(L_,  c( s0, n(a_(i_     )))   , t, t, x(C                  )       ),
R(L_,  c( s0, n( a(i_     )))   , t, _, x(C                  )       ),
R(L_,  c( s0,    a(i0     ) )   , t, t, x(C,x(S0,V0      )   )       ),
R(L_,  c( s0, n( a(i0     )))   , t, t, x(C,x(S0,V0      )   )       ),
R(L_,  c( s0,    c(s0, i0 ) )   , t, t, x(C,x(S0,x(S0,V0))   )       ),
R(L_,  c( s0, n(c_(s0, i_ )))   , t, t, x(C                  )       ), // * 1
R(L_,  c( s0, n(c_(s0, i_ )))   , _, _, x(C,x(S0         )   )       ), // * 2
R(L_,  c( s0, n(c_(s0, i_ )))   , _, f, x(C,x(S0,Nil,T   )   )       ), // * 3
R(L_,  c( s0, n( c(s0, i0 )))   , t, t, x(C,x(S0,x(S0,V0))   )       ),
R(L_,  c( s0,    d(  a(i0)) )   , t, t, x(C,x(S0,V0      )   )       ),
R(L_,  c( s0, n(d_( a_(i_))))   , t, t, x(C                  )       ),
R(L_,  c( s0, n( d(  a(i0))))   , t, t, x(C,x(S0,V0      )   )       ),
R(L_,  c( s0,    e(e0, 0  ) )   , _, t, x(C,x(S0,E0      )   )       ),
R(L_,  c( s0,    e(e0, 0  ) )   , t, t, x(C,x(S0,""      )   )       ),
R(L_,  c( s0, n(e_(e0     )))   , t, t, x(C                  )       ), // * 1
R(L_,  c( s0, n( e(e0, 0  )))   , t, t, x(C,x(S0,""      )   )       ),
R(L_,  c( s0, n( e(e0, 0  )))   , _, t, x(C,x(S0,E0      )   )       ),
R(L_,  c( s0, n( e(e0, 0  )))   , _, t, x(C,x(S0         )   )       ), // * 2
R(L_,  c( s0, n( e(e0, 0  )))   , _, t, x(C,x(S0,Nil,T   )   )       ), // * 3
R(L_,  c( s0,    s(a0, i0 ) )   , t, t, x(C,x(S0,x(A0,V0))   )       ),
R(L_,  c( s0, n(s_(a0, i_ )))   , t, t, x(C                  )       ), // * 1
R(L_,  c( s0, n( s(a0, i0 )))   , _, t, x(C,x(S0         )   )       ), // * 2
R(L_,  c( s0, n( s(a0, i0 )))   , _, t, x(C,x(S0,Nil,T   )   )       ), // * 3
R(L_,  c( s0, n( s(a0, i0 )))   , t, t, x(C,x(S0,x(A0,V0))   )       ),
R(L_,  c(ns0,          i0   )   , t, t, x(C,x(S0,V0      )   )       ),
R(L_,  c(ns0, n(       i_  ))   , t, t, x(C                  )       ), // * 1
R(L_,  c(ns0, n(       i_  ))   , _, t, x(C,x(S0         )   )       ), // * 2
R(L_,  c(ns0, n(       i_  ))   , _, t, x(C,x(S0,Nil,T   )   )       ), // * 3
R(L_,  c(ns0, n(       i0  ))   , t, t, x(C,x(S0,V0      )   )       ),
R(L_,  c(ns0,    a(i_     ) )   , t, t, x(C                  )       ),
R(L_,  c(ns0, n(a_(i_     )))   , t, t, x(C                  )       ), // * 1
R(L_,  c(ns0, n(a_(i_     )))   , _, t, x(C,x(S0         )   )       ), // * 2
R(L_,  c(ns0, n(a_(i_     )))   , _, t, x(C,x(S0,Nil,T   )   )       ), // * 3
R(L_,  c(ns0, n( a(i_     )))   , t, _, x(C                  )       ),
R(L_,  c(ns0,    a(i0     ) )   , t, t, x(C,x(S0,V0      )   )       ),
R(L_,  c(ns0, n( a(i0     )))   , t, t, x(C,x(S0,V0      )   )       ),
R(L_,  c(ns0,    c(s0, i0 ) )   , t, t, x(C,x(S0,x(S0,V0))   )       ),
R(L_,  c(ns0, n(c_(s0, i_ )))   , t, t, x(C                  )       ), // * 1
R(L_,  c(ns0, n(c_(s0, i_ )))   , _, t, x(C,x(S0         )   )       ), // * 2
R(L_,  c(ns0, n(c_(s0, i_ )))   , _, t, x(C,x(S0,Nil,T   )   )       ), // * 3
R(L_,  c(ns0, n( c(s0, i0 )))   , t, t, x(C,x(S0,x(S0,V0))   )       ),
R(L_,  c(ns0,    d(  a(i0)) )   , t, t, x(C,x(S0,V0      )   )       ),
R(L_,  c(ns0, n(d_( a_(i_))))   , t, t, x(C                  )       ), // * 1
R(L_,  c(ns0, n(d_( a_(i_))))   , _, t, x(C,x(S0         )   )       ), // * 2
R(L_,  c(ns0, n(d_( a_(i_))))   , _, t, x(C,x(S0,Nil,T   )   )       ), // * 3
R(L_,  c(ns0, n( d(  a(i0))))   , t, t, x(C,x(S0,V0      )   )       ),
R(L_,  c(ns0,    e(e0, 0  ) )   , t, t, x(C,x(S0,""      )   )       ),
R(L_,  c(ns0,    e(e0, 0  ) )   , _, t, x(C,x(S0,E0      )   )       ),
R(L_,  c(ns0, n(e_(e0     )))   , t, t, x(C                  )       ), // * 1
R(L_,  c(ns0, n(e_(e0     )))   , _, t, x(C,x(S0         )   )       ), // * 2
R(L_,  c(ns0, n(e_(e0     )))   , _, t, x(C,x(S0,Nil,T   )   )       ), // * 3
R(L_,  c(ns0, n( e(e0, 0  )))   , t, _, x(C,x(S0,""      )   )       ),
R(L_,  c(ns0, n( e(e0, 0  )))   , _, t, x(C,x(S0,E0      )   )       ),
R(L_,  c(ns0,    s(a0, i0 ) )   , t, t, x(C,x(S0,x(A0,V0))   )       ),
#endif
R(L_,  c(ns0, n(s_(a0, i_ )))   , t, t, x(C                  )       ), // * 1
R(L_,  c(ns0, n(s_(a0, i_ )))   , _, t, x(C,x(S0         )   )       ), // * 2
R(L_,  c(ns0, n(s_(a0, i_ )))   , _, t, x(C,x(S0,Nil,T   )   )       ), // * 3
#ifndef U_SKIP_DUE_TO_COMPILER_RESOURCE_LIMITATIONS
R(L_,  c(ns0, n( s(a0, i0 )))   , t, t, x(C,x(S0,x(A0,V0))   )       ),
#endif
// Double-selection choices.
#ifndef U_SKIP_DUE_TO_COMPILER_RESOURCE_LIMITATIONS
R(L_,  c( s0, s1,  i_ ,  d0 )   , t, t, x(C,x(S1,D0   ))             ),
R(L_,  c( s0, s1,  i_ ,n(f_))   , t, _, x(C            )             ),
R(L_,  c( s0, s1,  i_ ,n(d0))   , t, t, x(C,x(S1,D0   ))             ),
R(L_,  c( s0, s1,  i0 ,  f_ )   , t, t, x(C,x(S0,V0   ))             ),
R(L_,  c( s0, s1,n(i_),  f_ )   , t, t, x(C            )             ),
R(L_,  c( s0, s1,n(i0),  f_ )   , t, t, x(C,x(S0,V0   ))             ),
R(L_,  c(ns0,ns1,  i_ ,  d0 )   , t, t, x(C,x(S1,D0   ))             ),
R(L_,  c(ns0,ns1,  i_ ,n(f_))   , t, _, x(C            )             ),
R(L_,  c(ns0,ns1,  i_ ,n(d0))   , t, t, x(C,x(S1,D0   ))             ),
R(L_,  c(ns0,ns1,  i0 ,  f_ )   , t, t, x(C,x(S0,V0   ))             ),
#endif
R(L_,  c(ns0,ns1,n(i_),  f_ )   , t, t, x(C            )             ), // * 1
R(L_,  c(ns0,ns1,n(i_),  f_ )   , _, t, x(C,x(S0      ))             ), // * 2
R(L_,  c(ns0,ns1,n(i_),  f_ )   , _, t, x(C,x(S0,Nil,T))             ), // * 3
R(L_,  c(ns0,ns1,n(i0),  f_ )   , t, t, x(C,x(S0,V0   ))             ),
// Customized types.
R(L_,  ct(i0,i_)                , t, t, x(CT,V0)                     ),
R(L_,  ct(d0,f_)                , t, t, x(CT,D0)                     ),
// Dynamic types.
#ifndef U_SKIP_DUE_TO_COMPILER_RESOURCE_LIMITATIONS
// Top-level arrays, even when wrapped in a dynamic type, are not currently
// supported.
//R(L_,  d(a(i_)       )          , f, t, NA                           ),
//R(L_,  d(a(i0)       )          , f, _, NA                           ),
//R(L_,  d(a(i0,i1)    )          , f, _, NA                           ),
R(L_,  d(c( s0,  i0 ))          , t, t, x(D,x(S0,V0   ))             ),
R(L_,  d(c( s0,n(i_)))          , t, t, x(D            )             ),
R(L_,  d(c( s0,n(i0)))          , t, t, x(D,x(S0,V0   ))             ),
R(L_,  d(c(ns0,  i0 ))          , t, t, x(D,x(S0,V0   ))             ),
#endif
R(L_,  d(c(ns0,n(i_)))          , t, t, x(D            )             ), // * 1
R(L_,  d(c(ns0,n(i_)))          , _, t, x(D,x(S0      ))             ), // * 2
R(L_,  d(c(ns0,n(i_)))          , _, t, x(D,x(S0,Nil,T))             ), // * 3
#ifndef U_SKIP_DUE_TO_COMPILER_RESOURCE_LIMITATIONS
R(L_,  d(c(ns0,n(i0)))          , t, t, x(D,x(S0,V0   ))             ),
R(L_,  d(s( a0,  i0 ))          , t, t, x(D,x(A0,V0   ))             ),
#endif
R(L_,  d(s( a0,n(i_)))          , t, t, x(D            )             ), // * 1
R(L_,  d(s( a0,n(i_)))          , _, t, x(D,x(S0      ))             ), // * 2
R(L_,  d(s( a0,n(i_)))          , _, t, x(D,x(S0,Nil,T))             ), // * 3
#ifndef U_SKIP_DUE_TO_COMPILER_RESOURCE_LIMITATIONS
R(L_,  d(s( a0,n(i0)))          , t, t, x(D,x(A0,V0   ))             ),
R(L_,  d(s(na0,  i0 ))          , t, t, x(D,x(A0,V0   ))             ),
#endif
R(L_,  d(s(na0,n(i_)))          , t, t, x(D            )             ), // * 1
R(L_,  d(s(na0,n(i_)))          , _, t, x(D,x(S0      ))             ), // * 2
R(L_,  d(s(na0,n(i_)))          , _, t, x(D,x(S0,Nil,T))             ), // * 3
R(L_,  d(s(na0,n(i0)))          , t, t, x(D,x(A0,V0   ))             ),
// Enumerations.
R(L_,  e(e0, 0)                 , t, t, x(E,"")                      ),
R(L_,  e(e0, 0)                 , _, t, x(E,E0)                      ),
R(L_,  e(e0, e1, 0)             , t, t, x(E,"")                      ),
R(L_,  e(e0, e1, 0)             , _, t, x(E,E0)                      ),
R(L_,  e(e0, e1, 1)             , t, t, x(E,E1)                      ),
// Nullable values.  Compilation fails in the decoder, and the encoder does
// not support top-level nullable values.
//R(L_,  n(i_)                    , f, f, NA                           ),
//R(L_,  n(i0)                    , f, f, NA                           ),
//R(L_,  n(s_(a0,i_))             , f, f, NA                           ),
//R(L_,  n( s(a0,i0))             , f, f, NA                           ),
//R(L_,  n( c(s0,s1,i0,f_))       , f, f, NA                           ),
// Single-attribute sequence.
R(L_,  s( a0,  i0)              , t, t, x(S,x(A0,V0))                ),
R(L_,  s( a0,n(i_))             , t, t, x(S)                         ), // * 1
R(L_,  s( a0,n(i0))             , _, t, x(S,x(A0      ))             ), // * 2
R(L_,  s( a0,n(i0))             , _, t, x(S,x(A0,Nil,T))             ), // * 3
#ifndef U_SKIP_DUE_TO_COMPILER_RESOURCE_LIMITATIONS
R(L_,  s( a0,n(i0))             , t, t, x(S,x(A0,V0   ))             ),
R(L_,  s( a0,  a(i_   ) )       , t, t, x(S)                         ),
R(L_,  s( a0,  a(i0   ) )       , t, t, x(S,x(A0,V0))                ),
R(L_,  s( a0,  a(i0,i1) )       , t, t, x(S,x(A0,V0),x(A0,V1))       ),
R(L_,  s( a0,n(a(i_   )))       , t, _, x(S)                         ),
R(L_,  s( a0,n(a(i0   )))       , t, t, x(S,x(A0,V0))                ),
R(L_,  s( a0,n(a(i0,i1)))       , t, t, x(S,x(A0,V0),x(A0,V1))       ),
R(L_,  s( a0,   c( s0,   i0 ) ) , t, t, x(S,x(A0,x(S0,V0)))          ),
R(L_,  s( a0,   c( s0, n(i_)) ) , t, t, x(S,x(A0         ))          ),
R(L_,  s( a0,   c( s0, n(i0)) ) , t, t, x(S,x(A0,x(S0,V0)))          ),
R(L_,  s( a0,   c(ns0,   i0 ) ) , t, t, x(S,x(A0,x(S0,V0)))          ),
R(L_,  s( a0,   c(ns0, n(i_)) ) , t, t, x(S,x(A0         ))          ),
R(L_,  s( a0,   c(ns0, n(i0)) ) , t, t, x(S,x(A0,x(S0,V0)))          ),
R(L_,  s( a0,n(c_( s0, n_(i_)))), t, t, x(S               )          ),
R(L_,  s( a0,n( c( s0,   i0 ))) , t, t, x(S,x(A0,x(S0,V0)))          ),
R(L_,  s( a0,n( c( s0, n(i_)))) , t, t, x(S,x(A0         ))          ),
R(L_,  s( a0,n( c( s0, n(i0)))) , t, t, x(S,x(A0,x(S0,V0)))          ),
R(L_,  s( a0,n( c(ns0,   i0 ))) , t, t, x(S,x(A0,x(S0,V0)))          ),
R(L_,  s( a0,n( c(ns0, n(i_)))) , t, t, x(S,x(A0         ))          ),
R(L_,  s( a0,n( c(ns0, n(i0)))) , t, t, x(S,x(A0,x(S0,V0)))          ),
R(L_,  s( a0, d( a(i_   )))     , t, t, x(S               )          ),
R(L_,  s( a0, d( a(i0   )))     , t, t, x(S,x(A0,V0)      )          ),
R(L_,  s( a0, d( a(i0,i1)))     , t, t, x(S,x(A0,V0),x(A0,V1))       ),
R(L_,  s( a0, d( c(s0,i0)))     , t, t, x(S,x(A0,x(S0,V0)))          ),
R(L_,  s( a0, d( s(a0,i0)))     , t, t, x(S,x(A0,x(A0,V0)))          ),
R(L_,  s( a0,   e(e0,e1,0)    ) , t, t, x(S,x(A0,""))                ),
R(L_,  s( a0,   e(e0,e1,0)    ) , _, t, x(S,x(A0,E0))                ),
R(L_,  s( a0,n(e_(e0,e1  )   )) , t, t, x(S         )                ),
R(L_,  s( a0,n( e(e0,e1,0)   )) , t, t, x(S,x(A0,""))                ),
R(L_,  s( a0,n( e(e0,e1,0)   )) , _, t, x(S,x(A0,E0))                ),
R(L_,  s( a0,    s(a0,i0) )     , t, t, x(S,x(A0,x(A0,V0)))          ),
R(L_,  s( a0, n(s_(a0,i_)))     , t, t, x(S               )          ),
R(L_,  s( a0, n( s(a0,i0)))     , t, t, x(S,x(A0,x(A0,V0)))          ),
R(L_,  s(na0,  i0)              , t, t, x(S,x(A0,V0))                ),
#endif
R(L_,  s(na0,n(i_))             , t, t, x(S)                         ), // * 1
R(L_,  s(na0,n(i_))             , _, t, x(S,x(A0))                   ), // * 2
R(L_,  s(na0,n(i_))             , _, t, x(S,x(A0,Nil,T   ))          ), // * 3
#ifndef U_SKIP_DUE_TO_COMPILER_RESOURCE_LIMITATIONS
R(L_,  s(na0,n(i0))             , t, t, x(S,x(A0,V0))                ),
R(L_,  s(na0,  a(i_   ) )       , t, t, x(S)                         ),
R(L_,  s(na0,  a(i0   ) )       , t, t, x(S,x(A0,V0))                ),
R(L_,  s(na0,  a(i0,i1) )       , t, t, x(S,x(A0,V0),x(A0,V1))       ),
R(L_,  s(na0,n(a(i_   )))       , t, _, x(S)                         ),
R(L_,  s(na0,n(a(i0   )))       , t, t, x(S,x(A0,V0))                ),
R(L_,  s(na0,n(a(i0,i1)))       , t, _, x(S,x(A0,V0),x(A0,V1))       ),
R(L_,  s(na0,   c( s0,   i0 ) ) , t, t, x(S,x(A0,x(S0,V0)))          ),
R(L_,  s(na0,   c( s0, n(i_)) ) , t, t, x(S,x(A0         ))          ),
R(L_,  s(na0,   c( s0, n(i0)) ) , t, t, x(S,x(A0,x(S0,V0)))          ),
R(L_,  s(na0,   c(ns0,   i0 ) ) , t, t, x(S,x(A0,x(S0,V0)))          ),
R(L_,  s(na0,   c(ns0, n(i_)) ) , t, t, x(S,x(A0         ))          ), // * 1
R(L_,  s(na0,   c(ns0, n(i_)) ) , _, t, x(S,x(A0,x(S0)))             ), // * 2
R(L_,  s(na0,   c(ns0, n(i_)) ) , _, t, x(S,x(A0,x(S0,Nil,T)))       ), // * 3
R(L_,  s(na0,   c(ns0, n(i0)) ) , t, t, x(S,x(A0,x(S0,V0)))          ),
#endif
R(L_,  s(na0,n(c_( s0, n_(i_)))), t, t, x(S               )          ), // * 1
R(L_,  s(na0,n(c_( s0, n_(i_)))), _, t, x(S,x(A0      )   )          ), // * 2
R(L_,  s(na0,n(c_( s0, n_(i_)))), _, t, x(S,x(A0,Nil,T)   )          ), // * 3
#ifndef U_SKIP_DUE_TO_COMPILER_RESOURCE_LIMITATIONS
R(L_,  s(na0,n( c( s0,   i0 ))) , t, t, x(S,x(A0,x(S0,V0)))          ),
R(L_,  s(na0,n( c( s0, n(i_)))) , t, _, x(S,x(A0         ))          ),
R(L_,  s(na0,n( c( s0, n(i0)))) , t, t, x(S,x(A0,x(S0,V0)))          ),
R(L_,  s(na0,n( c(ns0,   i0 ))) , t, t, x(S,x(A0,x(S0,V0)))          ),
R(L_,  s(na0,n( c(ns0, n(i_)))) , t, _, x(S,x(A0         ))          ),
R(L_,  s(na0,n( c(ns0, n(i0)))) , t, t, x(S,x(A0,x(S0,V0)))          ),
R(L_,  s(na0, d( a(i_   )))     , t, t, x(S               )          ),
R(L_,  s(na0, d( a(i0   )))     , t, t, x(S,x(A0,V0)      )          ),
R(L_,  s(na0, d( a(i0,i1)))     , t, t, x(S,x(A0,V0),x(A0,V1))       ),
R(L_,  s(na0, d( c(s0,i0)))     , t, t, x(S,x(A0,x(S0,V0)))          ),
R(L_,  s(na0, d( s(a0,i0)))     , t, t, x(S,x(A0,x(A0,V0)))          ),
R(L_,  s(na0,   e(e0,e1,0)    ) , t, t, x(S,x(A0,""))                ),
R(L_,  s(na0,   e(e0,e1,0)    ) , _, t, x(S,x(A0,E0))                ),
R(L_,  s(na0,n(e_(e0,e1  )   )) , t, t, x(S         )                ), // * 1
R(L_,  s(na0,n(e_(e0,e1  )   )) , _, t, x(S,x(A0))                   ), // * 2
R(L_,  s(na0,n(e_(e0,e1  )   )) , _, t, x(S,x(A0,Nil,T))             ), // * 3
R(L_,  s(na0,n( e(e0,e1,0)   )) , t, _, x(S,x(A0,""))                ),
R(L_,  s(na0,n( e(e0,e1,0)   )) , _, t, x(S,x(A0,E0))                ),
R(L_,  s(na0,    s(a0,i0) )     , t, t, x(S,x(A0,x(A0,V0)))          ),
#endif
R(L_,  s(na0, n(s_(a0,i_)))     , t, t, x(S               )          ), // * 1
R(L_,  s(na0, n(s_(a0,i_)))     , _, t, x(S,x(A0))                   ), // * 2
R(L_,  s(na0, n(s_(a0,i_)))     , _, t, x(S,x(A0,Nil,T))             ), // * 3
#ifndef U_SKIP_DUE_TO_COMPILER_RESOURCE_LIMITATIONS
R(L_,  s(na0, n( s(a0,i0)))     , t, t, x(S,x(A0,x(A0,V0)))          ),
#endif
// Double-attribute sequences.
#ifndef U_SKIP_DUE_TO_COMPILER_RESOURCE_LIMITATIONS
R(L_,  s( a0, a1,  i0,   i1 )   , t, t, x(S,x(A0,V0   ),x(A1,V1   )) ),
R(L_,  s( a0, a1,  i0 ,n(i_))   , t, t, x(S,x(A0,V0   )            ) ),
R(L_,  s( a0, a1,  i0 ,n(i1))   , t, t, x(S,x(A0,V0   ),x(A1,V1   )) ),
R(L_,  s( a0, a1,n(i_),  i1 )   , t, t, x(S            ,x(A1,V1   )) ),
R(L_,  s( a0, a1,n(i_),n(i_))   , t, t, x(S                        ) ),
R(L_,  s( a0, a1,n(i_),n(i1))   , t, t, x(S            ,x(A1,V1   )) ),
R(L_,  s( a0, a1,n(i0),  i1 )   , t, t, x(S,x(A0,V0   ),x(A1,V1   )) ),
R(L_,  s( a0, a1,n(i0),n(i_))   , t, t, x(S,x(A0,V0   )            ) ),
R(L_,  s( a0, a1,n(i0),n(i1))   , t, t, x(S,x(A0,V0   ),x(A1,V1   )) ),
R(L_,  s(na0,na1,  i0 ,  i1 )   , t, t, x(S,x(A0,V0   ),x(A1,V1   )) ), // *
R(L_,  s(na0,na1,  i0 ,n(i_))   , t, t, x(S,x(A0,V0   )            ) ), // *
R(L_,  s(na0,na1,  i0 ,n(i_))   , _, t, x(S,x(A0,V0   ),x(A1      )) ), // *
R(L_,  s(na0,na1,  i0 ,n(i_))   , _, t, x(S,x(A0,V0   ),x(A1,Nil,T)) ), // *
R(L_,  s(na0,na1,  i0 ,n(i1))   , t, t, x(S,x(A0,V0   ),x(A1,V1   )) ), // *
R(L_,  s(na0,na1,n(i_),  i1 )   , t, t, x(S            ,x(A1,V1   )) ), // *
R(L_,  s(na0,na1,n(i_),  i1 )   , _, t, x(S,x(A0      ),x(A1,V1   )) ), // *
R(L_,  s(na0,na1,n(i_),  i1 )   , _, t, x(S,x(A0,Nil,T),x(A1,V1   )) ), // *
R(L_,  s(na0,na1,n(i_),n(i_))   , t, t, x(S                        ) ), // *
R(L_,  s(na0,na1,n(i_),n(i_))   , _, t, x(S            ,x(A1      )) ), // *
R(L_,  s(na0,na1,n(i_),n(i_))   , _, t, x(S            ,x(A1,Nil,T)) ), // *
R(L_,  s(na0,na1,n(i_),n(i_))   , _, t, x(S,x(A0      )            ) ), // *
R(L_,  s(na0,na1,n(i_),n(i_))   , _, t, x(S,x(A0      ),x(A1      )) ), // *
R(L_,  s(na0,na1,n(i_),n(i_))   , _, t, x(S,x(A0      ),x(A1,Nil,T)) ), // *
R(L_,  s(na0,na1,n(i_),n(i_))   , _, t, x(S,x(A0,Nil,T)            ) ), // *
R(L_,  s(na0,na1,n(i_),n(i_))   , _, t, x(S,x(A0,Nil,T),x(A1      )) ), // *
R(L_,  s(na0,na1,n(i_),n(i_))   , _, t, x(S,x(A0,Nil,T),x(A1,Nil,T)) ), // *
R(L_,  s(na0,na1,n(i_),n(i1))   , t, t, x(S            ,x(A1,V1   )) ), // *
R(L_,  s(na0,na1,n(i_),n(i1))   , _, t, x(S,x(A0      ),x(A1,V1   )) ), // *
R(L_,  s(na0,na1,n(i_),n(i1))   , _, t, x(S,x(A0,Nil,T),x(A1,V1   )) ), // *
R(L_,  s(na0,na1,n(i0),  i1 )   , t, t, x(S,x(A0,V0   ),x(A1,V1   )) ), // *
R(L_,  s(na0,na1,n(i0),n(i_))   , t, t, x(S,x(A0,V0   )            ) ), // *
#endif
R(L_,  s(na0,na1,n(i0),n(i_))   , _, t, x(S,x(A0,V0   ),x(A1      )) ), // *
R(L_,  s(na0,na1,n(i0),n(i_))   , _, t, x(S,x(A0,V0   ),x(A1,Nil,T)) ), // *
R(L_,  s(na0,na1,n(i0),n(i1))   , t, t, x(S,x(A0,V0   ),x(A1,V1   )) )  // *
//^---------v
    };

#undef U_SKIP_DUE_TO_COMPILER_RESOURCE_LIMITATIONS
#ifdef U_SKIP_DUE_TO_COMPILER_RESOURCE_LIMITATIONS
#error Please do not forget to uncomment the #undef line above!
#endif

    const int NUM_DATA = sizeof DATA / sizeof DATA[0];

    for (int i = 0; i != NUM_DATA; ++i) {
        const TestCase14Row& ROW = DATA[i];

        ROW.runTest();
    }
}

// ============================================================================
//                               USAGE EXAMPLE
// ----------------------------------------------------------------------------

// The following snippets of code illustrate the usage of this component.
// Suppose we have an XML schema inside a file named 'employee.xsd':
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
//  </xs:schema>
//..
// Using the 'bas_codegen.pl' tool, we generate C++ classes for this schema as
// follows:
//..
//  $ bas_codegen.pl -m msg -p test employee.xsd
//..
// This tool will generate the header and implementation files for the
// 'test_messages' components in the current directory.
//
// Now suppose we wanted to encode information about a particular employee
// using XML encoding to the standard output, using the 'PRETTY' option for
// formatting the output.  The following function will do this:
//..
//  #include <test_messages.h>
//
//  #include <balxml_encoder.h>
//  #include <balxml_encodingstyle.h>
//
//  #include <bsl_iostream.h>
//  #include <bsl_sstream.h>
//
//  using namespace BloombergLP;

    void usageExample()
    {
        test::Employee bob;

        bob.name()                 = "Bob";
        bob.homeAddress().street() = "Some Street";
        bob.homeAddress().city()   = "Some City";
        bob.homeAddress().state()  = "Some State";
        bob.age()                  = 21;

        balxml::EncoderOptions options;
        options.setEncodingStyle(balxml::EncodingStyle::PRETTY);

        balxml::Encoder encoder(&options, &bsl::cerr, &bsl::cerr);

        const bsl::string EXPECTED_OUTPUT =
         "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
         "<Employee xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\">\n"
         "    <name>Bob</name>\n"
         "    <homeAddress>\n"
         "        <street>Some Street</street>\n"
         "        <city>Some City</city>\n"
         "        <state>Some State</state>\n"
         "    </homeAddress>\n"
         "    <age>21</age>\n"
         "</Employee>\n";

        bsl::ostringstream os;
        const int rc = encoder.encodeToStream(os, bob);

        ASSERT(0 == rc);
        ASSERT(EXPECTED_OUTPUT == os.str());
    }
//..

// ============================================================================
//                               MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;

    verbose             = argc > 2;
    veryVerbose         = argc > 3;
    veryVeryVerbose     = argc > 4;
    veryVeryVeryVerbose = argc > 5;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;;

    // CONCERN: 'BSLS_REVIEW' failures should lead to test failures.
    bsls::ReviewFailureHandlerGuard reviewGuard(&bsls::Review::failByAbort);

    switch (test) { case 0:  // Zero is always the leading case.
      case 19: {
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
      case 18: {
        // --------------------------------------------------------------------
        // REPRODUCE SCENARIO FROM DRQS 169438741
        //
        // Concerns:
        //: 1 'bdlb::Variant2<bdlt::Date, bdlt::DateTz>' value can be encoded
        //:   to XML.
        //:
        //: 2 'bdlb::Variant2<bdlt::Time, bdlt::TimeTz>' value can be encoded
        //:   to XML.
        //:
        //: 3 'bdlb::Variant2<bdlt::Datetime, bdlt::DatetimeTz>' value can be
        //:   encoded to XML.
        //
        // Plan:
        //: 1 Create several objects and encode them to XML using
        //:   'balxml::Encoder'.  Verify obtained results.  (C-1..3)
        //
        // Testing:
        //   DRQS 169438741
        // --------------------------------------------------------------------

        if (verbose) cout << "\nREPRODUCE SCENARIO FROM DRQS 169438741"
                          << "\n======================================"
                          << endl;

        typedef bdlb::Variant2<bdlt::Date, bdlt::DateTz> DateOrDateTz;
        typedef bdlb::Variant2<bdlt::Time, bdlt::TimeTz> TimeOrTimeTz;
        typedef bdlb::Variant2<bdlt::Datetime, bdlt::DatetimeTz>
                                                      DatetimeOrDatetimeTz;

        balxml::EncoderOptions options;
        balxml::Encoder encoder(&options, 0, 0);

        // Testing 'bdlt::Date' and 'bdltDateTz'.
        {
            const DateOrDateTz DATE(bdlt::Date(2, 2, 2));
            const DateOrDateTz DATETZ(
                                  bdlt::DateTz(bdlt::Date(9999, 12, 31), -90));
            const bsl::string_view EXP_DATE =
                                  "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>"
                                  "<anyType" XSI ">"
                                     "0002-02-02"
                                  "</anyType>";

            bsl::stringstream resultDate;
            int rc = encoder.encodeToStream(resultDate, DATE);

            ASSERTV(rc, 0 == rc);
            ASSERTV(EXP_DATE, resultDate.str(), EXP_DATE == resultDate.str());

            const bsl::string_view EXP_DATETZ =
                                  "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>"
                                  "<anyType" XSI ">"
                                     "9999-12-31-01:30"
                                  "</anyType>";

            bsl::stringstream resultDateTz;
            rc = encoder.encodeToStream(resultDateTz, DATETZ);

            ASSERTV(rc, 0 == rc);
            ASSERTV(EXP_DATETZ, resultDateTz.str(),
                    EXP_DATETZ == resultDateTz.str());
        }

        // Testing 'bdlt::Time' and 'bdltTimeTz'.
        {
            const TimeOrTimeTz TIME(bdlt::Time(1, 1, 1));
            const TimeOrTimeTz TIMETZ(
                          bdlt::TimeTz(bdlt::Time(23, 59, 59, 999, 999), -90));
            const bsl::string_view EXP_TIME =
                                  "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>"
                                  "<anyType" XSI ">"
                                     "01:01:01.000000"
                                  "</anyType>";

            bsl::stringstream resultTime;
            int rc = encoder.encodeToStream(resultTime, TIME);

            ASSERTV(rc, 0 == rc);
            ASSERTV(EXP_TIME, resultTime.str(), EXP_TIME == resultTime.str());

            const bsl::string_view EXP_TIMETZ =
                                  "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>"
                                  "<anyType" XSI ">"
                                     "23:59:59.999999-01:30"
                                  "</anyType>";

            bsl::stringstream resultTimeTz;
            rc = encoder.encodeToStream(resultTimeTz, TIMETZ);

            ASSERTV(rc, 0 == rc);
            ASSERTV(EXP_TIMETZ, resultTimeTz.str(),
                    EXP_TIMETZ == resultTimeTz.str());
        }

        // Testing 'bdlt::Datetime' and 'bdltDatetimeTz'.
        {
            const DatetimeOrDatetimeTz DATETIME(
                                       bdlt::Datetime(1, 1, 1, 0, 0, 0, 0, 0));
            const DatetimeOrDatetimeTz DATETIMETZ(bdlt::DatetimeTz(
                bdlt::Datetime(9999, 12, 31, 23, 59, 59, 999, 999), -90));

            const bsl::string_view EXP_DATETIME =
                                  "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>"
                                  "<anyType" XSI ">"
                                     "0001-01-01T00:00:00.000000"
                                  "</anyType>";

            bsl::stringstream resultDatetime;
            int rc = encoder.encodeToStream(resultDatetime, DATETIME);

            ASSERTV(rc, 0 == rc);
            ASSERTV(EXP_DATETIME, resultDatetime.str(),
                    EXP_DATETIME == resultDatetime.str());

            const bsl::string_view EXP_DATETIMETZ =
                                  "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>"
                                  "<anyType" XSI ">"
                                     "9999-12-31T23:59:59.999999-01:30"
                                  "</anyType>";

            bsl::stringstream resultDatetimeTz;
            rc = encoder.encodeToStream(resultDatetimeTz, DATETIMETZ);

            ASSERTV(rc, 0 == rc);
            ASSERTV(EXP_DATETIMETZ, resultDatetimeTz.str(),
                    EXP_DATETIMETZ == resultDatetimeTz.str());
        }
      } break;
      case 17: {
        //---------------------------------------------------------------------
        // TEST CASE DOCUMENTATION IS REPEATED IN THE 'runTestCase17()'
        // function so it is also near the actual test code.  Make sure that
        // anything you change here, you also changed in 'runTestCase17()' and
        // vice versa.
        // --------------------------------------------------------------------
        // TESTING DECIMAL ATTRIBUTE ENCODING
        //   This case tests that the "MaxDecimalTotalDigits" and
        //   "MaxDecimalFractionDigits" options apply when
        //   encoding attributes of sequence types that are represented by
        //   'double's and have the 'bdlat_FormattingMode::e_DEC' formatting
        //   mode.
        //
        // Concerns:
        //: 1 If neither option is set, decimal-formatted, 'double'-valued
        //:   attributes are encoded as if "MaxDecimalTotalDigits" were 16
        //:   and "MaxDecimalFractionDigits" were 12.
        //:
        //: 2 Neither option affects the number of digits in the encoding of
        //:   the non-fractional component of such an attribute (i.e. neither
        //:   option affects the number of digits that can appear to the left
        //:   of the decimal point.)
        //:
        //: 3 The encoding of such an attribute always includes at least 1
        //:   fractional digit, even if the values of the options would
        //:   otherwise prohibit it.
        //:
        //: 4 The total number of fractional digits in the encoding of
        //:   such an attribute is exactly equal to the difference between
        //:   "MaxDecimalTotalDigits" and "MaxDecimalFractionDigits", or 1,
        //:   whichever is greater.
        //:
        //: 5 If the value of the fractional component of the attribute can be
        //:   represented in fewer than the number of digits alloted, the
        //:   remaining digits are filled with zeros.
        //:
        //: 6 Neither option affects the encoding of 'double'-valued attributes
        //:   with the 'bdlat_FormattingMode::e_DEFAULT' formatting mode.
        //:
        //: 7 Neither option affects the encoding of 'bdldfp::Decimal64'-valued
        //:   attributes with any formatting mode.
        //:
        // Plan:
        //: 1 Define a set N of numbers that can be exactly represented by
        //:   the IEEE 754-2008 64-bit binary floating-point format, and that
        //:   have different numbers of digits in their decimal representation.
        //:   (Selecting positive and negative powers of 2 works.)
        //:
        //: 2 For each number n in N, enumerate several permutations of values
        //:   for "MaxDecimalTotalDigits" and "MaxDecimalFractionDigits" and
        //:   perform the following:
        //:
        //:   1 Encode an object having a 'double'-valued, decimal-formatted
        //:     attribute with the value n.
        //:
        //:   2 Verify that the encoding of the attribute satisfies the
        //:     constraints defined in the concerns.
        //:
        //:   3 Encode an object having a 'double'-valued, default-formatted
        //:     attribute with the value n.
        //:
        //:   4 Verify that the encoding of the attribute is not affected by
        //:     the value of "MaxDecimalTotalDigits" or
        //:     "MaxDecimalFractionDigits".  (Notice that the encoding of the
        //:     attribute satisfies the constraints defined in the W3C XML
        //:     specification for 'double' values.)
        //:
        //: 3 Define a set P of numbers that can be exactly represented by the
        //:   IEEE 754-2008 64-bit *decimal* floating-point format, and that
        //:   have different numbers of digits in their decimal representation.
        //:   (Selecting positive and negative powers of 10 with different
        //:   amounts of precision works.)
        //:
        //: 4 For each number p in P, enumerate several permutations of
        //:   values for "MaxDecimalTotalDigits" and "MaxDecimalFractionDigits"
        //:   and perform the following:
        //:
        //:   5 Encode an object having a 'bdldfp::Decimal64'-valued,
        //:     default-formatted attribute with the value p.
        //:
        //:   6 Verify that the encoding of the attribute is not affected
        //:     by the value of "MaxDecimalTotalDigits" or
        //:     "MaxDecimalFractionDigits".  (Notice that the encoding of the
        //:     attribute satisfies the constraints defined in the W3C XML
        //:     specification for 'precisionDecimal' values.)
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting Decimal Attribute Encoding"
                          << "\n==================================" << endl;

        runTestCase17();

      } break;
      case 16: {
        // --------------------------------------------------------------------
        // TESTING FORMATTER OPTION CALCULATION
        //   This case tests that 'balxml::Encoder' correctly calculates the
        //   options to use for its internal XML formatter.
        //
        // Concerns:
        //: 1 If the encoding style is compact, the formatter's initial indent
        //:   level, spaces per level, and wrap column are 0, 0, and -1,
        //:   respectively, regardless of what the values for those fields are
        //:   in the encoder's options.
        //:
        //: 2 If the encoding style is pretty, the formatter's initial indent
        //:   level, spaces per level, and wrap column are equal to the
        //:   respective values from the encoder's options.
        //:
        //: 3 The encoder forwards the values of all other options to the
        //:   formatter except: "DatetimeFractionalSecondPrecision",
        //:   "AllowControlCharacters", and "UseZAbbreviationForUtc".  Note
        //:   that this is to maintain bug-compatibility with a prior version
        //:   of this component that did not correctly forward the encoder's
        //:   options to the formatter.
        //:
        // Plan:
        //: 1 Calculate the formatter options from the default encoder options
        //:   and verify that the formatter options' initial indent level is 0,
        //:   its spaces per level is 0, and its wrap column is -1.
        //:
        //: 2 Calculate the formatter options from a set of encoder options
        //:   with a compact encoding style, as well as an initial indent
        //:   level, spaces per level, and a wrap column all set to 10.  Verify
        //:   that the formatter options' initial indent level is 0, its spaces
        //:   per level is 0, and its wrap column is -1.
        //:
        //: 3 Calculate the formatter options from a set of encoder options
        //:   with pretty encoding style and all other options default. Verify
        //:   that the formatter options' initial indent level is 0, its spaces
        //:   per level if 4, and its wrap column is 80.
        //:
        //: 4 Calculate the formatter options from a set of encoder options
        //:   with pretty encoding style, as well as an initial indent level,
        //:   spaces per level, and wrap column all set to 10.  Verify that the
        //:   formatter options' initial indent level, spaces per level, and
        //:   wrap column are likewise all 10.
        //:
        //: 5 Calculate the formatter options from a set of encoder options
        //:   having each option set to a non-default value.  Verify that all
        //:   of the formatter's options have the corresponding non-default
        //:   value, except for "DatetimeFractionalSecondPrecision",
        //:   "AllowControlCharacters", and "UseZAbbreviationForUtc", which
        //:   have their respective default values.
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting Formatter Option Calculation"
                          << "\n====================================" << endl;

        typedef balxml::Encoder_OptionsCompatibilityUtil Util;

        {
            EncoderOptions encoderOptions;

            int formatterIndentLevel    = 0;
            int formatterSpacesPerLevel = 0;
            int formatterWrapColumn     = 0;

            EncoderOptions formatterOptions;

            Util::getFormatterOptions(&formatterIndentLevel,
                                      &formatterSpacesPerLevel,
                                      &formatterWrapColumn,
                                      &formatterOptions,
                                      encoderOptions);

            ASSERT(0 == formatterIndentLevel);
            ASSERT(0 == formatterSpacesPerLevel);
            ASSERT(-1 == formatterWrapColumn);
        }

        {
            EncoderOptions encoderOptions;
            encoderOptions.setEncodingStyle(balxml::EncodingStyle::COMPACT);
            encoderOptions.setInitialIndentLevel(10);
            encoderOptions.setSpacesPerLevel(10);
            encoderOptions.setWrapColumn(10);

            int formatterIndentLevel    = 0;
            int formatterSpacesPerLevel = 0;
            int formatterWrapColumn     = 0;

            EncoderOptions formatterOptions;

            Util::getFormatterOptions(&formatterIndentLevel,
                                      &formatterSpacesPerLevel,
                                      &formatterWrapColumn,
                                      &formatterOptions,
                                      encoderOptions);

            ASSERT( 0 == formatterIndentLevel);
            ASSERT( 0 == formatterSpacesPerLevel);
            ASSERT(-1 == formatterWrapColumn);
        }

        {
            EncoderOptions encoderOptions;
            encoderOptions.setEncodingStyle(balxml::EncodingStyle::PRETTY);

            int formatterIndentLevel    = 0;
            int formatterSpacesPerLevel = 0;
            int formatterWrapColumn     = 0;

            EncoderOptions formatterOptions;

            Util::getFormatterOptions(&formatterIndentLevel,
                                      &formatterSpacesPerLevel,
                                      &formatterWrapColumn,
                                      &formatterOptions,
                                      encoderOptions);

            ASSERT( 0 == formatterIndentLevel);
            ASSERT( 4 == formatterSpacesPerLevel);
            ASSERT(80 == formatterWrapColumn);
        }

        {
            EncoderOptions encoderOptions;
            encoderOptions.setEncodingStyle(balxml::EncodingStyle::PRETTY);
            encoderOptions.setInitialIndentLevel(10);
            encoderOptions.setSpacesPerLevel(10);
            encoderOptions.setWrapColumn(10);

            int formatterIndentLevel    = 0;
            int formatterSpacesPerLevel = 0;
            int formatterWrapColumn     = 0;

            EncoderOptions formatterOptions;

            Util::getFormatterOptions(&formatterIndentLevel,
                                      &formatterSpacesPerLevel,
                                      &formatterWrapColumn,
                                      &formatterOptions,
                                      encoderOptions);

            ASSERT(10 == formatterIndentLevel);
            ASSERT(10 == formatterSpacesPerLevel);
            ASSERT(10 == formatterWrapColumn);
        }

        {
            EncoderOptions encoderOptions;
            encoderOptions.setObjectNamespace("notDefault");
            encoderOptions.setSchemaLocation("notDefault");
            encoderOptions.setTag("notDefault");
            encoderOptions.setFormattingMode(12345);
            // skip initial indent level
            // skip spaces per level
            // skip wrap column
            encoderOptions.setMaxDecimalTotalDigits(12345);
            encoderOptions.setMaxDecimalFractionDigits(12345);
            encoderOptions.setSignificantDoubleDigits(12345);
            // skip encoding style
            encoderOptions.setAllowControlCharacters(true);
            encoderOptions.setOutputXMLHeader(false);
            encoderOptions.setOutputXSIAlias(false);
            encoderOptions.setDatetimeFractionalSecondPrecision(12345);
            encoderOptions.setUseZAbbreviationForUtc(true);

            // Assert that none of the options set above are equal to their
            // default values.
            const EncoderOptions defaultOptions;

            ASSERT(defaultOptions.objectNamespace() !=
                   encoderOptions.objectNamespace());
            ASSERT(defaultOptions.schemaLocation() !=
                   encoderOptions.schemaLocation());
            ASSERT(defaultOptions.tag() != encoderOptions.tag());
            ASSERT(defaultOptions.formattingMode() !=
                   encoderOptions.formattingMode());
            ASSERT(defaultOptions.maxDecimalTotalDigits() !=
                   encoderOptions.maxDecimalTotalDigits());
            ASSERT(defaultOptions.maxDecimalFractionDigits() !=
                   encoderOptions.maxDecimalFractionDigits());
            ASSERT(defaultOptions.significantDoubleDigits() !=
                   encoderOptions.significantDoubleDigits());
            ASSERT(defaultOptions.allowControlCharacters() !=
                   encoderOptions.allowControlCharacters());
            ASSERT(defaultOptions.outputXMLHeader() !=
                   encoderOptions.outputXMLHeader());
            ASSERT(defaultOptions.outputXSIAlias() !=
                   encoderOptions.outputXSIAlias());
            ASSERT(defaultOptions.datetimeFractionalSecondPrecision() !=
                   encoderOptions.datetimeFractionalSecondPrecision());
            ASSERT(defaultOptions.useZAbbreviationForUtc() !=
                   encoderOptions.useZAbbreviationForUtc());

            int formatterIndentLevel    = 0;
            int formatterSpacesPerLevel = 0;
            int formatterWrapColumn     = 0;

            EncoderOptions formatterOptions;

            Util::getFormatterOptions(&formatterIndentLevel,
                                      &formatterSpacesPerLevel,
                                      &formatterWrapColumn,
                                      &formatterOptions,
                                      encoderOptions);


            ASSERT("notDefault" == formatterOptions.objectNamespace());
            ASSERT(defaultOptions.objectNamespace() !=
                   formatterOptions.objectNamespace());

            ASSERT("notDefault" == formatterOptions.schemaLocation());
            ASSERT(defaultOptions.schemaLocation() !=
                   formatterOptions.schemaLocation());

            ASSERT("notDefault" == formatterOptions.tag());
            ASSERT(defaultOptions.tag() != formatterOptions.tag());

            ASSERT(12345 == formatterOptions.formattingMode());
            ASSERT(defaultOptions.formattingMode() !=
                   formatterOptions.formattingMode());

            ASSERT(12345 == formatterOptions.maxDecimalTotalDigits());
            ASSERT(defaultOptions.maxDecimalTotalDigits() !=
                   formatterOptions.maxDecimalTotalDigits());

            ASSERT(12345 == formatterOptions.maxDecimalFractionDigits());
            ASSERT(defaultOptions.maxDecimalFractionDigits() !=
                   formatterOptions.maxDecimalFractionDigits());

            ASSERT(12345 == formatterOptions.significantDoubleDigits());
            ASSERT(defaultOptions.significantDoubleDigits() !=
                   formatterOptions.significantDoubleDigits());

            // AllowControlCharacters should be defaulted.
            ASSERT(false == formatterOptions.allowControlCharacters());
            ASSERT(defaultOptions.allowControlCharacters() ==
                   formatterOptions.allowControlCharacters());

            ASSERT(false == formatterOptions.outputXMLHeader());
            ASSERT(defaultOptions.outputXMLHeader() !=
                   formatterOptions.outputXMLHeader());

            ASSERT(false == formatterOptions.outputXSIAlias());
            ASSERT(defaultOptions.outputXSIAlias() !=
                   formatterOptions.outputXSIAlias());

            // DatetimeFractionalSecondPrecision should be defaulted.
            ASSERT(6 == formatterOptions.datetimeFractionalSecondPrecision());
            ASSERT(
                defaultOptions.datetimeFractionalSecondPrecision() ==
                formatterOptions.datetimeFractionalSecondPrecision());

            // UseZAbbreviationforUtc should be defaulted.
            ASSERT(false == formatterOptions.useZAbbreviationForUtc());
            ASSERT(defaultOptions.useZAbbreviationForUtc() ==
                   formatterOptions.useZAbbreviationForUtc());
        }
      } break;
      case 15: {
        // --------------------------------------------------------------------
        // Testing Decimal64
        //
        // Concerns:
        //: 1 That the encoder can encoder a field of type 'Decimal64'.
        //
        // Plan:
        //: 1 Copy the 'Employee' type from the usage example, creating a
        //:   'Contractor' type where the integer 'age' field is replaced by
        //:   by a 'Decimal64' 'hourlyRate' field.
        //:
        //: 2 Create a table containing decimal values are the expected text
        //:   output from encoding them.
        //:
        //: 3 Run the encoder and verify the results.
        //
        // Testing:
        //   Type Decimal64
        // --------------------------------------------------------------------

        if (verbose) cout << "Testing Decimal64\n"
                             "=================\n";

        using namespace BloombergLP;
        namespace TC = DECIMALTEST;

#define DFP(X) BDLDFP_DECIMAL_DD(X)

        const char         preField[]  = { "<hourlyRate>" };
        const char        *postField   = "</hourlyRate>";
        const bsl::size_t  preFieldLen = sizeof(preField) - 1;

        static const struct Data {
            int                d_line;
            bdldfp::Decimal64  d_value;
            const char        *d_text;
        } DATA[] = {
            { L_, DFP(0.0), "0.0" },
            { L_, DFP(15.13), "15.13" },
            { L_, DFP(892.0), "892.0" },
            { L_, DFP(-15.13), "-15.13" },
            { L_, DFP(-892.0), "-892.0" },
            { L_, DFP(47.3e34), "4.73e+35" },
            { L_, DFP(-47.3e34), "-4.73e+35" }
        };
        enum { k_NUM_DATA = sizeof DATA / sizeof *DATA };

        for (int ti = 0; ti < k_NUM_DATA; ++ti) {
            const Data&               data  = DATA[ti];
            const int                 LINE  = data.d_line;
            const bdldfp::Decimal64&  VALUE = data.d_value;
            const bsl::string         TEXT  = data.d_text;

            TC::Contractor bob;
            bob.name()                 = "Bob";
            bob.homeAddress().street() = "Some Street";
            bob.homeAddress().city()   = "Some City";
            bob.homeAddress().state()  = "Some State";
            bob.hourlyRate()           = VALUE;

            balxml::EncoderOptions options;
            options.setEncodingStyle(balxml::EncodingStyle::PRETTY);

            balxml::Encoder encoder(&options, &bsl::cerr, &bsl::cerr);

            const bsl::string EXPECTED =
                 "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                 "<Contractor xmlns:xsi"
                            "=\"http://www.w3.org/2001/XMLSchema-instance\">\n"
                 "    <name>Bob</name>\n"
                 "    <homeAddress>\n"
                 "        <street>Some Street</street>\n"
                 "        <city>Some City</city>\n"
                 "        <state>Some State</state>\n"
                 "    </homeAddress>\n"
                 "    <hourlyRate>" + TEXT + "</hourlyRate>\n"
                 "</Contractor>\n";

            bsl::ostringstream os;
            const int rc = encoder.encodeToStream(os, bob);
            ASSERTV(LINE, rc, 0 == rc);

            const bsl::string& OUTPUT = os.str();
            ASSERTV(EXPECTED, OUTPUT, EXPECTED == OUTPUT);

            const char *field = bsl::strstr(OUTPUT.c_str(), preField);
            ASSERT(field);    ASSERT(preFieldLen < bsl::strlen(field));
            field += preFieldLen;
            const char *end   = bsl::strstr(OUTPUT.c_str(), postField);
            ASSERT(end && field < end);
            const string_view fieldRef(field, end - field);

            ASSERTV(LINE, TEXT, fieldRef, TEXT == fieldRef);
        }
      } break;
      case 14: {
        //---------------------------------------------------------------------
        // TEST CASE DOCUMENTATION IS REPEATED IN THE 'runTestCase14()'
        // function so it is also near the actual test code.  Make sure that
        // anything you change here, you also changed in 'runTestCase14()' and
        // vice versa.
        // --------------------------------------------------------------------
        // TESTING NILLABLE ELEMENT ENCODING
        //   This case tests the 'balxml::Encoder::encode' operation when
        //   encoding objects that may or may not be "nullable", and may or may
        //   not have the "nillable" formatting mode applied.  In general, it
        //   is expected that null values encode to nothing in XML, without
        //   regard to the value of their "nillable" formatting mode.
        //
        // Concerns:
        //: 1 Attributes of sequence types and selections of choice types
        //:   that do not have the "nillable" formatting mode and are not null
        //:   encode to XML representations of their value.
        //:
        //: 2 Attributes of sequence types and selections of choice types that
        //:   do not have the "nillable" formatting mode and are null do not
        //:   appear in the XML representations of their value.
        //:
        //: 3 Attributes of sequence types and selections of choice types
        //:   that have the "nillable" formatting mode and are not null
        //:   encode to XML representations of their value.
        //:
        //: 4 Attributes of sequence types and selections of choice types
        //:   that have the "nillable" formatting mode and are null do not
        //:   appear in the XML representations of their value.
        //:
        //: 5 Objects of nullable types that do not have the null value encode
        //:   to XML representations of the underlying value.
        //:
        //: 6 Objects of nullable types that have the null value do not appear
        //:   in the XML representations of their value.
        //:
        //: 7 The above 6 properties hold for arbitrary nesting and
        //:   permutation of 'bdlat' concept implementations.  For example,
        //:   these properties should not only hold for a sequence of two
        //:   integers, but also a sequence of an enumeration and a choice
        //:   between an integer and a string.
        //
        // Plan:
        //: 1 Create objects enumerating all 8 'bdlat' attribute type concepts,
        //:   and where applicable, recursively up to a depth of 2.
        //:
        //: 2 For each of the above objects, create a variant that is non-null
        //:   but not nillable, a variant that is null but not nillable, a
        //:   variant that is non-null and nillable, and a variant that is null
        //:   and nillable.
        //:
        //: 3 For each of the above objects, verify that their XML encodings
        //:   created by 'balxml::Encoder::encode' satisfy the 6 properties
        //:   defined in the "Concerns".
        //
        // Testing:
        //   int encode(bsl::streambuf *buffer, const TYPE& object);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting Encoding of Nillable Elements"
                          << "\n=====================================" << endl;

        runTestCase14();

        if (verbose) cout << "\nEnd of Test." << endl;
      } break;
      case 13: {
        // --------------------------------------------------------------------
        // TESTING THE 'TestXmlElement' TEST FACILITY
        //   The subsequent test case uses the 'TestXmlElement' class in order
        //   to provide a concise notation for describing XML documents.  This
        //   case tests the printing operations of 'TestXmlElement'.
        //
        // Concerns:
        //: 1 Elements having no attributes nor any content print as
        //:   self-closing elements with no attributes.
        //:
        //: 2 Elements having content but no attributes print as an opening
        //:   element, the content, and a closing element.
        //:
        //: 3 Content may be text or zero or more child elements.
        //:
        //: 4 When printed, each attribute is separated from the previous token
        //:   by 1 space character.
        //:
        //: 5 Printing supports arbitrary levels of nesting of attributes,
        //:   child elements, and content.
        //
        // Plan:
        //: 1 Given a set of attribute sequences and a set of content, create
        //:   a simple XML element object given each element of the cartesian
        //:   product of those sets.
        //:
        //: 2 Print each of the resulting simple XML element objects, and
        //:   verify that the resulting text is valid XML and that it
        //:   represents the same value as the object.
        //:
        //: 3 Our test points explore the space of 0 to 3 attributes, elements,
        //:   and nesting.  Deeper tests would not be qualitatively different.
        //
        // Testing:
        //   TestXmlElement(*bA = 0);
        //   TestXmlElement(name, *bA = 0);
        //   TestXmlElement(name, attributes, *bA = 0);
        //   TestXmlElement(name, content, *bA = 0);
        //   TestXmlElement(name, attributes, *bA = 0);
        //   stream& operator<<(TestXmlElement&, stream&);
        // --------------------------------------------------------------------

        // abbreviation for the type under test
        typedef TestXmlElement X;

        // abbreviation for a 'bsl::pair<bsl::string, bsl::string>', used as
        // a key-value pair
        typedef X::Attribute  Attribute;
        // abbreviation for a 'bsl::vector<Attribute>'
        typedef X::Attributes Attributes;
        // abbreviation for a 'bsl::vector<X>'
        typedef X::Elements   Elements;
        // abbreviation for a 'bdlb::Variant<Elements, bsl::string>'
        typedef X::Content    Content;

        // attribute 0
        const Attribute A0("attr0", "val0");
        // attribute 1
        const Attribute A1("attr1", "val1");
        // attribute 2
        const Attribute A2("attr2", "");

        // modifiable attribute list containing attribute 0
        Attributes mAs0;
        mAs0.push_back(A0);
        // non-modifiable reference to an attribute list containing attribute 0
        const Attributes& As0 = mAs0;

        // modifiable attribute list containing attributes 0 and 1
        Attributes mAs01;
        mAs01.push_back(A0);
        mAs01.push_back(A1);
        // non-modifiable reference to an attribute list containing attributes
        // 0 and 1
        const Attributes& As01 = mAs01;

        // modifiable attribute list containing attributes 0 and 2
        Attributes mAs02;
        mAs02.push_back(A0);
        mAs02.push_back(A2);
        // non-modifiable reference to an attribute list containing attributes
        // 0 and 2
        const Attributes& As02 = mAs02;

        // modifiable content number 0, which contains the empty string
        Content mC0;
        mC0.createInPlace<bsl::string>();
        // non-modifiable reference to content number 0
        const Content& C0 = mC0;

        // modifiable content number 1, which contains the string "lorem ipsum"
        Content mC1;
        mC1.createInPlace<bsl::string>("Lorem ipsum.");
        // non-modifiable reference to content number 1
        const Content& C1 = mC1;

        // modifiable content number 2, which contains a single self-closing
        // child element
        Content mC2;
        mC2.createInPlace<Elements>();
        mC2.the<Elements>().push_back(X("DEF"));
        // non-modifiable reference to content number 2
        const Content& C2 = mC2;

        // modifiable content number 3, which contains two self-closing
        // child elements that each have some attributes
        Content mC3;
        mC3.createInPlace<Elements>();
        mC3.the<Elements>().push_back(X("GHI", As0));
        mC3.the<Elements>().push_back(X("JKL", As01));
        // non-modifiable reference to content number 3
        const Content& C3 = mC3;

        // modifiable content number 4, which contains two child elements that
        // contain some textual content and some child elements, respectively.
        Content mC4;
        mC4.createInPlace<Elements>();
        mC4.the<Elements>().push_back(X("MNO", C1));
        mC4.the<Elements>().push_back(X("PQR", As0, C2));
        // non-modifiable reference to content number 4
        const Content& C4 = mC4;

        // modifiable content number 5, which has a child nesting depth of 3,
        // and 3 children.
        Content mC5;
        mC5.createInPlace<Elements>();
        mC5.the<Elements>().push_back(X("STU", C4));
        mC5.the<Elements>().push_back(X("VWX"));
        mC5.the<Elements>().push_back(X("YZ1"));
        // non-modifiable reference to content number 5
        const Content& C5 = mC5;

        static const struct {
            int             d_line;    // line number
            TestXmlElement  d_xml;     // XML object representation
            const char     *d_string;  // expected printout of 'd_xml'
        } DATA[] = {
            //  LINE
            // /      XML OBJECT         EXPECTED PRINTOUT OF XML OBJECT
            //--- ------------------- -------------------------------------
            { L_ , X()               , "</>" },
            { L_ , X("ABC")          , "<ABC/>" },
            { L_ , X("ABC", As0)     , "<ABC attr0='val0'/>" },
            { L_ , X("ABC", As01)    , "<ABC attr0='val0' attr1='val1'/>" },
            { L_ , X("ABC", As02)    , "<ABC attr0='val0' attr2=''/>" },
            { L_ , X("ABC", C0)      , "<ABC>"
                                       "</ABC>" },

            { L_ , X("ABC", C1)      , "<ABC>"
                                           "Lorem ipsum."
                                       "</ABC>" },

            { L_ , X("ABC", C2)      , "<ABC>"
                                           "<DEF/>"
                                       "</ABC>" },

            { L_ , X("ABC", C3)      , "<ABC>"
                                           "<GHI attr0='val0'/>"
                                           "<JKL attr0='val0' attr1='val1'/>"
                                       "</ABC>" },

            { L_ , X("ABC", C4)      , "<ABC>"
                                           "<MNO>"
                                               "Lorem ipsum."
                                           "</MNO>"
                                           "<PQR attr0='val0'>"
                                               "<DEF/>"
                                           "</PQR>"
                                       "</ABC>" },

            { L_ , X("ABC", C5)      , "<ABC>"
                                           "<STU>"
                                               "<MNO>"
                                                   "Lorem ipsum."
                                               "</MNO>"
                                               "<PQR attr0='val0'>"
                                                   "<DEF/>"
                                               "</PQR>"
                                           "</STU>"
                                           "<VWX/>"
                                           "<YZ1/>"
                                       "</ABC>" },

            { L_ , X("ABC", As0, C0) , "<ABC attr0='val0'>"
                                       "</ABC>" },

            { L_ , X("ABC", As0, C1) , "<ABC attr0='val0'>"
                                           "Lorem ipsum."
                                       "</ABC>" },

            { L_ , X("ABC", As0, C2) , "<ABC attr0='val0'>"
                                           "<DEF/>"
                                       "</ABC>" },

            { L_ , X("ABC", As0, C3) , "<ABC attr0='val0'>"
                                           "<GHI attr0='val0'/>"
                                           "<JKL attr0='val0' attr1='val1'/>"
                                       "</ABC>" },

            { L_ , X("ABC", As0, C4) , "<ABC attr0='val0'>"
                                           "<MNO>"
                                               "Lorem ipsum."
                                           "</MNO>"
                                           "<PQR attr0='val0'>"
                                               "<DEF/>"
                                           "</PQR>"
                                       "</ABC>" },

            { L_ , X("ABC", As0, C5) , "<ABC attr0='val0'>"
                                           "<STU>"
                                               "<MNO>"
                                                   "Lorem ipsum."
                                               "</MNO>"
                                               "<PQR attr0='val0'>"
                                                   "<DEF/>"
                                               "</PQR>"
                                           "</STU>"
                                           "<VWX/>"
                                           "<YZ1/>"
                                       "</ABC>" },

            { L_ , X("ABC", As01, C0), "<ABC attr0='val0' attr1='val1'>"
                                       "</ABC>" },

            { L_ , X("ABC", As01, C1), "<ABC attr0='val0' attr1='val1'>"
                                           "Lorem ipsum."
                                       "</ABC>" },

            { L_ , X("ABC", As01, C2), "<ABC attr0='val0' attr1='val1'>"
                                           "<DEF/>"
                                       "</ABC>" },

            { L_ , X("ABC", As01, C3), "<ABC attr0='val0' attr1='val1'>"
                                           "<GHI attr0='val0'/>"
                                           "<JKL attr0='val0' attr1='val1'/>"
                                       "</ABC>" },

            { L_ , X("ABC", As01, C4), "<ABC attr0='val0' attr1='val1'>"
                                           "<MNO>"
                                               "Lorem ipsum."
                                           "</MNO>"
                                           "<PQR attr0='val0'>"
                                               "<DEF/>"
                                           "</PQR>"
                                       "</ABC>" },

            { L_ , X("ABC", As01, C5), "<ABC attr0='val0' attr1='val1'>"
                                           "<STU>"
                                               "<MNO>"
                                                   "Lorem ipsum."
                                               "</MNO>"
                                               "<PQR attr0='val0'>"
                                                   "<DEF/>"
                                               "</PQR>"
                                           "</STU>"
                                           "<VWX/>"
                                           "<YZ1/>"
                                       "</ABC>" },

            { L_ , X("ABC", As02, C0), "<ABC attr0='val0' attr2=''>"
                                       "</ABC>" },

            { L_ , X("ABC", As02, C1), "<ABC attr0='val0' attr2=''>"
                                           "Lorem ipsum."
                                       "</ABC>" },

            { L_ , X("ABC", As02, C2), "<ABC attr0='val0' attr2=''>"
                                           "<DEF/>"
                                       "</ABC>" },

            { L_ , X("ABC", As02, C3), "<ABC attr0='val0' attr2=''>"
                                           "<GHI attr0='val0'/>"
                                           "<JKL attr0='val0' attr1='val1'/>"
                                       "</ABC>" },

            { L_ , X("ABC", As02, C4), "<ABC attr0='val0' attr2=''>"
                                           "<MNO>"
                                               "Lorem ipsum."
                                           "</MNO>"
                                           "<PQR attr0='val0'>"
                                               "<DEF/>"
                                           "</PQR>"
                                       "</ABC>" },

            { L_ , X("ABC", As02, C5), "<ABC attr0='val0' attr2=''>"
                                           "<STU>"
                                               "<MNO>"
                                                   "Lorem ipsum."
                                               "</MNO>"
                                               "<PQR attr0='val0'>"
                                                   "<DEF/>"
                                               "</PQR>"
                                           "</STU>"
                                           "<VWX/>"
                                           "<YZ1/>"
                                       "</ABC>" }
        };

        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int i = 0; i != NUM_DATA; ++i) {
            const int             LINE            = DATA[i].d_line;
            const TestXmlElement& XML             = DATA[i].d_xml;
            const char *const     EXPECTED_STRING = DATA[i].d_string;

            bdlsb::MemOutStreamBuf xmlStreamBuf;
            bsl::ostream xmlStream(&xmlStreamBuf);
            xmlStream << XML;

            const string_view STRING(xmlStreamBuf.data(),
                                     xmlStreamBuf.length());
            ASSERTV(LINE, EXPECTED_STRING, STRING, EXPECTED_STRING == STRING);
        }

        if (verbose) cout << "\nEnd of Test." << endl;

      } break;
      case 12: {
        // --------------------------------------------------------------------
        // TESTING XML HEADER WITH 'outputXMLHeader' option (DRQS 22278116)
        //
        // Concerns:
        //: 1 If the 'outputXMLHeader' is set to 'true' or not set at all then
        //:   the xml header is always output.
        //:
        //: 2 If the 'outputXMLHeader' is set to 'false' then the xml header is
        //:   not output.
        //
        // Plan:
        //: 1 Create three 'balxml::EncoderOptions' objects.  Set the
        //:   'outputXMLHeader' option in one encoder options object to
        //:   'true' and to 'false' in the another object.  Leave the third
        //:   encoder options object unmodified.
        //:
        //: 2 Create three 'bsl::stringstream' objects.
        //:
        //: 3 Populate a 'MySequence' object.
        //:
        //: 4 Create three 'balxml::Encoder' objects passing the encoder
        //:   options created above.
        //:
        //: 5 Encode the 'MySequence' object onto a 'bsl::stringstream' using
        //:   one of the created 'balxml::Encoder' objects.
        //:
        //: 6 Ensure that the xml header is output only when 'outputXMLAlias'
        //:   is not set to 'false'.
        //:
        //: 8 Repeat steps 1 - 7 for 'BAEXML_COMPACT' encoding style.
        //:
        //: 9 Repeat steps 1 - 8 for 'MyChoice' and 'MySimpleContent' objects.
        //
        // Testing:
        //  Encoding of XML header
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting Encoding of XML Header"
                          << "\n==============================" << endl;

        if (verbose) cout << "\nUsing MySequence with PRETTY style." << endl;
        {
            typedef test::MySequence Type;

            Type mX;  const Type& X = mX;

            mX.attribute1() = 434;
            mX.attribute2() = "test string";

            const char EXPECTED_RESULT1[]
                              = "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                                "<MySequence" XSI ">\n"
                                "    <attribute1>434</attribute1>\n"
                                "    <attribute2>test string</attribute2>\n"
                                "</MySequence>\n";
            const char EXPECTED_RESULT2[]
                              = "<MySequence" XSI ">\n"
                                "    <attribute1>434</attribute1>\n"
                                "    <attribute2>test string</attribute2>\n"
                                "</MySequence>\n";
            const char EXPECTED_RESULT3[]
                              = "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                                "<MySequence" XSI ">\n"
                                "    <attribute1>434</attribute1>\n"
                                "    <attribute2>test string</attribute2>\n"
                                "</MySequence>\n";

            bsl::stringstream result1, result2, result3;

            EncoderOptions options1, options2, options3;
            options1.setEncodingStyle(EncodingStyle::PRETTY);
            options2.setEncodingStyle(EncodingStyle::PRETTY);
            options3.setEncodingStyle(EncodingStyle::PRETTY);
            options1.setOutputXMLHeader(true);
            options2.setOutputXMLHeader(false);

            balxml::Encoder encoder1(&options1, 0, 0);
            int rc = encoder1.encodeToStream(result1, X);
            LOOP_ASSERT(rc, 0 == rc);
            LOOP2_ASSERT(EXPECTED_RESULT1,   result1.str(),
                         EXPECTED_RESULT1 == result1.str());

            balxml::Encoder encoder2(&options2, 0, 0);
            rc = encoder2.encodeToStream(result2, X);
            LOOP_ASSERT(rc, 0 == rc);
            LOOP2_ASSERT(EXPECTED_RESULT2,   result2.str(),
                         EXPECTED_RESULT2 == result2.str());

            balxml::Encoder encoder3(&options3, 0, 0);
            rc = encoder3.encodeToStream(result3, X);
            LOOP_ASSERT(rc, 0 == rc);
            LOOP2_ASSERT(EXPECTED_RESULT3,   result3.str(),
                         EXPECTED_RESULT3 == result3.str());
        }

        if (verbose) cout << "\nUsing MySequence with COMPACT style." << endl;
        {
            typedef test::MySequence Type;

            Type mX;  const Type& X = mX;

            mX.attribute1() = 434;
            mX.attribute2() = "test string";

            const char EXPECTED_RESULT1[]
                              = "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>"
                                "<MySequence" XSI ">"
                                "<attribute1>434</attribute1>"
                                "<attribute2>test string</attribute2>"
                                "</MySequence>";
            const char EXPECTED_RESULT2[]
                              = "<MySequence" XSI ">"
                                "<attribute1>434</attribute1>"
                                "<attribute2>test string</attribute2>"
                                "</MySequence>";
            const char EXPECTED_RESULT3[]
                              = "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>"
                                "<MySequence" XSI ">"
                                "<attribute1>434</attribute1>"
                                "<attribute2>test string</attribute2>"
                                "</MySequence>";

            bsl::stringstream result1, result2, result3;

            EncoderOptions options1, options2, options3;
            options1.setEncodingStyle(EncodingStyle::COMPACT);
            options2.setEncodingStyle(EncodingStyle::COMPACT);
            options3.setEncodingStyle(EncodingStyle::COMPACT);
            options1.setOutputXMLHeader(true);
            options2.setOutputXMLHeader(false);

            balxml::Encoder encoder1(&options1, 0, 0);
            int rc = encoder1.encodeToStream(result1, X);
            LOOP_ASSERT(rc, 0 == rc);
            LOOP2_ASSERT(EXPECTED_RESULT1,   result1.str(),
                         EXPECTED_RESULT1 == result1.str());

            balxml::Encoder encoder2(&options2, 0, 0);
            rc = encoder2.encodeToStream(result2, X);
            LOOP_ASSERT(rc, 0 == rc);
            LOOP2_ASSERT(EXPECTED_RESULT2,   result2.str(),
                         EXPECTED_RESULT2 == result2.str());

            balxml::Encoder encoder3(&options3, 0, 0);
            rc = encoder3.encodeToStream(result3, X);
            LOOP_ASSERT(rc, 0 == rc);
            LOOP2_ASSERT(EXPECTED_RESULT3,   result3.str(),
                         EXPECTED_RESULT3 == result3.str());
        }

        if (verbose) cout << "\nUsing MyChoice with PRETTY style." << endl;
        {
            typedef test::MyChoice Type;

            Type mX;  const Type& X = mX;

            mX.makeSelection1();
            mX.selection1() = 434;

            const char EXPECTED_RESULT1[]
                              = "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                                "<MyChoice" XSI ">\n"
                                "    <selection1>434</selection1>\n"
                                "</MyChoice>\n";
            const char EXPECTED_RESULT2[]
                              = "<MyChoice" XSI ">\n"
                                "    <selection1>434</selection1>\n"
                                "</MyChoice>\n";
            const char EXPECTED_RESULT3[]
                              = "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                                "<MyChoice" XSI ">\n"
                                "    <selection1>434</selection1>\n"
                                "</MyChoice>\n";

            bsl::stringstream result1, result2, result3;

            EncoderOptions options1, options2, options3;
            options1.setEncodingStyle(EncodingStyle::PRETTY);
            options2.setEncodingStyle(EncodingStyle::PRETTY);
            options3.setEncodingStyle(EncodingStyle::PRETTY);
            options1.setOutputXMLHeader(true);
            options2.setOutputXMLHeader(false);

            balxml::Encoder encoder1(&options1, 0, 0);
            int rc = encoder1.encodeToStream(result1, X);
            LOOP_ASSERT(rc, 0 == rc);
            LOOP2_ASSERT(EXPECTED_RESULT1,   result1.str(),
                         EXPECTED_RESULT1 == result1.str());

            balxml::Encoder encoder2(&options2, 0, 0);
            rc = encoder2.encodeToStream(result2, X);
            LOOP_ASSERT(rc, 0 == rc);
            LOOP2_ASSERT(EXPECTED_RESULT2,   result2.str(),
                         EXPECTED_RESULT2 == result2.str());

            balxml::Encoder encoder3(&options3, 0, 0);
            rc = encoder3.encodeToStream(result3, X);
            LOOP_ASSERT(rc, 0 == rc);
            LOOP2_ASSERT(EXPECTED_RESULT3,   result3.str(),
                         EXPECTED_RESULT3 == result3.str());
        }

        if (verbose) cout << "\nUsing MyChoice with COMPACT style." << endl;
        {
            typedef test::MyChoice Type;

            Type mX;  const Type& X = mX;

            mX.makeSelection1();
            mX.selection1() = 434;

            const char EXPECTED_RESULT1[]
                              = "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>"
                                "<MyChoice" XSI ">"
                                "<selection1>434</selection1>"
                                "</MyChoice>";
            const char EXPECTED_RESULT2[]
                              = "<MyChoice" XSI ">"
                                "<selection1>434</selection1>"
                                "</MyChoice>";
            const char EXPECTED_RESULT3[]
                              = "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>"
                                "<MyChoice" XSI ">"
                                "<selection1>434</selection1>"
                                "</MyChoice>";

            bsl::stringstream result1, result2, result3;

            EncoderOptions options1, options2, options3;
            options1.setEncodingStyle(EncodingStyle::COMPACT);
            options2.setEncodingStyle(EncodingStyle::COMPACT);
            options3.setEncodingStyle(EncodingStyle::COMPACT);
            options1.setOutputXMLHeader(true);
            options2.setOutputXMLHeader(false);

            balxml::Encoder encoder1(&options1, 0, 0);
            int rc = encoder1.encodeToStream(result1, X);
            LOOP_ASSERT(rc, 0 == rc);
            LOOP2_ASSERT(EXPECTED_RESULT1,   result1.str(),
                         EXPECTED_RESULT1 == result1.str());

            balxml::Encoder encoder2(&options2, 0, 0);
            rc = encoder2.encodeToStream(result2, X);
            LOOP_ASSERT(rc, 0 == rc);
            LOOP2_ASSERT(EXPECTED_RESULT2,   result2.str(),
                         EXPECTED_RESULT2 == result2.str());

            balxml::Encoder encoder3(&options3, 0, 0);
            rc = encoder3.encodeToStream(result3, X);
            LOOP_ASSERT(rc, 0 == rc);
            LOOP2_ASSERT(EXPECTED_RESULT3,   result3.str(),
                         EXPECTED_RESULT3 == result3.str());
        }

        if (verbose) cout << "\nUsing MySimpleContent with PRETTY." << endl;
        {
            typedef test::MySimpleContent Type;

            Type mX;  const Type& X = mX;

            mX.attribute1() = true;
            mX.attribute2() = "Hello World!";
            mX.theContent() = "  Some Stuff ";

            const char EXPECTED_RESULT1[]
                          = "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                            "<MySimpleContent" XSI "\n    attribute1=\"true\" "
                            "attribute2=\"Hello World!\">"
                            "  Some Stuff "
                            "</MySimpleContent>\n";
            const char EXPECTED_RESULT2[]
                          = "<MySimpleContent" XSI "\n    attribute1=\"true\" "
                            "attribute2=\"Hello World!\">"
                            "  Some Stuff "
                            "</MySimpleContent>\n";
            const char EXPECTED_RESULT3[]
                          = "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                            "<MySimpleContent" XSI "\n    attribute1=\"true\" "
                            "attribute2=\"Hello World!\">"
                            "  Some Stuff "
                            "</MySimpleContent>\n";

            bsl::stringstream result1, result2, result3;

            EncoderOptions options1, options2, options3;
            options1.setEncodingStyle(EncodingStyle::PRETTY);
            options2.setEncodingStyle(EncodingStyle::PRETTY);
            options3.setEncodingStyle(EncodingStyle::PRETTY);
            options1.setOutputXMLHeader(true);
            options2.setOutputXMLHeader(false);

            balxml::Encoder encoder1(&options1, 0, 0);
            int rc = encoder1.encodeToStream(result1, X);
            LOOP_ASSERT(rc, 0 == rc);
            LOOP2_ASSERT(EXPECTED_RESULT1,   result1.str(),
                         EXPECTED_RESULT1 == result1.str());

            balxml::Encoder encoder2(&options2, 0, 0);
            rc = encoder2.encodeToStream(result2, X);
            LOOP_ASSERT(rc, 0 == rc);
            LOOP2_ASSERT(EXPECTED_RESULT2,   result2.str(),
                         EXPECTED_RESULT2 == result2.str());

            balxml::Encoder encoder3(&options3, 0, 0);
            rc = encoder3.encodeToStream(result3, X);
            LOOP_ASSERT(rc, 0 == rc);
            LOOP2_ASSERT(EXPECTED_RESULT3,   result3.str(),
                         EXPECTED_RESULT3 == result3.str());
        }

        if (verbose) cout << "\nUsing MySimpleContent with COMPACT." << endl;
        {
            typedef test::MySimpleContent Type;

            Type mX;  const Type& X = mX;

            mX.attribute1() = true;
            mX.attribute2() = "Hello World!";
            mX.theContent() = "  Some Stuff ";

            const char EXPECTED_RESULT1[]
                            = "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>"
                              "<MySimpleContent" XSI " attribute1=\"true\" "
                              "attribute2=\"Hello World!\">"
                              "  Some Stuff "
                              "</MySimpleContent>";
            const char EXPECTED_RESULT2[]
                            = "<MySimpleContent" XSI " attribute1=\"true\" "
                              "attribute2=\"Hello World!\">"
                              "  Some Stuff "
                              "</MySimpleContent>";
            const char EXPECTED_RESULT3[]
                            = "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>"
                              "<MySimpleContent" XSI " attribute1=\"true\" "
                              "attribute2=\"Hello World!\">"
                              "  Some Stuff "
                              "</MySimpleContent>";

            bsl::stringstream result1, result2, result3;

            EncoderOptions options1, options2, options3;
            options1.setEncodingStyle(EncodingStyle::COMPACT);
            options2.setEncodingStyle(EncodingStyle::COMPACT);
            options3.setEncodingStyle(EncodingStyle::COMPACT);
            options1.setOutputXMLHeader(true);
            options2.setOutputXMLHeader(false);

            balxml::Encoder encoder1(&options1, 0, 0);
            int rc = encoder1.encodeToStream(result1, X);
            LOOP_ASSERT(rc, 0 == rc);
            LOOP2_ASSERT(EXPECTED_RESULT1,   result1.str(),
                         EXPECTED_RESULT1 == result1.str());

            balxml::Encoder encoder2(&options2, 0, 0);
            rc = encoder2.encodeToStream(result2, X);
            LOOP_ASSERT(rc, 0 == rc);
            LOOP2_ASSERT(EXPECTED_RESULT2,   result2.str(),
                         EXPECTED_RESULT2 == result2.str());

            balxml::Encoder encoder3(&options3, 0, 0);
            rc = encoder3.encodeToStream(result3, X);
            LOOP_ASSERT(rc, 0 == rc);
            LOOP2_ASSERT(EXPECTED_RESULT3,   result3.str(),
                         EXPECTED_RESULT3 == result3.str());
        }

        if (verbose) cout << "\nEnd of Test." << endl;
      } break;
      case 11: {
        // --------------------------------------------------------------------
        // TESTING NILLABLES WITH 'outputXSIAlias' option (DRQS 29523606)
        //
        // Concerns:
        //: 1 If the 'outputXSIAlias' is set to 'true' or not set at all then
        //:   the xsi prefix is always aliased to
        //:   'xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\"'.
        //:
        //: 2 If the 'outputXSIAlias' is set to 'false' then the xsi prefix is
        //:   not aliased.
        //:
        //: 3 Nillable elements are encoded with 'xsi:nil=true' only if
        //:   'outputXSIAlias' is not set to 'false'.
        //:
        //: 4 The specification of the 'objectNamespace' and 'schemaLocation'
        //:   options interacts correctly with the 'outputXSIAlias' option.
        //
        // Plan:
        //: 1 Create three 'balxml::EncoderOptions' objects.  Set the
        //:   'outputXSIAlias' option in one encoder options object to
        //:   'true' and to 'false' in the another object.  Leave the third
        //:   encoder options object unmodified.
        //:
        //: 2 Create three 'bsl::stringstream' objects.
        //:
        //: 3 Populate a 'MySequenceWithNillables' object ensuring that one of
        //:   its nillable elements is null.
        //:
        //: 4 Create three 'balxml::Encoder' objects passing the encoder
        //:   options created above.
        //:
        //: 5 Encode the 'MySequenceWithNillables' object onto a
        //:   'bsl::stringstream' using one of the created 'balxml::Encoder'
        //:   objects.
        //:
        //: 6 Ensure that the xsi prefix is aliased only when 'outputXSIAlias'
        //:   is not set to 'false'.
        //:
        //: 7 Also ensure that the 'xsi:nil' attribute is output only when
        //:   'outputXSIAlias' is not set to 'false'.
        //:
        //: 8 Repeat steps 1 - 7 for 'BAEXML_COMPACT' encoding style.
        //:
        //: 9 Repeat steps 1 - 7 specifying an 'objectNamespace'.
        //:
        //:10 Repeat steps 1 - 7 specifying a 'schemaLocation'.
        //
        // Testing:
        //  Encoding of Nillables
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting Encoding of Nillables"
                          << "\n=============================" << endl;

        static const char OBJ_NS[] =
            "http://bloomberg.com/schemas/balxml_encoder.t.xsd";
        static const char PRETTY_NS_ATTR[] = "\n    "
            "xmlns=\"http://bloomberg.com/schemas/balxml_encoder.t.xsd\"";
        static const char PRETTY_XSI_ATTR[] = "\n    "
            "xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\"";
        static const char COMPACT_NS_ATTR[] =
            " xmlns=\"http://bloomberg.com/schemas/balxml_encoder.t.xsd\"";
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
                "    <attribute1%s/>\n"
                "    <attribute2>test string</attribute2>\n"
                "    <attribute3>\n"
                "        <attribute1>987</attribute1>\n"
                "        <attribute2>inner</attribute2>\n"
                "    </attribute3>\n"
                "</MySequenceWithNillables>\n",

                "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                "<MySequenceWithNillables%s%s>\n"
                "    <attribute1>123</attribute1>\n"
                "    <attribute2%s/>\n"
                "    <attribute3>\n"
                "        <attribute1>987</attribute1>\n"
                "        <attribute2>inner</attribute2>\n"
                "    </attribute3>\n"
                "</MySequenceWithNillables>\n",

                "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                "<MySequenceWithNillables%s%s>\n"
                "    <attribute1>123</attribute1>\n"
                "    <attribute2>test string</attribute2>\n"
                "    <attribute3%s/>\n"
                "</MySequenceWithNillables>\n",
            };

            static const char *PRETTY_RESULT_ABSENT_NULLS[3] = {
                "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                "<MySequenceWithNillables%s%s>\n"
                "    <attribute2>test string</attribute2>\n"
                "    <attribute3>\n"
                "        <attribute1>987</attribute1>\n"
                "        <attribute2>inner</attribute2>\n"
                "    </attribute3>\n"
                "</MySequenceWithNillables>\n",

                "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                "<MySequenceWithNillables%s%s>\n"
                "    <attribute1>123</attribute1>\n"
                "    <attribute3>\n"
                "        <attribute1>987</attribute1>\n"
                "        <attribute2>inner</attribute2>\n"
                "    </attribute3>\n"
                "</MySequenceWithNillables>\n",

                "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                "<MySequenceWithNillables%s%s>\n"
                "    <attribute1>123</attribute1>\n"
                "    <attribute2>test string</attribute2>\n"
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

            char expResult1[1000], expResult2[1000], expResult3[1000];
            const char *EXPECTED_RESULT1 = expResult1;
            const char *EXPECTED_RESULT2 = expResult2;
            const char *EXPECTED_RESULT3 = expResult3;

            EncoderOptions options1, options2, options3;
            options1.setOutputXSIAlias(true);
            options2.setOutputXSIAlias(false);

            if (verbose) cout << "PRETTY without object namespace" << endl;

            // Format expected result without object namespace and, therefore,
            // without 'xmlns', 'xmlns:xsi', and 'xsi:nil':
            bsl::sprintf(expResult1, PRETTY_RESULT_ABSENT_NULLS[i], "",
                         PRETTY_XSI_ATTR, "");
            bsl::sprintf(expResult2, PRETTY_RESULT_ABSENT_NULLS[i], "",
                         "", "");
            bsl::sprintf(expResult3, PRETTY_RESULT_ABSENT_NULLS[i], "",
                         PRETTY_XSI_ATTR, "");

            bsl::stringstream result1, result2, result3;

            options1.setEncodingStyle(EncodingStyle::PRETTY);
            options2.setEncodingStyle(EncodingStyle::PRETTY);
            options3.setEncodingStyle(EncodingStyle::PRETTY);

            int rc;
            balxml::Encoder encoder1(&options1, 0, 0);
            rc = encoder1.encodeToStream(result1, X);
            LOOP_ASSERT(rc, 0 == rc);
            LOOP2_ASSERT(EXPECTED_RESULT1,   result1.str(),
                         EXPECTED_RESULT1 == result1.str());

            balxml::Encoder encoder2(&options2, 0, 0);
            rc = encoder2.encodeToStream(result2, X);
            LOOP_ASSERT(rc, 0 == rc);
            LOOP2_ASSERT(EXPECTED_RESULT2,   result2.str(),
                         EXPECTED_RESULT2 == result2.str());

            balxml::Encoder encoder3(&options3, 0, 0);
            rc = encoder3.encodeToStream(result3, X);
            LOOP_ASSERT(rc, 0 == rc);
            LOOP2_ASSERT(EXPECTED_RESULT3,   result3.str(),
                         EXPECTED_RESULT3 == result3.str());


            if (verbose) cout << "PRETTY with object namespace" << endl;

            // Format expected result with object namespace and, therefore,
            // with 'xmlns', 'xmlns:xsi', and 'xsi:nil':
            bsl::sprintf(expResult1, PRETTY_RESULT[i],
                         PRETTY_NS_ATTR, PRETTY_XSI_ATTR, NIL_ATTR);
            bsl::sprintf(expResult2, PRETTY_RESULT_ABSENT_NULLS[i],
                         PRETTY_NS_ATTR, "", "");
            bsl::sprintf(expResult3, PRETTY_RESULT[i],
                         PRETTY_NS_ATTR, PRETTY_XSI_ATTR, NIL_ATTR);

            result1.str("");
            result2.str("");
            result3.str("");

            options1.setObjectNamespace(OBJ_NS);
            options1.setEncodingStyle(EncodingStyle::PRETTY);

            options2.setObjectNamespace(OBJ_NS);
            options2.setEncodingStyle(EncodingStyle::PRETTY);

            options3.setObjectNamespace(OBJ_NS);
            options3.setEncodingStyle(EncodingStyle::PRETTY);

            rc = encoder1.encodeToStream(result1, X);
            LOOP_ASSERT(rc, 0 == rc);
            LOOP2_ASSERT(EXPECTED_RESULT1,   result1.str(),
                         EXPECTED_RESULT1 == result1.str());

            rc = encoder2.encodeToStream(result2, X);
            LOOP_ASSERT(rc, 0 == rc);
            LOOP2_ASSERT(EXPECTED_RESULT2,   result2.str(),
                         EXPECTED_RESULT2 == result2.str());

            rc = encoder3.encodeToStream(result3, X);
            LOOP_ASSERT(rc, 0 == rc);
            LOOP2_ASSERT(EXPECTED_RESULT3,   result3.str(),
                         EXPECTED_RESULT3 == result3.str());

            static const char *COMPACT_RESULT[3] = {
                "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>"
                "<MySequenceWithNillables%s%s>"
                "<attribute1%s/>"
                "<attribute2>test string</attribute2>"
                "<attribute3>"
                "<attribute1>987</attribute1>"
                "<attribute2>inner</attribute2>"
                "</attribute3>"
                "</MySequenceWithNillables>",

                "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>"
                "<MySequenceWithNillables%s%s>"
                "<attribute1>123</attribute1>"
                "<attribute2%s/>"
                "<attribute3>"
                "<attribute1>987</attribute1>"
                "<attribute2>inner</attribute2>"
                "</attribute3>"
                "</MySequenceWithNillables>",

                "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>"
                "<MySequenceWithNillables%s%s>"
                "<attribute1>123</attribute1>"
                "<attribute2>test string</attribute2>"
                "<attribute3%s/>"
                "</MySequenceWithNillables>",
            };

            static const char *COMPACT_RESULT_ABSENT_NULLS[3] = {
                "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>"
                "<MySequenceWithNillables%s%s>"
                "<attribute2>test string</attribute2>"
                "<attribute3>"
                "<attribute1>987</attribute1>"
                "<attribute2>inner</attribute2>"
                "</attribute3>"
                "</MySequenceWithNillables>",

                "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>"
                "<MySequenceWithNillables%s%s>"
                "<attribute1>123</attribute1>"
                "<attribute3>"
                "<attribute1>987</attribute1>"
                "<attribute2>inner</attribute2>"
                "</attribute3>"
                "</MySequenceWithNillables>",

                "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>"
                "<MySequenceWithNillables%s%s>"
                "<attribute1>123</attribute1>"
                "<attribute2>test string</attribute2>"
                "</MySequenceWithNillables>",
            };

            if (verbose) cout << "COMPACT without object namespace" << endl;

            // Format expected result without object namespace and, therefore,
            // without 'xmlns', 'xmlns:xsi', and 'xsi:nil':
            bsl::sprintf(expResult1, COMPACT_RESULT_ABSENT_NULLS[i], "",
                         COMPACT_XSI_ATTR, "");
            bsl::sprintf(
                expResult2, COMPACT_RESULT_ABSENT_NULLS[i], "", "", "");
            bsl::sprintf(expResult3, COMPACT_RESULT_ABSENT_NULLS[i], "",
                         COMPACT_XSI_ATTR, "");

            result1.str("");
            result2.str("");
            result3.str("");

            options1.setObjectNamespace("");
            options1.setEncodingStyle(EncodingStyle::COMPACT);

            options2.setObjectNamespace("");
            options2.setEncodingStyle(EncodingStyle::COMPACT);

            options3.setObjectNamespace("");
            options3.setEncodingStyle(EncodingStyle::COMPACT);

            rc = encoder1.encodeToStream(result1, X);
            LOOP_ASSERT(rc, 0 == rc);
            LOOP2_ASSERT(EXPECTED_RESULT1,   result1.str(),
                         EXPECTED_RESULT1 == result1.str());

            rc = encoder2.encodeToStream(result2, X);
            LOOP_ASSERT(rc, 0 == rc);
            LOOP2_ASSERT(EXPECTED_RESULT2,   result2.str(),
                         EXPECTED_RESULT2 == result2.str());

            rc = encoder3.encodeToStream(result3, X);
            LOOP_ASSERT(rc, 0 == rc);
            LOOP2_ASSERT(EXPECTED_RESULT3,   result3.str(),
                         EXPECTED_RESULT3 == result3.str());

            if (verbose) cout << "COMPACT with object namespace" << endl;

            // Format expected result with object namespace and, therefore,
            // with 'xmlns', 'xmlns:xsi', and 'xsi:nil':
            bsl::sprintf(expResult1, COMPACT_RESULT[i],
                         COMPACT_NS_ATTR, COMPACT_XSI_ATTR, NIL_ATTR);
            bsl::sprintf(expResult2, COMPACT_RESULT_ABSENT_NULLS[i],
                         COMPACT_NS_ATTR, "", "");
            bsl::sprintf(expResult3, COMPACT_RESULT[i],
                         COMPACT_NS_ATTR, COMPACT_XSI_ATTR, NIL_ATTR);

            result1.str("");
            result2.str("");
            result3.str("");

            options1.setObjectNamespace(OBJ_NS);
            options1.setEncodingStyle(EncodingStyle::COMPACT);

            options2.setObjectNamespace(OBJ_NS);
            options2.setEncodingStyle(EncodingStyle::COMPACT);

            options3.setObjectNamespace(OBJ_NS);
            options3.setEncodingStyle(EncodingStyle::COMPACT);

            rc = encoder1.encodeToStream(result1, X);
            LOOP_ASSERT(rc, 0 == rc);
            LOOP2_ASSERT(EXPECTED_RESULT1,   result1.str(),
                         EXPECTED_RESULT1 == result1.str());

            rc = encoder2.encodeToStream(result2, X);
            LOOP_ASSERT(rc, 0 == rc);
            LOOP2_ASSERT(EXPECTED_RESULT2,   result2.str(),
                         EXPECTED_RESULT2 == result2.str());

            rc = encoder3.encodeToStream(result3, X);
            LOOP_ASSERT(rc, 0 == rc);
            LOOP2_ASSERT(EXPECTED_RESULT3,   result3.str(),
                         EXPECTED_RESULT3 == result3.str());
        } // End for i

        if (verbose) cout << "Testing with 'schemaLocation' specified" << endl;
        {
            Type mX; const Type& X = mX;

            bsl::stringstream result1, result2, result3;
            EncoderOptions options1, options2, options3;
            balxml::Encoder encoder1(&options1);
            balxml::Encoder encoder2(&options2);
            balxml::Encoder encoder3(&options3);

            options1.setOutputXSIAlias(true);
            options1.setSchemaLocation("bas/bassvc/bassvc.xsd");
            options2.setOutputXSIAlias(false);
            options2.setSchemaLocation("bas/bassvc/bassvc.xsd");
            options3.setSchemaLocation("bas/bassvc/bassvc.xsd");

            if (verbose) cout << "PRETTY with object namespace" << endl;

            result1.str("");
            result2.str("");
            result3.str("");

            options1.setObjectNamespace(OBJ_NS);
            options1.setEncodingStyle(EncodingStyle::PRETTY);

            options2.setObjectNamespace(OBJ_NS);
            options2.setEncodingStyle(EncodingStyle::PRETTY);

            options3.setObjectNamespace(OBJ_NS);
            options3.setEncodingStyle(EncodingStyle::PRETTY);

            int rc = encoder1.encodeToStream(result1, X);
            rc = encoder2.encodeToStream(result2, X);
            rc = encoder3.encodeToStream(result3, X);

            (void)rc;

            if (veryVerbose) {
                P(result1.str());
                P(result2.str());
                P(result3.str());
            }
        }

        if (verbose) cout << "\nEnd of Test." << endl;

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
            "http://bloomberg.com/schemas/balxml_encoder.t.xsd";
        static const char PRETTY_NS_ATTR[] = "\n    "
            "xmlns=\"http://bloomberg.com/schemas/balxml_encoder.t.xsd\"";
        static const char PRETTY_XSI_ATTR[] = "\n    "
            "xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\"";
        static const char COMPACT_NS_ATTR[] =
            " xmlns=\"http://bloomberg.com/schemas/balxml_encoder.t.xsd\"";
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
                "    <attribute1%s/>\n"
                "    <attribute2>test string</attribute2>\n"
                "    <attribute3>\n"
                "        <attribute1>987</attribute1>\n"
                "        <attribute2>inner</attribute2>\n"
                "    </attribute3>\n"
                "</MySequenceWithNillables>\n",

                "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                "<MySequenceWithNillables%s%s>\n"
                "    <attribute1>123</attribute1>\n"
                "    <attribute2%s/>\n"
                "    <attribute3>\n"
                "        <attribute1>987</attribute1>\n"
                "        <attribute2>inner</attribute2>\n"
                "    </attribute3>\n"
                "</MySequenceWithNillables>\n",

                "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                "<MySequenceWithNillables%s%s>\n"
                "    <attribute1>123</attribute1>\n"
                "    <attribute2>test string</attribute2>\n"
                "    <attribute3%s/>\n"
                "</MySequenceWithNillables>\n",
            };

            static const char *PRETTY_RESULT_ABSENT_NULLS[3] = {
                "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                "<MySequenceWithNillables%s%s>\n"
                "    <attribute2>test string</attribute2>\n"
                "    <attribute3>\n"
                "        <attribute1>987</attribute1>\n"
                "        <attribute2>inner</attribute2>\n"
                "    </attribute3>\n"
                "</MySequenceWithNillables>\n",

                "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                "<MySequenceWithNillables%s%s>\n"
                "    <attribute1>123</attribute1>\n"
                "    <attribute3>\n"
                "        <attribute1>987</attribute1>\n"
                "        <attribute2>inner</attribute2>\n"
                "    </attribute3>\n"
                "</MySequenceWithNillables>\n",

                "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                "<MySequenceWithNillables%s%s>\n"
                "    <attribute1>123</attribute1>\n"
                "    <attribute2>test string</attribute2>\n"
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

            // Format expected result without object namespace and, therefore,
            // without 'xmlns', 'xmlns:xsi', and 'xsi:nil':
            bsl::sprintf(expectedResult, PRETTY_RESULT_ABSENT_NULLS[i], "",
                         PRETTY_XSI_ATTR, "");

            if (veryVerbose) { T_ P_(i) P_(X) P(EXPECTED_RESULT) }

            bsl::stringstream result;

            EncoderOptions options;
            options.setEncodingStyle(EncodingStyle::PRETTY);

            balxml::Encoder encoder(&options, 0, 0);
            int rc = encoder.encodeToStream(result, X);

            LOOP_ASSERT(rc, 0 == rc);
            LOOP2_ASSERT(EXPECTED_RESULT,   result.str(),
                         EXPECTED_RESULT == result.str());

            if (verbose) cout << "PRETTY with object namespace" << endl;

            // Format expected result with object namespace and, therefore,
            // with 'xmlns', 'xmlns:xsi', and 'xsi:nil':
            bsl::sprintf(expectedResult, PRETTY_RESULT[i],
                         PRETTY_NS_ATTR, PRETTY_XSI_ATTR, NIL_ATTR);

            if (veryVerbose) { T_ P_(i) P_(X) P(EXPECTED_RESULT) }

            result.str("");

            options.setObjectNamespace(OBJ_NS);
            options.setEncodingStyle(EncodingStyle::PRETTY);

            rc = encoder.encodeToStream(result, X);

            LOOP_ASSERT(rc, 0 == rc);
            LOOP2_ASSERT(EXPECTED_RESULT,   result.str(),
                         EXPECTED_RESULT == result.str());

            static const char *COMPACT_RESULT[3] = {
                "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>"
                "<MySequenceWithNillables%s%s>"
                    "<attribute1%s/>"
                    "<attribute2>test string</attribute2>"
                    "<attribute3>"
                        "<attribute1>987</attribute1>"
                        "<attribute2>inner</attribute2>"
                    "</attribute3>"
                "</MySequenceWithNillables>",

                "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>"
                "<MySequenceWithNillables%s%s>"
                    "<attribute1>123</attribute1>"
                    "<attribute2%s/>"
                    "<attribute3>"
                        "<attribute1>987</attribute1>"
                        "<attribute2>inner</attribute2>"
                    "</attribute3>"
                "</MySequenceWithNillables>",

                "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>"
                "<MySequenceWithNillables%s%s>"
                    "<attribute1>123</attribute1>"
                    "<attribute2>test string</attribute2>"
                    "<attribute3%s/>"
                "</MySequenceWithNillables>",
            };

            static const char *COMPACT_RESULT_ABSENT_NULLS[3] = {
                "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>"
                "<MySequenceWithNillables%s%s>"
                    "<attribute2>test string</attribute2>"
                    "<attribute3>"
                        "<attribute1>987</attribute1>"
                        "<attribute2>inner</attribute2>"
                    "</attribute3>"
                "</MySequenceWithNillables>",

                "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>"
                "<MySequenceWithNillables%s%s>"
                    "<attribute1>123</attribute1>"
                    "<attribute3>"
                        "<attribute1>987</attribute1>"
                        "<attribute2>inner</attribute2>"
                    "</attribute3>"
                "</MySequenceWithNillables>",

                "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>"
                "<MySequenceWithNillables%s%s>"
                    "<attribute1>123</attribute1>"
                    "<attribute2>test string</attribute2>"
                "</MySequenceWithNillables>",
            };


            if (verbose) cout << "COMPACT without object namespace" << endl;

            // Format expected result without object namespace and, therefore,
            // without 'xmlns', 'xmlns:xsi', and 'xsi:nil':
            bsl::sprintf(expectedResult, COMPACT_RESULT_ABSENT_NULLS[i], "",
                         COMPACT_XSI_ATTR, "");

            if (veryVerbose) { T_ P_(i) P_(X) P(EXPECTED_RESULT) }

            result.str("");

            options.setObjectNamespace("");
            options.setEncodingStyle(EncodingStyle::COMPACT);

            rc = encoder.encodeToStream(result, X);

            LOOP_ASSERT(rc, 0 == rc);
            LOOP2_ASSERT(EXPECTED_RESULT,   result.str(),
                         EXPECTED_RESULT == result.str());

            if (verbose) cout << "COMPACT with object namespace" << endl;

            // Format expected result with object namespace and, therefore,
            // with 'xmlns', 'xmlns:xsi', and 'xsi:nil':
            bsl::sprintf(expectedResult, COMPACT_RESULT[i],
                         COMPACT_NS_ATTR, COMPACT_XSI_ATTR, NIL_ATTR);

            if (veryVerbose) { T_ P_(i) P_(X) P(EXPECTED_RESULT) }

            result.str("");

            options.setObjectNamespace(OBJ_NS);
            options.setEncodingStyle(EncodingStyle::COMPACT);

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
                            "<MySimpleContent" XSI "\n    attribute1=\"true\" "
                            "attribute2=\"Hello World!\">"
                            "  Some Stuff "
                            "</MySimpleContent>\n";

            if (veryVerbose) {
                T_ P_(X) P(EXPECTED_RESULT)
            }

            bsl::stringstream result;

            EncoderOptions options;
            options.setEncodingStyle(EncodingStyle::PRETTY);

            balxml::Encoder encoder(&options, 0, 0);
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
                              "<MySimpleContent" XSI " attribute1=\"true\" "
                              "attribute2=\"Hello World!\">"
                              "  Some Stuff "
                              "</MySimpleContent>";

            if (veryVerbose) {
                T_ P_(X) P(EXPECTED_RESULT)
            }

            bsl::stringstream result;

            EncoderOptions options;
            options.setEncodingStyle(EncodingStyle::COMPACT);

            balxml::Encoder encoder(&options, 0, 0);
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
                        "<MySimpleIntContent" XSI "\n    attribute1=\"false\" "
                        "attribute2=\"Hello World!\">"
                        "837"
                        "</MySimpleIntContent>\n";

            if (veryVerbose) {
                T_ P_(X) P(EXPECTED_RESULT)
            }

            bsl::stringstream result;

            EncoderOptions options;
            options.setEncodingStyle(EncodingStyle::PRETTY);

            balxml::Encoder encoder(&options, 0, 0);
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
                          "<MySimpleIntContent" XSI " attribute1=\"false\" "
                          "attribute2=\"Hello World!\">"
                          "837"
                          "</MySimpleIntContent>";

            if (veryVerbose) {
                T_ P_(X) P(EXPECTED_RESULT)
            }

            bsl::stringstream result;

            EncoderOptions options;
            options.setEncodingStyle(EncodingStyle::COMPACT);

            balxml::Encoder encoder(&options, 0, 0);
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
                               "<MySequenceWithAttributes\n   " XSI " "
                               "attribute1=\"34\">\n"
                               "    <element1>45</element1>\n"
                               "    <element2>Hello</element2>\n"
                               "</MySequenceWithAttributes>\n";

            if (veryVerbose) {
                T_ P_(X) P(EXPECTED_RESULT)
            }

            bsl::stringstream result;

            EncoderOptions options;
            options.setEncodingStyle(EncodingStyle::PRETTY);

            balxml::Encoder encoder(&options, 0, 0);
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
                              "<MySequenceWithAttributes\n   " XSI " "
                              "attribute1=\"34\"\n    attribute2=\"World!\">\n"
                              "    <element1>45</element1>\n"
                              "    <element2>Hello</element2>\n"
                              "</MySequenceWithAttributes>\n";

            if (veryVerbose) {
                T_ P_(X) P(EXPECTED_RESULT)
            }

            bsl::stringstream result;

            EncoderOptions options;
            options.setEncodingStyle(EncodingStyle::PRETTY);

            balxml::Encoder encoder(&options, 0, 0);
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
                               "<MySequenceWithAttributes" XSI " "
                               "attribute1=\"34\" "
                               "attribute2=\"World!\">"
                               "<element1>45</element1>"
                               "<element2>Hello</element2>"
                               "</MySequenceWithAttributes>";

            if (veryVerbose) {
                T_ P_(X) P(EXPECTED_RESULT)
            }

            bsl::stringstream result;

            EncoderOptions options;
            options.setEncodingStyle(EncodingStyle::COMPACT);

            balxml::Encoder encoder(&options, 0, 0);
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
            mX.choice().makeMyChoice1(67);
            mX.attribute2() = "Hello";

            const char EXPECTED_RESULT[]
                              = "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                                "<MySequenceWithAnonymousChoice\n   " XSI ">\n"
                                "    <attribute1>34</attribute1>\n"
                                "    <myChoice1>67</myChoice1>\n"
                                "    <attribute2>Hello</attribute2>\n"
                                "</MySequenceWithAnonymousChoice>\n";

            if (veryVerbose) {
                T_ P_(X) P(EXPECTED_RESULT)
            }

            bsl::stringstream result;

            EncoderOptions options;
            options.setEncodingStyle(EncodingStyle::PRETTY);

            balxml::Encoder encoder(&options, 0, 0);
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
            mX.choice().makeMyChoice2("World!");
            mX.attribute2() = "Hello";

            const char EXPECTED_RESULT[]
                              = "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                                "<MySequenceWithAnonymousChoice\n   " XSI ">\n"
                                "    <attribute1>34</attribute1>\n"
                                "    <myChoice2>World!</myChoice2>\n"
                                "    <attribute2>Hello</attribute2>\n"
                                "</MySequenceWithAnonymousChoice>\n";

            if (veryVerbose) {
                T_ P_(X) P(EXPECTED_RESULT)
            }

            bsl::stringstream result;

            EncoderOptions options;
            options.setEncodingStyle(EncodingStyle::PRETTY);

            balxml::Encoder encoder(&options, 0, 0);
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
            mX.choice().makeMyChoice2("World!");
            mX.attribute2() = "Hello";

            const char EXPECTED_RESULT[] =
                                "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>"
                                "<MySequenceWithAnonymousChoice" XSI ">"
                                "<attribute1>34</attribute1>"
                                "<myChoice2>World!</myChoice2>"
                                "<attribute2>Hello</attribute2>"
                                "</MySequenceWithAnonymousChoice>";

            if (veryVerbose) {
                T_ P_(X) P(EXPECTED_RESULT)
            }

            bsl::stringstream result;

            EncoderOptions options;
            options.setEncodingStyle(EncodingStyle::COMPACT);

            balxml::Encoder encoder(&options, 0, 0);
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
                                "<MySequenceWithNullables\n   " XSI ">\n"
                                "    <attribute2>test string</attribute2>\n"
                                "    <attribute3>\n"
                                "        <attribute1>987</attribute1>\n"
                                "        <attribute2>inner</attribute2>\n"
                                "    </attribute3>\n"
                                "</MySequenceWithNullables>\n",

                                "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                                "<MySequenceWithNullables\n   " XSI ">\n"
                                "    <attribute1>123</attribute1>\n"
                                "    <attribute3>\n"
                                "        <attribute1>987</attribute1>\n"
                                "        <attribute2>inner</attribute2>\n"
                                "    </attribute3>\n"
                                "</MySequenceWithNullables>\n",

                                "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                                "<MySequenceWithNullables\n   " XSI ">\n"
                                "    <attribute1>123</attribute1>\n"
                                "    <attribute2>test string</attribute2>\n"
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
                options.setEncodingStyle(EncodingStyle::PRETTY);

                balxml::Encoder encoder(&options, 0, 0);
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
                                "<MySequenceWithNullables" XSI ">"
                                "<attribute2>test string</attribute2>"
                                "<attribute3>"
                                "<attribute1>987</attribute1>"
                                "<attribute2>inner</attribute2>"
                                "</attribute3>"
                                "</MySequenceWithNullables>",

                                "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>"
                                "<MySequenceWithNullables" XSI ">"
                                "<attribute1>123</attribute1>"
                                "<attribute3>"
                                "<attribute1>987</attribute1>"
                                "<attribute2>inner</attribute2>"
                                "</attribute3>"
                                "</MySequenceWithNullables>",

                                "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>"
                                "<MySequenceWithNullables" XSI ">"
                                "<attribute1>123</attribute1>"
                                "<attribute2>test string</attribute2>"
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
                options.setEncodingStyle(EncodingStyle::COMPACT);

                balxml::Encoder encoder(&options, 0, 0);
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
                                "<MySequenceWithArrays" XSI ">\n"
                                "    <attribute1>123</attribute1>\n"
                                "    <attribute1>456</attribute1>\n"
                                "    <attribute1>927</attribute1>\n"
                                "    <attribute2>YWJjZA==</attribute2>\n"
                                "    <attribute3>61626364</attribute3>\n"
                                "    <attribute4>abcd</attribute4>\n"
                                "    <attribute5>97</attribute5>\n"
                                "    <attribute5>98</attribute5>\n"
                                "    <attribute5>99</attribute5>\n"
                                "    <attribute5>100</attribute5>\n"
                                "    <attribute6>97 98 99 100</attribute6>\n"
                                "    <attribute7>123 456 927</attribute7>\n"
                                "</MySequenceWithArrays>\n";

            if (veryVerbose) {
                T_ P_(X) P(EXPECTED_RESULT)
            }

            bsl::stringstream result;

            EncoderOptions options;
            options.setEncodingStyle(EncodingStyle::PRETTY);

            balxml::Encoder encoder(&options, 0, 0);
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
                                "<MySequenceWithArrays" XSI ">"
                                "<attribute1>123</attribute1>"
                                "<attribute1>456</attribute1>"
                                "<attribute1>927</attribute1>"
                                "<attribute2>YWJjZA==</attribute2>"
                                "<attribute3>61626364</attribute3>"
                                "<attribute4>abcd</attribute4>"
                                "<attribute5>97</attribute5>"
                                "<attribute5>98</attribute5>"
                                "<attribute5>99</attribute5>"
                                "<attribute5>100</attribute5>"
                                "<attribute6>97 98 99 100</attribute6>"
                                "<attribute7>123 456 927</attribute7>"
                                "</MySequenceWithArrays>";

            if (veryVerbose) {
                T_ P_(X) P(EXPECTED_RESULT)
            }

            bsl::stringstream result;

            EncoderOptions options;
            options.setEncodingStyle(EncodingStyle::COMPACT);

            balxml::Encoder encoder(&options, 0, 0);
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
                                "<MyChoice" XSI ">\n"
                                "    <selection1>434</selection1>\n"
                                "</MyChoice>\n";

            if (veryVerbose) {
                T_ P_(X) P(EXPECTED_RESULT)
            }

            bsl::stringstream result;

            EncoderOptions options;
            options.setEncodingStyle(EncodingStyle::PRETTY);

            balxml::Encoder encoder(&options, 0, 0);
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
                                "<MyChoice" XSI ">\n"
                                "    <selection2>test string</selection2>\n"
                                "</MyChoice>\n";

            if (veryVerbose) {
                T_ P_(X) P(EXPECTED_RESULT)
            }

            bsl::stringstream result;

            EncoderOptions options;
            options.setEncodingStyle(EncodingStyle::PRETTY);

            balxml::Encoder encoder(&options, 0, 0);
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
                                "<MySequence" XSI ">\n"
                                "    <attribute1>434</attribute1>\n"
                                "    <attribute2>test string</attribute2>\n"
                                "</MySequence>\n";

            if (veryVerbose) {
                T_ P_(X) P(EXPECTED_RESULT)
            }

            bsl::stringstream result;

            EncoderOptions options;
            options.setEncodingStyle(EncodingStyle::PRETTY);

            balxml::Encoder encoder(&options, 0, 0);
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
                                "<MySequence" XSI ">"
                                "<attribute1>434</attribute1>"
                                "<attribute2>test string</attribute2>"
                                "</MySequence>";

            if (veryVerbose) {
                T_ P_(X) P(EXPECTED_RESULT)
            }

            bsl::stringstream result;

            EncoderOptions options;
            options.setEncodingStyle(EncodingStyle::COMPACT);

            balxml::Encoder encoder(&options, 0, 0);
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
                               "<int" XSI ">0</int>\n"                       },
                { L_,   1,     "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                               "<int" XSI ">1</int>\n"                       },
                { L_,   123,   "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                               "<int" XSI ">123</int>\n"                     },
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
                options.setEncodingStyle(EncodingStyle::PRETTY);

                balxml::Encoder encoder(&options, 0, 0);
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
                               "<string" XSI "></string>\n"                  },
                { L_,   "a",   "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                               "<string" XSI ">a</string>\n"                 },
                { L_,   "abc", "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                               "<string" XSI ">abc</string>\n"               },
                { L_,   "\xc3\xb6" "abc",
                               "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                               "<string" XSI ">\xc3\xb6" "abc</string>\n"    },
                { L_,   "abc\xc3\xb6" "def",
                               "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                               "<string" XSI ">abc\xc3\xb6" "def</string>\n" },
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
                options.setEncodingStyle(EncodingStyle::PRETTY);

                balxml::Encoder encoder(&options, 0, 0);
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
                DEFAULT = bdlat_FormattingMode::e_DEFAULT,
                BASE64  = bdlat_FormattingMode::e_BASE64,
                HEX     = bdlat_FormattingMode::e_HEX,
                DEC     = bdlat_FormattingMode::e_DEC,
                TEXT    = bdlat_FormattingMode::e_TEXT,
                IS_LIST = bdlat_FormattingMode::e_LIST,
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
                             "<base64Binary" XSI ">YWJjZA==</base64Binary>\n"},
                { L_,   HEX,       "<?xml version=\"1.0\" "
                                   "encoding=\"UTF-8\" ?>\n"
                                   "<hexBinary" XSI ">61626364</hexBinary>\n"},
                { L_,   TEXT,      "<?xml version=\"1.0\" "
                                   "encoding=\"UTF-8\" ?>\n"
                                   "<string" XSI ">abcd</string>\n"          },
                { L_,   IS_LIST,   "<?xml version=\"1.0\" "
                                   "encoding=\"UTF-8\" ?>\n"
                                   "<list" XSI ">97 98 99 100</list>\n"      },
                { L_,   LIST_OR_DEC,
                                   "<?xml version=\"1.0\" "
                                   "encoding=\"UTF-8\" ?>\n"
                                   "<list" XSI ">97 98 99 100</list>\n"      },
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
                balxml::EncoderOptions options;

                if (FORMATTING_MODE & IS_LIST) {
                    options.setTag("list");  // xmlName behavior is undefined
                                             // if formatting mode is IS_LIST
                }

                options.setFormattingMode(FORMATTING_MODE);
                options.setEncodingStyle(EncodingStyle::PRETTY);

                balxml::Encoder encoder(&options, 0, 0);
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
                DEFAULT = bdlat_FormattingMode::e_DEFAULT,
                BASE64  = bdlat_FormattingMode::e_BASE64,
                HEX     = bdlat_FormattingMode::e_HEX,
                DEC     = bdlat_FormattingMode::e_DEC,
                TEXT    = bdlat_FormattingMode::e_TEXT,
                IS_LIST = bdlat_FormattingMode::e_LIST,
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
            "<hexBinary" XSI ">04152726FFFFB554C023DFA4AD2BDCE9</hexBinary>\n"
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
                balxml::EncoderOptions options;

                if (FORMATTING_MODE & IS_LIST) {
                    options.setTag("list");  // xmlName behavior is undefined
                                             // if formatting mode is IS_LIST
                }

                options.setFormattingMode(FORMATTING_MODE);
                options.setEncodingStyle(EncodingStyle::PRETTY);

                balxml::Encoder encoder(&options, 0, 0);
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

         balxml::EncoderOptions options;

         options.setObjectNamespace("http://bloomberg.com/schemas/apiy");

         bsl::stringstream result;

         const int OBJECT = 123;

         balxml::Encoder encoder(&options, 0, 0);
         int rc = encoder.encodeToStream(result, OBJECT);

         LOOP_ASSERT(rc, 0 == rc);
         if (veryVerbose) {
             T_ P(result.str())
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
