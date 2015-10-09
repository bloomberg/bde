// balxml_typesparserutil.t.cpp                                       -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <balxml_typesparserutil.h>

#include <bslim_testutil.h>

#include <balxml_typesprintutil.h>  // for testing only

#include <bdlat_enumeratorinfo.h>
#include <bdlat_enumfunctions.h>
#include <bdlat_typetraits.h>
#include <bdlat_valuetypefunctions.h>

#include <bdlb_chartype.h>
#include <bdlb_float.h>  // for printing vector
#include <bdlb_print.h>
#include <bdlb_printmethods.h>

#include <bdlt_datetime.h>

#include <bsl_cstdlib.h>
#include <bsl_cstring.h>
#include <bsl_iosfwd.h>
#include <bsl_iostream.h>
#include <bsl_limits.h>
#include <bsl_ostream.h>
#include <bsl_sstream.h>
#include <bsl_string.h>

#include <bslalg_typetraits.h>

#include <bslma_allocator.h>

#include <bsls_assert.h>
#include <bsls_types.h>

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

template <class T>
ostream& operator<<(ostream& out, const bsl::vector<T>& value)
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
        return stream << "VALUE1";
      case TestEnum::VALUE2:
        return stream << "VALUE2";
      case TestEnum::VALUE3:
        return stream << "VALUE3";
      default:
        return stream << "(* UNKNOWN *)";
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

// test_myenumeration.h   -*-C++-*-

//@PURPOSE: todo: provide purpose.
//
//@CLASSES:
//  MyEnumeration: an enumeration
//
//@DESCRIPTION:
//  todo: provide annotation for 'MyEnumeration'

namespace BloombergLP {

namespace test {

struct MyEnumeration {

  public:
    // TYPES
    enum Value {
        VALUE1 = 1,
            // todo: provide annotation
        VALUE2 = 2
            // todo: provide annotation
    };

    enum {
        NUM_ENUMERATORS = 2 // the number of enumerators in the 'Value'
                            // enumeration
    };

    // CONSTANTS
    static const char CLASS_NAME[];
        // the name of this class (i.e., "MyEnumeration")

    static const bdlat_EnumeratorInfo ENUMERATOR_INFO_ARRAY[];
        // enumerator information for each enumerator

    // CLASS METHODS
    static const char *toString(Value value);
        // Return the string representation exactly matching the enumerator
        // name corresponding to the specified enumeration 'value'.

    static int fromString(Value        *result,
                          const char   *string,
                          int           stringLength);
        // Load into the specified 'result' the enumerator matching the
        // specified 'string' of the specified 'stringLength'.  Return 0 on
        // success, and a non-zero value with no effect on 'result' otherwise
        // (i.e., 'string' does not match any enumerator).

    static int fromInt(Value *result, int number);
        // Load into the specified 'result' the enumerator matching the
        // specified 'number'.  Return 0 on success, and a non-zero value with
        // no effect on 'result' otherwise (i.e., 'number' does not match any
        // enumerator).

    static bsl::ostream& print(bsl::ostream& stream, Value value);
        // Write to the specified 'stream' the string representation of the
        // specified enumeration 'value'.  Return a reference to the modifiable
        // 'stream'.
};

// FREE OPERATORS
inline
bsl::ostream& operator<<(bsl::ostream& stream, MyEnumeration::Value rhs);
    // Format the specified 'rhs' to the specified output 'stream' and return a
    // reference to the modifiable 'stream'.

// ============================================================================
//                        INLINE FUNCTION DEFINITIONS
// ============================================================================

inline
int MyEnumeration::fromInt(MyEnumeration::Value *result, int number)
{
    enum { SUCCESS = 0, NOT_FOUND = 1 };

    switch (number) {
      case MyEnumeration::VALUE1:
      case MyEnumeration::VALUE2:
        *result = (MyEnumeration::Value)number;
        return SUCCESS;                                               // RETURN
      default:
        return NOT_FOUND;                                             // RETURN
    }
}

inline
bsl::ostream& MyEnumeration::print(bsl::ostream&        stream,
                                   MyEnumeration::Value value)
{
    return stream << toString(value);
}

// ----------------------------------------------------------------------------

// CLASS METHODS
inline
const char *MyEnumeration::toString(MyEnumeration::Value value)
{
    switch (value) {
      case VALUE1: {
        return "VALUE1";                                              // RETURN
      } break;
      case VALUE2: {
        return "VALUE2";                                              // RETURN
      } break;
      default:
        BSLS_ASSERT_SAFE(!"encountered out-of-bound enumerated value");
    }

    return 0;
}

}  // close namespace test

// TRAITS
BDLAT_DECL_ENUMERATION_TRAITS(test::MyEnumeration)

// FREE OPERATORS
inline
bsl::ostream &test::operator<<(bsl::ostream &stream, MyEnumeration::Value rhs)
{
    return test::MyEnumeration::print(stream, rhs);
}

}  // close enterprise namespace

// ----------------------------------------------------------------------------
//                       *End-of-file Block removed.*
// ----------------------------------------------------------------------------

// test_myenumeration.cpp  -*-C++-*-

namespace BloombergLP {
namespace test {

                               // ---------
                               // CONSTANTS
                               // ---------

const char MyEnumeration::CLASS_NAME[] = "MyEnumeration";
    // the name of this class

const bdlat_EnumeratorInfo MyEnumeration::ENUMERATOR_INFO_ARRAY[] = {
    {
        MyEnumeration::VALUE1,
        "VALUE1",                      // name
        sizeof("VALUE1") - 1,          // name length
        "todo: provide annotation"  // annotation
    },
    {
        MyEnumeration::VALUE2,
        "VALUE2",                      // name
        sizeof("VALUE2") - 1,          // name length
        "todo: provide annotation"  // annotation
    }
};

                               // -------------
                               // CLASS METHODS
                               // -------------

int MyEnumeration::fromString(Value      *result,
                              const char *string,
                              int         stringLength)
{

    enum { SUCCESS = 0, NOT_FOUND = 1 };

    switch(stringLength) {
        case 6: {
            if (bdlb::CharType::toUpper(string[0])=='V'
             && bdlb::CharType::toUpper(string[1])=='A'
             && bdlb::CharType::toUpper(string[2])=='L'
             && bdlb::CharType::toUpper(string[3])=='U'
             && bdlb::CharType::toUpper(string[4])=='E') {
                switch(bdlb::CharType::toUpper(string[5])) {
                    case '1': {
                        *result = VALUE1;
                        return SUCCESS;                               // RETURN
                    } break;
                    case '2': {
                        *result = VALUE2;
                        return SUCCESS;                               // RETURN
                    } break;
                }
            }
        } break;
    }

    return NOT_FOUND;

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

}  // close namespace test
}  // close enterprise namespace

// ----------------------------------------------------------------------------
//                       *End-of-file Block removed.*
// ----------------------------------------------------------------------------

// test_customizedint.h   -*-C++-*-

//@PURPOSE: todo: provide purpose.
//
//@CLASSES:
//  CustomizedInt: a customized integer
//
//@AUTHOR: Author Unknown
//
//@DESCRIPTION:
//  todo: provide annotation for 'CustomizedInt'

namespace BloombergLP {

namespace test {

class CustomizedInt {

  private:
    // PRIVATE DATA MEMBERS
    int d_value;  // stored value

    // FRIENDS
    friend bool operator==(const CustomizedInt& lhs, const CustomizedInt& rhs);
    friend bool operator!=(const CustomizedInt& lhs, const CustomizedInt& rhs);

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

}  // close namespace test

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
namespace test {

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

}  // close namespace test
}  // close enterprise namespace

// ----------------------------------------------------------------------------
//                       *End-of-file Block removed.*
// ----------------------------------------------------------------------------

// test_customizedstring.h   -*-C++-*-

//@PURPOSE: todo: provide purpose.
//
//@CLASSES:
//  CustomizedString: a customized string
//
//@AUTHOR: Author Unknown
//
//@DESCRIPTION:
//  todo: provide annotation for 'CustomizedString'

namespace BloombergLP {

namespace test {

class CustomizedString {

  private:
    // PRIVATE DATA MEMBERS
    bsl::string d_value;  // stored value

    // FRIENDS
    friend bool operator==(const CustomizedString& lhs,
                           const CustomizedString& rhs);
    friend bool operator!=(const CustomizedString& lhs,
                           const CustomizedString& rhs);

  public:
    // TYPES
    typedef bsl::string BaseType;

    // CONSTANTS
    static const char CLASS_NAME[];
        // the name of this class (i.e., "CustomizedString")

    // CREATORS
    explicit CustomizedString(bslma::Allocator *basicAllocator = 0);
        // Create an object of type 'CustomizedString' having the default
        // value.  Use the optionally specified 'basicAllocator' to supply
        // memory.  If 'basicAllocator' is 0, the currently installed default
        // allocator is used.

    CustomizedString(const CustomizedString&  original,
                     bslma::Allocator        *basicAllocator = 0);
        // Create an object of type 'CustomizedString' having the value of the
        // specified 'original' object.  Use the optionally specified
        // 'basicAllocator' to supply memory.  If 'basicAllocator' is 0, the
        // currently installed default allocator is used.

    explicit CustomizedString(const bsl::string&  value,
                              bslma::Allocator   *basicAllocator = 0);
        // Create an object of type 'CustomizedString' having the specified
        // 'value'.  Use the optionally specified 'basicAllocator' to supply
        // memory.  If 'basicAllocator' is 0, the currently installed default
        // allocator is used.

    ~CustomizedString();
        // Destroy this object.

    // MANIPULATORS
    CustomizedString& operator=(const CustomizedString& rhs);
        // Assign to this object the value of the specified 'rhs' object.

    void reset();
        // Reset this object to the default value (i.e., its value upon
        // default construction).

    int fromString(const bsl::string& value);
        // Convert from the specified 'value' to this type.  Return 0 if
        // successful and non-zero otherwise.

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

    const bsl::string& toString() const;
        // Convert this value to 'bsl::string'.
};

// FREE OPERATORS
inline
bool operator==(const CustomizedString& lhs, const CustomizedString& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects have
    // the same value, and 'false' otherwise.  Two attribute objects have the
    // same value if each respective attribute has the same value.

inline
bool operator!=(const CustomizedString& lhs, const CustomizedString& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects do not
    // have the same value, and 'false' otherwise.  Two attribute objects do
    // not have the same value if one or more respective attributes differ in
    // values.

inline
bsl::ostream& operator<<(bsl::ostream& stream, const CustomizedString& rhs);
    // Format the specified 'rhs' to the specified output 'stream' and return a
    // reference to the modifiable 'stream'.

// ============================================================================
//                        INLINE FUNCTION DEFINITIONS
// ============================================================================

// CREATORS

inline
CustomizedString::CustomizedString(bslma::Allocator *basicAllocator)
: d_value(basicAllocator)
{
}

inline
CustomizedString::CustomizedString(const CustomizedString&  original,
                                   bslma::Allocator        *basicAllocator)
: d_value(original.d_value, basicAllocator)
{
}

inline
CustomizedString::CustomizedString(const bsl::string&  value,
                                   bslma::Allocator   *basicAllocator)
: d_value(value, basicAllocator)
{
}

inline
CustomizedString::~CustomizedString()
{
}

// MANIPULATORS

inline
CustomizedString& CustomizedString::operator=(const CustomizedString& rhs)
{
    d_value = rhs.d_value;
    return *this;
}

inline
void CustomizedString::reset()
{
    bdlat_ValueTypeFunctions::reset(&d_value);
}

inline
int CustomizedString::fromString(const bsl::string& value)
{
    enum { SUCCESS = 0, FAILURE = -1 };

    if (5 < value.size()) {
        return FAILURE;                                               // RETURN
    }

    d_value = value;

    return SUCCESS;
}

// ACCESSORS

inline
bsl::ostream& CustomizedString::print(bsl::ostream& stream,
                                      int           level,
                                      int           spacesPerLevel) const
{
    return bdlb::PrintMethods::print(stream, d_value, level, spacesPerLevel);
}

inline
const bsl::string& CustomizedString::toString() const
{
    return d_value;
}

}  // close namespace test

// TRAITS

BDLAT_DECL_CUSTOMIZEDTYPE_WITH_ALLOCATOR_TRAITS(test::CustomizedString)

// FREE OPERATORS

inline
bool test::operator==(const CustomizedString& lhs, const CustomizedString& rhs)
{
    return lhs.d_value == rhs.d_value;
}

inline
bool test::operator!=(const CustomizedString& lhs, const CustomizedString& rhs)
{
    return lhs.d_value != rhs.d_value;
}

inline
bsl::ostream& test::operator<<(bsl::ostream&           stream,
                               const CustomizedString& rhs)
{
    return rhs.print(stream, 0, -1);
}

}  // close enterprise namespace

// ----------------------------------------------------------------------------
//                       *End-of-file Block removed.*
// ----------------------------------------------------------------------------

// test_customizedstring.cpp  -*-C++-*-

namespace BloombergLP {
namespace test {

                               // ---------
                               // CONSTANTS
                               // ---------

const char CustomizedString::CLASS_NAME[] = "CustomizedString";
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

}  // close namespace test
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
    const int  INPUT_LENGTH = sizeof(INPUT) - 2;

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
    int test = argc > 1 ? bsl::atoi(argv[1]) : 0;
    int verbose = argc > 2;
    int veryVerbose = argc > 3;
    int veryVeryVerbose = argc > 4;

    bsl::cout << "TEST " << __FILE__ << " CASE " << test << bsl::endl;;

    switch (test) { case 0:  // Zero is always the leading case.
      case 9: {
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
                const int   LINE          = DATA[i].d_lineNum;
                const char *INPUT         = DATA[i].d_input;
                const char *RESULT        = DATA[i].d_result;
                const int   INPUT_LENGTH  = bsl::strlen(INPUT)-1;
                const int   RESULT_LENGTH = bsl::strlen(RESULT);

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
                const int   INPUT_LENGTH    = bsl::strlen(INPUT)-1;
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
                const int   INPUT_LENGTH    = bsl::strlen(INPUT)-1;
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
                const int   INPUT_LENGTH    = bsl::strlen(INPUT)-1;
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
                const int   LINE          = DATA[i].d_lineNum;
                const char *INPUT         = DATA[i].d_input;
                const char *RESULT        = DATA[i].d_result;
                const int   INPUT_LENGTH  = bsl::strlen(INPUT)-1;
                const int   RESULT_LENGTH = bsl::strlen(RESULT);

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
                const int       INPUT_LENGTH = bsl::strlen(INPUT)-1;
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
                const int   INPUT_LENGTH    = bsl::strlen(INPUT)-1;
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

        if (verbose) cout << "\nEnd of Test." << endl;
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
                const int   INPUT_LENGTH    = bsl::strlen(INPUT)-1;
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
                const int   INPUT_LENGTH    = bsl::strlen(INPUT)-1;
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
                const int   INPUT_LENGTH    = bsl::strlen(INPUT)-1;
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
                const int   INPUT_LENGTH    = bsl::strlen(INPUT)-1;
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
                const int   INPUT_LENGTH    = bsl::strlen(INPUT)-1;
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
                const int   INPUT_LENGTH    = bsl::strlen(INPUT)-1;
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
                const int   INPUT_LENGTH    = bsl::strlen(INPUT)-1;
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
                //----    -----            ------
                { L_,     "0X",             0,               },
                { L_,     "1X",             1,               },
                { L_,     "4294967294X",    4294967294,      },
                { L_,     "4294967295X",    4294967295,      },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int   LINE            = DATA[i].d_lineNum;
                const char *INPUT           = DATA[i].d_input;
                const int   INPUT_LENGTH    = bsl::strlen(INPUT)-1;
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
                const int   INPUT_LENGTH    = bsl::strlen(INPUT)-1;
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
            Type sNaN   = bsl::numeric_limits<Type>::signaling_NaN();

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
                { L_,     "9.99E36X",  9.99E36          },
                { L_,     "+INFX",     posInf,          },
                { L_,     "-INFX",     -posInf,         },
                { L_,     "NaNX",      qNaN             },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int   LINE            = DATA[i].d_lineNum;
                const char *INPUT           = DATA[i].d_input;
                const int   INPUT_LENGTH    = bsl::strlen(INPUT)-1;
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
            Type sNaN   = bsl::numeric_limits<Type>::signaling_NaN();

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
                const int   INPUT_LENGTH    = bsl::strlen(INPUT)-1;
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
                const int   INPUT_LENGTH    = bsl::strlen(INPUT)-1;
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
                const int   LINE          = DATA[i].d_lineNum;
                const char *INPUT         = DATA[i].d_input;
                const char *RESULT        = DATA[i].d_result;
                const int   INPUT_LENGTH  = bsl::strlen(INPUT)-1;
                const int   RESULT_LENGTH = bsl::strlen(RESULT);

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
                const int   INPUT_LENGTH    = bsl::strlen(INPUT)-1;
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
                const int   INPUT_LENGTH    = bsl::strlen(INPUT)-1;
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
                const int   INPUT_LENGTH    = bsl::strlen(INPUT)-1;
                const Type  EXPECTED_RESULT = DATA[i].d_result;

                Type mX;  const Type& X = mX;

                int retCode = Util::parseDefault(&mX, INPUT, INPUT_LENGTH);

                LOOP2_ASSERT(LINE, retCode, 0               == retCode);
                LOOP2_ASSERT(LINE, X,       EXPECTED_RESULT == X);
            }
        }

        if (verbose) cout << "\nEnd of Test." << endl;
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
                const int   INPUT_LENGTH    = bsl::strlen(INPUT)-1;
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
                const int   INPUT_LENGTH    = bsl::strlen(INPUT)-1;
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
                const int   INPUT_LENGTH    = bsl::strlen(INPUT)-1;
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
                const int   LINE               = DATA[i].d_lineNum;
                const char *INPUT              = DATA[i].d_input;
                const int   INPUT_LENGTH       = bsl::strlen(INPUT)-1;
                const char *RESULT_DATA        = DATA[i].d_result;
                const int   RESULT_DATA_LENGTH = bsl::strlen(RESULT_DATA);

                const Type EXPECTED_RESULT(RESULT_DATA,
                                           RESULT_DATA + RESULT_DATA_LENGTH);

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
                const int   INPUT_LENGTH    = bsl::strlen(INPUT)-1;
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
                const int   LINE            = DATA[i].d_lineNum;
                const char *INPUT           = DATA[i].d_input;
                const int   INPUT_LENGTH    = bsl::strlen(INPUT)-1;

                Type mX;  const Type& X = mX;

                int retCode = Util::parseText(&mX, INPUT, INPUT_LENGTH);

                LOOP2_ASSERT(LINE, retCode, 0 != retCode);
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
                const int   INPUT_LENGTH    = bsl::strlen(INPUT)-1;
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
                { L_,     "abcdefX",    },
                { L_,     "abcdefgX",   },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int   LINE            = DATA[i].d_lineNum;
                const char *INPUT           = DATA[i].d_input;
                const int   INPUT_LENGTH    = bsl::strlen(INPUT)-1;

                Type mX;  const Type& X = mX;

                int retCode = Util::parseText(&mX, INPUT, INPUT_LENGTH);

                LOOP2_ASSERT(LINE, retCode, 0 != retCode);
            }
        }

        if (verbose) cout << "\nEnd of Test." << endl;
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
                const int       INPUT_LENGTH = bsl::strlen(INPUT)-1;
                const ElemType *RESULT       = DATA[i].d_result;
                const int       NUM_RESULT   = DATA[i].d_numResult;

                const Type EXPECTED_RESULT(RESULT, RESULT + NUM_RESULT);

                Type mX;  const Type& X = mX;

                int retCode = Util::parseList(&mX, INPUT, INPUT_LENGTH);

                LOOP2_ASSERT(LINE, retCode, 0               == retCode);
                LOOP2_ASSERT(LINE, X,       EXPECTED_RESULT == X);
            }
        }

        if (verbose) cout << "\nEnd of Test." << endl;
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
                const int   LINE          = DATA[i].d_lineNum;
                const char *INPUT         = DATA[i].d_input;
                const char *RESULT        = DATA[i].d_result;
                const int   INPUT_LENGTH  = bsl::strlen(INPUT)-1;
                const int   RESULT_LENGTH = bsl::strlen(RESULT);

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
                const int   LINE          = DATA[i].d_lineNum;
                const char *INPUT         = DATA[i].d_input;
                const char *RESULT        = DATA[i].d_result;
                const int   INPUT_LENGTH  = bsl::strlen(INPUT)-1;
                const int   RESULT_LENGTH = bsl::strlen(RESULT);

                const Type EXPECTED_RESULT(RESULT, RESULT + RESULT_LENGTH);

                Type mX;  const Type& X = mX;

                int retCode = Util::parseHex(&mX, INPUT, INPUT_LENGTH);

                LOOP2_ASSERT(LINE, retCode, 0               == retCode);
                LOOP2_ASSERT(LINE, X,       EXPECTED_RESULT == X);
            }
        }

        if (verbose) cout << "\nEnd of Test." << endl;
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
                const int   INPUT_LENGTH    = bsl::strlen(INPUT)-1;
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
                const int   INPUT_LENGTH    = bsl::strlen(INPUT)-1;
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
                const int   INPUT_LENGTH    = bsl::strlen(INPUT)-1;
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
                const int   INPUT_LENGTH    = bsl::strlen(INPUT)-1;
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
                const int   INPUT_LENGTH    = bsl::strlen(INPUT)-1;
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
                const int   INPUT_LENGTH    = bsl::strlen(INPUT)-1;
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
                const int   INPUT_LENGTH    = bsl::strlen(INPUT)-1;
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
                { L_,     "4294967294X",    4294967294      },
                { L_,     "4294967295X",    4294967295      },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int   LINE            = DATA[i].d_lineNum;
                const char *INPUT           = DATA[i].d_input;
                const int   INPUT_LENGTH    = bsl::strlen(INPUT)-1;
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
                const int   INPUT_LENGTH    = bsl::strlen(INPUT)-1;
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
                { L_,     "1X",    test::MyEnumeration::VALUE1      },
                { L_,     "2X",    test::MyEnumeration::VALUE2      },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int   LINE            = DATA[i].d_lineNum;
                const char *INPUT           = DATA[i].d_input;
                const int   INPUT_LENGTH    = bsl::strlen(INPUT)-1;
                const Type  EXPECTED_RESULT = DATA[i].d_result;

                Type mX = DATA[(i+1)%2].d_result;  const Type& X = mX;
                LOOP2_ASSERT(LINE, X,       EXPECTED_RESULT != X);

                int retCode = Util::parseDecimal(&mX, INPUT, INPUT_LENGTH);

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
                { L_,     "0X",        },
                { L_,     "3X",        },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int   LINE            = DATA[i].d_lineNum;
                const char *INPUT           = DATA[i].d_input;
                const int   INPUT_LENGTH    = bsl::strlen(INPUT)-1;

                Type mX;  const Type& X = mX;

                int retCode = Util::parseDecimal(&mX, INPUT, INPUT_LENGTH);

                LOOP2_ASSERT(LINE, retCode, 0 != retCode);
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
                const int   INPUT_LENGTH    = bsl::strlen(INPUT)-1;
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
                const int   LINE            = DATA[i].d_lineNum;
                const char *INPUT           = DATA[i].d_input;
                const int   INPUT_LENGTH    = bsl::strlen(INPUT)-1;

                Type mX;  const Type& X = mX;

                int retCode = Util::parseDecimal(&mX, INPUT, INPUT_LENGTH);

                LOOP2_ASSERT(LINE, retCode, 0 != retCode);
            }
        }

        if (verbose) cout << "\nUsing 'float'." << endl;
        {
            typedef float Type;

            Type posInf = bsl::numeric_limits<Type>::infinity();
            Type qNaN   = bsl::numeric_limits<Type>::quiet_NaN();
            Type sNaN   = bsl::numeric_limits<Type>::signaling_NaN();

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
                { L_,     "9.99E36X",  qNaN,            },
                { L_,     "+INFX",     qNaN,            },
                { L_,     "-INFX",     qNaN,            },
                { L_,     "NaNX",      qNaN,            },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int   LINE            = DATA[i].d_lineNum;
                const char *INPUT           = DATA[i].d_input;
                const int   INPUT_LENGTH    = bsl::strlen(INPUT)-1;
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

            Type posInf = bsl::numeric_limits<Type>::infinity();
            Type qNaN   = bsl::numeric_limits<Type>::quiet_NaN();
            Type sNaN   = bsl::numeric_limits<Type>::signaling_NaN();

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
                const int   INPUT_LENGTH    = bsl::strlen(INPUT) - 1;
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

        if (verbose) cout << "\nEnd of Test." << endl;
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
                const int   LINE          = DATA[i].d_lineNum;
                const char *INPUT         = DATA[i].d_input;
                const char *RESULT        = DATA[i].d_result;
                const int   INPUT_LENGTH  = bsl::strlen(INPUT)-1;
                const int   RESULT_LENGTH = bsl::strlen(RESULT);

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
                const int   LINE          = DATA[i].d_lineNum;
                const char *INPUT         = DATA[i].d_input;
                const char *RESULT        = DATA[i].d_result;
                const int   INPUT_LENGTH  = bsl::strlen(INPUT)-1;
                const int   RESULT_LENGTH = bsl::strlen(RESULT);

                const Type EXPECTED_RESULT(RESULT, RESULT + RESULT_LENGTH);

                Type mX;  const Type& X = mX;

                int retCode = Util::parseBase64(&mX, INPUT, INPUT_LENGTH);

                LOOP2_ASSERT(LINE, retCode, 0               == retCode);
                LOOP2_ASSERT(LINE, X,       EXPECTED_RESULT == X);
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
        if (verbose) {
            bsl::cout << "\nBREATHING TEST\n" << bsl::endl;
        }

        TestEnum::Value blah = TestEnum::VALUE2;
        if (veryVerbose) {
            T_ P(blah);
        }
        bsl::string str = "VALUE1";
        balxml::TypesParserUtil::parse(&blah, str.data(), str.length(),
                                      bdlat_FormattingMode::e_DEFAULT);

        if (veryVerbose) {
            T_ P(blah);
        }

        {
            bsl::string str = "2";
            float floatVal = 123.0f;
            balxml::TypesParserUtil::parse(&floatVal, str.data(),
                                          str.length(),
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
                                          str.length(),
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
                                              str.length(),
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
                                              str.length(),
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
                                              str.length(),
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
