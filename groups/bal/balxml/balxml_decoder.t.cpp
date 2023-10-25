// balxml_decoder.t.cpp                                               -*-C++-*-

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

#include <balxml_decoder.h>

#include <balxml_decoderoptions.h>
#include <balxml_errorinfo.h>
#include <balxml_minireader.h>

#include <s_baltst_address.h>
#include <s_baltst_customint.h>
#include <s_baltst_customstring.h>
#include <s_baltst_employee.h>
#include <s_baltst_enumerated.h>
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
#include <s_baltst_mysequencewithnillables.h>
#include <s_baltst_mysequencewithnullables.h>
#include <s_baltst_mysimplecontent.h>
#include <s_baltst_mysimpleintcontent.h>
#include <s_baltst_simplerequest.h>
#include <s_baltst_ratsnest.h>
#include <s_baltst_testchoice.h>
#include <s_baltst_testcustomizedtype.h>
#include <s_baltst_testdynamictype.h>
#include <s_baltst_testenumeration.h>
#include <s_baltst_testnilvalue.h>
#include <s_baltst_testplaceholder.h>
#include <s_baltst_testselection.h>
#include <s_baltst_testsequence.h>
#include <s_baltst_testtaggedvalue.h>
#include <s_baltst_topchoice.h>

#include <bslim_testutil.h>

#include <bdlat_attributeinfo.h>
#include <bdlat_choicefunctions.h>
#include <bdlat_enumeratorinfo.h>
#include <bdlat_formattingmode.h>
#include <bdlat_selectioninfo.h>
#include <bdlat_sequencefunctions.h>
#include <bdlat_typetraits.h>
#include <bdlat_valuetypefunctions.h>

#include <bdlb_chartype.h>
#include <bdlb_nullableallocatedvalue.h>
#include <bdlb_nullablevalue.h>
#include <bdlb_print.h>
#include <bdlb_printmethods.h>
#include <bdlb_string.h>
#include <bdlb_variant.h>
#include <bdlde_utf8util.h>
#include <bdldfp_decimal.h>
#include <bdls_filesystemutil.h>
#include <bdls_osutil.h>
#include <bdls_processutil.h>
#include <bdlsb_fixedmeminstreambuf.h>
#include <bdlsb_fixedmemoutstreambuf.h>
#include <bdlt_datetimetz.h>

#include <bslim_testutil.h>

#include <bsla_fallthrough.h>
#include <bslalg_typetraits.h>
#include <bslma_allocator.h>
#include <bslma_default.h>
#include <bslma_testallocator.h>
#include <bslmf_issame.h>

#include <bsls_assert.h>
#include <bsls_buildtarget.h>
#include <bsls_platform.h>
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
#include <bsl_string_view.h>
#include <bsl_typeinfo.h>
#include <bsl_unordered_map.h>
#include <bsl_unordered_set.h>

using namespace BloombergLP;
using bsl::cout;
using bsl::cerr;
using bsl::endl;
using bsl::flush;
namespace Test = s_baltst;

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
// [22] REPRODUCE SCENARIO FROM DRQS 169438741
// [-1] TESTING VALID & INVALID UTF-8: e_STRING
// [-1] TESTING VALID & INVALID UTF-8: e_STREAMBUF
// [-1] TESTING VALID & INVALID UTF-8: e_ISTREAM
// [-1] TESTING VALID & INVALID UTF-8: e_FILE
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

#define T2_          (cout << "  ");

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

int test;

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
    struct IsSequence<TestSequence0> : bsl::true_type {
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
    struct IsSequence<TestSequence1> : bsl::true_type {
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
: bsl::true_type { };
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
    struct IsChoice<TestChoice0> : bsl::true_type {
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
    struct IsChoice<TestChoice1> : bsl::true_type {
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
    struct IsChoice<TestChoice2> : bsl::true_type {
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
    struct IsSequence<TestSequenceWithVector> : bsl::true_type {
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
    struct IsEnumeration<DummyEnumeration::Value> : public bsl::true_type {
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
    struct IsCustomizedType<DummyCustomizedType> : public bsl::true_type {
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

    TestXmlElement(const bsl::string_view&  name,
                   bslma::Allocator         *basicAllocator = 0)
    : d_name(name, basicAllocator)
    , d_attributes(basicAllocator)
    , d_content(Elements(), basicAllocator)
    {
    }

    TestXmlElement(const bsl::string_view&  name,
                   const Attributes&         attributes,
                   bslma::Allocator         *basicAllocator = 0)
    : d_name(name, basicAllocator)
    , d_attributes(attributes, basicAllocator)
    , d_content(Elements(), basicAllocator)
    {
    }

    TestXmlElement(const bsl::string_view&  name,
                   const Content&            content,
                   bslma::Allocator         *basicAllocator = 0)
    : d_name(name, basicAllocator)
    , d_attributes(basicAllocator)
    , d_content(content, basicAllocator)
    {
    }

    TestXmlElement(const bsl::string_view&  name,
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
                                                const bsl::string_view& key,
                                                const bsl::string_view& value)
    {
        bsl::vector<Attribute> result;
        result.push_back(Attribute(bsl::string(key), bsl::string(value)));
        return result;
    }

    static bsl::vector<Attribute> generateAttributes(
                                               const bsl::string_view& key0,
                                               const bsl::string_view& value0,
                                               const bsl::string_view& key1,
                                               const bsl::string_view& value1)
    {
        bsl::vector<Attribute> result;
        result.push_back(Attribute(bsl::string(key0), bsl::string(value0)));
        result.push_back(Attribute(bsl::string(key1), bsl::string(value1)));
        return result;
    }

    static Content generateContent(const bsl::string_view& content)
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

    static TestXmlElement generateElement(const bsl::string_view& name)
    {
        return TestXmlElement(name);
    }

    static TestXmlElement generateElement(
                                       const bsl::string_view& name,
                                       const bsl::string_view& attributeKey,
                                       const bsl::string_view& attributeValue)
    {
        return TestXmlElement(
            name, generateAttributes(attributeKey, attributeValue));
    }

    static TestXmlElement generateElement(
                                      const bsl::string_view& name,
                                      const bsl::string_view& attributeKey0,
                                      const bsl::string_view& attributeValue0,
                                      const bsl::string_view& attributeKey1,
                                      const bsl::string_view& attributeValue1)
    {
        return TestXmlElement(name,
                              generateAttributes(attributeKey0,
                                                 attributeValue0,
                                                 attributeKey1,
                                                 attributeValue1));
    }

    static TestXmlElement generateElement(const bsl::string_view& name,
                                          const bsl::string_view& content)
    {
        return TestXmlElement(name, generateContent(content));
    }

    static TestXmlElement generateElement(const bsl::string_view& name,
                                          const TestXmlElement&    child)
    {
        return TestXmlElement(name, generateContent(child));
    }

    static TestXmlElement generateElement(const bsl::string_view& name,
                                          const TestXmlElement&    child0,
                                          const TestXmlElement&    child1)
    {
        return TestXmlElement(name, generateContent(child0, child1));
    }

    static TestXmlElement generateElement(
                                       const bsl::string_view& name,
                                       const bsl::string_view& attributeKey,
                                       const bsl::string_view& attributeValue,
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
    TestXmlElement operator()(const bsl::string_view& name) const
    {
        return Util::generateElement(name);
    }

    TestXmlElement operator()(const bsl::string_view& name,
                              const bsl::string_view& attributeKey,
                              const bsl::string_view& attributeValue) const
    {
        return Util::generateElement(name, attributeKey, attributeValue);
    }

    TestXmlElement operator()(const bsl::string_view& name,
                              const bsl::string_view& attributeKey0,
                              const bsl::string_view& attributeValue0,
                              const bsl::string_view& attributeKey1,
                              const bsl::string_view& attributeValue1) const
    {
        return Util::generateElement(name,
                                     attributeKey0,
                                     attributeValue0,
                                     attributeKey1,
                                     attributeValue1);
    }

    TestXmlElement operator()(const bsl::string_view& name,
                              const bsl::string_view& content) const
    {
        return Util::generateElement(name, content);
    }

    TestXmlElement operator()(const bsl::string_view& name,
                              const TestXmlElement&    child) const
    {
        return Util::generateElement(name, child);
    }

    TestXmlElement operator()(const bsl::string_view& name,
                              const TestXmlElement&    child0,
                              const TestXmlElement&    child1) const
    {
        return Util::generateElement(name, child0, child1);
    }

    TestXmlElement operator()(const bsl::string_view& name,
                              const bsl::string_view& attributeKey,
                              const bsl::string_view& attributeValue,
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
          s_baltst::TestTaggedValue<FailToManipulateSequenceTag, VALUE_TYPE> *,
          MANIPULATOR&,
          const char                                                         *,
          int)
{
    return -1;
}

template <class VALUE_TYPE, class MANIPULATOR>
int bdlat_typeCategoryManipulateSequence(
    s_baltst::TestTaggedValue<FailToManipulateSequenceTag,
                              s_baltst::TestDynamicType<VALUE_TYPE> > *,
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
                             const bsl::string_view&  loggedMessages,
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

        const bsl::string_view errors(errorStreamBuf.data(),
                                       errorStreamBuf.length());

        const bsl::string_view warnings(warningStreamBuf.data(),
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
    TestCase20Row(int line,
                  const s_baltst::TestPlaceHolder<VALUE_TYPE>&,
                  const TestXmlElement&     xml,
                  bool                      decodingSucceeds,
                  const bsl::string_view&  loggedMessages,
                  bslma::Allocator         *basicAllocator = 0)
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

namespace Utf8Test {

                                // =============
                                // class RandGen
                                // =============

class RandGen {
    // Random number generator using the high-order 32 bits of Donald Knuth's
    // MMIX algorithm.

    bsls::Types::Uint64 d_seed;

  public:
    explicit
    RandGen(int startSeed = 0);
        // Initialize the generator with the specified 'startSeed'.

    unsigned operator()();
        // Return the next random number in the series;
};

// CREATOR
inline
RandGen::RandGen(int startSeed)
: d_seed(startSeed)
{
    (void) (*this)();
    (void) (*this)();
    (void) (*this)();
}

// MANIPULATOR
inline
unsigned RandGen::operator()()
{
    d_seed = d_seed * 6364136223846793005ULL + 1442695040888963407ULL;
    return static_cast<unsigned>(d_seed >> 32);
}

int findLoc(int                *line,
            int                *column,
            const bsl::string&  pattern,
            bsl::size_t         offset)
{
    if (pattern.length() < offset) {
        return -1;                                                    // RETURN
    }

    *line = 1;
    *column = 1;

    for (unsigned pos = 0; pos < offset; ++pos) {
        const char c = pattern[pos];

        if ('\n' == c) {
            ++*line;
            *column = 1;
        }
        else {
            ++*column;
        }
    }

    return 0;
}

bsl::string utf8Dump(const char *utf8)
{
    bsl::string ret;

    for (const char *pc = utf8; *pc; ++pc) {
        unsigned char uc = *pc;

        ret += "\\x";
        for (int shift = 4; 0 <= shift; shift -= 4) {
            const int nybble = (uc >> shift) & 0xf;

            ret += static_cast<char>(nybble < 10 ? '0' + nybble
                                                 : 'a' + nybble - 10);
        }
    }

    return ret;
}

void writeStringToFile(const char         *fileName,
                       const bsl::string&  str)
{
    typedef bdls::FilesystemUtil FUtil;

    while (true) {
        FUtil::remove(fileName);

        bsl::ofstream of(fileName, bsl::ios_base::out | bsl::ios_base::binary);
        of << str << bsl::flush;
        of.close();

#if defined(BSLS_PLATFORM_OS_WINDOWS)
        // The above write to file sometimes fails on Windows, so we have to
        // verify that it succeeded.

        if (! FUtil::exists(fileName)) {
            continue;
        }

        bsls::Types::Uint64 fileSize = FUtil::getFileSize(fileName);
        if (fileSize != str.length()) {
            continue;
        }

        bsl::ifstream ifs(fileName, bsl::ios_base::in | bsl::ios_base::binary);
        bsl::string readStr;
        char readBuf[10 * 1024];
        ifs.read(readBuf, sizeof(readBuf));
        readBuf[ifs.gcount()] = 0;

        if (! ifs.eof() || str != readBuf) {
            continue;
        }
#endif
        return;                                                       // RETURN
    }
}

#if defined(BSLS_PLATFORM_OS_SOLARIS)
enum { e_PLAT_SOLARIS = 1 };
#else
enum { e_PLAT_SOLARIS = 0 };
#endif

#if defined(BSLS_PLATFORM_OS_AIX)
enum { e_PLAT_AIX = 1 };
#else
enum { e_PLAT_AIX = 0 };
#endif

enum Mode { e_STRING, e_FILE, e_ISTREAM, e_STREAMBUF, e_END };

bsl::ostream& operator<<(bsl::ostream& stream, Mode mode)
{
#undef  CASE
#define CASE(value)    case value: { stream << #value; } break

    switch (mode) {
      CASE(e_STRING);
      CASE(e_FILE);
      CASE(e_ISTREAM);
      CASE(e_STREAMBUF);
      CASE(e_END);
      default: {
        stream << "Unknown mode: " << static_cast<int>(mode);
      } break;
    }
#undef  CASE

    return stream;
}

void validAndInvalidUtf8Test(Mode mode, bool exhaustive = false)
    // ------------------------------------------------------------------------
    // TESTING VALID & INVALID UTF-8
    //
    // Run tests on valid and invalid UTF-8 injected into an XML message, where
    // the specified 'mode' determines whether the information passed to the
    // decoder is:
    //: o In string form.
    //:
    //: o In a file.
    //:
    //: o In a 'bsl::istream'.
    //:
    //: o In a 'bsl::streambuf'.
    //
    // Concerns:
    //: 1 The decoder successfully parses XML data having non-ASCII UTF-8
    //:   content when UTF-8 checking is enabled.
    //:
    //: 2 The decoder fails to parse invalid UTF-8 data, reports the line
    //:   number and column number of the location of the start of the invalid
    //:   UTF-8, and reports the type of error when UTF-8 checking is enabled
    //
    // Plan:
    //: 1 Start with 'xmlRaw', a string containing a sequence of syntactically
    //:   correct XML describing a valid 'SimpleRequest'.
    //:   o 'xmlRaw' begins with 'header', another string.  When the decoder is
    //:     opened, it will immediately read all of header without being called
    //:     to decode anything.  So if the test expects errors, in some cases
    //:     those errors will be reported by the 'open' function, before
    //:     'decode' is called.
    //:
    //: 2 The 'mode' argument determines what form the data is read in.  The
    //:   data can come as a string, the contents of a file, via a
    //:   'bsl::istream', or in a 'bsl::streambuf'.  The rest of the test is
    //:   identical, except that we do things in the case where the mode is
    //:   'e_FILE' to do fewer tests and speed up the test, because creating
    //:   the file and reading from it are orders of magnitude slower than the
    //:   other means of providing the data to the decoder.
    //:
    //: 3 Do a loop to test valid non-ascii UTF-8 sequences.
    //:   o Inject random correct UTF-8 sequences into a copy of 'xmlRaw' into
    //:     places where the resulting string will be syntactically correct
    //:     XML.
    //:
    //:   o The only places where random valid UTF-8 can be inserted into
    //:     'xmlRaw' without causing a syntax error is in the payload of the
    //:     'data' field, which starts out as "Woof".  Iterate twice, inserting
    //:     the random valid UTF-8 string before and after "Woof", and observe
    //:     that when we examing[examining] the 'data' field of the decoded
    //:     'SimpleRequest', that the exact UTF-8 we injected is where we
    //:     expect it.
    //:
    //: 4 Do a loop test with invalid non-ascii UTF-8 sequences.
    //:   o Inject random incorrect non-ascii UTF-8 sequences at random
    //:     positions into a copy of 'xmlRaw'.  (Note that we don't inject them
    //:     at location 0 -- this just results in an 'open' failure without a
    //:     UTF-8 information as the minireader thinks it hit EOF on the first
    //:     byte).
    //:
    //:   o Open the decoder in one of 4 ways according to 'mode'.
    //:
    //:   o If the position of the bad UTF-8 was before the end of 'header', we
    //:     expect the open to have failed, otherwise not.  Check this.
    //:
    //:   o If the open failed, attempt to decode a 'SimpleRequest'.
    //:
    //:   o Observe that now we have an error rcode , either from 'open' or
    //:     from 'decode'.  Examine the results:
    //:     1 Call 'TC::findLoc' to determine the expected line # and column #
    //:       where the error occurred.
    //:
    //:     2 Check the line # in 'errorInfo'.
    //:
    //:     3 Check the column # in 'errorInfo'.
    //:
    //:     4 Check that 'errorInfo.msg()' contains the string description of
    //:       the UTF-8 error as given by 'bdlde::Utf8Util::toAscii'.
    //------------------------------------------------------------------------
{
    namespace TC = Utf8Test;

    using bdlde::Utf8Util;

    const Utf8Util::ErrorStatus EIT = Utf8Util::k_END_OF_INPUT_TRUNCATION;
    const Utf8Util::ErrorStatus UCO =
                                     Utf8Util::k_UNEXPECTED_CONTINUATION_OCTET;
    const Utf8Util::ErrorStatus NCO = Utf8Util::k_NON_CONTINUATION_OCTET;
    const Utf8Util::ErrorStatus OLE = Utf8Util::k_OVERLONG_ENCODING;
    const Utf8Util::ErrorStatus IIO = Utf8Util::k_INVALID_INITIAL_OCTET;
    const Utf8Util::ErrorStatus VTL = Utf8Util::k_VALUE_LARGER_THAN_0X10FFFF;
    const Utf8Util::ErrorStatus SUR = Utf8Util::k_SURROGATE;

    static const struct Data {
        int                  d_lineNum;    // source line number

        const char           *d_utf8_p;     // UTF-8 input string

        int                  d_numBytes;   // length of spec (in bytes), not
                                           // including null-terminator

        int                  d_numCodePoints;
                                           // +ve number of UTF-8 code points
                                           // if valid, -ve
                                           // Utf8Util::ErrorStatus is invalid.

        int                  d_errOffset;  // byte offset to first invalid
                                           // sequence; -1 if valid

        int                  d_isValid;    // 1 if valid UTF-8; 0 otherwise
    } DATA[] = {
        //L#  input                          #b   #c  eo  result
        //--  -----                          --  ---  --  ------
        { L_, "",                         0,   0, -1,   1   },

        { L_, "H",                        1,   1, -1,   1   },
        { L_, "He",                       2,   2, -1,   1   },
        { L_, "Hel",                      3,   3, -1,   1   },
        { L_, "Hell",                     4,   4, -1,   1   },
        { L_, "Hello",                    5,   5, -1,   1   },

        // Check the boundary between 1-octet and 2-octet code points.

        { L_, "\x7f",                     1,   1, -1,   1   },
        { L_, "\xc2\x80",                 2,   1, -1,   1   },

        // Check the boundary between 2-octet and 3-octet code points.

        { L_, "\xdf\xbf",                 2,   1, -1,   1   },
        { L_, "\xe0\xa0\x80",             3,   1, -1,   1   },

        // Check the maximal 3-octet code point.

        { L_, "\xef\xbf\xbf",             3,   1, -1,   1   },

        // Make sure 4-octet code points are handled correctly.

        { L_, "\xf0\x90\x80\x80",         4,   1, -1,   1   },
        { L_, "\xf0\x90\x80\x80g",        5,   2, -1,   1   },
        { L_, "a\xf0\x90\x80\x81g",       6,   3, -1,   1   },
        { L_, "\xf4\x8f\xbf\xbe",         4,   1, -1,   1   },
        { L_, "\xf4\x8f\xbf\xbeg",        5,   2, -1,   1   },
        { L_, "a\xf4\x8f\xbf\xbfg",       6,   3, -1,   1   },

        // unexpected continuation octets

        { L_, "\x80",                     1, UCO,  0,   0   },
        { L_, "a\x85",                    2, UCO,  1,   0   },
        { L_, "\x90",                     1, UCO,  0,   0   },
        { L_, "\x9f",                     1, UCO,  0,   0   },
        { L_, "\xa1",                     1, UCO,  0,   0   },
        { L_, "\xaf",                     1, UCO,  0,   0   },
        { L_, "a\xb0",                    2, UCO,  1,   0   },
        { L_, "\xbf",                     1, UCO,  0,   0   },

        // Make sure partial 4-octet code points are handled correctly (with a
        // single error).

        { L_, "\xf0",                     1, EIT,  0,   0   },
        { L_, "\xf0\x80",                 2, EIT,  0,   0   },
        { L_, "\xf0\x80\x80",             3, EIT,  0,   0   },
        { L_, "\xf0g",                    2, NCO,  0,   0   },
        { L_, "\xf0\x80g",                3, NCO,  0,   0   },
        { L_, "\xf0\x80\x80g",            4, NCO,  0,   0   },

        // Make sure partial 4-octet code points are handled correctly (with a
        // single error).

        { L_, "\xe0\x80",                 2, EIT,  0,   0   },
        { L_, "\xe0",                     1, EIT,  0,   0   },
        { L_, "\xe0\x80g",                3, NCO,  0,   0   },
        { L_, "\xe0g",                    2, NCO,  0,   0   },

        // Make sure the "illegal" UTF-8 octets are handled correctly:
        //   o The octet values C0, C1, F5 to FF never appear.

        { L_, "\xc0",                     1, EIT,  0,   0   },
        { L_, "\xc1",                     1, EIT,  0,   0   },
        { L_, "\xf0",                     1, EIT,  0,   0   },
        { L_, "\xf7",                     1, EIT,  0,   0   },
        { L_, "\xf8",                     1, IIO,  0,   0   },
        { L_, "\xf8\xaf\xaf\xaf",         4, IIO,  0,   0   },
        { L_, "\xf8\x80\x80\x80",         4, IIO,  0,   0   },
        { L_, "\xf8",                     1, IIO,  0,   0   },
        { L_, "\xff",                     1, IIO,  0,   0   },

        // Make sure that the "illegal" UTF-8 octets are handled correctly
        // mid-string:
        //   o The octet values C0, C1, F5 to FF never appear.

        { L_, "a\xc0g",                   3, NCO,  1,   0   },
        { L_, "a\xc1g",                   3, NCO,  1,   0   },
        { L_, "a\xf5g",                   3, NCO,  1,   0   },
        { L_, "a\xf7g",                   3, NCO,  1,   0   },
        { L_, "a\xf8g",                   3, IIO,  1,   0   },
        { L_, "a\xfeg",                   3, IIO,  1,   0   },

        { L_, "\xc2\x80",                 2,   1, -1,   1   },
        { L_, "\xc2",                     1, EIT,  0,   0   },
        { L_, "\xc2\x80g",                3,   2, -1,   1   },
        { L_, "\xc3\xbf",                 2,   1, -1,   1   },
        { L_, "\x01z\x7f\xc3\xbf\xdf\xbf\xe0\xa0\x80\xef\xbf\xbf",
                                         13,   7, -1,   1   },

        { L_, "a\xef",                    2, EIT,  1,   0   },
        { L_, "a\xef\xbf",                2, EIT,  1,   0   },

        { L_, "\xef\xbf\xbf\xe0\xa0\x80\xdf\xbf\xc3\xbf\x7fz\x01",
                                         13,   7, -1,   1   },

        // Make sure illegal overlong encodings are not accepted.  These code
        // points are mathematically correctly encoded, but since there are
        // equivalent encodings with fewer octets, the UTF-8 standard disallows
        // them.

        { L_, "\xf0\x80\x80\x80",         4, OLE,  0,   0   },
        { L_, "\xf0\x8f\xbf\xbf",         4, OLE,  0,   0   },    // max OLE
        { L_, "\xf0\x90\x80\x80",         4,   1, -1,   1   },    // min legal
        { L_, "\xf1\x80\x80\x80",         4,   1, -1,   1   },    // norm legal
        { L_, "\xf1\xaa\xaa\xaa",         4,   1, -1,   1   },    // norm legal
        { L_, "\xf4\x8f\xbf\xbf",         4,   1, -1,   1   },    // max legal
        { L_, "\xf4\xa0\x80\x80",         4, VTL,  0,   0   },    //     VTL
        { L_, "\xf7\xbf\xbf\xbf",         4, VTL,  0,   0   },    // max VTL

        { L_, "\xe0\x9f\xbf",             3, OLE,  0,   0   },    // max OLE
        { L_, "\xe0\xa0\x80",             3,   1, -1,   1   },    // min legal

        { L_, "\xc0\x80",                 2, OLE,  0,   0   },
        { L_, "\xc1\xbf",                 2, OLE,  0,   0   },    // max OLE
        { L_, "\xc2\x80",                 2,   1,  0,   1   },    // min legal

        // Corrupted 2-octet code point:

        { L_, "\xc2",                     1, EIT,  0,   0   },
        { L_, "a\xc2",                    2, EIT,  1,   0   },
        { L_, "\xc2\xc2",                 2, NCO,  0,   0   },
        { L_, "\xc2\xef",                 2, NCO,  0,   0   },

        // Corrupted 2-octet code point followed by an invalid code point:

        { L_, "\xc2\xff",                 2, NCO,  0,   0   },
        { L_, "\xc2\xff",                 2, NCO,  0,   0   },

        // 3-octet code points corrupted after octet 1:

        { L_, "\xef",                     1, EIT,  0,   0   },
        { L_, "a\xef",                    2, EIT,  1,   0   },
        { L_, "\xefg",                    2, NCO,  0,   0   },
        { L_, "\xef\xefg",                3, NCO,  0,   0   },
        { L_, "\xef" "\xc2\x80",          3, NCO,  0,   0   },

        // 3-octet code points corrupted after octet 2:

        { L_, "\xef\xbf",                 2, EIT,  0,   0   },
        { L_, "a\xef\xbf@",               4, NCO,  1,   0   },
        { L_, "\xef\xbf\xef",             3, NCO,  0,   0   },

        { L_, "\xed\xa0\x80",             3, SUR,  0,   0   },
        { L_, "\xed\xb0\x85g",            4, SUR,  0,   0   },
        { L_, "\xed\xbf\xbf",             3, SUR,  0,   0   },
    };
    enum { k_NUM_DATA = sizeof DATA / sizeof *DATA };

    enum { k_BUF_LEN = 1024 };
    char fileName[k_BUF_LEN];
    {
        bsl::string osName, osVersion, osPatch;
        const char *hostName = bsl::getenv("HOSTNAME");
        if (!hostName) {
            ASSERT(0 == bdls::OsUtil::getOsInfo(&osName,
                                                &osVersion,
                                                &osPatch));
            osName += '.' + osVersion + '.' + osPatch;
            hostName = osName.c_str();
        }

        bdlsb::FixedMemOutStreamBuf fileSb(fileName, sizeof(fileName));
        bsl::ostream                fileStream(&fileSb);
        fileStream << "tmp.balxml_decoder." << test << '.' << hostName <<
                      '.' << bdls::ProcessUtil::getProcessId() <<
                                                       ".xml" << bsl::ends;

        BSLS_ASSERT(bsl::strlen(fileName) < sizeof(fileName));
    }

    // Try to set 'mod' so that the tests are done in less that about 7
    // seconds.  Higher values of 'mod' result in less of the randomized
    // testing, and hence faster run times.

    const int mod = TC::e_FILE != mode || exhaustive
                  ? 1
                  : e_PLAT_SOLARIS
                  ? 500
                  : e_PLAT_AIX
                  ? 600
                  : 180;

    if (verbose) { P_(exhaustive);    P(mod); }

    // The decoder will not open successfully unless it is able to read the
    // header.

    const bsl::string header =
        "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
        "<SimpleRequest xmlns=\"TestNamespace\"\n"
        "  xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\">";

    const bsl::string xmlRaw = header + "\n" +
        "  <data>Woof</data>\n"
        "  <responseLength>23</responseLength>\n"
        "</SimpleRequest>";

    const bsl::size_t woofPos = xmlRaw.find("Woof");
    BSLS_ASSERT_OPT(bsl::string::npos != woofPos);
    const bsl::size_t woofLen = bsl::strlen("Woof");

    if (verbose) {
        P_(fileName);    P_(header.length());    P(xmlRaw.length());
    }

    if (verbose) cout << "Test the insertion of valid UTf-8 snippets.\n";

    for (int di = 0; di < k_NUM_DATA; ++di) {
        const Data&        data      = DATA[di];
        const int          LINE      = data.d_lineNum;
        const char        *UTF8      = data.d_utf8_p;
        const bsl::size_t  NUM_BYTES = data.d_numBytes;
        const bool         IS_VALID  = data.d_isValid;

        if (!IS_VALID) {
            continue;
        }

        if (veryVerbose) { P_(LINE);    P(TC::utf8Dump(UTF8)); }

        for (int ai = 0; ai < 2; ++ai) {
            const bool AFTER = ai;

            bsl::string xmlStr = xmlRaw;

            bsl::string expData = "Woof";
            if (AFTER) {
                xmlStr.insert(woofPos + woofLen, UTF8, NUM_BYTES);
                expData.append(UTF8, NUM_BYTES);
            }
            else {
                xmlStr.insert(woofPos, UTF8, NUM_BYTES);
                expData.insert(bsl::size_t(0), UTF8, NUM_BYTES);
            }

            if (veryVeryVerbose) {
                T2_ T2_ P_(LINE); P(AFTER);
            }

            balxml::MiniReader reader;
            balxml::DecoderOptions options;
            options.setValidateInputIsUtf8(true);
            balxml::ErrorInfo e;
            balxml::Decoder decoder(&options, &reader, &e);

            bdlsb::FixedMemInStreamBuf sb(xmlStr.c_str(), xmlStr.length());
            bsl::istream is(&sb);

            switch (mode) {
              case TC::e_STRING: {
                ASSERT(0 == decoder.open(xmlStr.c_str(), xmlStr.length()));
              } break;
              case TC::e_FILE: {
                TC::writeStringToFile(fileName, xmlStr);

                ASSERT(0 == decoder.open(fileName));
              } break;
              case TC::e_ISTREAM: {
                ASSERT(0 == decoder.open(is));
              } break;
              case TC::e_STREAMBUF: {
                ASSERT(0 == decoder.open(&sb));
              } break;
              case TC::e_END: BSLA_FALLTHROUGH;
              default: {
                BSLS_ASSERT_INVOKE_NORETURN("invalid mode in 'open'");
              }
            }

            Test::SimpleRequest object;
            int rc = decoder.decode(&object);
            ASSERT(0 == rc);
            ASSERT(0 == reader.errorInfo());
            ASSERT(expData == object.data());
        }
    }

    if (verbose) cout << "Test the insertion of invalid UTf-8 snippets.\n";

    int colPass = 0, colFail = 0;
    TC::RandGen rand(100 * test + mode);

    for (int di = 0; di < k_NUM_DATA; ++di) {
        const Data&        data       = DATA[di];
        const int          LINE       = data.d_lineNum;
        const char        *UTF8       = data.d_utf8_p;
        const bsl::size_t  NUM_BYTES  = data.d_numBytes;
        const unsigned     ERR_OFFSET = data.d_errOffset;
        const int          ERR_TYPE   = data.d_numCodePoints;
        const bool         IS_VALID   = data.d_isValid;

        if (IS_VALID) {
            continue;
        }

        ASSERT(ERR_TYPE < 0);

        if (veryVerbose) {
            const bsl::string&  utf8Dump = TC::utf8Dump(UTF8);
            const char         *errMsg   = Utf8Util::toAscii(ERR_TYPE);

            P_(LINE);    P_(utf8Dump);    P(errMsg);
        }

        // A UTF-8 error at position 0 just results in a message about "Unable
        // to open reader" without any detail about UTF-8.

        for (bsl::size_t badPos = 1, endPos = xmlRaw.length() - 1; true;
                                              badPos += 1 + rand() % mod) {
            badPos = bsl::min(badPos, endPos);

            if (ERR_OFFSET && badPos < header.length()) {
                // If the pattern involves some random correct UTF-8 before the
                // error and it is injected in the header, it will result in a
                // non-UTF-8 syntax error and a wrong column reading.

                continue;
            }

            bsl::string xmlStr;
            xmlStr.reserve(xmlRaw.length() + 32);    // reduce # of allocs
            xmlStr = xmlRaw;
            xmlStr.insert(badPos, UTF8, NUM_BYTES);
            if (EIT == ERR_TYPE) {
                xmlStr.resize(badPos + NUM_BYTES);
            }

            if (veryVerbose) {
                if (!exhaustive || 0 == badPos % 20) {
                        T2_    P(badPos);
                }
            }

            // Modes 'e_FILE' and 'e_ISTREAM' read from a file and need
            // a file to be created for them.  The modes are adjacent
            // in the 'enum', so save time by creating the file once
            // and using it in both modes.

            if (veryVeryVerbose) { T2_   T2_   P_(mode);   P(badPos); }

            balxml::MiniReader reader;
            balxml::DecoderOptions options;
            options.setValidateInputIsUtf8(true);
            balxml::ErrorInfo e;
            balxml::Decoder decoder(&options, &reader, &e);

            bdlsb::FixedMemInStreamBuf sb(xmlStr.c_str(), xmlStr.length());
            bsl::istream is(&sb);

            int rc = 0;
            switch (mode) {
              case TC::e_STRING: {
                rc = decoder.open(xmlStr.c_str(), xmlStr.length());
              } break;
              case TC::e_FILE: {
                TC::writeStringToFile(fileName, xmlStr);

                rc = decoder.open(fileName);
              } break;
              case TC::e_ISTREAM: {
                rc = decoder.open(is);
              } break;
              case TC::e_STREAMBUF: {
                sb.pubsetbuf(&xmlStr[0], xmlStr.length());

                rc = decoder.open(&sb);
              } break;
              case TC::e_END: BSLA_FALLTHROUGH;
              default: {
                BSLS_ASSERT_INVOKE_NORETURN(
                                          "invalid mode in invalid UTF-8");
              }
            }

            ASSERTV(badPos, header.length(), rc,
                                  (badPos < header.length()) == (0 != rc));

            if (0 == rc) {
                Test::SimpleRequest object;
                rc = decoder.decode(&object);
            }
            ASSERTV(badPos, xmlStr.substr(0, badPos), 0 != rc);

            int expLine = -1;  // Initialized to avoid eager compiler warnings
            int expCol  = -1;
            ASSERT(0 == TC::findLoc(&expLine,
                                    &expCol,
                                    xmlStr,
                                    badPos + ERR_OFFSET));

            const balxml::ErrorInfo& errorInfo = *decoder.errorInfo();

            ASSERTV(errorInfo.lineNumber(), expLine,
                                        errorInfo.lineNumber() == expLine);
            if (errorInfo.columnNumber() == expCol) {
                ++colPass;
            }
            else {
                ++colFail;
                ASSERTV(LINE, errorInfo.columnNumber(), expCol, badPos,
                     colPass, colFail, errorInfo.columnNumber() == expCol);
                ASSERTV(LINE, badPos, xmlStr,
                                       errorInfo.columnNumber() == expCol);
            }

            const char *errTypeMsg = Utf8Util::toAscii(ERR_TYPE);
            ASSERTV(LINE, badPos, errorInfo.message(), errTypeMsg,
                    bsl::string::npos != errorInfo.message().find(errTypeMsg));

            if (endPos <= badPos) {
                break;
            }
        }
    }

    bdls::FilesystemUtil::remove(fileName);
}

}  // close namespace Utf8Test

// ============================================================================
//                             END TEST APPARATUS
// ----------------------------------------------------------------------------

// ============================================================================
//                              BEGIN TEST CASES
// ----------------------------------------------------------------------------
// Some test cases have been moved into separate functions to stop AIX xlC from
// trying to optimize them and run out of memory.


#if defined(BSLS_PLATFORM_CMP_GNU) &&                                         \
    defined(BDE_BUILD_TARGET_OPT) && defined(BDE_BUILD_TARGET_DBG)
// g++ gives an unhelpful warning (note) for this function about not being able
// to track all variables for the debug info, probably due to loop unrolling
__attribute__((optimize("no-var-tracking-assignments")))
#endif
void runTestCase19()
{
    //-------------------------------------------------------------------------
    // TEST CASE DOCUMENTATION IS REPEATED HERE SO IT IS WITH THE CODE.  It is
    // indented wrong so it does not have to be reformatted here if it needs a
    // change.  Make sure that anything you change here is also changed in
    // 'main' and vice versa.
    //---+
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

    // Abbreviations for the names of 'bdlat' concept test implementations,
    // which will become the tag names of the XML they generate.
    const bsl::string_view S = "MySequence";
    const bsl::string_view C = "MyChoice";
    const bsl::string_view CT = "MyCustomizedType";
    const bsl::string_view D = "MyDynamicType";
    const bsl::string_view E = "MyEnumeration";

    // Abbreviations for attribute and selection names.
    const bsl::string_view A0 = "attribute0";
    const bsl::string_view A1 = "attribute1";
    const bsl::string_view E0 = "enumerator0";
    const bsl::string_view E1 = "enumerator1";
    const bsl::string_view S0 = "selection0";
    const bsl::string_view S1 = "selection1";

    // Abbreviations for some test values.
    const int i0 = 0;
    const int i1 = 1;
    const double d0 = 1.5;

    // Abbreviations for XML-encoded representations of some test values.
    const bsl::string_view V0 = "0";
    const bsl::string_view V1 = "1";
    const bsl::string_view D0 = "1.5";

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
    const bsl::string_view Nil = "xsi:nil";
    const bsl::string_view T   = "true";

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
        f = false, // 0, (en/de)coding fails
        t = true,  // 1, (en/de)coding succeeds
        _ = 2      // 2, (en/de)coding succeeds, but gives different value
    };

    // An abbreviation for an XML structure that will not be used when testing
    // a particular row of the below test table.  The name is short for "Not
    // Applicable."
    const TestXmlElement NA("NA");

    // An abbreviation for the name of the type used to represent one row in
    // this table-based test.
    typedef TestCase19Row R;

    // A macro that is conditionally defined if compiling on platforms where
    // compilation is known to run into resource limitations (e.g. running out
    // of memory on IBM.)
#if defined(BSLS_PLATFORM_CMP_IBM)
// Code restructuring made it possible to build this test driver on AIX/IBM xlC
// again.  Should out of memory errors start happening again while compiling
// this file on IBM/AIX using xlC please uncomment the following line, and the
// '#undef' at the end of this function.
#define U_SKIP_DUE_TO_COMPILER_RESOURCE_LIMITATIONS
#endif

    ///Implementation Note
    ///-------------------
    // The following test table shares its structure with the table in case 14
    // of the 'balxml_encoder' component test driver.  These two test cases
    // share an identical test table structure in order to verify that,
    // abstractly, the encoding and decoding operations they perform are
    // "consistent".  Note that the "encoding result" is unused in this test
    // driver.
    //
    // Test case rows labeled with an asterisk "*" verify that different
    // encodings of null values that may be produced by the encoder are treated
    // as representing the same value (null) by the decoder.  In particular,
    // lines with a "1" after the asterisk verify the nullness of decoded
    // values of omitted tags, lines with a "2" verify the nullness of decoded
    // values of self-closing tags, and lines with a "3" verify the nullness of
    // decoded values of self-closing tags with  an 'xsi:nil="true"' attribute.

    static const TestCase19Row DATA[] = {
//v----------------^                ENCODING RESULT
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
#ifndef U_SKIP_DUE_TO_COMPILER_RESOURCE_LIMITATIONS
R(L_,  c(ns0, n( c(s0, i0 )))   , t, t, x(C,x(S0,x(S0,V0))   )       ),
#endif
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
        const TestCase19Row& ROW = DATA[i];

        ROW.runTest();
    }
}

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
// The following function decodes an XML string into a 'Test::Employee' object
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

        Test::Employee bob;

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

        Test::Employee bob;

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
            Test::Address addr;
            rc = decoder.decode(&addr);
            bsl::cout << addr;
        }
        else {
            Test::Employee bob;
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
      case 22: {
        // --------------------------------------------------------------------
        // REPRODUCE SCENARIO FROM DRQS 169438741
        //
        // Concerns:
        //: 1 Encoded 'bdlt::Date' and 'bdlt::DateTz' values can be decoded to
        //:   'bdlb::Variant2<bdlt::Date, bdlt::DateTz>' object.
        //:
        //: 2 Encoded 'bdlt::Time' and 'bdlt::TimeTz' values can be decoded to
        //:   'bdlb::Variant2<bdlt::Time, bdlt::TimeTz>' object.
        //:
        //: 3 Encoded 'bdlt::Datetime' and 'bdlt::DatetimeTz' values can be
        //:   decoded to 'bdlb::Variant2<bdlt::Datetime, bdlt::DatetimeTz>'
        //:   object.
        //
        // Plan:
        //: 1 For a number of different XML date-and-time representations,
        //:   enumerate XML arrays of such elements and verify that the XML
        //:   array decodes into the corresponding 'bsl::vector' value.
        //:   (C-1..3)
        //
        // Testing:
        //   DRQS 169438741
        // --------------------------------------------------------------------

        if (verbose) cout << "\nREPRODUCE SCENARIO FROM DRQS 169438741"
                          << "\n======================================"
                          << endl;
        {
            typedef bdlb::Variant2<bdlt::Date, bdlt::DateTz> DateOrDateTz;
            typedef bdlb::Variant2<bdlt::Time, bdlt::TimeTz> TimeOrTimeTz;
            typedef bdlb::Variant2<bdlt::Datetime, bdlt::DatetimeTz>
                                                          DatetimeOrDatetimeTz;

            balxml::MiniReader     reader;
            balxml::ErrorInfo      errInfo;
            balxml::DecoderOptions options;

            balxml::Decoder decoder(&options,
                                    &reader,
                                    &errInfo,
                                    &bsl::cerr,
                                    &bsl::cerr);

            // Testing 'bdlt::Date' and 'bdltDateTz'.
            {
                const bsl::string_view INPUT =
                                "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                                "<Dates " XSI ">\n"
                                "    0002-02-02\n"
                                "    9999-12-31\n"
                                "    0002-02-02Z\n"
                                "    9999-12-31-01:30\n"
                                "</Dates>";

                bdlsb::FixedMemInStreamBuf isb(INPUT.data(), INPUT.size());

                if (veryVerbose) {
                    T_ P(INPUT)
                }

                const bdlt::Date   EXP_DATE0(2,     2,  2);
                const bdlt::Date   EXP_DATE1(9999, 12, 31);
                const bdlt::DateTz EXP_DATE2(EXP_DATE0,   0);
                const bdlt::DateTz EXP_DATE3(EXP_DATE1, -90);

                bsl::vector<DateOrDateTz> dateVector(4);

                int            rc     = decoder.decode(&isb, &dateVector);
                const unsigned offset = reader.getCurrentPosition();

                ASSERTV(rc, 0 == rc);
                ASSERTV(offset, INPUT.size(), offset == INPUT.size());

                ASSERTV(dateVector[0].is<bdlt::Date>());
                ASSERTV(EXP_DATE0,   dateVector[0].the<bdlt::Date>(),
                        EXP_DATE0 == dateVector[0].the<bdlt::Date>());

                ASSERTV(dateVector[1].is<bdlt::Date>());
                ASSERTV(EXP_DATE1,   dateVector[1].the<bdlt::Date>(),
                        EXP_DATE1 == dateVector[1].the<bdlt::Date>());

                ASSERTV(dateVector[2].is<bdlt::DateTz>());
                ASSERTV(EXP_DATE2,   dateVector[2].the<bdlt::DateTz>(),
                        EXP_DATE2 == dateVector[2].the<bdlt::DateTz>());

                ASSERTV(dateVector[3].is<bdlt::DateTz>());
                ASSERTV(EXP_DATE3,   dateVector[3].the<bdlt::DateTz>(),
                        EXP_DATE3 == dateVector[3].the<bdlt::DateTz>());
            }

            // Testing 'bdlt::Time' and 'bdltTimeTz'.
            {
                const bsl::string_view INPUT =
                                "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                                "<Times " XSI ">\n"
                                "    01:01:01\n"
                                "    23:59:59.999999\n"
                                "    01:01:01Z\n"
                                "    23:59:59.999999-01:30\n"
                                "</Times>";

                bdlsb::FixedMemInStreamBuf isb(INPUT.data(), INPUT.size());

                if (veryVerbose) {
                    T_ P(INPUT)
                }

                const bdlt::Time   EXP_TIME0(1,   1,  1);
                const bdlt::Time   EXP_TIME1(23, 59, 59, 999, 999);
                const bdlt::TimeTz EXP_TIME2(EXP_TIME0,   0);
                const bdlt::TimeTz EXP_TIME3(EXP_TIME1, -90);

                bsl::vector<TimeOrTimeTz> timeVector(4);

                int            rc     = decoder.decode(&isb, &timeVector);
                const unsigned offset = reader.getCurrentPosition();

                ASSERTV(rc, 0 == rc);
                ASSERTV(offset, INPUT.size(), offset == INPUT.size());

                ASSERTV(timeVector[0].is<bdlt::Time>());
                ASSERTV(EXP_TIME0,   timeVector[0].the<bdlt::Time>(),
                        EXP_TIME0 == timeVector[0].the<bdlt::Time>());

                ASSERTV(timeVector[1].is<bdlt::Time>());
                ASSERTV(EXP_TIME1,   timeVector[1].the<bdlt::Time>(),
                        EXP_TIME1 == timeVector[1].the<bdlt::Time>());

                ASSERTV(timeVector[2].is<bdlt::TimeTz>());
                ASSERTV(EXP_TIME2,   timeVector[2].the<bdlt::TimeTz>(),
                        EXP_TIME2 == timeVector[2].the<bdlt::TimeTz>());

                ASSERTV(timeVector[3].is<bdlt::TimeTz>());
                ASSERTV(EXP_TIME3,   timeVector[3].the<bdlt::TimeTz>(),
                        EXP_TIME3 == timeVector[3].the<bdlt::TimeTz>());
            }

            // Testing 'bdlt::Datetime' and 'bdltDatetimeTz'.
            {
                const bsl::string_view INPUT =
                                "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                                "<Times " XSI ">\n"
                                "    0001-01-01T00:00:00.000000\n"
                                "    9998-12-31T23:59:60.9999999\n"
                                "    0001-01-01T00:00:00.0000001Z\n"
                                "    9998-12-31T23:59:60.9999999+00:30\n"
                                "</Times>";

                bdlsb::FixedMemInStreamBuf isb(INPUT.data(), INPUT.size());

                if (veryVerbose) {
                    T_ P(INPUT)
                }

               const bdlt::Datetime   EXP_DATETIME0(   1, 1, 1, 0, 0, 0, 0, 0);
               const bdlt::Datetime   EXP_DATETIME1(9999, 1, 1, 0, 0, 1, 0, 0);
               const bdlt::DatetimeTz EXP_DATETIME2(EXP_DATETIME0,   0);
               const bdlt::DatetimeTz EXP_DATETIME3(EXP_DATETIME1,  30);

                bsl::vector<DatetimeOrDatetimeTz> dtVector(4);

                int            rc     = decoder.decode(&isb, &dtVector);
                const unsigned offset = reader.getCurrentPosition();

                ASSERTV(rc, 0 == rc);
                ASSERTV(offset, INPUT.size(), offset == INPUT.size());

                ASSERTV(dtVector[0].is<bdlt::Datetime>());
                ASSERTV(EXP_DATETIME0,   dtVector[0].the<bdlt::Datetime>(),
                        EXP_DATETIME0 == dtVector[0].the<bdlt::Datetime>());

                ASSERTV(dtVector[1].is<bdlt::Datetime>());
                ASSERTV(EXP_DATETIME1,   dtVector[1].the<bdlt::Datetime>(),
                        EXP_DATETIME1 == dtVector[1].the<bdlt::Datetime>());

                ASSERTV(dtVector[2].is<bdlt::DatetimeTz>());
                ASSERTV(EXP_DATETIME2,   dtVector[2].the<bdlt::DatetimeTz>(),
                        EXP_DATETIME2 == dtVector[2].the<bdlt::DatetimeTz>());

                ASSERTV(dtVector[3].is<bdlt::DatetimeTz>());
                ASSERTV(EXP_DATETIME3,   dtVector[3].the<bdlt::DatetimeTz>(),
                        EXP_DATETIME3 == dtVector[3].the<bdlt::DatetimeTz>());
            }
        }
      } break;
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
        //:   returns zero for all 'bdlat' operations succeeds.
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

        const bsl::string_view D = "MyDynamicType";
        const bsl::string_view S = "MySequence";
        const bsl::string_view T = "MyTaggedValue";
            // Abbreviations for the names of 'bdlat' concept test
            // implementations, which will become the tag names of the XML they
            // generate.

        const bsl::string_view A0 = "attribute0";
            // Abbreviation for an attribute name.

        const bsl::string_view I1 = "1";
            // Abbreviation for a test value.

        typedef s_baltst::TestAttribute<0, attribute0Name> Attribute0;
        const Attribute0                         a0;
            // Abbreviation for a sequence attribute.

        const s_baltst::TestPlaceHolder<int>                   i_;
        const s_baltst::GenerateTestDynamicPlaceHolder     d_;
        const s_baltst::GenerateTestSequencePlaceHolder    s_;
        const s_baltst::GenerateTestTaggedValuePlaceHolder t_;
            // Abbreviations for function objects used to generate
            // placeholders.

        const s_baltst::TestPlaceHolder<DefaultTag>                  td_;
        const s_baltst::TestPlaceHolder<FailToManipulateSequenceTag> tf_;
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

        typedef s_baltst::TypedTestAttribute<int, Attribute0> IntAttribute0;
            // 'IntAttribute0' is an alias for a type that specifies an
            // attribute of a sequence having the type 'int', Id '0', and name
            // 'attribute0'.

        typedef s_baltst::TestSequence<IntAttribute0> ObjType;
            // 'ObjType' is an alias for a type that specifies a sequence
            // having one attribute, where that attribute is specified by
            // 'IntAttribute0'.  This type is prefixed with 'Obj' because it is
            // the "object" of the test, or more specifically, as an invariant
            // in the structure of the output parameter of the decoding
            // operation.

        const s_baltst::TestPlaceHolder<ObjType> obj;
            // 'obj' is a placeholder for an object of 'ObjType', used to
            // specify the type of the object to be used as the output
            // parameter of a decoding operation.

        const TestXmlElement       OBJ = x(A0, I1);
            // 'OBJ' is an abbreviation for an XML structure that always
            // successfully decodes into an object of type 'ObjType'.  This
            // object is labeled "OBJ" because it is the input-side dual to
            // "Obj".  It also characterizes the "object" of the test, meaning
            // that it is an invariant part of the structure of the input to
            // the decoding operation performed in this test.

        const bsl::string_view SuccessMsg = "";
            // 'SuccessMsg' is an abbreviation for the value of the
            // 'loggedMessages' attribute of a 'balxml::Decoder' after a
            // successful decoding operation.

        const bsl::string_view TagFailMsg =
                "STREAM.xml:1.28: Error: Unable to decode sub-element "
                "'attribute0'.\n";
            // 'TagFailMsg' is an abbreviation for the value of the
            // 'loggedMessages' attribute of a 'balxml::Decoder' after failing
            // to decode an 'attribute0' element.

        const bsl::string_view DynTagFailMsg =
                "STREAM.xml:1.16: Error: The object being decoded is a "
                "'DynamicType', and attempting to manipulate the object by "
                "its dynamic category returned a non-zero status.\n";
            // 'DynTagFailMsg' is an abbreviation for the value of the
            // 'loggedMessages' attribute of a 'balxml::Decoder' after failing
            // to decode an object with the 'Dynamic' 'bdlat' type category
            // due to a failure in the 'bdlat_typeCategoryManipulateSequence'
            // function.

        static const TestCase20Row DATA[] = {
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
        //---------------------------------------------------------------------
        // TEST CASE DOCUMENTATION IS REPEATED IN THE 'runTestCase19()'
        // function so it is also near the actual test code.  Make sure that
        // anything you change here, you also changed in 'runTestCase19()' and
        // vice versa.
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

        runTestCase19();

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

        const int NUM_DATA = sizeof DATA / sizeof DATA[0];

        for (int i = 0; i != NUM_DATA; ++i) {
            const int           LINE   = DATA[i].d_line;
            const TestXmlElement& XML    = DATA[i].d_xml;
            const char *const   EXPECTED_STRING = DATA[i].d_string;

            bdlsb::MemOutStreamBuf xmlStreamBuf;
            bsl::ostream xmlStream(&xmlStreamBuf);
            xmlStream << XML;                                           // TEST

            const bsl::string_view STRING(xmlStreamBuf.data(),
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

        static const bsl::string_view DATA[] = {
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

        for (int ti = 0; ti < 2 * NUM_DATA; ++ti) {
            const int  i          = ti % NUM_DATA;
            const bool CHECK_UTF8 = NUM_DATA <= ti;

            const bsl::string_view& STR = DATA[i];

            balxml::DecoderOptions options;
            options.setValidateInputIsUtf8(CHECK_UTF8);
            balxml::ErrorInfo e;
            balxml::Decoder decoder(&options, &reader, &e);

            bdlsb::FixedMemInStreamBuf isb(STR.data(), STR.size());

            Test::Topchoice object;
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
            for (int checkUtf8 = 0; checkUtf8 < 2; ++checkUtf8) {
                balxml::MiniReader reader;
                balxml::ErrorInfo  errInfo;
                balxml::DecoderOptions    options;
                options.setValidateInputIsUtf8(checkUtf8);

                balxml::Decoder mX(&options,
                                   &reader,
                                   &errInfo,
                                   &bsl::cerr,
                                   &bsl::cerr);
                const balxml::Decoder& X = mX;
                ASSERT(0 == X.numUnknownElementsSkipped());

                static const int DATA[] = { 0, 1, 5, 100, 2000 };
                const int NUM_DATA = sizeof DATA / sizeof *DATA;
                for (int i = 0; i < NUM_DATA; ++i) {
                    const int NUM_SKIPPED_ELEMS = DATA[i];
                    mX.setNumUnknownElementsSkipped(NUM_SKIPPED_ELEMS);
                    LOOP3_ASSERT(i, NUM_SKIPPED_ELEMS,
                               X.numUnknownElementsSkipped(),
                               NUM_SKIPPED_ELEMS ==
                                                X.numUnknownElementsSkipped());
                }
            }
        }

        // TestSequence2
        {
            const bsl::string_view INPUT =
                "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                "<TestSequence2 " XSI ">\n"
                "    <E3>abc</E3>\n"
                "    <E1>123</E1>\n"
                "</TestSequence2>\n";

            bdlsb::FixedMemInStreamBuf isb(INPUT.data(), INPUT.size());
            bsl::istream               is(&isb);

            if (veryVerbose) {
                T_ P(INPUT)
            }

            TestSequence2 ts;

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

            mX.decode(is, &ts);

            LOOP_ASSERT(is.fail(), !is.fail());
            LOOP_ASSERT(ts,
                    ts == TestSequence2(123, TestSequence2::DEFAULT_ELEMENT2));
            LOOP_ASSERT(X.numUnknownElementsSkipped(),
                        1 == X.numUnknownElementsSkipped());
        }

        // TestChoice2
        {
            const bsl::string_view INPUT =
                "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                "<TestChoice2 " XSI ">\n"
                "    <S3>123</S3>\n"
                "</TestChoice2>\n";

            bdlsb::FixedMemInStreamBuf isb(INPUT.data(), INPUT.size());
            bsl::istream               is(&isb);

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

            mX.decode(is, &tc);

            LOOP_ASSERT(is.fail(), !is.fail());
            LOOP_ASSERT(tc, TestChoice2() == tc);
            LOOP_ASSERT(X.numUnknownElementsSkipped(),
                        1 == X.numUnknownElementsSkipped());
        }

        {
            // This test, from baea_serializableobjectproxyutil.t.cpp, used to
            // fail a safe assert there.

            const bsl::string_view INPUT =
                "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                "<selection8>\n"
                "   <foo><selection1/></foo>\n"
                "   <foo><selection1/></foo>\n"
                "</selection8>\n";

            bdlsb::FixedMemInStreamBuf isb(INPUT.data(), INPUT.size());
            bsl::istream               is(&isb);

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

            mX.decode(is, &tc);

            LOOP_ASSERT(is.fail(), !is.fail());
            LOOP_ASSERT(X.numUnknownElementsSkipped(),
                        2 == X.numUnknownElementsSkipped());
        }

        // MySequence
        {
            typedef Test::MySequence Type;

            static const struct {
                int              d_line;
                bsl::string_view d_xml;
                int              d_numSkipped;

                // Type Data members
                int              d_attribute1;
                bsl::string_view d_attribute2;
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
                const bsl::string_view& XML         = DATA[i].d_xml;
                const int               NUM_SKIPPED = DATA[i].d_numSkipped;

                Type exp; const Type& EXP = exp;
                exp.attribute1() = DATA[i].d_attribute1;
                exp.attribute2() = DATA[i].d_attribute2;

                if (veryVerbose) {
                    T_ P_(i) P(XML) P(EXP)
                }

                bdlsb::FixedMemInStreamBuf isb(XML.data(), XML.size());
                bsl::istream               input(&isb);

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
            typedef Test::MySequenceWithAnonymousChoice Type;

            static const struct {
                int              d_line;
                bsl::string_view d_xml;
                int              d_numSkipped;

                // Type Data members
                bool             d_attr1Specified;
                int              d_attr1;

                int              d_choiceSelectionId;
                int              d_choiceAttr1;
                bsl::string_view d_choiceAttr2;

                bool             d_attr2Specified;
                bsl::string_view d_attr2;
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
                const bsl::string_view& XML   = DATA[i].d_xml;
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

                bdlsb::FixedMemInStreamBuf isb(XML.data(), XML.size());
                bsl::istream               input(&isb);

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

        static const struct Data {
            int              d_lineNum;  // source line number
            bsl::string_view d_input;    // input string
            int              d_retCode;  // expected ret code
            TS               d_result;   // expected result
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
        for (int ti = 0; ti < 2 * NUM_DATA; ++ti) {
            const Data&             data              = DATA[ti % 2];
            const int               LINE              = data.d_lineNum;
            const bsl::string_view  INPUT             = data.d_input;
            const int               EXPECTED_RET_CODE = data.d_retCode;
            const TS                EXPECTED_RESULT   = data.d_result;
            const bool              CHECK_UTF8        = NUM_DATA <= ti;

            if (veryVerbose) {
                T_ P(LINE)

                if (veryVeryVerbose) {
                    T_ T_ P(INPUT)
                    T_ T_ P_(EXPECTED_RET_CODE) P(EXPECTED_RESULT)
                }
            }
            bdlsb::FixedMemInStreamBuf isb(INPUT.data(), INPUT.size());
            bsl::istream               input(&isb);
            TS                         result = INIT_VALUE;
            balxml::DecoderOptions     options;
            options.setSkipUnknownElements(false);
            options.setValidateInputIsUtf8(CHECK_UTF8);

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
        for (int ti = 0; ti < 2 * NUM_DATA; ++ti) {
            const Data&             data              = DATA[ti % 2];
            const int               LINE              = data.d_lineNum;
            const bsl::string_view  INPUT             = data.d_input;
            const int               EXPECTED_RET_CODE = data.d_retCode;
            const TS                EXPECTED_RESULT   = data.d_result;
            const bool              CHECK_UTF8        = NUM_DATA <= ti;

            if (veryVerbose) {
                T_ P(LINE)

                if (veryVeryVerbose) {
                    T_ T_ P(INPUT)
                    T_ T_ P_(EXPECTED_RET_CODE) P(EXPECTED_RESULT)
                }
            }

            bdlsb::FixedMemInStreamBuf isb(INPUT.data(), INPUT.size());
            bsl::istream               input(&isb);
            TS                         result = INIT_VALUE;
            bsl::ostream               nullStream(0);
            balxml::DecoderOptions     options;
            options.setSkipUnknownElements(false);
            options.setValidateInputIsUtf8(CHECK_UTF8);

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
        for (int ti = 0; ti < 2 * NUM_DATA; ++ti) {
            const Data&             data              = DATA[ti % 2];
            const int               LINE              = data.d_lineNum;
            const bsl::string_view  INPUT             = data.d_input;
            const int               EXPECTED_RET_CODE = data.d_retCode;
            const TS                EXPECTED_RESULT   = data.d_result;
            const bool              CHECK_UTF8        = NUM_DATA <= ti;

            if (veryVerbose) {
                T_ P(LINE)

                if (veryVeryVerbose) {
                    T_ T_ P(INPUT)
                    T_ T_ P_(EXPECTED_RET_CODE) P(EXPECTED_RESULT)
                }
            }

            bdlsb::FixedMemInStreamBuf isb(INPUT.data(), INPUT.size());
            bsl::istream               input(&isb);
            TS                         result = INIT_VALUE;
            balxml::DecoderOptions     options;
            options.setSkipUnknownElements(false);
            options.setValidateInputIsUtf8(CHECK_UTF8);

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
        for (int ti = 0; ti < 2 * NUM_DATA; ++ti) {
            const Data&             data              = DATA[ti % 2];
            const int               LINE              = data.d_lineNum;
            const bsl::string_view  INPUT             = data.d_input;
            const int               EXPECTED_RET_CODE = data.d_retCode;
            const TS                EXPECTED_RESULT   = data.d_result;
            const bool              CHECK_UTF8        = NUM_DATA <= ti;

            if (veryVerbose) {
                T_ P(LINE)

                if (veryVeryVerbose) {
                    T_ T_ P(INPUT)
                    T_ T_ P_(EXPECTED_RET_CODE) P(EXPECTED_RESULT)
                }
            }

            bdlsb::FixedMemInStreamBuf isb(INPUT.data(), INPUT.size());
            bsl::istream               input(&isb);
            TS                         result = INIT_VALUE;
            bsl::ostream               nullStream(0);
            balxml::DecoderOptions     options;
            options.setSkipUnknownElements(false);
            options.setValidateInputIsUtf8(CHECK_UTF8);

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
            typedef Test::MySimpleContent Type;

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

            const bsl::string_view INPUTS[] = {
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
            const int NUM_INPUT = sizeof INPUTS / sizeof *INPUTS;

            for (int ti = 0; ti < 2 * NUM_INPUT; ++ti) {
                const int  i                 = ti % NUM_INPUT;
                const bsl::string_view INPUT = INPUTS[i];
                const bool CHECK_UTF8        = NUM_INPUT <= ti;

                Type mX;  const Type& X = mX;

                const Type& Y = EXPECTED_RESULT[i];

                if (veryVerbose) {
                    T_ P_(i) P_(Y) P(INPUT)
                }

                LOOP3_ASSERT(i, X, Y, X != Y);

                bdlsb::FixedMemInStreamBuf isb(INPUT.data(), INPUT.size());
                bsl::istream               input(&isb);

                balxml::MiniReader     reader;
                balxml::ErrorInfo      errInfo;
                balxml::DecoderOptions options;
                options.setValidateInputIsUtf8(CHECK_UTF8);

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
            typedef Test::MySimpleIntContent Type;

            Type EXPECTED_RESULT[2];

            EXPECTED_RESULT[0].attribute1() = true;
            EXPECTED_RESULT[0].attribute2() = "Hello World!";
            EXPECTED_RESULT[0].theContent() = 34;

            EXPECTED_RESULT[1].attribute1() = false;
            EXPECTED_RESULT[1].attribute2() = "Hello World!";
            EXPECTED_RESULT[1].theContent() = 34;

            const bsl::string_view INPUTS[] = {
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
            const int NUM_INPUT = sizeof INPUTS / sizeof *INPUTS;

            for (int ti = 0; ti < 2 * NUM_INPUT; ++ti) {
                const int  i                 = ti % NUM_INPUT;
                const bsl::string_view INPUT = INPUTS[i];
                const bool CHECK_UTF8        = NUM_INPUT <= ti;

                Type mX;  const Type& X = mX;

                const Type& Y = EXPECTED_RESULT[i];

                if (veryVerbose) {
                    T_ P_(i) P_(Y) P(INPUT)
                }

                LOOP3_ASSERT(i, X, Y, X != Y);

                bdlsb::FixedMemInStreamBuf isb(INPUT.data(), INPUT.size());
                bsl::istream               input(&isb);

                balxml::MiniReader     reader;
                balxml::ErrorInfo      errInfo;
                balxml::DecoderOptions options;
                options.setValidateInputIsUtf8(CHECK_UTF8);

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
            typedef Test::MySequenceWithAttributes Type;

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

            const bsl::string_view INPUTS[] = {
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
            const int NUM_INPUT = sizeof INPUTS / sizeof *INPUTS;

            for (int ti = 0; ti < 2 * NUM_INPUT; ++ti) {
                const int  i                 = ti % NUM_INPUT;
                const bsl::string_view INPUT = INPUTS[i];
                const bool CHECK_UTF8        = NUM_INPUT <= ti;

                Type mX;  const Type& X = mX;

                const Type& Y = EXPECTED_RESULT[i];

                if (veryVerbose) {
                    T_ P_(i) P_(Y) P(INPUT)
                }

                LOOP3_ASSERT(i, X, Y, X != Y);

                bdlsb::FixedMemInStreamBuf isb(INPUT.data(), INPUT.size());
                bsl::istream               input(&isb);

                balxml::MiniReader     reader;
                balxml::ErrorInfo      errInfo;
                balxml::DecoderOptions options;
                options.setValidateInputIsUtf8(CHECK_UTF8);

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
            typedef Test::MySequenceWithAnonymousChoice Type;

            const bsl::string_view INPUTS[] = {
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
            const int NUM_INPUT = sizeof INPUTS / sizeof *INPUTS;

            Type EXPECTED_RESULT[NUM_INPUT];

            int tj = 0;

            EXPECTED_RESULT[tj].attribute1() = 34;
            EXPECTED_RESULT[tj].choice().makeMyChoice1(67);
            EXPECTED_RESULT[tj++].attribute2() = "Hello";

            EXPECTED_RESULT[tj].attribute1() = 34;
            EXPECTED_RESULT[tj].choice().makeMyChoice1(67);
            EXPECTED_RESULT[tj++].attribute2() = "Hello";

            EXPECTED_RESULT[tj].attribute1() = 34;
            EXPECTED_RESULT[tj].choice().makeMyChoice2("World!");
            EXPECTED_RESULT[tj++].attribute2() = "Hello";

            EXPECTED_RESULT[tj].attribute1() = 34;
            EXPECTED_RESULT[tj].choice().makeMyChoice2("  World! ");
            EXPECTED_RESULT[tj++].attribute2() = "Hello";

            ASSERT(NUM_INPUT == tj);

            for (int ti = 0; ti < 2 * NUM_INPUT; ++ti) {
                const int               tii        = ti % NUM_INPUT;
                const bsl::string_view& INPUT      = INPUTS[tii];
                const bool              CHECK_UTF8 = NUM_INPUT <= ti;
                const Type&             Y          = EXPECTED_RESULT[tii];

                Type mX;  const Type& X = mX;

                if (veryVerbose) {
                    T_ P_(tii) P_(Y) P(INPUT)
                }

                LOOP3_ASSERT(tii, X, Y, X != Y);

                bdlsb::FixedMemInStreamBuf isb(INPUT.data(), INPUT.size());
                bsl::istream               input(&isb);

                balxml::MiniReader     reader;
                balxml::ErrorInfo      errInfo;
                balxml::DecoderOptions options;
                options.setValidateInputIsUtf8(CHECK_UTF8);

                balxml::Decoder decoder(&options,
                                        &reader,
                                        &errInfo,
                                        &bsl::cerr,
                                        &bsl::cerr);

                decoder.decode(input, &mX);

                LOOP_ASSERT(tii, input);
                LOOP3_ASSERT(tii, X, Y, X == Y);
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

        static const struct Data {
            int              d_lineNum;  // source line number
            bsl::string_view d_input;    // input string
            FormattingMode   d_mode;     // formatting mode
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

        for (int ti = 0; ti < 2 * NUM_DATA; ++ti) {
            const Data&            data            = DATA[ti % 2];
            const int              LINE            = data.d_lineNum;
            const bsl::string_view INPUT           = data.d_input;
            const FormattingMode   FORMATTING_MODE = data.d_mode;
            const bool             CHECK_UTF8      = NUM_DATA <= ti;

            if (veryVerbose) {
                T_ P(LINE)

                if (veryVeryVerbose) {
                    T_ T_ P_(INPUT) P((int)FORMATTING_MODE)
                }
            }

            const bsl::string INIT_VALUE = "qwer";

            bdlsb::FixedMemInStreamBuf isb(INPUT.data(), INPUT.size());

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
            options.setValidateInputIsUtf8(CHECK_UTF8);

            balxml::Decoder_StdVectorCharContext context(&result1,
                                                         FORMATTING_MODE);

            balxml::MiniReader reader;
            balxml::ErrorInfo  errInfo;
            balxml::Decoder    decoder(&options,
                                       &reader,
                                       &errInfo,
                                       &outStream,
                                       &outStream);

            decoder.open(&isb);
            int retCode = context.beginParse(&decoder);;

            const bsl::vector<char> EXPECTED_RESULT(
                                                EXPECTED_RESULT_DATA.data(),
                                                EXPECTED_RESULT_DATA.data()
                                                + EXPECTED_RESULT_DATA.size());

            LOOP2_ASSERT(LINE, retCode, 0 == retCode);
            ASSERTV(LINE, EXPECTED_RESULT == result1);
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

        static const struct Data {
            int              d_lineNum;  // source line number
            bsl::string_view d_input;    // input string
            FormattingMode   d_mode;     // formatting mode
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

        for (int ti = 0; ti < 2 * NUM_DATA; ++ti) {
            const Data&           data            = DATA[ti % 2];
            const int             LINE            = data.d_lineNum;
            bsl::string_view      INPUT           = data.d_input;
            const FormattingMode  FORMATTING_MODE = data.d_mode;
            const bool            CHECK_UTF8      = NUM_DATA <= ti;

            if (veryVerbose) {
                T_ P(LINE)

                if (veryVeryVerbose) {
                    T_ T_ P_(INPUT) P((int)FORMATTING_MODE)
                }
            }

            const bsl::string INIT_VALUE = "qwer";

            bdlsb::FixedMemInStreamBuf isb(INPUT.data(), INPUT.size());
            bsl::string                result1 = INIT_VALUE;
            bsl::ostream               nullStream(0);

            // display error messages on 'bsl::cerr' only if errors are not
            // expected or if very very very verbose
            bsl::ostream& outStream = (veryVeryVeryVerbose)
                                      ? bsl::cerr
                                      : nullStream;

            balxml::DecoderOptions options;
            options.setMaxDepth(MAX_DEPTH);
            options.setValidateInputIsUtf8(CHECK_UTF8);

            balxml::Decoder_StdStringContext context(&result1,
                                                     FORMATTING_MODE);

            balxml::MiniReader reader;
            balxml::ErrorInfo  errInfo;
            balxml::Decoder    decoder(&options,
                                       &reader,
                                       &errInfo,
                                       &outStream,
                                       &outStream);

            decoder.open(&isb);
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

            static const struct Data {
                int              d_lineNum;  // source line number
                bsl::string_view d_input;    // input string
                int              d_retCode;  // expected return code
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

            for (int ti = 0; ti < 2 * NUM_DATA; ++ti) {
                const Data&             data              = DATA[ti % 2];
                const int               LINE              = data.d_lineNum;
                const bsl::string_view& INPUT             = data.d_input;
                const int               EXPECTED_RET_CODE = data.d_retCode;
                const bool              CHECK_UTF8        = NUM_DATA <= ti;

                if (veryVerbose) {
                    T_ P(LINE)

                    if (veryVeryVerbose) {
                        T_ T_ P(INPUT)
                        T_ T_ P(EXPECTED_RET_CODE)
                    }
                }

                bdlsb::FixedMemInStreamBuf isb(INPUT.data(), INPUT.size());
                TestChoice0                result;

                balxml::DecoderOptions     options;
                options.setMaxDepth(MAX_DEPTH);
                options.setSkipUnknownElements(false);
                options.setValidateInputIsUtf8(CHECK_UTF8);

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

                decoder.open(&isb);
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

            static const struct Data {
                int              d_lineNum;  // source line number
                bsl::string_view d_input;    // input string
                int              d_retCode;  // expected return code
                TC               d_result;   // expected result
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

            for (int ti = 0; ti < 2 * NUM_DATA; ++ti) {
                const Data&             data              = DATA[ti % 2];
                const int               LINE              = data.d_lineNum;
                const bsl::string_view& INPUT             = data.d_input;
                const int               EXPECTED_RET_CODE = data.d_retCode;
                const TC                EXPECTED_RESULT   = data.d_result;
                const bool              CHECK_UTF8        = NUM_DATA <= ti;

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

                bdlsb::FixedMemInStreamBuf isb(INPUT.data(), INPUT.size());
                TC                         result1 = INIT_VALUE;

                balxml::DecoderOptions options;
                options.setMaxDepth(MAX_DEPTH);
                options.setSkipUnknownElements(false);
                options.setValidateInputIsUtf8(CHECK_UTF8);

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

                decoder.open(&isb);
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

            static const struct Data {
                int              d_lineNum;  // source line number
                bsl::string_view d_input;    // input string
                int              d_retCode;  // expected return code
                TC               d_result;   // expected result
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

            for (int ti = 0; ti < 2 * NUM_DATA; ++ti) {
                const Data&             data              = DATA[ti % 2];
                const int               LINE              = data.d_lineNum;
                const bsl::string_view  INPUT             = data.d_input;
                const int               EXPECTED_RET_CODE = data.d_retCode;
                const TC                EXPECTED_RESULT   = data.d_result;
                const bool              CHECK_UTF8        = NUM_DATA <= ti;

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

                bdlsb::FixedMemInStreamBuf isb(INPUT.data(), INPUT.size());
                TC                         result1 = INIT_VALUE;

                balxml::DecoderOptions options;
                options.setMaxDepth(MAX_DEPTH);
                options.setSkipUnknownElements(false);
                options.setValidateInputIsUtf8(CHECK_UTF8);

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

                decoder.open(&isb);
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

            static const struct Data {
                int              d_lineNum;  // source line number
                bsl::string_view d_input;    // input string
                int              d_retCode;  // expected return code
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

            for (int ti = 0; ti < 2 * NUM_DATA; ++ti) {
                const Data&             data              = DATA[ti % NUM_DATA];
                const int               LINE              = data.d_lineNum;
                const bsl::string_view  INPUT             = data.d_input;
                const int               EXPECTED_RET_CODE = data.d_retCode;
                const bool              CHECK_UTF8        = NUM_DATA <= ti;

                if (veryVerbose) {
                    T_ P(LINE)

                    if (veryVeryVerbose) {
                        T_ T_ P(INPUT)
                        T_ T_ P(EXPECTED_RET_CODE)
                    }
                }

                bdlsb::FixedMemInStreamBuf isb(INPUT.data(), INPUT.size());
                TestSequence0              result;

                balxml::DecoderOptions options;
                options.setMaxDepth(MAX_DEPTH);
                options.setSkipUnknownElements(false);
                options.setValidateInputIsUtf8(CHECK_UTF8);

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

                decoder.open(&isb);
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

            static const struct Data {
                int              d_lineNum;  // source line number
                bsl::string_view d_input;    // input string
                int              d_min1;     // min occurrences for element 1
                int              d_max1;     // max occurrences for element 1
                int              d_retCode;  // expected return code
                TS               d_result;   // expected result
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

            for (int ti = 0; ti < 2 * NUM_DATA; ++ti) {
                const Data&             data              = DATA[ti % NUM_DATA];
                const int               LINE              = data.d_lineNum;
                const bsl::string_view  INPUT             = data.d_input;
                const int               EXPECTED_RET_CODE = data.d_retCode;
                const TS                EXPECTED_RESULT   = data.d_result;
                const bool              CHECK_UTF8        = NUM_DATA <= ti;

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

                bdlsb::FixedMemInStreamBuf isb(INPUT.data(), INPUT.size());
                TS                         result1 = INIT_VALUE;

                balxml::DecoderOptions options;
                options.setMaxDepth(MAX_DEPTH);
                options.setSkipUnknownElements(false);
                options.setValidateInputIsUtf8(CHECK_UTF8);

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

                decoder.open(&isb);
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

            static const struct Data {
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

            for (int ti = 0; ti < 2 * NUM_DATA; ++ti) {
                const Data&             data              = DATA[ti % NUM_DATA];
                const int               LINE              = data.d_lineNum;
                const bsl::string_view  INPUT             = data.d_input;
                const int               EXPECTED_RET_CODE = data.d_retCode;
                const TS                EXPECTED_RESULT   = data.d_result;
                const bool              CHECK_UTF8        = NUM_DATA <= ti;

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

                bdlsb::FixedMemInStreamBuf isb(INPUT.data(), INPUT.size());
                TS                         result1 = INIT_VALUE;

                balxml::DecoderOptions options;
                options.setMaxDepth(MAX_DEPTH);
                options.setSkipUnknownElements(false);
                options.setValidateInputIsUtf8(CHECK_UTF8);

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

                decoder.open(&isb);
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
            typedef Test::MySequenceWithNullables Type;

            const int         ATTRIBUTE1_VALUE = 123;
            const bsl::string ATTRIBUTE2_VALUE = "test string";
            Test::MySequence  ATTRIBUTE3_VALUE;

            ATTRIBUTE3_VALUE.attribute1() = 987;
            ATTRIBUTE3_VALUE.attribute2() = "inner";

            const bsl::string_view INPUTS[Type::NUM_ATTRIBUTES] = {
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

            for (int ti = 0; ti < 2 * Type::NUM_ATTRIBUTES; ++ti) {
                const int              i          = ti % Type::NUM_ATTRIBUTES;
                const bsl::string_view INPUT      = INPUTS[i];
                const bool             CHECK_UTF8 = Type::NUM_ATTRIBUTES <= ti;

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
                    T_ P_(i) P_(X) P(INPUT)
                }

                bdlsb::FixedMemInStreamBuf isb(INPUT.data(), INPUT.size());
                bsl::istream               input(&isb);

                Type mY;  const Type& Y = mY;
                LOOP3_ASSERT(i, X, Y, X != Y);

                balxml::MiniReader     reader;
                balxml::ErrorInfo      errInfo;
                balxml::DecoderOptions options;
                options.setValidateInputIsUtf8(CHECK_UTF8);

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
            typedef Test::MySequenceWithNillables Type;

            const int         ATTRIBUTE1_VALUE = 123;
            const bsl::string ATTRIBUTE2_VALUE = "test string";
            Test::MySequence  ATTRIBUTE3_VALUE;

            ATTRIBUTE3_VALUE.attribute1() = 987;
            ATTRIBUTE3_VALUE.attribute2() = "inner";

            const bsl::string_view INPUTS[Type::NUM_ATTRIBUTES] = {
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

            for (int ti = 0; ti < 2 * Type::NUM_ATTRIBUTES; ++ti) {
                const int               i          = ti % Type::NUM_ATTRIBUTES;
                const bsl::string_view  INPUT      = INPUTS[i];
                const bool              CHECK_UTF8 = Type::NUM_ATTRIBUTES <= ti;

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
                    T_ P_(i) P_(X) P(INPUT)
                }

                bdlsb::FixedMemInStreamBuf isb(INPUT.data(), INPUT.size());
                bsl::istream               input(&isb);

                Type mY;  const Type& Y = mY;
                LOOP3_ASSERT(i, X, Y, X != Y);

                balxml::MiniReader     reader;
                balxml::ErrorInfo      errInfo;
                balxml::DecoderOptions options;
                options.setSkipUnknownElements(false);
                options.setValidateInputIsUtf8(CHECK_UTF8);

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
            int              d_lineNum;  // source line number
            bsl::string_view d_input;    // input string
            int              d_retCode;  // expected ret code
            int              d_result;   // expected result
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
            const int               LINE              = DATA[i].d_lineNum;
            const bsl::string_view& INPUT             = DATA[i].d_input;
            const int               EXPECTED_RET_CODE = DATA[i].d_retCode;
            const int               EXPECTED_RESULT   = DATA[i].d_result;

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

            bdlsb::FixedMemInStreamBuf isb(INPUT.data(), INPUT.size());
            int                        result1 = INIT_VALUE;
            bsl::ostream               nullStream(0);

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

            decoder.open(&isb);
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
            int               d_lineNum;  // source line number
            bsl::string_view  d_input;    // input string
            int               d_retCode;  // expected ret code
            const char       *d_result;   // expected result
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
            const int                LINE              = DATA[i].d_lineNum;
            const bsl::string_view&  INPUT             = DATA[i].d_input;
            const int                EXPECTED_RET_CODE = DATA[i].d_retCode;
            const char              *EXPECTED_DATA     = DATA[i].d_result;

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

                bdlsb::FixedMemInStreamBuf isb(INPUT.data(), INPUT.size());
                Type                       result1(
                                               INIT_VALUE,
                                               INIT_VALUE + INIT_VALUE_LENGTH);
                bsl::ostream               nullStream(0);

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

                decoder.open(&isb);
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

                bdlsb::FixedMemInStreamBuf isb(INPUT.data(), INPUT.size());
                Type                       result1(
                                               INIT_VALUE,
                                               INIT_VALUE + INIT_VALUE_LENGTH);
                bsl::ostream               nullStream(0);

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

                decoder.open(&isb);
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

                    ASSERTV(LINE, EXPECTED_RESULT == result1);
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
                typedef TestSequence0                         TestType;
                typedef
                balxml::Decoder_SelectContext<TestType>::Type Result;
                typedef
                balxml::Decoder_SequenceContext<TestType>     ExpectedResult;

                ASSERT((bslmf::IsSame<Result, ExpectedResult>::value));
            }
            {
                typedef TestSequence1                         TestType;
                typedef
                balxml::Decoder_SelectContext<TestType>::Type Result;
                typedef
                balxml::Decoder_SequenceContext<TestType>     ExpectedResult;

                ASSERT((bslmf::IsSame<Result, ExpectedResult>::value));
            }
            {
                typedef TestSequence2                         TestType;
                typedef
                balxml::Decoder_SelectContext<TestType>::Type Result;
                typedef
                balxml::Decoder_SequenceContext<TestType>     ExpectedResult;

                ASSERT((bslmf::IsSame<Result, ExpectedResult>::value));
            }
        }

        if (verbose) cout << "\nUsing choice types." << endl;
        {
            {
                typedef TestChoice0                           TestType;
                typedef
                balxml::Decoder_SelectContext<TestType>::Type Result;
                typedef
                balxml::Decoder_ChoiceContext<TestType>       ExpectedResult;

                ASSERT((bslmf::IsSame<Result, ExpectedResult>::value));
            }
            {
                typedef TestChoice1                           TestType;
                typedef
                balxml::Decoder_SelectContext<TestType>::Type Result;
                typedef
                balxml::Decoder_ChoiceContext<TestType>       ExpectedResult;

                ASSERT((bslmf::IsSame<Result, ExpectedResult>::value));
            }
            {
                typedef TestChoice2                       TestType;
                typedef
                balxml::Decoder_SelectContext<TestType>::Type Result;
                typedef
                balxml::Decoder_ChoiceContext<TestType>       ExpectedResult;

                ASSERT((bslmf::IsSame<Result, ExpectedResult>::value));
            }
        }

        if (verbose) cout << "\nUsing simple types." << endl;
        {
            {
                typedef int                                   TestType;
                typedef
                balxml::Decoder_SelectContext<TestType>::Type Result;
                typedef
                balxml::Decoder_SimpleContext<TestType>       ExpectedResult;

                ASSERT((bslmf::IsSame<Result, ExpectedResult>::value));
            }
            {
                typedef short                                 TestType;
                typedef
                balxml::Decoder_SelectContext<TestType>::Type Result;
                typedef
                balxml::Decoder_SimpleContext<TestType>       ExpectedResult;

                ASSERT((bslmf::IsSame<Result, ExpectedResult>::value));
            }
            {
                typedef double                                TestType;
                typedef
                balxml::Decoder_SelectContext<TestType>::Type Result;
                typedef
                balxml::Decoder_SimpleContext<TestType>       ExpectedResult;

                ASSERT((bslmf::IsSame<Result, ExpectedResult>::value));
            }
            {
                typedef float                                 TestType;
                typedef
                balxml::Decoder_SelectContext<TestType>::Type Result;
                typedef
                balxml::Decoder_SimpleContext<TestType>       ExpectedResult;

                ASSERT((bslmf::IsSame<Result, ExpectedResult>::value));
            }
            {
                typedef long long                             TestType;
                typedef
                balxml::Decoder_SelectContext<TestType>::Type Result;
                typedef
                balxml::Decoder_SimpleContext<TestType>       ExpectedResult;

                ASSERT((bslmf::IsSame<Result, ExpectedResult>::value));
            }
            {
                typedef DummyEnumeration::Value               TestType;
                typedef
                balxml::Decoder_SelectContext<TestType>::Type Result;
                typedef
                balxml::Decoder_SimpleContext<TestType>       ExpectedResult;

                ASSERT((bslmf::IsSame<Result, ExpectedResult>::value));
            }
            {
                typedef DummyCustomizedType                   TestType;
                typedef
                balxml::Decoder_SelectContext<TestType>::Type Result;
                typedef
                balxml::Decoder_CustomizedContext<TestType>   ExpectedResult;

                ASSERT((bslmf::IsSame<Result, ExpectedResult>::value));
            }
        }

        if (verbose) cout << "\nUsing array types." << endl;
        {
            {
                typedef bsl::vector<int>                            TestType;
                typedef
                balxml::Decoder_SelectContext<TestType>::Type       Result;
                typedef
                balxml::Decoder_PushParserContext<
                         TestType,
                         balxml::Decoder_ListParser<TestType> > ExpectedResult;

                ASSERT((bslmf::IsSame<Result, ExpectedResult>::value));
            }
            {
                typedef bsl::vector<short>                          TestType;
                typedef
                balxml::Decoder_SelectContext<TestType>::Type       Result;
                typedef
                balxml::Decoder_PushParserContext<
                         TestType,
                         balxml::Decoder_ListParser<TestType> > ExpectedResult;

                ASSERT((bslmf::IsSame<Result, ExpectedResult>::value));
            }
            {
                typedef bsl::vector<double>                         TestType;
                typedef
                balxml::Decoder_SelectContext<TestType>::Type       Result;
                typedef
                balxml::Decoder_PushParserContext<
                         TestType,
                         balxml::Decoder_ListParser<TestType> > ExpectedResult;

                ASSERT((bslmf::IsSame<Result, ExpectedResult>::value));
            }
        }

        if (verbose) cout << "\nUsing 'bsl::string'." << endl;
        {
            {
                typedef bsl::string                           TestType;
                typedef
                balxml::Decoder_SelectContext<TestType>::Type Result;
                typedef balxml::Decoder_StdStringContext      ExpectedResult;

                ASSERT((bslmf::IsSame<Result, ExpectedResult>::value));
            }
        }

        if (verbose) cout << "\nUsing 'bsl::vector<char>'." << endl;
        {
            {
                typedef bsl::vector<char>                     TestType;
                typedef
                balxml::Decoder_SelectContext<TestType>::Type Result;
                typedef balxml::Decoder_StdVectorCharContext  ExpectedResult;

                ASSERT((bslmf::IsSame<Result, ExpectedResult>::value));
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

        static const struct Data {
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

        for (int ti = 0; ti < 2 * NUM_DATA; ++ti) {
            const Data&            data                   = DATA[ti % NUM_DATA];
            const int              LINE                   = data.d_lineNum;
            const bsl::string_view INPUT                  = data.d_input;
            const int              MAX_DEPTH              = data.d_maxDepth;
            const bool             EXPECTED_SUCCESS       = data.d_success;
            const char            *EXPECTED_CALL_SEQUENCE = data.d_callSequence;
            const bool             CHECK_UTF8             = NUM_DATA <= ti;

            if (veryVerbose) {
                T_ P(LINE)

                if (veryVeryVerbose) {
                    T_ T_ P(INPUT)
                    T_ T_ P(EXPECTED_SUCCESS)
                    T_ T_ P(EXPECTED_CALL_SEQUENCE)
                }
            }

            bdlsb::FixedMemInStreamBuf isb(INPUT.data(), INPUT.size());
            bsl::stringstream          callSequence;
            bsl::ostream               errorStream(0);
            bsl::ostream               warningStream(0);

            bslma::TestAllocator testAllocator;

            balxml::DecoderOptions options;
            options.setMaxDepth(MAX_DEPTH);
            options.setValidateInputIsUtf8(CHECK_UTF8);

            TestContext context(callSequence, XML_NAME, &testAllocator);

            balxml::MiniReader reader;
            balxml::ErrorInfo  errInfo;
            balxml::Decoder    decoder(&options,
                                       &reader,
                                       &errInfo,
                                       &errorStream,
                                       &warningStream);

            decoder.open(&isb);
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
            const bsl::string_view INPUT =
                                "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                                "<Value " XSI ">\n"
                                "    123\n"
                                "</Value>";
            bdlsb::FixedMemInStreamBuf isb(INPUT.data(), INPUT.size());

            if (veryVerbose) {
                T_ P(INPUT)
            }

            balxml::MiniReader     reader;
            balxml::ErrorInfo      errInfo;
            balxml::DecoderOptions options;

            balxml::Decoder decoder(&options,
                                    &reader,
                                    &errInfo,
                                    &bsl::cerr,
                                    &bsl::cerr);
            int i;

            const int ret = decoder.decode(&isb, &i);
            const unsigned offset = reader.getCurrentPosition();

            LOOP_ASSERT(ret, 0 == ret);
            LOOP_ASSERT(i, 123 == i);
            ASSERTV(offset, INPUT.size(), offset == INPUT.size());
        }

        if (verbose) cout << "\nTesting bsl::string." << endl;
        {
            const bsl::string_view INPUT =
                                "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                                "<Value " XSI ">\n"
                                "    abc\n"
                                "</Value>";
            bdlsb::FixedMemInStreamBuf isb(INPUT.data(), INPUT.size());

            if (veryVerbose) {
                T_ P(INPUT)
            }

            balxml::MiniReader     reader;
            balxml::ErrorInfo      errInfo;
            balxml::DecoderOptions options;

            balxml::Decoder decoder(&options,
                                    &reader,
                                    &errInfo,
                                    &bsl::cerr,
                                    &bsl::cerr);
            bsl::string s;

            const int ret = decoder.decode(&isb, &s);
            const unsigned offset = reader.getCurrentPosition();

            LOOP_ASSERT(ret, 0 == ret);
            LOOP_ASSERT(s, "\n    abc\n" == s);
            ASSERTV(offset, INPUT.size(), offset == INPUT.size());
        }

        if (verbose) cout << "\nTesting TestSequence2." << endl;
        {
            const bsl::string_view INPUT =
                                "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                                "<TestSequence2 " XSI ">\n"
                                "    <E1>123</E1>\n"
                                "    <E2>abc</E2>\n"
                                "</TestSequence2>";
            bdlsb::FixedMemInStreamBuf isb(INPUT.data(), INPUT.size());
            bsl::istream               is(&isb);

            if (veryVerbose) {
                T_ P(INPUT)
            }

            balxml::MiniReader     reader;
            balxml::ErrorInfo      errInfo;
            balxml::DecoderOptions options;

            balxml::Decoder decoder(&options,
                                    &reader,
                                    &errInfo,
                                    &bsl::cerr,
                                    &bsl::cerr);
            TestSequence2 ts;

            decoder.decode(is, &ts);
            const unsigned offset = reader.getCurrentPosition();

            LOOP_ASSERT(is.fail(), !is.fail());
            LOOP_ASSERT(ts, TestSequence2(123, "abc") == ts);
            ASSERTV(offset, INPUT.size(), offset == INPUT.size());
        }

        if (verbose) cout << "\nTesting TestChoice2." << endl;
        {
            const bsl::string_view INPUT =
                                "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                                "<TestChoice2 " XSI ">\n"
                                "    <S1>123</S1>\n"
                                "</TestChoice2>";
            bdlsb::FixedMemInStreamBuf isb(INPUT.data(), INPUT.size());
            bsl::istream               is(&isb);

            if (veryVerbose) {
                T_ P(INPUT)
            }

            balxml::MiniReader     reader;
            balxml::ErrorInfo      errInfo;
            balxml::DecoderOptions options;

            balxml::Decoder decoder(&options,
                                    &reader,
                                    &errInfo,
                                    &bsl::cerr,
                                    &bsl::cerr);
            TestChoice2 tc;

            decoder.decode(is, &tc);
            const unsigned offset = reader.getCurrentPosition();

            LOOP_ASSERT(is.fail(), !is.fail());
            LOOP_ASSERT(tc, TestChoice2(123) == tc);
            ASSERTV(offset, INPUT.size(), offset == INPUT.size());
        }

        if (verbose) cout << "\nEnd of Breathing Test." << endl;
      } break;
      case -1: {
        // --------------------------------------------------------------------
        // EXHAUSTIVE TESTING VALID & INVALID UTF-8: e_FILE
        //
        // Concern:
        //: 1 We test valid and invalid UTF-8 injected into an XML string that
        //:   we decode.  In this test case we handle the situation where the
        //:   data is in a file.  We break this out of the previous test case
        //:   because it's much slower than the other media of transmitting the
        //:   data, which are all memory-only.
        //:
        //: 2 In this test, since it is a negative test case, we run the test
        //:   exhaustively, which will take more time than is acceptable in a
        //:   nightly build.
        //
        // Plan:
        //: 1 Call Utf8Test::validAndInvalidUtf8Test(e_FILE);
        //:
        //: 2 Further details in the doc of 'validAndInvalidUtf8Test'.
        //
        // Tesing:
        //   TESTING VALID & INVALID UTF-8: e_FILE
        // --------------------------------------------------------------------

        if (verbose) cout << "TESTING VALID & INVALID UTF-8: e_FILE\n"
                             "=====================================\n";

        namespace TC = Utf8Test;

        TC::validAndInvalidUtf8Test(TC::e_FILE, true);
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
