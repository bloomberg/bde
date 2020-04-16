// balxml_decoder.t.cpp                                               -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <balxml_decoder.h>

#include <balxml_decoderoptions.h>
#include <balxml_errorinfo.h>
#include <balxml_minireader.h>

#include <s_baltst_address.h>
#include <s_baltst_customint.h>
#include <s_baltst_customstring.h>
#include <s_baltst_employee.h>
#include <s_baltst_enumerated.h>
#include <s_baltst_mychoice.h>
#include <s_baltst_myenumeration.h>
#include <s_baltst_mysequence.h>
#include <s_baltst_mysequencewithanonymouschoice.h>
#include <s_baltst_mysequencewithanonymouschoicechoice.h>
#include <s_baltst_mysequencewithattributes.h>
#include <s_baltst_mysequencewithnillables.h>
#include <s_baltst_mysequencewithnullables.h>
#include <s_baltst_mysimplecontent.h>
#include <s_baltst_mysimpleintcontent.h>
#include <s_baltst_ratsnest.h>
#include <s_baltst_topchoice.h>

#include <bslim_testutil.h>

#include <bdlb_chartype.h>
#include <bdlb_nullableallocatedvalue.h>
#include <bdlb_nullablevalue.h>
#include <bdlb_print.h>
#include <bdlb_printmethods.h>
#include <bdlb_string.h>
#include <bdlb_variant.h>
#include <bdlde_utf8util.h>
#include <bdldfp_decimal.h>
#include <bdlsb_fixedmeminstreambuf.h>
#include <bdlat_attributeinfo.h>
#include <bdlat_choicefunctions.h>
#include <bdlat_enumeratorinfo.h>
#include <bdlat_formattingmode.h>
#include <bdlat_selectioninfo.h>
#include <bdlat_sequencefunctions.h>
#include <bdlat_typetraits.h>
#include <bdlat_valuetypefunctions.h>
#include <bdlt_datetimetz.h>

#include <bslim_testutil.h>

#include <bslalg_typetraits.h>
#include <bslma_allocator.h>
#include <bslma_default.h>
#include <bslma_testallocator.h>
#include <bslmf_issame.h>
#include <bsls_assert.h>
#include <bsls_review.h>

#include <bsl_cstddef.h>
#include <bsl_fstream.h>
#include <bsl_iomanip.h>
#include <bsl_iosfwd.h>
#include <bsl_iostream.h>
#include <bsl_list.h>
#include <bsl_memory.h>
#include <bsl_ostream.h>
#include <bsl_sstream.h>
#include <bsl_string.h>
#include <bsl_typeinfo.h>
#include <bsl_unordered_map.h>
#include <bsl_unordered_set.h>

using namespace BloombergLP;
using bsl::cout;
using bsl::cerr;
using bsl::endl;
using bsl::flush;
namespace test = s_baltst;

// ============================================================================
//                                 TEST PLAN
// ----------------------------------------------------------------------------
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
//     - types that support 'bdlat_SequenceFunctions'.
//     - types that support 'bdlat_ChoiceFunctions'.
//     - simple types that can be parsed by 'balxml_typesparserutil'.
//
// After breathing the component [1], we will first test the internal 'Parser'
// class through the 'Decoder' interface [2].  The purpose of this test will be
// to establish that the parser uses the 'Decoder_ElementContext' protocol
// correctly.  A customized 'TestContext' will be used for this purpose.  Then
// we will test the 'Decoder_SelectContext' meta-function [3] to check that,
// given a particular 'TYPE', the meta-function returns an appropriate context
// type that can be used by the parser to parse that 'TYPE'.
//
// Once we have established that the parser is working correctly, we can start
// testing the context types defined in this component.  The
// 'Decoder_UTF8Context' [4] and 'Decoder_Base64Context' [5] context types are
// tested first, because they are simple and non-templated.  Next, the
// 'Decoder_SimpleContext<TYPE>' [6] template is tested.
//
// The 'Decoder_SequenceContext<TYPE>' [8] and the
// 'Decoder_ChoiceContext<TYPE>' [9] templates make use of the
// 'Decoder_PrepareSubContext' [7] function class.  Therefore, this function
// class must be tested prior to these two class templates.
//
// The 'Decoder_VectorContext<TYPE>' [10] test makes use of the
// 'Decoder_SequenceContext<TYPE>' template, so it must be tested after the
// sequence test.
//
// At this point, the main functionality of this component has been thoroughly
// tested.  Now we need to test the 4 'decode' functions [11] in the 'Decoder'
// namespace.  These tests are trivial and only involve testing that the
// arguments and return values are passed correctly and that the input streams
// are invalidated if there is an error.
//
// Finally, we will test the usage example from the component-level
// documentation to check that it compiles and runs as expected.
//
// Note that the 'Decoder_ErrorReporter' and 'Decoder_ElementContext' protocol
// classes are tested implicitly in all test cases.
// ----------------------------------------------------------------------------
// [11] int balxml::Decoder::decode(sbuf*, TYPE, b_A*);
// [11] int balxml::Decoder::decode(sbuf*, TYPE, ostrm&, ostrm&, b_A*);
// [11] int balxml::Decoder::decode(istrm&, TYPE, b_A*);
// [11] int balxml::Decoder::decode(istrm&, TYPE, ostrm&, ostrm&, b_A*);
// [15] void setNumUnknownElementsSkipped(int value);
// [15] int numUnknownElementsSkipped() const;
// [ 3] balxml::Decoder_SelectContext
// [ 2] baexml_Decoder_ParserUtil
// [ 5] baexml_Decoder_Base64Context
// [ 9] balxml::Decoder_ChoiceContext<TYPE>
// [ 8] balxml::Decoder_SequenceContext<TYPE>
// [ 6] balxml::Decoder_SimpleContext<TYPE>
// [ 4] balxml::Decoder_UTF8Context
// [10] baexml_Decoder_VectorContext<TYPE>
// [ 7] baexml_Decoder_PrepareSubContext
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [16] USAGE EXAMPLES
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

template <class T>
bsl::ostream& operator<<(bsl::ostream& out, const bsl::vector<T>& value)
    // Output the specified container 'value' to the specified stream 'out' and
    // return that stream.
{
    printValue(out, value);
    return out;
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

void printValue(bsl::ostream& out, const char *value)
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

void printValue(bsl::ostream& out, const bslstl::StringRef& value)
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

#define XSI "xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\""

int verbose;
int veryVerbose;
int veryVeryVerbose;
int veryVeryVeryVerbose;

// ============================================================================
//                        GLOBAL CLASSES FOR TESTING
// ----------------------------------------------------------------------------

bool compareEqual(const bsl::string& lhs, const bsl::string& rhs)
{
    LOOP2_ASSERT(lhs.size(), rhs.size(), lhs.size() == rhs.size());
    const bsl::size_t LEN  = lhs.size();
    int               line = 1;
    for (bsl::size_t k = 0; k < LEN; ++k) {
        if (lhs[k] != rhs[k]) {
            LOOP4_ASSERT(line, k, lhs[k], rhs[k], lhs[k] == rhs[k]);
            return false;                                             // RETURN
        }
        if ('\n' == lhs[k]) {
            ++line;
        }
    }
    return true;
}


namespace baexml_Decoder_TestNamespace {

                            // ====================
                            // struct TestSequence0
                            // ====================

struct TestSequence0 {
    // A test sequence with 0 attributes.
};

            // ===================================================
            // bdlat_SequenceFunctions Overrides For TestSequence0
            // ===================================================

// MANIPULATORS
template <class MANIPULATOR>
int bdlat_sequenceManipulateAttribute(
                                     TestSequence0 * /* object */,
                                     MANIPULATOR&    /* manipulator */,
                                     const char    * /* attributeName */,
                                     int             /* attributeNameLength */)
{
    return -1;
}

template <class MANIPULATOR>
int bdlat_sequenceManipulateAttribute(TestSequence0 * /* object */,
                                      MANIPULATOR&    /* manipulator */,
                                      int             /* attributeId */)
{
    return -1;
}

template <class MANIPULATOR>
int bdlat_sequenceManipulateAttributes(TestSequence0 * /* object */,
                                       MANIPULATOR&    /* manipulator */)
{
    return -1;
}

// ACCESSORS
template <class ACCESSOR>
int bdlat_sequenceAccessAttribute(
                              const TestSequence0&   /* object */,
                              ACCESSOR&              /* accessor */,
                              const char           * /* attributeName */,
                              int                    /* attributeNameLength */)
{
    return -1;
}

template <class ACCESSOR>
int bdlat_sequenceAccessAttribute(const TestSequence0& /* object */,
                                  ACCESSOR&            /* accessor */,
                                  int                  /* attributeId */)
{
    return -1;
}

template <class ACCESSOR>
int bdlat_sequenceAccessAttribute(const TestSequence0& /* object */,
                                  ACCESSOR&            /* accessor */)
{
    return -1;
}

bool bdlat_sequenceHasAttribute(
                              const TestSequence0&   /* object */,
                              const char           * /* attributeName */,
                              int                    /* attributeNameLength */)
{
    return false;
}

bool bdlat_sequenceHasAttribute(const TestSequence0& /* object */,
                                int                  /* attributeId */)
{
    return false;
}

}  // close namespace baexml_Decoder_TestNamespace

using baexml_Decoder_TestNamespace::TestSequence0;

namespace BloombergLP {
namespace bdlat_SequenceFunctions {
    template <>
    struct IsSequence<TestSequence0> : bslmf::MetaInt<1> {
    };
}  // close namespace bdlat_SequenceFunctions
}  // close enterprise namespace


                            // ====================
                            // struct TestSequence1
                            // ====================

namespace baexml_Decoder_TestNamespace {

struct TestSequence1 {
    // A test sequence with 1 attributes.

    static bdlat_AttributeInfo ATTRIBUTE_INFO_ARRAY[];

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

bdlat_AttributeInfo TestSequence1::ATTRIBUTE_INFO_ARRAY[] = {
    { 1, "E1", 2, "Element 1", 0 },
};

const int TestSequence1::DEFAULT_ELEMENT1 = 0x71DEFA17;

            // ===================================================
            // bdlat_SequenceFunctions Overrides For TestSequence1
            // ===================================================

// MANIPULATORS
template <class MANIPULATOR>
int bdlat_sequenceManipulateAttribute(TestSequence1 *object,
                                      MANIPULATOR&   manipulator,
                                      const char    *attributeName,
                                      int            attributeNameLength)
{
    if (bsl::string(attributeName, attributeNameLength) ==
                             TestSequence1::ATTRIBUTE_INFO_ARRAY[0].d_name_p) {
        return manipulator(&object->d_element1,
                           TestSequence1::ATTRIBUTE_INFO_ARRAY[0]);   // RETURN
    }

    return -1;
}

template <class MANIPULATOR>
int bdlat_sequenceManipulateAttribute(TestSequence1 *object,
                                      MANIPULATOR&   manipulator,
                                      int            attributeId)
{
    if (attributeId == TestSequence1::ATTRIBUTE_INFO_ARRAY[0].d_id) {
        return manipulator(&object->d_element1,
                           TestSequence1::ATTRIBUTE_INFO_ARRAY[0]);   // RETURN
    }

    return -1;
}

template <class MANIPULATOR>
int bdlat_sequenceManipulateAttributes(TestSequence1 *object,
                                       MANIPULATOR&   manipulator)
{
    if (0 == manipulator(&object->d_element1,
                         TestSequence1::ATTRIBUTE_INFO_ARRAY[0])) {
        return 0;                                                     // RETURN
    }

    return -1;
}

// ACCESSORS
template <class ACCESSOR>
int bdlat_sequenceAccessAttribute(const TestSequence1&  object,
                                  ACCESSOR&             accessor,
                                  const char           *attributeName,
                                  int                   attributeNameLength)
{
    if (bsl::string(attributeName, attributeNameLength) ==
                             TestSequence1::ATTRIBUTE_INFO_ARRAY[0].d_name_p) {
        return accessor(object.d_element1,
                        TestSequence1::ATTRIBUTE_INFO_ARRAY[0]);      // RETURN
    }

    return -1;
}

template <class ACCESSOR>
int bdlat_sequenceAccessAttribute(const TestSequence1& object,
                                  ACCESSOR&            accessor,
                                  int                  attributeId)
{
    if (attributeId == TestSequence1::ATTRIBUTE_INFO_ARRAY[0].d_id) {
        return accessor(object.d_element1,
                        TestSequence1::ATTRIBUTE_INFO_ARRAY[0]);      // RETURN
    }

    return -1;
}

template <class ACCESSOR>
int bdlat_sequenceAccessAttribute(const TestSequence1& object,
                                  ACCESSOR&            accessor)
{
    if (0 == accessor(object.d_element1,
                      TestSequence1::ATTRIBUTE_INFO_ARRAY[0])) {
        return 0;                                                     // RETURN
    }

    return -1;
}

bool bdlat_sequenceHasAttribute(const TestSequence1&,
                                const char           *attributeName,
                                int                   attributeNameLength)
{
    if (bsl::string(attributeName, attributeNameLength)
                      == TestSequence1::ATTRIBUTE_INFO_ARRAY[0].d_name_p) {
        return true;                                                  // RETURN
    }
    return false;
}

bool bdlat_sequenceHasAttribute(const TestSequence1&, int attributeId)
{
    if (attributeId == TestSequence1::ATTRIBUTE_INFO_ARRAY[0].d_id) {
        return true;                                                  // RETURN
    }
    return false;
}

}  // close namespace baexml_Decoder_TestNamespace


using baexml_Decoder_TestNamespace::TestSequence1;

namespace BloombergLP {
namespace bdlat_SequenceFunctions {
    template <>
    struct IsSequence<TestSequence1> : bslmf::MetaInt<1> {
    };
}  // close namespace bdlat_SequenceFunctions
}  // close enterprise namespace


                            // ====================
                            // struct TestSequence2
                            // ====================

namespace baexml_Decoder_TestNamespace {

struct TestSequence2 {
    // A test sequence with 2 attributes.

    static bdlat_AttributeInfo ATTRIBUTE_INFO_ARRAY[];

    // We need default values for elements in 'TestSequence2' because this
    // class is used when testing the 'Decoder::decode' functions and we need
    // to test that elements are reset when these functions are called.

    static const int         DEFAULT_ELEMENT1;
    static const bsl::string DEFAULT_ELEMENT2;

    int         d_element1;
    bsl::string d_element2;

    TestSequence2()
    : d_element1(DEFAULT_ELEMENT1)
    , d_element2(DEFAULT_ELEMENT2)
    {
    }

    TestSequence2(int element1, const bsl::string& element2)
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

bdlat_AttributeInfo TestSequence2::ATTRIBUTE_INFO_ARRAY[] = {
    { 1, "E1", 2, "Element 1", 0 },
    { 2, "E2", 2, "Element 2", 0 },
};

const int         TestSequence2::DEFAULT_ELEMENT1
                                           = 0x72DEFA17;
const bsl::string TestSequence2::DEFAULT_ELEMENT2
                                           = "TestSequence2::DEFAULT_ELEMENT2";

            // ===================================================
            // bdlat_SequenceFunctions Overrides For TestSequence2
            // ===================================================

// MANIPULATORS
template <class MANIPULATOR>
int bdlat_sequenceManipulateAttribute(TestSequence2 *object,
                                      MANIPULATOR&   manipulator,
                                      const char    *attributeName,
                                      int            attributeNameLength)
{
    if (bsl::string(attributeName, attributeNameLength)
                          == TestSequence2::ATTRIBUTE_INFO_ARRAY[0].d_name_p) {
        return manipulator(&object->d_element1,
                           TestSequence2::ATTRIBUTE_INFO_ARRAY[0]);   // RETURN
    }
    if (bsl::string(attributeName, attributeNameLength)
                          == TestSequence2::ATTRIBUTE_INFO_ARRAY[1].d_name_p) {
        return manipulator(&object->d_element2,
                           TestSequence2::ATTRIBUTE_INFO_ARRAY[1]);   // RETURN
    }

    return -1;
}

template <class MANIPULATOR>
int bdlat_sequenceManipulateAttribute(TestSequence2 *object,
                                      MANIPULATOR&   manipulator,
                                      int            attributeId)
{
    if (attributeId == TestSequence2::ATTRIBUTE_INFO_ARRAY[0].d_id) {
        return manipulator(&object->d_element1,
                           TestSequence2::ATTRIBUTE_INFO_ARRAY[0]);   // RETURN
    }
    if (attributeId == TestSequence2::ATTRIBUTE_INFO_ARRAY[1].d_id) {
        return manipulator(&object->d_element2,
                           TestSequence2::ATTRIBUTE_INFO_ARRAY[1]);   // RETURN
    }

    return -1;
}

template <class MANIPULATOR>
int bdlat_sequenceManipulateAttributes(TestSequence2 *object,
                                       MANIPULATOR&   manipulator)
{
    if (0 == manipulator(&object->d_element1,
                         TestSequence2::ATTRIBUTE_INFO_ARRAY[0])
     && 0 == manipulator(&object->d_element2,
                         TestSequence2::ATTRIBUTE_INFO_ARRAY[1])) {
        return 0;                                                     // RETURN
    }

    return -1;
}

// ACCESSORS
template <class ACCESSOR>
int bdlat_sequenceAccessAttribute(const TestSequence2&  object,
                                  ACCESSOR&             accessor,
                                  const char           *attributeName,
                                  int                   attributeNameLength)
{
    if (bsl::string(attributeName, attributeNameLength)
                          == TestSequence2::ATTRIBUTE_INFO_ARRAY[0].d_name_p) {
        return accessor(object.d_element1,
                        TestSequence2::ATTRIBUTE_INFO_ARRAY[0]);      // RETURN
    }
    if (bsl::string(attributeName, attributeNameLength)
                          == TestSequence2::ATTRIBUTE_INFO_ARRAY[1].d_name_p) {
        return accessor(object.d_element2,
                        TestSequence2::ATTRIBUTE_INFO_ARRAY[1]);      // RETURN
    }

    return -1;
}

template <class ACCESSOR>
int bdlat_sequenceAccessAttribute(const TestSequence2& object,
                                  ACCESSOR&            accessor,
                                  int                  attributeId)
{
    if (attributeId == TestSequence2::ATTRIBUTE_INFO_ARRAY[0].d_id) {
        return accessor(object.d_element1,
                        TestSequence2::ATTRIBUTE_INFO_ARRAY[0]);      // RETURN
    }
    if (attributeId == TestSequence2::ATTRIBUTE_INFO_ARRAY[1].d_id) {
        return accessor(object.d_element2,
                        TestSequence2::ATTRIBUTE_INFO_ARRAY[1]);      // RETURN
    }

    return -1;
}

template <class ACCESSOR>
int bdlat_sequenceAccessAttribute(const TestSequence2& object,
                                  ACCESSOR&            accessor)
{
    if (0 == accessor(object.d_element1,
                      TestSequence2::ATTRIBUTE_INFO_ARRAY[0])
     && 0 == accessor(object.d_element2,
                      TestSequence2::ATTRIBUTE_INFO_ARRAY[1])) {
        return 0;                                                     // RETURN
    }

    return -1;
}

bool bdlat_sequenceHasAttribute(const TestSequence2&,
                                const char           *attributeName,
                                int                   attributeNameLength)
{
    if (bsl::string(attributeName, attributeNameLength)
                      == TestSequence2::ATTRIBUTE_INFO_ARRAY[0].d_name_p) {
        return true;                                                  // RETURN
    }
    if (bsl::string(attributeName, attributeNameLength)
                      == TestSequence2::ATTRIBUTE_INFO_ARRAY[1].d_name_p) {
        return true;                                                  // RETURN
    }
    return false;
}

bool bdlat_sequenceHasAttribute(const TestSequence2&, int attributeId)
{
    if (attributeId == TestSequence2::ATTRIBUTE_INFO_ARRAY[0].d_id) {
        return true;                                                  // RETURN
    }
    if (attributeId == TestSequence2::ATTRIBUTE_INFO_ARRAY[1].d_id) {
        return true;                                                  // RETURN
    }
    return false;
}

}  // close namespace baexml_Decoder_TestNamespace

using baexml_Decoder_TestNamespace::TestSequence2;

namespace BloombergLP {
namespace bdlat_SequenceFunctions {
template <>
struct IsSequence<TestSequence2>
: bslmf::MetaInt<1> { };
}  // close namespace bdlat_SequenceFunctions
}  // close enterprise namespace


                             // ==================
                             // struct TestChoice0
                             // ==================

namespace baexml_Decoder_TestNamespace {

struct TestChoice0 {
    // A test choice with 0 selections.
};

              // ===============================================
              // bdlat_ChoiceFunctions Overrides For TestChoice0
              // ===============================================

// MANIPULATORS
int bdlat_choiceMakeSelection(TestChoice0 * /* object */,
                              int           /* selectionId */)
{
    return -1;
}

int bdlat_choiceMakeSelection(TestChoice0 * /* object */,
                              const char  * /* name */,
                              int           /* nameLength */)
{
    return -1;
}

template <class MANIPULATOR>
int bdlat_choiceManipulateSelection(TestChoice0  * /* object */,
                                    MANIPULATOR&   /* manipulator */)
{
    return -1;
}

// ACCESSORS
template <class ACCESSOR>
int bdlat_choiceAccessSelection(const TestChoice0& /* object */,
                                ACCESSOR&          /* accessor */)
{
    return -1;
}

int bdlat_choiceSelectionId(const TestChoice0& /* object */)
{
    return -1;
}

bool bdlat_choiceHasSelection(const TestChoice0&   /* object */,
                              const char         * /* name */,
                              int                  /* length */)
{
    return false;
}

bool bdlat_choiceHasSelection(const TestChoice0& /* object */,
                              int                /* id */)
{
    return false;
}

}  // close namespace baexml_Decoder_TestNamespace

using baexml_Decoder_TestNamespace::TestChoice0;

namespace BloombergLP {
namespace bdlat_ChoiceFunctions {
    template <>
    struct IsChoice<TestChoice0> : bslmf::MetaInt<1> {
    };
}  // close namespace bdlat_ChoiceFunctions
}  // close enterprise namespace

                             // ==================
                             // struct TestChoice1
                             // ==================

namespace baexml_Decoder_TestNamespace {

struct TestChoice1 {
    // A test choice with 1 selection.

    static const bdlat_SelectionInfo SELECTION_INFO_ARRAY[];

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
        return 0;                                                     // RETURN
    }

    switch (rhs.d_choice) {
      case 0: {
        if (lhs.d_selection1 != rhs.d_selection1) {
            return 0;                                                 // RETURN
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

const bdlat_SelectionInfo TestChoice1::SELECTION_INFO_ARRAY[] = {
    { 1, "S1", 2, "Selection 1", 0 },
};

              // ===============================================
              // bdlat_ChoiceFunctions Overrides For TestChoice1
              // ===============================================

// MANIPULATORS
int bdlat_choiceMakeSelection(TestChoice1 *object, int selectionId)
{
    ASSERT(1 == selectionId);

    object->d_selection1 = 0;
    object->d_choice = selectionId - 1;

    return 0;
}

int bdlat_choiceMakeSelection(TestChoice1 *object,
                              const char  *name,
                              int          nameLength)
{
    if ("S1" == bsl::string(name, nameLength)) {
        object->d_selection1 = 0;
        object->d_choice = 0;
        return 0;                                                     // RETURN
    }

    return -1;
}

template <class MANIPULATOR>
int bdlat_choiceManipulateSelection(TestChoice1  *object,
                                    MANIPULATOR&  manipulator)
{
    if (0 == object->d_choice) {
        return manipulator(&object->d_selection1,
                           TestChoice1::SELECTION_INFO_ARRAY[0]);     // RETURN
    }

    return -1;
}

// ACCESSORS
template <class ACCESSOR>
int bdlat_choiceAccessSelection(const TestChoice1& object, ACCESSOR& accessor)
{
    if (0 == object.d_choice) {
        return accessor(object.d_selection1,
                        TestChoice1::SELECTION_INFO_ARRAY[0]);        // RETURN
    }

    return -1;
}

int bdlat_choiceSelectionId(const TestChoice1& object)
{
    return object.d_choice;
}

bool bdlat_choiceHasSelection(const TestChoice1&,
                              const char         *name,
                              int                 nameLength)
{
    if ("S1" == bsl::string(name, nameLength)) {
        return true;                                                  // RETURN
    }
    return false;
}

bool bdlat_choiceHasSelection(const TestChoice1&, int id)
{
    if (0 == id) {
        return true;                                                  // RETURN
    }
    return false;
}

}  // close namespace baexml_Decoder_TestNamespace

using baexml_Decoder_TestNamespace::TestChoice1;

namespace BloombergLP {
namespace bdlat_ChoiceFunctions {
    template <>
    struct IsChoice<TestChoice1> : bslmf::MetaInt<1> {
    };
}  // close namespace bdlat_ChoiceFunctions
}  // close enterprise namespace


                             // ==================
                             // struct TestChoice2
                             // ==================

namespace baexml_Decoder_TestNamespace {

struct TestChoice2 {
    // A test choice with 2 selections.

    static const bdlat_SelectionInfo SELECTION_INFO_ARRAY[];

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
        return 0;                                                     // RETURN
    }

    switch (rhs.d_choice) {
      case 0: {
        if (lhs.d_selection1 != rhs.d_selection1) {
            return 0;                                                 // RETURN
        }
      } break;
      case 1: {
        if (lhs.d_selection2 != rhs.d_selection2) {
            return 0;                                                 // RETURN
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

const bdlat_SelectionInfo TestChoice2::SELECTION_INFO_ARRAY[] = {
    { 1, "S1", 2, "Selection 1", 0 },
    { 2, "S2", 2, "Selection 2", 0 },
};

              // ===============================================
              // bdlat_ChoiceFunctions Overrides For TestChoice2
              // ===============================================

// MANIPULATORS
int bdlat_choiceMakeSelection(TestChoice2 *object, int selectionId)
{
    ASSERT(1 == selectionId || 2 == selectionId);

    object->d_selection1 = 0;
    object->d_selection2 = "";
    object->d_choice = selectionId - 1;

    return 0;
}

int bdlat_choiceMakeSelection(TestChoice2 *object,
                              const char  *name,
                              int          nameLength)
{
    if ("S1" == bsl::string(name, nameLength)) {
        object->d_selection1 = 0;
        object->d_choice = 0;
        return 0;                                                     // RETURN
    }
    else if ("S2" == bsl::string(name, nameLength)) {
        object->d_selection2 = "";
        object->d_choice = 1;
        return 0;                                                     // RETURN
    }

    return -1;
}

template <class MANIPULATOR>
int bdlat_choiceManipulateSelection(TestChoice2  *object,
                                    MANIPULATOR&  manipulator)
{
    if (0 == object->d_choice) {
        return manipulator(&object->d_selection1,
                           TestChoice2::SELECTION_INFO_ARRAY[0]);     // RETURN
    }
    if (1 == object->d_choice) {
        return manipulator(&object->d_selection2,
                           TestChoice2::SELECTION_INFO_ARRAY[1]);     // RETURN
    }

    return -1;
}

// ACCESSORS
template <class ACCESSOR>
int bdlat_choiceAccessSelection(const TestChoice2& object, ACCESSOR& accessor)
{
    if (0 == object.d_choice) {
        return accessor(object.d_selection1,
                        TestChoice2::SELECTION_INFO_ARRAY[0]);        // RETURN
    }
    if (1 == object.d_choice) {
        return accessor(object.d_selection2,
                        TestChoice2::SELECTION_INFO_ARRAY[1]);        // RETURN
    }

    return -1;
}

int bdlat_choiceSelectionId(const TestChoice2& object)
{
    return object.d_choice;
}

bool bdlat_choiceHasSelection(const TestChoice2&,
                              const char         *name,
                              int                 nameLength)
{
    if ("S1" == bsl::string(name, nameLength)) {
        return true;                                                  // RETURN
    }
    if ("S2" == bsl::string(name, nameLength)) {
        return true;                                                  // RETURN
    }
    return false;
}

bool bdlat_choiceHasSelection(const TestChoice2&, int id)
{
    if (0 == id) {
        return true;                                                  // RETURN
    }
    if (1 == id) {
        return true;                                                  // RETURN
    }
    return false;
}

}  // close namespace baexml_Decoder_TestNamespace

using baexml_Decoder_TestNamespace::TestChoice2;

namespace BloombergLP {
namespace bdlat_ChoiceFunctions {
    template <>
    struct IsChoice<TestChoice2> : bslmf::MetaInt<1> {
    };
}  // close namespace bdlat_ChoiceFunctions
}  // close enterprise namespace

                          // ========================
                          // class TestVectorElemType
                          // ========================

namespace baexml_Decoder_TestNamespace {

class TestVectorElemType {
    // This class will be used as the element type for the vector inside
    // 'TestSequenceWithVector' (see below).

    // PRIVATE DATA MEMBERS
    bsl::string d_callSequence;  // log of call sequence for this element

  public:
    // CREATORS

    // Generated by compiler:
    //  TestVectorElemType();
    //  TestVectorElemType(const TestVectorElemType&);
    //  ~TestVectorElemType();

    // MANIPULATORS

    // Generated by compiler:
    //  TestVectorElemType& operator=(TestVectorElemType&);

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

}  // close namespace baexml_Decoder_TestNamespace

using baexml_Decoder_TestNamespace::TestVectorElemType;

                       // =============================
                       // struct TestSequenceWithVector
                       // =============================

namespace baexml_Decoder_TestNamespace {

struct TestSequenceWithVector {
    // A test sequence with 1 vector attribute.

    static bdlat_AttributeInfo ATTRIBUTE_INFO_ARRAY[];

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

bdlat_AttributeInfo TestSequenceWithVector::ATTRIBUTE_INFO_ARRAY[] = {
    { 1, "Elem", 4, "Vector Element", 0 },
};

        // ============================================================
        // bdlat_SequenceFunctions Overrides For TestSequenceWithVector
        // ============================================================

// MANIPULATORS
template <class MANIPULATOR>
int bdlat_sequenceManipulateAttribute(
                                   TestSequenceWithVector *object,
                                   MANIPULATOR&            manipulator,
                                   const char             *attributeName,
                                   int                     attributeNameLength)
{
    if (bsl::string(attributeName, attributeNameLength)
                 == TestSequenceWithVector::ATTRIBUTE_INFO_ARRAY[0].d_name_p) {
        return manipulator(&object->d_vector,
                           TestSequenceWithVector::ATTRIBUTE_INFO_ARRAY[0]);
                                                                      // RETURN
    }

    return -1;
}

template <class MANIPULATOR>
int bdlat_sequenceManipulateAttribute(TestSequenceWithVector *object,
                                      MANIPULATOR&            manipulator,
                                      int                     attributeId)
{
    if (attributeId == TestSequenceWithVector::ATTRIBUTE_INFO_ARRAY[0].d_id) {
        return manipulator(&object->d_vector,
                           TestSequenceWithVector::ATTRIBUTE_INFO_ARRAY[0]);
                                                                      // RETURN
    }

    return -1;
}

template <class MANIPULATOR>
int bdlat_sequenceManipulateAttributes(TestSequenceWithVector *object,
                                       MANIPULATOR&            manipulator)
{
    if (0 == manipulator(&object->d_vector,
                         TestSequenceWithVector::ATTRIBUTE_INFO_ARRAY[0])) {
        return 0;                                                     // RETURN
    }

    return -1;
}

// ACCESSORS
template <class ACCESSOR>
int bdlat_sequenceAccessAttribute(
                            const TestSequenceWithVector&  object,
                            ACCESSOR&                      accessor,
                            const char                    *attributeName,
                            int                            attributeNameLength)
{
    if (bsl::string(attributeName, attributeNameLength)
                 == TestSequenceWithVector::ATTRIBUTE_INFO_ARRAY[0].d_name_p) {
        return accessor(object.d_vector,
                        TestSequenceWithVector::ATTRIBUTE_INFO_ARRAY[0]);
                                                                      // RETURN
    }

    return -1;
}

template <class ACCESSOR>
int bdlat_sequenceAccessAttribute(const TestSequenceWithVector& object,
                                  ACCESSOR&                     accessor,
                                  int                           attributeId)
{
    if (attributeId == TestSequenceWithVector::ATTRIBUTE_INFO_ARRAY[0].d_id) {
        return accessor(object.d_vector,
                        TestSequenceWithVector::ATTRIBUTE_INFO_ARRAY[0]);
                                                                      // RETURN
    }

    return -1;
}

template <class ACCESSOR>
int bdlat_sequenceAccessAttribute(const TestSequenceWithVector& object,
                                  ACCESSOR&                     accessor)
{
    if (0 == accessor(object.d_vector,
                      TestSequenceWithVector::ATTRIBUTE_INFO_ARRAY[0])) {
        return 0;                                                     // RETURN
    }

    return -1;
}

}  // close namespace baexml_Decoder_TestNamespace

using baexml_Decoder_TestNamespace::TestSequenceWithVector;

namespace BloombergLP {
namespace bdlat_SequenceFunctions {
    template <>
    struct IsSequence<TestSequenceWithVector> : bslmf::MetaInt<1> {
    };
}  // close namespace bdlat_SequenceFunctions
}  // close enterprise namespace

namespace baexml_Decoder_TestNamespace {

                             // =================
                             // class TestContext
                             // =================

class TestContext : public balxml::Decoder_ElementContext {
    // This class implements the 'Decoder_ElementContext' protocol and is used
    // to test the correct usage of this protocol by the parser.  When a method
    // from the protocol is called, the method call is recorded by appending
    // the method's name to the 'd_callSequence' member, along with the
    // arguments passed (if any).  This test context also checks that the error
    // stream and warning stream returned by the 'Decoder_ErrorReporter' object
    // is the same error stream and warning stream that was passed to the
    // 'Decoder_ParserUtil::parse' function.

    // PRIVATE DATA MEMBERS
    bsl::ostream&     d_callSequence;     // log of the call sequence
    bsl::string       d_elementName;      // element name
    bool              d_isInsideElement;  // true if we are inside
                                          // 'startElement' and 'endElement'
                                          // pair
    bslma::Allocator *d_allocator_p;      // allocator for supplying memory

  private:
    // NOT IMPLEMENTED
    TestContext(const TestContext&);
    TestContext& operator=(const TestContext&);

  public:
    // CREATORS
    TestContext(bsl::ostream&     callSequence,
                const char       *elementName,
                bslma::Allocator *basicAllocator = 0);
        // TBD: doc

    virtual ~TestContext();
        // TBD: doc

    // CALLBACKS
    virtual int startElement(balxml::Decoder *decoder);
        // Behavior is undefined if a previous successful call to
        // 'startElement' was not ended with a successful call to 'endElement'.

    virtual int endElement(balxml::Decoder *decoder);
        // Behavior is undefined unless the most recent call to 'startElement'
        // was successful and it was not already ended with a successful call
        // to 'endElement'.

    virtual int addCharacters(const char      *chars,
                              size_t           length,
                              balxml::Decoder *decoder);
        // TBD: doc
        //
        // Behavior is undefined unless the most recent call to 'startElement'
        // was successful and it was not already ended with a successful call
        // to 'endElement'.

    virtual int parseAttribute(const char      *name,
                               const char      *value,
                               size_t           lenValue,
                               balxml::Decoder *decoder);
        // TBD: doc

    virtual int parseSubElement(const char      *elementName,
                                balxml::Decoder *decoder);
        // TBD: doc
        //
        // Behavior is undefined unless the most recent call to 'startElement'
        // was successful and it was not already ended with a successful call
        // to 'endElement'.
};

                             // -----------------
                             // class TestContext
                             // -----------------

// CREATORS

TestContext::TestContext(bsl::ostream&     callSequence,
                         const char       *elementName,
                         bslma::Allocator *basicAllocator)
: d_callSequence(callSequence)
, d_elementName(elementName, basicAllocator)
, d_isInsideElement(false)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

TestContext::~TestContext()
{
    ASSERT(!d_isInsideElement);
}

// CALLBACKS

int TestContext::startElement(balxml::Decoder *)
{
    ASSERT(!d_isInsideElement);

    d_callSequence << d_elementName << ":startElement(...)";

    d_isInsideElement = true;

    if ("failOnStart" == d_elementName) {
        return -1;  // trigger failure in parser                      // RETURN
    }

    return 0;
}

int TestContext::endElement(balxml::Decoder *)
{
    ASSERT(d_isInsideElement);

    d_callSequence << d_elementName << ":endElement(...)";

    d_isInsideElement = false;

    if ("failOnEnd" == d_elementName) {
        return -1;  // trigger failure in parser                      // RETURN
    }

    return 0;
}

int TestContext::addCharacters(const char      *chars,
                               size_t           length,
                               balxml::Decoder *)
{
    ASSERT(d_isInsideElement);

    bsl::string strChars(chars, length);

    d_callSequence << d_elementName << ":addCharacters("
        << strChars << ", ...)";

    if ("failHere" == strChars) {
        return -1;  // trigger failure in parser                      // RETURN
    }

    return 0;
}

int TestContext::parseAttribute(const char      *name,
                                const char      *value,
                                size_t           lenValue,
                                balxml::Decoder *)
{
    ASSERT(d_isInsideElement);

    bsl::string strVal (value, lenValue);

    d_callSequence << d_elementName << ":parseAttribute("
                                    << name << ", " << strVal
                                    << ", ...)";

    return 0;
}

int TestContext::parseSubElement(const char      *elementName,
                                 balxml::Decoder *decoder)
{
    ASSERT(d_isInsideElement);

    d_callSequence << d_elementName << ":parseSubElement("
                                    << elementName
                                    << ", ...)";

    if (bsl::strcmp("failHere" , elementName) == 0)
    {
        return -1;  // trigger failure in parser                      // RETURN
    }

    TestContext subContext(d_callSequence, elementName, d_allocator_p);

    return subContext.beginParse(decoder);
}

}  // close namespace baexml_Decoder_TestNamespace

using baexml_Decoder_TestNamespace::TestContext;

namespace baexml_Decoder_TestNamespace {

                      // ===============================
                      // class TestVectorElemTypeContext
                      // ===============================

class TestVectorElemTypeContext : public balxml::Decoder_ElementContext {
    // This class implements the 'Decoder_ElementContext' protocol and is used
    // as the context for the 'TestVectorElemType' class.  Each call back is
    // recorded in the element object using the object's 'addCall' method.

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
    static bsl::ostream *s_loggingStream;  // stream used to verify logger was
                                           // passed correctly

    // CREATORS
    TestVectorElemTypeContext(TestVectorElemType *object,
                              bslma::Allocator   *basicAllocator = 0);
        // TBD: doc

    virtual ~TestVectorElemTypeContext();
        // TBD: doc

    // MANIPULATORS
    void reassociate(TestVectorElemType *object);
        // Reassociate this context with the specified 'object'.

    // CALLBACKS
    virtual int startElement(balxml::Decoder *decoder);
        // Behavior is undefined if a previous successful call to
        // 'startElement' was not ended with a successful call to 'endElement'.

    virtual int endElement(balxml::Decoder *decoder);
        // Behavior is undefined unless the most recent call to 'startElement'
        // was successful and it was not already ended with a successful call
        // to 'endElement'.

    virtual int addCharacters(const char      *chars,
                              size_t           length,
                              balxml::Decoder *decoder);

    virtual int parseAttribute(const char      *name,
                               const char      *value,
                               size_t           lenValue,
                               balxml::Decoder *decoder);

    virtual int parseSubElement(const char      *elementName,
                                balxml::Decoder *decoder);
        // TBD: doc
        //
        // Behavior is undefined unless the most recent call to 'startElement'
        // was successful and it was not already ended with a successful call
        // to 'endElement'.

    balxml::Decoder_ElementContext *createSubContext(
                                                  const char      *elementName,
                                                  balxml::Decoder *decoder);
        // TBD: doc
        //
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
                                            bslma::Allocator   *)
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

int TestVectorElemTypeContext::startElement(balxml::Decoder *)
{
    d_object_p->addCall("startElement(...)");

    if (3 == d_currentDepth) {
        return -1;  // trigger failure in parser                      // RETURN
    }

    ++d_currentDepth;

    return 0;
}

int TestVectorElemTypeContext::endElement(balxml::Decoder *)
{
    d_object_p->addCall("endElement(...)");

    --d_currentDepth;

    if (2 == d_currentDepth) {
        return -1;  // trigger failure in parser                      // RETURN
    }

    return 0;
}

int TestVectorElemTypeContext::addCharacters(const char      *chars,
                                             size_t           length,
                                             balxml::Decoder *)
{
    bsl::string strChars(chars, length);

    d_object_p->addCall("addCharacters(" + strChars + ", ...)");

    if ("failHere" == strChars) {
        return -1;  // trigger failure in parser                      // RETURN
    }

    return 0;
}

int TestVectorElemTypeContext::parseAttribute(const char      *name,
                                              const char      *value,
                                              size_t           lenValue,
                                              balxml::Decoder *)
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

int TestVectorElemTypeContext::parseSubElement(const char      *elementName,
                                               balxml::Decoder *)
{
    bsl::string strElementName (elementName);

    d_object_p->addCall("purseSubElement(" + strElementName + ", ...)");

    if ("failHere" == strElementName) {
        return -1;  // trigger failure in parser                      // RETURN
    }

    return 0;
}

balxml::Decoder_ElementContext *TestVectorElemTypeContext::createSubContext(
                                                  const char      *elementName,
                                                  balxml::Decoder *)
{
    bsl::string strElementName (elementName);

    d_object_p->addCall("createSubContext(" + strElementName + ", ...)");

    if ("failHere" == strElementName) {
        return 0;  // trigger failure in parser                       // RETURN
    }

    return this;
}

}  // close namespace baexml_Decoder_TestNamespace

using baexml_Decoder_TestNamespace::TestVectorElemTypeContext;

      // ===============================================================
      // Associate 'TestVectorElemType' With 'TestVectorElemTypeContext'
      // ===============================================================

namespace BloombergLP {
namespace balxml {
    template <>
    struct Decoder_SelectContext<TestVectorElemType> {
        typedef TestVectorElemTypeContext Type;
    };
}  // close package namespace
}  // close enterprise namespace

                              // ================
                              // DummyEnumeration
                              // ================

struct DummyEnumeration {
    enum Value {
        VALUE1, VALUE2, VALUE3
    };
};

namespace BloombergLP {

namespace bdlat_EnumFunctions {

    template <>
    struct IsEnumeration<DummyEnumeration::Value> {
        enum { VALUE = 1 };
    };

}  // close namespace bdlat_EnumFunctions
}  // close enterprise namespace

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

namespace bdlat_CustomizedTypeFunctions {

    template <>
    struct IsCustomizedType<DummyCustomizedType> {
        enum { VALUE = 1 };
    };

    template  <>
    struct BaseType<DummyCustomizedType> {
        typedef int Type;
    };

}  // close namespace bdlat_CustomizedTypeFunctions
}  // close enterprise namespace

namespace BloombergLP {
namespace s_baltst {

                               // ==============
                               // class Messages
                               // ==============

struct Messages {
    // This class serves as a place holder to reserve a type having the same
    // name as this component.  Doing so ensures that such a type cannot be
    // defined outside of this component in the current namespace.
};

}  // close namespace s_baltst
}  // close enterprise namespace

// ============================================================================
//                        BEGIN BDLAT TEST APPARATUS
// ----------------------------------------------------------------------------

namespace BloombergLP {

                             // ==================
                             // class TestNilValue
                             // ==================

class TestNilValue {
    // This in-memory value-semantic class provides a representation of the
    // type having only one value.
};

// FREE FUNCTIONS
bsl::ostream& operator<<(bsl::ostream& stream, const TestNilValue&)
{
    return stream << "nil";
}

inline
bool operator==(const TestNilValue&, const TestNilValue&)
{
    return true;
}

inline
bool operator!=(const TestNilValue&, const TestNilValue&)
{
    return false;
}

///Implementation Note
///-------------------
// The following set of types provide implementations of the 'bdlat' concepts
// for each category that does not have a canonical implementation.  For
// reference, the 8 'bdlat' concepts are:
//: 1 Array
//: 2 Choice
//: 3 CustomizedType
//: 4 DynamicType
//: 5 Enumeration
//: 6 NullableVAlue
//: 7 Sequence
//: 8 Simple
//
// Of these concepts, 'Array', 'NullableValue', and 'Simple' each have a
// pre-existing canonical implementation.  For these types, a testing
// implementation is not required.  The canonical implementations for 'Array',
// 'NullableValue' and 'Simple' are 'bsl::vector', 'bdlb::NullableValue', and
// the fundamental types (including string and date/time types), respectively.
//
// A test type is a value-semantic type that represents a 'bdlat' concept
// implementation.
//
// The test types were originally designed to allow one to specify different
// model types via template parameters in order to configure the behavior of
// the particular concept implementation.  This functionality was unnecessary
// in this test driver, but may warrant revisiting if these classes are moved
// out into their own components.

                             // =================
                             // class bsl::vector
                             // =================

// FREE FUNCTIONS
bsl::ostream& operator<<(bsl::ostream& stream, const bsl::vector<int>& object)
{
    stream << "(";

    typedef bsl::vector<int>::const_iterator ConstIterator;
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

                        // ============================
                        // struct TestSelectionDefaults
                        // ============================

struct TestSelectionDefaults {
    // This utility 'struct' provides a namespace for non-modifiable,
    // constant-initialized default values for the non-type template parameters
    // of a 'TestSelection'.

    // CLASS DATA
    static const char k_DEFAULT_ANNOTATION[1];
    static const char k_DEFAULT_NAME[1];
    enum { k_DEFAULT_FORMATTING_MODE = 0 };
};

// CLASS DATA
const char TestSelectionDefaults::k_DEFAULT_ANNOTATION[1] = "";
const char TestSelectionDefaults::k_DEFAULT_NAME[1]       = "";

                            // ===================
                            // class TestSelection
                            // ===================

template <int         ID,
          const char *NAME = TestSelectionDefaults::k_DEFAULT_NAME,

          const char *ANNOTATION = TestSelectionDefaults::k_DEFAULT_ANNOTATION,
          int         FORMATTING_MODE =
              TestSelectionDefaults::k_DEFAULT_FORMATTING_MODE>
class TestSelection {
    // This class provides a namespace for a suite of non-modifiable,
    // constant-initialized data that can be used to specify all attribute of a
    // 'bdlat_SelectionInfo' object.

  public:
    // CLASS DATA
    static const char *k_NAME;
    static const char *k_ANNOTATION;
    enum { k_ID = ID, k_FORMATTING_MODE = FORMATTING_MODE };

    // CLASS METHODS
    static int id() { return k_ID; }

    static bslstl::StringRef name() { return k_NAME; }

    static bslstl::StringRef annotation() { return k_ANNOTATION; }

    static int formattingMode() { return k_FORMATTING_MODE; }

    static bdlat_SelectionInfo selectionInfo()
    {
        bdlat_SelectionInfo result = {k_ID,
                                      k_NAME,
                                      static_cast<int>(bsl::strlen(k_NAME)),
                                      k_ANNOTATION,
                                      k_FORMATTING_MODE};

        return result;
    }

    // CREATORS
    TestSelection() {}
};

// CLASS DATA
template <int         ID,
          const char *NAME,
          const char *ANNOTATION,
          int         FORMATTING_MODE>
const char *TestSelection<ID, NAME, ANNOTATION, FORMATTING_MODE>::k_NAME =
    NAME;

template <int         ID,
          const char *NAME,
          const char *ANNOTATION,
          int         FORMATTING_MODE>
const char
    *TestSelection<ID, NAME, ANNOTATION, FORMATTING_MODE>::k_ANNOTATION =
        ANNOTATION;

                          // ========================
                          // class TypedTestSelection
                          // ========================

template <class TYPE, class TEST_SELECTION>
class TypedTestSelection {
    // This class provides two member type definitions, 'Type' and 'Selection',
    // which are aliases to the arguments supplied to the 'TYPE' and
    // 'TEST_SELECTION' template parameters, respectively.

  public:
    // TYPES
    typedef TYPE           Type;
    typedef TEST_SELECTION Selection;
};

                              // ================
                              // class TestChoice
                              // ================

template <class TYPED_SELECTION_0 =
              TypedTestSelection<TestNilValue, TestSelection<0> >,
          class TYPED_SELECTION_1 =
              TypedTestSelection<TestNilValue, TestSelection<0> >,
          class TYPED_SELECTION_2 =
              TypedTestSelection<TestNilValue, TestSelection<0> > >
class TestChoice {
    // This in-core value-semantic class provides a basic implementation of the
    // concept defined by the 'Choice' 'bdlat' type category.  The template
    // parameters 'TYPED_SELECTION_0', 'TYPED_SELECTION_1', and
    // 'TYPED_SELECTION_2' must all satisfy the following requirements:
    //: o The type must have two member type definitions, 'Type', and
    //:   'Selection'.
    //: o 'Type' must meet the requirements of an in-core value-semantic type.
    //: o 'Type' must meet the requirements of exactly one of the 'bdlat' value
    //:   categories.
    //: o 'Selection' must be a specialization of the 'TestSelection' type.
    // Further, each 'TestSelection' member type definition of a template
    // argument must return values for 'TestSelection::id()' and
    // 'TestSelection::name()' that are different from all others within this
    // 'TestChoice' specialization.
    //
    // Additionally, The 'Type' of any template argument may be 'TestNilValue'
    // if all 'Type' member type definitions of subsequent template arguments
    // are also 'TestNilValue'.
    //
    // The 'Type' and 'Selection' member type definitions of the template
    // arguments define the type and 'bdlat_SelectionInfo' of the selections of
    // the 'bdlat' 'Choice' implementation provided by this class.  A template
    // argument having a 'TestNilValue' 'Type' indicates that the corresponding
    // selection does not exist.

  public:
    // TYPES
    typedef typename TYPED_SELECTION_0::Selection Selection0;
    typedef typename TYPED_SELECTION_0::Type      Selection0Type;
    typedef typename TYPED_SELECTION_1::Selection Selection1;
    typedef typename TYPED_SELECTION_1::Type      Selection1Type;
    typedef typename TYPED_SELECTION_2::Selection Selection2;
    typedef typename TYPED_SELECTION_2::Type      Selection2Type;

  private:
    // PRIVATE TYPES
    typedef bdlb::Variant<Selection0Type, Selection1Type, Selection2Type>
        Value;

    // PRIVATE CLASS DATA
    enum {
        k_HAS_CHOICE_0 = !bslmf::IsSame<TestNilValue, Selection0Type>::value,
        k_HAS_CHOICE_1 = !bslmf::IsSame<TestNilValue, Selection1Type>::value,
        k_HAS_CHOICE_2 = !bslmf::IsSame<TestNilValue, Selection2Type>::value
    };

    // DATA
    bslalg::ConstructorProxy<Value> d_value;  // underlying value
    bslma::Allocator *d_allocator_p;  // memory supply (held, not owned)

  public:
    // CLASS METHODS
    static bool areEqual(const TestChoice& lhs, const TestChoice& rhs)
    {
        return lhs.d_value.object() == rhs.d_value.object();
    }

    // CREATORS
    TestChoice()
    : d_value(Selection0Type(), bslma::Default::allocator())
    , d_allocator_p(bslma::Default::allocator())
    {
    }

    explicit TestChoice(bslma::Allocator *basicAllocator)
    : d_value(Selection0Type(), basicAllocator)
    , d_allocator_p(bslma::Default::allocator(basicAllocator))
    {
    }

    template <class VALUE>
    explicit TestChoice(const VALUE&      value,
                        bslma::Allocator *basicAllocator = 0)
    : d_value(value, bslma::Default::allocator(basicAllocator))
    , d_allocator_p(basicAllocator)
    {
        BSLMF_ASSERT((bslmf::IsSame<Selection0Type, VALUE>::value ||
                      bslmf::IsSame<Selection1Type, VALUE>::value ||
                      bslmf::IsSame<Selection2Type, VALUE>::value));
    }

    TestChoice(const TestChoice&  original,
               bslma::Allocator  *basicAllocator = 0)
    : d_value(original.d_value.object(), basicAllocator)
    , d_allocator_p(bslma::Default::allocator(basicAllocator))
    {
    }

    // MANIPULATORS
    TestChoice& operator=(const TestChoice& original)
    {
        d_value.object() = original.d_value.object();
        return *this;
    }

    int makeSelection(int selectionId)
        // Set the value of this object to the default for the selection
        // indicated by the specified 'selectionId'.  Return 0 on success, and
        // a non-zero value otherwise.
    {
        switch (selectionId) {
          case 0: {
            if (!k_HAS_CHOICE_0) {
                return -1;
            }                                                         // RETURN

            d_value.object().template createInPlace<Selection0Type>();
            return 0;                                                 // RETURN
          } break;
          case 1: {
            if (!k_HAS_CHOICE_1) {
                return -1;                                            // RETURN
            }

            d_value.object().template createInPlace<Selection1Type>();
            return 0;                                                 // RETURN
          } break;
          case 2: {
            if (!k_HAS_CHOICE_2) {
                return -1;
            }                                                         // RETURN

            d_value.object().template createInPlace<Selection2Type>();
            return 0;                                                 // RETURN
          } break;
        }

        return -1;
    }

    int makeSelection(const char *selectionName, int selectionNameLength)
        // Set the value of this object to the default for the selection
        // indicated by the specified 'selectionName' of the specified
        // 'selectionNameLength'.  Return 0 on success, and a non-zero value
        // otherwise.
    {
        const bslstl::StringRef selection(selectionName, selectionNameLength);

        if (k_HAS_CHOICE_0 && Selection0::name() == selection) {
            d_value.object().template createInPlace<Selection0Type>();
            return 0;                                                 // RETURN
        }

        if (k_HAS_CHOICE_1 && Selection1::name() == selection) {
            d_value.object().template createInPlace<Selection1Type>();
            return 0;                                                 // RETURN
        }

        if (k_HAS_CHOICE_2 && Selection2::name() == selection) {
            d_value.object().template createInPlace<Selection2Type>();
            return 0;                                                 // RETURN
        }

        return -1;
    }

    template <class MANIPULATOR>
    int manipulateSelection(MANIPULATOR& manipulator)
        // Invoke the specified 'manipulator' on the address of the
        // (modifiable) selection of this object, supplying 'manipulator' with
        // the corresponding selection information structure.  Return -1 if the
        // selection is undefined, and the value returned from the invocation
        // of 'manipulator' otherwise.
    {
        switch (d_value.object().typeIndex()) {
          case 0: {
            return -1;                                                // RETURN
          } break;
          case 1: {
            BSLS_ASSERT(k_HAS_CHOICE_0);
            BSLS_ASSERT(d_value.object().template is<Selection0Type>());
            return manipulator(
                &d_value.object().template the<Selection0Type>(),
                Selection0::selectionInfo());                         // RETURN
          } break;
          case 2: {
            BSLS_ASSERT(k_HAS_CHOICE_1);
            BSLS_ASSERT(d_value.object().template is<Selection1Type>());
            return manipulator(
                &d_value.object().template the<Selection1Type>(),
                Selection1::selectionInfo());                         // RETURN
          } break;
          case 3: {
            BSLS_ASSERT(k_HAS_CHOICE_2);
            BSLS_ASSERT(d_value.object().template is<Selection2Type>());
            return manipulator(
                &d_value.object().template the<Selection2Type>(),
                Selection2::selectionInfo());                         // RETURN
          } break;
        }

        return -1;
    }

    void reset() { d_value.object().template createInPlace<Selection0Type>(); }

    // ACCESSORS
    template <class ACCESSOR>
    int accessSelection(ACCESSOR& accessor) const
        // Invoke the specified 'accessor' on the (non-modifiable) selection of
        // the this object, supplying 'accessor' with the corresponding
        // selection information structure.  Return -1 if the selection is
        // undefined, and the value returned from the invocation of 'accessor'
        // otherwise.
    {
        switch (d_value.object().typeIndex()) {
          case 0: {
            return -1;                                                // RETURN
          } break;
          case 1: {
            BSLS_ASSERT(k_HAS_CHOICE_0);
            BSLS_ASSERT(d_value.object().template is<Selection0Type>());
            return accessor(d_value.object().template the<Selection0Type>(),
                            Selection0::selectionInfo());             // RETURN
          } break;
          case 2: {
            BSLS_ASSERT(k_HAS_CHOICE_1);
            BSLS_ASSERT(d_value.object().template is<Selection1Type>());
            return accessor(d_value.object().template the<Selection1Type>(),
                            Selection1::selectionInfo());             // RETURN
          } break;
          case 3: {
            BSLS_ASSERT(k_HAS_CHOICE_2);
            BSLS_ASSERT(d_value.object().template is<Selection2Type>());
            return accessor(d_value.object().template the<Selection2Type>(),
                            Selection2::selectionInfo());             // RETURN
          } break;
        }

        return -1;
    }

    bool hasSelection(int selectionId) const
    {
        if (k_HAS_CHOICE_0 && Selection0::id() == selectionId) {
            return true;                                              // RETURN
        }

        if (k_HAS_CHOICE_1 && Selection1::id() == selectionId) {
            return true;                                              // RETURN
        }

        if (k_HAS_CHOICE_2 && Selection2::id() == selectionId) {
            return true;                                              // RETURN
        }

        return false;
    }

    bool hasSelection(const char *selectionName, int selectionNameLength) const
    {
        const bslstl::StringRef selection(selectionName, selectionNameLength);

        if (k_HAS_CHOICE_0 && Selection0::name() == selection) {
            return true;                                              // RETURN
        }

        if (k_HAS_CHOICE_1 && Selection1::name() == selection) {
            return true;                                              // RETURN
        }

        if (k_HAS_CHOICE_2 && Selection2::name() == selection) {
            return true;                                              // RETURN
        }

        return false;
    }

    const char *className() const
        // Return a null-terminated string containing the exported name for
        // this type.
    {
        return "MyChoice";
    }

    int selectionId() const { return d_value.object().typeIndex() - 1; }

    const Selection0Type& theSelection0() const
    {
        BSLS_ASSERT(d_value.object().typeIndex() == 1);
        return d_value.object().template the<Selection0Type>();
    }

    const Selection1Type& theSelection1() const
    {
        BSLS_ASSERT(d_value.object().typeIndex() == 2);
        return d_value.object().template the<Selection1Type>();
    }

    const Selection2Type& theSelection2() const
    {
        BSLS_ASSERT(d_value.object().typeIndex() == 3);
        return d_value.object().template the<Selection2Type>();
    }
};

// FREE OPERATORS
template <class V0, class V1, class V2>
bsl::ostream& operator<<(bsl::ostream&                 stream,
                         const TestChoice<V0, V1, V2>& choice)
{
    typedef TestChoice<V0, V1, V2> Choice;

    stream << "[" << choice.className() << " ";
    switch (choice.selectionId()) {
      case 0: {
        stream << Choice::Selection0::name() << " = "
               << choice.theSelection0();
      } break;
      case 1: {
        stream << Choice::Selection1::name() << " = "
               << choice.theSelection1();
      } break;
      case 2: {
        stream << Choice::Selection2::name() << " = "
               << choice.theSelection2();
      } break;
    }

    return stream << "]";
}

template <class C0, class C1, class C2>
inline
bool operator==(const TestChoice<C0, C1, C2>& lhs,
                const TestChoice<C0, C1, C2>& rhs)
{
    return TestChoice<C0, C1, C2>::areEqual(lhs, rhs);
}

template <class C0, class C1, class C2>
inline
bool operator!=(const TestChoice<C0, C1, C2>& lhs,
                const TestChoice<C0, C1, C2>& rhs)
{
    return !TestChoice<C0, C1, C2>::areEqual(lhs, rhs);
}

// TRAITS
template <class C0, class C1, class C2>
const char *bdlat_TypeName_className(const TestChoice<C0, C1, C2>& object)
    // Return a null-terminated string containing the exported name of the type
    // for the specified 'object'.
{
    return object.className();
}

template <class C0, class C1, class C2>
int bdlat_choiceMakeSelection(TestChoice<C0, C1, C2> *object, int selectionId)
    // Set the value of the specified 'object' to the default for the selection
    // indicated by the specified 'selectionId'.  Return 0 on success, and a
    // non-zero value otherwise.
{
    return object->makeSelection(selectionId);
}

template <class C0, class C1, class C2>
int bdlat_choiceMakeSelection(TestChoice<C0, C1, C2> *object,
                              const char             *selectionName,
                              int                     selectionNameLength)
    // Set the value of the specified 'object' to be the default for the
    // selection indicated by the specified 'selectionName' of the specified
    // 'selectionNameLength'.  Return 0 on success, and a non-zero value
    // otherwise.
{
    return object->makeSelection(selectionName, selectionNameLength);
}

template <class C0, class C1, class C2, class MANIPULATOR>
int bdlat_choiceManipulateSelection(TestChoice<C0, C1, C2> *object,
                                    MANIPULATOR&            manipulator)
    // Invoke the specified 'manipulator' on the address of the (modifiable)
    // selection of the specified 'object', supplying 'manipulator' with the
    // corresponding selection information structure.  Return -1 if the
    // selection is undefined, and the value returned from the invocation of
    // 'manipulator' otherwise.
{
    return object->manipulateSelection(manipulator);
}

template <class C0, class C1, class C2, class ACCESSOR>
int bdlat_choiceAccessSelection(const TestChoice<C0, C1, C2>& object,
                                ACCESSOR&                     accessor)
    // Invoke the specified 'accessor' on the (non-modifiable) selection of the
    // specified 'object', supplying 'accessor' with the corresponding
    // selection information structure.  Return -1 if the selection is
    // undefined, and the value returned from the invocation of 'accessor'
    // otherwise.
{
    return object.accessSelection(accessor);
}

template <class C0, class C1, class C2>
bool bdlat_choiceHasSelection(
                            const TestChoice<C0, C1, C2>&  object,
                            const char                    *selectionName,
                            int                            selectionNameLength)
{
    return object.hasSelection(selectionName, selectionNameLength);
}

template <class C0, class C1, class C2>
bool bdlat_choiceHasSelection(const TestChoice<C0, C1, C2>& object,
                              int                           selectionId)
{
    return object.hasSelection(selectionId);
}

template <class C0, class C1, class C2>
int bdlat_choiceSelectionId(const TestChoice<C0, C1, C2>& object)
    // Return the id of the current selection if the selection is defined, and
    // 'k_UNDEFINED_SELECTION_ID' otherwise.
{
    return object.selectionId();
}

template <class C0, class C1, class C2>
int bdlat_valueTypeAssign(TestChoice<C0, C1, C2>        *lhs,
                          const TestChoice<C0, C1, C2>&  rhs)
{
    *lhs = rhs;
    return 0;
}

template <class C0, class C1, class C2>
void bdlat_valueTypeReset(TestChoice<C0, C1, C2> *object)
{
    object->reset();
}

template <class V0, class V1>
struct bdlat_IsBasicChoice<TestChoice<V0, V1> > : bsl::true_type {
};

namespace bdlat_ChoiceFunctions {

template <class C0, class C1, class C2>
struct IsChoice<TestChoice<C0, C1, C2> > {
    enum { VALUE = 1 };
};

}  // close bdlat_ChoiceFunctions namespace

                          // ========================
                          // class TestCustomizedType
                          // ========================

template <class VALUE_TYPE, class BASE_TYPE>
class TestCustomizedType {
    // This in-core value-semantic class provides a basic implementation of the
    // 'bdlat' 'CustomizedType' concept.  The template parameter 'VALUE_TYPE'
    // specifies the underlying value for objects of this type, and the
    // template parameter 'BASE_TYPE' specifies one of the public base types of
    // 'VALUE_TYPE'.  This type is said to "customize" the 'BASE_TYPE'.  The
    // program is ill-formed unless 'VALUE_TYPE' is the same as 'BASE_TYPE', or
    // publicly inherits from it.

  public:
    // TYPES
    typedef VALUE_TYPE Value;
    typedef BASE_TYPE  BaseType;

  private:
    // DATA
    bslalg::ConstructorProxy<Value> d_value;  // underlying value
    bslma::Allocator *d_allocator_p;  // memory supply (held, not owned)

  public:
    // CLASS METHODS
    static bool areEqual(const TestCustomizedType& lhs,
                         const TestCustomizedType& rhs)
    {
        return lhs.d_value.object() == rhs.d_value.object();
    }

    // CREATORS
    TestCustomizedType()
    : d_value(bslma::Default::allocator())
    , d_allocator_p(bslma::Default::allocator())
    {
    }

    explicit TestCustomizedType(bslma::Allocator *basicAllocator)
    : d_value(basicAllocator)
    , d_allocator_p(bslma::Default::allocator(basicAllocator))
    {
    }

    explicit TestCustomizedType(const Value&      value,
                                bslma::Allocator *basicAllocator = 0)
    : d_value(value, basicAllocator)
    , d_allocator_p(bslma::Default::allocator(basicAllocator))
    {
    }

    TestCustomizedType(const TestCustomizedType&  original,
                       bslma::Allocator          *basicAllocator = 0)
    : d_value(original.d_value.object(), basicAllocator)
    , d_allocator_p(bslma::Default::allocator(basicAllocator))
    {
    }

    // MANIPULATORS
    TestCustomizedType& operator=(const TestCustomizedType& original)
    {
        d_value.object() = original.d_value.object();

        return *this;
    }

    template <class OTHER_BASE_TYPE>
    int convertFromBaseType(const OTHER_BASE_TYPE& value)
        // If an explicit conversion from the specified 'OTHER_BASE_TYPE' type
        // to the 'Value' type exists, load into the value of this object the
        // value of the specified 'base' object explicitly converted to
        // 'Value'.  Return 0 on success, and a non-zero value otherwise.
    {
        if (!bslmf::IsConvertible<Value, OTHER_BASE_TYPE>::value) {
            return -1;                                                // RETURN
        }

        d_value.object() = static_cast<Value>(value);
        return 0;
    }

    void setValue(const Value& value) { d_value.object() = value; }

    void reset() { d_value.object() = Value(); }

    // ACCESSORS
    const char *className() const
        // Return a null-terminated string containing the exported name for
        // this type.
    {
        return "MyCustomizedType";
    }

    const BaseType& convertToBaseType() const
        // Return a reference providing non-modifiable access to the 'Base'
        // subobject of the underlying value of this object.
    {
        return d_value.object();
    }

    const Value& value() const { return d_value.object(); }
};

// FREE FUNCTIONS
template <class VALUE_TYPE, class BASE_TYPE>
bsl::ostream& operator<<(
                       bsl::ostream&                                    stream,
                       const TestCustomizedType<VALUE_TYPE, BASE_TYPE>& object)
{
    return stream << "[" << object.className() << " value " << object.value()
                  << "]";
}

template <class VALUE_TYPE, class BASE_TYPE>
inline
bool operator==(const TestCustomizedType<VALUE_TYPE, BASE_TYPE>& lhs,
                const TestCustomizedType<VALUE_TYPE, BASE_TYPE>& rhs)
{
    return TestCustomizedType<VALUE_TYPE, BASE_TYPE>::areEqual(lhs, rhs);
}

template <class VALUE_TYPE, class BASE_TYPE>
inline
bool operator!=(const TestCustomizedType<VALUE_TYPE, BASE_TYPE>& lhs,
                const TestCustomizedType<VALUE_TYPE, BASE_TYPE>& rhs)
{
    return !TestCustomizedType<VALUE_TYPE, BASE_TYPE>::areEqual(lhs, rhs);
}

// TRAITS
template <class VALUE_TYPE, class BASE_TYPE>
const char *bdlat_TypeName_className(
                       const TestCustomizedType<VALUE_TYPE, BASE_TYPE>& object)
    // Return a null-terminated string containing the exported name of the type
    // for the specified 'object'.
{
    return object.className();
}

template <class VALUE_TYPE, class BASE_TYPE, class OTHER_BASE_TYPE>
int bdlat_customizedTypeConvertFromBaseType(
                             TestCustomizedType<VALUE_TYPE, BASE_TYPE> *object,
                             const OTHER_BASE_TYPE&                     base)
    // If an explicit conversion from the specified 'OTHER_BASE_TYPE' type to
    // the specified 'VALUE_TYPE' type exists, load into the underlying value
    // of the specified 'object' the value of the specified 'base' object
    // explicitly converted to 'VALUE_TYPE'.  Return 0 on success, and a
    // non-zero value otherwise.
{
    return object->convertFromBaseType(base);
}

template <class VALUE_TYPE, class BASE_TYPE>
const BASE_TYPE& bdlat_customizedTypeConvertToBaseType(
                       const TestCustomizedType<VALUE_TYPE, BASE_TYPE>& object)
    // Return a reference providing non-modifiable access to the 'Base'
    // subobject of the underlying value of the specified 'object'.
{
    return object.convertToBaseType();
}

template <class VALUE_TYPE, class BASE_TYPE>
int bdlat_valueTypeAssign(
                         TestCustomizedType<VALUE_TYPE, BASE_TYPE>        *lhs,
                         const TestCustomizedType<VALUE_TYPE, BASE_TYPE>&  rhs)
{
    *lhs = rhs;
    return 0;
}

template <class VALUE_TYPE, class BASE_TYPE>
void bdlat_valueTypeReset(TestCustomizedType<VALUE_TYPE, BASE_TYPE> *object)
{
    object->reset();
}

template <class VALUE_TYPE, class BASE_TYPE>
struct bdlat_IsBasicCustomizedType<TestCustomizedType<VALUE_TYPE, BASE_TYPE> >
: bsl::true_type {
};

namespace bdlat_CustomizedTypeFunctions {

template <class VALUE_TYPE, class BASE_TYPE>
struct IsCustomizedType<TestCustomizedType<VALUE_TYPE, BASE_TYPE> > {
    enum { VALUE = 1 };
};

}  // close bdlat_CustomizedTypeFunctions namespace

                     // ==================================
                     // class TestDynamicType_ArrayImpUtil
                     // ==================================

template <class VALUE_TYPE,
          bool IS_ARRAY =
              bdlat_TypeCategory::e_ARRAY_CATEGORY ==
              static_cast<bdlat_TypeCategory::Value>(
                  bdlat_TypeCategory::Select<VALUE_TYPE>::e_SELECTION)>
struct TestDynamicType_ArrayImpUtil {
    // This utility 'struct' provides a namespace for a suite of functions used
    // by 'TestDynamicType' to implement the 'bdlat' 'Array' concept for
    // 'VALUE_TYPE' template parameters that themselves satisfy the 'bdlat'
    // 'Array' concept.

    // TYPES
    typedef VALUE_TYPE Value;

    // CLASS METHODS
    template <class ACCESSOR>
    static int accessElement(const Value& value, ACCESSOR& accessor, int index)
        // Invoke the specified 'accessor' on the non-modifiable element at the
        // specified 'index' of the specified array 'value'.  Return the value
        // from the invocation of 'accessor'.  The behavior is undefined unless
        // '0 <= index' and 'index < size(value)'.
    {
        return bdlat_ArrayFunctions::accessElement(value, accessor, index);
    }

    template <class MANIPULATOR>
    static int manipulateElement(Value        *value,
                                 MANIPULATOR&  manipulator,
                                 int           index)
        // Invoke the specified 'manipulator' on the address of the element at
        // the specified 'index' of the specified array 'value'.  Return the
        // value from the invocation of 'manipulator'.  The behavior is
        // undefined unless '0 <= index' and 'index < size(*value)'.
    {
        return bdlat_ArrayFunctions::manipulateElement(
            value, manipulator, index);
    }

    static void resize(Value *value, int newSize)
        // Set the sizes of the specified array 'value' to the specified
        // 'newSize'.  If 'newSize > size(*value)', then
        // 'newSize - size(*value)' new elements with default values are
        // appended to 'value'.  If 'newSize < size(*value)' then the
        // 'size(*value) - newSize' elements at the end of 'value' are
        // destroyed.  The behavior is undefined unless '0 <= newSize'.
    {
        return bdlat_ArrayFunctions::resize(value, newSize);
    }

    static bsl::size_t size(const Value& value)
        // Return the number of elements in the specified array 'value'.
    {
        return bdlat_ArrayFunctions::size(value);
    }
};

template <class VALUE_TYPE>
struct TestDynamicType_ArrayImpUtil<VALUE_TYPE, false> {
    // This utility 'struct' provides a namespace for a suite of functions used
    // by 'TestDynamicType' to provide a stub implementation of the 'bdlat'
    // 'Array' concept for 'VALUE_TYPE' template parameters that do not satisfy
    // the 'bdlat' 'Array' concept.

    // TYPES
    typedef VALUE_TYPE Value;

    // CLASS METHODS
    template <class ACCESSOR>
    static int accessElement(const Value&, ACCESSOR, int)
    {
        bsl::abort();
    }

    template <class MANIPULATOR>
    static int manipulateElement(Value *, MANIPULATOR&, int)
    {
        bsl::abort();
    }

    static void resize(Value *, int) { bsl::abort(); }

    static bsl::size_t size(const Value&) { bsl::abort(); }
};

                    // ===================================
                    // class TestDynamicType_ChoiceImpUtil
                    // ===================================

template <class VALUE_TYPE,
          bool IS_CHOICE =
              bdlat_TypeCategory::e_CHOICE_CATEGORY ==
              static_cast<bdlat_TypeCategory::Value>(
                  bdlat_TypeCategory::Select<VALUE_TYPE>::e_SELECTION)>
struct TestDynamicType_ChoiceImpUtil {
    // This utility 'struct' provides a namespace for a suite of functions used
    // by 'TestDynamicType' to implement the 'bdlat' 'Choice' concept for
    // 'VALUE_TYPE' template parameters that themselves satisfy the 'bdlat'
    // 'Choice' concept.

    // TYPES
    typedef VALUE_TYPE Value;

    // CLASS METHODS
    template <class ACCESSOR>
    static int accessSelection(const Value& value, ACCESSOR& accessor)
        // Invoke the specified 'accessor' on the (non-modifiable) selection of
        // the specified 'value', supplying 'accessor' with the corresponding
        // selection information structure.  Return -1 if the selection is
        // undefined, and the value returned from the invocation of 'accessor'
        // otherwise.
    {
        return bdlat_ChoiceFunctions::accessSelection(value, accessor);
    }

    static bool hasSelection(const Value& value, int selectionId)
        // Return 'true' if the specified 'value' has a selection with the
        // specified 'selectionId', and 'false' otherwise.
    {
        return bdlat_ChoiceFunctions::hasSelection(value, selectionId);
    }

    static bool hasSelection(const Value&  value,
                             const char   *selectionName,
                             int           selectionNameLength)
        // Return 'true' if the specified 'value' has a selection with the
        // specified 'selectionName' of the specified 'selectionNameLength',
        // and 'false' otherwise.
    {
        return bdlat_ChoiceFunctions::hasSelection(
            value, selectionName, selectionNameLength);
    }

    static int makeSelection(Value *value, int selectionId)
        // Set the value of the specified 'value' to the default for the
        // selection indicated by the specified 'selectionId'.  Return 0 on
        // success, and a non-zero value otherwise.
    {
        return bdlat_ChoiceFunctions::makeSelection(value, selectionId);
    }

    static int makeSelection(Value      *value,
                             const char *selectionName,
                             int         selectionNameLength)
        // Set the value of the specified 'value' to be the default for the
        // selection indicated by the specified 'selectionName' of the
        // specified 'selectionNameLength'.  Return 0 on success, and a
        // non-zero value otherwise.
    {
        return bdlat_ChoiceFunctions::makeSelection(
            value, selectionName, selectionNameLength);
    }

    template <class MANIPULATOR>
    static int manipulateSelection(Value *value, MANIPULATOR& manipulator)
        // Invoke the specified 'manipulator' on the address of the
        // (modifiable) selection of the specified 'value', supplying
        // 'manipulator' with the corresponding selection information
        // structure.  Return -1 if the selection is undefined, and the value
        // returned from the invocation of 'manipulator' otherwise.
    {
        return bdlat_ChoiceFunctions::manipulateSelection(value, manipulator);
    }

    static int selectionId(const Value& value)
        // Return the id of the current selection of the specified 'value' if
        // the selection is defined, and -1 otherwise.
    {
        return bdlat_ChoiceFunctions::selectionId(value);
    }
};

template <class VALUE_TYPE>
struct TestDynamicType_ChoiceImpUtil<VALUE_TYPE, false> {
    // This utility 'struct' provides a namespace for a suite of functions used
    // by 'TestDynamicType' to provide a stub implementation of the 'bdlat'
    // 'Choice' concept for 'VALUE_TYPE' template parameters that do not
    // satisfy the 'bdlat' 'Choice' concept.

    // TYPES
    typedef VALUE_TYPE Value;

    // CLASS METHODS
    template <class ACCESSOR>
    static int accessSelection(const Value&, ACCESSOR&)
    {
        bsl::abort();
    }

    static bool hasSelection(const Value&, int) { bsl::abort(); }

    static bool hasSelection(const Value&, const char *, int) { bsl::abort(); }

    static int makeSelection(Value *, int) { bsl::abort(); }

    static int makeSelection(Value *, const char *, int) { bsl::abort(); }

    template <class MANIPULATOR>
    static int manipulateSelection(Value *, MANIPULATOR&)
    {
        bsl::abort();
    }

    static int selectionId(const Value&) { bsl::abort(); }
};

                // ===========================================
                // class TestDynamicType_CustomizedTypeImpUtil
                // ===========================================

template <class VALUE_TYPE,
          bool IS_CUSTOMIZED_TYPE =
              bdlat_TypeCategory::e_CUSTOMIZED_TYPE_CATEGORY ==
              static_cast<bdlat_TypeCategory::Value>(
                  bdlat_TypeCategory::Select<VALUE_TYPE>::e_SELECTION)>
struct TestDynamicType_CustomizedTypeImpUtil {
    // This utility 'struct' provides a namespace for a suite of utility
    // functions used by 'TestDynamicType' to implement the 'bdlat'
    // 'CustomizedType' concept for 'VALUE_TYPE' template parameters that
    // themselves satisfy the 'bdlat' 'CustomizedType' concept.

    // TYPES
    typedef VALUE_TYPE Value;
    typedef
        typename bdlat_CustomizedTypeFunctions::template BaseType<Value>::Type
            BaseType;

    // CLASS METHODS
    template <class BASE_TYPE>
    static int convertFromBaseType(Value *value, const BASE_TYPE& object)
    {
        return bdlat_CustomizedTypeFunctions::convertFromBaseType(value,
                                                                  object);
    }

    static const BaseType& convertToBaseType(const Value& value)
    {
        return bdlat_CustomizedTypeFunctions::convertToBaseType(value);
    }
};

template <class VALUE_TYPE>
struct TestDynamicType_CustomizedTypeImpUtil<VALUE_TYPE, false> {
    // This utility 'struct' provides a namespace for a suite of utility
    // functions used by 'TestDynamicType' to provide a stub implementation of
    // the 'bdlat' 'CustomizedType' concept for 'VALUE_TYPE' template
    // parameters that do not satisfy the 'bdlat' 'CustomizedType' concept.

    // TYPES
    typedef VALUE_TYPE Value;
    typedef VALUE_TYPE BaseType;

    // CLASS METHODS
    template <class BASE_TYPE>
    static int convertFromBaseType(Value *, const BASE_TYPE&)
    {
        bsl::abort();
    }

    static const BaseType& convertToBaseType(const Value&)
    {
        bsl::abort();
    }
};

                  // ========================================
                  // class TestDynamicType_EnumerationImpUtil
                  // ========================================

template <class VALUE_TYPE,
          bool IS_ENUMERATION =
              bdlat_TypeCategory::e_ENUMERATION_CATEGORY ==
              static_cast<bdlat_TypeCategory::Value>(
                  bdlat_TypeCategory::Select<VALUE_TYPE>::e_SELECTION)>
struct TestDynamicType_EnumerationImpUtil {
    // This utility 'struct' provides a namespace for a suite of functions used
    // by 'TestDynamicType' to implement the 'bdlat' 'Enumeration' concept for
    // 'VALUE_TYPE' template parameters that themselves satisfy the 'bdlat'
    // 'Enumeration' concept.

    // TYPES
    typedef VALUE_TYPE Value;

    // CLASS METHODS
    static int fromInt(Value *value, int integer)
    {
        return bdlat_EnumFunctions::fromInt(value, integer);
    }

    static int fromString(Value *value, const char *string, int stringLength)
    {
        return bdlat_EnumFunctions::fromString(value, string, stringLength);
    }

    static void toInt(int *result, const Value& value)
    {
        return bdlat_EnumFunctions::toInt(result, value);
    }

    static void toString(bsl::string *result, const Value& value)
    {
        return bdlat_EnumFunctions::toString(result, value);
    }
};

template <class VALUE_TYPE>
struct TestDynamicType_EnumerationImpUtil<VALUE_TYPE, false> {
    // This utility 'struct' provides a namespace for a suite of functions used
    // by 'TestDynamicType' to provide a stub implementation of the 'bdlat'
    // 'Enumeration' concept for 'VALUE_TYPE' template parameters that do not
    // satisfy the 'bdlat' 'Enumeration' concept.

    // TYPES
    typedef VALUE_TYPE Value;

    // CLASS METHODS
    static int fromInt(Value *, int) { bsl::abort(); }

    static int fromString(Value *, const char *, int) { bsl::abort(); }

    static void toInt(int *, const Value&) { bsl::abort(); }

    static void toString(bsl::string *, const Value&) { bsl::abort(); }
};

                 // ==========================================
                 // class TestDynamicType_NullableValueImpUtil
                 // ==========================================

template <class VALUE_TYPE,
          bool IS_NULLABLE_VALUE =
              bdlat_TypeCategory::e_NULLABLE_VALUE_CATEGORY ==
              static_cast<bdlat_TypeCategory::Value>(
                  bdlat_TypeCategory::Select<VALUE_TYPE>::e_SELECTION)>
struct TestDynamicType_NullableValueImpUtil {
    // This utility 'struct' provides a namespace for a suite of functions used
    // by 'TestDynamicType' to implement the 'bdlat' 'NullableValue' concept
    // for 'VALUE_TYPE' template parameters that themselves satisfy the 'bdlat'
    // 'NullableValue' concept.

    // TYPES
    typedef VALUE_TYPE Value;

    // CLASS METHODS
    static void makeValue(Value *value)
    {
        bdlat_NullableValueFunctions::makeValue(value);
    }

    template <class MANIPULATOR>
    static int manipulateValue(Value *value, MANIPULATOR& manipulator)
    {
        return bdlat_NullableValueFunctions::manipulateValue(value,
                                                             manipulator);
    }

    template <class ACCESSOR>
    static int accessValue(const Value& value, ACCESSOR& accessor)
    {
        return bdlat_NullableValueFunctions::accessValue(value, accessor);
    }

    static bool isNull(const Value& value)
    {
        return bdlat_NullableValueFunctions::isNull(value);
    }
};

template <class VALUE_TYPE>
struct TestDynamicType_NullableValueImpUtil<VALUE_TYPE, false> {
    // This utility 'struct' provides a namespace for a suite of functions used
    // by 'TestDynamicType' to provide a stub implementation of the 'bdlat'
    // 'NullableValue' concept for 'VALUE_TYPE' template parameters that do not
    // satisfy the 'bdlat' 'NullableValue' concept.

    // TYPES
    typedef VALUE_TYPE Value;

    // CLASS METHODS
    static void makeValue(Value *) { bsl::abort(); }

    template <class MANIPULATOR>
    static int manipulateValue(Value *, MANIPULATOR&)
    {
        bsl::abort();
    }

    template <class ACCESSOR>
    static int accessValue(const Value&, ACCESSOR&)
    {
        bsl::abort();
    }

    static bool isNull(const Value&) { bsl::abort(); }
};

                   // =====================================
                   // class TestDynamicType_SequenceImpUtil
                   // =====================================

template <class VALUE_TYPE,
          bool IS_SEQUENCE =
              bdlat_TypeCategory::e_SEQUENCE_CATEGORY ==
              static_cast<bdlat_TypeCategory::Value>(
                  bdlat_TypeCategory::Select<VALUE_TYPE>::e_SELECTION)>
struct TestDynamicType_SequenceImpUtil {
    // This utility 'struct' provides a namespace for a suite of functions used
    // by 'TestDynamicType' to implement the 'bdlat' 'Sequence' concept for
    // 'VALUE_TYPE' template parameters that themselves satisfy the 'bdlat'
    // 'Sequence' concept.

    // TYPES
    typedef VALUE_TYPE Value;

    // CLASS METHODS
    template <class ACCESSOR>
    static int accessAttribute(const Value&  value,
                               ACCESSOR&     accessor,
                               const char   *attributeName,
                               int           attributeNameLength)
        // Invoke the specified 'accessor' on the (non-modifiable) attribute of
        // the specified 'value' indicated by the specified 'attributeName' of
        // the specified 'attributeNameLength', supplying 'accessor' with the
        // corresponding attribute information structure.  Return a non-zero
        // value if the attribute is not found, and the value returned from the
        // invocation of 'accessor' otherwise.
    {
        return bdlat_SequenceFunctions::accessAttribute(
            value, accessor, attributeName, attributeNameLength);
    }

    template <class ACCESSOR>
    static int accessAttribute(const Value& value,
                               ACCESSOR&    accessor,
                               int          attributeId)
        // Invoke the specified 'accessor' on the (non-modifiable) attribute of
        // the specified 'value' with the specified 'attributeId', supplying
        // 'accessor' with the corresponding attribute information structure.
        // Return non-zero if the attribute is not found, and the value
        // returned from the invocation of 'accessor' otherwise.
    {
        return bdlat_SequenceFunctions::accessAttribute(
            value, accessor, attributeId);
    }

    template <class ACCESSOR>
    static int accessAttributes(const Value& value, ACCESSOR& accessor)
        // Invoke the specified 'accessor' sequentially on each attribute of
        // the specified 'value', supplying 'accessor' with the corresponding
        // attribute information structure until such invocation returns a
        // non-zero value.  Return the value from the last invocation of
        // 'accessor'.
    {
        return bdlat_SequenceFunctions::accessAttributes(value, accessor);
    }

    static bool hasAttribute(const Value&  value,
                             const char   *attributeName,
                             int           attributeNameLength)
        // Return 'true' if the specified 'value' has an attribute with the
        // specified 'attributeName' of the specified 'attributeNameLength',
        // and 'false' otherwise.
    {
        return bdlat_SequenceFunctions::hasAttribute(
            value, attributeName, attributeNameLength);
    }

    static bool hasAttribute(const Value& value, int attributeId)
        // Return 'true' if the specified 'value' has an attribute with the
        // specified 'attributeId', and 'false' otherwise.
    {
        return bdlat_SequenceFunctions::hasAttribute(value, attributeId);
    }

    template <class MANIPULATOR>
    static int manipulateAttribute(Value        *value,
                                   MANIPULATOR&  manipulator,
                                   const char   *attributeName,
                                   int           attributeNameLength)
        // Invoke the specified 'manipulator' on the address of the
        // (modifiable) attribute indicated by the specified 'attributeName' of
        // the specified 'attributeNameLength' of the specified 'value',
        // supplying 'manipulator' with the corresponding attribute information
        // structure.  Return a non-zero value if the attribute is not found,
        // and the value returned from the invocation of 'manipulator'
        // otherwise.
    {
        return bdlat_SequenceFunctions::manipulateAttribute(
            value, manipulator, attributeName, attributeNameLength);
    }

    template <class MANIPULATOR>
    static int manipulateAttribute(Value        *value,
                                   MANIPULATOR&  manipulator,
                                   int           attributeId)
        // Invoke the specified 'manipulator' on the address of the
        // (modifiable) attribute indicated by the specified 'attributeId' of
        // the specified 'value', supplying 'manipulator' with the
        // corresponding attribute information structure.  Return a non-zero
        // value if the attribute is not found, and the value returned from the
        // invocation of 'manipulator' otherwise.
    {
        return bdlat_SequenceFunctions::manipulateAttribute(
            value, manipulator, attributeId);
    }

    template <class MANIPULATOR>
    static int manipulateAttributes(Value *value, MANIPULATOR& manipulator)
        // Invoke the specified 'manipulator' sequentially on the address of
        // each (modifiable) attribute of the specified 'value', supplying
        // 'manipulator' with the corresponding attribute information structure
        // until such invocation returns a non-zero value.  Return the value
        // from the last invocation of 'manipulator'.
    {
        return bdlat_SequenceFunctions::manipulateAttributes(value,
                                                             manipulator);
    }
};

template <class VALUE_TYPE>
struct TestDynamicType_SequenceImpUtil<VALUE_TYPE, false> {
    // This utility 'struct' provides a namespace for a suite of functions used
    // by 'TestDynamicType' to provide stub implementations of the 'bdlat'
    // 'Sequence' concept for 'VALUE_TYPE' template parameters that do not
    // satisfy the 'bdlat' 'Sequence' concept.

    // TYPES
    typedef VALUE_TYPE Value;

    // CLASS METHODS
    template <class ACCESSOR>
    static int accessAttribute(const Value&, ACCESSOR&, const char *, int)
    {
        bsl::abort();
    }

    template <class ACCESSOR>
    static int accessAttribute(const Value&, ACCESSOR&, int)
    {
        bsl::abort();
    }

    template <class ACCESSOR>
    static int accessAttributes(const Value&, ACCESSOR&)
    {
        bsl::abort();
    }

    static bool hasAttribute(const Value&, const char *, int) { bsl::abort(); }

    static bool hasAttribute(const Value&, int) { bsl::abort(); }

    template <class MANIPULATOR>
    static int manipulateAttribute(Value *, MANIPULATOR&, const char *, int)
    {
        bsl::abort();
    }

    template <class MANIPULATOR>
    static int manipulateAttribute(Value *, MANIPULATOR&, int)
    {
        bsl::abort();
    }

    template <class MANIPULATOR>
    static int manipulateAttributes(Value *, MANIPULATOR&)
    {
        bsl::abort();
    }
};

                           // =====================
                           // class TestDynamicType
                           // =====================

template <class VALUE_TYPE>
class TestDynamicType {
    // This in-core value-semantic class provides a basic implementation of the
    // 'bdlat' 'DynamicType' concept.  The template parameter 'VALUE_TYPE'
    // specifies the underlying value of this type, and further, specifies
    // which of the 'bdlat' attribute concepts this type implements.  This type
    // implements the same 'bdlat' concept as the 'VALUE_TYPE' through the
    // 'bdlat' 'DynamicType' interface.  The program is ill-formed unless
    // 'VALUE_TYPE' meets the requirements of at least one of the 'bdlat'
    // 'Array', 'Choice', 'CustomizedType', 'Enumeration', 'NullableValue', or
    // 'Sequence' concepts.

  public:
    // TYPES
    typedef VALUE_TYPE Value;
        // An alias to the type that defines the underlying value of this
        // object.

  private:
    // PRIVATE TYPES
    typedef TestDynamicType_ArrayImpUtil<VALUE_TYPE>  ArrayImpUtil;
    typedef TestDynamicType_ChoiceImpUtil<VALUE_TYPE> ChoiceImpUtil;
    typedef TestDynamicType_CustomizedTypeImpUtil<VALUE_TYPE>
                                                           CustomizedTypeImpUtil;
    typedef TestDynamicType_EnumerationImpUtil<VALUE_TYPE> EnumerationImpUtil;
    typedef TestDynamicType_NullableValueImpUtil<VALUE_TYPE>
                                                        NullableValueImpUtil;
    typedef TestDynamicType_SequenceImpUtil<VALUE_TYPE> SequenceImpUtil;

    // PRIVATE CLASS DATA
    enum {
        e_ARRAY           = bdlat_TypeCategory::e_ARRAY_CATEGORY,
        e_CHOICE          = bdlat_TypeCategory::e_CHOICE_CATEGORY,
        e_CUSTOMIZED_TYPE = bdlat_TypeCategory::e_CUSTOMIZED_TYPE_CATEGORY,
        e_ENUMERATION     = bdlat_TypeCategory::e_ENUMERATION_CATEGORY,
        e_NULLABLE_VALUE  = bdlat_TypeCategory::e_NULLABLE_VALUE_CATEGORY,
        e_SEQUENCE        = bdlat_TypeCategory::e_SEQUENCE_CATEGORY
    };

    // PRIVATE CLASS FUNCTIONS
    static bool valueHasCategory(int category)
        // Return 'true' if the 'VALUE_TYPE' implements the 'bdlat' concept
        // identified by the specified 'category'.  The behavior is undefined
        // unless 'category' is equal to one of the enumerators of
        // 'bdlat_TypeCategory::Value'.
    {
        return category == bdlat_TypeCategory::Select<VALUE_TYPE>::e_SELECTION;
    }

    // DATA
    bslalg::ConstructorProxy<Value> d_value;  // underlying value
    bslma::Allocator *d_allocator_p;  // memory supply (held, not owned)

  public:
    // CLASS METHODS
    static bool areEqual(const TestDynamicType& lhs,
                         const TestDynamicType& rhs)
    {
        return lhs.d_value.object() == rhs.d_value.object();
    }

    // CREATORS
    TestDynamicType()
    : d_value(bslma::Default::allocator())
    , d_allocator_p(bslma::Default::allocator())
    {
    }

    explicit TestDynamicType(bslma::Allocator *basicAllocator)
    : d_value(basicAllocator)
    , d_allocator_p(bslma::Default::allocator())
    {
    }

    TestDynamicType(const Value& value, bslma::Allocator *basicAllocator = 0)
    : d_value(value, basicAllocator)
    , d_allocator_p(bslma::Default::allocator(basicAllocator))
    {
    }

    TestDynamicType(const TestDynamicType&  original,
                    bslma::Allocator       *basicAllocator = 0)
    : d_value(original.d_value.object(), basicAllocator)
    , d_allocator_p(bslma::Default::allocator(basicAllocator))
    {
    }

    // MANIPULATORS
    TestDynamicType& operator=(const TestDynamicType& original)
    {
        d_value.object() = original.d_value.object();
        return *this;
    }

    template <class MANIPULATOR>
    int arrayManipulateElement(MANIPULATOR& manipulator, int index)
        // Invoke the specified 'manipulator' on the address of the element at
        // the specified 'index' of this object.  Return the value from the
        // invocation of 'manipulator'.  The behavior is undefined unless
        // '0 <= index' and 'index < size()'.
    {
        BSLS_ASSERT(valueHasCategory(e_ARRAY));

        return ArrayImpUtil::manipulateElement(
            &d_value.object(), manipulator, index);
    }

    void arrayResize(int newSize)
        // Set the sizes of this object to the specified 'newSize'.  If
        // 'newSize > size()', then 'newSize - size()' new elements with
        // default values are appended to this object.  If 'newSize < size()'
        // then the 'size() - newSize' elements at the end of this object are
        // destroyed.  The behavior is undefined unless '0 <= newSize'.
    {
        BSLS_ASSERT(valueHasCategory(e_ARRAY));

        return ArrayImpUtil::resize(&d_value.object(), newSize);
    }

    int choiceMakeSelection(int selectionId)
        // Set the value of this object to the default for the selection
        // indicated by the specified 'selectionId'.  Return 0 on success, and
        // a non-zero value otherwise.
    {
        BSLS_ASSERT(valueHasCategory(e_CHOICE));

        return ChoiceImpUtil::makeSelection(&d_value.object(), selectionId);
    }

    int choiceMakeSelection(const char *selectionName, int selectionNameLength)
        // Set the value of this object to be the default for the selection
        // indicated by the specified 'selectionName' of the specified
        // 'selectionNameLength'.  Return 0 on success, and a non-zero value
        // otherwise.
    {
        BSLS_ASSERT(valueHasCategory(e_CHOICE));

        return ChoiceImpUtil::makeSelection(
            &d_value.object(), selectionName, selectionNameLength);
    }

    template <class MANIPULATOR>
    int choiceManipulateSelection(MANIPULATOR& manipulator)
        // Invoke the specified 'manipulator' on the address of the
        // (modifiable) selection of the this object, supplying 'manipulator'
        // with the corresponding selection information structure.  Return -1
        // if the selection is undefined, and the value returned from the
        // invocation of 'manipulator' otherwise.
    {
        BSLS_ASSERT(valueHasCategory(e_CHOICE));

        return ChoiceImpUtil::manipulateSelection(&d_value.object(),
                                                  manipulator);
    }

    template <class BASE_TYPE>
    int customizedTypeConvertFromBaseType(const BASE_TYPE& base)
        // If an explicit conversion from the specified 'BASE_TYPE' type to the
        // 'Value' type exists, load into the value of this object the value of
        // the specified 'base' object explicitly converted to 'Value'.  Return
        // 0 on success, and a non-zero value otherwise.
    {
        BSLS_ASSERT(valueHasCategory(e_CUSTOMIZED_TYPE));

        return CustomizedTypeImpUtil::convertFromBaseType(&d_value.object(),
                                                          base);
    }

    int enumerationFromInt(int number)
        // Load into the underlying value of this object the enumerator
        // matching the specified 'number'.  Return 0 on success, and a
        // non-zero value with no effect on 'value' if 'number' does not match
        // any enumerator.
    {
        BSLS_ASSERT(valueHasCategory(e_ENUMERATION));

        return EnumerationImpUtil::fromInt(&d_value.object(), number);
    }

    int enumerationFromString(const char *string, int stringLength)
        // Load into the underlying value of this object the enumerator
        // matching the specified 'string' of the specified 'stringLength'.
        // Return 0 on success, and a non-zero value with no effect on 'result'
        // if 'string' and 'stringLength' do not match any enumerator.
    {
        BSLS_ASSERT(valueHasCategory(e_ENUMERATION));

        return EnumerationImpUtil::fromString(
            &d_value.object(), string, stringLength);
    }

    void nullableValueMakeValue()
        // Assign to the underlying value of this object the default value
        // the contained type.
    {
        BSLS_ASSERT(valueHasCategory(e_NULLABLE_VALUE));

        return NullableValueImpUtil::makeValue(&d_value.object());
    }

    template <class MANIPULATOR>
    int nullableValueManipulateValue(MANIPULATOR& manipulator)
        // Invoke the specified 'manipulator' on the address of the underlying
        // value of this object.  Return the value from the invocation of
        // 'manipulator'.  The behavior is undefined unless this object does
        // not contain a null value.
    {
        BSLS_ASSERT(valueHasCategory(e_NULLABLE_VALUE));

        return NullableValueImpUtil::manipulateValue(&d_value.object(),
                                                     manipulator);
    }

    template <class MANIPULATOR>
    int sequenceManipulateAttribute(MANIPULATOR&  manipulator,
                                    const char   *attributeName,
                                    int           attributeNameLength)
        // Invoke the specified 'manipulator' on the address of the
        // (modifiable) attribute indicated by the specified 'attributeName' of
        // the specified 'attributeNameLength' of this object, supplying
        // 'manipulator' with the corresponding attribute information
        // structure.  Return a non-zero value if the attribute is not found,
        // and the value returned from the invocation of 'manipulator'
        // otherwise.
    {
        BSLS_ASSERT(valueHasCategory(e_SEQUENCE));

        return SequenceImpUtil::manipulateAttribute(&d_value.object(),
                                                    manipulator,
                                                    attributeName,
                                                    attributeNameLength);
    }

    template <class MANIPULATOR>
    int sequenceManipulateAttribute(MANIPULATOR& manipulator, int attributeId)
        // Invoke the specified 'manipulator' on the address of the
        // (modifiable) attribute indicated by the specified 'attributeId' of
        // this object, supplying 'manipulator' with the corresponding
        // attribute information structure.  Return a non-zero value if the
        // attribute is not found, and the value returned from the invocation
        // of 'manipulator' otherwise.
    {
        BSLS_ASSERT(valueHasCategory(e_SEQUENCE));

        return SequenceImpUtil::manipulateAttribute(
            &d_value.object(), manipulator, attributeId);
    }

    template <class MANIPULATOR>
    int sequenceManipulateAttributes(MANIPULATOR& manipulator)
        // Invoke the specified 'manipulator' sequentially on the address of
        // each (modifiable) attribute of this object, supplying 'manipulator'
        // with the corresponding attribute information structure until such
        // invocation returns a non-zero value.  Return the value from the last
        // invocation of 'manipulator'.
    {
        BSLS_ASSERT(valueHasCategory(e_SEQUENCE));

        return SequenceImpUtil::manipulateAttributes(&d_value.object(),
                                                     manipulator);
    }

    void reset() { bdlat_ValueTypeFunctions::reset(&d_value.object()); }

    void setValue(const Value& value) { d_value.object() = value; }

    // ACCESSORS
    const char *className() const
        // Return a null-terminated string containing the exported name for
        // this type.
    {
        return "MyDynamicType";
    }

    template <class ACCESSOR>
    int arrayAccessElement(ACCESSOR& accessor, int index) const
        // Invoke the specified 'accessor' on the non-modifiable element at the
        // specified 'index' of this object.  Return the value from the
        // invocation of 'accessor'.  The behavior is undefined unless
        // '0 <= index' and 'index < size(value)'.
    {
        BSLS_ASSERT(valueHasCategory(e_ARRAY));

        return ArrayImpUtil::accessElement(d_value.object(), accessor, index);
    }

    bsl::size_t arraySize() const
        // Return the number of elements in this object.
    {
        BSLS_ASSERT(valueHasCategory(e_ARRAY));

        return ArrayImpUtil::size(d_value.object());
    }

    template <class ACCESSOR>
    int choiceAccessSelection(ACCESSOR& accessor) const
        // Invoke the specified 'accessor' on the (non-modifiable) selection of
        // this object, supplying 'accessor' with the corresponding selection
        // information structure.  Return -1 if the selection is undefined, and
        // the value returned from the invocation of 'accessor' otherwise.
    {
        BSLS_ASSERT(valueHasCategory(e_CHOICE));

        return ChoiceImpUtil::accessSelection(d_value.object(), accessor);
    }

    bool choiceHasSelection(int selectionId) const
        // Return 'true' if this object has a selection with the specified
        // 'selectionId', and 'false' otherwise.
    {
        BSLS_ASSERT(valueHasCategory(e_CHOICE));

        return ChoiceImpUtil::hasSelection(d_value.object(), selectionId);
    }

    bool choiceHasSelection(const char *selectionName,
                            int         selectionNameLength) const
        // Return 'true' if this object has a selection with the specified
        // 'selectionName' of the specified 'selectionNameLength', and 'false'
        // otherwise.
    {
        BSLS_ASSERT(valueHasCategory(e_CHOICE));

        return ChoiceImpUtil::hasSelection(
            d_value.object(), selectionName, selectionNameLength);
    }

    int choiceSelectionId() const
        // Return the id of the current selection of this object if the
        // selection is defined, and -1 otherwise.
    {
        BSLS_ASSERT(valueHasCategory(e_CHOICE));

        return ChoiceImpUtil::selectionId(d_value.object());
    }

    const typename CustomizedTypeImpUtil::BaseType&
    customizedTypeConvertToBaseType() const
        // Return a reference providing non-modifiable access to the 'Base'
        // subobject of the underlying value of this object.
    {
        BSLS_ASSERT(valueHasCategory(e_CUSTOMIZED_TYPE));

        return CustomizedTypeImpUtil::convertToBaseType(d_value.object());
    }

    void enumerationToInt(int *result) const
        // Load into the specified 'result' the integer representation exactly
        // matching the enumerator name corresponding to the underlying value
        // of this object.
    {
        BSLS_ASSERT(valueHasCategory(e_ENUMERATION));

        EnumerationImpUtil::toInt(result, d_value.object());
    }

    void enumerationToString(bsl::string *result) const
        // Load into the specified 'result' the string representation exactly
        // matching the enumerator name corresponding to the underlying value
        // of this object.
    {
        BSLS_ASSERT(valueHasCategory(e_ENUMERATION));

        EnumerationImpUtil::toString(result, d_value.object());
    }

    template <class ACCESSOR>
    int nullableValueAccessValue(ACCESSOR& accessor) const
        // Invoke the specified 'accessor' on the non-modifiable underlying
        // value of this object.  Return the value from the invocation of
        // 'accessor'.  The behavior is undefined unless the underlying value
        // of this object does not contain a null value.
    {
        BSLS_ASSERT(valueHasCategory(e_NULLABLE_VALUE));

        return NullableValueImpUtil::accessValue(d_value.object(), accessor);
    }

    bool nullableValueIsNull() const
        // Return 'true' if the underlying value of this object contains a null
        // value, and 'false' otherwise.
    {
        BSLS_ASSERT(valueHasCategory(e_NULLABLE_VALUE));

        return NullableValueImpUtil::isNull(d_value.object());
    }

    template <class ACCESSOR>
    int sequenceAccessAttribute(ACCESSOR&   accessor,
                                const char *attributeName,
                                int         attributeNameLength) const
        // Invoke the specified 'accessor' on the (non-modifiable) attribute of
        // this object indicated by the specified 'attributeName' of the
        // specified 'attributeNameLength', supplying 'accessor' with the
        // corresponding attribute information structure.  Return a non-zero
        // value if the attribute is not found, and the value returned from the
        // invocation of 'accessor' otherwise.
    {
        BSLS_ASSERT(valueHasCategory(e_SEQUENCE));

        return SequenceImpUtil::accessAttribute(
            d_value.object(), accessor, attributeName, attributeNameLength);
    }

    template <class ACCESSOR>
    int sequenceAccessAttribute(ACCESSOR& accessor, int attributeId) const
        // Invoke the specified 'accessor' on the (non-modifiable) attribute of
        // this object with the specified 'attributeId', supplying 'accessor'
        // with the corresponding attribute information structure.  Return
        // non-zero if the attribute is not found, and the value returned from
        // the invocation of 'accessor' otherwise.
    {
        BSLS_ASSERT(valueHasCategory(e_SEQUENCE));

        return SequenceImpUtil::accessAttribute(
            d_value.object(), accessor, attributeId);
    }

    template <class ACCESSOR>
    int sequenceAccessAttributes(ACCESSOR& accessor) const
        // Invoke the specified 'accessor' sequentially on each attribute of
        // this object, supplying 'accessor' with the corresponding attribute
        // information structure until such invocation returns a non-zero
        // value.  Return the value from the last invocation of 'accessor'.
    {
        BSLS_ASSERT(valueHasCategory(e_SEQUENCE));

        return SequenceImpUtil::accessAttributes(d_value.object(), accessor);
    }

    bool sequenceHasAttribute(const char *attributeName,
                              int         attributeNameLength) const
        // Return 'true' if this object has an attribute with the specified
        // 'attributeName' of the specified 'attributeNameLength', and 'false'
        // otherwise.
    {
        BSLS_ASSERT(valueHasCategory(e_SEQUENCE));

        return SequenceImpUtil::hasAttribute(
            d_value.object(), attributeName, attributeNameLength);
    }

    bool sequenceHasAttribute(int attributeId) const
        // Return 'true' if this object has an attribute with the specified
        // 'attributeId', and 'false' otherwise.
    {
        BSLS_ASSERT(valueHasCategory(e_SEQUENCE));

        return SequenceImpUtil::hasAttribute(d_value.object(), attributeId);
    }

    bdlat_TypeCategory::Value select() const
        // Return the 'bdlat_TypeCategory::Value' value that identifies the
        // 'bdlat' concept that this object implements.
    {
        return static_cast<bdlat_TypeCategory::Value>(
            bdlat_TypeCategory::Select<Value>::e_SELECTION);
    }

    const Value& value() const { return d_value.object(); }
};

// FREE OPERATORS
template <class VALUE_TYPE>
bsl::ostream& operator<<(bsl::ostream&                      stream,
                         const TestDynamicType<VALUE_TYPE>& object)
{
    return stream << "[" << object.className() << " value " << object.value()
                  << "]";
}

template <class VALUE_TYPE>
inline
bool operator==(const TestDynamicType<VALUE_TYPE>& lhs,
                const TestDynamicType<VALUE_TYPE>& rhs)
{
    return TestDynamicType<VALUE_TYPE>::areEqual(lhs, rhs);
}

template <class VALUE_TYPE>
inline
bool operator!=(const TestDynamicType<VALUE_TYPE>& lhs,
                const TestDynamicType<VALUE_TYPE>& rhs)
{
    return !TestDynamicType<VALUE_TYPE>::areEqual(lhs, rhs);
}

// TRAITS
template <class VALUE_TYPE>
const char *bdlat_TypeName_className(const TestDynamicType<VALUE_TYPE>& object)
    // Return a null-terminated string containing the exported name for the
    // type of the specified 'object'.
{
    return object.className();
}

template <class VALUE_TYPE, class MANIPULATOR>
int bdlat_arrayManipulateElement(TestDynamicType<VALUE_TYPE> *object,
                                 MANIPULATOR&                 manipulator,
                                 int                          index)
    // Invoke the specified 'manipulator' on the address of the element at the
    // specified 'index' of the specified array 'object'.  Return the value
    // from the invocation of 'manipulator'.  The behavior is undefined unless
    // '0 <= index' and 'index < size(*value)'.
{
    return object->arrayManipulateElement(manipulator, index);
}

template <class VALUE_TYPE>
void bdlat_arrayResize(TestDynamicType<VALUE_TYPE> *object, int newSize)
    // Set the sizes of the specified array 'value' to the specified 'newSize'.
    // If 'newSize > size(*object)', then 'newSize - size(*object)' new
    // elements with default values are appended to 'value'.  If
    // 'newSize < size(*object)' then the 'size(*object) - newSize' elements at
    // the end of 'object' are destroyed.  The behavior is undefined unless
    // '0 <= newSize'.
{
    return object->arrayResize(newSize);
}

template <class VALUE_TYPE, class ACCESSOR>
int bdlat_arrayAccessElement(const TestDynamicType<VALUE_TYPE>& object,
                             ACCESSOR&                          accessor,
                             int                                index)
    // Invoke the specified 'accessor' on the non-modifiable element at the
    // specified 'index' of the specified array 'object'.  Return the value
    // from the invocation of 'accessor'.  The behavior is undefined unless
    // '0 <= index' and 'index < size(object)'.
{
    return object.arrayAccessElement(accessor, index);
}

template <class VALUE_TYPE>
bsl::size_t bdlat_arraySize(const TestDynamicType<VALUE_TYPE>& object)
    // Return the number of elements in the specified array 'object'.
{
    return object.arraySize();
}

template <class VALUE_TYPE>
int bdlat_choiceMakeSelection(TestDynamicType<VALUE_TYPE> *object,
                              int                          selectionId)
    // Set the value of the specified 'object' to the default for the selection
    // indicated by the specified 'selectionId'.  Return 0 on success, and a
    // non-zero value otherwise.
{
    return object->choiceMakeSelection(selectionId);
}

template <class VALUE_TYPE>
int bdlat_choiceMakeSelection(TestDynamicType<VALUE_TYPE> *object,
                              const char                  *selectionName,
                              int                          selectionNameLength)
    // Set the value of the specified 'object' to be the default for the
    // selection indicated by the specified 'selectionName' of the specified
    // 'selectionNameLength'.  Return 0 on success, and a non-zero value
    // otherwise.
{
    return object->choiceMakeSelection(selectionName, selectionNameLength);
}

template <class VALUE_TYPE, class MANIPULATOR>
int bdlat_choiceManipulateSelection(TestDynamicType<VALUE_TYPE> *object,
                                    MANIPULATOR&                 manipulator)
    // Invoke the specified 'manipulator' on the address of the (modifiable)
    // selection of the specified 'object', supplying 'manipulator' with the
    // corresponding selection information structure.  Return -1 if the
    // selection is undefined, and the value returned from the invocation of
    // 'manipulator' otherwise.
{
    return object->choiceManipulateSelection(manipulator);
}

template <class VALUE_TYPE, class ACCESSOR>
int bdlat_choiceAccessSelection(const TestDynamicType<VALUE_TYPE>& object,
                                ACCESSOR&                          accessor)
    // Invoke the specified 'accessor' on the (non-modifiable) selection of the
    // specified 'object', supplying 'accessor' with the corresponding
    // selection information structure.  Return -1 if the selection is
    // undefined, and the value returned from the invocation of 'accessor'
    // otherwise.
{
    return object.choiceAccessSelection(accessor);
}

template <class VALUE_TYPE>
bool bdlat_choiceHasSelection(const TestDynamicType<VALUE_TYPE>& object,
                              int                                selectionId)
    // Return 'true' if the specified 'object' has a selection with the
    // specified 'selectionId', and 'false' otherwise.
{
    return object.choiceHasSelection(selectionId);
}

template <class VALUE_TYPE>
bool bdlat_choiceHasSelection(
                       const TestDynamicType<VALUE_TYPE>&  object,
                       const char                         *selectionName,
                       int                                 selectionNameLength)
    // Return 'true' if the specified 'object' has a selection with the
    // specified 'selectionName' of the specified 'selectionNameLength', and
    // 'false' otherwise.
{
    return object.choiceHasSelection(selectionName, selectionNameLength);
}

template <class VALUE_TYPE, class BASE_TYPE>
int bdlat_customizedTypeConvertFromBaseType(
                                           TestDynamicType<VALUE_TYPE> *object,
                                           const BASE_TYPE&             value)
    // If an explicit conversion from the specified 'BASE_TYPE' type to the
    // specified 'VALUE_TYPE' type exists, load into the underlying value of
    // the specified 'object' the value of the specified 'base' object
    // explicitly converted to 'VALUE_TYPE'.  Return 0 on success, and a
    // non-zero value otherwise.
{
    return object->customizedTypeConvertFromBaseType(value);
}

template <class VALUE_TYPE>
const typename TestDynamicType<VALUE_TYPE>::BaseType&
bdlat_customizedTypeConvertToBaseType(
                                     const TestDynamicType<VALUE_TYPE>& object)
    // Return a reference providing non-modifiable access to the 'Base'
    // subobject of the underlying value of the specified 'object'.
{
    return object.customizedTypeConvertToBaseType();
}

template <class VALUE_TYPE>
int bdlat_enumFromInt(TestDynamicType<VALUE_TYPE> *object, int number)
    // Load into the specified 'object' the enumerator matching the specified
    // 'number'.  Return 0 on success, and a non-zero value with no effect on
    // 'value' if 'number' does not match any enumerator.
{
    return object->enumerationFromInt(number);
}

template <class VALUE_TYPE>
int bdlat_enumFromString(TestDynamicType<VALUE_TYPE> *object,
                         const char                  *string,
                         int                          stringLength)
    // Load into the specified 'object' the enumerator matching the specified
    // 'string' of the specified 'stringLength'.  Return 0 on success, and a
    // non-zero value with no effect on 'result' if 'string' and 'stringLength'
    // do not match any enumerator.
{
    return object->enumerationFromString(string, stringLength);
}

template <class VALUE_TYPE>
void bdlat_enumToInt(int *result, const TestDynamicType<VALUE_TYPE>& object)
    // Load into the specified 'result' the integer representation exactly
    // matching the enumerator name corresponding to the specified enumeration
    // 'object'.
{
    return object.enumerationToInt(result);
}

template <class VALUE_TYPE>
void bdlat_enumToString(bsl::string                        *result,
                        const TestDynamicType<VALUE_TYPE>&  object)
    // Load into the specified 'result' the string representation exactly
    // matching the enumerator name corresponding to the specified enumeration
    // 'object'.
{
    return object.enumerationToString(result);
}

template <class VALUE_TYPE>
void bdlat_nullableValueMakeValue(TestDynamicType<VALUE_TYPE> *object)
    // Assign to the specified "nullable" 'object' the default value for the
    // contained type.
{
    object->nullableValueMakeValue();
}

template <class VALUE_TYPE, class MANIPULATOR>
int bdlat_nullableValueManipulateValue(
                                      TestDynamicType<VALUE_TYPE> *object,
                                      MANIPULATOR&                 manipulator)
    // Invoke the specified 'manipulator' on the address of the value stored in
    // the specified "nullable" 'object'.  Return the value from the invocation
    // of 'manipulator'.  The behavior is undefined unless 'object' does not
    // contain a null value.
{
    return object->nullableValueManipulateValue(manipulator);
}

template <class VALUE_TYPE, class ACCESSOR>
int bdlat_nullableValueAccessValue(const TestDynamicType<VALUE_TYPE>& object,
                                   ACCESSOR&                          accessor)
    // Invoke the specified 'accessor' on the non-modifiable value stored in
    // the specified "nullable" 'object'.  Return the value from the invocation
    // of 'accessor'.  The behavior is undefined unless 'object' does not
    // contain a null value.
{
    return object.nullableValueAccessValue(accessor);
}

template <class VALUE_TYPE>
bool bdlat_nullableValueIsNull(const TestDynamicType<VALUE_TYPE>& object)
    // Return 'true' if the specified "nullable" 'object' contains a null
    // value, and 'false' otherwise.
{
    return object.nullableValueIsNull();
}

template <class VALUE_TYPE>
int bdlat_choiceSelectionId(const TestDynamicType<VALUE_TYPE>& object)
    // Return the id of the current selection of the specified 'object' if the
    // selection is defined, and -1 otherwise.
{
    return object.choiceSelectionId();
}

template <class VALUE_TYPE, class ACCESSOR>
int bdlat_sequenceAccessAttribute(
                       const TestDynamicType<VALUE_TYPE>&  object,
                       ACCESSOR&                           accessor,
                       const char                         *attributeName,
                       int                                 attributeNameLength)
    // Invoke the specified 'accessor' on the (non-modifiable) attribute of the
    // specified 'object' indicated by the specified 'attributeName' of the
    // specified 'attributeNameLength', supplying 'accessor' with the
    // corresponding attribute information structure.  Return a non-zero value
    // if the attribute is not found, and the value returned from the
    // invocation of 'accessor' otherwise.
{
    return object.sequenceAccessAttribute(
        accessor, attributeName, attributeNameLength);
}

template <class VALUE_TYPE, class ACCESSOR>
int bdlat_sequenceAccessAttribute(
                                const TestDynamicType<VALUE_TYPE>& object,
                                ACCESSOR&                          accessor,
                                int                                attributeId)
    // Invoke the specified 'accessor' on the (non-modifiable) attribute of the
    // specified 'object' with the specified 'attributeId', supplying
    // 'accessor' with the corresponding attribute information structure.
    // Return non-zero if the attribute is not found, and the value returned
    // from the invocation of 'accessor' otherwise.
{
    return object.sequenceAccessAttribute(accessor, attributeId);
}

template <class VALUE_TYPE, class ACCESSOR>
int bdlat_sequenceAccessAttributes(const TestDynamicType<VALUE_TYPE>& object,
                                   ACCESSOR&                          accessor)
    // Invoke the specified 'accessor' sequentially on each attribute of the
    // specified 'object', supplying 'accessor' with the corresponding
    // attribute information structure until such invocation returns a non-zero
    // value.  Return the value from the last invocation of 'accessor'.
{
    return object.sequenceAccessAttributes(accessor);
}

template <class VALUE_TYPE>
int bdlat_sequenceHasAttribute(
                       const TestDynamicType<VALUE_TYPE>&  object,
                       const char                         *attributeName,
                       int                                 attributeNameLength)
    // Return 'true' if the specified 'object' has an attribute with the
    // specified 'attributeName' of the specified 'attributeNameLength', and
    // 'false' otherwise.
{
    return object.sequenceHasAttribute(attributeName, attributeNameLength);
}

template <class VALUE_TYPE>
int bdlat_sequenceHasAttribute(const TestDynamicType<VALUE_TYPE>& object,
                               int                                attributeId)
    // Return 'true' if the specified 'object' has an attribute with the
    // specified 'attributeId', and 'false' otherwise.
{
    return object.sequenceHasAttribute(attributeId);
}

template <class VALUE_TYPE, class MANIPULATOR>
int bdlat_sequenceManipulateAttribute(
                              TestDynamicType<VALUE_TYPE> *object,
                              MANIPULATOR&                 manipulator,
                              const char                  *attributeName,
                              int                          attributeNameLength)
    // Invoke the specified 'manipulator' on the address of the (modifiable)
    // attribute indicated by the specified 'attributeName' of the specified
    // 'attributeNameLength' of the specified 'object', supplying 'manipulator'
    // with the corresponding attribute information structure.  Return a
    // non-zero value if the attribute is not found, and the value returned
    // from the invocation of 'manipulator' otherwise.
{
    return object->sequenceManipulateAttribute(
        manipulator, attributeName, attributeNameLength);
}

template <class VALUE_TYPE, class MANIPULATOR>
int bdlat_sequenceManipulateAttribute(TestDynamicType<VALUE_TYPE> *object,
                                      MANIPULATOR&                 manipulator,
                                      int                          attributeId)
    // Invoke the specified 'manipulator' on the address of the (modifiable)
    // attribute indicated by the specified 'attributeId' of the specified
    // 'object', supplying 'manipulator' with the corresponding attribute
    // information structure.  Return a non-zero value if the attribute is not
    // found, and the value returned from the invocation of 'manipulator'
    // otherwise.
{
    return object->sequenceManipulateAttribute(manipulator, attributeId);
}

template <class VALUE_TYPE, class MANIPULATOR>
int bdlat_sequenceManipulateAttributes(
                                      TestDynamicType<VALUE_TYPE> *object,
                                      MANIPULATOR&                 manipulator)
    // Invoke the specified 'manipulator' sequentially on the address of each
    // (modifiable) attribute of the specified 'object', supplying
    // 'manipulator' with the corresponding attribute information structure
    // until such invocation returns a non-zero value.  Return the value from
    // the last invocation of 'manipulator'.
{
    return object->sequenceManipulateAttributes(manipulator);
}

template <class VALUE_TYPE>
bdlat_TypeCategory::Value bdlat_typeCategorySelect(
                                     const TestDynamicType<VALUE_TYPE>& object)
    // Return the 'bdlat_TypeCategory::Value' value that identifies the 'bdlat'
    // concept of the specified 'object'.
{
    return object.select();
}

template <class VALUE_TYPE>
struct bdlat_TypeCategoryDeclareDynamic<TestDynamicType<VALUE_TYPE> > {
    enum { VALUE = 1 };
};

template <class VALUE_TYPE,
          bool IS_ARRAY =
              static_cast<bdlat_TypeCategory::Value>(
                  bdlat_TypeCategory::Select<VALUE_TYPE>::e_SELECTION) ==
              bdlat_TypeCategory::e_ARRAY_CATEGORY>
struct TestDynamicType_ElementTypeImpl {
    typedef typename bdlat_ArrayFunctions::ElementType<VALUE_TYPE>::Type Type;
};

template <class VALUE_TYPE>
struct TestDynamicType_ElementTypeImpl<VALUE_TYPE, false> {
    typedef struct {
    } Type;
};

namespace bdlat_ArrayFunctions {

template <class VALUE_TYPE>
struct ElementType<TestDynamicType<VALUE_TYPE> > {
    typedef typename TestDynamicType_ElementTypeImpl<VALUE_TYPE>::Type Type;
};

template <class VALUE_TYPE>
struct IsArray<TestDynamicType<VALUE_TYPE> > {
    enum { VALUE = IsArray<VALUE_TYPE>::VALUE };
};

}  // close bdlat_ArrayFunctions namespace

template <class VALUE_TYPE>
struct bdlat_IsBasicChoice<TestDynamicType<VALUE_TYPE> >
: bdlat_IsBasicChoice<VALUE_TYPE> {
};

namespace bdlat_ChoiceFunctions {

template <class VALUE_TYPE>
struct IsChoice<TestDynamicType<VALUE_TYPE> > {
    enum { VALUE = IsChoice<VALUE_TYPE>::VALUE };
};

}  // close bdlat_ChoiceFunctions namespace

template <class VALUE_TYPE,
          bool IS_CUSTOMIZED_TYPE =
              static_cast<bdlat_TypeCategory::Value>(
                  bdlat_TypeCategory::Select<VALUE_TYPE>::e_SELECTION) ==
              bdlat_TypeCategory::e_CUSTOMIZED_TYPE_CATEGORY>
struct TestDynamicType_BaseTypeImpl {
    typedef typename bdlat_CustomizedTypeFunctions::BaseType<VALUE_TYPE>::Type
        Type;
};

template <class VALUE_TYPE>
struct TestDynamicType_BaseTypeImpl<VALUE_TYPE, false> {
    typedef struct {
    } Type;
};

template <class VALUE_TYPE>
struct bdlat_IsBasicCustomizedType<TestDynamicType<VALUE_TYPE> >
: bdlat_IsBasicCustomizedType<VALUE_TYPE> {
};

namespace bdlat_CustomizedTypeFunctions {

template <class VALUE_TYPE>
struct BaseType<TestDynamicType<VALUE_TYPE> > {
    typedef typename TestDynamicType_BaseTypeImpl<VALUE_TYPE>::Type Type;
};

template <class VALUE_TYPE>
struct IsCustomizedType<TestDynamicType<VALUE_TYPE> > {
    enum { VALUE = IsCustomizedType<VALUE_TYPE>::VALUE };
};

}  // close bdlat_CustomizedTypeFunctions namespace

template <class VALUE_TYPE>
struct bdlat_IsBasicEnumeration<TestDynamicType<VALUE_TYPE> >
: bdlat_IsBasicEnumeration<VALUE_TYPE> {
};

namespace bdlat_EnumFunctions {

template <class VALUE_TYPE>
struct IsEnumeration<TestDynamicType<VALUE_TYPE> > {
    enum { VALUE = IsEnumeration<VALUE_TYPE>::VALUE };
};

}  // close bdlat_EnumFunctions namespace

template <class VALUE_TYPE,
          bool IS_NULLABLE_VALUE =
              static_cast<bdlat_TypeCategory::Value>(
                  bdlat_TypeCategory::Select<VALUE_TYPE>::e_SELECTION) ==
              bdlat_TypeCategory::e_NULLABLE_VALUE_CATEGORY>
struct TestDynamicType_ValueTypeImpl {
    typedef typename bdlat_NullableValueFunctions::ValueType<VALUE_TYPE>::Type
        Type;
};

template <class VALUE_TYPE>
struct TestDynamicType_ValueTypeImpl<VALUE_TYPE, false> {
    typedef struct {
    } Type;
};

namespace bdlat_NullableValueFunctions {

template <class VALUE_TYPE>
struct ValueType<TestDynamicType<VALUE_TYPE> > {
    typedef typename TestDynamicType_ValueTypeImpl<VALUE_TYPE>::Type Type;
};

template <class VALUE_TYPE>
struct IsNullableValue<TestDynamicType<VALUE_TYPE> > {
    enum { VALUE = IsNullableValue<VALUE_TYPE>::VALUE };
};

}  // close bdlat_NullableValueFunctions namespace

template <class VALUE_TYPE>
struct bdlat_IsBasicSequence<TestDynamicType<VALUE_TYPE> >
: bdlat_IsBasicSequence<VALUE_TYPE> {
};

namespace bdlat_SequenceFunctions {

template <class VALUE_TYPE>
struct IsSequence<TestDynamicType<VALUE_TYPE> > {
    enum { VALUE = IsSequence<VALUE_TYPE>::VALUE };
};

}  // close bdlat_SequenceFunctions namespace

                            // ====================
                            // class TestEnumerator
                            // ====================

template <int INT_VALUE, const char *STRING_VALUE>
class TestEnumerator {
    // This class provides a namespace for a suite of constant-initialized data
    // that can be used to specify the integer and string values of an
    // enumerator for a 'bdlat' 'Enumeration' type.

  public:
    // CLASS DATA
    static const char *k_STRING_VALUE;
    enum { k_INT_VALUE = INT_VALUE };

    // CLASS METHODS
    static int               intValue() { return k_INT_VALUE; }
    static bslstl::StringRef stringValue() { return k_STRING_VALUE; }

    // CREATORS
    TestEnumerator() {}
};

// CLASS DATA
template <int INT_VALUE, const char *STRING_VALUE>
const char *TestEnumerator<INT_VALUE, STRING_VALUE>::k_STRING_VALUE =
    STRING_VALUE;

                          // =======================
                          // class TestNilEnumerator
                          // =======================

class TestNilEnumerator {
    // This class provides a namespace for a type having a set of public
    // members with the same names and types as 'TestEnumerator', and that can
    // be used as a sentinel type to indicate that no such enumerator exists.

  public:
    // CLASS DATA
    static const char *k_STRING_VALUE;
    enum { k_INT_VALUE = 0 };

    // CLASS METHODS
    static int               intValue() { return k_INT_VALUE; }
    static bslstl::StringRef stringValue() { return k_STRING_VALUE; }
};

// CLASS DATA
const char *TestNilEnumerator::k_STRING_VALUE = "";

                           // =====================
                           // class TestEnumeration
                           // =====================

template <class E0, class E1 = TestNilEnumerator, class E2 = TestNilEnumerator>
class TestEnumeration {
    // This in-core value-semantic class provides a basic implementation of the
    // 'bdlat' 'Enumeration' concept.

  public:
    // INVARIANTS
    BSLMF_ASSERT((!bslmf::IsSame<TestNilEnumerator, E0>::value));

    // TYPES
    typedef E0 Enumerator0;
    typedef E1 Enumerator1;
    typedef E2 Enumerator2;

    // CLASS DATA
    enum {
        k_HAS_ENUMERATOR_1 = !bslmf::IsSame<TestNilValue, Enumerator1>::value,
        k_HAS_ENUMERATOR_2 = !bslmf::IsSame<TestNilValue, Enumerator2>::value
    };

  private:
    // DATA
    int d_value;

  public:
    // CLASS METHODS
    static bool areEqual(const TestEnumeration& lhs,
                         const TestEnumeration& rhs)
    {
        return lhs.d_value == rhs.d_value;
    }

    // CREATORS
    TestEnumeration()
    : d_value(Enumerator0::intValue())
    {
    }

    explicit TestEnumeration(int value)
    : d_value(value)
    {
    }

    TestEnumeration(const TestEnumeration& original)
    : d_value(original.d_value)
    {
    }

    // MANIPULATORS
    TestEnumeration& operator=(const TestEnumeration& original)
    {
        d_value = original.d_value;
        return *this;
    }

    int fromInt(int number)
        // Load into the underlying value of this object the enumerator
        // matching the specified 'number'.  Return 0 on success, and a
        // non-zero value with no effect on 'value' if 'number' does not match
        // any enumerator.
    {
        if (Enumerator0::intValue() == number) {
            d_value = Enumerator0::intValue();
            return 0;                                                 // RETURN
        }

        if (k_HAS_ENUMERATOR_1 && Enumerator1::intValue() == number) {
            d_value = Enumerator1::intValue();
            return 0;                                                 // RETURN
        }

        if (k_HAS_ENUMERATOR_2 && Enumerator2::intValue() == number) {
            d_value = Enumerator2::intValue();
            return 0;                                                 // RETURN
        }

        return -1;
    }

    int fromString(const char *string, int stringLength)
        // Load into the underlying value of this object the enumerator
        // matching the specified 'string' of the specified 'stringLength'.
        // Return 0 on success, and a non-zero value with no effect on 'result'
        // if 'string' and 'stringLength' do not match any enumerator.
    {
        const bslstl::StringRef stringRef(string, stringLength);

        if (Enumerator0::stringValue() == stringRef) {
            d_value = Enumerator0::intValue();
            return 0;                                                 // RETURN
        }

        if (k_HAS_ENUMERATOR_1 && Enumerator1::stringValue() == stringRef) {
            d_value = Enumerator1::intValue();
            return 0;                                                 // RETURN
        }

        if (k_HAS_ENUMERATOR_2 && Enumerator2::stringValue() == stringRef) {
            d_value = Enumerator2::intValue();
            return 0;                                                 // RETURN
        }

        return -1;
    }

    // ACCESSORS
    const char *className() const
        // Return a null-terminated string containing the exported name for
        // this type.
    {
        return "MyEnumeration";
    }

    void toInt(int *result) const
        // Load into the specified 'result' the integer representation exactly
        // matching the enumerator name corresponding to the underlying value
        // of this object.
    {
        BSLS_ASSERT(
            d_value == Enumerator0::intValue() ||
            (k_HAS_ENUMERATOR_1 && d_value == Enumerator1::intValue()) ||
            (k_HAS_ENUMERATOR_2 && d_value == Enumerator2::intValue()));

        *result = d_value;
    }

    void toString(bsl::string *result) const
        // Load into the specified 'result' the string representation exactly
        // matching the enumerator name corresponding to the underlying value
        // of this object.
    {
        BSLS_ASSERT(
            d_value == Enumerator0::intValue() ||
            (k_HAS_ENUMERATOR_1 && d_value == Enumerator1::intValue()) ||
            (k_HAS_ENUMERATOR_2 && d_value == Enumerator2::intValue()));

        if (Enumerator0::intValue() == d_value) {
            *result = Enumerator0::stringValue();
        }

        if (k_HAS_ENUMERATOR_1 && Enumerator1::intValue() == d_value) {
            *result = Enumerator1::stringValue();
        }

        if (k_HAS_ENUMERATOR_2 && Enumerator2::intValue() == d_value) {
            *result = Enumerator2::stringValue();
        }
    }
};

// FREE FUNCTIONS
template <class E0, class E1, class E2>
bsl::ostream& operator<<(bsl::ostream&                      stream,
                         const TestEnumeration<E0, E1, E2>& object)
{
    bsl::string value;
    object.toString(&value);

    return stream << "[" << object.className() << " value " << value << "]";
}

template <class E0, class E1, class E2>
bool operator==(const TestEnumeration<E0, E1, E2>& lhs,
                const TestEnumeration<E0, E1, E2>& rhs)
{
    return TestEnumeration<E0, E1, E2>::areEqual(lhs, rhs);
}

template <class E0, class E1, class E2>
bool operator!=(const TestEnumeration<E0, E1, E2>& lhs,
                const TestEnumeration<E0, E1, E2>& rhs)
{
    return !TestEnumeration<E0, E1, E2>::areEqual(lhs, rhs);
}

// TRAITS
template <class E0, class E1, class E2>
const char *bdlat_TypeName_className(const TestEnumeration<E0, E1, E2>& object)
    // Return a null-terminated string containing the exported name for the
    // type of the specified 'object'.
{
    return object.className();
}

template <class E0, class E1, class E2>
int bdlat_enumFromInt(TestEnumeration<E0, E1, E2> *result, int number)
    // Load into the specified 'result' the enumerator matching the specified
    // 'number'.  Return 0 on success, and a non-zero value with no effect on
    // 'value' if 'number' does not match any enumerator.
{
    return result->fromInt(number);
}

template <class E0, class E1, class E2>
int bdlat_enumFromString(TestEnumeration<E0, E1, E2> *result,
                         const char                  *string,
                         int                          stringLength)
    // Load into the specified 'result' the enumerator matching the specified
    // 'string' of the specified 'stringLength'.  Return 0 on success, and a
    // non-zero value with no effect on 'result' if 'string' and 'stringLength'
    // do not match any enumerator.
{
    return result->fromString(string, stringLength);
}

template <class E0, class E1, class E2>
void bdlat_enumToInt(int *result, const TestEnumeration<E0, E1, E2>& value)
    // Load into the specified 'result' the integer representation exactly
    // matching the enumerator name corresponding to the specified enumeration
    // 'value'.
{
    value.toInt(result);
}

template <class E0, class E1, class E2>
void bdlat_enumToString(bsl::string                        *result,
                        const TestEnumeration<E0, E1, E2>&  value)
    // Load into the specified 'result' the string representation exactly
    // matching the enumerator name corresponding to the specified enumeration
    // 'value'.
{
    value.toString(result);
}

namespace bdlat_EnumFunctions {

template <class E0, class E1, class E2>
struct IsEnumeration<TestEnumeration<E0, E1, E2> > {
    enum { VALUE = 1 };
};

}  // close bdlat_EnumFunctions namespace

                        // ============================
                        // struct TestAttributeDefaults
                        // ============================

struct TestAttributeDefaults {
    // This utility 'struct' provides a namespace for non-modifiable,
    // constant-initialized default values for the non-type template parameters
    // of a 'TestAttribute'.

    // CLASS DATA
    static const char k_DEFAULT_ANNOTATION[1];
    static const char k_DEFAULT_NAME[1];
    enum { k_DEFAULT_FORMATTING_MODE = 0 };
};

// CLASS DATA
const char TestAttributeDefaults::k_DEFAULT_ANNOTATION[1] = "";
const char TestAttributeDefaults::k_DEFAULT_NAME[1]       = "";

                            // ===================
                            // class TestAttribute
                            // ===================

template <int         ID,
          const char *NAME       = TestAttributeDefaults::k_DEFAULT_NAME,
          const char *ANNOTATION = TestAttributeDefaults::k_DEFAULT_ANNOTATION,
          int         FORMATTING_MODE =
              TestAttributeDefaults::k_DEFAULT_FORMATTING_MODE>
class TestAttribute {
    // This class provides a namespace for a suite of non-modifiable,
    // constant-initialized data that can be used to specify all attributes of
    // a 'bdlat_AttributeInfo' object.

  public:
    // CLASS DATA
    static const char *k_NAME;
    static const char *k_ANNOTATION;
    enum { k_ID = ID, k_FORMATTING_MODE = FORMATTING_MODE };

    // CLASS METHODS
    static int id() { return k_ID; }

    static bslstl::StringRef name() { return k_NAME; }

    static bslstl::StringRef annotation() { return k_ANNOTATION; }

    static int formattingMode() { return k_FORMATTING_MODE; }

    static bdlat_AttributeInfo attributeInfo()
    {
        bdlat_AttributeInfo result = {k_ID,
                                      k_NAME,
                                      static_cast<int>(bsl::strlen(k_NAME)),
                                      k_ANNOTATION,
                                      k_FORMATTING_MODE};

        return result;
    }

    // CREATORS
    TestAttribute() {}
};

// CLASS DATA
template <int         ID,
          const char *NAME,
          const char *ANNOTATION,
          int         FORMATTING_MODE>
const char *TestAttribute<ID, NAME, ANNOTATION, FORMATTING_MODE>::k_NAME =
    NAME;

template <int         ID,
          const char *NAME,
          const char *ANNOTATION,
          int         FORMATTING_MODE>
const char
    *TestAttribute<ID, NAME, ANNOTATION, FORMATTING_MODE>::k_ANNOTATION =
        ANNOTATION;

                          // ========================
                          // class TypedTestAttribute
                          // ========================

template <class TYPE, class TEST_ATTRIBUTE>
class TypedTestAttribute {
    // This class provides two member type definitions, 'Type', and
    // 'Attribute', which are aliases to the arguments supplied to the 'TYPE'
    // and 'TEST_SELECTION' template parameters, respectively.

  public:
    // TYPES
    typedef TYPE           Type;
    typedef TEST_ATTRIBUTE Attribute;
};

                             // ==================
                             // class TestSequence
                             // ==================

template <class TYPED_ATTRIBUTE_0 =
              TypedTestAttribute<TestNilValue, TestAttribute<0> >,
          class TYPED_ATTRIBUTE_1 =
              TypedTestAttribute<TestNilValue, TestAttribute<0> >,
          class TYPED_ATTRIBUTE_2 =
              TypedTestAttribute<TestNilValue, TestAttribute<0> > >
class TestSequence {
    // This in-core value-semantic class provides a basic implementation of the
    // concept defined by the 'bdlat' 'Sequence' type category.  The template
    // parameters 'TYPED_ATTRIBUTE_0', 'TYPED_ATTRIBUTE_1', and
    // 'TYPED_ATTRIBUTE_2' must all satisfy the following requirements:
    //: o The type must have two member type definitions, 'Type' and
    // 'Attribute'.
    //: o 'Type' must meet the requirements of an in-core value-semantic type.
    //: o 'Type' must meet the requirements of exactly one of the
    //:    'bdlat' value categories.
    //: o 'Attribute' must be a specialization of the 'TestAttribute' type.
    // Further, each 'TestAttribute' member type definition of a template
    // argument must return values for 'TestAttribute::id()' and
    // 'TestAttribute::name()' that are different from all others within this
    // 'TestSequence' specialization.
    //
    // Additionally, the 'Type' of any template argument may be 'TestNilValue'
    // if all 'Type' member type definitions of subsequent template arguments
    // are also 'TestNilValue'.
    //
    // The 'Type' and 'Attribute' member type definitions of the template
    // arguments define the type and 'bdlat_AttributeInfo' of the attributes of
    // the 'bdlat' 'Selection' implementation provided by this class.  A
    // template argument having a 'TestNilValue' 'Type' indicates that the
    // corresponding attribute does not exist.

  public:
    // TYPES
    typedef typename TYPED_ATTRIBUTE_0::Type      Attribute0Type;
    typedef typename TYPED_ATTRIBUTE_0::Attribute Attribute0;
    typedef typename TYPED_ATTRIBUTE_1::Type      Attribute1Type;
    typedef typename TYPED_ATTRIBUTE_1::Attribute Attribute1;
    typedef typename TYPED_ATTRIBUTE_2::Type      Attribute2Type;
    typedef typename TYPED_ATTRIBUTE_2::Attribute Attribute2;

    // CLASS DATA
    enum {
        k_HAS_ATTRIBUTE_0 =
            !bslmf::IsSame<TestNilValue, Attribute0Type>::value,
        k_HAS_ATTRIBUTE_1 =
            !bslmf::IsSame<TestNilValue, Attribute1Type>::value,
        k_HAS_ATTRIBUTE_2 = !bslmf::IsSame<TestNilValue, Attribute2Type>::value
    };

  private:
    // DATA
    bslalg::ConstructorProxy<Attribute0Type>  d_attribute0Value;
    bslalg::ConstructorProxy<Attribute1Type>  d_attribute1Value;
    bslalg::ConstructorProxy<Attribute2Type>  d_attribute2Value;
    bslma::Allocator                         *d_allocator_p;

  public:
    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(TestSequence, bslma::UsesBslmaAllocator);

    // CLASS METHODS
    static bool areEqual(const TestSequence& lhs, const TestSequence& rhs)
    {
        return lhs.d_attribute0Value.object() ==
                   rhs.d_attribute0Value.object() &&
               lhs.d_attribute1Value.object() ==
                   rhs.d_attribute1Value.object() &&
               lhs.d_attribute2Value.object() ==
                   rhs.d_attribute2Value.object();
    }

    // CREATORS
    TestSequence()
    : d_attribute0Value(bslma::Default::allocator())
    , d_attribute1Value(bslma::Default::allocator())
    , d_attribute2Value(bslma::Default::allocator())
    , d_allocator_p(bslma::Default::allocator())
    {
    }

    explicit TestSequence(bslma::Allocator *basicAllocator)
    : d_attribute0Value(bslma::Default::allocator(basicAllocator))
    , d_attribute1Value(bslma::Default::allocator(basicAllocator))
    , d_attribute2Value(bslma::Default::allocator(basicAllocator))
    , d_allocator_p(bslma::Default::allocator(basicAllocator))
    {
    }

    explicit TestSequence(const Attribute0Type  attribute0,
                          bslma::Allocator     *basicAllocator = 0)
    : d_attribute0Value(attribute0, bslma::Default::allocator(basicAllocator))
    , d_attribute1Value(bslma::Default::allocator(basicAllocator))
    , d_attribute2Value(bslma::Default::allocator(basicAllocator))
    , d_allocator_p(bslma::Default::allocator(basicAllocator))
    {
        BSLMF_ASSERT(k_HAS_ATTRIBUTE_0);
    }

    explicit TestSequence(const Attribute0Type  attribute0,
                          const Attribute1Type  attribute1,
                          bslma::Allocator     *basicAllocator = 0)
    : d_attribute0Value(attribute0, bslma::Default::allocator(basicAllocator))
    , d_attribute1Value(attribute1, bslma::Default::allocator(basicAllocator))
    , d_attribute2Value(bslma::Default::allocator(basicAllocator))
    , d_allocator_p(bslma::Default::allocator(basicAllocator))
    {
        BSLMF_ASSERT(k_HAS_ATTRIBUTE_0);
        BSLMF_ASSERT(k_HAS_ATTRIBUTE_1);
    }

    explicit TestSequence(const Attribute0Type  attribute0,
                          const Attribute1Type  attribute1,
                          const Attribute2Type  attribute2,
                          bslma::Allocator     *basicAllocator = 0)
    : d_attribute0Value(attribute0, bslma::Default::allocator(basicAllocator))
    , d_attribute1Value(attribute1, bslma::Default::allocator(basicAllocator))
    , d_attribute2Value(attribute2, bslma::Default::allocator(basicAllocator))
    , d_allocator_p(bslma::Default::allocator(basicAllocator))
    {
        BSLMF_ASSERT(k_HAS_ATTRIBUTE_0);
        BSLMF_ASSERT(k_HAS_ATTRIBUTE_1);
        BSLMF_ASSERT(k_HAS_ATTRIBUTE_2);
    }

    TestSequence(const TestSequence&  original,
                 bslma::Allocator    *basicAllocator = 0)
    : d_attribute0Value(original.d_attribute0Value.object(),
                        bslma::Default::allocator(basicAllocator))
    , d_attribute1Value(original.d_attribute1Value.object(),
                        bslma::Default::allocator(basicAllocator))
    , d_attribute2Value(original.d_attribute2Value.object(),
                        bslma::Default::allocator(basicAllocator))
    , d_allocator_p(bslma::Default::allocator(basicAllocator))
    {
    }

    // MANIPULATORS
    TestSequence& operator=(const TestSequence& original)
    {
        d_attribute0Value.object() = original.d_attribute0Value.object();
        d_attribute1Value.object() = original.d_attributa1Value.object();
        d_attribute2Value.object() = original.d_attribute2Value.object();
        return *this;
    }

    template <class MANIPULATOR>
    int manipulateAttribute(MANIPULATOR&  manipulator,
                            const char   *attributeName,
                            int           attributeNameLength)
        // Invoke the specified 'manipulator' on the address of the
        // (modifiable) attribute indicated by the specified 'attributeName'
        // and 'attributeNameLength' of this object, supplying 'manipulator'
        // with the corresponding attribute information structure.  Return a
        // non-zero value if the attribute is not found, and the value returned
        // from the invocation of 'manipulator' otherwise.
    {
        const bslstl::StringRef attributeNameRef(attributeName       ,
                                                 attributeNameLength);

        if (k_HAS_ATTRIBUTE_0 && Attribute0::name() == attributeNameRef) {
            return manipulator(&d_attribute0Value.object(),
                               Attribute0::attributeInfo());          // RETURN
        }

        if (k_HAS_ATTRIBUTE_1 && Attribute1::name() == attributeNameRef) {
            return manipulator(&d_attribute1Value.object(),
                               Attribute1::attributeInfo());          // RETURN
        }

        if (k_HAS_ATTRIBUTE_2 && Attribute2::name() == attributeNameRef) {
            return manipulator(&d_attribute2Value.object(),
                               Attribute2::attributeInfo());          // RETURN
        }

        return -1;
    }

    template <class MANIPULATOR>
    int manipulateAttribute(MANIPULATOR& manipulator, int attributeId)
        // Invoke the specified 'manipulator' on the address of the
        // (modifiable) attribute indicated by the specified 'attributeId' of
        // this object, supplying 'manipulator' with the corresponding
        // attribute information structure.  Return a non-zero value if the
        // attribute is not found, and the value returned from the invocation
        // of 'manipulator' otherwise.
    {
        if (k_HAS_ATTRIBUTE_0 && Attribute0::id() == attributeId) {
            return manipulator(&d_attribute0Value.object(),
                               Attribute0::attributeInfo());          // RETURN
        }

        if (k_HAS_ATTRIBUTE_1 && Attribute1::id() == attributeId) {
            return manipulator(&d_attribute1Value.object(),
                               Attribute1::attributeInfo());          // RETURN
        }

        if (k_HAS_ATTRIBUTE_2 && Attribute2::id() == attributeId) {
            return manipulator(&d_attribute2Value.object(),
                               Attribute2::attributeInfo());          // RETURN
        }

        return -1;
    }

    template <class MANIPULATOR>
    int manipulateAttributes(MANIPULATOR& manipulator)
        // Invoke the specified 'manipulator' sequentially on the address of
        // each (modifiable) attribute of this object, supplying 'manipulator'
        // with the corresponding attribute information structure until such
        // invocation returns a non-zero value.  Return the value from the last
        // invocation of 'manipulator'.
    {
        if (k_HAS_ATTRIBUTE_0) {
            int rc = manipulator(&d_attribute0Value.object(),
                                 Attribute0::attributeInfo());
            if (0 != rc) {
                return rc;                                            // RETURN
            }
        }

        if (k_HAS_ATTRIBUTE_1) {
            int rc = manipulator(&d_attribute1Value.object(),
                                 Attribute1::attributeInfo());
            if (0 != rc) {
                return rc;                                            // RETURN
            }
        }

        if (k_HAS_ATTRIBUTE_2) {
            int rc = manipulator(&d_attribute2Value.object(),
                                 Attribute2::attributeInfo());
            if (0 != rc) {
                return rc;                                            // RETURN
            }
        }

        return 0;
    }

    void reset()
    {
        if (k_HAS_ATTRIBUTE_0) {
            bdlat_ValueTypeFunctions::reset(&d_attribute0Value.object());
        }

        if (k_HAS_ATTRIBUTE_1) {
            bdlat_ValueTypeFunctions::reset(&d_attribute1Value.object());
        }

        if (k_HAS_ATTRIBUTE_2) {
            bdlat_ValueTypeFunctions::reset(&d_attribute2Value.object());
        }
    }

    // ACCESSORS
    template <class ACCESSOR>
    int accessAttribute(ACCESSOR&   accessor,
                        const char *attributeName,
                        int         attributeNameLength) const
        // Invoke the specified 'accessor' on the (non-modifiable) attribute of
        // this object indicated by the specified 'attributeName' and
        // 'attributeNameLength', supplying 'accessor' with the corresponding
        // attribute information structure.  Return a non-zero value if the
        // attribute is not found, and the value returned from the invocation
        // of 'accessor' otherwise.
    {
        const bslstl::StringRef attributeNameRef(attributeName       ,
                                                 attributeNameLength);

        if (k_HAS_ATTRIBUTE_0 && Attribute0::name() == attributeNameRef) {
            return accessor(d_attribute0Value.object(),
                            Attribute0::attributeInfo());             // RETURN
        }

        if (k_HAS_ATTRIBUTE_1 && Attribute1::name() == attributeNameRef) {
            return accessor(d_attribute1Value.object(),
                            Attribute1::attributeInfo());             // RETURN
        }

        if (k_HAS_ATTRIBUTE_2 && Attribute2::name() == attributeNameRef) {
            return accessor(d_attribute2Value.object(),
                            Attribute2::attributeInfo());             // RETURN
        }

        return -1;
    }

    template <class ACCESSOR>
    int accessAttribute(ACCESSOR& accessor, int attributeId) const
        // Invoke the specified 'accessor' on the attribute of this object with
        // the given 'attributeId', supplying 'accessor' with the corresponding
        // attribute information structure.  Return non-zero if the attribute
        // is not found, and the value returned from the invocation of
        // 'accessor' otherwise.
    {
        if (k_HAS_ATTRIBUTE_0 && Attribute0::id() == attributeId) {
            return accessor(d_attribute0Value.object(),
                            Attribute0::attributeInfo());             // RETURN
        }

        if (k_HAS_ATTRIBUTE_1 && Attribute1::id() == attributeId) {
            return accessor(d_attribute1Value.object(),
                            Attribute1::attributeInfo());             // RETURN
        }

        if (k_HAS_ATTRIBUTE_2 && Attribute2::id() == attributeId) {
            return accessor(d_attribute2Value.object(),
                            Attribute2::attributeInfo());             // RETURN
        }

        return -1;
    }

    template <class ACCESSOR>
    int accessAttributes(ACCESSOR& accessor) const
        // Invoke the specified 'accessor' sequentially on each attribute of
        // this object, supplying 'accessor' with the corresponding attribute
        // information structure until such invocation returns a non-zero
        // value.  Return the value from the last invocation of 'accessor'.
    {
        if (k_HAS_ATTRIBUTE_0) {
            int rc = accessor(d_attribute0Value.object(),
                              Attribute0::attributeInfo());
            if (0 != rc) {
                return rc;                                            // RETURN
            }
        }

        if (k_HAS_ATTRIBUTE_1) {
            int rc = accessor(d_attribute1Value.object(),
                              Attribute1::attributeInfo());
            if (0 != rc) {
                return rc;                                            // RETURN
            }
        }

        if (k_HAS_ATTRIBUTE_2) {
            int rc = accessor(d_attribute2Value.object(),
                              Attribute2::attributeInfo());
            if (0 != rc) {
                return rc;                                            // RETURN
            }
        }

        return 0;
    }

    const char *className() const
        // Return a null-terminated string containing the exported name for
        // this class.
    {
        return "MySequence";
    }

    bool hasAttribute(const char *attributeName, int attributeNameLength) const
        // Return 'true' if this object has an attribute with the specified
        // 'attributeName' of the specified 'attributeNameLength', and 'false'
        // otherwise.
    {
        const bslstl::StringRef attributeNameRef(attributeName       ,
                                                 attributeNameLength);

        if (k_HAS_ATTRIBUTE_0 && Attribute0::name() == attributeNameRef) {
            return true;                                              // RETURN
        }

        if (k_HAS_ATTRIBUTE_1 && Attribute1::name() == attributeNameRef) {
            return true;                                              // RETURN
        }

        if (k_HAS_ATTRIBUTE_2 && Attribute2::name() == attributeNameRef) {
            return true;                                              // RETURN
        }

        return false;
    }

    bool hasAttribute(int attributeId) const
        // Return 'true' if this object has an attribute with the specified
        // 'attributeId', and 'false' otherwise.
    {
        if (k_HAS_ATTRIBUTE_0 && Attribute0::id() == attributeId) {
            return true;                                              // RETURN
        }

        if (k_HAS_ATTRIBUTE_1 && Attribute1::id() == attributeId) {
            return true;                                              // RETURN
        }

        if (k_HAS_ATTRIBUTE_2 && Attribute2::id() == attributeId) {
            return true;                                              // RETURN
        }

        return false;
    }

    const Attribute0Type attribute0() const
    {
        BSLS_ASSERT(k_HAS_ATTRIBUTE_0);
        return d_attribute0Value.object();
    }

    const Attribute1Type attribute1() const
    {
        BSLS_ASSERT(k_HAS_ATTRIBUTE_1);
        return d_attribute1Value.object();
    }

    const Attribute2Type attribute2() const
    {
        BSLS_ASSERT(k_HAS_ATTRIBUTE_2);
        return d_attribute2Value.object();
    }
};

// FREE OPERATORS
template <class V0, class V1, class V2>
bsl::ostream& operator<<(bsl::ostream&                   stream,
                         const TestSequence<V0, V1, V2>& object)
{
    typedef TestSequence<V0, V1, V2> Sequence;

    stream << "[" << object.className() << " ";

    if (Sequence::k_HAS_ATTRIBUTE_0) {
        stream << Sequence::Attribute0::name() << " = " << object.attribute0();
    }

    if (Sequence::k_HAS_ATTRIBUTE_1) {
        stream << ", " << Sequence::Attribute1::name() << " = "
               << object.attribute1();
    }

    if (Sequence::k_HAS_ATTRIBUTE_2) {
        stream << ", " << Sequence::Attribute2::name() << " = "
               << object.attribute2();
    }

    return stream << "]";
}

template <class V0, class V1, class V2>
bool operator==(const TestSequence<V0, V1, V2>& lhs,
                const TestSequence<V0, V1, V2>& rhs)
{
    return TestSequence<V0, V1, V2>::areEqual(lhs, rhs);
}

template <class V0, class V1, class V2>
bool operator!=(const TestSequence<V0, V1, V2>& lhs,
                const TestSequence<V0, V1, V2>& rhs)
{
    return !TestSequence<V0, V1, V2>::areEqual(lhs, rhs);
}

// TRAITS
template <class V0, class V1, class V2>
const char *bdlat_TypeName_className(const TestSequence<V0, V1, V2>& object)
    // Return a null-terminated string containing the exported name for the
    // type of the specified 'object'.
{
    return object.className();
}

template <class V0, class V1, class V2, class MANIPULATOR>
int bdlat_sequenceManipulateAttribute(
                                 TestSequence<V0, V1, V2> *object,
                                 MANIPULATOR&              manipulator,
                                 const char               *attributeName,
                                 int                       attributeNameLength)
    // Invoke the specified 'manipulator' on the address of the (modifiable)
    // attribute indicated by the specified 'attributeName' and
    // 'attributeNameLength' of the specified 'object', supplying 'manipulator'
    // with the corresponding attribute information structure.  Return a
    // non-zero value if the attribute is not found, and the value returned
    // from the invocation of 'manipulator' otherwise.
{
    return object->manipulateAttribute(
        manipulator, attributeName, attributeNameLength);
}

template <class V0, class V1, class V2, class MANIPULATOR>
int bdlat_sequenceManipulateAttribute(TestSequence<V0, V1, V2> *object,
                                      MANIPULATOR&              manipulator,
                                      int                       attributeId)
    // Invoke the specified 'manipulator' on the address of the (modifiable)
    // attribute indicated by the specified 'attributeId' of the specified
    // 'object', supplying 'manipulator' with the corresponding attribute
    // information structure.  Return a non-zero value if the attribute is not
    // found, and the value returned from the invocation of 'manipulator'
    // otherwise.
{
    return object->manipulateAttribute(manipulator, attributeId);
}

template <class V0, class V1, class V2, class MANIPULATOR>
int bdlat_sequenceManipulateAttributes(TestSequence<V0, V1, V2> *object,
                                       MANIPULATOR&              manipulator)
    // Invoke the specified 'manipulator' sequentially on the address of each
    // (modifiable) attribute of the specified 'object', supplying
    // 'manipulator' with the corresponding attribute information structure
    // until such invocation returns a non-zero value.  Return the value from
    // the last invocation of 'manipulator'.
{
    return object->manipulateAttributes(manipulator);
}

template <class V0, class V1, class V2, class ACCESSOR>
int bdlat_sequenceAccessAttribute(
                          const TestSequence<V0, V1, V2>&  object,
                          ACCESSOR&                        accessor,
                          const char                      *attributeName,
                          int                              attributeNameLength)
    // Invoke the specified 'accessor' on the (non-modifiable) attribute of the
    // specified 'object' indicated by the specified 'attributeName' and
    // 'attributeNameLength', supplying 'accessor' with the corresponding
    // attribute information structure.  Return a non-zero value if the
    // attribute is not found, and the value returned from the invocation of
    // 'accessor' otherwise.
{
    return object.accessAttribute(
        accessor, attributeName, attributeNameLength);
}

template <class V0, class V1, class V2, class ACCESSOR>
int bdlat_sequenceAccessAttribute(const TestSequence<V0, V1, V2>& object,
                                  ACCESSOR&                       accessor,
                                  int                             attributeId)
    // Invoke the specified 'accessor' on the attribute of the specified
    // 'object' with the given 'attributeId', supplying 'accessor' with the
    // corresponding attribute information structure.  Return non-zero if the
    // attribute is not found, and the value returned from the invocation of
    // 'accessor' otherwise.
{
    return object.accessAttribute(accessor, attributeId);
}

template <class V0, class V1, class V2, class ACCESSOR>
int bdlat_sequenceAccessAttributes(const TestSequence<V0, V1, V2>& object,
                                   ACCESSOR&                       accessor)
    // Invoke the specified 'accessor' sequentially on each attribute of the
    // specified 'object', supplying 'accessor' with the corresponding
    // attribute information structure until such invocation returns a non-zero
    // value.  Return the value from the last invocation of 'accessor'.
{
    return object.accessAttributes(accessor);
}

template <class V0, class V1, class V2>
bool bdlat_sequenceHasAttribute(
                          const TestSequence<V0, V1, V2>&  object,
                          const char                      *attributeName,
                          int                              attributeNameLength)
    // Return 'true' if the specified 'object' has an attribute with the
    // specified 'attributeName' of the specified 'attributeNameLength', and
    // 'false' otherwise.
{
    return object.hasAttribute(attributeName, attributeNameLength);
}

template <class V0, class V1, class V2>
bool bdlat_sequenceHasAttribute(const TestSequence<V0, V1, V2>& object,
                                int                             attributeId)
    // Return 'true' if the specified 'object' has an attribute with the
    // specified 'attributeId', and 'false' otherwise.
{
    return object.hasAttribute(attributeId);
}

template <class V0, class V1, class V2>
int bdlat_valueTypeAssign(TestSequence<V0, V1, V2>        *lhs,
                          const TestSequence<V0, V1, V2>&  rhs)
{
    *lhs = rhs;
    return 0;
}

template <class V0, class V1, class V2>
void bdlat_valueTypeReset(TestSequence<V0, V1, V2> *object)
{
    object->reset();
}

template <class V0, class V1, class V2>
struct bdlat_IsBasicSequence<TestSequence<V0, V1, V2> > : bsl::true_type {
};

namespace bdlat_SequenceFunctions {

template <class V0, class V1, class V2>
struct IsSequence<TestSequence<V0, V1, V2> > {
    enum { VALUE = 1 };
};

}  // close bdlat_SequenceFunctions namespace

                      // ===============================
                      // class TestTaggedValue_ArrayBase
                      // ===============================

// FORWARD DECLARATIONS
template <class TAG_TYPE, class VALUE_TYPE>
class TestTaggedValue;

template <class TAG_TYPE,
          class VALUE_TYPE,
          bool = static_cast<bdlat_TypeCategory::Value>(
                     bdlat_TypeCategory::Select<VALUE_TYPE>::e_SELECTION) ==
                         bdlat_TypeCategory::e_ARRAY_CATEGORY ||
                 static_cast<bdlat_TypeCategory::Value>(
                     bdlat_TypeCategory::Select<VALUE_TYPE>::e_SELECTION) ==
                         bdlat_TypeCategory::e_DYNAMIC_CATEGORY>
class TestTaggedValue_ArrayBase {
    // This class provides a base class and member functions implementing the
    // 'bdlat' 'Array' concept for 'TestTaggedValue' specializations having a
    // 'VALUE_TYPE' that itself implements the 'bdlat' 'Array' concept.

  public:
    // TYPES
    typedef typename bdlat_ArrayFunctions::ElementType<VALUE_TYPE>::Type
        ElementType;

    // CLASS DATA
    enum {
        k_IS_ARRAY = bdlat_ArrayFunctions::IsArray<VALUE_TYPE>::VALUE
    };

  private:
    // PRIVATE MANIPULATORS
    VALUE_TYPE& object()
    {
        typedef TestTaggedValue<TAG_TYPE, VALUE_TYPE> Derived;
        return static_cast<Derived *>(this)->d_value.object();
    }

    // PRIVATE ACCESSORS
    const VALUE_TYPE& object() const
    {
        typedef TestTaggedValue<TAG_TYPE, VALUE_TYPE> Derived;
        return static_cast<const Derived *>(this)->d_value.object();
    }

  public:
    // MANIPULATORS
    template <class MANIPULATOR>
    int manipulateElement(MANIPULATOR& manipulator, int index)
    {
        return bdlat_ArrayFunctions::manipulateElement(
            &this->object(), manipulator, index);
    }

    void resize(int newSize)
    {
        return bdlat_ArrayFunctions::resize(&this->object(), newSize);
    }

    // ACCESSORS
    template <class ACCESSOR>
    int accessElement(ACCESSOR& accessor, int index) const
    {
        return bdlat_ArrayFunctions::accessElement(
            this->object(), accessor, index);
    }

    bsl::size_t size() const
    {
        return bdlat_ArrayFunctions::size(this->object());
    }
};

template <class TAG_TYPE, class VALUE_TYPE>
class TestTaggedValue_ArrayBase<TAG_TYPE, VALUE_TYPE, false> {
    // This class provides a base class for 'TestTaggedValue' specializations
    // having a 'VALUE_TYPE' that does not implement the 'bdlat' 'Array'
    // concept.

  public:
    // CLASS DATA
    enum { k_IS_ARRAY = false };
};

                      // ================================
                      // class TestTaggedValue_ChoiceBase
                      // ================================

template <class TAG_TYPE,
          class VALUE_TYPE,
          bool = static_cast<bdlat_TypeCategory::Value>(
                     bdlat_TypeCategory::Select<VALUE_TYPE>::e_SELECTION) ==
                        bdlat_TypeCategory::e_CHOICE_CATEGORY ||
                 static_cast<bdlat_TypeCategory::Value>(
                     bdlat_TypeCategory::Select<VALUE_TYPE>::e_SELECTION) ==
                         bdlat_TypeCategory::e_DYNAMIC_CATEGORY>
class TestTaggedValue_ChoiceBase {
    // This class provides a base class and member functions implementing the
    // 'bdlat' 'Choice' concept for 'TestTaggedValue' specializations having a
    // 'VALUE_TYPE' that itself implements the 'bdlat' 'Choice' concept.

  public:
    // CLASS DATA
    enum { k_IS_CHOICE = bdlat_ChoiceFunctions::IsChoice<VALUE_TYPE>::VALUE };

  private:
    // PRIVATE MANIPULATORS
    VALUE_TYPE& object()
    {
        typedef TestTaggedValue<TAG_TYPE, VALUE_TYPE> Derived;
        return static_cast<Derived *>(this)->d_value.object();
    }

    // PRIVATE ACCESSORS
    const VALUE_TYPE& object() const
    {
        typedef TestTaggedValue<TAG_TYPE, VALUE_TYPE> Derived;
        return static_cast<const Derived *>(this)->d_value.object();
    }

  public:
    // MANIPULATORS
    int makeSelection(int selectionId)
    {
        return bdlat_ChoiceFunctions::makeSelection(&this->object(),
                                                    selectionId);
    }

    int makeSelection(const char *selectionName, int selectionNameLength)
    {
        return bdlat_ChoiceFunctions::makeSelection(
            &this->object(), selectionName, selectionNameLength);
    }

    template <class MANIPULATOR>
    int manipulateSelection(MANIPULATOR& manipulator)
    {
        return bdlat_ChoiceFunctions::manipulateSelection(&this->object(),
                                                          manipulator);
    }

    // ACCESSORS
    template <class ACCESSOR>
    int accessSelection(ACCESSOR& accessor) const
    {
        return bdlat_ChoiceFunctions::accessSelection(this->object(),
                                                      accessor);
    }

    bool hasSelection(int selectionId) const
    {
        return bdlat_ChoiceFunctions::hasSelection(this->object(),
                                                   selectionId);
    }

    bool hasSelection(const char *selectionName, int selectionNameLength) const
    {
        return bdlat_ChoiceFunctions::hasSelection(
            this->object(), selectionName, selectionNameLength);
    }

    int selectionId() const
    {
        return bdlat_ChoiceFunctions::selectionId(this->object());
    }
};

template <class TAG_TYPE, class VALUE_TYPE>
class TestTaggedValue_ChoiceBase<TAG_TYPE, VALUE_TYPE, false> {
    // This class provides a base class for 'TestTaggedValue' specializations
    // having a 'VALUE_TYPE' that does not implement the 'bdlat' 'Choice'
    // concept.

  public:
    // CLASS DATA
    enum { k_IS_CHOICE = false };
};

                  // ========================================
                  // class TestTaggedValue_CustomizedTypeBase
                  // ========================================

template <class TAG_TYPE,
          class VALUE_TYPE,
          bool = static_cast<bdlat_TypeCategory::Value>(
                     bdlat_TypeCategory::Select<VALUE_TYPE>::e_SELECTION) ==
                         bdlat_TypeCategory::e_CUSTOMIZED_TYPE_CATEGORY ||
                 static_cast<bdlat_TypeCategory::Value>(
                     bdlat_TypeCategory::Select<VALUE_TYPE>::e_SELECTION) ==
                         bdlat_TypeCategory::e_DYNAMIC_CATEGORY>
class TestTaggedValue_CustomizedTypeBase {
    // This class provides a base class and member functions implementing the
    // 'bdlat' 'CustomizedType' concept for 'TestTaggedValue' specializations
    // having a 'VALUE_TYPE' that itself implements the 'bdlat'
    // 'CustomizedType' concept.

  public:
    // TYPES
    typedef typename bdlat_CustomizedTypeFunctions::BaseType<VALUE_TYPE>::Type
        BaseType;

    // CLASS DATA
    enum {
        k_IS_CUSTOMIZED_TYPE =
            bdlat_CustomizedTypeFunctions::IsCustomizedType<VALUE_TYPE>::VALUE
    };

  private:
    // PRIVATE MANIPULATORS
    VALUE_TYPE& object()
    {
        typedef TestTaggedValue<TAG_TYPE, VALUE_TYPE> Derived;
        return static_cast<Derived *>(this)->d_value.object();
    }

    // PRIVATE ACCESSORS
    const VALUE_TYPE& object() const
    {
        typedef TestTaggedValue<TAG_TYPE, VALUE_TYPE> Derived;
        return static_cast<const Derived *>(this)->d_value.object();
    }

  public:
    // MANIPULATORS
    template <class BASE_TYPE>
    int convertFromBaseType(const BASE_TYPE& object)
    {
        return bdlat_CustomizedTypeFunctions::convertFromBaseType(
            &this->object(), object);
    }

    // ACCESSORS
    const BaseType& convertToBaseType() const
    {
        return bdlat_CustomizedTypeFunctions::convertToBaseType(
            this->object());
    }
};

template <class TAG_TYPE, class VALUE_TYPE>
class TestTaggedValue_CustomizedTypeBase<TAG_TYPE, VALUE_TYPE, false> {
    // This class provides a base class for 'TestTaggedValue' specializations
    // having a 'VALUE_TYPE' that does not implement the 'bdlat'
    // 'CustomizedType' concept.

  public:
    // CLASS DATA
    enum { k_IS_CUSTOMIZED_TYPE = false };
};

                   // =====================================
                   // class TestTaggedValue_DynamicTypeBase
                   // =====================================

template <class TAG_TYPE,
          class VALUE_TYPE,
          bool = static_cast<bdlat_TypeCategory::Value>(
                     bdlat_TypeCategory::Select<VALUE_TYPE>::e_SELECTION) ==
                         bdlat_TypeCategory::e_DYNAMIC_CATEGORY>
class TestTaggedValue_DynamicTypeBase {
    // This class provides a base class and member functions implementing the
    // 'bdlat' 'DynamicType' concept for 'TestTaggedValue' specializations
    // having a 'VALUE_TYPE' that itself implements the 'bdlat' 'DynamicType'
    // concept.

  public:
    // CLASS DATA
    enum { k_IS_DYNAMIC_TYPE = true };

  private:
    // PRIVATE ACCESSORS
    const VALUE_TYPE& object() const
    {
        typedef TestTaggedValue<TAG_TYPE, VALUE_TYPE> Derived;
        return static_cast<const Derived *>(this)->d_value.object();
    }

  public:
    // ACCESSORS
    bdlat_TypeCategory::Value select() const
    {
        return bdlat_TypeCategoryFunctions::select(this->object());
    }
};

template <class TAG_TYPE, class VALUE_TYPE>
class TestTaggedValue_DynamicTypeBase<TAG_TYPE, VALUE_TYPE, false> {
    // This class provides a base class for 'TestTaggedValue' specializations
    // having a 'VALUE_TYPE' that does not implement the 'bdlat' 'DynamicType'
    // concept.

  public:
    // CLASS DATA
    enum { k_IS_DYNAMIC_TYPE = false };
};

                   // =====================================
                   // class TestTaggedValue_EnumerationBase
                   // =====================================

template <class TAG_TYPE,
          class VALUE_TYPE,
          bool = static_cast<bdlat_TypeCategory::Value>(
                     bdlat_TypeCategory::Select<VALUE_TYPE>::e_SELECTION) ==
                         bdlat_TypeCategory::e_ENUMERATION_CATEGORY ||
                 static_cast<bdlat_TypeCategory::Value>(
                     bdlat_TypeCategory::Select<VALUE_TYPE>::e_SELECTION) ==
                         bdlat_TypeCategory::e_DYNAMIC_CATEGORY>
class TestTaggedValue_EnumerationBase {
    // This class provides a base class and member functions implementing the
    // 'bdlat' 'Enumeration' concept for 'TestTaggedValue' specializations
    // having a 'VALUE_TYPE' that itself implements the 'bdlat' 'Enumeration'
    // concept.

  public:
    // CLASS DATA
    enum {
        k_IS_ENUMERATION =
            bdlat_EnumFunctions::IsEnumeration<VALUE_TYPE>::VALUE
    };

  private:
    // PRIVATE MANIPULATORS
    VALUE_TYPE& object()
    {
        typedef TestTaggedValue<TAG_TYPE, VALUE_TYPE> Derived;
        return static_cast<Derived *>(this)->d_value.object();
    }

    // PRIVATE ACCESSORS
    const VALUE_TYPE& object() const
    {
        typedef TestTaggedValue<TAG_TYPE, VALUE_TYPE> Derived;
        return static_cast<const Derived *>(this)->d_value.object();
    }

  public:
    // MANIPULATORS
    int fromInt(int value)
    {
        return bdlat_EnumFunctions::fromInt(&this->object(), value);
    }

    int fromString(const char *string, int stringLength)
    {
        return bdlat_EnumFunctions::fromString(
            &this->object(), string, stringLength);
    }

    // ACCESSORS
    void toInt(int *result) const
    {
        bdlat_EnumFunctions::toInt(result, this->object());
    }

    void toString(bsl::string *result) const
    {
        bdlat_EnumFunctions::toString(result, this->object());
    }
};

template <class TAG_TYPE, class VALUE_TYPE>
class TestTaggedValue_EnumerationBase<TAG_TYPE, VALUE_TYPE, false> {
    // This class provides a base class for 'TestTaggedValue' specializations
    // having a 'VALUE_TYPE' that does not implement the 'bdlat' 'Enumeration'
    // concept.

  public:
    // CLASS DATA
    enum { k_IS_ENUMERATION = false };
};

                  // =======================================
                  // class TestTaggedValue_NullableValueBase
                  // =======================================

template <class TAG_TYPE,
          class VALUE_TYPE,
          bool = static_cast<bdlat_TypeCategory::Value>(
                     bdlat_TypeCategory::Select<VALUE_TYPE>::e_SELECTION) ==
                         bdlat_TypeCategory::e_NULLABLE_VALUE_CATEGORY ||
                 static_cast<bdlat_TypeCategory::Value>(
                     bdlat_TypeCategory::Select<VALUE_TYPE>::e_SELECTION) ==
                         bdlat_TypeCategory::e_DYNAMIC_CATEGORY>
class TestTaggedValue_NullableValueBase {
    // This class provides a base class and member functions implementing the
    // 'bdlat' 'NullableValue' concept for 'TestTaggedValue' specializations
    // having a 'VALUE_TYPE' that itself implements the 'bdlat' 'NullableValue'
    // concept.

  public:
    // TYPES
    typedef typename bdlat_NullableValueFunctions::template ValueType<
        VALUE_TYPE>::Type ValueType;

    // CLASS DATA
    enum {
        k_IS_NULLABLE_VALUE =
            bdlat_NullableValueFunctions::IsNullableValue<VALUE_TYPE>::VALUE
    };

  private:
    // PRIVATE MANIPULATORS
    VALUE_TYPE& object()
    {
        typedef TestTaggedValue<TAG_TYPE, VALUE_TYPE> Derived;
        return static_cast<Derived *>(this)->d_value.object();
    }

    // PRIVATE ACCESSORS
    const VALUE_TYPE& object() const
    {
        typedef TestTaggedValue<TAG_TYPE, VALUE_TYPE> Derived;
        return static_cast<const Derived *>(this)->d_value.object();
    }

  public:
    // MANIPULATORS
    void makeValue()
    {
        bdlat_NullableValueFunctions::makeValue(&this->object());
    }

    template <class MANIPULATOR>
    int manipulateValue(MANIPULATOR& manipulator)
    {
        return bdlat_NullableValueFunctions::manipulateValue(&this->object(),
                                                             manipulator);
    }

    // ACCESSORS
    template <class ACCESSOR>
    int accessValue(ACCESSOR& accessor) const
    {
        return bdlat_NullableValueFunctions::accessValue(this->object(),
                                                         accessor);
    }

    bool isNull() const
    {
        return bdlat_NullableValueFunctions::isNull(this->object());
    }
};

template <class TAG_TYPE, class VALUE_TYPE>
class TestTaggedValue_NullableValueBase<TAG_TYPE, VALUE_TYPE, false> {
    // This class provides a base class for 'TestTaggedValue' specializations
    // having a 'VALUE_TYPE' that does not implement the 'bdlat'
    // 'NullableValue' concept.

  public:
    // CLASS DATA
    enum { k_IS_NULLABLE_VALUE = false };
};

                     // ==================================
                     // class TestTaggedValue_SequenceBase
                     // ==================================

template <class TAG_TYPE,
          class VALUE_TYPE,
          bool = static_cast<bdlat_TypeCategory::Value>(
                     bdlat_TypeCategory::Select<VALUE_TYPE>::e_SELECTION) ==
                         bdlat_TypeCategory::e_SEQUENCE_CATEGORY ||
                 static_cast<bdlat_TypeCategory::Value>(
                     bdlat_TypeCategory::Select<VALUE_TYPE>::e_SELECTION) ==
                         bdlat_TypeCategory::e_DYNAMIC_CATEGORY>
class TestTaggedValue_SequenceBase {
    // This class provides a base class and member functions implementing the
    // 'bdlat' 'Sequence' concept for 'TestTaggedValue' specializations having
    // a 'VALUE_TYPE' that itself implements the 'bdlat' 'Sequence' concept.

  public:
    // CLASS DATA
    enum {
        k_IS_SEQUENCE = bdlat_SequenceFunctions::IsSequence<VALUE_TYPE>::VALUE
    };

  private:
    // PRIVATE MANIPULATORS
    VALUE_TYPE& object()
    {
        typedef TestTaggedValue<TAG_TYPE, VALUE_TYPE> Derived;
        return static_cast<Derived *>(this)->d_value.object();
    }

    // PRIVATE ACCESSORS
    const VALUE_TYPE& object() const
    {
        typedef TestTaggedValue<TAG_TYPE, VALUE_TYPE> Derived;
        return static_cast<const Derived *>(this)->d_value.object();
    }

  public:
    // MANIPULATORS
    template <class MANIPULATOR>
    int manipulateAttribute(MANIPULATOR& manipulator, int attributeId)
    {
        return bdlat_SequenceFunctions::manipulateAttribute(
            &this->object(), manipulator, attributeId);
    }

    template <class MANIPULATOR>
    int manipulateAttribute(MANIPULATOR&  manipulator,
                            const char   *attributeName,
                            int           attributeNameLength)
    {
        return bdlat_SequenceFunctions::manipulateAttribute(
            &this->object(), manipulator, attributeName, attributeNameLength);
    }

    template <class MANIPULATOR>
    int manipulateAttributes(MANIPULATOR& manipulator)
    {
        return bdlat_SequenceFunctions::manipulateAttributes(&this->object(),
                                                             manipulator);
    }

    // ACCESSORS
    template <class ACCESSOR>
    int accessAttribute(ACCESSOR& accessor, int attributeId) const
    {
        return bdlat_SequenceFunctions::accessAttribute(
            this->object(), accessor, attributeId);
    }

    template <class ACCESSOR>
    int accessAttribute(ACCESSOR&   accessor,
                        const char *attributeName,
                        int         attributeNameLength) const
    {
        return bdlat_SequenceFunctions::accessAttribute(
            this->object(), accessor, attributeName, attributeNameLength);
    }

    template <class ACCESSOR>
    int accessAttributes(ACCESSOR& accessor) const
    {
        return bdlat_SequenceFunctions::accessAttributes(this->object(),
                                                         accessor);
    }

    bool hasAttribute(int attributeId) const
    {
        return bdlat_SequenceFunctions::hasAttribute(this->object(),
                                                     attributeId);
    }

    bool hasAttribute(const char *attributeName, int attributeNameLength) const
    {
        return bdlat_SequenceFunctions::hasAttribute(
            this->object(), attributeName, attributeNameLength);
    }
};

template <class TAG_TYPE, class VALUE_TYPE>
class TestTaggedValue_SequenceBase<TAG_TYPE, VALUE_TYPE, false> {
    // This class provides a base class for 'TestTaggedValue' specializations
    // having a 'VALUE_TYPE' that does not implement the 'bdlat' 'Sequence'
    // concept.

  public:
    // CLASS DATA
    enum { k_IS_SEQUENCE = false };
};

                           // =====================
                           // class TestTaggedValue
                           // =====================

template <class TAG_TYPE, class VALUE_TYPE>
class TestTaggedValue
: public TestTaggedValue_ArrayBase<TAG_TYPE, VALUE_TYPE>,
  public TestTaggedValue_ChoiceBase<TAG_TYPE, VALUE_TYPE>,
  public TestTaggedValue_CustomizedTypeBase<TAG_TYPE, VALUE_TYPE>,
  public TestTaggedValue_DynamicTypeBase<TAG_TYPE, VALUE_TYPE>,
  public TestTaggedValue_EnumerationBase<TAG_TYPE, VALUE_TYPE>,
  public TestTaggedValue_NullableValueBase<TAG_TYPE, VALUE_TYPE>,
  public TestTaggedValue_SequenceBase<TAG_TYPE, VALUE_TYPE> {
    // This class provides a wrapper around an object of the specified
    // 'VALUE_TYPE' that implements all 'bdlat' value category concepts the
    // 'VALUE_TYPE' implements by delegating the implementation to the wrapped
    // object.  However, more than just forwarding these operations, this class
    // permits one to overload particular 'bdlat' operations of the underlying
    // 'VALUE_TYPE' object by providing an overload for the corresponding
    // 'bdlat' free function with a particular 'TAG_TYPE'.  Such overloads will
    // be better matches during overload resolution.

  public:
    // TYPES
    typedef TAG_TYPE   Tag;
    typedef VALUE_TYPE Value;

  private:
    // DATA
    bslalg::ConstructorProxy<Value>  d_value;
    bslma::Allocator                *d_allocator_p;

    // FRIENDS
    friend class TestTaggedValue_ArrayBase<TAG_TYPE, VALUE_TYPE>;
    friend class TestTaggedValue_ChoiceBase<TAG_TYPE, VALUE_TYPE>;
    friend class TestTaggedValue_CustomizedTypeBase<TAG_TYPE, VALUE_TYPE>;
    friend class TestTaggedValue_DynamicTypeBase<TAG_TYPE, VALUE_TYPE>;
    friend class TestTaggedValue_EnumerationBase<TAG_TYPE, VALUE_TYPE>;
    friend class TestTaggedValue_NullableValueBase<TAG_TYPE, VALUE_TYPE>;
    friend class TestTaggedValue_SequenceBase<TAG_TYPE, VALUE_TYPE>;

  public:
    // CLASS METHODS
    bool areEqual(const TestTaggedValue& lhs, const TestTaggedValue& rhs)
    {
        return lhs.d_value.object() == rhs.d_value.object();
    }

    // CREATORS
    TestTaggedValue()
    : d_value(bslma::Default::allocator())
    , d_allocator_p(bslma::Default::allocator())
    {
    }

    explicit TestTaggedValue(bslma::Allocator *basicAllocator)
    : d_value(bslma::Default::allocator(basicAllocator))
    , d_allocator_p(bslma::Default::allocator(basicAllocator))
    {
    }

    explicit TestTaggedValue(const Value&      value,
                             bslma::Allocator *basicAllocator = 0)
    : d_value(value, bslma::Default::allocator(basicAllocator))
    , d_allocator_p(bslma::Default::allocator(basicAllocator))
    {
    }

    TestTaggedValue(const TestTaggedValue&  original,
                    bslma::Allocator       *basicAllocator = 0)
    : d_value(original.d_value.object(),
              bslma::Default::allocator(basicAllocator))
    , d_allocator_p(bslma::Default::allocator(basicAllocator))
    {
    }

    // MANIPULATORS
    TestTaggedValue& operator=(const TestTaggedValue& original)
    {
        d_value.object() = original.d_value.object();
        return *this;
    }

    int assign(const VALUE_TYPE& rhs)
    {
        return bdlat_ValueTypeFunctions::assign(&d_value.object(), rhs);
    }

    int assign(const TestTaggedValue& rhs)
    {
        return bdlat_ValueTypeFunctions::assign(&d_value.object(),
                                                rhs.d_value.object());
    }

    void reset() { bdlat_ValueTypeFunctions::reset(&d_value.object()); }

    // ACCESSORS
    const char *className() const { return "MyTaggedValue"; }
};

// FREE FUNCTIONS
template <class TAG_TYPE, class VALUE_TYPE>
bool operator==(const TestTaggedValue<TAG_TYPE, VALUE_TYPE>& lhs,
                const TestTaggedValue<TAG_TYPE, VALUE_TYPE>& rhs)
{
    return TestTaggedValue<TAG_TYPE, VALUE_TYPE>::areEqual(lhs, rhs);
}

template <class TAG_TYPE, class VALUE_TYPE>
bool operator!=(const TestTaggedValue<TAG_TYPE, VALUE_TYPE>& lhs,
                const TestTaggedValue<TAG_TYPE, VALUE_TYPE>& rhs)
{
    return TestTaggedValue<TAG_TYPE, VALUE_TYPE>::areEqual(lhs, rhs);
}

// TRAITS
template <class TAG_TYPE, class VALUE_TYPE, class MANIPULATOR>
int bdlat_arrayManipulateElement(
                            TestTaggedValue<TAG_TYPE, VALUE_TYPE> *object,
                            MANIPULATOR&                           manipulator,
                            int                                    index)
{
    return object->manipulateElement(manipulator, index);
}

template <class TAG_TYPE, class VALUE_TYPE>
void bdlat_arrayResize(TestTaggedValue<TAG_TYPE, VALUE_TYPE> *object,
                       int                                    newSize)
{
    object->resize(newSize);
}

template <class TAG_TYPE, class VALUE_TYPE, class ACCESSOR>
int bdlat_arrayAccessElement(
                         const TestTaggedValue<TAG_TYPE, VALUE_TYPE>& object,
                         ACCESSOR&                                    accessor,
                         int                                          index)
{
    return object->accessElement(accessor, index);
}

template <class TAG_TYPE, class VALUE_TYPE>
bsl::size_t bdlat_arraySize(
                           const TestTaggedValue<TAG_TYPE, VALUE_TYPE>& object)
{
    return object.size();
}

namespace bdlat_ArrayFunctions {

template <class TAG_TYPE, class VALUE_TYPE>
struct IsArray<TestTaggedValue<TAG_TYPE, VALUE_TYPE> > {
    enum { VALUE = TestTaggedValue<TAG_TYPE, VALUE_TYPE>::k_IS_ARRAY };
};

template <class TAG_TYPE, class VALUE_TYPE>
struct ElementType<TestTaggedValue<TAG_TYPE, VALUE_TYPE> > {
    typedef typename TestTaggedValue<TAG_TYPE, VALUE_TYPE>::ElementType Type;
};

}  // close bdlat_ArrayFunctions namespace

template <class TAG_TYPE, class VALUE_TYPE>
int bdlat_choiceMakeSelection(
                            TestTaggedValue<TAG_TYPE, VALUE_TYPE> *object,
                            int                                    selectionId)
{
    return object->makeSelection(selectionId);
}

template <class TAG_TYPE, class VALUE_TYPE>
int bdlat_choiceMakeSelection(
                    TestTaggedValue<TAG_TYPE, VALUE_TYPE> *object,
                    const char                            *selectionName,
                    int                                    selectionNameLength)
{
    return object->makeSelection(selectionName, selectionNameLength);
}

template <class TAG_TYPE, class VALUE_TYPE, class MANIPULATOR>
int bdlat_choiceManipulateSelection(
                            TestTaggedValue<TAG_TYPE, VALUE_TYPE> *object,
                            MANIPULATOR&                           manipulator)
{
    return object->manipulateSelection(manipulator);
}

template <class TAG_TYPE, class VALUE_TYPE, class ACCESSOR>
int bdlat_choiceAccessSelection(
                         const TestTaggedValue<TAG_TYPE, VALUE_TYPE>& object,
                         ACCESSOR&                                    accessor)
{
    return object.accessSelection(accessor);
}

template <class TAG_TYPE, class VALUE_TYPE>
bool bdlat_choiceHasSelection(
                      const TestTaggedValue<TAG_TYPE, VALUE_TYPE>& object,
                      int                                          selectionId)
{
    return object.hasSelection(selectionId);
}

template <class TAG_TYPE, class VALUE_TYPE>
bool bdlat_choiceHasSelection(
             const TestTaggedValue<TAG_TYPE, VALUE_TYPE>&  object,
             const char                                   *selectionName,
             int                                           selectionNameLength)
{
    return object.hasSelection(selectionName, selectionNameLength);
}

template <class TAG_TYPE, class VALUE_TYPE>
int bdlat_choiceSelectionId(
                           const TestTaggedValue<TAG_TYPE, VALUE_TYPE>& object)
{
    return object.selectionId();
}

template <class TAG_TYPE, class VALUE_TYPE>
struct bdlat_IsBasicChoice<TestTaggedValue<TAG_TYPE, VALUE_TYPE> >
: bsl::integral_constant<bool,
                         TestTaggedValue<TAG_TYPE, VALUE_TYPE>::k_IS_CHOICE> {
};

namespace bdlat_ChoiceFunctions {

template <class TAG_TYPE, class VALUE_TYPE>
struct IsChoice<TestTaggedValue<TAG_TYPE, VALUE_TYPE> > {
    enum { VALUE = TestTaggedValue<TAG_TYPE, VALUE_TYPE>::k_IS_CHOICE };
};

}  // close bdlat_ChoiceFunctions namespace

template <class TAG_TYPE, class VALUE_TYPE, class BASE_TYPE>
int bdlat_customizedTypeConvertFromBaseType(
                                 TestTaggedValue<TAG_TYPE, VALUE_TYPE> *object,
                                 const BASE_TYPE&                       value)
{
    return object->convertFromBaseType(value);
}

template <class TAG_TYPE, class VALUE_TYPE>
const typename TestTaggedValue<TAG_TYPE, VALUE_TYPE>::BaseType&
bdlat_customizedTypeConvertToBaseType(
                           const TestTaggedValue<TAG_TYPE, VALUE_TYPE>& object)
{
    return object.convertToBaseType();
}

template <class TAG_TYPE, class VALUE_TYPE>
struct bdlat_IsBasicCustomizedType<TestTaggedValue<TAG_TYPE, VALUE_TYPE> >
: bsl::integral_constant<
      bool,
      TestTaggedValue<TAG_TYPE, VALUE_TYPE>::k_IS_CUSTOMIZED_TYPE> {
};

namespace bdlat_CustomizedTypeFunctions {

template <class TAG_TYPE, class VALUE_TYPE>
struct IsCustomizedType<TestTaggedValue<TAG_TYPE, VALUE_TYPE> > {
    enum {
        VALUE = TestTaggedValue<TAG_TYPE, VALUE_TYPE>::k_IS_CUSTOMIZED_TYPE
    };
};

template <class TAG_TYPE, class VALUE_TYPE>
struct BaseType<TestTaggedValue<TAG_TYPE, VALUE_TYPE> > {
    typedef typename TestTaggedValue<TAG_TYPE, VALUE_TYPE>::BaseType Type;
};

}  // close bdlat_CustomizedTypeFunctions namespace

template <class TAG_TYPE, class VALUE_TYPE>
bdlat_TypeCategory::Value bdlat_typeCategorySelect(
                           const TestTaggedValue<TAG_TYPE, VALUE_TYPE>& object)
{
    return object.select();
}

template <class TAG_TYPE, class VALUE_TYPE>
struct bdlat_TypeCategoryDeclareDynamic<
    TestTaggedValue<TAG_TYPE, VALUE_TYPE> > {
    enum { VALUE = TestTaggedValue<TAG_TYPE, VALUE_TYPE>::k_IS_DYNAMIC_TYPE };
};

template <class TAG_TYPE, class VALUE_TYPE>
int bdlat_enumFromInt(TestTaggedValue<TAG_TYPE, VALUE_TYPE> *object, int value)
{
    return object->fromInt(value);
}

template <class TAG_TYPE, class VALUE_TYPE>
int bdlat_enumFromString(TestTaggedValue<TAG_TYPE, VALUE_TYPE> *object,
                         const char                            *string,
                         int                                    stringLength)
{
    return object->fromString(string, stringLength);
}

template <class TAG_TYPE, class VALUE_TYPE>
void bdlat_enumToInt(int                                          *result,
                     const TestTaggedValue<TAG_TYPE, VALUE_TYPE>&  object)
{
    return object.toInt(result);
}

template <class TAG_TYPE, class VALUE_TYPE>
void bdlat_enumToString(bsl::string                                  *result,
                        const TestTaggedValue<TAG_TYPE, VALUE_TYPE>&  object)
{
    return object.toString(result);
}

template <class TAG_TYPE, class VALUE_TYPE>
struct bdlat_IsBasicEnumeration<TestTaggedValue<TAG_TYPE, VALUE_TYPE> >
: bsl::integral_constant<
      bool,
      TestTaggedValue<TAG_TYPE, VALUE_TYPE>::k_IS_ENUMERATION> {
};

namespace bdlat_EnumFunctions {

template <class TAG_TYPE, class VALUE_TYPE>
struct IsEnumeration<TestTaggedValue<TAG_TYPE, VALUE_TYPE> > {
    enum { VALUE = TestTaggedValue<TAG_TYPE, VALUE_TYPE>::k_IS_ENUMERATION };
};

}  // close bdlat_EnumFunctions namespace

template <class TAG_TYPE, class VALUE_TYPE>
void bdlat_nullableValueMakeValue(
                                 TestTaggedValue<TAG_TYPE, VALUE_TYPE> *object)
{
    object->makeValue();
}

template <class TAG_TYPE, class VALUE_TYPE, class MANIPULATOR>
int bdlat_nullableValueManipulateValue(
                            TestTaggedValue<TAG_TYPE, VALUE_TYPE> *object,
                            MANIPULATOR&                           manipulator)
{
    return object->manipulateValue(manipulator);
}

template <class TAG_TYPE, class VALUE_TYPE, class ACCESSOR>
int bdlat_nullableValueAccessValue(
                         const TestTaggedValue<TAG_TYPE, VALUE_TYPE>& object,
                         ACCESSOR&                                    accessor)
{
    return object.accessValue(accessor);
}

template <class TAG_TYPE, class VALUE_TYPE>
bool bdlat_nullableValueIsNull(
                           const TestTaggedValue<TAG_TYPE, VALUE_TYPE>& object)
{
    return object.isNull();
}

namespace bdlat_NullableValueFunctions {

template <class TAG_TYPE, class VALUE_TYPE>
struct IsNullableValue<TestTaggedValue<TAG_TYPE, VALUE_TYPE> > {
    enum {
        VALUE = TestTaggedValue<TAG_TYPE, VALUE_TYPE>::k_IS_NULLABLE_VALUE
    };
};

template <class TAG_TYPE, class VALUE_TYPE>
struct ValueType<TestTaggedValue<TAG_TYPE, VALUE_TYPE> > {
    typedef typename TestTaggedValue<TAG_TYPE, VALUE_TYPE>::ValueType Type;
};

}  // close bdlat_NullableValueFunctions namespace

template <class TAG_TYPE, class VALUE_TYPE, class MANIPULATOR>
int bdlat_sequenceManipulateAttribute(
                            TestTaggedValue<TAG_TYPE, VALUE_TYPE> *object,
                            MANIPULATOR&                           manipulator,
                            int                                    attributeId)
{
    return object->manipulateAttribute(manipulator, attributeId);
}

template <class TAG_TYPE, class VALUE_TYPE, class MANIPULATOR>
int bdlat_sequenceManipulateAttribute(
                    TestTaggedValue<TAG_TYPE, VALUE_TYPE> *object,
                    MANIPULATOR&                           manipulator,
                    const char                            *attributeName,
                    int                                    attributeNameLength)
{
    return object->manipulateAttribute(
        manipulator, attributeName, attributeNameLength);
}

template <class TAG_TYPE, class VALUE_TYPE, class MANIPULATOR>
int bdlat_sequenceManipulateAttributes(
                            TestTaggedValue<TAG_TYPE, VALUE_TYPE> *object,
                            MANIPULATOR&                           manipulator)
{
    return object->manipulateAttributes(manipulator);
}

template <class TAG_TYPE, class VALUE_TYPE, class ACCESSOR>
int bdlat_sequenceAccessAttribute(
                      const TestTaggedValue<TAG_TYPE, VALUE_TYPE>& object,
                      ACCESSOR&                                    accessor,
                      int                                          attributeId)
{
    return object.accessAttribute(accessor, attributeId);
}

template <class TAG_TYPE, class VALUE_TYPE, class ACCESSOR>
int bdlat_sequenceAccessAttribute(
             const TestTaggedValue<TAG_TYPE, VALUE_TYPE>&  object,
             ACCESSOR&                                     accessor,
             const char                                   *attributeName,
             int                                           attributeNameLength)
{
    return object.accessAttribute(
        accessor, attributeName, attributeNameLength);
}

template <class TAG_TYPE, class VALUE_TYPE, class ACCESSOR>
int bdlat_sequenceAccessAttributes(
                         const TestTaggedValue<TAG_TYPE, VALUE_TYPE>& object,
                         ACCESSOR&                                    accessor)
{
    return object.accessAttributes(accessor);
}

template <class TAG_TYPE, class VALUE_TYPE>
bool bdlat_sequenceHasAttribute(
                      const TestTaggedValue<TAG_TYPE, VALUE_TYPE>& object,
                      int                                          attributeId)
{
    return object.hasAttribute(attributeId);
}

template <class TAG_TYPE, class VALUE_TYPE>
bool bdlat_sequenceHasAttribute(
             const TestTaggedValue<TAG_TYPE, VALUE_TYPE>&  object,
             const char                                   *attributeName,
             int                                           attributeNameLength)
{
    return object.hasAttribute(attributeName, attributeNameLength);
}

template <class TAG_TYPE, class VALUE_TYPE>
struct bdlat_IsBasicSequence<TestTaggedValue<TAG_TYPE, VALUE_TYPE> >
: public bsl::integral_constant<
      bool,
      TestTaggedValue<TAG_TYPE, VALUE_TYPE>::k_IS_SEQUENCE> {
};

namespace bdlat_SequenceFunctions {

template <class TAG_TYPE, class VALUE_TYPE>
struct IsSequence<TestTaggedValue<TAG_TYPE, VALUE_TYPE> > {
    enum { VALUE = TestTaggedValue<TAG_TYPE, VALUE_TYPE>::k_IS_SEQUENCE };
};

}  // close bdlat_SequenceFunctions namespace

template <class TAG_TYPE, class VALUE_TYPE>
const char *bdlat_TypeName_className(
                           const TestTaggedValue<TAG_TYPE, VALUE_TYPE>& object)
{
    return object.className();
}

template <class TAG_TYPE, class VALUE_TYPE>
int bdlat_valueTypeAssign(TestTaggedValue<TAG_TYPE, VALUE_TYPE> *object,
                          const VALUE_TYPE&                      value)
{
    return object->assign(value);
}

template <class TAG_TYPE, class VALUE_TYPE>
int bdlat_valueTypeAssign(TestTaggedValue<TAG_TYPE, VALUE_TYPE>        *object,
                          const TestTaggedValue<TAG_TYPE, VALUE_TYPE>&  value)
{
    return object->assign(value);
}

                             // =================
                             // class PlaceHolder
                             // =================

template <class TYPE>
class PlaceHolder {
    // This class provides subset of the semantics of an in-core value-semantic
    // type.  It is intended to be used to guide template (type) argument
    // deduction in function invocation expressions, where it may not make
    // sense for a value of the type to be supplied to guide the deduction, and
    // where specifying a template argument may require more characters than a
    // function argument.

  public:
    // CREATORS
    PlaceHolder() {}
};

                          // ========================
                          // struct AttributeTypeUtil
                          // ========================

struct AttributeTypeUtil {
    // This utility 'struct' provides a namespace for a suite of functions that
    // create objects of test implementation types, that in turn implement
    // 'bdlat' attribute type concepts.

    // CLASS METHODS
    template <class VALUE_TYPE>
    static bsl::vector<VALUE_TYPE> generateArray(
                                                const PlaceHolder<VALUE_TYPE>&)
    {
        return bsl::vector<VALUE_TYPE>();
    }

    template <class VALUE_TYPE>
    static bsl::vector<VALUE_TYPE> generateArray(const VALUE_TYPE& value)
    {
        bsl::vector<VALUE_TYPE> result;
        result.push_back(value);
        return result;
    }

    template <class VALUE_TYPE>
    static bsl::vector<VALUE_TYPE> generateArray(const VALUE_TYPE& value0,
                                                 const VALUE_TYPE& value1)
    {
        bsl::vector<VALUE_TYPE> result;
        result.push_back(value0);
        result.push_back(value1);
        return result;
    }

    template <class VALUE_TYPE>
    static PlaceHolder<bsl::vector<VALUE_TYPE> > generateArrayType(
                                                const PlaceHolder<VALUE_TYPE>&)
    {
        return PlaceHolder<bsl::vector<VALUE_TYPE> >();
    }

    template <class SELECTION_0, class TYPE_0>
    static TestChoice<TypedTestSelection<TYPE_0, SELECTION_0> > generateChoice(
                                                      const SELECTION_0& ,
                                                      const TYPE_0&      value)
    {
        typedef TypedTestSelection<TYPE_0, SELECTION_0> Selection0;

        return TestChoice<Selection0>(value);
    }

    template <class SELECTION_0, class SELECTION_1, class TYPE_0, class TYPE_1>
    static TestChoice<TypedTestSelection<TYPE_0, SELECTION_0>,
                      TypedTestSelection<TYPE_1, SELECTION_1> >
    generateChoice(const SELECTION_0&,
                   const SELECTION_1&,
                   const TYPE_0& value,
                   const PlaceHolder<TYPE_1>&)
    {
        typedef TypedTestSelection<TYPE_0, SELECTION_0> Selection0;
        typedef TypedTestSelection<TYPE_1, SELECTION_1> Selection1;

        return TestChoice<Selection0, Selection1>(value);
    }

    template <class SELECTION_0, class SELECTION_1, class TYPE_0, class TYPE_1>
    static TestChoice<TypedTestSelection<TYPE_0, SELECTION_0>,
                      TypedTestSelection<TYPE_1, SELECTION_1> >
    generateChoice(const SELECTION_0&,
                   const SELECTION_1&,
                   const PlaceHolder<TYPE_0>&,
                   const TYPE_1& value)
    {
        typedef TypedTestSelection<TYPE_0, SELECTION_0> Selection0;
        typedef TypedTestSelection<TYPE_1, SELECTION_1> Selection1;

        return TestChoice<Selection0, Selection1>(value);
    }

    template <class VALUE_TYPE, class BASE_TYPE>
    static TestCustomizedType<VALUE_TYPE, BASE_TYPE> generateCustomizedType(
                                           const VALUE_TYPE&             value,
                                           const PlaceHolder<BASE_TYPE>&)
    {
        return TestCustomizedType<VALUE_TYPE, BASE_TYPE>(value);
    }

    template <class VALUE_TYPE>
    static TestDynamicType<VALUE_TYPE> generateDynamicType(
                                                       const VALUE_TYPE& value)
    {
        return TestDynamicType<VALUE_TYPE>(value);
    }

    template <class ENUMERATOR_0>
    static TestEnumeration<ENUMERATOR_0> generateEnumeration(
                                                     const ENUMERATOR_0& ,
                                                     int                 value)
    {
        return TestEnumeration<ENUMERATOR_0>(value);
    }

    template <class ENUMERATOR_0, class ENUMERATOR_1>
    static TestEnumeration<ENUMERATOR_0, ENUMERATOR_1> generateEnumeration(
                                                     const ENUMERATOR_0& ,
                                                     const ENUMERATOR_1& ,
                                                     int                 value)
    {
        return TestEnumeration<ENUMERATOR_0, ENUMERATOR_1>(value);
    }

    template <class ENUMERATOR_0>
    static PlaceHolder<TestEnumeration<ENUMERATOR_0> >
    generateEnumerationPlaceHolder(const ENUMERATOR_0&)
    {
        typedef TestEnumeration<ENUMERATOR_0> Enumeration;

        return PlaceHolder<Enumeration>();
    }

    template <class ENUMERATOR_0, class ENUMERATOR_1>
    static PlaceHolder<TestEnumeration<ENUMERATOR_0, ENUMERATOR_1> >
    generateEnumerationPlaceHolder(const ENUMERATOR_0&, const ENUMERATOR_1&)
    {
        typedef TestEnumeration<ENUMERATOR_0, ENUMERATOR_1> Enumeration;

        return PlaceHolder<Enumeration>();
    }

    template <class VALUE_TYPE>
    static bdlb::NullableValue<VALUE_TYPE> generateNullableValue(
                                                const PlaceHolder<VALUE_TYPE>&)
    {
        return bdlb::NullableValue<VALUE_TYPE>();
    }

    template <class VALUE_TYPE>
    static bdlb::NullableValue<VALUE_TYPE> generateNullableValue(
                                                       const VALUE_TYPE& value)
    {
        return bdlb::NullableValue<VALUE_TYPE>(value);
    }

    template <class ATTRIBUTE_0, class TYPE_0>
    static TestSequence<TypedTestAttribute<TYPE_0, ATTRIBUTE_0> >
    generateSequence(const ATTRIBUTE_0&, const TYPE_0& value)
    {
        typedef TypedTestAttribute<TYPE_0, ATTRIBUTE_0> Attribute0;

        return TestSequence<Attribute0>(value);
    }

    template <class ATTRIBUTE_0, class ATTRIBUTE_1, class TYPE_0, class TYPE_1>
    static TestSequence<TypedTestAttribute<TYPE_0, ATTRIBUTE_0>,
                        TypedTestAttribute<TYPE_1, ATTRIBUTE_1> >
    generateSequence(const ATTRIBUTE_0&,
                     const ATTRIBUTE_1&,
                     const TYPE_0& value0,
                     const TYPE_1& value1)
    {
        typedef TypedTestAttribute<TYPE_0, ATTRIBUTE_0> Attribute0;
        typedef TypedTestAttribute<TYPE_1, ATTRIBUTE_1> Attribute1;

        return TestSequence<Attribute0, Attribute1>(value0, value1);
    }

    template <class ATTRIBUTE_0, class TYPE_0>
    static PlaceHolder<TestSequence<TypedTestAttribute<TYPE_0, ATTRIBUTE_0> > >
    generateSequencePlaceHolder(const ATTRIBUTE_0&, const PlaceHolder<TYPE_0>&)
    {
        typedef TypedTestAttribute<TYPE_0, ATTRIBUTE_0> Attribute0;
        typedef TestSequence<Attribute0>                Sequence;

        return PlaceHolder<Sequence>();
    }

    template <class ATTRIBUTE_0, class ATTRIBUTE_1, class TYPE_0, class TYPE_1>
    static PlaceHolder<TestSequence<TypedTestAttribute<TYPE_0, ATTRIBUTE_0>,
                                    TypedTestAttribute<TYPE_1, ATTRIBUTE_1> > >
    generateSequencePlaceHolder(const ATTRIBUTE_0&,
                                const ATTRIBUTE_1&,
                                const PlaceHolder<TYPE_0>&,
                                const PlaceHolder<TYPE_1>&)
    {
        typedef TypedTestAttribute<TYPE_0, ATTRIBUTE_0> Attribute0;
        typedef TypedTestAttribute<TYPE_1, ATTRIBUTE_1> Attribute1;
        typedef TestSequence<Attribute0, Attribute1>    Sequence;

        return PlaceHolder<Sequence>();
    }

    template <class TAG_TYPE, class VALUE_TYPE>
    static TestTaggedValue<TAG_TYPE, VALUE_TYPE> generateTaggedValue(
                                            const PlaceHolder<TAG_TYPE>& ,
                                            const VALUE_TYPE&            value)
    {
        return TestTaggedValue<TAG_TYPE, VALUE_TYPE>(value);
    }

    template <class TAG_TYPE, class VALUE_TYPE>
    static PlaceHolder<TestTaggedValue<TAG_TYPE, VALUE_TYPE> >
    generateTaggedValuePlaceHolder(const PlaceHolder<TAG_TYPE>&   ,
                                   const PlaceHolder<VALUE_TYPE>&)
    {
        typedef TestTaggedValue<TAG_TYPE, VALUE_TYPE> TaggedValue;

        return PlaceHolder<TaggedValue>();
    }
};

                          // =======================
                          // class GenerateTestArray
                          // =======================

class GenerateTestArray {
    // This in-core value-semantic class provides a function object whose
    // function call operator can be used to generate objects of
    // specializations of 'bsl::vector'.

  public:
    // TYPES
    typedef AttributeTypeUtil Util;

    // CREATORS
    GenerateTestArray() {}

    // ACCESSORS
    template <class VALUE_TYPE>
    bsl::vector<VALUE_TYPE> operator()(const PlaceHolder<VALUE_TYPE>&) const
    {
        return Util::generateArray(PlaceHolder<VALUE_TYPE>());
    }

    template <class VALUE_TYPE>
    bsl::vector<VALUE_TYPE> operator()(const VALUE_TYPE& value) const
    {
        return Util::generateArray(value);
    }

    template <class VALUE_TYPE>
    bsl::vector<VALUE_TYPE> operator()(const VALUE_TYPE& value0,
                                       const VALUE_TYPE& value1) const
    {
        return Util::generateArray(value0, value1);
    }
};

                     // ==================================
                     // class GenerateTestArrayPlaceHolder
                     // ==================================

class GenerateTestArrayPlaceHolder {
    // This in-core value-semantic class provides a function object whose
    // function call operator can be used to generate 'PlaceHolder'
    // specializations for 'bsl::vector' types.

  public:
    // TYPES
    typedef AttributeTypeUtil Util;

    // CREATORS
    GenerateTestArrayPlaceHolder() {}

    // ACCESSORS
    template <class VALUE_TYPE>
    PlaceHolder<bsl::vector<VALUE_TYPE> > operator()(
                                                const PlaceHolder<VALUE_TYPE>&) const
    {
        return PlaceHolder<bsl::vector<VALUE_TYPE> >();
    }
};

                          // ========================
                          // class GenerateTestChoice
                          // ========================

class GenerateTestChoice {
    // This in-core value-semantic class provides a function object whose
    // function call operator can be used to generate objects of
    // specializations of 'TestChoice'.

  public:
    // TYPES
    typedef AttributeTypeUtil Util;

    // CREATORS
    GenerateTestChoice() {}

    // ACCESSORS
    template <class SELECTION_0, class TYPE_0>
    TestChoice<TypedTestSelection<TYPE_0, SELECTION_0> > operator()(
                                                const SELECTION_0& selection,
                                                const TYPE_0&      value) const
    {
        return Util::generateChoice(selection, value);
    }

    template <class SELECTION_0, class SELECTION_1, class TYPE_0, class TYPE_1>
    TestChoice<TypedTestSelection<TYPE_0, SELECTION_0>,
               TypedTestSelection<TYPE_1, SELECTION_1> >
    operator()(const SELECTION_0&         selection0,
               const SELECTION_1&         selection1,
               const TYPE_0&              value,
               const PlaceHolder<TYPE_1>& placeHolder) const
    {
        return Util::generateChoice(
            selection0, selection1, value, placeHolder);
    }

    template <class SELECTION_0, class SELECTION_1, class TYPE_0, class TYPE_1>
    TestChoice<TypedTestSelection<TYPE_0, SELECTION_0>,
               TypedTestSelection<TYPE_1, SELECTION_1> >
    operator()(const SELECTION_0&         selection0,
               const SELECTION_1&         selection1,
               const PlaceHolder<TYPE_0>& placeHolder,
               const TYPE_1&              value) const
    {
        return Util::generateChoice(
            selection0, selection1, placeHolder, value);
    }
};

                    // ===================================
                    // class GenerateTestChoicePlaceHolder
                    // ===================================

class GenerateTestChoicePlaceHolder {
    // This in-core value-semantic class provides a function object whose
    // function call operator can be used to generate objects of
    // specializations of 'TestChoice'.

  public:
    // TYPES
    typedef AttributeTypeUtil Util;

    // CREATORS
    GenerateTestChoicePlaceHolder() {}

    // ACCESSORS
    template <class SELECTION_0, class TYPE_0>
    PlaceHolder<TestChoice<TypedTestSelection<TYPE_0, SELECTION_0> > >
    operator()(const SELECTION_0&, const PlaceHolder<TYPE_0>&) const
    {
        typedef TypedTestSelection<TYPE_0, SELECTION_0> Selection0;
        typedef TestChoice<Selection0>                  Choice;

        return PlaceHolder<Choice>();
    }

    template <class SELECTION_0, class SELECTION_1, class TYPE_0, class TYPE_1>
    PlaceHolder<TestChoice<TypedTestSelection<TYPE_0, SELECTION_0>,
                           TypedTestSelection<TYPE_1, SELECTION_1> > >
    operator()(const SELECTION_0&,
               const SELECTION_1&,
               const PlaceHolder<TYPE_0>&,
               const PlaceHolder<TYPE_1>&)
    {
        typedef TypedTestSelection<TYPE_0, SELECTION_0> Selection0;
        typedef TypedTestSelection<TYPE_1, SELECTION_1> Selection1;
        typedef TestChoice<Selection0, Selection1>      Choice;

        return PlaceHolder<Choice>();
    }
};

                      // ================================
                      // class GenerateTestCustomizedType
                      // ================================

class GenerateTestCustomizedType {
    // This in-core value-semantic class provides a function object whose
    // function call operator can be used to generate objects of
    // specializations of 'TestCustomizedType'.

  public:
    // TYPES
    typedef AttributeTypeUtil Util;

    // CREATORS
    GenerateTestCustomizedType() {}

    // ACCESSORS
    template <class VALUE_TYPE, class BASE_TYPE>
    TestCustomizedType<VALUE_TYPE, BASE_TYPE> operator()(
                                           const VALUE_TYPE&             value,
                                           const PlaceHolder<BASE_TYPE>&) const
    {
        return Util::generateCustomizedType(value, PlaceHolder<BASE_TYPE>());
    }
};

                       // =============================
                       // class GenerateTestDynamicType
                       // =============================

class GenerateTestDynamicType {
    // This in-core value-semantic class provides a function object whose
    // function call operator can be used to generate objects of
    // specializations of 'TestDynamicType'.

  public:
    // TYPES
    typedef AttributeTypeUtil Util;

    // CREATORS
    GenerateTestDynamicType() {}

    // ACCESSORS
    template <class VALUE_TYPE>
    TestDynamicType<VALUE_TYPE> operator()(const VALUE_TYPE& value) const
    {
        return Util::generateDynamicType(value);
    }
};

                    // ====================================
                    // class GenerateTestDynamicPlaceHolder
                    // ====================================

class GenerateTestDynamicPlaceHolder {
    // This in-core value-semantic class provides a function object whose
    // function call operator can be used to generate 'PlaceHolder'
    // specializations for 'TestDynamicType' types.

  public:
    // TYPES
    typedef AttributeTypeUtil Util;

    // CREATORS
    GenerateTestDynamicPlaceHolder() {}

    // ACCESSORS
    template <class VALUE_TYPE>
    PlaceHolder<TestDynamicType<VALUE_TYPE> > operator()(
                                                const PlaceHolder<VALUE_TYPE>&) const
    {
        return PlaceHolder<TestDynamicType<VALUE_TYPE> >();
    }
};

                       // =============================
                       // class GenerateTestEnumeration
                       // =============================

class GenerateTestEnumeration {
    // This in-core value-semantic class provides a function object whose
    // function call operator can be used to generate 'TestEnumeration'
    // objects.

  public:
    // TYPES
    typedef AttributeTypeUtil Util;

    // CREATORS
    GenerateTestEnumeration() {}

    // ACCESSORS
    template <class ENUMERATOR_0>
    TestEnumeration<ENUMERATOR_0> operator()(const ENUMERATOR_0& enumerator,
                                             int                 value) const
    {
        return Util::generateEnumeration(enumerator, value);
    }

    template <class ENUMERATOR_0, class ENUMERATOR_1>
    TestEnumeration<ENUMERATOR_0, ENUMERATOR_1> operator()(
                                               const ENUMERATOR_0& enumerator0,
                                               const ENUMERATOR_1& enumerator1,
                                               int                 value) const
    {
        return Util::generateEnumeration(enumerator0, enumerator1, value);
    }
};

                  // ========================================
                  // class GenerateTestEnumerationPlaceHolder
                  // ========================================

class GenerateTestEnumerationPlaceHolder {
    // This in-core value-semantic class provides a function object whose
    // function call operator can be used to generate 'PlaceHolder'
    // specializations for 'TestEnumeration' types.

  public:
    // TYPES
    typedef AttributeTypeUtil Util;

    // CREATORS
    GenerateTestEnumerationPlaceHolder() {}

    // ACCESSORS
    template <class ENUMERATOR_0>
    PlaceHolder<TestEnumeration<ENUMERATOR_0> > operator()(
                                          const ENUMERATOR_0& enumerator) const
    {
        return Util::generateEnumerationPlaceHolder(enumerator);
    }

    template <class ENUMERATOR_0, class ENUMERATOR_1>
    PlaceHolder<TestEnumeration<ENUMERATOR_0, ENUMERATOR_1> > operator()(
                                         const ENUMERATOR_0& enumerator0,
                                         const ENUMERATOR_1& enumerator1) const
    {
        return Util::generateEnumerationPlaceHolder(enumerator0, enumerator1);
    }
};

                      // ===============================
                      // class GenerateTestNullableValue
                      // ===============================

class GenerateTestNullableValue {
    // This in-core value-semantic class provides a function object whose
    // function call operator can be used to generate objects of
    // 'bdlb::NullableValue' specializations.

  public:
    // TYPES
    typedef AttributeTypeUtil Util;

    // CREATORS
    GenerateTestNullableValue() {}

    // ACCESSORS
    template <class VALUE_TYPE>
    bdlb::NullableValue<VALUE_TYPE> operator()(const PlaceHolder<VALUE_TYPE>&) const
    {
        return Util::generateNullableValue(PlaceHolder<VALUE_TYPE>());
    }

    template <class VALUE_TYPE>
    bdlb::NullableValue<VALUE_TYPE> operator()(const VALUE_TYPE& value) const
    {
        return Util::generateNullableValue(value);
    }
};

                   // =====================================
                   // class GenerateTestNullablePlaceHolder
                   // =====================================

class GenerateTestNullablePlaceHolder {
    // This in-core value-semantic class provides a function object whose
    // function call operator can be used to generate 'PlaceHolder'
    // specializations for 'bdlb::NullableValue' types.

  public:
    // TYPES
    typedef AttributeTypeUtil Util;

    // CREATORS
    GenerateTestNullablePlaceHolder() {}

    // ACCESSOR
    template <class VALUE_TYPE>
    PlaceHolder<bdlb::NullableValue<VALUE_TYPE> > operator()(
                                                const PlaceHolder<VALUE_TYPE>&) const
    {
        return PlaceHolder<bdlb::NullableValue<VALUE_TYPE> >();
    }
};

                         // ==========================
                         // class GenerateTestSequence
                         // ==========================

class GenerateTestSequence {
    // This in-core value-semantic class provides a function object whose
    // function call operator can be used to generate objects of 'TestSequence'
    // specializations.

  public:
    // TYPES
    typedef AttributeTypeUtil Util;

    // CREATORS
    GenerateTestSequence() {}

    // ACCESSORS
    template <class ATTRIBUTE_0, class TYPE_0>
    TestSequence<TypedTestAttribute<TYPE_0, ATTRIBUTE_0> > operator()(
                                                const ATTRIBUTE_0& attribute,
                                                const TYPE_0&      value) const
    {
        return Util::generateSequence(attribute, value);
    }

    template <class ATTRIBUTE_0, class ATTRIBUTE_1, class TYPE_0, class TYPE_1>
    TestSequence<TypedTestAttribute<TYPE_0, ATTRIBUTE_0>,
                 TypedTestAttribute<TYPE_1, ATTRIBUTE_1> >
    operator()(const ATTRIBUTE_0& attribute0,
               const ATTRIBUTE_1& attribute1,
               const TYPE_0&      value0,
               const TYPE_1&      value1) const
    {
        return Util::generateSequence(attribute0, attribute1, value0, value1);
    }
};

                   // =====================================
                   // class GenerateTestSequencePlaceHolder
                   // =====================================

class GenerateTestSequencePlaceHolder {
    // This in-core value-semantic class provides a function object whose
    // function call operator can be used to generate 'PlaceHolder'
    // specializations for 'TestSequence' types.

  public:
    // TYPES
    typedef AttributeTypeUtil Util;

    // CREATORS
    GenerateTestSequencePlaceHolder() {}

    // ACCESSORS
    template <class ATTRIBUTE_0, class TYPE_0>
    PlaceHolder<TestSequence<TypedTestAttribute<TYPE_0, ATTRIBUTE_0> > >
    operator()(const ATTRIBUTE_0&         attribute,
               const PlaceHolder<TYPE_0>& value) const
    {
        return Util::generateSequencePlaceHolder(attribute, value);
    }

    template <class ATTRIBUTE_0, class ATTRIBUTE_1, class TYPE_0, class TYPE_1>
    PlaceHolder<TestSequence<TypedTestAttribute<TYPE_0, ATTRIBUTE_0>,
                             TypedTestAttribute<TYPE_1, ATTRIBUTE_1> > >
    operator()(const ATTRIBUTE_0&         attribute0,
               const ATTRIBUTE_1&         attribute1,
               const PlaceHolder<TYPE_0>& value0,
               const PlaceHolder<TYPE_1>& value1) const
    {
        return Util::generateSequencePlaceHolder(
            attribute0, attribute1, value0, value1);
    }
};

                       // =============================
                       // class GenerateTestTaggedValue
                       // =============================

class GenerateTestTaggedValue {
  public:
    // TYPES
    typedef AttributeTypeUtil Util;

    // CREATORS
    GenerateTestTaggedValue() {}

    // ACCESSORS
    template <class TAG_TYPE, class VALUE_TYPE>
    TestTaggedValue<TAG_TYPE, VALUE_TYPE> operator()(
                                      const PlaceHolder<TAG_TYPE>& tag,
                                      const VALUE_TYPE&            value) const
    {
        return Util::generateTaggedValue(tag, value);
    }
};

                  // ========================================
                  // class GenerateTestTaggedValuePlaceHolder
                  // ========================================

class GenerateTestTaggedValuePlaceHolder {
  public:
    // TYPES
    typedef AttributeTypeUtil Util;

    // CREATORS
    GenerateTestTaggedValuePlaceHolder() {}

    // ACCESSORS
    template <class TAG_TYPE, class VALUE_TYPE>
    PlaceHolder<TestTaggedValue<TAG_TYPE, VALUE_TYPE> > operator()(
                                    const PlaceHolder<TAG_TYPE>&   tag,
                                    const PlaceHolder<VALUE_TYPE>& value) const
    {
        return Util::generateTaggedValuePlaceHolder(tag, value);
    }
};

}  // close enterprise namespace

// ============================================================================
//                          END BDLAT TEST APPARATUS
// ----------------------------------------------------------------------------

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

    TestXmlElement(const bslstl::StringRef&  name,
                   bslma::Allocator         *basicAllocator = 0)
    : d_name(name, basicAllocator)
    , d_attributes(basicAllocator)
    , d_content(Elements(), basicAllocator)
    {
    }

    TestXmlElement(const bslstl::StringRef&  name,
                   const Attributes&         attributes,
                   bslma::Allocator         *basicAllocator = 0)
    : d_name(name, basicAllocator)
    , d_attributes(attributes, basicAllocator)
    , d_content(Elements(), basicAllocator)
    {
    }

    TestXmlElement(const bslstl::StringRef&  name,
                   const Content&            content,
                   bslma::Allocator         *basicAllocator = 0)
    : d_name(name, basicAllocator)
    , d_attributes(basicAllocator)
    , d_content(content, basicAllocator)
    {
    }

    TestXmlElement(const bslstl::StringRef&  name,
                   const Attributes&         attributes,
                   const Content&            content,
                   bslma::Allocator         *basicAllocator = 0)
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
    static bsl::vector<Attribute> generateAttributes(
                                                const bslstl::StringRef& key,
                                                const bslstl::StringRef& value)
    {
        bsl::vector<Attribute> result;
        result.push_back(Attribute(key, value));
        return result;
    }

    static bsl::vector<Attribute> generateAttributes(
                                               const bslstl::StringRef& key0,
                                               const bslstl::StringRef& value0,
                                               const bslstl::StringRef& key1,
                                               const bslstl::StringRef& value1)
    {
        bsl::vector<Attribute> result;
        result.push_back(Attribute(key0, value0));
        result.push_back(Attribute(key1, value1));
        return result;
    }

    static Content generateContent(const bslstl::StringRef& content)
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

    static TestXmlElement generateElement(const bslstl::StringRef& name)
    {
        return TestXmlElement(name);
    }

    static TestXmlElement generateElement(
                                       const bslstl::StringRef& name,
                                       const bslstl::StringRef& attributeKey,
                                       const bslstl::StringRef& attributeValue)
    {
        return TestXmlElement(
            name, generateAttributes(attributeKey, attributeValue));
    }

    static TestXmlElement generateElement(
                                      const bslstl::StringRef& name,
                                      const bslstl::StringRef& attributeKey0,
                                      const bslstl::StringRef& attributeValue0,
                                      const bslstl::StringRef& attributeKey1,
                                      const bslstl::StringRef& attributeValue1)
    {
        return TestXmlElement(name,
                              generateAttributes(attributeKey0,
                                                 attributeValue0,
                                                 attributeKey1,
                                                 attributeValue1));
    }

    static TestXmlElement generateElement(const bslstl::StringRef& name,
                                          const bslstl::StringRef& content)
    {
        return TestXmlElement(name, generateContent(content));
    }

    static TestXmlElement generateElement(const bslstl::StringRef& name,
                                          const TestXmlElement&    child)
    {
        return TestXmlElement(name, generateContent(child));
    }

    static TestXmlElement generateElement(const bslstl::StringRef& name,
                                          const TestXmlElement&    child0,
                                          const TestXmlElement&    child1)
    {
        return TestXmlElement(name, generateContent(child0, child1));
    }

    static TestXmlElement generateElement(
                                       const bslstl::StringRef& name,
                                       const bslstl::StringRef& attributeKey,
                                       const bslstl::StringRef& attributeValue,
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
    TestXmlElement operator()(const bslstl::StringRef& name) const
    {
        return Util::generateElement(name);
    }

    TestXmlElement operator()(const bslstl::StringRef& name,
                              const bslstl::StringRef& attributeKey,
                              const bslstl::StringRef& attributeValue) const
    {
        return Util::generateElement(name, attributeKey, attributeValue);
    }

    TestXmlElement operator()(const bslstl::StringRef& name,
                              const bslstl::StringRef& attributeKey0,
                              const bslstl::StringRef& attributeValue0,
                              const bslstl::StringRef& attributeKey1,
                              const bslstl::StringRef& attributeValue1) const
    {
        return Util::generateElement(name,
                                     attributeKey0,
                                     attributeValue0,
                                     attributeKey1,
                                     attributeValue1);
    }

    TestXmlElement operator()(const bslstl::StringRef& name,
                              const bslstl::StringRef& content) const
    {
        return Util::generateElement(name, content);
    }

    TestXmlElement operator()(const bslstl::StringRef& name,
                              const TestXmlElement&    child) const
    {
        return Util::generateElement(name, child);
    }

    TestXmlElement operator()(const bslstl::StringRef& name,
                              const TestXmlElement&    child0,
                              const TestXmlElement&    child1) const
    {
        return Util::generateElement(name, child0, child1);
    }

    TestXmlElement operator()(const bslstl::StringRef& name,
                              const bslstl::StringRef& attributeKey,
                              const bslstl::StringRef& attributeValue,
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

                       // ==============================
                       // class TestCase19RowProtocolImp
                       // ==============================

class TestCase19RowProtocol {
  public:
    // CREATORS
    virtual ~TestCase19RowProtocol() {}

    // ACCESSORS
    virtual bslma::ManagedPtr<TestCase19RowProtocol> clone(
                                        bslma::Allocator *allocator) const = 0;
    virtual void runTest() const                                           = 0;
};

                       // ==============================
                       // class TestCase19RowProtocolImp
                       // ==============================

template <class VALUE_TYPE>
class TestCase19RowProtocolImp : public TestCase19RowProtocol {
  public:
    // TYPES
    typedef VALUE_TYPE Value;

    // CLASS DATA
    enum {
        e_DECODING_FAILS                  = 0,
        e_DECODING_SUCCEEDS               = 1,
        e_DECODING_YIELDS_DIFFERENT_VALUE = 2
    };

  private:
    // DATA
    int                             d_line;
    bslalg::ConstructorProxy<Value> d_value;
    int                             d_encodingStatus;
    int                             d_decodingStatus;
    TestXmlElement                    d_xml;

    // NOT IMPLEMENTED
    TestCase19RowProtocolImp(const TestCase19RowProtocolImp&);
    TestCase19RowProtocolImp& operator=(const TestCase19RowProtocolImp&);

  public:
    // CREATORS
    explicit TestCase19RowProtocolImp(int                  line,
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

    ~TestCase19RowProtocolImp() BSLS_KEYWORD_OVERRIDE {}

    // ACCESSORS
    bslma::ManagedPtr<TestCase19RowProtocol> clone(
                       bslma::Allocator *allocator) const BSLS_KEYWORD_OVERRIDE
    {
        bslma::Allocator *const basicAllocator = bslma::Default::allocator(
                                                                    allocator);

        return bslma::ManagedPtr<TestCase19RowProtocol>(
            new (*basicAllocator) TestCase19RowProtocolImp(d_line,
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
        const Value&        EXPECTED_VALUE  = d_value.object();
        const int           DECODING_STATUS = d_decodingStatus;
        const TestXmlElement& XML             = d_xml;

        bdlsb::MemOutStreamBuf xmlOutStreamBuf;
        bsl::ostream           xmlOutStream(&xmlOutStreamBuf);

        XML.print(xmlOutStream);

        balxml::MiniReader     reader;
        balxml::ErrorInfo      errorInfo;
        balxml::DecoderOptions options;

        balxml::Decoder mX(&options, &reader, &errorInfo);

        bdlsb::FixedMemInStreamBuf xmlStreamBuf(xmlOutStreamBuf.data(),
                                                xmlOutStreamBuf.length());

        Value value;
        int   rc = mX.decode(&xmlStreamBuf, &value);
        switch (DECODING_STATUS) {
          case e_DECODING_FAILS: {
              ASSERTV(LINE, rc, 0 != rc);
          } break;
          case e_DECODING_SUCCEEDS: {
              ASSERTV(LINE, rc, 0 == rc);
              if (0 != rc) {
                  P(mX.loggedMessages());
              }

              ASSERTV(LINE, EXPECTED_VALUE, value, EXPECTED_VALUE == value);
          } break;
          case e_DECODING_YIELDS_DIFFERENT_VALUE: {
              ASSERTV(LINE, rc, 0 == rc);
              if (0 != rc) {
                  P(mX.loggedMessages());
              }

              ASSERTV(LINE, EXPECTED_VALUE, value, EXPECTED_VALUE != value);
          } break;
        }
    }
};

                            // ===================
                            // class TestCase19Row
                            // ===================

class TestCase19Row {
    // DATA
    bslma::Allocator                         *d_allocator_p;
    bslma::ManagedPtr<TestCase19RowProtocol>  d_imp;

  public:
    // CREATORS
    template <class VALUE_TYPE>
    TestCase19Row(int                  line,
                  const VALUE_TYPE&    value,
                  int                  encodingStatus,
                  int                  decodingStatus,
                  const TestXmlElement&  xml,
                  bslma::Allocator    *basicAllocator = 0)
    : d_allocator_p(bslma::Default::allocator(basicAllocator))
    , d_imp(new (*d_allocator_p)
                TestCase19RowProtocolImp<VALUE_TYPE>(line,
                                                     value,
                                                     encodingStatus,
                                                     decodingStatus,
                                                     xml,
                                                     d_allocator_p),
            d_allocator_p)
    {
    }

    TestCase19Row(const TestCase19Row&  original,
                  bslma::Allocator     *basicAllocator = 0)
    : d_allocator_p(bslma::Default::allocator(basicAllocator))
    , d_imp(original.d_imp->clone(d_allocator_p))
    {
    }

    // MANIPULATORS
    TestCase19Row& operator=(const TestCase19Row& original)
    {
        d_imp = original.d_imp->clone(d_allocator_p);
        return *this;
    }

    // ACCESSORS
    void runTest() const { d_imp->runTest(); }
};

                         // ==========================
                         // struct FailToManipulateTag
                         // ==========================

///Implementation Note
///-------------------
// The following tag type and 'bdlat' customization point function overloads
// allow one to conditionally cause the associated 'bdlat' operations of
// 'TestTaggedType' specializations having a 'TAG_TYPE' of
// 'FailToManipulateSequenceTag' to return non-zero status codes.

struct DefaultTag {
    // This tag type may be used as the 'TAG_TYPE' of a 'TestTaggedType' in
    // order to indicate that no 'bdlat' operations of its underlying
    // 'VALUE_TYPE' are to be overloaded.
};

struct FailToManipulateSequenceTag {
    // This tag type may be used as the 'TAG_TYPE' of a 'TestTaggedType' in
    // order to overload the 'sequenceManipulateAttribute' and
    // 'typeCategoryManipulateSequence' operations of its underlying
    // 'VALUE_TYPE' with implementations that return non-zero.
};

template <class VALUE_TYPE, class MANIPULATOR>
int bdlat_sequenceManipulateAttribute(
                    TestTaggedValue<FailToManipulateSequenceTag, VALUE_TYPE> *,
                    MANIPULATOR&                                              ,
                    const char *,
                    int)
{
    return -1;
}

template <class VALUE_TYPE, class MANIPULATOR>
int bdlat_typeCategoryManipulateSequence(
  TestTaggedValue<FailToManipulateSequenceTag, TestDynamicType<VALUE_TYPE> > *,
  MANIPULATOR&)
{
    return -1;
}

                        // ===========================
                        // class TestCase20RowProtocol
                        // ===========================

class TestCase20RowProtocol {
    // This pure abstract base class provides a protocol for running test case
    // 20, which tests that errors returned from 'bdlat' operations of
    // 'DynamicType' objects having 'Sequence' dynamic categories cause the
    // whole decoding operation to fail.

  public:
    // CREATORS
    virtual ~TestCase20RowProtocol()
        // Destroy this object.
    {
    }

    // ACCESSORS
    virtual bslma::ManagedPtr<TestCase20RowProtocol> clone(
                               bslma::Allocator *basicAllocator = 0) const = 0;
        // Return a managed pointer that manages a copy of the value of this
        // object.  Optionally specify a 'basicAllocator' used to supply memory
        // to the resultant copy.  If 'basicAllocator' is 0, the currently
        // installed default allocator is used.

    virtual void runTest() const = 0;
        // Check some correctness properties of 'balxml::Decoder' and increment
        // the global 'testStatus' variable for each failed correctness check,
        // if any, and write relevant testing information to 'bsl::cout' and/or
        // 'bsl::cerr'.  Note that if no checks fail, the value of 'testStatus'
        // is left unmodified.
};

                       // ==============================
                       // class TestCase20RowProtocolImp
                       // ==============================

template <class VALUE_TYPE>
class TestCase20RowProtocolImp : public TestCase20RowProtocol {
    // This concrete class provides an implementation of the
    // 'TestCase20RowProtocol' to verify some correctness properties of the
    // 'balxml::Decoder::decode' operation when applied to 'VALUE_TYPE'
    // objects.  See test case 20 for documentation for what specific checks
    // are performed by this test.

  public:
    // TYPES
    typedef VALUE_TYPE Value;
        // Alias to the 'bdlat' value type for which this
        // 'TestCase20RowProtocol' implementation supplies to
        // 'balxml::Decoder::decode' in order to check some correctness
        // properties of the 'decode' operation.

  private:
    // DATA
    int            d_line;         // line number
    TestXmlElement d_xml;          // specification for xml to decode from
    bool d_decodeSuccessStatus;    // whether or not decoding should succeed
    bsl::string d_loggedMessages;  // expected 'loggedMessages' after decode

    // NOT IMPLEMENTED
    TestCase20RowProtocolImp(const TestCase20RowProtocolImp&)
                                                          BSLS_KEYWORD_DELETED;
    TestCase20RowProtocolImp& operator=(const TestCase20RowProtocolImp)
                                                          BSLS_KEYWORD_DELETED;

  public:
    // CREATORS
    TestCase20RowProtocolImp(int                       line,
                             const TestXmlElement&     xml,
                             bool                      decodeSuccessStatus,
                             const bslstl::StringRef&  loggedMessages,
                             bslma::Allocator         *basicAllocator = 0)
    : d_line(line)
    , d_xml(xml, basicAllocator)
    , d_decodeSuccessStatus(decodeSuccessStatus)
    , d_loggedMessages(loggedMessages, basicAllocator)
    {
    }

    ~TestCase20RowProtocolImp() BSLS_KEYWORD_OVERRIDE
        // Destroy this object.
    {
    }

    // ACCESSORS
    bslma::ManagedPtr<TestCase20RowProtocol> clone(
              bslma::Allocator *basicAllocator = 0) const BSLS_KEYWORD_OVERRIDE
        // Return a managed pointer that manages a copy of the value of this
        // object.  Optionally specify a 'basicAllocator' used to supply memory
        // to the resultant copy.  If 'basicAllocator' is 0, the currently
        // installed default allocator is used.
    {
        bslma::Allocator *const allocator = bslma::Default::allocator(
                                                                basicAllocator);
        return bslma::ManagedPtr<TestCase20RowProtocol>(

            new (*allocator)
                TestCase20RowProtocolImp(d_line,
                                         d_xml,
                                         d_decodeSuccessStatus,
                                         d_loggedMessages,
                                         allocator),
            allocator);
    }

    void runTest() const BSLS_KEYWORD_OVERRIDE
        // Check some correctness properties of 'balxml::Decoder' and increment
        // the global 'testStatus' variable for each failed correctness check,
        // if any, and write relevant testing information to 'bsl::cout' and/or
        // 'bsl::cerr'.  Note that if no checks fail, the value of 'testStatus'
        // is left unmodified.   See test case 20 for documentation for what
        // specific checks are performed by this test.
    {
        const int             LINE                  = d_line;
        const TestXmlElement& XML                   = d_xml;
        const bool            DECODE_SUCCESS_STATUS = d_decodeSuccessStatus;
        const bsl::string&    LOGGED_MESSAGES       = d_loggedMessages;

        bdlsb::MemOutStreamBuf xmlOutStreamBuf;
        bsl::ostream xmlOutStream(&xmlOutStreamBuf);
        XML.print(xmlOutStream);

        bdlsb::FixedMemInStreamBuf xmlInStreamBuf(xmlOutStreamBuf.data(),
                                                  xmlOutStreamBuf.length());

        balxml::MiniReader reader;
        balxml::ErrorInfo errorInfo;
        balxml::DecoderOptions options;
        options.setSkipUnknownElements(false);

        bdlsb::MemOutStreamBuf errorStreamBuf;
        bsl::ostream errorStream(&errorStreamBuf);

        bdlsb::MemOutStreamBuf warningStreamBuf;
        bsl::ostream warningStream(&warningStreamBuf);

        balxml::Decoder mX(&options, &reader, &errorInfo, &errorStream,
                           &warningStream);

        Value value;
        int rc = mX.decode(&xmlInStreamBuf, &value);

        const bslstl::StringRef errors(errorStreamBuf.data(),
                                       errorStreamBuf.length());

        const bslstl::StringRef warnings(warningStreamBuf.data(),
                                         warningStreamBuf.length());

        if (DECODE_SUCCESS_STATUS) {
            ASSERTV(L_, LINE, rc, 0 == rc);
            if (0 != rc) {
                P(warnings);
                P(errors);
            }
        }
        else {
            ASSERTV(L_, LINE, rc, 0 != rc);
        }

        ASSERTV(L_,
                LINE,
                mX.loggedMessages(),
                mX.loggedMessages() == LOGGED_MESSAGES);
    }
};

                            // ===================
                            // class TestCase20Row
                            // ===================

class TestCase20Row {
    // This class provides a wrapper around a specialization of
    // 'TestCase20RowProtocolImp' supplied on construction, and erases the
    // associated 'VALUE_TYPE', so that multiple 'TestCase20RowProtocolImp'
    // specializations for different 'VALUE_TYPE' types may be stored in an
    // array of 'TestCase20Row' in order to create a "test table".

    // PRIVATE
    bslma::ManagedPtr<TestCase20RowProtocol>  d_imp;          // managed imp
    bslma::Allocator                         *d_allocator_p;  // memory supply

  public:
    // CREATORS
    template <class VALUE_TYPE>
    TestCase20Row(int                             line,
                  const PlaceHolder<VALUE_TYPE>&,
                  const TestXmlElement&           xml,
                  bool                            decodingSucceeds,
                  const bslstl::StringRef&        loggedMessages,
                  bslma::Allocator               *basicAllocator = 0)
    : d_imp(new (*bslma::Default::allocator(basicAllocator))
                TestCase20RowProtocolImp<VALUE_TYPE>(line,
                                                     xml,
                                                     decodingSucceeds,
                                                     loggedMessages,
                                                     basicAllocator),
            bslma::Default::allocator(basicAllocator))
    , d_allocator_p(bslma::Default::allocator(basicAllocator))
    {
    }

    TestCase20Row(const TestCase20Row&  original,
                  bslma::Allocator     *basicAllocator = 0)
    : d_imp(original.d_imp->clone(basicAllocator))
    , d_allocator_p(bslma::Default::allocator(basicAllocator))
    {
    }

    void runTest() const
        // Check some correctness properties of 'balxml::Decoder' and increment
        // the global 'testStatus' variable for each failed correctness check,
        // if any, and write relevant testing information to 'bsl::cout' and/or
        // 'bsl::cerr'.  Note that if no checks fail, the value of 'testStatus'
        // is left unmodified.   See test case 20 for documentation for what
        // specific checks are performed by this test.
    {
        d_imp->runTest();
    }

    // MANIPULATORS
    TestCase20Row& operator=(const TestCase20Row& original)
    {
        d_imp = original.d_imp->clone(d_allocator_p);

        return *this;
    }
};

///Implementation Note
///-------------------
// The below set of string variables use constant-initialization, have external
// linkage, and have static storage duration so that they may be used as
// arguments for non-type template parameters (of 'const char *' type).

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
//                               USAGE EXAMPLE
// ----------------------------------------------------------------------------

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

    using namespace BloombergLP;

    int usageExample1()
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

        balxml::DecoderOptions options;
        balxml::MiniReader     reader;
        balxml::ErrorInfo      errInfo;

        balxml::Decoder decoder(&options, &reader, &errInfo);

        decoder.decode(ss, &bob);

        ASSERT(ss);
        ASSERT("Bob"         == bob.name());
        ASSERT("Some Street" == bob.homeAddress().street());
        ASSERT("Some City"   == bob.homeAddress().city());
        ASSERT("Some State"  == bob.homeAddress().state());
        ASSERT(21            == bob.age());

        return 0;
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

        balxml::DecoderOptions options;
        balxml::MiniReader     reader;
        balxml::ErrorInfo      errInfo;

        options.setSkipUnknownElements(false);
        balxml::Decoder decoder(&options,
                                &reader,
                                &errInfo,
                                &bsl::cerr,
                                &bsl::cerr);
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

        balxml::MiniReader     reader;
        balxml::ErrorInfo      errInfo;
        balxml::DecoderOptions options;

        balxml::Decoder decoder(&options,
                                &reader,
                                &errInfo,
                                &bsl::cerr,
                                &bsl::cerr);

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

// ============================================================================
//                               MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    verbose = argc > 2;
    veryVerbose = argc > 3;
    veryVeryVerbose = argc > 4;
    veryVeryVeryVerbose = argc > 5;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;;

    // CONCERN: 'BSLS_REVIEW' failures should lead to test failures.
    bsls::ReviewFailureHandlerGuard reviewGuard(&bsls::Review::failByAbort);

    switch (test) { case 0:  // Zero is always the leading case.
      case 21: {
        // --------------------------------------------------------------------
        // Testing Decimal64
        //
        // Concerns:
        //: 1 That the decoder can encoder a field of type 'Decimal64'.
        //
        // Plan:
        //: 1 Copy the style of translation done in the breathing test to
        //:   translate a single object of 'bdldfp::Decimal64' type.
        //
        // Testing:
        //   Type Decimal64
        // --------------------------------------------------------------------

        if (verbose) cout << "Testing Decimal64\n"
                             "=================\n";

#define DFP(X) BDLDFP_DECIMAL_DD(X)

        static const struct Data {
            int                d_line;
            bdldfp::Decimal64  d_exp;
            const char        *d_text;
        } DATA[] = {
            { L_, DFP(0.0), "0.0" },
            { L_, DFP(0.0), "-0.0" },
            { L_, DFP(15.13), "15.13" },
            { L_, DFP(-15.13), "-15.13" },
            { L_, DFP(15.13), "1513e-2" },
            { L_, DFP(-15.13), "-1513e-2" },
            { L_, DFP(892.0), "892.0" },
            { L_, DFP(892.0), "892.0" },
            { L_, DFP(-892.0), "-892.0" },
            { L_, DFP(892.0), "892" },
            { L_, DFP(-892.0), "-892" },
            { L_, DFP(4.73e35), "4.73e+35" },
            { L_, DFP(-4.73e35), "-4.73e+35" },
            { L_, DFP(4.73e35), "473e+33" },
            { L_, DFP(-4.73e35), "-473e+33" }
        };
        enum { k_NUM_DATA = sizeof DATA / sizeof *DATA };

        for (int ti = 0; ti < k_NUM_DATA; ++ti) {
            const Data&               data  = DATA[ti];
            const int                 LINE  = data.d_line;
            const bdldfp::Decimal64&  EXP   = data.d_exp;
            const char               *TEXT  = data.d_text;

            bsl::stringstream ss;
            ss << "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                  "<Value " XSI ">\n"
                  "    " << TEXT << "\n"
                  "</Value>\n";

            const bsl::string& INPUT = ss.str();

            if (veryVerbose) {
                T_;    P(INPUT);
            }

            bdldfp::Decimal64 d64;

            balxml::MiniReader     reader;
            balxml::ErrorInfo      errInfo;
            balxml::DecoderOptions options;

            balxml::Decoder decoder(&options,
                                    &reader,
                                    &errInfo,
                                    &bsl::cerr,
                                    &bsl::cerr);

            int ret = decoder.decode(ss.rdbuf(), &d64);

            LOOP_ASSERT(ret, 0 == ret);
            ASSERTV(LINE, INPUT, EXP, d64, EXP == d64);
        }
      } break;
      case 20: {
        // --------------------------------------------------------------------
        // TESTING ERROR CODE PROPOGATION FOR DYNAMIC TYPES
        //   This case tests that the 'balxml::Decoder::decode' operation
        //   returns a non-zero value when it invokes a 'bdlat' manipulation
        //   operation of a dynamic type that returns a non-zero value.
        //
        // Concerns:
        //: 1 Attempting to decode XML into a 'bdlat' 'Sequence' object that
        //:   returns a non-zero value when its 'manipulatAttribute' operation
        //:   is invoked causes the whole decoding operation to fail, and
        //:   thereby also return non-zero.  Note that the decoder invokes
        //:   'manipulateAttribute' on any 'Sequence' object in order to load
        //:   values into its attribute(s).
        //:
        //: 2 Attempting to decode XML into a 'bdlat' 'Sequence' object that
        //:   returns zero for all 'bdlat' operations causes the whole decode
        //:   operation to succeed, and thereby also return zero.
        //:
        //: 3 Attempting to decode XML into a 'bdlat' 'DynamicType' object
        //:   that has a dynamic category of 'Sequence' and that returns a
        //:   non-zero value when its 'typeCategoryManipulateSequence'
        //:   operation is invoked causes the whole decoding operation to fail,
        //:   and thereby also return non-zero.  Note that the decoder invokes
        //:   'typeCategoryManipulateSequence' in order to load a value into
        //:   any 'DynamicType' object having a 'Sequence' dynamic category.
        //:
        //: 4 Attempting to decode XML into a 'bdlat' 'DynamicType' object
        //:   that has a dynamic category of 'Sequence' and that returns
        //:   zero for all 'bdlat' operations causes the whole decode
        //:   operation to succeed, and thereby also return zero.
        //
        // Plan:
        //: 1 Verify that decoding suitable XML into a 'Sequence' object that
        //:   returns zero for all 'bdlat' operations suceeds.
        //:
        //: 2 Verify the same property for such a 'Sequence' wrapped in a
        //:   'TestTaggedValue' that overrides no operations of the
        //:   underlying 'Sequence'.
        //:
        //: 3 Verify that wrapping such a 'Sequence' in a 'TestTaggedValue'
        //:   that causes some 'bdlat' operations to fail during decoding
        //:   causes the decoding operation to fail.
        //:
        //: 4 Verify the above 3 properties for a 'Sequence' object that is
        //:   wrapped in a 'TestDynamicType'.
        //
        // Testing:
        //   int decode(bsl::streambuf *buffer, TYPE *object);
        // --------------------------------------------------------------------

        const bslstl::StringRef D = "MyDynamicType";
        const bslstl::StringRef S = "MySequence";
        const bslstl::StringRef T = "MyTaggedValue";
            // Abbreviations for the names of 'bdlat' concept test
            // implementations, which will become the tag names of the XML they
            // generate.

        const bslstl::StringRef A0 = "attribute0";
            // Abbreviation for an attribute name.

        const bslstl::StringRef I1 = "1";
            // Abbreviation for a test value.

        typedef TestAttribute<0, attribute0Name> Attribute0;
        const Attribute0                         a0;
            // Abbreviation for a sequence attribute.

        const PlaceHolder<int>                   i_;
        const GenerateTestDynamicPlaceHolder     d_;
        const GenerateTestSequencePlaceHolder    s_;
        const GenerateTestTaggedValuePlaceHolder t_;
            // Abbreviations for function objects used to generate
            // placeholders.

        const PlaceHolder<DefaultTag>                  td_;
        const PlaceHolder<FailToManipulateSequenceTag> tf_;
            // Abbreviations for tag type place holders used to control the
            // overload resolution for 'bdlat' customization point functions
            // for 'TestTaggedValue' specializations with the corresponding
            // tag.

        const GenerateXmlElement x;
            // Abbreviation for a function object used to generate XML document
            // structures for printing.

        typedef TestCase20Row R;
            // Abbreviation for the type used to implement each row of this
            // table-based test.

        enum {
            // Abbreviations for boolean values.

            no  = false,
            yes = true
        };

        typedef TypedTestAttribute<int, Attribute0> IntAttribute0;
            // 'IntAttribute0' is an alias for a type that specifies an
            // attribute of a sequence having the type 'int', Id '0', and name
            // 'attribute0'.

        typedef TestSequence<IntAttribute0> ObjType;
            // 'ObjType' is an alias for a type that specifies a sequence
            // having one attribute, where that attribute is specified by
            // 'IntAttribute0'.  This type is prefixed with 'Obj' because it is
            // the "object" of the test, or more specifically, as an invariant
            // in the structure of the output parameter of the decoding
            // operation.

        const PlaceHolder<ObjType> obj;
            // 'obj' is a placeholder for an object of 'ObjType', used to
            // specify the type of the object to be used as the output
            // parameter of a decoding operation.

        const TestXmlElement       OBJ = x(A0, I1);
            // 'OBJ' is an abbreviation for an XML structure that always
            // successfully decodes into an object of type 'ObjType'.  This
            // object is labelled "OBJ" because it is the input-side dual to
            // "Obj".  It also characterizes the "object" of the test, meaning
            // that it is an invariant part of the structure of the input to
            // the decoding operation performed in this test.

        const bslstl::StringRef SuccessMsg = "";
            // 'SuccessMsg' is an abbreviation for the value of the
            // 'loggedMessages' attribute of a 'balxml::Decoder' after a
            // successful decoding operation.

        const bslstl::StringRef TagFailMsg =
                "STREAM.xml:1.28: Error: Unable to decode sub-element "
                "'attribute0'.\n";
            // 'TagFailMsg' is an abbreviation for the value of the
            // 'loggedMessages' attribute of a 'balxml::Decoder' after failing
            // to decode an 'attribute0' element.

        const bslstl::StringRef DynTagFailMsg =
                "STREAM.xml:1.16: Error: The object being decoded is a "
                "'DynamicType', and attempting to manipulate the object by "
                "its dynamic category returned a non-zero status.\n";
            // 'DynTagFailMsg' is an abbreviation for the value of the
            // 'loggedMessages' attribute of a 'balxml::Decoder' after failing
            // to decode an object with the 'Dynamic' 'bdlat' type category
            // due to a failure in the 'bdlat_typeCategoryManipulateSequence'
            // function.

        const TestCase20Row DATA[] = {
            //LINE VALUE PLACEHOLDER    XML     DECODING SUCCESS STATUS
            //---- ----------------- ---------- -----------------------
            R(L_  ,            obj  , x(S, OBJ), yes, SuccessMsg   ),
                // Verify that decoding into a 'Sequence' that returns zero for
                // all 'bdlat' operations succeeds.

            R(L_  , t_(td_,    obj ), x(T, OBJ), yes, SuccessMsg   ),
                // Verify the same property for a 'Sequence' wrapped in a
                // 'TestTaggedValue' that does not change the behavior of the
                // underlying sequence.

            R(L_  , t_(tf_,    obj ), x(T, OBJ), no , TagFailMsg   ),
                // Verify that, for the same sequence value, wrapping it in a
                // 'TestTaggedValue' that overrides the 'manipulateAttribute'
                // operation to return non-zero causes the decode operation to
                // fail.

            R(L_  ,         d_(obj) , x(D, OBJ), yes, SuccessMsg   ),
            R(L_  , t_(td_, d_(obj)), x(T, OBJ), yes, SuccessMsg   ),
                // Verify the above first two properties for 'DynamicType'
                // objects having a dynamic type wrapping the above 'Sequence'.

            R(L_  , t_(tf_, d_(obj)), x(T, OBJ), no , DynTagFailMsg),
                // And note that this 'TestTaggedValue' overrides the
                // 'typeCategoryManipulateSequence' operation of the
                // 'DynamicType' to return non-zero, which should also cause
                // the decode operation to fail.
        };

        const int NUM_DATA = sizeof DATA / sizeof DATA[0];

        for (int i = 0; i != NUM_DATA; ++i) {
            const TestCase20Row& ROW = DATA[i];

            ROW.runTest();
        }

      } break;
      case 19: {
        // --------------------------------------------------------------------
        // TESTING NILLABLE ELEMENT DECODING
        //   This case tests the 'balxml::Decoder::decode' operation when
        //   decoding objects that may or may not be "nullable", and may or may
        //   not have the "nillable" formatting mode applied.  In general, it
        //   is expected that absent XML tags decode to null values, and
        //   self-closing tags with the attribute 'xsi:nil="true"' decode
        //   to null values only if the corresponding attribute is marked
        //   nillable.  For backwards compatibility, it is expected that
        //   self-closing tags with no attributes also decode to null values
        //   only if the corresponding attribute is marked nillable.
        //
        // Concerns:
        //: 1 Attributes of sequence types and selections of choice types that
        //:   do not have the "nillable" formatting mode decode non-null values
        //:   from XML representations of their value.
        //:
        //: 2 Attributes of sequence types and selections of choice types that
        //:   do not have the "nillable" formatting mode decode null values
        //:   absent XML tags.
        //:
        //: 3 Attributes of sequence types and selections of choice types
        //:   that have the "nillable" formatting mode decode non-null values
        //:   from XML representations of their value.
        //:
        //: 4 Attributes of sequence types and selections of choice types
        //:   that have the "nillable" formatting mode decode null values from
        //:   absent XML tags, self-closing tags with no attributes, and
        //:   self-closing tags with the attribute 'xsi:nil="true"'.
        //:
        //: 5 Nullable types decode XML representations of their underlying
        //:   value as non-null values.
        //:
        //: 6 Nullable types decode absent tags as the null value.
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
        //: 3 For each of the above objects, verify that their XML decoded
        //:   values created by 'balxml::Decoder::decode' satisfy the 6
        //:   properties defined in the "Concerns".
        //
        // Testing:
        //   int decode(bsl::streambuf *buffer, TYPE *object);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting Decoding of Nillable Elements"
                          << "\n=====================================" << endl;

        // Abbreviations for the names of 'bdlat' concept test implementations,
        // which will become the tag names of the XML they generate.
        const bslstl::StringRef S = "MySequence";
        const bslstl::StringRef C = "MyChoice";
        const bslstl::StringRef CT = "MyCustomizedType";
        const bslstl::StringRef D = "MyDynamicType";
        const bslstl::StringRef E = "MyEnumeration";

        // Abbreviations for attribute and selection names.
        const bslstl::StringRef A0 = "attribute0";
        const bslstl::StringRef A1 = "attribute1";
        const bslstl::StringRef E0 = "enumerator0";
        const bslstl::StringRef E1 = "enumerator1";
        const bslstl::StringRef S0 = "selection0";
        const bslstl::StringRef S1 = "selection1";

        // Abbreviations for some test values.
        const int i0 = 0;
        const int i1 = 1;
        const double d0 = 1.5;

        // Abbreviations for XML-encoded representations of some test values.
        const bslstl::StringRef V0 = "0";
        const bslstl::StringRef V1 = "1";
        const bslstl::StringRef D0 = "1.5";

        // Abbreviations for function objects used to generate objects that
        // implement various 'bdlat' attribute type concepts.
        const GenerateTestArray          a;
        const GenerateTestChoice         c;
        const GenerateTestCustomizedType ct;
        const GenerateTestDynamicType    d;
        const GenerateTestEnumeration    e;
        const GenerateTestNullableValue  n;
        const GenerateTestSequence       s;

        // Abbreviations for some sequence attributes.
        typedef TestAttribute<0, attribute0Name> Attribute0;
        const Attribute0 a0;

        typedef TestAttribute<1, attribute1Name> Attribute1;
        const Attribute1 a1;

        typedef TestAttribute<0,
                              attribute0Name,
                              TestAttributeDefaults::k_DEFAULT_ANNOTATION,
                              bdlat_FormattingMode::e_NILLABLE>
            NillableAttribute0;
        const NillableAttribute0 na0;

        typedef TestAttribute<1,
                              attribute1Name,
                              TestAttributeDefaults::k_DEFAULT_ANNOTATION,
                              bdlat_FormattingMode::e_NILLABLE>
            NillableAttribute1;
        const NillableAttribute1 na1;

        // Abbreviations for some enumeration enumerators.
        typedef TestEnumerator<0, enumerator0String> Enumerator0;
        const Enumerator0 e0;

        typedef TestEnumerator<1, enumerator1String> Enumerator1;
        const Enumerator1 e1;

        // Abbreviations for some choice selections.
        typedef TestSelection<0, selection0Name> Selection0;
        const Selection0 s0;

        typedef TestSelection<1, selection1Name> Selection1;
        const Selection1 s1;

        typedef TestSelection<0,
                              selection0Name,
                              TestSelectionDefaults::k_DEFAULT_ANNOTATION,
                              bdlat_FormattingMode::e_NILLABLE>
            NillableSelection0;
        const NillableSelection0 ns0;

        typedef TestSelection<1,
                              selection1Name,
                              TestSelectionDefaults::k_DEFAULT_ANNOTATION,
                              bdlat_FormattingMode::e_NILLABLE>
            NillableSelection1;
        const NillableSelection1 ns1;

        // Abbreviation for a function object used to generate XML document
        // structures for printing.
        const GenerateXmlElement x;

        // Abbreviations for some XML attribute keys and values.
        const bslstl::StringRef Nil = "xsi:nil";
        const bslstl::StringRef T = "true";

        // Abbreviations for function objects used to generate placeholders.
        const PlaceHolder<int>                   i_;
        const PlaceHolder<double>                f_;
        const GenerateTestArrayPlaceHolder       a_;
        const GenerateTestChoicePlaceHolder      c_;
        const GenerateTestDynamicPlaceHolder     d_;
        const GenerateTestEnumerationPlaceHolder e_;
        const GenerateTestNullablePlaceHolder    n_;
        const GenerateTestSequencePlaceHolder    s_;

        // Abbreviations for possible results of a decoding operation.
        enum {
            f = false, // 0, (en/de)coding fails
            t = true,  // 1, (en/de)coding succeeds
            _ = 2      // 2, (en/de)coding succeeds, but gives different value
        };

        // An abbreviation for an XML structure that will not be used when
        // testing a particular row of the below test table.  The name is short
        // for "Not Applicable."
        const TestXmlElement NA("NA");

        // An abbreviation for the name of the type used to represent one
        // row in this table-based test.
        typedef TestCase19Row R;

        // A macro that is conditionally defined if compiling on platforms
        // where compilation is known to run into resource limitations (e.g.
        // running out of memory on IBM.)
#ifdef BSLS_PLATFORM_CMP_IBM
#define U_SKIP_DUE_TO_COMPILER_RESOURCE_LIMITATIONS
#endif

        ///Implementation Note
        ///-------------------
        // The following test table shares its structure with the table in case
        // 14 of the 'balxml_encoder' component test driver.  These two test
        // cases share an identical test table structure in order to verify
        // that, abstractly, the encoding and decoding operations they perform
        // are "consistent".  Note that the "encoding result" is unused in this
        // test driver.
        //
        // Test case rows labeled with an asterisk "*" verify that different
        // encodings of null values that may be produced by the encoder are
        // treated as representing the same value (null) by the decoder.  In
        // particular, lines with a "1" after the asterisk verify the nullness
        // of decoded values of omitted tags, lines with a "2" verify the
        // nullness of decoded values of self-closing tags, and lines with a
        // "3" verify the nullness of decoded values of self-closing tags with
        // an 'xsi:nil="true"' attribute.
        const TestCase19Row DATA[] = {
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
#endif
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
R(L_,  c(ns0, n(s_(a0, i_ )))   , t, t, x(C                  )       ), // * 1
R(L_,  c(ns0, n(s_(a0, i_ )))   , _, t, x(C,x(S0         )   )       ), // * 2
R(L_,  c(ns0, n(s_(a0, i_ )))   , _, t, x(C,x(S0,Nil,T   )   )       ), // * 3
R(L_,  c(ns0, n( s(a0, i0 )))   , t, t, x(C,x(S0,x(A0,V0))   )       ),
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
#endif
R(L_,  s(na0,  i0)              , t, t, x(S,x(A0,V0))                ),
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
#endif
R(L_,  s(na0,   c(ns0, n(i_)) ) , t, t, x(S,x(A0         ))          ), // * 1
R(L_,  s(na0,   c(ns0, n(i_)) ) , _, t, x(S,x(A0,x(S0)))             ), // * 2
R(L_,  s(na0,   c(ns0, n(i_)) ) , _, t, x(S,x(A0,x(S0,Nil,T)))       ), // * 3
R(L_,  s(na0,   c(ns0, n(i0)) ) , t, t, x(S,x(A0,x(S0,V0)))          ),
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
#endif
R(L_,  s(na0,n(e_(e0,e1  )   )) , t, t, x(S         )                ), // * 1
R(L_,  s(na0,n(e_(e0,e1  )   )) , _, t, x(S,x(A0))                   ), // * 2
R(L_,  s(na0,n(e_(e0,e1  )   )) , _, t, x(S,x(A0,Nil,T))             ), // * 3
R(L_,  s(na0,n( e(e0,e1,0)   )) , t, _, x(S,x(A0,""))                ),
R(L_,  s(na0,n( e(e0,e1,0)   )) , _, t, x(S,x(A0,E0))                ),
R(L_,  s(na0,    s(a0,i0) )     , t, t, x(S,x(A0,x(A0,V0)))          ),
R(L_,  s(na0, n(s_(a0,i_)))     , t, t, x(S               )          ), // * 1
R(L_,  s(na0, n(s_(a0,i_)))     , _, t, x(S,x(A0))                   ), // * 2
R(L_,  s(na0, n(s_(a0,i_)))     , _, t, x(S,x(A0,Nil,T))             ), // * 3
R(L_,  s(na0, n( s(a0,i0)))     , t, t, x(S,x(A0,x(A0,V0)))          ),
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

        const int NUM_DATA = sizeof DATA / sizeof DATA[0];

        for (int i = 0; i != NUM_DATA; ++i) {
            const TestCase19Row& ROW = DATA[i];

            ROW.runTest();
        }

        if (verbose) cout << "\nEnd of Test." << endl;
      } break;
      case 18: {
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

        const struct {
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

        const int NUM_DATA = sizeof DATA / sizeof DATA[0];

        for (int i = 0; i != NUM_DATA; ++i) {
            const int           LINE   = DATA[i].d_line;
            const TestXmlElement& XML    = DATA[i].d_xml;
            const char *const   EXPECTED_STRING = DATA[i].d_string;

            bdlsb::MemOutStreamBuf xmlStreamBuf;
            bsl::ostream xmlStream(&xmlStreamBuf);
            xmlStream << XML;                                           // TEST

            const bslstl::StringRef STRING(xmlStreamBuf.data(),
                                           xmlStreamBuf.length());
            ASSERTV(LINE, EXPECTED_STRING, STRING, EXPECTED_STRING == STRING);
        }

        if (verbose) cout << "\nEnd of Test." << endl;

      } break;
      case 17: {
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
      case 16: {
        // --------------------------------------------------------------------
        // EXHAUSTIVE XML DECODING TEST
        //
        // Concerns:
        //   Arbitrary complex XML data should be correctly parsed and properly
        //   decoded.
        //
        // Plan:
        //   Decode an XML element using a binding adaptor with
        //   'balxml::Decoder'.
        //
        // Testing:
        //   static bsl::istream& decode(istream&,
        //                               TYPE,
        //                               bsl::ostream&,
        //                               bsl::ostream&)
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "\nEXHAUSTIVE XML DECODING TEST"
                                  "\n----------------------------"
                               << bsl::endl;

        const bsl::string DATA[] = {
        "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
        "<Topchoice xmlns=\"TestNamespace\"\n"
        "  xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\">\n"
        "  <selection1>\n"
        "    <element3>\n"
        "      <selection1>true</selection1>\n"
        "    </element3>\n"
        "  </selection1>\n"
        "</Topchoice>\n",

        "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
        "<Topchoice xmlns=\"TestNamespace\"\n"
        "  xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\">\n"
        "  <selection1>\n"
        "    <element1>\n"
        "      <selection1>\n"
        "        <element1>0</element1>\n"
        "        <element2>custom</element2>\n"
        "        <element3>999</element3>\n"
        "        <element4>3123123123</element4>\n"
        "        <element5>0</element5>\n"
        "        <element6 xsi:nil=\"true\"/>\n"
        "        <element6 xsi:nil=\"true\"/>\n"
        "        <element7>custom</element7>\n"
        "        <element8>999</element8>\n"
        "        <element9>3123123123</element9>\n"
        "        <element10>0</element10>\n"
        "        <element10>0</element10>\n"
        "        <element11>custom</element11>\n"
        "        <element11>custom</element11>\n"
        "        <element12>3123123123</element12>\n"
        "        <element12>3123123123</element12>\n"
        "        <element13 xsi:nil=\"true\"/>\n"
        "        <element13 xsi:nil=\"true\"/>\n"
        "        <element14>999</element14>\n"
        "        <element14>999</element14>\n"
        "        <element15 xsi:nil=\"true\"/>\n"
        "        <element15 xsi:nil=\"true\"/>\n"
        "      </selection1>\n"
        "    </element1>\n"
        "    <element2>\n"
        "      <selection1>2</selection1>\n"
        "    </element2>\n"
        "    <element2>\n"
        "      <selection1>2</selection1>\n"
        "    </element2>\n"
        "    <element3>\n"
        "      <selection2>\n"
        "      </selection2>\n"
        "    </element3>\n"
        "    <element4 xsi:nil=\"true\"/>\n"
        "    <element4 xsi:nil=\"true\"/>\n"
        "    <element5>\n"
        "      <selection1>\n"
        "        <element1>0</element1>\n"
        "        <element2>custom</element2>\n"
        "        <element3>999</element3>\n"
        "        <element4>3123123123</element4>\n"
        "        <element5>0</element5>\n"
        "        <element6 xsi:nil=\"true\"/>\n"
        "        <element6 xsi:nil=\"true\"/>\n"
        "        <element7>custom</element7>\n"
        "        <element8>999</element8>\n"
        "        <element9>3123123123</element9>\n"
        "        <element10>0</element10>\n"
        "        <element10>0</element10>\n"
        "        <element11>custom</element11>\n"
        "        <element11>custom</element11>\n"
        "        <element12>3123123123</element12>\n"
        "        <element12>3123123123</element12>\n"
        "        <element13 xsi:nil=\"true\"/>\n"
        "        <element13 xsi:nil=\"true\"/>\n"
        "        <element14>999</element14>\n"
        "        <element14>999</element14>\n"
        "        <element15 xsi:nil=\"true\"/>\n"
        "        <element15 xsi:nil=\"true\"/>\n"
        "      </selection1>\n"
        "    </element5>\n"
        "    <element5>\n"
        "      <selection1>\n"
        "        <element1>0</element1>\n"
        "        <element2>custom</element2>\n"
        "        <element3>999</element3>\n"
        "        <element4>3123123123</element4>\n"
        "        <element5>0</element5>\n"
        "        <element6 xsi:nil=\"true\"/>\n"
        "        <element6 xsi:nil=\"true\"/>\n"
        "        <element7>custom</element7>\n"
        "        <element8>999</element8>\n"
        "        <element9>3123123123</element9>\n"
        "        <element10>0</element10>\n"
        "        <element10>0</element10>\n"
        "        <element11>custom</element11>\n"
        "        <element11>custom</element11>\n"
        "        <element12>3123123123</element12>\n"
        "        <element12>3123123123</element12>\n"
        "        <element13 xsi:nil=\"true\"/>\n"
        "        <element13 xsi:nil=\"true\"/>\n"
        "        <element14>999</element14>\n"
        "        <element14>999</element14>\n"
        "        <element15 xsi:nil=\"true\"/>\n"
        "        <element15 xsi:nil=\"true\"/>\n"
        "      </selection1>\n"
        "    </element5>\n"
        "  </selection1>\n"
        "</Topchoice>\n",

        "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
        "<Topchoice xmlns=\"TestNamespace\"\n"
        "  xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\">\n"
        "  <selection1>\n"
        "    <element1>\n"
        "      <selection2>255</selection2>\n"
        "    </element1>\n"
        "    <element2>\n"
        "      <selection3>\n"
        "        <element1>\n"
        "          <element1>LONDON</element1>\n"
        "          <element1>LONDON</element1>\n"
        "          <element2>arbitrary string value</element2>\n"
        "          <element2>arbitrary string value</element2>\n"
        "          <element3>true</element3>\n"
        "          <element4>arbitrary string value</element4>\n"
        "          <element5>\n"
        "            <element1>\n"
        "              <element1>LONDON</element1>\n"
        "              <element1>LONDON</element1>\n"
        "              <element2>arbitrary string value</element2>\n"
        "              <element2>arbitrary string value</element2>\n"
        "              <element3>true</element3>\n"
        "              <element4>arbitrary string value</element4>\n"
        "              <element5>\n"
        "                <element1>\n"
        "                  <element1>LONDON</element1>\n"
        "                  <element1>LONDON</element1>\n"
        "                  <element2>arbitrary string value</element2>\n"
        "                  <element2>arbitrary string value</element2>\n"
        "                  <element3>true</element3>\n"
        "                  <element4>arbitrary string value</element4>\n"
        "                  <element6 xsi:nil=\"true\"/>\n"
        "                  <element6 xsi:nil=\"true\"/>\n"
        "                </element1>\n"
        "                <element2 xsi:nil=\"true\"/>\n"
        "                <element2 xsi:nil=\"true\"/>\n"
        "                <element3 xsi:nil=\"true\"/>\n"
        "                <element3 xsi:nil=\"true\"/>\n"
        "                <element4 xsi:nil=\"true\"/>\n"
        "                <element4 xsi:nil=\"true\"/>\n"
        "                <element5 xsi:nil=\"true\"/>\n"
        "                <element5 xsi:nil=\"true\"/>\n"
        "                <element6 xsi:nil=\"true\"/>\n"
        "                <element6 xsi:nil=\"true\"/>\n"
        "              </element5>\n"
        "              <element6 xsi:nil=\"true\"/>\n"
        "              <element6 xsi:nil=\"true\"/>\n"
        "            </element1>\n"
        "            <element2 xsi:nil=\"true\"/>\n"
        "            <element2 xsi:nil=\"true\"/>\n"
        "            <element3 xsi:nil=\"true\"/>\n"
        "            <element3 xsi:nil=\"true\"/>\n"
        "            <element4 xsi:nil=\"true\"/>\n"
        "            <element4 xsi:nil=\"true\"/>\n"
        "            <element5 xsi:nil=\"true\"/>\n"
        "            <element5 xsi:nil=\"true\"/>\n"
        "            <element6 xsi:nil=\"true\"/>\n"
        "            <element6 xsi:nil=\"true\"/>\n"
        "          </element5>\n"
        "          <element6 xsi:nil=\"true\"/>\n"
        "          <element6 xsi:nil=\"true\"/>\n"
        "        </element1>\n"
        "        <element1>\n"
        "          <element1>LONDON</element1>\n"
        "          <element1>LONDON</element1>\n"
        "          <element2>arbitrary string value</element2>\n"
        "          <element2>arbitrary string value</element2>\n"
        "          <element3>true</element3>\n"
        "          <element4>arbitrary string value</element4>\n"
        "          <element5>\n"
        "            <element1>\n"
        "              <element1>LONDON</element1>\n"
        "              <element1>LONDON</element1>\n"
        "              <element2>arbitrary string value</element2>\n"
        "              <element2>arbitrary string value</element2>\n"
        "              <element3>true</element3>\n"
        "              <element4>arbitrary string value</element4>\n"
        "              <element5>\n"
        "                <element1>\n"
        "                  <element1>LONDON</element1>\n"
        "                  <element1>LONDON</element1>\n"
        "                  <element2>arbitrary string value</element2>\n"
        "                  <element2>arbitrary string value</element2>\n"
        "                  <element3>true</element3>\n"
        "                  <element4>arbitrary string value</element4>\n"
        "                  <element6 xsi:nil=\"true\"/>\n"
        "                  <element6 xsi:nil=\"true\"/>\n"
        "                </element1>\n"
        "                <element2 xsi:nil=\"true\"/>\n"
        "                <element2 xsi:nil=\"true\"/>\n"
        "                <element3 xsi:nil=\"true\"/>\n"
        "                <element3 xsi:nil=\"true\"/>\n"
        "                <element4 xsi:nil=\"true\"/>\n"
        "                <element4 xsi:nil=\"true\"/>\n"
        "                <element5 xsi:nil=\"true\"/>\n"
        "                <element5 xsi:nil=\"true\"/>\n"
        "                <element6 xsi:nil=\"true\"/>\n"
        "                <element6 xsi:nil=\"true\"/>\n"
        "              </element5>\n"
        "              <element6 xsi:nil=\"true\"/>\n"
        "              <element6 xsi:nil=\"true\"/>\n"
        "            </element1>\n"
        "            <element2 xsi:nil=\"true\"/>\n"
        "            <element2 xsi:nil=\"true\"/>\n"
        "            <element3 xsi:nil=\"true\"/>\n"
        "            <element3 xsi:nil=\"true\"/>\n"
        "            <element4 xsi:nil=\"true\"/>\n"
        "            <element4 xsi:nil=\"true\"/>\n"
        "            <element5 xsi:nil=\"true\"/>\n"
        "            <element5 xsi:nil=\"true\"/>\n"
        "            <element6 xsi:nil=\"true\"/>\n"
        "            <element6 xsi:nil=\"true\"/>\n"
        "          </element5>\n"
        "          <element6 xsi:nil=\"true\"/>\n"
        "          <element6 xsi:nil=\"true\"/>\n"
        "        </element1>\n"
        "        <element2>\n"
        "          <selection1>2</selection1>\n"
        "        </element2>\n"
        "        <element2>\n"
        "          <selection1>2</selection1>\n"
        "        </element2>\n"
        "        <element3>FF0001</element3>\n"
        "        <element4>2</element4>\n"
        "        <element5>2012-08-18T13:25:00.000+00:00</element5>\n"
        "        <element6>custom</element6>\n"
        "        <element7>LONDON</element7>\n"
        "        <element8>true</element8>\n"
        "        <element9>arbitrary string value</element9>\n"
        "        <element10>1.5</element10>\n"
        "        <element11>FF0001</element11>\n"
        "        <element12>2</element12>\n"
        "        <element13>LONDON</element13>\n"
        "        <element14>true</element14>\n"
        "        <element14>true</element14>\n"
        "        <element15>1.5</element15>\n"
        "        <element15>1.5</element15>\n"
        "        <element16>FF0001</element16>\n"
        "        <element16>FF0001</element16>\n"
        "        <element17>2</element17>\n"
        "        <element17>2</element17>\n"
        "        <element18>2012-08-18T13:25:00.000+00:00</element18>\n"
        "        <element18>2012-08-18T13:25:00.000+00:00</element18>\n"
        "        <element19>custom</element19>\n"
        "        <element19>custom</element19>\n"
        "      </selection3>\n"
        "    </element2>\n"
        "    <element2>\n"
        "      <selection3>\n"
        "        <element1>\n"
        "          <element1>LONDON</element1>\n"
        "          <element1>LONDON</element1>\n"
        "          <element2>arbitrary string value</element2>\n"
        "          <element2>arbitrary string value</element2>\n"
        "          <element3>true</element3>\n"
        "          <element4>arbitrary string value</element4>\n"
        "          <element5>\n"
        "            <element1>\n"
        "              <element1>LONDON</element1>\n"
        "              <element1>LONDON</element1>\n"
        "              <element2>arbitrary string value</element2>\n"
        "              <element2>arbitrary string value</element2>\n"
        "              <element3>true</element3>\n"
        "              <element4>arbitrary string value</element4>\n"
        "              <element5>\n"
        "                <element1>\n"
        "                  <element1>LONDON</element1>\n"
        "                  <element1>LONDON</element1>\n"
        "                  <element2>arbitrary string value</element2>\n"
        "                  <element2>arbitrary string value</element2>\n"
        "                  <element3>true</element3>\n"
        "                  <element4>arbitrary string value</element4>\n"
        "                  <element6 xsi:nil=\"true\"/>\n"
        "                  <element6 xsi:nil=\"true\"/>\n"
        "                </element1>\n"
        "                <element2 xsi:nil=\"true\"/>\n"
        "                <element2 xsi:nil=\"true\"/>\n"
        "                <element3 xsi:nil=\"true\"/>\n"
        "                <element3 xsi:nil=\"true\"/>\n"
        "                <element4 xsi:nil=\"true\"/>\n"
        "                <element4 xsi:nil=\"true\"/>\n"
        "                <element5 xsi:nil=\"true\"/>\n"
        "                <element5 xsi:nil=\"true\"/>\n"
        "                <element6 xsi:nil=\"true\"/>\n"
        "                <element6 xsi:nil=\"true\"/>\n"
        "              </element5>\n"
        "              <element6 xsi:nil=\"true\"/>\n"
        "              <element6 xsi:nil=\"true\"/>\n"
        "            </element1>\n"
        "            <element2 xsi:nil=\"true\"/>\n"
        "            <element2 xsi:nil=\"true\"/>\n"
        "            <element3 xsi:nil=\"true\"/>\n"
        "            <element3 xsi:nil=\"true\"/>\n"
        "            <element4 xsi:nil=\"true\"/>\n"
        "            <element4 xsi:nil=\"true\"/>\n"
        "            <element5 xsi:nil=\"true\"/>\n"
        "            <element5 xsi:nil=\"true\"/>\n"
        "            <element6 xsi:nil=\"true\"/>\n"
        "            <element6 xsi:nil=\"true\"/>\n"
        "          </element5>\n"
        "          <element6 xsi:nil=\"true\"/>\n"
        "          <element6 xsi:nil=\"true\"/>\n"
        "        </element1>\n"
        "        <element1>\n"
        "          <element1>LONDON</element1>\n"
        "          <element1>LONDON</element1>\n"
        "          <element2>arbitrary string value</element2>\n"
        "          <element2>arbitrary string value</element2>\n"
        "          <element3>true</element3>\n"
        "          <element4>arbitrary string value</element4>\n"
        "          <element5>\n"
        "            <element1>\n"
        "              <element1>LONDON</element1>\n"
        "              <element1>LONDON</element1>\n"
        "              <element2>arbitrary string value</element2>\n"
        "              <element2>arbitrary string value</element2>\n"
        "              <element3>true</element3>\n"
        "              <element4>arbitrary string value</element4>\n"
        "              <element5>\n"
        "                <element1>\n"
        "                  <element1>LONDON</element1>\n"
        "                  <element1>LONDON</element1>\n"
        "                  <element2>arbitrary string value</element2>\n"
        "                  <element2>arbitrary string value</element2>\n"
        "                  <element3>true</element3>\n"
        "                  <element4>arbitrary string value</element4>\n"
        "                  <element6 xsi:nil=\"true\"/>\n"
        "                  <element6 xsi:nil=\"true\"/>\n"
        "                </element1>\n"
        "                <element2 xsi:nil=\"true\"/>\n"
        "                <element2 xsi:nil=\"true\"/>\n"
        "                <element3 xsi:nil=\"true\"/>\n"
        "                <element3 xsi:nil=\"true\"/>\n"
        "                <element4 xsi:nil=\"true\"/>\n"
        "                <element4 xsi:nil=\"true\"/>\n"
        "                <element5 xsi:nil=\"true\"/>\n"
        "                <element5 xsi:nil=\"true\"/>\n"
        "                <element6 xsi:nil=\"true\"/>\n"
        "                <element6 xsi:nil=\"true\"/>\n"
        "              </element5>\n"
        "              <element6 xsi:nil=\"true\"/>\n"
        "              <element6 xsi:nil=\"true\"/>\n"
        "            </element1>\n"
        "            <element2 xsi:nil=\"true\"/>\n"
        "            <element2 xsi:nil=\"true\"/>\n"
        "            <element3 xsi:nil=\"true\"/>\n"
        "            <element3 xsi:nil=\"true\"/>\n"
        "            <element4 xsi:nil=\"true\"/>\n"
        "            <element4 xsi:nil=\"true\"/>\n"
        "            <element5 xsi:nil=\"true\"/>\n"
        "            <element5 xsi:nil=\"true\"/>\n"
        "            <element6 xsi:nil=\"true\"/>\n"
        "            <element6 xsi:nil=\"true\"/>\n"
        "          </element5>\n"
        "          <element6 xsi:nil=\"true\"/>\n"
        "          <element6 xsi:nil=\"true\"/>\n"
        "        </element1>\n"
        "        <element2>\n"
        "          <selection1>2</selection1>\n"
        "        </element2>\n"
        "        <element2>\n"
        "          <selection1>2</selection1>\n"
        "        </element2>\n"
        "        <element3>FF0001</element3>\n"
        "        <element4>2</element4>\n"
        "        <element5>2012-08-18T13:25:00.000+00:00</element5>\n"
        "        <element6>custom</element6>\n"
        "        <element7>LONDON</element7>\n"
        "        <element8>true</element8>\n"
        "        <element9>arbitrary string value</element9>\n"
        "        <element10>1.5</element10>\n"
        "        <element11>FF0001</element11>\n"
        "        <element12>2</element12>\n"
        "        <element13>LONDON</element13>\n"
        "        <element14>true</element14>\n"
        "        <element14>true</element14>\n"
        "        <element15>1.5</element15>\n"
        "        <element15>1.5</element15>\n"
        "        <element16>FF0001</element16>\n"
        "        <element16>FF0001</element16>\n"
        "        <element17>2</element17>\n"
        "        <element17>2</element17>\n"
        "        <element18>2012-08-18T13:25:00.000+00:00</element18>\n"
        "        <element18>2012-08-18T13:25:00.000+00:00</element18>\n"
        "        <element19>custom</element19>\n"
        "        <element19>custom</element19>\n"
        "      </selection3>\n"
        "    </element2>\n"
        "    <element3>\n"
        "      <selection3>\n"
        "        <selection1>2</selection1>\n"
        "      </selection3>\n"
        "    </element3>\n"
        "    <element4>\n"
        "      <selection3>\n"
        "        <element1>\n"
        "          <element1>LONDON</element1>\n"
        "          <element1>LONDON</element1>\n"
        "          <element2>arbitrary string value</element2>\n"
        "          <element2>arbitrary string value</element2>\n"
        "          <element3>true</element3>\n"
        "          <element4>arbitrary string value</element4>\n"
        "          <element5>\n"
        "            <element1>\n"
        "              <element1>LONDON</element1>\n"
        "              <element1>LONDON</element1>\n"
        "              <element2>arbitrary string value</element2>\n"
        "              <element2>arbitrary string value</element2>\n"
        "              <element3>true</element3>\n"
        "              <element4>arbitrary string value</element4>\n"
        "              <element5>\n"
        "                <element1>\n"
        "                  <element1>LONDON</element1>\n"
        "                  <element1>LONDON</element1>\n"
        "                  <element2>arbitrary string value</element2>\n"
        "                  <element2>arbitrary string value</element2>\n"
        "                  <element3>true</element3>\n"
        "                  <element4>arbitrary string value</element4>\n"
        "                  <element6 xsi:nil=\"true\"/>\n"
        "                  <element6 xsi:nil=\"true\"/>\n"
        "                </element1>\n"
        "                <element2 xsi:nil=\"true\"/>\n"
        "                <element2 xsi:nil=\"true\"/>\n"
        "                <element3 xsi:nil=\"true\"/>\n"
        "                <element3 xsi:nil=\"true\"/>\n"
        "                <element4 xsi:nil=\"true\"/>\n"
        "                <element4 xsi:nil=\"true\"/>\n"
        "                <element5 xsi:nil=\"true\"/>\n"
        "                <element5 xsi:nil=\"true\"/>\n"
        "                <element6 xsi:nil=\"true\"/>\n"
        "                <element6 xsi:nil=\"true\"/>\n"
        "              </element5>\n"
        "              <element6 xsi:nil=\"true\"/>\n"
        "              <element6 xsi:nil=\"true\"/>\n"
        "            </element1>\n"
        "            <element2 xsi:nil=\"true\"/>\n"
        "            <element2 xsi:nil=\"true\"/>\n"
        "            <element3 xsi:nil=\"true\"/>\n"
        "            <element3 xsi:nil=\"true\"/>\n"
        "            <element4 xsi:nil=\"true\"/>\n"
        "            <element4 xsi:nil=\"true\"/>\n"
        "            <element5 xsi:nil=\"true\"/>\n"
        "            <element5 xsi:nil=\"true\"/>\n"
        "            <element6 xsi:nil=\"true\"/>\n"
        "            <element6 xsi:nil=\"true\"/>\n"
        "          </element5>\n"
        "          <element6 xsi:nil=\"true\"/>\n"
        "          <element6 xsi:nil=\"true\"/>\n"
        "        </element1>\n"
        "        <element1>\n"
        "          <element1>LONDON</element1>\n"
        "          <element1>LONDON</element1>\n"
        "          <element2>arbitrary string value</element2>\n"
        "          <element2>arbitrary string value</element2>\n"
        "          <element3>true</element3>\n"
        "          <element4>arbitrary string value</element4>\n"
        "          <element5>\n"
        "            <element1>\n"
        "              <element1>LONDON</element1>\n"
        "              <element1>LONDON</element1>\n"
        "              <element2>arbitrary string value</element2>\n"
        "              <element2>arbitrary string value</element2>\n"
        "              <element3>true</element3>\n"
        "              <element4>arbitrary string value</element4>\n"
        "              <element5>\n"
        "                <element1>\n"
        "                  <element1>LONDON</element1>\n"
        "                  <element1>LONDON</element1>\n"
        "                  <element2>arbitrary string value</element2>\n"
        "                  <element2>arbitrary string value</element2>\n"
        "                  <element3>true</element3>\n"
        "                  <element4>arbitrary string value</element4>\n"
        "                  <element6 xsi:nil=\"true\"/>\n"
        "                  <element6 xsi:nil=\"true\"/>\n"
        "                </element1>\n"
        "                <element2 xsi:nil=\"true\"/>\n"
        "                <element2 xsi:nil=\"true\"/>\n"
        "                <element3 xsi:nil=\"true\"/>\n"
        "                <element3 xsi:nil=\"true\"/>\n"
        "                <element4 xsi:nil=\"true\"/>\n"
        "                <element4 xsi:nil=\"true\"/>\n"
        "                <element5 xsi:nil=\"true\"/>\n"
        "                <element5 xsi:nil=\"true\"/>\n"
        "                <element6 xsi:nil=\"true\"/>\n"
        "                <element6 xsi:nil=\"true\"/>\n"
        "              </element5>\n"
        "              <element6 xsi:nil=\"true\"/>\n"
        "              <element6 xsi:nil=\"true\"/>\n"
        "            </element1>\n"
        "            <element2 xsi:nil=\"true\"/>\n"
        "            <element2 xsi:nil=\"true\"/>\n"
        "            <element3 xsi:nil=\"true\"/>\n"
        "            <element3 xsi:nil=\"true\"/>\n"
        "            <element4 xsi:nil=\"true\"/>\n"
        "            <element4 xsi:nil=\"true\"/>\n"
        "            <element5 xsi:nil=\"true\"/>\n"
        "            <element5 xsi:nil=\"true\"/>\n"
        "            <element6 xsi:nil=\"true\"/>\n"
        "            <element6 xsi:nil=\"true\"/>\n"
        "          </element5>\n"
        "          <element6 xsi:nil=\"true\"/>\n"
        "          <element6 xsi:nil=\"true\"/>\n"
        "        </element1>\n"
        "        <element2>\n"
        "          <selection1>2</selection1>\n"
        "        </element2>\n"
        "        <element2>\n"
        "          <selection1>2</selection1>\n"
        "        </element2>\n"
        "        <element3>FF0001</element3>\n"
        "        <element4>2</element4>\n"
        "        <element5>2012-08-18T13:25:00.000+00:00</element5>\n"
        "        <element6>custom</element6>\n"
        "        <element7>LONDON</element7>\n"
        "        <element8>true</element8>\n"
        "        <element9>arbitrary string value</element9>\n"
        "        <element10>1.5</element10>\n"
        "        <element11>FF0001</element11>\n"
        "        <element12>2</element12>\n"
        "        <element13>LONDON</element13>\n"
        "        <element14>true</element14>\n"
        "        <element14>true</element14>\n"
        "        <element15>1.5</element15>\n"
        "        <element15>1.5</element15>\n"
        "        <element16>FF0001</element16>\n"
        "        <element16>FF0001</element16>\n"
        "        <element17>2</element17>\n"
        "        <element17>2</element17>\n"
        "        <element18>2012-08-18T13:25:00.000+00:00</element18>\n"
        "        <element18>2012-08-18T13:25:00.000+00:00</element18>\n"
        "        <element19>custom</element19>\n"
        "        <element19>custom</element19>\n"
        "      </selection3>\n"
        "    </element4>\n"
        "    <element4>\n"
        "      <selection3>\n"
        "        <element1>\n"
        "          <element1>LONDON</element1>\n"
        "          <element1>LONDON</element1>\n"
        "          <element2>arbitrary string value</element2>\n"
        "          <element2>arbitrary string value</element2>\n"
        "          <element3>true</element3>\n"
        "          <element4>arbitrary string value</element4>\n"
        "          <element5>\n"
        "            <element1>\n"
        "              <element1>LONDON</element1>\n"
        "              <element1>LONDON</element1>\n"
        "              <element2>arbitrary string value</element2>\n"
        "              <element2>arbitrary string value</element2>\n"
        "              <element3>true</element3>\n"
        "              <element4>arbitrary string value</element4>\n"
        "              <element5>\n"
        "                <element1>\n"
        "                  <element1>LONDON</element1>\n"
        "                  <element1>LONDON</element1>\n"
        "                  <element2>arbitrary string value</element2>\n"
        "                  <element2>arbitrary string value</element2>\n"
        "                  <element3>true</element3>\n"
        "                  <element4>arbitrary string value</element4>\n"
        "                  <element6 xsi:nil=\"true\"/>\n"
        "                  <element6 xsi:nil=\"true\"/>\n"
        "                </element1>\n"
        "                <element2 xsi:nil=\"true\"/>\n"
        "                <element2 xsi:nil=\"true\"/>\n"
        "                <element3 xsi:nil=\"true\"/>\n"
        "                <element3 xsi:nil=\"true\"/>\n"
        "                <element4 xsi:nil=\"true\"/>\n"
        "                <element4 xsi:nil=\"true\"/>\n"
        "                <element5 xsi:nil=\"true\"/>\n"
        "                <element5 xsi:nil=\"true\"/>\n"
        "                <element6 xsi:nil=\"true\"/>\n"
        "                <element6 xsi:nil=\"true\"/>\n"
        "              </element5>\n"
        "              <element6 xsi:nil=\"true\"/>\n"
        "              <element6 xsi:nil=\"true\"/>\n"
        "            </element1>\n"
        "            <element2 xsi:nil=\"true\"/>\n"
        "            <element2 xsi:nil=\"true\"/>\n"
        "            <element3 xsi:nil=\"true\"/>\n"
        "            <element3 xsi:nil=\"true\"/>\n"
        "            <element4 xsi:nil=\"true\"/>\n"
        "            <element4 xsi:nil=\"true\"/>\n"
        "            <element5 xsi:nil=\"true\"/>\n"
        "            <element5 xsi:nil=\"true\"/>\n"
        "            <element6 xsi:nil=\"true\"/>\n"
        "            <element6 xsi:nil=\"true\"/>\n"
        "          </element5>\n"
        "          <element6 xsi:nil=\"true\"/>\n"
        "          <element6 xsi:nil=\"true\"/>\n"
        "        </element1>\n"
        "        <element1>\n"
        "          <element1>LONDON</element1>\n"
        "          <element1>LONDON</element1>\n"
        "          <element2>arbitrary string value</element2>\n"
        "          <element2>arbitrary string value</element2>\n"
        "          <element3>true</element3>\n"
        "          <element4>arbitrary string value</element4>\n"
        "          <element5>\n"
        "            <element1>\n"
        "              <element1>LONDON</element1>\n"
        "              <element1>LONDON</element1>\n"
        "              <element2>arbitrary string value</element2>\n"
        "              <element2>arbitrary string value</element2>\n"
        "              <element3>true</element3>\n"
        "              <element4>arbitrary string value</element4>\n"
        "              <element5>\n"
        "                <element1>\n"
        "                  <element1>LONDON</element1>\n"
        "                  <element1>LONDON</element1>\n"
        "                  <element2>arbitrary string value</element2>\n"
        "                  <element2>arbitrary string value</element2>\n"
        "                  <element3>true</element3>\n"
        "                  <element4>arbitrary string value</element4>\n"
        "                  <element6 xsi:nil=\"true\"/>\n"
        "                  <element6 xsi:nil=\"true\"/>\n"
        "                </element1>\n"
        "                <element2 xsi:nil=\"true\"/>\n"
        "                <element2 xsi:nil=\"true\"/>\n"
        "                <element3 xsi:nil=\"true\"/>\n"
        "                <element3 xsi:nil=\"true\"/>\n"
        "                <element4 xsi:nil=\"true\"/>\n"
        "                <element4 xsi:nil=\"true\"/>\n"
        "                <element5 xsi:nil=\"true\"/>\n"
        "                <element5 xsi:nil=\"true\"/>\n"
        "                <element6 xsi:nil=\"true\"/>\n"
        "                <element6 xsi:nil=\"true\"/>\n"
        "              </element5>\n"
        "              <element6 xsi:nil=\"true\"/>\n"
        "              <element6 xsi:nil=\"true\"/>\n"
        "            </element1>\n"
        "            <element2 xsi:nil=\"true\"/>\n"
        "            <element2 xsi:nil=\"true\"/>\n"
        "            <element3 xsi:nil=\"true\"/>\n"
        "            <element3 xsi:nil=\"true\"/>\n"
        "            <element4 xsi:nil=\"true\"/>\n"
        "            <element4 xsi:nil=\"true\"/>\n"
        "            <element5 xsi:nil=\"true\"/>\n"
        "            <element5 xsi:nil=\"true\"/>\n"
        "            <element6 xsi:nil=\"true\"/>\n"
        "            <element6 xsi:nil=\"true\"/>\n"
        "          </element5>\n"
        "          <element6 xsi:nil=\"true\"/>\n"
        "          <element6 xsi:nil=\"true\"/>\n"
        "        </element1>\n"
        "        <element2>\n"
        "          <selection1>2</selection1>\n"
        "        </element2>\n"
        "        <element2>\n"
        "          <selection1>2</selection1>\n"
        "        </element2>\n"
        "        <element3>FF0001</element3>\n"
        "        <element4>2</element4>\n"
        "        <element5>2012-08-18T13:25:00.000+00:00</element5>\n"
        "        <element6>custom</element6>\n"
        "        <element7>LONDON</element7>\n"
        "        <element8>true</element8>\n"
        "        <element9>arbitrary string value</element9>\n"
        "        <element10>1.5</element10>\n"
        "        <element11>FF0001</element11>\n"
        "        <element12>2</element12>\n"
        "        <element13>LONDON</element13>\n"
        "        <element14>true</element14>\n"
        "        <element14>true</element14>\n"
        "        <element15>1.5</element15>\n"
        "        <element15>1.5</element15>\n"
        "        <element16>FF0001</element16>\n"
        "        <element16>FF0001</element16>\n"
        "        <element17>2</element17>\n"
        "        <element17>2</element17>\n"
        "        <element18>2012-08-18T13:25:00.000+00:00</element18>\n"
        "        <element18>2012-08-18T13:25:00.000+00:00</element18>\n"
        "        <element19>custom</element19>\n"
        "        <element19>custom</element19>\n"
        "      </selection3>\n"
        "    </element4>\n"
        "    <element5>\n"
        "      <selection2>255</selection2>\n"
        "    </element5>\n"
        "    <element5>\n"
        "      <selection2>255</selection2>\n"
        "    </element5>\n"
        "  </selection1>\n"
        "</Topchoice>\n",

        "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
        "<Topchoice xmlns=\"TestNamespace\"\n"
        "  xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\">\n"
        "  <selection1>\n"
        "    <element1>\n"
        "      <selection2>0</selection2>\n"
        "    </element1>\n"
        "    <element2>\n"
        "      <selection3>\n"
        "        <element1>\n"
        "          <element2>\n"
        "          </element2>\n"
        "          <element2>\n"
        "          </element2>\n"
        "          <element4>\n"
        "          </element4>\n"
        "          <element5>\n"
        "            <element1>\n"
        "              <element2>\n"
        "              </element2>\n"
        "              <element2>\n"
        "              </element2>\n"
        "              <element4>\n"
        "              </element4>\n"
        "              <element5>\n"
        "                <element1>\n"
        "                  <element2>\n"
        "                  </element2>\n"
        "                  <element2>\n"
        "                  </element2>\n"
        "                  <element4>\n"
        "                  </element4>\n"
        "                  <element6>LONDON</element6>\n"
        "                  <element6>LONDON</element6>\n"
        "                </element1>\n"
        "                <element2>true</element2>\n"
        "                <element2>true</element2>\n"
        "                <element3>1.5</element3>\n"
        "                <element3>1.5</element3>\n"
        "                <element4>FF0001</element4>\n"
        "                <element4>FF0001</element4>\n"
        "                <element5>-980123</element5>\n"
        "                <element5>-980123</element5>\n"
        "                <element6>2012-08-18T13:25:00.000+00:00</element6>\n"
        "                <element6>2012-08-18T13:25:00.000+00:00</element6>\n"
        "              </element5>\n"
        "              <element6>LONDON</element6>\n"
        "              <element6>LONDON</element6>\n"
        "            </element1>\n"
        "            <element2>true</element2>\n"
        "            <element2>true</element2>\n"
        "            <element3>1.5</element3>\n"
        "            <element3>1.5</element3>\n"
        "            <element4>FF0001</element4>\n"
        "            <element4>FF0001</element4>\n"
        "            <element5>-980123</element5>\n"
        "            <element5>-980123</element5>\n"
        "            <element6>2012-08-18T13:25:00.000+00:00</element6>\n"
        "            <element6>2012-08-18T13:25:00.000+00:00</element6>\n"
        "          </element5>\n"
        "          <element6>LONDON</element6>\n"
        "          <element6>LONDON</element6>\n"
        "        </element1>\n"
        "        <element1>\n"
        "          <element2>\n"
        "          </element2>\n"
        "          <element2>\n"
        "          </element2>\n"
        "          <element4>\n"
        "          </element4>\n"
        "          <element5>\n"
        "            <element1>\n"
        "              <element2>\n"
        "              </element2>\n"
        "              <element2>\n"
        "              </element2>\n"
        "              <element4>\n"
        "              </element4>\n"
        "              <element5>\n"
        "                <element1>\n"
        "                  <element2>\n"
        "                  </element2>\n"
        "                  <element2>\n"
        "                  </element2>\n"
        "                  <element4>\n"
        "                  </element4>\n"
        "                  <element6>LONDON</element6>\n"
        "                  <element6>LONDON</element6>\n"
        "                </element1>\n"
        "                <element2>true</element2>\n"
        "                <element2>true</element2>\n"
        "                <element3>1.5</element3>\n"
        "                <element3>1.5</element3>\n"
        "                <element4>FF0001</element4>\n"
        "                <element4>FF0001</element4>\n"
        "                <element5>-980123</element5>\n"
        "                <element5>-980123</element5>\n"
        "                <element6>2012-08-18T13:25:00.000+00:00</element6>\n"
        "                <element6>2012-08-18T13:25:00.000+00:00</element6>\n"
        "              </element5>\n"
        "              <element6>LONDON</element6>\n"
        "              <element6>LONDON</element6>\n"
        "            </element1>\n"
        "            <element2>true</element2>\n"
        "            <element2>true</element2>\n"
        "            <element3>1.5</element3>\n"
        "            <element3>1.5</element3>\n"
        "            <element4>FF0001</element4>\n"
        "            <element4>FF0001</element4>\n"
        "            <element5>-980123</element5>\n"
        "            <element5>-980123</element5>\n"
        "            <element6>2012-08-18T13:25:00.000+00:00</element6>\n"
        "            <element6>2012-08-18T13:25:00.000+00:00</element6>\n"
        "          </element5>\n"
        "          <element6>LONDON</element6>\n"
        "          <element6>LONDON</element6>\n"
        "        </element1>\n"
        "        <element2>\n"
        "          <selection2>1.5</selection2>\n"
        "        </element2>\n"
        "        <element2>\n"
        "          <selection2>1.5</selection2>\n"
        "        </element2>\n"
        "        <element4>-980123</element4>\n"
        "        <element8>true</element8>\n"
        "        <element9>\n"
        "        </element9>\n"
        "        <element10>1.5</element10>\n"
        "        <element11>FF0001</element11>\n"
        "        <element12>-980123</element12>\n"
        "        <element13>LONDON</element13>\n"
        "        <element17>-980123</element17>\n"
        "        <element17>-980123</element17>\n"
        "      </selection3>\n"
        "    </element2>\n"
        "    <element2>\n"
        "      <selection3>\n"
        "        <element1>\n"
        "          <element2>\n"
        "          </element2>\n"
        "          <element2>\n"
        "          </element2>\n"
        "          <element4>\n"
        "          </element4>\n"
        "          <element5>\n"
        "            <element1>\n"
        "              <element2>\n"
        "              </element2>\n"
        "              <element2>\n"
        "              </element2>\n"
        "              <element4>\n"
        "              </element4>\n"
        "              <element5>\n"
        "                <element1>\n"
        "                  <element2>\n"
        "                  </element2>\n"
        "                  <element2>\n"
        "                  </element2>\n"
        "                  <element4>\n"
        "                  </element4>\n"
        "                  <element6>LONDON</element6>\n"
        "                  <element6>LONDON</element6>\n"
        "                </element1>\n"
        "                <element2>true</element2>\n"
        "                <element2>true</element2>\n"
        "                <element3>1.5</element3>\n"
        "                <element3>1.5</element3>\n"
        "                <element4>FF0001</element4>\n"
        "                <element4>FF0001</element4>\n"
        "                <element5>-980123</element5>\n"
        "                <element5>-980123</element5>\n"
        "                <element6>2012-08-18T13:25:00.000+00:00</element6>\n"
        "                <element6>2012-08-18T13:25:00.000+00:00</element6>\n"
        "              </element5>\n"
        "              <element6>LONDON</element6>\n"
        "              <element6>LONDON</element6>\n"
        "            </element1>\n"
        "            <element2>true</element2>\n"
        "            <element2>true</element2>\n"
        "            <element3>1.5</element3>\n"
        "            <element3>1.5</element3>\n"
        "            <element4>FF0001</element4>\n"
        "            <element4>FF0001</element4>\n"
        "            <element5>-980123</element5>\n"
        "            <element5>-980123</element5>\n"
        "            <element6>2012-08-18T13:25:00.000+00:00</element6>\n"
        "            <element6>2012-08-18T13:25:00.000+00:00</element6>\n"
        "          </element5>\n"
        "          <element6>LONDON</element6>\n"
        "          <element6>LONDON</element6>\n"
        "        </element1>\n"
        "        <element1>\n"
        "          <element2>\n"
        "          </element2>\n"
        "          <element2>\n"
        "          </element2>\n"
        "          <element4>\n"
        "          </element4>\n"
        "          <element5>\n"
        "            <element1>\n"
        "              <element2>\n"
        "              </element2>\n"
        "              <element2>\n"
        "              </element2>\n"
        "              <element4>\n"
        "              </element4>\n"
        "              <element5>\n"
        "                <element1>\n"
        "                  <element2>\n"
        "                  </element2>\n"
        "                  <element2>\n"
        "                  </element2>\n"
        "                  <element4>\n"
        "                  </element4>\n"
        "                  <element6>LONDON</element6>\n"
        "                  <element6>LONDON</element6>\n"
        "                </element1>\n"
        "                <element2>true</element2>\n"
        "                <element2>true</element2>\n"
        "                <element3>1.5</element3>\n"
        "                <element3>1.5</element3>\n"
        "                <element4>FF0001</element4>\n"
        "                <element4>FF0001</element4>\n"
        "                <element5>-980123</element5>\n"
        "                <element5>-980123</element5>\n"
        "                <element6>2012-08-18T13:25:00.000+00:00</element6>\n"
        "                <element6>2012-08-18T13:25:00.000+00:00</element6>\n"
        "              </element5>\n"
        "              <element6>LONDON</element6>\n"
        "              <element6>LONDON</element6>\n"
        "            </element1>\n"
        "            <element2>true</element2>\n"
        "            <element2>true</element2>\n"
        "            <element3>1.5</element3>\n"
        "            <element3>1.5</element3>\n"
        "            <element4>FF0001</element4>\n"
        "            <element4>FF0001</element4>\n"
        "            <element5>-980123</element5>\n"
        "            <element5>-980123</element5>\n"
        "            <element6>2012-08-18T13:25:00.000+00:00</element6>\n"
        "            <element6>2012-08-18T13:25:00.000+00:00</element6>\n"
        "          </element5>\n"
        "          <element6>LONDON</element6>\n"
        "          <element6>LONDON</element6>\n"
        "        </element1>\n"
        "        <element2>\n"
        "          <selection2>1.5</selection2>\n"
        "        </element2>\n"
        "        <element2>\n"
        "          <selection2>1.5</selection2>\n"
        "        </element2>\n"
        "        <element4>-980123</element4>\n"
        "        <element8>true</element8>\n"
        "        <element9>\n"
        "        </element9>\n"
        "        <element10>1.5</element10>\n"
        "        <element11>FF0001</element11>\n"
        "        <element12>-980123</element12>\n"
        "        <element13>LONDON</element13>\n"
        "        <element17>-980123</element17>\n"
        "        <element17>-980123</element17>\n"
        "      </selection3>\n"
        "    </element2>\n"
        "    <element3>\n"
        "      <selection3>\n"
        "        <selection2>1.5</selection2>\n"
        "      </selection3>\n"
        "    </element3>\n"
        "    <element4>\n"
        "      <selection3>\n"
        "        <element1>\n"
        "          <element2>\n"
        "          </element2>\n"
        "          <element2>\n"
        "          </element2>\n"
        "          <element4>\n"
        "          </element4>\n"
        "          <element5>\n"
        "            <element1>\n"
        "              <element2>\n"
        "              </element2>\n"
        "              <element2>\n"
        "              </element2>\n"
        "              <element4>\n"
        "              </element4>\n"
        "              <element5>\n"
        "                <element1>\n"
        "                  <element2>\n"
        "                  </element2>\n"
        "                  <element2>\n"
        "                  </element2>\n"
        "                  <element4>\n"
        "                  </element4>\n"
        "                  <element6>LONDON</element6>\n"
        "                  <element6>LONDON</element6>\n"
        "                </element1>\n"
        "                <element2>true</element2>\n"
        "                <element2>true</element2>\n"
        "                <element3>1.5</element3>\n"
        "                <element3>1.5</element3>\n"
        "                <element4>FF0001</element4>\n"
        "                <element4>FF0001</element4>\n"
        "                <element5>-980123</element5>\n"
        "                <element5>-980123</element5>\n"
        "                <element6>2012-08-18T13:25:00.000+00:00</element6>\n"
        "                <element6>2012-08-18T13:25:00.000+00:00</element6>\n"
        "              </element5>\n"
        "              <element6>LONDON</element6>\n"
        "              <element6>LONDON</element6>\n"
        "            </element1>\n"
        "            <element2>true</element2>\n"
        "            <element2>true</element2>\n"
        "            <element3>1.5</element3>\n"
        "            <element3>1.5</element3>\n"
        "            <element4>FF0001</element4>\n"
        "            <element4>FF0001</element4>\n"
        "            <element5>-980123</element5>\n"
        "            <element5>-980123</element5>\n"
        "            <element6>2012-08-18T13:25:00.000+00:00</element6>\n"
        "            <element6>2012-08-18T13:25:00.000+00:00</element6>\n"
        "          </element5>\n"
        "          <element6>LONDON</element6>\n"
        "          <element6>LONDON</element6>\n"
        "        </element1>\n"
        "        <element1>\n"
        "          <element2>\n"
        "          </element2>\n"
        "          <element2>\n"
        "          </element2>\n"
        "          <element4>\n"
        "          </element4>\n"
        "          <element5>\n"
        "            <element1>\n"
        "              <element2>\n"
        "              </element2>\n"
        "              <element2>\n"
        "              </element2>\n"
        "              <element4>\n"
        "              </element4>\n"
        "              <element5>\n"
        "                <element1>\n"
        "                  <element2>\n"
        "                  </element2>\n"
        "                  <element2>\n"
        "                  </element2>\n"
        "                  <element4>\n"
        "                  </element4>\n"
        "                  <element6>LONDON</element6>\n"
        "                  <element6>LONDON</element6>\n"
        "                </element1>\n"
        "                <element2>true</element2>\n"
        "                <element2>true</element2>\n"
        "                <element3>1.5</element3>\n"
        "                <element3>1.5</element3>\n"
        "                <element4>FF0001</element4>\n"
        "                <element4>FF0001</element4>\n"
        "                <element5>-980123</element5>\n"
        "                <element5>-980123</element5>\n"
        "                <element6>2012-08-18T13:25:00.000+00:00</element6>\n"
        "                <element6>2012-08-18T13:25:00.000+00:00</element6>\n"
        "              </element5>\n"
        "              <element6>LONDON</element6>\n"
        "              <element6>LONDON</element6>\n"
        "            </element1>\n"
        "            <element2>true</element2>\n"
        "            <element2>true</element2>\n"
        "            <element3>1.5</element3>\n"
        "            <element3>1.5</element3>\n"
        "            <element4>FF0001</element4>\n"
        "            <element4>FF0001</element4>\n"
        "            <element5>-980123</element5>\n"
        "            <element5>-980123</element5>\n"
        "            <element6>2012-08-18T13:25:00.000+00:00</element6>\n"
        "            <element6>2012-08-18T13:25:00.000+00:00</element6>\n"
        "          </element5>\n"
        "          <element6>LONDON</element6>\n"
        "          <element6>LONDON</element6>\n"
        "        </element1>\n"
        "        <element2>\n"
        "          <selection2>1.5</selection2>\n"
        "        </element2>\n"
        "        <element2>\n"
        "          <selection2>1.5</selection2>\n"
        "        </element2>\n"
        "        <element4>-980123</element4>\n"
        "        <element8>true</element8>\n"
        "        <element9>\n"
        "        </element9>\n"
        "        <element10>1.5</element10>\n"
        "        <element11>FF0001</element11>\n"
        "        <element12>-980123</element12>\n"
        "        <element13>LONDON</element13>\n"
        "        <element17>-980123</element17>\n"
        "        <element17>-980123</element17>\n"
        "      </selection3>\n"
        "    </element4>\n"
        "    <element4>\n"
        "      <selection3>\n"
        "        <element1>\n"
        "          <element2>\n"
        "          </element2>\n"
        "          <element2>\n"
        "          </element2>\n"
        "          <element4>\n"
        "          </element4>\n"
        "          <element5>\n"
        "            <element1>\n"
        "              <element2>\n"
        "              </element2>\n"
        "              <element2>\n"
        "              </element2>\n"
        "              <element4>\n"
        "              </element4>\n"
        "              <element5>\n"
        "                <element1>\n"
        "                  <element2>\n"
        "                  </element2>\n"
        "                  <element2>\n"
        "                  </element2>\n"
        "                  <element4>\n"
        "                  </element4>\n"
        "                  <element6>LONDON</element6>\n"
        "                  <element6>LONDON</element6>\n"
        "                </element1>\n"
        "                <element2>true</element2>\n"
        "                <element2>true</element2>\n"
        "                <element3>1.5</element3>\n"
        "                <element3>1.5</element3>\n"
        "                <element4>FF0001</element4>\n"
        "                <element4>FF0001</element4>\n"
        "                <element5>-980123</element5>\n"
        "                <element5>-980123</element5>\n"
        "                <element6>2012-08-18T13:25:00.000+00:00</element6>\n"
        "                <element6>2012-08-18T13:25:00.000+00:00</element6>\n"
        "              </element5>\n"
        "              <element6>LONDON</element6>\n"
        "              <element6>LONDON</element6>\n"
        "            </element1>\n"
        "            <element2>true</element2>\n"
        "            <element2>true</element2>\n"
        "            <element3>1.5</element3>\n"
        "            <element3>1.5</element3>\n"
        "            <element4>FF0001</element4>\n"
        "            <element4>FF0001</element4>\n"
        "            <element5>-980123</element5>\n"
        "            <element5>-980123</element5>\n"
        "            <element6>2012-08-18T13:25:00.000+00:00</element6>\n"
        "            <element6>2012-08-18T13:25:00.000+00:00</element6>\n"
        "          </element5>\n"
        "          <element6>LONDON</element6>\n"
        "          <element6>LONDON</element6>\n"
        "        </element1>\n"
        "        <element1>\n"
        "          <element2>\n"
        "          </element2>\n"
        "          <element2>\n"
        "          </element2>\n"
        "          <element4>\n"
        "          </element4>\n"
        "          <element5>\n"
        "            <element1>\n"
        "              <element2>\n"
        "              </element2>\n"
        "              <element2>\n"
        "              </element2>\n"
        "              <element4>\n"
        "              </element4>\n"
        "              <element5>\n"
        "                <element1>\n"
        "                  <element2>\n"
        "                  </element2>\n"
        "                  <element2>\n"
        "                  </element2>\n"
        "                  <element4>\n"
        "                  </element4>\n"
        "                  <element6>LONDON</element6>\n"
        "                  <element6>LONDON</element6>\n"
        "                </element1>\n"
        "                <element2>true</element2>\n"
        "                <element2>true</element2>\n"
        "                <element3>1.5</element3>\n"
        "                <element3>1.5</element3>\n"
        "                <element4>FF0001</element4>\n"
        "                <element4>FF0001</element4>\n"
        "                <element5>-980123</element5>\n"
        "                <element5>-980123</element5>\n"
        "                <element6>2012-08-18T13:25:00.000+00:00</element6>\n"
        "                <element6>2012-08-18T13:25:00.000+00:00</element6>\n"
        "              </element5>\n"
        "              <element6>LONDON</element6>\n"
        "              <element6>LONDON</element6>\n"
        "            </element1>\n"
        "            <element2>true</element2>\n"
        "            <element2>true</element2>\n"
        "            <element3>1.5</element3>\n"
        "            <element3>1.5</element3>\n"
        "            <element4>FF0001</element4>\n"
        "            <element4>FF0001</element4>\n"
        "            <element5>-980123</element5>\n"
        "            <element5>-980123</element5>\n"
        "            <element6>2012-08-18T13:25:00.000+00:00</element6>\n"
        "            <element6>2012-08-18T13:25:00.000+00:00</element6>\n"
        "          </element5>\n"
        "          <element6>LONDON</element6>\n"
        "          <element6>LONDON</element6>\n"
        "        </element1>\n"
        "        <element2>\n"
        "          <selection2>1.5</selection2>\n"
        "        </element2>\n"
        "        <element2>\n"
        "          <selection2>1.5</selection2>\n"
        "        </element2>\n"
        "        <element4>-980123</element4>\n"
        "        <element8>true</element8>\n"
        "        <element9>\n"
        "        </element9>\n"
        "        <element10>1.5</element10>\n"
        "        <element11>FF0001</element11>\n"
        "        <element12>-980123</element12>\n"
        "        <element13>LONDON</element13>\n"
        "        <element17>-980123</element17>\n"
        "        <element17>-980123</element17>\n"
        "      </selection3>\n"
        "    </element4>\n"
        "    <element5>\n"
        "      <selection2>0</selection2>\n"
        "    </element5>\n"
        "    <element5>\n"
        "      <selection2>0</selection2>\n"
        "    </element5>\n"
        "  </selection1>\n"
        "</Topchoice>\n",

        "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
        "<Topchoice xmlns=\"TestNamespace\"\n"
        " xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\">\n"
        " <selection1>\n"
        "  <element1>\n"
        "   <selection2>0</selection2>\n"
        "  </element1>\n"
        "  <element2>\n"
        "   <selection3>\n"
        "    <element1>\n"
        "     <element2></element2>\n"
        "     <element2></element2>\n"
        "     <element4></element4>\n"
        "     <element5>\n"
        "      <element1>\n"
        "       <element2></element2>\n"
        "       <element2></element2>\n"
        "       <element4></element4>\n"
        "       <element5>\n"
        "        <element1>\n"
        "         <element2></element2>\n"
        "         <element2></element2>\n"
        "         <element4></element4>\n"
        "         <element6>LONDON</element6>\n"
        "         <element6>LONDON</element6>\n"
        "        </element1>\n"
        "        <element2>true</element2>\n"
        "        <element2>true</element2>\n"
        "        <element3>1.5</element3>\n"
        "        <element3>1.5</element3>\n"
        "        <element4>FF0001</element4>\n"
        "        <element4>FF0001</element4>\n"
        "        <element5>-980123</element5>\n"
        "        <element5>-980123</element5>\n"
        "        <element6>2012-08-18T13:25:00.000+00:00</element6>\n"
        "        <element6>2012-08-18T13:25:00.000+00:00</element6>\n"
        "       </element5>\n"
        "       <element6>LONDON</element6>\n"
        "       <element6>LONDON</element6>\n"
        "      </element1>\n"
        "      <element2>true</element2>\n"
        "      <element2>true</element2>\n"
        "      <element3>1.5</element3>\n"
        "      <element3>1.5</element3>\n"
        "      <element4>FF0001</element4>\n"
        "      <element4>FF0001</element4>\n"
        "      <element5>-980123</element5>\n"
        "      <element5>-980123</element5>\n"
        "      <element6>2012-08-18T13:25:00.000+00:00</element6>\n"
        "      <element6>2012-08-18T13:25:00.000+00:00</element6>\n"
        "     </element5>\n"
        "     <element6>LONDON</element6>\n"
        "     <element6>LONDON</element6>\n"
        "    </element1>\n"
        "    <element1>\n"
        "     <element2></element2>\n"
        "     <element2></element2>\n"
        "     <element4></element4>\n"
        "     <element5>\n"
        "      <element1>\n"
        "       <element2></element2>\n"
        "       <element2></element2>\n"
        "       <element4></element4>\n"
        "       <element5>\n"
        "        <element1>\n"
        "         <element2></element2>\n"
        "         <element2></element2>\n"
        "         <element4></element4>\n"
        "         <element6>LONDON</element6>\n"
        "         <element6>LONDON</element6>\n"
        "        </element1>\n"
        "        <element2>true</element2>\n"
        "        <element2>true</element2>\n"
        "        <element3>1.5</element3>\n"
        "        <element3>1.5</element3>\n"
        "        <element4>FF0001</element4>\n"
        "        <element4>FF0001</element4>\n"
        "        <element5>-980123</element5>\n"
        "        <element5>-980123</element5>\n"
        "        <element6>2012-08-18T13:25:00.000+00:00</element6>\n"
        "        <element6>2012-08-18T13:25:00.000+00:00</element6>\n"
        "       </element5>\n"
        "       <element6>LONDON</element6>\n"
        "       <element6>LONDON</element6>\n"
        "      </element1>\n"
        "      <element2>true</element2>\n"
        "      <element2>true</element2>\n"
        "      <element3>1.5</element3>\n"
        "      <element3>1.5</element3>\n"
        "      <element4>FF0001</element4>\n"
        "      <element4>FF0001</element4>\n"
        "      <element5>-980123</element5>\n"
        "      <element5>-980123</element5>\n"
        "      <element6>2012-08-18T13:25:00.000+00:00</element6>\n"
        "      <element6>2012-08-18T13:25:00.000+00:00</element6>\n"
        "     </element5>\n"
        "     <element6>LONDON</element6>\n"
        "     <element6>LONDON</element6>\n"
        "    </element1>\n"
        "    <element2>\n"
        "     <selection2>1.5</selection2>\n"
        "    </element2>\n"
        "    <element2>\n"
        "     <selection2>1.5</selection2>\n"
        "    </element2>\n"
        "    <element4>-980123</element4>\n"
        "    <element8>true</element8>\n"
        "    <element9></element9>\n"
        "    <element10>1.5</element10>\n"
        "    <element11>FF0001</element11>\n"
        "    <element12>-980123</element12>\n"
        "    <element13>LONDON</element13>\n"
        "    <element17>-980123</element17>\n"
        "    <element17>-980123</element17>\n"
        "   </selection3>\n"
        "  </element2>\n"
        "  <element2>\n"
        "   <selection3>\n"
        "    <element1>\n"
        "     <element2></element2>\n"
        "     <element2></element2>\n"
        "     <element4></element4>\n"
        "     <element5>\n"
        "      <element1>\n"
        "       <element2></element2>\n"
        "       <element2></element2>\n"
        "       <element4></element4>\n"
        "       <element5>\n"
        "        <element1>\n"
        "         <element2></element2>\n"
        "         <element2></element2>\n"
        "         <element4></element4>\n"
        "         <element6>LONDON</element6>\n"
        "         <element6>LONDON</element6>\n"
        "        </element1>\n"
        "        <element2>true</element2>\n"
        "        <element2>true</element2>\n"
        "        <element3>1.5</element3>\n"
        "        <element3>1.5</element3>\n"
        "        <element4>FF0001</element4>\n"
        "        <element4>FF0001</element4>\n"
        "        <element5>-980123</element5>\n"
        "        <element5>-980123</element5>\n"
        "        <element6>2012-08-18T13:25:00.000+00:00</element6>\n"
        "        <element6>2012-08-18T13:25:00.000+00:00</element6>\n"
        "       </element5>\n"
        "       <element6>LONDON</element6>\n"
        "       <element6>LONDON</element6>\n"
        "      </element1>\n"
        "      <element2>true</element2>\n"
        "      <element2>true</element2>\n"
        "      <element3>1.5</element3>\n"
        "      <element3>1.5</element3>\n"
        "      <element4>FF0001</element4>\n"
        "      <element4>FF0001</element4>\n"
        "      <element5>-980123</element5>\n"
        "      <element5>-980123</element5>\n"
        "      <element6>2012-08-18T13:25:00.000+00:00</element6>\n"
        "      <element6>2012-08-18T13:25:00.000+00:00</element6>\n"
        "     </element5>\n"
        "     <element6>LONDON</element6>\n"
        "     <element6>LONDON</element6>\n"
        "    </element1>\n"
        "    <element1>\n"
        "     <element2></element2>\n"
        "     <element2></element2>\n"
        "     <element4></element4>\n"
        "     <element5>\n"
        "      <element1>\n"
        "       <element2></element2>\n"
        "       <element2></element2>\n"
        "       <element4></element4>\n"
        "       <element5>\n"
        "        <element1>\n"
        "         <element2></element2>\n"
        "         <element2></element2>\n"
        "         <element4></element4>\n"
        "         <element6>LONDON</element6>\n"
        "         <element6>LONDON</element6>\n"
        "        </element1>\n"
        "        <element2>true</element2>\n"
        "        <element2>true</element2>\n"
        "        <element3>1.5</element3>\n"
        "        <element3>1.5</element3>\n"
        "        <element4>FF0001</element4>\n"
        "        <element4>FF0001</element4>\n"
        "        <element5>-980123</element5>\n"
        "        <element5>-980123</element5>\n"
        "        <element6>2012-08-18T13:25:00.000+00:00</element6>\n"
        "        <element6>2012-08-18T13:25:00.000+00:00</element6>\n"
        "       </element5>\n"
        "       <element6>LONDON</element6>\n"
        "       <element6>LONDON</element6>\n"
        "      </element1>\n"
        "      <element2>true</element2>\n"
        "      <element2>true</element2>\n"
        "      <element3>1.5</element3>\n"
        "      <element3>1.5</element3>\n"
        "      <element4>FF0001</element4>\n"
        "      <element4>FF0001</element4>\n"
        "      <element5>-980123</element5>\n"
        "      <element5>-980123</element5>\n"
        "      <element6>2012-08-18T13:25:00.000+00:00</element6>\n"
        "      <element6>2012-08-18T13:25:00.000+00:00</element6>\n"
        "     </element5>\n"
        "     <element6>LONDON</element6>\n"
        "     <element6>LONDON</element6>\n"
        "    </element1>\n"
        "    <element2>\n"
        "     <selection2>1.5</selection2>\n"
        "    </element2>\n"
        "    <element2>\n"
        "     <selection2>1.5</selection2>\n"
        "    </element2>\n"
        "    <element4>-980123</element4>\n"
        "    <element8>true</element8>\n"
        "    <element9></element9>\n"
        "    <element10>1.5</element10>\n"
        "    <element11>FF0001</element11>\n"
        "    <element12>-980123</element12>\n"
        "    <element13>LONDON</element13>\n"
        "    <element17>-980123</element17>\n"
        "    <element17>-980123</element17>\n"
        "   </selection3>\n"
        "  </element2>\n"
        "  <element3>\n"
        "   <selection3>\n"
        "    <selection2>1.5</selection2>\n"
        "   </selection3>\n"
        "  </element3>\n"
        "  <element4>\n"
        "   <selection3>\n"
        "    <element1>\n"
        "     <element2></element2>\n"
        "     <element2></element2>\n"
        "     <element4></element4>\n"
        "     <element5>\n"
        "      <element1>\n"
        "       <element2></element2>\n"
        "       <element2></element2>\n"
        "       <element4></element4>\n"
        "       <element5>\n"
        "        <element1>\n"
        "         <element2></element2>\n"
        "         <element2></element2>\n"
        "         <element4></element4>\n"
        "         <element6>LONDON</element6>\n"
        "         <element6>LONDON</element6>\n"
        "        </element1>\n"
        "        <element2>true</element2>\n"
        "        <element2>true</element2>\n"
        "        <element3>1.5</element3>\n"
        "        <element3>1.5</element3>\n"
        "        <element4>FF0001</element4>\n"
        "        <element4>FF0001</element4>\n"
        "        <element5>-980123</element5>\n"
        "        <element5>-980123</element5>\n"
        "        <element6>2012-08-18T13:25:00.000+00:00</element6>\n"
        "        <element6>2012-08-18T13:25:00.000+00:00</element6>\n"
        "       </element5>\n"
        "       <element6>LONDON</element6>\n"
        "       <element6>LONDON</element6>\n"
        "      </element1>\n"
        "      <element2>true</element2>\n"
        "      <element2>true</element2>\n"
        "      <element3>1.5</element3>\n"
        "      <element3>1.5</element3>\n"
        "      <element4>FF0001</element4>\n"
        "      <element4>FF0001</element4>\n"
        "      <element5>-980123</element5>\n"
        "      <element5>-980123</element5>\n"
        "      <element6>2012-08-18T13:25:00.000+00:00</element6>\n"
        "      <element6>2012-08-18T13:25:00.000+00:00</element6>\n"
        "     </element5>\n"
        "     <element6>LONDON</element6>\n"
        "     <element6>LONDON</element6>\n"
        "    </element1>\n"
        "    <element1>\n"
        "     <element2></element2>\n"
        "     <element2></element2>\n"
        "     <element4></element4>\n"
        "     <element5>\n"
        "      <element1>\n"
        "       <element2></element2>\n"
        "       <element2></element2>\n"
        "       <element4></element4>\n"
        "       <element5>\n"
        "        <element1>\n"
        "         <element2></element2>\n"
        "         <element2></element2>\n"
        "         <element4></element4>\n"
        "         <element6>LONDON</element6>\n"
        "         <element6>LONDON</element6>\n"
        "        </element1>\n"
        "        <element2>true</element2>\n"
        "        <element2>true</element2>\n"
        "        <element3>1.5</element3>\n"
        "        <element3>1.5</element3>\n"
        "        <element4>FF0001</element4>\n"
        "        <element4>FF0001</element4>\n"
        "        <element5>-980123</element5>\n"
        "        <element5>-980123</element5>\n"
        "        <element6>2012-08-18T13:25:00.000+00:00</element6>\n"
        "        <element6>2012-08-18T13:25:00.000+00:00</element6>\n"
        "       </element5>\n"
        "       <element6>LONDON</element6>\n"
        "       <element6>LONDON</element6>\n"
        "      </element1>\n"
        "      <element2>true</element2>\n"
        "      <element2>true</element2>\n"
        "      <element3>1.5</element3>\n"
        "      <element3>1.5</element3>\n"
        "      <element4>FF0001</element4>\n"
        "      <element4>FF0001</element4>\n"
        "      <element5>-980123</element5>\n"
        "      <element5>-980123</element5>\n"
        "      <element6>2012-08-18T13:25:00.000+00:00</element6>\n"
        "      <element6>2012-08-18T13:25:00.000+00:00</element6>\n"
        "     </element5>\n"
        "     <element6>LONDON</element6>\n"
        "     <element6>LONDON</element6>\n"
        "    </element1>\n"
        "    <element2>\n"
        "     <selection2>1.5</selection2>\n"
        "    </element2>\n"
        "    <element2>\n"
        "     <selection2>1.5</selection2>\n"
        "    </element2>\n"
        "    <element4>-980123</element4>\n"
        "    <element8>true</element8>\n"
        "    <element9></element9>\n"
        "    <element10>1.5</element10>\n"
        "    <element11>FF0001</element11>\n"
        "    <element12>-980123</element12>\n"
        "    <element13>LONDON</element13>\n"
        "    <element17>-980123</element17>\n"
        "    <element17>-980123</element17>\n"
        "   </selection3>\n"
        "  </element4>\n"
        "  <element4>\n"
        "   <selection3>\n"
        "    <element1>\n"
        "     <element2></element2>\n"
        "     <element2></element2>\n"
        "     <element4></element4>\n"
        "     <element5>\n"
        "      <element1>\n"
        "       <element2></element2>\n"
        "       <element2></element2>\n"
        "       <element4></element4>\n"
        "       <element5>\n"
        "        <element1>\n"
        "         <element2></element2>\n"
        "         <element2></element2>\n"
        "         <element4></element4>\n"
        "         <element6>LONDON</element6>\n"
        "         <element6>LONDON</element6>\n"
        "        </element1>\n"
        "        <element2>true</element2>\n"
        "        <element2>true</element2>\n"
        "        <element3>1.5</element3>\n"
        "        <element3>1.5</element3>\n"
        "        <element4>FF0001</element4>\n"
        "        <element4>FF0001</element4>\n"
        "        <element5>-980123</element5>\n"
        "        <element5>-980123</element5>\n"
        "        <element6>2012-08-18T13:25:00.000+00:00</element6>\n"
        "        <element6>2012-08-18T13:25:00.000+00:00</element6>\n"
        "       </element5>\n"
        "       <element6>LONDON</element6>\n"
        "       <element6>LONDON</element6>\n"
        "      </element1>\n"
        "      <element2>true</element2>\n"
        "      <element2>true</element2>\n"
        "      <element3>1.5</element3>\n"
        "      <element3>1.5</element3>\n"
        "      <element4>FF0001</element4>\n"
        "      <element4>FF0001</element4>\n"
        "      <element5>-980123</element5>\n"
        "      <element5>-980123</element5>\n"
        "      <element6>2012-08-18T13:25:00.000+00:00</element6>\n"
        "      <element6>2012-08-18T13:25:00.000+00:00</element6>\n"
        "     </element5>\n"
        "     <element6>LONDON</element6>\n"
        "     <element6>LONDON</element6>\n"
        "    </element1>\n"
        "    <element1>\n"
        "     <element2></element2>\n"
        "     <element2></element2>\n"
        "     <element4></element4>\n"
        "     <element5>\n"
        "      <element1>\n"
        "       <element2></element2>\n"
        "       <element2></element2>\n"
        "       <element4></element4>\n"
        "       <element5>\n"
        "        <element1>\n"
        "         <element2></element2>\n"
        "         <element2></element2>\n"
        "         <element4></element4>\n"
        "         <element6>LONDON</element6>\n"
        "         <element6>LONDON</element6>\n"
        "        </element1>\n"
        "        <element2>true</element2>\n"
        "        <element2>true</element2>\n"
        "        <element3>1.5</element3>\n"
        "        <element3>1.5</element3>\n"
        "        <element4>FF0001</element4>\n"
        "        <element4>FF0001</element4>\n"
        "        <element5>-980123</element5>\n"
        "        <element5>-980123</element5>\n"
        "        <element6>2012-08-18T13:25:00.000+00:00</element6>\n"
        "        <element6>2012-08-18T13:25:00.000+00:00</element6>\n"
        "       </element5>\n"
        "       <element6>LONDON</element6>\n"
        "       <element6>LONDON</element6>\n"
        "      </element1>\n"
        "      <element2>true</element2>\n"
        "      <element2>true</element2>\n"
        "      <element3>1.5</element3>\n"
        "      <element3>1.5</element3>\n"
        "      <element4>FF0001</element4>\n"
        "      <element4>FF0001</element4>\n"
        "      <element5>-980123</element5>\n"
        "      <element5>-980123</element5>\n"
        "      <element6>2012-08-18T13:25:00.000+00:00</element6>\n"
        "      <element6>2012-08-18T13:25:00.000+00:00</element6>\n"
        "     </element5>\n"
        "     <element6>LONDON</element6>\n"
        "     <element6>LONDON</element6>\n"
        "    </element1>\n"
        "    <element2>\n"
        "     <selection2>1.5</selection2>\n"
        "    </element2>\n"
        "    <element2>\n"
        "     <selection2>1.5</selection2>\n"
        "    </element2>\n"
        "    <element4>-980123</element4>\n"
        "    <element8>true</element8>\n"
        "    <element9></element9>\n"
        "    <element10>1.5</element10>\n"
        "    <element11>FF0001</element11>\n"
        "    <element12>-980123</element12>\n"
        "    <element13>LONDON</element13>\n"
        "    <element17>-980123</element17>\n"
        "    <element17>-980123</element17>\n"
        "   </selection3>\n"
        "  </element4>\n"
        "  <element5>\n"
        "   <selection2>0</selection2>\n"
        "  </element5>\n"
        "  <element5>\n"
        "   <selection2>0</selection2>\n"
        "  </element5>\n"
        " </selection1>\n"
        "</Topchoice>\n",

        "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
        "<Topchoice xmlns=\"TestNamespace\"\n"
        " xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\">\n"
        " <selection1>\n"
        "  <element1>\n"
        "   <selection4>999</selection4>\n"
        "  </element1>\n"
        "  <element2>\n"
        "   <selection3>\n"
        "    <element1>\n"
        "     <element2>arbitrary string value</element2>\n"
        "     <element2>arbitrary string value</element2>\n"
        "     <element4>arbitrary string value</element4>\n"
        "     <element5>\n"
        "      <element1>\n"
        "       <element2>arbitrary string value</element2>\n"
        "       <element2>arbitrary string value</element2>\n"
        "       <element4>arbitrary string value</element4>\n"
        "       <element5>\n"
        "        <element1>\n"
        "         <element2>arbitrary string value</element2>\n"
        "         <element2>arbitrary string value</element2>\n"
        "         <element4>arbitrary string value</element4>\n"
        "         <element6 xsi:nil=\"true\"/>\n"
        "         <element6 xsi:nil=\"true\"/>\n"
        "        </element1>\n"
        "        <element2 xsi:nil=\"true\"/>\n"
        "        <element2 xsi:nil=\"true\"/>\n"
        "        <element3 xsi:nil=\"true\"/>\n"
        "        <element3 xsi:nil=\"true\"/>\n"
        "        <element4 xsi:nil=\"true\"/>\n"
        "        <element4 xsi:nil=\"true\"/>\n"
        "        <element6 xsi:nil=\"true\"/>\n"
        "        <element6 xsi:nil=\"true\"/>\n"
        "       </element5>\n"
        "       <element6 xsi:nil=\"true\"/>\n"
        "       <element6 xsi:nil=\"true\"/>\n"
        "      </element1>\n"
        "      <element2 xsi:nil=\"true\"/>\n"
        "      <element2 xsi:nil=\"true\"/>\n"
        "      <element3 xsi:nil=\"true\"/>\n"
        "      <element3 xsi:nil=\"true\"/>\n"
        "      <element4 xsi:nil=\"true\"/>\n"
        "      <element4 xsi:nil=\"true\"/>\n"
        "      <element6 xsi:nil=\"true\"/>\n"
        "      <element6 xsi:nil=\"true\"/>\n"
        "     </element5>\n"
        "     <element6 xsi:nil=\"true\"/>\n"
        "     <element6 xsi:nil=\"true\"/>\n"
        "    </element1>\n"
        "    <element1>\n"
        "     <element2>arbitrary string value</element2>\n"
        "     <element2>arbitrary string value</element2>\n"
        "     <element4>arbitrary string value</element4>\n"
        "     <element5>\n"
        "      <element1>\n"
        "       <element2>arbitrary string value</element2>\n"
        "       <element2>arbitrary string value</element2>\n"
        "       <element4>arbitrary string value</element4>\n"
        "       <element5>\n"
        "        <element1>\n"
        "         <element2>arbitrary string value</element2>\n"
        "         <element2>arbitrary string value</element2>\n"
        "         <element4>arbitrary string value</element4>\n"
        "         <element6 xsi:nil=\"true\"/>\n"
        "         <element6 xsi:nil=\"true\"/>\n"
        "        </element1>\n"
        "        <element2 xsi:nil=\"true\"/>\n"
        "        <element2 xsi:nil=\"true\"/>\n"
        "        <element3 xsi:nil=\"true\"/>\n"
        "        <element3 xsi:nil=\"true\"/>\n"
        "        <element4 xsi:nil=\"true\"/>\n"
        "        <element4 xsi:nil=\"true\"/>\n"
        "        <element6 xsi:nil=\"true\"/>\n"
        "        <element6 xsi:nil=\"true\"/>\n"
        "       </element5>\n"
        "       <element6 xsi:nil=\"true\"/>\n"
        "       <element6 xsi:nil=\"true\"/>\n"
        "      </element1>\n"
        "      <element2 xsi:nil=\"true\"/>\n"
        "      <element2 xsi:nil=\"true\"/>\n"
        "      <element3 xsi:nil=\"true\"/>\n"
        "      <element3 xsi:nil=\"true\"/>\n"
        "      <element4 xsi:nil=\"true\"/>\n"
        "      <element4 xsi:nil=\"true\"/>\n"
        "      <element6 xsi:nil=\"true\"/>\n"
        "      <element6 xsi:nil=\"true\"/>\n"
        "     </element5>\n"
        "     <element6 xsi:nil=\"true\"/>\n"
        "     <element6 xsi:nil=\"true\"/>\n"
        "    </element1>\n"
        "    <element2>\n"
        "     <selection3>\n"
        "      <element1>\n"
        "       <element1>LONDON</element1>\n"
        "       <element1>LONDON</element1>\n"
        "       <element2>arbitrary string value</element2>\n"
        "       <element2>arbitrary string value</element2>\n"
        "       <element3>true</element3>\n"
        "       <element4>arbitrary string value</element4>\n"
        "       <element5>\n"
        "        <element1>\n"
        "         <element1>LONDON</element1>\n"
        "         <element1>LONDON</element1>\n"
        "         <element2>arbitrary string value</element2>\n"
        "         <element2>arbitrary string value</element2>\n"
        "         <element3>true</element3>\n"
        "         <element4>arbitrary string value</element4>\n"
        "         <element5>\n"
        "          <element1>\n"
        "           <element1>LONDON</element1>\n"
        "           <element1>LONDON</element1>\n"
        "           <element2>arbitrary string value</element2>\n"
        "           <element2>arbitrary string value</element2>\n"
        "           <element3>true</element3>\n"
        "           <element4>arbitrary string value</element4>\n"
        "           <element6 xsi:nil=\"true\"/>\n"
        "           <element6 xsi:nil=\"true\"/>\n"
        "          </element1>\n"
        "          <element2 xsi:nil=\"true\"/>\n"
        "          <element2 xsi:nil=\"true\"/>\n"
        "          <element3 xsi:nil=\"true\"/>\n"
        "          <element3 xsi:nil=\"true\"/>\n"
        "          <element4 xsi:nil=\"true\"/>\n"
        "          <element4 xsi:nil=\"true\"/>\n"
        "          <element5 xsi:nil=\"true\"/>\n"
        "          <element5 xsi:nil=\"true\"/>\n"
        "          <element6 xsi:nil=\"true\"/>\n"
        "          <element6 xsi:nil=\"true\"/>\n"
        "         </element5>\n"
        "         <element6 xsi:nil=\"true\"/>\n"
        "         <element6 xsi:nil=\"true\"/>\n"
        "        </element1>\n"
        "        <element2 xsi:nil=\"true\"/>\n"
        "        <element2 xsi:nil=\"true\"/>\n"
        "        <element3 xsi:nil=\"true\"/>\n"
        "        <element3 xsi:nil=\"true\"/>\n"
        "        <element4 xsi:nil=\"true\"/>\n"
        "        <element4 xsi:nil=\"true\"/>\n"
        "        <element5 xsi:nil=\"true\"/>\n"
        "        <element5 xsi:nil=\"true\"/>\n"
        "        <element6 xsi:nil=\"true\"/>\n"
        "        <element6 xsi:nil=\"true\"/>\n"
        "       </element5>\n"
        "       <element6 xsi:nil=\"true\"/>\n"
        "       <element6 xsi:nil=\"true\"/>\n"
        "      </element1>\n"
        "      <element1>\n"
        "       <element1>LONDON</element1>\n"
        "       <element1>LONDON</element1>\n"
        "       <element2>arbitrary string value</element2>\n"
        "       <element2>arbitrary string value</element2>\n"
        "       <element3>true</element3>\n"
        "       <element4>arbitrary string value</element4>\n"
        "       <element5>\n"
        "        <element1>\n"
        "         <element1>LONDON</element1>\n"
        "         <element1>LONDON</element1>\n"
        "         <element2>arbitrary string value</element2>\n"
        "         <element2>arbitrary string value</element2>\n"
        "         <element3>true</element3>\n"
        "         <element4>arbitrary string value</element4>\n"
        "         <element5>\n"
        "          <element1>\n"
        "           <element1>LONDON</element1>\n"
        "           <element1>LONDON</element1>\n"
        "           <element2>arbitrary string value</element2>\n"
        "           <element2>arbitrary string value</element2>\n"
        "           <element3>true</element3>\n"
        "           <element4>arbitrary string value</element4>\n"
        "           <element6 xsi:nil=\"true\"/>\n"
        "           <element6 xsi:nil=\"true\"/>\n"
        "          </element1>\n"
        "          <element2 xsi:nil=\"true\"/>\n"
        "          <element2 xsi:nil=\"true\"/>\n"
        "          <element3 xsi:nil=\"true\"/>\n"
        "          <element3 xsi:nil=\"true\"/>\n"
        "          <element4 xsi:nil=\"true\"/>\n"
        "          <element4 xsi:nil=\"true\"/>\n"
        "          <element5 xsi:nil=\"true\"/>\n"
        "          <element5 xsi:nil=\"true\"/>\n"
        "          <element6 xsi:nil=\"true\"/>\n"
        "          <element6 xsi:nil=\"true\"/>\n"
        "         </element5>\n"
        "         <element6 xsi:nil=\"true\"/>\n"
        "         <element6 xsi:nil=\"true\"/>\n"
        "        </element1>\n"
        "        <element2 xsi:nil=\"true\"/>\n"
        "        <element2 xsi:nil=\"true\"/>\n"
        "        <element3 xsi:nil=\"true\"/>\n"
        "        <element3 xsi:nil=\"true\"/>\n"
        "        <element4 xsi:nil=\"true\"/>\n"
        "        <element4 xsi:nil=\"true\"/>\n"
        "        <element5 xsi:nil=\"true\"/>\n"
        "        <element5 xsi:nil=\"true\"/>\n"
        "        <element6 xsi:nil=\"true\"/>\n"
        "        <element6 xsi:nil=\"true\"/>\n"
        "       </element5>\n"
        "       <element6 xsi:nil=\"true\"/>\n"
        "       <element6 xsi:nil=\"true\"/>\n"
        "      </element1>\n"
        "      <element2>\n"
        "       <selection1>2</selection1>\n"
        "      </element2>\n"
        "      <element2>\n"
        "       <selection1>2</selection1>\n"
        "      </element2>\n"
        "      <element3>FF0001</element3>\n"
        "      <element4>2</element4>\n"
        "      <element5>2012-08-18T13:25:00.000+00:00</element5>\n"
        "      <element6>custom</element6>\n"
        "      <element7>LONDON</element7>\n"
        "      <element8>true</element8>\n"
        "      <element9>arbitrary string value</element9>\n"
        "      <element10>1.5</element10>\n"
        "      <element11>FF0001</element11>\n"
        "      <element12>2</element12>\n"
        "      <element13>LONDON</element13>\n"
        "      <element14>true</element14>\n"
        "      <element14>true</element14>\n"
        "      <element15>1.5</element15>\n"
        "      <element15>1.5</element15>\n"
        "      <element16>FF0001</element16>\n"
        "      <element16>FF0001</element16>\n"
        "      <element17>2</element17>\n"
        "      <element17>2</element17>\n"
        "      <element18>2012-08-18T13:25:00.000+00:00</element18>\n"
        "      <element18>2012-08-18T13:25:00.000+00:00</element18>\n"
        "      <element19>custom</element19>\n"
        "      <element19>custom</element19>\n"
        "     </selection3>\n"
        "    </element2>\n"
        "    <element2>\n"
        "     <selection3>\n"
        "      <element1>\n"
        "       <element1>LONDON</element1>\n"
        "       <element1>LONDON</element1>\n"
        "       <element2>arbitrary string value</element2>\n"
        "       <element2>arbitrary string value</element2>\n"
        "       <element3>true</element3>\n"
        "       <element4>arbitrary string value</element4>\n"
        "       <element5>\n"
        "        <element1>\n"
        "         <element1>LONDON</element1>\n"
        "         <element1>LONDON</element1>\n"
        "         <element2>arbitrary string value</element2>\n"
        "         <element2>arbitrary string value</element2>\n"
        "         <element3>true</element3>\n"
        "         <element4>arbitrary string value</element4>\n"
        "         <element5>\n"
        "          <element1>\n"
        "           <element1>LONDON</element1>\n"
        "           <element1>LONDON</element1>\n"
        "           <element2>arbitrary string value</element2>\n"
        "           <element2>arbitrary string value</element2>\n"
        "           <element3>true</element3>\n"
        "           <element4>arbitrary string value</element4>\n"
        "           <element6 xsi:nil=\"true\"/>\n"
        "           <element6 xsi:nil=\"true\"/>\n"
        "          </element1>\n"
        "          <element2 xsi:nil=\"true\"/>\n"
        "          <element2 xsi:nil=\"true\"/>\n"
        "          <element3 xsi:nil=\"true\"/>\n"
        "          <element3 xsi:nil=\"true\"/>\n"
        "          <element4 xsi:nil=\"true\"/>\n"
        "          <element4 xsi:nil=\"true\"/>\n"
        "          <element5 xsi:nil=\"true\"/>\n"
        "          <element5 xsi:nil=\"true\"/>\n"
        "          <element6 xsi:nil=\"true\"/>\n"
        "          <element6 xsi:nil=\"true\"/>\n"
        "         </element5>\n"
        "         <element6 xsi:nil=\"true\"/>\n"
        "         <element6 xsi:nil=\"true\"/>\n"
        "        </element1>\n"
        "        <element2 xsi:nil=\"true\"/>\n"
        "        <element2 xsi:nil=\"true\"/>\n"
        "        <element3 xsi:nil=\"true\"/>\n"
        "        <element3 xsi:nil=\"true\"/>\n"
        "        <element4 xsi:nil=\"true\"/>\n"
        "        <element4 xsi:nil=\"true\"/>\n"
        "        <element5 xsi:nil=\"true\"/>\n"
        "        <element5 xsi:nil=\"true\"/>\n"
        "        <element6 xsi:nil=\"true\"/>\n"
        "        <element6 xsi:nil=\"true\"/>\n"
        "       </element5>\n"
        "       <element6 xsi:nil=\"true\"/>\n"
        "       <element6 xsi:nil=\"true\"/>\n"
        "      </element1>\n"
        "      <element1>\n"
        "       <element1>LONDON</element1>\n"
        "       <element1>LONDON</element1>\n"
        "       <element2>arbitrary string value</element2>\n"
        "       <element2>arbitrary string value</element2>\n"
        "       <element3>true</element3>\n"
        "       <element4>arbitrary string value</element4>\n"
        "       <element5>\n"
        "        <element1>\n"
        "         <element1>LONDON</element1>\n"
        "         <element1>LONDON</element1>\n"
        "         <element2>arbitrary string value</element2>\n"
        "         <element2>arbitrary string value</element2>\n"
        "         <element3>true</element3>\n"
        "         <element4>arbitrary string value</element4>\n"
        "         <element5>\n"
        "          <element1>\n"
        "           <element1>LONDON</element1>\n"
        "           <element1>LONDON</element1>\n"
        "           <element2>arbitrary string value</element2>\n"
        "           <element2>arbitrary string value</element2>\n"
        "           <element3>true</element3>\n"
        "           <element4>arbitrary string value</element4>\n"
        "           <element6 xsi:nil=\"true\"/>\n"
        "           <element6 xsi:nil=\"true\"/>\n"
        "          </element1>\n"
        "          <element2 xsi:nil=\"true\"/>\n"
        "          <element2 xsi:nil=\"true\"/>\n"
        "          <element3 xsi:nil=\"true\"/>\n"
        "          <element3 xsi:nil=\"true\"/>\n"
        "          <element4 xsi:nil=\"true\"/>\n"
        "          <element4 xsi:nil=\"true\"/>\n"
        "          <element5 xsi:nil=\"true\"/>\n"
        "          <element5 xsi:nil=\"true\"/>\n"
        "          <element6 xsi:nil=\"true\"/>\n"
        "          <element6 xsi:nil=\"true\"/>\n"
        "         </element5>\n"
        "         <element6 xsi:nil=\"true\"/>\n"
        "         <element6 xsi:nil=\"true\"/>\n"
        "        </element1>\n"
        "        <element2 xsi:nil=\"true\"/>\n"
        "        <element2 xsi:nil=\"true\"/>\n"
        "        <element3 xsi:nil=\"true\"/>\n"
        "        <element3 xsi:nil=\"true\"/>\n"
        "        <element4 xsi:nil=\"true\"/>\n"
        "        <element4 xsi:nil=\"true\"/>\n"
        "        <element5 xsi:nil=\"true\"/>\n"
        "        <element5 xsi:nil=\"true\"/>\n"
        "        <element6 xsi:nil=\"true\"/>\n"
        "        <element6 xsi:nil=\"true\"/>\n"
        "       </element5>\n"
        "       <element6 xsi:nil=\"true\"/>\n"
        "       <element6 xsi:nil=\"true\"/>\n"
        "      </element1>\n"
        "      <element2>\n"
        "       <selection1>2</selection1>\n"
        "      </element2>\n"
        "      <element2>\n"
        "       <selection1>2</selection1>\n"
        "      </element2>\n"
        "      <element3>FF0001</element3>\n"
        "      <element4>2</element4>\n"
        "      <element5>2012-08-18T13:25:00.000+00:00</element5>\n"
        "      <element6>custom</element6>\n"
        "      <element7>LONDON</element7>\n"
        "      <element8>true</element8>\n"
        "      <element9>arbitrary string value</element9>\n"
        "      <element10>1.5</element10>\n"
        "      <element11>FF0001</element11>\n"
        "      <element12>2</element12>\n"
        "      <element13>LONDON</element13>\n"
        "      <element14>true</element14>\n"
        "      <element14>true</element14>\n"
        "      <element15>1.5</element15>\n"
        "      <element15>1.5</element15>\n"
        "      <element16>FF0001</element16>\n"
        "      <element16>FF0001</element16>\n"
        "      <element17>2</element17>\n"
        "      <element17>2</element17>\n"
        "      <element18>2012-08-18T13:25:00.000+00:00</element18>\n"
        "      <element18>2012-08-18T13:25:00.000+00:00</element18>\n"
        "      <element19>custom</element19>\n"
        "      <element19>custom</element19>\n"
        "     </selection3>\n"
        "    </element2>\n"
        "    <element4>2</element4>\n"
        "    <element8>true</element8>\n"
        "    <element9></element9>\n"
        "    <element10>1.5</element10>\n"
        "    <element11>FF0001</element11>\n"
        "    <element12>-980123</element12>\n"
        "    <element13>LONDON</element13>\n"
        "    <element17>2</element17>\n"
        "    <element17>2</element17>\n"
        "   </selection3>\n"
        "  </element2>\n"
        "  <element2>\n"
        "   <selection3>\n"
        "    <element1>\n"
        "     <element2>arbitrary string value</element2>\n"
        "     <element2>arbitrary string value</element2>\n"
        "     <element4>arbitrary string value</element4>\n"
        "     <element5>\n"
        "      <element1>\n"
        "       <element2>arbitrary string value</element2>\n"
        "       <element2>arbitrary string value</element2>\n"
        "       <element4>arbitrary string value</element4>\n"
        "       <element5>\n"
        "        <element1>\n"
        "         <element2>arbitrary string value</element2>\n"
        "         <element2>arbitrary string value</element2>\n"
        "         <element4>arbitrary string value</element4>\n"
        "         <element6 xsi:nil=\"true\"/>\n"
        "         <element6 xsi:nil=\"true\"/>\n"
        "        </element1>\n"
        "        <element2 xsi:nil=\"true\"/>\n"
        "        <element2 xsi:nil=\"true\"/>\n"
        "        <element3 xsi:nil=\"true\"/>\n"
        "        <element3 xsi:nil=\"true\"/>\n"
        "        <element4 xsi:nil=\"true\"/>\n"
        "        <element4 xsi:nil=\"true\"/>\n"
        "        <element6 xsi:nil=\"true\"/>\n"
        "        <element6 xsi:nil=\"true\"/>\n"
        "       </element5>\n"
        "       <element6 xsi:nil=\"true\"/>\n"
        "       <element6 xsi:nil=\"true\"/>\n"
        "      </element1>\n"
        "      <element2 xsi:nil=\"true\"/>\n"
        "      <element2 xsi:nil=\"true\"/>\n"
        "      <element3 xsi:nil=\"true\"/>\n"
        "      <element3 xsi:nil=\"true\"/>\n"
        "      <element4 xsi:nil=\"true\"/>\n"
        "      <element4 xsi:nil=\"true\"/>\n"
        "      <element6 xsi:nil=\"true\"/>\n"
        "      <element6 xsi:nil=\"true\"/>\n"
        "     </element5>\n"
        "     <element6 xsi:nil=\"true\"/>\n"
        "     <element6 xsi:nil=\"true\"/>\n"
        "    </element1>\n"
        "    <element1>\n"
        "     <element2>arbitrary string value</element2>\n"
        "     <element2>arbitrary string value</element2>\n"
        "     <element4>arbitrary string value</element4>\n"
        "     <element5>\n"
        "      <element1>\n"
        "       <element2>arbitrary string value</element2>\n"
        "       <element2>arbitrary string value</element2>\n"
        "       <element4>arbitrary string value</element4>\n"
        "       <element5>\n"
        "        <element1>\n"
        "         <element2>arbitrary string value</element2>\n"
        "         <element2>arbitrary string value</element2>\n"
        "         <element4>arbitrary string value</element4>\n"
        "         <element6 xsi:nil=\"true\"/>\n"
        "         <element6 xsi:nil=\"true\"/>\n"
        "        </element1>\n"
        "        <element2 xsi:nil=\"true\"/>\n"
        "        <element2 xsi:nil=\"true\"/>\n"
        "        <element3 xsi:nil=\"true\"/>\n"
        "        <element3 xsi:nil=\"true\"/>\n"
        "        <element4 xsi:nil=\"true\"/>\n"
        "        <element4 xsi:nil=\"true\"/>\n"
        "        <element6 xsi:nil=\"true\"/>\n"
        "        <element6 xsi:nil=\"true\"/>\n"
        "       </element5>\n"
        "       <element6 xsi:nil=\"true\"/>\n"
        "       <element6 xsi:nil=\"true\"/>\n"
        "      </element1>\n"
        "      <element2 xsi:nil=\"true\"/>\n"
        "      <element2 xsi:nil=\"true\"/>\n"
        "      <element3 xsi:nil=\"true\"/>\n"
        "      <element3 xsi:nil=\"true\"/>\n"
        "      <element4 xsi:nil=\"true\"/>\n"
        "      <element4 xsi:nil=\"true\"/>\n"
        "      <element6 xsi:nil=\"true\"/>\n"
        "      <element6 xsi:nil=\"true\"/>\n"
        "     </element5>\n"
        "     <element6 xsi:nil=\"true\"/>\n"
        "     <element6 xsi:nil=\"true\"/>\n"
        "    </element1>\n"
        "    <element2>\n"
        "     <selection3>\n"
        "      <element1>\n"
        "       <element1>LONDON</element1>\n"
        "       <element1>LONDON</element1>\n"
        "       <element2>arbitrary string value</element2>\n"
        "       <element2>arbitrary string value</element2>\n"
        "       <element3>true</element3>\n"
        "       <element4>arbitrary string value</element4>\n"
        "       <element5>\n"
        "        <element1>\n"
        "         <element1>LONDON</element1>\n"
        "         <element1>LONDON</element1>\n"
        "         <element2>arbitrary string value</element2>\n"
        "         <element2>arbitrary string value</element2>\n"
        "         <element3>true</element3>\n"
        "         <element4>arbitrary string value</element4>\n"
        "         <element5>\n"
        "          <element1>\n"
        "           <element1>LONDON</element1>\n"
        "           <element1>LONDON</element1>\n"
        "           <element2>arbitrary string value</element2>\n"
        "           <element2>arbitrary string value</element2>\n"
        "           <element3>true</element3>\n"
        "           <element4>arbitrary string value</element4>\n"
        "           <element6 xsi:nil=\"true\"/>\n"
        "           <element6 xsi:nil=\"true\"/>\n"
        "          </element1>\n"
        "          <element2 xsi:nil=\"true\"/>\n"
        "          <element2 xsi:nil=\"true\"/>\n"
        "          <element3 xsi:nil=\"true\"/>\n"
        "          <element3 xsi:nil=\"true\"/>\n"
        "          <element4 xsi:nil=\"true\"/>\n"
        "          <element4 xsi:nil=\"true\"/>\n"
        "          <element5 xsi:nil=\"true\"/>\n"
        "          <element5 xsi:nil=\"true\"/>\n"
        "          <element6 xsi:nil=\"true\"/>\n"
        "          <element6 xsi:nil=\"true\"/>\n"
        "         </element5>\n"
        "         <element6 xsi:nil=\"true\"/>\n"
        "         <element6 xsi:nil=\"true\"/>\n"
        "        </element1>\n"
        "        <element2 xsi:nil=\"true\"/>\n"
        "        <element2 xsi:nil=\"true\"/>\n"
        "        <element3 xsi:nil=\"true\"/>\n"
        "        <element3 xsi:nil=\"true\"/>\n"
        "        <element4 xsi:nil=\"true\"/>\n"
        "        <element4 xsi:nil=\"true\"/>\n"
        "        <element5 xsi:nil=\"true\"/>\n"
        "        <element5 xsi:nil=\"true\"/>\n"
        "        <element6 xsi:nil=\"true\"/>\n"
        "        <element6 xsi:nil=\"true\"/>\n"
        "       </element5>\n"
        "       <element6 xsi:nil=\"true\"/>\n"
        "       <element6 xsi:nil=\"true\"/>\n"
        "      </element1>\n"
        "      <element1>\n"
        "       <element1>LONDON</element1>\n"
        "       <element1>LONDON</element1>\n"
        "       <element2>arbitrary string value</element2>\n"
        "       <element2>arbitrary string value</element2>\n"
        "       <element3>true</element3>\n"
        "       <element4>arbitrary string value</element4>\n"
        "       <element5>\n"
        "        <element1>\n"
        "         <element1>LONDON</element1>\n"
        "         <element1>LONDON</element1>\n"
        "         <element2>arbitrary string value</element2>\n"
        "         <element2>arbitrary string value</element2>\n"
        "         <element3>true</element3>\n"
        "         <element4>arbitrary string value</element4>\n"
        "         <element5>\n"
        "          <element1>\n"
        "           <element1>LONDON</element1>\n"
        "           <element1>LONDON</element1>\n"
        "           <element2>arbitrary string value</element2>\n"
        "           <element2>arbitrary string value</element2>\n"
        "           <element3>true</element3>\n"
        "           <element4>arbitrary string value</element4>\n"
        "           <element6 xsi:nil=\"true\"/>\n"
        "           <element6 xsi:nil=\"true\"/>\n"
        "          </element1>\n"
        "          <element2 xsi:nil=\"true\"/>\n"
        "          <element2 xsi:nil=\"true\"/>\n"
        "          <element3 xsi:nil=\"true\"/>\n"
        "          <element3 xsi:nil=\"true\"/>\n"
        "          <element4 xsi:nil=\"true\"/>\n"
        "          <element4 xsi:nil=\"true\"/>\n"
        "          <element5 xsi:nil=\"true\"/>\n"
        "          <element5 xsi:nil=\"true\"/>\n"
        "          <element6 xsi:nil=\"true\"/>\n"
        "          <element6 xsi:nil=\"true\"/>\n"
        "         </element5>\n"
        "         <element6 xsi:nil=\"true\"/>\n"
        "         <element6 xsi:nil=\"true\"/>\n"
        "        </element1>\n"
        "        <element2 xsi:nil=\"true\"/>\n"
        "        <element2 xsi:nil=\"true\"/>\n"
        "        <element3 xsi:nil=\"true\"/>\n"
        "        <element3 xsi:nil=\"true\"/>\n"
        "        <element4 xsi:nil=\"true\"/>\n"
        "        <element4 xsi:nil=\"true\"/>\n"
        "        <element5 xsi:nil=\"true\"/>\n"
        "        <element5 xsi:nil=\"true\"/>\n"
        "        <element6 xsi:nil=\"true\"/>\n"
        "        <element6 xsi:nil=\"true\"/>\n"
        "       </element5>\n"
        "       <element6 xsi:nil=\"true\"/>\n"
        "       <element6 xsi:nil=\"true\"/>\n"
        "      </element1>\n"
        "      <element2>\n"
        "       <selection1>2</selection1>\n"
        "      </element2>\n"
        "      <element2>\n"
        "       <selection1>2</selection1>\n"
        "      </element2>\n"
        "      <element3>FF0001</element3>\n"
        "      <element4>2</element4>\n"
        "      <element5>2012-08-18T13:25:00.000+00:00</element5>\n"
        "      <element6>custom</element6>\n"
        "      <element7>LONDON</element7>\n"
        "      <element8>true</element8>\n"
        "      <element9>arbitrary string value</element9>\n"
        "      <element10>1.5</element10>\n"
        "      <element11>FF0001</element11>\n"
        "      <element12>2</element12>\n"
        "      <element13>LONDON</element13>\n"
        "      <element14>true</element14>\n"
        "      <element14>true</element14>\n"
        "      <element15>1.5</element15>\n"
        "      <element15>1.5</element15>\n"
        "      <element16>FF0001</element16>\n"
        "      <element16>FF0001</element16>\n"
        "      <element17>2</element17>\n"
        "      <element17>2</element17>\n"
        "      <element18>2012-08-18T13:25:00.000+00:00</element18>\n"
        "      <element18>2012-08-18T13:25:00.000+00:00</element18>\n"
        "      <element19>custom</element19>\n"
        "      <element19>custom</element19>\n"
        "     </selection3>\n"
        "    </element2>\n"
        "    <element2>\n"
        "     <selection3>\n"
        "      <element1>\n"
        "       <element1>LONDON</element1>\n"
        "       <element1>LONDON</element1>\n"
        "       <element2>arbitrary string value</element2>\n"
        "       <element2>arbitrary string value</element2>\n"
        "       <element3>true</element3>\n"
        "       <element4>arbitrary string value</element4>\n"
        "       <element5>\n"
        "        <element1>\n"
        "         <element1>LONDON</element1>\n"
        "         <element1>LONDON</element1>\n"
        "         <element2>arbitrary string value</element2>\n"
        "         <element2>arbitrary string value</element2>\n"
        "         <element3>true</element3>\n"
        "         <element4>arbitrary string value</element4>\n"
        "         <element5>\n"
        "          <element1>\n"
        "           <element1>LONDON</element1>\n"
        "           <element1>LONDON</element1>\n"
        "           <element2>arbitrary string value</element2>\n"
        "           <element2>arbitrary string value</element2>\n"
        "           <element3>true</element3>\n"
        "           <element4>arbitrary string value</element4>\n"
        "           <element6 xsi:nil=\"true\"/>\n"
        "           <element6 xsi:nil=\"true\"/>\n"
        "          </element1>\n"
        "          <element2 xsi:nil=\"true\"/>\n"
        "          <element2 xsi:nil=\"true\"/>\n"
        "          <element3 xsi:nil=\"true\"/>\n"
        "          <element3 xsi:nil=\"true\"/>\n"
        "          <element4 xsi:nil=\"true\"/>\n"
        "          <element4 xsi:nil=\"true\"/>\n"
        "          <element5 xsi:nil=\"true\"/>\n"
        "          <element5 xsi:nil=\"true\"/>\n"
        "          <element6 xsi:nil=\"true\"/>\n"
        "          <element6 xsi:nil=\"true\"/>\n"
        "         </element5>\n"
        "         <element6 xsi:nil=\"true\"/>\n"
        "         <element6 xsi:nil=\"true\"/>\n"
        "        </element1>\n"
        "        <element2 xsi:nil=\"true\"/>\n"
        "        <element2 xsi:nil=\"true\"/>\n"
        "        <element3 xsi:nil=\"true\"/>\n"
        "        <element3 xsi:nil=\"true\"/>\n"
        "        <element4 xsi:nil=\"true\"/>\n"
        "        <element4 xsi:nil=\"true\"/>\n"
        "        <element5 xsi:nil=\"true\"/>\n"
        "        <element5 xsi:nil=\"true\"/>\n"
        "        <element6 xsi:nil=\"true\"/>\n"
        "        <element6 xsi:nil=\"true\"/>\n"
        "       </element5>\n"
        "       <element6 xsi:nil=\"true\"/>\n"
        "       <element6 xsi:nil=\"true\"/>\n"
        "      </element1>\n"
        "      <element1>\n"
        "       <element1>LONDON</element1>\n"
        "       <element1>LONDON</element1>\n"
        "       <element2>arbitrary string value</element2>\n"
        "       <element2>arbitrary string value</element2>\n"
        "       <element3>true</element3>\n"
        "       <element4>arbitrary string value</element4>\n"
        "       <element5>\n"
        "        <element1>\n"
        "         <element1>LONDON</element1>\n"
        "         <element1>LONDON</element1>\n"
        "         <element2>arbitrary string value</element2>\n"
        "         <element2>arbitrary string value</element2>\n"
        "         <element3>true</element3>\n"
        "         <element4>arbitrary string value</element4>\n"
        "         <element5>\n"
        "          <element1>\n"
        "           <element1>LONDON</element1>\n"
        "           <element1>LONDON</element1>\n"
        "           <element2>arbitrary string value</element2>\n"
        "           <element2>arbitrary string value</element2>\n"
        "           <element3>true</element3>\n"
        "           <element4>arbitrary string value</element4>\n"
        "           <element6 xsi:nil=\"true\"/>\n"
        "           <element6 xsi:nil=\"true\"/>\n"
        "          </element1>\n"
        "          <element2 xsi:nil=\"true\"/>\n"
        "          <element2 xsi:nil=\"true\"/>\n"
        "          <element3 xsi:nil=\"true\"/>\n"
        "          <element3 xsi:nil=\"true\"/>\n"
        "          <element4 xsi:nil=\"true\"/>\n"
        "          <element4 xsi:nil=\"true\"/>\n"
        "          <element5 xsi:nil=\"true\"/>\n"
        "          <element5 xsi:nil=\"true\"/>\n"
        "          <element6 xsi:nil=\"true\"/>\n"
        "          <element6 xsi:nil=\"true\"/>\n"
        "         </element5>\n"
        "         <element6 xsi:nil=\"true\"/>\n"
        "         <element6 xsi:nil=\"true\"/>\n"
        "        </element1>\n"
        "        <element2 xsi:nil=\"true\"/>\n"
        "        <element2 xsi:nil=\"true\"/>\n"
        "        <element3 xsi:nil=\"true\"/>\n"
        "        <element3 xsi:nil=\"true\"/>\n"
        "        <element4 xsi:nil=\"true\"/>\n"
        "        <element4 xsi:nil=\"true\"/>\n"
        "        <element5 xsi:nil=\"true\"/>\n"
        "        <element5 xsi:nil=\"true\"/>\n"
        "        <element6 xsi:nil=\"true\"/>\n"
        "        <element6 xsi:nil=\"true\"/>\n"
        "       </element5>\n"
        "       <element6 xsi:nil=\"true\"/>\n"
        "       <element6 xsi:nil=\"true\"/>\n"
        "      </element1>\n"
        "      <element2>\n"
        "       <selection1>2</selection1>\n"
        "      </element2>\n"
        "      <element2>\n"
        "       <selection1>2</selection1>\n"
        "      </element2>\n"
        "      <element3>FF0001</element3>\n"
        "      <element4>2</element4>\n"
        "      <element5>2012-08-18T13:25:00.000+00:00</element5>\n"
        "      <element6>custom</element6>\n"
        "      <element7>LONDON</element7>\n"
        "      <element8>true</element8>\n"
        "      <element9>arbitrary string value</element9>\n"
        "      <element10>1.5</element10>\n"
        "      <element11>FF0001</element11>\n"
        "      <element12>2</element12>\n"
        "      <element13>LONDON</element13>\n"
        "      <element14>true</element14>\n"
        "      <element14>true</element14>\n"
        "      <element15>1.5</element15>\n"
        "      <element15>1.5</element15>\n"
        "      <element16>FF0001</element16>\n"
        "      <element16>FF0001</element16>\n"
        "      <element17>2</element17>\n"
        "      <element17>2</element17>\n"
        "      <element18>2012-08-18T13:25:00.000+00:00</element18>\n"
        "      <element18>2012-08-18T13:25:00.000+00:00</element18>\n"
        "      <element19>custom</element19>\n"
        "      <element19>custom</element19>\n"
        "     </selection3>\n"
        "    </element2>\n"
        "    <element4>2</element4>\n"
        "    <element8>true</element8>\n"
        "    <element9></element9>\n"
        "    <element10>1.5</element10>\n"
        "    <element11>FF0001</element11>\n"
        "    <element12>-980123</element12>\n"
        "    <element13>LONDON</element13>\n"
        "    <element17>2</element17>\n"
        "    <element17>2</element17>\n"
        "   </selection3>\n"
        "  </element2>\n"
        "  <element3>\n"
        "   <selection3>\n"
        "    <selection3>\n"
        "     <element1>\n"
        "      <element1>LONDON</element1>\n"
        "      <element1>LONDON</element1>\n"
        "      <element2>arbitrary string value</element2>\n"
        "      <element2>arbitrary string value</element2>\n"
        "      <element3>true</element3>\n"
        "      <element4>arbitrary string value</element4>\n"
        "      <element5>\n"
        "       <element1>\n"
        "        <element1>LONDON</element1>\n"
        "        <element1>LONDON</element1>\n"
        "        <element2>arbitrary string value</element2>\n"
        "        <element2>arbitrary string value</element2>\n"
        "        <element3>true</element3>\n"
        "        <element4>arbitrary string value</element4>\n"
        "        <element5>\n"
        "         <element1>\n"
        "          <element1>LONDON</element1>\n"
        "          <element1>LONDON</element1>\n"
        "          <element2>arbitrary string value</element2>\n"
        "          <element2>arbitrary string value</element2>\n"
        "          <element3>true</element3>\n"
        "          <element4>arbitrary string value</element4>\n"
        "          <element6 xsi:nil=\"true\"/>\n"
        "          <element6 xsi:nil=\"true\"/>\n"
        "         </element1>\n"
        "         <element2 xsi:nil=\"true\"/>\n"
        "         <element2 xsi:nil=\"true\"/>\n"
        "         <element3 xsi:nil=\"true\"/>\n"
        "         <element3 xsi:nil=\"true\"/>\n"
        "         <element4 xsi:nil=\"true\"/>\n"
        "         <element4 xsi:nil=\"true\"/>\n"
        "         <element5 xsi:nil=\"true\"/>\n"
        "         <element5 xsi:nil=\"true\"/>\n"
        "         <element6 xsi:nil=\"true\"/>\n"
        "         <element6 xsi:nil=\"true\"/>\n"
        "        </element5>\n"
        "        <element6 xsi:nil=\"true\"/>\n"
        "        <element6 xsi:nil=\"true\"/>\n"
        "       </element1>\n"
        "       <element2 xsi:nil=\"true\"/>\n"
        "       <element2 xsi:nil=\"true\"/>\n"
        "       <element3 xsi:nil=\"true\"/>\n"
        "       <element3 xsi:nil=\"true\"/>\n"
        "       <element4 xsi:nil=\"true\"/>\n"
        "       <element4 xsi:nil=\"true\"/>\n"
        "       <element5 xsi:nil=\"true\"/>\n"
        "       <element5 xsi:nil=\"true\"/>\n"
        "       <element6 xsi:nil=\"true\"/>\n"
        "       <element6 xsi:nil=\"true\"/>\n"
        "      </element5>\n"
        "      <element6 xsi:nil=\"true\"/>\n"
        "      <element6 xsi:nil=\"true\"/>\n"
        "     </element1>\n"
        "     <element1>\n"
        "      <element1>LONDON</element1>\n"
        "      <element1>LONDON</element1>\n"
        "      <element2>arbitrary string value</element2>\n"
        "      <element2>arbitrary string value</element2>\n"
        "      <element3>true</element3>\n"
        "      <element4>arbitrary string value</element4>\n"
        "      <element5>\n"
        "       <element1>\n"
        "        <element1>LONDON</element1>\n"
        "        <element1>LONDON</element1>\n"
        "        <element2>arbitrary string value</element2>\n"
        "        <element2>arbitrary string value</element2>\n"
        "        <element3>true</element3>\n"
        "        <element4>arbitrary string value</element4>\n"
        "        <element5>\n"
        "         <element1>\n"
        "          <element1>LONDON</element1>\n"
        "          <element1>LONDON</element1>\n"
        "          <element2>arbitrary string value</element2>\n"
        "          <element2>arbitrary string value</element2>\n"
        "          <element3>true</element3>\n"
        "          <element4>arbitrary string value</element4>\n"
        "          <element6 xsi:nil=\"true\"/>\n"
        "          <element6 xsi:nil=\"true\"/>\n"
        "         </element1>\n"
        "         <element2 xsi:nil=\"true\"/>\n"
        "         <element2 xsi:nil=\"true\"/>\n"
        "         <element3 xsi:nil=\"true\"/>\n"
        "         <element3 xsi:nil=\"true\"/>\n"
        "         <element4 xsi:nil=\"true\"/>\n"
        "         <element4 xsi:nil=\"true\"/>\n"
        "         <element5 xsi:nil=\"true\"/>\n"
        "         <element5 xsi:nil=\"true\"/>\n"
        "         <element6 xsi:nil=\"true\"/>\n"
        "         <element6 xsi:nil=\"true\"/>\n"
        "        </element5>\n"
        "        <element6 xsi:nil=\"true\"/>\n"
        "        <element6 xsi:nil=\"true\"/>\n"
        "       </element1>\n"
        "       <element2 xsi:nil=\"true\"/>\n"
        "       <element2 xsi:nil=\"true\"/>\n"
        "       <element3 xsi:nil=\"true\"/>\n"
        "       <element3 xsi:nil=\"true\"/>\n"
        "       <element4 xsi:nil=\"true\"/>\n"
        "       <element4 xsi:nil=\"true\"/>\n"
        "       <element5 xsi:nil=\"true\"/>\n"
        "       <element5 xsi:nil=\"true\"/>\n"
        "       <element6 xsi:nil=\"true\"/>\n"
        "       <element6 xsi:nil=\"true\"/>\n"
        "      </element5>\n"
        "      <element6 xsi:nil=\"true\"/>\n"
        "      <element6 xsi:nil=\"true\"/>\n"
        "     </element1>\n"
        "     <element2>\n"
        "      <selection1>2</selection1>\n"
        "     </element2>\n"
        "     <element2>\n"
        "      <selection1>2</selection1>\n"
        "     </element2>\n"
        "     <element3>FF0001</element3>\n"
        "     <element4>2</element4>\n"
        "     <element5>2012-08-18T13:25:00.000+00:00</element5>\n"
        "     <element6>custom</element6>\n"
        "     <element7>LONDON</element7>\n"
        "     <element8>true</element8>\n"
        "     <element9>arbitrary string value</element9>\n"
        "     <element10>1.5</element10>\n"
        "     <element11>FF0001</element11>\n"
        "     <element12>2</element12>\n"
        "     <element13>LONDON</element13>\n"
        "     <element14>true</element14>\n"
        "     <element14>true</element14>\n"
        "     <element15>1.5</element15>\n"
        "     <element15>1.5</element15>\n"
        "     <element16>FF0001</element16>\n"
        "     <element16>FF0001</element16>\n"
        "     <element17>2</element17>\n"
        "     <element17>2</element17>\n"
        "     <element18>2012-08-18T13:25:00.000+00:00</element18>\n"
        "     <element18>2012-08-18T13:25:00.000+00:00</element18>\n"
        "     <element19>custom</element19>\n"
        "     <element19>custom</element19>\n"
        "    </selection3>\n"
        "   </selection3>\n"
        "  </element3>\n"
        "  <element4>\n"
        "   <selection3>\n"
        "    <element1>\n"
        "     <element2>arbitrary string value</element2>\n"
        "     <element2>arbitrary string value</element2>\n"
        "     <element4>arbitrary string value</element4>\n"
        "     <element5>\n"
        "      <element1>\n"
        "       <element2>arbitrary string value</element2>\n"
        "       <element2>arbitrary string value</element2>\n"
        "       <element4>arbitrary string value</element4>\n"
        "       <element5>\n"
        "        <element1>\n"
        "         <element2>arbitrary string value</element2>\n"
        "         <element2>arbitrary string value</element2>\n"
        "         <element4>arbitrary string value</element4>\n"
        "         <element6 xsi:nil=\"true\"/>\n"
        "         <element6 xsi:nil=\"true\"/>\n"
        "        </element1>\n"
        "        <element2 xsi:nil=\"true\"/>\n"
        "        <element2 xsi:nil=\"true\"/>\n"
        "        <element3 xsi:nil=\"true\"/>\n"
        "        <element3 xsi:nil=\"true\"/>\n"
        "        <element4 xsi:nil=\"true\"/>\n"
        "        <element4 xsi:nil=\"true\"/>\n"
        "        <element6 xsi:nil=\"true\"/>\n"
        "        <element6 xsi:nil=\"true\"/>\n"
        "       </element5>\n"
        "       <element6 xsi:nil=\"true\"/>\n"
        "       <element6 xsi:nil=\"true\"/>\n"
        "      </element1>\n"
        "      <element2 xsi:nil=\"true\"/>\n"
        "      <element2 xsi:nil=\"true\"/>\n"
        "      <element3 xsi:nil=\"true\"/>\n"
        "      <element3 xsi:nil=\"true\"/>\n"
        "      <element4 xsi:nil=\"true\"/>\n"
        "      <element4 xsi:nil=\"true\"/>\n"
        "      <element6 xsi:nil=\"true\"/>\n"
        "      <element6 xsi:nil=\"true\"/>\n"
        "     </element5>\n"
        "     <element6 xsi:nil=\"true\"/>\n"
        "     <element6 xsi:nil=\"true\"/>\n"
        "    </element1>\n"
        "    <element1>\n"
        "     <element2>arbitrary string value</element2>\n"
        "     <element2>arbitrary string value</element2>\n"
        "     <element4>arbitrary string value</element4>\n"
        "     <element5>\n"
        "      <element1>\n"
        "       <element2>arbitrary string value</element2>\n"
        "       <element2>arbitrary string value</element2>\n"
        "       <element4>arbitrary string value</element4>\n"
        "       <element5>\n"
        "        <element1>\n"
        "         <element2>arbitrary string value</element2>\n"
        "         <element2>arbitrary string value</element2>\n"
        "         <element4>arbitrary string value</element4>\n"
        "         <element6 xsi:nil=\"true\"/>\n"
        "         <element6 xsi:nil=\"true\"/>\n"
        "        </element1>\n"
        "        <element2 xsi:nil=\"true\"/>\n"
        "        <element2 xsi:nil=\"true\"/>\n"
        "        <element3 xsi:nil=\"true\"/>\n"
        "        <element3 xsi:nil=\"true\"/>\n"
        "        <element4 xsi:nil=\"true\"/>\n"
        "        <element4 xsi:nil=\"true\"/>\n"
        "        <element6 xsi:nil=\"true\"/>\n"
        "        <element6 xsi:nil=\"true\"/>\n"
        "       </element5>\n"
        "       <element6 xsi:nil=\"true\"/>\n"
        "       <element6 xsi:nil=\"true\"/>\n"
        "      </element1>\n"
        "      <element2 xsi:nil=\"true\"/>\n"
        "      <element2 xsi:nil=\"true\"/>\n"
        "      <element3 xsi:nil=\"true\"/>\n"
        "      <element3 xsi:nil=\"true\"/>\n"
        "      <element4 xsi:nil=\"true\"/>\n"
        "      <element4 xsi:nil=\"true\"/>\n"
        "      <element6 xsi:nil=\"true\"/>\n"
        "      <element6 xsi:nil=\"true\"/>\n"
        "     </element5>\n"
        "     <element6 xsi:nil=\"true\"/>\n"
        "     <element6 xsi:nil=\"true\"/>\n"
        "    </element1>\n"
        "    <element2>\n"
        "     <selection3>\n"
        "      <element1>\n"
        "       <element1>LONDON</element1>\n"
        "       <element1>LONDON</element1>\n"
        "       <element2>arbitrary string value</element2>\n"
        "       <element2>arbitrary string value</element2>\n"
        "       <element3>true</element3>\n"
        "       <element4>arbitrary string value</element4>\n"
        "       <element5>\n"
        "        <element1>\n"
        "         <element1>LONDON</element1>\n"
        "         <element1>LONDON</element1>\n"
        "         <element2>arbitrary string value</element2>\n"
        "         <element2>arbitrary string value</element2>\n"
        "         <element3>true</element3>\n"
        "         <element4>arbitrary string value</element4>\n"
        "         <element5>\n"
        "          <element1>\n"
        "           <element1>LONDON</element1>\n"
        "           <element1>LONDON</element1>\n"
        "           <element2>arbitrary string value</element2>\n"
        "           <element2>arbitrary string value</element2>\n"
        "           <element3>true</element3>\n"
        "           <element4>arbitrary string value</element4>\n"
        "           <element6 xsi:nil=\"true\"/>\n"
        "           <element6 xsi:nil=\"true\"/>\n"
        "          </element1>\n"
        "          <element2 xsi:nil=\"true\"/>\n"
        "          <element2 xsi:nil=\"true\"/>\n"
        "          <element3 xsi:nil=\"true\"/>\n"
        "          <element3 xsi:nil=\"true\"/>\n"
        "          <element4 xsi:nil=\"true\"/>\n"
        "          <element4 xsi:nil=\"true\"/>\n"
        "          <element5 xsi:nil=\"true\"/>\n"
        "          <element5 xsi:nil=\"true\"/>\n"
        "          <element6 xsi:nil=\"true\"/>\n"
        "          <element6 xsi:nil=\"true\"/>\n"
        "         </element5>\n"
        "         <element6 xsi:nil=\"true\"/>\n"
        "         <element6 xsi:nil=\"true\"/>\n"
        "        </element1>\n"
        "        <element2 xsi:nil=\"true\"/>\n"
        "        <element2 xsi:nil=\"true\"/>\n"
        "        <element3 xsi:nil=\"true\"/>\n"
        "        <element3 xsi:nil=\"true\"/>\n"
        "        <element4 xsi:nil=\"true\"/>\n"
        "        <element4 xsi:nil=\"true\"/>\n"
        "        <element5 xsi:nil=\"true\"/>\n"
        "        <element5 xsi:nil=\"true\"/>\n"
        "        <element6 xsi:nil=\"true\"/>\n"
        "        <element6 xsi:nil=\"true\"/>\n"
        "       </element5>\n"
        "       <element6 xsi:nil=\"true\"/>\n"
        "       <element6 xsi:nil=\"true\"/>\n"
        "      </element1>\n"
        "      <element1>\n"
        "       <element1>LONDON</element1>\n"
        "       <element1>LONDON</element1>\n"
        "       <element2>arbitrary string value</element2>\n"
        "       <element2>arbitrary string value</element2>\n"
        "       <element3>true</element3>\n"
        "       <element4>arbitrary string value</element4>\n"
        "       <element5>\n"
        "        <element1>\n"
        "         <element1>LONDON</element1>\n"
        "         <element1>LONDON</element1>\n"
        "         <element2>arbitrary string value</element2>\n"
        "         <element2>arbitrary string value</element2>\n"
        "         <element3>true</element3>\n"
        "         <element4>arbitrary string value</element4>\n"
        "         <element5>\n"
        "          <element1>\n"
        "           <element1>LONDON</element1>\n"
        "           <element1>LONDON</element1>\n"
        "           <element2>arbitrary string value</element2>\n"
        "           <element2>arbitrary string value</element2>\n"
        "           <element3>true</element3>\n"
        "           <element4>arbitrary string value</element4>\n"
        "           <element6 xsi:nil=\"true\"/>\n"
        "           <element6 xsi:nil=\"true\"/>\n"
        "          </element1>\n"
        "          <element2 xsi:nil=\"true\"/>\n"
        "          <element2 xsi:nil=\"true\"/>\n"
        "          <element3 xsi:nil=\"true\"/>\n"
        "          <element3 xsi:nil=\"true\"/>\n"
        "          <element4 xsi:nil=\"true\"/>\n"
        "          <element4 xsi:nil=\"true\"/>\n"
        "          <element5 xsi:nil=\"true\"/>\n"
        "          <element5 xsi:nil=\"true\"/>\n"
        "          <element6 xsi:nil=\"true\"/>\n"
        "          <element6 xsi:nil=\"true\"/>\n"
        "         </element5>\n"
        "         <element6 xsi:nil=\"true\"/>\n"
        "         <element6 xsi:nil=\"true\"/>\n"
        "        </element1>\n"
        "        <element2 xsi:nil=\"true\"/>\n"
        "        <element2 xsi:nil=\"true\"/>\n"
        "        <element3 xsi:nil=\"true\"/>\n"
        "        <element3 xsi:nil=\"true\"/>\n"
        "        <element4 xsi:nil=\"true\"/>\n"
        "        <element4 xsi:nil=\"true\"/>\n"
        "        <element5 xsi:nil=\"true\"/>\n"
        "        <element5 xsi:nil=\"true\"/>\n"
        "        <element6 xsi:nil=\"true\"/>\n"
        "        <element6 xsi:nil=\"true\"/>\n"
        "       </element5>\n"
        "       <element6 xsi:nil=\"true\"/>\n"
        "       <element6 xsi:nil=\"true\"/>\n"
        "      </element1>\n"
        "      <element2>\n"
        "       <selection1>2</selection1>\n"
        "      </element2>\n"
        "      <element2>\n"
        "       <selection1>2</selection1>\n"
        "      </element2>\n"
        "      <element3>FF0001</element3>\n"
        "      <element4>2</element4>\n"
        "      <element5>2012-08-18T13:25:00.000+00:00</element5>\n"
        "      <element6>custom</element6>\n"
        "      <element7>LONDON</element7>\n"
        "      <element8>true</element8>\n"
        "      <element9>arbitrary string value</element9>\n"
        "      <element10>1.5</element10>\n"
        "      <element11>FF0001</element11>\n"
        "      <element12>2</element12>\n"
        "      <element13>LONDON</element13>\n"
        "      <element14>true</element14>\n"
        "      <element14>true</element14>\n"
        "      <element15>1.5</element15>\n"
        "      <element15>1.5</element15>\n"
        "      <element16>FF0001</element16>\n"
        "      <element16>FF0001</element16>\n"
        "      <element17>2</element17>\n"
        "      <element17>2</element17>\n"
        "      <element18>2012-08-18T13:25:00.000+00:00</element18>\n"
        "      <element18>2012-08-18T13:25:00.000+00:00</element18>\n"
        "      <element19>custom</element19>\n"
        "      <element19>custom</element19>\n"
        "     </selection3>\n"
        "    </element2>\n"
        "    <element2>\n"
        "     <selection3>\n"
        "      <element1>\n"
        "       <element1>LONDON</element1>\n"
        "       <element1>LONDON</element1>\n"
        "       <element2>arbitrary string value</element2>\n"
        "       <element2>arbitrary string value</element2>\n"
        "       <element3>true</element3>\n"
        "       <element4>arbitrary string value</element4>\n"
        "       <element5>\n"
        "        <element1>\n"
        "         <element1>LONDON</element1>\n"
        "         <element1>LONDON</element1>\n"
        "         <element2>arbitrary string value</element2>\n"
        "         <element2>arbitrary string value</element2>\n"
        "         <element3>true</element3>\n"
        "         <element4>arbitrary string value</element4>\n"
        "         <element5>\n"
        "          <element1>\n"
        "           <element1>LONDON</element1>\n"
        "           <element1>LONDON</element1>\n"
        "           <element2>arbitrary string value</element2>\n"
        "           <element2>arbitrary string value</element2>\n"
        "           <element3>true</element3>\n"
        "           <element4>arbitrary string value</element4>\n"
        "           <element6 xsi:nil=\"true\"/>\n"
        "           <element6 xsi:nil=\"true\"/>\n"
        "          </element1>\n"
        "          <element2 xsi:nil=\"true\"/>\n"
        "          <element2 xsi:nil=\"true\"/>\n"
        "          <element3 xsi:nil=\"true\"/>\n"
        "          <element3 xsi:nil=\"true\"/>\n"
        "          <element4 xsi:nil=\"true\"/>\n"
        "          <element4 xsi:nil=\"true\"/>\n"
        "          <element5 xsi:nil=\"true\"/>\n"
        "          <element5 xsi:nil=\"true\"/>\n"
        "          <element6 xsi:nil=\"true\"/>\n"
        "          <element6 xsi:nil=\"true\"/>\n"
        "         </element5>\n"
        "         <element6 xsi:nil=\"true\"/>\n"
        "         <element6 xsi:nil=\"true\"/>\n"
        "        </element1>\n"
        "        <element2 xsi:nil=\"true\"/>\n"
        "        <element2 xsi:nil=\"true\"/>\n"
        "        <element3 xsi:nil=\"true\"/>\n"
        "        <element3 xsi:nil=\"true\"/>\n"
        "        <element4 xsi:nil=\"true\"/>\n"
        "        <element4 xsi:nil=\"true\"/>\n"
        "        <element5 xsi:nil=\"true\"/>\n"
        "        <element5 xsi:nil=\"true\"/>\n"
        "        <element6 xsi:nil=\"true\"/>\n"
        "        <element6 xsi:nil=\"true\"/>\n"
        "       </element5>\n"
        "       <element6 xsi:nil=\"true\"/>\n"
        "       <element6 xsi:nil=\"true\"/>\n"
        "      </element1>\n"
        "      <element1>\n"
        "       <element1>LONDON</element1>\n"
        "       <element1>LONDON</element1>\n"
        "       <element2>arbitrary string value</element2>\n"
        "       <element2>arbitrary string value</element2>\n"
        "       <element3>true</element3>\n"
        "       <element4>arbitrary string value</element4>\n"
        "       <element5>\n"
        "        <element1>\n"
        "         <element1>LONDON</element1>\n"
        "         <element1>LONDON</element1>\n"
        "         <element2>arbitrary string value</element2>\n"
        "         <element2>arbitrary string value</element2>\n"
        "         <element3>true</element3>\n"
        "         <element4>arbitrary string value</element4>\n"
        "         <element5>\n"
        "          <element1>\n"
        "           <element1>LONDON</element1>\n"
        "           <element1>LONDON</element1>\n"
        "           <element2>arbitrary string value</element2>\n"
        "           <element2>arbitrary string value</element2>\n"
        "           <element3>true</element3>\n"
        "           <element4>arbitrary string value</element4>\n"
        "           <element6 xsi:nil=\"true\"/>\n"
        "           <element6 xsi:nil=\"true\"/>\n"
        "          </element1>\n"
        "          <element2 xsi:nil=\"true\"/>\n"
        "          <element2 xsi:nil=\"true\"/>\n"
        "          <element3 xsi:nil=\"true\"/>\n"
        "          <element3 xsi:nil=\"true\"/>\n"
        "          <element4 xsi:nil=\"true\"/>\n"
        "          <element4 xsi:nil=\"true\"/>\n"
        "          <element5 xsi:nil=\"true\"/>\n"
        "          <element5 xsi:nil=\"true\"/>\n"
        "          <element6 xsi:nil=\"true\"/>\n"
        "          <element6 xsi:nil=\"true\"/>\n"
        "         </element5>\n"
        "         <element6 xsi:nil=\"true\"/>\n"
        "         <element6 xsi:nil=\"true\"/>\n"
        "        </element1>\n"
        "        <element2 xsi:nil=\"true\"/>\n"
        "        <element2 xsi:nil=\"true\"/>\n"
        "        <element3 xsi:nil=\"true\"/>\n"
        "        <element3 xsi:nil=\"true\"/>\n"
        "        <element4 xsi:nil=\"true\"/>\n"
        "        <element4 xsi:nil=\"true\"/>\n"
        "        <element5 xsi:nil=\"true\"/>\n"
        "        <element5 xsi:nil=\"true\"/>\n"
        "        <element6 xsi:nil=\"true\"/>\n"
        "        <element6 xsi:nil=\"true\"/>\n"
        "       </element5>\n"
        "       <element6 xsi:nil=\"true\"/>\n"
        "       <element6 xsi:nil=\"true\"/>\n"
        "      </element1>\n"
        "      <element2>\n"
        "       <selection1>2</selection1>\n"
        "      </element2>\n"
        "      <element2>\n"
        "       <selection1>2</selection1>\n"
        "      </element2>\n"
        "      <element3>FF0001</element3>\n"
        "      <element4>2</element4>\n"
        "      <element5>2012-08-18T13:25:00.000+00:00</element5>\n"
        "      <element6>custom</element6>\n"
        "      <element7>LONDON</element7>\n"
        "      <element8>true</element8>\n"
        "      <element9>arbitrary string value</element9>\n"
        "      <element10>1.5</element10>\n"
        "      <element11>FF0001</element11>\n"
        "      <element12>2</element12>\n"
        "      <element13>LONDON</element13>\n"
        "      <element14>true</element14>\n"
        "      <element14>true</element14>\n"
        "      <element15>1.5</element15>\n"
        "      <element15>1.5</element15>\n"
        "      <element16>FF0001</element16>\n"
        "      <element16>FF0001</element16>\n"
        "      <element17>2</element17>\n"
        "      <element17>2</element17>\n"
        "      <element18>2012-08-18T13:25:00.000+00:00</element18>\n"
        "      <element18>2012-08-18T13:25:00.000+00:00</element18>\n"
        "      <element19>custom</element19>\n"
        "      <element19>custom</element19>\n"
        "     </selection3>\n"
        "    </element2>\n"
        "    <element4>2</element4>\n"
        "    <element8>true</element8>\n"
        "    <element9></element9>\n"
        "    <element10>1.5</element10>\n"
        "    <element11>FF0001</element11>\n"
        "    <element12>-980123</element12>\n"
        "    <element13>LONDON</element13>\n"
        "    <element17>2</element17>\n"
        "    <element17>2</element17>\n"
        "   </selection3>\n"
        "  </element4>\n"
        "  <element4>\n"
        "   <selection3>\n"
        "    <element1>\n"
        "     <element2>arbitrary string value</element2>\n"
        "     <element2>arbitrary string value</element2>\n"
        "     <element4>arbitrary string value</element4>\n"
        "     <element5>\n"
        "      <element1>\n"
        "       <element2>arbitrary string value</element2>\n"
        "       <element2>arbitrary string value</element2>\n"
        "       <element4>arbitrary string value</element4>\n"
        "       <element5>\n"
        "        <element1>\n"
        "         <element2>arbitrary string value</element2>\n"
        "         <element2>arbitrary string value</element2>\n"
        "         <element4>arbitrary string value</element4>\n"
        "         <element6 xsi:nil=\"true\"/>\n"
        "         <element6 xsi:nil=\"true\"/>\n"
        "        </element1>\n"
        "        <element2 xsi:nil=\"true\"/>\n"
        "        <element2 xsi:nil=\"true\"/>\n"
        "        <element3 xsi:nil=\"true\"/>\n"
        "        <element3 xsi:nil=\"true\"/>\n"
        "        <element4 xsi:nil=\"true\"/>\n"
        "        <element4 xsi:nil=\"true\"/>\n"
        "        <element6 xsi:nil=\"true\"/>\n"
        "        <element6 xsi:nil=\"true\"/>\n"
        "       </element5>\n"
        "       <element6 xsi:nil=\"true\"/>\n"
        "       <element6 xsi:nil=\"true\"/>\n"
        "      </element1>\n"
        "      <element2 xsi:nil=\"true\"/>\n"
        "      <element2 xsi:nil=\"true\"/>\n"
        "      <element3 xsi:nil=\"true\"/>\n"
        "      <element3 xsi:nil=\"true\"/>\n"
        "      <element4 xsi:nil=\"true\"/>\n"
        "      <element4 xsi:nil=\"true\"/>\n"
        "      <element6 xsi:nil=\"true\"/>\n"
        "      <element6 xsi:nil=\"true\"/>\n"
        "     </element5>\n"
        "     <element6 xsi:nil=\"true\"/>\n"
        "     <element6 xsi:nil=\"true\"/>\n"
        "    </element1>\n"
        "    <element1>\n"
        "     <element2>arbitrary string value</element2>\n"
        "     <element2>arbitrary string value</element2>\n"
        "     <element4>arbitrary string value</element4>\n"
        "     <element5>\n"
        "      <element1>\n"
        "       <element2>arbitrary string value</element2>\n"
        "       <element2>arbitrary string value</element2>\n"
        "       <element4>arbitrary string value</element4>\n"
        "       <element5>\n"
        "        <element1>\n"
        "         <element2>arbitrary string value</element2>\n"
        "         <element2>arbitrary string value</element2>\n"
        "         <element4>arbitrary string value</element4>\n"
        "         <element6 xsi:nil=\"true\"/>\n"
        "         <element6 xsi:nil=\"true\"/>\n"
        "        </element1>\n"
        "        <element2 xsi:nil=\"true\"/>\n"
        "        <element2 xsi:nil=\"true\"/>\n"
        "        <element3 xsi:nil=\"true\"/>\n"
        "        <element3 xsi:nil=\"true\"/>\n"
        "        <element4 xsi:nil=\"true\"/>\n"
        "        <element4 xsi:nil=\"true\"/>\n"
        "        <element6 xsi:nil=\"true\"/>\n"
        "        <element6 xsi:nil=\"true\"/>\n"
        "       </element5>\n"
        "       <element6 xsi:nil=\"true\"/>\n"
        "       <element6 xsi:nil=\"true\"/>\n"
        "      </element1>\n"
        "      <element2 xsi:nil=\"true\"/>\n"
        "      <element2 xsi:nil=\"true\"/>\n"
        "      <element3 xsi:nil=\"true\"/>\n"
        "      <element3 xsi:nil=\"true\"/>\n"
        "      <element4 xsi:nil=\"true\"/>\n"
        "      <element4 xsi:nil=\"true\"/>\n"
        "      <element6 xsi:nil=\"true\"/>\n"
        "      <element6 xsi:nil=\"true\"/>\n"
        "     </element5>\n"
        "     <element6 xsi:nil=\"true\"/>\n"
        "     <element6 xsi:nil=\"true\"/>\n"
        "    </element1>\n"
        "    <element2>\n"
        "     <selection3>\n"
        "      <element1>\n"
        "       <element1>LONDON</element1>\n"
        "       <element1>LONDON</element1>\n"
        "       <element2>arbitrary string value</element2>\n"
        "       <element2>arbitrary string value</element2>\n"
        "       <element3>true</element3>\n"
        "       <element4>arbitrary string value</element4>\n"
        "       <element5>\n"
        "        <element1>\n"
        "         <element1>LONDON</element1>\n"
        "         <element1>LONDON</element1>\n"
        "         <element2>arbitrary string value</element2>\n"
        "         <element2>arbitrary string value</element2>\n"
        "         <element3>true</element3>\n"
        "         <element4>arbitrary string value</element4>\n"
        "         <element5>\n"
        "          <element1>\n"
        "           <element1>LONDON</element1>\n"
        "           <element1>LONDON</element1>\n"
        "           <element2>arbitrary string value</element2>\n"
        "           <element2>arbitrary string value</element2>\n"
        "           <element3>true</element3>\n"
        "           <element4>arbitrary string value</element4>\n"
        "           <element6 xsi:nil=\"true\"/>\n"
        "           <element6 xsi:nil=\"true\"/>\n"
        "          </element1>\n"
        "          <element2 xsi:nil=\"true\"/>\n"
        "          <element2 xsi:nil=\"true\"/>\n"
        "          <element3 xsi:nil=\"true\"/>\n"
        "          <element3 xsi:nil=\"true\"/>\n"
        "          <element4 xsi:nil=\"true\"/>\n"
        "          <element4 xsi:nil=\"true\"/>\n"
        "          <element5 xsi:nil=\"true\"/>\n"
        "          <element5 xsi:nil=\"true\"/>\n"
        "          <element6 xsi:nil=\"true\"/>\n"
        "          <element6 xsi:nil=\"true\"/>\n"
        "         </element5>\n"
        "         <element6 xsi:nil=\"true\"/>\n"
        "         <element6 xsi:nil=\"true\"/>\n"
        "        </element1>\n"
        "        <element2 xsi:nil=\"true\"/>\n"
        "        <element2 xsi:nil=\"true\"/>\n"
        "        <element3 xsi:nil=\"true\"/>\n"
        "        <element3 xsi:nil=\"true\"/>\n"
        "        <element4 xsi:nil=\"true\"/>\n"
        "        <element4 xsi:nil=\"true\"/>\n"
        "        <element5 xsi:nil=\"true\"/>\n"
        "        <element5 xsi:nil=\"true\"/>\n"
        "        <element6 xsi:nil=\"true\"/>\n"
        "        <element6 xsi:nil=\"true\"/>\n"
        "       </element5>\n"
        "       <element6 xsi:nil=\"true\"/>\n"
        "       <element6 xsi:nil=\"true\"/>\n"
        "      </element1>\n"
        "      <element1>\n"
        "       <element1>LONDON</element1>\n"
        "       <element1>LONDON</element1>\n"
        "       <element2>arbitrary string value</element2>\n"
        "       <element2>arbitrary string value</element2>\n"
        "       <element3>true</element3>\n"
        "       <element4>arbitrary string value</element4>\n"
        "       <element5>\n"
        "        <element1>\n"
        "         <element1>LONDON</element1>\n"
        "         <element1>LONDON</element1>\n"
        "         <element2>arbitrary string value</element2>\n"
        "         <element2>arbitrary string value</element2>\n"
        "         <element3>true</element3>\n"
        "         <element4>arbitrary string value</element4>\n"
        "         <element5>\n"
        "          <element1>\n"
        "           <element1>LONDON</element1>\n"
        "           <element1>LONDON</element1>\n"
        "           <element2>arbitrary string value</element2>\n"
        "           <element2>arbitrary string value</element2>\n"
        "           <element3>true</element3>\n"
        "           <element4>arbitrary string value</element4>\n"
        "           <element6 xsi:nil=\"true\"/>\n"
        "           <element6 xsi:nil=\"true\"/>\n"
        "          </element1>\n"
        "          <element2 xsi:nil=\"true\"/>\n"
        "          <element2 xsi:nil=\"true\"/>\n"
        "          <element3 xsi:nil=\"true\"/>\n"
        "          <element3 xsi:nil=\"true\"/>\n"
        "          <element4 xsi:nil=\"true\"/>\n"
        "          <element4 xsi:nil=\"true\"/>\n"
        "          <element5 xsi:nil=\"true\"/>\n"
        "          <element5 xsi:nil=\"true\"/>\n"
        "          <element6 xsi:nil=\"true\"/>\n"
        "          <element6 xsi:nil=\"true\"/>\n"
        "         </element5>\n"
        "         <element6 xsi:nil=\"true\"/>\n"
        "         <element6 xsi:nil=\"true\"/>\n"
        "        </element1>\n"
        "        <element2 xsi:nil=\"true\"/>\n"
        "        <element2 xsi:nil=\"true\"/>\n"
        "        <element3 xsi:nil=\"true\"/>\n"
        "        <element3 xsi:nil=\"true\"/>\n"
        "        <element4 xsi:nil=\"true\"/>\n"
        "        <element4 xsi:nil=\"true\"/>\n"
        "        <element5 xsi:nil=\"true\"/>\n"
        "        <element5 xsi:nil=\"true\"/>\n"
        "        <element6 xsi:nil=\"true\"/>\n"
        "        <element6 xsi:nil=\"true\"/>\n"
        "       </element5>\n"
        "       <element6 xsi:nil=\"true\"/>\n"
        "       <element6 xsi:nil=\"true\"/>\n"
        "      </element1>\n"
        "      <element2>\n"
        "       <selection1>2</selection1>\n"
        "      </element2>\n"
        "      <element2>\n"
        "       <selection1>2</selection1>\n"
        "      </element2>\n"
        "      <element3>FF0001</element3>\n"
        "      <element4>2</element4>\n"
        "      <element5>2012-08-18T13:25:00.000+00:00</element5>\n"
        "      <element6>custom</element6>\n"
        "      <element7>LONDON</element7>\n"
        "      <element8>true</element8>\n"
        "      <element9>arbitrary string value</element9>\n"
        "      <element10>1.5</element10>\n"
        "      <element11>FF0001</element11>\n"
        "      <element12>2</element12>\n"
        "      <element13>LONDON</element13>\n"
        "      <element14>true</element14>\n"
        "      <element14>true</element14>\n"
        "      <element15>1.5</element15>\n"
        "      <element15>1.5</element15>\n"
        "      <element16>FF0001</element16>\n"
        "      <element16>FF0001</element16>\n"
        "      <element17>2</element17>\n"
        "      <element17>2</element17>\n"
        "      <element18>2012-08-18T13:25:00.000+00:00</element18>\n"
        "      <element18>2012-08-18T13:25:00.000+00:00</element18>\n"
        "      <element19>custom</element19>\n"
        "      <element19>custom</element19>\n"
        "     </selection3>\n"
        "    </element2>\n"
        "    <element2>\n"
        "     <selection3>\n"
        "      <element1>\n"
        "       <element1>LONDON</element1>\n"
        "       <element1>LONDON</element1>\n"
        "       <element2>arbitrary string value</element2>\n"
        "       <element2>arbitrary string value</element2>\n"
        "       <element3>true</element3>\n"
        "       <element4>arbitrary string value</element4>\n"
        "       <element5>\n"
        "        <element1>\n"
        "         <element1>LONDON</element1>\n"
        "         <element1>LONDON</element1>\n"
        "         <element2>arbitrary string value</element2>\n"
        "         <element2>arbitrary string value</element2>\n"
        "         <element3>true</element3>\n"
        "         <element4>arbitrary string value</element4>\n"
        "         <element5>\n"
        "          <element1>\n"
        "           <element1>LONDON</element1>\n"
        "           <element1>LONDON</element1>\n"
        "           <element2>arbitrary string value</element2>\n"
        "           <element2>arbitrary string value</element2>\n"
        "           <element3>true</element3>\n"
        "           <element4>arbitrary string value</element4>\n"
        "           <element6 xsi:nil=\"true\"/>\n"
        "           <element6 xsi:nil=\"true\"/>\n"
        "          </element1>\n"
        "          <element2 xsi:nil=\"true\"/>\n"
        "          <element2 xsi:nil=\"true\"/>\n"
        "          <element3 xsi:nil=\"true\"/>\n"
        "          <element3 xsi:nil=\"true\"/>\n"
        "          <element4 xsi:nil=\"true\"/>\n"
        "          <element4 xsi:nil=\"true\"/>\n"
        "          <element5 xsi:nil=\"true\"/>\n"
        "          <element5 xsi:nil=\"true\"/>\n"
        "          <element6 xsi:nil=\"true\"/>\n"
        "          <element6 xsi:nil=\"true\"/>\n"
        "         </element5>\n"
        "         <element6 xsi:nil=\"true\"/>\n"
        "         <element6 xsi:nil=\"true\"/>\n"
        "        </element1>\n"
        "        <element2 xsi:nil=\"true\"/>\n"
        "        <element2 xsi:nil=\"true\"/>\n"
        "        <element3 xsi:nil=\"true\"/>\n"
        "        <element3 xsi:nil=\"true\"/>\n"
        "        <element4 xsi:nil=\"true\"/>\n"
        "        <element4 xsi:nil=\"true\"/>\n"
        "        <element5 xsi:nil=\"true\"/>\n"
        "        <element5 xsi:nil=\"true\"/>\n"
        "        <element6 xsi:nil=\"true\"/>\n"
        "        <element6 xsi:nil=\"true\"/>\n"
        "       </element5>\n"
        "       <element6 xsi:nil=\"true\"/>\n"
        "       <element6 xsi:nil=\"true\"/>\n"
        "      </element1>\n"
        "      <element1>\n"
        "       <element1>LONDON</element1>\n"
        "       <element1>LONDON</element1>\n"
        "       <element2>arbitrary string value</element2>\n"
        "       <element2>arbitrary string value</element2>\n"
        "       <element3>true</element3>\n"
        "       <element4>arbitrary string value</element4>\n"
        "       <element5>\n"
        "        <element1>\n"
        "         <element1>LONDON</element1>\n"
        "         <element1>LONDON</element1>\n"
        "         <element2>arbitrary string value</element2>\n"
        "         <element2>arbitrary string value</element2>\n"
        "         <element3>true</element3>\n"
        "         <element4>arbitrary string value</element4>\n"
        "         <element5>\n"
        "          <element1>\n"
        "           <element1>LONDON</element1>\n"
        "           <element1>LONDON</element1>\n"
        "           <element2>arbitrary string value</element2>\n"
        "           <element2>arbitrary string value</element2>\n"
        "           <element3>true</element3>\n"
        "           <element4>arbitrary string value</element4>\n"
        "           <element6 xsi:nil=\"true\"/>\n"
        "           <element6 xsi:nil=\"true\"/>\n"
        "          </element1>\n"
        "          <element2 xsi:nil=\"true\"/>\n"
        "          <element2 xsi:nil=\"true\"/>\n"
        "          <element3 xsi:nil=\"true\"/>\n"
        "          <element3 xsi:nil=\"true\"/>\n"
        "          <element4 xsi:nil=\"true\"/>\n"
        "          <element4 xsi:nil=\"true\"/>\n"
        "          <element5 xsi:nil=\"true\"/>\n"
        "          <element5 xsi:nil=\"true\"/>\n"
        "          <element6 xsi:nil=\"true\"/>\n"
        "          <element6 xsi:nil=\"true\"/>\n"
        "         </element5>\n"
        "         <element6 xsi:nil=\"true\"/>\n"
        "         <element6 xsi:nil=\"true\"/>\n"
        "        </element1>\n"
        "        <element2 xsi:nil=\"true\"/>\n"
        "        <element2 xsi:nil=\"true\"/>\n"
        "        <element3 xsi:nil=\"true\"/>\n"
        "        <element3 xsi:nil=\"true\"/>\n"
        "        <element4 xsi:nil=\"true\"/>\n"
        "        <element4 xsi:nil=\"true\"/>\n"
        "        <element5 xsi:nil=\"true\"/>\n"
        "        <element5 xsi:nil=\"true\"/>\n"
        "        <element6 xsi:nil=\"true\"/>\n"
        "        <element6 xsi:nil=\"true\"/>\n"
        "       </element5>\n"
        "       <element6 xsi:nil=\"true\"/>\n"
        "       <element6 xsi:nil=\"true\"/>\n"
        "      </element1>\n"
        "      <element2>\n"
        "       <selection1>2</selection1>\n"
        "      </element2>\n"
        "      <element2>\n"
        "       <selection1>2</selection1>\n"
        "      </element2>\n"
        "      <element3>FF0001</element3>\n"
        "      <element4>2</element4>\n"
        "      <element5>2012-08-18T13:25:00.000+00:00</element5>\n"
        "      <element6>custom</element6>\n"
        "      <element7>LONDON</element7>\n"
        "      <element8>true</element8>\n"
        "      <element9>arbitrary string value</element9>\n"
        "      <element10>1.5</element10>\n"
        "      <element11>FF0001</element11>\n"
        "      <element12>2</element12>\n"
        "      <element13>LONDON</element13>\n"
        "      <element14>true</element14>\n"
        "      <element14>true</element14>\n"
        "      <element15>1.5</element15>\n"
        "      <element15>1.5</element15>\n"
        "      <element16>FF0001</element16>\n"
        "      <element16>FF0001</element16>\n"
        "      <element17>2</element17>\n"
        "      <element17>2</element17>\n"
        "      <element18>2012-08-18T13:25:00.000+00:00</element18>\n"
        "      <element18>2012-08-18T13:25:00.000+00:00</element18>\n"
        "      <element19>custom</element19>\n"
        "      <element19>custom</element19>\n"
        "     </selection3>\n"
        "    </element2>\n"
        "    <element4>2</element4>\n"
        "    <element8>true</element8>\n"
        "    <element9></element9>\n"
        "    <element10>1.5</element10>\n"
        "    <element11>FF0001</element11>\n"
        "    <element12>-980123</element12>\n"
        "    <element13>LONDON</element13>\n"
        "    <element17>2</element17>\n"
        "    <element17>2</element17>\n"
        "   </selection3>\n"
        "  </element4>\n"
        "  <element5>\n"
        "   <selection4>999</selection4>\n"
        "  </element5>\n"
        "  <element5>\n"
        "   <selection4>999</selection4>\n"
        "  </element5>\n"
        " </selection1>\n"
        "</Topchoice>\n",

        "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
        "<Topchoice xmlns=\"TestNamespace\"\n"
        " xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\">\n"
        " <selection1>\n"
        "  <element1>\n"
        "   <selection1>\n"
        "    <element4>3123123123</element4>\n"
        "    <element5>255</element5>\n"
        "    <element7>custom</element7>\n"
        "    <element8>999</element8>\n"
        "   </selection1>\n"
        "  </element1>\n"
        "  <element2>\n"
        "   <selection3>\n"
        "    <element1>\n"
        "     <element1>LONDON</element1>\n"
        "     <element1>LONDON</element1>\n"
        "     <element2></element2>\n"
        "     <element2></element2>\n"
        "     <element3>true</element3>\n"
        "     <element4></element4>\n"
        "     <element5>\n"
        "      <element1>\n"
        "       <element1>LONDON</element1>\n"
        "       <element1>LONDON</element1>\n"
        "       <element2></element2>\n"
        "       <element2></element2>\n"
        "       <element3>true</element3>\n"
        "       <element4></element4>\n"
        "       <element5>\n"
        "        <element1>\n"
        "         <element1>LONDON</element1>\n"
        "         <element1>LONDON</element1>\n"
        "         <element2></element2>\n"
        "         <element2></element2>\n"
        "         <element3>true</element3>\n"
        "         <element4></element4>\n"
        "         <element6>LONDON</element6>\n"
        "         <element6>LONDON</element6>\n"
        "        </element1>\n"
        "        <element2>true</element2>\n"
        "        <element2>true</element2>\n"
        "        <element3>1.5</element3>\n"
        "        <element3>1.5</element3>\n"
        "        <element4>FF0001</element4>\n"
        "        <element4>FF0001</element4>\n"
        "        <element5 xsi:nil=\"true\"/>\n"
        "        <element5 xsi:nil=\"true\"/>\n"
        "        <element6>2012-08-18T13:25:00.000+00:00</element6>\n"
        "        <element6>2012-08-18T13:25:00.000+00:00</element6>\n"
        "       </element5>\n"
        "       <element6>LONDON</element6>\n"
        "       <element6>LONDON</element6>\n"
        "      </element1>\n"
        "      <element2>true</element2>\n"
        "      <element2>true</element2>\n"
        "      <element3>1.5</element3>\n"
        "      <element3>1.5</element3>\n"
        "      <element4>FF0001</element4>\n"
        "      <element4>FF0001</element4>\n"
        "      <element5 xsi:nil=\"true\"/>\n"
        "      <element5 xsi:nil=\"true\"/>\n"
        "      <element6>2012-08-18T13:25:00.000+00:00</element6>\n"
        "      <element6>2012-08-18T13:25:00.000+00:00</element6>\n"
        "     </element5>\n"
        "     <element6>LONDON</element6>\n"
        "     <element6>LONDON</element6>\n"
        "    </element1>\n"
        "    <element1>\n"
        "     <element1>LONDON</element1>\n"
        "     <element1>LONDON</element1>\n"
        "     <element2></element2>\n"
        "     <element2></element2>\n"
        "     <element3>true</element3>\n"
        "     <element4></element4>\n"
        "     <element5>\n"
        "      <element1>\n"
        "       <element1>LONDON</element1>\n"
        "       <element1>LONDON</element1>\n"
        "       <element2></element2>\n"
        "       <element2></element2>\n"
        "       <element3>true</element3>\n"
        "       <element4></element4>\n"
        "       <element5>\n"
        "        <element1>\n"
        "         <element1>LONDON</element1>\n"
        "         <element1>LONDON</element1>\n"
        "         <element2></element2>\n"
        "         <element2></element2>\n"
        "         <element3>true</element3>\n"
        "         <element4></element4>\n"
        "         <element6>LONDON</element6>\n"
        "         <element6>LONDON</element6>\n"
        "        </element1>\n"
        "        <element2>true</element2>\n"
        "        <element2>true</element2>\n"
        "        <element3>1.5</element3>\n"
        "        <element3>1.5</element3>\n"
        "        <element4>FF0001</element4>\n"
        "        <element4>FF0001</element4>\n"
        "        <element5 xsi:nil=\"true\"/>\n"
        "        <element5 xsi:nil=\"true\"/>\n"
        "        <element6>2012-08-18T13:25:00.000+00:00</element6>\n"
        "        <element6>2012-08-18T13:25:00.000+00:00</element6>\n"
        "       </element5>\n"
        "       <element6>LONDON</element6>\n"
        "       <element6>LONDON</element6>\n"
        "      </element1>\n"
        "      <element2>true</element2>\n"
        "      <element2>true</element2>\n"
        "      <element3>1.5</element3>\n"
        "      <element3>1.5</element3>\n"
        "      <element4>FF0001</element4>\n"
        "      <element4>FF0001</element4>\n"
        "      <element5 xsi:nil=\"true\"/>\n"
        "      <element5 xsi:nil=\"true\"/>\n"
        "      <element6>2012-08-18T13:25:00.000+00:00</element6>\n"
        "      <element6>2012-08-18T13:25:00.000+00:00</element6>\n"
        "     </element5>\n"
        "     <element6>LONDON</element6>\n"
        "     <element6>LONDON</element6>\n"
        "    </element1>\n"
        "    <element2>\n"
        "     <selection3>\n"
        "      <element1>\n"
        "       <element2></element2>\n"
        "       <element2></element2>\n"
        "       <element4></element4>\n"
        "       <element5>\n"
        "        <element1>\n"
        "         <element2></element2>\n"
        "         <element2></element2>\n"
        "         <element4></element4>\n"
        "         <element5>\n"
        "          <element1>\n"
        "           <element2></element2>\n"
        "           <element2></element2>\n"
        "           <element4></element4>\n"
        "           <element6>LONDON</element6>\n"
        "           <element6>LONDON</element6>\n"
        "          </element1>\n"
        "          <element2>true</element2>\n"
        "          <element2>true</element2>\n"
        "          <element3>1.5</element3>\n"
        "          <element3>1.5</element3>\n"
        "          <element4>FF0001</element4>\n"
        "          <element4>FF0001</element4>\n"
        "          <element5>-980123</element5>\n"
        "          <element5>-980123</element5>\n"
        "          <element6>2012-08-18T13:25:00.000+00:00</element6>\n"
        "          <element6>2012-08-18T13:25:00.000+00:00</element6>\n"
        "         </element5>\n"
        "         <element6>LONDON</element6>\n"
        "         <element6>LONDON</element6>\n"
        "        </element1>\n"
        "        <element2>true</element2>\n"
        "        <element2>true</element2>\n"
        "        <element3>1.5</element3>\n"
        "        <element3>1.5</element3>\n"
        "        <element4>FF0001</element4>\n"
        "        <element4>FF0001</element4>\n"
        "        <element5>-980123</element5>\n"
        "        <element5>-980123</element5>\n"
        "        <element6>2012-08-18T13:25:00.000+00:00</element6>\n"
        "        <element6>2012-08-18T13:25:00.000+00:00</element6>\n"
        "       </element5>\n"
        "       <element6>LONDON</element6>\n"
        "       <element6>LONDON</element6>\n"
        "      </element1>\n"
        "      <element1>\n"
        "       <element2></element2>\n"
        "       <element2></element2>\n"
        "       <element4></element4>\n"
        "       <element5>\n"
        "        <element1>\n"
        "         <element2></element2>\n"
        "         <element2></element2>\n"
        "         <element4></element4>\n"
        "         <element5>\n"
        "          <element1>\n"
        "           <element2></element2>\n"
        "           <element2></element2>\n"
        "           <element4></element4>\n"
        "           <element6>LONDON</element6>\n"
        "           <element6>LONDON</element6>\n"
        "          </element1>\n"
        "          <element2>true</element2>\n"
        "          <element2>true</element2>\n"
        "          <element3>1.5</element3>\n"
        "          <element3>1.5</element3>\n"
        "          <element4>FF0001</element4>\n"
        "          <element4>FF0001</element4>\n"
        "          <element5>-980123</element5>\n"
        "          <element5>-980123</element5>\n"
        "          <element6>2012-08-18T13:25:00.000+00:00</element6>\n"
        "          <element6>2012-08-18T13:25:00.000+00:00</element6>\n"
        "         </element5>\n"
        "         <element6>LONDON</element6>\n"
        "         <element6>LONDON</element6>\n"
        "        </element1>\n"
        "        <element2>true</element2>\n"
        "        <element2>true</element2>\n"
        "        <element3>1.5</element3>\n"
        "        <element3>1.5</element3>\n"
        "        <element4>FF0001</element4>\n"
        "        <element4>FF0001</element4>\n"
        "        <element5>-980123</element5>\n"
        "        <element5>-980123</element5>\n"
        "        <element6>2012-08-18T13:25:00.000+00:00</element6>\n"
        "        <element6>2012-08-18T13:25:00.000+00:00</element6>\n"
        "       </element5>\n"
        "       <element6>LONDON</element6>\n"
        "       <element6>LONDON</element6>\n"
        "      </element1>\n"
        "      <element2>\n"
        "       <selection2>1.5</selection2>\n"
        "      </element2>\n"
        "      <element2>\n"
        "       <selection2>1.5</selection2>\n"
        "      </element2>\n"
        "      <element4>-980123</element4>\n"
        "      <element8>true</element8>\n"
        "      <element9></element9>\n"
        "      <element10>1.5</element10>\n"
        "      <element11>FF0001</element11>\n"
        "      <element12>-980123</element12>\n"
        "      <element13>LONDON</element13>\n"
        "      <element17>-980123</element17>\n"
        "      <element17>-980123</element17>\n"
        "     </selection3>\n"
        "    </element2>\n"
        "    <element2>\n"
        "     <selection3>\n"
        "      <element1>\n"
        "       <element2></element2>\n"
        "       <element2></element2>\n"
        "       <element4></element4>\n"
        "       <element5>\n"
        "        <element1>\n"
        "         <element2></element2>\n"
        "         <element2></element2>\n"
        "         <element4></element4>\n"
        "         <element5>\n"
        "          <element1>\n"
        "           <element2></element2>\n"
        "           <element2></element2>\n"
        "           <element4></element4>\n"
        "           <element6>LONDON</element6>\n"
        "           <element6>LONDON</element6>\n"
        "          </element1>\n"
        "          <element2>true</element2>\n"
        "          <element2>true</element2>\n"
        "          <element3>1.5</element3>\n"
        "          <element3>1.5</element3>\n"
        "          <element4>FF0001</element4>\n"
        "          <element4>FF0001</element4>\n"
        "          <element5>-980123</element5>\n"
        "          <element5>-980123</element5>\n"
        "          <element6>2012-08-18T13:25:00.000+00:00</element6>\n"
        "          <element6>2012-08-18T13:25:00.000+00:00</element6>\n"
        "         </element5>\n"
        "         <element6>LONDON</element6>\n"
        "         <element6>LONDON</element6>\n"
        "        </element1>\n"
        "        <element2>true</element2>\n"
        "        <element2>true</element2>\n"
        "        <element3>1.5</element3>\n"
        "        <element3>1.5</element3>\n"
        "        <element4>FF0001</element4>\n"
        "        <element4>FF0001</element4>\n"
        "        <element5>-980123</element5>\n"
        "        <element5>-980123</element5>\n"
        "        <element6>2012-08-18T13:25:00.000+00:00</element6>\n"
        "        <element6>2012-08-18T13:25:00.000+00:00</element6>\n"
        "       </element5>\n"
        "       <element6>LONDON</element6>\n"
        "       <element6>LONDON</element6>\n"
        "      </element1>\n"
        "      <element1>\n"
        "       <element2></element2>\n"
        "       <element2></element2>\n"
        "       <element4></element4>\n"
        "       <element5>\n"
        "        <element1>\n"
        "         <element2></element2>\n"
        "         <element2></element2>\n"
        "         <element4></element4>\n"
        "         <element5>\n"
        "          <element1>\n"
        "           <element2></element2>\n"
        "           <element2></element2>\n"
        "           <element4></element4>\n"
        "           <element6>LONDON</element6>\n"
        "           <element6>LONDON</element6>\n"
        "          </element1>\n"
        "          <element2>true</element2>\n"
        "          <element2>true</element2>\n"
        "          <element3>1.5</element3>\n"
        "          <element3>1.5</element3>\n"
        "          <element4>FF0001</element4>\n"
        "          <element4>FF0001</element4>\n"
        "          <element5>-980123</element5>\n"
        "          <element5>-980123</element5>\n"
        "          <element6>2012-08-18T13:25:00.000+00:00</element6>\n"
        "          <element6>2012-08-18T13:25:00.000+00:00</element6>\n"
        "         </element5>\n"
        "         <element6>LONDON</element6>\n"
        "         <element6>LONDON</element6>\n"
        "        </element1>\n"
        "        <element2>true</element2>\n"
        "        <element2>true</element2>\n"
        "        <element3>1.5</element3>\n"
        "        <element3>1.5</element3>\n"
        "        <element4>FF0001</element4>\n"
        "        <element4>FF0001</element4>\n"
        "        <element5>-980123</element5>\n"
        "        <element5>-980123</element5>\n"
        "        <element6>2012-08-18T13:25:00.000+00:00</element6>\n"
        "        <element6>2012-08-18T13:25:00.000+00:00</element6>\n"
        "       </element5>\n"
        "       <element6>LONDON</element6>\n"
        "       <element6>LONDON</element6>\n"
        "      </element1>\n"
        "      <element2>\n"
        "       <selection2>1.5</selection2>\n"
        "      </element2>\n"
        "      <element2>\n"
        "       <selection2>1.5</selection2>\n"
        "      </element2>\n"
        "      <element4>-980123</element4>\n"
        "      <element8>true</element8>\n"
        "      <element9></element9>\n"
        "      <element10>1.5</element10>\n"
        "      <element11>FF0001</element11>\n"
        "      <element12>-980123</element12>\n"
        "      <element13>LONDON</element13>\n"
        "      <element17>-980123</element17>\n"
        "      <element17>-980123</element17>\n"
        "     </selection3>\n"
        "    </element2>\n"
        "    <element3>FF0001</element3>\n"
        "    <element4>-980123</element4>\n"
        "    <element5>2012-08-18T13:25:00.000+00:00</element5>\n"
        "    <element6>custom</element6>\n"
        "    <element7>LONDON</element7>\n"
        "    <element8>true</element8>\n"
        "    <element9>arbitrary string value</element9>\n"
        "    <element10>1.5</element10>\n"
        "    <element11>FF0001</element11>\n"
        "    <element12>2</element12>\n"
        "    <element13>LONDON</element13>\n"
        "    <element14>true</element14>\n"
        "    <element14>true</element14>\n"
        "    <element15>1.5</element15>\n"
        "    <element15>1.5</element15>\n"
        "    <element16>FF0001</element16>\n"
        "    <element16>FF0001</element16>\n"
        "    <element17>-980123</element17>\n"
        "    <element17>-980123</element17>\n"
        "    <element18>2012-08-18T13:25:00.000+00:00</element18>\n"
        "    <element18>2012-08-18T13:25:00.000+00:00</element18>\n"
        "    <element19>custom</element19>\n"
        "    <element19>custom</element19>\n"
        "   </selection3>\n"
        "  </element2>\n"
        "  <element2>\n"
        "   <selection3>\n"
        "    <element1>\n"
        "     <element1>LONDON</element1>\n"
        "     <element1>LONDON</element1>\n"
        "     <element2></element2>\n"
        "     <element2></element2>\n"
        "     <element3>true</element3>\n"
        "     <element4></element4>\n"
        "     <element5>\n"
        "      <element1>\n"
        "       <element1>LONDON</element1>\n"
        "       <element1>LONDON</element1>\n"
        "       <element2></element2>\n"
        "       <element2></element2>\n"
        "       <element3>true</element3>\n"
        "       <element4></element4>\n"
        "       <element5>\n"
        "        <element1>\n"
        "         <element1>LONDON</element1>\n"
        "         <element1>LONDON</element1>\n"
        "         <element2></element2>\n"
        "         <element2></element2>\n"
        "         <element3>true</element3>\n"
        "         <element4></element4>\n"
        "         <element6>LONDON</element6>\n"
        "         <element6>LONDON</element6>\n"
        "        </element1>\n"
        "        <element2>true</element2>\n"
        "        <element2>true</element2>\n"
        "        <element3>1.5</element3>\n"
        "        <element3>1.5</element3>\n"
        "        <element4>FF0001</element4>\n"
        "        <element4>FF0001</element4>\n"
        "        <element5 xsi:nil=\"true\"/>\n"
        "        <element5 xsi:nil=\"true\"/>\n"
        "        <element6>2012-08-18T13:25:00.000+00:00</element6>\n"
        "        <element6>2012-08-18T13:25:00.000+00:00</element6>\n"
        "       </element5>\n"
        "       <element6>LONDON</element6>\n"
        "       <element6>LONDON</element6>\n"
        "      </element1>\n"
        "      <element2>true</element2>\n"
        "      <element2>true</element2>\n"
        "      <element3>1.5</element3>\n"
        "      <element3>1.5</element3>\n"
        "      <element4>FF0001</element4>\n"
        "      <element4>FF0001</element4>\n"
        "      <element5 xsi:nil=\"true\"/>\n"
        "      <element5 xsi:nil=\"true\"/>\n"
        "      <element6>2012-08-18T13:25:00.000+00:00</element6>\n"
        "      <element6>2012-08-18T13:25:00.000+00:00</element6>\n"
        "     </element5>\n"
        "     <element6>LONDON</element6>\n"
        "     <element6>LONDON</element6>\n"
        "    </element1>\n"
        "    <element1>\n"
        "     <element1>LONDON</element1>\n"
        "     <element1>LONDON</element1>\n"
        "     <element2></element2>\n"
        "     <element2></element2>\n"
        "     <element3>true</element3>\n"
        "     <element4></element4>\n"
        "     <element5>\n"
        "      <element1>\n"
        "       <element1>LONDON</element1>\n"
        "       <element1>LONDON</element1>\n"
        "       <element2></element2>\n"
        "       <element2></element2>\n"
        "       <element3>true</element3>\n"
        "       <element4></element4>\n"
        "       <element5>\n"
        "        <element1>\n"
        "         <element1>LONDON</element1>\n"
        "         <element1>LONDON</element1>\n"
        "         <element2></element2>\n"
        "         <element2></element2>\n"
        "         <element3>true</element3>\n"
        "         <element4></element4>\n"
        "         <element6>LONDON</element6>\n"
        "         <element6>LONDON</element6>\n"
        "        </element1>\n"
        "        <element2>true</element2>\n"
        "        <element2>true</element2>\n"
        "        <element3>1.5</element3>\n"
        "        <element3>1.5</element3>\n"
        "        <element4>FF0001</element4>\n"
        "        <element4>FF0001</element4>\n"
        "        <element5 xsi:nil=\"true\"/>\n"
        "        <element5 xsi:nil=\"true\"/>\n"
        "        <element6>2012-08-18T13:25:00.000+00:00</element6>\n"
        "        <element6>2012-08-18T13:25:00.000+00:00</element6>\n"
        "       </element5>\n"
        "       <element6>LONDON</element6>\n"
        "       <element6>LONDON</element6>\n"
        "      </element1>\n"
        "      <element2>true</element2>\n"
        "      <element2>true</element2>\n"
        "      <element3>1.5</element3>\n"
        "      <element3>1.5</element3>\n"
        "      <element4>FF0001</element4>\n"
        "      <element4>FF0001</element4>\n"
        "      <element5 xsi:nil=\"true\"/>\n"
        "      <element5 xsi:nil=\"true\"/>\n"
        "      <element6>2012-08-18T13:25:00.000+00:00</element6>\n"
        "      <element6>2012-08-18T13:25:00.000+00:00</element6>\n"
        "     </element5>\n"
        "     <element6>LONDON</element6>\n"
        "     <element6>LONDON</element6>\n"
        "    </element1>\n"
        "    <element2>\n"
        "     <selection3>\n"
        "      <element1>\n"
        "       <element2></element2>\n"
        "       <element2></element2>\n"
        "       <element4></element4>\n"
        "       <element5>\n"
        "        <element1>\n"
        "         <element2></element2>\n"
        "         <element2></element2>\n"
        "         <element4></element4>\n"
        "         <element5>\n"
        "          <element1>\n"
        "           <element2></element2>\n"
        "           <element2></element2>\n"
        "           <element4></element4>\n"
        "           <element6>LONDON</element6>\n"
        "           <element6>LONDON</element6>\n"
        "          </element1>\n"
        "          <element2>true</element2>\n"
        "          <element2>true</element2>\n"
        "          <element3>1.5</element3>\n"
        "          <element3>1.5</element3>\n"
        "          <element4>FF0001</element4>\n"
        "          <element4>FF0001</element4>\n"
        "          <element5>-980123</element5>\n"
        "          <element5>-980123</element5>\n"
        "          <element6>2012-08-18T13:25:00.000+00:00</element6>\n"
        "          <element6>2012-08-18T13:25:00.000+00:00</element6>\n"
        "         </element5>\n"
        "         <element6>LONDON</element6>\n"
        "         <element6>LONDON</element6>\n"
        "        </element1>\n"
        "        <element2>true</element2>\n"
        "        <element2>true</element2>\n"
        "        <element3>1.5</element3>\n"
        "        <element3>1.5</element3>\n"
        "        <element4>FF0001</element4>\n"
        "        <element4>FF0001</element4>\n"
        "        <element5>-980123</element5>\n"
        "        <element5>-980123</element5>\n"
        "        <element6>2012-08-18T13:25:00.000+00:00</element6>\n"
        "        <element6>2012-08-18T13:25:00.000+00:00</element6>\n"
        "       </element5>\n"
        "       <element6>LONDON</element6>\n"
        "       <element6>LONDON</element6>\n"
        "      </element1>\n"
        "      <element1>\n"
        "       <element2></element2>\n"
        "       <element2></element2>\n"
        "       <element4></element4>\n"
        "       <element5>\n"
        "        <element1>\n"
        "         <element2></element2>\n"
        "         <element2></element2>\n"
        "         <element4></element4>\n"
        "         <element5>\n"
        "          <element1>\n"
        "           <element2></element2>\n"
        "           <element2></element2>\n"
        "           <element4></element4>\n"
        "           <element6>LONDON</element6>\n"
        "           <element6>LONDON</element6>\n"
        "          </element1>\n"
        "          <element2>true</element2>\n"
        "          <element2>true</element2>\n"
        "          <element3>1.5</element3>\n"
        "          <element3>1.5</element3>\n"
        "          <element4>FF0001</element4>\n"
        "          <element4>FF0001</element4>\n"
        "          <element5>-980123</element5>\n"
        "          <element5>-980123</element5>\n"
        "          <element6>2012-08-18T13:25:00.000+00:00</element6>\n"
        "          <element6>2012-08-18T13:25:00.000+00:00</element6>\n"
        "         </element5>\n"
        "         <element6>LONDON</element6>\n"
        "         <element6>LONDON</element6>\n"
        "        </element1>\n"
        "        <element2>true</element2>\n"
        "        <element2>true</element2>\n"
        "        <element3>1.5</element3>\n"
        "        <element3>1.5</element3>\n"
        "        <element4>FF0001</element4>\n"
        "        <element4>FF0001</element4>\n"
        "        <element5>-980123</element5>\n"
        "        <element5>-980123</element5>\n"
        "        <element6>2012-08-18T13:25:00.000+00:00</element6>\n"
        "        <element6>2012-08-18T13:25:00.000+00:00</element6>\n"
        "       </element5>\n"
        "       <element6>LONDON</element6>\n"
        "       <element6>LONDON</element6>\n"
        "      </element1>\n"
        "      <element2>\n"
        "       <selection2>1.5</selection2>\n"
        "      </element2>\n"
        "      <element2>\n"
        "       <selection2>1.5</selection2>\n"
        "      </element2>\n"
        "      <element4>-980123</element4>\n"
        "      <element8>true</element8>\n"
        "      <element9></element9>\n"
        "      <element10>1.5</element10>\n"
        "      <element11>FF0001</element11>\n"
        "      <element12>-980123</element12>\n"
        "      <element13>LONDON</element13>\n"
        "      <element17>-980123</element17>\n"
        "      <element17>-980123</element17>\n"
        "     </selection3>\n"
        "    </element2>\n"
        "    <element2>\n"
        "     <selection3>\n"
        "      <element1>\n"
        "       <element2></element2>\n"
        "       <element2></element2>\n"
        "       <element4></element4>\n"
        "       <element5>\n"
        "        <element1>\n"
        "         <element2></element2>\n"
        "         <element2></element2>\n"
        "         <element4></element4>\n"
        "         <element5>\n"
        "          <element1>\n"
        "           <element2></element2>\n"
        "           <element2></element2>\n"
        "           <element4></element4>\n"
        "           <element6>LONDON</element6>\n"
        "           <element6>LONDON</element6>\n"
        "          </element1>\n"
        "          <element2>true</element2>\n"
        "          <element2>true</element2>\n"
        "          <element3>1.5</element3>\n"
        "          <element3>1.5</element3>\n"
        "          <element4>FF0001</element4>\n"
        "          <element4>FF0001</element4>\n"
        "          <element5>-980123</element5>\n"
        "          <element5>-980123</element5>\n"
        "          <element6>2012-08-18T13:25:00.000+00:00</element6>\n"
        "          <element6>2012-08-18T13:25:00.000+00:00</element6>\n"
        "         </element5>\n"
        "         <element6>LONDON</element6>\n"
        "         <element6>LONDON</element6>\n"
        "        </element1>\n"
        "        <element2>true</element2>\n"
        "        <element2>true</element2>\n"
        "        <element3>1.5</element3>\n"
        "        <element3>1.5</element3>\n"
        "        <element4>FF0001</element4>\n"
        "        <element4>FF0001</element4>\n"
        "        <element5>-980123</element5>\n"
        "        <element5>-980123</element5>\n"
        "        <element6>2012-08-18T13:25:00.000+00:00</element6>\n"
        "        <element6>2012-08-18T13:25:00.000+00:00</element6>\n"
        "       </element5>\n"
        "       <element6>LONDON</element6>\n"
        "       <element6>LONDON</element6>\n"
        "      </element1>\n"
        "      <element1>\n"
        "       <element2></element2>\n"
        "       <element2></element2>\n"
        "       <element4></element4>\n"
        "       <element5>\n"
        "        <element1>\n"
        "         <element2></element2>\n"
        "         <element2></element2>\n"
        "         <element4></element4>\n"
        "         <element5>\n"
        "          <element1>\n"
        "           <element2></element2>\n"
        "           <element2></element2>\n"
        "           <element4></element4>\n"
        "           <element6>LONDON</element6>\n"
        "           <element6>LONDON</element6>\n"
        "          </element1>\n"
        "          <element2>true</element2>\n"
        "          <element2>true</element2>\n"
        "          <element3>1.5</element3>\n"
        "          <element3>1.5</element3>\n"
        "          <element4>FF0001</element4>\n"
        "          <element4>FF0001</element4>\n"
        "          <element5>-980123</element5>\n"
        "          <element5>-980123</element5>\n"
        "          <element6>2012-08-18T13:25:00.000+00:00</element6>\n"
        "          <element6>2012-08-18T13:25:00.000+00:00</element6>\n"
        "         </element5>\n"
        "         <element6>LONDON</element6>\n"
        "         <element6>LONDON</element6>\n"
        "        </element1>\n"
        "        <element2>true</element2>\n"
        "        <element2>true</element2>\n"
        "        <element3>1.5</element3>\n"
        "        <element3>1.5</element3>\n"
        "        <element4>FF0001</element4>\n"
        "        <element4>FF0001</element4>\n"
        "        <element5>-980123</element5>\n"
        "        <element5>-980123</element5>\n"
        "        <element6>2012-08-18T13:25:00.000+00:00</element6>\n"
        "        <element6>2012-08-18T13:25:00.000+00:00</element6>\n"
        "       </element5>\n"
        "       <element6>LONDON</element6>\n"
        "       <element6>LONDON</element6>\n"
        "      </element1>\n"
        "      <element2>\n"
        "       <selection2>1.5</selection2>\n"
        "      </element2>\n"
        "      <element2>\n"
        "       <selection2>1.5</selection2>\n"
        "      </element2>\n"
        "      <element4>-980123</element4>\n"
        "      <element8>true</element8>\n"
        "      <element9></element9>\n"
        "      <element10>1.5</element10>\n"
        "      <element11>FF0001</element11>\n"
        "      <element12>-980123</element12>\n"
        "      <element13>LONDON</element13>\n"
        "      <element17>-980123</element17>\n"
        "      <element17>-980123</element17>\n"
        "     </selection3>\n"
        "    </element2>\n"
        "    <element3>FF0001</element3>\n"
        "    <element4>-980123</element4>\n"
        "    <element5>2012-08-18T13:25:00.000+00:00</element5>\n"
        "    <element6>custom</element6>\n"
        "    <element7>LONDON</element7>\n"
        "    <element8>true</element8>\n"
        "    <element9>arbitrary string value</element9>\n"
        "    <element10>1.5</element10>\n"
        "    <element11>FF0001</element11>\n"
        "    <element12>2</element12>\n"
        "    <element13>LONDON</element13>\n"
        "    <element14>true</element14>\n"
        "    <element14>true</element14>\n"
        "    <element15>1.5</element15>\n"
        "    <element15>1.5</element15>\n"
        "    <element16>FF0001</element16>\n"
        "    <element16>FF0001</element16>\n"
        "    <element17>-980123</element17>\n"
        "    <element17>-980123</element17>\n"
        "    <element18>2012-08-18T13:25:00.000+00:00</element18>\n"
        "    <element18>2012-08-18T13:25:00.000+00:00</element18>\n"
        "    <element19>custom</element19>\n"
        "    <element19>custom</element19>\n"
        "   </selection3>\n"
        "  </element2>\n"
        "  <element3>\n"
        "   <selection3>\n"
        "    <selection3>\n"
        "     <element1>\n"
        "      <element2></element2>\n"
        "      <element2></element2>\n"
        "      <element4></element4>\n"
        "      <element5>\n"
        "       <element1>\n"
        "        <element2></element2>\n"
        "        <element2></element2>\n"
        "        <element4></element4>\n"
        "        <element5>\n"
        "         <element1>\n"
        "          <element2></element2>\n"
        "          <element2></element2>\n"
        "          <element4></element4>\n"
        "          <element6>LONDON</element6>\n"
        "          <element6>LONDON</element6>\n"
        "         </element1>\n"
        "         <element2>true</element2>\n"
        "         <element2>true</element2>\n"
        "         <element3>1.5</element3>\n"
        "         <element3>1.5</element3>\n"
        "         <element4>FF0001</element4>\n"
        "         <element4>FF0001</element4>\n"
        "         <element5>-980123</element5>\n"
        "         <element5>-980123</element5>\n"
        "         <element6>2012-08-18T13:25:00.000+00:00</element6>\n"
        "         <element6>2012-08-18T13:25:00.000+00:00</element6>\n"
        "        </element5>\n"
        "        <element6>LONDON</element6>\n"
        "        <element6>LONDON</element6>\n"
        "       </element1>\n"
        "       <element2>true</element2>\n"
        "       <element2>true</element2>\n"
        "       <element3>1.5</element3>\n"
        "       <element3>1.5</element3>\n"
        "       <element4>FF0001</element4>\n"
        "       <element4>FF0001</element4>\n"
        "       <element5>-980123</element5>\n"
        "       <element5>-980123</element5>\n"
        "       <element6>2012-08-18T13:25:00.000+00:00</element6>\n"
        "       <element6>2012-08-18T13:25:00.000+00:00</element6>\n"
        "      </element5>\n"
        "      <element6>LONDON</element6>\n"
        "      <element6>LONDON</element6>\n"
        "     </element1>\n"
        "     <element1>\n"
        "      <element2></element2>\n"
        "      <element2></element2>\n"
        "      <element4></element4>\n"
        "      <element5>\n"
        "       <element1>\n"
        "        <element2></element2>\n"
        "        <element2></element2>\n"
        "        <element4></element4>\n"
        "        <element5>\n"
        "         <element1>\n"
        "          <element2></element2>\n"
        "          <element2></element2>\n"
        "          <element4></element4>\n"
        "          <element6>LONDON</element6>\n"
        "          <element6>LONDON</element6>\n"
        "         </element1>\n"
        "         <element2>true</element2>\n"
        "         <element2>true</element2>\n"
        "         <element3>1.5</element3>\n"
        "         <element3>1.5</element3>\n"
        "         <element4>FF0001</element4>\n"
        "         <element4>FF0001</element4>\n"
        "         <element5>-980123</element5>\n"
        "         <element5>-980123</element5>\n"
        "         <element6>2012-08-18T13:25:00.000+00:00</element6>\n"
        "         <element6>2012-08-18T13:25:00.000+00:00</element6>\n"
        "        </element5>\n"
        "        <element6>LONDON</element6>\n"
        "        <element6>LONDON</element6>\n"
        "       </element1>\n"
        "       <element2>true</element2>\n"
        "       <element2>true</element2>\n"
        "       <element3>1.5</element3>\n"
        "       <element3>1.5</element3>\n"
        "       <element4>FF0001</element4>\n"
        "       <element4>FF0001</element4>\n"
        "       <element5>-980123</element5>\n"
        "       <element5>-980123</element5>\n"
        "       <element6>2012-08-18T13:25:00.000+00:00</element6>\n"
        "       <element6>2012-08-18T13:25:00.000+00:00</element6>\n"
        "      </element5>\n"
        "      <element6>LONDON</element6>\n"
        "      <element6>LONDON</element6>\n"
        "     </element1>\n"
        "     <element2>\n"
        "      <selection2>1.5</selection2>\n"
        "     </element2>\n"
        "     <element2>\n"
        "      <selection2>1.5</selection2>\n"
        "     </element2>\n"
        "     <element4>-980123</element4>\n"
        "     <element8>true</element8>\n"
        "     <element9></element9>\n"
        "     <element10>1.5</element10>\n"
        "     <element11>FF0001</element11>\n"
        "     <element12>-980123</element12>\n"
        "     <element13>LONDON</element13>\n"
        "     <element17>-980123</element17>\n"
        "     <element17>-980123</element17>\n"
        "    </selection3>\n"
        "   </selection3>\n"
        "  </element3>\n"
        "  <element4>\n"
        "   <selection3>\n"
        "    <element1>\n"
        "     <element1>LONDON</element1>\n"
        "     <element1>LONDON</element1>\n"
        "     <element2></element2>\n"
        "     <element2></element2>\n"
        "     <element3>true</element3>\n"
        "     <element4></element4>\n"
        "     <element5>\n"
        "      <element1>\n"
        "       <element1>LONDON</element1>\n"
        "       <element1>LONDON</element1>\n"
        "       <element2></element2>\n"
        "       <element2></element2>\n"
        "       <element3>true</element3>\n"
        "       <element4></element4>\n"
        "       <element5>\n"
        "        <element1>\n"
        "         <element1>LONDON</element1>\n"
        "         <element1>LONDON</element1>\n"
        "         <element2></element2>\n"
        "         <element2></element2>\n"
        "         <element3>true</element3>\n"
        "         <element4></element4>\n"
        "         <element6>LONDON</element6>\n"
        "         <element6>LONDON</element6>\n"
        "        </element1>\n"
        "        <element2>true</element2>\n"
        "        <element2>true</element2>\n"
        "        <element3>1.5</element3>\n"
        "        <element3>1.5</element3>\n"
        "        <element4>FF0001</element4>\n"
        "        <element4>FF0001</element4>\n"
        "        <element5 xsi:nil=\"true\"/>\n"
        "        <element5 xsi:nil=\"true\"/>\n"
        "        <element6>2012-08-18T13:25:00.000+00:00</element6>\n"
        "        <element6>2012-08-18T13:25:00.000+00:00</element6>\n"
        "       </element5>\n"
        "       <element6>LONDON</element6>\n"
        "       <element6>LONDON</element6>\n"
        "      </element1>\n"
        "      <element2>true</element2>\n"
        "      <element2>true</element2>\n"
        "      <element3>1.5</element3>\n"
        "      <element3>1.5</element3>\n"
        "      <element4>FF0001</element4>\n"
        "      <element4>FF0001</element4>\n"
        "      <element5 xsi:nil=\"true\"/>\n"
        "      <element5 xsi:nil=\"true\"/>\n"
        "      <element6>2012-08-18T13:25:00.000+00:00</element6>\n"
        "      <element6>2012-08-18T13:25:00.000+00:00</element6>\n"
        "     </element5>\n"
        "     <element6>LONDON</element6>\n"
        "     <element6>LONDON</element6>\n"
        "    </element1>\n"
        "    <element1>\n"
        "     <element1>LONDON</element1>\n"
        "     <element1>LONDON</element1>\n"
        "     <element2></element2>\n"
        "     <element2></element2>\n"
        "     <element3>true</element3>\n"
        "     <element4></element4>\n"
        "     <element5>\n"
        "      <element1>\n"
        "       <element1>LONDON</element1>\n"
        "       <element1>LONDON</element1>\n"
        "       <element2></element2>\n"
        "       <element2></element2>\n"
        "       <element3>true</element3>\n"
        "       <element4></element4>\n"
        "       <element5>\n"
        "        <element1>\n"
        "         <element1>LONDON</element1>\n"
        "         <element1>LONDON</element1>\n"
        "         <element2></element2>\n"
        "         <element2></element2>\n"
        "         <element3>true</element3>\n"
        "         <element4></element4>\n"
        "         <element6>LONDON</element6>\n"
        "         <element6>LONDON</element6>\n"
        "        </element1>\n"
        "        <element2>true</element2>\n"
        "        <element2>true</element2>\n"
        "        <element3>1.5</element3>\n"
        "        <element3>1.5</element3>\n"
        "        <element4>FF0001</element4>\n"
        "        <element4>FF0001</element4>\n"
        "        <element5 xsi:nil=\"true\"/>\n"
        "        <element5 xsi:nil=\"true\"/>\n"
        "        <element6>2012-08-18T13:25:00.000+00:00</element6>\n"
        "        <element6>2012-08-18T13:25:00.000+00:00</element6>\n"
        "       </element5>\n"
        "       <element6>LONDON</element6>\n"
        "       <element6>LONDON</element6>\n"
        "      </element1>\n"
        "      <element2>true</element2>\n"
        "      <element2>true</element2>\n"
        "      <element3>1.5</element3>\n"
        "      <element3>1.5</element3>\n"
        "      <element4>FF0001</element4>\n"
        "      <element4>FF0001</element4>\n"
        "      <element5 xsi:nil=\"true\"/>\n"
        "      <element5 xsi:nil=\"true\"/>\n"
        "      <element6>2012-08-18T13:25:00.000+00:00</element6>\n"
        "      <element6>2012-08-18T13:25:00.000+00:00</element6>\n"
        "     </element5>\n"
        "     <element6>LONDON</element6>\n"
        "     <element6>LONDON</element6>\n"
        "    </element1>\n"
        "    <element2>\n"
        "     <selection3>\n"
        "      <element1>\n"
        "       <element2></element2>\n"
        "       <element2></element2>\n"
        "       <element4></element4>\n"
        "       <element5>\n"
        "        <element1>\n"
        "         <element2></element2>\n"
        "         <element2></element2>\n"
        "         <element4></element4>\n"
        "         <element5>\n"
        "          <element1>\n"
        "           <element2></element2>\n"
        "           <element2></element2>\n"
        "           <element4></element4>\n"
        "           <element6>LONDON</element6>\n"
        "           <element6>LONDON</element6>\n"
        "          </element1>\n"
        "          <element2>true</element2>\n"
        "          <element2>true</element2>\n"
        "          <element3>1.5</element3>\n"
        "          <element3>1.5</element3>\n"
        "          <element4>FF0001</element4>\n"
        "          <element4>FF0001</element4>\n"
        "          <element5>-980123</element5>\n"
        "          <element5>-980123</element5>\n"
        "          <element6>2012-08-18T13:25:00.000+00:00</element6>\n"
        "          <element6>2012-08-18T13:25:00.000+00:00</element6>\n"
        "         </element5>\n"
        "         <element6>LONDON</element6>\n"
        "         <element6>LONDON</element6>\n"
        "        </element1>\n"
        "        <element2>true</element2>\n"
        "        <element2>true</element2>\n"
        "        <element3>1.5</element3>\n"
        "        <element3>1.5</element3>\n"
        "        <element4>FF0001</element4>\n"
        "        <element4>FF0001</element4>\n"
        "        <element5>-980123</element5>\n"
        "        <element5>-980123</element5>\n"
        "        <element6>2012-08-18T13:25:00.000+00:00</element6>\n"
        "        <element6>2012-08-18T13:25:00.000+00:00</element6>\n"
        "       </element5>\n"
        "       <element6>LONDON</element6>\n"
        "       <element6>LONDON</element6>\n"
        "      </element1>\n"
        "      <element1>\n"
        "       <element2></element2>\n"
        "       <element2></element2>\n"
        "       <element4></element4>\n"
        "       <element5>\n"
        "        <element1>\n"
        "         <element2></element2>\n"
        "         <element2></element2>\n"
        "         <element4></element4>\n"
        "         <element5>\n"
        "          <element1>\n"
        "           <element2></element2>\n"
        "           <element2></element2>\n"
        "           <element4></element4>\n"
        "           <element6>LONDON</element6>\n"
        "           <element6>LONDON</element6>\n"
        "          </element1>\n"
        "          <element2>true</element2>\n"
        "          <element2>true</element2>\n"
        "          <element3>1.5</element3>\n"
        "          <element3>1.5</element3>\n"
        "          <element4>FF0001</element4>\n"
        "          <element4>FF0001</element4>\n"
        "          <element5>-980123</element5>\n"
        "          <element5>-980123</element5>\n"
        "          <element6>2012-08-18T13:25:00.000+00:00</element6>\n"
        "          <element6>2012-08-18T13:25:00.000+00:00</element6>\n"
        "         </element5>\n"
        "         <element6>LONDON</element6>\n"
        "         <element6>LONDON</element6>\n"
        "        </element1>\n"
        "        <element2>true</element2>\n"
        "        <element2>true</element2>\n"
        "        <element3>1.5</element3>\n"
        "        <element3>1.5</element3>\n"
        "        <element4>FF0001</element4>\n"
        "        <element4>FF0001</element4>\n"
        "        <element5>-980123</element5>\n"
        "        <element5>-980123</element5>\n"
        "        <element6>2012-08-18T13:25:00.000+00:00</element6>\n"
        "        <element6>2012-08-18T13:25:00.000+00:00</element6>\n"
        "       </element5>\n"
        "       <element6>LONDON</element6>\n"
        "       <element6>LONDON</element6>\n"
        "      </element1>\n"
        "      <element2>\n"
        "       <selection2>1.5</selection2>\n"
        "      </element2>\n"
        "      <element2>\n"
        "       <selection2>1.5</selection2>\n"
        "      </element2>\n"
        "      <element4>-980123</element4>\n"
        "      <element8>true</element8>\n"
        "      <element9></element9>\n"
        "      <element10>1.5</element10>\n"
        "      <element11>FF0001</element11>\n"
        "      <element12>-980123</element12>\n"
        "      <element13>LONDON</element13>\n"
        "      <element17>-980123</element17>\n"
        "      <element17>-980123</element17>\n"
        "     </selection3>\n"
        "    </element2>\n"
        "    <element2>\n"
        "     <selection3>\n"
        "      <element1>\n"
        "       <element2></element2>\n"
        "       <element2></element2>\n"
        "       <element4></element4>\n"
        "       <element5>\n"
        "        <element1>\n"
        "         <element2></element2>\n"
        "         <element2></element2>\n"
        "         <element4></element4>\n"
        "         <element5>\n"
        "          <element1>\n"
        "           <element2></element2>\n"
        "           <element2></element2>\n"
        "           <element4></element4>\n"
        "           <element6>LONDON</element6>\n"
        "           <element6>LONDON</element6>\n"
        "          </element1>\n"
        "          <element2>true</element2>\n"
        "          <element2>true</element2>\n"
        "          <element3>1.5</element3>\n"
        "          <element3>1.5</element3>\n"
        "          <element4>FF0001</element4>\n"
        "          <element4>FF0001</element4>\n"
        "          <element5>-980123</element5>\n"
        "          <element5>-980123</element5>\n"
        "          <element6>2012-08-18T13:25:00.000+00:00</element6>\n"
        "          <element6>2012-08-18T13:25:00.000+00:00</element6>\n"
        "         </element5>\n"
        "         <element6>LONDON</element6>\n"
        "         <element6>LONDON</element6>\n"
        "        </element1>\n"
        "        <element2>true</element2>\n"
        "        <element2>true</element2>\n"
        "        <element3>1.5</element3>\n"
        "        <element3>1.5</element3>\n"
        "        <element4>FF0001</element4>\n"
        "        <element4>FF0001</element4>\n"
        "        <element5>-980123</element5>\n"
        "        <element5>-980123</element5>\n"
        "        <element6>2012-08-18T13:25:00.000+00:00</element6>\n"
        "        <element6>2012-08-18T13:25:00.000+00:00</element6>\n"
        "       </element5>\n"
        "       <element6>LONDON</element6>\n"
        "       <element6>LONDON</element6>\n"
        "      </element1>\n"
        "      <element1>\n"
        "       <element2></element2>\n"
        "       <element2></element2>\n"
        "       <element4></element4>\n"
        "       <element5>\n"
        "        <element1>\n"
        "         <element2></element2>\n"
        "         <element2></element2>\n"
        "         <element4></element4>\n"
        "         <element5>\n"
        "          <element1>\n"
        "           <element2></element2>\n"
        "           <element2></element2>\n"
        "           <element4></element4>\n"
        "           <element6>LONDON</element6>\n"
        "           <element6>LONDON</element6>\n"
        "          </element1>\n"
        "          <element2>true</element2>\n"
        "          <element2>true</element2>\n"
        "          <element3>1.5</element3>\n"
        "          <element3>1.5</element3>\n"
        "          <element4>FF0001</element4>\n"
        "          <element4>FF0001</element4>\n"
        "          <element5>-980123</element5>\n"
        "          <element5>-980123</element5>\n"
        "          <element6>2012-08-18T13:25:00.000+00:00</element6>\n"
        "          <element6>2012-08-18T13:25:00.000+00:00</element6>\n"
        "         </element5>\n"
        "         <element6>LONDON</element6>\n"
        "         <element6>LONDON</element6>\n"
        "        </element1>\n"
        "        <element2>true</element2>\n"
        "        <element2>true</element2>\n"
        "        <element3>1.5</element3>\n"
        "        <element3>1.5</element3>\n"
        "        <element4>FF0001</element4>\n"
        "        <element4>FF0001</element4>\n"
        "        <element5>-980123</element5>\n"
        "        <element5>-980123</element5>\n"
        "        <element6>2012-08-18T13:25:00.000+00:00</element6>\n"
        "        <element6>2012-08-18T13:25:00.000+00:00</element6>\n"
        "       </element5>\n"
        "       <element6>LONDON</element6>\n"
        "       <element6>LONDON</element6>\n"
        "      </element1>\n"
        "      <element2>\n"
        "       <selection2>1.5</selection2>\n"
        "      </element2>\n"
        "      <element2>\n"
        "       <selection2>1.5</selection2>\n"
        "      </element2>\n"
        "      <element4>-980123</element4>\n"
        "      <element8>true</element8>\n"
        "      <element9></element9>\n"
        "      <element10>1.5</element10>\n"
        "      <element11>FF0001</element11>\n"
        "      <element12>-980123</element12>\n"
        "      <element13>LONDON</element13>\n"
        "      <element17>-980123</element17>\n"
        "      <element17>-980123</element17>\n"
        "     </selection3>\n"
        "    </element2>\n"
        "    <element3>FF0001</element3>\n"
        "    <element4>-980123</element4>\n"
        "    <element5>2012-08-18T13:25:00.000+00:00</element5>\n"
        "    <element6>custom</element6>\n"
        "    <element7>LONDON</element7>\n"
        "    <element8>true</element8>\n"
        "    <element9>arbitrary string value</element9>\n"
        "    <element10>1.5</element10>\n"
        "    <element11>FF0001</element11>\n"
        "    <element12>2</element12>\n"
        "    <element13>LONDON</element13>\n"
        "    <element14>true</element14>\n"
        "    <element14>true</element14>\n"
        "    <element15>1.5</element15>\n"
        "    <element15>1.5</element15>\n"
        "    <element16>FF0001</element16>\n"
        "    <element16>FF0001</element16>\n"
        "    <element17>-980123</element17>\n"
        "    <element17>-980123</element17>\n"
        "    <element18>2012-08-18T13:25:00.000+00:00</element18>\n"
        "    <element18>2012-08-18T13:25:00.000+00:00</element18>\n"
        "    <element19>custom</element19>\n"
        "    <element19>custom</element19>\n"
        "   </selection3>\n"
        "  </element4>\n"
        "  <element4>\n"
        "   <selection3>\n"
        "    <element1>\n"
        "     <element1>LONDON</element1>\n"
        "     <element1>LONDON</element1>\n"
        "     <element2></element2>\n"
        "     <element2></element2>\n"
        "     <element3>true</element3>\n"
        "     <element4></element4>\n"
        "     <element5>\n"
        "      <element1>\n"
        "       <element1>LONDON</element1>\n"
        "       <element1>LONDON</element1>\n"
        "       <element2></element2>\n"
        "       <element2></element2>\n"
        "       <element3>true</element3>\n"
        "       <element4></element4>\n"
        "       <element5>\n"
        "        <element1>\n"
        "         <element1>LONDON</element1>\n"
        "         <element1>LONDON</element1>\n"
        "         <element2></element2>\n"
        "         <element2></element2>\n"
        "         <element3>true</element3>\n"
        "         <element4></element4>\n"
        "         <element6>LONDON</element6>\n"
        "         <element6>LONDON</element6>\n"
        "        </element1>\n"
        "        <element2>true</element2>\n"
        "        <element2>true</element2>\n"
        "        <element3>1.5</element3>\n"
        "        <element3>1.5</element3>\n"
        "        <element4>FF0001</element4>\n"
        "        <element4>FF0001</element4>\n"
        "        <element5 xsi:nil=\"true\"/>\n"
        "        <element5 xsi:nil=\"true\"/>\n"
        "        <element6>2012-08-18T13:25:00.000+00:00</element6>\n"
        "        <element6>2012-08-18T13:25:00.000+00:00</element6>\n"
        "       </element5>\n"
        "       <element6>LONDON</element6>\n"
        "       <element6>LONDON</element6>\n"
        "      </element1>\n"
        "      <element2>true</element2>\n"
        "      <element2>true</element2>\n"
        "      <element3>1.5</element3>\n"
        "      <element3>1.5</element3>\n"
        "      <element4>FF0001</element4>\n"
        "      <element4>FF0001</element4>\n"
        "      <element5 xsi:nil=\"true\"/>\n"
        "      <element5 xsi:nil=\"true\"/>\n"
        "      <element6>2012-08-18T13:25:00.000+00:00</element6>\n"
        "      <element6>2012-08-18T13:25:00.000+00:00</element6>\n"
        "     </element5>\n"
        "     <element6>LONDON</element6>\n"
        "     <element6>LONDON</element6>\n"
        "    </element1>\n"
        "    <element1>\n"
        "     <element1>LONDON</element1>\n"
        "     <element1>LONDON</element1>\n"
        "     <element2></element2>\n"
        "     <element2></element2>\n"
        "     <element3>true</element3>\n"
        "     <element4></element4>\n"
        "     <element5>\n"
        "      <element1>\n"
        "       <element1>LONDON</element1>\n"
        "       <element1>LONDON</element1>\n"
        "       <element2></element2>\n"
        "       <element2></element2>\n"
        "       <element3>true</element3>\n"
        "       <element4></element4>\n"
        "       <element5>\n"
        "        <element1>\n"
        "         <element1>LONDON</element1>\n"
        "         <element1>LONDON</element1>\n"
        "         <element2></element2>\n"
        "         <element2></element2>\n"
        "         <element3>true</element3>\n"
        "         <element4></element4>\n"
        "         <element6>LONDON</element6>\n"
        "         <element6>LONDON</element6>\n"
        "        </element1>\n"
        "        <element2>true</element2>\n"
        "        <element2>true</element2>\n"
        "        <element3>1.5</element3>\n"
        "        <element3>1.5</element3>\n"
        "        <element4>FF0001</element4>\n"
        "        <element4>FF0001</element4>\n"
        "        <element5 xsi:nil=\"true\"/>\n"
        "        <element5 xsi:nil=\"true\"/>\n"
        "        <element6>2012-08-18T13:25:00.000+00:00</element6>\n"
        "        <element6>2012-08-18T13:25:00.000+00:00</element6>\n"
        "       </element5>\n"
        "       <element6>LONDON</element6>\n"
        "       <element6>LONDON</element6>\n"
        "      </element1>\n"
        "      <element2>true</element2>\n"
        "      <element2>true</element2>\n"
        "      <element3>1.5</element3>\n"
        "      <element3>1.5</element3>\n"
        "      <element4>FF0001</element4>\n"
        "      <element4>FF0001</element4>\n"
        "      <element5 xsi:nil=\"true\"/>\n"
        "      <element5 xsi:nil=\"true\"/>\n"
        "      <element6>2012-08-18T13:25:00.000+00:00</element6>\n"
        "      <element6>2012-08-18T13:25:00.000+00:00</element6>\n"
        "     </element5>\n"
        "     <element6>LONDON</element6>\n"
        "     <element6>LONDON</element6>\n"
        "    </element1>\n"
        "    <element2>\n"
        "     <selection3>\n"
        "      <element1>\n"
        "       <element2></element2>\n"
        "       <element2></element2>\n"
        "       <element4></element4>\n"
        "       <element5>\n"
        "        <element1>\n"
        "         <element2></element2>\n"
        "         <element2></element2>\n"
        "         <element4></element4>\n"
        "         <element5>\n"
        "          <element1>\n"
        "           <element2></element2>\n"
        "           <element2></element2>\n"
        "           <element4></element4>\n"
        "           <element6>LONDON</element6>\n"
        "           <element6>LONDON</element6>\n"
        "          </element1>\n"
        "          <element2>true</element2>\n"
        "          <element2>true</element2>\n"
        "          <element3>1.5</element3>\n"
        "          <element3>1.5</element3>\n"
        "          <element4>FF0001</element4>\n"
        "          <element4>FF0001</element4>\n"
        "          <element5>-980123</element5>\n"
        "          <element5>-980123</element5>\n"
        "          <element6>2012-08-18T13:25:00.000+00:00</element6>\n"
        "          <element6>2012-08-18T13:25:00.000+00:00</element6>\n"
        "         </element5>\n"
        "         <element6>LONDON</element6>\n"
        "         <element6>LONDON</element6>\n"
        "        </element1>\n"
        "        <element2>true</element2>\n"
        "        <element2>true</element2>\n"
        "        <element3>1.5</element3>\n"
        "        <element3>1.5</element3>\n"
        "        <element4>FF0001</element4>\n"
        "        <element4>FF0001</element4>\n"
        "        <element5>-980123</element5>\n"
        "        <element5>-980123</element5>\n"
        "        <element6>2012-08-18T13:25:00.000+00:00</element6>\n"
        "        <element6>2012-08-18T13:25:00.000+00:00</element6>\n"
        "       </element5>\n"
        "       <element6>LONDON</element6>\n"
        "       <element6>LONDON</element6>\n"
        "      </element1>\n"
        "      <element1>\n"
        "       <element2></element2>\n"
        "       <element2></element2>\n"
        "       <element4></element4>\n"
        "       <element5>\n"
        "        <element1>\n"
        "         <element2></element2>\n"
        "         <element2></element2>\n"
        "         <element4></element4>\n"
        "         <element5>\n"
        "          <element1>\n"
        "           <element2></element2>\n"
        "           <element2></element2>\n"
        "           <element4></element4>\n"
        "           <element6>LONDON</element6>\n"
        "           <element6>LONDON</element6>\n"
        "          </element1>\n"
        "          <element2>true</element2>\n"
        "          <element2>true</element2>\n"
        "          <element3>1.5</element3>\n"
        "          <element3>1.5</element3>\n"
        "          <element4>FF0001</element4>\n"
        "          <element4>FF0001</element4>\n"
        "          <element5>-980123</element5>\n"
        "          <element5>-980123</element5>\n"
        "          <element6>2012-08-18T13:25:00.000+00:00</element6>\n"
        "          <element6>2012-08-18T13:25:00.000+00:00</element6>\n"
        "         </element5>\n"
        "         <element6>LONDON</element6>\n"
        "         <element6>LONDON</element6>\n"
        "        </element1>\n"
        "        <element2>true</element2>\n"
        "        <element2>true</element2>\n"
        "        <element3>1.5</element3>\n"
        "        <element3>1.5</element3>\n"
        "        <element4>FF0001</element4>\n"
        "        <element4>FF0001</element4>\n"
        "        <element5>-980123</element5>\n"
        "        <element5>-980123</element5>\n"
        "        <element6>2012-08-18T13:25:00.000+00:00</element6>\n"
        "        <element6>2012-08-18T13:25:00.000+00:00</element6>\n"
        "       </element5>\n"
        "       <element6>LONDON</element6>\n"
        "       <element6>LONDON</element6>\n"
        "      </element1>\n"
        "      <element2>\n"
        "       <selection2>1.5</selection2>\n"
        "      </element2>\n"
        "      <element2>\n"
        "       <selection2>1.5</selection2>\n"
        "      </element2>\n"
        "      <element4>-980123</element4>\n"
        "      <element8>true</element8>\n"
        "      <element9></element9>\n"
        "      <element10>1.5</element10>\n"
        "      <element11>FF0001</element11>\n"
        "      <element12>-980123</element12>\n"
        "      <element13>LONDON</element13>\n"
        "      <element17>-980123</element17>\n"
        "      <element17>-980123</element17>\n"
        "     </selection3>\n"
        "    </element2>\n"
        "    <element2>\n"
        "     <selection3>\n"
        "      <element1>\n"
        "       <element2></element2>\n"
        "       <element2></element2>\n"
        "       <element4></element4>\n"
        "       <element5>\n"
        "        <element1>\n"
        "         <element2></element2>\n"
        "         <element2></element2>\n"
        "         <element4></element4>\n"
        "         <element5>\n"
        "          <element1>\n"
        "           <element2></element2>\n"
        "           <element2></element2>\n"
        "           <element4></element4>\n"
        "           <element6>LONDON</element6>\n"
        "           <element6>LONDON</element6>\n"
        "          </element1>\n"
        "          <element2>true</element2>\n"
        "          <element2>true</element2>\n"
        "          <element3>1.5</element3>\n"
        "          <element3>1.5</element3>\n"
        "          <element4>FF0001</element4>\n"
        "          <element4>FF0001</element4>\n"
        "          <element5>-980123</element5>\n"
        "          <element5>-980123</element5>\n"
        "          <element6>2012-08-18T13:25:00.000+00:00</element6>\n"
        "          <element6>2012-08-18T13:25:00.000+00:00</element6>\n"
        "         </element5>\n"
        "         <element6>LONDON</element6>\n"
        "         <element6>LONDON</element6>\n"
        "        </element1>\n"
        "        <element2>true</element2>\n"
        "        <element2>true</element2>\n"
        "        <element3>1.5</element3>\n"
        "        <element3>1.5</element3>\n"
        "        <element4>FF0001</element4>\n"
        "        <element4>FF0001</element4>\n"
        "        <element5>-980123</element5>\n"
        "        <element5>-980123</element5>\n"
        "        <element6>2012-08-18T13:25:00.000+00:00</element6>\n"
        "        <element6>2012-08-18T13:25:00.000+00:00</element6>\n"
        "       </element5>\n"
        "       <element6>LONDON</element6>\n"
        "       <element6>LONDON</element6>\n"
        "      </element1>\n"
        "      <element1>\n"
        "       <element2></element2>\n"
        "       <element2></element2>\n"
        "       <element4></element4>\n"
        "       <element5>\n"
        "        <element1>\n"
        "         <element2></element2>\n"
        "         <element2></element2>\n"
        "         <element4></element4>\n"
        "         <element5>\n"
        "          <element1>\n"
        "           <element2></element2>\n"
        "           <element2></element2>\n"
        "           <element4></element4>\n"
        "           <element6>LONDON</element6>\n"
        "           <element6>LONDON</element6>\n"
        "          </element1>\n"
        "          <element2>true</element2>\n"
        "          <element2>true</element2>\n"
        "          <element3>1.5</element3>\n"
        "          <element3>1.5</element3>\n"
        "          <element4>FF0001</element4>\n"
        "          <element4>FF0001</element4>\n"
        "          <element5>-980123</element5>\n"
        "          <element5>-980123</element5>\n"
        "          <element6>2012-08-18T13:25:00.000+00:00</element6>\n"
        "          <element6>2012-08-18T13:25:00.000+00:00</element6>\n"
        "         </element5>\n"
        "         <element6>LONDON</element6>\n"
        "         <element6>LONDON</element6>\n"
        "        </element1>\n"
        "        <element2>true</element2>\n"
        "        <element2>true</element2>\n"
        "        <element3>1.5</element3>\n"
        "        <element3>1.5</element3>\n"
        "        <element4>FF0001</element4>\n"
        "        <element4>FF0001</element4>\n"
        "        <element5>-980123</element5>\n"
        "        <element5>-980123</element5>\n"
        "        <element6>2012-08-18T13:25:00.000+00:00</element6>\n"
        "        <element6>2012-08-18T13:25:00.000+00:00</element6>\n"
        "       </element5>\n"
        "       <element6>LONDON</element6>\n"
        "       <element6>LONDON</element6>\n"
        "      </element1>\n"
        "      <element2>\n"
        "       <selection2>1.5</selection2>\n"
        "      </element2>\n"
        "      <element2>\n"
        "       <selection2>1.5</selection2>\n"
        "      </element2>\n"
        "      <element4>-980123</element4>\n"
        "      <element8>true</element8>\n"
        "      <element9></element9>\n"
        "      <element10>1.5</element10>\n"
        "      <element11>FF0001</element11>\n"
        "      <element12>-980123</element12>\n"
        "      <element13>LONDON</element13>\n"
        "      <element17>-980123</element17>\n"
        "      <element17>-980123</element17>\n"
        "     </selection3>\n"
        "    </element2>\n"
        "    <element3>FF0001</element3>\n"
        "    <element4>-980123</element4>\n"
        "    <element5>2012-08-18T13:25:00.000+00:00</element5>\n"
        "    <element6>custom</element6>\n"
        "    <element7>LONDON</element7>\n"
        "    <element8>true</element8>\n"
        "    <element9>arbitrary string value</element9>\n"
        "    <element10>1.5</element10>\n"
        "    <element11>FF0001</element11>\n"
        "    <element12>2</element12>\n"
        "    <element13>LONDON</element13>\n"
        "    <element14>true</element14>\n"
        "    <element14>true</element14>\n"
        "    <element15>1.5</element15>\n"
        "    <element15>1.5</element15>\n"
        "    <element16>FF0001</element16>\n"
        "    <element16>FF0001</element16>\n"
        "    <element17>-980123</element17>\n"
        "    <element17>-980123</element17>\n"
        "    <element18>2012-08-18T13:25:00.000+00:00</element18>\n"
        "    <element18>2012-08-18T13:25:00.000+00:00</element18>\n"
        "    <element19>custom</element19>\n"
        "    <element19>custom</element19>\n"
        "   </selection3>\n"
        "  </element4>\n"
        "  <element5>\n"
        "   <selection1>\n"
        "    <element4>3123123123</element4>\n"
        "    <element5>255</element5>\n"
        "    <element7>custom</element7>\n"
        "    <element8>999</element8>\n"
        "   </selection1>\n"
        "  </element5>\n"
        "  <element5>\n"
        "   <selection1>\n"
        "    <element4>3123123123</element4>\n"
        "    <element5>255</element5>\n"
        "    <element7>custom</element7>\n"
        "    <element8>999</element8>\n"
        "   </selection1>\n"
        "  </element5>\n"
        " </selection1>\n"
        "</Topchoice>\n",

        "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
        "<Topchoice xmlns=\"TestNamespace\"\n"
        " xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\">\n"
        " <selection1>\n"
        "  <element2>\n"
        "   <selection4>\n"
        "    <selection1>true</selection1>\n"
        "   </selection4>\n"
        "  </element2>\n"
        "  <element2>\n"
        "   <selection4>\n"
        "    <selection1>true</selection1>\n"
        "   </selection4>\n"
        "  </element2>\n"
        "  <element3>\n"
        "   <selection3>\n"
        "    <selection3>\n"
        "     <element1>\n"
        "      <element1>LONDON</element1>\n"
        "      <element1>LONDON</element1>\n"
        "      <element3>true</element3>\n"
        "      <element5>\n"
        "       <element1>\n"
        "        <element1>LONDON</element1>\n"
        "        <element1>LONDON</element1>\n"
        "        <element3>true</element3>\n"
        "        <element5>\n"
        "         <element1>\n"
        "          <element1>LONDON</element1>\n"
        "          <element1>LONDON</element1>\n"
        "          <element3>true</element3>\n"
        "         </element1>\n"
        "         <element5>2</element5>\n"
        "         <element5>2</element5>\n"
        "        </element5>\n"
        "       </element1>\n"
        "       <element5>2</element5>\n"
        "       <element5>2</element5>\n"
        "      </element5>\n"
        "     </element1>\n"
        "     <element1>\n"
        "      <element1>LONDON</element1>\n"
        "      <element1>LONDON</element1>\n"
        "      <element3>true</element3>\n"
        "      <element5>\n"
        "       <element1>\n"
        "        <element1>LONDON</element1>\n"
        "        <element1>LONDON</element1>\n"
        "        <element3>true</element3>\n"
        "        <element5>\n"
        "         <element1>\n"
        "          <element1>LONDON</element1>\n"
        "          <element1>LONDON</element1>\n"
        "          <element3>true</element3>\n"
        "         </element1>\n"
        "         <element5>2</element5>\n"
        "         <element5>2</element5>\n"
        "        </element5>\n"
        "       </element1>\n"
        "       <element5>2</element5>\n"
        "       <element5>2</element5>\n"
        "      </element5>\n"
        "     </element1>\n"
        "     <element2>\n"
        "      <selection3>\n"
        "       <element8>true</element8>\n"
        "       <element9></element9>\n"
        "       <element10>1.5</element10>\n"
        "       <element11>FF0001</element11>\n"
        "       <element12>-980123</element12>\n"
        "       <element13>LONDON</element13>\n"
        "      </selection3>\n"
        "     </element2>\n"
        "     <element2>\n"
        "      <selection3>\n"
        "       <element8>true</element8>\n"
        "       <element9></element9>\n"
        "       <element10>1.5</element10>\n"
        "       <element11>FF0001</element11>\n"
        "       <element12>-980123</element12>\n"
        "       <element13>LONDON</element13>\n"
        "      </selection3>\n"
        "     </element2>\n"
        "     <element3>FF0001</element3>\n"
        "     <element5>2012-08-18T13:25:00.000+00:00</element5>\n"
        "     <element6>custom</element6>\n"
        "     <element7>LONDON</element7>\n"
        "     <element8>true</element8>\n"
        "     <element9>arbitrary string value</element9>\n"
        "     <element10>1.5</element10>\n"
        "     <element11>FF0001</element11>\n"
        "     <element12>2</element12>\n"
        "     <element13>LONDON</element13>\n"
        "     <element14>true</element14>\n"
        "     <element14>true</element14>\n"
        "     <element15>1.5</element15>\n"
        "     <element15>1.5</element15>\n"
        "     <element16>FF0001</element16>\n"
        "     <element16>FF0001</element16>\n"
        "     <element18>2012-08-18T13:25:00.000+00:00</element18>\n"
        "     <element18>2012-08-18T13:25:00.000+00:00</element18>\n"
        "     <element19>custom</element19>\n"
        "     <element19>custom</element19>\n"
        "    </selection3>\n"
        "   </selection3>\n"
        "  </element3>\n"
        "  <element4>\n"
        "   <selection4>\n"
        "    <selection1>true</selection1>\n"
        "   </selection4>\n"
        "  </element4>\n"
        "  <element4>\n"
        "   <selection4>\n"
        "    <selection1>true</selection1>\n"
        "   </selection4>\n"
        "  </element4>\n"
        " </selection1>\n"
        "</Topchoice>\n",

        "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
        "<Topchoice xmlns=\"TestNamespace\"\n"
        " xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\">\n"
        " <selection1>\n"
        "  <element1>\n"
        "   <selection1>\n"
        "    <element1>0</element1>\n"
        "    <element2>custom</element2>\n"
        "    <element3>999</element3>\n"
        "    <element4>3123123123</element4>\n"
        "    <element5>0</element5>\n"
        "    <element6 xsi:nil=\"true\"/>\n"
        "    <element6 xsi:nil=\"true\"/>\n"
        "    <element7>custom</element7>\n"
        "    <element8>999</element8>\n"
        "    <element9>3123123123</element9>\n"
        "    <element10>0</element10>\n"
        "    <element10>0</element10>\n"
        "    <element11>custom</element11>\n"
        "    <element11>custom</element11>\n"
        "    <element12>3123123123</element12>\n"
        "    <element12>3123123123</element12>\n"
        "    <element13 xsi:nil=\"true\"/>\n"
        "    <element13 xsi:nil=\"true\"/>\n"
        "    <element14>999</element14>\n"
        "    <element14>999</element14>\n"
        "    <element15 xsi:nil=\"true\"/>\n"
        "    <element15 xsi:nil=\"true\"/>\n"
        "   </selection1>\n"
        "  </element1>\n"
        "  <element2>\n"
        "   <selection4>\n"
        "    <selection2>\n"
        "    </selection2>\n"
        "   </selection4>\n"
        "  </element2>\n"
        "  <element2>\n"
        "   <selection4>\n"
        "    <selection2>\n"
        "    </selection2>\n"
        "   </selection4>\n"
        "  </element2>\n"
        "  <element3>\n"
        "   <selection3>\n"
        "    <selection3>\n"
        "     <element1>\n"
        "      <element2>arbitrary string value</element2>\n"
        "      <element2>arbitrary string value</element2>\n"
        "      <element4>arbitrary string value</element4>\n"
        "      <element5>\n"
        "       <element1>\n"
        "        <element2>arbitrary string value</element2>\n"
        "        <element2>arbitrary string value</element2>\n"
        "        <element4>arbitrary string value</element4>\n"
        "        <element5>\n"
        "         <element1>\n"
        "          <element2>arbitrary string value</element2>\n"
        "          <element2>arbitrary string value</element2>\n"
        "          <element4>arbitrary string value</element4>\n"
        "          <element6 xsi:nil=\"true\"/>\n"
        "          <element6 xsi:nil=\"true\"/>\n"
        "         </element1>\n"
        "         <element2 xsi:nil=\"true\"/>\n"
        "         <element2 xsi:nil=\"true\"/>\n"
        "         <element3 xsi:nil=\"true\"/>\n"
        "         <element3 xsi:nil=\"true\"/>\n"
        "         <element4 xsi:nil=\"true\"/>\n"
        "         <element4 xsi:nil=\"true\"/>\n"
        "         <element6 xsi:nil=\"true\"/>\n"
        "         <element6 xsi:nil=\"true\"/>\n"
        "        </element5>\n"
        "        <element6 xsi:nil=\"true\"/>\n"
        "        <element6 xsi:nil=\"true\"/>\n"
        "       </element1>\n"
        "       <element2 xsi:nil=\"true\"/>\n"
        "       <element2 xsi:nil=\"true\"/>\n"
        "       <element3 xsi:nil=\"true\"/>\n"
        "       <element3 xsi:nil=\"true\"/>\n"
        "       <element4 xsi:nil=\"true\"/>\n"
        "       <element4 xsi:nil=\"true\"/>\n"
        "       <element6 xsi:nil=\"true\"/>\n"
        "       <element6 xsi:nil=\"true\"/>\n"
        "      </element5>\n"
        "      <element6 xsi:nil=\"true\"/>\n"
        "      <element6 xsi:nil=\"true\"/>\n"
        "     </element1>\n"
        "     <element1>\n"
        "      <element2>arbitrary string value</element2>\n"
        "      <element2>arbitrary string value</element2>\n"
        "      <element4>arbitrary string value</element4>\n"
        "      <element5>\n"
        "       <element1>\n"
        "        <element2>arbitrary string value</element2>\n"
        "        <element2>arbitrary string value</element2>\n"
        "        <element4>arbitrary string value</element4>\n"
        "        <element5>\n"
        "         <element1>\n"
        "          <element2>arbitrary string value</element2>\n"
        "          <element2>arbitrary string value</element2>\n"
        "          <element4>arbitrary string value</element4>\n"
        "          <element6 xsi:nil=\"true\"/>\n"
        "          <element6 xsi:nil=\"true\"/>\n"
        "         </element1>\n"
        "         <element2 xsi:nil=\"true\"/>\n"
        "         <element2 xsi:nil=\"true\"/>\n"
        "         <element3 xsi:nil=\"true\"/>\n"
        "         <element3 xsi:nil=\"true\"/>\n"
        "         <element4 xsi:nil=\"true\"/>\n"
        "         <element4 xsi:nil=\"true\"/>\n"
        "         <element6 xsi:nil=\"true\"/>\n"
        "         <element6 xsi:nil=\"true\"/>\n"
        "        </element5>\n"
        "        <element6 xsi:nil=\"true\"/>\n"
        "        <element6 xsi:nil=\"true\"/>\n"
        "       </element1>\n"
        "       <element2 xsi:nil=\"true\"/>\n"
        "       <element2 xsi:nil=\"true\"/>\n"
        "       <element3 xsi:nil=\"true\"/>\n"
        "       <element3 xsi:nil=\"true\"/>\n"
        "       <element4 xsi:nil=\"true\"/>\n"
        "       <element4 xsi:nil=\"true\"/>\n"
        "       <element6 xsi:nil=\"true\"/>\n"
        "       <element6 xsi:nil=\"true\"/>\n"
        "      </element5>\n"
        "      <element6 xsi:nil=\"true\"/>\n"
        "      <element6 xsi:nil=\"true\"/>\n"
        "     </element1>\n"
        "     <element2>\n"
        "      <selection3>\n"
        "       <element1>\n"
        "        <element1>LONDON</element1>\n"
        "        <element1>LONDON</element1>\n"
        "        <element2>arbitrary string value</element2>\n"
        "        <element2>arbitrary string value</element2>\n"
        "        <element3>true</element3>\n"
        "        <element4>arbitrary string value</element4>\n"
        "        <element5>\n"
        "         <element1>\n"
        "          <element1>LONDON</element1>\n"
        "          <element1>LONDON</element1>\n"
        "          <element2>arbitrary string value</element2>\n"
        "          <element2>arbitrary string value</element2>\n"
        "          <element3>true</element3>\n"
        "          <element4>arbitrary string value</element4>\n"
        "          <element5>\n"
        "           <element1>\n"
        "            <element1>LONDON</element1>\n"
        "            <element1>LONDON</element1>\n"
        "            <element2>arbitrary string value</element2>\n"
        "            <element2>arbitrary string value</element2>\n"
        "            <element3>true</element3>\n"
        "            <element4>arbitrary string value</element4>\n"
        "            <element6 xsi:nil=\"true\"/>\n"
        "            <element6 xsi:nil=\"true\"/>\n"
        "           </element1>\n"
        "           <element2 xsi:nil=\"true\"/>\n"
        "           <element2 xsi:nil=\"true\"/>\n"
        "           <element3 xsi:nil=\"true\"/>\n"
        "           <element3 xsi:nil=\"true\"/>\n"
        "           <element4 xsi:nil=\"true\"/>\n"
        "           <element4 xsi:nil=\"true\"/>\n"
        "           <element5 xsi:nil=\"true\"/>\n"
        "           <element5 xsi:nil=\"true\"/>\n"
        "           <element6 xsi:nil=\"true\"/>\n"
        "           <element6 xsi:nil=\"true\"/>\n"
        "          </element5>\n"
        "          <element6 xsi:nil=\"true\"/>\n"
        "          <element6 xsi:nil=\"true\"/>\n"
        "         </element1>\n"
        "         <element2 xsi:nil=\"true\"/>\n"
        "         <element2 xsi:nil=\"true\"/>\n"
        "         <element3 xsi:nil=\"true\"/>\n"
        "         <element3 xsi:nil=\"true\"/>\n"
        "         <element4 xsi:nil=\"true\"/>\n"
        "         <element4 xsi:nil=\"true\"/>\n"
        "         <element5 xsi:nil=\"true\"/>\n"
        "         <element5 xsi:nil=\"true\"/>\n"
        "         <element6 xsi:nil=\"true\"/>\n"
        "         <element6 xsi:nil=\"true\"/>\n"
        "        </element5>\n"
        "        <element6 xsi:nil=\"true\"/>\n"
        "        <element6 xsi:nil=\"true\"/>\n"
        "       </element1>\n"
        "       <element1>\n"
        "        <element1>LONDON</element1>\n"
        "        <element1>LONDON</element1>\n"
        "        <element2>arbitrary string value</element2>\n"
        "        <element2>arbitrary string value</element2>\n"
        "        <element3>true</element3>\n"
        "        <element4>arbitrary string value</element4>\n"
        "        <element5>\n"
        "         <element1>\n"
        "          <element1>LONDON</element1>\n"
        "          <element1>LONDON</element1>\n"
        "          <element2>arbitrary string value</element2>\n"
        "          <element2>arbitrary string value</element2>\n"
        "          <element3>true</element3>\n"
        "          <element4>arbitrary string value</element4>\n"
        "          <element5>\n"
        "           <element1>\n"
        "            <element1>LONDON</element1>\n"
        "            <element1>LONDON</element1>\n"
        "            <element2>arbitrary string value</element2>\n"
        "            <element2>arbitrary string value</element2>\n"
        "            <element3>true</element3>\n"
        "            <element4>arbitrary string value</element4>\n"
        "            <element6 xsi:nil=\"true\"/>\n"
        "            <element6 xsi:nil=\"true\"/>\n"
        "           </element1>\n"
        "           <element2 xsi:nil=\"true\"/>\n"
        "           <element2 xsi:nil=\"true\"/>\n"
        "           <element3 xsi:nil=\"true\"/>\n"
        "           <element3 xsi:nil=\"true\"/>\n"
        "           <element4 xsi:nil=\"true\"/>\n"
        "           <element4 xsi:nil=\"true\"/>\n"
        "           <element5 xsi:nil=\"true\"/>\n"
        "           <element5 xsi:nil=\"true\"/>\n"
        "           <element6 xsi:nil=\"true\"/>\n"
        "           <element6 xsi:nil=\"true\"/>\n"
        "          </element5>\n"
        "          <element6 xsi:nil=\"true\"/>\n"
        "          <element6 xsi:nil=\"true\"/>\n"
        "         </element1>\n"
        "         <element2 xsi:nil=\"true\"/>\n"
        "         <element2 xsi:nil=\"true\"/>\n"
        "         <element3 xsi:nil=\"true\"/>\n"
        "         <element3 xsi:nil=\"true\"/>\n"
        "         <element4 xsi:nil=\"true\"/>\n"
        "         <element4 xsi:nil=\"true\"/>\n"
        "         <element5 xsi:nil=\"true\"/>\n"
        "         <element5 xsi:nil=\"true\"/>\n"
        "         <element6 xsi:nil=\"true\"/>\n"
        "         <element6 xsi:nil=\"true\"/>\n"
        "        </element5>\n"
        "        <element6 xsi:nil=\"true\"/>\n"
        "        <element6 xsi:nil=\"true\"/>\n"
        "       </element1>\n"
        "       <element2>\n"
        "        <selection1>2</selection1>\n"
        "       </element2>\n"
        "       <element2>\n"
        "        <selection1>2</selection1>\n"
        "       </element2>\n"
        "       <element3>FF0001</element3>\n"
        "       <element4>2</element4>\n"
        "       <element5>2012-08-18T13:25:00.000+00:00</element5>\n"
        "       <element6>custom</element6>\n"
        "       <element7>LONDON</element7>\n"
        "       <element8>true</element8>\n"
        "       <element9>arbitrary string value</element9>\n"
        "       <element10>1.5</element10>\n"
        "       <element11>FF0001</element11>\n"
        "       <element12>2</element12>\n"
        "       <element13>LONDON</element13>\n"
        "       <element14>true</element14>\n"
        "       <element14>true</element14>\n"
        "       <element15>1.5</element15>\n"
        "       <element15>1.5</element15>\n"
        "       <element16>FF0001</element16>\n"
        "       <element16>FF0001</element16>\n"
        "       <element17>2</element17>\n"
        "       <element17>2</element17>\n"
        "       <element18>2012-08-18T13:25:00.000+00:00</element18>\n"
        "       <element18>2012-08-18T13:25:00.000+00:00</element18>\n"
        "       <element19>custom</element19>\n"
        "       <element19>custom</element19>\n"
        "      </selection3>\n"
        "     </element2>\n"
        "     <element2>\n"
        "      <selection3>\n"
        "       <element1>\n"
        "        <element1>LONDON</element1>\n"
        "        <element1>LONDON</element1>\n"
        "        <element2>arbitrary string value</element2>\n"
        "        <element2>arbitrary string value</element2>\n"
        "        <element3>true</element3>\n"
        "        <element4>arbitrary string value</element4>\n"
        "        <element5>\n"
        "         <element1>\n"
        "          <element1>LONDON</element1>\n"
        "          <element1>LONDON</element1>\n"
        "          <element2>arbitrary string value</element2>\n"
        "          <element2>arbitrary string value</element2>\n"
        "          <element3>true</element3>\n"
        "          <element4>arbitrary string value</element4>\n"
        "          <element5>\n"
        "           <element1>\n"
        "            <element1>LONDON</element1>\n"
        "            <element1>LONDON</element1>\n"
        "            <element2>arbitrary string value</element2>\n"
        "            <element2>arbitrary string value</element2>\n"
        "            <element3>true</element3>\n"
        "            <element4>arbitrary string value</element4>\n"
        "            <element6 xsi:nil=\"true\"/>\n"
        "            <element6 xsi:nil=\"true\"/>\n"
        "           </element1>\n"
        "           <element2 xsi:nil=\"true\"/>\n"
        "           <element2 xsi:nil=\"true\"/>\n"
        "           <element3 xsi:nil=\"true\"/>\n"
        "           <element3 xsi:nil=\"true\"/>\n"
        "           <element4 xsi:nil=\"true\"/>\n"
        "           <element4 xsi:nil=\"true\"/>\n"
        "           <element5 xsi:nil=\"true\"/>\n"
        "           <element5 xsi:nil=\"true\"/>\n"
        "           <element6 xsi:nil=\"true\"/>\n"
        "           <element6 xsi:nil=\"true\"/>\n"
        "          </element5>\n"
        "          <element6 xsi:nil=\"true\"/>\n"
        "          <element6 xsi:nil=\"true\"/>\n"
        "         </element1>\n"
        "         <element2 xsi:nil=\"true\"/>\n"
        "         <element2 xsi:nil=\"true\"/>\n"
        "         <element3 xsi:nil=\"true\"/>\n"
        "         <element3 xsi:nil=\"true\"/>\n"
        "         <element4 xsi:nil=\"true\"/>\n"
        "         <element4 xsi:nil=\"true\"/>\n"
        "         <element5 xsi:nil=\"true\"/>\n"
        "         <element5 xsi:nil=\"true\"/>\n"
        "         <element6 xsi:nil=\"true\"/>\n"
        "         <element6 xsi:nil=\"true\"/>\n"
        "        </element5>\n"
        "        <element6 xsi:nil=\"true\"/>\n"
        "        <element6 xsi:nil=\"true\"/>\n"
        "       </element1>\n"
        "       <element1>\n"
        "        <element1>LONDON</element1>\n"
        "        <element1>LONDON</element1>\n"
        "        <element2>arbitrary string value</element2>\n"
        "        <element2>arbitrary string value</element2>\n"
        "        <element3>true</element3>\n"
        "        <element4>arbitrary string value</element4>\n"
        "        <element5>\n"
        "         <element1>\n"
        "          <element1>LONDON</element1>\n"
        "          <element1>LONDON</element1>\n"
        "          <element2>arbitrary string value</element2>\n"
        "          <element2>arbitrary string value</element2>\n"
        "          <element3>true</element3>\n"
        "          <element4>arbitrary string value</element4>\n"
        "          <element5>\n"
        "           <element1>\n"
        "            <element1>LONDON</element1>\n"
        "            <element1>LONDON</element1>\n"
        "            <element2>arbitrary string value</element2>\n"
        "            <element2>arbitrary string value</element2>\n"
        "            <element3>true</element3>\n"
        "            <element4>arbitrary string value</element4>\n"
        "            <element6 xsi:nil=\"true\"/>\n"
        "            <element6 xsi:nil=\"true\"/>\n"
        "           </element1>\n"
        "           <element2 xsi:nil=\"true\"/>\n"
        "           <element2 xsi:nil=\"true\"/>\n"
        "           <element3 xsi:nil=\"true\"/>\n"
        "           <element3 xsi:nil=\"true\"/>\n"
        "           <element4 xsi:nil=\"true\"/>\n"
        "           <element4 xsi:nil=\"true\"/>\n"
        "           <element5 xsi:nil=\"true\"/>\n"
        "           <element5 xsi:nil=\"true\"/>\n"
        "           <element6 xsi:nil=\"true\"/>\n"
        "           <element6 xsi:nil=\"true\"/>\n"
        "          </element5>\n"
        "          <element6 xsi:nil=\"true\"/>\n"
        "          <element6 xsi:nil=\"true\"/>\n"
        "         </element1>\n"
        "         <element2 xsi:nil=\"true\"/>\n"
        "         <element2 xsi:nil=\"true\"/>\n"
        "         <element3 xsi:nil=\"true\"/>\n"
        "         <element3 xsi:nil=\"true\"/>\n"
        "         <element4 xsi:nil=\"true\"/>\n"
        "         <element4 xsi:nil=\"true\"/>\n"
        "         <element5 xsi:nil=\"true\"/>\n"
        "         <element5 xsi:nil=\"true\"/>\n"
        "         <element6 xsi:nil=\"true\"/>\n"
        "         <element6 xsi:nil=\"true\"/>\n"
        "        </element5>\n"
        "        <element6 xsi:nil=\"true\"/>\n"
        "        <element6 xsi:nil=\"true\"/>\n"
        "       </element1>\n"
        "       <element2>\n"
        "        <selection1>2</selection1>\n"
        "       </element2>\n"
        "       <element2>\n"
        "        <selection1>2</selection1>\n"
        "       </element2>\n"
        "       <element3>FF0001</element3>\n"
        "       <element4>2</element4>\n"
        "       <element5>2012-08-18T13:25:00.000+00:00</element5>\n"
        "       <element6>custom</element6>\n"
        "       <element7>LONDON</element7>\n"
        "       <element8>true</element8>\n"
        "       <element9>arbitrary string value</element9>\n"
        "       <element10>1.5</element10>\n"
        "       <element11>FF0001</element11>\n"
        "       <element12>2</element12>\n"
        "       <element13>LONDON</element13>\n"
        "       <element14>true</element14>\n"
        "       <element14>true</element14>\n"
        "       <element15>1.5</element15>\n"
        "       <element15>1.5</element15>\n"
        "       <element16>FF0001</element16>\n"
        "       <element16>FF0001</element16>\n"
        "       <element17>2</element17>\n"
        "       <element17>2</element17>\n"
        "       <element18>2012-08-18T13:25:00.000+00:00</element18>\n"
        "       <element18>2012-08-18T13:25:00.000+00:00</element18>\n"
        "       <element19>custom</element19>\n"
        "       <element19>custom</element19>\n"
        "      </selection3>\n"
        "     </element2>\n"
        "     <element4>2</element4>\n"
        "     <element8>true</element8>\n"
        "     <element9></element9>\n"
        "     <element10>1.5</element10>\n"
        "     <element11>FF0001</element11>\n"
        "     <element12>-980123</element12>\n"
        "     <element13>LONDON</element13>\n"
        "     <element17>2</element17>\n"
        "     <element17>2</element17>\n"
        "    </selection3>\n"
        "   </selection3>\n"
        "  </element3>\n"
        "  <element4>\n"
        "   <selection4>\n"
        "    <selection2>\n"
        "    </selection2>\n"
        "   </selection4>\n"
        "  </element4>\n"
        "  <element4>\n"
        "   <selection4>\n"
        "    <selection2>\n"
        "    </selection2>\n"
        "   </selection4>\n"
        "  </element4>\n"
        "  <element5>\n"
        "   <selection1>\n"
        "    <element1>0</element1>\n"
        "    <element2>custom</element2>\n"
        "    <element3>999</element3>\n"
        "    <element4>3123123123</element4>\n"
        "    <element5>0</element5>\n"
        "    <element6 xsi:nil=\"true\"/>\n"
        "    <element6 xsi:nil=\"true\"/>\n"
        "    <element7>custom</element7>\n"
        "    <element8>999</element8>\n"
        "    <element9>3123123123</element9>\n"
        "    <element10>0</element10>\n"
        "    <element10>0</element10>\n"
        "    <element11>custom</element11>\n"
        "    <element11>custom</element11>\n"
        "    <element12>3123123123</element12>\n"
        "    <element12>3123123123</element12>\n"
        "    <element13 xsi:nil=\"true\"/>\n"
        "    <element13 xsi:nil=\"true\"/>\n"
        "    <element14>999</element14>\n"
        "    <element14>999</element14>\n"
        "    <element15 xsi:nil=\"true\"/>\n"
        "    <element15 xsi:nil=\"true\"/>\n"
        "   </selection1>\n"
        "  </element5>\n"
        "  <element5>\n"
        "   <selection1>\n"
        "    <element1>0</element1>\n"
        "    <element2>custom</element2>\n"
        "    <element3>999</element3>\n"
        "    <element4>3123123123</element4>\n"
        "    <element5>0</element5>\n"
        "    <element6 xsi:nil=\"true\"/>\n"
        "    <element6 xsi:nil=\"true\"/>\n"
        "    <element7>custom</element7>\n"
        "    <element8>999</element8>\n"
        "    <element9>3123123123</element9>\n"
        "    <element10>0</element10>\n"
        "    <element10>0</element10>\n"
        "    <element11>custom</element11>\n"
        "    <element11>custom</element11>\n"
        "    <element12>3123123123</element12>\n"
        "    <element12>3123123123</element12>\n"
        "    <element13 xsi:nil=\"true\"/>\n"
        "    <element13 xsi:nil=\"true\"/>\n"
        "    <element14>999</element14>\n"
        "    <element14>999</element14>\n"
        "    <element15 xsi:nil=\"true\"/>\n"
        "    <element15 xsi:nil=\"true\"/>\n"
        "   </selection1>\n"
        "  </element5>\n"
        " </selection1>\n"
        "</Topchoice>\n",

        "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
        "<Topchoice xmlns=\"TestNamespace\"\n"
        " xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\">\n"
        " <selection1>\n"
        "  <element1>\n"
        "   <selection1>\n"
        "    <element1>255</element1>\n"
        "    <element4>3123123123</element4>\n"
        "    <element5>255</element5>\n"
        "    <element6>999</element6>\n"
        "    <element6>999</element6>\n"
        "    <element7>custom</element7>\n"
        "    <element8>999</element8>\n"
        "    <element10>255</element10>\n"
        "    <element10>255</element10>\n"
        "    <element13>255</element13>\n"
        "    <element13>255</element13>\n"
        "    <element15>3123123123</element15>\n"
        "    <element15>3123123123</element15>\n"
        "   </selection1>\n"
        "  </element1>\n"
        "  <element2>\n"
        "   <selection4>\n"
        "    <selection2>arbitrary string value</selection2>\n"
        "   </selection4>\n"
        "  </element2>\n"
        "  <element2>\n"
        "   <selection4>\n"
        "    <selection2>arbitrary string value</selection2>\n"
        "   </selection4>\n"
        "  </element2>\n"
        "  <element3>\n"
        "   <selection3>\n"
        "    <selection3>\n"
        "     <element1>\n"
        "      <element1>LONDON</element1>\n"
        "      <element1>LONDON</element1>\n"
        "      <element2></element2>\n"
        "      <element2></element2>\n"
        "      <element3>true</element3>\n"
        "      <element4></element4>\n"
        "      <element5>\n"
        "       <element1>\n"
        "        <element1>LONDON</element1>\n"
        "        <element1>LONDON</element1>\n"
        "        <element2></element2>\n"
        "        <element2></element2>\n"
        "        <element3>true</element3>\n"
        "        <element4></element4>\n"
        "        <element5>\n"
        "         <element1>\n"
        "          <element1>LONDON</element1>\n"
        "          <element1>LONDON</element1>\n"
        "          <element2></element2>\n"
        "          <element2></element2>\n"
        "          <element3>true</element3>\n"
        "          <element4></element4>\n"
        "          <element6>LONDON</element6>\n"
        "          <element6>LONDON</element6>\n"
        "         </element1>\n"
        "         <element2>true</element2>\n"
        "         <element2>true</element2>\n"
        "         <element3>1.5</element3>\n"
        "         <element3>1.5</element3>\n"
        "         <element4>FF0001</element4>\n"
        "         <element4>FF0001</element4>\n"
        "         <element5 xsi:nil=\"true\"/>\n"
        "         <element5 xsi:nil=\"true\"/>\n"
        "         <element6>2012-08-18T13:25:00.000+00:00</element6>\n"
        "         <element6>2012-08-18T13:25:00.000+00:00</element6>\n"
        "        </element5>\n"
        "        <element6>LONDON</element6>\n"
        "        <element6>LONDON</element6>\n"
        "       </element1>\n"
        "       <element2>true</element2>\n"
        "       <element2>true</element2>\n"
        "       <element3>1.5</element3>\n"
        "       <element3>1.5</element3>\n"
        "       <element4>FF0001</element4>\n"
        "       <element4>FF0001</element4>\n"
        "       <element5 xsi:nil=\"true\"/>\n"
        "       <element5 xsi:nil=\"true\"/>\n"
        "       <element6>2012-08-18T13:25:00.000+00:00</element6>\n"
        "       <element6>2012-08-18T13:25:00.000+00:00</element6>\n"
        "      </element5>\n"
        "      <element6>LONDON</element6>\n"
        "      <element6>LONDON</element6>\n"
        "     </element1>\n"
        "     <element1>\n"
        "      <element1>LONDON</element1>\n"
        "      <element1>LONDON</element1>\n"
        "      <element2></element2>\n"
        "      <element2></element2>\n"
        "      <element3>true</element3>\n"
        "      <element4></element4>\n"
        "      <element5>\n"
        "       <element1>\n"
        "        <element1>LONDON</element1>\n"
        "        <element1>LONDON</element1>\n"
        "        <element2></element2>\n"
        "        <element2></element2>\n"
        "        <element3>true</element3>\n"
        "        <element4></element4>\n"
        "        <element5>\n"
        "         <element1>\n"
        "          <element1>LONDON</element1>\n"
        "          <element1>LONDON</element1>\n"
        "          <element2></element2>\n"
        "          <element2></element2>\n"
        "          <element3>true</element3>\n"
        "          <element4></element4>\n"
        "          <element6>LONDON</element6>\n"
        "          <element6>LONDON</element6>\n"
        "         </element1>\n"
        "         <element2>true</element2>\n"
        "         <element2>true</element2>\n"
        "         <element3>1.5</element3>\n"
        "         <element3>1.5</element3>\n"
        "         <element4>FF0001</element4>\n"
        "         <element4>FF0001</element4>\n"
        "         <element5 xsi:nil=\"true\"/>\n"
        "         <element5 xsi:nil=\"true\"/>\n"
        "         <element6>2012-08-18T13:25:00.000+00:00</element6>\n"
        "         <element6>2012-08-18T13:25:00.000+00:00</element6>\n"
        "        </element5>\n"
        "        <element6>LONDON</element6>\n"
        "        <element6>LONDON</element6>\n"
        "       </element1>\n"
        "       <element2>true</element2>\n"
        "       <element2>true</element2>\n"
        "       <element3>1.5</element3>\n"
        "       <element3>1.5</element3>\n"
        "       <element4>FF0001</element4>\n"
        "       <element4>FF0001</element4>\n"
        "       <element5 xsi:nil=\"true\"/>\n"
        "       <element5 xsi:nil=\"true\"/>\n"
        "       <element6>2012-08-18T13:25:00.000+00:00</element6>\n"
        "       <element6>2012-08-18T13:25:00.000+00:00</element6>\n"
        "      </element5>\n"
        "      <element6>LONDON</element6>\n"
        "      <element6>LONDON</element6>\n"
        "     </element1>\n"
        "     <element2>\n"
        "      <selection3>\n"
        "       <element1>\n"
        "        <element2></element2>\n"
        "        <element2></element2>\n"
        "        <element4></element4>\n"
        "        <element5>\n"
        "         <element1>\n"
        "          <element2></element2>\n"
        "          <element2></element2>\n"
        "          <element4></element4>\n"
        "          <element5>\n"
        "           <element1>\n"
        "            <element2></element2>\n"
        "            <element2></element2>\n"
        "            <element4></element4>\n"
        "            <element6>LONDON</element6>\n"
        "            <element6>LONDON</element6>\n"
        "           </element1>\n"
        "           <element2>true</element2>\n"
        "           <element2>true</element2>\n"
        "           <element3>1.5</element3>\n"
        "           <element3>1.5</element3>\n"
        "           <element4>FF0001</element4>\n"
        "           <element4>FF0001</element4>\n"
        "           <element5>-980123</element5>\n"
        "           <element5>-980123</element5>\n"
        "           <element6>2012-08-18T13:25:00.000+00:00</element6>\n"
        "           <element6>2012-08-18T13:25:00.000+00:00</element6>\n"
        "          </element5>\n"
        "          <element6>LONDON</element6>\n"
        "          <element6>LONDON</element6>\n"
        "         </element1>\n"
        "         <element2>true</element2>\n"
        "         <element2>true</element2>\n"
        "         <element3>1.5</element3>\n"
        "         <element3>1.5</element3>\n"
        "         <element4>FF0001</element4>\n"
        "         <element4>FF0001</element4>\n"
        "         <element5>-980123</element5>\n"
        "         <element5>-980123</element5>\n"
        "         <element6>2012-08-18T13:25:00.000+00:00</element6>\n"
        "         <element6>2012-08-18T13:25:00.000+00:00</element6>\n"
        "        </element5>\n"
        "        <element6>LONDON</element6>\n"
        "        <element6>LONDON</element6>\n"
        "       </element1>\n"
        "       <element1>\n"
        "        <element2></element2>\n"
        "        <element2></element2>\n"
        "        <element4></element4>\n"
        "        <element5>\n"
        "         <element1>\n"
        "          <element2></element2>\n"
        "          <element2></element2>\n"
        "          <element4></element4>\n"
        "          <element5>\n"
        "           <element1>\n"
        "            <element2></element2>\n"
        "            <element2></element2>\n"
        "            <element4></element4>\n"
        "            <element6>LONDON</element6>\n"
        "            <element6>LONDON</element6>\n"
        "           </element1>\n"
        "           <element2>true</element2>\n"
        "           <element2>true</element2>\n"
        "           <element3>1.5</element3>\n"
        "           <element3>1.5</element3>\n"
        "           <element4>FF0001</element4>\n"
        "           <element4>FF0001</element4>\n"
        "           <element5>-980123</element5>\n"
        "           <element5>-980123</element5>\n"
        "           <element6>2012-08-18T13:25:00.000+00:00</element6>\n"
        "           <element6>2012-08-18T13:25:00.000+00:00</element6>\n"
        "          </element5>\n"
        "          <element6>LONDON</element6>\n"
        "          <element6>LONDON</element6>\n"
        "         </element1>\n"
        "         <element2>true</element2>\n"
        "         <element2>true</element2>\n"
        "         <element3>1.5</element3>\n"
        "         <element3>1.5</element3>\n"
        "         <element4>FF0001</element4>\n"
        "         <element4>FF0001</element4>\n"
        "         <element5>-980123</element5>\n"
        "         <element5>-980123</element5>\n"
        "         <element6>2012-08-18T13:25:00.000+00:00</element6>\n"
        "         <element6>2012-08-18T13:25:00.000+00:00</element6>\n"
        "        </element5>\n"
        "        <element6>LONDON</element6>\n"
        "        <element6>LONDON</element6>\n"
        "       </element1>\n"
        "       <element2>\n"
        "        <selection2>1.5</selection2>\n"
        "       </element2>\n"
        "       <element2>\n"
        "        <selection2>1.5</selection2>\n"
        "       </element2>\n"
        "       <element4>-980123</element4>\n"
        "       <element8>true</element8>\n"
        "       <element9></element9>\n"
        "       <element10>1.5</element10>\n"
        "       <element11>FF0001</element11>\n"
        "       <element12>-980123</element12>\n"
        "       <element13>LONDON</element13>\n"
        "       <element17>-980123</element17>\n"
        "       <element17>-980123</element17>\n"
        "      </selection3>\n"
        "     </element2>\n"
        "     <element2>\n"
        "      <selection3>\n"
        "       <element1>\n"
        "        <element2></element2>\n"
        "        <element2></element2>\n"
        "        <element4></element4>\n"
        "        <element5>\n"
        "         <element1>\n"
        "          <element2></element2>\n"
        "          <element2></element2>\n"
        "          <element4></element4>\n"
        "          <element5>\n"
        "           <element1>\n"
        "            <element2></element2>\n"
        "            <element2></element2>\n"
        "            <element4></element4>\n"
        "            <element6>LONDON</element6>\n"
        "            <element6>LONDON</element6>\n"
        "           </element1>\n"
        "           <element2>true</element2>\n"
        "           <element2>true</element2>\n"
        "           <element3>1.5</element3>\n"
        "           <element3>1.5</element3>\n"
        "           <element4>FF0001</element4>\n"
        "           <element4>FF0001</element4>\n"
        "           <element5>-980123</element5>\n"
        "           <element5>-980123</element5>\n"
        "           <element6>2012-08-18T13:25:00.000+00:00</element6>\n"
        "           <element6>2012-08-18T13:25:00.000+00:00</element6>\n"
        "          </element5>\n"
        "          <element6>LONDON</element6>\n"
        "          <element6>LONDON</element6>\n"
        "         </element1>\n"
        "         <element2>true</element2>\n"
        "         <element2>true</element2>\n"
        "         <element3>1.5</element3>\n"
        "         <element3>1.5</element3>\n"
        "         <element4>FF0001</element4>\n"
        "         <element4>FF0001</element4>\n"
        "         <element5>-980123</element5>\n"
        "         <element5>-980123</element5>\n"
        "         <element6>2012-08-18T13:25:00.000+00:00</element6>\n"
        "         <element6>2012-08-18T13:25:00.000+00:00</element6>\n"
        "        </element5>\n"
        "        <element6>LONDON</element6>\n"
        "        <element6>LONDON</element6>\n"
        "       </element1>\n"
        "       <element1>\n"
        "        <element2></element2>\n"
        "        <element2></element2>\n"
        "        <element4></element4>\n"
        "        <element5>\n"
        "         <element1>\n"
        "          <element2></element2>\n"
        "          <element2></element2>\n"
        "          <element4></element4>\n"
        "          <element5>\n"
        "           <element1>\n"
        "            <element2></element2>\n"
        "            <element2></element2>\n"
        "            <element4></element4>\n"
        "            <element6>LONDON</element6>\n"
        "            <element6>LONDON</element6>\n"
        "           </element1>\n"
        "           <element2>true</element2>\n"
        "           <element2>true</element2>\n"
        "           <element3>1.5</element3>\n"
        "           <element3>1.5</element3>\n"
        "           <element4>FF0001</element4>\n"
        "           <element4>FF0001</element4>\n"
        "           <element5>-980123</element5>\n"
        "           <element5>-980123</element5>\n"
        "           <element6>2012-08-18T13:25:00.000+00:00</element6>\n"
        "           <element6>2012-08-18T13:25:00.000+00:00</element6>\n"
        "          </element5>\n"
        "          <element6>LONDON</element6>\n"
        "          <element6>LONDON</element6>\n"
        "         </element1>\n"
        "         <element2>true</element2>\n"
        "         <element2>true</element2>\n"
        "         <element3>1.5</element3>\n"
        "         <element3>1.5</element3>\n"
        "         <element4>FF0001</element4>\n"
        "         <element4>FF0001</element4>\n"
        "         <element5>-980123</element5>\n"
        "         <element5>-980123</element5>\n"
        "         <element6>2012-08-18T13:25:00.000+00:00</element6>\n"
        "         <element6>2012-08-18T13:25:00.000+00:00</element6>\n"
        "        </element5>\n"
        "        <element6>LONDON</element6>\n"
        "        <element6>LONDON</element6>\n"
        "       </element1>\n"
        "       <element2>\n"
        "        <selection2>1.5</selection2>\n"
        "       </element2>\n"
        "       <element2>\n"
        "        <selection2>1.5</selection2>\n"
        "       </element2>\n"
        "       <element4>-980123</element4>\n"
        "       <element8>true</element8>\n"
        "       <element9></element9>\n"
        "       <element10>1.5</element10>\n"
        "       <element11>FF0001</element11>\n"
        "       <element12>-980123</element12>\n"
        "       <element13>LONDON</element13>\n"
        "       <element17>-980123</element17>\n"
        "       <element17>-980123</element17>\n"
        "      </selection3>\n"
        "     </element2>\n"
        "     <element3>FF0001</element3>\n"
        "     <element4>-980123</element4>\n"
        "     <element5>2012-08-18T13:25:00.000+00:00</element5>\n"
        "     <element6>custom</element6>\n"
        "     <element7>LONDON</element7>\n"
        "     <element8>true</element8>\n"
        "     <element9>arbitrary string value</element9>\n"
        "     <element10>1.5</element10>\n"
        "     <element11>FF0001</element11>\n"
        "     <element12>2</element12>\n"
        "     <element13>LONDON</element13>\n"
        "     <element14>true</element14>\n"
        "     <element14>true</element14>\n"
        "     <element15>1.5</element15>\n"
        "     <element15>1.5</element15>\n"
        "     <element16>FF0001</element16>\n"
        "     <element16>FF0001</element16>\n"
        "     <element17>-980123</element17>\n"
        "     <element17>-980123</element17>\n"
        "     <element18>2012-08-18T13:25:00.000+00:00</element18>\n"
        "     <element18>2012-08-18T13:25:00.000+00:00</element18>\n"
        "     <element19>custom</element19>\n"
        "     <element19>custom</element19>\n"
        "    </selection3>\n"
        "   </selection3>\n"
        "  </element3>\n"
        "  <element4>\n"
        "   <selection4>\n"
        "    <selection2>arbitrary string value</selection2>\n"
        "   </selection4>\n"
        "  </element4>\n"
        "  <element4>\n"
        "   <selection4>\n"
        "    <selection2>arbitrary string value</selection2>\n"
        "   </selection4>\n"
        "  </element4>\n"
        "  <element5>\n"
        "   <selection1>\n"
        "    <element1>255</element1>\n"
        "    <element4>3123123123</element4>\n"
        "    <element5>255</element5>\n"
        "    <element6>999</element6>\n"
        "    <element6>999</element6>\n"
        "    <element7>custom</element7>\n"
        "    <element8>999</element8>\n"
        "    <element10>255</element10>\n"
        "    <element10>255</element10>\n"
        "    <element13>255</element13>\n"
        "    <element13>255</element13>\n"
        "    <element15>3123123123</element15>\n"
        "    <element15>3123123123</element15>\n"
        "   </selection1>\n"
        "  </element5>\n"
        "  <element5>\n"
        "   <selection1>\n"
        "    <element1>255</element1>\n"
        "    <element4>3123123123</element4>\n"
        "    <element5>255</element5>\n"
        "    <element6>999</element6>\n"
        "    <element6>999</element6>\n"
        "    <element7>custom</element7>\n"
        "    <element8>999</element8>\n"
        "    <element10>255</element10>\n"
        "    <element10>255</element10>\n"
        "    <element13>255</element13>\n"
        "    <element13>255</element13>\n"
        "    <element15>3123123123</element15>\n"
        "    <element15>3123123123</element15>\n"
        "   </selection1>\n"
        "  </element5>\n"
        " </selection1>\n"
        "</Topchoice>\n",

        "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
        "<Topchoice xmlns=\"TestNamespace\"\n"
        "  xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\">\n"
        "  <selection1>\n"
        "    <element1>\n"
        "      <selection1>\n"
        "        <element2>custom</element2>\n"
        "        <element3>999</element3>\n"
        "        <element4>3123123123</element4>\n"
        "        <element5>0</element5>\n"
        "        <element7>custom</element7>\n"
        "        <element8>999</element8>\n"
        "        <element9>3123123123</element9>\n"
        "        <element11>custom</element11>\n"
        "        <element11>custom</element11>\n"
        "        <element12>3123123123</element12>\n"
        "        <element12>3123123123</element12>\n"
        "        <element13>0</element13>\n"
        "        <element13>0</element13>\n"
        "        <element14>999</element14>\n"
        "        <element14>999</element14>\n"
        "      </selection1>\n"
        "    </element1>\n"
        "    <element2>\n"
        "      <selection4>\n"
        "        <selection3>\n"
        "          <selection1>-980123</selection1>\n"
        "        </selection3>\n"
        "      </selection4>\n"
        "    </element2>\n"
        "    <element2>\n"
        "      <selection4>\n"
        "        <selection3>\n"
        "          <selection1>-980123</selection1>\n"
        "        </selection3>\n"
        "      </selection4>\n"
        "    </element2>\n"
        "    <element3>\n"
        "      <selection4>3123123123</selection4>\n"
        "    </element3>\n"
        "    <element4>\n"
        "      <selection4>\n"
        "        <selection3>\n"
        "          <selection1>-980123</selection1>\n"
        "        </selection3>\n"
        "      </selection4>\n"
        "    </element4>\n"
        "    <element4>\n"
        "      <selection4>\n"
        "        <selection3>\n"
        "          <selection1>-980123</selection1>\n"
        "        </selection3>\n"
        "      </selection4>\n"
        "    </element4>\n"
        "    <element5>\n"
        "      <selection1>\n"
        "        <element2>custom</element2>\n"
        "        <element3>999</element3>\n"
        "        <element4>3123123123</element4>\n"
        "        <element5>0</element5>\n"
        "        <element7>custom</element7>\n"
        "        <element8>999</element8>\n"
        "        <element9>3123123123</element9>\n"
        "        <element11>custom</element11>\n"
        "        <element11>custom</element11>\n"
        "        <element12>3123123123</element12>\n"
        "        <element12>3123123123</element12>\n"
        "        <element13>0</element13>\n"
        "        <element13>0</element13>\n"
        "        <element14>999</element14>\n"
        "        <element14>999</element14>\n"
        "      </selection1>\n"
        "    </element5>\n"
        "    <element5>\n"
        "      <selection1>\n"
        "        <element2>custom</element2>\n"
        "        <element3>999</element3>\n"
        "        <element4>3123123123</element4>\n"
        "        <element5>0</element5>\n"
        "        <element7>custom</element7>\n"
        "        <element8>999</element8>\n"
        "        <element9>3123123123</element9>\n"
        "        <element11>custom</element11>\n"
        "        <element11>custom</element11>\n"
        "        <element12>3123123123</element12>\n"
        "        <element12>3123123123</element12>\n"
        "        <element13>0</element13>\n"
        "        <element13>0</element13>\n"
        "        <element14>999</element14>\n"
        "        <element14>999</element14>\n"
        "      </selection1>\n"
        "    </element5>\n"
        "  </selection1>\n"
        "</Topchoice>\n",

        "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
        "<Topchoice xmlns=\"TestNamespace\"\n"
        "  xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\">\n"
        "  <selection1>\n"
        "    <element1>\n"
        "      <selection2>255</selection2>\n"
        "    </element1>\n"
        "    <element2>\n"
        "      <selection1>-980123</selection1>\n"
        "    </element2>\n"
        "    <element2>\n"
        "      <selection1>-980123</selection1>\n"
        "    </element2>\n"
        "    <element3>\n"
        "      <selection1>true</selection1>\n"
        "    </element3>\n"
        "    <element4>\n"
        "      <selection1>-980123</selection1>\n"
        "    </element4>\n"
        "    <element4>\n"
        "      <selection1>-980123</selection1>\n"
        "    </element4>\n"
        "    <element5>\n"
        "      <selection2>255</selection2>\n"
        "    </element5>\n"
        "    <element5>\n"
        "      <selection2>255</selection2>\n"
        "    </element5>\n"
        "  </selection1>\n"
        "</Topchoice>\n",

        "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
        "<Topchoice xmlns=\"TestNamespace\"\n"
        "  xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\">\n"
        "  <selection1>\n"
        "    <element1>\n"
        "      <selection2>0</selection2>\n"
        "    </element1>\n"
        "    <element3>\n"
        "      <selection2>\n"
        "      </selection2>\n"
        "    </element3>\n"
        "    <element4>\n"
        "      <selection1>2</selection1>\n"
        "    </element4>\n"
        "    <element4>\n"
        "      <selection1>2</selection1>\n"
        "    </element4>\n"
        "    <element5>\n"
        "      <selection2>0</selection2>\n"
        "    </element5>\n"
        "    <element5>\n"
        "      <selection2>0</selection2>\n"
        "    </element5>\n"
        "  </selection1>\n"
        "</Topchoice>\n",

        "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
        "<Topchoice xmlns=\"TestNamespace\"\n"
        "  xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\">\n"
        "  <selection2>FF0001</selection2>\n"
        "</Topchoice>\n",

        "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
        "<Topchoice xmlns=\"TestNamespace\"\n"
        "  xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\">\n"
        "  <selection3>\n"
        "    <element1>custom</element1>\n"
        "    <element2>255</element2>\n"
        "    <element3>2012-08-18T13:25:00.000+00:00</element3>\n"
        "  </selection3>\n"
        "</Topchoice>\n",

        "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
        "<Topchoice xmlns=\"TestNamespace\"\n"
        "  xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\">\n"
        "  <selection3>\n"
        "    <element1>custom</element1>\n"
        "    <element2>0</element2>\n"
        "    <element3>2012-08-18T13:25:00.000+00:00</element3>\n"
        "    <element4>\n"
        "      <selection1>2</selection1>\n"
        "    </element4>\n"
        "    <element5>1.5</element5>\n"
        "    <element6 xsi:nil=\"true\"/>\n"
        "    <element6 xsi:nil=\"true\"/>\n"
        "    <element7 xsi:nil=\"true\"/>\n"
        "    <element7 xsi:nil=\"true\"/>\n"
        "  </selection3>\n"
        "</Topchoice>\n",

        "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
        "<Topchoice xmlns=\"TestNamespace\"\n"
        "  xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\">\n"
        "  <selection3>\n"
        "    <element1>custom</element1>\n"
        "    <element2>255</element2>\n"
        "    <element3>2012-08-18T13:25:00.000+00:00</element3>\n"
        "    <element4>\n"
        "      <selection2>1.5</selection2>\n"
        "    </element4>\n"
        "    <element6>\n"
        "    </element6>\n"
        "    <element6>\n"
        "    </element6>\n"
        "    <element7>custom</element7>\n"
        "    <element7>custom</element7>\n"
        "  </selection3>\n"
        "</Topchoice>\n",

        "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
        "<Topchoice xmlns=\"TestNamespace\"\n"
        "  xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\">\n"
        "  <selection3>\n"
        "    <element1>custom</element1>\n"
        "    <element2>0</element2>\n"
        "    <element3>2012-08-18T13:25:00.000+00:00</element3>\n"
        "    <element4>\n"
        "      <selection3>\n"
        "        <element8>true</element8>\n"
        "        <element9>\n"
        "        </element9>\n"
        "        <element10>1.5</element10>\n"
        "        <element11>FF0001</element11>\n"
        "        <element12>-980123</element12>\n"
        "        <element13>LONDON</element13>\n"
        "      </selection3>\n"
        "    </element4>\n"
        "    <element5>1.5</element5>\n"
        "    <element6>arbitrary string value</element6>\n"
        "    <element6>arbitrary string value</element6>\n"
        "  </selection3>\n"
        "</Topchoice>\n",

        "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
        "<Topchoice xmlns=\"TestNamespace\"\n"
        "  xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\">\n"
        "  <selection3>\n"
        "    <element1>custom</element1>\n"
        "    <element2>255</element2>\n"
        "    <element3>2012-08-18T13:25:00.000+00:00</element3>\n"
        "    <element4>\n"
        "      <selection3>\n"
        "        <element1>\n"
        "          <element1>LONDON</element1>\n"
        "          <element1>LONDON</element1>\n"
        "          <element2>arbitrary string value</element2>\n"
        "          <element2>arbitrary string value</element2>\n"
        "          <element3>true</element3>\n"
        "          <element4>arbitrary string value</element4>\n"
        "          <element5>\n"
        "            <element1>\n"
        "              <element1>LONDON</element1>\n"
        "              <element1>LONDON</element1>\n"
        "              <element2>arbitrary string value</element2>\n"
        "              <element2>arbitrary string value</element2>\n"
        "              <element3>true</element3>\n"
        "              <element4>arbitrary string value</element4>\n"
        "              <element5>\n"
        "                <element1>\n"
        "                  <element1>LONDON</element1>\n"
        "                  <element1>LONDON</element1>\n"
        "                  <element2>arbitrary string value</element2>\n"
        "                  <element2>arbitrary string value</element2>\n"
        "                  <element3>true</element3>\n"
        "                  <element4>arbitrary string value</element4>\n"
        "                  <element6 xsi:nil=\"true\"/>\n"
        "                  <element6 xsi:nil=\"true\"/>\n"
        "                </element1>\n"
        "                <element2 xsi:nil=\"true\"/>\n"
        "                <element2 xsi:nil=\"true\"/>\n"
        "                <element3 xsi:nil=\"true\"/>\n"
        "                <element3 xsi:nil=\"true\"/>\n"
        "                <element4 xsi:nil=\"true\"/>\n"
        "                <element4 xsi:nil=\"true\"/>\n"
        "                <element5 xsi:nil=\"true\"/>\n"
        "                <element5 xsi:nil=\"true\"/>\n"
        "                <element6 xsi:nil=\"true\"/>\n"
        "                <element6 xsi:nil=\"true\"/>\n"
        "              </element5>\n"
        "              <element6 xsi:nil=\"true\"/>\n"
        "              <element6 xsi:nil=\"true\"/>\n"
        "            </element1>\n"
        "            <element2 xsi:nil=\"true\"/>\n"
        "            <element2 xsi:nil=\"true\"/>\n"
        "            <element3 xsi:nil=\"true\"/>\n"
        "            <element3 xsi:nil=\"true\"/>\n"
        "            <element4 xsi:nil=\"true\"/>\n"
        "            <element4 xsi:nil=\"true\"/>\n"
        "            <element5 xsi:nil=\"true\"/>\n"
        "            <element5 xsi:nil=\"true\"/>\n"
        "            <element6 xsi:nil=\"true\"/>\n"
        "            <element6 xsi:nil=\"true\"/>\n"
        "          </element5>\n"
        "          <element6 xsi:nil=\"true\"/>\n"
        "          <element6 xsi:nil=\"true\"/>\n"
        "        </element1>\n"
        "        <element1>\n"
        "          <element1>LONDON</element1>\n"
        "          <element1>LONDON</element1>\n"
        "          <element2>arbitrary string value</element2>\n"
        "          <element2>arbitrary string value</element2>\n"
        "          <element3>true</element3>\n"
        "          <element4>arbitrary string value</element4>\n"
        "          <element5>\n"
        "            <element1>\n"
        "              <element1>LONDON</element1>\n"
        "              <element1>LONDON</element1>\n"
        "              <element2>arbitrary string value</element2>\n"
        "              <element2>arbitrary string value</element2>\n"
        "              <element3>true</element3>\n"
        "              <element4>arbitrary string value</element4>\n"
        "              <element5>\n"
        "                <element1>\n"
        "                  <element1>LONDON</element1>\n"
        "                  <element1>LONDON</element1>\n"
        "                  <element2>arbitrary string value</element2>\n"
        "                  <element2>arbitrary string value</element2>\n"
        "                  <element3>true</element3>\n"
        "                  <element4>arbitrary string value</element4>\n"
        "                  <element6 xsi:nil=\"true\"/>\n"
        "                  <element6 xsi:nil=\"true\"/>\n"
        "                </element1>\n"
        "                <element2 xsi:nil=\"true\"/>\n"
        "                <element2 xsi:nil=\"true\"/>\n"
        "                <element3 xsi:nil=\"true\"/>\n"
        "                <element3 xsi:nil=\"true\"/>\n"
        "                <element4 xsi:nil=\"true\"/>\n"
        "                <element4 xsi:nil=\"true\"/>\n"
        "                <element5 xsi:nil=\"true\"/>\n"
        "                <element5 xsi:nil=\"true\"/>\n"
        "                <element6 xsi:nil=\"true\"/>\n"
        "                <element6 xsi:nil=\"true\"/>\n"
        "              </element5>\n"
        "              <element6 xsi:nil=\"true\"/>\n"
        "              <element6 xsi:nil=\"true\"/>\n"
        "            </element1>\n"
        "            <element2 xsi:nil=\"true\"/>\n"
        "            <element2 xsi:nil=\"true\"/>\n"
        "            <element3 xsi:nil=\"true\"/>\n"
        "            <element3 xsi:nil=\"true\"/>\n"
        "            <element4 xsi:nil=\"true\"/>\n"
        "            <element4 xsi:nil=\"true\"/>\n"
        "            <element5 xsi:nil=\"true\"/>\n"
        "            <element5 xsi:nil=\"true\"/>\n"
        "            <element6 xsi:nil=\"true\"/>\n"
        "            <element6 xsi:nil=\"true\"/>\n"
        "          </element5>\n"
        "          <element6 xsi:nil=\"true\"/>\n"
        "          <element6 xsi:nil=\"true\"/>\n"
        "        </element1>\n"
        "        <element2>\n"
        "          <selection1>2</selection1>\n"
        "        </element2>\n"
        "        <element2>\n"
        "          <selection1>2</selection1>\n"
        "        </element2>\n"
        "        <element3>FF0001</element3>\n"
        "        <element4>2</element4>\n"
        "        <element5>2012-08-18T13:25:00.000+00:00</element5>\n"
        "        <element6>custom</element6>\n"
        "        <element7>LONDON</element7>\n"
        "        <element8>true</element8>\n"
        "        <element9>arbitrary string value</element9>\n"
        "        <element10>1.5</element10>\n"
        "        <element11>FF0001</element11>\n"
        "        <element12>2</element12>\n"
        "        <element13>LONDON</element13>\n"
        "        <element14>true</element14>\n"
        "        <element14>true</element14>\n"
        "        <element15>1.5</element15>\n"
        "        <element15>1.5</element15>\n"
        "        <element16>FF0001</element16>\n"
        "        <element16>FF0001</element16>\n"
        "        <element17>2</element17>\n"
        "        <element17>2</element17>\n"
        "        <element18>2012-08-18T13:25:00.000+00:00</element18>\n"
        "        <element18>2012-08-18T13:25:00.000+00:00</element18>\n"
        "        <element19>custom</element19>\n"
        "        <element19>custom</element19>\n"
        "      </selection3>\n"
        "    </element4>\n"
        "    <element7 xsi:nil=\"true\"/>\n"
        "    <element7 xsi:nil=\"true\"/>\n"
        "  </selection3>\n"
        "</Topchoice>\n",

        "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
        "<Topchoice xmlns=\"TestNamespace\"\n"
        "  xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\">\n"
        "  <selection3>\n"
        "    <element1>custom</element1>\n"
        "    <element2>0</element2>\n"
        "    <element3>2012-08-18T13:25:00.000+00:00</element3>\n"
        "    <element4>\n"
        "      <selection3>\n"
        "        <element1>\n"
        "          <element2>\n"
        "          </element2>\n"
        "          <element2>\n"
        "          </element2>\n"
        "          <element4>\n"
        "          </element4>\n"
        "          <element5>\n"
        "            <element1>\n"
        "              <element2>\n"
        "              </element2>\n"
        "              <element2>\n"
        "              </element2>\n"
        "              <element4>\n"
        "              </element4>\n"
        "              <element5>\n"
        "                <element1>\n"
        "                  <element2>\n"
        "                  </element2>\n"
        "                  <element2>\n"
        "                  </element2>\n"
        "                  <element4>\n"
        "                  </element4>\n"
        "                  <element6>LONDON</element6>\n"
        "                  <element6>LONDON</element6>\n"
        "                </element1>\n"
        "                <element2>true</element2>\n"
        "                <element2>true</element2>\n"
        "                <element3>1.5</element3>\n"
        "                <element3>1.5</element3>\n"
        "                <element4>FF0001</element4>\n"
        "                <element4>FF0001</element4>\n"
        "                <element5>-980123</element5>\n"
        "                <element5>-980123</element5>\n"
        "                <element6>2012-08-18T13:25:00.000+00:00</element6>\n"
        "                <element6>2012-08-18T13:25:00.000+00:00</element6>\n"
        "              </element5>\n"
        "              <element6>LONDON</element6>\n"
        "              <element6>LONDON</element6>\n"
        "            </element1>\n"
        "            <element2>true</element2>\n"
        "            <element2>true</element2>\n"
        "            <element3>1.5</element3>\n"
        "            <element3>1.5</element3>\n"
        "            <element4>FF0001</element4>\n"
        "            <element4>FF0001</element4>\n"
        "            <element5>-980123</element5>\n"
        "            <element5>-980123</element5>\n"
        "            <element6>2012-08-18T13:25:00.000+00:00</element6>\n"
        "            <element6>2012-08-18T13:25:00.000+00:00</element6>\n"
        "          </element5>\n"
        "          <element6>LONDON</element6>\n"
        "          <element6>LONDON</element6>\n"
        "        </element1>\n"
        "        <element1>\n"
        "          <element2>\n"
        "          </element2>\n"
        "          <element2>\n"
        "          </element2>\n"
        "          <element4>\n"
        "          </element4>\n"
        "          <element5>\n"
        "            <element1>\n"
        "              <element2>\n"
        "              </element2>\n"
        "              <element2>\n"
        "              </element2>\n"
        "              <element4>\n"
        "              </element4>\n"
        "              <element5>\n"
        "                <element1>\n"
        "                  <element2>\n"
        "                  </element2>\n"
        "                  <element2>\n"
        "                  </element2>\n"
        "                  <element4>\n"
        "                  </element4>\n"
        "                  <element6>LONDON</element6>\n"
        "                  <element6>LONDON</element6>\n"
        "                </element1>\n"
        "                <element2>true</element2>\n"
        "                <element2>true</element2>\n"
        "                <element3>1.5</element3>\n"
        "                <element3>1.5</element3>\n"
        "                <element4>FF0001</element4>\n"
        "                <element4>FF0001</element4>\n"
        "                <element5>-980123</element5>\n"
        "                <element5>-980123</element5>\n"
        "                <element6>2012-08-18T13:25:00.000+00:00</element6>\n"
        "                <element6>2012-08-18T13:25:00.000+00:00</element6>\n"
        "              </element5>\n"
        "              <element6>LONDON</element6>\n"
        "              <element6>LONDON</element6>\n"
        "            </element1>\n"
        "            <element2>true</element2>\n"
        "            <element2>true</element2>\n"
        "            <element3>1.5</element3>\n"
        "            <element3>1.5</element3>\n"
        "            <element4>FF0001</element4>\n"
        "            <element4>FF0001</element4>\n"
        "            <element5>-980123</element5>\n"
        "            <element5>-980123</element5>\n"
        "            <element6>2012-08-18T13:25:00.000+00:00</element6>\n"
        "            <element6>2012-08-18T13:25:00.000+00:00</element6>\n"
        "          </element5>\n"
        "          <element6>LONDON</element6>\n"
        "          <element6>LONDON</element6>\n"
        "        </element1>\n"
        "        <element2>\n"
        "          <selection2>1.5</selection2>\n"
        "        </element2>\n"
        "        <element2>\n"
        "          <selection2>1.5</selection2>\n"
        "        </element2>\n"
        "        <element4>-980123</element4>\n"
        "        <element8>true</element8>\n"
        "        <element9>\n"
        "        </element9>\n"
        "        <element10>1.5</element10>\n"
        "        <element11>FF0001</element11>\n"
        "        <element12>-980123</element12>\n"
        "        <element13>LONDON</element13>\n"
        "        <element17>-980123</element17>\n"
        "        <element17>-980123</element17>\n"
        "      </selection3>\n"
        "    </element4>\n"
        "    <element5>1.5</element5>\n"
        "    <element6 xsi:nil=\"true\"/>\n"
        "    <element6 xsi:nil=\"true\"/>\n"
        "    <element7>custom</element7>\n"
        "    <element7>custom</element7>\n"
        "  </selection3>\n"
        "</Topchoice>\n",

        "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
        "<Topchoice xmlns=\"TestNamespace\"\n"
        "  xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\">\n"
        "  <selection3>\n"
        "    <element1>custom</element1>\n"
        "    <element2>255</element2>\n"
        "    <element3>2012-08-18T13:25:00.000+00:00</element3>\n"
        "    <element4>\n"
        "      <selection3>\n"
        "        <element1>\n"
        "          <element1>LONDON</element1>\n"
        "          <element1>LONDON</element1>\n"
        "          <element3>true</element3>\n"
        "          <element5>\n"
        "            <element1>\n"
        "              <element1>LONDON</element1>\n"
        "              <element1>LONDON</element1>\n"
        "              <element3>true</element3>\n"
        "              <element5>\n"
        "                <element1>\n"
        "                  <element1>LONDON</element1>\n"
        "                  <element1>LONDON</element1>\n"
        "                  <element3>true</element3>\n"
        "                </element1>\n"
        "                <element5>2</element5>\n"
        "                <element5>2</element5>\n"
        "              </element5>\n"
        "            </element1>\n"
        "            <element5>2</element5>\n"
        "            <element5>2</element5>\n"
        "          </element5>\n"
        "        </element1>\n"
        "        <element1>\n"
        "          <element1>LONDON</element1>\n"
        "          <element1>LONDON</element1>\n"
        "          <element3>true</element3>\n"
        "          <element5>\n"
        "            <element1>\n"
        "              <element1>LONDON</element1>\n"
        "              <element1>LONDON</element1>\n"
        "              <element3>true</element3>\n"
        "              <element5>\n"
        "                <element1>\n"
        "                  <element1>LONDON</element1>\n"
        "                  <element1>LONDON</element1>\n"
        "                  <element3>true</element3>\n"
        "                </element1>\n"
        "                <element5>2</element5>\n"
        "                <element5>2</element5>\n"
        "              </element5>\n"
        "            </element1>\n"
        "            <element5>2</element5>\n"
        "            <element5>2</element5>\n"
        "          </element5>\n"
        "        </element1>\n"
        "        <element2>\n"
        "          <selection3>\n"
        "            <element8>true</element8>\n"
        "            <element9>\n"
        "            </element9>\n"
        "            <element10>1.5</element10>\n"
        "            <element11>FF0001</element11>\n"
        "            <element12>-980123</element12>\n"
        "            <element13>LONDON</element13>\n"
        "          </selection3>\n"
        "        </element2>\n"
        "        <element2>\n"
        "          <selection3>\n"
        "            <element8>true</element8>\n"
        "            <element9>\n"
        "            </element9>\n"
        "            <element10>1.5</element10>\n"
        "            <element11>FF0001</element11>\n"
        "            <element12>-980123</element12>\n"
        "            <element13>LONDON</element13>\n"
        "          </selection3>\n"
        "        </element2>\n"
        "        <element3>FF0001</element3>\n"
        "        <element5>2012-08-18T13:25:00.000+00:00</element5>\n"
        "        <element6>custom</element6>\n"
        "        <element7>LONDON</element7>\n"
        "        <element8>true</element8>\n"
        "        <element9>arbitrary string value</element9>\n"
        "        <element10>1.5</element10>\n"
        "        <element11>FF0001</element11>\n"
        "        <element12>2</element12>\n"
        "        <element13>LONDON</element13>\n"
        "        <element14>true</element14>\n"
        "        <element14>true</element14>\n"
        "        <element15>1.5</element15>\n"
        "        <element15>1.5</element15>\n"
        "        <element16>FF0001</element16>\n"
        "        <element16>FF0001</element16>\n"
        "        <element18>2012-08-18T13:25:00.000+00:00</element18>\n"
        "        <element18>2012-08-18T13:25:00.000+00:00</element18>\n"
        "        <element19>custom</element19>\n"
        "        <element19>custom</element19>\n"
        "      </selection3>\n"
        "    </element4>\n"
        "    <element6>\n"
        "    </element6>\n"
        "    <element6>\n"
        "    </element6>\n"
        "  </selection3>\n"
        "</Topchoice>\n",

        "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
        "<Topchoice xmlns=\"TestNamespace\"\n"
        "  xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\">\n"
        "  <selection3>\n"
        "    <element1>custom</element1>\n"
        "    <element2>0</element2>\n"
        "    <element3>2012-08-18T13:25:00.000+00:00</element3>\n"
        "    <element4>\n"
        "      <selection3>\n"
        "        <element1>\n"
        "          <element2>arbitrary string value</element2>\n"
        "          <element2>arbitrary string value</element2>\n"
        "          <element4>arbitrary string value</element4>\n"
        "          <element5>\n"
        "            <element1>\n"
        "              <element2>arbitrary string value</element2>\n"
        "              <element2>arbitrary string value</element2>\n"
        "              <element4>arbitrary string value</element4>\n"
        "              <element5>\n"
        "                <element1>\n"
        "                  <element2>arbitrary string value</element2>\n"
        "                  <element2>arbitrary string value</element2>\n"
        "                  <element4>arbitrary string value</element4>\n"
        "                  <element6 xsi:nil=\"true\"/>\n"
        "                  <element6 xsi:nil=\"true\"/>\n"
        "                </element1>\n"
        "                <element2 xsi:nil=\"true\"/>\n"
        "                <element2 xsi:nil=\"true\"/>\n"
        "                <element3 xsi:nil=\"true\"/>\n"
        "                <element3 xsi:nil=\"true\"/>\n"
        "                <element4 xsi:nil=\"true\"/>\n"
        "                <element4 xsi:nil=\"true\"/>\n"
        "                <element6 xsi:nil=\"true\"/>\n"
        "                <element6 xsi:nil=\"true\"/>\n"
        "              </element5>\n"
        "              <element6 xsi:nil=\"true\"/>\n"
        "              <element6 xsi:nil=\"true\"/>\n"
        "            </element1>\n"
        "            <element2 xsi:nil=\"true\"/>\n"
        "            <element2 xsi:nil=\"true\"/>\n"
        "            <element3 xsi:nil=\"true\"/>\n"
        "            <element3 xsi:nil=\"true\"/>\n"
        "            <element4 xsi:nil=\"true\"/>\n"
        "            <element4 xsi:nil=\"true\"/>\n"
        "            <element6 xsi:nil=\"true\"/>\n"
        "            <element6 xsi:nil=\"true\"/>\n"
        "          </element5>\n"
        "          <element6 xsi:nil=\"true\"/>\n"
        "          <element6 xsi:nil=\"true\"/>\n"
        "        </element1>\n"
        "        <element1>\n"
        "          <element2>arbitrary string value</element2>\n"
        "          <element2>arbitrary string value</element2>\n"
        "          <element4>arbitrary string value</element4>\n"
        "          <element5>\n"
        "            <element1>\n"
        "              <element2>arbitrary string value</element2>\n"
        "              <element2>arbitrary string value</element2>\n"
        "              <element4>arbitrary string value</element4>\n"
        "              <element5>\n"
        "                <element1>\n"
        "                  <element2>arbitrary string value</element2>\n"
        "                  <element2>arbitrary string value</element2>\n"
        "                  <element4>arbitrary string value</element4>\n"
        "                  <element6 xsi:nil=\"true\"/>\n"
        "                  <element6 xsi:nil=\"true\"/>\n"
        "                </element1>\n"
        "                <element2 xsi:nil=\"true\"/>\n"
        "                <element2 xsi:nil=\"true\"/>\n"
        "                <element3 xsi:nil=\"true\"/>\n"
        "                <element3 xsi:nil=\"true\"/>\n"
        "                <element4 xsi:nil=\"true\"/>\n"
        "                <element4 xsi:nil=\"true\"/>\n"
        "                <element6 xsi:nil=\"true\"/>\n"
        "                <element6 xsi:nil=\"true\"/>\n"
        "              </element5>\n"
        "              <element6 xsi:nil=\"true\"/>\n"
        "              <element6 xsi:nil=\"true\"/>\n"
        "            </element1>\n"
        "            <element2 xsi:nil=\"true\"/>\n"
        "            <element2 xsi:nil=\"true\"/>\n"
        "            <element3 xsi:nil=\"true\"/>\n"
        "            <element3 xsi:nil=\"true\"/>\n"
        "            <element4 xsi:nil=\"true\"/>\n"
        "            <element4 xsi:nil=\"true\"/>\n"
        "            <element6 xsi:nil=\"true\"/>\n"
        "            <element6 xsi:nil=\"true\"/>\n"
        "          </element5>\n"
        "          <element6 xsi:nil=\"true\"/>\n"
        "          <element6 xsi:nil=\"true\"/>\n"
        "        </element1>\n"
        "        <element2>\n"
        "          <selection3>\n"
        "            <element1>\n"
        "              <element1>LONDON</element1>\n"
        "              <element1>LONDON</element1>\n"
        "              <element2>arbitrary string value</element2>\n"
        "              <element2>arbitrary string value</element2>\n"
        "              <element3>true</element3>\n"
        "              <element4>arbitrary string value</element4>\n"
        "              <element5>\n"
        "                <element1>\n"
        "                  <element1>LONDON</element1>\n"
        "                  <element1>LONDON</element1>\n"
        "                  <element2>arbitrary string value</element2>\n"
        "                  <element2>arbitrary string value</element2>\n"
        "                  <element3>true</element3>\n"
        "                  <element4>arbitrary string value</element4>\n"
        "                  <element5>\n"
        "                    <element1>\n"
        "                      <element1>LONDON</element1>\n"
        "                      <element1>LONDON</element1>\n"
        "                      <element2>arbitrary string value</element2>\n"
        "                      <element2>arbitrary string value</element2>\n"
        "                      <element3>true</element3>\n"
        "                      <element4>arbitrary string value</element4>\n"
        "                      <element6 xsi:nil=\"true\"/>\n"
        "                      <element6 xsi:nil=\"true\"/>\n"
        "                    </element1>\n"
        "                    <element2 xsi:nil=\"true\"/>\n"
        "                    <element2 xsi:nil=\"true\"/>\n"
        "                    <element3 xsi:nil=\"true\"/>\n"
        "                    <element3 xsi:nil=\"true\"/>\n"
        "                    <element4 xsi:nil=\"true\"/>\n"
        "                    <element4 xsi:nil=\"true\"/>\n"
        "                    <element5 xsi:nil=\"true\"/>\n"
        "                    <element5 xsi:nil=\"true\"/>\n"
        "                    <element6 xsi:nil=\"true\"/>\n"
        "                    <element6 xsi:nil=\"true\"/>\n"
        "                  </element5>\n"
        "                  <element6 xsi:nil=\"true\"/>\n"
        "                  <element6 xsi:nil=\"true\"/>\n"
        "                </element1>\n"
        "                <element2 xsi:nil=\"true\"/>\n"
        "                <element2 xsi:nil=\"true\"/>\n"
        "                <element3 xsi:nil=\"true\"/>\n"
        "                <element3 xsi:nil=\"true\"/>\n"
        "                <element4 xsi:nil=\"true\"/>\n"
        "                <element4 xsi:nil=\"true\"/>\n"
        "                <element5 xsi:nil=\"true\"/>\n"
        "                <element5 xsi:nil=\"true\"/>\n"
        "                <element6 xsi:nil=\"true\"/>\n"
        "                <element6 xsi:nil=\"true\"/>\n"
        "              </element5>\n"
        "              <element6 xsi:nil=\"true\"/>\n"
        "              <element6 xsi:nil=\"true\"/>\n"
        "            </element1>\n"
        "            <element1>\n"
        "              <element1>LONDON</element1>\n"
        "              <element1>LONDON</element1>\n"
        "              <element2>arbitrary string value</element2>\n"
        "              <element2>arbitrary string value</element2>\n"
        "              <element3>true</element3>\n"
        "              <element4>arbitrary string value</element4>\n"
        "              <element5>\n"
        "                <element1>\n"
        "                  <element1>LONDON</element1>\n"
        "                  <element1>LONDON</element1>\n"
        "                  <element2>arbitrary string value</element2>\n"
        "                  <element2>arbitrary string value</element2>\n"
        "                  <element3>true</element3>\n"
        "                  <element4>arbitrary string value</element4>\n"
        "                  <element5>\n"
        "                    <element1>\n"
        "                      <element1>LONDON</element1>\n"
        "                      <element1>LONDON</element1>\n"
        "                      <element2>arbitrary string value</element2>\n"
        "                      <element2>arbitrary string value</element2>\n"
        "                      <element3>true</element3>\n"
        "                      <element4>arbitrary string value</element4>\n"
        "                      <element6 xsi:nil=\"true\"/>\n"
        "                      <element6 xsi:nil=\"true\"/>\n"
        "                    </element1>\n"
        "                    <element2 xsi:nil=\"true\"/>\n"
        "                    <element2 xsi:nil=\"true\"/>\n"
        "                    <element3 xsi:nil=\"true\"/>\n"
        "                    <element3 xsi:nil=\"true\"/>\n"
        "                    <element4 xsi:nil=\"true\"/>\n"
        "                    <element4 xsi:nil=\"true\"/>\n"
        "                    <element5 xsi:nil=\"true\"/>\n"
        "                    <element5 xsi:nil=\"true\"/>\n"
        "                    <element6 xsi:nil=\"true\"/>\n"
        "                    <element6 xsi:nil=\"true\"/>\n"
        "                  </element5>\n"
        "                  <element6 xsi:nil=\"true\"/>\n"
        "                  <element6 xsi:nil=\"true\"/>\n"
        "                </element1>\n"
        "                <element2 xsi:nil=\"true\"/>\n"
        "                <element2 xsi:nil=\"true\"/>\n"
        "                <element3 xsi:nil=\"true\"/>\n"
        "                <element3 xsi:nil=\"true\"/>\n"
        "                <element4 xsi:nil=\"true\"/>\n"
        "                <element4 xsi:nil=\"true\"/>\n"
        "                <element5 xsi:nil=\"true\"/>\n"
        "                <element5 xsi:nil=\"true\"/>\n"
        "                <element6 xsi:nil=\"true\"/>\n"
        "                <element6 xsi:nil=\"true\"/>\n"
        "              </element5>\n"
        "              <element6 xsi:nil=\"true\"/>\n"
        "              <element6 xsi:nil=\"true\"/>\n"
        "            </element1>\n"
        "            <element2>\n"
        "              <selection1>2</selection1>\n"
        "            </element2>\n"
        "            <element2>\n"
        "              <selection1>2</selection1>\n"
        "            </element2>\n"
        "            <element3>FF0001</element3>\n"
        "            <element4>2</element4>\n"
        "            <element5>2012-08-18T13:25:00.000+00:00</element5>\n"
        "            <element6>custom</element6>\n"
        "            <element7>LONDON</element7>\n"
        "            <element8>true</element8>\n"
        "            <element9>arbitrary string value</element9>\n"
        "            <element10>1.5</element10>\n"
        "            <element11>FF0001</element11>\n"
        "            <element12>2</element12>\n"
        "            <element13>LONDON</element13>\n"
        "            <element14>true</element14>\n"
        "            <element14>true</element14>\n"
        "            <element15>1.5</element15>\n"
        "            <element15>1.5</element15>\n"
        "            <element16>FF0001</element16>\n"
        "            <element16>FF0001</element16>\n"
        "            <element17>2</element17>\n"
        "            <element17>2</element17>\n"
        "            <element18>2012-08-18T13:25:00.000+00:00</element18>\n"
        "            <element18>2012-08-18T13:25:00.000+00:00</element18>\n"
        "            <element19>custom</element19>\n"
        "            <element19>custom</element19>\n"
        "          </selection3>\n"
        "        </element2>\n"
        "        <element2>\n"
        "          <selection3>\n"
        "            <element1>\n"
        "              <element1>LONDON</element1>\n"
        "              <element1>LONDON</element1>\n"
        "              <element2>arbitrary string value</element2>\n"
        "              <element2>arbitrary string value</element2>\n"
        "              <element3>true</element3>\n"
        "              <element4>arbitrary string value</element4>\n"
        "              <element5>\n"
        "                <element1>\n"
        "                  <element1>LONDON</element1>\n"
        "                  <element1>LONDON</element1>\n"
        "                  <element2>arbitrary string value</element2>\n"
        "                  <element2>arbitrary string value</element2>\n"
        "                  <element3>true</element3>\n"
        "                  <element4>arbitrary string value</element4>\n"
        "                  <element5>\n"
        "                    <element1>\n"
        "                      <element1>LONDON</element1>\n"
        "                      <element1>LONDON</element1>\n"
        "                      <element2>arbitrary string value</element2>\n"
        "                      <element2>arbitrary string value</element2>\n"
        "                      <element3>true</element3>\n"
        "                      <element4>arbitrary string value</element4>\n"
        "                      <element6 xsi:nil=\"true\"/>\n"
        "                      <element6 xsi:nil=\"true\"/>\n"
        "                    </element1>\n"
        "                    <element2 xsi:nil=\"true\"/>\n"
        "                    <element2 xsi:nil=\"true\"/>\n"
        "                    <element3 xsi:nil=\"true\"/>\n"
        "                    <element3 xsi:nil=\"true\"/>\n"
        "                    <element4 xsi:nil=\"true\"/>\n"
        "                    <element4 xsi:nil=\"true\"/>\n"
        "                    <element5 xsi:nil=\"true\"/>\n"
        "                    <element5 xsi:nil=\"true\"/>\n"
        "                    <element6 xsi:nil=\"true\"/>\n"
        "                    <element6 xsi:nil=\"true\"/>\n"
        "                  </element5>\n"
        "                  <element6 xsi:nil=\"true\"/>\n"
        "                  <element6 xsi:nil=\"true\"/>\n"
        "                </element1>\n"
        "                <element2 xsi:nil=\"true\"/>\n"
        "                <element2 xsi:nil=\"true\"/>\n"
        "                <element3 xsi:nil=\"true\"/>\n"
        "                <element3 xsi:nil=\"true\"/>\n"
        "                <element4 xsi:nil=\"true\"/>\n"
        "                <element4 xsi:nil=\"true\"/>\n"
        "                <element5 xsi:nil=\"true\"/>\n"
        "                <element5 xsi:nil=\"true\"/>\n"
        "                <element6 xsi:nil=\"true\"/>\n"
        "                <element6 xsi:nil=\"true\"/>\n"
        "              </element5>\n"
        "              <element6 xsi:nil=\"true\"/>\n"
        "              <element6 xsi:nil=\"true\"/>\n"
        "            </element1>\n"
        "            <element1>\n"
        "              <element1>LONDON</element1>\n"
        "              <element1>LONDON</element1>\n"
        "              <element2>arbitrary string value</element2>\n"
        "              <element2>arbitrary string value</element2>\n"
        "              <element3>true</element3>\n"
        "              <element4>arbitrary string value</element4>\n"
        "              <element5>\n"
        "                <element1>\n"
        "                  <element1>LONDON</element1>\n"
        "                  <element1>LONDON</element1>\n"
        "                  <element2>arbitrary string value</element2>\n"
        "                  <element2>arbitrary string value</element2>\n"
        "                  <element3>true</element3>\n"
        "                  <element4>arbitrary string value</element4>\n"
        "                  <element5>\n"
        "                    <element1>\n"
        "                      <element1>LONDON</element1>\n"
        "                      <element1>LONDON</element1>\n"
        "                      <element2>arbitrary string value</element2>\n"
        "                      <element2>arbitrary string value</element2>\n"
        "                      <element3>true</element3>\n"
        "                      <element4>arbitrary string value</element4>\n"
        "                      <element6 xsi:nil=\"true\"/>\n"
        "                      <element6 xsi:nil=\"true\"/>\n"
        "                    </element1>\n"
        "                    <element2 xsi:nil=\"true\"/>\n"
        "                    <element2 xsi:nil=\"true\"/>\n"
        "                    <element3 xsi:nil=\"true\"/>\n"
        "                    <element3 xsi:nil=\"true\"/>\n"
        "                    <element4 xsi:nil=\"true\"/>\n"
        "                    <element4 xsi:nil=\"true\"/>\n"
        "                    <element5 xsi:nil=\"true\"/>\n"
        "                    <element5 xsi:nil=\"true\"/>\n"
        "                    <element6 xsi:nil=\"true\"/>\n"
        "                    <element6 xsi:nil=\"true\"/>\n"
        "                  </element5>\n"
        "                  <element6 xsi:nil=\"true\"/>\n"
        "                  <element6 xsi:nil=\"true\"/>\n"
        "                </element1>\n"
        "                <element2 xsi:nil=\"true\"/>\n"
        "                <element2 xsi:nil=\"true\"/>\n"
        "                <element3 xsi:nil=\"true\"/>\n"
        "                <element3 xsi:nil=\"true\"/>\n"
        "                <element4 xsi:nil=\"true\"/>\n"
        "                <element4 xsi:nil=\"true\"/>\n"
        "                <element5 xsi:nil=\"true\"/>\n"
        "                <element5 xsi:nil=\"true\"/>\n"
        "                <element6 xsi:nil=\"true\"/>\n"
        "                <element6 xsi:nil=\"true\"/>\n"
        "              </element5>\n"
        "              <element6 xsi:nil=\"true\"/>\n"
        "              <element6 xsi:nil=\"true\"/>\n"
        "            </element1>\n"
        "            <element2>\n"
        "              <selection1>2</selection1>\n"
        "            </element2>\n"
        "            <element2>\n"
        "              <selection1>2</selection1>\n"
        "            </element2>\n"
        "            <element3>FF0001</element3>\n"
        "            <element4>2</element4>\n"
        "            <element5>2012-08-18T13:25:00.000+00:00</element5>\n"
        "            <element6>custom</element6>\n"
        "            <element7>LONDON</element7>\n"
        "            <element8>true</element8>\n"
        "            <element9>arbitrary string value</element9>\n"
        "            <element10>1.5</element10>\n"
        "            <element11>FF0001</element11>\n"
        "            <element12>2</element12>\n"
        "            <element13>LONDON</element13>\n"
        "            <element14>true</element14>\n"
        "            <element14>true</element14>\n"
        "            <element15>1.5</element15>\n"
        "            <element15>1.5</element15>\n"
        "            <element16>FF0001</element16>\n"
        "            <element16>FF0001</element16>\n"
        "            <element17>2</element17>\n"
        "            <element17>2</element17>\n"
        "            <element18>2012-08-18T13:25:00.000+00:00</element18>\n"
        "            <element18>2012-08-18T13:25:00.000+00:00</element18>\n"
        "            <element19>custom</element19>\n"
        "            <element19>custom</element19>\n"
        "          </selection3>\n"
        "        </element2>\n"
        "        <element4>2</element4>\n"
        "        <element8>true</element8>\n"
        "        <element9>\n"
        "        </element9>\n"
        "        <element10>1.5</element10>\n"
        "        <element11>FF0001</element11>\n"
        "        <element12>-980123</element12>\n"
        "        <element13>LONDON</element13>\n"
        "        <element17>2</element17>\n"
        "        <element17>2</element17>\n"
        "      </selection3>\n"
        "    </element4>\n"
        "    <element5>1.5</element5>\n"
        "    <element6>arbitrary string value</element6>\n"
        "    <element6>arbitrary string value</element6>\n"
        "    <element7 xsi:nil=\"true\"/>\n"
        "    <element7 xsi:nil=\"true\"/>\n"
        "  </selection3>\n"
        "</Topchoice>\n",

        "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
        "<Topchoice xmlns=\"TestNamespace\"\n"
        "  xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\">\n"
        "  <selection3>\n"
        "    <element1>custom</element1>\n"
        "    <element2>255</element2>\n"
        "    <element3>2012-08-18T13:25:00.000+00:00</element3>\n"
        "    <element4>\n"
        "      <selection3>\n"
        "        <element1>\n"
        "          <element1>LONDON</element1>\n"
        "          <element1>LONDON</element1>\n"
        "          <element2>\n"
        "          </element2>\n"
        "          <element2>\n"
        "          </element2>\n"
        "          <element3>true</element3>\n"
        "          <element4>\n"
        "          </element4>\n"
        "          <element5>\n"
        "            <element1>\n"
        "              <element1>LONDON</element1>\n"
        "              <element1>LONDON</element1>\n"
        "              <element2>\n"
        "              </element2>\n"
        "              <element2>\n"
        "              </element2>\n"
        "              <element3>true</element3>\n"
        "              <element4>\n"
        "              </element4>\n"
        "              <element5>\n"
        "                <element1>\n"
        "                  <element1>LONDON</element1>\n"
        "                  <element1>LONDON</element1>\n"
        "                  <element2>\n"
        "                  </element2>\n"
        "                  <element2>\n"
        "                  </element2>\n"
        "                  <element3>true</element3>\n"
        "                  <element4>\n"
        "                  </element4>\n"
        "                  <element6>LONDON</element6>\n"
        "                  <element6>LONDON</element6>\n"
        "                </element1>\n"
        "                <element2>true</element2>\n"
        "                <element2>true</element2>\n"
        "                <element3>1.5</element3>\n"
        "                <element3>1.5</element3>\n"
        "                <element4>FF0001</element4>\n"
        "                <element4>FF0001</element4>\n"
        "                <element5 xsi:nil=\"true\"/>\n"
        "                <element5 xsi:nil=\"true\"/>\n"
        "                <element6>2012-08-18T13:25:00.000+00:00</element6>\n"
        "                <element6>2012-08-18T13:25:00.000+00:00</element6>\n"
        "              </element5>\n"
        "              <element6>LONDON</element6>\n"
        "              <element6>LONDON</element6>\n"
        "            </element1>\n"
        "            <element2>true</element2>\n"
        "            <element2>true</element2>\n"
        "            <element3>1.5</element3>\n"
        "            <element3>1.5</element3>\n"
        "            <element4>FF0001</element4>\n"
        "            <element4>FF0001</element4>\n"
        "            <element5 xsi:nil=\"true\"/>\n"
        "            <element5 xsi:nil=\"true\"/>\n"
        "            <element6>2012-08-18T13:25:00.000+00:00</element6>\n"
        "            <element6>2012-08-18T13:25:00.000+00:00</element6>\n"
        "          </element5>\n"
        "          <element6>LONDON</element6>\n"
        "          <element6>LONDON</element6>\n"
        "        </element1>\n"
        "        <element1>\n"
        "          <element1>LONDON</element1>\n"
        "          <element1>LONDON</element1>\n"
        "          <element2>\n"
        "          </element2>\n"
        "          <element2>\n"
        "          </element2>\n"
        "          <element3>true</element3>\n"
        "          <element4>\n"
        "          </element4>\n"
        "          <element5>\n"
        "            <element1>\n"
        "              <element1>LONDON</element1>\n"
        "              <element1>LONDON</element1>\n"
        "              <element2>\n"
        "              </element2>\n"
        "              <element2>\n"
        "              </element2>\n"
        "              <element3>true</element3>\n"
        "              <element4>\n"
        "              </element4>\n"
        "              <element5>\n"
        "                <element1>\n"
        "                  <element1>LONDON</element1>\n"
        "                  <element1>LONDON</element1>\n"
        "                  <element2>\n"
        "                  </element2>\n"
        "                  <element2>\n"
        "                  </element2>\n"
        "                  <element3>true</element3>\n"
        "                  <element4>\n"
        "                  </element4>\n"
        "                  <element6>LONDON</element6>\n"
        "                  <element6>LONDON</element6>\n"
        "                </element1>\n"
        "                <element2>true</element2>\n"
        "                <element2>true</element2>\n"
        "                <element3>1.5</element3>\n"
        "                <element3>1.5</element3>\n"
        "                <element4>FF0001</element4>\n"
        "                <element4>FF0001</element4>\n"
        "                <element5 xsi:nil=\"true\"/>\n"
        "                <element5 xsi:nil=\"true\"/>\n"
        "                <element6>2012-08-18T13:25:00.000+00:00</element6>\n"
        "                <element6>2012-08-18T13:25:00.000+00:00</element6>\n"
        "              </element5>\n"
        "              <element6>LONDON</element6>\n"
        "              <element6>LONDON</element6>\n"
        "            </element1>\n"
        "            <element2>true</element2>\n"
        "            <element2>true</element2>\n"
        "            <element3>1.5</element3>\n"
        "            <element3>1.5</element3>\n"
        "            <element4>FF0001</element4>\n"
        "            <element4>FF0001</element4>\n"
        "            <element5 xsi:nil=\"true\"/>\n"
        "            <element5 xsi:nil=\"true\"/>\n"
        "            <element6>2012-08-18T13:25:00.000+00:00</element6>\n"
        "            <element6>2012-08-18T13:25:00.000+00:00</element6>\n"
        "          </element5>\n"
        "          <element6>LONDON</element6>\n"
        "          <element6>LONDON</element6>\n"
        "        </element1>\n"
        "        <element2>\n"
        "          <selection3>\n"
        "            <element1>\n"
        "              <element2>\n"
        "              </element2>\n"
        "              <element2>\n"
        "              </element2>\n"
        "              <element4>\n"
        "              </element4>\n"
        "              <element5>\n"
        "                <element1>\n"
        "                  <element2>\n"
        "                  </element2>\n"
        "                  <element2>\n"
        "                  </element2>\n"
        "                  <element4>\n"
        "                  </element4>\n"
        "                  <element5>\n"
        "                    <element1>\n"
        "                      <element2>\n"
        "                      </element2>\n"
        "                      <element2>\n"
        "                      </element2>\n"
        "                      <element4>\n"
        "                      </element4>\n"
        "                      <element6>LONDON</element6>\n"
        "                      <element6>LONDON</element6>\n"
        "                    </element1>\n"
        "                    <element2>true</element2>\n"
        "                    <element2>true</element2>\n"
        "                    <element3>1.5</element3>\n"
        "                    <element3>1.5</element3>\n"
        "                    <element4>FF0001</element4>\n"
        "                    <element4>FF0001</element4>\n"
        "                    <element5>-980123</element5>\n"
        "                    <element5>-980123</element5>\n"
     "                    <element6>2012-08-18T13:25:00.000+00:00</element6>\n"
     "                    <element6>2012-08-18T13:25:00.000+00:00</element6>\n"
        "                  </element5>\n"
        "                  <element6>LONDON</element6>\n"
        "                  <element6>LONDON</element6>\n"
        "                </element1>\n"
        "                <element2>true</element2>\n"
        "                <element2>true</element2>\n"
        "                <element3>1.5</element3>\n"
        "                <element3>1.5</element3>\n"
        "                <element4>FF0001</element4>\n"
        "                <element4>FF0001</element4>\n"
        "                <element5>-980123</element5>\n"
        "                <element5>-980123</element5>\n"
        "                <element6>2012-08-18T13:25:00.000+00:00</element6>\n"
        "                <element6>2012-08-18T13:25:00.000+00:00</element6>\n"
        "              </element5>\n"
        "              <element6>LONDON</element6>\n"
        "              <element6>LONDON</element6>\n"
        "            </element1>\n"
        "            <element1>\n"
        "              <element2>\n"
        "              </element2>\n"
        "              <element2>\n"
        "              </element2>\n"
        "              <element4>\n"
        "              </element4>\n"
        "              <element5>\n"
        "                <element1>\n"
        "                  <element2>\n"
        "                  </element2>\n"
        "                  <element2>\n"
        "                  </element2>\n"
        "                  <element4>\n"
        "                  </element4>\n"
        "                  <element5>\n"
        "                    <element1>\n"
        "                      <element2>\n"
        "                      </element2>\n"
        "                      <element2>\n"
        "                      </element2>\n"
        "                      <element4>\n"
        "                      </element4>\n"
        "                      <element6>LONDON</element6>\n"
        "                      <element6>LONDON</element6>\n"
        "                    </element1>\n"
        "                    <element2>true</element2>\n"
        "                    <element2>true</element2>\n"
        "                    <element3>1.5</element3>\n"
        "                    <element3>1.5</element3>\n"
        "                    <element4>FF0001</element4>\n"
        "                    <element4>FF0001</element4>\n"
        "                    <element5>-980123</element5>\n"
        "                    <element5>-980123</element5>\n"
     "                    <element6>2012-08-18T13:25:00.000+00:00</element6>\n"
     "                    <element6>2012-08-18T13:25:00.000+00:00</element6>\n"
        "                  </element5>\n"
        "                  <element6>LONDON</element6>\n"
        "                  <element6>LONDON</element6>\n"
        "                </element1>\n"
        "                <element2>true</element2>\n"
        "                <element2>true</element2>\n"
        "                <element3>1.5</element3>\n"
        "                <element3>1.5</element3>\n"
        "                <element4>FF0001</element4>\n"
        "                <element4>FF0001</element4>\n"
        "                <element5>-980123</element5>\n"
        "                <element5>-980123</element5>\n"
        "                <element6>2012-08-18T13:25:00.000+00:00</element6>\n"
        "                <element6>2012-08-18T13:25:00.000+00:00</element6>\n"
        "              </element5>\n"
        "              <element6>LONDON</element6>\n"
        "              <element6>LONDON</element6>\n"
        "            </element1>\n"
        "            <element2>\n"
        "              <selection2>1.5</selection2>\n"
        "            </element2>\n"
        "            <element2>\n"
        "              <selection2>1.5</selection2>\n"
        "            </element2>\n"
        "            <element4>-980123</element4>\n"
        "            <element8>true</element8>\n"
        "            <element9>\n"
        "            </element9>\n"
        "            <element10>1.5</element10>\n"
        "            <element11>FF0001</element11>\n"
        "            <element12>-980123</element12>\n"
        "            <element13>LONDON</element13>\n"
        "            <element17>-980123</element17>\n"
        "            <element17>-980123</element17>\n"
        "          </selection3>\n"
        "        </element2>\n"
        "        <element2>\n"
        "          <selection3>\n"
        "            <element1>\n"
        "              <element2>\n"
        "              </element2>\n"
        "              <element2>\n"
        "              </element2>\n"
        "              <element4>\n"
        "              </element4>\n"
        "              <element5>\n"
        "                <element1>\n"
        "                  <element2>\n"
        "                  </element2>\n"
        "                  <element2>\n"
        "                  </element2>\n"
        "                  <element4>\n"
        "                  </element4>\n"
        "                  <element5>\n"
        "                    <element1>\n"
        "                      <element2>\n"
        "                      </element2>\n"
        "                      <element2>\n"
        "                      </element2>\n"
        "                      <element4>\n"
        "                      </element4>\n"
        "                      <element6>LONDON</element6>\n"
        "                      <element6>LONDON</element6>\n"
        "                    </element1>\n"
        "                    <element2>true</element2>\n"
        "                    <element2>true</element2>\n"
        "                    <element3>1.5</element3>\n"
        "                    <element3>1.5</element3>\n"
        "                    <element4>FF0001</element4>\n"
        "                    <element4>FF0001</element4>\n"
        "                    <element5>-980123</element5>\n"
        "                    <element5>-980123</element5>\n"
     "                    <element6>2012-08-18T13:25:00.000+00:00</element6>\n"
     "                    <element6>2012-08-18T13:25:00.000+00:00</element6>\n"
        "                  </element5>\n"
        "                  <element6>LONDON</element6>\n"
        "                  <element6>LONDON</element6>\n"
        "                </element1>\n"
        "                <element2>true</element2>\n"
        "                <element2>true</element2>\n"
        "                <element3>1.5</element3>\n"
        "                <element3>1.5</element3>\n"
        "                <element4>FF0001</element4>\n"
        "                <element4>FF0001</element4>\n"
        "                <element5>-980123</element5>\n"
        "                <element5>-980123</element5>\n"
        "                <element6>2012-08-18T13:25:00.000+00:00</element6>\n"
        "                <element6>2012-08-18T13:25:00.000+00:00</element6>\n"
        "              </element5>\n"
        "              <element6>LONDON</element6>\n"
        "              <element6>LONDON</element6>\n"
        "            </element1>\n"
        "            <element1>\n"
        "              <element2>\n"
        "              </element2>\n"
        "              <element2>\n"
        "              </element2>\n"
        "              <element4>\n"
        "              </element4>\n"
        "              <element5>\n"
        "                <element1>\n"
        "                  <element2>\n"
        "                  </element2>\n"
        "                  <element2>\n"
        "                  </element2>\n"
        "                  <element4>\n"
        "                  </element4>\n"
        "                  <element5>\n"
        "                    <element1>\n"
        "                      <element2>\n"
        "                      </element2>\n"
        "                      <element2>\n"
        "                      </element2>\n"
        "                      <element4>\n"
        "                      </element4>\n"
        "                      <element6>LONDON</element6>\n"
        "                      <element6>LONDON</element6>\n"
        "                    </element1>\n"
        "                    <element2>true</element2>\n"
        "                    <element2>true</element2>\n"
        "                    <element3>1.5</element3>\n"
        "                    <element3>1.5</element3>\n"
        "                    <element4>FF0001</element4>\n"
        "                    <element4>FF0001</element4>\n"
        "                    <element5>-980123</element5>\n"
        "                    <element5>-980123</element5>\n"
     "                    <element6>2012-08-18T13:25:00.000+00:00</element6>\n"
     "                    <element6>2012-08-18T13:25:00.000+00:00</element6>\n"
        "                  </element5>\n"
        "                  <element6>LONDON</element6>\n"
        "                  <element6>LONDON</element6>\n"
        "                </element1>\n"
        "                <element2>true</element2>\n"
        "                <element2>true</element2>\n"
        "                <element3>1.5</element3>\n"
        "                <element3>1.5</element3>\n"
        "                <element4>FF0001</element4>\n"
        "                <element4>FF0001</element4>\n"
        "                <element5>-980123</element5>\n"
        "                <element5>-980123</element5>\n"
        "                <element6>2012-08-18T13:25:00.000+00:00</element6>\n"
        "                <element6>2012-08-18T13:25:00.000+00:00</element6>\n"
        "              </element5>\n"
        "              <element6>LONDON</element6>\n"
        "              <element6>LONDON</element6>\n"
        "            </element1>\n"
        "            <element2>\n"
        "              <selection2>1.5</selection2>\n"
        "            </element2>\n"
        "            <element2>\n"
        "              <selection2>1.5</selection2>\n"
        "            </element2>\n"
        "            <element4>-980123</element4>\n"
        "            <element8>true</element8>\n"
        "            <element9>\n"
        "            </element9>\n"
        "            <element10>1.5</element10>\n"
        "            <element11>FF0001</element11>\n"
        "            <element12>-980123</element12>\n"
        "            <element13>LONDON</element13>\n"
        "            <element17>-980123</element17>\n"
        "            <element17>-980123</element17>\n"
        "          </selection3>\n"
        "        </element2>\n"
        "        <element3>FF0001</element3>\n"
        "        <element4>-980123</element4>\n"
        "        <element5>2012-08-18T13:25:00.000+00:00</element5>\n"
        "        <element6>custom</element6>\n"
        "        <element7>LONDON</element7>\n"
        "        <element8>true</element8>\n"
        "        <element9>arbitrary string value</element9>\n"
        "        <element10>1.5</element10>\n"
        "        <element11>FF0001</element11>\n"
        "        <element12>2</element12>\n"
        "        <element13>LONDON</element13>\n"
        "        <element14>true</element14>\n"
        "        <element14>true</element14>\n"
        "        <element15>1.5</element15>\n"
        "        <element15>1.5</element15>\n"
        "        <element16>FF0001</element16>\n"
        "        <element16>FF0001</element16>\n"
        "        <element17>-980123</element17>\n"
        "        <element17>-980123</element17>\n"
        "        <element18>2012-08-18T13:25:00.000+00:00</element18>\n"
        "        <element18>2012-08-18T13:25:00.000+00:00</element18>\n"
        "        <element19>custom</element19>\n"
        "        <element19>custom</element19>\n"
        "      </selection3>\n"
        "    </element4>\n"
        "    <element7>custom</element7>\n"
        "    <element7>custom</element7>\n"
        "  </selection3>\n"
        "</Topchoice>\n",

        "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
        "<Topchoice xmlns=\"TestNamespace\"\n"
        "  xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\">\n"
        "  <selection3>\n"
        "    <element1>custom</element1>\n"
        "    <element2>0</element2>\n"
        "    <element3>2012-08-18T13:25:00.000+00:00</element3>\n"
        "    <element4>\n"
        "      <selection4>\n"
        "        <selection1>true</selection1>\n"
        "      </selection4>\n"
        "    </element4>\n"
        "    <element5>1.5</element5>\n"
        "    <element6 xsi:nil=\"true\"/>\n"
        "    <element6 xsi:nil=\"true\"/>\n"
        "  </selection3>\n"
        "</Topchoice>\n",

        "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
        "<Topchoice xmlns=\"TestNamespace\"\n"
        "  xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\">\n"
        "  <selection3>\n"
        "    <element1>custom</element1>\n"
        "    <element2>255</element2>\n"
        "    <element3>2012-08-18T13:25:00.000+00:00</element3>\n"
        "    <element4>\n"
        "      <selection4>\n"
        "        <selection2>\n"
        "        </selection2>\n"
        "      </selection4>\n"
        "    </element4>\n"
        "    <element6>\n"
        "    </element6>\n"
        "    <element6>\n"
        "    </element6>\n"
        "    <element7 xsi:nil=\"true\"/>\n"
        "    <element7 xsi:nil=\"true\"/>\n"
        "  </selection3>\n"
        "</Topchoice>\n",

        "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
        "<Topchoice xmlns=\"TestNamespace\"\n"
        "  xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\">\n"
        "  <selection3>\n"
        "    <element1>custom</element1>\n"
        "    <element2>0</element2>\n"
        "    <element3>2012-08-18T13:25:00.000+00:00</element3>\n"
        "    <element4>\n"
        "      <selection4>\n"
        "        <selection2>arbitrary string value</selection2>\n"
        "      </selection4>\n"
        "    </element4>\n"
        "    <element5>1.5</element5>\n"
        "    <element6>arbitrary string value</element6>\n"
        "    <element6>arbitrary string value</element6>\n"
        "    <element7>custom</element7>\n"
        "    <element7>custom</element7>\n"
        "  </selection3>\n"
        "</Topchoice>\n",

        "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
        "<Topchoice xmlns=\"TestNamespace\"\n"
        "  xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\">\n"
        "  <selection3>\n"
        "    <element1>custom</element1>\n"
        "    <element2>255</element2>\n"
        "    <element3>2012-08-18T13:25:00.000+00:00</element3>\n"
        "    <element4>\n"
        "      <selection4>\n"
        "        <selection3>\n"
        "          <selection1>-980123</selection1>\n"
        "        </selection3>\n"
        "      </selection4>\n"
        "    </element4>\n"
        "  </selection3>\n"
        "</Topchoice>\n",

        "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
        "<Topchoice xmlns=\"TestNamespace\"\n"
        "  xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\">\n"
        "  <selection3>\n"
        "    <element1>custom</element1>\n"
        "    <element2>0</element2>\n"
        "    <element3>2012-08-18T13:25:00.000+00:00</element3>\n"
        "    <element4>\n"
        "      <selection1>-980123</selection1>\n"
        "    </element4>\n"
        "    <element5>1.5</element5>\n"
        "    <element6 xsi:nil=\"true\"/>\n"
        "    <element6 xsi:nil=\"true\"/>\n"
        "    <element7 xsi:nil=\"true\"/>\n"
        "    <element7 xsi:nil=\"true\"/>\n"
        "  </selection3>\n"
        "</Topchoice>\n",

        "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
        "<Topchoice xmlns=\"TestNamespace\"\n"
        "  xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\">\n"
        "  <selection4/>\n"
        "</Topchoice>\n",

        "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
        "<Topchoice xmlns=\"TestNamespace\"\n"
        "  xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\">\n"
        "  <selection4>\n"
        "    <element1>LONDON</element1>\n"
        "    <element1>LONDON</element1>\n"
        "    <element2>arbitrary string value</element2>\n"
        "    <element2>arbitrary string value</element2>\n"
        "    <element3>true</element3>\n"
        "    <element4>arbitrary string value</element4>\n"
        "    <element5>\n"
        "      <element1>\n"
        "        <element1>LONDON</element1>\n"
        "        <element1>LONDON</element1>\n"
        "        <element2>arbitrary string value</element2>\n"
        "        <element2>arbitrary string value</element2>\n"
        "        <element3>true</element3>\n"
        "        <element4>arbitrary string value</element4>\n"
        "        <element5>\n"
        "          <element1>\n"
        "            <element1>LONDON</element1>\n"
        "            <element1>LONDON</element1>\n"
        "            <element2>arbitrary string value</element2>\n"
        "            <element2>arbitrary string value</element2>\n"
        "            <element3>true</element3>\n"
        "            <element4>arbitrary string value</element4>\n"
        "            <element6 xsi:nil=\"true\"/>\n"
        "            <element6 xsi:nil=\"true\"/>\n"
        "          </element1>\n"
        "          <element2 xsi:nil=\"true\"/>\n"
        "          <element2 xsi:nil=\"true\"/>\n"
        "          <element3 xsi:nil=\"true\"/>\n"
        "          <element3 xsi:nil=\"true\"/>\n"
        "          <element4 xsi:nil=\"true\"/>\n"
        "          <element4 xsi:nil=\"true\"/>\n"
        "          <element5 xsi:nil=\"true\"/>\n"
        "          <element5 xsi:nil=\"true\"/>\n"
        "          <element6 xsi:nil=\"true\"/>\n"
        "          <element6 xsi:nil=\"true\"/>\n"
        "        </element5>\n"
        "        <element6 xsi:nil=\"true\"/>\n"
        "        <element6 xsi:nil=\"true\"/>\n"
        "      </element1>\n"
        "      <element2 xsi:nil=\"true\"/>\n"
        "      <element2 xsi:nil=\"true\"/>\n"
        "      <element3 xsi:nil=\"true\"/>\n"
        "      <element3 xsi:nil=\"true\"/>\n"
        "      <element4 xsi:nil=\"true\"/>\n"
        "      <element4 xsi:nil=\"true\"/>\n"
        "      <element5 xsi:nil=\"true\"/>\n"
        "      <element5 xsi:nil=\"true\"/>\n"
        "      <element6 xsi:nil=\"true\"/>\n"
        "      <element6 xsi:nil=\"true\"/>\n"
        "    </element5>\n"
        "    <element6 xsi:nil=\"true\"/>\n"
        "    <element6 xsi:nil=\"true\"/>\n"
        "  </selection4>\n"
        "</Topchoice>\n",

        "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
        "<Topchoice xmlns=\"TestNamespace\"\n"
        "  xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\">\n"
        "  <selection4>\n"
        "    <element2>\n"
        "    </element2>\n"
        "    <element2>\n"
        "    </element2>\n"
        "    <element4>\n"
        "    </element4>\n"
        "    <element5>\n"
        "      <element1>\n"
        "        <element2>\n"
        "        </element2>\n"
        "        <element2>\n"
        "        </element2>\n"
        "        <element4>\n"
        "        </element4>\n"
        "        <element5>\n"
        "          <element1>\n"
        "            <element2>\n"
        "            </element2>\n"
        "            <element2>\n"
        "            </element2>\n"
        "            <element4>\n"
        "            </element4>\n"
        "            <element6>LONDON</element6>\n"
        "            <element6>LONDON</element6>\n"
        "          </element1>\n"
        "          <element2>true</element2>\n"
        "          <element2>true</element2>\n"
        "          <element3>1.5</element3>\n"
        "          <element3>1.5</element3>\n"
        "          <element4>FF0001</element4>\n"
        "          <element4>FF0001</element4>\n"
        "          <element5>-980123</element5>\n"
        "          <element5>-980123</element5>\n"
        "          <element6>2012-08-18T13:25:00.000+00:00</element6>\n"
        "          <element6>2012-08-18T13:25:00.000+00:00</element6>\n"
        "        </element5>\n"
        "        <element6>LONDON</element6>\n"
        "        <element6>LONDON</element6>\n"
        "      </element1>\n"
        "      <element2>true</element2>\n"
        "      <element2>true</element2>\n"
        "      <element3>1.5</element3>\n"
        "      <element3>1.5</element3>\n"
        "      <element4>FF0001</element4>\n"
        "      <element4>FF0001</element4>\n"
        "      <element5>-980123</element5>\n"
        "      <element5>-980123</element5>\n"
        "      <element6>2012-08-18T13:25:00.000+00:00</element6>\n"
        "      <element6>2012-08-18T13:25:00.000+00:00</element6>\n"
        "    </element5>\n"
        "    <element6>LONDON</element6>\n"
        "    <element6>LONDON</element6>\n"
        "  </selection4>\n"
        "</Topchoice>\n",

        "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
        "<Topchoice xmlns=\"TestNamespace\"\n"
        "  xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\">\n"
        "  <selection4>\n"
        "    <element1>LONDON</element1>\n"
        "    <element1>LONDON</element1>\n"
        "    <element3>true</element3>\n"
        "    <element5>\n"
        "      <element1>\n"
        "        <element1>LONDON</element1>\n"
        "        <element1>LONDON</element1>\n"
        "        <element3>true</element3>\n"
        "        <element5>\n"
        "          <element1>\n"
        "            <element1>LONDON</element1>\n"
        "            <element1>LONDON</element1>\n"
        "            <element3>true</element3>\n"
        "          </element1>\n"
        "          <element5>2</element5>\n"
        "          <element5>2</element5>\n"
        "        </element5>\n"
        "      </element1>\n"
        "      <element5>2</element5>\n"
        "      <element5>2</element5>\n"
        "    </element5>\n"
        "  </selection4>\n"
        "</Topchoice>\n",

        "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
        "<Topchoice xmlns=\"TestNamespace\"\n"
        "  xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\">\n"
        "  <selection4>\n"
        "    <element2>arbitrary string value</element2>\n"
        "    <element2>arbitrary string value</element2>\n"
        "    <element4>arbitrary string value</element4>\n"
        "    <element5>\n"
        "      <element1>\n"
        "        <element2>arbitrary string value</element2>\n"
        "        <element2>arbitrary string value</element2>\n"
        "        <element4>arbitrary string value</element4>\n"
        "        <element5>\n"
        "          <element1>\n"
        "            <element2>arbitrary string value</element2>\n"
        "            <element2>arbitrary string value</element2>\n"
        "            <element4>arbitrary string value</element4>\n"
        "            <element6 xsi:nil=\"true\"/>\n"
        "            <element6 xsi:nil=\"true\"/>\n"
        "          </element1>\n"
        "          <element2 xsi:nil=\"true\"/>\n"
        "          <element2 xsi:nil=\"true\"/>\n"
        "          <element3 xsi:nil=\"true\"/>\n"
        "          <element3 xsi:nil=\"true\"/>\n"
        "          <element4 xsi:nil=\"true\"/>\n"
        "          <element4 xsi:nil=\"true\"/>\n"
        "          <element6 xsi:nil=\"true\"/>\n"
        "          <element6 xsi:nil=\"true\"/>\n"
        "        </element5>\n"
        "        <element6 xsi:nil=\"true\"/>\n"
        "        <element6 xsi:nil=\"true\"/>\n"
        "      </element1>\n"
        "      <element2 xsi:nil=\"true\"/>\n"
        "      <element2 xsi:nil=\"true\"/>\n"
        "      <element3 xsi:nil=\"true\"/>\n"
        "      <element3 xsi:nil=\"true\"/>\n"
        "      <element4 xsi:nil=\"true\"/>\n"
        "      <element4 xsi:nil=\"true\"/>\n"
        "      <element6 xsi:nil=\"true\"/>\n"
        "      <element6 xsi:nil=\"true\"/>\n"
        "    </element5>\n"
        "    <element6 xsi:nil=\"true\"/>\n"
        "    <element6 xsi:nil=\"true\"/>\n"
        "  </selection4>\n"
        "</Topchoice>\n",

        "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
        "<Topchoice xmlns=\"TestNamespace\"\n"
        "  xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\">\n"
        "  <selection5>2012-08-18T13:25:00.000+00:00</selection5>\n"
        "</Topchoice>\n",

        "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
        "<Topchoice xmlns=\"TestNamespace\"\n"
        "  xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\">\n"
        "  <selection6>custom</selection6>\n"
        "</Topchoice>\n",

        "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
        "<Topchoice xmlns=\"TestNamespace\"\n"
        "  xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\">\n"
        "  <selection7>LONDON</selection7>\n"
        "</Topchoice>\n",

        "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
        "<Topchoice xmlns=\"TestNamespace\"\n"
        "  xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\">\n"
        "  <selection8>\n"
        "    <selection1>\n"
        "      <element4>3123123123</element4>\n"
        "      <element5>255</element5>\n"
        "      <element7>custom</element7>\n"
        "      <element8>999</element8>\n"
        "    </selection1>\n"
        "  </selection8>\n"
        "</Topchoice>\n",

        "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
        "<Topchoice xmlns=\"TestNamespace\"\n"
        "  xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\">\n"
        "  <selection8>\n"
        "    <selection1>\n"
        "      <element1>0</element1>\n"
        "      <element2>custom</element2>\n"
        "      <element3>999</element3>\n"
        "      <element4>3123123123</element4>\n"
        "      <element5>0</element5>\n"
        "      <element6 xsi:nil=\"true\"/>\n"
        "      <element6 xsi:nil=\"true\"/>\n"
        "      <element7>custom</element7>\n"
        "      <element8>999</element8>\n"
        "      <element9>3123123123</element9>\n"
        "      <element10>0</element10>\n"
        "      <element10>0</element10>\n"
        "      <element11>custom</element11>\n"
        "      <element11>custom</element11>\n"
        "      <element12>3123123123</element12>\n"
        "      <element12>3123123123</element12>\n"
        "      <element13 xsi:nil=\"true\"/>\n"
        "      <element13 xsi:nil=\"true\"/>\n"
        "      <element14>999</element14>\n"
        "      <element14>999</element14>\n"
        "      <element15 xsi:nil=\"true\"/>\n"
        "      <element15 xsi:nil=\"true\"/>\n"
        "    </selection1>\n"
        "  </selection8>\n"
        "</Topchoice>\n",

        "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
        "<Topchoice xmlns=\"TestNamespace\"\n"
        "  xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\">\n"
        "  <selection8>\n"
        "    <selection1>\n"
        "      <element1>255</element1>\n"
        "      <element4>3123123123</element4>\n"
        "      <element5>255</element5>\n"
        "      <element6>999</element6>\n"
        "      <element6>999</element6>\n"
        "      <element7>custom</element7>\n"
        "      <element8>999</element8>\n"
        "      <element10>255</element10>\n"
        "      <element10>255</element10>\n"
        "      <element13>255</element13>\n"
        "      <element13>255</element13>\n"
        "      <element15>3123123123</element15>\n"
        "      <element15>3123123123</element15>\n"
        "    </selection1>\n"
        "  </selection8>\n"
        "</Topchoice>\n",

        "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
        "<Topchoice xmlns=\"TestNamespace\"\n"
        "  xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\">\n"
        "  <selection8>\n"
        "    <selection1>\n"
        "      <element2>custom</element2>\n"
        "      <element3>999</element3>\n"
        "      <element4>3123123123</element4>\n"
        "      <element5>0</element5>\n"
        "      <element7>custom</element7>\n"
        "      <element8>999</element8>\n"
        "      <element9>3123123123</element9>\n"
        "      <element11>custom</element11>\n"
        "      <element11>custom</element11>\n"
        "      <element12>3123123123</element12>\n"
        "      <element12>3123123123</element12>\n"
        "      <element13>0</element13>\n"
        "      <element13>0</element13>\n"
        "      <element14>999</element14>\n"
        "      <element14>999</element14>\n"
        "    </selection1>\n"
        "  </selection8>\n"
        "</Topchoice>\n",

        "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
        "<Topchoice xmlns=\"TestNamespace\"\n"
        "  xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\">\n"
        "  <selection8>\n"
        "    <selection2>255</selection2>\n"
        "  </selection8>\n"
        "</Topchoice>\n",

        "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
        "<Topchoice xmlns=\"TestNamespace\"\n"
        "  xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\">\n"
        "  <selection8>\n"
        "    <selection2>0</selection2>\n"
        "  </selection8>\n"
        "</Topchoice>\n",

        "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
        "<Topchoice xmlns=\"TestNamespace\"\n"
        "  xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\">\n"
        "  <selection8>\n"
        "    <selection3>custom</selection3>\n"
        "  </selection8>\n"
        "</Topchoice>\n",

        "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
        "<Topchoice xmlns=\"TestNamespace\"\n"
        "  xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\">\n"
        "  <selection8>\n"
        "    <selection4>999</selection4>\n"
        "  </selection8>\n"
        "</Topchoice>\n"
    };
    const int NUM_DATA = sizeof DATA / sizeof *DATA;

    // The 'SCHEMA' string is not used, but is provided for reference, so that
    // 'DATA' can be externally validated.

    const char SCHEMA[] =
        "<?xml version='1.0' encoding='UTF-8'?>\n"
        "<xs:schema xmlns:xs='http://www.w3.org/2001/XMLSchema'\n"
        "           xmlns:tns='urn:x-bloomberg-com:test'\n"
        "           xmlns:bdem='http://bloomberg.com/schemas/bdem'\n"
        "           targetNamespace='urn:x-bloomberg-com:test'\n"
        "           bdem:package='test'\n"
        "           elementFormDefault='qualified'>\n"
        ""
        "<xs:complexType name='Choice1'>\n"
        "  <xs:choice>\n"
        "    <xs:element name='selection1' type='xs:int'/>\n"
        "    <xs:element name='selection2' type='xs:double'/>\n"
        "    <xs:element name='selection3' type='tns:Sequence4'/>\n"
        "    <xs:element name='selection4' type='tns:Choice2'/>\n"
        "  </xs:choice>\n"
        "</xs:complexType>\n"
        ""
        "<xs:complexType name='Choice2'>\n"
        "  <xs:choice>\n"
        "    <xs:element name='selection1' type='xs:boolean'/>\n"
        "    <xs:element name='selection2' type='xs:string'/>\n"
        "    <xs:element name='selection3' type='tns:Choice1'/>\n"
        "    <xs:element name='selection4' type='xs:unsignedInt'/>\n"
        "  </xs:choice>\n"
        "</xs:complexType>\n"
        ""
        "<xs:complexType name='Choice3'>\n"
        "  <xs:choice>\n"
        "    <xs:element name='selection1' type='tns:Sequence6'/>\n"
        "    <xs:element name='selection2' type='xs:unsignedByte'/>\n"
        "    <xs:element name='selection3' type='tns:CustomString'/>\n"
        "    <xs:element name='selection4' type='tns:CustomInt'/>\n"
        "  </xs:choice>\n"
        "</xs:complexType>\n"
        ""
        "<xs:simpleType name='CustomInt'>\n"
        "  <xs:restriction base='xs:int'>\n"
        "    <xs:maxInclusive value='1000'/>\n"
        "  </xs:restriction>\n"
        "</xs:simpleType>\n"
        ""
        "<xs:simpleType name='CustomString'>\n"
        "  <xs:restriction base='xs:string'>\n"
        "    <xs:maxLength value='8'/>\n"
        "  </xs:restriction>\n"
        "</xs:simpleType>\n"
        ""
        "<xs:simpleType name='Enumerated' bdem:preserveEnumOrder='true'>\n"
        "  <xs:restriction base='xs:string'>\n"
        "     <xs:enumeration value='NEW_YORK'/>\n"
        "     <xs:enumeration value='NEW_JERSEY'/>\n"
        "     <xs:enumeration value='LONDON'/>\n"
        "  </xs:restriction>\n"
        "</xs:simpleType>\n"
        ""
        "<xs:complexType name='Sequence1'>\n"
        "  <xs:sequence>\n"
        "    <xs:element name='element1' type='tns:Choice3' minOccurs='0'/>\n"
        "    <xs:element name='element2' type='tns:Choice1'"
                                     " minOccurs='0' maxOccurs='unbounded'/>\n"
        "    <xs:element name='element3' type='tns:Choice2' />\n"
        "    <xs:element name='element4' type='tns:Choice1'"
                     " nillable='true' minOccurs='0' maxOccurs='unbounded'/>\n"
        "    <xs:element name='element5' type='tns:Choice3' minOccurs='0'"
                                                   " maxOccurs='unbounded'/>\n"
        "  </xs:sequence>\n"
        "</xs:complexType>\n"
        ""
        "<xs:complexType name='Sequence2'>\n"
        "  <xs:sequence>\n"
        "    <xs:element name='element1' type='tns:CustomString' />\n"
        "    <xs:element name='element2' type='xs:unsignedByte' />\n"
        "    <xs:element name='element3' type='xs:dateTime' />\n"
        "    <xs:element name='element4' type='tns:Choice1' minOccurs='0'/>\n"
        "    <xs:element name='element5' type='xs:double' minOccurs='0'/>\n"
        "    <xs:element name='element6' type='xs:string' nillable='true'"
                                     " minOccurs='0' maxOccurs='unbounded'/>\n"
        "    <xs:element name='element7' type='tns:CustomString'"
                     " nillable='true' minOccurs='0' maxOccurs='unbounded'/>\n"
        "  </xs:sequence>\n"
        "</xs:complexType>\n"
        ""
        "<xs:complexType name='Sequence3'>\n"
        "  <xs:sequence>\n"
        "    <xs:element name='element1' type='tns:Enumerated'"
                                     " minOccurs='0' maxOccurs='unbounded'/>\n"
        "    <xs:element name='element2' type='xs:string'"
                                     " minOccurs='0' maxOccurs='unbounded'/>\n"
        "    <xs:element name='element3' type='xs:boolean' minOccurs='0'/>\n"
        "    <xs:element name='element4' type='xs:string' minOccurs='0'/>\n"
        "    <xs:element name='element5' type='tns:Sequence5'"
                                                           " minOccurs='0'/>\n"
        "    <xs:element name='element6' type='tns:Enumerated'"
                     " nillable='true' minOccurs='0' maxOccurs='unbounded'/>\n"
        "  </xs:sequence>\n"
        "</xs:complexType>\n"
        ""
        "<xs:complexType name='Sequence4'>\n"
        "  <xs:sequence>\n"
        "    <xs:element name='element1' type='tns:Sequence3'"
                                     " minOccurs='0' maxOccurs='unbounded'/>\n"
        "    <xs:element name='element2' type='tns:Choice1'"
                                     " minOccurs='0' maxOccurs='unbounded'/>\n"
        "    <xs:element name='element3' type='xs:hexBinary' minOccurs='0'/>\n"
        "    <xs:element name='element4' type='xs:int' minOccurs='0'/>\n"
        "    <xs:element name='element5' type='xs:dateTime' minOccurs='0'/>\n"
        "    <xs:element name='element6' type='tns:CustomString'"
                                                           " minOccurs='0'/>\n"
        "    <xs:element name='element7' type='tns:Enumerated'"
                                                           " minOccurs='0'/>\n"
        "    <xs:element name='element8' type='xs:boolean' />\n"
        "    <xs:element name='element9' type='xs:string' />\n"
        "    <xs:element name='element10' type='xs:double' />\n"
        "    <xs:element name='element11' type='xs:hexBinary' />\n"
        "    <xs:element name='element12' type='xs:int' />\n"
        "    <xs:element name='element13' type='tns:Enumerated' />\n"
        "    <xs:element name='element14' type='xs:boolean'"
                                     " minOccurs='0' maxOccurs='unbounded'/>\n"
        "    <xs:element name='element15' type='xs:double'"
                                     " minOccurs='0' maxOccurs='unbounded'/>\n"
        "    <xs:element name='element16' type='xs:hexBinary'"
                                     " minOccurs='0' maxOccurs='unbounded'/>\n"
        "    <xs:element name='element17' type='xs:int'"
                                     " minOccurs='0' maxOccurs='unbounded'/>\n"
        "    <xs:element name='element18' type='xs:dateTime'"
                                     " minOccurs='0' maxOccurs='unbounded'/>\n"
        "    <xs:element name='element19' type='tns:CustomString'"
                                     " minOccurs='0' maxOccurs='unbounded'/>\n"
        "  </xs:sequence>\n"
        "</xs:complexType>\n"
        ""
        "<xs:complexType name='Sequence5'>\n"
        "  <xs:sequence>\n"
        "    <xs:element name='element1' type='tns:Sequence3' />\n"
        "    <xs:element name='element2' type='xs:boolean' nillable='true'"
                                     " minOccurs='0' maxOccurs='unbounded'/>\n"
        "    <xs:element name='element3' type='xs:double' nillable='true'"
                                     " minOccurs='0' maxOccurs='unbounded'/>\n"
        "    <xs:element name='element4' type='xs:hexBinary' nillable='true'"
                                     " minOccurs='0' maxOccurs='unbounded'/>\n"
        "    <xs:element name='element5' type='xs:int' nillable='true'"
                                     " minOccurs='0' maxOccurs='unbounded'/>\n"
        "    <xs:element name='element6' type='xs:dateTime' nillable='true'"
                                     " minOccurs='0' maxOccurs='unbounded'/>\n"
        "    <xs:element name='element7' type='tns:Sequence3' nillable='true'"
                                     " minOccurs='0' maxOccurs='unbounded'/>\n"
        "  </xs:sequence>\n"
        "</xs:complexType>\n"
        ""
        "<xs:complexType name='Sequence6'>\n"
        "  <xs:sequence>\n"
        "    <xs:element name='element1' type='xs:unsignedByte'"
                                                           " minOccurs='0'/>\n"
        "    <xs:element name='element2' type='tns:CustomString'"
                                                           " minOccurs='0'/>\n"
        "    <xs:element name='element3' type='tns:CustomInt'"
                                                           " minOccurs='0'/>\n"
        "    <xs:element name='element4' type='xs:unsignedInt' />\n"
        "    <xs:element name='element5' type='xs:unsignedByte' />\n"
        "    <xs:element name='element6' type='tns:CustomInt' nillable='true'"
                                     " minOccurs='0' maxOccurs='unbounded'/>\n"
        "    <xs:element name='element7' type='tns:CustomString' />\n"
        "    <xs:element name='element8' type='tns:CustomInt' />\n"
        "    <xs:element name='element9' type='xs:unsignedInt'"
                                                           " minOccurs='0'/>\n"
        "    <xs:element name='element10' type='xs:unsignedByte'"
                                     " minOccurs='0' maxOccurs='unbounded'/>\n"
        "    <xs:element name='element11' type='tns:CustomString'"
                                     " minOccurs='0' maxOccurs='unbounded'/>\n"
        "    <xs:element name='element12' type='xs:unsignedInt'"
                                     " minOccurs='0' maxOccurs='unbounded'/>\n"
        "    <xs:element name='element13' type='xs:unsignedByte'"
                     " nillable='true' minOccurs='0' maxOccurs='unbounded'/>\n"
        "    <xs:element name='element14' type='tns:CustomInt'"
                                     " minOccurs='0' maxOccurs='unbounded'/>\n"
        "    <xs:element name='element15' type='xs:unsignedInt'"
                     " nillable='true' minOccurs='0' maxOccurs='unbounded'/>\n"
        "  </xs:sequence>\n"
        "</xs:complexType>\n"
        ""
        "<xs:complexType name='Topchoice'>\n"
        "  <xs:choice>\n"
        "    <xs:element name='selection1' type='tns:Sequence1'/>\n"
        "    <xs:element name='selection2' type='xs:hexBinary'/>\n"
        "    <xs:element name='selection3' type='tns:Sequence2'/>\n"
        "    <xs:element name='selection4' type='tns:Sequence3'/>\n"
        "    <xs:element name='selection5' type='xs:dateTime'/>\n"
        "    <xs:element name='selection6' type='tns:CustomString'/>\n"
        "    <xs:element name='selection7' type='tns:Enumerated'/>\n"
        "    <xs:element name='selection8' type='tns:Choice3'/>\n"
        "  </xs:choice>\n"
        "</xs:complexType>\n"
        ""
        "</xs:schema>";

        (void)SCHEMA;

        balxml::MiniReader reader;

        for (int i = 0; i < NUM_DATA; ++i) {

            const bsl::string& STR = DATA[i];

            balxml::DecoderOptions options;
            balxml::ErrorInfo e;
            balxml::Decoder decoder(&options, &reader, &e);

            bdlsb::FixedMemInStreamBuf isb(STR.c_str(), STR.size());

            test::Topchoice object;
            int rc = decoder.decode(&isb, &object);
            if (rc) {
                cout << "Decoding failed for " << i
                     << " with rc: " << rc << endl;
                cout << decoder.loggedMessages() << "," << e << endl;
            }
        }
      } break;

      case 15: {
        // --------------------------------------------------------------------
        // TESTING functions related to skipped elements
        //   This test exercises functions that apply to skipped elements.
        //
        // Concerns:
        //   a. The setNumUnknownElementsSkipped sets the number of skipped
        //      elements correctly.
        //   b. The numUnknownElementsSkipped returns the number of skipped
        //      elements correctly.
        //   c. Decoding of various functions correctly increments the number
        //      of unknown elements that are skipped.
        //
        // Plan:
        //
        // Testing:
        //   void setNumUnknownElementsSkipped(int value);
        //   int numUnknownElementsSkipped() const;
        // --------------------------------------------------------------------

        if (verbose) cout << "\nFUNCTIONS related to skipped elements"
                          << "\n=====================================" << endl;

        if (verbose) cout << "\nTesting setting and getting num skipped elems."
                          << endl;
        {
            balxml::MiniReader reader;
            balxml::ErrorInfo  errInfo;
            balxml::DecoderOptions    options;

            balxml::Decoder mX(&options,
                               &reader,
                               &errInfo,
                               &bsl::cerr,
                               &bsl::cerr);
            const balxml::Decoder& X = mX;
            ASSERT(0 == X.numUnknownElementsSkipped());

            const int DATA[] = { 0, 1, 5, 100, 2000 };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;
            for (int i = 0; i < NUM_DATA; ++i) {
                const int NUM_SKIPPED_ELEMS = DATA[i];
                mX.setNumUnknownElementsSkipped(NUM_SKIPPED_ELEMS);
                LOOP3_ASSERT(i, NUM_SKIPPED_ELEMS,
                           X.numUnknownElementsSkipped(),
                           NUM_SKIPPED_ELEMS == X.numUnknownElementsSkipped());
            }
        }

        // TestSequence2
        {
            bsl::string INPUT = "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                                "<TestSequence2 " XSI ">\n"
                                "    <E3>abc</E3>\n"
                                "    <E1>123</E1>\n"
                                "</TestSequence2>\n";
            bsl::stringstream ss(INPUT);

            if (veryVerbose) {
                T_ P(INPUT)
            }

            TestSequence2 ts;

            balxml::MiniReader reader;
            balxml::ErrorInfo  errInfo;
            balxml::DecoderOptions    options;

            balxml::Decoder mX(&options,
                               &reader,
                               &errInfo,
                               &bsl::cerr,
                               &bsl::cerr);
            const balxml::Decoder& X = mX;
            ASSERT(0 == X.numUnknownElementsSkipped());

            mX.decode(ss, &ts);

            LOOP_ASSERT(ss.fail(), !ss.fail());
            LOOP_ASSERT(ts,
                    ts == TestSequence2(123, TestSequence2::DEFAULT_ELEMENT2));
            LOOP_ASSERT(X.numUnknownElementsSkipped(),
                        1 == X.numUnknownElementsSkipped());
        }

        // TestChoice2
        {
            bsl::string INPUT = "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                                "<TestChoice2 " XSI ">\n"
                                "    <S3>123</S3>\n"
                                "</TestChoice2>\n";
            bsl::stringstream ss(INPUT);

            if (veryVerbose) {
                T_ P(INPUT)
            }

            TestChoice2 tc;

            balxml::MiniReader reader;
            balxml::ErrorInfo  errInfo;
            balxml::DecoderOptions    options;

            balxml::Decoder mX(&options,
                               &reader,
                               &errInfo,
                               &bsl::cerr,
                               &bsl::cerr);
            const balxml::Decoder& X = mX;
            ASSERT(0 == X.numUnknownElementsSkipped());

            mX.decode(ss, &tc);

            LOOP_ASSERT(ss.fail(), !ss.fail());
            LOOP_ASSERT(tc, TestChoice2() == tc);
            LOOP_ASSERT(X.numUnknownElementsSkipped(),
                        1 == X.numUnknownElementsSkipped());
        }

        {
            // This test, from baea_serializableobjectproxyutil.t.cpp, used to
            // fail a safe assert there.

            bsl::string INPUT = "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                                "<selection8>\n"
                                "   <foo><selection1/></foo>\n"
                                "   <foo><selection1/></foo>\n"
                                "</selection8>\n";

            bsl::stringstream ss(INPUT);

            if (veryVerbose) { T_ P(INPUT) }

            TestChoice0 tc;

            balxml::MiniReader     reader;
            balxml::ErrorInfo      errInfo;
            balxml::DecoderOptions options;

            balxml::Decoder mX(&options,
                               &reader,
                               &errInfo,
                               &bsl::cerr,
                               &bsl::cerr);
            const balxml::Decoder& X = mX;
            ASSERT(0 == X.numUnknownElementsSkipped());

            mX.decode(ss, &tc);

            LOOP_ASSERT(ss.fail(), !ss.fail());
            LOOP_ASSERT(X.numUnknownElementsSkipped(),
                        2 == X.numUnknownElementsSkipped());
        }

        // MySequence
        {
            typedef test::MySequence Type;

            const struct {
                int         d_line;
                bsl::string d_xml;
                int         d_numSkipped;

                // Type Data members
                int         d_attribute1;
                bsl::string d_attribute2;
            } DATA[] = {
                {
                    L_,
                    "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                    "<MySequence " XSI ">\n"
                    "    <element3>45</element3>\n"
                    "</MySequence>\n",
                    1,
                    0,
                    "",
                },
                {
                    L_,
                    "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                    "<MySequence " XSI ">\n"
                    "    <attribute1>45</attribute1>\n"
                    "    <attribute2>Hello</attribute2>\n"
                    "    <attribute3>World</attribute3>\n"
                    "</MySequence>\n",
                    1,
                    45,
                    "Hello",
                },
                {
                    L_,
                    "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                    "<MySequence " XSI ">\n"
                    "    <attribute1>45</attribute1>\n"
                    "    <attribute3>World</attribute3>\n"
                    "    <attribute2>Hello</attribute2>\n"
                    "</MySequence>\n",
                    1,
                    45,
                    "Hello",
                },
                {
                    L_,
                    "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                    "<MySequence " XSI ">\n"
                    "    <attribute3>Hello</attribute3>\n"
                    "    <attribute4>World</attribute4>\n"
                    "</MySequence>\n",
                    2,
                    0,
                    "",
                },
                {
                    L_,
                    "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                    "<MySequence " XSI ">\n"
                    "    <attribute1>45</attribute1>\n"
                    "    <attribute3>Hello</attribute3>\n"
                    "    <attribute4>World</attribute4>\n"
                    "</MySequence>\n",
                    2,
                    45,
                    "",
                },
                {
                    L_,
                    "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                    "<MySequence " XSI ">\n"
                    "    <attribute1>45</attribute1>\n"
                    "    <attribute3>Hello</attribute3>\n"
                    "    <attribute2>Hello</attribute2>\n"
                    "    <attribute4>World</attribute4>\n"
                    "</MySequence>\n",
                    2,
                    45,
                    "Hello",
                },
                {
                    L_,
                    "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                    "<MySequence " XSI ">\n"
                    "    <attribute1>45</attribute1>\n"
                    "    <attribute3>Hello</attribute3>\n"
                    "    <attribute2>Hello</attribute2>\n"
                    "    <attribute4>World</attribute4>\n"
                    "    <attribute5>World</attribute5>\n"
                    "</MySequence>\n",
                    3,
                    45,
                    "Hello",
                },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const bsl::string XML         = DATA[i].d_xml;
                const int         NUM_SKIPPED = DATA[i].d_numSkipped;

                Type exp; const Type& EXP = exp;
                exp.attribute1()              = DATA[i].d_attribute1;
                exp.attribute2()              = DATA[i].d_attribute2;

                if (veryVerbose) {
                    T_ P_(i) P(XML) P(EXP)
                }

                bsl::stringstream input(XML);

                balxml::MiniReader     reader;
                balxml::ErrorInfo      errInfo;
                balxml::DecoderOptions options;

                balxml::Decoder decoder(&options,
                                        &reader,
                                        &errInfo,
                                        &bsl::cerr,
                                        &bsl::cerr);
                ASSERT(0 == decoder.numUnknownElementsSkipped());

                Type obj; const Type& OBJ = obj;
                decoder.decode(input, &obj);

                LOOP_ASSERT(input.fail(), !input.fail());
                LOOP3_ASSERT(i, EXP, OBJ, EXP == OBJ);
                LOOP2_ASSERT(decoder.numUnknownElementsSkipped(),
                             NUM_SKIPPED,
                           NUM_SKIPPED == decoder.numUnknownElementsSkipped());
            }
        }

        // MySequenceWithAnonymousChoice
        {
            typedef test::MySequenceWithAnonymousChoice Type;

            const struct {
                int         d_line;
                bsl::string d_xml;
                int         d_numSkipped;

                // Type Data members
                bool        d_attr1Specified;
                int         d_attr1;

                int         d_choiceSelectionId;
                int         d_choiceAttr1;
                bsl::string d_choiceAttr2;

                bool        d_attr2Specified;
                bsl::string d_attr2;
            } DATA[] = {
                {
                    L_,
                    "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                    "<MySequenceWithAnonymousChoice " XSI ">\n"
                    "    <attribute3>45</attribute3>\n"
                    "</MySequenceWithAnonymousChoice>\n",
                    1,

                    false, 0,        // Attribute 1
                    -1, 0, "",       // Anonymous Choice
                    false, ""        // Attribute 2
                },
                {
                    L_,
                    "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                    "<MySequenceWithAnonymousChoice " XSI ">\n"
                    "    <attribute1>35</attribute1>\n"
                    "    <attribute3>45</attribute3>\n"
                    "</MySequenceWithAnonymousChoice>\n",
                    1,

                    true, 35,        // Attribute 1
                    -1, 0, "",       // Anonymous Choice
                    false, ""        // Attribute 2
                },
                {
                    L_,
                    "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                    "<MySequenceWithAnonymousChoice " XSI ">\n"
                    "    <attribute3>45</attribute3>\n"
                    "    <attribute2>Hello</attribute2>\n"
                    "</MySequenceWithAnonymousChoice>\n",
                    1,

                    false, 0,        // Attribute 1
                    -1, 0, "",       // Anonymous Choice
                    true, "Hello"    // Attribute 2
                },
                {
                    L_,
                    "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                    "<MySequenceWithAnonymousChoice " XSI ">\n"
                    "    <myChoice1>35</myChoice1>\n"
                    "    <attribute3>45</attribute3>\n"
                    "</MySequenceWithAnonymousChoice>\n",
                    1,

                    false, 0,        // Attribute 1
                    0, 35, "",       // Anonymous Choice
                    false, ""        // Attribute 2
                },
                {
                    L_,
                    "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                    "<MySequenceWithAnonymousChoice " XSI ">\n"
                    "    <myChoice2>Hello</myChoice2>\n"
                    "    <attribute3>45</attribute3>\n"
                    "</MySequenceWithAnonymousChoice>\n",
                    1,

                    false, 0,        // Attribute 1
                    1, 0, "Hello",   // Anonymous Choice
                    false, ""        // Attribute 2
                },
                {
                    L_,
                    "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                    "<MySequenceWithAnonymousChoice " XSI ">\n"
                    "    <attribute1>35</attribute1>\n"
                    "    <attribute3>45</attribute3>\n"
                    "    <myChoice2>Hello</myChoice2>\n"
                    "    <attribute2>World</attribute2>\n"
                    "    <attribute4>World</attribute4>\n"
                    "</MySequenceWithAnonymousChoice>\n",
                    2,

                    true, 35,        // Attribute 1
                    1, 0, "Hello",   // Anonymous Choice
                    true, "World"    // Attribute 2
                },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const bsl::string XML         = DATA[i].d_xml;
                const int         NUM_SKIPPED = DATA[i].d_numSkipped;

                const bool        ATTR1_SPECIFIED = DATA[i].d_attr1Specified;
                const bool        ATTR2_SPECIFIED = DATA[i].d_attr2Specified;

                const int         SELECTION_ID   = DATA[i].d_choiceSelectionId;

                Type exp; const Type& EXP = exp;
                if (ATTR1_SPECIFIED) {
                    exp.attribute1() = DATA[i].d_attr1;
                }

                if (ATTR2_SPECIFIED) {
                    exp.attribute2() = DATA[i].d_attr2;
                }

                if (0 == SELECTION_ID) {
                    exp.choice().makeMyChoice1() = DATA[i].d_choiceAttr1;
                }
                else  if (1 == SELECTION_ID) {
                    exp.choice().makeMyChoice2() = DATA[i].d_choiceAttr2;
                }

                if (veryVerbose) {
                    T_ P_(i) P(XML) P(EXP)
                }

                bsl::stringstream input(XML);

                balxml::MiniReader     reader;
                balxml::ErrorInfo      errInfo;
                balxml::DecoderOptions options;

                balxml::Decoder decoder(&options,
                                        &reader,
                                        &errInfo,
                                        &bsl::cerr,
                                        &bsl::cerr);
                ASSERT(0 == decoder.numUnknownElementsSkipped());

                Type obj; const Type& OBJ = obj;
                decoder.decode(input, &obj);

                LOOP_ASSERT(input.fail(), !input.fail());
                LOOP3_ASSERT(i, EXP, OBJ, EXP == OBJ);
                LOOP2_ASSERT(decoder.numUnknownElementsSkipped(),
                             NUM_SKIPPED,
                           NUM_SKIPPED == decoder.numUnknownElementsSkipped());
            }
        }
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
        //   int balxml::Decoder::decode(sbuf*, TYPE, b_A*);
        //   int balxml::Decoder::decode(sbuf*, TYPE, ostrm&, ostrm&, b_A*);
        //   int balxml::Decoder::decode(istrm&, TYPE, b_A*);
        //   int balxml::Decoder::decode(istrm&, TYPE, ostrm&, ostrm&, b_A*);
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
                    "<TestSequence2 " XSI ">\n"
                    "</TestSequence2>\n",  0,       TS()                     },

            { L_,   "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                    "<TestSequence2 " XSI ">\n"
                    "  <E1>123</E1>\n"
                    "</TestSequence2>\n",  0,       TS(123,
                                                       TS::DEFAULT_ELEMENT2) },

            { L_,   "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                    "<TestSequence2 " XSI ">\n"
                    "  <E2>abc</E2>\n"
                    "</TestSequence2>\n",  0,       TS(TS::DEFAULT_ELEMENT1,
                                                       "abc")                },

            { L_,   "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                    "<TestSequence2 " XSI ">\n"
                    "  <E1>123</E1>\n"
                    "  <E2>abc</E2>\n"
                    "</TestSequence2>\n",  0,       TS(123, "abc")           },

            { L_,   "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                    "<TestSequence2 " XSI ">\n"
                    "  <E1>123</E1>\n"
                    "  <E2_Wrong>123</E2_Wrong>\n"
                    "</TestSequence2>\n",  1,       TS(123,
                                                       TS::DEFAULT_ELEMENT2) },

            { L_,   "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                    "<TestSequence2 " XSI ">\n"
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
            balxml::DecoderOptions    options;
            options.setSkipUnknownElements(false);

            balxml::MiniReader reader;
            balxml::ErrorInfo  errInfo;

            balxml::Decoder decoder(&options, &reader, &errInfo);

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
                          << "TYPE*, bsl::ostream&, ostream&)'."
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
            balxml::DecoderOptions    options;
            options.setSkipUnknownElements(false);

            // display error messages on 'bsl::cerr' only if errors are not
            // expected or if very very very verbose
            bsl::ostream& outStream
                              = (0 == EXPECTED_RET_CODE || veryVeryVeryVerbose)
                                ? bsl::cerr
                                : nullStream;

            balxml::MiniReader reader;
            balxml::ErrorInfo  errInfo;

            balxml::Decoder decoder(&options,
                                    &reader,
                                    &errInfo,
                                    &outStream,
                                    &outStream);

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
            balxml::DecoderOptions    options;
            options.setSkipUnknownElements(false);

            LOOP_ASSERT(LINE, input.good());

            balxml::MiniReader reader;
            balxml::ErrorInfo  errInfo;

            balxml::Decoder decoder(&options, &reader, &errInfo);

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
                          << "TYPE*, bsl::ostream&, ostream&)'."
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
            balxml::DecoderOptions    options;
            options.setSkipUnknownElements(false);

            // display error messages on 'bsl::cerr' only if errors are not
            // expected or if very very very verbose
            bsl::ostream& outStream
                              = (0 == EXPECTED_RET_CODE || veryVeryVeryVerbose)
                                ? bsl::cerr
                                : nullStream;

            LOOP_ASSERT(LINE, input.good());

            balxml::MiniReader reader;
            balxml::ErrorInfo  errInfo;

            balxml::Decoder  decoder(&options,
                                     &reader,
                                     &errInfo,
                                     &outStream,
                                     &outStream);
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
                              "<MySimpleContent " XSI " attribute1=\"true\" "
                              "attribute2=\"Hello World!\">"
                              ""
                              "</MySimpleContent>\n",

                              "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                              "<MySimpleContent " XSI " attribute1=\"false\" "
                              "attribute2=\"Hello World!\">"
                              "Some Stuff"
                              "</MySimpleContent>\n",

                              "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                              "<MySimpleContent " XSI " attribute1=\"true\" "
                              "attribute2=\"Hello World!\">"
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

                balxml::MiniReader     reader;
                balxml::ErrorInfo      errInfo;
                balxml::DecoderOptions options;

                balxml::Decoder decoder(&options,
                                        &reader,
                                        &errInfo,
                                        &bsl::cerr,
                                        &bsl::cerr);

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
                            "<MySimpleIntContent " XSI " attribute1=\"true\" "
                            "attribute2=\"Hello World!\">"
                            "34"
                            "</MySimpleIntContent>\n",

                            "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                            "<MySimpleIntContent " XSI " attribute1=\"false\" "
                            "attribute2=\"Hello World!\">"
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

                balxml::MiniReader     reader;
                balxml::ErrorInfo      errInfo;
                balxml::DecoderOptions options;

                balxml::Decoder decoder(&options,
                                        &reader,
                                        &errInfo,
                                        &bsl::cerr,
                                        &bsl::cerr);

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
                       "<MySequenceWithAttributes " XSI " attribute1=\"34\">\n"
                       "    <element1>45</element1>\n"
                       "    <element2>Hello</element2>\n"
                       "</MySequenceWithAttributes>\n",

                        "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                        "<MySequenceWithAttributes " XSI " attribute1=\"34\" "
                        "attribute2=\"World!\">\n"
                        "    <element1>45</element1>\n"
                        "    <element2>Hello</element2>\n"
                        "</MySequenceWithAttributes>\n",

                        "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                        "<MySequenceWithAttributes " XSI " attribute1=\"34\" "
                        "attribute2=\"  World ! \">\n"
                        "    <element1>45</element1>\n"
                        "    <element2>Hello</element2>\n"
                        "</MySequenceWithAttributes>\n",

                        "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                        "<MySequenceWithAttributes " XSI " attribute1=\"34\" "
                        "attribute2=\"  World ! \">\n"
                        "</MySequenceWithAttributes>\n",

                       "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                       "<MySequenceWithAttributes " XSI " attribute1=\"34\">\n"
                       "</MySequenceWithAttributes>\n",

                      "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                      "<MySequenceWithAttributes " XSI " attribute1=\"34\"/>\n"
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

                balxml::MiniReader     reader;
                balxml::ErrorInfo      errInfo;
                balxml::DecoderOptions options;

                balxml::Decoder decoder(&options,
                                        &reader,
                                        &errInfo,
                                        &bsl::cerr,
                                        &bsl::cerr);

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
            EXPECTED_RESULT[0].choice().makeMyChoice1(67);
            EXPECTED_RESULT[0].attribute2() = "Hello";

            EXPECTED_RESULT[1].attribute1() = 34;
            EXPECTED_RESULT[1].choice().makeMyChoice1(67);
            EXPECTED_RESULT[1].attribute2() = "Hello";

            EXPECTED_RESULT[2].attribute1() = 34;
            EXPECTED_RESULT[2].choice().makeMyChoice2("World!");
            EXPECTED_RESULT[2].attribute2() = "Hello";

            EXPECTED_RESULT[3].attribute1() = 34;
            EXPECTED_RESULT[3].choice().makeMyChoice2("  World! ");
            EXPECTED_RESULT[3].attribute2() = "Hello";

            const char *INPUT[4]
                            = {
                                "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                                "<MySequenceWithAnonymousChoice " XSI ">\n"
                                "    <attribute1>34</attribute1>\n"
                                "    <myChoice1>67</myChoice1>\n"
                                "    <attribute2>Hello</attribute2>\n"
                                "</MySequenceWithAnonymousChoice>\n",

                                "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                                "<MySequenceWithAnonymousChoice " XSI ">\n"
                                "    <attribute1>34</attribute1>\n"
                                "    <myChoice1>  67 </myChoice1>\n"
                                "    <attribute2>Hello</attribute2>\n"
                                "</MySequenceWithAnonymousChoice>\n",

                                "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                                "<MySequenceWithAnonymousChoice " XSI ">\n"
                                "    <attribute1>34</attribute1>\n"
                                "    <myChoice2>World!</myChoice2>\n"
                                "    <attribute2>Hello</attribute2>\n"
                                "</MySequenceWithAnonymousChoice>\n",

                                "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                                "<MySequenceWithAnonymousChoice " XSI ">\n"
                                "    <attribute1>34</attribute1>\n"
                                "    <myChoice2>  World! </myChoice2>\n"
                                "    <attribute2>Hello</attribute2>\n"
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

                balxml::MiniReader     reader;
                balxml::ErrorInfo      errInfo;
                balxml::DecoderOptions options;

                balxml::Decoder decoder(&options,
                                        &reader,
                                        &errInfo,
                                        &bsl::cerr,
                                        &bsl::cerr);

                decoder.decode(input, &mX);

                LOOP_ASSERT(i, input);
                LOOP3_ASSERT(i, X, Y, X == Y);
            }
        }

        if (verbose) cout << "\nEnd of Test." << endl;
      } break;
      case 10: {
        // --------------------------------------------------------------------
        // TESTING balxml::Decoder_StdVectorCharContext
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
        //   balxml::Decoder_StdVectorCharContext
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting balxml::Decoder_StdVectorCharContext"
                          << "\n========================================"
                          << endl;

        const bsl::string EXPECTED_RESULT_DATA = "abcd";

        enum FormattingMode {
            DEFAULT = bdlat_FormattingMode::e_DEFAULT,
            BASE64  = bdlat_FormattingMode::e_BASE64,
            HEX     = bdlat_FormattingMode::e_HEX,
            IS_LIST = bdlat_FormattingMode::e_LIST,
            TEXT    = bdlat_FormattingMode::e_TEXT,
            LIST_OR_DEC = IS_LIST | bdlat_FormattingMode::e_DEC
        };

        static const struct {
            int             d_lineNum;  // source line number
            const char     *d_input;    // input string
            FormattingMode  d_mode;     // formatting mode
        } DATA[] = {
            //line  input                                 mode
            //----  -----                                 ----

            { L_,   "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                    "<Value " XSI ">YWJjZA==</Value>\n",          DEFAULT,  },
            { L_,   "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                    "<Value " XSI ">YWJjZA==</Value>\n",          BASE64,   },
            { L_,   "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                    "<Value " XSI ">61626364</Value>\n",          HEX,      },
            { L_,   "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                    "<Value " XSI ">97 98 99 100</Value>\n",      IS_LIST,  },
            { L_,   "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                    "<Value " XSI ">abcd</Value>\n",              TEXT,     },
            { L_,   "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                    "<Value " XSI ">97 98 99 100</Value>\n",    LIST_OR_DEC, },
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

            balxml::DecoderOptions options;
            options.setMaxDepth(MAX_DEPTH);

            balxml::Decoder_StdVectorCharContext context(&result1,
                                                         FORMATTING_MODE);

            balxml::MiniReader reader;
            balxml::ErrorInfo  errInfo;
            balxml::Decoder    decoder(&options,
                                       &reader,
                                       &errInfo,
                                       &outStream,
                                       &outStream);

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
            cout << "\nEnd of balxml::Decoder_StdVectorCharContext Test."
                 << endl;
      } break;
      case 9: {
        // --------------------------------------------------------------------
        // TESTING balxml::Decoder_StdStringContext
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
        //   balxml::Decoder_StdStringContext
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting balxml::Decoder_StdStringContext"
                          << "\n===================================="
                          << endl;

        const bsl::string EXPECTED_RESULT = "abcd";

        enum FormattingMode {
            DEFAULT = bdlat_FormattingMode::e_DEFAULT,
            TEXT    = bdlat_FormattingMode::e_TEXT,
            BASE64  = bdlat_FormattingMode::e_BASE64,
            HEX     = bdlat_FormattingMode::e_HEX
        };

        static const struct {
            int             d_lineNum;  // source line number
            const char     *d_input;    // input string
            FormattingMode  d_mode;     // formatting mode
        } DATA[] = {
            //line  input                                 mode
            //----  -----                                 ----

            { L_,   "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                    "<Value " XSI ">abcd</Value>\n",              DEFAULT,  },
            { L_,   "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                    "<Value " XSI ">abcd</Value>\n",              TEXT,     },
            { L_,   "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                    "<Value " XSI ">YWJjZA==</Value>\n",          BASE64,   },
            { L_,   "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                    "<Value " XSI ">61626364</Value>\n",          HEX,      },
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

            balxml::DecoderOptions options;
            options.setMaxDepth(MAX_DEPTH);

            balxml::Decoder_StdStringContext context(&result1,
                                                     FORMATTING_MODE);

            balxml::MiniReader reader;
            balxml::ErrorInfo  errInfo;
            balxml::Decoder    decoder(&options,
                                       &reader,
                                       &errInfo,
                                       &outStream,
                                       &outStream);

            decoder.open(ss.rdbuf());
            int retCode = context.beginParse(&decoder);;

            LOOP2_ASSERT(LINE, retCode, 0 == retCode);
            LOOP3_ASSERT(LINE, EXPECTED_RESULT,   result1,
                               EXPECTED_RESULT == result1);
        }

        if (verbose) cout << "\nEnd of balxml::Decoder_StdStringContext Test."
                          << endl;
      } break;
      case 8: {
        // --------------------------------------------------------------------
        // TESTING balxml::Decoder_ChoiceContext<TYPE>
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
        //   supports the 'bdlat_ChoiceFunctions'.  The number at the end of
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
        //   balxml::Decoder_ChoiceContext<TYPE>
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting balxml::Decoder_ChoiceContext<TYPE>"
                          << "\n======================================="
                          << endl;

        if (verbose) cout << "\nUsing TestChoice0." << endl;
        {
            // Here we will test the 'Decoder_ChoiceContext' class template
            // using a choice with 0 selections.  We cannot really test much
            // here.  We can only test the following errors:
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
                        "<RE " XSI ">\n"
                        "</RE>\n",                                        2  },

                { L_,   "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                        "<RE " XSI ">\n"
                        "    blah"
                        "</RE>\n",                                        4  },

                { L_,   "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                        "<RE " XSI ">\n"
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

                balxml::DecoderOptions options;
                options.setMaxDepth(MAX_DEPTH);
                options.setSkipUnknownElements(false);

                balxml::Decoder_ChoiceContext<TestChoice0>
                                  context(&result,
                                          bdlat_FormattingMode::e_DEFAULT);

                bsl::ostream nullStream(0);

                // display error messages on 'bsl::cerr' only if errors are not
                // expected or if very very very verbose
                bsl::ostream& outStream
                              = (0 == EXPECTED_RET_CODE || veryVeryVeryVerbose)
                                ? bsl::cerr
                                : nullStream;

                balxml::MiniReader reader;
                balxml::ErrorInfo  errInfo;
                balxml::Decoder    decoder(&options,
                                           &reader,
                                           &errInfo,
                                           &outStream,
                                           &outStream);

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
            // Here we will test the 'Decoder_ChoiceContext' class template
            // using a choice with 1 selection.  We will test the following
            // errors:
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
                         "<RE " XSI ">\n"
                         "</RE>\n",    2,       TC(0)                        },

                { L_,    "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                         "<RE " XSI ">\n"
                         "  <S1>123</S1>\n"
                         "</RE>\n",    0,        TC(123)                     },

                { L_,    "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                         "<RE " XSI ">\n"
                         "  <S1/>\n"
                         "</RE>\n",    0,        TC(0)                       },

                { L_,    "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                         "<RE " XSI ">\n"
                         "  <S1>123</S1>\n"
                         "  <S1>456</S1>\n"
                         "</RE>\n",    0,        TC(456)                     },

                { L_,    "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                         "<RE " XSI ">\n"
                         "  <S1>123</S1>\n"
                         "  <S1>456</S1>\n"
                         "  <S1>789</S1>\n"
                         "</RE>\n",    0,        TC(789)                     },

                { L_,    "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                         "<RE " XSI ">\n"
                         "  <S1_Wrong>123</S1_Wrong>\n"
                         "</RE>\n",    3,       TC(0)                        },

                { L_,    "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                         "<RE " XSI ">\n"
                         "  <S1>blah</S1>\n"
                         "</RE>\n",    4,       TC(0)                        },

                { L_,    "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                         "<RE " XSI ">\n"
                         "  blah\n"
                         "</RE>\n",    4,       TC(0)                        },
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

                balxml::DecoderOptions options;
                options.setMaxDepth(MAX_DEPTH);
                options.setSkipUnknownElements(false);

                balxml::Decoder_ChoiceContext<TC>
                                  context(&result1,
                                          bdlat_FormattingMode::e_DEFAULT);

                bsl::ostream nullStream(0);

                // display error messages on 'bsl::cerr' only if errors are not
                // expected or if very very very verbose
                bsl::ostream& outStream
                              = (0 == EXPECTED_RET_CODE || veryVeryVeryVerbose)
                                ? bsl::cerr
                                : nullStream;

                balxml::MiniReader reader;
                balxml::ErrorInfo  errInfo;
                balxml::Decoder    decoder(&options,
                                           &reader,
                                           &errInfo,
                                           &outStream,
                                           &outStream);

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
            // Here we will test the 'Decoder_ChoiceContext' class template
            // using a choice with 2 selections.  We will test the following
            // errors:
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
                         "<RE " XSI ">\n"
                         "</RE>\n",    2,        TC(0)                       },

                { L_,    "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                         "<RE " XSI ">\n"
                         "  <S1>123</S1>\n"
                         "</RE>\n",    0,        TC(123)                     },

                { L_,    "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                         "<RE " XSI ">\n"
                         "  <S2>abc</S2>\n"
                         "</RE>\n",    0,        TC("abc")                   },

                { L_,    "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                         "<RE " XSI ">\n"
                         "  <S1/>\n"
                         "</RE>\n",    0,        TC(0)                       },

                { L_,    "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                         "<RE " XSI ">\n"
                         "  <S2/>\n"
                         "</RE>\n",    0,        TC("")                      },

                { L_,    "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                         "<RE " XSI ">\n"
                         "  <S1>123</S1>\n"
                         "  <S1>456</S1>\n"
                         "</RE>\n",    0,        TC(456)                     },

                { L_,    "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                         "<RE " XSI ">\n"
                         "  <S1>123</S1>\n"
                         "  <S2>abc</S2>\n"
                         "</RE>\n",    3,        TC(0)                       },

                { L_,    "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                         "<RE " XSI ">\n"
                         "  <S2>abc</S2>\n"
                         "  <S1>123</S1>\n"
                         "</RE>\n",    3,        TC(0)                       },

                { L_,    "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                         "<RE " XSI ">\n"
                         "  <S1>123</S1>\n"
                         "  <S1>456</S1>\n"
                         "  <S1>789</S1>\n"
                         "</RE>\n",    0,        TC(789)                     },

                { L_,    "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                         "<RE " XSI ">\n"
                         "  <S2>abc</S2>\n"
                         "  <S2>def</S2>\n"
                         "  <S2>ghi</S2>\n"
                         "</RE>\n",    0,        TC("ghi")                   },

                { L_,    "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                         "<RE " XSI ">\n"
                         "  <S1>123</S1>\n"
                         "  <S2>abc</S2>\n"
                         "  <S1>456</S1>\n"
                         "</RE>\n",    3,        TC(0)                       },

                { L_,    "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                         "<RE " XSI ">\n"
                         "  <S2>abc</S2>\n"
                         "  <S1>123</S1>\n"
                         "  <S2>def</S2>\n"
                         "</RE>\n",    3,        TC(0)                       },

                { L_,    "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                         "<RE " XSI ">\n"
                         "  <S1_Wrong>123</S1_Wrong>\n"
                         "</RE>\n",    3,        TC(0)                       },

                { L_,    "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                         "<RE " XSI ">\n"
                         "  <S2_Wrong>abc</S2_Wrong>\n"
                         "</RE>\n",    3,        TC(0)                       },

                { L_,    "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                         "<RE " XSI ">\n"
                         "  <S1>blah</S1>\n"
                         "</RE>\n",    4,        TC(0)                       },

                { L_,    "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                         "<RE " XSI ">\n"
                         "  blah\n"
                         "</RE>\n",    4,        TC(0)                       },
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

                balxml::DecoderOptions options;
                options.setMaxDepth(MAX_DEPTH);
                options.setSkipUnknownElements(false);

                balxml::Decoder_ChoiceContext<TC>
                                  context(&result1,
                                          bdlat_FormattingMode::e_DEFAULT);

                bsl::ostream nullStream(0);

                // display error messages on 'bsl::cerr' only if errors are not
                // expected or if very very very verbose
                bsl::ostream& outStream
                              = (0 == EXPECTED_RET_CODE || veryVeryVeryVerbose)
                                ? bsl::cerr
                                : nullStream;

                balxml::MiniReader reader;
                balxml::ErrorInfo  errInfo;
                balxml::Decoder    decoder(&options,
                                           &reader,
                                           &errInfo,
                                           &outStream,
                                           &outStream);

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

        if (verbose) cout << "\nEnd of balxml::Decoder_ChoiceContext<TYPE> "
                          << "Test." << endl;
      } break;
      case 7: {
        // --------------------------------------------------------------------
        // TESTING balxml::Decoder_SequenceContext<TYPE>
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
        //   supports the 'bdlat_SequenceFunctions'.  The number at the end of
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
        //   balxml::Decoder_SequenceContext<TYPE>
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting balxml::Decoder_SequenceContext<TYPE>"
                          << "\n========================================="
                          << endl;

        if (verbose) cout << "\nUsing TestSequence0." << endl;
        {
            // Here we will test the 'Decoder_SequenceContext' class template
            // using a sequence with 0 attributes.  We cannot really test
            // min/max occurrences here.  We can only test the following
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
                        "<RE " XSI ">\n"
                        "</RE>\n",                                        1  },

                { L_,   "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                        "<RE " XSI ">\n"
                        "    blah"
                        "</RE>\n",                                        3  },

                { L_,   "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                        "<RE " XSI ">\n"
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

                balxml::DecoderOptions options;
                options.setMaxDepth(MAX_DEPTH);
                options.setSkipUnknownElements(false);

                balxml::Decoder_SequenceContext<TestSequence0>
                                  context(&result,
                                          bdlat_FormattingMode::e_DEFAULT);

                bsl::ostream nullStream(0);

                // display error messages on 'bsl::cerr' only if errors are not
                // expected or if very very very verbose
                bsl::ostream& outStream
                              = (0 == EXPECTED_RET_CODE || veryVeryVeryVerbose)
                                ? bsl::cerr
                                : nullStream;

                balxml::MiniReader reader;
                balxml::ErrorInfo  errInfo;
                balxml::Decoder    decoder(&options,
                                           &reader,
                                           &errInfo,
                                           &outStream,
                                           &outStream);

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
            // Here we will test the 'Decoder_SequenceContext' class template
            // using a sequence with 1 attribute.  We will test the following
            // errors:
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
                         "<RE " XSI ">\n"
                         "</RE>\n",
                         0,     0,     0,        TS(INIT1)                   },

                { L_,    "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                         "<RE " XSI ">\n"
                         "  <E1>123</E1>\n"
                         "</RE>\n",
                         0,     1,     0,        TS(123)                     },

                { L_,    "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                         "<RE " XSI ">\n"
                         "  <E1/>\n"
                         "</RE>\n",
                         0,     1,     0,        TS(9876)                    },

                { L_,    "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                         "<RE " XSI ">\n"
                         "  <E1>123</E1>\n"
                         "</RE>\n",
                         1,     1,     0,        TS(123)                     },

                { L_,    "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                         "<RE " XSI ">\n"
                         "  <E1_Wrong>123</E1_Wrong>\n"
                         "</RE>\n",
                         0,     1,     2,       TS(0)                        },

                { L_,    "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                         "<RE " XSI ">\n"
                         "  <E1>blah</E1>\n"
                         "</RE>\n",
                         0,     1,     4,       TS(0)                        },

                { L_,    "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                         "<RE " XSI ">\n"
                         "  blah\n"
                         "</RE>\n",
                         0,     0,     2,       TS(0)                        },
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

                balxml::DecoderOptions options;
                options.setMaxDepth(MAX_DEPTH);
                options.setSkipUnknownElements(false);

                balxml::Decoder_SequenceContext<TS>
                                  context(&result1,
                                          bdlat_FormattingMode::e_DEFAULT);

                bsl::ostream nullStream(0);

                // display error messages on 'bsl::cerr' only if errors are not
                // expected or if very very very verbose
                bsl::ostream& outStream
                              = (0 == EXPECTED_RET_CODE || veryVeryVeryVerbose)
                                ? bsl::cerr
                                : nullStream;

                balxml::MiniReader reader;
                balxml::ErrorInfo  errInfo;
                balxml::Decoder    decoder(&options,
                                           &reader,
                                           &errInfo,
                                           &outStream,
                                           &outStream);

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
            // Here we will test the 'Decoder_SequenceContext' class template
            // using a sequence with 2 attributes.  We will test the following
            // errors:
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
                         "<RE " XSI ">\n"
                         "</RE>\n",
                         0,    0,    0,    0,    0,       TS(INIT1, INIT2)   },

                { L_,    "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                         "<RE " XSI ">\n"
                         "  <E1>123</E1>\n"
                         "</RE>\n",
                         0,    1,    0,    0,    0,       TS(123, INIT2)     },

                { L_,    "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                         "<RE " XSI ">\n"
                         "  <E2>abc</E2>\n"
                         "</RE>\n",
                         0,    0,    0,    1,    0,       TS(INIT1, "abc")   },

                { L_,    "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                         "<RE " XSI ">\n"
                         "  <E1>123</E1>\n"
                         "</RE>\n",
                         1,    1,    0,    0,    0,       TS(123, INIT2)     },

                { L_,    "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                         "<RE " XSI ">\n"
                         "  <E2>abc</E2>\n"
                         "</RE>\n",
                         0,    0,    1,    1,    0,       TS(INIT1, "abc")   },

                { L_,    "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                         "<RE " XSI ">\n"
                         "</RE>\n",
                         1,    1,    0,    0,    0,      TS(INIT1, INIT2)    },

                { L_,    "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                         "<RE " XSI ">\n"
                         "</RE>\n",
                         0,    0,    1,    1,    0,      TS(INIT1, INIT2)    },

                { L_,    "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                         "<RE " XSI ">\n"
                         "  <E2>abc</E2>\n"
                         "</RE>\n",
                         1,    1,    0,    1,    0,      TS(INIT1, "abc")    },

                { L_,    "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                         "<RE " XSI ">\n"
                         "  <E1>123</E1>\n"
                         "</RE>\n",
                         0,    1,    1,    1,    0,      TS(123, INIT2)      },

                { L_,    "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                         "<RE " XSI ">\n"
                         "  <E1>123</E1>\n"
                         "</RE>\n",
                         0,    0,    0,    0,    0,      TS(123, INIT2)      },

                { L_,    "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                         "<RE " XSI ">\n"
                         "  <E2>abc</E2>\n"
                         "</RE>\n",
                         0,    0,    0,    0,    0,      TS(INIT1, "abc")    },

                { L_,    "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                         "<RE " XSI ">\n"
                         "  <E1>123</E1>\n"
                         "  <E2>abc</E2>\n"
                         "</RE>\n",
                         0,    0,    0,    0,    0,      TS(123, "abc")      },

                { L_,    "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                         "<RE " XSI ">\n"
                         "  <E1/>\n"
                         "  <E2>abc</E2>\n"
                         "</RE>\n",
                         0,    0,    0,    0,    0,      TS(INIT1, "abc")    },

                { L_,    "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                         "<RE " XSI ">\n"
                         "  <E1>123</E1>\n"
                         "  <E2/>\n"
                         "</RE>\n",
                         0,    0,    0,    0,    0,      TS(123, INIT2)      },

                { L_,    "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                         "<RE " XSI ">\n"
                         "  <E1_Wrong>123</E1_Wrong>\n"
                         "</RE>\n",
                         0,    1,    0,    0,    2,      TS(0, "")           },

                { L_,    "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                         "<RE " XSI ">\n"
                         "  <E2_Wrong>abc</E2_Wrong>\n"
                         "</RE>\n",
                         0,    0,    0,    1,    2,      TS(0, "")           },

                { L_,    "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                         "<RE " XSI ">\n"
                         "  <E1>blah</E1>\n"
                         "</RE>\n",
                         0,    1,    0,    0,    4,      TS(0, "")           },

                { L_,    "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                         "<RE " XSI ">\n"
                         "  blah\n"
                         "</RE>\n",
                         0,    0,    0,    0,    2,      TS(0, "")           },
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

                balxml::DecoderOptions options;
                options.setMaxDepth(MAX_DEPTH);
                options.setSkipUnknownElements(false);

                balxml::Decoder_SequenceContext<TS>
                                  context(&result1,
                                          bdlat_FormattingMode::e_DEFAULT);

                bsl::ostream nullStream(0);

                // display error messages on 'bsl::cerr' only if errors are not
                // expected or if very very very verbose
                bsl::ostream& outStream
                              = (0 == EXPECTED_RET_CODE || veryVeryVeryVerbose)
                                ? bsl::cerr
                                : nullStream;

                balxml::MiniReader reader;
                balxml::ErrorInfo  errInfo;
                balxml::Decoder    decoder(&options,
                                           &reader,
                                           &errInfo,
                                           &outStream,
                                           &outStream);

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
                                "<MySequenceWithNullables " XSI ">\n"
                                "    <attribute2>test string</attribute2>\n"
                                "    <attribute3>\n"
                                "        <attribute1>987</attribute1>\n"
                                "        <attribute2>inner</attribute2>\n"
                                "    </attribute3>\n"
                                "</MySequenceWithNullables>\n",

                                "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                                "<MySequenceWithNullables " XSI ">\n"
                                "    <attribute1>123</attribute1>\n"
                                "    <attribute3>\n"
                                "        <attribute1>987</attribute1>\n"
                                "        <attribute2>inner</attribute2>\n"
                                "    </attribute3>\n"
                                "</MySequenceWithNullables>\n",

                                "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                                "<MySequenceWithNullables " XSI ">\n"
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
                    T_ P_(i) P_(X) P(INPUT[i])
                }

                bsl::stringstream input(INPUT[i]);

                Type mY;  const Type& Y = mY;
                LOOP3_ASSERT(i, X, Y, X != Y);

                balxml::MiniReader     reader;
                balxml::ErrorInfo      errInfo;
                balxml::DecoderOptions options;

                balxml::Decoder decoder(&options,
                                        &reader,
                                        &errInfo,
                                        &bsl::cerr,
                                        &bsl::cerr);

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
                                "<MySequenceWithNillables " XSI ">\n"
                                "    <attribute1 xsi:nil='true'/>\n"
                                "    <attribute2>test string</attribute2>\n"
                                "    <attribute3>\n"
                                "        <attribute1>987</attribute1>\n"
                                "        <attribute2>inner</attribute2>\n"
                                "    </attribute3>\n"
                                "</MySequenceWithNillables>\n",

                                "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                                "<MySequenceWithNillables " XSI ">\n"
                                "    <attribute1>123</attribute1>\n"
                                "    <attribute2/>\n"
                                "    <attribute3>\n"
                                "        <attribute1>987</attribute1>\n"
                                "        <attribute2>inner</attribute2>\n"
                                "    </attribute3>\n"
                                "</MySequenceWithNillables>\n",

                                "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                                "<MySequenceWithNillables " XSI ">\n"
                                "    <attribute1>123</attribute1>\n"
                                "    <attribute2>test string</attribute2>\n"
                                "    <attribute3 xsi:nil='true'/>\n"
                                "</MySequenceWithNillables>\n",
                              };

            for (int i = 0; i < Type::NUM_ATTRIBUTES; ++i) {
                Type mX; const Type& X = mX;
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

                balxml::MiniReader     reader;
                balxml::ErrorInfo      errInfo;
                balxml::DecoderOptions options;
                options.setSkipUnknownElements(false);

                balxml::Decoder decoder(&options,
                                        &reader,
                                        &errInfo,
                                        &bsl::cerr,
                                        &bsl::cerr);

                decoder.decode(input, &mY);

                LOOP_ASSERT(i, input);
                LOOP3_ASSERT(i, X, Y, X == Y);
            }
        }

        if (verbose) cout << "\nEnd of balxml::Decoder_SequenceContext<TYPE> "
                          << "Test." << endl;
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // TESTING balxml::Decoder_ParseObject
        //   This will test the 'Decoder_ParseObject' function class.
        //
        // Concerns:
        //   When the function object is called, the correct 'execute' method
        //   must be called (i.e., for 'TYPE's that fall into the
        //   'bdlat_ArrayCategory' (except 'bsl::vector<char>'), the object
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
        //   balxml::Decoder_ParseObject
        // --------------------------------------------------------------------

        //if (verbose) cout << "\nTesting 'Decoder_ParseObject'"
        //                  << "\n=================================" << endl;

        //bsl::ostream            nullStream(0);
        //bsl::string             nullElementName;
        //TestErrorReporter       errorReporter(nullStream, nullStream);
        //TestParser              parser(&errorReporter);

        //baexml_Decoder_DecoderContext decoderContext;
        //decoderContext.d_reporter_p = &errorReporter;

        //balxml::Decoder_ParseObject parseObject(&parser,
        //                                    &decoderContext,
        //                                    nullElementName.c_str(),
        //                                    nullElementName.length());

        //if (verbose) cout << "\nUsing 'int'." << endl;
        //{
        //    int object = 987;

        //    parser.setCharsToAdd("123");
        //    parseObject.execute(&object,
        //                        bdlat_FormattingMode::e_DEFAULT);

        //    LOOP_ASSERT(object, 123 == object);
        //}

        //if (verbose) cout << "\nUsing 'bdlb::NullableValue<int>'." << endl;
        //{
        //    bdlb::NullableValue<int> object;

        //    parser.setCharsToAdd("123");
        //    parseObject.execute(&object,
        //                        bdlat_FormattingMode::e_DEFAULT);

        //    LOOP_ASSERT(object, 123 == object.value());
        //}
        //{
        //    bdlb::NullableValue<int> object;

        //    object.makeValue(432);

        //    parser.setCharsToAdd("123");
        //    parseObject.execute(&object,
        //                        bdlat_FormattingMode::e_DEFAULT);

        //    LOOP_ASSERT(object, 123 == object.value());
        //}

        //if (verbose) cout << "\nUsing 'bsl::vector<char>'." << endl;
        //{
        //    const char INIT[] = "InIt VaLuE";

        //    bsl::vector<char> object(INIT, INIT + sizeof(INIT)-1);

        //    parser.setCharsToAdd("YWJjZA==");
        //    parseObject.execute(&object,
        //                        bdlat_FormattingMode::e_DEFAULT);

        //    bsl::string value(&object[0], object.size());

        //    LOOP_ASSERT(object, "abcd" == value);
        //}

        //if (verbose) cout << "\nUsing 'bsl::vector<int>'." << endl;
        //{
        //    bsl::vector<int> object;

        //    parser.setCharsToAdd("123");
        //    parseObject.execute(&object, bdlat_FormattingMode::e_DEC);

        //    LOOP_ASSERT(object.size(), 1   == object.size());
        //    LOOP_ASSERT(object[0],     123 == object[0]);

        //    parser.setCharsToAdd("456");
        //    parseObject.execute(&object, bdlat_FormattingMode::e_DEC);

        //    LOOP_ASSERT(object.size(), 2   == object.size());
        //    LOOP_ASSERT(object[0],     123 == object[0]);
        //    LOOP_ASSERT(object[1],     456 == object[1]);

        //    parser.setCharsToAdd("789");
        //    parseObject.execute(&object, bdlat_FormattingMode::e_DEC);

        //    LOOP_ASSERT(object.size(), 3   == object.size());
        //    LOOP_ASSERT(object[0],     123 == object[0]);
        //    LOOP_ASSERT(object[1],     456 == object[1]);
        //    LOOP_ASSERT(object[2],     789 == object[2]);
        //}

        //if (verbose) cout << "\nUsing 'bsl::vector<int>' (list)." << endl;
        //{
        //    bsl::vector<int> object;

        //    parser.setCharsToAdd("  123 456   789");
        //    parseObject.execute(&object, bdlat_FormattingMode::e_LIST);

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
        // TESTING balxml::Decoder_SimpleContext<TYPE>
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
        //   balxml::Decoder_SimpleContext<TYPE>
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting balxml::Decoder_SimpleContext<TYPE>"
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
                    "<Value " XSI ">-2147483648</Value>\n", 0,  -2147483647-1},
            { L_,   "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                    "<Value " XSI ">-1</Value>\n",       0,        -1        },
            { L_,   "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                    "<Value " XSI ">0</Value>\n",        0,        0         },
            { L_,   "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                    "<Value " XSI ">1</Value>\n",        0,        1         },
            { L_,   "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                    "<Value " XSI ">2147483647</Value>\n", 0,     2147483647 },

            // arbitrary values
            { L_,   "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                    "<Value " XSI ">123</Value>\n",      0,        123       },
            { L_,   "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                    "<Value " XSI ">-4567</Value>\n",    0,        -4567     },

            // arbitrary values with surrounding whitespace
            { L_,   "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                    "<Value " XSI "> \n 123  </Value>\n",0,        123       },
            { L_,   "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                    "<Value " XSI "> -4567 \n\t </Value>\n", 0,    -4567     },

            // invalid input
            { L_,   "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                    "<Value " XSI ">123<Bad></Bad></Value>\n", 3,  0         },
            { L_,   "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                    "<Value " XSI ">123 567</Value>\n",        2,  0         },
            { L_,   "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                    "<Value " XSI ">blah</Value>\n",           2,  0         },
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

            balxml::DecoderOptions options;
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

            balxml::Decoder_SimpleContext<int> context(
                                              &result1,
                                              bdlat_FormattingMode::e_DEFAULT);

            balxml::MiniReader reader;
            balxml::ErrorInfo  errInfo;
            balxml::Decoder    decoder(&options,
                                       &reader,
                                       &errInfo,
                                       &outStream,
                                       &outStream);

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

        if (verbose) cout << "\nEnd of balxml::Decoder_SimpleContext<TYPE> "
                          << "Test." << endl;
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING balxml::Decoder_UTF8Context
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
        //   balxml::Decoder_UTF8Context
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting balxml::Decoder_UTF8Context"
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
                    "<Value " XSI "></Value>\n",           0,          ""    },

            { L_,   "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                    "<Value " XSI "> </Value>\n",          0,          " "   },
            { L_,   "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                    "<Value " XSI ">\n</Value>\n",         0,          "\n"  },
            { L_,   "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                    "<Value " XSI ">  </Value>\n",         0,          "  "  },
            { L_,   "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                    "<Value " XSI "></Value>\n",           0,          ""    },

            { L_,   "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                    "<Value " XSI ">abc</Value>\n",        0,          "abc" },
            { L_,   "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                    "<Value " XSI ">ab cd</Value>\n",      0,        "ab cd" },

            { L_,   "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                    "<Value " XSI ">A&amp;B</Value>\n",    0,          "A&B" },
            { L_,   "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                    "<Value " XSI ">A&lt;B</Value>\n",     0,          "A<B" },
            { L_,   "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                    "<Value " XSI ">A&gt;B</Value>\n",     0,          "A>B" },
            { L_,   "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                    "<Value " XSI ">A&apos;B</Value>\n",   0,          "A\'B"},
            { L_,   "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                    "<Value " XSI ">A&quot;B</Value>\n",   0,          "A\"B"},
            { L_,   "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                    "<Value " XSI ">A" "\xC3\xA4" "B</Value>\n", 0,
                                                         "A" "\xC3\xA4" "B"  },

            { L_,   "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                    "<Value " XSI ">abc<Bad></Bad></Value>\n",   3,     ""   },
        };
        const int NUM_DATA  = sizeof DATA / sizeof *DATA;
        const int MAX_DEPTH = 5;

        for (int i = 0; i < NUM_DATA; ++i) {
            const int   LINE              = DATA[i].d_lineNum;
            const char *INPUT             = DATA[i].d_input;
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

                balxml::DecoderOptions options;
                options.setMaxDepth(MAX_DEPTH);

                // display error messages on 'bsl::cerr' only if errors are not
                // expected or if very very very verbose
                bsl::ostream& outStream
                              = (0 == EXPECTED_RET_CODE || veryVeryVeryVerbose)
                                ? bsl::cerr
                                : nullStream;

                balxml::Decoder_UTF8Context<Type> context(
                                              &result1,
                                              bdlat_FormattingMode::e_DEFAULT);
                balxml::MiniReader reader;
                balxml::ErrorInfo  errInfo;
                balxml::Decoder    decoder(&options,
                                           &reader,
                                           &errInfo,
                                           &outStream,
                                           &outStream);

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
                    const bsl::size_t EXPECTED_DATA_LENGTH = bsl::strlen(
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

                balxml::DecoderOptions options;
                options.setMaxDepth(MAX_DEPTH);

                balxml::Decoder_UTF8Context<Type> context(
                                                 &result1,
                                                 bdlat_FormattingMode::e_TEXT);

                balxml::MiniReader reader;
                balxml::ErrorInfo  errInfo;
                balxml::Decoder    decoder(&options,
                                           &reader,
                                           &errInfo,
                                           &outStream,
                                           &outStream);

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
                    const bsl::size_t EXPECTED_DATA_LENGTH = bsl::strlen(
                                                                EXPECTED_DATA);

                    Type EXPECTED_RESULT(EXPECTED_DATA,
                                         EXPECTED_DATA + EXPECTED_DATA_LENGTH);

                    LOOP3_ASSERT(LINE, EXPECTED_RESULT,   result1,
                                       EXPECTED_RESULT == result1);
                }
            }
        }

        if (verbose) cout << "\nEnd of balxml::Decoder_UTF8Context Test."
                          << endl;
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING balxml::Decoder_SelectContext
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
        //   balxml::Decoder_SelectContext
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting balxml::Decoder_SelectContext"
                          << "\n=================================" << endl;

        if (verbose) cout << "\nUsing sequence types." << endl;
        {
            {
                typedef TestSequence0                     TestType;
                typedef
                balxml::Decoder_SelectContext<TestType>::Type Result;
                typedef
                balxml::Decoder_SequenceContext<TestType>     ExpectedResult;

                ASSERT((bslmf::IsSame<Result, ExpectedResult>::VALUE));
            }
            {
                typedef TestSequence1                     TestType;
                typedef
                balxml::Decoder_SelectContext<TestType>::Type Result;
                typedef
                balxml::Decoder_SequenceContext<TestType>     ExpectedResult;

                ASSERT((bslmf::IsSame<Result, ExpectedResult>::VALUE));
            }
            {
                typedef TestSequence2                     TestType;
                typedef
                balxml::Decoder_SelectContext<TestType>::Type Result;
                typedef
                balxml::Decoder_SequenceContext<TestType>     ExpectedResult;

                ASSERT((bslmf::IsSame<Result, ExpectedResult>::VALUE));
            }
        }

        if (verbose) cout << "\nUsing choice types." << endl;
        {
            {
                typedef TestChoice0                       TestType;
                typedef
                balxml::Decoder_SelectContext<TestType>::Type Result;
                typedef
                balxml::Decoder_ChoiceContext<TestType>       ExpectedResult;

                ASSERT((bslmf::IsSame<Result, ExpectedResult>::VALUE));
            }
            {
                typedef TestChoice1                       TestType;
                typedef
                balxml::Decoder_SelectContext<TestType>::Type Result;
                typedef
                balxml::Decoder_ChoiceContext<TestType>       ExpectedResult;

                ASSERT((bslmf::IsSame<Result, ExpectedResult>::VALUE));
            }
            {
                typedef TestChoice2                       TestType;
                typedef
                balxml::Decoder_SelectContext<TestType>::Type Result;
                typedef
                balxml::Decoder_ChoiceContext<TestType>       ExpectedResult;

                ASSERT((bslmf::IsSame<Result, ExpectedResult>::VALUE));
            }
        }

        if (verbose) cout << "\nUsing simple types." << endl;
        {
            {
                typedef int                               TestType;
                typedef
                balxml::Decoder_SelectContext<TestType>::Type Result;
                typedef
                balxml::Decoder_SimpleContext<TestType>       ExpectedResult;

                ASSERT((bslmf::IsSame<Result, ExpectedResult>::VALUE));
            }
            {
                typedef short                             TestType;
                typedef
                balxml::Decoder_SelectContext<TestType>::Type Result;
                typedef
                balxml::Decoder_SimpleContext<TestType>       ExpectedResult;

                ASSERT((bslmf::IsSame<Result, ExpectedResult>::VALUE));
            }
            {
                typedef double                            TestType;
                typedef
                balxml::Decoder_SelectContext<TestType>::Type Result;
                typedef
                balxml::Decoder_SimpleContext<TestType>       ExpectedResult;

                ASSERT((bslmf::IsSame<Result, ExpectedResult>::VALUE));
            }
            {
                typedef float                             TestType;
                typedef
                balxml::Decoder_SelectContext<TestType>::Type Result;
                typedef
                balxml::Decoder_SimpleContext<TestType>       ExpectedResult;

                ASSERT((bslmf::IsSame<Result, ExpectedResult>::VALUE));
            }
            {
                typedef long long                         TestType;
                typedef
                balxml::Decoder_SelectContext<TestType>::Type Result;
                typedef
                balxml::Decoder_SimpleContext<TestType>       ExpectedResult;

                ASSERT((bslmf::IsSame<Result, ExpectedResult>::VALUE));
            }
            {
                typedef DummyEnumeration::Value           TestType;
                typedef
                balxml::Decoder_SelectContext<TestType>::Type Result;
                typedef
                balxml::Decoder_SimpleContext<TestType>       ExpectedResult;

                ASSERT((bslmf::IsSame<Result, ExpectedResult>::VALUE));
            }
            {
                typedef DummyCustomizedType               TestType;
                typedef
                balxml::Decoder_SelectContext<TestType>::Type Result;
                typedef
                balxml::Decoder_CustomizedContext<TestType>   ExpectedResult;

                ASSERT((bslmf::IsSame<Result, ExpectedResult>::VALUE));
            }
        }

        if (verbose) cout << "\nUsing array types." << endl;
        {
            {
                typedef bsl::vector<int>                        TestType;
                typedef
                balxml::Decoder_SelectContext<TestType>::Type       Result;
                typedef
                balxml::Decoder_PushParserContext<
                         TestType,
                         balxml::Decoder_ListParser<TestType> > ExpectedResult;

                ASSERT((bslmf::IsSame<Result, ExpectedResult>::VALUE));
            }
            {
                typedef bsl::vector<short>                      TestType;
                typedef
                balxml::Decoder_SelectContext<TestType>::Type       Result;
                typedef
                balxml::Decoder_PushParserContext<
                         TestType,
                         balxml::Decoder_ListParser<TestType> > ExpectedResult;

                ASSERT((bslmf::IsSame<Result, ExpectedResult>::VALUE));
            }
            {
                typedef bsl::vector<double>                     TestType;
                typedef
                balxml::Decoder_SelectContext<TestType>::Type       Result;
                typedef
                balxml::Decoder_PushParserContext<
                         TestType,
                         balxml::Decoder_ListParser<TestType> > ExpectedResult;

                ASSERT((bslmf::IsSame<Result, ExpectedResult>::VALUE));
            }
        }

        if (verbose) cout << "\nUsing 'bsl::string'." << endl;
        {
            {
                typedef bsl::string                       TestType;
                typedef
                balxml::Decoder_SelectContext<TestType>::Type Result;
                typedef balxml::Decoder_StdStringContext      ExpectedResult;

                ASSERT((bslmf::IsSame<Result, ExpectedResult>::VALUE));
            }
        }

        if (verbose) cout << "\nUsing 'bsl::vector<char>'." << endl;
        {
            {
                typedef bsl::vector<char>                 TestType;
                typedef
                balxml::Decoder_SelectContext<TestType>::Type Result;
                typedef balxml::Decoder_StdVectorCharContext  ExpectedResult;

                ASSERT((bslmf::IsSame<Result, ExpectedResult>::VALUE));
            }
        }

        if (verbose) cout << "\nEnd of balxml::Decoder_SelectContext Test."
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

            bslma::TestAllocator testAllocator;

            balxml::DecoderOptions options;
            options.setMaxDepth(MAX_DEPTH);

            TestContext context(callSequence, XML_NAME, &testAllocator);

            balxml::MiniReader reader;
            balxml::ErrorInfo  errInfo;
            balxml::Decoder    decoder(&options,
                                       &reader,
                                       &errInfo,
                                       &errorStream,
                                       &warningStream);

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
                                "<Value " XSI ">\n"
                                "    123\n"
                                "</Value>\n";
            bsl::stringstream ss(INPUT);

            if (veryVerbose) {
                T_ P(INPUT)
            }

            int i;

            balxml::MiniReader     reader;
            balxml::ErrorInfo      errInfo;
            balxml::DecoderOptions options;

            balxml::Decoder decoder(&options,
                                    &reader,
                                    &errInfo,
                                    &bsl::cerr,
                                    &bsl::cerr);

           int ret = decoder.decode(ss.rdbuf(), &i);

            LOOP_ASSERT(ret, 0 == ret);
            LOOP_ASSERT(i, 123 == i);
        }

        if (verbose) cout << "\nTesting bsl::string." << endl;
        {
            bsl::string INPUT = "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                                "<Value " XSI ">\n"
                                "    abc\n"
                                "</Value>\n";
            bsl::stringstream ss(INPUT);

            if (veryVerbose) {
                T_ P(INPUT)
            }

            bsl::string s;

            balxml::MiniReader     reader;
            balxml::ErrorInfo      errInfo;
            balxml::DecoderOptions options;

            balxml::Decoder decoder(&options,
                                    &reader,
                                    &errInfo,
                                    &bsl::cerr,
                                    &bsl::cerr);

            int ret = decoder.decode(ss.rdbuf(), &s);

            LOOP_ASSERT(ret, 0 == ret);
            LOOP_ASSERT(s, "\n    abc\n" == s);
        }

        if (verbose) cout << "\nTesting TestSequence2." << endl;
        {
            bsl::string INPUT = "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                                "<TestSequence2 " XSI ">\n"
                                "    <E1>123</E1>\n"
                                "    <E2>abc</E2>\n"
                                "</TestSequence2>\n";
            bsl::stringstream ss(INPUT);

            if (veryVerbose) {
                T_ P(INPUT)
            }

            TestSequence2 ts;

            balxml::MiniReader     reader;
            balxml::ErrorInfo      errInfo;
            balxml::DecoderOptions options;

            balxml::Decoder decoder(&options,
                                    &reader,
                                    &errInfo,
                                    &bsl::cerr,
                                    &bsl::cerr);

            decoder.decode(ss, &ts);

            LOOP_ASSERT(ss.fail(), !ss.fail());
            LOOP_ASSERT(ts, TestSequence2(123, "abc") == ts);
        }

        if (verbose) cout << "\nTesting TestChoice2." << endl;
        {
            bsl::string INPUT = "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                                "<TestChoice2 " XSI ">\n"
                                "    <S1>123</S1>\n"
                                "</TestChoice2>\n";
            bsl::stringstream ss(INPUT);

            if (veryVerbose) {
                T_ P(INPUT)
            }

            TestChoice2 tc;

            balxml::MiniReader     reader;
            balxml::ErrorInfo      errInfo;
            balxml::DecoderOptions options;

            balxml::Decoder decoder(&options,
                                    &reader,
                                    &errInfo,
                                    &bsl::cerr,
                                    &bsl::cerr);

            decoder.decode(ss, &tc);

            LOOP_ASSERT(ss.fail(), !ss.fail());
            LOOP_ASSERT(tc, TestChoice2(123) == tc);
        }

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
