// balxml_typesprintutil.t.cpp                                        -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <balxml_typesprintutil.h>

#include <bdlat_enumeratorinfo.h>
#include <bdlat_enumfunctions.h>
#include <bdlat_typetraits.h>
#include <bdlat_valuetypefunctions.h>

#include <bdlb_chartype.h>
#include <bdlb_float.h>
#include <bdlb_nullablevalue.h>
#include <bdlb_print.h>
#include <bdlb_printmethods.h>

#include <bdlsb_memoutstreambuf.h>

#include <bdlt_datetime.h>

#include <bslalg_typetraits.h>

#include <bslim_testutil.h>

#include <bslalg_typetraits.h>

#include <bslma_allocator.h>

#include <bsls_assert.h>
#include <bsls_platform.h>
#include <bsls_types.h>

#include <s_baltst_customizedstring.h>
#include <s_baltst_generatetestarray.h>
#include <s_baltst_generatetestnullablevalue.h>
#include <s_baltst_myenumeration.h>
#include <s_baltst_testplaceholder.h>

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
#include <bsl_vector.h>

using namespace BloombergLP;
using bsl::cout;
using bsl::cerr;
using bsl::endl;

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

#define LOOP1_ASSERT(L, X) ASSERTV(L, X)
#define LOOP1_ASSERT_EQ(L, X, Y) ASSERTV(L, X, Y, X == Y)

#define Q            BSLIM_TESTUTIL_Q   // Quote identifier literally.
#define P            BSLIM_TESTUTIL_P   // Print identifier and value.
#define P_           BSLIM_TESTUTIL_P_  // P(X) without '\n'.
#define T_           BSLIM_TESTUTIL_T_  // Print a tab (w/o newline).
#define L_           BSLIM_TESTUTIL_L_  // current Line number

// ============================================================================
//                   GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
// ----------------------------------------------------------------------------

#if defined(BSLS_PLATFORM_CPU_64_BIT) && !defined(BSLS_PLATFORM_OS_WINDOWS)
#define U_LONG_IS_64_BITS
    // On 64 bit systems 'long' may be 32 or 64 bits.
#endif

typedef balxml::TypesPrintUtil Util;

                       // ==============================
                       // class AssertPrintedTextIsEqual
                       // ==============================

class AssertPrintedTextIsEqual {

  public:
    // CREATORS
    AssertPrintedTextIsEqual()
    {
    }

    // ACCESSORS
    template <class TYPE>
    void operator()(int                     line,
                    const TYPE&             value,
                    int                     formattingMode,
                    const bsl::string_view& expectedXml) const
    {
        bdlsb::MemOutStreamBuf streamBuffer;
        bsl::ostream           stream(&streamBuffer);

        Util::print(stream, value, formattingMode);
        LOOP1_ASSERT(line, static_cast<bool>(stream));

        const bsl::string_view actualXml(streamBuffer.data(),
                                         streamBuffer.length());

        LOOP1_ASSERT_EQ(line, expectedXml, actualXml);
    }
};

                            // ====================
                            // class GenerateVector
                            // ====================

template <class ELEMENT_TYPE>
class GenerateVector {

  public:
    // CREATORS
    GenerateVector()
    {
    }

    // ACCESSORS
    bsl::vector<ELEMENT_TYPE> operator()() const
    {
        return bsl::vector<ELEMENT_TYPE>();
    }

    bsl::vector<ELEMENT_TYPE> operator()(const ELEMENT_TYPE& element0) const
    {
        bsl::vector<ELEMENT_TYPE> result;
        result.push_back(element0);
        return result;
    }

    bsl::vector<ELEMENT_TYPE> operator()(const ELEMENT_TYPE& element0,
                                         const ELEMENT_TYPE& element1) const
    {
        bsl::vector<ELEMENT_TYPE> result;
        result.push_back(element0);
        result.push_back(element1);
        return result;
    }

    bsl::vector<ELEMENT_TYPE> operator()(const ELEMENT_TYPE& element0,
                                         const ELEMENT_TYPE& element1,
                                         const ELEMENT_TYPE& element2) const
    {
        bsl::vector<ELEMENT_TYPE> result;
        result.push_back(element0);
        result.push_back(element1);
        result.push_back(element2);
        return result;
    }
};

class MyStringRef : public bslstl::StringRef {
  public:
    // CREATORS
    MyStringRef(const char *data)                                   // IMPLICIT
    : bslstl::StringRef(data)
    {}

    MyStringRef(const bsl::string& string)                          // IMPLICIT
    : bslstl::StringRef(string)
    {}

    MyStringRef(const char *from, const char *to)
    : bslstl::StringRef(from, to) {}
};

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
    struct IsEnumeration<TestEnum::Value> : bsl::true_type {
    };

    template <>
    void toInt<TestEnum::Value>(int *result, const TestEnum::Value& value)
    {
        *result = static_cast<int>(value);
    }

    template <>
    void toString<TestEnum::Value>(bsl::string            *result,
                                   const TestEnum::Value&  value)
    {
        switch (value) {
          case TestEnum::VALUE1:
            *result = "VALUE1";
            break;
          case TestEnum::VALUE2:
            *result = "VALUE2";
            break;
          case TestEnum::VALUE3:
            *result = "VALUE3";
            break;
          default:
            *result = "(* UNKNOWN *)";
            break;
        }
    }

}  // close namespace bdlat_EnumFunctions
}  // close enterprise namespace


// ----------------------------------------------------------------------------
//                       *End-of-file Block removed.*
// ----------------------------------------------------------------------------

// test_customizedint.h                                               -*-C++-*-

//@PURPOSE: todo: provide purpose.
//
//@CLASSES:
//  CustomizedInt: a customized int
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

// test_customizedint.cpp                                             -*-C++-*-

namespace BloombergLP {
namespace s_baltst {

                               // ---------
                               // CONSTANTS
                               // ---------

const char CustomizedInt::CLASS_NAME[] = "s_baltst::CustomizedInt";
    // the name of this class

}  // close namespace s_baltst
}  // close enterprise namespace

// ----------------------------------------------------------------------------
//                       *End-of-file Block removed.*
// ----------------------------------------------------------------------------

bool matchFloatingPointResult(const bsl::string_view& result,
                              const bsl::string_view& matchPattern)
    // Match the specified floating point print 'result' against the specified
    // 'matchPattern' and return 'true' if 'result' is considered a match, or
    // 'false' if 'result' does not satisfy the 'matchPattern'.  The
    // pattern-matching is very simple, only one character is considered
    // special: 'X' matches any digit, every other character matches itself.
    // Note that this means that the *length* of 'matchPattern' must match the
    // length of 'result', there is no way (or need) to specify optional
    // characters.
    //..
    // Pattern Examples:
    // -----------------
    //
    // "0.125"    -- matches "0.125", and "0.125" only.
    //
    // "0.12X"    -- matches "0.120" to "0.129", but not "0.12" or "0.1234"
    //..
{

    const bsl::size_t lenRes = result.length();
    const bsl::size_t lenPat = matchPattern.length();
    if (lenRes != lenPat) {
        return false;                                                 // RETURN
    }

    for (bsl::size_t i = 0; i < lenPat; ++i) {
        if (matchPattern[i] == 'X') {
            if (0 == bsl::strchr("0123456789", result[i])) {
                // Not a digit!

                return false;                                         // RETURN
            }
            continue;                                               // CONTINUE
        }
        if (matchPattern[i] != result[i]) {
            return false;                                             // RETURN
        }
    }
    return true;
}

bool verifyFloatingPointResult(const bsl::string_view& result,
                               const bsl::string_view& pattern)
    // Verify the specified floating point print 'result' against the specified
    // 'pattern' and return 'true' if 'result' is considered a match to the
    // 'pattern', or 'false' if 'result' does not satisfy the 'pattern'.  The
    // pattern-matching is very simple, only two characters are considered
    // special: 'X' matches any digit, and a single '|' may be used to specify
    // an alternate pattern considered on Microsoft Visual C++ builds only (see
    // explanation in the code why that is needed).  In case an alternative is
    // specified the first simple pattern (left of the '|') is considered
    // on all platforms, and the second simple-pattern (after the '|') is
    // considered *only* on MSVC.  Every other character matches itself.  Note
    // that the *length* of a simple pattern must match the length of 'result',
    // there is no way (or need) to specify optional characters.  See
    // 'matchFloatingPointResult' above for examples of simple patterns and
    // what they match.
    //..
    // Alternate Pattern Example:
    // --------------------------
    //
    // "0.2|0.3" -- Will match "0.2" on *all* platforms, will *also* match
    //              "0.3" on Microsoft Visual C++
    //..
{

    const bsl::size_t altPos = pattern.find('|', 0);

    if (altPos != bsl::string_view::npos) {
        // We have an alternative value for the broken MSVC 'sprintf' that may
        // round the wrong way, depending on the C library version.  Since that
        // may depend on the machine the executable is run on, we have to
        // accept two alternates run-time, as we don't know compile-time which
        // one it will be.
        return matchFloatingPointResult(result, pattern.substr(0, altPos))
#ifdef BSLS_PLATFORM_CMP_MSVC
            || matchFloatingPointResult(result, pattern.substr(altPos + 1))
#endif
        ;                                                             // RETURN
    }

    return matchFloatingPointResult(result, pattern);
}

// ============================================================================
//                               USAGE EXAMPLE
// ----------------------------------------------------------------------------

// The following snippets of code illustrate the usage of this component.  It
// prints an 'bsl::vector<char>' object using the 'BASE64' formatting mode:
//..
//  #include <balxml_typesprintutil.h>
//
//  #include <cassert>
//  #include <sstream>
//  #include <vector>
//
//  using namespace BloombergLP;
//
void usageExample1()
{
    bsl::vector<char> vec;

    vec.push_back('a');
    vec.push_back('b');
    vec.push_back('c');
    vec.push_back('d');

    bsl::ostringstream ss;

    balxml::TypesPrintUtil::printBase64(ss, vec);

    const char EXPECTED_RESULT[] = "YWJjZA==";

    ASSERT(EXPECTED_RESULT == ss.str());
}
//..
// The following snippet of shows what can be expected when printing valid or
// invalid data via 'printText':
//..
void usageExample2()
{
    bsl::ostringstream ss;

    const char VALID_STR[] = "Hello \t 'World'";
//..
// Note that all characters in the range '0x01' to '0x7F' are valid first bytes
// (including printable ASCII, TAB '0x09', or LF '0x0a', but excluding control
// characters other than TAB and LF) and that ampersand ('&' or '0x26'),
// less-than ('<' or '0x3c'), greater-than ('>' or '0x3e'), apostrophe
// ('0x27'), and quote ('"') will be printed as '&amp;', '&lt;', '&gt',
// '&apos;' and '&quot;' respectively.  Hence the expected output for the above
// string 'VALID_STR' is:
//..
    const char EXPECTED_RESULT[] = "Hello \t &apos;World&apos;";
//..
// We can test that 'printText' will successfully print the string:
//..
    balxml::TypesPrintUtil::printText(ss, VALID_STR);
    ASSERT(ss.good());
    ASSERTV(EXPECTED_RESULT, ss.str(), EXPECTED_RESULT == ss.str());
//..
// In addition, when invalid data is printed, the stream is set to a bad
// state which is the proper means for the user to detect an error, as shown in
// the following code snippet:
//..
    ss.str("");
    const char INVALID_STR[] = "Hello \300\t 'World'";
    balxml::TypesPrintUtil::printText(ss, INVALID_STR);
    const char EXPECTED_INVALID_RESULT[] = "Hello ";
    ASSERT(ss.fail());
    ASSERTV(EXPECTED_INVALID_RESULT, ss.str(),
                 EXPECTED_INVALID_RESULT == ss.str());
}
//..

// ============================================================================
//                               MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int    test = argc > 1 ? bsl::atoi(argv[1]) : 0;
    int verbose = argc > 2;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:  // Zero is always the leading case.
      case 10: {
        // --------------------------------------------------------------------
        // TESTING USAGE EXAMPLE
        //
        // Concerns:
        //
        // Plan:
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTESTING USAGE EXAMPLE"
                          << "\n=====================" << endl;

        usageExample1();

        usageExample2();
      } break;
      case 9: {
        // --------------------------------------------------------------------
        // TESTING PRINTING LISTS OF NULLABLE SIMPLE TYPES
        //   This case tests that printing lists of nullable simple types
        //   results in empty strings for null values, and the expected
        //   representation of non-null values, and correctly space-delimits
        //   individual list elements (that are not empty strings).
        //
        // Concerns:
        //: 1 list-formatted array of nullable, simple types can be printed.
        //:
        //: 2 The textual representation of a nullable simple type that is
        //:   not null is the same as the simple type.
        //:
        //: 3 The textual representation of any null value is the empty string.
        //:
        //: 4 Non-null elements are delimited by 1 space character even if
        //:   separated by one or more null elements.
        //
        // Plan:
        //: 1 Given two simple types, 'int' and 'bsl::string', do the
        //:   following:
        //:
        //:   1 Enumerate arrays of length 0 to 3 and verify the textual
        //:     representation of the array contains space-delimited
        //:     representations of the value.
        //:
        //:   2 Enumerate arrays of nullable elements of length 0 to 3,
        //:     with all possible permutations of null and non-null values,
        //:     and verify the textual representation of the array contains
        //:     space-delimited representations of the value.
        //
        // Testing:
        //   PRINTING LISTS OF NULLABLE SIMPLE TYPES
        // --------------------------------------------------------------------

        if (verbose)
            cout << "\nTESTING PRINTING LISTS OF NULLABLE SIMPLE TYPES"
                 << "\n==============================================="
                 << endl;

        const AssertPrintedTextIsEqual TEST;

        const s_baltst::TestPlaceHolder<int>            ip;
        const s_baltst::TestPlaceHolder<bsl::string>    sp;
        const s_baltst::GenerateTestArray               a_;
        const s_baltst::GenerateTestNullableValue       n_;
        const s_baltst::GenerateTestNullablePlaceHolder np;

        const bsl::string s("S");

        const int LIST = bdlat_FormattingMode::e_LIST;

        //                   FORMATTING MODE
        //                   ---------------.
        // LINE           ARRAY              \.   EXPECTED OUTPUT
        //   -- --------------------------- ----- ---------------
        TEST(L_, a_(ip                    ), LIST, ""            );
        TEST(L_, a_( 1                    ), LIST, "1"           );
        TEST(L_, a_( 1, 1                 ), LIST, "1 1"         );
        TEST(L_, a_( 1, 1, 1              ), LIST, "1 1 1"       );

        TEST(L_, a_(np(ip)                ), LIST, ""            );
        TEST(L_, a_(n_(ip)                ), LIST, ""            );
        TEST(L_, a_(n_( 1)                ), LIST, "1"           );

        TEST(L_, a_(n_(ip), n_(ip)        ), LIST, ""            );
        TEST(L_, a_(n_(ip), n_( 1)        ), LIST, "1"           );
        TEST(L_, a_(n_( 1), n_(ip)        ), LIST, "1"           );
        TEST(L_, a_(n_( 1), n_( 1)        ), LIST, "1 1"         );

        TEST(L_, a_(n_(ip), n_(ip), n_(ip)), LIST, ""            );
        TEST(L_, a_(n_(ip), n_(ip), n_( 1)), LIST, "1"           );
        TEST(L_, a_(n_(ip), n_( 1), n_(ip)), LIST, "1"           );
        TEST(L_, a_(n_(ip), n_( 1), n_( 1)), LIST, "1 1"         );
        TEST(L_, a_(n_( 1), n_(ip), n_(ip)), LIST, "1"           );
        TEST(L_, a_(n_( 1), n_(ip), n_( 1)), LIST, "1 1"         );
        TEST(L_, a_(n_( 1), n_( 1), n_(ip)), LIST, "1 1"         );
        TEST(L_, a_(n_( 1), n_( 1), n_( 1)), LIST, "1 1 1"       );

        TEST(L_, a_(sp                    ), LIST, ""            );
        TEST(L_, a_( s                    ), LIST, "S"           );
        TEST(L_, a_( s, s                 ), LIST, "S S"         );
        TEST(L_, a_( s, s, s              ), LIST, "S S S"       );

        TEST(L_, a_(np(sp)                ), LIST, ""            );
        TEST(L_, a_(n_(sp)                ), LIST, ""            );
        TEST(L_, a_(n_( s)                ), LIST, "S"           );

        TEST(L_, a_(n_(sp), n_(sp)        ), LIST, ""            );
        TEST(L_, a_(n_(sp), n_( s)        ), LIST, "S"           );
        TEST(L_, a_(n_( s), n_(sp)        ), LIST, "S"           );
        TEST(L_, a_(n_( s), n_( s)        ), LIST, "S S"         );

        TEST(L_, a_(n_(sp), n_(sp), n_(sp)), LIST, ""            );
        TEST(L_, a_(n_(sp), n_(sp), n_( s)), LIST, "S"           );
        TEST(L_, a_(n_(sp), n_( s), n_(sp)), LIST, "S"           );
        TEST(L_, a_(n_(sp), n_( s), n_( s)), LIST, "S S"         );
        TEST(L_, a_(n_( s), n_(sp), n_(sp)), LIST, "S"           );
        TEST(L_, a_(n_( s), n_(sp), n_( s)), LIST, "S S"         );
        TEST(L_, a_(n_( s), n_( s), n_(sp)), LIST, "S S"         );
        TEST(L_, a_(n_( s), n_( s), n_( s)), LIST, "S S S"       );
      } break;
      case 8: {
        // --------------------------------------------------------------------
        // TESTING 'print' FUNCTION
        //
        // Concerns:
        //
        // Plan:
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTESTING 'print' FUNCTION"
                          << "\n========================" << endl;

        if (verbose) cout << "\nUsing 'BASE64'." << endl;
        {
            typedef bsl::vector<char> Type;

            static const struct {
                int         d_lineNum;
                const char *d_input;
                const char *d_result;
            } DATA[] = {
                //line    input       result
                //----    -----       ------
                { L_,     "",         ""              },
                { L_,     "a",        "YQ=="          },
                { L_,     "ab",       "YWI="          },
                { L_,     "abc",      "YWJj"          },
                { L_,     "abcd",     "YWJjZA=="      },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int          LINE          = DATA[i].d_lineNum;
                const char        *INPUT         = DATA[i].d_input;
                const char        *RESULT        = DATA[i].d_result;
                const bsl::size_t  INPUT_LENGTH  = bsl::strlen(INPUT);

                Type mX(INPUT, INPUT + INPUT_LENGTH); const Type& X = mX;
                bsl::stringstream ss;

                Util::print(ss, X, bdlat_FormattingMode::e_BASE64);

                ASSERTV(LINE, ss.str(), RESULT == ss.str());
            }
        }

        if (verbose) cout << "\nUsing 'DEC'." << endl;
        {
            typedef bool Type;

            static const struct {
                int         d_lineNum;
                Type        d_input;
                const char *d_result;
            } DATA[] = {
                //line    input       result
                //----    -----       ------
                { L_,     true,       "1"             },
                { L_,     false,      "0"             },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int   LINE   = DATA[i].d_lineNum;
                const Type  INPUT  = DATA[i].d_input;
                const char *RESULT = DATA[i].d_result;

                bsl::stringstream ss;

                Util::print(ss, INPUT, bdlat_FormattingMode::e_DEC);

                ASSERTV(LINE, ss.str(), RESULT == ss.str());
            }
        }

        if (verbose) cout << "\nUsing 'DEFAULT' (bool->TEXT)." << endl;
        {
            typedef bool Type;

            static const struct {
                int         d_lineNum;
                Type        d_input;
                const char *d_result;
            } DATA[] = {
                //line    input       result
                //----    -----       ------
                { L_,     true,       "true"             },
                { L_,     false,      "false"            },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int   LINE   = DATA[i].d_lineNum;
                const Type  INPUT  = DATA[i].d_input;
                const char *RESULT = DATA[i].d_result;

                bsl::stringstream ss;

                Util::print(ss, INPUT, bdlat_FormattingMode::e_DEFAULT);

                ASSERTV(LINE, ss.str(), RESULT == ss.str());
            }
        }

        if (verbose) cout << "\nUsing 'DEFAULT' (char->DEC)." << endl;
        {
            typedef char Type;

            static const struct {
                int         d_lineNum;
                Type        d_input;
                const char *d_result;
            } DATA[] = {
                //line    input       result
                //----    -----       ------
                { L_,     -128,       "-128"          },
                { L_,     -127,       "-127"          },
                { L_,     -1,         "-1"            },
                { L_,     0,          "0"             },
                { L_,     1,          "1"             },
                { L_,     126,        "126"           },
                { L_,     127,        "127"           },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int   LINE   = DATA[i].d_lineNum;
                const Type  INPUT  = DATA[i].d_input;
                const char *RESULT = DATA[i].d_result;

                bsl::stringstream ss;

                Util::print(ss, INPUT, bdlat_FormattingMode::e_DEFAULT);

                ASSERTV(LINE, ss.str(), RESULT == ss.str());
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
                //line    input       result
                //----    -----       ------
                { L_,     "",         ""              },
                { L_,     "a",        "61"            },
                { L_,     "ab",       "6162"          },
                { L_,     "abc",      "616263"        },
                { L_,     "abcd",     "61626364"      },
                { L_,     "\x20\x01\x02\x03\x04\x05\x06\x07"
                          "\x08\x09\x0A\x0B\x0C\x0D\x0E\x0F",
                          "200102030405060708090A0B0C0D0E0F"      },
                { L_,     "\xF0\xF1\xF2\xF3\xF4\xF5\xF6\xF7"
                          "\xF8\xF9\xFA\xFB\xFC\xFD\xFE\xFF",
                          "F0F1F2F3F4F5F6F7F8F9FAFBFCFDFEFF"      },
                { L_,     "\x01\x23\x45\x67\x89\xAB\xCD\xEF",
                          "0123456789ABCDEF"                      },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int          LINE          = DATA[i].d_lineNum;
                const char        *INPUT         = DATA[i].d_input;
                const char        *RESULT        = DATA[i].d_result;
                const bsl::size_t  INPUT_LENGTH  = bsl::strlen(INPUT);

                Type mX(INPUT, INPUT + INPUT_LENGTH); const Type& X = mX;
                bsl::stringstream ss;

                Util::print(ss, X, bdlat_FormattingMode::e_HEX);

                ASSERTV(LINE, ss.str(), RESULT == ss.str());
            }
        }

        if (verbose) cout << "\nUsing 'IS_LIST'." << endl;
        {
            typedef int                   ElemType;
            typedef bsl::vector<ElemType> Type;

            static const struct {
                int         d_lineNum;
                ElemType    d_input[5];
                int         d_numInput;
                const char *d_result;
            } DATA[] = {
                //line    input                numInput  result
                //----    -----                --------  ------
                { L_,     { },                 0,        ""             },
                { L_,     { 1 },               1,        "1"            },
                { L_,     { 1, 4 },            2,        "1 4"          },
                { L_,     { 1, 4, 2 },         3,        "1 4 2"        },
                { L_,     { 1, 4, 2, 8 },      4,        "1 4 2 8"      },
                { L_,     { 1, 4, 2, 8, 23 },  5,        "1 4 2 8 23"   },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int       LINE      = DATA[i].d_lineNum;
                const ElemType *INPUT     = DATA[i].d_input;
                const int       NUM_INPUT = DATA[i].d_numInput;
                const char     *RESULT    = DATA[i].d_result;

                bsl::stringstream ss;

                Type mX(INPUT, INPUT + NUM_INPUT);  const Type& X = mX;

                Util::print(ss, X, bdlat_FormattingMode::e_LIST);

                ASSERTV(LINE, ss.str(), RESULT == ss.str());

                ss.str("");

                Util::print(ss, X, bdlat_FormattingMode::e_LIST |
                                   bdlat_FormattingMode::e_DEC);

                ASSERTV(LINE, ss.str(), RESULT == ss.str());
            }
        }

        if (verbose) cout << "\nUsing 'TEXT'." << endl;
        {
            typedef bool Type;

            static const struct {
                int         d_lineNum;
                Type        d_input;
                const char *d_result;
            } DATA[] = {
                //line    input       result
                //----    -----       ------
                { L_,     true,       "true"          },
                { L_,     false,      "false"         },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int   LINE   = DATA[i].d_lineNum;
                const Type  INPUT  = DATA[i].d_input;
                const char *RESULT = DATA[i].d_result;

                bsl::stringstream ss;

                Util::print(ss, INPUT, bdlat_FormattingMode::e_TEXT);

                ASSERTV(LINE, ss.str(), RESULT == ss.str());
            }
        }
      } break;
      case 7: {
        // --------------------------------------------------------------------
        // TESTING 'printDefault' FUNCTIONS
        //
        // Concerns:
        //
        // Plan:
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTESTING 'printDefault' FUNCTIONS"
                          << "\n================================" << endl;

        if (verbose) cout << "\nUsing 'bool'." << endl;
        {
            typedef bool Type;

            static const struct {
                int         d_lineNum;
                Type        d_input;
                const char *d_result;
            } DATA[] = {
                //line    input       result
                //----    -----       ------
                { L_,     true,       "true"             },
                { L_,     false,      "false"            },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int   LINE   = DATA[i].d_lineNum;
                const Type  INPUT  = DATA[i].d_input;
                const char *RESULT = DATA[i].d_result;

                bsl::stringstream ss;

                Util::printDefault(ss, INPUT);

                ASSERTV(LINE, ss.str(), RESULT == ss.str());
            }
        }

        if (verbose) cout << "\nUsing 'char'." << endl;
        {
            typedef char Type;

            static const struct {
                int         d_lineNum;
                Type        d_input;
                const char *d_result;
            } DATA[] = {
                //line    input       result
                //----    -----       ------
                { L_,     -128,       "-128"          },
                { L_,     -127,       "-127"          },
                { L_,     -1,         "-1"            },
                { L_,     0,          "0"             },
                { L_,     1,          "1"             },
                { L_,     126,        "126"           },
                { L_,     127,        "127"           },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int   LINE   = DATA[i].d_lineNum;
                const Type  INPUT  = DATA[i].d_input;
                const char *RESULT = DATA[i].d_result;

                bsl::stringstream ss;

                Util::printDefault(ss, INPUT);

                ASSERTV(LINE, ss.str(), RESULT == ss.str());
            }
        }

        if (verbose) cout << "\nUsing 'short'." << endl;
        {
            typedef short Type;

            static const struct {
                int         d_lineNum;
                Type        d_input;
                const char *d_result;
            } DATA[] = {
                //line    input       result
                //----    -----       ------
                { L_,     -32768,     "-32768"        },
                { L_,     -32767,     "-32767"        },
                { L_,     -1,         "-1"            },
                { L_,     0,          "0"             },
                { L_,     1,          "1"             },
                { L_,     32766,      "32766"         },
                { L_,     32767,      "32767"         },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int   LINE   = DATA[i].d_lineNum;
                const Type  INPUT  = DATA[i].d_input;
                const char *RESULT = DATA[i].d_result;

                bsl::stringstream ss;

                Util::printDefault(ss, INPUT);

                ASSERTV(LINE, ss.str(), RESULT == ss.str());
            }
        }

        if (verbose) cout << "\nUsing 'int'." << endl;
        {
            typedef int Type;

            static const struct {
                int         d_lineNum;
                Type        d_input;
                const char *d_result;
            } DATA[] = {
                //line    input            result
                //----    -----            ------
                { L_,     -2147483647-1,   "-2147483648"   },
                { L_,     -2147483647,     "-2147483647"   },
                { L_,     -1,              "-1"            },
                { L_,     0,               "0"             },
                { L_,     1,               "1"             },
                { L_,     2147483646,      "2147483646"    },
                { L_,     2147483647,      "2147483647"    },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int   LINE   = DATA[i].d_lineNum;
                const Type  INPUT  = DATA[i].d_input;
                const char *RESULT = DATA[i].d_result;

                bsl::stringstream ss;

                Util::printDefault(ss, INPUT);

                ASSERTV(LINE, ss.str(), RESULT == ss.str());
            }
        }

        if (verbose) cout << "\nUsing 'long'." << endl;
        {
            typedef long Type;

            static const struct {
                int         d_lineNum;
                Type        d_input;
                const char *d_result;
            } DATA[] = {
                //line  input                       result
                //----  --------------------------  -------------------------
                { L_,   -2147483647-1,              "-2147483648"            },
                { L_,   -2147483647,                "-2147483647"            },
                { L_,   -1,                         "-1"                     },
                { L_,   0,                          "0"                      },
                { L_,   1,                          "1"                      },
                { L_,   2147483646,                 "2147483646"             },
                { L_,   2147483647,                 "2147483647"             },
#ifdef U_LONG_IS_64_BITS
                { L_,   -9223372036854775807LL - 1, "-9223372036854775808"   },
                { L_,   -9223372036854775807LL,     "-9223372036854775807"   },
                { L_,   9223372036854775806LL,      "9223372036854775806"    },
                { L_,   9223372036854775807LL,      "9223372036854775807"    },
#endif
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int   LINE   = DATA[i].d_lineNum;
                const Type  INPUT  = DATA[i].d_input;
                const char *RESULT = DATA[i].d_result;

                bsl::stringstream ss;

                Util::printDefault(ss, INPUT);

                ASSERTV(LINE, ss.str(), RESULT == ss.str());
            }
        }

        if (verbose) cout << "\nUsing 'bsls::Types::Int64'." << endl;
        {
            typedef bsls::Types::Int64 Type;

            static const struct {
                int         d_lineNum;
                Type        d_input;
                const char *d_result;
            } DATA[] = {
                //line  input                       result
                //----  -----                       -----------------------
                { L_,   -9223372036854775807LL - 1, "-9223372036854775808" },
                { L_,   -9223372036854775807LL,     "-9223372036854775807" },
                { L_,   -1LL,                       "-1"                   },
                { L_,    0LL,                        "0"                   },
                { L_,    1LL,                        "1"                   },
                { L_,    9223372036854775806LL,      "9223372036854775806" },
                { L_,    9223372036854775807LL,      "9223372036854775807" },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int   LINE   = DATA[i].d_lineNum;
                const Type  INPUT  = DATA[i].d_input;
                const char *RESULT = DATA[i].d_result;

                bsl::stringstream ss;

                Util::printDefault(ss, INPUT);

                ASSERTV(LINE, ss.str(), RESULT == ss.str());
            }
        }

        if (verbose) cout << "\nUsing 'unsigned char'." << endl;
        {
            typedef unsigned char Type;

            static const struct {
                int         d_lineNum;
                Type        d_input;
                const char *d_result;
            } DATA[] = {
                //line    input       result
                //----    -----       ------
                { L_,     0,          "0"             },
                { L_,     1,          "1"             },
                { L_,     254,        "254"           },
                { L_,     255,        "255"           },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int   LINE   = DATA[i].d_lineNum;
                const Type  INPUT  = DATA[i].d_input;
                const char *RESULT = DATA[i].d_result;

                bsl::stringstream ss;

                Util::printDefault(ss, INPUT);

                ASSERTV(LINE, ss.str(), RESULT == ss.str());
            }
        }

        if (verbose) cout << "\nUsing 'unsigned short'." << endl;
        {
            typedef unsigned short Type;

            static const struct {
                int         d_lineNum;
                Type        d_input;
                const char *d_result;
            } DATA[] = {
                //line    input       result
                //----    -----       ------
                { L_,     0,          "0"             },
                { L_,     1,          "1"             },
                { L_,     65534,      "65534"         },
                { L_,     65535,      "65535"         },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int   LINE   = DATA[i].d_lineNum;
                const Type  INPUT  = DATA[i].d_input;
                const char *RESULT = DATA[i].d_result;

                bsl::stringstream ss;

                Util::printDefault(ss, INPUT);

                ASSERTV(LINE, ss.str(), RESULT == ss.str());
            }
        }

        if (verbose) cout << "\nUsing 'unsigned int'." << endl;
        {
            typedef unsigned int Type;

            static const struct {
                int         d_lineNum;
                Type        d_input;
                const char *d_result;
            } DATA[] = {
                //line    input            result
                //----    -----            ------
                { L_,     0,               "0"             },
                { L_,     1,               "1"             },
                { L_,     4294967294U,     "4294967294"    },
                { L_,     4294967295U,     "4294967295"    },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int   LINE   = DATA[i].d_lineNum;
                const Type  INPUT  = DATA[i].d_input;
                const char *RESULT = DATA[i].d_result;

                bsl::stringstream ss;

                Util::printDefault(ss, INPUT);

                ASSERTV(LINE, ss.str(), RESULT == ss.str());
            }
        }

        if (verbose) cout << "\nUsing 'bsls::Types::Uint64'." << endl;
        {
            typedef bsls::Types::Uint64 Type;

            static const struct {
                int         d_lineNum;
                Type        d_input;
                const char *d_result;
            } DATA[] = {
                //line    input                     result
                //----    -----                     ------
                { L_,     0ULL,                     "0"                      },
                { L_,     1ULL,                     "1"                      },
                { L_,     18446744073709551614ULL,  "18446744073709551614"   },
                { L_,     18446744073709551615ULL,  "18446744073709551615"   },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int   LINE   = DATA[i].d_lineNum;
                const Type  INPUT  = DATA[i].d_input;
                const char *RESULT = DATA[i].d_result;

                bsl::stringstream ss;

                Util::printDefault(ss, INPUT);

                ASSERTV(LINE, ss.str(), RESULT == ss.str());
            }
        }

        if (verbose) cout << "\nUsing 'float'." << endl;
        {
            typedef float Type;

            typedef bsl::numeric_limits<Type> NumLimits;

            const float neg0 = copysignf(0.0f, -1.0f);

            static const struct {
                int         d_lineNum;
                Type        d_input;
                const char *d_inputStr;
                const char *d_result;
            } DATA[] = {
#define F(input, result) { L_, input, #input, result }
                //    input                  result
                //--------------    -------------------
                F( neg0,            "-0"               ),
                F(-1.0f,            "-1"               ),
                F(-0.1f,            "-0.1"             ),
                F(-1234567.f,       "-1234567"         ),
                F(-0.1234567f,      "-0.1234567"       ),
                F(-1.234567e35f,    "-1.234567e+35"    ),
                F(-1.234567e-35f,   "-1.234567e-35"    ),

                F( 0.0f,             "0"               ),
                F( 1.0f,             "1"               ),
                F( 0.1f,             "0.1"             ),
                F( 1234567.f,        "1234567"         ),
                F( 0.1234567f,       "0.1234567"       ),
                F( 1.234567e35f,     "1.234567e+35"    ),
                F( 1.234567e-35f,    "1.234567e-35"    ),

                // Values from 'balxml_encoder.t.cpp' 'runTestCase17'
                F(1.1920928e-07f        , "1.1920928e-07" ),
                F(2.3841856e-07f        , "2.3841856e-07" ),
                F(1.5258789e-05f        , "1.5258789e-05" ),
                F(2.4414062e-04f        , "0.00024414062" ),
                F(3.90625e-03f          , "0.00390625"    ),
                F(6.25e-02f             , "0.0625"        ),
                F(5e-1f                 , "0.5"           ),
                F(                  1.0f,             "1" ),
                F(               1024.0f,          "1024" ),
                F(           16777216.0f,      "16777216" ),
                F(       137438953472.0f,  "137438953472" ),
                F(   1125899906842624.0f, "1.1258999e+15" ),
                F(  18014398509481984.0f, "1.8014399e+16" ),

                // {DRQS 165162213} regression, 2^24 loses precision as float
                F(1.0f * 0xFFFFFF, "16777215"),

                // Full Mantissa Integers
                F(1.0f * 0xFFFFFF      // this is also
                       * (1ull << 63)  // 'NumLimits::max()'
                       * (1ull << 41),  "3.4028235e+38"),
                // Boundary Values
                F( NumLimits::min(),    "1.1754944e-38"),
                F( NumLimits::max(),    "3.4028235e+38"),
                F(-NumLimits::min(),   "-1.1754944e-38"),
                F(-NumLimits::max(),   "-3.4028235e+38"),

                // Non-numeric Values
                F( NumLimits::infinity(),      "+INF"),
                F(-NumLimits::infinity(),      "-INF"),
                F( NumLimits::signaling_NaN(),  "NaN"),
                F( NumLimits::quiet_NaN(),      "NaN"),
#undef F
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            const int DO_NOT_SET = -42;
            static int TOTAL_DIGITS_DATA[] = { DO_NOT_SET, 6, 12 };
            const int NUM_TOTAL_DIGITS =
                          sizeof TOTAL_DIGITS_DATA / sizeof *TOTAL_DIGITS_DATA;

            static int FRACTION_DIGITS_DATA[] = { DO_NOT_SET, 0, 1, 8 };
            const int NUM_FRACTION_DIGITS =
                                sizeof FRACTION_DIGITS_DATA
                                                / sizeof *FRACTION_DIGITS_DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int   LINE      = DATA[i].d_lineNum;
                const Type  INPUT     = DATA[i].d_input;
                const char *INPUT_STR = DATA[i].d_inputStr;
                const char *EXPECTED  = DATA[i].d_result;

                bsl::stringstream ss;

                Util::printDefault(ss, INPUT);

                ASSERTV(LINE, INPUT_STR, ss.str(), EXPECTED,
                        verifyFloatingPointResult(ss.str(), EXPECTED));

                for (int j = 0; j < NUM_TOTAL_DIGITS; ++j) {
                    for (int k = 0; k < NUM_FRACTION_DIGITS; ++k) {
                        const int    TOTAL_DIGITS =    TOTAL_DIGITS_DATA[j];
                        const int FRACTION_DIGITS = FRACTION_DIGITS_DATA[k];

                        // Verify that decimal format options are *ignored* by
                        // the default format printing, passing encoding
                        // options should not change the resulting string.
                        balxml::EncoderOptions options;
                        if (TOTAL_DIGITS != DO_NOT_SET) {
                            options.setMaxDecimalTotalDigits(TOTAL_DIGITS);
                        }
                        if (FRACTION_DIGITS != DO_NOT_SET) {
                            options.setMaxDecimalFractionDigits(
                                                              FRACTION_DIGITS);
                        }

                        bsl::stringstream optss;

                        Util::printDefault(optss, INPUT, &options);

                        ASSERTV(LINE, INPUT_STR, ss.str(), options,
                                ss.str() == optss.str());

                        ASSERTV(LINE, INPUT_STR, ss.str(), EXPECTED,
                                verifyFloatingPointResult(optss.str(),
                                                          EXPECTED));
                    }
                }
            }
        }

        if (verbose) cout << "\nUsing 'double'." << endl;
        {
            typedef double Type;

            typedef bsl::numeric_limits<Type> NumLimits;

            const double neg0 = copysign(0.0, -1.0);

            static const struct {
                int         d_lineNum;
                Type        d_input;
                const char* d_inputStr;
                const char* d_result;
            } DATA[] = {
#define D(input, result) { L_, input, #input, result }
                //        input               result
                //--------------------   ------------------------------------
                D( neg0,                             "-0"),
                D(-1.0,                              "-1"                    ),
                D(-0.1,                              "-0.1"                  ),
                D(-0.123456789012345,                "-0.123456789012345"    ),
                D(-1.23456789012345e+105,            "-1.23456789012345e+105"),
                D(-1.23456789012345e-105,            "-1.23456789012345e-105"),

                D( 0.0,                               "0"                    ),
                D( 0.1,                               "0.1"                  ),
                D( 1.0,                               "1"                    ),
                D( 1.23456789012345e105,              "1.23456789012345e+105"),
                D( 123.4567,                        "123.4567"               ),
                D(1234567890123456.,   "1234567890123456"                    ),

                // Values from 'balxml_encoder.t.cpp' 'runTestCase17'
                D(1.1920928955078125e-07, "1.1920928955078125e-07"           ),
                D(2.384185791015625e-07 , "2.384185791015625e-07"            ),
                D(1.52587890625e-05     , "1.52587890625e-05"                ),
                D(2.44140625e-04        , "0.000244140625"                   ),
                D(3.90625e-03           , "0.00390625"                       ),
                D(6.25e-02              , "0.0625"                           ),
                D(5e-1                  , "0.5"                              ),
                D(                   1.0,                 "1"                ),
                D(                1024.0,              "1024"                ),
                D(            16777216.0,          "16777216"                ),
                D(        137438953472.0,      "137438953472"                ),
                D(    1125899906842624.0,  "1125899906842624"                ),
                D(   18014398509481984.0, "18014398509481984"                ),

                // Small Integers
                D(123456789012345.,     "123456789012345"                    ),
                D(1234567890123456.,   "1234567890123456"                    ),

                // Full Mantissa Integers
                D(1.0 * 0x1FFFFFFFFFFFFFull, "9007199254740991"),
                D(1.0 * 0x1FFFFFFFFFFFFFull  // This is also 'NumLimits::max()'
                      * (1ull << 63) * (1ull << 63) * (1ull << 63)
                      * (1ull << 63) * (1ull << 63) * (1ull << 63)
                      * (1ull << 63) * (1ull << 63) * (1ull << 63)
                      * (1ull << 63) * (1ull << 63) * (1ull << 63)
                      * (1ull << 63) * (1ull << 63) * (1ull << 63)
                      * (1ull << 26),               "1.7976931348623157e+308"),

                // Boundary Values
                D( NumLimits::min(),                "2.2250738585072014e-308"),
                D( NumLimits::max(),                "1.7976931348623157e+308"),
                D(-NumLimits::min(),               "-2.2250738585072014e-308"),
                D(-NumLimits::max(),               "-1.7976931348623157e+308"),

                // Non-numeric Values
                D( NumLimits::infinity(),      "+INF"),
                D(-NumLimits::infinity(),      "-INF"),
                D( NumLimits::signaling_NaN(),  "NaN"),
                D( NumLimits::quiet_NaN(),      "NaN"),
#undef D
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            const int DO_NOT_SET = -42;
            static int TOTAL_DIGITS_DATA[] = { DO_NOT_SET, 6, 12 };
            const int NUM_TOTAL_DIGITS =
                          sizeof TOTAL_DIGITS_DATA / sizeof *TOTAL_DIGITS_DATA;

            static int FRACTION_DIGITS_DATA[] = { DO_NOT_SET, 0, 1, 8 };
            const int NUM_FRACTION_DIGITS =
                                sizeof FRACTION_DIGITS_DATA
                                                / sizeof *FRACTION_DIGITS_DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int   LINE      = DATA[i].d_lineNum;
                const Type  INPUT     = DATA[i].d_input;
                const char *INPUT_STR = DATA[i].d_inputStr;
                const char *EXPECTED  = DATA[i].d_result;

                bsl::stringstream ss;

                Util::printDefault(ss, INPUT);

                ASSERTV(LINE, INPUT_STR, ss.str(), EXPECTED,
                        verifyFloatingPointResult(ss.str(), EXPECTED));

                for (int j = 0; j < NUM_TOTAL_DIGITS; ++j) {
                    for (int k = 0; k < NUM_FRACTION_DIGITS; ++k) {
                        const int    TOTAL_DIGITS =    TOTAL_DIGITS_DATA[j];
                        const int FRACTION_DIGITS = FRACTION_DIGITS_DATA[k];

                        // Verify that decimal format options are *ignored* by
                        // the default format printing, passing encoding
                        // options should not change the resulting string.
                        balxml::EncoderOptions options;
                        if (TOTAL_DIGITS != DO_NOT_SET) {
                            options.setMaxDecimalTotalDigits(TOTAL_DIGITS);
                        }
                        if (FRACTION_DIGITS != DO_NOT_SET) {
                            options.setMaxDecimalFractionDigits(
                                                              FRACTION_DIGITS);
                        }

                        bsl::stringstream optss;

                        Util::printDefault(optss, INPUT, &options);

                        ASSERTV(LINE, INPUT_STR, ss.str(), options,
                                ss.str() == optss.str());

                        ASSERTV(LINE, INPUT_STR, ss.str(), EXPECTED,
                                verifyFloatingPointResult(optss.str(),
                                                          EXPECTED));
                    }
                }
            }
        }

        if (verbose) cout << "\nUsing 'Decimal64'." << endl;
        {
            typedef bdldfp::Decimal64         Type;
            typedef bsl::numeric_limits<Type> Limits;

#define DFP(X) BDLDFP_DECIMAL_DD(X)

            const struct Data {
                int         d_lineNum;
                Type        d_input;
                char        d_style;
                int         d_precision;
                const char *d_result;
                bool        d_weird;
            } DATA[] = {
//---------------------------------------------------------------------------
// LN  VALUE                        STYLE     RESULT
//                                        PRS                            WEIRD
//---------------------------------------------------------------------------
{ L_,  DFP(0.0),                    'N',  0,  "0.0",                     0 },
{ L_,  DFP(15.13),                  'N',  0,  "15.13",                   0 },
{ L_,  DFP(-9.876543210987654e307), 'N',  0,  "-9.876543210987654e+307", 0 },
{ L_,  DFP(0.001),                  'N',  0,  "0.001",                   0 },
{ L_,  DFP(0.01),                   'N',  0,  "0.01",                    0 },
{ L_,  DFP(0.1),                    'N',  0,  "0.1",                     0 },
{ L_,  DFP(1.),                     'N',  0,  "1",                       0 },
{ L_,  DFP(1.0),                    'N',  0,  "1.0",                     0 },
{ L_,  DFP(1.00),                   'N',  0,  "1.00",                    0 },
{ L_,  DFP(1.000),                  'N',  0,  "1.000",                   0 },
{ L_,  Limits::max(),               'N',  0,  "9.999999999999999e+384",  0 },
{ L_,  -Limits::max(),              'N',  0,  "-9.999999999999999e+384", 0 },
{ L_,  Limits::min(),               'N',  0,  "1e-383",                  0 },
{ L_,  -Limits::min(),              'N',  0,  "-1e-383",                 0 },
{ L_,  Limits::infinity(),          'N',  0,   "INF",                    1 },
{ L_, -Limits::infinity(),          'N',  0,  "-INF",                    1 },
{ L_,  Limits::signaling_NaN(),     'N',  0,   "NaN",                    1 },
{ L_,  Limits::quiet_NaN(),         'N',  0,   "NaN",                    1 },

{ L_,  DFP(0.0),                    'F',  2,  "0.00",                    0 },
{ L_,  DFP(15.13),                  'F',  2,  "15.13",                   0 },
{ L_,  DFP(-9876543210987654.0),    'F',  0,  "-9876543210987654",       0 },
{ L_,  DFP(0.001),                  'F',  0,  "0",                       0 },
{ L_,  DFP(0.001),                  'F',  1,  "0.0",                     0 },
{ L_,  DFP(0.001),                  'F',  2,  "0.00",                    0 },
{ L_,  DFP(0.001),                  'F',  3,  "0.001",                   0 },
{ L_,  DFP(0.001),                  'F',  4,  "0.0010",                  0 },
{ L_,  DFP(0.01),                   'F',  0,  "0",                       0 },
{ L_,  DFP(0.01),                   'F',  1,  "0.0",                     0 },
{ L_,  DFP(0.01),                   'F',  2,  "0.01",                    0 },
{ L_,  DFP(0.01),                   'F',  3,  "0.010",                   0 },
{ L_,  DFP(0.1),                    'F',  0,  "0",                       0 },
{ L_,  DFP(0.1),                    'F',  1,  "0.1",                     0 },
{ L_,  DFP(0.1),                    'F',  2,  "0.10",                    0 },
{ L_,  DFP(1.),                     'F',  0,  "1",                       0 },
{ L_,  DFP(1.),                     'F',  0,  "1",                       0 },
{ L_,  DFP(1.),                     'F',  1,  "1.0",                     0 },
{ L_,  DFP(1.0),                    'F',  0,  "1",                       0 },
{ L_,  DFP(1.0),                    'F',  1,  "1.0",                     0 },
{ L_,  DFP(1.0),                    'F',  2,  "1.00",                    0 },
{ L_,  DFP(1.00),                   'F',  0,  "1",                       0 },
{ L_,  DFP(1.00),                   'F',  1,  "1.0",                     0 },
{ L_,  DFP(1.00),                   'F',  2,  "1.00",                    0 },
{ L_,  DFP(1.00),                   'F',  3,  "1.000",                   0 },
{ L_,  DFP(1.000),                  'F',  0,  "1",                       0 },
{ L_,  DFP(1.000),                  'F',  1,  "1.0",                     0 },
{ L_,  DFP(1.000),                  'F',  2,  "1.00",                    0 },
{ L_,  DFP(1.000),                  'F',  3,  "1.000",                   0 },
{ L_,  DFP(1.000),                  'F',  4,  "1.0000",                  0 },
{ L_,  Limits::min(),               'F',  0,  "0",                       0 },
{ L_, -Limits::min(),               'F',  0,  "-0",                      0 },
{ L_,  Limits::infinity(),          'F',  0,   "INF",                    1 },
{ L_, -Limits::infinity(),          'F',  0,  "-INF",                    1 },
{ L_,  Limits::signaling_NaN(),     'F',  0,   "NaN",                    1 },
{ L_,  Limits::quiet_NaN(),         'F',  0,   "NaN",                    1 },

{ L_,  DFP(0.1),                    'S',  0,  "1e-01",                   0 },
{ L_,  DFP(15.13),                  'S',  3,  "1.513e+01",               0 },
{ L_,  DFP(-9.876543210987654e307), 'S', 11,  "-9.87654321099e+307",     0 },
{ L_,  DFP(0.001),                  'S',  0,  "1e-03",                   0 },
{ L_,  DFP(0.001),                  'S',  1,  "1.0e-03",                 0 },
{ L_,  DFP(0.001),                  'S',  2,  "1.00e-03",                0 },
{ L_,  DFP(0.01),                   'S',  0,  "1e-02",                   0 },
{ L_,  DFP(0.01),                   'S',  1,  "1.0e-02",                 0 },
{ L_,  DFP(0.01),                   'S',  2,  "1.00e-02",                0 },
{ L_,  DFP(0.1),                    'S',  0,  "1e-01",                   0 },
{ L_,  DFP(0.1),                    'S',  1,  "1.0e-01",                 0 },
{ L_,  DFP(0.1),                    'S',  2,  "1.00e-01",                0 },
{ L_,  DFP(1.),                     'S',  0,  "1e+00",                   0 },
{ L_,  DFP(1.),                     'S',  1,  "1.0e+00",                 0 },
{ L_,  DFP(1.),                     'S',  2,  "1.00e+00",                0 },
{ L_,  DFP(1.0),                    'S',  0,  "1e+00",                   0 },
{ L_,  DFP(1.0),                    'S',  1,  "1.0e+00",                 0 },
{ L_,  DFP(1.0),                    'S',  2,  "1.00e+00",                0 },
{ L_,  DFP(1.00),                   'S',  0,  "1e+00",                   0 },
{ L_,  DFP(1.00),                   'S',  1,  "1.0e+00",                 0 },
{ L_,  DFP(1.00),                   'S',  2,  "1.00e+00",                0 },
{ L_,  DFP(1.000),                  'S',  0,  "1e+00",                   0 },
{ L_,  DFP(1.000),                  'S',  1,  "1.0e+00",                 0 },
{ L_,  DFP(1.000),                  'S',  2,  "1.00e+00",                0 },
{ L_,  Limits::max(),               'S',  0,  "1e+385",                  0 },
{ L_, -Limits::max(),               'S',  0,  "-1e+385",                 0 },
{ L_,  Limits::min(),               'S',  0,  "1e-383",                  0 },
{ L_, -Limits::min(),               'S',  0,  "-1e-383",                 0 },
{ L_,  Limits::infinity(),          'S',  0,   "INF",                    1 },
{ L_, -Limits::infinity(),          'S',  0,  "-INF",                    1 },
{ L_,  Limits::signaling_NaN(),     'S',  0,   "NaN",                    1 },
{ L_,  Limits::quiet_NaN(),         'S',  0,   "NaN",                    1 },
#undef DFP
            };

            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int ti = 0; ti < 2 * NUM_DATA; ++ti) {
                const bool  DECIMAL   = ti % 2;
                const Data& data      = DATA[ti / 2];
                const int   LINE      = data.d_lineNum;
                const Type  INPUT     = data.d_input;
                const char  STYLE     = data.d_style;
                const char *RESULT    = data.d_result;
                const int   PRECISION = data.d_precision;
                const bool  WEIRD     = data.d_weird;
                const bool  SUCCESS   = !DECIMAL || !WEIRD;

                bsl::stringstream ss;
                ss.precision(PRECISION);
                if ('F' == STYLE) ss << bsl::fixed;
                if ('S' == STYLE) ss << bsl::scientific;

                DECIMAL ? Util::printDecimal(ss, INPUT)
                        : Util::printDefault(ss, INPUT);

                ASSERTV(LINE, SUCCESS, ss.fail(), !SUCCESS == ss.fail());

                ASSERTV(LINE, SUCCESS, ss.str(), DECIMAL,
                                          (SUCCESS ? RESULT : "") == ss.str());
            }
        }


        if (verbose) cout << "\nUsing 'char*'." << endl;
        {
            typedef const char* Type;

            static const struct {
                int         d_lineNum;
                Type        d_input;
                const char *d_result;
            } DATA[] = {
                //line    input       result
                //----    -----       ------
                { L_,     "",         ""         },
                { L_,     "Hello",    "Hello"    },
                { L_,     "World!!",  "World!!"  },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int   LINE   = DATA[i].d_lineNum;
                const Type  INPUT  = DATA[i].d_input;
                const char *RESULT = DATA[i].d_result;

                bsl::stringstream ss;

                Util::printDefault(ss, INPUT);

                ASSERTV(LINE, ss.str(), RESULT == ss.str());
            }
        }

        if (verbose) cout << "\nUsing 'bsl::string'." << endl;
        {
            typedef bsl::string Type;

            static const struct {
                int         d_lineNum;
                Type        d_input;
                const char *d_result;
            } DATA[] = {
                //line    input       result
                //----    -----       ------
                { L_,     "",         ""          },
                { L_,     "Hello",    "Hello"     },
                { L_,     "World!!",  "World!!"   },
                { L_,     "&AB",      "&amp;AB"   },
                { L_,     "A&B",      "A&amp;B"   },
                { L_,     "AB&",      "AB&amp;"   },
                { L_,     "<AB",      "&lt;AB"    },
                { L_,     "A<B",      "A&lt;B"    },
                { L_,     "AB<",      "AB&lt;"    },
                { L_,     ">AB",      "&gt;AB"    },
                { L_,     "A>B",      "A&gt;B"    },
                { L_,     "AB>",      "AB&gt;"    },
                { L_,     "\'AB",     "&apos;AB"  },
                { L_,     "A\'B",     "A&apos;B"  },
                { L_,     "AB\'",     "AB&apos;"  },
                { L_,     "\"AB",     "&quot;AB"  },
                { L_,     "A\"B",     "A&quot;B"  },
                { L_,     "AB\"",     "AB&quot;"  },
                { L_,     "\xC3\xB6" "AB",    "\xC3\xB6" "AB"     },
                { L_,     "A" "\xC3\xB6" "B", "A" "\xC3\xB6" "B"  },
                { L_,     "AB" "\xC3\xB6",    "AB" "\xC3\xB6"     },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int   LINE   = DATA[i].d_lineNum;
                const Type  INPUT  = DATA[i].d_input;
                const char *RESULT = DATA[i].d_result;

                bsl::stringstream ss;

                Util::printDefault(ss, INPUT);

                ASSERTV(LINE, ss.str(), RESULT == ss.str());
            }
        }

        if (verbose) cout << "\nUsing 'bslstl::StringRef'." << endl;
        {
            typedef bslstl::StringRef Type;

            static const struct {
                int         d_lineNum;
                Type        d_input;
                const char *d_result;
            } DATA[] = {
                //line    input       result
                //----    -----       ------
                { L_,     "",         ""          },
                { L_,     "Hello",    "Hello"     },
                { L_,     "World!!",  "World!!"   },
                { L_,     "&AB",      "&amp;AB"   },
                { L_,     "A&B",      "A&amp;B"   },
                { L_,     "AB&",      "AB&amp;"   },
                { L_,     "<AB",      "&lt;AB"    },
                { L_,     "A<B",      "A&lt;B"    },
                { L_,     "AB<",      "AB&lt;"    },
                { L_,     ">AB",      "&gt;AB"    },
                { L_,     "A>B",      "A&gt;B"    },
                { L_,     "AB>",      "AB&gt;"    },
                { L_,     "\'AB",     "&apos;AB"  },
                { L_,     "A\'B",     "A&apos;B"  },
                { L_,     "AB\'",     "AB&apos;"  },
                { L_,     "\"AB",     "&quot;AB"  },
                { L_,     "A\"B",     "A&quot;B"  },
                { L_,     "AB\"",     "AB&quot;"  },
                { L_,     "\xC3\xB6" "AB",    "\xC3\xB6" "AB"     },
                { L_,     "A" "\xC3\xB6" "B", "A" "\xC3\xB6" "B"  },
                { L_,     "AB" "\xC3\xB6",    "AB" "\xC3\xB6"     },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int   LINE   = DATA[i].d_lineNum;
                const Type  INPUT  = DATA[i].d_input;
                const char *RESULT = DATA[i].d_result;

                bsl::stringstream ss;

                Util::printDefault(ss, INPUT);

                ASSERTV(LINE, ss.str(), RESULT == ss.str());
            }
        }

        if (verbose) cout << "\nUsing 'MyStringRef'." << endl;
        {
            typedef MyStringRef Type;

            static const struct {
                int         d_lineNum;
                Type        d_input;
                const char *d_result;
            } DATA[] = {
                //line    input       result
                //----    -----       ------
                { L_,     "",         ""          },
                { L_,     "Hello",    "Hello"     },
                { L_,     "World!!",  "World!!"   },
                { L_,     "&AB",      "&amp;AB"   },
                { L_,     "A&B",      "A&amp;B"   },
                { L_,     "AB&",      "AB&amp;"   },
                { L_,     "<AB",      "&lt;AB"    },
                { L_,     "A<B",      "A&lt;B"    },
                { L_,     "AB<",      "AB&lt;"    },
                { L_,     ">AB",      "&gt;AB"    },
                { L_,     "A>B",      "A&gt;B"    },
                { L_,     "AB>",      "AB&gt;"    },
                { L_,     "\'AB",     "&apos;AB"  },
                { L_,     "A\'B",     "A&apos;B"  },
                { L_,     "AB\'",     "AB&apos;"  },
                { L_,     "\"AB",     "&quot;AB"  },
                { L_,     "A\"B",     "A&quot;B"  },
                { L_,     "AB\"",     "AB&quot;"  },
                { L_,     "\xC3\xB6" "AB",    "\xC3\xB6" "AB"     },
                { L_,     "A" "\xC3\xB6" "B", "A" "\xC3\xB6" "B"  },
                { L_,     "AB" "\xC3\xB6",    "AB" "\xC3\xB6"     },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int   LINE   = DATA[i].d_lineNum;
                const Type  INPUT  = DATA[i].d_input;
                const char *RESULT = DATA[i].d_result;

                bsl::stringstream ss;

                Util::printDefault(ss, INPUT);

                ASSERTV(LINE, ss.str(), RESULT == ss.str());
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
                { L_,     "",         ""              },
                { L_,     "a",        "YQ=="          },
                { L_,     "ab",       "YWI="          },
                { L_,     "abc",      "YWJj"          },
                { L_,     "abcd",     "YWJjZA=="      },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int          LINE          = DATA[i].d_lineNum;
                const char        *INPUT         = DATA[i].d_input;
                const char        *RESULT        = DATA[i].d_result;
                const bsl::size_t  INPUT_LENGTH  = bsl::strlen(INPUT);

                Type mX(INPUT, INPUT + INPUT_LENGTH); const Type& X = mX;
                bsl::stringstream ss;

                Util::printDefault(ss, X);

                ASSERTV(LINE, ss.str(), RESULT == ss.str());
            }
        }

        if (verbose) cout << "\nUsing 'MyEnumeration::Value'." << endl;
        {
            typedef test::MyEnumeration::Value Type;

            static const struct {
                int         d_lineNum;
                Type        d_input;
                const char *d_result;
            } DATA[] = {
                //line    input                         result
                //----    -----                         ------
                { L_,     test::MyEnumeration::VALUE1,  "VALUE1"    },
                { L_,     test::MyEnumeration::VALUE2,  "VALUE2"    },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int   LINE   = DATA[i].d_lineNum;
                const Type  INPUT  = DATA[i].d_input;
                const char *RESULT = DATA[i].d_result;

                bsl::stringstream ss;

                Util::printDefault(ss, INPUT);

                ASSERTV(LINE, ss.str(), RESULT == ss.str());
            }
        }

        if (verbose) cout << "\nUsing 'CustomizedString'." << endl;
        {
            typedef test::CustomizedString Type;

            static const struct {
                int         d_lineNum;
                Type        d_input;
                const char *d_result;
            } DATA[] = {
                //line    input               result
                //----    -----               ------
                { L_,     Type(""),           ""         },
                { L_,     Type("a"),          "a"        },
                { L_,     Type("ab"),         "ab"       },
                { L_,     Type("abc"),        "abc"      },
                { L_,     Type("abcd"),       "abcd"     },
                { L_,     Type("abcde"),      "abcde"    },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int   LINE   = DATA[i].d_lineNum;
                const Type  INPUT  = DATA[i].d_input;
                const char *RESULT = DATA[i].d_result;

                bsl::stringstream ss;

                Util::printDefault(ss, Type(INPUT));

                ASSERTV(LINE, ss.str(), RESULT == ss.str());
            }
        }

        if (verbose) cout << "\nUsing 'CustomizedInt'." << endl;
        {
            typedef test::CustomizedInt Type;

            static const struct {
                int         d_lineNum;
                Type        d_input;
                const char *d_result;
            } DATA[] = {
                //line    input                result
                //----    -----                ------
                { L_,     Type(-2147483647-1), "-2147483648"   },
                { L_,     Type(-2147483647),   "-2147483647"   },
                { L_,     Type(-1),            "-1"            },
                { L_,     Type(0),             "0"             },
                { L_,     Type(1),             "1"             },
                { L_,     Type(2147483646),    "2147483646"    },
                { L_,     Type(2147483647),    "2147483647"    },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int   LINE   = DATA[i].d_lineNum;
                const Type  INPUT  = DATA[i].d_input;
                const char *RESULT = DATA[i].d_result;

                bsl::stringstream ss;

                Util::printDefault(ss, Type(INPUT));

                ASSERTV(LINE, ss.str(), RESULT == ss.str());
            }
        }

        if (verbose) cout << "\nUsing date and time types." << endl;
        {
            const struct {
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

            const char *expectedDate[] = {
                "0001-01-01",
                "2005-01-01",
                "0123-06-15",
                "1999-10-12",
                "1999-10-12",
                "1999-10-12",
                "1999-10-12",
                "1999-10-12",
                "1999-12-31"
            };

            const char *expectedDateTz[] = {
                "0001-01-01+00:00",
                "2005-01-01-01:30",
                "0123-06-15-04:00",
                "1999-10-12-12:00",
                "1999-10-12+01:30",
                "1999-10-12+04:00",
                "1999-10-12+04:00",
                "1999-10-12+12:00",
                "1999-12-31+12:00"
            };

            const char *expectedDateTzUseZ[] = {
                "0001-01-01Z",
                "2005-01-01-01:30",
                "0123-06-15-04:00",
                "1999-10-12-12:00",
                "1999-10-12+01:30",
                "1999-10-12+04:00",
                "1999-10-12+04:00",
                "1999-10-12+12:00",
                "1999-12-31+12:00"
            };

            const char *expectedTime[] = {
                "00:00:00.000000",
                "00:00:00.000000",
                "13:40:59.000000",
                "23:00:01.000000",
                "23:00:01.000000",
                "23:00:01.456000",
                "23:00:01.456000",
                "23:00:01.999000",
                "23:59:59.999000"
            };

            const char *expectedTimeTz[] = {
                "00:00:00.000000+00:00",
                "00:00:00.000000-01:30",
                "13:40:59.000000-04:00",
                "23:00:01.000000-12:00",
                "23:00:01.000000+01:30",
                "23:00:01.456000+04:00",
                "23:00:01.456000+04:00",
                "23:00:01.999000+12:00",
                "23:59:59.999000+12:00"
            };

            const char *expectedTimeTzUseZ[] = {
                "00:00:00.000000Z",
                "00:00:00.000000-01:30",
                "13:40:59.000000-04:00",
                "23:00:01.000000-12:00",
                "23:00:01.000000+01:30",
                "23:00:01.456000+04:00",
                "23:00:01.456000+04:00",
                "23:00:01.999000+12:00",
                "23:59:59.999000+12:00"
            };

            const char *expectedDatetime[] = {
                "0001-01-01T00:00:00.000000",
                "2005-01-01T00:00:00.000000",
                "0123-06-15T13:40:59.000000",
                "1999-10-12T23:00:01.000000",
                "1999-10-12T23:00:01.000000",
                "1999-10-12T23:00:01.456000",
                "1999-10-12T23:00:01.456789",
                "1999-10-12T23:00:01.999789",
                "1999-12-31T23:59:59.999999"
            };

            const char *expectedDatetimeMs[] = {
                "0001-01-01T00:00:00.000",
                "2005-01-01T00:00:00.000",
                "0123-06-15T13:40:59.000",
                "1999-10-12T23:00:01.000",
                "1999-10-12T23:00:01.000",
                "1999-10-12T23:00:01.456",
                "1999-10-12T23:00:01.456",
                "1999-10-12T23:00:01.999",
                "1999-12-31T23:59:59.999"
            };

            const char *expectedDatetimeTz[] = {
                "0001-01-01T00:00:00.000000+00:00",
                "2005-01-01T00:00:00.000000-01:30",
                "0123-06-15T13:40:59.000000-04:00",
                "1999-10-12T23:00:01.000000-12:00",
                "1999-10-12T23:00:01.000000+01:30",
                "1999-10-12T23:00:01.456000+04:00",
                "1999-10-12T23:00:01.456789+04:00",
                "1999-10-12T23:00:01.999789+12:00",
                "1999-12-31T23:59:59.999999+12:00"
            };

            const char *expectedDatetimeTzUseZ[] = {
                "0001-01-01T00:00:00.000000Z",
                "2005-01-01T00:00:00.000000-01:30",
                "0123-06-15T13:40:59.000000-04:00",
                "1999-10-12T23:00:01.000000-12:00",
                "1999-10-12T23:00:01.000000+01:30",
                "1999-10-12T23:00:01.456000+04:00",
                "1999-10-12T23:00:01.456789+04:00",
                "1999-10-12T23:00:01.999789+12:00",
                "1999-12-31T23:59:59.999999+12:00"
            };

            const char *expectedDatetimeTzMs[] = {
                "0001-01-01T00:00:00.000+00:00",
                "2005-01-01T00:00:00.000-01:30",
                "0123-06-15T13:40:59.000-04:00",
                "1999-10-12T23:00:01.000-12:00",
                "1999-10-12T23:00:01.000+01:30",
                "1999-10-12T23:00:01.456+04:00",
                "1999-10-12T23:00:01.456+04:00",
                "1999-10-12T23:00:01.999+12:00",
                "1999-12-31T23:59:59.999+12:00"
            };

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

                bdlt::Date       theDate(YEAR, MONTH, DAY);
                bdlt::Time       theTime(HOUR, MINUTE, SECOND,
                                        MILLISECOND);
                bdlt::Datetime   theDatetime(YEAR, MONTH, DAY,
                                            HOUR, MINUTE, SECOND,
                                            MILLISECOND, MICROSECOND);

                bdlt::DateTz     theDateTz(theDate, OFFSET);
                bdlt::TimeTz     theTimeTz(theTime, OFFSET);
                bdlt::DatetimeTz theDatetimeTz(theDatetime, OFFSET);

                if (verbose) cout << "Print Date" << endl;
                {
                    const char *EXP = expectedDate[ti];
                    bsl::ostringstream oss;
                    Util::printDefault(oss, theDate);

                    bsl::string result = oss.str();
                    ASSERTV(LINE, result, EXP, result == EXP);
                }

                if (verbose) cout << "Print DateTz" << endl;
                {
                    const char *EXP = expectedDateTz[ti];

                    bsl::ostringstream oss;
                    Util::printDefault(oss, theDateTz);

                    bsl::string result = oss.str();
                    ASSERTV(LINE, result, EXP, result == EXP);
                }

                if (verbose) cout << "Print DateTzUseZ" << endl;
                {
                    const char *EXP = expectedDateTzUseZ[ti];

                    balxml::EncoderOptions options;
                    options.setUseZAbbreviationForUtc(true);

                    bsl::ostringstream oss;
                    Util::printDefault(oss, theDateTz, &options);

                    bsl::string result = oss.str();
                    ASSERTV(LINE, result, EXP, result == EXP);
                }

                if (verbose) cout << "Print Time" << endl;
                {
                    const char *EXP = expectedTime[ti];

                    bsl::ostringstream oss;
                    Util::printDefault(oss, theTime);

                    bsl::string result = oss.str();
                    ASSERTV(LINE, result, EXP, result == EXP);
                }

                if (verbose) cout << "Print TimeTz" << endl;
                {
                    const char *EXP = expectedTimeTz[ti];

                    bsl::ostringstream oss;
                    Util::printDefault(oss, theTimeTz);

                    bsl::string result = oss.str();
                    ASSERTV(LINE, result, EXP, result == EXP);
                }

                if (verbose) cout << "Print TimeTzUseZ" << endl;
                {
                    const char *EXP = expectedTimeTzUseZ[ti];

                    balxml::EncoderOptions options;
                    options.setUseZAbbreviationForUtc(true);

                    bsl::ostringstream oss;
                    Util::printDefault(oss, theTimeTz, &options);

                    bsl::string result = oss.str();
                    ASSERTV(LINE, result, EXP, result == EXP);
                }

                if (verbose) cout << "Print Datetime" << endl;
                {
                    const char *EXP = expectedDatetime[ti];

                    bsl::ostringstream oss;
                    Util::printDefault(oss, theDatetime);

                    bsl::string result = oss.str();
                    ASSERTV(LINE, result, EXP, result == EXP);
                }

                if (verbose) cout << "Print DatetimeMs" << endl;
                {
                    const char *EXP = expectedDatetimeMs[ti];

                    bsl::ostringstream oss;
                    balxml::EncoderOptions options;
                    options.setDatetimeFractionalSecondPrecision(3);
                    Util::printDefault(oss, theDatetime, &options);

                    bsl::string result = oss.str();
                    ASSERTV(LINE, result, EXP, result == EXP);
                }

                if (verbose) cout << "Print DatetimeTz" << endl;
                {
                    const char *EXP = expectedDatetimeTz[ti];

                    bsl::ostringstream oss;
                    Util::printDefault(oss, theDatetimeTz);

                    bsl::string result = oss.str();
                    ASSERTV(LINE, result, EXP, result == EXP);
                }

                if (verbose) cout << "Print DatetimeTzUseZ" << endl;
                {
                    const char *EXP = expectedDatetimeTzUseZ[ti];

                    balxml::EncoderOptions options;
                    options.setUseZAbbreviationForUtc(true);

                    bsl::ostringstream oss;
                    Util::printDefault(oss, theDatetimeTz, &options);

                    bsl::string result = oss.str();
                    ASSERTV(LINE, result, EXP, result == EXP);
                }

                if (verbose) cout << "Print DatetimeTzMs" << endl;
                {
                    const char *EXP = expectedDatetimeTzMs[ti];

                    bsl::ostringstream oss;
                    balxml::EncoderOptions options;
                    options.setDatetimeFractionalSecondPrecision(3);
                    Util::printDefault(oss, theDatetimeTz, &options);

                    bsl::string result = oss.str();
                    ASSERTV(LINE, result, EXP, result == EXP);
                }

                if (verbose) cout << "Print 'DateOrDateTz'" << endl;
                {
                    bdlb::Variant2<bdlt::Date, bdlt::DateTz> dateVariant(
                                                                      theDate);
                    ASSERTV(dateVariant.is<bdlt::Date>());
                    ASSERTV(theDate == dateVariant.the<bdlt::Date>());

                    const char *EXP_DATE = expectedDate[ti];
                    bsl::ostringstream ossDate;
                    Util::printDefault(ossDate, dateVariant);

                    bsl::string result = ossDate.str();
                    ASSERTV(LINE, result, EXP_DATE, EXP_DATE == result);

                    dateVariant = theDateTz;
                    ASSERTV(dateVariant.is<bdlt::DateTz>());
                    ASSERTV(theDateTz == dateVariant.the<bdlt::DateTz>());

                    const char *EXP_DATETZ = expectedDateTz[ti];
                    bsl::ostringstream ossDateTz;
                    Util::printDefault(ossDateTz, dateVariant);

                    result = ossDateTz.str();
                    ASSERTV(LINE, result, EXP_DATETZ, EXP_DATETZ == result);
                }

                if (verbose) cout << "Print DateOrDateTzUseZ" << endl;
                {
                    const char *EXP = expectedDateTzUseZ[ti];

                    balxml::EncoderOptions options;
                    options.setUseZAbbreviationForUtc(true);

                    bdlb::Variant2<bdlt::Date, bdlt::DateTz> dateVariant(
                                                                    theDateTz);

                    ASSERTV(dateVariant.is<bdlt::DateTz>());
                    ASSERTV(theDateTz == dateVariant.the<bdlt::DateTz>());

                    bsl::ostringstream oss;
                    Util::printDefault(oss, dateVariant, &options);

                    bsl::string result = oss.str();
                    ASSERTV(LINE, result, EXP, result == EXP);
                }

                if (verbose) cout << "Print 'TimeOrTimeTz'" << endl;
                {
                    bdlb::Variant2<bdlt::Time, bdlt::TimeTz> dateVariant(
                                                                      theTime);
                    ASSERTV(dateVariant.is<bdlt::Time>());
                    ASSERTV(theTime == dateVariant.the<bdlt::Time>());

                    const char *EXP_TIME = expectedTime[ti];
                    bsl::ostringstream ossTime;
                    Util::printDefault(ossTime, dateVariant);

                    bsl::string result = ossTime.str();
                    ASSERTV(LINE, result, EXP_TIME, EXP_TIME == result);

                    dateVariant = theTimeTz;
                    ASSERTV(dateVariant.is<bdlt::TimeTz>());
                    ASSERTV(theTimeTz == dateVariant.the<bdlt::TimeTz>());

                    const char *EXP_TIMETZ = expectedTimeTz[ti];
                    bsl::ostringstream ossTimeTz;
                    Util::printDefault(ossTimeTz, dateVariant);

                    result = ossTimeTz.str();
                    ASSERTV(LINE, result, EXP_TIMETZ, EXP_TIMETZ == result);
                }

                if (verbose) cout << "Print TimeOrTimeTzUseZ" << endl;
                {
                    const char *EXP = expectedTimeTzUseZ[ti];

                    balxml::EncoderOptions options;
                    options.setUseZAbbreviationForUtc(true);

                    bdlb::Variant2<bdlt::Time, bdlt::TimeTz> dateVariant(
                                                                    theTimeTz);

                    ASSERTV(dateVariant.is<bdlt::TimeTz>());
                    ASSERTV(theTimeTz == dateVariant.the<bdlt::TimeTz>());

                    bsl::ostringstream oss;
                    Util::printDefault(oss, dateVariant, &options);

                    bsl::string result = oss.str();
                    ASSERTV(LINE, result, EXP, result == EXP);
                }

                if (verbose) cout << "Print 'DatetimeOrDatetimeTz'" << endl;
                {
                    bdlb::Variant2<bdlt::Datetime, bdlt::DatetimeTz>
                                                      dateVariant(theDatetime);

                    ASSERTV(dateVariant.is<bdlt::Datetime>());
                    ASSERTV(theDatetime == dateVariant.the<bdlt::Datetime>());

                    const char *EXP_DATETIME = expectedDatetime[ti];
                    bsl::ostringstream ossDatetime;
                    Util::printDefault(ossDatetime, dateVariant);

                    bsl::string result = ossDatetime.str();
                    ASSERTV(LINE, result, EXP_DATETIME,
                            EXP_DATETIME == result);

                    dateVariant = theDatetimeTz;
                    ASSERTV(dateVariant.is<bdlt::DatetimeTz>());
                    ASSERTV(theDatetimeTz ==
                            dateVariant.the<bdlt::DatetimeTz>());

                    const char *EXP_DATETIMETZ = expectedDatetimeTz[ti];
                    bsl::ostringstream ossDatetimeTz;
                    Util::printDefault(ossDatetimeTz, dateVariant);

                    result = ossDatetimeTz.str();
                    ASSERTV(LINE, result, EXP_DATETIMETZ,
                            EXP_DATETIMETZ == result);
                }

                if (verbose) cout << "Print DatetimeOrDatetimeTzUseZ" << endl;
                {
                    const char *EXP = expectedDatetimeTzUseZ[ti];

                    balxml::EncoderOptions options;
                    options.setUseZAbbreviationForUtc(true);

                    bdlb::Variant2<bdlt::Datetime, bdlt::DatetimeTz>
                                                    dateVariant(theDatetimeTz);

                    ASSERTV(dateVariant.is<bdlt::DatetimeTz>());
                    ASSERTV(theDatetimeTz ==
                            dateVariant.the<bdlt::DatetimeTz>());

                    bsl::ostringstream oss;
                    Util::printDefault(oss, dateVariant, &options);

                    bsl::string result = oss.str();
                    ASSERTV(LINE, result, EXP, result == EXP);
                }

                if (verbose) cout << "Print DatetimeMs" << endl;
                {
                    bsl::ostringstream oss;
                    balxml::EncoderOptions options;
                    options.setDatetimeFractionalSecondPrecision(3);

                    bdlb::Variant2<bdlt::Datetime, bdlt::DatetimeTz>
                                                      dateVariant(theDatetime);

                    ASSERTV(dateVariant.is<bdlt::Datetime>());
                    ASSERTV(theDatetime == dateVariant.the<bdlt::Datetime>());

                    const char *EXP_DATETIME = expectedDatetimeMs[ti];
                    bsl::ostringstream ossDatetime;
                    Util::printDefault(ossDatetime, dateVariant, &options);

                    bsl::string result = ossDatetime.str();
                    ASSERTV(LINE, result, EXP_DATETIME,
                            EXP_DATETIME == result);

                    dateVariant = theDatetimeTz;
                    ASSERTV(dateVariant.is<bdlt::DatetimeTz>());
                    ASSERTV(theDatetimeTz ==
                            dateVariant.the<bdlt::DatetimeTz>());

                    const char *EXP_DATETIMETZ = expectedDatetimeTzMs[ti];
                    bsl::ostringstream ossDatetimeTz;
                    Util::printDefault(ossDatetimeTz, dateVariant, &options);

                    result = ossDatetimeTz.str();
                    ASSERTV(LINE, result, EXP_DATETIMETZ,
                            EXP_DATETIMETZ == result);
                }
            }
        }
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // TESTING 'printText' FUNCTIONS
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
        //: 4 That valid strings are printed correctly, and strings with
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

        if (verbose) cout << "\nTESTING 'printText' FUNCTIONS"
                          << "\n=============================" << endl;

        if (verbose) cout << "\nUsing 'bool'." << endl;
        {
            typedef bool Type;

            static const struct {
                int         d_lineNum;
                Type        d_input;
                const char *d_result;
            } DATA[] = {
                //line    input       result
                //----    -----       ------
                { L_,     true,       "true"             },
                { L_,     false,      "false"            },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int   LINE   = DATA[i].d_lineNum;
                const Type  INPUT  = DATA[i].d_input;
                const char *RESULT = DATA[i].d_result;

                bsl::stringstream ss;

                Util::printText(ss, INPUT);

                ASSERTV(LINE, ss.str(), RESULT == ss.str());
            }
        }

        if (verbose) cout << "\nUsing 'char' on valid input." << endl;
        {
            typedef char Type;

            static const struct {
                int         d_lineNum;
                Type        d_input;
                const char *d_result;
            } DATA[] = {
                //line    input       result
                //----    -----       ------
                { L_,     0x09,       "\t"         },
                { L_,     0x0a,       "\n"         },
                { L_,     0x0d,       "\x0d"       },
                { L_,     0x20,       " "          },
                { L_,     0x21,       "!"          },
                { L_,     0x22,       "&quot;"     },
                { L_,     0x23,       "#"          },
                { L_,     0x24,       "$"          },
                { L_,     0x25,       "%"          },
                { L_,     0x26,       "&amp;"      },
                { L_,     0x27,       "&apos;"     },
                { L_,     0x28,       "("          },
                { L_,     0x29,       ")"          },
                { L_,     0x2a,       "*"          },
                { L_,     0x2b,       "+"          },
                { L_,     0x2c,       ","          },
                { L_,     0x2d,       "-"          },
                { L_,     0x2e,       "."          },
                { L_,     0x2f,       "/"          },
                { L_,     0x30,       "0"          },
                { L_,     0x31,       "1"          },
                { L_,     0x32,       "2"          },
                { L_,     0x33,       "3"          },
                { L_,     0x34,       "4"          },
                { L_,     0x35,       "5"          },
                { L_,     0x36,       "6"          },
                { L_,     0x37,       "7"          },
                { L_,     0x38,       "8"          },
                { L_,     0x39,       "9"          },
                { L_,     0x3a,       ":"          },
                { L_,     0x3b,       ";"          },
                { L_,     0x3c,       "&lt;"       },
                { L_,     0x3d,       "="          },
                { L_,     0x3e,       "&gt;"       },
                { L_,     0x3f,       "?"          },
                { L_,     0x40,       "@"          },
                { L_,     0x41,       "A"          },
                { L_,     'A',        "A"          },
                // treat all uppercase (0x41 until 0x5a) as a region, and only
                // test the boundaries.
                { L_,     'Z',        "Z"          },
                { L_,     0x5a,       "Z"          },
                { L_,     0x5b,       "["          },
                { L_,     0x5c,       "\\"         },
                { L_,     0x5d,       "]"          },
                { L_,     0x5e,       "^"          },
                { L_,     0x5f,       "_"          },
                { L_,     0x60,       "`"          },
                { L_,     0x61,       "a"          },
                { L_,     'a',        "a"          },
                // treat all lowercase (0x61 until 0x7a) as a region, and only
                // test the boundaries.
                { L_,     'z',        "z"          },
                { L_,     0x7a,       "z"          },
                { L_,     0x7b,       "{"          },
                { L_,     0x7c,       "|"          },
                { L_,     0x7d,       "}"          },
                { L_,     0x7e,       "~"          },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int   LINE   = DATA[i].d_lineNum;
                const Type  INPUT  = DATA[i].d_input;
                const char *RESULT = DATA[i].d_result;

                bsl::stringstream ss;

                Util::printText(ss, INPUT);

                ASSERTV(LINE, RESULT, ss.str(), RESULT == ss.str());
            }
        }

        if (verbose) cout << "\nUsing 'char' on invalid input." << endl;
        {
            typedef unsigned char Type;

            static const struct {
                int         d_lineNum;
                Type        d_input;
                const char *d_result;
            } DATA[] = {
                //line    input       result
                //----    -----       ------
                { L_,     0x00,        ""          },
                { L_,     0x01,        ""          },
                { L_,     0x02,        ""          },
                { L_,     0x03,        ""          },
                { L_,     0x04,        ""          },
                { L_,     0x05,        ""          },
                { L_,     0x06,        ""          },
                { L_,     0x07,        ""          },
                { L_,     0x08,        ""          },
                // skip TAB (0x09), LF (0x0a) and CR (0x0d)
                { L_,     0x0b,        ""          },
                { L_,     0x0c,        ""          },
                { L_,     0x0e,        ""          },
                { L_,     0x0f,        ""          },
                { L_,     0x10,        ""          },
                { L_,     0x11,        ""          },
                { L_,     0x12,        ""          },
                { L_,     0x13,        ""          },
                { L_,     0x14,        ""          },
                { L_,     0x15,        ""          },
                { L_,     0x16,        ""          },
                { L_,     0x17,        ""          },
                { L_,     0x18,        ""          },
                { L_,     0x19,        ""          },
                { L_,     0x1a,        ""          },
                { L_,     0x1b,        ""          },
                { L_,     0x1c,        ""          },
                { L_,     0x1d,        ""          },
                { L_,     0x1e,        ""          },
                { L_,     0x1f,        ""          },
                // skip ASCII chars (0x20 until 0x7e), but not DEL (0x7f)
                { L_,     0x7f,        ""          },
                { L_,     0x80,        ""          },
                // treat all unencodables (0x80 until 0xc1) as a region, and
                // only test the boundaries.
                { L_,     0xc1,        ""          },
                { L_,     0xc2,        ""          },
                // treat all two-byte sequences (0xc2 until 0xdf) as a region,
                // and only test the boundaries.
                { L_,     0xdf,        ""          },
                { L_,     0xee,        ""          },
                // treat all three-byte sequences (0xee until 0xef) as a
                // region, and only test the boundaries.
                { L_,     0xef,        ""          },
                { L_,     0xf0,        ""          },
                // treat all four-byte sequences (0xf0 until 0xf4) as a region,
                // and only test the boundaries.
                { L_,     0xf4,        ""          },
                { L_,     0xf5,        ""          },
                // treat all unencodables (0xf5 until 0xff) as a region, and
                // only test the boundaries.
                { L_,     0xff,        ""          },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int   LINE   = DATA[i].d_lineNum;
                const char  INPUT  = (char) DATA[i].d_input;
                const char *RESULT = DATA[i].d_result;

                bsl::stringstream ss;

                Util::printText(ss, INPUT);

                ASSERTV(LINE, RESULT, ss.str(), RESULT == ss.str());
                ASSERTV(LINE, ss.fail());
            }
        }

        // Note that we have already tested the valid single byte characters
        // individually, so the only concerns here are multibyte character
        // strings and boundary conditions (empty string).  We follow the
        // boundary and area selection methods, by picking the boundary values
        // and a value in between at random, for each range, and taking the
        // cross product of all these values for making sure that all boundary
        // faces (in a hypercube) are covered.

        {
            static const struct {
                int         d_lineNum;
                const char *d_input;
                const char *d_result;
            } VALID_DATA[] = {
                //line input                     result
                //---- -----                     ------
                { L_,  "",                       ""                          },

                { L_,  "\x09",                   "\t"                        },
                { L_,  "\x0a",                   "\n"                        },
                { L_,  "\x0d",                   "\x0d"                      },
                { L_,  "\x22",                   "&quot;"                    },
                { L_,  "\x26",                   "&amp;"                     },
                { L_,  "\x27",                   "&apos;"                    },
                { L_,  "\x3c",                   "&lt;"                      },
                { L_,  "\x3e",                   "&gt;"                      },

                { L_,  "Hello",                  "Hello"                     },
                { L_,  "Hello World!!",          "Hello World!!"             },
                { L_,  "Hello \t World",         "Hello \t World"            },
                { L_,  "Hello \n World",         "Hello \n World"            },
                { L_,  "Hello \x0d World",       "Hello \x0d World"          },
                { L_,  "Pi is < 3.15",           "Pi is &lt; 3.15"           },
                { L_,  "Pi is > 3.14",           "Pi is &gt; 3.14"           },
                { L_,  "Tom & Jerry",            "Tom &amp; Jerry"           },
                { L_,  "'Hello' World!",         "&apos;Hello&apos; World!"  },
                { L_,  "Hello \"World\"",        "Hello &quot;World&quot;"   },

                { L_,  "<![CDATA&]]>",           "&lt;![CDATA&amp;]]&gt;"    },
                { L_,  "![CDATA[&]]>",           "![CDATA[&amp;]]&gt;"       },
                { L_,  "<![CDATA[Hello]]>World",
                                               "&lt;![CDATA[Hello]]&gt;World"},

                // Two-byte character sequences.

                { L_,  "\xc2\x80",               "\xc2\x80"                  },
                { L_,  "\xc2\xa3",               "\xc2\xa3"                  },
                { L_,  "\xc2\xbf",               "\xc2\xbf"                  },
                { L_,  "\xd0\x80",               "\xd0\x80"                  },
                { L_,  "\xd0\x9d",               "\xd0\x9d"                  },
                { L_,  "\xd0\xbf",               "\xd0\xbf"                  },
                { L_,  "\xdf\x80",               "\xdf\x80"                  },
                { L_,  "\xdf\xa6",               "\xdf\xa6"                  },
                { L_,  "\xdf\xbf",               "\xdf\xbf"                  },

                // Three-byte character sequences.

                // Note that first byte 0xe0 is special (second byte ranges in
                // 0xa0..0xbf instead of the usual 0x80..0xbf).  Note also that
                // first byte 0xed is special (second byte ranges in 0x80..0x9f
                // instead of the usual 0x80..0xbf).

                { L_,  "\xe0\xa0\x80",           "\xe0\xa0\x80"              },
                { L_,  "\xe0\xa0\xa3",           "\xe0\xa0\xa3"              },
                { L_,  "\xe0\xa0\xbf",           "\xe0\xa0\xbf"              },
                { L_,  "\xe0\xb5\x80",           "\xe0\xb5\x80"              },
                { L_,  "\xe0\xab\x9d",           "\xe0\xab\x9d"              },
                { L_,  "\xe0\xa9\xbf",           "\xe0\xa9\xbf"              },
                { L_,  "\xe0\xbf\x80",           "\xe0\xbf\x80"              },
                { L_,  "\xe0\xbf\xaf",           "\xe0\xbf\xaf"              },
                { L_,  "\xe0\xbf\xbf",           "\xe0\xbf\xbf"              },

                { L_,  "\xe1\x80\x80",           "\xe1\x80\x80"              },
                { L_,  "\xe1\x80\xa3",           "\xe1\x80\xa3"              },
                { L_,  "\xe1\x80\xbf",           "\xe1\x80\xbf"              },
                { L_,  "\xe1\x9a\x80",           "\xe1\x9a\x80"              },
                { L_,  "\xe1\x85\x9d",           "\xe1\x85\x9d"              },
                { L_,  "\xe1\xab\xbf",           "\xe1\xab\xbf"              },
                { L_,  "\xe1\xbf\x80",           "\xe1\xbf\x80"              },
                { L_,  "\xe1\xbf\xa6",           "\xe1\xbf\xa6"              },
                { L_,  "\xe1\xbf\xbf",           "\xe1\xbf\xbf"              },

                { L_,  "\xe7\x80\x80",           "\xe7\x80\x80"              },
                { L_,  "\xe7\x80\xa3",           "\xe7\x80\xa3"              },
                { L_,  "\xe7\x80\xbf",           "\xe7\x80\xbf"              },
                { L_,  "\xe7\x9a\x80",           "\xe7\x9a\x80"              },
                { L_,  "\xe7\x85\x9d",           "\xe7\x85\x9d"              },
                { L_,  "\xe7\xab\xbf",           "\xe7\xab\xbf"              },
                { L_,  "\xe7\xbf\x80",           "\xe7\xbf\x80"              },
                { L_,  "\xe7\xbf\xa6",           "\xe7\xbf\xa6"              },
                { L_,  "\xe7\xbf\xbf",           "\xe7\xbf\xbf"              },

                { L_,  "\xec\x80\x80",           "\xec\x80\x80"              },
                { L_,  "\xec\x80\xa3",           "\xec\x80\xa3"              },
                { L_,  "\xec\x80\xbf",           "\xec\x80\xbf"              },
                { L_,  "\xec\x9a\x80",           "\xec\x9a\x80"              },
                { L_,  "\xec\xab\x9d",           "\xec\xab\x9d"              },
                { L_,  "\xec\xb3\xbf",           "\xec\xb3\xbf"              },
                { L_,  "\xec\xbf\x80",           "\xec\xbf\x80"              },
                { L_,  "\xec\xbf\x98",           "\xec\xbf\x98"              },
                { L_,  "\xec\xbf\xbf",           "\xec\xbf\xbf"              },

                { L_,  "\xed\x80\x80",           "\xed\x80\x80"              },
                { L_,  "\xed\x80\x83",           "\xed\x80\x83"              },
                { L_,  "\xed\x80\x9f",           "\xed\x80\x9f"              },
                { L_,  "\xed\x9a\x80",           "\xed\x9a\x80"              },
                { L_,  "\xed\x8b\x9d",           "\xed\x8b\x9d"              },
                { L_,  "\xed\x93\xbf",           "\xed\x93\xbf"              },
                { L_,  "\xed\x9f\x80",           "\xed\x9f\x80"              },
                { L_,  "\xed\x9f\x98",           "\xed\x9f\x98"              },
                { L_,  "\xed\x9f\xbf",           "\xed\x9f\xbf"              },

                { L_,  "\xee\x80\x80",           "\xee\x80\x80"              },
                { L_,  "\xee\x80\xa3",           "\xee\x80\xa3"              },
                { L_,  "\xee\x80\xbf",           "\xee\x80\xbf"              },
                { L_,  "\xee\x9a\x80",           "\xee\x9a\x80"              },
                { L_,  "\xee\x85\x9d",           "\xee\x85\x9d"              },
                { L_,  "\xee\xab\xbf",           "\xee\xab\xbf"              },
                { L_,  "\xee\xbf\x80",           "\xee\xbf\x80"              },
                { L_,  "\xee\xbf\xa6",           "\xee\xbf\xa6"              },
                { L_,  "\xee\xbf\xbf",           "\xee\xbf\xbf"              },

                { L_,  "\xef\x80\x80",           "\xef\x80\x80"              },
                { L_,  "\xef\x80\xa3",           "\xef\x80\xa3"              },
                { L_,  "\xef\x80\xbf",           "\xef\x80\xbf"              },
                { L_,  "\xef\x9a\x80",           "\xef\x9a\x80"              },
                { L_,  "\xef\xab\x9d",           "\xef\xab\x9d"              },
                { L_,  "\xef\xb3\xbf",           "\xef\xb3\xbf"              },
                { L_,  "\xef\xbf\x80",           "\xef\xbf\x80"              },
                { L_,  "\xef\xbf\x98",           "\xef\xbf\x98"              },
                { L_,  "\xef\xbf\xbf",           "\xef\xbf\xbf"              },

                // Four-byte character sequences.

                // Note that first byte 0xf0 is special (second byte ranges in
                // 0x90..0xbf instead of the usual 0x80..0xbf).  Note also that
                // first byte 0xf4 is special (second byte ranges in 0x80..0x8f
                // instead of the usual 0x80..0xbf).

                { L_,  "\xf0\x90\x80\x80",       "\xf0\x90\x80\x80"          },
                { L_,  "\xf0\x90\x80\x98",       "\xf0\x90\x80\x98"          },
                { L_,  "\xf0\x90\x80\xbf",       "\xf0\x90\x80\xbf"          },
                { L_,  "\xf0\x90\xa7\x80",       "\xf0\x90\xa7\x80"          },
                { L_,  "\xf0\x90\x95\xa6",       "\xf0\x90\x95\xa6"          },
                { L_,  "\xf0\x90\xa5\xbf",       "\xf0\x90\xa5\xbf"          },
                { L_,  "\xf0\x90\xbf\x80",       "\xf0\x90\xbf\x80"          },
                { L_,  "\xf0\x90\xbf\xa2",       "\xf0\x90\xbf\xa2"          },
                { L_,  "\xf0\x90\xbf\xbf",       "\xf0\x90\xbf\xbf"          },
                { L_,  "\xf0\xa1\x80\x80",       "\xf0\xa1\x80\x80"          },
                { L_,  "\xf0\xa2\x80\x85",       "\xf0\xa2\x80\x85"          },
                { L_,  "\xf0\xa5\x80\xbf",       "\xf0\xa5\x80\xbf"          },
                { L_,  "\xf0\xa9\xa3\x80",       "\xf0\xa9\xa3\x80"          },
                { L_,  "\xf0\x93\xa3\xa6",       "\xf0\x93\xa3\xa6"          },
                { L_,  "\xf0\x95\xa3\xbf",       "\xf0\x95\xa3\xbf"          },
                { L_,  "\xf0\x98\xbf\x80",       "\xf0\x98\xbf\x80"          },
                { L_,  "\xf0\x9d\xbf\xa6",       "\xf0\x9d\xbf\xa6"          },
                { L_,  "\xf0\x9f\xbf\xbf",       "\xf0\x9f\xbf\xbf"          },
                { L_,  "\xf0\xbf\x80\x80",       "\xf0\xbf\x80\x80"          },
                { L_,  "\xf0\xbf\x80\xa6",       "\xf0\xbf\x80\xa6"          },
                { L_,  "\xf0\xbf\x80\xbf",       "\xf0\xbf\x80\xbf"          },
                { L_,  "\xf0\xbf\xa3\x80",       "\xf0\xbf\xa3\x80"          },
                { L_,  "\xf0\xbf\xa3\xa6",       "\xf0\xbf\xa3\xa6"          },
                { L_,  "\xf0\xbf\xa3\xbf",       "\xf0\xbf\xa3\xbf"          },
                { L_,  "\xf0\xbf\xbf\x80",       "\xf0\xbf\xbf\x80"          },
                { L_,  "\xf0\xbf\xbf\xa6",       "\xf0\xbf\xbf\xa6"          },
                { L_,  "\xf0\xbf\xbf\xbf",       "\xf0\xbf\xbf\xbf"          },

                { L_,  "\xf1\x80\x80\x80",       "\xf1\x80\x80\x80"          },
                { L_,  "\xf1\x80\x80\x98",       "\xf1\x80\x80\x98"          },
                { L_,  "\xf1\x80\x80\xbf",       "\xf1\x80\x80\xbf"          },
                { L_,  "\xf1\x80\xa7\x80",       "\xf1\x80\xa7\x80"          },
                { L_,  "\xf1\x80\x95\xa6",       "\xf1\x80\x95\xa6"          },
                { L_,  "\xf1\x80\xa5\xbf",       "\xf1\x80\xa5\xbf"          },
                { L_,  "\xf1\x80\xbf\x80",       "\xf1\x80\xbf\x80"          },
                { L_,  "\xf1\x80\xbf\xa2",       "\xf1\x80\xbf\xa2"          },
                { L_,  "\xf1\x80\xbf\xbf",       "\xf1\x80\xbf\xbf"          },
                { L_,  "\xf1\x81\x80\x80",       "\xf1\x81\x80\x80"          },
                { L_,  "\xf1\xa2\x80\x85",       "\xf1\xa2\x80\x85"          },
                { L_,  "\xf1\x85\x80\xbf",       "\xf1\x85\x80\xbf"          },
                { L_,  "\xf1\xa9\xa3\x80",       "\xf1\xa9\xa3\x80"          },
                { L_,  "\xf1\x93\xa3\xa6",       "\xf1\x93\xa3\xa6"          },
                { L_,  "\xf1\x85\xa3\xbf",       "\xf1\x85\xa3\xbf"          },
                { L_,  "\xf1\x98\xbf\x80",       "\xf1\x98\xbf\x80"          },
                { L_,  "\xf1\x9d\xbf\xa6",       "\xf1\x9d\xbf\xa6"          },
                { L_,  "\xf1\x9f\xbf\xbf",       "\xf1\x9f\xbf\xbf"          },
                { L_,  "\xf1\xbf\x80\x80",       "\xf1\xbf\x80\x80"          },
                { L_,  "\xf1\xbf\x80\xa6",       "\xf1\xbf\x80\xa6"          },
                { L_,  "\xf1\xbf\x80\xbf",       "\xf1\xbf\x80\xbf"          },
                { L_,  "\xf1\xbf\xa3\x80",       "\xf1\xbf\xa3\x80"          },
                { L_,  "\xf1\xbf\xa3\xa6",       "\xf1\xbf\xa3\xa6"          },
                { L_,  "\xf1\xbf\xa3\xbf",       "\xf1\xbf\xa3\xbf"          },
                { L_,  "\xf1\xbf\xbf\x80",       "\xf1\xbf\xbf\x80"          },
                { L_,  "\xf1\xbf\xbf\xa6",       "\xf1\xbf\xbf\xa6"          },
                { L_,  "\xf1\xbf\xbf\xbf",       "\xf1\xbf\xbf\xbf"          },

                { L_,  "\xf2\x80\x80\x80",       "\xf2\x80\x80\x80"          },
                { L_,  "\xf2\x80\x80\x98",       "\xf2\x80\x80\x98"          },
                { L_,  "\xf2\x80\x80\xbf",       "\xf2\x80\x80\xbf"          },
                { L_,  "\xf2\x80\xa7\x80",       "\xf2\x80\xa7\x80"          },
                { L_,  "\xf2\x80\x95\xa6",       "\xf2\x80\x95\xa6"          },
                { L_,  "\xf2\x80\xa5\xbf",       "\xf2\x80\xa5\xbf"          },
                { L_,  "\xf2\x80\xbf\x80",       "\xf2\x80\xbf\x80"          },
                { L_,  "\xf2\x80\xbf\xa2",       "\xf2\x80\xbf\xa2"          },
                { L_,  "\xf2\x80\xbf\xbf",       "\xf2\x80\xbf\xbf"          },
                { L_,  "\xf2\x81\x80\x80",       "\xf2\x81\x80\x80"          },
                { L_,  "\xf2\xa2\x80\x85",       "\xf2\xa2\x80\x85"          },
                { L_,  "\xf2\x85\x80\xbf",       "\xf2\x85\x80\xbf"          },
                { L_,  "\xf2\xa9\xa3\x80",       "\xf2\xa9\xa3\x80"          },
                { L_,  "\xf2\x93\xa3\xa6",       "\xf2\x93\xa3\xa6"          },
                { L_,  "\xf2\x85\xa3\xbf",       "\xf2\x85\xa3\xbf"          },
                { L_,  "\xf2\x98\xbf\x80",       "\xf2\x98\xbf\x80"          },
                { L_,  "\xf2\x9d\xbf\xa6",       "\xf2\x9d\xbf\xa6"          },
                { L_,  "\xf2\x9f\xbf\xbf",       "\xf2\x9f\xbf\xbf"          },
                { L_,  "\xf2\xbf\x80\x80",       "\xf2\xbf\x80\x80"          },
                { L_,  "\xf2\xbf\x80\xa6",       "\xf2\xbf\x80\xa6"          },
                { L_,  "\xf2\xbf\x80\xbf",       "\xf2\xbf\x80\xbf"          },
                { L_,  "\xf2\xbf\xa3\x80",       "\xf2\xbf\xa3\x80"          },
                { L_,  "\xf2\xbf\xa3\xa6",       "\xf2\xbf\xa3\xa6"          },
                { L_,  "\xf2\xbf\xa3\xbf",       "\xf2\xbf\xa3\xbf"          },
                { L_,  "\xf2\xbf\xbf\x80",       "\xf2\xbf\xbf\x80"          },
                { L_,  "\xf2\xbf\xbf\xa6",       "\xf2\xbf\xbf\xa6"          },
                { L_,  "\xf2\xbf\xbf\xbf",       "\xf2\xbf\xbf\xbf"          },

                { L_,  "\xf3\x80\x80\x80",       "\xf3\x80\x80\x80"          },
                { L_,  "\xf3\x80\x80\x98",       "\xf3\x80\x80\x98"          },
                { L_,  "\xf3\x80\x80\xbf",       "\xf3\x80\x80\xbf"          },
                { L_,  "\xf3\x80\xa7\x80",       "\xf3\x80\xa7\x80"          },
                { L_,  "\xf3\x80\x95\xa6",       "\xf3\x80\x95\xa6"          },
                { L_,  "\xf3\x80\xa5\xbf",       "\xf3\x80\xa5\xbf"          },
                { L_,  "\xf3\x80\xbf\x80",       "\xf3\x80\xbf\x80"          },
                { L_,  "\xf3\x80\xbf\xa2",       "\xf3\x80\xbf\xa2"          },
                { L_,  "\xf3\x80\xbf\xbf",       "\xf3\x80\xbf\xbf"          },
                { L_,  "\xf3\x81\x80\x80",       "\xf3\x81\x80\x80"          },
                { L_,  "\xf3\xa2\x80\x85",       "\xf3\xa2\x80\x85"          },
                { L_,  "\xf3\x85\x80\xbf",       "\xf3\x85\x80\xbf"          },
                { L_,  "\xf3\xa9\xa3\x80",       "\xf3\xa9\xa3\x80"          },
                { L_,  "\xf3\x93\xa3\xa6",       "\xf3\x93\xa3\xa6"          },
                { L_,  "\xf3\x85\xa3\xbf",       "\xf3\x85\xa3\xbf"          },
                { L_,  "\xf3\x98\xbf\x80",       "\xf3\x98\xbf\x80"          },
                { L_,  "\xf3\x9d\xbf\xa6",       "\xf3\x9d\xbf\xa6"          },
                { L_,  "\xf3\x9f\xbf\xbf",       "\xf3\x9f\xbf\xbf"          },
                { L_,  "\xf3\xbf\x80\x80",       "\xf3\xbf\x80\x80"          },
                { L_,  "\xf3\xbf\x80\xa6",       "\xf3\xbf\x80\xa6"          },
                { L_,  "\xf3\xbf\x80\xbf",       "\xf3\xbf\x80\xbf"          },
                { L_,  "\xf3\xbf\xa3\x80",       "\xf3\xbf\xa3\x80"          },
                { L_,  "\xf3\xbf\xa3\xa6",       "\xf3\xbf\xa3\xa6"          },
                { L_,  "\xf3\xbf\xa3\xbf",       "\xf3\xbf\xa3\xbf"          },
                { L_,  "\xf3\xbf\xbf\x80",       "\xf3\xbf\xbf\x80"          },
                { L_,  "\xf3\xbf\xbf\xa6",       "\xf3\xbf\xbf\xa6"          },
                { L_,  "\xf3\xbf\xbf\xbf",       "\xf3\xbf\xbf\xbf"          },

                { L_,  "\xf4\x80\x80\x80",       "\xf4\x80\x80\x80"          },
                { L_,  "\xf4\x80\x80\x98",       "\xf4\x80\x80\x98"          },
                { L_,  "\xf4\x80\x80\xbf",       "\xf4\x80\x80\xbf"          },
                { L_,  "\xf4\x80\xa7\x80",       "\xf4\x80\xa7\x80"          },
                { L_,  "\xf4\x80\x95\xa6",       "\xf4\x80\x95\xa6"          },
                { L_,  "\xf4\x80\xa5\xbf",       "\xf4\x80\xa5\xbf"          },
                { L_,  "\xf4\x80\xbf\x80",       "\xf4\x80\xbf\x80"          },
                { L_,  "\xf4\x80\xbf\xa2",       "\xf4\x80\xbf\xa2"          },
                { L_,  "\xf4\x80\xbf\xbf",       "\xf4\x80\xbf\xbf"          },
                { L_,  "\xf4\x81\x80\x80",       "\xf4\x81\x80\x80"          },
                { L_,  "\xf4\x82\x80\x85",       "\xf4\x82\x80\x85"          },
                { L_,  "\xf4\x85\x80\xbf",       "\xf4\x85\x80\xbf"          },
                { L_,  "\xf4\x89\xa3\x80",       "\xf4\x89\xa3\x80"          },
                { L_,  "\xf4\x83\xa3\xa6",       "\xf4\x83\xa3\xa6"          },
                { L_,  "\xf4\x85\xa3\xbf",       "\xf4\x85\xa3\xbf"          },
                { L_,  "\xf4\x88\xbf\x80",       "\xf4\x88\xbf\x80"          },
                { L_,  "\xf4\x8d\xbf\xa6",       "\xf4\x8d\xbf\xa6"          },
                { L_,  "\xf4\x8e\xbf\xbf",       "\xf4\x8e\xbf\xbf"          },
                { L_,  "\xf4\x8f\x80\x80",       "\xf4\x8f\x80\x80"          },
                { L_,  "\xf4\x8f\x80\xa6",       "\xf4\x8f\x80\xa6"          },
                { L_,  "\xf4\x8f\x80\xbf",       "\xf4\x8f\x80\xbf"          },
                { L_,  "\xf4\x8f\xa3\x80",       "\xf4\x8f\xa3\x80"          },
                { L_,  "\xf4\x8f\xa3\xa6",       "\xf4\x8f\xa3\xa6"          },
                { L_,  "\xf4\x8f\xa3\xbf",       "\xf4\x8f\xa3\xbf"          },
                { L_,  "\xf4\x8f\xbf\x80",       "\xf4\x8f\xbf\x80"          },
                { L_,  "\xf4\x8f\xbf\xa6",       "\xf4\x8f\xbf\xa6"          },
                { L_,  "\xf4\x8f\xbf\xbf",       "\xf4\x8f\xbf\xbf"          },

            };
            const int NUM_DATA = sizeof VALID_DATA / sizeof *VALID_DATA;

            // Use orthogonal perturbations for making sure that printing is
            // correct regardless of position in the string: Use header,
            // trailer, or both.

            bsl::string mHEADER("HeAdEr");
            bsl::string mTRAILER("TrAiLeR");

            const bsl::string& HEADER = mHEADER;
            const bsl::string& TRAILER = mTRAILER;

            if (verbose) cout << "\nUsing 'char*' on valid strings." << endl;
            {
                typedef const char* Type;

                for (int i = 0; i < NUM_DATA; ++i) {
                    const int   LINE   = VALID_DATA[i].d_lineNum;
                    const Type  INPUT  = VALID_DATA[i].d_input;
                    const char *RESULT = VALID_DATA[i].d_result;

                    bsl::stringstream ss;
                    Util::printText(ss, INPUT);
                    ASSERTV(LINE, RESULT, ss.str(), RESULT == ss.str());
                    ASSERTV(LINE, ss.good());

                    char mINPUT2[64];             const char *INPUT2 = mINPUT2;
                    bsl::strcpy(mINPUT2, HEADER.c_str());
                    bsl::strcat(mINPUT2, INPUT);
                    char mRESULT2[64];          const char *RESULT2 = mRESULT2;
                    bsl::strcpy(mRESULT2, HEADER.c_str());
                    bsl::strcat(mRESULT2, RESULT);
                    ss.str("");
                    Util::printText(ss, INPUT2);
                    ASSERTV(LINE, RESULT2, ss.str(), RESULT2 == ss.str());
                    ASSERTV(LINE, ss.good());

                    char mINPUT3[64];             const char *INPUT3 = mINPUT3;
                    bsl::strcpy(mINPUT3, INPUT);
                    bsl::strcat(mINPUT3, TRAILER.c_str());
                    char mRESULT3[64];          const char *RESULT3 = mRESULT3;
                    bsl::strcpy(mRESULT3, RESULT);
                    bsl::strcat(mRESULT3, TRAILER.c_str());
                    ss.str("");
                    Util::printText(ss, INPUT3);
                    ASSERTV(LINE, RESULT3, ss.str(), RESULT3 == ss.str());
                    ASSERTV(LINE, ss.good());

                    char mINPUT4[64];             const char *INPUT4 = mINPUT4;
                    bsl::strcpy(mINPUT4, HEADER.c_str());
                    bsl::strcat(mINPUT4, INPUT);
                    bsl::strcat(mINPUT4, TRAILER.c_str());
                    char mRESULT4[64];          const char *RESULT4 = mRESULT4;
                    bsl::strcpy(mRESULT4, HEADER.c_str());
                    bsl::strcat(mRESULT4, RESULT);
                    bsl::strcat(mRESULT4, TRAILER.c_str());
                    ss.str("");
                    Util::printText(ss, INPUT4);
                    ASSERTV(LINE, RESULT4, ss.str(), RESULT4 == ss.str());
                    ASSERTV(LINE, ss.good());
                }
            }

            if (verbose)
                cout << "\nUsing 'bsl::string' on valid strings." << endl;
            {
                typedef bsl::string Type;

                for (int i = 0; i < NUM_DATA; ++i) {
                    const int  LINE   = VALID_DATA[i].d_lineNum;
                    const Type INPUT  = VALID_DATA[i].d_input;
                    const Type RESULT = VALID_DATA[i].d_result;

                    bsl::stringstream ss;
                    Util::printText(ss, INPUT);
                    ASSERTV(LINE, RESULT, ss.str(), RESULT == ss.str());
                    ASSERTV(LINE, ss.good());

                    const Type INPUT2 = HEADER + INPUT;
                    const Type RESULT2 = HEADER + RESULT;
                    ss.str("");
                    Util::printText(ss, INPUT2);
                    ASSERTV(LINE, RESULT2, ss.str(), RESULT2 == ss.str());
                    ASSERTV(LINE, ss.good());

                    const Type INPUT3 = INPUT + TRAILER;
                    const Type RESULT3 = RESULT + TRAILER;
                    ss.str("");
                    Util::printText(ss, INPUT3);
                    ASSERTV(LINE, RESULT3, ss.str(), RESULT3 == ss.str());
                    ASSERTV(LINE, ss.good());

                    const Type INPUT4 = HEADER + INPUT + TRAILER;
                    const Type RESULT4 = HEADER + RESULT + TRAILER;
                    ss.str("");
                    Util::printText(ss, INPUT4);
                    ASSERTV(LINE, RESULT4, ss.str(), RESULT4 == ss.str());
                    ASSERTV(LINE, ss.good());
                }
            }

            if (verbose)
                cout << "\nUsing 'bslstl::StringRef' on valid strings.\n";
            {
                typedef bslstl::StringRef Type;

                for (int i = 0; i < NUM_DATA; ++i) {
                    const int  LINE   = VALID_DATA[i].d_lineNum;
                    const Type INPUT  = VALID_DATA[i].d_input;
                    const Type RESULT = VALID_DATA[i].d_result;

                    bsl::stringstream ss;
                    Util::printText(ss, INPUT);
                    ASSERTV(LINE, RESULT, ss.str(), RESULT == ss.str());
                    ASSERTV(LINE, ss.good());

                    const bsl::string INPUT2_STR = HEADER + INPUT;
                    const bsl::string RESULT2_STR = HEADER + RESULT;
                    const Type INPUT2 = INPUT2_STR;
                    const Type RESULT2 = RESULT2_STR;
                    ss.str("");
                    Util::printText(ss, INPUT2);
                    ASSERTV(LINE, RESULT2, ss.str(), RESULT2 == ss.str());
                    ASSERTV(LINE, ss.good());

                    const bsl::string INPUT3_STR = INPUT + TRAILER;
                    const bsl::string RESULT3_STR = RESULT + TRAILER;
                    const Type INPUT3 = INPUT3_STR;
                    const Type RESULT3 = RESULT3_STR;
                    ss.str("");
                    Util::printText(ss, INPUT3);
                    ASSERTV(LINE, RESULT3, ss.str(), RESULT3 == ss.str());
                    ASSERTV(LINE, ss.good());

                    const bsl::string INPUT4_STR = HEADER + INPUT + TRAILER;
                    const bsl::string RESULT4_STR = HEADER + RESULT + TRAILER;
                    const Type INPUT4 = INPUT4_STR;
                    const Type RESULT4 = RESULT4_STR;
                    ss.str("");
                    Util::printText(ss, INPUT4);
                    ASSERTV(LINE, RESULT4, ss.str(), RESULT4 == ss.str());
                    ASSERTV(LINE, ss.good());
                }
            }

            if (verbose)
                cout << "\nUsing 'MyStringRef' on valid strings.\n";
            {
                typedef MyStringRef Type;

                for (int i = 0; i < NUM_DATA; ++i) {
                    const int  LINE   = VALID_DATA[i].d_lineNum;
                    const Type INPUT  = VALID_DATA[i].d_input;
                    const Type RESULT = VALID_DATA[i].d_result;

                    bsl::stringstream ss;
                    Util::printText(ss, INPUT);
                    ASSERTV(LINE, RESULT, ss.str(), RESULT == ss.str());
                    ASSERTV(LINE, ss.good());

                    const bsl::string INPUT2_STR = HEADER + INPUT;
                    const bsl::string RESULT2_STR = HEADER + RESULT;
                    const Type INPUT2 = INPUT2_STR;
                    const Type RESULT2 = RESULT2_STR;
                    ss.str("");
                    Util::printText(ss, INPUT2);
                    ASSERTV(LINE, RESULT2, ss.str(), RESULT2 == ss.str());
                    ASSERTV(LINE, ss.good());

                    const bsl::string INPUT3_STR = INPUT + TRAILER;
                    const bsl::string RESULT3_STR = RESULT + TRAILER;
                    const Type INPUT3 = INPUT3_STR;
                    const Type RESULT3 = RESULT3_STR;
                    ss.str("");
                    Util::printText(ss, INPUT3);
                    ASSERTV(LINE, RESULT3, ss.str(), RESULT3 == ss.str());
                    ASSERTV(LINE, ss.good());

                    const bsl::string INPUT4_STR = HEADER + INPUT + TRAILER;
                    const bsl::string RESULT4_STR = HEADER + RESULT + TRAILER;
                    const Type INPUT4 = INPUT4_STR;
                    const Type RESULT4 = RESULT4_STR;
                    ss.str("");
                    Util::printText(ss, INPUT4);
                    ASSERTV(LINE, RESULT4, ss.str(), RESULT4 == ss.str());
                    ASSERTV(LINE, ss.good());
                }
            }

            if (verbose)
                cout << "\nUsing 'vector<char>' on valid strings." << endl;
            {
                typedef bsl::vector<char> Type;

                for (int i = 0; i < NUM_DATA; ++i) {
                    const bsl::size_t  LENGTH =
                                            bsl::strlen(VALID_DATA[i].d_input);
                    const int          LINE   = VALID_DATA[i].d_lineNum;
                    const char        *CINPUT = VALID_DATA[i].d_input;
                    const Type         INPUT(CINPUT, CINPUT + LENGTH);
                    const bsl::string  RESULT = VALID_DATA[i].d_result;

                    bsl::stringstream ss;
                    Util::printText(ss, INPUT);
                    ASSERTV(LINE, RESULT, ss.str(), RESULT == ss.str());
                    ASSERTV(LINE, ss.good());

                    Type mINPUT2;                 const Type& INPUT2 = mINPUT2;
                    mINPUT2.insert(mINPUT2.end(),
                                   mHEADER.begin(), mHEADER.end());
                    mINPUT2.insert(mINPUT2.end(), INPUT.begin(), INPUT.end());
                    const bsl::string RESULT2 = HEADER + RESULT;
                    ss.str("");
                    Util::printText(ss, INPUT2);
                    ASSERTV(LINE, RESULT2, ss.str(), RESULT2 == ss.str());
                    ASSERTV(LINE, ss.good());

                    Type mINPUT3;                 const Type& INPUT3 = mINPUT3;
                    mINPUT3.insert(mINPUT3.end(), INPUT.begin(), INPUT.end());
                    mINPUT3.insert(mINPUT3.end(),
                                   mTRAILER.begin(), mTRAILER.end());
                    const bsl::string RESULT3 = RESULT + TRAILER;
                    ss.str("");
                    Util::printText(ss, INPUT3);
                    ASSERTV(LINE, RESULT3, ss.str(), RESULT3 == ss.str());
                    ASSERTV(LINE, ss.good());

                    Type mINPUT4;                 const Type& INPUT4 = mINPUT4;
                    mINPUT4.insert(mINPUT4.end(),
                                   mHEADER.begin(), mHEADER.end());
                    mINPUT4.insert(mINPUT4.end(), INPUT.begin(), INPUT.end());
                    mINPUT4.insert(mINPUT4.end(),
                                   mTRAILER.begin(), mTRAILER.end());
                    const bsl::string RESULT4 = HEADER + RESULT + TRAILER;
                    ss.str("");
                    Util::printText(ss, INPUT4);
                    ASSERTV(LINE, RESULT4, ss.str(), RESULT4 == ss.str());
                    ASSERTV(LINE, ss.good());
                }
            }

            if (verbose)
              cout << "\nUsing 'CustomizedString' on valid strings." << endl;
            {
                typedef test::CustomizedString Type;

                for (int i = 0; i < NUM_DATA; ++i) {
                    const int          LINE  = VALID_DATA[i].d_lineNum;
                    const Type         INPUT (
                                          bsl::string(VALID_DATA[i].d_input) );
                    const  bsl::string RESULT = VALID_DATA[i].d_result;

                    bsl::stringstream ss;
                    Util::printText(ss, INPUT);
                    ASSERTV(LINE, RESULT, ss.str(), RESULT == ss.str());
                    ASSERTV(LINE, ss.good());

                    const bsl::string pre2 = HEADER + INPUT.toString();
                    if (25 < pre2.length()) {
                        continue;
                    }
                    const Type INPUT2 ( pre2 );
                    bsl::string RESULT2 = HEADER + RESULT;
                    ss.str("");
                    Util::printText(ss, INPUT2);
                    ASSERTV(LINE, RESULT2, ss.str(), RESULT2 == ss.str());
                    ASSERTV(LINE, ss.good());

                    const bsl::string pre3 = INPUT.toString() + TRAILER;
                    if (25 < pre3.length()) {
                        continue;
                    }
                    const Type INPUT3(pre3);
                    bsl::string RESULT3 = RESULT + TRAILER;
                    ss.str("");
                    Util::printText(ss, INPUT3);
                    ASSERTV(LINE, RESULT3, ss.str(), RESULT3 == ss.str());
                    ASSERTV(LINE, ss.good());

                    const bsl::string pre4 = HEADER+INPUT.toString()+TRAILER;
                    if (25 < pre4.length()) {
                        continue;
                    }
                    const Type INPUT4 (pre4);
                    bsl::string RESULT4 = HEADER + RESULT + TRAILER;
                    ss.str("");
                    Util::printText(ss, INPUT4);
                    ASSERTV(LINE, RESULT4, ss.str(), RESULT4 == ss.str());
                    ASSERTV(LINE, ss.good());
                }
            }
        }

        // Note that we have already tested the invalid single byte characters
        // individually, so the only concern here are 1, multibyte characters,
        // including ill-terminated strings in the middle of a multibyte
        // character, and 2, getting the portion of the string prior to the
        // first invalid character correctly.

        {
            static const struct {
                int         d_lineNum;
                const char *d_input;
            } INVALID_DATA[] = {
                //line input
                //---- -----
                { L_,  "\x01",                                               },
                { L_,  "\x02",                                               },
                { L_,  "\x03",                                               },
                { L_,  "\x04",                                               },
                { L_,  "\x04",                                               },
                { L_,  "\x05",                                               },
                { L_,  "\x06",                                               },
                { L_,  "\x07",                                               },
                { L_,  "\x08",                                               },
                // Skip TAB (0x09) and LF (0x0a)
                { L_,  "\x0b",                                               },
                { L_,  "\x0c",                                               },
                // Skip CR (0x0d)
                { L_,  "\x0e",                                               },
                { L_,  "\x0f",                                               },
                { L_,  "\x11",                                               },
                { L_,  "\x12",                                               },
                { L_,  "\x13",                                               },
                { L_,  "\x14",                                               },
                { L_,  "\x14",                                               },
                { L_,  "\x15",                                               },
                { L_,  "\x16",                                               },
                { L_,  "\x17",                                               },
                { L_,  "\x18",                                               },
                { L_,  "\x19",                                               },
                { L_,  "\x1a",                                               },
                { L_,  "\x1b",                                               },
                { L_,  "\x1c",                                               },
                { L_,  "\x1d",                                               },
                { L_,  "\x1e",                                               },
                { L_,  "\x1f",                                               },
                { L_,  "\x7f",                                               },

                // Invalid first byte.  Interpret either as two-byte,
                // three-byte, or four-byte sequence.

                { L_,  "\x80",                                               },

                { L_,  "\x80\x80",                                           },
                { L_,  "\x80\xa3",                                           },
                { L_,  "\x80\xbf",                                           },

                { L_,  "\x80\xa0\x80",                                       },
                { L_,  "\x80\xa0\xa3",                                       },
                { L_,  "\x80\xa0\xbf",                                       },
                { L_,  "\x80\xb5\x80",                                       },
                { L_,  "\x80\xab\x9d",                                       },
                { L_,  "\x80\x9a\xbf",                                       },
                { L_,  "\x80\xbf\x80",                                       },
                { L_,  "\x80\xbf\xaf",                                       },
                { L_,  "\x80\xbf\xbf",                                       },

                { L_,  "\x80\x80\x80\x80",                                   },
                { L_,  "\x80\x80\x80\x98",                                   },
                { L_,  "\x80\x80\x80\xbf",                                   },
                { L_,  "\x80\x80\xa7\x80",                                   },
                { L_,  "\x80\x80\x95\xa6",                                   },
                { L_,  "\x80\x80\xa5\xbf",                                   },
                { L_,  "\x80\x80\xbf\x80",                                   },
                { L_,  "\x80\x80\xbf\xa2",                                   },
                { L_,  "\x80\x80\xbf\xbf",                                   },
                { L_,  "\x80\x81\x80\x80",                                   },
                { L_,  "\x80\xa2\x80\x85",                                   },
                { L_,  "\x80\x85\x80\xbf",                                   },
                { L_,  "\x80\xa9\xa3\x80",                                   },
                { L_,  "\x80\x93\xa3\xa6",                                   },
                { L_,  "\x80\x85\xa3\xbf",                                   },
                { L_,  "\x80\x98\xbf\x80",                                   },
                { L_,  "\x80\x9d\xbf\xa6",                                   },
                { L_,  "\x80\x9f\xbf\xbf",                                   },
                { L_,  "\x80\xbf\x80\x80",                                   },
                { L_,  "\x80\xbf\x80\xa6",                                   },
                { L_,  "\x80\xbf\x80\xbf",                                   },
                { L_,  "\x80\xbf\xa3\x80",                                   },
                { L_,  "\x80\xbf\xa3\xa6",                                   },
                { L_,  "\x80\xbf\xa3\xbf",                                   },
                { L_,  "\x80\xbf\xbf\x80",                                   },
                { L_,  "\x80\xbf\xbf\xa6",                                   },
                { L_,  "\x80\xbf\xbf\xbf",                                   },

                // For boundary and area testing, try also different first
                // bytes, but do not be as thorough about the next bytes for
                // the random value in area, simply try boundaries.

                { L_,  "\x9f",                                               },
                { L_,  "\x9f\x80",                                           },
                { L_,  "\x9f\xbf",                                           },

                { L_,  "\xc1",                                               },
                { L_,  "\xc1\x80",                                           },
                { L_,  "\xc1\xbf",                                           },
                { L_,  "\xc1\xa0\x80",                                       },
                { L_,  "\xc1\xa0\xbf",                                       },
                { L_,  "\xc1\xbf\x80",                                       },
                { L_,  "\xc1\xbf\xbf",                                       },
                { L_,  "\xc1\x80\x80\x80",                                   },
                { L_,  "\xc1\x80\x80\xbf",                                   },
                { L_,  "\xc1\x80\xbf\x80",                                   },
                { L_,  "\xc1\x80\xbf\xbf",                                   },
                { L_,  "\xc1\xbf\x80\x80",                                   },
                { L_,  "\xc1\xbf\x80\xbf",                                   },
                { L_,  "\xc1\xbf\xbf\x80",                                   },
                { L_,  "\xc1\xbf\xbf\xbf",                                   },

                // Invalid two-byte character sequences.  Even though printing
                // should fail on first byte of sequence, we insert valid and
                // invalid second bytes for control.

                // Valid first byte but invalid second byte.

                { L_,  "\xc2",                                               },
                { L_,  "\xc2\x00",                                           },
                { L_,  "\xc2\x7f",                                           },
                { L_,  "\xc2\xc0",                                           },
                { L_,  "\xc2\xd5",                                           },
                { L_,  "\xc2\xff",                                           },

                { L_,  "\xc7",                                               },
                { L_,  "\xc7\x00",                                           },
                { L_,  "\xc7\x7f",                                           },
                { L_,  "\xc7\xc0",                                           },
                { L_,  "\xc7\xd5",                                           },
                { L_,  "\xc7\xff",                                           },

                { L_,  "\xdf",                                               },
                { L_,  "\xdf\x00",                                           },
                { L_,  "\xdf\x7f",                                           },
                { L_,  "\xdf\xc0",                                           },
                { L_,  "\xdf\xd5",                                           },
                { L_,  "\xdf\xff",                                           },

                // Invalid three-byte character sequences.  Even though
                // printing should fail on first byte of sequence, we insert
                // valid and invalid second bytes for control.

                // Valid first byte, but invalid second byte.

                       // equal to 0xe0
                { L_,  "\xe0\x00",                                           },
                { L_,  "\xe0\x35",                                           },
                { L_,  "\xe0\x9f",                                           },
                { L_,  "\xe0\x9f\x00",                                       },
                { L_,  "\xe0\x9f\x21",                                       },
                { L_,  "\xe0\x9f\x80",                                       },
                { L_,  "\xe0\x9f\x9d",                                       },
                { L_,  "\xe0\x9f\xbf",                                       },
                { L_,  "\xe0\x9f\xff",                                       },
                { L_,  "\xe0\xc0",                                           },
                { L_,  "\xe0\xc0\x00",                                       },
                { L_,  "\xe0\xc0\x21",                                       },
                { L_,  "\xe0\xc0\x80",                                       },
                { L_,  "\xe0\xc0\x9d",                                       },
                { L_,  "\xe0\xc0\xbf",                                       },
                { L_,  "\xe0\xc0\xff",                                       },
                { L_,  "\xe0\xd5",                                           },
                { L_,  "\xe0\xff",                                           },

                       // in range 0xe1..0xec or 0xee..0xef
                { L_,  "\xe1\x00",                                           },
                { L_,  "\xe2\x35",                                           },
                { L_,  "\xe3\x7f",                                           },
                { L_,  "\xe4\x7f\x00",                                       },
                { L_,  "\xe5\x7f\x21",                                       },
                { L_,  "\xe6\x7f\x80",                                       },
                { L_,  "\xe7\x7f\x9d",                                       },
                { L_,  "\xe8\x7f\xbf",                                       },
                { L_,  "\xe9\x7f\xff",                                       },
                { L_,  "\xea\xc0",                                           },
                { L_,  "\xeb\xc0\x00",                                       },
                { L_,  "\xec\xc0\x21",                                       },
                { L_,  "\xe1\xc0\x80",                                       },
                { L_,  "\xe2\xc0\x9d",                                       },
                { L_,  "\xee\xc0\xbf",                                       },
                { L_,  "\xef\xc0\xff",                                       },
                { L_,  "\xee\xd5",                                           },
                { L_,  "\xef\xff",                                           },

                       // equal to 0xe0
                { L_,  "\xed\x00",                                           },
                { L_,  "\xed\x35",                                           },
                { L_,  "\xed\x7f",                                           },
                { L_,  "\xed\x7f\x00",                                       },
                { L_,  "\xed\x7f\x21",                                       },
                { L_,  "\xed\x7f\x80",                                       },
                { L_,  "\xed\x7f\x9d",                                       },
                { L_,  "\xed\x7f\xbf",                                       },
                { L_,  "\xed\x7f\xff",                                       },
                { L_,  "\xed\xa0",                                           },
                { L_,  "\xed\xa0\x00",                                       },
                { L_,  "\xed\xa0\x21",                                       },
                { L_,  "\xed\xa0\x80",                                       },
                { L_,  "\xed\xa0\x9d",                                       },
                { L_,  "\xed\xa0\xbf",                                       },
                { L_,  "\xed\xa0\xff",                                       },
                { L_,  "\xed\xd5",                                           },
                { L_,  "\xed\xff",                                           },

                // Valid first and second bytes, but invalid third byte.

                       // equal to 0xe0
                { L_,  "\xe0\xa0\x00",                                       },
                { L_,  "\xe0\xa0\x21",                                       },
                { L_,  "\xe0\xa0\x7f",                                       },
                { L_,  "\xe0\xa0\xc0",                                       },
                { L_,  "\xe0\xa0\xff",                                       },
                { L_,  "\xe0\xbf\x00",                                       },
                { L_,  "\xe0\xbf\x21",                                       },
                { L_,  "\xe0\xbf\x7f",                                       },
                { L_,  "\xe0\xbf\xc0",                                       },
                { L_,  "\xe0\xbf\xff",                                       },

                       // in range 0xe1..0xec or 0xee..0xef
                { L_,  "\xe1\x80\x00",                                       },
                { L_,  "\xe2\x80\x21",                                       },
                { L_,  "\xe3\x80\x7f",                                       },
                { L_,  "\xe8\x80\xc0",                                       },
                { L_,  "\xe9\x80\xff",                                       },
                { L_,  "\xea\xbf\x00",                                       },
                { L_,  "\xeb\xbf\x21",                                       },
                { L_,  "\xec\xbf\x7f",                                       },
                { L_,  "\xee\xbf\xc0",                                       },
                { L_,  "\xef\xbf\xff",                                       },

                       // equal to 0xed
                { L_,  "\xed\xa0\x00",                                       },
                { L_,  "\xed\xa0\x21",                                       },
                { L_,  "\xed\xa0\x7f",                                       },
                { L_,  "\xed\xa0\xc0",                                       },
                { L_,  "\xed\xa0\xff",                                       },
                { L_,  "\xed\xbf\x00",                                       },
                { L_,  "\xed\xbf\x21",                                       },
                { L_,  "\xed\xbf\x7f",                                       },
                { L_,  "\xed\xbf\xc0",                                       },
                { L_,  "\xed\xbf\xff",                                       },

            };
            const int NUM_DATA = sizeof INVALID_DATA / sizeof *INVALID_DATA;

            // Use orthogonal perturbations for making sure that printing is
            // correct regardless of position in the string: use header,
            // trailer, or both.

            bsl::string mHEADER("HeAdEr");
            bsl::string mTRAILER("TrAiLeR");

            const bsl::string& HEADER = mHEADER;
            const bsl::string& TRAILER = mTRAILER;

            if (verbose) cout << "\nUsing 'char*' on invalid strings." << endl;
            {
                typedef const char* Type;

                for (int i = 0; i < NUM_DATA; ++i) {
                    const int   LINE   = INVALID_DATA[i].d_lineNum;
                    const Type  INPUT  = INVALID_DATA[i].d_input;
                    const char *RESULT = "";

                    bsl::stringstream ss;
                    Util::printText(ss, INPUT);
                    ASSERTV(LINE, RESULT, ss.str(), RESULT == ss.str());
                    ASSERTV(LINE, ss.fail());

                    char mINPUT2[64];             const char *INPUT2 = mINPUT2;
                    bsl::strcpy(mINPUT2, HEADER.c_str());
                    bsl::strcat(mINPUT2, INPUT);
                    char mRESULT2[64];          const char *RESULT2 = mRESULT2;
                    bsl::strcpy(mRESULT2, HEADER.c_str());
                    ss.clear(); ASSERT(ss.good()); ss.str("");
                    Util::printText(ss, INPUT2);
                    ASSERTV(LINE, RESULT2, ss.str(), RESULT2 == ss.str());
                    ASSERTV(LINE, ss.fail());

                    char mINPUT3[64];             const char *INPUT3 = mINPUT3;
                    bsl::strcpy(mINPUT3, INPUT);
                    bsl::strcat(mINPUT3, TRAILER.c_str());
                    char mRESULT3[64] = "";     const char *RESULT3 = mRESULT3;
                    ss.clear(); ASSERT(ss.good()); ss.str("");
                    Util::printText(ss, INPUT3);
                    ASSERTV(LINE, RESULT3, ss.str(), RESULT3 == ss.str());
                    ASSERTV(LINE, ss.fail());

                    char mINPUT4[64];             const char *INPUT4 = mINPUT4;
                    bsl::strcpy(mINPUT4, HEADER.c_str());
                    bsl::strcat(mINPUT4, INPUT);
                    bsl::strcat(mINPUT4, TRAILER.c_str());
                    char mRESULT4[64];          const char *RESULT4 = mRESULT4;
                    bsl::strcpy(mRESULT4, HEADER.c_str());
                    ss.clear(); ASSERT(ss.good()); ss.str("");
                    Util::printText(ss, INPUT4);
                    ASSERTV(LINE, RESULT4, ss.str(), RESULT4 == ss.str());
                    ASSERTV(LINE, ss.fail());
                }
            }

            if (verbose)
                cout << "\nUsing 'bsl::string' on invalid strings." << endl;
            {
                typedef bsl::string Type;

                for (int i = 0; i < NUM_DATA; ++i) {
                    const int   LINE   = INVALID_DATA[i].d_lineNum;
                    const Type  INPUT  = INVALID_DATA[i].d_input;
                    const char *RESULT = "";

                    bsl::stringstream ss;
                    Util::printText(ss, INPUT);
                    ASSERTV(LINE, RESULT, ss.str(), RESULT == ss.str());
                    ASSERTV(LINE, ss.fail());

                    const Type INPUT2 = HEADER + INPUT;
                    const Type RESULT2 = HEADER + Type(RESULT);
                    ss.clear(); ASSERT(ss.good()); ss.str("");
                    Util::printText(ss, INPUT2);
                    ASSERTV(LINE, RESULT2, ss.str(), RESULT2 == ss.str());
                    ASSERTV(LINE, ss.fail());

                    const Type INPUT3 = INPUT + TRAILER;
                    const Type RESULT3 = RESULT;
                    ss.clear(); ASSERT(ss.good()); ss.str("");
                    Util::printText(ss, INPUT3);
                    ASSERTV(LINE, RESULT3, ss.str(), RESULT3 == ss.str());
                    ASSERTV(LINE, ss.fail());

                    const Type INPUT4 = HEADER + INPUT + TRAILER;
                    const Type RESULT4 = HEADER + Type(RESULT);
                    ss.clear(); ASSERT(ss.good()); ss.str("");
                    Util::printText(ss, INPUT4);
                    ASSERTV(LINE, RESULT4, ss.str(), RESULT4 == ss.str());
                    ASSERTV(LINE, ss.fail());
                }
            }

            if (verbose)
                cout << "\nUsing 'bslstl::StringRef' on invalid strings."
                     << endl;
            {
                typedef bslstl::StringRef Type;

                for (int i = 0; i < NUM_DATA; ++i) {
                    const int   LINE   = INVALID_DATA[i].d_lineNum;
                    const Type  INPUT  = INVALID_DATA[i].d_input;
                    const char *RESULT = "";

                    bsl::stringstream ss;
                    Util::printText(ss, INPUT);
                    ASSERTV(LINE, RESULT, ss.str(), RESULT == ss.str());
                    ASSERTV(LINE, ss.fail());

                    const bsl::string INPUT2_STR = HEADER + INPUT;
                    const bsl::string RESULT2_STR = HEADER + RESULT;
                    const Type INPUT2 = INPUT2_STR;
                    const Type RESULT2 = RESULT2_STR;
                    ss.clear(); ASSERT(ss.good()); ss.str("");
                    Util::printText(ss, INPUT2);
                    ASSERTV(LINE, RESULT2, ss.str(), RESULT2 == ss.str());
                    ASSERTV(LINE, ss.fail());

                    const bsl::string INPUT3_STR = INPUT + TRAILER;
                    const Type INPUT3 = INPUT3_STR;
                    const Type RESULT3 = RESULT;
                    ss.clear(); ASSERT(ss.good()); ss.str("");
                    Util::printText(ss, INPUT3);
                    ASSERTV(LINE, RESULT3, ss.str(), RESULT3 == ss.str());
                    ASSERTV(LINE, ss.fail());

                    const bsl::string INPUT4_STR = HEADER + INPUT + TRAILER;
                    const bsl::string RESULT4_STR = HEADER + RESULT;
                    const Type INPUT4 = INPUT4_STR;
                    const Type RESULT4 = RESULT4_STR;
                    ss.clear(); ASSERT(ss.good()); ss.str("");
                    Util::printText(ss, INPUT4);
                    ASSERTV(LINE, RESULT4, ss.str(), RESULT4 == ss.str());
                    ASSERTV(LINE, ss.fail());
                }
            }

            if (verbose)
                cout << "\nUsing 'MyStringRef' on invalid strings."
                     << endl;
            {
                typedef MyStringRef Type;

                for (int i = 0; i < NUM_DATA; ++i) {
                    const int   LINE   = INVALID_DATA[i].d_lineNum;
                    const Type  INPUT  = INVALID_DATA[i].d_input;
                    const char *RESULT = "";

                    bsl::stringstream ss;
                    Util::printText(ss, INPUT);
                    ASSERTV(LINE, RESULT, ss.str(), RESULT == ss.str());
                    ASSERTV(LINE, ss.fail());

                    const bsl::string INPUT2_STR = HEADER + INPUT;
                    const bsl::string RESULT2_STR = HEADER + RESULT;
                    const Type INPUT2 = INPUT2_STR;
                    const Type RESULT2 = RESULT2_STR;
                    ss.clear(); ASSERT(ss.good()); ss.str("");
                    Util::printText(ss, INPUT2);
                    ASSERTV(LINE, RESULT2, ss.str(), RESULT2 == ss.str());
                    ASSERTV(LINE, ss.fail());

                    const bsl::string INPUT3_STR = INPUT + TRAILER;
                    const Type INPUT3 = INPUT3_STR;
                    const Type RESULT3 = RESULT;
                    ss.clear(); ASSERT(ss.good()); ss.str("");
                    Util::printText(ss, INPUT3);
                    ASSERTV(LINE, RESULT3, ss.str(), RESULT3 == ss.str());
                    ASSERTV(LINE, ss.fail());

                    const bsl::string INPUT4_STR = HEADER + INPUT + TRAILER;
                    const bsl::string RESULT4_STR = HEADER + RESULT;
                    const Type INPUT4 = INPUT4_STR;
                    const Type RESULT4 = RESULT4_STR;
                    ss.clear(); ASSERT(ss.good()); ss.str("");
                    Util::printText(ss, INPUT4);
                    ASSERTV(LINE, RESULT4, ss.str(), RESULT4 == ss.str());
                    ASSERTV(LINE, ss.fail());
                }
            }

            if (verbose)
                cout << "\nUsing 'vector<char>' on invalid strings." << endl;
            {
                typedef bsl::vector<char> Type;

                for (int i = 0; i < NUM_DATA; ++i) {
                    const bsl::size_t  LENGTH =
                                          bsl::strlen(INVALID_DATA[i].d_input);
                    const int          LINE   = INVALID_DATA[i].d_lineNum;
                    const char        *CINPUT = INVALID_DATA[i].d_input;
                    const Type         INPUT(CINPUT, CINPUT + LENGTH);
                    const bsl::string  RESULT;

                    bsl::stringstream ss;
                    Util::printText(ss, INPUT);
                    ASSERTV(LINE, RESULT, ss.str(), RESULT == ss.str());
                    ASSERTV(LINE, ss.fail());

                    Type mINPUT2;                 const Type& INPUT2 = mINPUT2;
                    mINPUT2.insert(mINPUT2.end(),
                                   mHEADER.begin(), mHEADER.end());
                    mINPUT2.insert(mINPUT2.end(), INPUT.begin(), INPUT.end());
                    const bsl::string RESULT2 = HEADER;
                    ss.clear(); ASSERT(ss.good()); ss.str("");
                    Util::printText(ss, INPUT2);
                    ASSERTV(LINE, RESULT2, ss.str(), RESULT2 == ss.str());
                    ASSERTV(LINE, ss.fail());

                    Type mINPUT3;                 const Type& INPUT3 = mINPUT3;
                    mINPUT3.insert(mINPUT3.end(), INPUT.begin(), INPUT.end());
                    mINPUT3.insert(mINPUT3.end(),
                                   mTRAILER.begin(), mTRAILER.end());
                    const bsl::string RESULT3;
                    ss.clear(); ASSERT(ss.good()); ss.str("");
                    Util::printText(ss, INPUT3);
                    ASSERTV(LINE, RESULT3, ss.str(), RESULT3 == ss.str());
                    ASSERTV(LINE, ss.fail());

                    Type mINPUT4;                 const Type& INPUT4 = mINPUT4;
                    mINPUT4.insert(mINPUT4.end(),
                                   mHEADER.begin(), mHEADER.end());
                    mINPUT4.insert(mINPUT4.end(), INPUT.begin(), INPUT.end());
                    mINPUT4.insert(mINPUT4.end(),
                                   mTRAILER.begin(), mTRAILER.end());
                    const bsl::string RESULT4 = HEADER;
                    ss.clear(); ASSERT(ss.good()); ss.str("");
                    Util::printText(ss, INPUT4);
                    ASSERTV(LINE, RESULT4, ss.str(), RESULT4 == ss.str());
                    ASSERTV(LINE, ss.fail());
                }
            }

#if 0
// It appears to be undefined behavior to feed invalid UTF-8 to a BAS-generated
// type.  See DRQS 156550647.  It wasn't at the time this test was originally
// written.

            if (verbose)
              cout << "\nUsing 'CustomizedString' on invalid strings." << endl;
            {
                typedef test::CustomizedString Type;

                for (int i = 0; i < NUM_DATA; ++i) {
                    const int   LINE   = INVALID_DATA[i].d_lineNum;
                    const Type  INPUT  ( bsl::string(INVALID_DATA[i].d_input));
                    const char *RESULT = "";

                    bsl::stringstream ss;
                    Util::printText(ss, INPUT);
                    ASSERTV(LINE, RESULT, ss.str(), RESULT == ss.str());
                    ASSERTV(LINE, ss.fail());

                    const Type INPUT2 ( HEADER + INPUT.toString() );
                    const char *RESULT2 = HEADER.c_str();
                    ss.clear(); ASSERT(ss.good()); ss.str("");
                    Util::printText(ss, INPUT2);
                    ASSERTV(LINE, RESULT2, ss.str(), RESULT2 == ss.str());
                    ASSERTV(LINE, ss.fail());

                    const Type INPUT3 ( INPUT.toString() + TRAILER );
                    const char *RESULT3 = RESULT;
                    ss.clear(); ASSERT(ss.good()); ss.str("");
                    Util::printText(ss, INPUT3);
                    ASSERTV(LINE, RESULT3, ss.str(), RESULT3 == ss.str());
                    ASSERTV(LINE, ss.fail());

                    const Type INPUT4 ( HEADER + INPUT.toString() + TRAILER );
                    const char *RESULT4 = HEADER.c_str();
                    ss.clear(); ASSERT(ss.good()); ss.str("");
                    Util::printText(ss, INPUT4);
                    ASSERTV(LINE, RESULT4, ss.str(), RESULT4 == ss.str());
                    ASSERTV(LINE, ss.fail());
                }
            }
#endif
        }

        if (verbose) cout << "\nUsing 'MyEnumeration::Value'." << endl;
        {
            typedef test::MyEnumeration::Value Type;

            static const struct {
                int         d_lineNum;
                Type        d_input;
                const char *d_result;
            } DATA[] = {
                //line    input                         result
                //----    -----                         ------
                { L_,     test::MyEnumeration::VALUE1,  "VALUE1"    },
                { L_,     test::MyEnumeration::VALUE2,  "VALUE2"    },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int   LINE   = DATA[i].d_lineNum;
                const Type  INPUT  = DATA[i].d_input;
                const char *RESULT = DATA[i].d_result;

                bsl::stringstream ss;

                Util::printText(ss, INPUT);

                ASSERTV(LINE, RESULT, ss.str(), RESULT == ss.str());
                ASSERTV(LINE, ss.good());
            }
        }
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING 'printList' FUNCTIONS
        //
        // Concerns:
        //
        // Plan:
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTESTING 'printList' FUNCTIONS"
                          << "\n=============================" << endl;

        if (verbose) cout << "\nUsing 'bsl::vector<int>'." << endl;
        {
            typedef int                   ElemType;
            typedef bsl::vector<ElemType> Type;

            static const struct {
                int         d_lineNum;
                ElemType    d_input[5];
                int         d_numInput;
                const char *d_result;
            } DATA[] = {
                //line    input                numInput  result
                //----    -----                --------  ------
                { L_,     { },                 0,        ""             },
                { L_,     { 1 },               1,        "1"            },
                { L_,     { 1, 4 },            2,        "1 4"          },
                { L_,     { 1, 4, 2 },         3,        "1 4 2"        },
                { L_,     { 1, 4, 2, 8 },      4,        "1 4 2 8"      },
                { L_,     { 1, 4, 2, 8, 23 },  5,        "1 4 2 8 23"   },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int       LINE      = DATA[i].d_lineNum;
                const ElemType *INPUT     = DATA[i].d_input;
                const int       NUM_INPUT = DATA[i].d_numInput;
                const char     *RESULT    = DATA[i].d_result;

                bsl::stringstream ss;

                Type mX(INPUT, INPUT + NUM_INPUT);  const Type& X = mX;

                Util::printList(ss, X);

                ASSERTV(LINE, ss.str(), RESULT == ss.str());
            }
        }
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING 'printHex' FUNCTIONS
        //
        // Concerns:
        //
        // Plan:
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTESTING 'printHex' FUNCTIONS"
                          << "\n============================" << endl;

        static const struct {
            int         d_lineNum;
            const char *d_input;
            const char *d_result;
        } DATA[] = {
            //line    input       result
            //----    -----       ------
            { L_,     "",         ""              },
            { L_,     "a",        "61"            },
            { L_,     "ab",       "6162"          },
            { L_,     "abc",      "616263"        },
            { L_,     "abcd",     "61626364"      },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        if (verbose) cout << "\nUsing 'bsl::string'." << endl;
        {
            typedef bsl::string Type;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int          LINE          = DATA[i].d_lineNum;
                const char        *INPUT         = DATA[i].d_input;
                const char        *RESULT        = DATA[i].d_result;
                const bsl::size_t  INPUT_LENGTH  = bsl::strlen(INPUT);

                Type mX(INPUT, INPUT + INPUT_LENGTH); const Type& X = mX;
                bsl::stringstream ss;

                Util::printHex(ss, X);

                ASSERTV(LINE, ss.str(), RESULT == ss.str());
            }
        }

        if (verbose) cout << "\nUsing 'bslstl::StringRef'." << endl;
        {
            typedef bslstl::StringRef Type;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int          LINE          = DATA[i].d_lineNum;
                const char        *INPUT         = DATA[i].d_input;
                const char        *RESULT        = DATA[i].d_result;
                const bsl::size_t  INPUT_LENGTH  = bsl::strlen(INPUT);

                Type mX(INPUT, INPUT + INPUT_LENGTH); const Type& X = mX;
                bsl::stringstream ss;

                Util::printHex(ss, X);

                ASSERTV(LINE, ss.str(), RESULT == ss.str());
            }
        }

        if (verbose) cout << "\nUsing 'MyStringRef'." << endl;
        {
            typedef MyStringRef Type;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int          LINE          = DATA[i].d_lineNum;
                const char        *INPUT         = DATA[i].d_input;
                const char        *RESULT        = DATA[i].d_result;
                const bsl::size_t  INPUT_LENGTH  = bsl::strlen(INPUT);

                Type mX(INPUT, INPUT + INPUT_LENGTH); const Type& X = mX;
                bsl::stringstream ss;

                Util::printHex(ss, X);

                ASSERTV(LINE, ss.str(), RESULT == ss.str());
            }
        }

        if (verbose) cout << "\nUsing 'bsl::vector<char>'." << endl;
        {
            typedef bsl::vector<char> Type;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int          LINE          = DATA[i].d_lineNum;
                const char        *INPUT         = DATA[i].d_input;
                const char        *RESULT        = DATA[i].d_result;
                const bsl::size_t  INPUT_LENGTH  = bsl::strlen(INPUT);

                Type mX(INPUT, INPUT + INPUT_LENGTH); const Type& X = mX;
                bsl::stringstream ss;

                Util::printHex(ss, X);

                ASSERTV(LINE, ss.str(), RESULT == ss.str());
            }
        }
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING 'printDecimal' FUNCTIONS
        //
        // Concerns:
        //
        // Plan:
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTESTING 'printDecimal' FUNCTIONS"
                          << "\n================================" << endl;

        if (verbose) cout << "\nUsing 'bool'." << endl;
        {
            typedef bool Type;

            static const struct {
                int         d_lineNum;
                Type        d_input;
                const char *d_result;
            } DATA[] = {
                //line    input       result
                //----    -----       ------
                { L_,     true,       "1"             },
                { L_,     false,      "0"             },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int   LINE   = DATA[i].d_lineNum;
                const Type  INPUT  = DATA[i].d_input;
                const char *RESULT = DATA[i].d_result;

                bsl::stringstream ss;

                Util::printDecimal(ss, INPUT);

                ASSERTV(LINE, ss.str(), RESULT == ss.str());
            }
        }

        if (verbose) cout << "\nUsing 'char'." << endl;
        {
            typedef char Type;

            static const struct {
                int         d_lineNum;
                Type        d_input;
                const char *d_result;
            } DATA[] = {
                //line    input       result
                //----    -----       ------
                { L_,     -128,       "-128"          },
                { L_,     -127,       "-127"          },
                { L_,     -1,         "-1"            },
                { L_,     0,          "0"             },
                { L_,     1,          "1"             },
                { L_,     126,        "126"           },
                { L_,     127,        "127"           },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int   LINE   = DATA[i].d_lineNum;
                const Type  INPUT  = DATA[i].d_input;
                const char *RESULT = DATA[i].d_result;

                bsl::stringstream ss;

                Util::printDecimal(ss, INPUT);

                ASSERTV(LINE, ss.str(), RESULT == ss.str());
            }
        }

        if (verbose) cout << "\nUsing 'short'." << endl;
        {
            typedef short Type;

            static const struct {
                int         d_lineNum;
                Type        d_input;
                const char *d_result;
            } DATA[] = {
                //line    input       result
                //----    -----       ------
                { L_,     -32768,     "-32768"        },
                { L_,     -32767,     "-32767"        },
                { L_,     -1,         "-1"            },
                { L_,     0,          "0"             },
                { L_,     1,          "1"             },
                { L_,     32766,      "32766"         },
                { L_,     32767,      "32767"         },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int   LINE   = DATA[i].d_lineNum;
                const Type  INPUT  = DATA[i].d_input;
                const char *RESULT = DATA[i].d_result;

                bsl::stringstream ss;

                Util::printDecimal(ss, INPUT);

                ASSERTV(LINE, ss.str(), RESULT == ss.str());
            }
        }

        if (verbose) cout << "\nUsing 'int'." << endl;
        {
            typedef int Type;

            static const struct {
                int         d_lineNum;
                Type        d_input;
                const char *d_result;
            } DATA[] = {
                //line    input            result
                //----    -----            ------
                { L_,     -2147483647-1,   "-2147483648"   },
                { L_,     -2147483647,     "-2147483647"   },
                { L_,     -1,              "-1"            },
                { L_,     0,               "0"             },
                { L_,     1,               "1"             },
                { L_,     2147483646,      "2147483646"    },
                { L_,     2147483647,      "2147483647"    },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int   LINE   = DATA[i].d_lineNum;
                const Type  INPUT  = DATA[i].d_input;
                const char *RESULT = DATA[i].d_result;

                bsl::stringstream ss;

                Util::printDecimal(ss, INPUT);

                ASSERTV(LINE, ss.str(), RESULT == ss.str());
            }
        }

        if (verbose) cout << "\nUsing 'long'." << endl;
        {
            typedef long Type;

            static const struct {
                int         d_lineNum;
                Type        d_input;
                const char *d_result;
            } DATA[] = {
                //line  input                       result
                //----  --------------------------  -------------------------
                { L_,   -2147483647 - 1,            "-2147483648"            },
                { L_,   -2147483647,                "-2147483647"            },
                { L_,   -1,                         "-1"                     },
                { L_,    0,                          "0"                     },
                { L_,    1,                          "1"                     },
                { L_,    2147483646,                 "2147483646"            },
                { L_,    2147483647,                 "2147483647"            },
#ifdef U_LONG_IS_64_BITS
                { L_,   -9223372036854775807LL - 1, "-9223372036854775808"   },
                { L_,   -9223372036854775807LL,     "-9223372036854775807"   },
                { L_,    9223372036854775806LL,      "9223372036854775806"   },
                { L_,    9223372036854775807LL,      "9223372036854775807"   },
#endif
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int   LINE   = DATA[i].d_lineNum;
                const Type  INPUT  = DATA[i].d_input;
                const char *RESULT = DATA[i].d_result;

                bsl::stringstream ss;

                Util::printDecimal(ss, INPUT);

                ASSERTV(LINE, ss.str(), RESULT, RESULT == ss.str());
            }
        }

        if (verbose) cout << "\nUsing 'bsls::Types::Int64'." << endl;
        {
            typedef bsls::Types::Int64 Type;

            static const struct {
                int         d_lineNum;
                Type        d_input;
                const char *d_result;
            } DATA[] = {
                //line  input                       result
                //----  -----                       -------------------------
                { L_,   -9223372036854775807LL - 1, "-9223372036854775808"   },
                { L_,   -9223372036854775807LL,     "-9223372036854775807"   },
                { L_,   -1LL,                       "-1"                     },
                { L_,    0LL,                        "0"                     },
                { L_,    1LL,                        "1"                     },
                { L_,    9223372036854775806LL,      "9223372036854775806"   },
                { L_,    9223372036854775807LL,      "9223372036854775807"   },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int   LINE   = DATA[i].d_lineNum;
                const Type  INPUT  = DATA[i].d_input;
                const char *RESULT = DATA[i].d_result;

                bsl::stringstream ss;

                Util::printDecimal(ss, INPUT);

                ASSERTV(LINE, ss.str(), RESULT == ss.str());
            }
        }

        if (verbose) cout << "\nUsing 'unsigned char'." << endl;
        {
            typedef unsigned char Type;

            static const struct {
                int         d_lineNum;
                Type        d_input;
                const char *d_result;
            } DATA[] = {
                //line    input       result
                //----    -----       ------
                { L_,     0,          "0"             },
                { L_,     1,          "1"             },
                { L_,     254,        "254"           },
                { L_,     255,        "255"           },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int   LINE   = DATA[i].d_lineNum;
                const Type  INPUT  = DATA[i].d_input;
                const char *RESULT = DATA[i].d_result;

                bsl::stringstream ss;

                Util::printDecimal(ss, INPUT);

                ASSERTV(LINE, ss.str(), RESULT == ss.str());
            }
        }

        if (verbose) cout << "\nUsing 'unsigned short'." << endl;
        {
            typedef unsigned short Type;

            static const struct {
                int         d_lineNum;
                Type        d_input;
                const char *d_result;
            } DATA[] = {
                //line    input       result
                //----    -----       ------
                { L_,     0,          "0"             },
                { L_,     1,          "1"             },
                { L_,     65534,      "65534"         },
                { L_,     65535,      "65535"         },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int   LINE   = DATA[i].d_lineNum;
                const Type  INPUT  = DATA[i].d_input;
                const char *RESULT = DATA[i].d_result;

                bsl::stringstream ss;

                Util::printDecimal(ss, INPUT);

                ASSERTV(LINE, ss.str(), RESULT == ss.str());
            }
        }

        if (verbose) cout << "\nUsing 'unsigned int'." << endl;
        {
            typedef unsigned int Type;

            static const struct {
                int         d_lineNum;
                Type        d_input;
                const char *d_result;
            } DATA[] = {
                //line    input            result
                //----    -----            ------
                { L_,     0,               "0"             },
                { L_,     1,               "1"             },
                { L_,     4294967294U,     "4294967294"    },
                { L_,     4294967295U,     "4294967295"    },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int   LINE   = DATA[i].d_lineNum;
                const Type  INPUT  = DATA[i].d_input;
                const char *RESULT = DATA[i].d_result;

                bsl::stringstream ss;

                Util::printDecimal(ss, INPUT);

                ASSERTV(LINE, ss.str(), RESULT == ss.str());
            }
        }

        if (verbose) cout << "\nUsing 'unsigned long'." << endl;
        {
            typedef unsigned long Type;

            static const struct {
                int         d_lineNum;
                Type        d_input;
                const char *d_result;
            } DATA[] = {
                //line  input                    result
                //----  -----------------------  -----------------------
                { L_,   0,                       "0"                    },
                { L_,   1,                       "1"                    },
                { L_,   4294967294UL,            "4294967294"           },
                { L_,   4294967295UL,            "4294967295"           },
#ifdef U_LONG_IS_64_BITS
                { L_,   0LL,                     "0"                    },
                { L_,   1LL,                     "1"                    },
                { L_,   18446744073709551614ULL, "18446744073709551614" },
                { L_,   18446744073709551615ULL, "18446744073709551615" },
#endif
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int   LINE   = DATA[i].d_lineNum;
                const Type  INPUT  = DATA[i].d_input;
                const char *RESULT = DATA[i].d_result;

                bsl::stringstream ss;

                Util::printDecimal(ss, INPUT);

                ASSERTV(LINE, ss.str(), RESULT, RESULT == ss.str());
            }
        }

        if (verbose) cout << "\nUsing 'bsls::Types::Uint64'." << endl;
        {
            typedef bsls::Types::Uint64 Type;

            static const struct {
                int         d_lineNum;
                Type        d_input;
                const char *d_result;
            } DATA[] = {
                //line    input                     result
                //----    -----                     ------
                { L_,     0ULL,                     "0"                      },
                { L_,     1ULL,                     "1"                      },
                { L_,     18446744073709551614ULL,  "18446744073709551614"   },
                { L_,     18446744073709551615ULL,  "18446744073709551615"   },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int   LINE   = DATA[i].d_lineNum;
                const Type  INPUT  = DATA[i].d_input;
                const char *RESULT = DATA[i].d_result;

                bsl::stringstream ss;

                Util::printDecimal(ss, INPUT);

                ASSERTV(LINE, ss.str(), RESULT == ss.str());
            }
        }

        if (verbose) cout << "\nUsing 'MyEnumeration::Value'." << endl;
        {
            typedef test::MyEnumeration::Value Type;

            static const struct {
                int         d_lineNum;
                Type        d_input;
                const char *d_result;
            } DATA[] = {
                //line    input                         result
                //----    -----                         ------
                { L_,     test::MyEnumeration::VALUE1,  "0"         },
                { L_,     test::MyEnumeration::VALUE2,  "1"         },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int   LINE   = DATA[i].d_lineNum;
                const Type  INPUT  = DATA[i].d_input;
                const char *RESULT = DATA[i].d_result;

                bsl::stringstream ss;

                Util::printDecimal(ss, INPUT);

                ASSERTV(LINE, RESULT, ss.str(), RESULT == ss.str());
            }
        }

        if (verbose) cout << "\nUsing 'CustomizedInt'." << endl;
        {
            typedef test::CustomizedInt Type;

            static const struct {
                int         d_lineNum;
                Type        d_input;
                const char *d_result;
            } DATA[] = {
                //line    input                result
                //----    -----                ------
                { L_,     Type(-2147483647-1), "-2147483648"   },
                { L_,     Type(-2147483647),   "-2147483647"   },
                { L_,     Type(-1),            "-1"            },
                { L_,     Type(0),             "0"             },
                { L_,     Type(1),             "1"             },
                { L_,     Type(2147483646),    "2147483646"    },
                { L_,     Type(2147483647),    "2147483647"    },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int   LINE   = DATA[i].d_lineNum;
                const Type  INPUT  = DATA[i].d_input;
                const char *RESULT = DATA[i].d_result;

                bsl::stringstream ss;

                Util::printDecimal(ss, Type(INPUT));

                ASSERTV(LINE, ss.str(), RESULT == ss.str());
            }
        }

        if (verbose) cout << "\nUsing 'float'." << endl;
        {
            typedef float Type;

            typedef bsl::numeric_limits<Type> NumLimits;

            static const struct {
                int         d_lineNum;
                Type        d_input;
                const char *d_inputStr;
                const char *d_result;
            } DATA[] = {
#define F(input, result) { L_, input, #input, result }
                //    input                result
                //---------------     ---------------------------------------
                F(   -1.0f,                "-1"                              ),
                F(   -0.1f,                "-0.1"                            ),
                F(   -0.123456f,           "-0.123456"                       ),
                F(    0.0f,                 "0"                              ),
                F(    0.1f,                 "0.1"                            ),
                F(    1.0f,                 "1"                              ),
                F(  123.4567f,            "123.4567"                         ),

                // Values from 'balxml_encoder.t.cpp' 'runTestCase17'
                F(1.5258789e-05f      ,                   "0.000015258789"),
                F(3.0517578e-05f      ,                   "0.000030517578"),
                F(6.1035156e-05f      ,                   "0.000061035156"),
                F(1.2207031e-04f      ,                   "0.00012207031" ),
                F(2.4414062e-04f      ,                   "0.00024414062" ),
                F(4.8828125e-04f      ,                   "0.00048828125" ),
                F(9.765625e-04f       ,                   "0.0009765625"  ),
                F(1.953125e-03f       ,                   "0.001953125"   ),
                F(3.90625e-03f        ,                   "0.00390625"    ),
                F(7.8125e-03f         ,                   "0.0078125"     ),
                F(1.5625e-02f         ,                   "0.015625"      ),
                F(3.125e-02f          ,                   "0.03125"       ),
                F(6.25e-02f           ,                   "0.0625"        ),
                F(1.25e-01f           ,                   "0.125"         ),
                F(2.5e-1f             ,                   "0.25"          ),
                F(5e-1f               ,                   "0.5"           ),
                F(                 1.f,                   "1"             ),
                F(                 8.f,                   "8"             ),
                F(                64.f,                  "64"             ),
                F(               128.f,                 "128"             ),
                F(              1024.f,                "1024"             ),
                F(             16384.f,               "16384"             ),
                F(            131072.f,              "131072"             ),
                F(           1048576.f,             "1048576"             ),
                F(          16777216.f,            "16777216"             ),
                F(         134217728.f,           "134217728"             ),
                F(        1073741824.f,          "1073741824"             ),
                F(       17179869184.f,         "17179869184"             ),
                F(      137438953472.f,        "137438953472"             ),
                F(     1099511627776.f,       "1099511627776"             ),
                F(    17592186044416.f,      "17592186044416"             ),
                F(   140737488355328.f,     "140737488355328"             ),
                F(  1125899906842624.f,    "1125899906842624"             ),
                F( 18014398509481984.f,   "18014398509481984"             ),
                F(144115188075855870.f,  "144115188075855872"             ),

                // Arbitrary Large and Small Number
                F( 1.234567e35f,   "123456XXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"    ),
                F(-1.234567e35f,  "-123456XXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"    ),

                F( 1.234567e-35f,
                                "0.00000000000000000000000000000000001234567"),
                F(-1.234567e-35f,
                               "-0.00000000000000000000000000000000001234567"),

                // Small Integers
                F(    1234567.f,      "1234567"       ),
                F(    12345678.f,    "12345678"       ),
                F(    123456789.f,  "123456792"       ),

                // Full Mantissa Integers
                F(1.0f * 0xFFFFFF,   "16777215"       ),
                F(1.0f * 0xFFFFFF * (1ull << 63) * (1ull << 41), // this's max
                                    "340282346638528859811704183484516925440"),

                // Boundary Values
                F( NumLimits::min(),
                            "0.000000000000000000000000000000000000011754944"),
                F( NumLimits::max(),
                                    "340282346638528859811704183484516925440"),
                F(-NumLimits::min(),
                           "-0.000000000000000000000000000000000000011754944"),
                F(-NumLimits::max(),
                                   "-340282346638528859811704183484516925440"),

                // Non-numeric Values
                F( NumLimits::infinity(),      "+INF"),
                F(-NumLimits::infinity(),      "-INF"),
                F( NumLimits::signaling_NaN(),  "NaN"),
                F( NumLimits::quiet_NaN(),      "NaN"),
#undef F
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int   LINE      = DATA[i].d_lineNum;
                const Type  INPUT     = DATA[i].d_input;
                const char *INPUT_STR = DATA[i].d_inputStr;
                const char *RESULT    = DATA[i].d_result;

                bsl::stringstream ss;

                balxml::EncoderOptions options;
                Util::printDecimal(ss, INPUT, &options);
                // 'float' decimal printing uses no options

                if (bdlb::Float::isNan(INPUT) ||
                    bdlb::Float::isInfinite(INPUT)) {

                    ASSERTV(LINE, ss.str(), !ss);
                }
                else {
                    ASSERTV(LINE, INPUT_STR, ss.str(), RESULT,
                            verifyFloatingPointResult(ss.str(), RESULT));
                }

                bsl::stringstream noOptions;
                Util::printDecimal(noOptions, INPUT, 0);
                ASSERTV(LINE, INPUT_STR, noOptions.str(), ss.str(),
                        noOptions.str() == ss.str());
            }
        }

        if (verbose) cout << "\nUsing 'double'." << endl;
        {
            typedef double Type;

            typedef bsl::numeric_limits<Type> NumLimits;

            static const struct {
                int         d_lineNum;
                Type        d_input;
                const char* d_inputStr;
                const char* d_expected;
            } DATA[] = {
#define D(input, expected) { L_, input, #input, expected }
                //input               result
                //------------------  -------------------------------------
                D(-1.0,                                "-1"                ),
                D(-0.1,                                "-0.1"              ),
                D(-0.123456789012345,                  "-0.123456789012345"),
                D( 0.0,                                 "0"                ),
                D( 0.1,                                 "0.1"              ),
                D( 1.0,                                 "1"                ),
                D( 123.4567,                          "123.4567"           ),

                // Values from 'balxml_encoder.t.cpp' 'runTestCase17'
                D(1.52587890625e-05  ,                   "0.0000152587890625"),
                D(3.0517578125e-05   ,                   "0.000030517578125" ),
                D(6.103515625e-05    ,                   "0.00006103515625"  ),
                D(1.220703125e-04    ,                   "0.0001220703125"   ),
                D(2.44140625e-04     ,                   "0.000244140625"    ),
                D(4.8828125e-04      ,                   "0.00048828125"     ),
                D(9.765625e-04       ,                   "0.0009765625"      ),
                D(1.953125e-03       ,                   "0.001953125"       ),
                D(3.90625e-03        ,                   "0.00390625"        ),
                D(7.8125e-03         ,                   "0.0078125"         ),
                D(1.5625e-02         ,                   "0.015625"          ),
                D(3.125e-02          ,                   "0.03125"           ),
                D(6.25e-02           ,                   "0.0625"            ),
                D(1.25e-01           ,                   "0.125"             ),
                D(2.5e-1             ,                   "0.25"              ),
                D(5e-1               ,                   "0.5"               ),
                D(                 1.,                   "1"                 ),
                D(                 8.,                   "8"                 ),
                D(                64.,                  "64"                 ),
                D(               128.,                 "128"                 ),
                D(              1024.,                "1024"                 ),
                D(             16384.,               "16384"                 ),
                D(            131072.,              "131072"                 ),
                D(           1048576.,             "1048576"                 ),
                D(          16777216.,            "16777216"                 ),
                D(         134217728.,           "134217728"                 ),
                D(        1073741824.,          "1073741824"                 ),
                D(       17179869184.,         "17179869184"                 ),
                D(      137438953472.,        "137438953472"                 ),
                D(     1099511627776.,       "1099511627776"                 ),
                D(    17592186044416.,      "17592186044416"                 ),
                D(   140737488355328.,     "140737488355328"                 ),
                D(  1125899906842624.,    "1125899906842624"                 ),
                D( 18014398509481984.,   "18014398509481984"                 ),
                D(144115188075855870.,  "144115188075855872"                 ),

                // Small Integers
                D(123456789012345.,       "123456789012345"                ),
                D(1234567890123456.,     "1234567890123456"                ),
                D(12345678901234567.,   "1234567890123456X"                ),
                D(123456789012345678., "1234567890123456XX"                ),

                // Full Mantissa Integers
                D(1.0 * 0x1FFFFFFFFFFFFFull, "9007199254740991"),
                D(1.0 * 0x1FFFFFFFFFFFFFull // This is also 'NumLimits::max()'
                      * (1ull << 63) * (1ull << 63) * (1ull << 63)
                      * (1ull << 63) * (1ull << 63) * (1ull << 63)
                      * (1ull << 63) * (1ull << 63) * (1ull << 63)
                      * (1ull << 63) * (1ull << 63) * (1ull << 63)
                      * (1ull << 63) * (1ull << 63) * (1ull << 63)
                      * (1ull << 26),
                  "17976931348623157081452742373170435679807056752584" //  50
                  "49965989174768031572607800285387605895586327668781" // 100
                  "71540458953514382464234321326889464182768467546703" // 150
                  "53751698604991057655128207624549009038932894407586" // 200
                  "85084551339423045832369032229481658085593321233482" // 250
                  "74797826204144723168738177180919299881250404026184" // 300
                  "124858368"),                                        // 309

                // Boundary Values
                D( NumLimits::min(), "0."                              //+2
                  "00000000000000000000000000000000000000000000000000" //  50+2
                  "00000000000000000000000000000000000000000000000000" // 100+2
                  "00000000000000000000000000000000000000000000000000" // 150+2
                  "00000000000000000000000000000000000000000000000000" // 200+2
                  "00000000000000000000000000000000000000000000000000" // 250+2
                  "00000000000000000000000000000000000000000000000000" // 300+2
                  "000000022250738585072014"),                         // 324+2

                D( NumLimits::max(),
                  "17976931348623157081452742373170435679807056752584" //  50
                  "49965989174768031572607800285387605895586327668781" // 100
                  "71540458953514382464234321326889464182768467546703" // 150
                  "53751698604991057655128207624549009038932894407586" // 200
                  "85084551339423045832369032229481658085593321233482" // 250
                  "74797826204144723168738177180919299881250404026184" // 300
                  "124858368"),                                        // 309

                D(-NumLimits::min(), "-0."                             // +3
                  "00000000000000000000000000000000000000000000000000" //  50+3
                  "00000000000000000000000000000000000000000000000000" // 100+3
                  "00000000000000000000000000000000000000000000000000" // 150+3
                  "00000000000000000000000000000000000000000000000000" // 200+3
                  "00000000000000000000000000000000000000000000000000" // 250+3
                  "00000000000000000000000000000000000000000000000000" // 300+3
                  "000000022250738585072014"),                         // 324+3

                D(-NumLimits::max(), "-"                               // +1
                  "17976931348623157081452742373170435679807056752584" //  50+1
                  "49965989174768031572607800285387605895586327668781" // 100+1
                  "71540458953514382464234321326889464182768467546703" // 150+1
                  "53751698604991057655128207624549009038932894407586" // 200+1
                  "85084551339423045832369032229481658085593321233482" // 250+1
                  "74797826204144723168738177180919299881250404026184" // 300+1
                  "124858368"),                                        // 309+1

                // Non-numeric Values
                D( NumLimits::infinity(),       "+INF"),
                D(-NumLimits::infinity(),       "-INF"),
                D( NumLimits::signaling_NaN(),   "NaN"),
                D( NumLimits::quiet_NaN(),       "NaN"),
#undef D
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int   LINE      = DATA[i].d_lineNum;
                const Type  INPUT     = DATA[i].d_input;
                const char *INPUT_STR = DATA[i].d_inputStr;
                const char *EXPECTED  = DATA[i].d_expected;

                bsl::stringstream ss;

                balxml::EncoderOptions options;
                Util::printDecimal(ss, INPUT, &options);

                if (bdlb::Float::isNan(INPUT) ||
                    bdlb::Float::isInfinite(INPUT)) {

                    ASSERTV(LINE, ss.str(), !ss);
                }
                else {
                    ASSERTV(LINE, INPUT_STR, ss.str(), EXPECTED,
                            verifyFloatingPointResult(ss.str(), EXPECTED));
                }

                bsl::stringstream noOptions;
                Util::printDecimal(noOptions, INPUT, 0);
                ASSERTV(LINE, INPUT_STR, noOptions.str(), ss.str(),
                        noOptions.str() == ss.str());
            }
        }

        if (verbose) cout << "\nUsing 'double' with encoder options" << endl;
        {
            typedef double Type;

            typedef bsl::numeric_limits<Type> Limits;

            const int N = -42;

            static const struct {
                int         d_lineNum;
                Type        d_input;
                const char *d_inputStr;
                int         d_maxTotalDigits;
                int         d_maxFractionDigits;
                const char *d_expected;
            } DATA[] = {
#define D(input, maxTotalDigits, maxFractionDigits, expected)                 \
             { L_, input, #input, maxTotalDigits, maxFractionDigits, expected }
        //  input              TD   FD            result
        // -------             --   --           ----------------------------
        D(      0,              0,   0,           "0.0"                      ),
        D(      1,              0,   0,           "1.0"                      ),
        D(     -1,              0,   0,           "-1.0"                     ),

        D(      0,              0,   1,           "0.0"                      ),
        D(      1,              0,   1,           "1.0"                      ),
        D(     -1,              0,   1,           "-1.0"                     ),

        D(      0,              1,   0,           "0.0"                      ),
        D(      1,              1,   0,           "1.0"                      ),
        D(     -1,              1,   0,           "-1.0"                     ),

        D(      0,              1,   1,           "0.0"                      ),
        D(      1,              1,   1,           "1.0"                      ),
        D(     -1,              1,   1,           "-1.0"                     ),

        D(      0,              2,   1,           "0.0"                      ),
        D(      1,              2,   1,           "1.0"                      ),
        D(     -1,              2,   1,           "-1.0"                     ),

        D(      0,              2,   2,           "0.0"                      ),
        D(      1,              2,   2,           "1.0"                      ),
        D(     -1,              2,   2,           "-1.0"                     ),

        D(      0,              2,   3,           "0.0"                      ),
        D(      1,              2,   3,           "1.0"                      ),
        D(     -1,              2,   3,           "-1.0"                     ),

        D(      0,              3,   1,           "0.0"                      ),
        D(      1,              3,   1,           "1.0"                      ),
        D(     -1,              3,   1,           "-1.0"                     ),

        D(      0,              3,   2,           "0.00"                     ),
        D(      1,              3,   2,           "1.00"                     ),
        D(     -1,              3,   2,           "-1.00"                    ),

        D(      0,              3,   3,           "0.00"                     ),
        D(      1,              3,   3,           "1.00"                     ),
        D(     -1,              3,   3,           "-1.00"                    ),

        D(      0,              4,   2,           "0.00"                     ),
        D(      1,              4,   2,           "1.00"                     ),
        D(     -1,              4,   2,           "-1.00"                    ),

        D(      0,              4,   3,           "0.000"                    ),
        D(      1,              4,   3,           "1.000"                    ),
        D(     -1,              4,   3,           "-1.000"                   ),

        D(      0.0,            2,   0,           "0.0"                      ),
        D(      1.0,            2,   0,           "1.0"                      ),
        D(     -1.0,            2,   0,           "-1.0"                     ),

        D(      0.0,            2,   1,           "0.0"                      ),
        D(      1.0,            2,   1,           "1.0"                      ),
        D(     -1.0,            2,   1,           "-1.0"                     ),

        D(      0.0,            2,   2,           "0.0"                      ),
        D(      1.0,            2,   2,           "1.0"                      ),
        D(     -1.0,            2,   2,           "-1.0"                     ),

        D(      0.0,            2,   3,           "0.0"                      ),
        D(      1.0,            2,   3,           "1.0"                      ),
        D(     -1.0,            2,   3,           "-1.0"                     ),

        D(      0.0,            3,   2,           "0.00"                     ),
        D(      1.0,            3,   2,           "1.00"                     ),
        D(     -1.0,            3,   2,           "-1.00"                    ),

        D(      0.0,            3,   3,           "0.00"                     ),
        D(      1.0,            3,   3,           "1.00"                     ),
        D(     -1.0,            3,   3,           "-1.00"                    ),

        D(      0.1,            2,   0,           "0.1"                      ),
        D(     -0.1,            2,   0,           "-0.1"                     ),

        D(      0.1,            2,   1,           "0.1"                      ),
        D(     -0.1,            2,   1,           "-0.1"                     ),

        D(      0.1,            2,   2,           "0.1"                      ),
        D(     -0.1,            2,   2,           "-0.1"                     ),

        D(      0.1,            2,   3,           "0.1"                      ),
        D(     -0.1,            2,   3,           "-0.1"                     ),

        D(      0.1234,         0,   0,           "0.1"                      ),
        D(     -0.1234,         0,   0,           "-0.1"                     ),

        D(      0.1234,         1,   0,           "0.1"                      ),
        D(     -0.1234,         1,   0,           "-0.1"                     ),

        D(      0.1234,         0,   1,           "0.1"                      ),
        D(     -0.1234,         0,   1,           "-0.1"                     ),

        D(      0.1234,         1,   1,           "0.1"                      ),
        D(     -0.1234,         1,   1,           "-0.1"                     ),

        D(      79.864,         0,   0,           "79.9"                     ),
        D(     -63.234,         0,   0,           "-63.2"                    ),

        D(      79.864,         1,   0,           "79.9"                     ),
        D(     -63.234,         1,   0,           "-63.2"                    ),

        D(      79.864,         1,   1,           "79.9"                     ),
        D(     -63.234,         1,   1,           "-63.2"                    ),

        D(      79.864,         2,   1,           "79.9"                     ),
        D(     -63.234,         2,   1,           "-63.2"                    ),

        D(      79.864,         2,   2,           "79.9"                     ),
        D(     -63.234,         2,   2,           "-63.2"                    ),

        D(      79.864,         2,   3,           "79.9"                     ),
        D(     -63.234,         2,   3,           "-63.2"                    ),

        D(      79.864,         3,   3,           "79.8"                     ),
        D(     -63.234,         3,   3,           "-63.2"                    ),

        D(      79.864,         3,   2,           "79.8"                     ),
        D(     -63.234,         3,   2,           "-63.2"                    ),

        D(      79.864,         5,   3,           "79.864"                   ),
        D(     -63.234,         5,   3,           "-63.234"                  ),

        D(      79.864,         5,   4,           "79.864"                   ),
        D(     -63.234,         5,   4,           "-63.234"                  ),

        D(      79.864,         6,   3,           "79.864"                   ),
        D(     -63.234,         6,   3,           "-63.234"                  ),

        D(      79.864,         6,   4,           "79.8640"                  ),
        D(     -63.234,         6,   4,           "-63.2340"                 ),

        // Examples from the implementation comments
        D(      65.4321,        4,   N,            "65.43"                   ),
        D(    1234.001,         4,   N,          "1234.0"                    ),
        D(       1.45623,       4,   N,             "1.456"                  ),
        D(      65.4321,        4,   2,            "65.43"                   ),
        D(    1234.001,         4,   2,          "1234.0"                    ),
        D(       1.45623,       4,   2,             "1.46"                   ),

        // Large integer parts writing more digits than asked for
        D(  123456.001,         4,   N,          "123456.0"                  ),

        // Values from 'balxml_encoder.t.cpp' 'runTestCase17' (no opts set)
        D(1.52587890625e-05  ,  N,   N,                  "0.0000152587890625"),
        D(3.0517578125e-05   ,  N,   N,                  "0.000030517578125" ),
        D(6.103515625e-05    ,  N,   N,                  "0.00006103515625"  ),
        D(1.220703125e-04    ,  N,   N,                  "0.0001220703125"   ),
        D(2.44140625e-04     ,  N,   N,                  "0.000244140625"    ),
        D(4.8828125e-04      ,  N,   N,                  "0.00048828125"     ),
        D(9.765625e-04       ,  N,   N,                  "0.0009765625"      ),
        D(1.953125e-03       ,  N,   N,                  "0.001953125"       ),
        D(3.90625e-03        ,  N,   N,                  "0.00390625"        ),
        D(7.8125e-03         ,  N,   N,                  "0.0078125"         ),
        D(1.5625e-02         ,  N,   N,                  "0.015625"          ),
        D(3.125e-02          ,  N,   N,                  "0.03125"           ),
        D(6.25e-02           ,  N,   N,                  "0.0625"            ),
        D(1.25e-01           ,  N,   N,                  "0.125"             ),
        D(2.5e-1             ,  N,   N,                  "0.25"              ),
        D(5e-1               ,  N,   N,                  "0.5"               ),
        D(                 1.,  N,   N,                  "1"                 ),
        D(                 8.,  N,   N,                  "8"                 ),
        D(                64.,  N,   N,                 "64"                 ),
        D(               128.,  N,   N,                "128"                 ),
        D(              1024.,  N,   N,               "1024"                 ),
        D(             16384.,  N,   N,              "16384"                 ),
        D(            131072.,  N,   N,             "131072"                 ),
        D(           1048576.,  N,   N,            "1048576"                 ),
        D(          16777216.,  N,   N,           "16777216"                 ),
        D(         134217728.,  N,   N,          "134217728"                 ),
        D(        1073741824.,  N,   N,         "1073741824"                 ),
        D(       17179869184.,  N,   N,        "17179869184"                 ),
        D(      137438953472.,  N,   N,       "137438953472"                 ),
        D(     1099511627776.,  N,   N,      "1099511627776"                 ),
        D(    17592186044416.,  N,   N,     "17592186044416"                 ),
        D(   140737488355328.,  N,   N,    "140737488355328"                 ),
        D(  1125899906842624.,  N,   N,   "1125899906842624"                 ),
        D( 18014398509481984.,  N,   N,  "18014398509481984"                 ),
        D(144115188075855870.,  N,   N, "144115188075855872"                 ),

        // More from 'balxml_encoder.t.cpp' 'runTestCase17' (with options set)
        D(1.52587890625e-05   ,   N,   0,                  "0.0"             ),
        D(3.0517578125e-05    ,   N,   0,                  "0.0"             ),
        D(6.103515625e-05     ,   N,   0,                  "0.0"             ),
        D(1.220703125e-04     ,   N,   0,                  "0.0"             ),
        D(2.44140625e-04      ,   N,   0,                  "0.0"             ),
        D(4.8828125e-04       ,   N,   0,                  "0.0"             ),
        D(9.765625e-04        ,   N,   0,                  "0.0"             ),
        D(1.953125e-03        ,   N,   0,                  "0.0"             ),
        D(3.90625e-03         ,   N,   0,                  "0.0"             ),
        D(7.8125e-03          ,   N,   0,                  "0.0"             ),
        D(1.5625e-02          ,   N,   0,                  "0.0"             ),
        D(3.125e-02           ,   N,   0,                  "0.0"             ),
        D(6.25e-02            ,   N,   0,                  "0.1"             ),
        D(1.25e-01            ,   N,   0,                  "0.1"             ),
        D(2.5e-1              ,   N,   0,                  "0.2|0.3"         ),
        D(5e-1                ,   N,   0,                  "0.5"             ),
        D(                 1.0,   N,   0,                  "1.0"             ),
        D(                 8.0,   N,   0,                  "8.0"             ),
        D(                64.0,   N,   0,                 "64.0"             ),
        D(               128.0,   N,   0,                "128.0"             ),
        D(              1024.0,   N,   0,               "1024.0"             ),
        D(             16384.0,   N,   0,              "16384.0"             ),
        D(            131072.0,   N,   0,             "131072.0"             ),
        D(           1048576.0,   N,   0,            "1048576.0"             ),
        D(          16777216.0,   N,   0,           "16777216.0"             ),
        D(         134217728.0,   N,   0,          "134217728.0"             ),
        D(        1073741824.0,   N,   0,         "1073741824.0"             ),
        D(       17179869184.0,   N,   0,        "17179869184.0"             ),
        D(      137438953472.0,   N,   0,       "137438953472.0"             ),
        D(     1099511627776.0,   N,   0,      "1099511627776.0"             ),
        D(    17592186044416.0,   N,   0,     "17592186044416.0"             ),
        D(   140737488355328.0,   N,   0,    "140737488355328.0"             ),
        D(  1125899906842624.0,   N,   0,   "1125899906842624.0"             ),
        D( 18014398509481984.0,   N,   0,  "18014398509481984.0"             ),
        D(144115188075855870.0,   N,   0, "144115188075855872.0"             ),

        D(   123456789012345.0,  14,   0,    "123456789012345.0"             ),
        D(   123456789012345.0,  15,   0,    "123456789012345.0"             ),
        D(   123456789012345.0,  16,   0,    "123456789012345.0"             ),
        D(   123456789012345.0,  17,   0,    "123456789012345.0"             ),
        D(   123456789012345.0,  18,   0,    "123456789012345.0"             ),
        D(   123456789012345.0,  19,   0,    "123456789012345.0"             ),
        D(   123456789012345.0,  20,   0,    "123456789012345.0"             ),

        D(   123456789012345.0,  14,   1,    "123456789012345.0"             ),
        D(   123456789012345.0,  15,   1,    "123456789012345.0"             ),
        D(   123456789012345.0,  16,   1,    "123456789012345.0"             ),
        D(   123456789012345.0,  17,   1,    "123456789012345.0"             ),
        D(   123456789012345.0,  18,   1,    "123456789012345.0"             ),
        D(   123456789012345.0,  19,   1,    "123456789012345.0"             ),
        D(   123456789012345.0,  20,   1,    "123456789012345.0"             ),

        D(   123456789012345.0,  14,   2,    "123456789012345.0"             ),
        D(   123456789012345.0,  15,   2,    "123456789012345.0"             ),
        D(   123456789012345.0,  16,   2,    "123456789012345.0"             ),
        D(   123456789012345.0,  17,   2,    "123456789012345.00"            ),
        D(   123456789012345.0,  18,   2,    "123456789012345.00"            ),
        D(   123456789012345.0,  19,   2,    "123456789012345.00"            ),
        D(   123456789012345.0,  20,   2,    "123456789012345.00"            ),

        D(   123456789012345.0,  15,   0,    "123456789012345.0"             ),
        D(   123456789012345.0,  16,   0,    "123456789012345.0"             ),
        D(   123456789012345.0,  17,   0,    "123456789012345.0"             ),
        D(   123456789012345.0,  18,   0,    "123456789012345.0"             ),
        D(   123456789012345.0,  19,   0,    "123456789012345.0"             ),
        D(   123456789012345.0,  20,   0,    "123456789012345.0"             ),

        D(    123456789.012345,  14,   0,          "123456789.0"             ),
        D(    123456789.012345,  15,   0,          "123456789.0"             ),
        D(    123456789.012345,  16,   0,          "123456789.0"             ),
        D(    123456789.012345,  17,   0,          "123456789.0"             ),
        D(    123456789.012345,  18,   0,          "123456789.0"             ),
        D(    123456789.012345,  19,   0,          "123456789.0"             ),
        D(    123456789.012345,  20,   0,          "123456789.0"             ),

        D(    123456789.012345,  14,   1,          "123456789.0"             ),
        D(    123456789.012345,  15,   1,          "123456789.0"             ),
        D(    123456789.012345,  16,   1,          "123456789.0"             ),
        D(    123456789.012345,  17,   1,          "123456789.0"             ),
        D(    123456789.012345,  18,   1,          "123456789.0"             ),
        D(    123456789.012345,  19,   1,          "123456789.0"             ),
        D(    123456789.012345,  20,   1,          "123456789.0"             ),

        D(    123456789.012345,  14,   2,          "123456789.01"            ),
        D(    123456789.012345,  15,   2,          "123456789.01"            ),
        D(    123456789.012345,  16,   2,          "123456789.01"            ),
        D(    123456789.012345,  17,   2,          "123456789.01"            ),
        D(    123456789.012345,  18,   2,          "123456789.01"            ),
        D(    123456789.012345,  19,   2,          "123456789.01"            ),
        D(    123456789.012345,  20,   2,          "123456789.01"            ),

        D(    123456789.012345,  14,   6,          "123456789.01234"         ),
        D(    123456789.012345,  15,   6,          "123456789.012345"        ),
        D(    123456789.012345,  16,   6,          "123456789.012345"        ),
        D(    123456789.012345,  17,   6,          "123456789.012345"        ),
        D(    123456789.012345,  18,   6,          "123456789.012345"        ),
        D(    123456789.012345,  19,   6,          "123456789.012345"        ),
        D(    123456789.012345,  20,   6,          "123456789.012345"        ),

        D(    123456789.012345,  14,   7,          "123456789.01234"         ),
        D(    123456789.012345,  15,   7,          "123456789.012345"        ),
        D(    123456789.012345,  16,   7,          "123456789.0123450"       ),
        D(    123456789.012345,  17,   7,          "123456789.0123450"       ),
        D(    123456789.012345,  18,   7,          "123456789.0123450"       ),
        D(    123456789.012345,  19,   7,          "123456789.0123450"       ),
        D(    123456789.012345,  20,   7,          "123456789.0123450"       ),

        D(  Limits::infinity(),      0, 0, "+INF" ),
        D( -Limits::infinity(),      0, 0, "-INF" ),
        D(  Limits::signaling_NaN(), 0, 0,  "NaN" ),
        D(  Limits::quiet_NaN(),     0, 0,  "NaN" ),

        D(Limits::max(), N, N,
          "179769313486231570814527423731704356798070567525844996598917" //  60
          "476803157260780028538760589558632766878171540458953514382464" // 120
          "234321326889464182768467546703537516986049910576551282076245" // 180
          "490090389328944075868508455133942304583236903222948165808559" // 240
          "332123348274797826204144723168738177180919299881250404026184" // 300
          "124858368"),                                                  // 309

#ifndef BSLS_PLATFORM_OS_AIX
        D(Limits::max(), N, 0,
          "179769313486231570814527423731704356798070567525844996598917" //  60
          "476803157260780028538760589558632766878171540458953514382464" // 120
          "234321326889464182768467546703537516986049910576551282076245" // 180
          "490090389328944075868508455133942304583236903222948165808559" // 240
          "332123348274797826204144723168738177180919299881250404026184" // 300
          "124858368.0"),                                                // 311
#else
        // AIX 'sprintf' is "lazy"
        D(Limits::max(), N, 0,
          "179769313486231570814527423731704356800000000000000000000000" //  60
          "000000000000000000000000000000000000000000000000000000000000" // 120
          "000000000000000000000000000000000000000000000000000000000000" // 180
          "000000000000000000000000000000000000000000000000000000000000" // 240
          "000000000000000000000000000000000000000000000000000000000000" // 300
          "000000000.0"),                                                // 311
#endif

#ifndef BSLS_PLATFORM_OS_AIX
        D(Limits::max(), 326, 17,
          "179769313486231570814527423731704356798070567525844996598917" //  60
          "476803157260780028538760589558632766878171540458953514382464" // 120
          "234321326889464182768467546703537516986049910576551282076245" // 180
          "490090389328944075868508455133942304583236903222948165808559" // 240
          "332123348274797826204144723168738177180919299881250404026184" // 300
          "124858368.00000000000000000"),                                // 327
#else
        // AIX 'sprintf' is "lazy"
        D(Limits::max(), 326, 17,
          "179769313486231570814527423731704356800000000000000000000000" //  60
          "000000000000000000000000000000000000000000000000000000000000" // 120
          "000000000000000000000000000000000000000000000000000000000000" // 180
          "000000000000000000000000000000000000000000000000000000000000" // 240
          "000000000000000000000000000000000000000000000000000000000000" // 300
          "000000000.00000000000000000"),                                // 327
#endif
        D(Limits::min(), N, N, "0."                                  //     + 2
               "00000000000000000000000000000000000000000000000000"  //  50 + 2
               "00000000000000000000000000000000000000000000000000"  // 100 + 2
               "00000000000000000000000000000000000000000000000000"  // 150 + 2
               "00000000000000000000000000000000000000000000000000"  // 200 + 2
               "00000000000000000000000000000000000000000000000000"  // 250 + 2
               "00000000000000000000000000000000000000000000000000"  // 300 + 2
               "000000022250738585072014"),                          // 324 + 2

        D(Limits::min(), 326, 17, "0.00000000000000000"),

        D(Limits::min(), N, 0, "0.0" ),

        D(-Limits::max(), N, N, "-"                                  //     + 1
               "17976931348623157081452742373170435679807056752584"  //  50 + 1
               "49965989174768031572607800285387605895586327668781"  // 100 + 1
               "71540458953514382464234321326889464182768467546703"  // 150 + 1
               "53751698604991057655128207624549009038932894407586"  // 200 + 1
               "85084551339423045832369032229481658085593321233482"  // 250 + 1
               "74797826204144723168738177180919299881250404026184"  // 300 + 1
               "124858368"),                                         // 309 + 1

#ifndef BSLS_PLATFORM_OS_AIX
        D(-Limits::max(), N, 0, "-"                                  //     + 1
               "17976931348623157081452742373170435679807056752584"  //  50 + 1
               "49965989174768031572607800285387605895586327668781"  // 100 + 1
               "71540458953514382464234321326889464182768467546703"  // 150 + 1
               "53751698604991057655128207624549009038932894407586"  // 200 + 1
               "85084551339423045832369032229481658085593321233482"  // 250 + 1
               "74797826204144723168738177180919299881250404026184"  // 300 + 1
               "124858368.0"),                                       // 311 + 1
#else
        // AIX 'sprintf' is "lazy"
        D(-Limits::max(), N, 0, "-"                                  //     + 1
               "17976931348623157081452742373170435680000000000000"  //  50 + 1
               "00000000000000000000000000000000000000000000000000"  // 100 + 1
               "00000000000000000000000000000000000000000000000000"  // 150 + 1
               "00000000000000000000000000000000000000000000000000"  // 200 + 1
               "00000000000000000000000000000000000000000000000000"  // 250 + 1
               "00000000000000000000000000000000000000000000000000"  // 300 + 1
               "000000000.0"),                                       // 311 + 1
#endif

#ifndef BSLS_PLATFORM_OS_AIX
        D(-Limits::max(), 326, 17, "-"                               //     + 1
               "17976931348623157081452742373170435679807056752584"  //  50 + 1
               "49965989174768031572607800285387605895586327668781"  // 100 + 1
               "71540458953514382464234321326889464182768467546703"  // 150 + 1
               "53751698604991057655128207624549009038932894407586"  // 200 + 1
               "85084551339423045832369032229481658085593321233482"  // 250 + 1
               "74797826204144723168738177180919299881250404026184"  // 300 + 1
               "124858368.00000000000000000"),                       // 327 + 1
#else
        // AIX 'sprintf' is "lazy"
        D(-Limits::max(), 326, 17, "-"                               //     + 1
               "17976931348623157081452742373170435680000000000000"  //  50 + 1
               "00000000000000000000000000000000000000000000000000"  // 100 + 1
               "00000000000000000000000000000000000000000000000000"  // 150 + 1
               "00000000000000000000000000000000000000000000000000"  // 200 + 1
               "00000000000000000000000000000000000000000000000000"  // 250 + 1
               "00000000000000000000000000000000000000000000000000"  // 300 + 1
               "000000000.00000000000000000"),                       // 327 + 1
#endif

        D(-Limits::min(), N, N, "-0."                                //     + 3
           "00000000000000000000000000000000000000000000000000"      //  50 + 3
           "00000000000000000000000000000000000000000000000000"      // 100 + 3
           "00000000000000000000000000000000000000000000000000"      // 150 + 3
           "00000000000000000000000000000000000000000000000000"      // 200 + 3
           "00000000000000000000000000000000000000000000000000"      // 250 + 3
           "00000000000000000000000000000000000000000000000000"      // 300 + 3
           "000000022250738585072014"),                              // 324 + 3

        D(-Limits::min(), 326, 17, "-0.00000000000000000"),

            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int   LINE      = DATA[i].d_lineNum;
                const Type  INPUT     = DATA[i].d_input;
                const char *INPUT_STR = DATA[i].d_inputStr;
                const int   TD        = DATA[i].d_maxTotalDigits;
                const int   FD        = DATA[i].d_maxFractionDigits;
                const char *EXPECTED  = DATA[i].d_expected;

                balxml::EncoderOptions options;
                if (TD != N) options.setMaxDecimalTotalDigits(TD);
                if (FD != N) options.setMaxDecimalFractionDigits(FD);

                bsl::stringstream ss;

                Util::printDecimal(ss, INPUT, &options);

                if (bdlb::Float::isNan(INPUT) ||
                    bdlb::Float::isInfinite(INPUT)) {

                    ASSERTV(LINE, INPUT_STR, ss.str(), !ss);
                }
                else {
                    ASSERTV(LINE, INPUT_STR, ss.str(), EXPECTED,
                            verifyFloatingPointResult(ss.str(), EXPECTED));
                }
            }
        }
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING 'printBase64' FUNCTIONS
        //
        // Concerns:
        //
        // Plan:
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTESTING 'printBase64' FUNCTIONS"
                          << "\n===============================" << endl;

        static const struct {
            int         d_lineNum;
            const char *d_input;
            const char *d_result;
        } DATA[] = {
            //line    input       result
            //----    -----       ------
            { L_,     "",         ""              },
            { L_,     "a",        "YQ=="          },
            { L_,     "ab",       "YWI="          },
            { L_,     "abc",      "YWJj"          },
            { L_,     "abcd",     "YWJjZA=="      },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        if (verbose) cout << "\nUsing 'bsl::string'." << endl;
        {
            typedef bsl::string Type;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int          LINE          = DATA[i].d_lineNum;
                const char        *INPUT         = DATA[i].d_input;
                const char        *RESULT        = DATA[i].d_result;
                const bsl::size_t  INPUT_LENGTH  = bsl::strlen(INPUT);

                Type mX(INPUT, INPUT + INPUT_LENGTH); const Type& X = mX;
                bsl::stringstream ss;

                Util::printBase64(ss, X);

                ASSERTV(LINE, ss.str(), RESULT == ss.str());
            }
        }

        if (verbose) cout << "\nUsing 'bslstl::StringRef'." << endl;
        {
            typedef bslstl::StringRef Type;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int          LINE          = DATA[i].d_lineNum;
                const char        *INPUT         = DATA[i].d_input;
                const char        *RESULT        = DATA[i].d_result;
                const bsl::size_t  INPUT_LENGTH  = bsl::strlen(INPUT);

                Type mX(INPUT, INPUT + INPUT_LENGTH); const Type& X = mX;
                bsl::stringstream ss;

                Util::printBase64(ss, X);

                ASSERTV(LINE, ss.str(), RESULT == ss.str());
            }
        }

        if (verbose) cout << "\nUsing 'MyStringRef'." << endl;
        {
            typedef MyStringRef Type;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int          LINE          = DATA[i].d_lineNum;
                const char        *INPUT         = DATA[i].d_input;
                const char        *RESULT        = DATA[i].d_result;
                const bsl::size_t  INPUT_LENGTH  = bsl::strlen(INPUT);

                Type mX(INPUT, INPUT + INPUT_LENGTH); const Type& X = mX;
                bsl::stringstream ss;

                Util::printBase64(ss, X);

                ASSERTV(LINE, ss.str(), RESULT == ss.str());
            }
        }

        if (verbose) cout << "\nUsing 'bsl::vector<char>'." << endl;
        {
            typedef bsl::vector<char> Type;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int          LINE          = DATA[i].d_lineNum;
                const char        *INPUT         = DATA[i].d_input;
                const char        *RESULT        = DATA[i].d_result;
                const bsl::size_t  INPUT_LENGTH  = bsl::strlen(INPUT);

                Type mX(INPUT, INPUT + INPUT_LENGTH); const Type& X = mX;
                bsl::stringstream ss;

                Util::printBase64(ss, X);

                ASSERTV(LINE, ss.str(), RESULT == ss.str());
            }
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
        //: 1 Use a string streams as output.
        //: 2 Write some scalar values in different formatting modes.
        //: 3 Write 'vector<char>' (stand-in for "binary data) in Base64 & Hex.
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) cout << "\nBREATHING TEST"
                          << "\n==============" << endl;

        {
            bsl::stringstream ss;

            balxml::TypesPrintUtil::print(ss,
                                          TestEnum::VALUE2,
                                          bdlat_FormattingMode::e_DEFAULT);
            ASSERTV(ss.str(), "VALUE2" == ss.str());
        }

        {
            bsl::stringstream ss;

            balxml::TypesPrintUtil::print(ss,
                                          TestEnum::VALUE2,
                                          bdlat_FormattingMode::e_DEC);
            ASSERTV(ss.str(), "2" == ss.str());
        }

        {
            bsl::vector<char> vec;
            bsl::stringstream ss;

            vec.push_back('a');
            vec.push_back('b');
            vec.push_back('c');
            vec.push_back('d');

            balxml::TypesPrintUtil::printBase64(ss, vec);

            ASSERTV(ss.str(), "YWJjZA==" == ss.str());
        }

        {
            bsl::vector<char> vec;
            bsl::stringstream ss;

            vec.push_back('a');
            vec.push_back('b');
            vec.push_back('c');
            vec.push_back('d');

            balxml::TypesPrintUtil::printHex(ss, vec);

            ASSERTV(ss.str(), "61626364" == ss.str());
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
// Copyright 2017 Bloomberg Finance L.P.
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
