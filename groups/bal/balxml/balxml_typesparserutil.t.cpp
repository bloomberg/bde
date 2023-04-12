// balxml_typesparserutil.t.cpp                                       -*-C++-*-

#include <balxml_typesparserutil.h>

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <balxml_typesprintutil.h>  // for testing only

#include <s_baltst_customizedstring.h>
#include <s_baltst_myenumeration.h>
#include <s_baltst_myenumerationwithfallback.h>

#include <bdlat_enumeratorinfo.h>
#include <bdlat_enumfunctions.h>
#include <bdlat_typetraits.h>
#include <bdlat_valuetypefunctions.h>

#include <bdlb_chartype.h>
#include <bdlb_float.h>  // for printing vector
#include <bdlb_print.h>
#include <bdlb_printmethods.h>

#include <bdldfp_decimalutil.h>

#include <bdlsb_fixedmeminstreambuf.h>

#include <bdlt_datetime.h>

#include <bslim_testutil.h>

#include <bslalg_typetraits.h>
#include <bslma_allocator.h>

#include <bslmf_conditional.h>
#include <bslmf_issame.h>

#include <bsl_cstddef.h>
#include <bsl_cstdlib.h>
#include <bsl_cstring.h>
#include <bsl_iosfwd.h>
#include <bsl_iostream.h>
#include <bsl_limits.h>
#include <bsl_ostream.h>
#include <bsl_sstream.h>
#include <bsl_string.h>
#include <bsl_string_view.h>

#include <bsls_assert.h>
#include <bsls_types.h>

#include <s_baltst_generatetestarray.h>
#include <s_baltst_generatetestnullablevalue.h>
#include <s_baltst_testplaceholder.h>

using namespace BloombergLP;
using bsl::cout;
using bsl::cerr;
using bsl::endl;
using bsl::flush;
namespace test = BloombergLP::s_baltst;

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

                       // =============================
                       // class AssertParsedTextIsEqual
                       // =============================

class AssertParsedTextIsEqual {
  public:
    // CREATORS
    AssertParsedTextIsEqual() {}

    // ACCESSORS
    template <class TYPE>
    void operator()(int                     line,
                    const bsl::string_view& xml,
                    const TYPE&             expectedValue,
                    int                     formattingMode) const
    {
        TYPE value;
        int  rc = balxml::TypesParserUtil::parse(&value,
                                                 xml.data(),
                                                 static_cast<int>(xml.length()),
                                                 formattingMode);
        LOOP1_ASSERT(line, 0 == rc);
        LOOP1_ASSERT(line, expectedValue == value);
    }
};

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

// ============================================================================
//                   GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
// ----------------------------------------------------------------------------

typedef balxml::TypesParserUtil Util;

typedef balxml::TypesPrintUtil  Print;


// ============================================================================
//                       ROUND TRIP TESTING MACHINERY
// ----------------------------------------------------------------------------

namespace TestMachinery {
#if defined(BSLS_PLATFORM_CPU_64_BIT) && !defined(BSLS_PLATFORM_OS_WINDOWS)
#define U_LONG_IS_64_BITS
    // On 64 bit systems 'long' may be 32 or 64 bits.
#endif

template <class ENCODED_TYPE>
struct GetDecodeType {
    // Meta-function to chose a type we parse/decode into.  For most types we
    // just use the type we encoded from.

    typedef ENCODED_TYPE Type;

};

template <>
struct GetDecodeType<signed long int> {
    // 'signed long int' is not supported to parse/decode into, as it may have
    // a differing size on different 64 bit platforms.

#ifdef U_LONG_IS_64_BITS
    typedef long long int          Type;
#else
    typedef int                    Type;
#endif
};

template <>
struct GetDecodeType<unsigned long int> {
    // 'unsigned long int' is not supported to parse/decode into, as it may
    // have a differing size on different 64 bit platforms.

#ifdef U_LONG_IS_64_BITS
    typedef unsigned long long int Type;
#else
    typedef unsigned int           Type;
#endif
};

template <>
struct GetDecodeType<const char *> {
    // Can't parse into a 'const char *', but can into a 'bsl::string'.
    typedef bsl::string Type;
};

template <>
struct GetDecodeType<bslstl::StringRef> {
    // Can't parse into a string reference, but can into a 'bsl::string'.
    typedef bsl::string Type;
};

int intLength(const bsl::string_view& s)
    // Return the length of 's' as an 'int' after verifying it fits in 'int'.
    // The verifying part is more for show, we don't expect 2 gigabyte strings.
{
    ASSERTV(s.length(),
            s.length() <=
                    static_cast<bsl::size_t>(bsl::numeric_limits<int>::max()));

    return static_cast<int>(s.length());
}

template <class       TEST_DATA,
          bsl::size_t NUM_DATA>
void printDecimalRoundTripTester(const TEST_DATA (&DATA)[NUM_DATA])
    // Verify round trip of 'printDecimal'/'parseDecimal' for the values in the
    // specified 'DATA' array of the deduced 'NUM_DATA' length.  The deduced
    // 'TEST_DATA' must provide a member 'Type' that will be printed from, and
    // parsed into.
{
    typedef typename TEST_DATA::Type Type;

    typedef typename GetDecodeType<Type>::Type DecodeType;
        // Some types we can print/encode from, but cannot parse/decode into.
        // 'GetDecodeType' is the level of indirection that solves that issue.

    for (bsl::size_t i = 0; i < NUM_DATA; ++i) {
        const int   LINE  = DATA[i].d_line;
        const Type  INPUT = DATA[i].d_input;

        bsl::stringstream ss;
        Print::printDecimal(ss, INPUT);
        ASSERTV(LINE, ss.good());
        const bsl::string ENCODED(ss.str());

        DecodeType decoded;
        int rc;
        ASSERTV(LINE, 0 == (rc = Util::parseDecimal(&decoded,
                                                    ENCODED.data(),
                                                    intLength(ENCODED))));
        if (0 == rc) {
            // To avoid a "reading uninitialized 'decode'" warning
            ASSERTV(LINE, ENCODED, decoded, INPUT, INPUT == decoded);
        }
    }
}

bsl::string replaceCharEntities(const bsl::string& withEntities)
    // Replace select XML character references in the specified 'withEntities'
    // with the corresponding ASCII character and return the result.
    //
    // The following pre-defined character entities are recognized only:
    //..
    //  Entity  Char    Name
    //  ======  ====    ====
    //  &lt;    <       Less-than
    //  &gt;    >       Greater-than
    //  &amp;   &       Ampersand
    //  &apos;  '       Apostrophe
    //  &quot;  "       Quote
{
    // This code is neither fast, nor clever, and it is only correct within the
    // confines of the round trip testing in this test driver.

    static const struct Entity {
        const char  *d_ent;
        bsl::size_t  d_len;
        char         d_ch;
    } ENTITIES[] = {
        { "&lt;",   4, '<'  },
        { "&gt;",   4, '>'  },
        { "&amp;",  5, '&'  },
        { "&apos;", 6, '\'' },
        { "&quot;", 6, '"'  },
        { 0, 0, 0 }
    };

    bsl::string rv(withEntities);

    const Entity *entity = ENTITIES;
    while (entity->d_ent) {
        bsl::size_t pos = rv.find(entity->d_ent, 0);

        while (pos != bsl::string::npos) {
            rv.replace(pos, entity->d_len, 1, entity->d_ch);
            pos = rv.find(entity->d_ent, pos + 1);
        }

        ++entity;
    }

    return rv;
}

template <class       TEST_DATA,
          bsl::size_t NUM_DATA>
void printTextRoundTripScalarTester(const TEST_DATA (&DATA)[NUM_DATA])
    // Verify round trip of 'printDecimal'/'parseDecimal' for the values in the
    // specified 'DATA' array of the deduced 'NUM_DATA' length.  The deduced
    // 'TEST_DATA' must provide a member scalar 'Type' that will be printed
    // from, and parsed into.
{
    typedef typename TEST_DATA::Type Type;

    typedef typename GetDecodeType<Type>::Type DecodeType;
        // Some types we can print/encode from, but cannot parse/decode into.
        // 'GetDecodeType' is the level of indirection that solves that issue.

    for (bsl::size_t i = 0; i < NUM_DATA; ++i) {
        const int  LINE  = DATA[i].d_line;
        const Type INPUT = DATA[i].d_input;

        bsl::stringstream ss;
        Print::printText(ss, INPUT);
        ASSERTV(LINE, ss.good());
        const bsl::string ENCODED(replaceCharEntities(ss.str()));

        DecodeType decoded;
        int rc;
        ASSERTV(LINE, 0 == (rc = Util::parseText(&decoded,
                                                 ENCODED.data(),
                                                 intLength(ENCODED))));
        if (0 == rc) {
            // To avoid a "reading uninitialized 'decode'" warning
            ASSERTV(LINE, ENCODED, decoded, INPUT, INPUT == decoded);
        }
    }
}

template <class OBJECT_TYPE>
OBJECT_TYPE buildPrintTextInput(const bsl::string_view&  HEADER,
                                const OBJECT_TYPE&       INPUT,
                                const bsl::string_view&  TRAILER)
    // Build, and return a 'printText' input by concatenating the specified
    // 'HEADER', 'INPUT', and ' TRAILER'.
{
    OBJECT_TYPE rv(HEADER.begin(), HEADER.end());

    rv.insert(rv.end(), INPUT.begin(), INPUT.end());

    rv.insert(rv.end(), TRAILER.begin(), TRAILER.end());

    return rv;
}

template <class       TEST_DATA,
          bsl::size_t NUM_DATA>
void printDefaultRoundTripTester(const TEST_DATA (&DATA)[NUM_DATA])
    // Verify round trip of 'printDefault'/'parseDefault' for the values in the
    // specified 'DATA' array of the deduced 'NUM_DATA' length.  The deduced
    // 'TEST_DATA' must provide a member 'Type' that will be printed from, and
    // parsed into.
{
    typedef typename TEST_DATA::Type Type;

    typedef typename GetDecodeType<Type>::Type DecodeType;
        // Some types we can print/encode from, but cannot parse/decode into.
        // 'GetDecodeType' is the level of indirection that solves that issue.

    for (bsl::size_t i = 0; i < NUM_DATA; ++i) {
        const int   LINE  = DATA[i].d_line;
        const Type  INPUT = DATA[i].d_input;

        bsl::stringstream ss;
        Print::printDefault(ss, INPUT);
        ASSERTV(LINE, ss.good());
        const bsl::string ENCODED(replaceCharEntities(ss.str()));

        DecodeType decoded;
        int rc;
        ASSERTV(LINE, 0 == (rc = Util::parseDefault(&decoded,
                                                    ENCODED.data(),
                                                    intLength(ENCODED))));
        if (0 == rc) {
            // To avoid a "reading uninitialized 'decode'" warning
            ASSERTV(LINE, ENCODED, decoded, INPUT, INPUT == decoded);
        }
    }
}

template <class INPUT_TYPE>
struct RoundTripTestData {
    // The non-local test data type that is required for test helper function
    // templates to compile in C++03.

    typedef INPUT_TYPE Type;

    int  d_line;
    Type d_input;
};

}  // close namespace TestMachinery

                              // ===============
                              // struct TestEnum
                              // ===============

namespace TestNamespace {

struct TestEnum {
    enum Value {
        VALUE1 = 1,
        VALUE2 = 2,
        VALUE3 = 3
    };
};

bsl::ostream& operator<<(bsl::ostream& stream, TestEnum::Value rhs)
{
    switch (rhs) {
      case TestEnum::VALUE1:
        return stream << "VALUE1";                                    // RETURN
      case TestEnum::VALUE2:
        return stream << "VALUE2";                                    // RETURN
      case TestEnum::VALUE3:
        return stream << "VALUE3";                                    // RETURN
      default:
        return stream << "(* UNKNOWN *)";                             // RETURN
    }
}

}  // close namespace TestNamespace

using TestNamespace::TestEnum;

                 // ==========================================
                 // bdlat_EnumFunctions Overrides for TestEnum
                 // ==========================================

namespace BloombergLP {

namespace bdlat_EnumFunctions {

    template <>
    struct IsEnumeration<TestEnum::Value> : bslmf::MetaInt<1> {
    };

    template <>
    int fromInt<TestEnum::Value>(TestEnum::Value *result, int number)
    {
        switch (number) {
          case TestEnum::VALUE1:
            *result = TestEnum::VALUE1;
            return 0;                                                 // RETURN
          case TestEnum::VALUE2:
            *result = TestEnum::VALUE2;
            return 0;                                                 // RETURN
          case TestEnum::VALUE3:
            *result = TestEnum::VALUE3;
            return 0;                                                 // RETURN
        }

        return -1;
    }

    template <>
    int fromString<TestEnum::Value>(TestEnum::Value *result,
                                    const char      *string,
                                    int              stringLength)
    {
        if ("VALUE1" == bsl::string(string, stringLength)) {
            *result = TestEnum::VALUE1;
            return 0;                                                 // RETURN
        }

        if ("VALUE2" == bsl::string(string, stringLength)) {
            *result = TestEnum::VALUE2;
            return 0;                                                 // RETURN
        }

        if ("VALUE3" == bsl::string(string, stringLength)) {
            *result = TestEnum::VALUE3;
            return 0;                                                 // RETURN
        }

        return -1;
    }

}  // close namespace bdlat_EnumFunctions
}  // close enterprise namespace


// test_customizedint.h   -*-C++-*-

//@PURPOSE: todo: provide purpose.
//
//@CLASSES:
//  CustomizedInt: a customized integer
//
//@DESCRIPTION:
//  todo: provide annotation for 'CustomizedInt'

namespace BloombergLP {
namespace s_baltst {

class CustomizedInt {

  private:
    // PRIVATE DATA MEMBERS
    int d_value;  // stored value

    // FRIENDS
    friend bool operator==(const CustomizedInt&, const CustomizedInt&);
    friend bool operator!=(const CustomizedInt&, const CustomizedInt&);

  public:
    // TYPES
    typedef int BaseType;

    // CONSTANTS
    static const char CLASS_NAME[];
        // the name of this class (i.e., "CustomizedInt")

    // CREATORS
    CustomizedInt();
        // Create an object of type 'CustomizedInt' having the default value.

    CustomizedInt(const CustomizedInt& original);
        // Create an object of type 'CustomizedInt' having the value of the
        // specified 'original' object.

    explicit CustomizedInt(int value);
        // Create an object of type 'CustomizedInt' having the specified
        // 'value'.

    ~CustomizedInt();
        // Destroy this object.

    // MANIPULATORS
    CustomizedInt& operator=(const CustomizedInt& rhs);
        // Assign to this object the value of the specified 'rhs' object.

    void reset();
        // Reset this object to the default value (i.e., its value upon
        // default construction).

    int fromInt(int value);
        // Convert from the specified 'value' to this type.  Return 0 if
        // successful and non-zero otherwise.

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

    const int& toInt() const;
        // Convert this value to 'int'.
};

// FREE OPERATORS
inline
bool operator==(const CustomizedInt& lhs, const CustomizedInt& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects have
    // the same value, and 'false' otherwise.  Two attribute objects have the
    // same value if each respective attribute has the same value.

inline
bool operator!=(const CustomizedInt& lhs, const CustomizedInt& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects do not
    // have the same value, and 'false' otherwise.  Two attribute objects do
    // not have the same value if one or more respective attributes differ in
    // values.

inline
bsl::ostream& operator<<(bsl::ostream& stream, const CustomizedInt& rhs);
    // Format the specified 'rhs' to the specified output 'stream' and return a
    // reference to the modifiable 'stream'.

// ============================================================================
//                        INLINE FUNCTION DEFINITIONS
// ============================================================================

// CREATORS

inline
CustomizedInt::CustomizedInt()
{
}

inline
CustomizedInt::CustomizedInt(const CustomizedInt& original)
: d_value(original.d_value)
{
}

inline
CustomizedInt::CustomizedInt(int value)
: d_value(value)
{
}

inline
CustomizedInt::~CustomizedInt()
{
}

// MANIPULATORS

inline
CustomizedInt& CustomizedInt::operator=(const CustomizedInt& rhs)
{
    d_value = rhs.d_value;
    return *this;
}

inline
void CustomizedInt::reset()
{
    bdlat_ValueTypeFunctions::reset(&d_value);
}

inline
int CustomizedInt::fromInt(int value)
{
    enum { SUCCESS = 0, FAILURE = -1 };

    if (5 < value) {
        return FAILURE;                                               // RETURN
    }

    d_value = value;

    return SUCCESS;
}

// ACCESSORS

inline
bsl::ostream& CustomizedInt::print(bsl::ostream& stream,
                                   int           level,
                                   int           spacesPerLevel) const
{
    return bdlb::PrintMethods::print(stream, d_value, level, spacesPerLevel);
}

inline
const int& CustomizedInt::toInt() const
{
    return d_value;
}

}  // close namespace s_baltst

// TRAITS

BDLAT_DECL_CUSTOMIZEDTYPE_TRAITS(test::CustomizedInt)

// FREE OPERATORS

inline
bool test::operator==(const CustomizedInt& lhs, const CustomizedInt& rhs)
{
    return lhs.d_value == rhs.d_value;
}

inline
bool test::operator!=(const CustomizedInt& lhs, const CustomizedInt& rhs)
{
    return lhs.d_value != rhs.d_value;
}

inline
bsl::ostream& test::operator<<(bsl::ostream& stream, const CustomizedInt& rhs)
{
    return rhs.print(stream, 0, -1);
}

}  // close enterprise namespace

// ----------------------------------------------------------------------------
//                       *End-of-file Block removed.*
// ----------------------------------------------------------------------------

// test_customizedint.cpp  -*-C++-*-

namespace BloombergLP {
namespace s_baltst {

                               // ---------
                               // CONSTANTS
                               // ---------

const char CustomizedInt::CLASS_NAME[] = "CustomizedInt";
    // the name of this class

                                // -------------
                                // CLASS METHODS
                                // -------------

                                // --------
                                // CREATORS
                                // --------

                                // ------------
                                // MANIPULATORS
                                // ------------

                                // ---------
                                // ACCESSORS
                                // ---------

}  // close namespace s_baltst
}  // close enterprise namespace

// ----------------------------------------------------------------------------
//                       *End-of-file Block removed.*
// ----------------------------------------------------------------------------

// ============================================================================
//                               USAGE EXAMPLE
// ----------------------------------------------------------------------------

// The following snippets of code illustrate the usage of this component.  It
// parses a Base64 string into an 'bsl::vector<char>':
//..
//  #include <balxml_typesparserutil.h>
//
//  #include <cassert>
//  #include <vector>
//
//  using namespace BloombergLP;
//
void usageExample()
{
    const char INPUT[]      = "YWJjZA==X";  // "abcd" in Base64
    const int  INPUT_LENGTH = static_cast<int>(sizeof(INPUT)) - 2;

    bsl::vector<char> vec;

    int retCode = balxml::TypesParserUtil::parseBase64(&vec,
                                                       INPUT,
                                                       INPUT_LENGTH);

    ASSERT(0   == retCode);
    ASSERT(4   == vec.size());
    ASSERT('a' == vec[0]);
    ASSERT('b' == vec[1]);
    ASSERT('c' == vec[2]);
    ASSERT('d' == vec[3]);
}
//..

// ============================================================================
//                               MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int        test = argc > 1 ? bsl::atoi(argv[1]) : 0;
    int     verbose = argc > 2;
    int veryVerbose = argc > 3;

    bsl::cout << "TEST " << __FILE__ << " CASE " << test << bsl::endl;;

    switch (test) { case 0:  // Zero is always the leading case.
      case 16: {
        // --------------------------------------------------------------------
        // TESTING USAGE EXAMPLE
        //
        // Concerns:
        //
        // Plan:
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "\nTesting Usage Example"
                               << "\n=====================" << bsl::endl;

        usageExample();

        if (verbose) bsl::cout << "\nEnd of test." << bsl::endl;
      } break;
      case 15: {
        // --------------------------------------------------------------------
        // TESTING PARSING LISTS OF NULLABLE SIMPLE TYPES
        //   This case tests that whitespace-delimited representations of
        //   simple types can be decoded into arrays of the corresponding
        //   nullable simple type.
        //
        // Concerns:
        //: 1 Whitespace-separated representations of simple types can be
        //:   decoded into arrays of the corresponding nullable simple type
        //:   if the array has the list formatting mode.
        //:
        //: 2 The amount of whitespace between elements is insignificant.
        //:
        //: 3 The resulting array has no null elements.  Note that this
        //:   property results from the fact that the encoded representation
        //:   of null elements is the empty string.
        //
        // Plan:
        //: 1 Given two simple types, 'int' and 'bsl::string', do the
        //:   following:
        //:
        //:   1 Enumerate all possible textual representations of lists of
        //:     simple values up to length 3, parse these representations into
        //:     arrays of the corresponding simple types, and verify parsing
        //:     succeeds.
        //:
        //:   2 Enumerate all possible textual representations of lists of
        //:     simple values up to length 3, parse these representations into
        //:     arrays of the corresponding nullable simple types, and verify
        //:     parsing succeeds and that no elements of the resulting arrays
        //:     are null.
        //:
        //: 2 Repeat the above for textual representations that contain
        //:   insignificant whitespace.
        //
        // Testing:
        //   PARSING LISTS OF NULLABLE SIMPLE TYPES
        // --------------------------------------------------------------------

        if (verbose)
            cout << "\nTESTING PARSING LISTS OF NULLABLE SIMPLE TYPES"
                 << "\n==============================================" << endl;

        const AssertParsedTextIsEqual TEST;

        const s_baltst::TestPlaceHolder<int>            ip;
        const s_baltst::TestPlaceHolder<bsl::string>    sp;
        const s_baltst::GenerateTestArray               a_;
        const s_baltst::GenerateTestNullableValue       n_;
        const s_baltst::GenerateTestNullablePlaceHolder np;

        const bsl::string s("S");

        const int LIST = bdlat_FormattingMode::e_LIST;

        // LINE     TEXT           EXPECTED ARRAY        FMT MODE
        //   -- ------------ --------------------------- --------
        TEST(L_, ""         , a_(ip                    ), LIST   );
        TEST(L_, "1"        , a_( 1                    ), LIST   );
        TEST(L_, "1 1"      , a_( 1, 1                 ), LIST   );
        TEST(L_, "1 1 1"    , a_( 1, 1, 1              ), LIST   );

        TEST(L_, "1"        , a_(n_( 1)                ), LIST   );
        TEST(L_, "1 1"      , a_(n_( 1), n_( 1)        ), LIST   );
        TEST(L_, "1 1 1"    , a_(n_( 1), n_( 1), n_( 1)), LIST   );

        TEST(L_, ""         , a_(sp                    ), LIST   );
        TEST(L_, "S"        , a_( s                    ), LIST   );
        TEST(L_, "S S"      , a_( s, s                 ), LIST   );
        TEST(L_, "S S S"    , a_( s, s, s              ), LIST   );

        TEST(L_, "S"        , a_(n_( s)                ), LIST   );
        TEST(L_, "S S"      , a_(n_( s), n_( s)        ), LIST   );
        TEST(L_, "S S S"    , a_(n_( s), n_( s), n_( s)), LIST   );

        TEST(L_, " 1"       , a_( 1                    ), LIST   );
        TEST(L_, "  1"      , a_( 1                    ), LIST   );
        TEST(L_, "1 "       , a_( 1                    ), LIST   );
        TEST(L_, "1  "      , a_( 1                    ), LIST   );
        TEST(L_, " 1  "     , a_( 1                    ), LIST   );
        TEST(L_, "  1  "    , a_( 1                    ), LIST   );

        TEST(L_, "1 1"      , a_( 1, 1                 ), LIST   );
        TEST(L_, " 1 1"     , a_( 1, 1                 ), LIST   );
        TEST(L_, "  1 1"    , a_( 1, 1                 ), LIST   );
        TEST(L_, "1 1 "     , a_( 1, 1                 ), LIST   );
        TEST(L_, "1 1  "    , a_( 1, 1                 ), LIST   );
        TEST(L_, "1  1"     , a_( 1, 1                 ), LIST   );
        TEST(L_, " 1  1"    , a_( 1, 1                 ), LIST   );
        TEST(L_, "  1  1"   , a_( 1, 1                 ), LIST   );
        TEST(L_, "1  1 "    , a_( 1, 1                 ), LIST   );
        TEST(L_, "1  1  "   , a_( 1, 1                 ), LIST   );
        TEST(L_, " 1  1 "   , a_( 1, 1                 ), LIST   );
        TEST(L_, " 1  1  "  , a_( 1, 1                 ), LIST   );
        TEST(L_, "  1  1 "  , a_( 1, 1                 ), LIST   );
        TEST(L_, "  1  1  " , a_( 1, 1                 ), LIST   );

        TEST(L_, " S"       , a_( s                    ), LIST   );
        TEST(L_, "  S"      , a_( s                    ), LIST   );
        TEST(L_, "S "       , a_( s                    ), LIST   );
        TEST(L_, "S  "      , a_( s                    ), LIST   );
        TEST(L_, " S  "     , a_( s                    ), LIST   );
        TEST(L_, "  S  "    , a_( s                    ), LIST   );

        TEST(L_, "S S"      , a_( s, s                 ), LIST   );
        TEST(L_, " S S"     , a_( s, s                 ), LIST   );
        TEST(L_, "  S S"    , a_( s, s                 ), LIST   );
        TEST(L_, "S S "     , a_( s, s                 ), LIST   );
        TEST(L_, "S S  "    , a_( s, s                 ), LIST   );
        TEST(L_, "S  S"     , a_( s, s                 ), LIST   );
        TEST(L_, " S  S"    , a_( s, s                 ), LIST   );
        TEST(L_, "  S  S"   , a_( s, s                 ), LIST   );
        TEST(L_, "S  S "    , a_( s, s                 ), LIST   );
        TEST(L_, "S  S  "   , a_( s, s                 ), LIST   );
        TEST(L_, " S  S "   , a_( s, s                 ), LIST   );
        TEST(L_, " S  S  "  , a_( s, s                 ), LIST   );
        TEST(L_, "  S  S "  , a_( s, s                 ), LIST   );
        TEST(L_, "  S  S  " , a_( s, s                 ), LIST   );

      } break;
      case 14: {
        // --------------------------------------------------------------------
        // TESTING 'printDefault' ROUND TRIP
        //
        // Concerns:
        //
        // Plan:
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTESTING 'printDefault' ROUND TRIP"
                          << "\n=================================" << endl;

        using TestMachinery::RoundTripTestData;
        using TestMachinery::printDefaultRoundTripTester;

        if (verbose) cout << "\nTesting 'bool'." << endl;
        {
            static const RoundTripTestData<bool> DATA[] =
            {
                //line    input
                //----    -----
                { L_,     true  },
                { L_,     false },
            };
            printDefaultRoundTripTester(DATA);
        }

        if (verbose) cout << "\nTesting 'char'." << endl;
        {
            static const RoundTripTestData<char> DATA[] =
            {
                //line    input
                //----    -----
                { L_,     -128  },
                { L_,     -127  },
                { L_,     -1    },
                { L_,     0     },
                { L_,     1     },
                { L_,     126   },
                { L_,     127   },
            };
            printDefaultRoundTripTester(DATA);
        }

        if (verbose) cout << "\nTesting 'short'." << endl;
        {
            static const RoundTripTestData<short> DATA[] =
            {
                //line    input
                //----    -----
                { L_,     -32768  },
                { L_,     -32767  },
                { L_,     -1      },
                { L_,     0       },
                { L_,     1       },
                { L_,     32766   },
                { L_,     32767   },
            };
            printDefaultRoundTripTester(DATA);
        }

        if (verbose) cout << "\nTesting 'int'." << endl;
        {
            static const RoundTripTestData<int> DATA[] =
            {
                //line    input
                //----    -----
                { L_,     -2147483647 - 1 },
                { L_,     -2147483647     },
                { L_,     -1              },
                { L_,     0               },
                { L_,     1               },
                { L_,     2147483646      },
                { L_,     2147483647      },
            };
            printDefaultRoundTripTester(DATA);
        }

        if (verbose) cout << "\nTesting 'long'." << endl;
        {
            static const RoundTripTestData<long> DATA[] =
            {
                //line  input
                //----  --------------------------
                { L_,   -2147483647 - 1            },
                { L_,   -2147483647                },
                { L_,   -1                         },
                { L_,   0                          },
                { L_,   1                          },
                { L_,   2147483646                 },
                { L_,   2147483647                 },
#ifdef U_LONG_IS_64_BITS
                { L_,   -9223372036854775807LL - 1 },
                { L_,   -9223372036854775807LL     },
                { L_,   9223372036854775806LL      },
                { L_,   9223372036854775807LL      },
#endif
            };
            printDefaultRoundTripTester(DATA);
        }

        if (verbose) cout << "\nTesting 'bsls::Types::Int64'." << endl;
        {
            static const RoundTripTestData<bsls::Types::Int64> DATA[] =
            {
                //line  input
                //----  -----
                { L_,   -9223372036854775807LL - 1 },
                { L_,   -9223372036854775807LL     },
                { L_,   -1LL                       },
                { L_,    0LL                       },
                { L_,    1LL                       },
                { L_,    9223372036854775806LL     },
                { L_,    9223372036854775807LL     },
            };
            printDefaultRoundTripTester(DATA);
        }

        if (verbose) cout << "\nTesting 'unsigned char'." << endl;
        {
            static const RoundTripTestData<unsigned char> DATA[] =
            {
                //line    input
                //----    -----
                { L_,     0     },
                { L_,     1     },
                { L_,     254   },
                { L_,     255   },
            };
            printDefaultRoundTripTester(DATA);
        }

        if (verbose) cout << "\nTesting 'unsigned short'." << endl;
        {

            static const RoundTripTestData<unsigned short> DATA[] =
            {
                    //line    input
                //----    -----
                { L_,     0     },
                { L_,     1     },
                { L_,     65534 },
                { L_,     65535 },
            };
            printDefaultRoundTripTester(DATA);
        }

        if (verbose) cout << "\nTesting 'unsigned int'." << endl;
        {
            static const RoundTripTestData<unsigned int> DATA[] =
            {
                //line    input
                //----    -----------
                { L_,     0           },
                { L_,     1           },
                { L_,     4294967294U },
                { L_,     4294967295U },
            };
            printDefaultRoundTripTester(DATA);
        }

        if (verbose) cout << "\nTesting 'bsls::Types::Uint64'." << endl;
        {
            static const RoundTripTestData<bsls::Types::Uint64> DATA[] =
            {
                //line    input
                //----    -----
                { L_,     0ULL                    },
                { L_,     1ULL                    },
                { L_,     18446744073709551614ULL },
                { L_,     18446744073709551615ULL },
            };
            printDefaultRoundTripTester(DATA);
        }

        if (verbose) cout << "\nTesting 'float'." << endl;
        {
            typedef bsl::numeric_limits<float> NumLimits;

            const float neg0 = copysignf(0.0f, -1.0f);

            static const RoundTripTestData<float> DATA[] =
            {
                //line   input
                //----  --------------------
                { L_,    neg0                },
                { L_,   -1.0f                },
                { L_,   -0.1f                },
                { L_,   -1234567.f           },
                { L_,   -0.1234567f          },
                { L_,   -1.234567e35f        },
                { L_,   -1.234567e-35f       },

                { L_,    0.0f                },
                { L_,    1.0f                },
                { L_,    0.1f                },
                { L_,    1234567.f           },
                { L_,    0.1234567f          },
                { L_,    1.234567e35f        },
                { L_,    1.234567e-35f       },

                // Values from 'balxml_encoder.t.cpp' 'runTestCase17'
                { L_,   1.1920928e-07f       },
                { L_,   2.3841856e-07f       },
                { L_,   1.5258789e-05f       },
                { L_,   2.4414062e-04f       },
                { L_,   3.90625e-03f         },
                { L_,   6.25e-02f            },
                { L_,   5e-1f                },
                { L_,                   1.0f },
                { L_,                1024.0f },
                { L_,            16777216.0f },
                { L_,        137438953472.0f },
                { L_,    1125899906842624.0f },
                { L_,   18014398509481984.0f },

                // {DRQS 165162213} regression, 2^24 loses precision as float
                { L_, 1.0f * 0xFFFFFF        },

                // Full Mantissa Integers
                { L_, 1.0f * 0xFFFFFF   // this is also
                       * (1ull << 63)  // 'NumLimits::max()'
                       * (1ull << 41)        },
                // Boundary Values
                { L_,  NumLimits::min()      },
                { L_,  NumLimits::max()      },
                { L_, -NumLimits::min()      },
                { L_, -NumLimits::max()      },
            };
            printDefaultRoundTripTester(DATA);
        }

        if (verbose) cout << "\nTesting 'double'." << endl;
        {
            typedef bsl::numeric_limits<double> NumLimits;

            const double neg0 = copysign(0.0, -1.0);

            static const RoundTripTestData<double> DATA[] =
            {
                //line             input
                //----  ------------------------------------
                { L_,               neg0                     },
                { L_,                 -1.0                   },
                { L_,                 -0.1                   },
                { L_,                 -0.123456789012345     },
                { L_,                 -1.23456789012345e+105 },
                { L_,                 -1.23456789012345e-105 },

                { L_,                  0.0                   },
                { L_,                  0.1                   },
                { L_,                  1.0                   },
                { L_,                  1.23456789012345e105  },
                { L_,                 123.4567               },

                // Values from 'balxml_encoder.t.cpp' 'runTestCase17'
                { L_,                   1.0                  },
                { L_,                1024.0                  },
                { L_,            16777216.0                  },
                { L_,        137438953472.0                  },
                { L_,    1125899906842624.0                  },
                { L_,   18014398509481984.0                  },

                { L_, 1.1920928955078125e-07                 },
                { L_, 2.384185791015625e-07                  },
                { L_, 1.52587890625e-05                      },
                { L_, 2.44140625e-04                         },
                { L_, 3.90625e-03                            },
                { L_, 6.25e-02                               },
                { L_, 5e-1                                   },

                // Small Integers
                { L_,    123456789012345.                    },
                { L_,   1234567890123456.                    },

                // Full Mantissa Integers
                { L_, 1.0 * 0x1FFFFFFFFFFFFFull              },
                { L_, 1.0 * 0x1FFFFFFFFFFFFFull       // This number is also
                      * (1ull << 63) * (1ull << 63)   // 'NumLimits::max()'
                      * (1ull << 63) * (1ull << 63)
                      * (1ull << 63) * (1ull << 63)
                      * (1ull << 63) * (1ull << 63)
                      * (1ull << 63) * (1ull << 63)
                      * (1ull << 63) * (1ull << 63)
                      * (1ull << 63) * (1ull << 63)
                      * (1ull << 63) * (1ull << 26)          },

                // Boundary Values
                { L_,  NumLimits::min()                      },
                { L_,  NumLimits::max()                      },
                { L_, -NumLimits::min()                      },
                { L_, -NumLimits::max()                      },
            };
            printDefaultRoundTripTester(DATA);
        }

        if (verbose) cout << "\nTesting 'Decimal64'." << endl;
        {
            typedef bdldfp::Decimal64         Type;
            typedef bsl::numeric_limits<Type> Limits;

#define DFP(X) BDLDFP_DECIMAL_DD(X)

            const struct TestData {
                int  d_line;
                Type d_input;
                char d_style;
                int  d_precision;
                bool d_weird;
            } DATA[] = {
//--------------------------------------------------------
// LN  VALUE                      STYLE  PRECISION  WEIRD
//--------------------------------------------------------
{ L_,  DFP(0.0),                    'N',   0,         0 },
{ L_,  DFP(15.13),                  'N',   0,         0 },
{ L_,  DFP(-9.876543210987654e307), 'N',   0,         0 },
{ L_,  DFP(0.001),                  'N',   0,         0 },
{ L_,  DFP(0.01),                   'N',   0,         0 },
{ L_,  DFP(0.1),                    'N',   0,         0 },
{ L_,  DFP(1.),                     'N',   0,         0 },
{ L_,  DFP(1.0),                    'N',   0,         0 },
{ L_,  DFP(1.00),                   'N',   0,         0 },
{ L_,  DFP(1.000),                  'N',   0,         0 },
{ L_,  Limits::max(),               'N',   0,         0 },
{ L_,  -Limits::max(),              'N',   0,         0 },
{ L_,  Limits::min(),               'N',   0,         0 },
{ L_,  -Limits::min(),              'N',   0,         0 },
{ L_,  Limits::infinity(),          'N',   0,         1 },
{ L_, -Limits::infinity(),          'N',   0,         1 },
{ L_,  Limits::signaling_NaN(),     'N',   0,         1 },
{ L_,  Limits::quiet_NaN(),         'N',   0,         1 },

{ L_,  DFP(0.0),                    'F',   2,         0 },
{ L_,  DFP(15.13),                  'F',   2,         0 },
{ L_,  DFP(-9876543210987654.0),    'F',   0,         0 },
{ L_,  DFP(0.001),                  'F',   3,         0 },
{ L_,  DFP(0.001),                  'F',   4,         0 },
{ L_,  DFP(0.01),                   'F',   2,         0 },
{ L_,  DFP(0.01),                   'F',   3,         0 },
{ L_,  DFP(0.1),                    'F',   1,         0 },
{ L_,  DFP(0.1),                    'F',   2,         0 },
{ L_,  DFP(1.),                     'F',   0,         0 },
{ L_,  DFP(1.),                     'F',   0,         0 },
{ L_,  DFP(1.),                     'F',   1,         0 },
{ L_,  DFP(1.0),                    'F',   0,         0 },
{ L_,  DFP(1.0),                    'F',   1,         0 },
{ L_,  DFP(1.0),                    'F',   2,         0 },
{ L_,  DFP(1.00),                   'F',   0,         0 },
{ L_,  DFP(1.00),                   'F',   1,         0 },
{ L_,  DFP(1.00),                   'F',   2,         0 },
{ L_,  DFP(1.00),                   'F',   3,         0 },
{ L_,  DFP(1.000),                  'F',   0,         0 },
{ L_,  DFP(1.000),                  'F',   1,         0 },
{ L_,  DFP(1.000),                  'F',   2,         0 },
{ L_,  DFP(1.000),                  'F',   3,         0 },
{ L_,  DFP(1.000),                  'F',   4,         0 },
{ L_,  Limits::min(),               'F', 383,         0 },
{ L_, -Limits::min(),               'F', 383,         0 },
{ L_,  Limits::infinity(),          'F',   0,         1 },
{ L_, -Limits::infinity(),          'F',   0,         1 },
{ L_,  Limits::signaling_NaN(),     'F',   0,         1 },
{ L_,  Limits::quiet_NaN(),         'F',   0,         1 },

{ L_,  DFP(0.1),                    'S',   0,         0 },
{ L_,  DFP(15.13),                  'S',   3,         0 },
{ L_,  DFP(-9.876543210987654e307), 'S',  15,         0 },
{ L_,  DFP(0.001),                  'S',   0,         0 },
{ L_,  DFP(0.001),                  'S',   1,         0 },
{ L_,  DFP(0.001),                  'S',   2,         0 },
{ L_,  DFP(0.01),                   'S',   0,         0 },
{ L_,  DFP(0.01),                   'S',   1,         0 },
{ L_,  DFP(0.01),                   'S',   2,         0 },
{ L_,  DFP(0.1),                    'S',   0,         0 },
{ L_,  DFP(0.1),                    'S',   1,         0 },
{ L_,  DFP(0.1),                    'S',   2,         0 },
{ L_,  DFP(1.),                     'S',   0,         0 },
{ L_,  DFP(1.),                     'S',   1,         0 },
{ L_,  DFP(1.),                     'S',   2,         0 },
{ L_,  DFP(1.0),                    'S',   0,         0 },
{ L_,  DFP(1.0),                    'S',   1,         0 },
{ L_,  DFP(1.0),                    'S',   2,         0 },
{ L_,  DFP(1.00),                   'S',   0,         0 },
{ L_,  DFP(1.00),                   'S',   1,         0 },
{ L_,  DFP(1.00),                   'S',   2,         0 },
{ L_,  DFP(1.000),                  'S',   0,         0 },
{ L_,  DFP(1.000),                  'S',   1,         0 },
{ L_,  DFP(1.000),                  'S',   2,         0 },
{ L_,  Limits::max(),               'S',  15,         0 },
{ L_, -Limits::max(),               'S',  15,         0 },
{ L_,  Limits::min(),               'S',   0,         0 },
{ L_, -Limits::min(),               'S',   0,         0 },
{ L_,  Limits::infinity(),          'S',   0,         1 },
{ L_, -Limits::infinity(),          'S',   0,         1 },
{ L_,  Limits::signaling_NaN(),     'S',   0,         1 },
{ L_,  Limits::quiet_NaN(),         'S',   0,         1 },
#undef DFP
            };

            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int ti = 0; ti < 2 * NUM_DATA; ++ti) {
                const bool      DECIMAL   = ti % 2;
                const TestData& data      = DATA[ti / 2];
                const int       LINE      = data.d_line;
                const Type      INPUT     = data.d_input;
                const char      STYLE     = data.d_style;
                const int       PRECISION = data.d_precision;
                const bool      WEIRD     = data.d_weird;

                if (DECIMAL && WEIRD) {
                    // SKIP unprintable combinations
                    continue;                                       // CONTINUE
                }

                bsl::stringstream ss;
                ss.precision(PRECISION);
                if ('F' == STYLE) ss << bsl::fixed;
                if ('S' == STYLE) ss << bsl::scientific;

                DECIMAL ? Print::printDecimal(ss, INPUT)
                        : Print::printDefault(ss, INPUT);

                ASSERTV(LINE, false == ss.fail());

                const bsl::string ENCODED(ss.str());
                Type decoded;
                if (DECIMAL) {
                    using TestMachinery::intLength;
                    ASSERTV(LINE, ENCODED, PRECISION, STYLE,
                            0 == Util::parseDecimal(&decoded,
                                                    ENCODED.data(),
                                                    intLength(ENCODED)));
                }
                else {
                    using TestMachinery::intLength;
                    ASSERTV(LINE, ENCODED, PRECISION, STYLE,
                            0 == Util::parseDefault(&decoded,
                                                    ENCODED.data(),
                                                    intLength(ENCODED)));
                }

                if (INPUT != INPUT) { // NaN
                    ASSERTV(LINE, ENCODED, PRECISION, STYLE, INPUT, decoded,
                            decoded != decoded);
                }
                else { // Comparable values
                    ASSERTV(LINE, ENCODED, PRECISION, STYLE, INPUT, decoded,
                            INPUT == decoded);
                }
            }
        }


        if (verbose) cout << "\nTesting 'char *'." << endl;
        {
            static const RoundTripTestData<const char *> DATA[] =
            {
                //line    input
                //----    -----
                { L_,     ""         },
                { L_,     "Hello"    },
                { L_,     "World!!"  },
            };
            printDefaultRoundTripTester(DATA);
        }

        if (verbose) cout << "\nTesting 'bsl::string'." << endl;
        {
            static const RoundTripTestData<bsl::string> DATA[] =
            {
                //line    input
                //----    ------------------
                { L_,     ""                 },
                { L_,     "Hello"            },
                { L_,     "World!!"          },
                { L_,     "&AB"              },
                { L_,     "A&B"              },
                { L_,     "AB&"              },
                { L_,     "<AB"              },
                { L_,     "A<B"              },
                { L_,     "AB<"              },
                { L_,     ">AB"              },
                { L_,     "A>B"              },
                { L_,     "AB>"              },
                { L_,     "\'AB"             },
                { L_,     "A\'B"             },
                { L_,     "AB\'"             },
                { L_,     "\"AB"             },
                { L_,     "A\"B"             },
                { L_,     "AB\""             },
                { L_,     "\xC3\xB6" "AB"    },
                { L_,     "A" "\xC3\xB6" "B" },
                { L_,     "AB" "\xC3\xB6"    },
            };
            printDefaultRoundTripTester(DATA);
        }

        if (verbose) cout << "\nTesting 'bslstl::StringRef'." << endl;
        {
            static const RoundTripTestData<bslstl::StringRef> DATA[] =
            {
                //line    input
                //----    ------------------
                { L_,     ""                 },
                { L_,     "Hello"            },
                { L_,     "World!!"          },
                { L_,     "&AB"              },
                { L_,     "A&B"              },
                { L_,     "AB&"              },
                { L_,     "<AB"              },
                { L_,     "A<B"              },
                { L_,     "AB<"              },
                { L_,     ">AB"              },
                { L_,     "A>B"              },
                { L_,     "AB>"              },
                { L_,     "\'AB"             },
                { L_,     "A\'B"             },
                { L_,     "AB\'"             },
                { L_,     "\"AB"             },
                { L_,     "A\"B"             },
                { L_,     "AB\""             },
                { L_,     "\xC3\xB6" "AB"    },
                { L_,     "A" "\xC3\xB6" "B" },
                { L_,     "AB" "\xC3\xB6"    },
            };
            printDefaultRoundTripTester(DATA);
        }

        if (verbose) cout << "\nTesting 'MyEnumeration::Value'." << endl;
        {
            static const RoundTripTestData<test::MyEnumeration::Value> DATA[] =
            {
                //line    input
                //----    -----
                { L_,     test::MyEnumeration::VALUE1 },
                { L_,     test::MyEnumeration::VALUE2 },
            };
            printDefaultRoundTripTester(DATA);
        }

        if (verbose) cout << "\nTesting 'MyEnumerationWithFallback::Value'."
                          << endl;
        {
            typedef test::MyEnumerationWithFallback::Value Obj;
            static const RoundTripTestData<Obj> DATA[] =
            {
                //line    input
                //----    -----
                { L_,     test::MyEnumerationWithFallback::VALUE1  },
                { L_,     test::MyEnumerationWithFallback::VALUE2  },
                { L_,     test::MyEnumerationWithFallback::UNKNOWN },
            };
            printDefaultRoundTripTester(DATA);
        }

        if (verbose) cout << "\nTesting 'CustomizedString'." << endl;
        {
            typedef test::CustomizedString Type;

            static const RoundTripTestData<Type> DATA[] =
            {
                //line    input
                //----    -----
                { L_,     Type("")      },
                { L_,     Type("a")     },
                { L_,     Type("ab")    },
                { L_,     Type("abc")   },
                { L_,     Type("abcd")  },
                { L_,     Type("abcde") },
            };
            printDefaultRoundTripTester(DATA);
        }

        if (verbose) cout << "\nTesting 'CustomizedInt'." << endl;
        {
            typedef test::CustomizedInt Type;

            static const RoundTripTestData<Type> DATA[] =
            {
                //line    input
                //----    -----
                { L_,     Type(-2147483647-1) },
                { L_,     Type(-2147483647)   },
                { L_,     Type(-1)            },
                { L_,     Type(0)             },
                { L_,     Type(1)             },
                { L_,     Type(2)             },
                { L_,     Type(3)             },
                { L_,     Type(4)             },
                { L_,     Type(5)             },
            };
            printDefaultRoundTripTester(DATA);
        }

        if (verbose) cout << "\nTesting date and time types." << endl;
        {
            static const struct TestData {
                int d_line;
                int d_year;
                int d_month;
                int d_day;
                int d_hour;
                int d_minute;
                int d_second;
                int d_millisecond;
                int d_microsecond;
                int d_offset;
            } DATA[] = {
                //Line Year   Mon  Day  Hour  Min  Sec     ms   us   offset
                //---- ----   ---  ---  ----  ---  ---     --   --   ------

                // Valid dates and times
                { L_,     1,   1,   1,    0,   0,   0,     0,    0,      0 },
                { L_,  2005,   1,   1,    0,   0,   0,     0,    0,    -90 },
                { L_,   123,   6,  15,   13,  40,  59,     0,    0,   -240 },
                { L_,  1999,  10,  12,   23,   0,   1,     0,    0,   -720 },

                // Vary milliseconds
                { L_,  1999,  10,  12,   23,   0,   1,     0,    0,     90 },
                { L_,  1999,  10,  12,   23,   0,   1,   456,    0,    240 },
                { L_,  1999,  10,  12,   23,   0,   1,   456,  789,    240 },
                { L_,  1999,  10,  12,   23,   0,   1,   999,  789,    720 },
                { L_,  1999,  12,  31,   23,  59,  59,   999,  999,    720 }
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int LINE        = DATA[ti].d_line;
                const int YEAR        = DATA[ti].d_year;
                const int MONTH       = DATA[ti].d_month;
                const int DAY         = DATA[ti].d_day;
                const int HOUR        = DATA[ti].d_hour;
                const int MINUTE      = DATA[ti].d_minute;
                const int SECOND      = DATA[ti].d_second;
                const int MILLISECOND = DATA[ti].d_millisecond;
                const int MICROSECOND = DATA[ti].d_microsecond;
                const int OFFSET      = DATA[ti].d_offset;;

                const bdlt::Date       IN_DATE(YEAR, MONTH, DAY);
                const bdlt::Time       IN_TIME(HOUR, MINUTE, SECOND,
                                               MILLISECOND);
                const bdlt::Datetime   IN_DATETIME(YEAR, MONTH, DAY,
                                                   HOUR, MINUTE, SECOND,
                                                   MILLISECOND, MICROSECOND);

                const bdlt::DateTz     IN_DATETZ(IN_DATE, OFFSET);
                const bdlt::TimeTz     IN_TIMETZ(IN_TIME, OFFSET);
                const bdlt::DatetimeTz IN_DATETIMETZ(IN_DATETIME, OFFSET);

                using TestMachinery::intLength;

                {
                    bsl::ostringstream ss;
                    Print::printDefault(ss, IN_DATE);
                    ASSERTV(LINE, ss.good());

                    const bsl::string ENCODED(ss.str());
                    bdlt::Date decoded;
                    ASSERTV(LINE, ENCODED,
                            0 == Util::parseDefault(&decoded,
                                                    ENCODED.data(),
                                                    intLength(ENCODED)));
                    ASSERTV(LINE, ENCODED, IN_DATE, decoded,
                            IN_DATE == decoded);
                }

                {
                    bsl::ostringstream ss;
                    Print::printDefault(ss, IN_DATETZ);
                    ASSERTV(LINE, ss.good());

                    const bsl::string ENCODED(ss.str());
                    bdlt::DateTz decoded;
                    ASSERTV(LINE, ENCODED,
                            0 == Util::parseDefault(&decoded,
                                                    ENCODED.data(),
                                                    intLength(ENCODED)));
                    ASSERTV(LINE, ENCODED, IN_DATETZ, decoded,
                            IN_DATETZ == decoded);
                }

                {
                    balxml::EncoderOptions options;
                    options.setUseZAbbreviationForUtc(true);

                    bsl::ostringstream ss;
                    Print::printDefault(ss, IN_DATETZ, &options);
                    ASSERTV(LINE, ss.good());

                    const bsl::string ENCODED(ss.str());
                    bdlt::DateTz decoded;
                    ASSERTV(LINE, ENCODED,
                            0 == Util::parseDefault(&decoded,
                                                    ENCODED.data(),
                                                    intLength(ENCODED)));
                    ASSERTV(LINE, ENCODED, IN_DATETZ, decoded,
                            IN_DATETZ == decoded);
                }

                {
                    bsl::ostringstream ss;
                    Print::printDefault(ss, IN_TIME);
                    ASSERTV(LINE, ss.good());

                    const bsl::string ENCODED(ss.str());
                    bdlt::Time decoded;
                    ASSERTV(LINE, ENCODED,
                            0 == Util::parseDefault(&decoded,
                                                    ENCODED.data(),
                                                    intLength(ENCODED)));
                    ASSERTV(LINE, ENCODED, IN_TIME, decoded,
                            IN_TIME == decoded);
                }

                {
                    bsl::ostringstream ss;
                    Print::printDefault(ss, IN_TIMETZ);
                    ASSERTV(LINE, ss.good());

                    const bsl::string ENCODED(ss.str());
                    bdlt::TimeTz decoded;
                    ASSERTV(LINE, ENCODED,
                            0 == Util::parseDefault(&decoded,
                                                    ENCODED.data(),
                                                    intLength(ENCODED)));
                    ASSERTV(LINE, ENCODED, IN_TIMETZ, decoded,
                            IN_TIMETZ == decoded);
                }

                {
                    balxml::EncoderOptions options;
                    options.setUseZAbbreviationForUtc(true);

                    bsl::ostringstream ss;
                    Print::printDefault(ss, IN_TIMETZ, &options);
                    ASSERTV(LINE, ss.good());

                    const bsl::string ENCODED(ss.str());
                    bdlt::TimeTz decoded;
                    ASSERTV(LINE, ENCODED,
                            0 == Util::parseDefault(&decoded,
                                                    ENCODED.data(),
                                                    intLength(ENCODED)));
                    ASSERTV(LINE, ENCODED, IN_TIMETZ, decoded,
                            IN_TIMETZ == decoded);
                }

                {
                    bsl::ostringstream ss;
                    Print::printDefault(ss, IN_DATETIME);
                    ASSERTV(LINE, ss.good());

                    const bsl::string ENCODED(ss.str());
                    bdlt::Datetime decoded;
                    ASSERTV(LINE, ENCODED,
                            0 == Util::parseDefault(&decoded,
                                                    ENCODED.data(),
                                                    intLength(ENCODED)));
                    ASSERTV(LINE, ENCODED, IN_DATETIME, decoded,
                            IN_DATETIME == decoded);
                }

                {
                    bsl::ostringstream ss;
                    Print::printDefault(ss, IN_DATETIMETZ);
                    ASSERTV(LINE, ss.good());

                    const bsl::string ENCODED(ss.str());
                    bdlt::DatetimeTz decoded;
                    ASSERTV(LINE, ENCODED,
                            0 == Util::parseDefault(&decoded,
                                                    ENCODED.data(),
                                                    intLength(ENCODED)));
                    ASSERTV(LINE, ENCODED, IN_DATETIMETZ, decoded,
                            IN_DATETIMETZ == decoded);
                }

                {
                    balxml::EncoderOptions options;
                    options.setUseZAbbreviationForUtc(true);

                    bsl::ostringstream ss;
                    Print::printDefault(ss, IN_DATETIMETZ, &options);
                    ASSERTV(LINE, ss.good());

                    const bsl::string ENCODED(ss.str());
                    bdlt::DatetimeTz decoded;
                    ASSERTV(LINE, ENCODED,
                            0 == Util::parseDefault(&decoded,
                                                    ENCODED.data(),
                                                    intLength(ENCODED)));
                    ASSERTV(LINE, ENCODED, IN_DATETIMETZ, decoded,
                            IN_DATETIMETZ == decoded);
                }

                {
                    bsl::ostringstream ss;
                    Print::printDefault(ss, IN_DATE);
                    ASSERTV(LINE, ss.good());

                    const bsl::string ENCODED(ss.str());

                    bdlb::Variant2<bdlt::Date, bdlt::DateTz> decoded;
                    ASSERTV(LINE, ENCODED,
                            0 == Util::parseDefault(&decoded,
                                                    ENCODED.data(),
                                                    intLength(ENCODED)));
                    ASSERTV(LINE, decoded.is<bdlt::Date>())
                    ASSERTV(LINE, ENCODED, IN_DATE, decoded,
                            IN_DATE == decoded.the<bdlt::Date>());
                }

                {
                    bsl::ostringstream ss;
                    Print::printDefault(ss, IN_DATETZ);
                    ASSERTV(LINE, ss.good());

                    const bsl::string ENCODED(ss.str());

                    bdlb::Variant2<bdlt::Date, bdlt::DateTz> decoded;
                    ASSERTV(LINE, ENCODED,
                            0 == Util::parseDefault(&decoded,
                                                    ENCODED.data(),
                                                    intLength(ENCODED)));
                    ASSERTV(LINE, decoded.is<bdlt::DateTz>())
                    ASSERTV(LINE, ENCODED, IN_DATETZ, decoded,
                            IN_DATETZ == decoded.the<bdlt::DateTz>());
                }

                {
                    balxml::EncoderOptions options;
                    options.setUseZAbbreviationForUtc(true);

                    bsl::ostringstream ss;
                    Print::printDefault(ss, IN_DATETZ, &options);
                    ASSERTV(LINE, ss.good());

                    const bsl::string ENCODED(ss.str());

                    bdlb::Variant2<bdlt::Date, bdlt::DateTz> decoded;
                    ASSERTV(LINE, ENCODED,
                            0 == Util::parseDefault(&decoded,
                                                    ENCODED.data(),
                                                    intLength(ENCODED)));
                    ASSERTV(LINE, decoded.is<bdlt::DateTz>())
                    ASSERTV(LINE, ENCODED, IN_DATETZ, decoded,
                            IN_DATETZ == decoded.the<bdlt::DateTz>());
                }

                {
                    bsl::ostringstream ss;
                    Print::printDefault(ss, IN_TIME);
                    ASSERTV(LINE, ss.good());

                    const bsl::string ENCODED(ss.str());

                    bdlb::Variant2<bdlt::Time, bdlt::TimeTz> decoded;
                    ASSERTV(LINE, ENCODED,
                            0 == Util::parseDefault(&decoded,
                                                    ENCODED.data(),
                                                    intLength(ENCODED)));
                    ASSERTV(LINE, decoded.is<bdlt::Time>())
                    ASSERTV(LINE, ENCODED, IN_TIME, decoded,
                            IN_TIME == decoded.the<bdlt::Time>());
                }

                {
                    bsl::ostringstream ss;
                    Print::printDefault(ss, IN_TIMETZ);
                    ASSERTV(LINE, ss.good());

                    const bsl::string ENCODED(ss.str());

                    bdlb::Variant2<bdlt::Time, bdlt::TimeTz> decoded;
                    ASSERTV(LINE, ENCODED,
                            0 == Util::parseDefault(&decoded,
                                                    ENCODED.data(),
                                                    intLength(ENCODED)));
                    ASSERTV(LINE, decoded.is<bdlt::TimeTz>())
                    ASSERTV(LINE, ENCODED, IN_TIMETZ, decoded,
                            IN_TIMETZ == decoded.the<bdlt::TimeTz>());
                }

                {
                    balxml::EncoderOptions options;
                    options.setUseZAbbreviationForUtc(true);

                    bsl::ostringstream ss;
                    Print::printDefault(ss, IN_TIMETZ, &options);
                    ASSERTV(LINE, ss.good());

                    const bsl::string ENCODED(ss.str());

                    bdlb::Variant2<bdlt::Time, bdlt::TimeTz> decoded;
                    ASSERTV(LINE, ENCODED,
                            0 == Util::parseDefault(&decoded,
                                                    ENCODED.data(),
                                                    intLength(ENCODED)));
                    ASSERTV(LINE, decoded.is<bdlt::TimeTz>())
                    ASSERTV(LINE, ENCODED, IN_TIMETZ, decoded,
                            IN_TIMETZ == decoded.the<bdlt::TimeTz>());
                }

                {
                    bsl::ostringstream ss;
                    Print::printDefault(ss, IN_DATETIME);
                    ASSERTV(LINE, ss.good());

                    const bsl::string ENCODED(ss.str());

                    bdlb::Variant2<bdlt::Datetime, bdlt::DatetimeTz> decoded;
                    ASSERTV(LINE, ENCODED,
                            0 == Util::parseDefault(&decoded,
                                                    ENCODED.data(),
                                                    intLength(ENCODED)));
                    ASSERTV(LINE, decoded.is<bdlt::Datetime>())
                    ASSERTV(LINE, ENCODED, IN_DATETIME, decoded,
                            IN_DATETIME == decoded.the<bdlt::Datetime>());
                }

                {
                    bsl::ostringstream ss;
                    Print::printDefault(ss, IN_DATETIMETZ);
                    ASSERTV(LINE, ss.good());

                    const bsl::string ENCODED(ss.str());

                    bdlb::Variant2<bdlt::Datetime, bdlt::DatetimeTz> decoded;
                    ASSERTV(LINE, ENCODED,
                            0 == Util::parseDefault(&decoded,
                                                    ENCODED.data(),
                                                    intLength(ENCODED)));
                    ASSERTV(LINE, decoded.is<bdlt::DatetimeTz>())
                    ASSERTV(LINE, ENCODED, IN_DATETIMETZ, decoded,
                            IN_DATETIMETZ == decoded.the<bdlt::DatetimeTz>());
                }

                {
                    balxml::EncoderOptions options;
                    options.setUseZAbbreviationForUtc(true);

                    bsl::ostringstream ss;
                    Print::printDefault(ss, IN_DATETIMETZ, &options);
                    ASSERTV(LINE, ss.good());

                    const bsl::string ENCODED(ss.str());

                    bdlb::Variant2<bdlt::Datetime, bdlt::DatetimeTz> decoded;
                    ASSERTV(LINE, ENCODED,
                            0 == Util::parseDefault(&decoded,
                                                    ENCODED.data(),
                                                    intLength(ENCODED)));
                    ASSERTV(LINE, decoded.is<bdlt::DatetimeTz>())
                    ASSERTV(LINE, ENCODED, IN_DATETIMETZ, decoded,
                            IN_DATETIMETZ == decoded.the<bdlt::DatetimeTz>());
                }
            }
        }
      } break;
      case 13: {
        // --------------------------------------------------------------------
        // TESTING 'printText' ROUND TRIP
        //
        // Concerns:
        //: 1 That booleans and enumerations are printed as text properly.
        //:
        //: 2 That types that have the CustomizedString trait are printed
        //:   correctly.
        //:
        //: 3 That invalid characters (single- and multi-bytes) are not
        //:   printed.
        //:
        //: 4 That valid input are printed correctly, and strings with
        //:   invalid characters printed until the first invalid character.
        //
        // Plan:
        //   Exercise 'printText' with typical data to ascertain that it prints
        //   as expected for concerns 1 and 2.  For concerns 3 and 4, select
        //   test data with the boundary and area testing methods, and verify
        //   that output is as expected.  Note that since we are in effect
        //   testing the internal method 'printTextReplacingXMLEscapes', there
        //   is no need to test it for several types, since it is called on the
        //   internal text buffer.  For this reason, we test only with 'char*'
        //   data thoroughly, and trust that the forwarding for 'bsl::string'
        //   and other string types will call the same method.
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTESTING 'printText' ROUND TRIP"
                          << "\n==============================" << endl;

        using TestMachinery::RoundTripTestData;
        using TestMachinery::printTextRoundTripScalarTester;

        if (verbose) cout << "\nTesting 'bool'." << endl;
        {
            static const RoundTripTestData<bool> DATA[] =
            {
                //line    input
                //----    -----
                { L_,     true  },
                { L_,     false },
            };
            printTextRoundTripScalarTester(DATA);
        }

        if (verbose) cout << "\nTesting 'char'." << endl;
        {
            static const RoundTripTestData<char> DATA[] =
            {
                //line    input
                //----    -----
                { L_,     0x09  },
                { L_,     0x0a  },
                { L_,     0x0d  },
                { L_,     0x20  },
                { L_,     0x21  },
                { L_,     0x22  },
                { L_,     0x23  },
                { L_,     0x24  },
                { L_,     0x25  },
                { L_,     0x26  },
                { L_,     0x27  },
                { L_,     0x28  },
                { L_,     0x29  },
                { L_,     0x2a  },
                { L_,     0x2b  },
                { L_,     0x2c  },
                { L_,     0x2d  },
                { L_,     0x2e  },
                { L_,     0x2f  },
                { L_,     0x30  },
                { L_,     0x31  },
                { L_,     0x32  },
                { L_,     0x33  },
                { L_,     0x34  },
                { L_,     0x35  },
                { L_,     0x36  },
                { L_,     0x37  },
                { L_,     0x38  },
                { L_,     0x39  },
                { L_,     0x3a  },
                { L_,     0x3b  },
                { L_,     0x3c  },
                { L_,     0x3d  },
                { L_,     0x3e  },
                { L_,     0x3f  },
                { L_,     0x40  },
                { L_,     0x41  },
                { L_,     'A'   },
                // treat all uppercase (0x41 until 0x5a) as a region, and only
                // test the boundaries.
                { L_,     'Z'   },
                { L_,     0x5a  },
                { L_,     0x5b  },
                { L_,     0x5c  },
                { L_,     0x5d  },
                { L_,     0x5e  },
                { L_,     0x5f  },
                { L_,     0x60  },
                { L_,     0x61  },
                { L_,     'a'   },
                // treat all lowercase (0x61 until 0x7a) as a region, and only
                // test the boundaries.
                { L_,     'z'   },
                { L_,     0x7a  },
                { L_,     0x7b  },
                { L_,     0x7c  },
                { L_,     0x7d  },
                { L_,     0x7e  },
            };
            printTextRoundTripScalarTester(DATA);
        }

        // Note that we have already tested the valid single byte characters
        // individually, so the only concerns here are multibyte character
        // strings and boundary conditions (empty string).  We follow the
        // boundary and area selection methods, by picking the boundary values
        // and a value in between at random, for each range, and taking the
        // cross product of all these values for making sure that all boundary
        // faces (in a hypercube) are covered.

        {
            static const struct TestData {
                int         d_line;
                const char *d_input;
            } DATA[] = {
                //line input
                //---- -----
                { L_,  ""                       },

                { L_,  "\x09"                   },
                { L_,  "\x0a"                   },
                { L_,  "\x0d"                   },
                { L_,  "\x22"                   },
                { L_,  "\x26"                   },
                { L_,  "\x27"                   },
                { L_,  "\x3c"                   },
                { L_,  "\x3e"                   },

                { L_,  "Hello"                  },
                { L_,  "Hello World!!"          },
                { L_,  "Hello \t World"         },
                { L_,  "Hello \n World"         },
                { L_,  "Hello \x0d World"       },
                { L_,  "Pi is < 3.15"           },
                { L_,  "Pi is > 3.14"           },
                { L_,  "Tom & Jerry"            },
                { L_,  "'Hello' World!"         },
                { L_,  "Hello \"World\""        },

                { L_,  "<![CDATA&]]>"           },
                { L_,  "![CDATA[&]]>"           },
                { L_,  "<![CDATA[Hello]]>World" },

                // Two-byte character sequences.

                { L_,  "\xc2\x80"               },
                { L_,  "\xc2\xa3"               },
                { L_,  "\xc2\xbf"               },
                { L_,  "\xd0\x80"               },
                { L_,  "\xd0\x9d"               },
                { L_,  "\xd0\xbf"               },
                { L_,  "\xdf\x80"               },
                { L_,  "\xdf\xa6"               },
                { L_,  "\xdf\xbf"               },

                // Three-byte character sequences.

                // Note that first byte 0xe0 is special (second byte ranges in
                // 0xa0..0xbf instead of the usual 0x80..0xbf).  Note also that
                // first byte 0xed is special (second byte ranges in 0x80..0x9f
                // instead of the usual 0x80..0xbf).

                { L_,  "\xe0\xa0\x80"           },
                { L_,  "\xe0\xa0\xa3"           },
                { L_,  "\xe0\xa0\xbf"           },
                { L_,  "\xe0\xb5\x80"           },
                { L_,  "\xe0\xab\x9d"           },
                { L_,  "\xe0\xa9\xbf"           },
                { L_,  "\xe0\xbf\x80"           },
                { L_,  "\xe0\xbf\xaf"           },
                { L_,  "\xe0\xbf\xbf"           },

                { L_,  "\xe1\x80\x80"           },
                { L_,  "\xe1\x80\xa3"           },
                { L_,  "\xe1\x80\xbf"           },
                { L_,  "\xe1\x9a\x80"           },
                { L_,  "\xe1\x85\x9d"           },
                { L_,  "\xe1\xab\xbf"           },
                { L_,  "\xe1\xbf\x80"           },
                { L_,  "\xe1\xbf\xa6"           },
                { L_,  "\xe1\xbf\xbf"           },

                { L_,  "\xe7\x80\x80"           },
                { L_,  "\xe7\x80\xa3"           },
                { L_,  "\xe7\x80\xbf"           },
                { L_,  "\xe7\x9a\x80"           },
                { L_,  "\xe7\x85\x9d"           },
                { L_,  "\xe7\xab\xbf"           },
                { L_,  "\xe7\xbf\x80"           },
                { L_,  "\xe7\xbf\xa6"           },
                { L_,  "\xe7\xbf\xbf"           },

                { L_,  "\xec\x80\x80"           },
                { L_,  "\xec\x80\xa3"           },
                { L_,  "\xec\x80\xbf"           },
                { L_,  "\xec\x9a\x80"           },
                { L_,  "\xec\xab\x9d"           },
                { L_,  "\xec\xb3\xbf"           },
                { L_,  "\xec\xbf\x80"           },
                { L_,  "\xec\xbf\x98"           },
                { L_,  "\xec\xbf\xbf"           },

                { L_,  "\xed\x80\x80"           },
                { L_,  "\xed\x80\x83"           },
                { L_,  "\xed\x80\x9f"           },
                { L_,  "\xed\x9a\x80"           },
                { L_,  "\xed\x8b\x9d"           },
                { L_,  "\xed\x93\xbf"           },
                { L_,  "\xed\x9f\x80"           },
                { L_,  "\xed\x9f\x98"           },
                { L_,  "\xed\x9f\xbf"           },

                { L_,  "\xee\x80\x80"           },
                { L_,  "\xee\x80\xa3"           },
                { L_,  "\xee\x80\xbf"           },
                { L_,  "\xee\x9a\x80"           },
                { L_,  "\xee\x85\x9d"           },
                { L_,  "\xee\xab\xbf"           },
                { L_,  "\xee\xbf\x80"           },
                { L_,  "\xee\xbf\xa6"           },
                { L_,  "\xee\xbf\xbf"           },

                { L_,  "\xef\x80\x80"           },
                { L_,  "\xef\x80\xa3"           },
                { L_,  "\xef\x80\xbf"           },
                { L_,  "\xef\x9a\x80"           },
                { L_,  "\xef\xab\x9d"           },
                { L_,  "\xef\xb3\xbf"           },
                { L_,  "\xef\xbf\x80"           },
                { L_,  "\xef\xbf\x98"           },
                { L_,  "\xef\xbf\xbf"           },

                // Four-byte character sequences.

                // Note that first byte 0xf0 is special (second byte ranges in
                // 0x90..0xbf instead of the usual 0x80..0xbf).  Note also that
                // first byte 0xf4 is special (second byte ranges in 0x80..0x8f
                // instead of the usual 0x80..0xbf).

                { L_,  "\xf0\x90\x80\x80"       },
                { L_,  "\xf0\x90\x80\x98"       },
                { L_,  "\xf0\x90\x80\xbf"       },
                { L_,  "\xf0\x90\xa7\x80"       },
                { L_,  "\xf0\x90\x95\xa6"       },
                { L_,  "\xf0\x90\xa5\xbf"       },
                { L_,  "\xf0\x90\xbf\x80"       },
                { L_,  "\xf0\x90\xbf\xa2"       },
                { L_,  "\xf0\x90\xbf\xbf"       },
                { L_,  "\xf0\xa1\x80\x80"       },
                { L_,  "\xf0\xa2\x80\x85"       },
                { L_,  "\xf0\xa5\x80\xbf"       },
                { L_,  "\xf0\xa9\xa3\x80"       },
                { L_,  "\xf0\x93\xa3\xa6"       },
                { L_,  "\xf0\x95\xa3\xbf"       },
                { L_,  "\xf0\x98\xbf\x80"       },
                { L_,  "\xf0\x9d\xbf\xa6"       },
                { L_,  "\xf0\x9f\xbf\xbf"       },
                { L_,  "\xf0\xbf\x80\x80"       },
                { L_,  "\xf0\xbf\x80\xa6"       },
                { L_,  "\xf0\xbf\x80\xbf"       },
                { L_,  "\xf0\xbf\xa3\x80"       },
                { L_,  "\xf0\xbf\xa3\xa6"       },
                { L_,  "\xf0\xbf\xa3\xbf"       },
                { L_,  "\xf0\xbf\xbf\x80"       },
                { L_,  "\xf0\xbf\xbf\xa6"       },
                { L_,  "\xf0\xbf\xbf\xbf"       },

                { L_,  "\xf1\x80\x80\x80"       },
                { L_,  "\xf1\x80\x80\x98"       },
                { L_,  "\xf1\x80\x80\xbf"       },
                { L_,  "\xf1\x80\xa7\x80"       },
                { L_,  "\xf1\x80\x95\xa6"       },
                { L_,  "\xf1\x80\xa5\xbf"       },
                { L_,  "\xf1\x80\xbf\x80"       },
                { L_,  "\xf1\x80\xbf\xa2"       },
                { L_,  "\xf1\x80\xbf\xbf"       },
                { L_,  "\xf1\x81\x80\x80"       },
                { L_,  "\xf1\xa2\x80\x85"       },
                { L_,  "\xf1\x85\x80\xbf"       },
                { L_,  "\xf1\xa9\xa3\x80"       },
                { L_,  "\xf1\x93\xa3\xa6"       },
                { L_,  "\xf1\x85\xa3\xbf"       },
                { L_,  "\xf1\x98\xbf\x80"       },
                { L_,  "\xf1\x9d\xbf\xa6"       },
                { L_,  "\xf1\x9f\xbf\xbf"       },
                { L_,  "\xf1\xbf\x80\x80"       },
                { L_,  "\xf1\xbf\x80\xa6"       },
                { L_,  "\xf1\xbf\x80\xbf"       },
                { L_,  "\xf1\xbf\xa3\x80"       },
                { L_,  "\xf1\xbf\xa3\xa6"       },
                { L_,  "\xf1\xbf\xa3\xbf"       },
                { L_,  "\xf1\xbf\xbf\x80"       },
                { L_,  "\xf1\xbf\xbf\xa6"       },
                { L_,  "\xf1\xbf\xbf\xbf"       },

                { L_,  "\xf2\x80\x80\x80"       },
                { L_,  "\xf2\x80\x80\x98"       },
                { L_,  "\xf2\x80\x80\xbf"       },
                { L_,  "\xf2\x80\xa7\x80"       },
                { L_,  "\xf2\x80\x95\xa6"       },
                { L_,  "\xf2\x80\xa5\xbf"       },
                { L_,  "\xf2\x80\xbf\x80"       },
                { L_,  "\xf2\x80\xbf\xa2"       },
                { L_,  "\xf2\x80\xbf\xbf"       },
                { L_,  "\xf2\x81\x80\x80"       },
                { L_,  "\xf2\xa2\x80\x85"       },
                { L_,  "\xf2\x85\x80\xbf"       },
                { L_,  "\xf2\xa9\xa3\x80"       },
                { L_,  "\xf2\x93\xa3\xa6"       },
                { L_,  "\xf2\x85\xa3\xbf"       },
                { L_,  "\xf2\x98\xbf\x80"       },
                { L_,  "\xf2\x9d\xbf\xa6"       },
                { L_,  "\xf2\x9f\xbf\xbf"       },
                { L_,  "\xf2\xbf\x80\x80"       },
                { L_,  "\xf2\xbf\x80\xa6"       },
                { L_,  "\xf2\xbf\x80\xbf"       },
                { L_,  "\xf2\xbf\xa3\x80"       },
                { L_,  "\xf2\xbf\xa3\xa6"       },
                { L_,  "\xf2\xbf\xa3\xbf"       },
                { L_,  "\xf2\xbf\xbf\x80"       },
                { L_,  "\xf2\xbf\xbf\xa6"       },
                { L_,  "\xf2\xbf\xbf\xbf"       },

                { L_,  "\xf3\x80\x80\x80"       },
                { L_,  "\xf3\x80\x80\x98"       },
                { L_,  "\xf3\x80\x80\xbf"       },
                { L_,  "\xf3\x80\xa7\x80"       },
                { L_,  "\xf3\x80\x95\xa6"       },
                { L_,  "\xf3\x80\xa5\xbf"       },
                { L_,  "\xf3\x80\xbf\x80"       },
                { L_,  "\xf3\x80\xbf\xa2"       },
                { L_,  "\xf3\x80\xbf\xbf"       },
                { L_,  "\xf3\x81\x80\x80"       },
                { L_,  "\xf3\xa2\x80\x85"       },
                { L_,  "\xf3\x85\x80\xbf"       },
                { L_,  "\xf3\xa9\xa3\x80"       },
                { L_,  "\xf3\x93\xa3\xa6"       },
                { L_,  "\xf3\x85\xa3\xbf"       },
                { L_,  "\xf3\x98\xbf\x80"       },
                { L_,  "\xf3\x9d\xbf\xa6"       },
                { L_,  "\xf3\x9f\xbf\xbf"       },
                { L_,  "\xf3\xbf\x80\x80"       },
                { L_,  "\xf3\xbf\x80\xa6"       },
                { L_,  "\xf3\xbf\x80\xbf"       },
                { L_,  "\xf3\xbf\xa3\x80"       },
                { L_,  "\xf3\xbf\xa3\xa6"       },
                { L_,  "\xf3\xbf\xa3\xbf"       },
                { L_,  "\xf3\xbf\xbf\x80"       },
                { L_,  "\xf3\xbf\xbf\xa6"       },
                { L_,  "\xf3\xbf\xbf\xbf"       },

                { L_,  "\xf4\x80\x80\x80"       },
                { L_,  "\xf4\x80\x80\x98"       },
                { L_,  "\xf4\x80\x80\xbf"       },
                { L_,  "\xf4\x80\xa7\x80"       },
                { L_,  "\xf4\x80\x95\xa6"       },
                { L_,  "\xf4\x80\xa5\xbf"       },
                { L_,  "\xf4\x80\xbf\x80"       },
                { L_,  "\xf4\x80\xbf\xa2"       },
                { L_,  "\xf4\x80\xbf\xbf"       },
                { L_,  "\xf4\x81\x80\x80"       },
                { L_,  "\xf4\x82\x80\x85"       },
                { L_,  "\xf4\x85\x80\xbf"       },
                { L_,  "\xf4\x89\xa3\x80"       },
                { L_,  "\xf4\x83\xa3\xa6"       },
                { L_,  "\xf4\x85\xa3\xbf"       },
                { L_,  "\xf4\x88\xbf\x80"       },
                { L_,  "\xf4\x8d\xbf\xa6"       },
                { L_,  "\xf4\x8e\xbf\xbf"       },
                { L_,  "\xf4\x8f\x80\x80"       },
                { L_,  "\xf4\x8f\x80\xa6"       },
                { L_,  "\xf4\x8f\x80\xbf"       },
                { L_,  "\xf4\x8f\xa3\x80"       },
                { L_,  "\xf4\x8f\xa3\xa6"       },
                { L_,  "\xf4\x8f\xa3\xbf"       },
                { L_,  "\xf4\x8f\xbf\x80"       },
                { L_,  "\xf4\x8f\xbf\xa6"       },
                { L_,  "\xf4\x8f\xbf\xbf"       },

            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            using TestMachinery::buildPrintTextInput;
            using TestMachinery::replaceCharEntities;

            // Use orthogonal perturbations for making sure that printing is
            // correct regardless of position in the string: Use header,
            // trailer, or both.

            const bsl::string_view HEADER("HeAdEr");
            const bsl::string_view TRAILER("TrAiLeR");

            using TestMachinery::intLength;

            if (verbose) cout << "\nTesting 'bsl::string'." << endl;
            {
                typedef bsl::string Type;

                for (int i = 0; i < NUM_DATA; ++i) {
                    const int  LINE  = DATA[i].d_line;
                    const Type INPUT = DATA[i].d_input;

                    bsl::stringstream ss;
                    Print::printText(ss, INPUT);
                    ASSERTV(LINE, ss.good());
                    const bsl::string ENCODED(replaceCharEntities(ss.str()));
                    Type decoded;
                    ASSERTV(LINE, 0 == Util::parseText(&decoded,
                                                       ENCODED.data(),
                                                       intLength(ENCODED)));
                    ASSERTV(LINE, ENCODED, decoded, INPUT, INPUT == decoded);

                    const Type INPUT2(buildPrintTextInput(HEADER, INPUT, ""));
                    ss.str("");
                    Print::printText(ss, INPUT2);
                    ASSERTV(LINE, ss.good());
                    const bsl::string ENCODED2(replaceCharEntities(ss.str()));
                    ASSERTV(LINE, 0 == Util::parseText(&decoded,
                                                       ENCODED2.data(),
                                                       intLength(ENCODED2)));
                    ASSERTV(LINE, ENCODED2, decoded, INPUT2,
                            INPUT2 == decoded);

                    const Type INPUT3(buildPrintTextInput("", INPUT, TRAILER));
                    ss.str("");
                    Print::printText(ss, INPUT3);
                    ASSERTV(LINE, ss.good());
                    const bsl::string ENCODED3(replaceCharEntities(ss.str()));
                    ASSERTV(LINE, 0 == Util::parseText(&decoded,
                                                       ENCODED3.data(),
                                                       intLength(ENCODED3)));
                    ASSERTV(LINE, ENCODED3, decoded, INPUT3,
                            INPUT3 == decoded);

                    const Type INPUT4(buildPrintTextInput(HEADER,
                                                          INPUT,
                                                          TRAILER));
                    ss.str("");
                    Print::printText(ss, INPUT4);
                    ASSERTV(LINE, ss.good());
                    const bsl::string ENCODED4(replaceCharEntities(ss.str()));
                    ASSERTV(LINE, 0 == Util::parseText(&decoded,
                                                       ENCODED4.data(),
                                                       intLength(ENCODED4)));
                    ASSERTV(LINE, ENCODED4, decoded, INPUT4,
                            INPUT4 == decoded);
                }
            }

            if (verbose) cout << "\nUsing 'vector<char>'." << endl;
            {
                typedef bsl::vector<char> Type;

                for (int i = 0; i < NUM_DATA; ++i) {
                    const bsl::size_t  LENGTH = bsl::strlen(DATA[i].d_input);
                    const int          LINE   = DATA[i].d_line;
                    const char        *CINPUT = DATA[i].d_input;
                    const Type         INPUT(CINPUT, CINPUT + LENGTH);

                    bsl::stringstream ss;
                    Print::printText(ss, INPUT);
                    ASSERTV(LINE, ss.good());
                    const bsl::string ENCODED(replaceCharEntities(ss.str()));
                    Type decoded;
                    ASSERTV(LINE, 0 == Util::parseText(&decoded,
                                                       ENCODED.data(),
                                                       intLength(ENCODED)));
                    ASSERTV(LINE, ENCODED, decoded, INPUT, INPUT == decoded);

                    const Type INPUT2(buildPrintTextInput(HEADER, INPUT, ""));
                    ss.str("");
                    Print::printText(ss, INPUT2);
                    ASSERTV(LINE, ss.good());
                    const bsl::string ENCODED2(replaceCharEntities(ss.str()));
                    ASSERTV(LINE, 0 == Util::parseText(&decoded,
                                                       ENCODED2.data(),
                                                       intLength(ENCODED2)));
                    ASSERTV(LINE, ENCODED2, decoded, INPUT2,
                            INPUT2 == decoded);

                    const Type INPUT3(buildPrintTextInput("", INPUT, TRAILER));
                    ss.str("");
                    Print::printText(ss, INPUT3);
                    ASSERTV(LINE, ss.good());
                    const bsl::string ENCODED3(replaceCharEntities(ss.str()));
                    ASSERTV(LINE, 0 == Util::parseText(&decoded,
                                                       ENCODED3.data(),
                                                       intLength(ENCODED3)));
                    ASSERTV(LINE, ENCODED3, decoded, INPUT3,
                            INPUT3 == decoded);

                    const Type INPUT4(buildPrintTextInput(HEADER,
                                                          INPUT,
                                                          TRAILER));
                    ss.str("");
                    Print::printText(ss, INPUT4);
                    ASSERTV(LINE, ss.good());
                    const bsl::string ENCODED4(replaceCharEntities(ss.str()));
                    ASSERTV(LINE, 0 == Util::parseText(&decoded,
                                                       ENCODED4.data(),
                                                       intLength(ENCODED4)));
                    ASSERTV(LINE, ENCODED4, decoded, INPUT4,
                            INPUT4 == decoded);
                }
            }

            if (verbose) cout << "\nUsing 'CustomizedString'." << endl;
            {
                typedef test::CustomizedString Type;

                for (int i = 0; i < NUM_DATA; ++i) {
                    const int          LINE  = DATA[i].d_line;
                    const bsl::string  INSTR = DATA[i].d_input;
                    const Type         INPUT(INSTR);

                    bsl::stringstream ss;
                    Print::printText(ss, INPUT);
                    ASSERTV(LINE, ss.good());
                    const bsl::string ENCODED(replaceCharEntities(ss.str()));
                    Type decoded;
                    ASSERTV(LINE, 0 == Util::parseText(&decoded,
                                                       ENCODED.data(),
                                                       intLength(ENCODED)));
                    ASSERTV(LINE, ENCODED, decoded, INPUT, INPUT == decoded);

                    if (25 < HEADER.size() + INSTR.size()) {
                        continue;                                   // CONTINUE
                    }
                    const Type INPUT2(buildPrintTextInput(HEADER, INSTR, ""));
                    ss.str("");
                    Print::printText(ss, INPUT2);
                    ASSERTV(LINE, ss.good());
                    const bsl::string ENCODED2(replaceCharEntities(ss.str()));
                    ASSERTV(LINE, 0 == Util::parseText(&decoded,
                                                       ENCODED2.data(),
                                                       intLength(ENCODED2)));
                    ASSERTV(LINE, ENCODED2, decoded, INPUT2,
                            INPUT2 == decoded);

                    if (25 < INSTR.size() + TRAILER.size()) {
                        continue;                                   // CONTINUE
                    }
                    const Type INPUT3(buildPrintTextInput("", INSTR, TRAILER));
                    ss.str("");
                    Print::printText(ss, INPUT3);
                    ASSERTV(LINE, ss.good());
                    const bsl::string ENCODED3(replaceCharEntities(ss.str()));
                    ASSERTV(LINE, 0 == Util::parseText(&decoded,
                                                       ENCODED3.data(),
                                                       intLength(ENCODED3)));
                    ASSERTV(LINE, ENCODED3, decoded, INPUT3,
                            INPUT3 == decoded);

                    if (25 < HEADER.size() + INSTR.size() + TRAILER.size()) {
                        continue;                                   // CONTINUE
                    }
                    const Type INPUT4(buildPrintTextInput(HEADER,
                                                          INSTR,
                                                          TRAILER));
                    ss.str("");
                    Print::printText(ss, INPUT4);
                    ASSERTV(LINE, ss.good());
                    const bsl::string ENCODED4(replaceCharEntities(ss.str()));
                    ASSERTV(LINE, 0 == Util::parseText(&decoded,
                                                       ENCODED4.data(),
                                                       intLength(ENCODED4)));
                    ASSERTV(LINE, ENCODED4, decoded, INPUT4,
                            INPUT4 == decoded);
                }
            }
        }

        if (verbose) cout << "\nTesting 'MyEnumeration::Value'." << endl;
        {
            static const RoundTripTestData<test::MyEnumeration::Value> DATA[] =
            {
                //line    input
                //----    -----
                { L_,     test::MyEnumeration::VALUE1 },
                { L_,     test::MyEnumeration::VALUE2 },
            };
            printTextRoundTripScalarTester(DATA);
        }

        if (verbose) cout << "\nTesting 'MyEnumerationWithFallback::Value'."
                          << endl;
        {
            typedef test::MyEnumerationWithFallback::Value Obj;
            static const RoundTripTestData<Obj> DATA[] =
            {
                //line    input
                //----    -----
                { L_,     test::MyEnumerationWithFallback::VALUE1  },
                { L_,     test::MyEnumerationWithFallback::VALUE2  },
                { L_,     test::MyEnumerationWithFallback::UNKNOWN },
            };
            printTextRoundTripScalarTester(DATA);
        }
      } break;
      case 12: {
        // --------------------------------------------------------------------
        // TESTING 'printList' ROUND TRIP
        //
        // Concerns:
        //
        // Plan:
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTESTING 'printList' ROUND TRIP"
                          << "\n==============================" << endl;

        typedef int                   ElemType;
        typedef bsl::vector<ElemType> Type;

        static const struct TestData {
            int         d_line;
            ElemType    d_input[5];
            int         d_numInput;
        } DATA[] = {
            //line    input                numInput
            //----    -----                --------
            { L_,     { },                 0,       },
            { L_,     { 1 },               1,       },
            { L_,     { 1, 4 },            2,       },
            { L_,     { 1, 4, 2 },         3,       },
            { L_,     { 1, 4, 2, 8 },      4,       },
            { L_,     { 1, 4, 2, 8, 23 },  5,       },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int i = 0; i < NUM_DATA; ++i) {
            const int       LINE      = DATA[i].d_line;
            const ElemType *INPUT     = DATA[i].d_input;
            const int       NUM_INPUT = DATA[i].d_numInput;


            const Type IN(INPUT, INPUT + NUM_INPUT);
            bsl::stringstream ss;
            Print::printList(ss, IN);
            ASSERTV(LINE, ss.good());
            const bsl::string ENCODED(ss.str());

            using TestMachinery::intLength;

            Type mDecoded; const Type& DECODED = mDecoded;
            ASSERTV(LINE, 0 == Util::parseList(&mDecoded,
                                               ENCODED.data(),
                                               intLength(ENCODED)));

            ASSERTV(LINE, ENCODED, DECODED, IN, IN == DECODED);

        }
      } break;
      case 11: {
        // --------------------------------------------------------------------
        // TESTING 'printHex' ROUND TRIP
        //
        // Concerns:
        //
        // Plan:
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTESTING 'printHex' ROUND TRIP"
                          << "\n=============================" << endl;

        static const struct TestData {
            int         d_line;
            const char *d_input;
        } DATA[] = {
            //line    input
            //----    -------
            { L_,     "",     },
            { L_,     "a",    },
            { L_,     "ab",   },
            { L_,     "abc",  },
            { L_,     "abcd", },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int i = 0; i < NUM_DATA; ++i) {
            const int          LINE         = DATA[i].d_line;
            const char        *INPUT        = DATA[i].d_input;
            const bsl::size_t  INPUT_LENGTH = bsl::strlen(INPUT);

            const bsl::string IN(INPUT, INPUT + INPUT_LENGTH);
            bsl::stringstream ss;
            Print::printHex(ss, IN);
            ASSERTV(LINE, ss.good());
            const bsl::string ENCODED(ss.str());

            using TestMachinery::intLength;

            {
                typedef bsl::string Type;

                Type mDecoded; const Type& DECODED = mDecoded;
                ASSERTV(LINE, 0 == Util::parseHex(&mDecoded,
                                                  ENCODED.data(),
                                                  intLength(ENCODED)));

                ASSERTV(LINE, ENCODED, DECODED, INPUT, INPUT == DECODED);
            }
            {
                typedef bsl::vector<char> Type;

                Type mDecoded; const Type& DECODED = mDecoded;
                ASSERTV(LINE, 0 == Util::parseHex(&mDecoded,
                                                  ENCODED.data(),
                                                  intLength(ENCODED)));

                const Type IN_VEC(IN.begin(), IN.end());
                ASSERTV(LINE, ENCODED, DECODED, IN_VEC, IN_VEC == DECODED);
            }
        }
      } break;
      case 10: {
        // --------------------------------------------------------------------
        // TESTING 'printDecimal' ROUND TRIP
        //
        // Concerns:
        //
        // Plan:
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTESTING 'printDecimal' ROUND TRIP"
                          << "\n=================================" << endl;

        using TestMachinery::RoundTripTestData;
        using TestMachinery::printDecimalRoundTripTester;

        if (verbose) cout << "\nTesting 'bool'." << endl;
        {
            static const RoundTripTestData<bool> DATA[] =
            {
                //line    input
                //----    -----
                { L_,     true   },
                { L_,     false  },
            };
            printDecimalRoundTripTester(DATA);
        }


        if (verbose) cout << "\nTesting 'char'." << endl;
        {
            static const RoundTripTestData<char> DATA[] =
            {
                //line    input
                //----    -----
                { L_,     -128  },
                { L_,     -127  },
                { L_,     -1    },
                { L_,     0     },
                { L_,     1     },
                { L_,     126   },
                { L_,     127   },
            };
            printDecimalRoundTripTester(DATA);
        }

        if (verbose) cout << "\nTesting 'short'." << endl;
        {
            static const RoundTripTestData<short> DATA[] =
            {
                //line    input
                //----    -----
                { L_,     -32768  },
                { L_,     -32767  },
                { L_,     -1      },
                { L_,     0       },
                { L_,     1       },
                { L_,     32766   },
                { L_,     32767   },
            };
            printDecimalRoundTripTester(DATA);
        }

        if (verbose) cout << "\nTesting 'int'." << endl;
        {
            static const RoundTripTestData<int> DATA[] =
            {
                //line    input
                //----    -----
                { L_,     -2147483647-1 },
                { L_,     -2147483647   },
                { L_,     -1            },
                { L_,     0             },
                { L_,     1             },
                { L_,     2147483646    },
                { L_,     2147483647    },
            };
            printDecimalRoundTripTester(DATA);
        }

        if (verbose) cout << "\nUsing 'long'." << endl;
        {
            static const RoundTripTestData<long> DATA[] =
            {
                //line    input
                //----    -----
                { L_,     -2147483647-1  },
                { L_,     -2147483647    },
                { L_,     -1             },
                { L_,     0              },
                { L_,     1              },
                { L_,     2147483646     },
                { L_,     2147483647     },
#ifdef U_LONG_IS_64_BITS
                { L_,     -9223372036854775807LL - 1 },
                { L_,     -9223372036854775807LL     },
                { L_,     9223372036854775806LL      },
                { L_,     9223372036854775807LL      },
#endif
            };
            printDecimalRoundTripTester(DATA);
        }

        if (verbose) cout << "\nTesting 'bsls::Types::Int64'." << endl;
        {
            static const RoundTripTestData<bsls::Types::Int64> DATA[] =
            {
                //line    input
                //----    -----
                { L_,     -9223372036854775807LL - 1 },
                { L_,     -9223372036854775807LL     },
                { L_,     -1LL                       },
                { L_,     0LL                        },
                { L_,     1LL                        },
                { L_,     9223372036854775806LL      },
                { L_,     9223372036854775807LL      },
            };
            printDecimalRoundTripTester(DATA);
        }

        if (verbose) cout << "\nTesting 'unsigned char'." << endl;
        {
            static const RoundTripTestData<unsigned char> DATA[] =
            {
                //line    input
                //----    -----
                { L_,     0     },
                { L_,     1     },
                { L_,     254   },
                { L_,     255   },
            };
            printDecimalRoundTripTester(DATA);
        }

        if (verbose) cout << "\nTesting 'unsigned short'." << endl;
        {
            static const RoundTripTestData<unsigned short> DATA[] =
            {
                //line    input
                //----    -----
                { L_,     0     },
                { L_,     1     },
                { L_,     65534 },
                { L_,     65535 },
            };
            printDecimalRoundTripTester(DATA);
        }

        if (verbose) cout << "\nTesting 'unsigned int'." << endl;
        {
            static const RoundTripTestData<unsigned int> DATA[] =
            {
                //line    input
                //----    -----
                { L_,     0           },
                { L_,     1           },
                { L_,     4294967294U },
                { L_,     4294967295U },
            };
            printDecimalRoundTripTester(DATA);
        }

        if (verbose) cout << "\nTesting 'unsigned long'." << endl;
        {
            static const RoundTripTestData<unsigned long> DATA[] =
            {
                //line    input
                //----    -----
                { L_,     0LL,                    },
                { L_,     1LL,                    },
                { L_,     4294967294U             },
                { L_,     4294967295UL            },
#ifdef U_LONG_IS_64_BITS
                { L_,     18446744073709551614ULL },
                { L_,     18446744073709551615ULL },
#endif
            };
            printDecimalRoundTripTester(DATA);
        }

        if (verbose) cout << "\nTesting 'bsls::Types::Uint64'." << endl;
        {
            static const RoundTripTestData<bsls::Types::Uint64> DATA[] =
            {
                //line    input
                //----    -----
                { L_,     0ULL                    },
                { L_,     1ULL                    },
                { L_,     18446744073709551614ULL },
                { L_,     18446744073709551615ULL },
            };
            printDecimalRoundTripTester(DATA);
        }

        if (verbose) cout << "\nTesting 'MyEnumeration::Value'." << endl;
        {
            static const RoundTripTestData<test::MyEnumeration::Value> DATA[] =
            {
                //line    input
                //----    -----
                { L_,     test::MyEnumeration::VALUE1 },
                { L_,     test::MyEnumeration::VALUE2 },
            };
            printDecimalRoundTripTester(DATA);
        }

        if (verbose) cout << "\nTesting 'MyEnumerationWithFallback::Value'."
                          << endl;
        {
            typedef test::MyEnumerationWithFallback::Value Obj;
            static const RoundTripTestData<Obj> DATA[] =
            {
                //line    input
                //----    -----
                { L_,     test::MyEnumerationWithFallback::VALUE1  },
                { L_,     test::MyEnumerationWithFallback::VALUE2  },
                { L_,     test::MyEnumerationWithFallback::UNKNOWN },
            };
            printDecimalRoundTripTester(DATA);
        }

        if (verbose) cout << "\nTesting 'CustomizedInt'." << endl;
        {
            typedef test::CustomizedInt Type;

            static const RoundTripTestData<Type> DATA[] =
            {
                //line    input
                //----    -----
                { L_,     Type(5)               },  // 'CustomizedInt' fails
                { L_,     Type(4)               },  // to convert back from
                { L_,     Type(3)               },  // integer for values not
                { L_,     Type(2)               },  // less than 5.
                { L_,     Type(1)               },
                { L_,     Type(0)               },
                { L_,     Type(-1)              },
                { L_,     Type(-2)              },
                { L_,     Type(-2147483647 - 1) },
                { L_,     Type(-2147483647)     },
            };
            printDecimalRoundTripTester(DATA);
        }

        if (verbose) cout << "\nTesting 'float'." << endl;
        {
            typedef bsl::numeric_limits<float> NumLimits;

            static const RoundTripTestData<float> DATA[] =
            {
                //line    input
                //----   -----------
                { L_,    -1.0f       },
                { L_,    -0.1f       },
                { L_,    -0.123456f  },
                { L_,     0.0f       },
                { L_,     0.1f       },
                { L_,     1.0f       },
                { L_,   123.4567f    },

                // Values from 'balxml_encoder.t.cpp' 'runTestCase17'
                { L_, 1.5258789e-05f       },
                { L_, 3.0517578e-05f       },
                { L_, 6.1035156e-05f       },
                { L_, 1.2207031e-04f       },
                { L_, 2.4414062e-04f       },
                { L_, 4.8828125e-04f       },
                { L_, 9.765625e-04f        },
                { L_, 1.953125e-03f        },
                { L_, 3.90625e-03f         },
                { L_, 7.8125e-03f          },
                { L_, 1.5625e-02f          },
                { L_, 3.125e-02f           },
                { L_, 6.25e-02f            },
                { L_, 1.25e-01f            },
                { L_, 2.5e-1f              },
                { L_, 5e-1f                },
                { L_,                  1.f },
                { L_,                  8.f },
                { L_,                 64.f },
                { L_,                128.f },
                { L_,               1024.f },
                { L_,              16384.f },
                { L_,             131072.f },
                { L_,            1048576.f },
                { L_,           16777216.f },
                { L_,          134217728.f },
                { L_,         1073741824.f },
                { L_,        17179869184.f },
                { L_,       137438953472.f },
                { L_,      1099511627776.f },
                { L_,     17592186044416.f },
                { L_,    140737488355328.f },
                { L_,   1125899906842624.f },
                { L_,  18014398509481984.f },
                { L_, 144115188075855870.f },

                // Arbitrary Large and Small Number
                { L_,  1.234567e35f        },
                { L_, -1.234567e35f        },

                { L_,  1.234567e-35f       },
                { L_, -1.234567e-35f       },

                // Small Integers
                { L_,     1234567.f        },
                { L_,     12345678.f       },
                { L_,     123456789.f      },

                // Full Mantissa Integers
                { L_, 1.0f * 0xFFFFFF      },
                { L_, 1.0f * 0xFFFFFF * (1ull << 63) * (1ull << 41) },
                    // the above is 'bsl::numeric_limits<float>::max()'

                // Boundary Values
                { L_,  NumLimits::min()    },
                { L_,  NumLimits::max()    },
                { L_, -NumLimits::min()    },
                { L_, -NumLimits::max()    },
            };
            printDecimalRoundTripTester(DATA);
        }

        if (verbose) cout << "\nTesting 'double'." << endl;
        {
            typedef bsl::numeric_limits<double> NumLimits;

            static const RoundTripTestData<double> DATA[] =
            {
                //line  input
                //----  -------------------
                { L_,   -1.0               },
                { L_,   -0.1               },
                { L_,   -0.123456789012345 },
                { L_,    0.0               },
                { L_,    0.1               },
                { L_,    1.0               },
                { L_,    123.4567          },

                // Values from 'balxml_encoder.t.cpp' 'runTestCase17'
                { L_, 1.52587890625e-05    },
                { L_, 3.0517578125e-05     },
                { L_, 6.103515625e-05      },
                { L_, 1.220703125e-04      },
                { L_, 2.44140625e-04       },
                { L_, 4.8828125e-04        },
                { L_, 9.765625e-04         },
                { L_, 1.953125e-03         },
                { L_, 3.90625e-03          },
                { L_, 7.8125e-03           },
                { L_, 1.5625e-02           },
                { L_, 3.125e-02            },
                { L_, 6.25e-02             },
                { L_, 1.25e-01             },
                { L_, 2.5e-1               },
                { L_, 5e-1                 },
                { L_,                  1.  },
                { L_,                  8.  },
                { L_,                 64.  },
                { L_,                128.  },
                { L_,               1024.  },
                { L_,              16384.  },
                { L_,             131072.  },
                { L_,            1048576.  },
                { L_,           16777216.  },
                { L_,          134217728.  },
                { L_,         1073741824.  },
                { L_,        17179869184.  },
                { L_,       137438953472.  },
                { L_,      1099511627776.  },
                { L_,     17592186044416.  },
                { L_,    140737488355328.  },
                { L_,   1125899906842624.  },
                { L_,  18014398509481984.  },
                { L_, 144115188075855870.  },

                // Small Integers
                { L_, 123456789012345.     },
                { L_, 1234567890123456.    },
                { L_, 12345678901234567.   },
                { L_, 123456789012345678.  },

                // Full Mantissa Integers
                { L_, 1.0 * 0x1FFFFFFFFFFFFFull },
                { L_, 1.0 * 0x1FFFFFFFFFFFFFull
                      * (1ull << 63) * (1ull << 63) * (1ull << 63)
                      * (1ull << 63) * (1ull << 63) * (1ull << 63)
                      * (1ull << 63) * (1ull << 63) * (1ull << 63)
                      * (1ull << 63) * (1ull << 63) * (1ull << 63)
                      * (1ull << 63) * (1ull << 63) * (1ull << 63)
                      * (1ull << 26)                               },
                      // The above is the value of 'NumLimits::max()'

                // Boundary Values
                { L_,  NumLimits::min() },
                { L_,  NumLimits::max() },
                { L_, -NumLimits::min() },
                { L_, -NumLimits::max() },
            };
            printDecimalRoundTripTester(DATA);
        }

        if (verbose) cout << "\nTesting 'double' with encoder options" << endl;
        {
            typedef double Type;

            typedef bsl::numeric_limits<Type> Limits;

            const Type ROUND_TRIPS = Limits::infinity();
                // We use infinity to indicate that with the specified encoder
                // options the exact same binary numeric value must be parsed
                // back, as 'printDecimal'/'parseDecimal' do not allow it as
                // input/output.

            const int N = -42;
                // 'N' indicates an option not set.

            static const struct TestData {
                int  d_line;
                Type d_input;
                int  d_maxTotalDigits;
                int  d_maxFractionDigits;
                Type d_expected;
            } DATA[] = {
#define D(input, maxTotalDigits, maxFractionDigits, expected)                 \
    {L_, input, maxTotalDigits, maxFractionDigits, expected }
                //  input               TD   FD             result
                // -------              --   --           ------------------
                D(      0,               0,   0,          ROUND_TRIPS       ),
                D(      1,               0,   0,          ROUND_TRIPS       ),
                D(     -1,               0,   0,          ROUND_TRIPS       ),

                D(      0,               0,   1,          ROUND_TRIPS       ),
                D(      1,               0,   1,          ROUND_TRIPS       ),
                D(     -1,               0,   1,          ROUND_TRIPS       ),

                D(      0,               1,   0,          ROUND_TRIPS       ),
                D(      1,               1,   0,          ROUND_TRIPS       ),
                D(     -1,               1,   0,          ROUND_TRIPS       ),

                D(      0,               1,   1,          ROUND_TRIPS       ),
                D(      1,               1,   1,          ROUND_TRIPS       ),
                D(     -1,               1,   1,          ROUND_TRIPS       ),

                D(      0,               2,   1,          ROUND_TRIPS       ),
                D(      1,               2,   1,          ROUND_TRIPS       ),
                D(     -1,               2,   1,          ROUND_TRIPS       ),

                D(      0,               2,   2,          ROUND_TRIPS       ),
                D(      1,               2,   2,          ROUND_TRIPS       ),
                D(     -1,               2,   2,          ROUND_TRIPS       ),

                D(      0,               2,   3,          ROUND_TRIPS       ),
                D(      1,               2,   3,          ROUND_TRIPS       ),
                D(     -1,               2,   3,          ROUND_TRIPS       ),

                D(      0,               3,   1,          ROUND_TRIPS       ),
                D(      1,               3,   1,          ROUND_TRIPS       ),
                D(     -1,               3,   1,          ROUND_TRIPS       ),

                D(      0,               3,   2,          ROUND_TRIPS       ),
                D(      1,               3,   2,          ROUND_TRIPS       ),
                D(     -1,               3,   2,          ROUND_TRIPS       ),

                D(      0,               3,   3,          ROUND_TRIPS       ),
                D(      1,               3,   3,          ROUND_TRIPS       ),
                D(     -1,               3,   3,          ROUND_TRIPS       ),

                D(      0,               4,   2,          ROUND_TRIPS       ),
                D(      1,               4,   2,          ROUND_TRIPS       ),
                D(     -1,               4,   2,          ROUND_TRIPS       ),

                D(      0,               4,   3,          ROUND_TRIPS       ),
                D(      1,               4,   3,          ROUND_TRIPS       ),
                D(     -1,               4,   3,          ROUND_TRIPS       ),

                D(      0.0,             2,   0,          ROUND_TRIPS       ),
                D(      1.0,             2,   0,          ROUND_TRIPS       ),
                D(     -1.0,             2,   0,          ROUND_TRIPS       ),

                D(      0.0,             2,   1,          ROUND_TRIPS       ),
                D(      1.0,             2,   1,          ROUND_TRIPS       ),
                D(     -1.0,             2,   1,          ROUND_TRIPS       ),

                D(      0.0,             2,   2,          ROUND_TRIPS       ),
                D(      1.0,             2,   2,          ROUND_TRIPS       ),
                D(     -1.0,             2,   2,          ROUND_TRIPS       ),

                D(      0.0,             2,   3,          ROUND_TRIPS       ),
                D(      1.0,             2,   3,          ROUND_TRIPS       ),
                D(     -1.0,             2,   3,          ROUND_TRIPS       ),

                D(      0.0,             3,   2,          ROUND_TRIPS       ),
                D(      1.0,             3,   2,          ROUND_TRIPS       ),
                D(     -1.0,             3,   2,          ROUND_TRIPS       ),

                D(      0.0,             3,   3,          ROUND_TRIPS       ),
                D(      1.0,             3,   3,          ROUND_TRIPS       ),
                D(     -1.0,             3,   3,          ROUND_TRIPS       ),

                D(      0.1,             2,   0,          ROUND_TRIPS       ),
                D(     -0.1,             2,   0,          ROUND_TRIPS       ),

                D(      0.1,             2,   1,          ROUND_TRIPS       ),
                D(     -0.1,             2,   1,          ROUND_TRIPS       ),

                D(      0.1,             2,   2,          ROUND_TRIPS       ),
                D(     -0.1,             2,   2,          ROUND_TRIPS       ),

                D(      0.1,             2,   3,          ROUND_TRIPS       ),
                D(     -0.1,             2,   3,          ROUND_TRIPS       ),

                D(      0.1234,          0,   0,                   0.1      ),
                D(     -0.1234,          0,   0,                  -0.1      ),

                D(      0.1234,          1,   0,                   0.1      ),
                D(     -0.1234,          1,   0,                  -0.1      ),

                D(      0.1234,          0,   1,                   0.1      ),
                D(     -0.1234,          0,   1,                  -0.1      ),

                D(      0.1234,          1,   1,                   0.1      ),
                D(     -0.1234,          1,   1,                  -0.1      ),

                D(      79.864,          0,   0,                  79.9      ),
                D(     -63.234,          0,   0,                 -63.2      ),

                D(      79.864,          1,   0,                  79.9      ),
                D(     -63.234,          1,   0,                 -63.2      ),

                D(      79.864,          1,   1,                  79.9      ),
                D(     -63.234,          1,   1,                 -63.2      ),

                D(      79.864,          2,   1,                  79.9      ),
                D(     -63.234,          2,   1,                 -63.2      ),

                D(      79.864,          2,   2,                  79.9      ),
                D(     -63.234,          2,   2,                 -63.2      ),

                D(      79.864,          2,   3,                  79.9      ),
                D(     -63.234,          2,   3,                 -63.2      ),

                D(      79.864,          3,   3,                  79.8      ),
                D(     -63.234,          3,   3,                 -63.2      ),

                D(      79.864,          3,   2,                  79.8      ),
                D(     -63.234,          3,   2,                 -63.2      ),
                D(      79.864,          5,   3,          ROUND_TRIPS       ),
                D(     -63.234,          5,   3,          ROUND_TRIPS       ),

                D(      79.864,          5,   4,          ROUND_TRIPS       ),
                D(     -63.234,          5,   4,          ROUND_TRIPS       ),

                D(      79.864,          6,   3,          ROUND_TRIPS       ),
                D(     -63.234,          6,   3,          ROUND_TRIPS       ),

                D(      79.864,          6,   4,          ROUND_TRIPS       ),
                D(     -63.234,          6,   4,          ROUND_TRIPS       ),

                // Examples from the implementation comments
                D(      65.4321,         4,   N,                  65.43     ),
                D(    1234.001,          4,   N,                1234.0      ),
                D(       1.45623,        4,   N,                   1.456    ),
                D(      65.4321,         4,   2,                  65.43     ),
                D(    1234.001,          4,   2,                1234.0      ),
                D(       1.45623,        4,   2,                   1.46     ),

                // Large integer parts writing more digits than asked for
                D(  123456.001,          4,   N,              123456.0      ),

                // Values from 'balxml_encoder.t.cpp' 'runTestCase17' (no opts)
                D(1.52587890625e-05  ,   N,   N,          ROUND_TRIPS       ),
                D(3.0517578125e-05   ,   N,   N,          ROUND_TRIPS       ),
                D(6.103515625e-05    ,   N,   N,          ROUND_TRIPS       ),
                D(1.220703125e-04    ,   N,   N,          ROUND_TRIPS       ),
                D(2.44140625e-04     ,   N,   N,          ROUND_TRIPS       ),
                D(4.8828125e-04      ,   N,   N,          ROUND_TRIPS       ),
                D(9.765625e-04       ,   N,   N,          ROUND_TRIPS       ),
                D(1.953125e-03       ,   N,   N,          ROUND_TRIPS       ),
                D(3.90625e-03        ,   N,   N,          ROUND_TRIPS       ),
                D(7.8125e-03         ,   N,   N,          ROUND_TRIPS       ),
                D(1.5625e-02         ,   N,   N,          ROUND_TRIPS       ),
                D(3.125e-02          ,   N,   N,          ROUND_TRIPS       ),
                D(6.25e-02           ,   N,   N,          ROUND_TRIPS       ),
                D(1.25e-01           ,   N,   N,          ROUND_TRIPS       ),
                D(2.5e-1             ,   N,   N,          ROUND_TRIPS       ),
                D(5e-1               ,   N,   N,          ROUND_TRIPS       ),
                D(                 1.,   N,   N,          ROUND_TRIPS       ),
                D(                 8.,   N,   N,          ROUND_TRIPS       ),
                D(                64.,   N,   N,          ROUND_TRIPS       ),
                D(               128.,   N,   N,          ROUND_TRIPS       ),
                D(              1024.,   N,   N,          ROUND_TRIPS       ),
                D(             16384.,   N,   N,          ROUND_TRIPS       ),
                D(            131072.,   N,   N,          ROUND_TRIPS       ),
                D(           1048576.,   N,   N,          ROUND_TRIPS       ),
                D(          16777216.,   N,   N,          ROUND_TRIPS       ),
                D(         134217728.,   N,   N,          ROUND_TRIPS       ),
                D(        1073741824.,   N,   N,          ROUND_TRIPS       ),
                D(       17179869184.,   N,   N,          ROUND_TRIPS       ),
                D(      137438953472.,   N,   N,          ROUND_TRIPS       ),
                D(     1099511627776.,   N,   N,          ROUND_TRIPS       ),
                D(    17592186044416.,   N,   N,          ROUND_TRIPS       ),
                D(   140737488355328.,   N,   N,          ROUND_TRIPS       ),
                D(  1125899906842624.,   N,   N,          ROUND_TRIPS       ),
                D( 18014398509481984.,   N,   N,          ROUND_TRIPS       ),
                D(144115188075855870.,   N,   N,          ROUND_TRIPS       ),

                // More from 'balxml_encoder.t.cpp' 'runTestCase17' (with opts)
                D(1.52587890625e-05   ,   N,   0,                  0.0      ),
                D(3.0517578125e-05    ,   N,   0,                  0.0      ),
                D(6.103515625e-05     ,   N,   0,                  0.0      ),
                D(1.220703125e-04     ,   N,   0,                  0.0      ),
                D(2.44140625e-04      ,   N,   0,                  0.0      ),
                D(4.8828125e-04       ,   N,   0,                  0.0      ),
                D(9.765625e-04        ,   N,   0,                  0.0      ),
                D(1.953125e-03        ,   N,   0,                  0.0      ),
                D(3.90625e-03         ,   N,   0,                  0.0      ),
                D(7.8125e-03          ,   N,   0,                  0.0      ),
                D(1.5625e-02          ,   N,   0,                  0.0      ),
                D(3.125e-02           ,   N,   0,                  0.0      ),
                D(6.25e-02            ,   N,   0,                  0.1      ),
                D(1.25e-01            ,   N,   0,                  0.1      ),
//               D(2.5e-1              ,   N,   0,                  0.2      ),
// The above line may trigger a bug in  Microsoft's 'sprintf' implementation
// depending on what version of their C library ends up being used.  That
// unfortunately may depends on the actual machine where the code runs, not
// where it is built.  Since this value with the 0 'maxFractionDigits' (that
// becomes the minim allowed 1) does not round-trip anyway, this line does
// not add value to the round-trip test.  So instead of adding exra code to
// accepting 0.3 as well as 0.2 (when using Microsoft 'sprintf') we just skip
// this test line.

                D(5e-1                ,   N,   0,         ROUND_TRIPS       ),
                D(                 1.0,   N,   0,         ROUND_TRIPS       ),
                D(                 8.0,   N,   0,         ROUND_TRIPS       ),
                D(                64.0,   N,   0,         ROUND_TRIPS       ),
                D(               128.0,   N,   0,         ROUND_TRIPS       ),
                D(              1024.0,   N,   0,         ROUND_TRIPS       ),
                D(             16384.0,   N,   0,         ROUND_TRIPS       ),
                D(            131072.0,   N,   0,         ROUND_TRIPS       ),
                D(           1048576.0,   N,   0,         ROUND_TRIPS       ),
                D(          16777216.0,   N,   0,         ROUND_TRIPS       ),
                D(         134217728.0,   N,   0,         ROUND_TRIPS       ),
                D(        1073741824.0,   N,   0,         ROUND_TRIPS       ),
                D(       17179869184.0,   N,   0,         ROUND_TRIPS       ),
                D(      137438953472.0,   N,   0,         ROUND_TRIPS       ),
                D(     1099511627776.0,   N,   0,         ROUND_TRIPS       ),
                D(    17592186044416.0,   N,   0,         ROUND_TRIPS       ),
                D(   140737488355328.0,   N,   0,         ROUND_TRIPS       ),
                D(  1125899906842624.0,   N,   0,         ROUND_TRIPS       ),
                D( 18014398509481984.0,   N,   0,         ROUND_TRIPS       ),
                D(144115188075855870.0,   N,   0,         ROUND_TRIPS       ),

                D(   123456789012345.0,  14,   0,         ROUND_TRIPS       ),
                D(   123456789012345.0,  15,   0,         ROUND_TRIPS       ),
                D(   123456789012345.0,  16,   0,         ROUND_TRIPS       ),
                D(   123456789012345.0,  17,   0,         ROUND_TRIPS       ),
                D(   123456789012345.0,  18,   0,         ROUND_TRIPS       ),
                D(   123456789012345.0,  19,   0,         ROUND_TRIPS       ),
                D(   123456789012345.0,  20,   0,         ROUND_TRIPS       ),

                D(   123456789012345.0,  14,   1,         ROUND_TRIPS       ),
                D(   123456789012345.0,  15,   1,         ROUND_TRIPS       ),
                D(   123456789012345.0,  16,   1,         ROUND_TRIPS       ),
                D(   123456789012345.0,  17,   1,         ROUND_TRIPS       ),
                D(   123456789012345.0,  18,   1,         ROUND_TRIPS       ),
                D(   123456789012345.0,  19,   1,         ROUND_TRIPS       ),
                D(   123456789012345.0,  20,   1,         ROUND_TRIPS       ),

                D(   123456789012345.0,  14,   2,         ROUND_TRIPS       ),
                D(   123456789012345.0,  15,   2,         ROUND_TRIPS       ),
                D(   123456789012345.0,  16,   2,         ROUND_TRIPS       ),
                D(   123456789012345.0,  17,   2,         ROUND_TRIPS       ),
                D(   123456789012345.0,  18,   2,         ROUND_TRIPS       ),
                D(   123456789012345.0,  19,   2,         ROUND_TRIPS       ),
                D(   123456789012345.0,  20,   2,         ROUND_TRIPS       ),

                D(   123456789012345.0,  15,   0,         ROUND_TRIPS       ),
                D(   123456789012345.0,  16,   0,         ROUND_TRIPS       ),
                D(   123456789012345.0,  17,   0,         ROUND_TRIPS       ),
                D(   123456789012345.0,  18,   0,         ROUND_TRIPS       ),
                D(   123456789012345.0,  19,   0,         ROUND_TRIPS       ),
                D(   123456789012345.0,  20,   0,         ROUND_TRIPS       ),

                D(    123456789.012345,  14,   0,          123456789.0      ),
                D(    123456789.012345,  15,   0,          123456789.0      ),
                D(    123456789.012345,  16,   0,          123456789.0      ),
                D(    123456789.012345,  17,   0,          123456789.0      ),
                D(    123456789.012345,  18,   0,          123456789.0      ),
                D(    123456789.012345,  19,   0,          123456789.0      ),
                D(    123456789.012345,  20,   0,          123456789.0      ),

                D(    123456789.012345,  14,   1,          123456789.0      ),
                D(    123456789.012345,  15,   1,          123456789.0      ),
                D(    123456789.012345,  16,   1,          123456789.0      ),
                D(    123456789.012345,  17,   1,          123456789.0      ),
                D(    123456789.012345,  18,   1,          123456789.0      ),
                D(    123456789.012345,  19,   1,          123456789.0      ),
                D(    123456789.012345,  20,   1,          123456789.0      ),

                D(    123456789.012345,  14,   2,          123456789.01     ),
                D(    123456789.012345,  15,   2,          123456789.01     ),
                D(    123456789.012345,  16,   2,          123456789.01     ),
                D(    123456789.012345,  17,   2,          123456789.01     ),
                D(    123456789.012345,  18,   2,          123456789.01     ),
                D(    123456789.012345,  19,   2,          123456789.01     ),
                D(    123456789.012345,  20,   2,          123456789.01     ),

                D(    123456789.012345,  14,   6,          123456789.01234  ),
                D(    123456789.012345,  15,   6,         ROUND_TRIPS       ),
                D(    123456789.012345,  16,   6,         ROUND_TRIPS       ),
                D(    123456789.012345,  17,   6,         ROUND_TRIPS       ),
                D(    123456789.012345,  18,   6,         ROUND_TRIPS       ),
                D(    123456789.012345,  19,   6,         ROUND_TRIPS       ),
                D(    123456789.012345,  20,   6,         ROUND_TRIPS       ),

                D(    123456789.012345,  14,   7,           123456789.01234 ),
                D(    123456789.012345,  15,   7,          ROUND_TRIPS      ),
                D(    123456789.012345,  16,   7,          ROUND_TRIPS      ),
                D(    123456789.012345,  17,   7,          ROUND_TRIPS      ),
                D(    123456789.012345,  18,   7,          ROUND_TRIPS      ),
                D(    123456789.012345,  19,   7,          ROUND_TRIPS      ),
                D(    123456789.012345,  20,   7,          ROUND_TRIPS      ),

                D( Limits::max(),         N,   N,          ROUND_TRIPS      ),
                D( Limits::max(),         N,   0,          ROUND_TRIPS      ),
                D( Limits::max(),       326,  17,          ROUND_TRIPS      ),

                D( Limits::min(),         N,   N,          ROUND_TRIPS      ),
                D( Limits::min(),       326,  17,                   0.0     ),
                D( Limits::min(),         N,   0,                   0.0     ),

                D(-Limits::max(),         N,   N,          ROUND_TRIPS      ),
                D(-Limits::max(),         N,   0,          ROUND_TRIPS      ),
                D(-Limits::max(),       326,  17,          ROUND_TRIPS      ),

                D(-Limits::min(),         N,   N,          ROUND_TRIPS      ),
                D(-Limits::min(),       326,  17,                   0.0     ),
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int   LINE      = DATA[i].d_line;
                const Type  INPUT     = DATA[i].d_input;
                const int   TD        = DATA[i].d_maxTotalDigits;
                const int   FD        = DATA[i].d_maxFractionDigits;
                const Type  EXPECTED  = DATA[i].d_expected;

                balxml::EncoderOptions options;
                if (TD != N) options.setMaxDecimalTotalDigits(TD);
                if (FD != N) options.setMaxDecimalFractionDigits(FD);

                bsl::stringstream ss;
                Print::printDecimal(ss, INPUT, &options);
                ASSERTV(LINE, ss.good());
                const bsl::string ENCODED(ss.str());

                using TestMachinery::intLength;
                Type decoded;
                ASSERTV(LINE, 0 == Util::parseDecimal(&decoded,
                                                      ENCODED.data(),
                                                      intLength(ENCODED)));


                if (EXPECTED == ROUND_TRIPS) {
                    ASSERTV(LINE, ENCODED, decoded, INPUT, INPUT == decoded);
                }
                else {
                    ASSERTV(LINE, ENCODED, decoded, INPUT, EXPECTED,
                            EXPECTED == decoded);
                }
            }
        }
      } break;
      case 9: {
        // --------------------------------------------------------------------
        // TESTING 'printBase64' ROUND TRIP
        //
        // Concerns:
        //
        // Plan:
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTESTING 'printBase64' ROUND TRIP"
                          << "\n================================" << endl;

        static const struct TestData {
            int         d_line;
            const char *d_input;
        } DATA[] = {
            //line    input
            //----    -----
            { L_,     ""     },
            { L_,     "a"    },
            { L_,     "ab"   },
            { L_,     "abc"  },
            { L_,     "abcd" },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int i = 0; i < NUM_DATA; ++i) {
            const int          LINE         = DATA[i].d_line;
            const char        *INPUT        = DATA[i].d_input;
            const bsl::size_t  INPUT_LENGTH = bsl::strlen(INPUT);

            const bsl::string IN(INPUT, INPUT + INPUT_LENGTH);
            bsl::stringstream ss;
            Print::printBase64(ss, IN);
            ASSERTV(LINE, ss.good());
            const bsl::string ENCODED(ss.str());

            using TestMachinery::intLength;

            {
                typedef bsl::string Type;

                Type mDecoded; const Type& DECODED = mDecoded;
                ASSERTV(LINE, 0 == Util::parseBase64(&mDecoded,
                                                     ENCODED.data(),
                                                     intLength(ENCODED)));

                ASSERTV(LINE, ENCODED, DECODED, INPUT, INPUT == DECODED);
            }
            {
                typedef bsl::vector<char> Type;

                Type mDecoded; const Type& DECODED = mDecoded;
                ASSERTV(LINE, 0 == Util::parseBase64(&mDecoded,
                                                     ENCODED.data(),
                                                     intLength(ENCODED)));

                const Type IN_VEC(IN.begin(), IN.end());
                ASSERTV(LINE, ENCODED, DECODED, IN_VEC, IN_VEC == DECODED);
            }
        }
      } break;
      case 8: {
        // --------------------------------------------------------------------
        // TESTING 'parse' FUNCTION
        //
        // Concerns:
        //
        // Plan:
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting 'parse' Function"
                          << "\n========================" << endl;

        if (verbose) cout << "\nUsing 'BASE64'." << endl;
        {
            typedef bsl::vector<char> Type;

            static const struct {
                int         d_lineNum;
                const char *d_input;
                const char *d_result;
            } DATA[] = {
                //line    input           result
                //----    -----           ------
                { L_,     "X",             "",         },
                { L_,     "YQ==X",         "a",        },
                { L_,     "YWI=X",         "ab",       },
                { L_,     "YWJjX",         "abc",      },
                { L_,     "YWJjZA==X",     "abcd",     },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int          LINE          = DATA[i].d_lineNum;
                const char        *INPUT         = DATA[i].d_input;
                const char        *RESULT        = DATA[i].d_result;
                const int          INPUT_LENGTH  =
                                      static_cast<int>(bsl::strlen(INPUT)) - 1;
                const bsl::size_t  RESULT_LENGTH = bsl::strlen(RESULT);

                const Type EXPECTED_RESULT(RESULT, RESULT + RESULT_LENGTH);

                Type mX;  const Type& X = mX;

                int retCode = Util::parse(&mX,
                                          INPUT,
                                          INPUT_LENGTH,
                                          bdlat_FormattingMode::e_BASE64);

                LOOP2_ASSERT(LINE, retCode, 0               == retCode);
                LOOP2_ASSERT(LINE, X,       EXPECTED_RESULT == X);
            }
        }

        if (verbose) cout << "\nUsing 'DEC'." << endl;
        {
            typedef bool Type;

            static const struct {
                int         d_lineNum;
                const char *d_input;
                Type        d_result;
            } DATA[] = {
                //line    input       result
                //----    -----       ------
                { L_,     "1X",        true,           },
                { L_,     "0X",        false,          },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int   LINE            = DATA[i].d_lineNum;
                const char *INPUT           = DATA[i].d_input;
                const int   INPUT_LENGTH    =
                                      static_cast<int>(bsl::strlen(INPUT)) - 1;
                const Type  EXPECTED_RESULT = DATA[i].d_result;

                Type mX(!EXPECTED_RESULT);  const Type& X = mX;

                int retCode = Util::parse(&mX,
                                          INPUT,
                                          INPUT_LENGTH,
                                          bdlat_FormattingMode::e_DEC);

                LOOP2_ASSERT(LINE, retCode, 0               == retCode);
                LOOP2_ASSERT(LINE, X,       EXPECTED_RESULT == X);
            }
        }

        if (verbose) cout << "\nUsing 'DEFAULT' (bool->TEXT)." << endl;
        {
            typedef bool Type;

            static const struct {
                int         d_lineNum;
                const char *d_input;
                Type        d_result;
            } DATA[] = {
                //line    input       result
                //----    -----       ------
                { L_,     "trueX",     true,              },
                { L_,     "falseX",    false,             },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int   LINE            = DATA[i].d_lineNum;
                const char *INPUT           = DATA[i].d_input;
                const int   INPUT_LENGTH    =
                                      static_cast<int>(bsl::strlen(INPUT)) - 1;
                const Type  EXPECTED_RESULT = DATA[i].d_result;

                Type mX(!EXPECTED_RESULT);  const Type& X = mX;

                int retCode = Util::parse(&mX,
                                          INPUT,
                                          INPUT_LENGTH,
                                          bdlat_FormattingMode::e_DEFAULT);

                LOOP2_ASSERT(LINE, retCode, 0               == retCode);
                LOOP2_ASSERT(LINE, X,       EXPECTED_RESULT == X);
            }
        }

        if (verbose) cout << "\nUsing 'DEFAULT' (char->DEC)." << endl;
        {
            typedef char Type;

            static const struct {
                int         d_lineNum;
                const char *d_input;
                Type        d_result;
            } DATA[] = {
                //line    input       result
                //----    -----       ------
                { L_,     "-128X",     -128,           },
                { L_,     "-127X",     -127,           },
                { L_,     "-1X",       -1,             },
                { L_,     "0X",        0,              },
                { L_,     "1X",        1,              },
                { L_,     "126X",      126,            },
                { L_,     "127X",      127,            },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int   LINE            = DATA[i].d_lineNum;
                const char *INPUT           = DATA[i].d_input;
                const int   INPUT_LENGTH    =
                                      static_cast<int>(bsl::strlen(INPUT)) - 1;
                const Type  EXPECTED_RESULT = DATA[i].d_result;

                Type mX(!EXPECTED_RESULT);  const Type& X = mX;

                int retCode = Util::parse(&mX,
                                          INPUT,
                                          INPUT_LENGTH,
                                          bdlat_FormattingMode::e_DEFAULT);

                LOOP2_ASSERT(LINE, retCode, 0               == retCode);
                LOOP2_ASSERT(LINE, X,       EXPECTED_RESULT == X);
            }
        }

        if (verbose) cout << "\nUsing 'HEX'." << endl;
        {
            typedef bsl::vector<char> Type;

            static const struct {
                int         d_lineNum;
                const char *d_input;
                const char *d_result;
            } DATA[] = {
                //line    input           result
                //----    -----           ------
                { L_,     "X",             "",         },
                { L_,     "61X",           "a",        },
                { L_,     "6162X",         "ab",       },
                { L_,     "616263X",       "abc",      },
                { L_,     "61626364X",     "abcd",     },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int          LINE          = DATA[i].d_lineNum;
                const char        *INPUT         = DATA[i].d_input;
                const char        *RESULT        = DATA[i].d_result;
                const int          INPUT_LENGTH  =
                                      static_cast<int>(bsl::strlen(INPUT)) - 1;
                const bsl::size_t  RESULT_LENGTH = bsl::strlen(RESULT);

                const Type EXPECTED_RESULT(RESULT, RESULT + RESULT_LENGTH);

                Type mX;  const Type& X = mX;

                int retCode = Util::parse(&mX,
                                          INPUT,
                                          INPUT_LENGTH,
                                          bdlat_FormattingMode::e_HEX);

                LOOP2_ASSERT(LINE, retCode, 0               == retCode);
                LOOP2_ASSERT(LINE, X,       EXPECTED_RESULT == X);
            }
        }

        if (verbose) cout << "\nUsing 'IS_LIST'." << endl;
        {
            typedef int                   ElemType;
            typedef bsl::vector<ElemType> Type;

            static const struct {
                int         d_lineNum;
                const char *d_input;
                ElemType    d_result[5];
                int         d_numResult;
            } DATA[] = {
                //line    input           result               numResult
                //----    -----           ------               ---------
                { L_,     "X",             { },                 0,       },
                { L_,     "1X",            { 1 },               1,       },
                { L_,     "1 4X",          { 1, 4 },            2,       },
                { L_,     "1 4 2X",        { 1, 4, 2 },         3,       },
                { L_,     "1 4 2 8X",      { 1, 4, 2, 8 },      4,       },
                { L_,     "1 4 2 8 23X",   { 1, 4, 2, 8, 23 },  5,       },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int       LINE         = DATA[i].d_lineNum;
                const char     *INPUT        = DATA[i].d_input;
                const int       INPUT_LENGTH =
                                      static_cast<int>(bsl::strlen(INPUT)) - 1;
                const ElemType *RESULT       = DATA[i].d_result;
                const int       NUM_RESULT   = DATA[i].d_numResult;

                const Type EXPECTED_RESULT(RESULT, RESULT + NUM_RESULT);

                Type mX;  const Type& X = mX;
                Type mY;  const Type& Y = mY;

                int retCode = Util::parse(&mX,
                                          INPUT,
                                          INPUT_LENGTH,
                                          bdlat_FormattingMode::e_LIST);

                LOOP2_ASSERT(LINE, retCode, 0               == retCode);
                LOOP2_ASSERT(LINE, X,       EXPECTED_RESULT == X);

                retCode = Util::parse(&mY,
                                      INPUT,
                                      INPUT_LENGTH,
                                      bdlat_FormattingMode::e_LIST
                                    | bdlat_FormattingMode::e_DEC);

                LOOP2_ASSERT(LINE, retCode, 0               == retCode);
                LOOP2_ASSERT(LINE, Y,       EXPECTED_RESULT == Y);
            }
        }

        if (verbose) cout << "\nUsing 'TEXT'." << endl;
        {
            typedef bool Type;

            static const struct {
                int         d_lineNum;
                const char *d_input;
                Type        d_result;
            } DATA[] = {
                //line    input           result
                //----    -----           ------
                { L_,     "trueX",         true,       },
                { L_,     "falseX",        false,      },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int   LINE            = DATA[i].d_lineNum;
                const char *INPUT           = DATA[i].d_input;
                const int   INPUT_LENGTH    =
                                      static_cast<int>(bsl::strlen(INPUT)) - 1;
                const Type  EXPECTED_RESULT = DATA[i].d_result;

                Type mX(!EXPECTED_RESULT);  const Type& X = mX;

                int retCode = Util::parse(&mX,
                                          INPUT,
                                          INPUT_LENGTH,
                                          bdlat_FormattingMode::e_TEXT);

                LOOP2_ASSERT(LINE, retCode, 0               == retCode);
                LOOP2_ASSERT(LINE, X,       EXPECTED_RESULT == X);
            }
        }
      } break;
      case 7: {
        // --------------------------------------------------------------------
        // TESTING 'parseDefault' FUNCTIONS
        //
        // Concerns:
        //
        // Plan:
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting 'parseDefault' Functions"
                          << "\n================================" << endl;

        if (verbose) cout << "\nUsing 'bool'." << endl;
        {
            typedef bool Type;

            static const struct {
                int         d_lineNum;
                const char *d_input;
                Type        d_result;
            } DATA[] = {
                //line    input       result
                //----    -----       ------
                { L_,     "trueX",     true,             },
                { L_,     "falseX",    false,            },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int   LINE            = DATA[i].d_lineNum;
                const char *INPUT           = DATA[i].d_input;
                const int   INPUT_LENGTH    =
                                      static_cast<int>(bsl::strlen(INPUT)) - 1;
                const Type  EXPECTED_RESULT = DATA[i].d_result;

                Type mX(!EXPECTED_RESULT);  const Type& X = mX;

                int retCode = Util::parseDefault(&mX, INPUT, INPUT_LENGTH);

                LOOP2_ASSERT(LINE, retCode, 0               == retCode);
                LOOP2_ASSERT(LINE, X,       EXPECTED_RESULT == X);
            }
        }

        if (verbose) cout << "\nUsing 'char'." << endl;
        {
            typedef char Type;

            static const struct {
                int         d_lineNum;
                const char *d_input;
                Type        d_result;
            } DATA[] = {
                //line    input       result
                //----    -----       ------
                { L_,     "-128X",     -128,            },
                { L_,     "-127X",     -127,            },
                { L_,     "-1X",       -1,              },
                { L_,     "0X",        0,               },
                { L_,     "1X",        1,               },
                { L_,     "126X",      126,             },
                { L_,     "127X",      127,             },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int   LINE            = DATA[i].d_lineNum;
                const char *INPUT           = DATA[i].d_input;
                const int   INPUT_LENGTH    =
                                      static_cast<int>(bsl::strlen(INPUT)) - 1;
                const Type  EXPECTED_RESULT = DATA[i].d_result;

                Type mX(!EXPECTED_RESULT);  const Type& X = mX;

                int retCode = Util::parseDefault(&mX, INPUT, INPUT_LENGTH);

                LOOP2_ASSERT(LINE, retCode, 0               == retCode);
                LOOP2_ASSERT(LINE, X,       EXPECTED_RESULT == X);
            }
        }

        if (verbose) cout << "\nUsing 'short'." << endl;
        {
            typedef short Type;

            static const struct {
                int         d_lineNum;
                const char *d_input;
                Type        d_result;
            } DATA[] = {
                //line    input             result
                //----    -----             ------
                { L_,     "-32768X",         -32768,    },
                { L_,     "-32767X",         -32767,    },
                { L_,     "-1X",             -1,        },
                { L_,     "0X",              0,         },
                { L_,     "1X",              1,         },
                { L_,     "32766X",          32766,     },
                { L_,     "32767X",          32767,     },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int   LINE            = DATA[i].d_lineNum;
                const char *INPUT           = DATA[i].d_input;
                const int   INPUT_LENGTH    =
                                      static_cast<int>(bsl::strlen(INPUT)) - 1;
                const Type  EXPECTED_RESULT = DATA[i].d_result;

                Type mX(!EXPECTED_RESULT);  const Type& X = mX;

                int retCode = Util::parseDefault(&mX, INPUT, INPUT_LENGTH);

                LOOP2_ASSERT(LINE, retCode, 0               == retCode);
                LOOP2_ASSERT(LINE, X,       EXPECTED_RESULT == X);
            }
        }

        if (verbose) cout << "\nUsing 'int'." << endl;
        {
            typedef int Type;

            static const struct {
                int         d_lineNum;
                const char *d_input;
                Type        d_result;
            } DATA[] = {
                //line    input            result
                //----    -----            ------
                { L_,     "-2147483648X",   -2147483647-1    },
                { L_,     "-2147483647X",   -2147483647,     },
                { L_,     "-1X",            -1,              },
                { L_,     "0X",             0,               },
                { L_,     "1X",             1,               },
                { L_,     "2147483646X",    2147483646,      },
                { L_,     "2147483647X",    2147483647,      },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int   LINE            = DATA[i].d_lineNum;
                const char *INPUT           = DATA[i].d_input;
                const int   INPUT_LENGTH    =
                                      static_cast<int>(bsl::strlen(INPUT)) - 1;
                const Type  EXPECTED_RESULT = DATA[i].d_result;

                Type mX(!EXPECTED_RESULT);  const Type& X = mX;

                int retCode = Util::parseDefault(&mX, INPUT, INPUT_LENGTH);

                LOOP2_ASSERT(LINE, retCode, 0               == retCode);
                LOOP2_ASSERT(LINE, X,       EXPECTED_RESULT == X);
            }
        }

        if (verbose) cout << "\nUsing 'bsls::Types::Int64'." << endl;
        {
            typedef bsls::Types::Int64 Type;

            static const struct {
                int         d_lineNum;
                const char *d_input;
                Type        d_result;
            } DATA[] = {
                //line    input                    result
                //----    -----                    ------
                { L_,     "-9223372036854775808X",
                                static_cast<Type>(-9223372036854775808ULL),  },
                { L_,     "-9223372036854775807X",  -9223372036854775807LL,  },
                { L_,     "-1X",                    -1LL,                    },
                { L_,     "0X",                     0LL,                     },
                { L_,     "1X",                     1LL,                     },
                { L_,     "9223372036854775806X",   9223372036854775806LL,   },
                { L_,     "9223372036854775807X",   9223372036854775807LL,   },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int   LINE            = DATA[i].d_lineNum;
                const char *INPUT           = DATA[i].d_input;
                const int   INPUT_LENGTH    =
                                      static_cast<int>(bsl::strlen(INPUT)) - 1;
                const Type  EXPECTED_RESULT = DATA[i].d_result;

                Type mX(!EXPECTED_RESULT);  const Type& X = mX;

                int retCode = Util::parseDefault(&mX, INPUT, INPUT_LENGTH);

                LOOP2_ASSERT(LINE, retCode, 0               == retCode);
                LOOP2_ASSERT(LINE, X,       EXPECTED_RESULT == X);
            }
        }

        if (verbose) cout << "\nUsing 'unsigned char'." << endl;
        {
            typedef unsigned char Type;

            static const struct {
                int         d_lineNum;
                const char *d_input;
                Type        d_result;
            } DATA[] = {
                //line    input       result
                //----    -----       ------
                { L_,     "0X",        0,              },
                { L_,     "1X",        1,              },
                { L_,     "254X",      254,            },
                { L_,     "255X",      255,            },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int   LINE            = DATA[i].d_lineNum;
                const char *INPUT           = DATA[i].d_input;
                const int   INPUT_LENGTH    =
                                      static_cast<int>(bsl::strlen(INPUT)) - 1;
                const Type  EXPECTED_RESULT = DATA[i].d_result;

                Type mX(!EXPECTED_RESULT);  const Type& X = mX;

                int retCode = Util::parseDefault(&mX, INPUT, INPUT_LENGTH);

                LOOP2_ASSERT(LINE, retCode, 0               == retCode);
                LOOP2_ASSERT(LINE, X,       EXPECTED_RESULT == X);
            }
        }

        if (verbose) cout << "\nUsing 'unsigned short'." << endl;
        {
            typedef unsigned short Type;

            static const struct {
                int         d_lineNum;
                const char *d_input;
                Type        d_result;
            } DATA[] = {
                //line    input           result
                //----    -----           ------
                { L_,     "0X",            0,          },
                { L_,     "1X",            1,          },
                { L_,     "65534X",        65534,      },
                { L_,     "65535X",        65535,      },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int   LINE            = DATA[i].d_lineNum;
                const char *INPUT           = DATA[i].d_input;
                const int   INPUT_LENGTH    =
                                      static_cast<int>(bsl::strlen(INPUT)) - 1;
                const Type  EXPECTED_RESULT = DATA[i].d_result;

                Type mX(!EXPECTED_RESULT);  const Type& X = mX;

                int retCode = Util::parseDefault(&mX, INPUT, INPUT_LENGTH);

                LOOP2_ASSERT(LINE, retCode, 0               == retCode);
                LOOP2_ASSERT(LINE, X,       EXPECTED_RESULT == X);
            }
        }

        if (verbose) cout << "\nUsing 'unsigned int'." << endl;
        {
            typedef unsigned int Type;

            static const struct {
                int         d_lineNum;
                const char *d_input;
                Type        d_result;
            } DATA[] = {
                //line    input            result
                //----    -----          -----------
                { L_,     "0X",          0           },
                { L_,     "1X",          1           },
                { L_,     "4294967294X", 4294967294U },
                { L_,     "4294967295X", 4294967295U },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int   LINE            = DATA[i].d_lineNum;
                const char *INPUT           = DATA[i].d_input;
                const int   INPUT_LENGTH    =
                                      static_cast<int>(bsl::strlen(INPUT)) - 1;
                const Type  EXPECTED_RESULT = DATA[i].d_result;

                Type mX(!EXPECTED_RESULT);  const Type& X = mX;

                int retCode = Util::parseDefault(&mX, INPUT, INPUT_LENGTH);

                LOOP2_ASSERT(LINE, retCode, 0               == retCode);
                LOOP2_ASSERT(LINE, X,       EXPECTED_RESULT == X);
            }
        }

        if (verbose) cout << "\nUsing 'bsls::Types::Uint64'." << endl;
        {
            typedef bsls::Types::Uint64 Type;

            static const struct {
                int         d_lineNum;
                const char *d_input;
                Type        d_result;
            } DATA[] = {
                //line    input                    result
                //----    -----                    ------
                { L_,     "0X",                    0ULL,                    },
                { L_,     "1X",                    1ULL,                    },
                { L_,     "18446744073709551614X", 18446744073709551614ULL, },
                { L_,     "18446744073709551615X", 18446744073709551615ULL, },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int   LINE            = DATA[i].d_lineNum;
                const char *INPUT           = DATA[i].d_input;
                const int   INPUT_LENGTH    =
                                      static_cast<int>(bsl::strlen(INPUT)) - 1;
                const Type  EXPECTED_RESULT = DATA[i].d_result;

                Type mX(!EXPECTED_RESULT);  const Type& X = mX;

                int retCode = Util::parseDefault(&mX, INPUT, INPUT_LENGTH);

                LOOP2_ASSERT(LINE, retCode, 0               == retCode);
                LOOP2_ASSERT(LINE, X,       EXPECTED_RESULT == X);
            }
        }

        if (verbose) cout << "\nUsing 'float'." << endl;
        {
            typedef float Type;

            Type posInf = bsl::numeric_limits<Type>::infinity();
            Type qNaN   = bsl::numeric_limits<Type>::quiet_NaN();

            static const struct {
                int         d_lineNum;
                const char *d_input;
                Type        d_result;
            } DATA[] = {
                //line    input        result
                //----    -----       ---------
                { L_,     "-1X",      -1.0f     },
                { L_,     "-0.1X",    -0.1f     },
                { L_,     "0X",        0.0f     },
                { L_,     "0.1X",      0.1f     },
                { L_,     "1X",        1.0f     },
                { L_,     "123.4X",    123.4f   },
                { L_,     "0.005X",    0.005f   },
                { L_,     "9.99E36X",  9.99E36f },
                { L_,     "+INFX",     posInf   },
                { L_,     "-INFX",    -posInf   },
                { L_,     "NaNX",      qNaN     },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int   LINE            = DATA[i].d_lineNum;
                const char *INPUT           = DATA[i].d_input;
                const int   INPUT_LENGTH    =
                                      static_cast<int>(bsl::strlen(INPUT)) - 1;
                const Type  EXPECTED_RESULT = DATA[i].d_result;

                Type mX;  const Type& X = mX;

                int retCode = Util::parseDefault(&mX, INPUT, INPUT_LENGTH);

                LOOP2_ASSERT(LINE, retCode, 0 == retCode);

                if (bdlb::Float::isNan(EXPECTED_RESULT)) {
                    LOOP2_ASSERT(LINE, X, bdlb::Float::isNan(X));
                }
                else {
                    LOOP2_ASSERT(LINE, X, EXPECTED_RESULT == X);
                }
            }
        }

        if (verbose) cout << "\nUsing 'double'." << endl;
        {
            typedef double Type;

            Type posInf = bsl::numeric_limits<Type>::infinity();
            Type qNaN   = bsl::numeric_limits<Type>::quiet_NaN();

            static const struct {
                int         d_lineNum;
                const char *d_input;
                Type        d_result;
            } DATA[] = {
                //line    input       result
                //----    -----       ------
                { L_,     "-1X",       -1.0,            },
                { L_,     "-0.1X",     -0.1,            },
                { L_,     "0X",        0.0,             },
                { L_,     "0.1X",      0.1,             },
                { L_,     "1X",        1.0,             },
                { L_,     "123.4X",    123.4,           },
                { L_,     "0.005X",    0.005            },
                { L_,     "9.99E306X", 9.99E306         },
                { L_,     "+INFX",     posInf,          },
                { L_,     "-INFX",     -posInf,         },
                { L_,     "NaNX",      qNaN             },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int   LINE            = DATA[i].d_lineNum;
                const char *INPUT           = DATA[i].d_input;
                const int   INPUT_LENGTH    =
                                      static_cast<int>(bsl::strlen(INPUT)) - 1;
                const Type  EXPECTED_RESULT = DATA[i].d_result;

                Type mX;  const Type& X = mX;

                int retCode = Util::parseDefault(&mX, INPUT, INPUT_LENGTH);

                LOOP2_ASSERT(LINE, retCode, 0 == retCode);

                if (bdlb::Float::isNan(EXPECTED_RESULT)) {
                    LOOP2_ASSERT(LINE, X, bdlb::Float::isNan(X));
                }
                else {
                    LOOP2_ASSERT(LINE, X, EXPECTED_RESULT == X);
                }
            }
        }

        if (verbose) cout << "\nUsing 'Decimal64'." << endl;
        {
            typedef bdldfp::Decimal64 Type;

            const Type posInf = bsl::numeric_limits<Type>::infinity();
            const Type qNaN   = bsl::numeric_limits<Type>::quiet_NaN();
            const Type qNull  = BDLDFP_DECIMAL_DD(-2.3e5);

            static const struct Data {
                int         d_lineNum;
                const char *d_input;
                Type        d_result;
                bool        d_weird;
            } DATA[] = {
                //line    input        result                       weird
                //----    -----        ------                       -----
                { L_,     "-1X",       BDLDFP_DECIMAL_DD(-1.0),     0 },
                { L_,     "-0.1X",     BDLDFP_DECIMAL_DD(-0.1),     0 },
                { L_,     "0X",        BDLDFP_DECIMAL_DD(0.0),      0 },
                { L_,     "0.1X",      BDLDFP_DECIMAL_DD(0.1),      0 },
                { L_,     "1X",        BDLDFP_DECIMAL_DD(1.0),      0 },
                { L_,     "123.4X",    BDLDFP_DECIMAL_DD(123.4),    0 },
                { L_,     "0.005X",    BDLDFP_DECIMAL_DD(0.005),    0 },
                { L_,     "9.99E306X", BDLDFP_DECIMAL_DD(9.99E306), 0 },
                { L_,     "+INFX",     posInf,                      1 },
                { L_,     "-INFX",     -posInf,                     1 },
                { L_,     "NaNX",      qNaN,                        1 },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int ti = 0; ti < 2 * NUM_DATA; ++ti) {
                const bool  DECIMAL         = ti % 2;
                const Data& data            = DATA[ti / 2];
                const int   LINE            = data.d_lineNum;
                const char *INPUT           = data.d_input;
                const int   INPUT_LENGTH    =
                                      static_cast<int>(bsl::strlen(INPUT)) - 1;
                const Type  EXPECTED_RESULT = data.d_result;
                const bool  WEIRD           = data.d_weird;
                const bool  SUCCESS         = !DECIMAL || !WEIRD;

                Type mX(qNull);  const Type& X = mX;

                int rc = DECIMAL
                         ? Util::parseDecimal(&mX, INPUT, INPUT_LENGTH)
                         : Util::parseDefault(&mX, INPUT, INPUT_LENGTH);

                ASSERTV(LINE, rc, SUCCESS, SUCCESS == (0 == rc));

                if (SUCCESS && bdldfp::DecimalUtil::isNan(EXPECTED_RESULT)) {
                    ASSERTV(LINE, X, bdldfp::DecimalUtil::isNan(X));
                }
                else {
                    ASSERTV(LINE, X, EXPECTED_RESULT, SUCCESS,
                                     (SUCCESS ? EXPECTED_RESULT : qNull) == X);
                }
            }
        }

        if (verbose) cout << "\nUsing 'bsl::string'." << endl;
        {
            typedef bsl::string Type;

            static const struct {
                int         d_lineNum;
                const char *d_input;
                Type        d_result;
            } DATA[] = {
                //line    input       result
                //----    -----       ------
                { L_,     "X",         "",         },
                { L_,     "HelloX",    "Hello",    },
                { L_,     "World!!X",  "World!!",  },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int   LINE            = DATA[i].d_lineNum;
                const char *INPUT           = DATA[i].d_input;
                const int   INPUT_LENGTH    =
                                      static_cast<int>(bsl::strlen(INPUT)) - 1;
                const Type  EXPECTED_RESULT = DATA[i].d_result;

                Type mX;  const Type& X = mX;

                int retCode = Util::parseDefault(&mX, INPUT, INPUT_LENGTH);

                LOOP2_ASSERT(LINE, retCode, 0               == retCode);
                LOOP2_ASSERT(LINE, X,       EXPECTED_RESULT == X);
            }
        }

        if (verbose) cout << "\nUsing 'bsl::vector<char>'." << endl;
        {
            typedef bsl::vector<char> Type;

            static const struct {
                int         d_lineNum;
                const char *d_input;
                const char *d_result;
            } DATA[] = {
                //line    input           result
                //----    -----           ------
                { L_,     "X",             "",         },
                { L_,     "YQ==X",         "a",        },
                { L_,     "YWI=X",         "ab",       },
                { L_,     "YWJjX",         "abc",      },
                { L_,     "YWJjZA==X",     "abcd",     },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int          LINE          = DATA[i].d_lineNum;
                const char        *INPUT         = DATA[i].d_input;
                const char        *RESULT        = DATA[i].d_result;
                const int          INPUT_LENGTH  =
                                      static_cast<int>(bsl::strlen(INPUT)) - 1;
                const bsl::size_t  RESULT_LENGTH = bsl::strlen(RESULT);

                const Type EXPECTED_RESULT(RESULT, RESULT + RESULT_LENGTH);

                Type mX;  const Type& X = mX;

                int retCode = Util::parseDefault(&mX, INPUT, INPUT_LENGTH);

                LOOP2_ASSERT(LINE, retCode, 0               == retCode);
                LOOP2_ASSERT(LINE, X,       EXPECTED_RESULT == X);
            }
        }

        if (verbose) cout << "\nUsing 'MyEnumeration::Value'." << endl;
        {
            typedef test::MyEnumeration::Value Type;

            static const struct {
                int         d_lineNum;
                const char *d_input;
                Type        d_result;
            } DATA[] = {
                //line    input        result
                //----    -----        ------
                { L_,     "VALUE1X",    test::MyEnumeration::VALUE1,  },
                { L_,     "VALUE2X",    test::MyEnumeration::VALUE2,  },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int   LINE            = DATA[i].d_lineNum;
                const char *INPUT           = DATA[i].d_input;
                const int   INPUT_LENGTH    =
                                      static_cast<int>(bsl::strlen(INPUT)) - 1;
                const Type  EXPECTED_RESULT = DATA[i].d_result;

                Type mX;  const Type& X = mX;

                int retCode = Util::parseDefault(&mX, INPUT, INPUT_LENGTH);

                LOOP2_ASSERT(LINE, retCode, 0               == retCode);
                LOOP2_ASSERT(LINE, X,       EXPECTED_RESULT == X);
            }
        }
        // Failure case for 'MyEnumeration::Value', where the enumerator is
        // unknown. (In contrast, 'MyEnumerationWithFallback::Value', below,
        // always succeeds in this case.)
        {
            typedef test::MyEnumeration::Value Type;

            const char *INPUT = "VALUE3X";
            const int   INPUT_LENGTH    =
                                  static_cast<int>(bsl::strlen(INPUT)) - 1;
            Type        mX;

            const int   retCode = Util::parseDefault(&mX, INPUT, INPUT_LENGTH);

            ASSERTV(retCode, 0 != retCode);
        }

        if (verbose) cout << "\nUsing 'MyEnumerationWithFallback::Value'."
                          << endl;
        {
            typedef test::MyEnumerationWithFallback::Value Type;

            static const struct {
                int         d_lineNum;
                const char *d_input;
                Type        d_result;
            } DATA[] = {
                //line input       result
                //---- -----       ------
                { L_,  "VALUE1X",  test::MyEnumerationWithFallback::VALUE1  },
                { L_,  "UNKNOWNX", test::MyEnumerationWithFallback::UNKNOWN },
                { L_,  "VALUE2X",  test::MyEnumerationWithFallback::VALUE2  },
                { L_,  "VALUE3X",  test::MyEnumerationWithFallback::UNKNOWN },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int   LINE            = DATA[i].d_lineNum;
                const char *INPUT           = DATA[i].d_input;
                const int   INPUT_LENGTH    =
                                      static_cast<int>(bsl::strlen(INPUT)) - 1;
                const Type  EXPECTED_RESULT = DATA[i].d_result;

                Type mX;  const Type& X = mX;

                int retCode = Util::parseDefault(&mX, INPUT, INPUT_LENGTH);

                LOOP2_ASSERT(LINE, retCode, 0               == retCode);
                LOOP2_ASSERT(LINE, X,       EXPECTED_RESULT == X);
            }
        }

        if (verbose) cout << "\nUsing 'CustomizedString'." << endl;
        {
            typedef test::CustomizedString Type;

            static const struct {
                int         d_lineNum;
                const char *d_input;
                Type        d_result;
            } DATA[] = {
                //line    input       result
                //----    -----       ------
                { L_,     "X",         Type(""),           },
                { L_,     "aX",        Type("a"),          },
                { L_,     "abX",       Type("ab"),         },
                { L_,     "abcX",      Type("abc"),        },
                { L_,     "abcdX",     Type("abcd"),       },
                { L_,     "abcdeX",    Type("abcde"),      },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int   LINE            = DATA[i].d_lineNum;
                const char *INPUT           = DATA[i].d_input;
                const int   INPUT_LENGTH    =
                                      static_cast<int>(bsl::strlen(INPUT)) - 1;
                const Type  EXPECTED_RESULT = DATA[i].d_result;

                Type mX;  const Type& X = mX;

                int retCode = Util::parseDefault(&mX, INPUT, INPUT_LENGTH);

                LOOP2_ASSERT(LINE, retCode, 0               == retCode);
                LOOP2_ASSERT(LINE, X,       EXPECTED_RESULT == X);
            }
        }

        if (verbose) cout << "\nUsing 'CustomizedInt'." << endl;
        {
            typedef test::CustomizedInt Type;

            static const struct {
                int         d_lineNum;
                const char *d_input;
                Type        d_result;
            } DATA[] = {
                //line    input             result
                //----    -----             ------
                { L_,     "-2147483648X",    Type(-2147483647-1), },
                { L_,     "-2147483647X",    Type(-2147483647),   },
                { L_,     "-1X",             Type(-1),            },
                { L_,     "0X",              Type(0),             },
                { L_,     "1X",              Type(1),             },
                { L_,     "2X",              Type(2),             },
                { L_,     "3X",              Type(3),             },
                { L_,     "4X",              Type(4),             },
                { L_,     "5X",              Type(5),             },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int   LINE            = DATA[i].d_lineNum;
                const char *INPUT           = DATA[i].d_input;
                const int   INPUT_LENGTH    =
                                      static_cast<int>(bsl::strlen(INPUT)) - 1;
                const Type  EXPECTED_RESULT = DATA[i].d_result;

                Type mX;  const Type& X = mX;

                int retCode = Util::parseDefault(&mX, INPUT, INPUT_LENGTH);

                LOOP2_ASSERT(LINE, retCode, 0               == retCode);
                LOOP2_ASSERT(LINE, X,       EXPECTED_RESULT == X);
            }
        }

        if (verbose) cout << "\nUsing 'Date'." << endl;
        {
            static const struct {
                int         d_line;      // source line number
                const char *d_input_p;   // input
                int         d_year;      // year under test
                int         d_month;     // month under test
                int         d_day;       // day under test
                bool        d_isValid;   // isValid flag
            } DATA[] = {
                //line  input           year month   day  isValid
                //----  --------------  ---- -----   ---  -------
                // valid input
                {  L_, "0001-01-01",     1,     1,    1,   true  },
                {  L_, "0009-09-09",     9,     9,    9,   true  },
                {  L_, "0030-10-20",    30,    10,   20,   true  },
                {  L_, "0842-12-19",   842,    12,   19,   true  },
                {  L_, "1847-05-19",  1847,     5,   19,   true  },
                {  L_, "2000-02-29",  2000,     2,   29,   true  },
                {  L_, "9999-12-31",  9999,    12,   31,   true  },

                // invalid input
                {  L_, "05-05-05",       1,     1,    1,  false  },
                {  L_, "005-05-5",       1,     1,    1,  false  },
                {  L_, "0001 01-01",     1,     1,    1,  false  },
                {  L_, "0001-01:01",     1,     1,    1,  false  },
                {  L_, "0000-01-01",     1,     1,    1,  false  },
                {  L_, "0001-01-32",     1,     1,    1,  false  },
                {  L_, "0001-04-31",     1,     1,    1,  false  },
                {  L_, "1970-12-310",    1,     1,    1,  false  },

            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int         LINE        = DATA[i].d_line;
                const bsl::string INPUT       = DATA[i].d_input_p;
                const int         YEAR        = DATA[i].d_year;
                const int         MONTH       = DATA[i].d_month;
                const int         DAY         = DATA[i].d_day;
                const bool        IS_VALID    = DATA[i].d_isValid;

                const bdlt::Date EXP(YEAR, MONTH, DAY);

                bdlt::Date        mX;
                const bdlt::Date& X = mX;

                int retCode = Util::parseDefault(
                                             &mX,
                                             INPUT.c_str(),
                                             static_cast<int>(INPUT.length()));
                if (IS_VALID) {
                    ASSERTV(LINE, retCode,  0   == retCode);
                    ASSERTV(LINE, INPUT, X, EXP == X      );
                }
                else {
                    ASSERTV(LINE, retCode, 0 != retCode);
                }
            }
        }

        if (verbose) cout << "\nUsing 'DateTz'." << endl;
        {
            static const struct {
                int         d_line;      // source line number
                const char *d_input_p;   // input
                int         d_year;      // year under test
                int         d_month;     // month under test
                int         d_day;       // day under test
                int         d_offset;    // offset in minutes from UTC
                bool        d_isValid;   // isValid flag
            } DATA[] = {
                //line  input                year  month   day   off isValid
                //----  --------------       ----  -----   --- ----- -------
                // valid input
                {  L_, "0001-01-01Z",          1,     1,    1,    0,  true  },
                {  L_, "0009-09-09-02:00",     9,     9,    9, -120,  true  },
                {  L_, "0030-10-20-00:30",    30,    10,   20,  -30,  true  },
                {  L_, "0842-12-19+00:00",   842,    12,   19,    0,  true  },
                {  L_, "1847-05-19+01:30",  1847,     5,   19,   90,  true  },
                {  L_, "2000-02-29+04:00",  2000,     2,   29,  240,  true  },
                {  L_, "9999-12-31+23:59",  9999,    12,   31, 1439,  true  },

                // invalid input
                {  L_, "05-05-05Z",            1,     1,    1,    0, false  },
                {  L_, "005-05-5-02:00",       1,     1,    1,    0, false  },
                {  L_, "0001-01-01-02-00",     1,     1,    1,    0, false  },
                {  L_, "0001-01-01+02",        1,     1,    1,    0, false  },
                {  L_, "0000-01-01+02:",       1,     1,    1,    0, false  },

            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int         LINE        = DATA[i].d_line;
                const bsl::string INPUT       = DATA[i].d_input_p;
                const int         YEAR        = DATA[i].d_year;
                const int         MONTH       = DATA[i].d_month;
                const int         DAY         = DATA[i].d_day;
                const int         OFFSET      = DATA[i].d_offset;
                const bool        IS_VALID    = DATA[i].d_isValid;

                const bdlt::DateTz EXP(bdlt::Date(YEAR, MONTH, DAY), OFFSET);

                bdlt::DateTz        mX;
                const bdlt::DateTz& X = mX;

                int retCode = Util::parseDefault(
                                             &mX,
                                             INPUT.c_str(),
                                             static_cast<int>(INPUT.length()));
                if (IS_VALID) {
                    ASSERTV(LINE, retCode,       0   == retCode);
                    ASSERTV(LINE, INPUT, EXP, X, EXP == X      );
                }
                else {
                    ASSERTV(LINE, retCode, 0 != retCode);
                }
            }
        }

        if (verbose) cout << "\nUsing 'Time'." << endl;
        {
            static const struct {
                int         d_line;      // source line number
                const char *d_input_p;   // input
                int         d_hour;      // hour under test
                int         d_minutes;   // minutes under test
                int         d_seconds;   // seconds under test
                int         d_milliSecs; // milli seconds under test
                int         d_microSecs; // micro seconds under test
                bool        d_isValid;   // isValid flag
            } DATA[] = {
                //LINE INPUT                H    M    S    MS    US    VALID
                //---- -----------------    --   --   --   ---   ---   -----
                // valid input
                {  L_, "00:00:00.000000",    0,   0,   0,    0,    0,   true },
                {  L_, "01:02:03.004005",    1,   2,   3,    4,    5,   true },
                {  L_, "10:20:30.040050",   10,  20,  30,   40,   50,   true },
                {  L_, "19:43:27.805107",   19,  43,  27,  805,  107,   true },
                {  L_, "23:59:59.999999",   23,  59,  59,  999,  999,   true },
                {  L_, "24:00:00.0000001",  24,   0,   0,    0,    0,   true },

                // invalid input
                {  L_, "12:00:1",            0,   0,   0,    0,    0,  false },
                {  L_, "12:0:01",            0,   0,   0,    0,    0,  false },
                {  L_, "12:2:001",           0,   0,   0,    0,    0,  false },
                {  L_, "3:02:001",           0,   0,   0,    0,    0,  false },
                {  L_, "3:2:0001",           0,   0,   0,    0,    0,  false },
                {  L_, "20:20:61",           0,   0,   0,    0,    0,  false },
                {  L_, "03:02:001.",         0,   0,   0,    0,    0,  false },
                {  L_, "03:02:001,",         0,   0,   0,    0,    0,  false },
            };

            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int         LINE        = DATA[i].d_line;
                const bsl::string INPUT       = DATA[i].d_input_p;
                const int         HOUR        = DATA[i].d_hour;
                const int         MINUTE      = DATA[i].d_minutes;
                const int         SECOND      = DATA[i].d_seconds;
                const int         MILLISECOND = DATA[i].d_milliSecs;
                const int         MICROSECOND = DATA[i].d_microSecs;
                const bool        IS_VALID    = DATA[i].d_isValid;

                const bdlt::Time EXP(HOUR,
                                     MINUTE,
                                     SECOND,
                                     MILLISECOND,
                                     MICROSECOND);

                bdlt::Time        mX;
                const bdlt::Time& X = mX;

                int retCode = Util::parseDefault(
                                             &mX,
                                             INPUT.c_str(),
                                             static_cast<int>(INPUT.length()));
                if (IS_VALID) {
                    ASSERTV(LINE, retCode,  0   == retCode);
                    ASSERTV(LINE, INPUT, X, EXP == X      );
                }
                else {
                    ASSERTV(LINE, retCode, 0 != retCode);
                }
            }
        }

        if (verbose) cout << "\nUsing 'TimeTz'." << endl;
        {
            static const struct {
                int         d_line;      // source line number
                const char *d_input_p;   // input
                int         d_hour;      // hour under test
                int         d_minutes;   // minutes under test
                int         d_seconds;   // seconds under test
                int         d_milliSecs; // milli seconds under test
                int         d_microSecs; // micro seconds under test
                int         d_offset;    // offset in minutes from UTC
                bool        d_isValid;   // isValid flag
            } DATA[] = {
         //LINE INPUT                     H   M   S   MS   US   OFF    VALID
         //---- -----------------         --  --  --  ---  ---  -----  -----
         // valid input
         {  L_, "00:00:00.000000Z",        0,  0,  0,   0,   0,     0,  true },
         {  L_, "01:02:03.004005-23:59",   1,  2,  3,   4,   5, -1439,  true },
         {  L_, "10:20:30.040050-02:00",  10, 20, 30,  40,  50,  -120,  true },
         {  L_, "19:43:27.805107-00:30",  19, 43, 27, 805, 107,   -30,  true },
         {  L_, "23:59:59.9999991+00:00", 23, 59, 59, 999, 999,     0,  true },

         // invalid input
         {  L_, "12:00:1Z",                0,  0,  0,   0,   0,     0, false },
         {  L_, "12:0:01-23:59",           0,  0,  0,   0,   0,     0, false },
         {  L_, "00:00:00.000000+",        0,  0,  0,   0,   0,     0, false },
         {  L_, "01:02:03.004005-00",      0,  0,  0,   0,   0,     0, false },
         {  L_, "10:20:30.040050+03:",     0,  0,  0,   0,   0,     0, false }
            };

            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int         LINE        = DATA[i].d_line;
                const bsl::string INPUT       = DATA[i].d_input_p;
                const int         HOUR        = DATA[i].d_hour;
                const int         MINUTE      = DATA[i].d_minutes;
                const int         SECOND      = DATA[i].d_seconds;
                const int         MILLISECOND = DATA[i].d_milliSecs;
                const int         MICROSECOND = DATA[i].d_microSecs;
                const int         OFFSET      = DATA[i].d_offset;
                const bool        IS_VALID    = DATA[i].d_isValid;

                const bdlt::TimeTz EXP(bdlt::Time(HOUR,
                                                  MINUTE,
                                                  SECOND,
                                                  MILLISECOND,
                                                  MICROSECOND),
                                       OFFSET);

                bdlt::TimeTz        mX;
                const bdlt::TimeTz& X = mX;

                int retCode = Util::parseDefault(
                                             &mX,
                                             INPUT.c_str(),
                                             static_cast<int>(INPUT.length()));
                if (IS_VALID) {
                    ASSERTV(LINE, retCode,       0   == retCode);
                    ASSERTV(LINE, INPUT, EXP, X, EXP == X      );
                }
                else {
                    ASSERTV(LINE, retCode, 0 != retCode);
                }
            }
        }

        if (verbose) cout << "\nUsing 'Datetime'." << endl;
        {
            static const struct {
                int         d_line;      // source line number
                const char *d_input_p;   // input
                int         d_year;      // year under test
                int         d_month;     // month under test
                int         d_day;       // day under test
                int         d_hour;      // hour under test
                int         d_minutes;   // minutes under test
                int         d_seconds;   // seconds under test
                int         d_milliSecs; // milli seconds under test
                int         d_microSecs; // micro seconds under test
                bool        d_isValid;   // isValid flag
            } DATA[] = {
  //line    input year month   day   hour    min   sec    ms   us   isValid
  //----    ----- ---- -----   ---   ----    ---   ---    --   --   -------
    {   L_, "0001-01-01T00:00:00.000",
                   1,     1,    1,     0,     0,    0,    0,   0,   true   },
    {   L_, "0001-01-01T00:00:00.000000",
                   1,     1,    1,     0,     0,    0,    0,   0,   true   },

    {   L_, "0001-01-01T01:01:01.001",
                   1,     1,    1,     1,     1,    1,    1,   0,   true   },
    {   L_, "0001-01-01T01:01:01.001000",
                   1,     1,    1,     1,     1,    1,    1,   0,   true   },
    {   L_, "0001-01-01T01:23:59.059",
                   1,     1,    1,     1,    23,   59,   59,   0,   true   },
    {   L_, "0001-01-01T01:23:59.059059",
                   1,     1,    1,     1,    23,   59,   59,  59,   true   },
    {   L_, "0001-01-02T00:00:00.000",
                   1,     1,    2,     0,     0,    0,    0,   0,   true   },
    {   L_, "0001-01-02T00:00:00.000000",
                   1,     1,    2,     0,     0,    0,    0,   0,   true   },
    {   L_, "0001-01-02T01:01:01.001",
                   1,     1,    2,     1,     1,    1,    1,   0,   true   },
    {   L_, "0001-01-02T01:01:01.001000",
                   1,     1,    2,     1,     1,    1,    1,   0,   true   },

    {   L_, "0001-01-02T01:23:59.059",
                   1,     1,    2,     1,    23,   59,   59,   0,   true   },
    {   L_, "0001-01-02T01:23:59.059501",
                   1,     1,    2,     1,    23,   59,   59, 501,   true   },

    {   L_, "0001-02-01T23:59:59.000",
                   1,     2,    1,    23,    59,   59,    0,   0,   true   },
    {   L_, "0001-02-01T23:59:59.000001",
                   1,     2,    1,    23,    59,   59,    0,   1,   true   },

    {   L_, "0001-12-31T00:00:00.000",
                   1,    12,   31,     0,     0,    0,    0,   0,   true   },
    {   L_, "0001-12-31T23:59:59.000",
                   1,    12,   31,    23,    59,   59,    0,   0,   true   },

    {   L_, "0002-01-01T00:00:00.000000",
                   2,     1,    1,     0,     0,    0,    0,   0,   true   },
    {   L_, "0002-01-01T23:59:59.000000",
                   2,     1,    1,    23,    59,   59,    0,   0,   true   },

    {   L_, "0004-02-28T00:00:00.000000",
                   4,     2,   28,     0,     0,    0,    0,   0,   true   },
    {   L_, "0004-02-28T23:59:59.000000",
                   4,     2,   28,    23,    59,   59,    0,   0,   true   },
    {   L_, "0004-02-28T23:59:59.000000",
                   4,     2,   28,    23,    59,   59,    0,   0,   true   },

    {   L_, "0004-02-29T00:00:00.000000",
                   4,     2,   29,     0,     0,    0,    0,   0,   true   },
    {   L_, "0004-02-29T23:59:59.000000",
                   4,     2,   29,    23,    59,   59,    0,   0,   true   },

    {   L_, "0004-03-01T00:00:00.000000",
                   4,     3,    1,     0,     0,    0,    0,   0,   true   },
    {   L_, "0004-03-01T23:59:59.000000",
                   4,     3,    1,    23,    59,   59,    0,   0,   true   },
    {   L_, "0004-03-01T23:59:59.000000",
                   4,     3,    1,    23,    59,   59,    0,   0,   true   },

    {   L_, "0008-02-28T00:00:00.000000",
                   8,     2,   28,     0,     0,    0,    0,   0,   true   },
    {   L_, "0008-02-28T23:59:59.000000",
                   8,     2,   28,    23,    59,   59,    0,   0,   true   },

    {   L_, "0008-02-29T00:00:00.000000",
                   8,     2,   29,     0,     0,    0,    0,   0,   true   },
    {   L_, "0008-02-29T23:59:59.000000",
                   8,     2,   29,    23,    59,   59,    0,   0,   true   },

    {   L_, "0008-03-01T00:00:00.000000",
                   8,     3,    1,     0,     0,    0,    0,   0,   true   },
    {   L_, "0008-03-01T23:59:59.000000",
                   8,     3,    1,    23,    59,   59,    0,   0,   true   },

    {   L_, "0100-02-28T00:00:00.000000",
                 100,     2,   28,     0,     0,    0,    0,   0,   true   },
    {   L_, "0100-02-28T23:59:59.000000",
                 100,     2,   28,    23,    59,   59,    0,   0,   true   },
    {   L_, "0100-02-28T23:59:59.000000",
                 100,     2,   28,    23,    59,   59,    0,   0,   true   },

    {   L_, "0100-03-01T00:00:00.000000",
                 100,     3,    1,     0,     0,    0,    0,   0,   true   },
    {   L_, "0100-03-01T23:59:59.000000",
                 100,     3,    1,    23,    59,   59,    0,   0,   true   },
    {   L_, "0100-03-01T23:59:59.000000",
                 100,     3,    1,    23,    59,   59,    0,   0,   true   },

    {   L_, "0400-02-28T00:00:00.000000",
                 400,     2,   28,     0,     0,    0,    0,   0,   true   },
    {   L_, "0400-02-28T23:59:59.000000",
                 400,     2,   28,    23,    59,   59,    0,   0,   true   },
    {   L_, "0400-02-28T23:59:59.000000",
                 400,     2,   28,    23,    59,   59,    0,   0,   true   },

    {   L_, "0400-02-29T00:00:00.000000",
                 400,     2,   29,     0,     0,    0,    0,   0,   true   },
    {   L_, "0400-02-29T23:59:59.000000",
                 400,     2,   29,    23,    59,   59,    0,   0,   true   },
    {   L_, "0400-02-29T23:59:59.000000",
                 400,     2,   29,    23,    59,   59,    0,   0,   true   },

    {   L_, "0400-03-01T00:00:00.000000",
                 400,     3,    1,     0,     0,    0,    0,   0,   true   },
    {   L_, "0400-03-01T23:59:59.000000",
                 400,     3,    1,    23,    59,   59,    0,   0,   true   },
    {   L_, "0400-03-01T23:59:59.000000",
                 400,     3,    1,    23,    59,   59,    0,   0,   true   },

    {   L_, "0500-02-28T00:00:00.000000",
                 500,     2,   28,     0,     0,    0,    0,   0,   true   },
    {   L_, "0500-02-28T23:59:59.000000",
                 500,     2,   28,    23,    59,   59,    0,   0,   true   },

    {   L_, "0500-03-01T00:00:00.000000",
                 500,     3,    1,     0,     0,    0,    0,   0,   true   },
    {   L_, "0500-03-01T23:59:59.000000",
                 500,     3,    1,    23,    59,   59,    0,   0,   true   },

    {   L_, "0800-02-28T00:00:00.000000",
                 800,     2,   28,     0,     0,    0,    0,   0,   true   },
    {   L_, "0800-02-28T23:59:59.000000",
                 800,     2,   28,    23,    59,   59,    0,   0,   true   },

    {   L_, "0800-02-29T00:00:00.000000",
                 800,     2,   29,     0,     0,    0,    0,   0,   true   },
    {   L_, "0800-02-29T23:59:59.000000",
                 800,     2,   29,    23,    59,   59,    0,   0,   true   },

    {   L_, "0800-03-01T00:00:00.000000",
                 800,     3,    1,     0,     0,    0,    0,   0,   true   },
    {   L_, "0800-03-01T23:59:59.000000",
                 800,     3,    1,    23,    59,   59,    0,   0,   true   },

    {   L_, "1000-02-28T00:00:00.000000",
                1000,     2,   28,     0,     0,    0,    0,   0,   true   },
    {   L_, "1000-02-28T23:59:59.000000",
                1000,     2,   28,    23,    59,   59,    0,   0,   true   },

    {   L_, "1000-03-01T00:00:00.000000",
                1000,     3,    1,     0,     0,    0,    0,   0,   true   },
    {   L_, "1000-03-01T23:59:59.000000",
                1000,     3,    1,    23,    59,   59,    0,   0,   true   },

    {   L_, "2000-02-28T00:00:00.000000",
                2000,     2,   28,     0,     0,    0,    0,   0,   true   },
    {   L_, "2000-02-28T23:59:59.000000",
                2000,     2,   28,    23,    59,   59,    0,   0,   true   },

    {   L_, "2000-02-29T00:00:00.000000",
                2000,     2,   29,     0,     0,    0,    0,   0,   true   },
    {   L_, "2000-02-29T23:59:59.000000",
                2000,     2,   29,    23,    59,   59,    0,   0,   true   },

    {   L_, "2000-03-01T00:00:00.000000",
                2000,     3,    1,     0,     0,    0,    0,   0,   true   },
    {   L_, "2000-03-01T23:59:59.000000",
                2000,     3,    1,    23,    59,   59,    0,   0,   true   },

    {   L_, "2016-12-31T00:00:00.000000",
                2016,    12,   31,     0,     0,    0,    0,   0,   true   },
    {   L_, "2017-12-31T00:00:00.000000",
                2017,    12,   31,     0,     0,    0,    0,   0,   true   },
    {   L_, "2018-12-31T00:00:00.000000",
                2018,    12,   31,     0,     0,    0,    0,   0,   true   },
    {   L_, "2019-12-31T00:00:00.000000",
                2019,    12,   31,     0,     0,    0,    0,   0,   true   },

    {   L_, "2020-01-01T00:00:00.000000",
                2020,     1,    1,     0,     0,    0,    0,   0,   true   },
    {   L_, "2020-01-01T00:00:00.000000",
                2020,     1,    1,     0,     0,    0,    0,   0,   true   },
    {   L_, "2020-01-01T00:00:00.000000",
                2020,     1,    1,     0,     0,    0,    0,   0,   true   },

    {   L_, "2020-01-01T23:59:59.999999",
                2020,     1,    1,    23,    59,   59,  999, 999,   true   },
    {   L_, "2020-01-01T23:59:59.999999",
                2020,     1,    1,    23,    59,   59,  999, 999,   true   },
    {   L_, "2020-01-01T23:59:59.999999",
                2020,     1,    1,    23,    59,   59,  999, 999,   true   },

    {   L_, "2020-01-02T00:00:00.000000",
                2020,     1,    2,     0,     0,    0,    0,   0,   true   },
    {   L_, "2020-01-02T00:00:00.000000",
                2020,     1,    2,     0,     0,    0,    0,   0,   true   },
    {   L_, "2020-01-02T00:00:00.000000",
                2020,     1,    2,     0,     0,    0,    0,   0,   true   },

    {   L_, "2020-02-28T00:00:00.000000",
                2020,     2,   28,     0,     0,    0,    0,   0,   true   },
    {   L_, "2020-02-28T23:59:59.000000",
                2020,     2,   28,    23,    59,   59,    0,   0,   true   },
    {   L_, "2020-02-28T23:59:59.000000",
                2020,     2,   28,    23,    59,   59,    0,   0,   true   },

    {   L_, "2020-02-29T00:00:00.000000",
                2020,     2,   29,     0,     0,    0,    0,   0,   true   },
    {   L_, "2020-02-29T23:59:59.000000",
                2020,     2,   29,    23,    59,   59,    0,   0,   true   },
    {   L_, "2020-02-29T23:59:59.000000",
                2020,     2,   29,    23,    59,   59,    0,   0,   true   },

    {   L_, "2020-03-01T00:00:00.000000",
                2020,     3,    1,     0,     0,    0,    0,   0,   true   },
    {   L_, "2020-03-01T23:59:59.000000",
                2020,     3,    1,    23,    59,   59,    0,   0,   true   },
    {   L_, "2020-03-01T23:59:59.000000",
                2020,     3,    1,    23,    59,   59,    0,   0,   true   },

    {   L_, "2021-01-02T00:00:00.000000",
                2021,     1,    2,     0,     0,    0,    0,   0,   true   },
    {   L_, "2022-01-02T00:00:00.000000",
                2022,     1,    2,     0,     0,    0,    0,   0,   true   },

    {   L_, "9999-02-28T00:00:00.000000",
                9999,     2,   28,     0,     0,    0,    0,   0,   true   },
    {   L_, "9999-02-28T23:59:59.000000",
                9999,     2,   28,    23,    59,   59,    0,   0,   true   },
    {   L_, "9999-02-28T23:59:59.000000",
                9999,     2,   28,    23,    59,   59,    0,   0,   true   },

    {   L_, "9999-03-01T00:00:00.000000",
                9999,     3,    1,     0,     0,    0,    0,   0,   true   },
    {   L_, "9999-03-01T23:59:59.000000",
                9999,     3,    1,    23,    59,   59,    0,   0,   true   },
    {   L_, "9999-03-01T23:59:59.000000",
                9999,     3,    1,    23,    59,   59,    0,   0,   true   },

    {   L_, "9999-12-30T00:00:00.000000",
                9999,    12,   30,     0,     0,    0,    0,   0,   true   },
    {   L_, "9999-12-30T23:59:59.000000",
                9999,    12,   30,    23,    59,   59,    0,   0,   true   },

    {   L_, "9999-12-31T00:00:00.000000",
                9999,    12,   31,     0,     0,    0,    0,   0,   true   },
    {   L_, "9999-12-31T23:59:59.000000",
                9999,    12,   31,    23,    59,   59,    0,   0,   true   },

    {   L_, "GARBAGE",
                   1,     1,    1,    24,     0,    0,    0,   0,   false  },
    {   L_, ".9999",
                   1,     1,    1,    24,     0,    0,    0,   0,   false  },
    {   L_, "23:59:59.9999",
                   1,     1,    1,    24,     0,    0,    0,   0,   false  },
    {   L_, "T23:59:59.9999",
                   1,     1,    1,    24,     0,    0,    0,   0,   false  },
    {   L_, "01T23:59:59.9999",
                   1,     1,    1,    24,     0,    0,    0,   0,   false  },
    {   L_, "01-01T23:59:59.9999",
                   1,     1,    1,    24,     0,    0,    0,   0,   false  },
    {   L_, "0001-01-01T00:00.000",
                   1,     1,    1,    24,     0,    0,    0,   0,   false  },

    {   L_, "0000-01-01T00:00:00.000+00:00",
                   1,     1,    1,    24,     0,    0,    0,   0,   false  },
    {   L_, "0000-01-01T00:00:00.000Z",
                   1,     1,    1,    24,     0,    0,    0,   0,   false  },
    {   L_, "0000-01-01T00:00:00.000z",
                   1,     1,    1,    24,     0,    0,    0,   0,   false  },
    {   L_, "0000-01-01T00:00:00.000-00:00",
                   1,     1,    1,    24,     0,    0,    0,   0,   false  },

    {   L_, "0001-00-01T00:00:00.000",
                   1,     1,    1,    24,     0,    0,    0,   0,   false  },
    {   L_, "0000-01-01T00:00:00.000000",
                   1,     1,    1,    24,     0,    0,    0,   0,   false  },
    {   L_, "0000-00-00T00:00:00.000",
                   1,     1,    1,    24,     0,    0,    0,   0,   false  },
    {   L_, "0001-13-00T00:00:00.000",
                   1,     1,    1,    24,     0,    0,    0,   0,   false  },
    {   L_, "0001-01-32T00:00:00.000",
                   1,     1,    1,    24,     0,    0,    0,   0,   false  },
    {   L_, "0001-01-01T25:00:00.000",
                   1,     1,    1,    24,     0,    0,    0,   0,   false  },
    {   L_, "0001-01-01T00:61:00.000",
                   1,     1,    1,    24,     0,    0,    0,   0,   false  },
    {   L_, "0001-01-01T00:00:61.000",
                   1,     1,    1,    24,     0,    0,    0,   0,   false  },

        };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int         LINE        = DATA[i].d_line;
                const bsl::string INPUT       = DATA[i].d_input_p;
                const int         YEAR        = DATA[i].d_year;
                const int         MONTH       = DATA[i].d_month;
                const int         DAY         = DATA[i].d_day;
                const int         HOUR        = DATA[i].d_hour;
                const int         MINUTE      = DATA[i].d_minutes;
                const int         SECOND      = DATA[i].d_seconds;
                const int         MILLISECOND = DATA[i].d_milliSecs;
                const int         MICROSECOND = DATA[i].d_microSecs;
                const bool        IS_VALID    = DATA[i].d_isValid;

                bdlt::Datetime exp(YEAR, MONTH, DAY,
                                   HOUR, MINUTE, SECOND,
                                   MILLISECOND, MICROSECOND);
                const bdlt::Datetime& EXP = exp;

                bdlt::Datetime mX; const bdlt::Datetime& X = mX;

                int retCode = Util::parseDefault(
                                             &mX,
                                             INPUT.c_str(),
                                             static_cast<int>(INPUT.length()));
                if (IS_VALID) {
                    LOOP2_ASSERT(LINE, retCode, 0 == retCode);
                    LOOP3_ASSERT(LINE, INPUT, X, EXP == X);
                }
                else {
                    LOOP2_ASSERT(LINE, retCode, 0 != retCode);
                }
            }
        }

        if (verbose) cout << "\nUsing 'DatetimeTz'." << endl;
        {
            static const struct {
                int         d_line;      // source line number
                const char *d_input_p;   // input
                int         d_year;      // year under test
                int         d_month;     // month under test
                int         d_day;       // day under test
                int         d_hour;      // hour under test
                int         d_minutes;   // minutes under test
                int         d_seconds;   // seconds under test
                int         d_milliSecs; // milli seconds under test
                int         d_microSecs; // micro seconds under test
                int         d_tzoffset;  // time zone offset
                bool        d_isValid;   // isValid flag
            } DATA[] = {
//line  input year month day   hour    min   sec    ms    us  offset isValid
//----  ----- ---- ----- ---   ----    ---   ---    --    --  ------ -------
    {   L_, "0001-01-01T00:00:00.000+00:00",
              1,     1,    1,     0,     0,    0,    0,   0,      0,  true   },
    {   L_, "0001-01-01T00:00:00.000+00:45",
              1,     1,    1,     0,     0,    0,    0,   0,     45,  true   },
    {   L_, "0001-01-01T00:00:00.000-23:59",
              1,     1,    1,     0,     0,    0,    0,   0,  -1439,  true   },
    {   L_, "0001-01-01T00:00:00.000000+00:00",
              1,     1,    1,     0,     0,    0,    0,   0,      0,  true   },
    {   L_, "0001-01-01T00:00:00.000000+00:45",
              1,     1,    1,     0,     0,    0,    0,   0,     45,  true   },
    {   L_, "0001-01-01T00:00:00.000000-23:59",
              1,     1,    1,     0,     0,    0,    0,   0,  -1439,  true   },

    {   L_, "0001-01-01T01:01:01.001+00:00",
              1,     1,    1,     1,     1,    1,    1,   0,      0,  true   },
    {   L_, "0001-01-01T01:01:01.001+08:20",
              1,     1,    1,     1,     1,    1,    1,   0,    500,  true   },
    {   L_, "0001-01-01T01:01:01.001000+00:00",
              1,     1,    1,     1,     1,    1,    1,   0,      0,  true   },
    {   L_, "0001-01-01T01:01:01.001000+08:20",
              1,     1,    1,     1,     1,    1,    1,   0,    500,  true   },
    {   L_, "0001-01-01T00:00:00.000000-23:59",
              1,     1,    1,     0,     0,    0,    0,   0,  -1439,  true   },
    {   L_, "0001-01-01T01:23:59.059059+00:00",
              1,     1,    1,     1,    23,   59,   59,  59,      0,  true   },
    {   L_, "0001-01-01T01:23:59.059059+23:59",
              1,     1,    1,     1,    23,   59,   59,  59,   1439,  true   },
    {   L_, "0001-01-01T01:23:59.059059-23:59",
              1,     1,    1,     1,    23,   59,   59,  59,  -1439,  true   },
    {   L_, "0001-01-02T00:00:00.000000+00:00",
              1,     1,    2,     0,     0,    0,    0,   0,      0,  true   },
    {   L_, "0001-01-02T00:00:00.000000+23:59",
              1,     1,    2,     0,     0,    0,    0,   0,   1439,  true   },
    {   L_, "0001-01-02T00:00:00.000000-23:59",
              1,     1,    2,     0,     0,    0,    0,   0,  -1439,  true   },
    {   L_, "0001-01-02T01:01:01.000001+00:00",
              1,     1,    2,     1,     1,    1,    0,   1,      0,  true   },
    {   L_, "0001-01-02T01:01:01.000001+08:20",
              1,     1,    2,     1,     1,    1,    0,   1,    500,  true   },

    {   L_, "0001-01-02T01:23:59.059168+00:00",
              1,     1,    2,     1,    23,   59,   59, 168,      0,  true   },
    {   L_, "0001-01-02T01:23:59.059168+08:20",
              1,     1,    2,     1,    23,   59,   59, 168,    500,  true   },
    {   L_, "0001-01-02T01:23:59.059168-08:20",
              1,     1,    2,     1,    23,   59,   59, 168,   -500,  true   },
    {   L_, "0001-01-10T00:00:00.000000+00:00",
              1,     1,   10,     0,     0,    0,    0,   0,      0,  true   },
    {   L_, "0001-01-10T01:01:01.001+01:39",
              1,     1,   10,     1,     1,    1,    1,   0,     99,  true   },
    {   L_, "0001-01-10T01:01:01.000001+01:39",
              1,     1,   10,     1,     1,    1,    0,   1,     99,  true   },

    {   L_, "0001-01-30T00:00:00.000000+00:00",
              1,     1,   30,     0,     0,    0,    0,   0,      0,  true   },
    {   L_, "0001-01-31T00:00:00.000000+23:59",
              1,     1,   31,     0,     0,    0,    0,   0,   1439,  true   },
    {   L_, "0001-01-31T00:00:00.000000-23:59",
              1,     1,   31,     0,     0,    0,    0,   0,  -1439,  true   },

    {   L_, "0001-02-01T00:00:00.000000+00:00",
              1,     2,    1,     0,     0,    0,    0,   0,      0,  true   },
    {   L_, "0001-02-01T23:59:59.000000+23:59",
              1,     2,    1,    23,    59,   59,    0,   0,   1439,  true   },

    {   L_, "0001-12-31T00:00:00.000000+00:00",
              1,    12,   31,     0,     0,    0,    0,   0,      0,  true   },
    {   L_, "0001-12-31T23:59:59.000000+23:59",
              1,    12,   31,    23,    59,   59,    0,   0,   1439,  true   },

    {   L_, "0002-01-01T00:00:00.000000+00:00",
              2,     1,    1,     0,     0,    0,    0,   0,      0,  true   },
    {   L_, "0002-01-01T23:59:59.000000+23:59",
              2,     1,    1,    23,    59,   59,    0,   0,   1439,  true   },

    {   L_, "0004-01-01T00:00:00.000000+00:00",
              4,     1,    1,     0,     0,    0,    0,   0,      0,  true   },
    {   L_, "0004-01-01T23:59:59.000000+23:59",
              4,     1,    1,    23,    59,   59,    0,   0,   1439,  true   },

    {   L_, "0004-02-28T00:00:00.000000+00:00",
              4,     2,   28,     0,     0,    0,    0,   0,      0,  true   },
    {   L_, "0004-02-28T23:59:59.000000+23:59",
              4,     2,   28,    23,    59,   59,    0,   0,   1439,  true   },
    {   L_, "0004-02-28T23:59:59.000000-23:59",
              4,     2,   28,    23,    59,   59,    0,   0,  -1439,  true   },

    {   L_, "0004-02-29T00:00:00.000000+00:00",
              4,     2,   29,     0,     0,    0,    0,   0,      0,  true   },
    {   L_, "0004-02-29T23:59:59.000000+23:59",
              4,     2,   29,    23,    59,   59,    0,   0,   1439,  true   },
    {   L_, "0004-02-29T23:59:59.000000-23:59",
              4,     2,   29,    23,    59,   59,    0,   0,  -1439,  true   },

    {   L_, "0004-03-01T00:00:00.000000+00:00",
              4,     3,    1,     0,     0,    0,    0,   0,      0,  true   },
    {   L_, "0004-03-01T23:59:59.000000+23:59",
              4,     3,    1,    23,    59,   59,    0,   0,   1439,  true   },
    {   L_, "0004-03-01T23:59:59.000000-23:59",
              4,     3,    1,    23,    59,   59,    0,   0,  -1439,  true   },

    {   L_, "0008-02-28T00:00:00.000000+00:00",
              8,     2,   28,     0,     0,    0,    0,   0,      0,  true   },
    {   L_, "0008-02-28T23:59:59.000000+23:59",
              8,     2,   28,    23,    59,   59,    0,   0,   1439,  true   },

    {   L_, "0008-02-29T00:00:00.000000+00:00",
              8,     2,   29,     0,     0,    0,    0,   0,      0,  true   },
    {   L_, "0008-02-29T23:59:59.000000+23:59",
              8,     2,   29,    23,    59,   59,    0,   0,   1439,  true   },

    {   L_, "0008-03-01T00:00:00.000000+00:00",
              8,     3,    1,     0,     0,    0,    0,   0,      0,  true   },
    {   L_, "0008-03-01T23:59:59.000000+23:59",
              8,     3,    1,    23,    59,   59,    0,   0,   1439,  true   },

    {   L_, "0100-02-28T00:00:00.000000+00:00",
            100,     2,   28,     0,     0,    0,    0,   0,      0,  true   },
    {   L_, "0100-02-28T23:59:59.000000+23:59",
            100,     2,   28,    23,    59,   59,    0,   0,   1439,  true   },
    {   L_, "0100-02-28T23:59:59.000000-23:59",
            100,     2,   28,    23,    59,   59,    0,   0,  -1439,  true   },

    {   L_, "0100-03-01T00:00:00.000000+00:00",
            100,     3,    1,     0,     0,    0,    0,   0,      0,  true   },
    {   L_, "0100-03-01T23:59:59.000000+23:59",
            100,     3,    1,    23,    59,   59,    0,   0,   1439,  true   },
    {   L_, "0100-03-01T23:59:59.000000-23:59",
            100,     3,    1,    23,    59,   59,    0,   0,  -1439,  true   },

    {   L_, "0400-02-28T00:00:00.000000+00:00",
            400,     2,   28,     0,     0,    0,    0,   0,      0,  true   },
    {   L_, "0400-02-28T23:59:59.000000+23:59",
            400,     2,   28,    23,    59,   59,    0,   0,   1439,  true   },
    {   L_, "0400-02-28T23:59:59.000000-23:59",
            400,     2,   28,    23,    59,   59,    0,   0,  -1439,  true   },

    {   L_, "0400-02-29T00:00:00.000000+00:00",
            400,     2,   29,     0,     0,    0,    0,   0,      0,  true   },
    {   L_, "0400-02-29T23:59:59.000000+23:59",
            400,     2,   29,    23,    59,   59,    0,   0,   1439,  true   },
    {   L_, "0400-02-29T23:59:59.000000-23:59",
            400,     2,   29,    23,    59,   59,    0,   0,  -1439,  true   },

    {   L_, "0400-03-01T00:00:00.000000+00:00",
            400,     3,    1,     0,     0,    0,    0,   0,      0,  true   },
    {   L_, "0400-03-01T23:59:59.000000+23:59",
            400,     3,    1,    23,    59,   59,    0,   0,   1439,  true   },
    {   L_, "0400-03-01T23:59:59.000000-23:59",
            400,     3,    1,    23,    59,   59,    0,   0,  -1439,  true   },

    {   L_, "0500-02-28T00:00:00.000000+00:00",
            500,     2,   28,     0,     0,    0,    0,   0,      0,  true   },
    {   L_, "0500-02-28T23:59:59.000000+23:59",
            500,     2,   28,    23,    59,   59,    0,   0,   1439,  true   },

    {   L_, "0500-03-01T00:00:00.000000+00:00",
            500,     3,    1,     0,     0,    0,    0,   0,      0,  true   },
    {   L_, "0500-03-01T23:59:59.000000+23:59",
            500,     3,    1,    23,    59,   59,    0,   0,   1439,  true   },

    {   L_, "0800-02-28T00:00:00.000000+00:00",
            800,     2,   28,     0,     0,    0,    0,   0,      0,  true   },
    {   L_, "0800-02-28T23:59:59.000000+23:59",
            800,     2,   28,    23,    59,   59,    0,   0,   1439,  true   },

    {   L_, "0800-02-29T00:00:00.000000+00:00",
            800,     2,   29,     0,     0,    0,    0,   0,      0,  true   },
    {   L_, "0800-02-29T23:59:59.000000+23:59",
            800,     2,   29,    23,    59,   59,    0,   0,   1439,  true   },

    {   L_, "0800-03-01T00:00:00.000000+00:00",
            800,     3,    1,     0,     0,    0,    0,   0,      0,  true   },
    {   L_, "0800-03-01T23:59:59.000000+23:59",
            800,     3,    1,    23,    59,   59,    0,   0,   1439,  true   },

    {   L_, "1000-02-28T00:00:00.000000+00:00",
           1000,     2,   28,     0,     0,    0,    0,   0,      0,  true   },
    {   L_, "1000-02-28T23:59:59.000000+23:59",
           1000,     2,   28,    23,    59,   59,    0,   0,   1439,  true   },

    {   L_, "1000-03-01T00:00:00.000000+00:00",
           1000,     3,    1,     0,     0,    0,    0,   0,      0,  true   },
    {   L_, "1000-03-01T23:59:59.000000+23:59",
           1000,     3,    1,    23,    59,   59,    0,   0,   1439,  true   },

    {   L_, "2000-02-28T00:00:00.000000+00:00",
           2000,     2,   28,     0,     0,    0,    0,   0,      0,  true   },
    {   L_, "2000-02-28T23:59:59.000000+23:59",
           2000,     2,   28,    23,    59,   59,    0,   0,   1439,  true   },

    {   L_, "2000-02-29T00:00:00.000000+00:00",
           2000,     2,   29,     0,     0,    0,    0,   0,      0,  true   },
    {   L_, "2000-02-29T23:59:59.000000+23:59",
           2000,     2,   29,    23,    59,   59,    0,   0,   1439,  true   },

    {   L_, "2000-03-01T00:00:00.000000+00:00",
           2000,     3,    1,     0,     0,    0,    0,   0,      0,  true   },
    {   L_, "2000-03-01T23:59:59.000000+23:59",
           2000,     3,    1,    23,    59,   59,    0,   0,   1439,  true   },

    {   L_, "2016-12-31T00:00:00.000000+00:00",
           2016,    12,   31,     0,     0,    0,    0,   0,      0,  true   },
    {   L_, "2017-12-31T00:00:00.000000+00:00",
           2017,    12,   31,     0,     0,    0,    0,   0,      0,  true   },
    {   L_, "2018-12-31T00:00:00.000000+00:00",
           2018,    12,   31,     0,     0,    0,    0,   0,      0,  true   },
    {   L_, "2019-12-31T00:00:00.000000+00:00",
           2019,    12,   31,     0,     0,    0,    0,   0,      0,  true   },

    {   L_, "2020-01-01T00:00:00.000000+00:00",
           2020,     1,    1,     0,     0,    0,    0,   0,      0,  true   },
    {   L_, "2020-01-01T00:00:00.000000+23:59",
           2020,     1,    1,     0,     0,    0,    0,   0,   1439,  true   },
    {   L_, "2020-01-01T00:00:00.000000-23:59",
           2020,     1,    1,     0,     0,    0,    0,   0,  -1439,  true   },

    {   L_, "2020-01-01T23:59:59.999999+00:00",
           2020,     1,    1,    23,    59,   59,  999, 999,      0,  true   },
    {   L_, "2020-01-01T23:59:59.999999+23:59",
           2020,     1,    1,    23,    59,   59,  999, 999,   1439,  true   },
    {   L_, "2020-01-01T23:59:59.999999-23:59",
           2020,     1,    1,    23,    59,   59,  999, 999,  -1439,  true   },

    {   L_, "2020-01-02T00:00:00.000000+00:00",
           2020,     1,    2,     0,     0,    0,    0,   0,      0,  true   },
    {   L_, "2020-01-02T00:00:00.000000+23:59",
           2020,     1,    2,     0,     0,    0,    0,   0,   1439,  true   },
    {   L_, "2020-01-02T00:00:00.000000-23:59",
           2020,     1,    2,     0,     0,    0,    0,   0,  -1439,  true   },

    {   L_, "2020-02-28T00:00:00.000000+00:00",
           2020,     2,   28,     0,     0,    0,    0,   0,      0,  true   },
    {   L_, "2020-02-28T23:59:59.000000+23:59",
           2020,     2,   28,    23,    59,   59,    0,   0,   1439,  true   },
    {   L_, "2020-02-28T23:59:59.000000-23:59",
           2020,     2,   28,    23,    59,   59,    0,   0,  -1439,  true   },

    {   L_, "2020-02-29T00:00:00.000000+00:00",
           2020,     2,   29,     0,     0,    0,    0,   0,      0,  true   },
    {   L_, "2020-02-29T23:59:59.000000+23:59",
           2020,     2,   29,    23,    59,   59,    0,   0,   1439,  true   },
    {   L_, "2020-02-29T23:59:59.000000-23:59",
           2020,     2,   29,    23,    59,   59,    0,   0,  -1439,  true   },

    {   L_, "2020-03-01T00:00:00.000000+00:00",
           2020,     3,    1,     0,     0,    0,    0,   0,      0,  true   },
    {   L_, "2020-03-01T23:59:59.000000+23:59",
           2020,     3,    1,    23,    59,   59,    0,   0,   1439,  true   },
    {   L_, "2020-03-01T23:59:59.000000-23:59",
           2020,     3,    1,    23,    59,   59,    0,   0,  -1439,  true   },

    {   L_, "2021-01-02T00:00:00.000000+00:00",
           2021,     1,    2,     0,     0,    0,    0,   0,      0,  true   },
    {   L_, "2022-01-02T00:00:00.000000+00:00",
           2022,     1,    2,     0,     0,    0,    0,   0,      0,  true   },

    {   L_, "9999-02-28T00:00:00.000000+00:00",
           9999,     2,   28,     0,     0,    0,    0,   0,      0,  true   },
    {   L_, "9999-02-28T23:59:59.000000+23:59",
           9999,     2,   28,    23,    59,   59,    0,   0,   1439,  true   },
    {   L_, "9999-02-28T23:59:59.000000-23:59",
           9999,     2,   28,    23,    59,   59,    0,   0,  -1439,  true   },

    {   L_, "9999-03-01T00:00:00.000000+00:00",
           9999,     3,    1,     0,     0,    0,    0,   0,      0,  true   },
    {   L_, "9999-03-01T23:59:59.000000+23:59",
           9999,     3,    1,    23,    59,   59,    0,   0,   1439,  true   },
    {   L_, "9999-03-01T23:59:59.000000-23:59",
           9999,     3,    1,    23,    59,   59,    0,   0,  -1439,  true   },

    {   L_, "9999-12-30T00:00:00.000000+00:00",
           9999,    12,   30,     0,     0,    0,    0,   0,      0,  true   },
    {   L_, "9999-12-30T23:59:59.000000+23:59",
           9999,    12,   30,    23,    59,   59,    0,   0,   1439,  true   },

    {   L_, "9999-12-31T00:00:00.000000+00:00",
           9999,    12,   31,     0,     0,    0,    0,   0,      0,  true   },
    {   L_, "9999-12-31T23:59:59.000000+23:59",
           9999,    12,   31,    23,    59,   59,    0,   0,   1439,  true   },

    {   L_, "0000-01-01T00:00:00.000000+00:00",
              1,     1,    1,    24,     0,    0,    0,   0,      0,  false  },
    {   L_, "0001-00-01T00:00:00.000000+00:00",
              1,     1,    1,    24,     0,    0,    0,   0,      0,  false  },
    {   L_, "0001-01-00T00:00:00.000000+00:00",
              1,     1,    1,    24,     0,    0,    0,   0,      0,  false  },
    {   L_, "0000-00-00T00:00:00.000000+00:00",
              1,     1,    1,    24,     0,    0,    0,   0,      0,  false  },
    {   L_, "0001-13-00T00:00:00.000000+00:00",
              1,     1,    1,    24,     0,    0,    0,   0,      0,  false  },
    {   L_, "0001-01-32T00:00:00.000000+00:00",
              1,     1,    1,    24,     0,    0,    0,   0,      0,  false  },
    {   L_, "0001-01-01T25:00:00.000000+00:00",
              1,     1,    1,    24,     0,    0,    0,   0,      0,  false  },
    {   L_, "0001-01-01T00:61:00.000000+00:00",
              1,     1,    1,    24,     0,    0,    0,   0,      0,  false  },
    {   L_, "0001-01-01T00:00:61.000000+00:00",
              1,     1,    1,    24,     0,    0,    0,   0,      0,  false  },
    {   L_, "0001-01-01T00:00:00.000000+24:00",
              1,     1,    1,    24,     0,    0,    0,   0,      0,  false  },
    {   L_, "0001-01-01T00:00:00.000000+00:61",
              1,     1,    1,    24,     0,    0,    0,   0,      0,  false  },
    {   L_, "0001-01-01T00:00:00.000000-24:00",
              1,     1,    1,    24,     0,    0,    0,   0,      0,  false  },
    {   L_, "0001-01-01T00:00:00.000000-00:61",
              1,     1,    1,    24,     0,    0,    0,   0,      0,  false  },

    {   L_, "GARBAGE",
              1,     1,    1,    24,     0,    0,    0,   0,      0,  false  },
    {   L_, ".9999",
              1,     1,    1,    24,     0,    0,    0,   0,      0,  false  },
    {   L_, "23:59:59.9999",
              1,     1,    1,    24,     0,    0,    0,   0,      0,  false  },
    {   L_, "T23:59:59.9999",
              1,     1,    1,    24,     0,    0,    0,   0,      0,  false  },
    {   L_, "01T23:59:59.9999",
              1,     1,    1,    24,     0,    0,    0,   0,      0,  false  },
    {   L_, "01-01T23:59:59.9999",
              1,     1,    1,    24,     0,    0,    0,   0,      0,  false  },
    {   L_, "0001-01-01T00:00.000",
              1,     1,    1,    24,     0,    0,    0,   0,      0,  false  },
    {   L_, "0001-01-01T00:00.000000",
              1,     1,    1,    24,     0,    0,    0,   0,      0,  false  },
    {   L_, "01-01-01T23:59:59.9999",
              1,     1,    1,    24,     0,    0,    0,   0,      0,  false  },
    {   L_, "01-01-01T23:59:59.9999999",
              1,     1,    1,    24,     0,    0,    0,   0,      0,  false  },
        };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int         LINE         = DATA[i].d_line;
                const bsl::string INPUT        = DATA[i].d_input_p;
                const int         YEAR         = DATA[i].d_year;
                const int         MONTH        = DATA[i].d_month;
                const int         DAY          = DATA[i].d_day;
                const int         HOUR         = DATA[i].d_hour;
                const int         MINUTE       = DATA[i].d_minutes;
                const int         SECOND       = DATA[i].d_seconds;
                const int         MILLISECOND  = DATA[i].d_milliSecs;
                const int         MICROSECOND  = DATA[i].d_microSecs;
                const int         OFFSET       = DATA[i].d_tzoffset;
                const bool        IS_VALID     = DATA[i].d_isValid;

                bdlt::Datetime dt(YEAR, MONTH, DAY,
                                  HOUR, MINUTE, SECOND,
                                  MILLISECOND, MICROSECOND);
                const bdlt::Datetime& DT = dt;
                bdlt::DatetimeTz exp(DT, OFFSET);
                const bdlt::DatetimeTz& EXP = exp;

                bdlt::DatetimeTz mX; const bdlt::DatetimeTz& X = mX;

                int retCode = Util::parseDefault(
                                             &mX,
                                             INPUT.c_str(),
                                             static_cast<int>(INPUT.length()));
                if (IS_VALID) {
                    LOOP2_ASSERT(LINE, retCode, 0 == retCode);
                    LOOP3_ASSERT(LINE, INPUT, X, EXP == X);
                }
                else {
                    LOOP2_ASSERT(LINE, retCode, 0 != retCode);
                }
            }
        }

        if (verbose) cout << "\nUsing 'DateOrDateTz'." << endl;
        {
            static const struct {
                int         d_line;      // source line number

                const char *d_input_p;   // input

                int         d_year;      // year under test

                int         d_month;     // month under test

                int         d_day;       // day under test

                int         d_offset;    // offset in minutes from UTC

                bool        d_isValid;   // isValid flag

                bool        d_isDateTz;  // flag indicating whether the result
                                         // object is expected to contain
                                         // 'DateTz' or 'Date' object
            } DATA[] = {
            //line  input                year  month   day   off valid DateTz
            //----  --------------       ----  -----   --- ----- ----- ------
            // valid input
            {  L_, "0001-01-01",          1,     1,    1,    0,  true, false },
            {  L_, "0009-09-09",          9,     9,    9,    0,  true, false },
            {  L_, "0030-10-20",         30,    10,   20,    0,  true, false },
            {  L_, "0842-12-19",        842,    12,   19,    0,  true, false },
            {  L_, "1847-05-19",       1847,     5,   19,    0,  true, false },
            {  L_, "2000-02-29",       2000,     2,   29,    0,  true, false },
            {  L_, "9999-12-31",       9999,    12,   31,    0,  true, false },

            {  L_, "0001-01-01Z",         1,     1,    1,    0,  true,  true },
            {  L_, "0009-09-09-02:00",    9,     9,    9, -120,  true,  true },
            {  L_, "0030-10-20-00:30",   30,    10,   20,  -30,  true,  true },
            {  L_, "0842-12-19+00:00",  842,    12,   19,    0,  true,  true },
            {  L_, "1847-05-19+01:30", 1847,     5,   19,   90,  true,  true },
            {  L_, "2000-02-29+04:00", 2000,     2,   29,  240,  true,  true },
            {  L_, "9999-12-31+23:59", 9999,    12,   31, 1439,  true,  true },

            // invalid input
            {  L_, "05-05-05",            1,     1,    1,    0, false, false },
            {  L_, "005-05-5",            1,     1,    1,    0, false, false },
            {  L_, "0001 01-01",          1,     1,    1,    0, false, false },
            {  L_, "0001-01:01",          1,     1,    1,    0, false, false },
            {  L_, "0000-01-01",          1,     1,    1,    0, false, false },
            {  L_, "0001-01-32",          1,     1,    1,    0, false, false },
            {  L_, "0001-04-31",          1,     1,    1,    0, false, false },
            {  L_, "1970-12-310",         1,     1,    1,    0, false, false },

            {  L_, "05-05-05Z",           1,     1,    1,    0, false, false },
            {  L_, "005-05-5-02:00",      1,     1,    1,    0, false, false },
            {  L_, "0001-01-01-02-00",    1,     1,    1,    0, false, false },
            {  L_, "0001-01-01+02",       1,     1,    1,    0, false, false },
            {  L_, "0000-01-01+02:",      1,     1,    1,    0, false, false },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int         LINE      = DATA[i].d_line;
                const bsl::string INPUT     = DATA[i].d_input_p;
                const int         YEAR      = DATA[i].d_year;
                const int         MONTH     = DATA[i].d_month;
                const int         DAY       = DATA[i].d_day;
                const int         OFFSET    = DATA[i].d_offset;
                const bool        IS_VALID  = DATA[i].d_isValid;
                const bool        IS_DATETZ = DATA[i].d_isDateTz;

                const bdlt::Date   EXP_DATE(YEAR, MONTH, DAY);
                const bdlt::DateTz EXP_DATETZ(EXP_DATE, OFFSET);

                bdlb::Variant2<bdlt::Date, bdlt::DateTz>        mX;
                const bdlb::Variant2<bdlt::Date, bdlt::DateTz>& X = mX;

                int retCode = Util::parseDefault(
                                             &mX,
                                             INPUT.c_str(),
                                             static_cast<int>(INPUT.length()));
                if (IS_VALID) {
                    ASSERTV(LINE, retCode, 0 == retCode);
                    if (IS_DATETZ) {
                        ASSERTV(LINE, X.is<bdlt::DateTz>());
                        ASSERTV(LINE, INPUT, EXP_DATETZ, X.the<bdlt::DateTz>(),
                                EXP_DATETZ == X.the<bdlt::DateTz>());
                    }
                    else {
                        ASSERTV(LINE, X.is<bdlt::Date>());
                        ASSERTV(LINE, INPUT, EXP_DATE, X.the<bdlt::Date>(),
                                EXP_DATE == X.the<bdlt::Date>());
                    }
                }
                else {
                    ASSERTV(LINE, retCode, 0 != retCode);
                    ASSERTV(LINE, X.isUnset());
                }
            }
        }

        if (verbose) cout << "\nUsing 'TimeOrTimeTz'." << endl;
        {
            static const struct {
                int         d_line;      // source line number

                const char *d_input_p;   // input

                int         d_hour;      // hour under test

                int         d_minutes;   // minutes under test

                int         d_seconds;   // seconds under test

                int         d_milliSecs; // milli seconds under test

                int         d_microSecs; // micro seconds under test

                int         d_offset;    // offset in minutes from UTC

                bool        d_isValid;   // isValid flag

                bool        d_isTimeTz;  // flag indicating whether the result
                                         // object is expected to contain
                                         // 'TimeTz' or 'Time' object
            } DATA[] = {
  //LINE INPUT                     H   M   S   MS   US   OFF    VALID  TIMETZ
  //---- -----------------         --  --  --  ---  ---  -----  -----  ------
  // valid input
  {  L_, "00:00:00.000000",         0,  0,  0,   0,   0,     0,  true, false },
  {  L_, "01:02:03.004005",         1,  2,  3,   4,   5,     0,  true, false },
  {  L_, "10:20:30.040050",        10, 20, 30,  40,  50,     0,  true, false },
  {  L_, "19:43:27.805107",        19, 43, 27, 805, 107,     0,  true, false },
  {  L_, "23:59:59.999999",        23, 59, 59, 999, 999,     0,  true, false },
  {  L_, "24:00:00.0000001",       24,  0,  0,   0,   0,     0,  true, false },

  {  L_, "00:00:00.000000Z",        0,  0,  0,   0,   0,     0,  true,  true },
  {  L_, "01:02:03.004005-23:59",   1,  2,  3,   4,   5, -1439,  true,  true },
  {  L_, "10:20:30.040050-02:00",  10, 20, 30,  40,  50,  -120,  true,  true },
  {  L_, "19:43:27.805107-00:30",  19, 43, 27, 805, 107,   -30,  true,  true },
  {  L_, "23:59:59.9999991+00:00", 23, 59, 59, 999, 999,     0,  true,  true },

  // invalid input
  {  L_, "12:00:1",                 0,  0,  0,   0,   0,     0, false, false },
  {  L_, "12:0:01",                 0,  0,  0,   0,   0,     0, false, false },
  {  L_, "12:2:001",                0,  0,  0,   0,   0,     0, false, false },
  {  L_, "3:02:001",                0,  0,  0,   0,   0,     0, false, false },
  {  L_, "3:2:0001",                0,  0,  0,   0,   0,     0, false, false },
  {  L_, "20:20:61",                0,  0,  0,   0,   0,     0, false, false },
  {  L_, "03:02:001.",              0,  0,  0,   0,   0,     0, false, false },
  {  L_, "03:02:001,",              0,  0,  0,   0,   0,     0, false, false },

  {  L_, "12:00:1Z",                0,  0,  0,   0,   0,     0, false, false },
  {  L_, "12:0:01-23:59",           0,  0,  0,   0,   0,     0, false, false },
  {  L_, "00:00:00.000000+",        0,  0,  0,   0,   0,     0, false, false },
  {  L_, "01:02:03.004005-00",      0,  0,  0,   0,   0,     0, false, false },
  {  L_, "10:20:30.040050+03:",     0,  0,  0,   0,   0,     0, false, false }
            };

            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int         LINE        = DATA[i].d_line;
                const bsl::string INPUT       = DATA[i].d_input_p;
                const int         HOUR        = DATA[i].d_hour;
                const int         MINUTE      = DATA[i].d_minutes;
                const int         SECOND      = DATA[i].d_seconds;
                const int         MILLISECOND = DATA[i].d_milliSecs;
                const int         MICROSECOND = DATA[i].d_microSecs;
                const int         OFFSET      = DATA[i].d_offset;
                const bool        IS_VALID    = DATA[i].d_isValid;
                const bool        IS_TIMETZ   = DATA[i].d_isTimeTz;

                const bdlt::Time EXP_TIME(HOUR,
                                          MINUTE,
                                          SECOND,
                                          MILLISECOND,
                                          MICROSECOND);
                const bdlt::TimeTz EXP_TIMETZ(EXP_TIME, OFFSET);

                bdlb::Variant2<bdlt::Time, bdlt::TimeTz>        mX;
                const bdlb::Variant2<bdlt::Time, bdlt::TimeTz>& X = mX;

                int retCode = Util::parseDefault(
                                             &mX,
                                             INPUT.c_str(),
                                             static_cast<int>(INPUT.length()));
                if (IS_VALID) {
                    ASSERTV(LINE, retCode, 0 == retCode);
                    if (IS_TIMETZ) {
                        ASSERTV(LINE, X.is<bdlt::TimeTz>());
                        ASSERTV(LINE, INPUT, EXP_TIMETZ, X.the<bdlt::TimeTz>(),
                                EXP_TIMETZ == X.the<bdlt::TimeTz>());
                    }
                    else {
                        ASSERTV(LINE, X.is<bdlt::Time>());
                        ASSERTV(LINE, INPUT, EXP_TIME, X.the<bdlt::Time>(),
                                EXP_TIME == X.the<bdlt::Time>());
                    }
                }
                else {
                    ASSERTV(LINE, retCode, 0 != retCode);
                    ASSERTV(LINE, X.isUnset());
                }
            }
        }

        if (verbose) cout << "\nUsing 'DatetimeTz'." << endl;
        {
            static const struct {
                int         d_line;          // source line number

                const char *d_input_p;       // input

                int         d_year;          // year under test

                int         d_month;         // month under test

                int         d_day;           // day under test

                int         d_hour;          // hour under test

                int         d_minutes;       // minutes under test

                int         d_seconds;       // seconds under test

                int         d_milliSecs;     // milli seconds under test

                int         d_microSecs;     // micro seconds under test

                int         d_tzoffset;      // time zone offset

                bool        d_isValid;       // isValid flag

                bool        d_isDatetimeTz;  // flag indicating whether the
                                             // result object is expected to
                                             // contain 'DatetimeTz' or
                                             // 'Datetime' object
            } DATA[] = {
//line  input
//----  ---------------------------------------
//       year  month   day   hour   min   sec    ms   us   off valid   tz
//       ----  -----   ---   ----   ---   ---    --   --  ---- ------  -----
    // valid input
    {   L_, "0001-01-01T00:00:00.000",
            1,     1,    1,     0,    0,    0,    0,   0,    0,  true, false },
    {   L_, "0001-01-01T00:00:00.000000",
            1,     1,    1,     0,    0,    0,    0,   0,    0,  true, false },
    {   L_, "9999-12-31T00:00:00.000000",
         9999,    12,   31,     0,    0,    0,    0,   0,    0,  true, false },
    {   L_, "9999-12-31T23:59:59.000000",
         9999,    12,   31,    23,   59,   59,    0,   0,    0,  true, false },

    {   L_, "0001-01-01T00:00:00.000Z",
            1,     1,    1,     0,    0,    0,    0,   0,    0,  true,  true },
    {   L_, "0001-01-01T00:00:00.000+00:45",
            1,     1,    1,     0,    0,    0,    0,   0,   45,  true,  true },
    {   L_, "9999-12-31T00:00:00.000000+00:00",
         9999,    12,   31,     0,    0,    0,    0,   0,    0,  true,  true },
    {   L_, "9999-12-31T23:59:59.9999991+23:59",
         9999,    12,   31,    23,   59,   59,  999, 999, 1439,  true,  true },

    // invalid input
    {   L_, "0001-00-01T00:00:00.000",
            1,     1,    1,    24,    0,    0,    0,   0,    0, false, false },
    {   L_, "0000-01-01T00:00:00.000000",
            1,     1,    1,    24,    0,    0,    0,   0,    0, false, false },
    {   L_, "0001-01-01T00:61:00.000",
            1,     1,    1,    24,    0,    0,    0,   0,    0, false, false },
    {   L_, "0001-01-01T00:00:61.000",
            1,     1,    1,    24,    0,    0,    0,   0,    0, false, false },

    {   L_, "0000-01-01T00:00:00.000000+00:00",
            1,     1,    1,    24,    0,    0,    0,   0,    0, false, false },
    {   L_, "0001-00-01T00:00:00.000000+00:00",
            1,     1,    1,    24,    0,    0,    0,   0,    0, false, false },
    {   L_, "0001-01-01T00:00:00.000000-24:00",
            1,     1,    1,    24,    0,    0,    0,   0,    0, false, false },
    {   L_, "0001-01-01T00:00:00.000000-00:61",
            1,     1,    1,    24,    0,    0,    0,   0,    0, false, false },
        };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int         LINE          = DATA[i].d_line;
                const bsl::string INPUT         = DATA[i].d_input_p;
                const int         YEAR          = DATA[i].d_year;
                const int         MONTH         = DATA[i].d_month;
                const int         DAY           = DATA[i].d_day;
                const int         HOUR          = DATA[i].d_hour;
                const int         MINUTE        = DATA[i].d_minutes;
                const int         SECOND        = DATA[i].d_seconds;
                const int         MILLISECOND   = DATA[i].d_milliSecs;
                const int         MICROSECOND   = DATA[i].d_microSecs;
                const int         OFFSET        = DATA[i].d_tzoffset;
                const bool        IS_VALID      = DATA[i].d_isValid;
                const bool        IS_DATETIMETZ = DATA[i].d_isDatetimeTz;

                const bdlt::Datetime   EXP_DATETIME(YEAR,
                                                    MONTH,
                                                    DAY,
                                                    HOUR,
                                                    MINUTE,
                                                    SECOND,
                                                    MILLISECOND,
                                                    MICROSECOND);
                const bdlt::DatetimeTz EXP_DATETIMETZ(EXP_DATETIME, OFFSET);

                bdlb::Variant2<bdlt::Datetime, bdlt::DatetimeTz>        mX;
                const bdlb::Variant2<bdlt::Datetime, bdlt::DatetimeTz>& X = mX;

                int retCode = Util::parseDefault(
                                             &mX,
                                             INPUT.c_str(),
                                             static_cast<int>(INPUT.length()));
                if (IS_VALID) {
                    ASSERTV(LINE, retCode, 0 == retCode);
                    if (IS_DATETIMETZ) {
                        ASSERTV(LINE, X.is<bdlt::DatetimeTz>());
                        ASSERTV(LINE, INPUT,
                                EXP_DATETIMETZ, X.the<bdlt::DatetimeTz>(),
                                EXP_DATETIMETZ == X.the<bdlt::DatetimeTz>());
                    }
                    else {
                        ASSERTV(LINE, X.is<bdlt::Datetime>());
                        ASSERTV(LINE, INPUT,
                                EXP_DATETIME, X.the<bdlt::Datetime>(),
                                EXP_DATETIME == X.the<bdlt::Datetime>());
                    }
                }
                else {
                    ASSERTV(LINE, retCode, 0 != retCode);
                    ASSERTV(LINE, X.isUnset());
                }
            }
        }
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // TESTING 'parseText' FUNCTIONS
        //
        // Concerns:
        //
        // Plan:
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting 'parseText' Functions"
                          << "\n=============================" << endl;

        if (verbose) cout << "\nUsing 'bool'." << endl;
        {
            typedef bool Type;

            static const struct {
                int         d_lineNum;
                const char *d_input;
                Type        d_result;
            } DATA[] = {
                //line    input       result
                //----    -----       ------
                { L_,     "trueX",     true,         },
                { L_,     "falseX",    false,        },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int   LINE            = DATA[i].d_lineNum;
                const char *INPUT           = DATA[i].d_input;
                const int   INPUT_LENGTH    =
                                      static_cast<int>(bsl::strlen(INPUT)) - 1;
                const Type  EXPECTED_RESULT = DATA[i].d_result;

                Type mX(!EXPECTED_RESULT);  const Type& X = mX;

                int retCode = Util::parseText(&mX, INPUT, INPUT_LENGTH);

                LOOP2_ASSERT(LINE, retCode, 0               == retCode);
                LOOP2_ASSERT(LINE, X,       EXPECTED_RESULT == X);
            }
        }

        if (verbose) cout << "\nUsing 'char'." << endl;
        {
            typedef char Type;

            static const struct {
                int         d_lineNum;
                const char *d_input;
                Type        d_result;
            } DATA[] = {
                //line    input       result
                //----    -----       ------
                { L_,     "aX",        'a',         },
                { L_,     "bX",        'b',         },
                { L_,     "ZX",        'Z',         },
                { L_,     "zX",        'z',         },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int   LINE            = DATA[i].d_lineNum;
                const char *INPUT           = DATA[i].d_input;
                const int   INPUT_LENGTH    =
                                      static_cast<int>(bsl::strlen(INPUT)) - 1;
                const Type  EXPECTED_RESULT = DATA[i].d_result;

                Type mX;  const Type& X = mX;

                int retCode = Util::parseText(&mX, INPUT, INPUT_LENGTH);

                LOOP2_ASSERT(LINE, retCode, 0               == retCode);
                LOOP2_ASSERT(LINE, X,       EXPECTED_RESULT == X);
            }
        }

        if (verbose) cout << "\nUsing 'bsl::string'." << endl;
        {
            typedef bsl::string Type;

            static const struct {
                int         d_lineNum;
                const char *d_input;
                Type        d_result;
            } DATA[] = {
                //line    input       result
                //----    -----       ------
                { L_,     "X",         "",         },
                { L_,     "HelloX",    "Hello",    },
                { L_,     "World!!X",  "World!!",  },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int   LINE            = DATA[i].d_lineNum;
                const char *INPUT           = DATA[i].d_input;
                const int   INPUT_LENGTH    =
                                      static_cast<int>(bsl::strlen(INPUT)) - 1;
                const Type  EXPECTED_RESULT = DATA[i].d_result;

                Type mX;  const Type& X = mX;

                int retCode = Util::parseText(&mX, INPUT, INPUT_LENGTH);

                LOOP2_ASSERT(LINE, retCode, 0               == retCode);
                LOOP2_ASSERT(LINE, X,       EXPECTED_RESULT == X);
            }
        }

        if (verbose) cout << "\nUsing 'bsl::vector<char>'." << endl;
        {
            typedef bsl::vector<char> Type;

            static const struct {
                int         d_lineNum;
                const char *d_input;
                const char *d_result;
            } DATA[] = {
                //line    input       result
                //----    -----       ------
                { L_,     "X",         "",         },
                { L_,     "HelloX",    "Hello",    },
                { L_,     "World!!X",  "World!!",  },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int          LINE          = DATA[i].d_lineNum;
                const char        *INPUT         = DATA[i].d_input;
                const int          INPUT_LENGTH  =
                                      static_cast<int>(bsl::strlen(INPUT)) - 1;
                const char        *RESULT        = DATA[i].d_result;
                const bsl::size_t  RESULT_LENGTH = bsl::strlen(RESULT);

                const Type EXPECTED_RESULT(RESULT,
                                           RESULT + RESULT_LENGTH);

                Type mX;  const Type& X = mX;

                int retCode = Util::parseText(&mX, INPUT, INPUT_LENGTH);

                LOOP2_ASSERT(LINE, retCode, 0               == retCode);
                LOOP2_ASSERT(LINE, X,       EXPECTED_RESULT == X);
            }
        }

        if (verbose) cout << "\nUsing 'MyEnumeration::Value' (with success)."
                          << endl;
        {
            typedef test::MyEnumeration::Value Type;

            static const struct {
                int         d_lineNum;
                const char *d_input;
                Type        d_result;
            } DATA[] = {
                //line    input       result
                //----    -----       ------
                { L_,     "VALUE1X",   test::MyEnumeration::VALUE1,   },
                { L_,     "VALUE2X",   test::MyEnumeration::VALUE2,   },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int   LINE            = DATA[i].d_lineNum;
                const char *INPUT           = DATA[i].d_input;
                const int   INPUT_LENGTH    =
                                      static_cast<int>(bsl::strlen(INPUT)) - 1;
                const Type  EXPECTED_RESULT = DATA[i].d_result;

                Type mX;  const Type& X = mX;

                int retCode = Util::parseText(&mX, INPUT, INPUT_LENGTH);

                LOOP2_ASSERT(LINE, retCode, 0               == retCode);
                LOOP2_ASSERT(LINE, X,       EXPECTED_RESULT == X);
            }
        }

        if (verbose) cout << "\nUsing 'MyEnumeration::Value' (with failure)."
                          << endl;
        {
            typedef test::MyEnumeration::Value Type;

            static const struct {
                int         d_lineNum;
                const char *d_input;
            } DATA[] = {
                //line    input
                //----    -----
                { L_,     "VALUE0X",      },
                { L_,     "VALUE3X",      },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int   LINE         = DATA[i].d_lineNum;
                const char *INPUT        = DATA[i].d_input;
                const int   INPUT_LENGTH =
                                      static_cast<int>(bsl::strlen(INPUT)) - 1;

                Type mX;

                int retCode = Util::parseText(&mX, INPUT, INPUT_LENGTH);

                LOOP2_ASSERT(LINE, retCode, 0 != retCode);
            }
        }

        if (verbose) cout << "\nUsing 'MyEnumerationWithFallback::Value'."
                          << endl;
        {
            typedef test::MyEnumerationWithFallback::Value Type;

            static const struct {
                int         d_lineNum;
                const char *d_input;
                Type        d_result;
            } DATA[] = {
                //line input       result
                //---- -----       ------
                { L_,  "VALUE1X",  test::MyEnumerationWithFallback::VALUE1  },
                { L_,  "UNKNOWNX", test::MyEnumerationWithFallback::UNKNOWN },
                { L_,  "VALUE2X",  test::MyEnumerationWithFallback::VALUE2  },
                { L_,  "VALUE3X",  test::MyEnumerationWithFallback::UNKNOWN },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int   LINE            = DATA[i].d_lineNum;
                const char *INPUT           = DATA[i].d_input;
                const int   INPUT_LENGTH    =
                                      static_cast<int>(bsl::strlen(INPUT)) - 1;
                const Type  EXPECTED_RESULT = DATA[i].d_result;

                Type mX;  const Type& X = mX;

                int retCode = Util::parseText(&mX, INPUT, INPUT_LENGTH);

                LOOP2_ASSERT(LINE, retCode, 0               == retCode);
                LOOP2_ASSERT(LINE, X,       EXPECTED_RESULT == X);
            }
        }

        if (verbose) cout << "\nUsing 'CustomizedString' (with success)."
                          << endl;
        {
            typedef test::CustomizedString Type;

            static const struct {
                int         d_lineNum;
                const char *d_input;
                Type        d_result;
            } DATA[] = {
                //line    input         result
                //----    -----         ------
                { L_,     "X",           Type(""),        },
                { L_,     "aX",          Type("a"),       },
                { L_,     "abX",         Type("ab"),      },
                { L_,     "abcX",        Type("abc"),     },
                { L_,     "abcdX",       Type("abcd"),    },
                { L_,     "abcdeX",      Type("abcde"),   },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int   LINE            = DATA[i].d_lineNum;
                const char *INPUT           = DATA[i].d_input;
                const int   INPUT_LENGTH    =
                                      static_cast<int>(bsl::strlen(INPUT)) - 1;
                const Type  EXPECTED_RESULT = DATA[i].d_result;

                Type mX;  const Type& X = mX;

                int retCode = Util::parseText(&mX, INPUT, INPUT_LENGTH);

                LOOP2_ASSERT(LINE, retCode, 0               == retCode);
                LOOP2_ASSERT(LINE, X,       EXPECTED_RESULT == X);
            }
        }

        if (verbose) cout << "\nUsing 'CustomizedString' (with failure)."
                          << endl;
        {
            typedef test::CustomizedString Type;

            static const struct {
                int         d_lineNum;
                const char *d_input;
            } DATA[] = {
                //line    input
                //----    -----
                { L_,     "0123456789_0123456789_12345X",    },
                { L_,     "0123456789_0123456789_123456X",   },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int   LINE         = DATA[i].d_lineNum;
                const char *INPUT        = DATA[i].d_input;
                const int   INPUT_LENGTH =
                                      static_cast<int>(bsl::strlen(INPUT)) - 1;

                Type mX;

                int retCode = Util::parseText(&mX, INPUT, INPUT_LENGTH);

                LOOP2_ASSERT(LINE, retCode, 0 != retCode);
            }
        }
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING 'parseList' FUNCTIONS
        //
        // Concerns:
        //
        // Plan:
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting 'parseList' Functions"
                          << "\n=============================" << endl;

        if (verbose) cout << "\nUsing 'bsl::vector<int>'." << endl;
        {
            typedef int                   ElemType;
            typedef bsl::vector<ElemType> Type;

            static const struct {
                int         d_lineNum;
                const char *d_input;
                ElemType    d_result[5];
                int         d_numResult;
            } DATA[] = {
                //line    input           result               numResult
                //----    -----           ------               ---------
                { L_,     "X",             { },                 0,       },
                { L_,     "1X",            { 1 },               1,       },
                { L_,     "1 4X",          { 1, 4 },            2,       },
                { L_,     "1 4 2X",        { 1, 4, 2 },         3,       },
                { L_,     "1 4 2 8X",      { 1, 4, 2, 8 },      4,       },
                { L_,     "1 4 2 8 23X",   { 1, 4, 2, 8, 23 },  5,       },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int       LINE         = DATA[i].d_lineNum;
                const char     *INPUT        = DATA[i].d_input;
                const int       INPUT_LENGTH =
                                      static_cast<int>(bsl::strlen(INPUT)) - 1;
                const ElemType *RESULT       = DATA[i].d_result;
                const int       NUM_RESULT   = DATA[i].d_numResult;

                const Type EXPECTED_RESULT(RESULT, RESULT + NUM_RESULT);

                Type mX;  const Type& X = mX;

                int retCode = Util::parseList(&mX, INPUT, INPUT_LENGTH);

                LOOP2_ASSERT(LINE, retCode, 0               == retCode);
                LOOP2_ASSERT(LINE, X,       EXPECTED_RESULT == X);
            }
        }
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING 'parseHex' FUNCTIONS
        //
        // Concerns:
        //
        // Plan:
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting 'parseHex' Functions"
                          << "\n============================" << endl;

        static const struct {
            int         d_lineNum;
            const char *d_input;
            const char *d_result;
        } DATA[] = {
            //line    input            result
            //----    -----            ------
            { L_,     "X",              ""         },
            { L_,     "61X",            "a"        },
            { L_,     "6162X",          "ab"       },
            { L_,     "616263X",        "abc"      },
            { L_,     "61626364X",      "abcd"     },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        if (verbose) cout << "\nUsing 'bsl::string'." << endl;
        {
            typedef bsl::string Type;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int          LINE          = DATA[i].d_lineNum;
                const char        *INPUT         = DATA[i].d_input;
                const char        *RESULT        = DATA[i].d_result;
                const int          INPUT_LENGTH  =
                                      static_cast<int>(bsl::strlen(INPUT)) - 1;
                const bsl::size_t  RESULT_LENGTH = bsl::strlen(RESULT);

                const Type EXPECTED_RESULT(RESULT, RESULT + RESULT_LENGTH);

                Type mX;  const Type& X = mX;

                int retCode = Util::parseHex(&mX, INPUT, INPUT_LENGTH);

                LOOP2_ASSERT(LINE, retCode, 0               == retCode);
                LOOP2_ASSERT(LINE, X,       EXPECTED_RESULT == X);
            }
        }

        if (verbose) cout << "\nUsing 'bsl::vector<char>'." << endl;
        {
            typedef bsl::vector<char> Type;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int          LINE          = DATA[i].d_lineNum;
                const char        *INPUT         = DATA[i].d_input;
                const char        *RESULT        = DATA[i].d_result;
                const int          INPUT_LENGTH  =
                                      static_cast<int>(bsl::strlen(INPUT)) - 1;
                const bsl::size_t  RESULT_LENGTH = bsl::strlen(RESULT);

                const Type EXPECTED_RESULT(RESULT, RESULT + RESULT_LENGTH);

                Type mX;  const Type& X = mX;

                int retCode = Util::parseHex(&mX, INPUT, INPUT_LENGTH);

                LOOP2_ASSERT(LINE, retCode, 0               == retCode);
                LOOP2_ASSERT(LINE, X,       EXPECTED_RESULT == X);
            }
        }
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING 'parseDecimal' FUNCTIONS
        //
        // Concerns:
        //
        // Plan:
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting 'parseDecimal' Functions"
                          << "\n================================" << endl;

        if (verbose) cout << "\nUsing 'bool'." << endl;
        {
            typedef bool Type;

            static const struct {
                int         d_lineNum;
                const char *d_input;
                Type        d_result;
            } DATA[] = {
                //line    input       result
                //----    -----       ------
                { L_,     "1X",        true          },
                { L_,     "0X",        false         },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int   LINE            = DATA[i].d_lineNum;
                const char *INPUT           = DATA[i].d_input;
                const int   INPUT_LENGTH    =
                                      static_cast<int>(bsl::strlen(INPUT)) - 1;
                const Type  EXPECTED_RESULT = DATA[i].d_result;

                Type mX(!EXPECTED_RESULT);  const Type& X = mX;

                int retCode = Util::parseDecimal(&mX, INPUT, INPUT_LENGTH);

                LOOP2_ASSERT(LINE, retCode, 0               == retCode);
                LOOP2_ASSERT(LINE, X,       EXPECTED_RESULT == X);
            }
        }

        if (verbose) cout << "\nUsing 'char'." << endl;
        {
            typedef char Type;

            static const struct {
                int         d_lineNum;
                const char *d_input;
                Type        d_result;
            } DATA[] = {
                //line    input       result
                //----    -----       ------
                { L_,     "-128X",     -128            },
                { L_,     "-127X",     -127            },
                { L_,     "-1X",       -1              },
                { L_,     "0X",        0               },
                { L_,     "1X",        1               },
                { L_,     "126X",      126             },
                { L_,     "127X",      127             },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int   LINE            = DATA[i].d_lineNum;
                const char *INPUT           = DATA[i].d_input;
                const int   INPUT_LENGTH    =
                                      static_cast<int>(bsl::strlen(INPUT)) - 1;
                const Type  EXPECTED_RESULT = DATA[i].d_result;

                Type mX(!EXPECTED_RESULT);  const Type& X = mX;

                int retCode = Util::parseDecimal(&mX, INPUT, INPUT_LENGTH);

                LOOP2_ASSERT(LINE, retCode, 0               == retCode);
                LOOP2_ASSERT(LINE, X,       EXPECTED_RESULT == X);
            }
        }

        if (verbose) cout << "\nUsing 'short'." << endl;
        {
            typedef short Type;

            static const struct {
                int         d_lineNum;
                const char *d_input;
                Type        d_result;
            } DATA[] = {
                //line    input       result
                //----    -----       ------
                { L_,     "-32768X",   -32768          },
                { L_,     "-32767X",   -32767          },
                { L_,     "-1X",       -1              },
                { L_,     "0X",        0               },
                { L_,     "1X",        1               },
                { L_,     "32766X",    32766           },
                { L_,     "32767X",    32767           },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int   LINE            = DATA[i].d_lineNum;
                const char *INPUT           = DATA[i].d_input;
                const int   INPUT_LENGTH    =
                                      static_cast<int>(bsl::strlen(INPUT)) - 1;
                const Type  EXPECTED_RESULT = DATA[i].d_result;

                Type mX(!EXPECTED_RESULT);  const Type& X = mX;

                int retCode = Util::parseDecimal(&mX, INPUT, INPUT_LENGTH);

                LOOP2_ASSERT(LINE, retCode, 0               == retCode);
                LOOP2_ASSERT(LINE, X,       EXPECTED_RESULT == X);
            }
        }

        if (verbose) cout << "\nUsing 'int'." << endl;
        {
            typedef int Type;

            static const struct {
                int         d_lineNum;
                const char *d_input;
                Type        d_result;
            } DATA[] = {
                //line    input            result
                //----    -----            ------
                { L_,     "-2147483648X",   -2147483647-1     },
                { L_,     "-2147483647X",   -2147483647       },
                { L_,     "-1X",            -1                },
                { L_,     "0X",             0                 },
                { L_,     "1X",             1                 },
                { L_,     "2147483646X",    2147483646        },
                { L_,     "2147483647X",    2147483647        },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int   LINE            = DATA[i].d_lineNum;
                const char *INPUT           = DATA[i].d_input;
                const int   INPUT_LENGTH    =
                                      static_cast<int>(bsl::strlen(INPUT)) - 1;
                const Type  EXPECTED_RESULT = DATA[i].d_result;

                Type mX(!EXPECTED_RESULT);  const Type& X = mX;

                int retCode = Util::parseDecimal(&mX, INPUT, INPUT_LENGTH);

                LOOP2_ASSERT(LINE, retCode, 0               == retCode);
                LOOP2_ASSERT(LINE, X,       EXPECTED_RESULT == X);
            }
        }

        if (verbose) cout << "\nUsing 'bsls::Types::Int64'." << endl;
        {
            typedef bsls::Types::Int64 Type;

            static const struct {
                int         d_lineNum;
                const char *d_input;
                Type        d_result;
            } DATA[] = {
                //line    input                     result
                //----    -----                     ------
                { L_,     "-9223372036854775808X",
                                static_cast<Type>(-9223372036854775808ULL)  },
                { L_,     "-9223372036854775807X",  -9223372036854775807LL   },
                { L_,     "-1X",                    -1LL                     },
                { L_,     "0X",                     0LL                      },
                { L_,     "1X",                     1LL                      },
                { L_,     "9223372036854775806X",   9223372036854775806LL    },
                { L_,     "9223372036854775807X",   9223372036854775807LL    },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int   LINE            = DATA[i].d_lineNum;
                const char *INPUT           = DATA[i].d_input;
                const int   INPUT_LENGTH    =
                                      static_cast<int>(bsl::strlen(INPUT)) - 1;
                const Type  EXPECTED_RESULT = DATA[i].d_result;

                Type mX(!EXPECTED_RESULT);  const Type& X = mX;

                int retCode = Util::parseDecimal(&mX, INPUT, INPUT_LENGTH);

                LOOP2_ASSERT(LINE, retCode, 0               == retCode);
                LOOP2_ASSERT(LINE, X,       EXPECTED_RESULT == X);
            }
        }

        if (verbose) cout << "\nUsing 'unsigned char'." << endl;
        {
            typedef unsigned char Type;

            static const struct {
                int         d_lineNum;
                const char *d_input;
                Type        d_result;
            } DATA[] = {
                //line    input       result
                //----    -----       ------
                { L_,     "0X",        0               },
                { L_,     "1X",        1               },
                { L_,     "254X",      254             },
                { L_,     "255X",      255             },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int   LINE            = DATA[i].d_lineNum;
                const char *INPUT           = DATA[i].d_input;
                const int   INPUT_LENGTH    =
                                      static_cast<int>(bsl::strlen(INPUT)) - 1;
                const Type  EXPECTED_RESULT = DATA[i].d_result;

                Type mX(!EXPECTED_RESULT);  const Type& X = mX;

                int retCode = Util::parseDecimal(&mX, INPUT, INPUT_LENGTH);

                LOOP2_ASSERT(LINE, retCode, 0               == retCode);
                LOOP2_ASSERT(LINE, X,       EXPECTED_RESULT == X);
            }
        }

        if (verbose) cout << "\nUsing 'unsigned short'." << endl;
        {
            typedef unsigned short Type;

            static const struct {
                int         d_lineNum;
                const char *d_input;
                Type        d_result;
            } DATA[] = {
                //line    input       result
                //----    -----       ------
                { L_,     "0X",        0               },
                { L_,     "1X",        1               },
                { L_,     "65534X",    65534           },
                { L_,     "65535X",    65535           },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int   LINE            = DATA[i].d_lineNum;
                const char *INPUT           = DATA[i].d_input;
                const int   INPUT_LENGTH    =
                                      static_cast<int>(bsl::strlen(INPUT)) - 1;
                const Type  EXPECTED_RESULT = DATA[i].d_result;

                Type mX(!EXPECTED_RESULT);  const Type& X = mX;

                int retCode = Util::parseDecimal(&mX, INPUT, INPUT_LENGTH);

                LOOP2_ASSERT(LINE, retCode, 0               == retCode);
                LOOP2_ASSERT(LINE, X,       EXPECTED_RESULT == X);
            }
        }

        if (verbose) cout << "\nUsing 'unsigned int'." << endl;
        {
            typedef unsigned int Type;

            static const struct {
                int         d_lineNum;
                const char *d_input;
                Type        d_result;
            } DATA[] = {
                //line    input            result
                //----    -----            ------
                { L_,     "0X",             0               },
                { L_,     "1X",             1               },
                { L_,     "4294967294X",    4294967294U     },
                { L_,     "4294967295X",    4294967295U     },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int   LINE            = DATA[i].d_lineNum;
                const char *INPUT           = DATA[i].d_input;
                const int   INPUT_LENGTH    =
                                      static_cast<int>(bsl::strlen(INPUT)) - 1;
                const Type  EXPECTED_RESULT = DATA[i].d_result;

                Type mX(!EXPECTED_RESULT);  const Type& X = mX;

                int retCode = Util::parseDecimal(&mX, INPUT, INPUT_LENGTH);

                LOOP2_ASSERT(LINE, retCode, 0               == retCode);
                LOOP2_ASSERT(LINE, X,       EXPECTED_RESULT == X);
            }
        }

        if (verbose) cout << "\nUsing 'bsls::Types::Uint64'." << endl;
        {
            typedef bsls::Types::Uint64 Type;

            static const struct {
                int         d_lineNum;
                const char *d_input;
                Type        d_result;
            } DATA[] = {
                //line    input                     result
                //----    -----                     ------
                { L_,     "0X",                     0ULL                     },
                { L_,     "1X",                     1ULL                     },
                { L_,     "18446744073709551614X",  18446744073709551614ULL  },
                { L_,     "18446744073709551615X",  18446744073709551615ULL  },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int   LINE            = DATA[i].d_lineNum;
                const char *INPUT           = DATA[i].d_input;
                const int   INPUT_LENGTH    =
                                      static_cast<int>(bsl::strlen(INPUT)) - 1;
                const Type  EXPECTED_RESULT = DATA[i].d_result;

                Type mX(!EXPECTED_RESULT);  const Type& X = mX;

                int retCode = Util::parseDecimal(&mX, INPUT, INPUT_LENGTH);

                LOOP2_ASSERT(LINE, retCode, 0               == retCode);
                LOOP2_ASSERT(LINE, X,       EXPECTED_RESULT == X);
            }
        }

        if (verbose) cout << "\nUsing 'MyEnumeration::Value' (with success)."
                          << endl;
        {
            typedef test::MyEnumeration::Value Type;

            static const struct {
                int         d_lineNum;
                const char *d_input;
                Type        d_result;
            } DATA[] = {
                //line    input   result
                //----    -----   ------
                { L_,     "0X",    test::MyEnumeration::VALUE1      },
                { L_,     "1X",    test::MyEnumeration::VALUE2      },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int   LINE            = DATA[i].d_lineNum;
                const char *INPUT           = DATA[i].d_input;
                const int   INPUT_LENGTH    =
                                      static_cast<int>(bsl::strlen(INPUT)) - 1;
                const Type  EXPECTED_RESULT = DATA[i].d_result;

                Type mX = DATA[(i+1)%2].d_result;  const Type& X = mX;
                LOOP2_ASSERT(LINE, X,       EXPECTED_RESULT != X);

                int retCode = Util::parseDecimal(&mX, INPUT, INPUT_LENGTH);

                ASSERTV(LINE, retCode, 0               == retCode);
                ASSERTV(LINE, EXPECTED_RESULT, X, EXPECTED_RESULT == X);
            }
        }

        if (verbose) cout << "\nUsing 'MyEnumeration::Value' (with failure)."
                          << endl;
        {
            typedef test::MyEnumeration::Value Type;

            static const struct {
                int         d_lineNum;
                const char *d_input;
            } DATA[] = {
                //line    input
                //----    -----
                { L_,     "2X",        },
                { L_,     "3X",        },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int   LINE         = DATA[i].d_lineNum;
                const char *INPUT        = DATA[i].d_input;
                const int   INPUT_LENGTH =
                                      static_cast<int>(bsl::strlen(INPUT)) - 1;

                Type mX;

                int retCode = Util::parseDecimal(&mX, INPUT, INPUT_LENGTH);

                LOOP2_ASSERT(LINE, retCode, 0 != retCode);
            }
        }

        if (verbose) cout << "\nUsing 'MyEnumerationWithFallback::Value'."
                          << endl;
        {
            typedef test::MyEnumerationWithFallback::Value Type;

            static const struct {
                int         d_lineNum;
                const char *d_input;
                Type        d_result;
            } DATA[] = {
                //line    input   result
                //----    -----   ------
                { L_,     "0X",    test::MyEnumerationWithFallback::VALUE1  },
                { L_,     "3X",    test::MyEnumerationWithFallback::UNKNOWN },
                { L_,     "1X",    test::MyEnumerationWithFallback::VALUE2  },
                { L_,     "2X",    test::MyEnumerationWithFallback::UNKNOWN },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int   LINE            = DATA[i].d_lineNum;
                const char *INPUT           = DATA[i].d_input;
                const int   INPUT_LENGTH    =
                                      static_cast<int>(bsl::strlen(INPUT)) - 1;
                const Type  EXPECTED_RESULT = DATA[i].d_result;

                Type mX = DATA[(i+1)%2].d_result;  const Type& X = mX;
                LOOP2_ASSERT(LINE, X,       EXPECTED_RESULT != X);

                int retCode = Util::parseDecimal(&mX, INPUT, INPUT_LENGTH);

                ASSERTV(LINE, retCode, 0               == retCode);
                ASSERTV(LINE, EXPECTED_RESULT, X, EXPECTED_RESULT == X);
            }
        }

        if (verbose) cout << "\nUsing 'CustomizedInt' (with success)." << endl;
        {
            typedef test::CustomizedInt Type;

            static const struct {
                int         d_lineNum;
                const char *d_input;
                Type        d_result;
            } DATA[] = {
                //line    input               result
                //----    -----               ------
                { L_,     "-2147483648X",      Type(-2147483647-1)},
                { L_,     "-2147483647X",      Type(-2147483647)  },
                { L_,     "-1X",               Type(-1)           },
                { L_,     "0X",                Type(0)            },
                { L_,     "1X",                Type(1)            },
                { L_,     "2X",                Type(2)            },
                { L_,     "3X",                Type(3)            },
                { L_,     "4X",                Type(4)            },
                { L_,     "5X",                Type(5)            },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int   LINE            = DATA[i].d_lineNum;
                const char *INPUT           = DATA[i].d_input;
                const int   INPUT_LENGTH    =
                                      static_cast<int>(bsl::strlen(INPUT)) - 1;
                const Type  EXPECTED_RESULT = DATA[i].d_result;

                Type mX;  const Type& X = mX;

                int retCode = Util::parseDecimal(&mX, INPUT, INPUT_LENGTH);

                LOOP2_ASSERT(LINE, retCode, 0               == retCode);
                LOOP2_ASSERT(LINE, X,       EXPECTED_RESULT == X);
            }
        }

        if (verbose) cout << "\nUsing 'CustomizedInt' (with failure)." << endl;
        {
            typedef test::CustomizedInt Type;

            static const struct {
                int         d_lineNum;
                const char *d_input;
            } DATA[] = {
                //line    input
                //----    -----
                { L_,     "6X",             },
                { L_,     "7X",             },
                { L_,     "2147483646X",    },
                { L_,     "2147483647X",    },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int   LINE         = DATA[i].d_lineNum;
                const char *INPUT        = DATA[i].d_input;
                const int   INPUT_LENGTH =
                                      static_cast<int>(bsl::strlen(INPUT)) - 1;

                Type mX;

                int retCode = Util::parseDecimal(&mX, INPUT, INPUT_LENGTH);

                LOOP2_ASSERT(LINE, retCode, 0 != retCode);
            }
        }

        if (verbose) cout << "\nUsing 'float'." << endl;
        {
            typedef float Type;

            Type qNaN   = bsl::numeric_limits<Type>::quiet_NaN();

            static const struct {
                int         d_lineNum;
                const char *d_input;
                Type        d_result;
            } DATA[] = {
                //line    input       result
                //----    -----       ------
                { L_,     "-1X",      -1.0f   },
                { L_,     "-0.1X",    -0.1f   },
                { L_,     "0X",        0.0f   },
                { L_,     "0.1X",      0.1f   },
                { L_,     "1X",        1.0f   },
                { L_,     "123.4X",    123.4f },
                { L_,     "0.005X",    0.005f },
                { L_,     "9.99E36X",  qNaN   },
                { L_,     "+INFX",     qNaN   },
                { L_,     "-INFX",     qNaN   },
                { L_,     "NaNX",      qNaN   },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int   LINE            = DATA[i].d_lineNum;
                const char *INPUT           = DATA[i].d_input;
                const int   INPUT_LENGTH    =
                                      static_cast<int>(bsl::strlen(INPUT)) - 1;
                const Type  EXPECTED_RESULT = DATA[i].d_result;

                Type mX;  const Type& X = mX;

                int retCode = Util::parseDecimal(&mX, INPUT, INPUT_LENGTH);

                if (bdlb::Float::isNan(EXPECTED_RESULT)) {

                    LOOP2_ASSERT(LINE, retCode, 0 != retCode);
                }
                else {
                    LOOP2_ASSERT(LINE, retCode, 0 == retCode);
                    LOOP2_ASSERT(LINE, X, EXPECTED_RESULT == X);
                }
            }
        }

        if (verbose) cout << "\nUsing 'double'." << endl;
        {
            typedef double Type;

            Type qNaN   = bsl::numeric_limits<Type>::quiet_NaN();

            static const struct {
                int         d_lineNum;
                const char *d_input;
                Type        d_result;
            } DATA[] = {
                //line    input       result
                //----    -----       ------
                { L_,     "-1X",       -1.0,            },
                { L_,     "-0.1X",     -0.1,            },
                { L_,     "0X",        0.0,             },
                { L_,     "0.1X",      0.1,             },
                { L_,     "1X",        1.0,             },
                { L_,     "123.4X",    123.4,           },
                { L_,     "79.9X",      79.9,           },
                { L_,     "79.8645X",   79.8645,        },
                { L_,     "-63.2X",     -63.2,          },
                { L_,     "-63.2347X",  -63.2347,       },
                { L_,     "0.005X",    0.005            },
                { L_,     "9.99E36X",  qNaN,            },
                { L_,     "+INFX",     qNaN,            },
                { L_,     "-INFX",     qNaN,            },
                { L_,     "NaNX",      qNaN,            },


                { L_, "-0.10000000000000001X", -0.1      },
                { L_, "-1.0000000000000000X",  -1        },
                { L_, "-0.12345678901234500X", -0.123456789012345        },
                { L_, "0.00000000000000000X",  0      },
                { L_, "0.00000000000000001X",  0.00000000000000001      },
                { L_, "0.10000000000000001X",  0.1      },
                { L_, "1.0000000000000000X",   1        },
                { L_, "123456789012345.0X",    123456789012345.0    },
                { L_, "123456789.012345X",     123456789.012345     },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int   LINE            = DATA[i].d_lineNum;
                const char *INPUT           = DATA[i].d_input;
                const int   INPUT_LENGTH    =
                                      static_cast<int>(bsl::strlen(INPUT)) - 1;
                const Type  EXPECTED_RESULT = DATA[i].d_result;

                Type mX; const Type& X = mX;

                int retCode = Util::parseDecimal(&mX, INPUT, INPUT_LENGTH);

                if (bdlb::Float::isNan(EXPECTED_RESULT)) {

                    LOOP2_ASSERT(LINE, retCode, 0 != retCode);
                }
                else {
                    LOOP2_ASSERT(LINE, retCode, 0 == retCode);
                    LOOP3_ASSERT(LINE, X, EXPECTED_RESULT,
                                 EXPECTED_RESULT == X);
                }
            }
        }
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING 'parseBase64' FUNCTIONS
        //
        // Concerns:
        //
        // Plan:
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting 'parseBase64' Functions"
                          << "\n===============================" << endl;

        static const struct {
            int         d_lineNum;
            const char *d_input;
            const char *d_result;
        } DATA[] = {
            //line    input        result
            //----    -----        ------
            { L_,     "X",          ""            },
            { L_,     "YQ==X",      "a"           },
            { L_,     "YWI=X",      "ab"          },
            { L_,     "YWJjX",      "abc"         },
            { L_,     "YWJjZA==X",  "abcd"        },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        if (verbose) cout << "\nUsing 'bsl::string'." << endl;
        {
            typedef bsl::string Type;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int          LINE          = DATA[i].d_lineNum;
                const char        *INPUT         = DATA[i].d_input;
                const char        *RESULT        = DATA[i].d_result;
                const int          INPUT_LENGTH  =
                                      static_cast<int>(bsl::strlen(INPUT)) - 1;
                const bsl::size_t  RESULT_LENGTH = bsl::strlen(RESULT);

                const Type EXPECTED_RESULT(RESULT, RESULT + RESULT_LENGTH);

                Type mX;  const Type& X = mX;

                int retCode = Util::parseBase64(&mX, INPUT, INPUT_LENGTH);

                LOOP2_ASSERT(LINE, retCode, 0               == retCode);
                LOOP2_ASSERT(LINE, X,       EXPECTED_RESULT == X);
            }
        }

        if (verbose) cout << "\nUsing 'bsl::vector<char>'." << endl;
        {
            typedef bsl::vector<char> Type;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int          LINE          = DATA[i].d_lineNum;
                const char        *INPUT         = DATA[i].d_input;
                const char        *RESULT        = DATA[i].d_result;
                const int          INPUT_LENGTH  =
                                      static_cast<int>(bsl::strlen(INPUT)) - 1;
                const bsl::size_t  RESULT_LENGTH = bsl::strlen(RESULT);

                const Type EXPECTED_RESULT(RESULT, RESULT + RESULT_LENGTH);

                Type mX;  const Type& X = mX;

                int retCode = Util::parseBase64(&mX, INPUT, INPUT_LENGTH);

                LOOP2_ASSERT(LINE, retCode, 0               == retCode);
                LOOP2_ASSERT(LINE, X,       EXPECTED_RESULT == X);
            }
        }
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //
        // Concerns:
        //
        // Plan:
        // --------------------------------------------------------------------
        if (verbose) {
            bsl::cout << "\nBREATHING TEST\n" << bsl::endl;
        }

        TestEnum::Value blah = TestEnum::VALUE2;
        if (veryVerbose) {
            T_ P(blah);
        }
        bsl::string str = "VALUE1";
        balxml::TypesParserUtil::parse(&blah,
                                       str.data(),
                                       static_cast<int>(str.length()),
                                       bdlat_FormattingMode::e_DEFAULT);

        if (veryVerbose) {
            T_ P(blah);
        }

        {
            bsl::string str = "2";
            float floatVal = 123.0f;
            balxml::TypesParserUtil::parse(&floatVal, str.data(),
                                           static_cast<int>(str.length()),
                                           bdlat_FormattingMode::e_DEFAULT);
            if (veryVerbose) {
              T_ P(floatVal);
            }
            int intVal = static_cast<int>(floatVal);
            if (veryVerbose) {
              T_ P(intVal);
            }
        }
        {
            bsl::string str = "2";
            double doubleVal = 123.0;
            balxml::TypesParserUtil::parse(&doubleVal, str.data(),
                                           static_cast<int>(str.length()),
                                           bdlat_FormattingMode::e_DEFAULT);
            if (veryVerbose) {
                T_ P(doubleVal);
            }
            int intVal = static_cast<int>(doubleVal);
            if (veryVerbose) {
                T_ P(intVal);
            }
        }

        {
            bdlt::Datetime dateTime;
            bsl::string str = "2005-01-31T08:59:59.123456-04:00";
            int ret = balxml::TypesParserUtil::parse(
                                              &dateTime,
                                              str.data(),
                                              static_cast<int>(str.length()),
                                              bdlat_FormattingMode::e_DEFAULT);
            LOOP_ASSERT(ret, 0 == ret);
            if (veryVerbose) {
              T_ P(dateTime);
            }
        }

        {
            bdlt::Time timeValue;
            bsl::string str = "08:59:59.123456-04:00";
            int ret = balxml::TypesParserUtil::parse(
                                              &timeValue,
                                              str.data(),
                                              static_cast<int>(str.length()),
                                              bdlat_FormattingMode::e_DEFAULT);
            LOOP_ASSERT(ret, 0 == ret);
            if (veryVerbose) {
                T_ P(timeValue);
            }
        }

        {
            bdlt::Date dateValue;
            bsl::string str = "2005-01-31-04:00";
            int ret = balxml::TypesParserUtil::parse(
                                              &dateValue,
                                              str.data(),
                                              static_cast<int>(str.length()),
                                              bdlat_FormattingMode::e_DEFAULT);
            LOOP_ASSERT(ret, 0 == ret);
            if (veryVerbose) {
                T_ P(dateValue);
            }
        }
      } break;
      default: {
        bsl::cerr << "WARNING: CASE `" << test << "' NOT FOUND." << bsl::endl;
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        bsl::cerr << "Error, non-zero test status = " << testStatus << "."
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
