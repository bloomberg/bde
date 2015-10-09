// bdlb_printmethods.t.cpp                                            -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <bdlb_printmethods.h>

#include <bslim_testutil.h>

#include <bslalg_typetraits.h>
#include <bslmf_assert.h>
#include <bslalg_hastrait.h>
#include <bslmf_nestedtraitdeclaration.h>
#include <bsls_timeinterval.h>

#include <bsl_cstring.h>
#include <bsl_iostream.h>
#include <bsl_map.h>
#include <bsl_sstream.h>
#include <bsl_string.h>

using namespace BloombergLP;
using namespace bsl;  // automatically added by script

// ============================================================================
//                                 TEST PLAN
// ----------------------------------------------------------------------------
//                                  Overview
//                                  --------
// The test plan for this component is fairly straightforward:
//
//: o First do a short breathing test [1].
//: o Then thoroughly test the specialized print method for
//:   'bsl::vector<char, ALLOC>' [2].
//: o Then test each of the 'print' method implementation functions
//:   individually [3,4,5,6].
//: o Then thoroughly test the trait detection meta-function to make sure it
//:   returns the correct print method selector [7].
//: o Then test the generic print method to make sure it uses the trait
//:   detector correctly and calls the correct print implementation method [8].
//: o Then test 'bsl::vector' '<<' output stream operator to make sure it
//:   successfully calls the the print method to print on a single line and
//:   suppress indentation [9]
//: o Finally, test the usage example to make sure it compiles and runs [10].
// ----------------------------------------------------------------------------
// [ 4] bdlb::PrintMethods_Imp<TYPE, bdlb::HasPrintMethod>::print(...);
// [ 6] bdlb::PrintMethods_Imp<TYPE, bslalg::HasStlIterators>::print(...);
// [ 5] bdlb::PrintMethods_Imp<TYPE, bslmf::IsPair>::print(...);
// [ 3] bdlb::PrintMethods_Imp<TYPE, bsl::false_type>::print(...);
// [ 7] bdlb::PrintMethods::print(..., const TYPE&, ...);
// [ 2] bdlb::PrintMethods::print(..., const vector<char, ALLOC>, ...);
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 8] USAGE EXAMPLE

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
    // Specialize for 'char *'.  Need to expand '\r', '\n', '\t' and surround
    // with 'DQUOTE' characters.
{
    out << '"';

    while (*value) {
        out << printableCharacters[static_cast<unsigned>(*value)];
        ++value;
    }

    out << '"';
}

void printValue(ostream& out, const string& value)
    // Need to expand '\r', '\n', '\t' and surround with 'DQUOTE' characters.
{
    printValue(out, value.c_str());
}

// ============================================================================
//                   GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
// ----------------------------------------------------------------------------

class TestType_PrintMethod {
    // This type is used for testing the component for types that have a
    // 'print' method.

    // PRIVATE DATA MEMBERS
    mutable bsl::ostream *d_stream;          // stream passed to 'print'
    mutable int           d_level;           // level passed to 'print'
    mutable int           d_spacesPerLevel;  // spaces per level passed to
                                             // 'print'

  public:

    // CREATORS
    TestType_PrintMethod()
    : d_stream(0)
    , d_level(0)
    , d_spacesPerLevel(0)
    {
    }

    // ACCESSORS
    bsl::ostream& print(bsl::ostream& stream,
                        int           level,
                        int           spacesPerLevel) const
    {
        d_stream         = &stream;
        d_level          = level;
        d_spacesPerLevel = spacesPerLevel;

        return stream;
    }

    bsl::ostream* stream() const
    {
        return d_stream;
    }

    int level() const
    {
        return d_level;
    }

    int spacesPerLevel() const
    {
        return d_spacesPerLevel;
    }
};

class TestType_PrintMethod_StlIterators {
    // This type is used for testing the trait detection meta-function.  This
    // type declares 'bdlb::TypeTraitHasPrintMethod' and
    // 'bslalg::TypeTraitHasStlIterators'.  Note that the traits just need to
    // be declared but the functionality does not need to be implemented,
    // because they will not be used at run-time.
};

class TestType_PrintMethod_Pair {
    // This type is used for testing the trait detection meta-function.  This
    // type declares 'bdlb::TypeTraitHasPrintMethod' and
    // 'bslalg::TypeTraitPair'.  Note that the traits just need to be declared
    // but the functionality does not need to be implemented, because they will
    // not be used at run-time in this test driver.
};

class TestType_PrintMethod_StlIterators_Pair {
    // This type is used for testing the trait detection meta-function.  This
    // type declares 'bdlb::TypeTraitHasPrintMethod',
    // 'bslalg::TypeTraitHasStlIterators', and 'bslalg::TypeTraitPair'.  Note
    // that the traits just need to be declared but the functionality does not
    // need to be implemented, because they will not be used at run-time.
};

class TestType_StlIterators {
    // This type is used for testing the trait detection meta-function.  This
    // type declares 'bslalg::TypeTraitHasStlIterators'.  Note that the traits
    // just need to be declared but the functionality does not need to be
    // implemented, because they will not be used at run-time.
};

class TestType_StlIterators_Pair {
    // This type is used for testing the trait detection meta-function.  This
    // type declares 'bslalg::TypeTraitHasStlIterators' and
    // 'bslalg::TypeTraitPair'.  Note that the traits just need to be declared
    // but the functionality does not need to be implemented, because they will
    // not be used at run-time.
};

class TestType_Pair {
    // This type is used for testing the trait detection meta-function.  This
    // type declares 'bslalg::TypeTraitPair'.  Note that the traits just need
    // to be declared but the functionality does not need to be implemented,
    // because they will not be used at run-time.
};

class TestType_NoTraits {
    // This type is used for testing the trait detection meta-function.  This
    // type does not have any traits.
};

// The following template specializations declare the traits for the above
// types.  Each needs to be in the same namespace as the primary traits
// template.
namespace BloombergLP {

namespace bdlb {
template <> struct HasPrintMethod<TestType_PrintMethod> :
    bsl::true_type { };
template <> struct HasPrintMethod<TestType_PrintMethod_Pair> :
    bsl::true_type { };
template <> struct HasPrintMethod<TestType_PrintMethod_StlIterators> :
    bsl::true_type { };
template <> struct HasPrintMethod<TestType_PrintMethod_StlIterators_Pair>:
    bsl::true_type { };
}  // close package namespace

namespace bslalg {
template <> struct HasStlIterators<TestType_StlIterators> :
    bsl::true_type { };
template <> struct HasStlIterators<TestType_StlIterators_Pair> :
    bsl::true_type { };
template <> struct HasStlIterators<TestType_PrintMethod_StlIterators> :
    bsl::true_type { };
template <> struct HasStlIterators<TestType_PrintMethod_StlIterators_Pair> :
    bsl::true_type { };
}  // close namespace bslalg

namespace bslmf {
template <> struct IsPair<TestType_Pair> : bsl::true_type { };
template <> struct IsPair<TestType_StlIterators_Pair> : bsl::true_type { };
template <> struct IsPair<TestType_PrintMethod_Pair> : bsl::true_type { };
template <> struct IsPair<TestType_PrintMethod_StlIterators_Pair> :
    bsl::true_type { };
}  // close namespace bslmf

}  // close enterprise namespace

// ============================================================================
//                               USAGE EXAMPLE
// ----------------------------------------------------------------------------

                        // ============
                        // class MyDate
                        // ============

class MyDate
{
    // This class is used as an example of a conventional BDE value-semantic
    // type.  Accordingly, this type implements a 'print' method; albeit a stub
    // function.

  public:
    // TRAITS
    BSLALG_DECLARE_NESTED_TRAITS(MyDate, bdlb::TypeTraitHasPrintMethod);

    // CREATOR
    MyDate();
    MyDate(const MyDate& original);
    // ...

    // ACCESSORS

                        // Aspects

    bsl::ostream& print(bsl::ostream& stream,
                        int           level = 0,
                        int           spacesPerLevel = 4) const;
};

                        // ------------
                        // class MyDate
                        // ------------

// CREATORS
MyDate::MyDate()
{
}

MyDate::MyDate(const MyDate& )
{
}

// ACCESSORS
bsl::ostream& MyDate::print(bsl::ostream& stream,
                            int           ,
                            int           ) const
{
    stream << "01JAN0001\n";
    return stream;
}

///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Supplying a 'print' Method for a Parameterized Class
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose we must create a value-semantic class that holds an object of
// parameterized 'TYPE' and, per BDE convention for VSTs, provides a 'print'
// method that shows the value in some human-readable format.
//
// First, we define the wrapper class:
//..
    template <class TYPE>
    class MyWrapper {
      // An example wrapper class for a 'TYPE' object.

      // PRIVATE DATA MEMBERS
      TYPE d_obj;  // wrapped object

      public:
        // TRAITS
        BSLALG_DECLARE_NESTED_TRAITS(MyWrapper, bdlb::TypeTraitHasPrintMethod);

        // CREATORS
        MyWrapper(): d_obj() {};
        MyWrapper(const TYPE& value) : d_obj(value) { }
        // ... other constructors and destructor ...

        // MANIPULATORS
        // ... assignment operator, etc. ...

        // ACCESSORS
        bsl::ostream& print(bsl::ostream& stream,
                            int           level          = 0,
                            int           spacesPerLevel = 4) const;
            // Format the contained 'TYPE' to the specified output 'stream' at
            // the (absolute value of) the optionally specified indentation
            // 'level' and return a reference to 'stream'.  If 'level' is
            // specified, optionally specify 'spacesPerLevel', the number of
            // spaces per indentation level for this and all of its nested
            // objects.  If 'level' is negative, suppress indentation of the
            // first line.  If 'spacesPerLevel' is negative, format the entire
            // output on one line, suppressing all but the initial indentation
            // (as governed by 'level').  If 'stream' is not valid on entry,
            // this operation has no effect.
    };
//..
// Now, we implement the 'print' method of 'MyWrapper' using the
// 'bdlb::PrintMethods' utility.  Doing so gives us a method that produces
// results both when 'TYPE' defines a 'print' method and when it does not.  In
// the latter case 'TYPE::operator<<' is used.
//..
    template <class TYPE>
    bsl::ostream& MyWrapper<TYPE>::print(bsl::ostream& stream,
                                         int           level,
                                         int           spacesPerLevel) const
    {
        return bdlb::PrintMethods::print(stream, d_obj, level, spacesPerLevel);
    }
//..
// Finally, we exercise our 'MyWrapper' class using several representative
// types, starting with 'MyDate' (not shown) a class that implements a 'print'
// method.
//..
    static void usingMyWrapper()
    {
        BSLMF_ASSERT(bdlb::HasPrintMethod<MyDate>::value);

        MyDate            myDate;
        MyWrapper<MyDate> myWrapperForMyDate(myDate);

        BSLMF_ASSERT(!bdlb::HasPrintMethod<int>::value);

        bsl::ostringstream oss1;
        myWrapperForMyDate.print(oss1); // No problem expected since
                                        // 'bsls::TimeInterval' has a 'print'
                                        // method.
        ASSERT("01JAN0001\n" == oss1.str());
//..
// Using an 'int' type shows how 'bdlb::PrintMethods::print' transparently
// handles types that do not provide 'print' methods:
//..
        int            myInt = 123;
        MyWrapper<int> myWrapperForInt(myInt);

        bsl::ostringstream oss2;
        myWrapperForInt.print(oss2);    // 'int' has no 'print' method.
                                        // Problem?
        ASSERT("123\n" == oss2.str());  // No problem!
//..
// Lastly, since 'MyWrapper' itself is a type that implements 'print' -- and
// sets the 'bdlb::TypeTraitHasPrintMethod' trait -- one instance of the
// 'MyWrapper' type can be wrapped by another.
//..
        BSLMF_ASSERT(bdlb::HasPrintMethod<MyWrapper<int> >::value);

        MyWrapper<MyWrapper<int> > myWrappedWrapper;

        bsl::ostringstream oss3;
        myWrappedWrapper.print(oss3);
        ASSERT("0\n" == oss3.str());
    }
//..
// See the {'bslalg_typetraits'} component for more information about
// declaring traits for user-defined classes.

// ============================================================================
//                               MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int             test = argc > 1 ? atoi(argv[1]) : 0;
    bool         verbose = argc > 2;
    bool     veryVerbose = argc > 3;
    bool veryVeryVerbose = argc > 4;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;;

    switch (test) { case 0:  // Zero is always the leading case.
      case 8: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //   Extracted from component header file.
        //
        // Concerns:
        //: 1 The usage example provided in the component header file compiles,
        //:   links, and runs as shown.
        //
        // Plan:
        //: 1 Incorporate usage example from header into test driver, remove
        //:   leading comment characters, and replace 'assert' with 'ASSERT'.
        //:   (C-1)
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) cout << "\n" "USAGE EXAMPLE" "\n"
                                  "=============" "\n";
        usingMyWrapper();

      } break;
      case 7: {
        // --------------------------------------------------------------------
        // TESTING GENERIC 'print' METHOD
        //   This will test the generic 'print' method for arbitrary types.
        //
        // Concerns:
        //: 1 Since each 'bdlb' has-trait meta-function and each print
        //:   implementation function have already been thoroughly tested, we
        //:   only need to make sure that the meta-function is used correctly
        //:   and that it correctly forwards arguments to the appropriate print
        //:   implementation function.
        //
        // Plan:
        //: 1 Use a selection of test data from test cases 3, 4, 5, and 6 and
        //:   exercise this method to make sure the correct traits are detected
        //:   and the correct print implementation function is called with the
        //:   correct arguments.
        //
        // Testing:
        //   bdlb::PrintMethods::print(..., const TYPE&, ...);
        // --------------------------------------------------------------------

        if (verbose) cout << "TESTING GENERIC 'print' METHOD" << endl
                          << "==============================" << endl;

        if (verbose) cout << "\nUsing 'operator<<'." << endl;
        {
            static const struct {
                int         d_lineNum;
                int         d_level;
                int         d_spacesPerLevel;
                const char *d_expectedPrefix;
                const char *d_expectedSuffix;
            } DATA[] = {
                //LINE   LEVEL   SPCS/LVL    PREFIX    SUFFIX
                //----   -----   --------    ------    ------
                { L_,    -2,     -2,         "",       ""        },
                { L_,    -2,     -1,         "",       ""        },
                { L_,    -2,      0,         "",       "\n"      },
                { L_,    -2,      1,         "",       "\n"      },
                { L_,    -2,      2,         "",       "\n"      },

                { L_,    -1,     -2,         "",       ""        },
                { L_,    -1,     -1,         "",       ""        },
                { L_,    -1,      0,         "",       "\n"      },
                { L_,    -1,      1,         "",       "\n"      },
                { L_,    -1,      2,         "",       "\n"      },

                { L_,     0,     -2,         "",       ""        },
                { L_,     0,     -1,         "",       ""        },
                { L_,     0,      0,         "",       "\n"      },
                { L_,     0,      1,         "",       "\n"      },
                { L_,     0,      2,         "",       "\n"      },

                { L_,     1,     -2,         "  ",     ""        },
                { L_,     1,     -1,         " ",      ""        },
                { L_,     1,      0,         "",       "\n"      },
                { L_,     1,      1,         " ",      "\n"      },
                { L_,     1,      2,         "  ",     "\n"      },

                { L_,     2,     -2,         "    ",   ""        },
                { L_,     2,     -1,         "  ",     ""        },
                { L_,     2,      0,         "",       "\n"      },
                { L_,     2,      1,         "  ",     "\n"      },
                { L_,     2,      2,         "    ",   "\n"      },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            if (veryVerbose) cout << "\tUsing 'int'." << bsl::endl;
            {
                typedef int Type;

                const Type   VALUE          =  45;
                const string EXPECTED_VALUE = "45";

                for (int i = 0; i < NUM_DATA; ++i) {
                    const int   LINE             = DATA[i].d_lineNum;
                    const int   LEVEL            = DATA[i].d_level;
                    const int   SPACES_PER_LEVEL = DATA[i].d_spacesPerLevel;
                    const char *EXPECTED_PREFIX  = DATA[i].d_expectedPrefix;
                    const char *EXPECTED_SUFFIX  = DATA[i].d_expectedSuffix;

                    const string EXPECTED_RESULT = EXPECTED_PREFIX
                                                 + EXPECTED_VALUE
                                                 + EXPECTED_SUFFIX;

                    stringstream ss;

                    ostream& ret = bdlb::PrintMethods::print(ss,
                                                             VALUE,
                                                             LEVEL,
                                                             SPACES_PER_LEVEL);

                    LOOP_ASSERT(LINE, &ss == &ret);
                    LOOP3_ASSERT(LINE, EXPECTED_RESULT,   ss.str(),
                                       EXPECTED_RESULT == ss.str());
                }
            }

            if (veryVerbose) cout << "\tUsing 'bsl::string'." << bsl::endl;
            {
                typedef bsl::string Type;

                const Type   VALUE          = "Hello World";
                const string EXPECTED_VALUE = "Hello World";

                for (int i = 0; i < NUM_DATA; ++i) {
                    const int   LINE             = DATA[i].d_lineNum;
                    const int   LEVEL            = DATA[i].d_level;
                    const int   SPACES_PER_LEVEL = DATA[i].d_spacesPerLevel;
                    const char *EXPECTED_PREFIX  = DATA[i].d_expectedPrefix;
                    const char *EXPECTED_SUFFIX  = DATA[i].d_expectedSuffix;

                    const string EXPECTED_RESULT = EXPECTED_PREFIX
                                                 + EXPECTED_VALUE
                                                 + EXPECTED_SUFFIX;

                    if (veryVeryVerbose) {
                        T_ T_ P_(LINE) P_(EXPECTED_RESULT)
                    }

                    stringstream ss;

                    ostream& ret = bdlb::PrintMethods::print(ss,
                                                             VALUE,
                                                             LEVEL,
                                                             SPACES_PER_LEVEL);

                    LOOP_ASSERT(LINE, &ss == &ret);
                    LOOP3_ASSERT(LINE, EXPECTED_RESULT,   ss.str(),
                                       EXPECTED_RESULT == ss.str());

                    if (veryVeryVerbose) {
                        P(ss.str())
                    }
                }
            }

            if (veryVerbose) cout << "\tUsing 'bool'." << bsl::endl;
            {
                const bool TRUE_VAL  = true;
                const bool FALSE_VAL = false;

                {
                    stringstream ss;

                    ostream& ret = bdlb::PrintMethods::print(ss,
                                                            TRUE_VAL,
                                                            0,
                                                            -1);

                    ASSERT(&ss == &ret);
                    LOOP_ASSERT(ss.str(), "1" == ss.str());

                    if (veryVeryVerbose) {
                        T_ T_ P(ss.str())
                    }
                }

                {
                    stringstream ss;

                    ostream& ret = bdlb::PrintMethods::print(ss,
                                                            FALSE_VAL,
                                                            0,
                                                            -1);

                    ASSERT(&ss == &ret);
                    LOOP_ASSERT(ss.str(), "0" == ss.str());

                    if (veryVeryVerbose) {
                        T_ T_ P(ss.str())
                    }
                }
            }
        }

        if (verbose) cout << "\nUsing 'PrintMethods'." << endl;
        {
            static const struct {
                int d_lineNum;
                int d_level;
                int d_spacesPerLevel;
            } DATA[] = {
                //LINE   LEVEL   SPCS/LVL
                //----   -----   --------
                { L_,    -2,     -2,     },
                { L_,    -2,     -1,     },
                { L_,    -2,      0,     },
                { L_,    -2,      1,     },
                { L_,    -2,      2,     },

                { L_,    -1,     -2,     },
                { L_,    -1,     -1,     },
                { L_,    -1,      0,     },
                { L_,    -1,      1,     },
                { L_,    -1,      2,     },

                { L_,     0,     -2,     },
                { L_,     0,     -1,     },
                { L_,     0,      0,     },
                { L_,     0,      1,     },
                { L_,     0,      2,     },

                { L_,     1,     -2,     },
                { L_,     1,     -1,     },
                { L_,     1,      0,     },
                { L_,     1,      1,     },
                { L_,     1,      2,     },

                { L_,     2,     -2,     },
                { L_,     2,     -1,     },
                { L_,     2,      0,     },
                { L_,     2,      1,     },
                { L_,     2,      2,     },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int LINE             = DATA[i].d_lineNum;
                const int LEVEL            = DATA[i].d_level;
                const int SPACES_PER_LEVEL = DATA[i].d_spacesPerLevel;

                typedef TestType_PrintMethod Type;

                const Type VALUE;

                stringstream ss;

                ostream& ret = bdlb::PrintMethods::print(ss,
                                                         VALUE,
                                                         LEVEL,
                                                         SPACES_PER_LEVEL);

                LOOP_ASSERT(LINE, &ss == &ret);
                LOOP_ASSERT(LINE, &ss == VALUE.stream());
                LOOP2_ASSERT(LINE,               VALUE.level(),
                             LEVEL            == VALUE.level());
                LOOP2_ASSERT(LINE,               VALUE.spacesPerLevel(),
                             SPACES_PER_LEVEL == VALUE.spacesPerLevel());
            }
        }

        if (verbose) cout << "\nUsing 'bsl::pair'." << endl;
        {
            typedef pair<int, double> Type;

            const int    INT_VALUE    = 45;
            const double DOUBLE_VALUE =  1.23;
            const Type   VALUE        = Type(INT_VALUE, DOUBLE_VALUE);

            static const struct {
                int         d_lineNum;
                int         d_level;
                int         d_spacesPerLevel;
                const char *d_expectedResult;
            } DATA[] = {
                //LINE   LEVEL   SPCS/LVL    EXPECTED_RESULT
                //----   -----   --------    ---------------

                // level = -2, spacesPerLevel = -2 .. +2
                { L_,    -2,     -2,         "[ 45 1.23 ]"                   },
                { L_,    -2,     -1,         "[ 45 1.23 ]"                   },
                { L_,    -2,      0,         "[\n"
                                             "45\n"
                                             "1.23\n"
                                             "]\n"                           },
                { L_,    -2,      1,         "[\n"
                                             "   45\n"
                                             "   1.23\n"
                                             "  ]\n"                         },
                { L_,    -2,      2,         "[\n"
                                             "      45\n"
                                             "      1.23\n"
                                             "    ]\n"                       },

                // level = -1, spacesPerLevel = -2 .. +2
                { L_,    -1,     -2,         "[ 45 1.23 ]"                   },
                { L_,    -1,     -1,         "[ 45 1.23 ]"                   },
                { L_,    -1,      0,         "[\n"
                                             "45\n"
                                             "1.23\n"
                                             "]\n"                           },
                { L_,    -1,      1,         "[\n"
                                             "  45\n"
                                             "  1.23\n"
                                             " ]\n"                          },
                { L_,    -1,      2,         "[\n"
                                             "    45\n"
                                             "    1.23\n"
                                             "  ]\n"                         },

                // level = 0, spacesPerLevel = -2 .. +2
                { L_,     0,     -2,         "[ 45 1.23 ]"                   },
                { L_,     0,     -1,         "[ 45 1.23 ]"                   },
                { L_,     0,      0,         "[\n"
                                             "45\n"
                                             "1.23\n"
                                             "]\n"                           },
                { L_,     0,      1,         "[\n"
                                             " 45\n"
                                             " 1.23\n"
                                             "]\n"                           },
                { L_,     0,      2,         "[\n"
                                             "  45\n"
                                             "  1.23\n"
                                             "]\n"                           },

                // level = 1, spacesPerLevel = -2 .. +2
                { L_,     1,     -2,         "  [ 45 1.23 ]"                 },
                { L_,     1,     -1,         " [ 45 1.23 ]"                  },
                { L_,     1,      0,         "[\n"
                                             "45\n"
                                             "1.23\n"
                                             "]\n"                           },
                { L_,     1,      1,          " [\n"
                                             "  45\n"
                                             "  1.23\n"
                                             " ]\n"                          },
                { L_,     1,      2,          "  [\n"
                                             "    45\n"
                                             "    1.23\n"
                                             "  ]\n"                         },

                // level = 2, spacesPerLevel = -2 .. +2
                { L_,     2,     -2,         "    [ 45 1.23 ]"               },
                { L_,     2,     -1,         "  [ 45 1.23 ]"                 },
                { L_,     2,      0,         "[\n"
                                             "45\n"
                                             "1.23\n"
                                             "]\n"                           },
                { L_,     2,      1,         "  [\n"
                                             "   45\n"
                                             "   1.23\n"
                                             "  ]\n"                         },
                { L_,     2,      2,          "    [\n"
                                             "      45\n"
                                             "      1.23\n"
                                             "    ]\n"                       },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int   LINE             = DATA[i].d_lineNum;
                const int   LEVEL            = DATA[i].d_level;
                const int   SPACES_PER_LEVEL = DATA[i].d_spacesPerLevel;
                const char *EXPECTED_RESULT  = DATA[i].d_expectedResult;

                stringstream ss;

                ostream& ret = bdlb::PrintMethods::print(ss,
                                                         VALUE,
                                                         LEVEL,
                                                         SPACES_PER_LEVEL);

                LOOP_ASSERT(LINE, &ss == &ret);
                LOOP3_ASSERT(LINE, EXPECTED_RESULT,   ss.str(),
                                   EXPECTED_RESULT == ss.str());
            }
        }

        if (verbose) cout << "\nUsing 'Using Standard Iterators'." << endl;
        {
            typedef vector<int> Type;

            const int ELEMENTS[]   = { 45, 123 };
            const int NUM_ELEMENTS = sizeof ELEMENTS / sizeof *ELEMENTS;

            const Type VALUE(ELEMENTS, ELEMENTS + NUM_ELEMENTS);

            static const struct {
                int         d_lineNum;
                int         d_level;
                int         d_spacesPerLevel;
                const char *d_expectedResult;
            } DATA[] = {
                //LINE   LEVEl   SPCS/LVL    EXPECTED_RESULT
                //----   -----   --------    ---------------

                // level = -2, spacesPerLevel = -2 .. +2
                { L_,    -2,     -2,         "[ 45 123 ]"                    },
                { L_,    -2,     -1,         "[ 45 123 ]"                    },
                { L_,    -2,     0,          "[\n"
                                             "45\n"
                                             "123\n"
                                             "]\n"                           },
                { L_,    -2,     1,          "[\n"
                                             "   45\n"
                                             "   123\n"
                                             "  ]\n"                         },
                { L_,    -2,      2,         "[\n"
                                             "      45\n"
                                             "      123\n"
                                             "    ]\n"                       },

                // level = -1, spacesPerLevel = -2 .. +2
                { L_,    -1,     -2,         "[ 45 123 ]"                    },
                { L_,    -1,     -1,         "[ 45 123 ]"                    },
                { L_,    -1,      0,         "[\n"
                                             "45\n"
                                             "123\n"
                                             "]\n"                           },
                { L_,    -1,      1,         "[\n"
                                             "  45\n"
                                             "  123\n"
                                             " ]\n"                          },
                { L_,    -1,      2,         "[\n"
                                             "    45\n"
                                             "    123\n"
                                             "  ]\n"                         },

                // level = 0, spacesPerLevel = -2 .. +2
                { L_,     0,      -2,        "[ 45 123 ]"                    },
                { L_,     0,      -1,        "[ 45 123 ]"                    },
                { L_,     0,       0,        "[\n"
                                             "45\n"
                                             "123\n"
                                             "]\n"                           },
                { L_,     0,       1,        "[\n"
                                             " 45\n"
                                             " 123\n"
                                             "]\n"                           },
                { L_,     0,       2,        "[\n"
                                             "  45\n"
                                             "  123\n"
                                             "]\n"                           },

                // level = 1, spacesPerLevel = -2 .. +2
                { L_,     1,      -2,       "  [ 45 123 ]"                  },
                { L_,     1,      -1,       " [ 45 123 ]"                   },
                { L_,     1,       0,       "[\n"
                                            "45\n"
                                            "123\n"
                                            "]\n"                           },
                { L_,     1,       1,       " [\n"
                                            "  45\n"
                                            "  123\n"
                                            " ]\n"                          },
                { L_,     1,       2,       "  [\n"
                                            "    45\n"
                                            "    123\n"
                                            "  ]\n"                         },

                // level = 2, spacesPerLevel = -2 .. +2
                { L_,     2,      -2,       "    [ 45 123 ]"                },
                { L_,     2,      -1,       "  [ 45 123 ]"                  },
                { L_,     2,       0,       "[\n"
                                            "45\n"
                                            "123\n"
                                            "]\n"                           },
                { L_,     2,       1,       "  [\n"
                                            "   45\n"
                                            "   123\n"
                                            "  ]\n"                         },
                { L_,     2,       2,       "    [\n"
                                            "      45\n"
                                            "      123\n"
                                            "    ]\n"                       },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int   LINE             = DATA[i].d_lineNum;
                const int   LEVEL            = DATA[i].d_level;
                const int   SPACES_PER_LEVEL = DATA[i].d_spacesPerLevel;
                const char *EXPECTED_RESULT  = DATA[i].d_expectedResult;

                stringstream ss;

                ostream& ret = bdlb::PrintMethods::print(ss,
                                                         VALUE,
                                                         LEVEL,
                                                         SPACES_PER_LEVEL);

                LOOP_ASSERT(LINE, &ss == &ret);
                LOOP3_ASSERT(LINE, EXPECTED_RESULT,   ss.str(),
                                   EXPECTED_RESULT == ss.str());
            }
        }
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // TESTING 'HasStlIterators' PRINT IMPLEMENTATION
        //   This will test the print implementation function for types that
        //   have STL iterators.  The test plan is very similar to the test
        //   plan for testing the 'IsPair' print implementation function (test
        //   case 5).
        //
        // Concerns:
        //: 1 Indenting should work as expected.
        //: 2 The appropriate print method should be called for each element.
        //: 3 Printing with bad streams should be a no-op.
        //
        // Plan:
        //: 1 First, test indenting using simple types such as 'vector<int>'.
        //: 2 Then, test using vectors of increasing size.
        //: 3 Next, to test that this function routes to the correct print
        //:   method for the contained elements, test types that contain
        //:   objects that invoke different print methods, i.e., use the
        //:   following types:
        //:   o 'vector<vector<char> >'
        //:   o 'vector<vector<int> >'
        //:   o 'vector<pair<int, double> >'
        //:   o 'vector<TestType_PrintMethod>'
        //
        // Testing:
        //   bdlb::PrintMethods_Imp<TYPE, bslalg::HasStlIterators>::print(...);
        // --------------------------------------------------------------------

        if (verbose)
            cout << "\n" "TESTING 'HasStlIterators' PRINT IMPLEMENTATION"
                 << "\n" "=============================================="
                 << endl;

        if (verbose) cout << "\nTesting indentation." << endl;
        {
            typedef vector<int> Type;
            typedef bslmf::SelectTraitCase<bslalg::HasStlIterators>::Type
                                BdlbPrintMethod;

            const int ELEMENTS[]   = { 45, 123 };
            const int NUM_ELEMENTS = sizeof ELEMENTS / sizeof *ELEMENTS;

            const Type VALUE(ELEMENTS, ELEMENTS + NUM_ELEMENTS);

            static const struct {
                int         d_lineNum;
                int         d_level;
                int         d_spacesPerLevel;
                const char *d_expectedResult;
            } DATA[] = {
                //LINE   LEVEL   SPCS/LVL    EXPECTED_RESULT
                //----   -----   --------    ---------------

                // level = -2, spacesPerLevel = -2 .. +2
                { L_,    -2,     -2,         "[ 45 123 ]"                    },
                { L_,    -2,     -1,         "[ 45 123 ]"                    },
                { L_,    -2,      0,         "[\n"
                                             "45\n"
                                             "123\n"
                                             "]\n"                           },
                { L_,    -2,      1,         "[\n"
                                             "   45\n"
                                             "   123\n"
                                             "  ]\n"                         },
                { L_,    -2,      2,         "[\n"
                                             "      45\n"
                                             "      123\n"
                                             "    ]\n"                       },

                // level = -1, spacesPerLevel = -2 .. +2
                { L_,    -1,     -2,         "[ 45 123 ]"                    },
                { L_,    -1,     -1,         "[ 45 123 ]"                    },
                { L_,    -1,      0,         "[\n"
                                             "45\n"
                                             "123\n"
                                             "]\n"                           },
                { L_,    -1,      1,         "[\n"
                                             "  45\n"
                                             "  123\n"
                                             " ]\n"                          },
                { L_,    -1,      2,         "[\n"
                                             "    45\n"
                                             "    123\n"
                                             "  ]\n"                         },

                // level = 0, spacesPerLevel = -2 .. +2
                { L_,     0,     -2,         "[ 45 123 ]"                    },
                { L_,     0,     -1,         "[ 45 123 ]"                    },
                { L_,     0,      0,         "[\n"
                                             "45\n"
                                             "123\n"
                                             "]\n"                           },
                { L_,     0,      1,         "[\n"
                                             " 45\n"
                                             " 123\n"
                                             "]\n"                           },
                { L_,     0,      2,         "[\n"
                                             "  45\n"
                                             "  123\n"
                                             "]\n"                           },

                // level = 1, spacesPerLevel = -2 .. +2
                { L_,     1,     -2,         "  [ 45 123 ]"                  },
                { L_,     1,     -1,         " [ 45 123 ]"                   },
                { L_,     1,      0,         "[\n"
                                             "45\n"
                                             "123\n"
                                             "]\n"                           },
                { L_,     1,      1,         " [\n"
                                             "  45\n"
                                             "  123\n"
                                             " ]\n"                          },
                { L_,     1,      2,         "  [\n"
                                             "    45\n"
                                             "    123\n"
                                             "  ]\n"                         },

                // level = 2, spacesPerLevel = -2 .. +2
                { L_,     2,     -2,         "    [ 45 123 ]"                },
                { L_,     2,     -1,         "  [ 45 123 ]"                  },
                { L_,     2,      0,          "[\n"
                                             "45\n"
                                             "123\n"
                                             "]\n"                           },
                { L_,     2,      1,         "  [\n"
                                             "   45\n"
                                             "   123\n"
                                             "  ]\n"                         },
                { L_,     2,      2,         "    [\n"
                                             "      45\n"
                                             "      123\n"
                                             "    ]\n"                       },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int   LINE             = DATA[i].d_lineNum;
                const int   LEVEL            = DATA[i].d_level;
                const int   SPACES_PER_LEVEL = DATA[i].d_spacesPerLevel;
                const char *EXPECTED_RESULT  = DATA[i].d_expectedResult;

                stringstream ss;

                ostream& ret = bdlb::PrintMethods_Imp<Type,
                                                      BdlbPrintMethod>::print(
                                                             ss,
                                                             VALUE,
                                                             LEVEL,
                                                             SPACES_PER_LEVEL);

                LOOP_ASSERT(LINE, &ss == &ret);
                LOOP3_ASSERT(LINE, EXPECTED_RESULT,   ss.str(),
                                   EXPECTED_RESULT == ss.str());
            }
        }

        if (verbose) cout << "\nWith increasing number of elements." << endl;
        {
            const int MAX_ELEMENTS = 3;

            static const struct {
                int         d_lineNum;
                int         d_elements[MAX_ELEMENTS];
                int         d_numElements;
                const char *d_expectedMultiLineResult;
                const char *d_expectedSingleLineResult;
            } DATA[] = {
                //LINE   ELEMENTS      NUM_ELEMENTS    multiLine
                //----   --------      ------------    ---------
                //                                     singleLine
                //                                     ----------
                { L_,    { },          0,              "[\n]\n",
                                                       "[ ]"                 },
                { L_,    { 4 },        1,              "[\n4\n]\n",
                                                       "[ 4 ]"               },
                { L_,    { 4, 2 },     2,              "[\n4\n2\n]\n",
                                                       "[ 4 2 ]"             },
                { L_,    { 4, 2, 9 },  3,              "[\n4\n2\n9\n]\n",
                                                       "[ 4 2 9 ]"           },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int  LINE         = DATA[i].d_lineNum;
                const int *ELEMENTS     = DATA[i].d_elements;
                const int  NUM_ELEMENTS = DATA[i].d_numElements;

                typedef vector<int> Type;
                typedef bslmf::SelectTraitCase<bslalg::HasStlIterators>::Type
                                    BdlbPrintMethod;

                const Type VALUE(ELEMENTS, ELEMENTS + NUM_ELEMENTS);

                // Testing multiline output.

                {
                    const char *EXPECTED_RESULT
                                           = DATA[i].d_expectedMultiLineResult;

                    stringstream ss;

                    ostream& ret = bdlb::PrintMethods_Imp<
                                                 Type,
                                                 BdlbPrintMethod>::print(ss,
                                                                         VALUE,
                                                                         0,
                                                                         0);

                    LOOP_ASSERT(LINE, &ss == &ret);
                    LOOP3_ASSERT(LINE, EXPECTED_RESULT,   ss.str(),
                                       EXPECTED_RESULT == ss.str());
                }

                // Testing single line output.

                {
                    const char *EXPECTED_RESULT
                                          = DATA[i].d_expectedSingleLineResult;

                    stringstream ss;

                    ostream& ret = bdlb::PrintMethods_Imp<
                                                 Type,
                                                 BdlbPrintMethod>::print(ss,
                                                                         VALUE,
                                                                         0,
                                                                         -1);

                    LOOP_ASSERT(LINE, &ss == &ret);
                    LOOP3_ASSERT(LINE, EXPECTED_RESULT,   ss.str(),
                                       EXPECTED_RESULT == ss.str());
                }
            }
        }

        if (verbose) cout << "\nTesting with 'vector<char>' elements." << endl;
        {
            typedef vector<char>     ElemType;
            typedef vector<ElemType> Type;
            typedef bslmf::SelectTraitCase<bslalg::HasStlIterators>::Type
                                     BdlbPrintMethod;

            const char FIRST_DATA[]  = "Hello\r";
            const char SECOND_DATA[] = "World\n";

            const ElemType DATA[] = { ElemType(FIRST_DATA,
                                               FIRST_DATA
                                             + sizeof(FIRST_DATA)-1),
                                      ElemType(SECOND_DATA,
                                               SECOND_DATA
                                             + sizeof(SECOND_DATA)-1) };

            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            const Type VALUE(DATA, DATA + NUM_DATA);

            if (veryVerbose) cout << "\tUsing multiline output." << endl;
            {
                const char EXPECTED_OUTPUT[] = "[\n"
                                               "\"Hello\\x0D\"\n"
                                               "\"World\\x0A\"\n"
                                               "]\n";

                stringstream ss;

                ostream& ret = bdlb::PrintMethods_Imp<
                                                 Type,
                                                 BdlbPrintMethod>::print(ss,
                                                                         VALUE,
                                                                         0,
                                                                         0);

                ASSERT(&ss == &ret);
                LOOP2_ASSERT(EXPECTED_OUTPUT,   ss.str(),
                             EXPECTED_OUTPUT == ss.str());
            }

            if (veryVerbose) cout << "\tUsing single line output." << endl;
            {
                const char EXPECTED_OUTPUT[] = "[ "
                                               "\"Hello\\x0D\" "
                                               "\"World\\x0A\" "
                                               "]";

                stringstream ss;

                ostream& ret = bdlb::PrintMethods_Imp<
                                                 Type,
                                                 BdlbPrintMethod>::print(ss,
                                                                         VALUE,
                                                                         0,
                                                                         -1);

                ASSERT(&ss == &ret);
                LOOP2_ASSERT(EXPECTED_OUTPUT,   ss.str(),
                             EXPECTED_OUTPUT == ss.str());
            }
        }

        if (verbose) cout << "\nTesting with 'vector<int>' elements." << endl;
        {
            typedef vector<int>      ElemType;
            typedef vector<ElemType> Type;
            typedef bslmf::SelectTraitCase<bslalg::HasStlIterators>::Type
                                     BdlbPrintMethod;

            const int FIRST_DATA[]  = { 2, 6, 23 };
            const int SECOND_DATA[] = { 54, 2 };

            const int NUM_FIRST_DATA  = sizeof  FIRST_DATA
                                      / sizeof *FIRST_DATA;
            const int NUM_SECOND_DATA = sizeof  SECOND_DATA
                                      / sizeof *SECOND_DATA;

            const ElemType DATA[] = { ElemType(FIRST_DATA,
                                               FIRST_DATA  + NUM_FIRST_DATA),
                                      ElemType(SECOND_DATA,
                                               SECOND_DATA + NUM_SECOND_DATA)
                                    };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            const Type VALUE(DATA, DATA + NUM_DATA);

            if (veryVerbose) cout << "\tUsing multiline output." << endl;
            {
                const char EXPECTED_OUTPUT[] = "[\n"
                                               "[\n"
                                               "2\n"
                                               "6\n"
                                               "23\n"
                                               "]\n"
                                               "[\n"
                                               "54\n"
                                               "2\n"
                                               "]\n"
                                               "]\n";

                stringstream ss;

                ostream& ret = bdlb::PrintMethods_Imp<
                                                 Type,
                                                 BdlbPrintMethod>::print(ss,
                                                                         VALUE,
                                                                         0,
                                                                         0);

                ASSERT(&ss == &ret);
                LOOP2_ASSERT(EXPECTED_OUTPUT,   ss.str(),
                             EXPECTED_OUTPUT == ss.str());
            }

            if (veryVerbose) cout << "\tUsing single line output." << endl;
            {
                const char EXPECTED_OUTPUT[] = "[ [ 2 6 23 ] [ 54 2 ] ]";

                stringstream ss;

                ostream& ret = bdlb::PrintMethods_Imp<
                                                 Type,
                                                 BdlbPrintMethod>::print(ss,
                                                                         VALUE,
                                                                         0,
                                                                         -1);

                ASSERT(&ss == &ret);
                LOOP2_ASSERT(EXPECTED_OUTPUT,   ss.str(),
                             EXPECTED_OUTPUT == ss.str());
            }
        }

        if (verbose) cout << "\nTesting with 'pair<...>' elements." << endl;
        {
            typedef pair<int, double> ElemType;
            typedef vector<ElemType>  Type;
            typedef bslmf::SelectTraitCase<bslalg::HasStlIterators>::Type
                                      BdlbPrintMethod;

            const ElemType DATA[]   = { ElemType(45,  1.23),
                                        ElemType(21, 97.54) };
            const int      NUM_DATA = sizeof DATA / sizeof *DATA;

            const Type VALUE(DATA, DATA + NUM_DATA);

            if (veryVerbose) cout << "\tUsing multiline output." << endl;
            {
                const char EXPECTED_OUTPUT[] = "[\n"
                                               "[\n"
                                               "45\n"
                                               "1.23\n"
                                               "]\n"
                                               "[\n"
                                               "21\n"
                                               "97.54\n"
                                               "]\n"
                                               "]\n";

                stringstream ss;

                ostream& ret = bdlb::PrintMethods_Imp<
                                                 Type,
                                                 BdlbPrintMethod>::print(ss,
                                                                         VALUE,
                                                                         0,
                                                                         0);

                ASSERT(&ss == &ret);
                LOOP2_ASSERT(EXPECTED_OUTPUT,   ss.str(),
                             EXPECTED_OUTPUT == ss.str());
            }

            if (veryVerbose) cout << "\tUsing single line output." << endl;
            {
                const char EXPECTED_OUTPUT[] = "[ [ 45 1.23 ] [ 21 97.54 ] ]";

                stringstream ss;

                ostream& ret = bdlb::PrintMethods_Imp<
                                                 Type,
                                                 BdlbPrintMethod>::print(ss,
                                                                         VALUE,
                                                                         0,
                                                                         -1);

                ASSERT(&ss == &ret);
                LOOP2_ASSERT(EXPECTED_OUTPUT,   ss.str(),
                             EXPECTED_OUTPUT == ss.str());
            }
        }

        if (verbose) cout << "\nTesting with 'TestType_PrintMethod' elements."
                          << endl;
        {
            typedef TestType_PrintMethod ElemType;
            typedef vector<ElemType> Type;
            typedef bslmf::SelectTraitCase<bslalg::HasStlIterators>::Type
                                     BdlbPrintMethod;

            if (veryVerbose) cout << "\tUsing multiline output." << endl;
            {
                const TestType_PrintMethod DATA[1];
                const int                  NUM_DATA = 1;

                const Type VALUE(DATA, DATA + NUM_DATA);

                const int LEVEL            = 3;
                const int SPACES_PER_LEVEL = 8;

                const int EXPECTED_LEVEL            = 4;
                const int EXPECTED_SPACES_PER_LEVEL = 8;

                stringstream ss;

                ostream& ret = bdlb::PrintMethods_Imp<Type,
                                                      BdlbPrintMethod>::print(
                                                             ss,
                                                             VALUE,
                                                             LEVEL,
                                                             SPACES_PER_LEVEL);

                ASSERT(&ss == &ret);

                ASSERT(&ss == VALUE[0].stream());

                LOOP2_ASSERT(EXPECTED_LEVEL,   VALUE[0].level(),
                             EXPECTED_LEVEL == VALUE[0].level());

                LOOP2_ASSERT(
                    EXPECTED_SPACES_PER_LEVEL,   VALUE[0].spacesPerLevel(),
                    EXPECTED_SPACES_PER_LEVEL == VALUE[0].spacesPerLevel());
            }

            if (veryVerbose) cout << "\tUsing single line output." << endl;
            {
                const TestType_PrintMethod DATA[1];
                const int                  NUM_DATA = 1;

                const Type VALUE(DATA, DATA + NUM_DATA);

                const int LEVEL            =  3;
                const int SPACES_PER_LEVEL = -8;

                const int EXPECTED_LEVEL            =  0;
                const int EXPECTED_SPACES_PER_LEVEL = -1;

                stringstream ss;

                ostream& ret = bdlb::PrintMethods_Imp<Type,
                                                      BdlbPrintMethod>::print(
                                                             ss,
                                                             VALUE,
                                                             LEVEL,
                                                             SPACES_PER_LEVEL);

                ASSERT(&ss == &ret);

                ASSERT(&ss == VALUE[0].stream());

                LOOP2_ASSERT(EXPECTED_LEVEL,   VALUE[0].level(),
                             EXPECTED_LEVEL == VALUE[0].level());

                LOOP2_ASSERT(
                    EXPECTED_SPACES_PER_LEVEL,   VALUE[0].spacesPerLevel(),
                    EXPECTED_SPACES_PER_LEVEL == VALUE[0].spacesPerLevel());
            }
        }

        if (verbose) cout << "\nTesting with invalid stream." << endl;
        {
            typedef vector<int> Type;
            typedef bslmf::SelectTraitCase<bslalg::HasStlIterators>::Type
                                BdlbPrintMethod;

            const Type VALUE;

            stringstream ss;

            ss.setstate(ios_base::badbit);

            ostream& ret = bdlb::PrintMethods_Imp<Type,
                                                  BdlbPrintMethod>::print(
                                                                         ss,
                                                                         VALUE,
                                                                         0,
                                                                         -1);

            ASSERT(&ss == &ret);
            LOOP_ASSERT(ss.str(), "" == ss.str());
        }
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING 'IsPair' PRINT IMPLEMENTATION
        //   This will test the print implementation function for pairs.
        //
        // Concerns:
        //: 1 Indenting should work as expected.
        //: 2 The appropriate print method should be called for each element.
        //: 2 Printing with bad streams should be a no-op.
        //
        // Plan:
        //: 1 First, test indenting using simple types, like 'int' and
        //:   'double'.
        //: 2 Then, to test that this function routes to the correct print
        //:   method for the contained elements, test pairs that contain
        //:   objects that invoke different print methods, i.e., use the
        //:   following pair types:
        //:   o 'pair<vector<char>, vector<char> >'
        //:   o 'pair<vector<int>, vector<int> >'
        //:   o 'pair<pair<int, int>, pair<double, double> >'
        //:   o 'pair<TestType_PrintMethod, TestType_PrintMethod>'
        //
        // Testing:
        //   bdlb::PrintMethods_Imp<TYPE, bslmf::IsPair>::print(...);
        // --------------------------------------------------------------------

        if (verbose) cout << "\n" "TESTING 'IsPair' PRINT IMPLEMENTATION"
                          << "\n" "====================================="
                          << endl;

        if (verbose) cout << "\nTesting indentation." << endl;
        {
            typedef pair<int, double> Type;
            typedef bslmf::SelectTraitCase<bslmf::IsPair>::Type
                                      BdlbPrintMethod;

            const int    INT_VALUE    = 45;
            const double DOUBLE_VALUE =  1.23;
            const Type   VALUE        = Type(INT_VALUE, DOUBLE_VALUE);

            static const struct {
                int         d_lineNum;
                int         d_level;
                int         d_spacesPerLevel;
                const char *d_expectedResult;
            } DATA[] = {
                //LINE   LEVEL   SPCS/LVL    EXPECTED_RESULT
                //----   -----   --------    ---------------

                // level = -2, spacesPerLevel = -2 .. +2
                { L_,    -2,     -2,         "[ 45 1.23 ]"                   },
                { L_,    -2,     -1,         "[ 45 1.23 ]"                   },
                { L_,    -2,     0,          "[\n"
                                             "45\n"
                                             "1.23\n"
                                             "]\n"                           },
                { L_,    -2,     1,          "[\n"
                                             "   45\n"
                                             "   1.23\n"
                                             "  ]\n"                         },
                { L_,    -2,     2,          "[\n"
                                             "      45\n"
                                             "      1.23\n"
                                             "    ]\n"                       },

                // level = -1, spacesPerLevel = -2 .. +2
                { L_,    -1,     -2,         "[ 45 1.23 ]"                   },
                { L_,    -1,     -1,         "[ 45 1.23 ]"                   },
                { L_,    -1,     0,          "[\n"
                                             "45\n"
                                             "1.23\n"
                                             "]\n"                           },
                { L_,    -1,     1,          "[\n"
                                             "  45\n"
                                             "  1.23\n"
                                             " ]\n"                          },
                { L_,    -1,     2,          "[\n"
                                             "    45\n"
                                             "    1.23\n"
                                             "  ]\n"                         },

                // level = 0, spacesPerLevel = -2 .. +2
                { L_,    0,      -2,         "[ 45 1.23 ]"                   },
                { L_,    0,      -1,         "[ 45 1.23 ]"                   },
                { L_,    0,      0,          "[\n"
                                             "45\n"
                                             "1.23\n"
                                             "]\n"                           },
                { L_,    0,      1,          "[\n"
                                             " 45\n"
                                             " 1.23\n"
                                             "]\n"                           },
                { L_,    0,      2,          "[\n"
                                             "  45\n"
                                             "  1.23\n"
                                             "]\n"                           },

                // level = 1, spacesPerLevel = -2 .. +2
                { L_,    1,      -2,         "  [ 45 1.23 ]"                 },
                { L_,    1,      -1,         " [ 45 1.23 ]"                  },
                { L_,    1,      0,          "[\n"
                                             "45\n"
                                             "1.23\n"
                                             "]\n"                           },
                { L_,    1,      1,          " [\n"
                                             "  45\n"
                                             "  1.23\n"
                                             " ]\n"                          },
                { L_,    1,      2,          "  [\n"
                                             "    45\n"
                                             "    1.23\n"
                                             "  ]\n"                         },

                // level = 2, spacesPerLevel = -2 .. +2
                { L_,    2,      -2,         "    [ 45 1.23 ]"               },
                { L_,    2,      -1,         "  [ 45 1.23 ]"                 },
                { L_,    2,      0,          "[\n"
                                             "45\n"
                                             "1.23\n"
                                             "]\n"                           },
                { L_,    2,      1,          "  [\n"
                                             "   45\n"
                                             "   1.23\n"
                                             "  ]\n"                         },
                { L_,    2,      2,          "    [\n"
                                             "      45\n"
                                             "      1.23\n"
                                             "    ]\n"                       },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int   LINE             = DATA[i].d_lineNum;
                const int   LEVEL            = DATA[i].d_level;
                const int   SPACES_PER_LEVEL = DATA[i].d_spacesPerLevel;
                const char *EXPECTED_RESULT  = DATA[i].d_expectedResult;

                stringstream ss;

                ostream& ret = bdlb::PrintMethods_Imp<Type,
                                                      BdlbPrintMethod>::print(
                                                             ss,
                                                             VALUE,
                                                             LEVEL,
                                                             SPACES_PER_LEVEL);

                LOOP_ASSERT(LINE, &ss == &ret);
                LOOP3_ASSERT(LINE, EXPECTED_RESULT,   ss.str(),
                                   EXPECTED_RESULT == ss.str());
            }
        }

        if (verbose) cout << "\nTesting with 'vector<char>' elements." << endl;
        {
            typedef pair<vector<char>, vector<char> > Type;
            typedef bslmf::SelectTraitCase<bslmf::IsPair>::Type
                                                      BdlbPrintMethod;

            const char FIRST_DATA[]  = "Hello\r";
            const char SECOND_DATA[] = "World\n";

            const vector<char> FIRST_VALUE(FIRST_DATA,
                                           FIRST_DATA + sizeof(FIRST_DATA)-1);
            const vector<char> SECOND_VALUE(
                                          SECOND_DATA,
                                          SECOND_DATA + sizeof(SECOND_DATA)-1);

            const Type VALUE = Type(FIRST_VALUE, SECOND_VALUE);

            if (veryVerbose) cout << "\tUsing multiline output." << endl;
            {
                const char EXPECTED_OUTPUT[] = "[\n"
                                               "\"Hello\\x0D\"\n"
                                               "\"World\\x0A\"\n"
                                               "]\n";

                stringstream ss;

                ostream& ret = bdlb::PrintMethods_Imp<Type,
                                                      BdlbPrintMethod>::print(
                                                                         ss,
                                                                         VALUE,
                                                                         0,
                                                                         0);

                ASSERT(&ss == &ret);
                LOOP2_ASSERT(EXPECTED_OUTPUT,   ss.str(),
                             EXPECTED_OUTPUT == ss.str());
            }

            if (veryVerbose) cout << "\tUsing single line output." << endl;
            {
                const char EXPECTED_OUTPUT[] = "[ "
                                               "\"Hello\\x0D\" "
                                               "\"World\\x0A\" "
                                               "]";

                stringstream ss;

                ostream& ret = bdlb::PrintMethods_Imp<Type,
                                                      BdlbPrintMethod>::print(
                                                                        ss,
                                                                        VALUE,
                                                                        0, -1);

                ASSERT(&ss == &ret);
                LOOP2_ASSERT(EXPECTED_OUTPUT,   ss.str(),
                             EXPECTED_OUTPUT == ss.str());
            }
        }

        if (verbose) cout << "\nTesting with 'vector<int>' elements." << endl;
        {
            typedef pair<vector<int>, vector<int> > Type;
            typedef bslmf::SelectTraitCase<bslmf::IsPair>::Type
                                                    BdlbPrintMethod;

            const int FIRST_DATA[]  = { 2, 6, 23 };
            const int SECOND_DATA[] = { 54, 2 };

            const int NUM_FIRST_DATA  = sizeof FIRST_DATA / sizeof *FIRST_DATA;
            const int NUM_SECOND_DATA = sizeof SECOND_DATA
                                        / sizeof *SECOND_DATA;

            const vector<int> FIRST_VALUE(FIRST_DATA,
                                          FIRST_DATA + NUM_FIRST_DATA);
            const vector<int> SECOND_VALUE(SECOND_DATA,
                                           SECOND_DATA + NUM_SECOND_DATA);

            const Type VALUE = Type(FIRST_VALUE, SECOND_VALUE);

            if (veryVerbose) cout << "\tUsing multiline output." << endl;
            {
                const char EXPECTED_OUTPUT[] = "[\n"
                                               "[\n"
                                               "2\n"
                                               "6\n"
                                               "23\n"
                                               "]\n"
                                               "[\n"
                                               "54\n"
                                               "2\n"
                                               "]\n"
                                               "]\n";

                stringstream ss;

                ostream& ret = bdlb::PrintMethods_Imp<Type,
                                                      BdlbPrintMethod>::print(
                                                                         ss,
                                                                         VALUE,
                                                                         0,
                                                                         0);

                ASSERT(&ss == &ret);
                LOOP2_ASSERT(EXPECTED_OUTPUT,   ss.str(),
                             EXPECTED_OUTPUT == ss.str());
            }

            if (veryVerbose) cout << "\tUsing single line output." << endl;
            {
                const char EXPECTED_OUTPUT[] = "[ [ 2 6 23 ] [ 54 2 ] ]";

                stringstream ss;

                ostream& ret = bdlb::PrintMethods_Imp<Type,
                                                      BdlbPrintMethod>::print(
                                                                        ss,
                                                                        VALUE,
                                                                        0, -1);

                ASSERT(&ss == &ret);
                LOOP2_ASSERT(EXPECTED_OUTPUT,   ss.str(),
                             EXPECTED_OUTPUT == ss.str());
            }
        }

        if (verbose) cout << "\nTesting with 'pair<...>' elements." << endl;
        {
            typedef pair<int, int>            IntPair;
            typedef pair<double, double>      DoublePair;
            typedef pair<IntPair, DoublePair> Type;
            typedef bslmf::SelectTraitCase<bslmf::IsPair>::Type
                                              BdlbPrintMethod;

            const Type VALUE = Type(IntPair(45, 21),
                                    DoublePair(1.23, 97.54));

            if (veryVerbose) cout << "\tUsing multiline output." << endl;
            {
                const char EXPECTED_OUTPUT[] = "[\n"
                                               "[\n"
                                               "45\n"
                                               "21\n"
                                               "]\n"
                                               "[\n"
                                               "1.23\n"
                                               "97.54\n"
                                               "]\n"
                                               "]\n";

                stringstream ss;

                ostream& ret = bdlb::PrintMethods_Imp<Type,
                                                      BdlbPrintMethod>::print(
                                                                         ss,
                                                                         VALUE,
                                                                         0,
                                                                         0);

                ASSERT(&ss == &ret);
                LOOP2_ASSERT(EXPECTED_OUTPUT,   ss.str(),
                             EXPECTED_OUTPUT == ss.str());
            }

            if (veryVerbose) cout << "\tUsing single line output." << endl;
            {
                const char EXPECTED_OUTPUT[] = "[ [ 45 21 ] [ 1.23 97.54 ] ]";

                stringstream ss;

                ostream& ret = bdlb::PrintMethods_Imp<Type,
                                                      BdlbPrintMethod>::print(
                                                                        ss,
                                                                        VALUE,
                                                                        0, -1);

                ASSERT(&ss == &ret);
                LOOP2_ASSERT(EXPECTED_OUTPUT,   ss.str(),
                             EXPECTED_OUTPUT == ss.str());
            }
        }

        if (verbose) cout << "\nTesting with 'TestType_PrintMethod' elements."
                          << endl;
        {
            typedef pair<TestType_PrintMethod, TestType_PrintMethod> Type;
            typedef bslmf::SelectTraitCase<bslmf::IsPair>::Type
                                                               BdlbPrintMethod;

            if (veryVerbose) cout << "\tUsing multiline output." << endl;
            {
                const Type VALUE;

                const int LEVEL            = 3;
                const int SPACES_PER_LEVEL = 8;

                const int EXPECTED_LEVEL            = 4;
                const int EXPECTED_SPACES_PER_LEVEL = 8;

                stringstream ss;

                ostream& ret = bdlb::PrintMethods_Imp<Type,
                                                      BdlbPrintMethod>::print(
                                                             ss,
                                                             VALUE,
                                                             LEVEL,
                                                             SPACES_PER_LEVEL);

                ASSERT(&ss == &ret);

                ASSERT(&ss == VALUE.first.stream());
                ASSERT(&ss == VALUE.second.stream());

                LOOP2_ASSERT(EXPECTED_LEVEL,   VALUE.first.level(),
                             EXPECTED_LEVEL == VALUE.first.level());
                LOOP2_ASSERT(EXPECTED_LEVEL,   VALUE.second.level(),
                             EXPECTED_LEVEL == VALUE.second.level());

                LOOP2_ASSERT(
                    EXPECTED_SPACES_PER_LEVEL,   VALUE.first.spacesPerLevel(),
                    EXPECTED_SPACES_PER_LEVEL == VALUE.first.spacesPerLevel());

                LOOP2_ASSERT(
                   EXPECTED_SPACES_PER_LEVEL,   VALUE.second.spacesPerLevel(),
                   EXPECTED_SPACES_PER_LEVEL == VALUE.second.spacesPerLevel());
            }

            if (veryVerbose) cout << "\tUsing single line output." << endl;
            {
                const Type VALUE;

                const int LEVEL            = 3;
                const int SPACES_PER_LEVEL = -8;

                const int EXPECTED_LEVEL            = 0;
                const int EXPECTED_SPACES_PER_LEVEL = -1;

                stringstream ss;

                ostream& ret = bdlb::PrintMethods_Imp<Type,
                                                      BdlbPrintMethod>::print(
                                                             ss,
                                                             VALUE,
                                                             LEVEL,
                                                             SPACES_PER_LEVEL);

                ASSERT(&ss == &ret);

                ASSERT(&ss == VALUE.first.stream());
                ASSERT(&ss == VALUE.second.stream());

                LOOP2_ASSERT(EXPECTED_LEVEL,   VALUE.first.level(),
                             EXPECTED_LEVEL == VALUE.first.level());
                LOOP2_ASSERT(EXPECTED_LEVEL,   VALUE.second.level(),
                             EXPECTED_LEVEL == VALUE.second.level());

                LOOP2_ASSERT(
                    EXPECTED_SPACES_PER_LEVEL,   VALUE.first.spacesPerLevel(),
                    EXPECTED_SPACES_PER_LEVEL == VALUE.first.spacesPerLevel());

                LOOP2_ASSERT(
                   EXPECTED_SPACES_PER_LEVEL,   VALUE.second.spacesPerLevel(),
                   EXPECTED_SPACES_PER_LEVEL == VALUE.second.spacesPerLevel());
            }
        }

        if (verbose) cout << "\nTesting with invalid stream." << endl;
        {
            typedef pair<int, double> Type;
            typedef bslmf::SelectTraitCase<bslmf::IsPair>::Type
                                      BdlbPrintMethod;

            const int    INT_VALUE    = 45;
            const double DOUBLE_VALUE = 1.23;
            const Type   VALUE        = Type(INT_VALUE, DOUBLE_VALUE);

            stringstream ss;

            ss.setstate(ios_base::badbit);

            ostream& ret = bdlb::PrintMethods_Imp<Type, BdlbPrintMethod>::
                                                       print(ss, VALUE, 0, -1);

            ASSERT(&ss == &ret);
            LOOP_ASSERT(ss.str(), "" == ss.str());
        }
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING 'HasPrintMethod' PRINT IMPLEMENTATION
        //   This will test the print implementation function that uses the
        //   object's 'print' method.
        //
        // Concerns:
        //: 1 The arguments should be passed correctly.
        //
        // Plan:
        //: 1 Use the 'TestType_PrintMethod' class to test the values passed to
        //:   the 'print' method.
        //: 2 Use varying values for 'level' and 'spacesPerLevel'.
        //
        // Testing:
        //   bdlb::PrintMethods_Imp<TYPE, bdlb::HasPrintMethod>::print(...);
        // --------------------------------------------------------------------

        if (verbose) cout
                        << "\n" "TESTING 'HasPrintMethod' PRINT IMPLEMENTATION"
                        << "\n" "============================================="
                        << endl;

        static const struct {
            int d_lineNum;
            int d_level;
            int d_spacesPerLevel;
        } DATA[] = {
            //LINE   LEVEL   SPCS/LVL
            //----   -----   --------
            { L_,    -2,     -2,       },
            { L_,    -2,     -1,       },
            { L_,    -2,      0,       },
            { L_,    -2,      1,       },
            { L_,    -2,      2,       },

            { L_,    -1,     -2,       },
            { L_,    -1,     -1,       },
            { L_,    -1,      0,       },
            { L_,    -1,      1,       },
            { L_,    -1,      2,       },

            { L_,     0,      -2,      },
            { L_,     0,      -1,      },
            { L_,     0,       0,      },
            { L_,     0,       1,      },
            { L_,     0,       2,      },

            { L_,     1,      -2,      },
            { L_,     1,      -1,      },
            { L_,     1,       0,      },
            { L_,     1,       1,      },
            { L_,     1,       2,      },

            { L_,     2,      -2,      },
            { L_,     2,      -1,      },
            { L_,     2,       0,      },
            { L_,     2,       1,      },
            { L_,     2,       2,      },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int i = 0; i < NUM_DATA; ++i) {
            const int LINE             = DATA[i].d_lineNum;
            const int LEVEL            = DATA[i].d_level;
            const int SPACES_PER_LEVEL = DATA[i].d_spacesPerLevel;

            typedef TestType_PrintMethod Type;
            typedef bslmf::SelectTraitCase<bdlb::HasPrintMethod>::Type
                                         BdlbPrintMethod;

            const Type VALUE;

            stringstream ss;

            ostream& ret = bdlb::PrintMethods_Imp<Type, BdlbPrintMethod>::
                                     print(ss, VALUE, LEVEL, SPACES_PER_LEVEL);

            LOOP_ASSERT(LINE, &ss == &ret);
            LOOP_ASSERT(LINE, &ss == VALUE.stream());
            LOOP2_ASSERT(LINE,               VALUE.level(),
                         LEVEL            == VALUE.level());
            LOOP2_ASSERT(LINE,               VALUE.spacesPerLevel(),
                         SPACES_PER_LEVEL == VALUE.spacesPerLevel());
        }
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING 'operator<<'-BASED PRINT IMPLEMENTATION
        //   This will test the print implementation function that uses the
        //   '<<' output stream operator.
        //
        // Concerns:
        //: 1 Indentation should work as expected.
        //: 2 Printing with bad streams should be a no-op.
        //
        // Plan:
        //: 1 Using a string stream, test the print implementation method using
        //:   'int', 'double' and 'bsl::string' objects, using varying
        //:   level &  spacesPerLevel values.  Check that the results are as
        //:   expected.
        //
        // Testing:
        //   bdlb::PrintMethods_Imp<TYPE, bsl::false_type>::print(...);
        // --------------------------------------------------------------------

        if (verbose)
            cout << "\n" "TESTING 'operator<<'-BASED PRINT IMPLEMENTATION"
                 << "\n" "==============================================="
                 << endl;

        // false_type == Default == stream operator
        typedef bslmf::SelectTraitCase<>::Type BdlbPrintMethod;

        static const struct {
            int         d_lineNum;
            int         d_level;
            int         d_spacesPerLevel;
            const char *d_expectedPrefix;
            const char *d_expectedSuffix;
        } DATA[] = {
            //LINE   LEVEL   SPCS/LVL    PREFIX    SUFFIX
            //----   -----   --------    ------    ------
            { L_,    -2,     -2,         "",       ""        },
            { L_,    -2,     -1,         "",       ""        },
            { L_,    -2,      0,         "",       "\n"      },
            { L_,    -2,      1,         "",       "\n"      },
            { L_,    -2,      2,         "",       "\n"      },

            { L_,    -1,     -2,         "",       ""        },
            { L_,    -1,     -1,         "",       ""        },
            { L_,    -1,      0,         "",       "\n"      },
            { L_,    -1,      1,         "",       "\n"      },
            { L_,    -1,      2,         "",       "\n"      },

            { L_,     0,     -2,         "",       ""        },
            { L_,     0,     -1,         "",       ""        },
            { L_,     0,      0,         "",       "\n"      },
            { L_,     0,      1,         "",       "\n"      },
            { L_,     0,      2,         "",       "\n"      },

            { L_,     1,     -2,         "  ",     ""        },
            { L_,     1,     -1,         " ",      ""        },
            { L_,     1,      0,         "",       "\n"      },
            { L_,     1,      1,         " ",      "\n"      },
            { L_,     1,      2,         "  ",     "\n"      },

            { L_,     2,      -2,        "    ",   ""        },
            { L_,     2,      -1,        "  ",     ""        },
            { L_,     2,       0,        "",       "\n"      },
            { L_,     2,       1,        "  ",     "\n"      },
            { L_,     2,       2,        "    ",   "\n"      },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        if (verbose) cout << "\nUsing 'int' object." << endl;
        {
            typedef int Type;

            const Type   VALUE          =  45;
            const string EXPECTED_VALUE = "45";

            for (int i = 0; i < NUM_DATA; ++i) {
                const int   LINE             = DATA[i].d_lineNum;
                const int   LEVEL            = DATA[i].d_level;
                const int   SPACES_PER_LEVEL = DATA[i].d_spacesPerLevel;
                const char *EXPECTED_PREFIX  = DATA[i].d_expectedPrefix;
                const char *EXPECTED_SUFFIX  = DATA[i].d_expectedSuffix;

                const string EXPECTED_RESULT = EXPECTED_PREFIX
                                             + EXPECTED_VALUE
                                             + EXPECTED_SUFFIX;

                stringstream ss;

                ostream& ret = bdlb::PrintMethods_Imp<Type,
                                                      BdlbPrintMethod>::print(
                                                             ss,
                                                             VALUE,
                                                             LEVEL,
                                                             SPACES_PER_LEVEL);

                LOOP_ASSERT(LINE, &ss == &ret);
                LOOP3_ASSERT(LINE, EXPECTED_RESULT,   ss.str(),
                                   EXPECTED_RESULT == ss.str());
            }
        }

        if (verbose) cout << "\nUsing 'double' object." << endl;
        {
            typedef double Type;

            const Type   VALUE          =  45.678;
            const string EXPECTED_VALUE = "45.678";

            for (int i = 0; i < NUM_DATA; ++i) {
                const int   LINE             = DATA[i].d_lineNum;
                const int   LEVEL            = DATA[i].d_level;
                const int   SPACES_PER_LEVEL = DATA[i].d_spacesPerLevel;
                const char *EXPECTED_PREFIX  = DATA[i].d_expectedPrefix;
                const char *EXPECTED_SUFFIX  = DATA[i].d_expectedSuffix;

                const string EXPECTED_RESULT = EXPECTED_PREFIX
                                             + EXPECTED_VALUE
                                             + EXPECTED_SUFFIX;

                stringstream ss;

                ostream& ret = bdlb::PrintMethods_Imp<Type,
                                                      BdlbPrintMethod>::print(
                                                             ss,
                                                             VALUE,
                                                             LEVEL,
                                                             SPACES_PER_LEVEL);

                LOOP_ASSERT(LINE, &ss == &ret);
                LOOP3_ASSERT(LINE, EXPECTED_RESULT,   ss.str(),
                                   EXPECTED_RESULT == ss.str());
            }
        }

#if 0
        if (verbose) cout << "\nUsing 'bsl::string' object." << endl;
        {
            typedef bsl::string Type;

            const Type   VALUE          = "Hello World";
            const string EXPECTED_VALUE = "Hello World";

            for (int i = 0; i < NUM_DATA; ++i) {
                const int   LINE             = DATA[i].d_lineNum;
                const int   LEVEL            = DATA[i].d_level;
                const int   SPACES_PER_LEVEL = DATA[i].d_spacesPerLevel;
                const char *EXPECTED_PREFIX  = DATA[i].d_expectedPrefix;
                const char *EXPECTED_SUFFIX  = DATA[i].d_expectedSuffix;

                const string EXPECTED_RESULT = EXPECTED_PREFIX
                                               + EXPECTED_VALUE
                                               + EXPECTED_SUFFIX;

                stringstream ss;

                ostream& ret = bdlb::PrintMethods_Imp<Type,
                                                      BdlbPrintMethod>::print(
                                                             ss,
                                                             VALUE,
                                                             LEVEL,
                                                             SPACES_PER_LEVEL);

                LOOP_ASSERT(LINE, &ss == &ret);
                LOOP3_ASSERT(LINE, EXPECTED_RESULT,   ss.str(),
                                   EXPECTED_RESULT == ss.str());
            }
        }
#endif

        if (verbose) cout << "\nTesting with invalid stream." << endl;
        {
            typedef int Type;

            const Type VALUE = 45;

            stringstream ss;

            ss.setstate(ios_base::badbit);

            ostream& ret = bdlb::PrintMethods_Imp<Type,
                                                  BdlbPrintMethod>::print(
                                                                         ss,
                                                                         VALUE,
                                                                         0,
                                                                         -1);

            ASSERT(&ss == &ret);
            LOOP_ASSERT(ss.str(), "" == ss.str());
        }
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING 'vector<char>' 'print' METHOD
        //   This will test the specialized 'print' method for 'vector<char>'
        //   objects.
        //
        // Concerns:
        //: 1 Output should be enclosed in double quotes.
        //: 2 Indentation should work as expected.
        //: 3 Non-printable characters must be printed using their hexadecimal
        //:   representation.
        //: 4 Interleaved printable and non-printable characters must work as
        //:   expected.
        //: 5 Printing with bad streams should be a no-op.
        //
        // Plan:
        //: 1 For each vector in an array of 'vector<char>' objects, print the
        //:   vector to a string stream and verify that the results are as
        //:   expected.
        //
        // Testing:
        //   bdlb::PrintMethods::print(..., const vector<char, ALLOC>, ...);
        // --------------------------------------------------------------------

        if (verbose) cout << "\n" "TESTING 'vector<char>' 'print' METHOD"
                          << "\n" "====================================="
                          << endl;

        static const struct {
            int         d_lineNum;
            const char *d_spec;
            int         d_level;
            int         d_spacesPerLevel;
            const char *d_expectedResult;
        } DATA[] = {
            //LINE   SPEC          LEVEL SPCS/LVL  EXPECTED_RESULT
            //----   ----          ----- --------  ---------------

            // Indenting & spacing.
            { L_,    "",             -1,    -2,    "\"\""                   },
            { L_,    "",             -1,    -1,    "\"\""                   },
            { L_,    "",             -1,     0,    "\"\"\n"                 },
            { L_,    "",             -1,     1,    "\"\"\n"                 },
            { L_,    "",             -1,     2,    "\"\"\n"                 },
            { L_,    "",              0,    -2,    "\"\""                   },
            { L_,    "",              0,    -1,    "\"\""                   },
            { L_,    "",              0,     0,    "\"\"\n"                 },
            { L_,    "",              0,     1,    "\"\"\n"                 },
            { L_,    "",              0,     2,    "\"\"\n"                 },
            { L_,    "",              1,    -2,    "  \"\""                 },
            { L_,    "",              1,    -1,    " \"\""                  },
            { L_,    "",              1,     0,    "\"\"\n"                 },
            { L_,    "",              1,     1,    " \"\"\n"                },
            { L_,    "",              1,     2,    "  \"\"\n"               },
            { L_,    "",              2,    -2,    "    \"\""               },
            { L_,    "",              2,    -1,    "  \"\""                 },
            { L_,    "",              2,     0,    "\"\"\n"                 },
            { L_,    "",              2,     1,    "  \"\"\n"               },
            { L_,    "",              2,     2,    "    \"\"\n"             },

            // Hex conversion.  Note: 0x00 is tested separately below.
            { L_,    "\x01",          0,    -1,    "\"\\x01\""              },
            { L_,    "\x02",          0,    -1,    "\"\\x02\""              },
            { L_,    "\n",            0,    -1,    "\"\\x0A\""              },
            { L_,    "\r",            0,    -1,    "\"\\x0D\""              },
            { L_,    "\x7F",          0,    -1,    "\"\\x7F\""              },
            { L_,    "\x80",          0,    -1,    "\"\\x80\""              },
            { L_,    "\x81",          0,    -1,    "\"\\x81\""              },
            { L_,    "\xFE",          0,    -1,    "\"\\xFE\""              },
            { L_,    "\xFF",          0,    -1,    "\"\\xFF\""              },

            // All printable characters, increasing length.
            { L_,    "a",             0,    -1,    "\"a\""                  },
            { L_,    "ab",            0,    -1,    "\"ab\""                 },
            { L_,    "abc",           0,    -1,    "\"abc\""                },
            { L_,    "abcd",          0,    -1,    "\"abcd\""               },
            { L_,    "abcde",         0,    -1,    "\"abcde\""              },

            // Interleaved printable/non-printable.
            { L_,    "a\rb\nc",       0,    -1,    "\"a\\x0Db\\x0Ac\""      },
            { L_,    "aA\rbB\ncC",    0,    -1,    "\"aA\\x0DbB\\x0AcC\""   },

            { L_,    "\r\n",          0,    -1,    "\"\\x0D\\x0A\""         },
            { L_,    "\r\nz",         0,    -1,    "\"\\x0D\\x0Az\""        },
            { L_,    "\r\nzy",        0,    -1,    "\"\\x0D\\x0Azy\""       },
            { L_,    "\r\nzyx",       0,    -1,    "\"\\x0D\\x0Azyx\""      },

            { L_,    "a\r\n",         0,    -1,    "\"a\\x0D\\x0A\""        },
            { L_,    "a\r\nz",        0,    -1,    "\"a\\x0D\\x0Az\""       },
            { L_,    "a\r\nzy",       0,    -1,    "\"a\\x0D\\x0Azy\""      },
            { L_,    "a\r\nzyx",      0,    -1,    "\"a\\x0D\\x0Azyx\""     },

            { L_,    "ab\r\n",        0,    -1,    "\"ab\\x0D\\x0A\""       },
            { L_,    "ab\r\nz",       0,    -1,    "\"ab\\x0D\\x0Az\""      },
            { L_,    "ab\r\nzy",      0,    -1,    "\"ab\\x0D\\x0Azy\""     },
            { L_,    "ab\r\nzyx",     0,    -1,    "\"ab\\x0D\\x0Azyx\""    },

            { L_,    "abc\r\n",       0,    -1,    "\"abc\\x0D\\x0A\""      },
            { L_,    "abc\r\nz",      0,    -1,    "\"abc\\x0D\\x0Az\""     },
            { L_,    "abc\r\nzy",     0,    -1,    "\"abc\\x0D\\x0Azy\""    },
            { L_,    "abc\r\nzyx",    0,    -1,    "\"abc\\x0D\\x0Azyx\""   },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        if (verbose) cout << "\nTesting with valid stream." << endl;
        for (int i = 0; i < NUM_DATA; ++i) {
            const int   LINE             = DATA[i].d_lineNum;
            const char *SPEC             = DATA[i].d_spec;
            const int   LEVEL            = DATA[i].d_level;
            const int   SPACES_PER_LEVEL = DATA[i].d_spacesPerLevel;
            const char *EXPECTED_RESULT  = DATA[i].d_expectedResult;

            const vector<char> VALUE(SPEC, SPEC + bsl::strlen(SPEC));

            stringstream ss;

            ostream& ret = bdlb::PrintMethods::print(ss,
                                                    VALUE,
                                                    LEVEL,
                                                    SPACES_PER_LEVEL);

            LOOP_ASSERT(LINE, &ss == &ret);
            LOOP3_ASSERT(LINE, EXPECTED_RESULT,   ss.str(),
                               EXPECTED_RESULT == ss.str());
        }

        if (verbose) cout << "\nTesting with invalid stream." << endl;
        for (int i = 0; i < NUM_DATA; ++i) {
            const int   LINE             = DATA[i].d_lineNum;
            const char *SPEC             = DATA[i].d_spec;
            const int   LEVEL            = DATA[i].d_level;
            const int   SPACES_PER_LEVEL = DATA[i].d_spacesPerLevel;

            const vector<char> VALUE(SPEC, SPEC + bsl::strlen(SPEC));

            stringstream ss;

            ss.setstate(ios_base::badbit);

            ostream& ret = bdlb::PrintMethods::print(ss,
                                                     VALUE,
                                                     LEVEL,
                                                     SPACES_PER_LEVEL);

            LOOP_ASSERT(LINE, &ss == &ret);
            LOOP2_ASSERT(LINE, ss.str(),
                         "" == ss.str());
        }

        if (verbose) cout << "\nTesting null hex conversion." << endl;
        {
            const char SPEC[] = "\x00";

            const vector<char> VALUE(SPEC, SPEC + 1);
            const char         EXPECTED_RETURN[] = "\"\\x00\"";

            stringstream ss;

            ostream& ret = bdlb::PrintMethods::print(ss, VALUE, 0, -1);

            ASSERT(&ss == &ret);
            LOOP_ASSERT(ss.str(), EXPECTED_RETURN == ss.str());
        }
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //   This case exercises (but does not fully test) basic functionality.
        //
        // Concerns:
        //: 1 The class is sufficiently functional to enable comprehensive
        //:   testing in subsequent test cases.
        //
        // Plan:
        //: 1 Print a selection of different types to an output string stream
        //:   and verify that the results are as expected.
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) cout << "\n" "BREATHING TEST" "\n"
                                  "==============" "\n";

        if (verbose) cout << "\nWith Fundamental Types." << endl;
        {
            if (veryVerbose) cout << "\tWith 'int'." << endl;
            {
                const int  VALUE             =  123;
                const char EXPECTED_OUTPUT[] = "123";

                stringstream ss;

                bdlb::PrintMethods::print(ss, VALUE, 0, -1);

                LOOP_ASSERT(ss.str(), EXPECTED_OUTPUT == ss.str());
            }

            if (veryVerbose) cout << "\tWith 'double'." << endl;
            {
                const double VALUE             =  123.456;
                const char   EXPECTED_OUTPUT[] = "123.456";

                stringstream ss;

                bdlb::PrintMethods::print(ss, VALUE, 0, -1);

                LOOP_ASSERT(ss.str(), EXPECTED_OUTPUT == ss.str());
            }
        }

        if (verbose) cout << "\nWith STL Containers." << endl;
        {
            if (veryVerbose) cout << "\tWith vector<char>." << endl;
            {
                const char DATA[]   = "Hello\0\rWorld\x7f";
                const int  NUM_DATA = sizeof(DATA) - 1;

                const vector<char> VALUE(DATA, DATA + NUM_DATA);
                const char         EXPECTED_OUTPUT[]
                                             = "\"Hello\\x00\\x0DWorld\\x7F\"";

                stringstream ss;

                bdlb::PrintMethods::print(ss, VALUE, 0, -1);

                LOOP_ASSERT(ss.str(), EXPECTED_OUTPUT == ss.str());
            }

            if (veryVerbose) cout << "\tWith vector<int>." << endl;
            {
                const int DATA[] = { 2, 5, 9, 21, 3, 45 };

                const int NUM_DATA = sizeof DATA / sizeof *DATA;

                const vector<int> VALUE(DATA, DATA + NUM_DATA);
                const char        EXPECTED_OUTPUT[] = "[ 2 5 9 21 3 45 ]";

                stringstream ss;

                bdlb::PrintMethods::print(ss, VALUE, 0, -1);

                LOOP_ASSERT(ss.str(), EXPECTED_OUTPUT == ss.str());
            }

            if (veryVerbose) cout << "\tWith map<string, int>." << endl;
            {
                typedef pair<const string, int> Pair;

                const Pair DATA[]   = { Pair("First",   2),
                                        Pair("Second",  9),
                                        Pair("Third",  21) };
                const int  NUM_DATA = sizeof DATA / sizeof *DATA;

                const map<string, int> VALUE(DATA, DATA + NUM_DATA);
                const char             EXPECTED_OUTPUT[] = "[ [ First 2 ] "
                                                           "[ Second 9 ] "
                                                           "[ Third 21 ] ]";

                stringstream ss;

                bdlb::PrintMethods::print(ss, VALUE, 0, -1);

                LOOP_ASSERT(ss.str(), EXPECTED_OUTPUT == ss.str());
            }
        }

        if (verbose) cout << "\nWith Pairs." << endl;
        {
            typedef pair<const string, int> Pair;

            const Pair VALUE("Hello", 45);
            const char EXPECTED_OUTPUT[] = "[ Hello 45 ]";

            stringstream ss;

            bdlb::PrintMethods::print(ss, VALUE, 0, -1);

            LOOP_ASSERT(ss.str(), EXPECTED_OUTPUT == ss.str());
        }

        if (verbose) cout << "\nWith 'print' Method." << endl;
        {
            const TestType_PrintMethod VALUE;
            const int LEVEL            = 4;
            const int SPACES_PER_LEVEL = 7;

            stringstream ss;

            bdlb::PrintMethods::print(ss, VALUE, LEVEL, SPACES_PER_LEVEL);

            ASSERT(&ss == VALUE.stream());
            LOOP_ASSERT(VALUE.level(),
                        LEVEL            == VALUE.level());
            LOOP_ASSERT(VALUE.spacesPerLevel(),
                        SPACES_PER_LEVEL == VALUE.spacesPerLevel());
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
