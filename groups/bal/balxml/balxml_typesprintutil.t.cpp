// balxml_typesprintutil.t.cpp                                        -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <balxml_typesprintutil.h>

#include <bslim_testutil.h>

#include <bdlat_enumeratorinfo.h>
#include <bdlat_enumfunctions.h>
#include <bdlat_typetraits.h>
#include <bdlat_valuetypefunctions.h>

#include <bdlb_chartype.h>
#include <bdlb_float.h>
#include <bdlb_nullablevalue.h>
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
#include <bsl_vector.h>

#include <bslalg_typetraits.h>

#include <bslma_allocator.h>

#include <bsls_assert.h>
#include <bsls_platform.h>
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
//                   GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
// ----------------------------------------------------------------------------

typedef balxml::TypesPrintUtil Util;

                              // ===============
                              // struct TestEnum
                              // ===============

namespace TestNamespace {

struct TestEnum {
    enum Value {
        VALUE1= 1,
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
        // the number of enumerators in the 'Value' enumeration
        NUM_ENUMERATORS = 2
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
int MyEnumeration::fromInt(Value *result, int number)
{
    enum { SUCCESS = 0, NOT_FOUND = 1 };

    switch (number) {
      case VALUE1:
      case VALUE2:
        *result = (Value)number;
        return SUCCESS;                                               // RETURN
      default:
        return NOT_FOUND;                                             // RETURN
    }
}

inline
bsl::ostream& MyEnumeration::print(bsl::ostream& stream, Value value)
{
    return stream << toString(value);
}

// ----------------------------------------------------------------------------

// CLASS METHODS
inline
const char *MyEnumeration::toString(Value value)
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
bsl::ostream& test::operator<<(bsl::ostream& stream, MyEnumeration::Value rhs)
{
    return MyEnumeration::print(stream, rhs);
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
        VALUE1,
        "VALUE1",                      // name
        sizeof("VALUE1") - 1,          // name length
        "todo: provide annotation"  // annotation
    },
    {
        VALUE2,
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
            if (bdlb::CharType::toUpper(string[0])=='V' &&
                bdlb::CharType::toUpper(string[1])=='A' &&
                bdlb::CharType::toUpper(string[2])=='L' &&
                bdlb::CharType::toUpper(string[3])=='U' &&
                bdlb::CharType::toUpper(string[4])=='E') {
                switch(bdlb::CharType::toUpper(string[5])) {
                    case '1': {
                        *result = MyEnumeration::VALUE1;
                        return SUCCESS;                               // RETURN
                    } break;
                    case '2': {
                        *result = MyEnumeration::VALUE2;
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
//  CustomizedInt: a customized int
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

bool testFloatPointResult(const char *result, const char *expected)
{
    int lenRes = bsl::strlen(result);
    int lenExp = bsl::strlen(expected);
    if (lenRes != lenExp) {
        return false;                                                 // RETURN
    }

    for (int i=0; i < lenExp; ++i) {
        if (expected[i] == 'X') {
            continue;
        }
        if (expected[i] != result[i]) {
            return false;                                             // RETURN
        }
    }
    return true;

    //const char *pos = bsl::strchr(expected,'X');
    //
    //if (pos == 0) {  // no pattern matching, exact compare
    //    return (0 == bsl::strcmp(result, expected));
    //}

    //int lenCmp = pos - expected;
    //
    //return (lenRes <= lenExp) &&
    //       (lenRes >= lenCmp) &&
    //       (0 == bsl::memcmp(result, expected, lenCmp));
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
    LOOP2_ASSERT(EXPECTED_RESULT, ss.str(), EXPECTED_RESULT == ss.str());
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
    LOOP2_ASSERT(EXPECTED_INVALID_RESULT, ss.str(),
                 EXPECTED_INVALID_RESULT == ss.str());
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

        if (verbose) cout << "\nTESTING USAGE EXAMPLES"
                          << "\n======================" << endl;

        usageExample1();

        usageExample2();

        if (verbose) cout << "\nEnd of Test." << endl;
      } break;
      case 8: {
        // --------------------------------------------------------------------
        // TESTING 'print' FUNCTION
        //
        // Concerns:
        //
        // Plan:
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting 'print' Function"
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
                const int   LINE          = DATA[i].d_lineNum;
                const char *INPUT         = DATA[i].d_input;
                const char *RESULT        = DATA[i].d_result;
                const int   INPUT_LENGTH  = bsl::strlen(INPUT);

                Type mX(INPUT, INPUT + INPUT_LENGTH); const Type& X = mX;
                bsl::stringstream ss;

                Util::print(ss, X, bdlat_FormattingMode::e_BASE64);

                LOOP2_ASSERT(LINE, ss.str(), RESULT == ss.str());
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

                LOOP2_ASSERT(LINE, ss.str(), RESULT == ss.str());
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

                LOOP2_ASSERT(LINE, ss.str(), RESULT == ss.str());
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

                LOOP2_ASSERT(LINE, ss.str(), RESULT == ss.str());
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
                const int   LINE          = DATA[i].d_lineNum;
                const char *INPUT         = DATA[i].d_input;
                const char *RESULT        = DATA[i].d_result;
                const int   INPUT_LENGTH  = bsl::strlen(INPUT);

                Type mX(INPUT, INPUT + INPUT_LENGTH); const Type& X = mX;
                bsl::stringstream ss;

                Util::print(ss, X, bdlat_FormattingMode::e_HEX);

                LOOP2_ASSERT(LINE, ss.str(), RESULT == ss.str());
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

                LOOP2_ASSERT(LINE, ss.str(), RESULT == ss.str());

                ss.str("");

                Util::print(ss, X, bdlat_FormattingMode::e_LIST |
                                   bdlat_FormattingMode::e_DEC);

                LOOP2_ASSERT(LINE, ss.str(), RESULT == ss.str());
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

                LOOP2_ASSERT(LINE, ss.str(), RESULT == ss.str());
            }
        }

        if (verbose) cout << "\nEnd of Test." << endl;
      } break;
      case 7: {
        // --------------------------------------------------------------------
        // TESTING 'printDefault' FUNCTIONS
        //
        // Concerns:
        //
        // Plan:
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting 'printDefault' Functions"
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

                LOOP2_ASSERT(LINE, ss.str(), RESULT == ss.str());
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

                LOOP2_ASSERT(LINE, ss.str(), RESULT == ss.str());
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

                LOOP2_ASSERT(LINE, ss.str(), RESULT == ss.str());
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

                LOOP2_ASSERT(LINE, ss.str(), RESULT == ss.str());
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
#if defined(BSLS_PLATFORM_CPU_64_BIT) && !defined(BSLS_PLATFORM_OS_WINDOWS)
                //line    input                     result
                //----    -----                     ------
                { L_,     static_cast<Type>(-9223372036854775808ULL),
                                                    "-9223372036854775808"   },
                { L_,     -9223372036854775807LL,   "-9223372036854775807"   },
                { L_,     -1LL,                     "-1"                     },
                { L_,     0LL,                      "0"                      },
                { L_,     1LL,                      "1"                      },
                { L_,     9223372036854775806LL,    "9223372036854775806"    },
                { L_,     9223372036854775807LL,    "9223372036854775807"    },
#else
                //line    input                     result
                //----    -----                     ------
                { L_,     -2147483647-1,            "-2147483648"   },
                { L_,     -2147483647,              "-2147483647"   },
                { L_,     -1,                       "-1"            },
                { L_,     0,                        "0"             },
                { L_,     1,                        "1"             },
                { L_,     2147483646,               "2147483646"    },
                { L_,     2147483647,               "2147483647"    },
#endif
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int   LINE   = DATA[i].d_lineNum;
                const Type  INPUT  = DATA[i].d_input;
                const char *RESULT = DATA[i].d_result;

                bsl::stringstream ss;

                Util::printDefault(ss, INPUT);

                LOOP2_ASSERT(LINE, ss.str(), RESULT == ss.str());
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
                //line    input                     result
                //----    -----                     ------
                { L_,     static_cast<Type>(-9223372036854775808ULL),
                                                    "-9223372036854775808"   },
                { L_,     -9223372036854775807LL,   "-9223372036854775807"   },
                { L_,     -1LL,                     "-1"                     },
                { L_,     0LL,                      "0"                      },
                { L_,     1LL,                      "1"                      },
                { L_,     9223372036854775806LL,    "9223372036854775806"    },
                { L_,     9223372036854775807LL,    "9223372036854775807"    },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int   LINE   = DATA[i].d_lineNum;
                const Type  INPUT  = DATA[i].d_input;
                const char *RESULT = DATA[i].d_result;

                bsl::stringstream ss;

                Util::printDefault(ss, INPUT);

                LOOP2_ASSERT(LINE, ss.str(), RESULT == ss.str());
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

                LOOP2_ASSERT(LINE, ss.str(), RESULT == ss.str());
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

                LOOP2_ASSERT(LINE, ss.str(), RESULT == ss.str());
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
                { L_,     4294967294,      "4294967294"    },
                { L_,     4294967295,      "4294967295"    },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int   LINE   = DATA[i].d_lineNum;
                const Type  INPUT  = DATA[i].d_input;
                const char *RESULT = DATA[i].d_result;

                bsl::stringstream ss;

                Util::printDefault(ss, INPUT);

                LOOP2_ASSERT(LINE, ss.str(), RESULT == ss.str());
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

                LOOP2_ASSERT(LINE, ss.str(), RESULT == ss.str());
            }
        }

        if (verbose) cout << "\nUsing 'float'." << endl;
        {
            typedef float Type;

#if defined(BSLS_PLATFORM_CMP_MSVC)
#define ZERO   "0"
#else
#define ZERO
#endif

            static const struct {
                int         d_lineNum;
                Type        d_input;
                const char *d_result;
            } DATA[] = {
                //line    input           result
                //----    -----           ------
                { L_,     -1.0f,          "-1"                            },
                { L_,     -0.1f,          "-0.1"                          },
                { L_,     -0.1234567f,    "-0.1234567"                    },
                { L_,     -1.234567e-35,  "-1.234567e-" ZERO "35"         },
                { L_,     0.0f,           "0"                             },
                { L_,     0.1f,           "0.1"                           },
                { L_,     1.0f,           "1"                             },
                { L_,     1234567.0f,     "1234567"                       },
                { L_,     1.234567e35,    "1.234567e+" ZERO "35"          },
                { L_,     bsl::numeric_limits<float>::infinity(),
                                          "+INF"                          },
                { L_,    -bsl::numeric_limits<float>::infinity(),
                                          "-INF"                          },
                { L_,     bsl::numeric_limits<float>::signaling_NaN(),
                                          "NaN"                           },
            };
#undef ZERO
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int   LINE   = DATA[i].d_lineNum;
                const Type  INPUT  = DATA[i].d_input;
                const char *RESULT = DATA[i].d_result;

                bsl::stringstream ss;

                Util::printDefault(ss, INPUT);

                LOOP2_ASSERT(LINE, ss.str(), RESULT == ss.str());
            }
        }

        if (verbose) cout << "\nUsing 'double'." << endl;
        {
            typedef double Type;

            static const struct {
                int         d_lineNum;
                Type        d_input;
                const char *d_result;
            } DATA[] = {
                //line    input       result
                //----    -----       ------
                { L_,     -1.0,       "-1"             },
                { L_,     -0.1,       "-0.1"           },
                { L_,-0.123456789012345,"-0.123456789012345"  },
                { L_,-1.23456789012345e-105,"-1.23456789012345e-105"  },
                { L_,     0.0,        "0"              },
                { L_,     0.1,        "0.1"            },
                { L_,     1.0,        "1"              },
                { L_,123456789012345.0, "123456789012345"},
                { L_,1.23456789012345e105,"1.23456789012345e+105"  },
                { L_,  bsl::numeric_limits<double>::infinity(), "+INF"},
                { L_, -bsl::numeric_limits<double>::infinity(), "-INF"},
                { L_,  bsl::numeric_limits<double>::signaling_NaN(), "NaN"},
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int   LINE   = DATA[i].d_lineNum;
                const Type  INPUT  = DATA[i].d_input;
                const char *RESULT = DATA[i].d_result;

                bsl::stringstream ss;

                Util::printDefault(ss, INPUT);

                LOOP2_ASSERT(LINE, ss.str(), RESULT == ss.str());
            }
        }

        // TBD: Currently this test case is commented out till the
        // implementation uses the encoderOptions that are passed in.
#if 0
        if (verbose) cout << "\nUsing 'double' with encoder options." << endl;
        {
            typedef double Type;

            static const struct {
                int         d_lineNum;
                Type        d_input;
                int         d_totalDigits;
                int         d_fractionDigits;
                const char *d_result;
            } DATA[] = {
                // No fractional digits
                {
                    L_,                       // line
                    12345,                    // input
                    0,                        // num total digits
                    0,                        // num fractions digits
                    "12345"                   // output
                },
                {
                    L_,                       // line
                    12345,                    // input
                    4,                        // num total digits
                    2,                        // num fractions digits
                    "12345"                   // output
                },
                {
                    L_,                       // line
                    12345,                    // input
                    5,                        // num total digits
                    2,                        // num fractions digits
                    "12345"                   // output
                },
                {
                    L_,                       // line
                    12345,                    // input
                    6,                        // num total digits
                    2,                        // num fractions digits
                    "12345"                   // output
                },
                {
                    L_,                       // line
                    12345,                    // input
                    7,                        // num total digits
                    2,                        // num fractions digits
                    "12345"                   // output
                },
                // Fractional digits
                {
                    L_,                       // line
                    -1.1,                     // input
                    0,                        // num total digits
                    0,                        // num fractions digits
                    "-1"                      // output
                },
                {
                    L_,                       // line
                    -1.1,                     // input
                    0,                        // num total digits
                    1,                        // num fractions digits
                    "-1"                      // output
                },
                {
                    L_,                       // line
                    -1.1,                     // input
                    0,                        // num total digits
                    2,                        // num fractions digits
                    "-1"                      // output
                },
                {
                    L_,                       // line
                    -1.1,                     // input
                    0,                        // num total digits
                    3,                        // num fractions digits
                    "-1"                      // output
                },
                {
                    L_,                       // line
                    -1.1,                     // input
                    1,                        // num total digits
                    1,                        // num fractions digits
                    "-1"                      // output
                },
                {
                    L_,                       // line
                    -1.1,                     // input
                    2,                        // num total digits
                    1,                        // num fractions digits
                    "-1.1"                    // output
                },
                {
                    L_,                       // line
                    -1.1,                     // input
                    3,                        // num total digits
                    1,                        // num fractions digits
                    "-1.1"                    // output
                },
                {
                    L_,                       // line
                    -1.1,                     // input
                    4,                        // num total digits
                    1,                        // num fractions digits
                    "-1.1"                    // output
                },
                {
                    L_,                       // line
                    -1.1,                     // input
                    1,                        // num total digits
                    2,                        // num fractions digits
                    "-1"                      // output
                },
                {
                    L_,                       // line
                    -1.1,                     // input
                    2,                        // num total digits
                    2,                        // num fractions digits
                    "-1.1"                    // output
                },
                {
                    L_,                       // line
                    -1.1,                     // input
                    3,                        // num total digits
                    2,                        // num fractions digits
                    "-1.1"                    // output
                },
                {
                    L_,                       // line
                    -1.1,                     // input
                    4,                        // num total digits
                    2,                        // num fractions digits
                    "-1.1"                    // output
                },
                {
                    L_,                       // line
                    -1.1,                     // input
                    5,                        // num total digits
                    3,                        // num fractions digits
                    "-1.1"                    // output
                },
                {
                    L_,                       // line
                    0.12345,                  // input
                    0,                        // num total digits
                    0,                        // num fractions digits
                    "0"                       // output
                },
                {
                    L_,                       // line
                    0.12345,                  // input
                    0,                        // num total digits
                    1,                        // num fractions digits
                    "0"                       // output
                },
                {
                    L_,                       // line
                    0.12345,                  // input
                    0,                        // num total digits
                    2,                        // num fractions digits
                    "0"                       // output
                },
                {
                    L_,                       // line
                    0.12345,                  // input
                    1,                        // num total digits
                    0,                        // num fractions digits
                    "0"                       // output
                },
                {
                    L_,                       // line
                    0.12345,                  // input
                    1,                        // num total digits
                    1,                        // num fractions digits
                    "0"                       // output
                },
                {
                    L_,                       // line
                    0.12345,                  // input
                    1,                        // num total digits
                    2,                        // num fractions digits
                    "0"                       // output
                },
                {
                    L_,                       // line
                    0.12345,                  // input
                    2,                        // num total digits
                    0,                        // num fractions digits
                    "0"                       // output
                },
                {
                    L_,                       // line
                    0.12345,                  // input
                    2,                        // num total digits
                    1,                        // num fractions digits
                    "0.1"                     // output
                },
                {
                    L_,                       // line
                    0.12345,                  // input
                    2,                        // num total digits
                    2,                        // num fractions digits
                    "0.1"                     // output
                },
                {
                    L_,                       // line
                    0.12345,                  // input
                    3,                        // num total digits
                    0,                        // num fractions digits
                    "0"                       // output
                },
                {
                    L_,                       // line
                    0.12345,                  // input
                    3,                        // num total digits
                    1,                        // num fractions digits
                    "0.1"                       // output
                },
                {
                    L_,                       // line
                    0.12345,                  // input
                    3,                        // num total digits
                    2,                        // num fractions digits
                    "0.12"                    // output
                },
                {
                    L_,                       // line
                    0.12345,                  // input
                    4,                        // num total digits
                    0,                        // num fractions digits
                    "0"                       // output
                },
                {
                    L_,                       // line
                    0.12345,                  // input
                    4,                        // num total digits
                    1,                        // num fractions digits
                    "0.1"                       // output
                },
                {
                    L_,                       // line
                    0.12345,                  // input
                    4,                        // num total digits
                    2,                        // num fractions digits
                    "0.12"                       // output
                },
                {
                    L_,                       // line
                    0.12345,                  // input
                    4,                        // num total digits
                    3,                        // num fractions digits
                    "0.123"                   // output
                },
                {
                    L_,                       // line
                    0.0,                      // input
                    0,                        // num total digits
                    0,                        // num fractions digits
                    "0"                       // output
                },
                {
                    L_,                       // line
                    0.0,                      // input
                    1,                        // num total digits
                    0,                        // num fractions digits
                    "0"                       // output
                },
                {
                    L_,                       // line
                    0.0,                      // input
                    2,                        // num total digits
                    0,                        // num fractions digits
                    "0"                       // output
                },
                {
                    L_,                       // line
                    0.0,                      // input
                    0,                        // num total digits
                    1,                        // num fractions digits
                    "0"                       // output
                },
                {
                    L_,                       // line
                    0.0,                      // input
                    1,                        // num total digits
                    1,                        // num fractions digits
                    "0"                       // output
                },
                {
                    L_,                       // line
                    0.0,                      // input
                    2,                        // num total digits
                    0,                        // num fractions digits
                    "0"                       // output
                },
                {
                    L_,                       // line
                    0.0,                      // input
                    2,                        // num total digits
                    1,                        // num fractions digits
                    "0"                       // output
                },
                {
                    L_,                       // line
                    0.0,                      // input
                    2,                        // num total digits
                    2,                        // num fractions digits
                    "0"                       // output
                },
                {
                    L_,                       // line
                    -1.23456789012345e-105,   // input
                    0,                        // num total digits
                    0,                        // num fractions digits
                    "0"                       // output
                },
                {
                    L_,                       // line
                    123.123,                  // input
                    0,                        // num total digits
                    0,                        // num fractions digits
                    "123"                     // output
                },
                {
                    L_,                       // line
                    123.123,                  // input
                    1,                        // num total digits
                    0,                        // num fractions digits
                    "123"                     // output
                },
                {
                    L_,                       // line
                    123.123,                  // input
                    2,                        // num total digits
                    0,                        // num fractions digits
                    "123"                     // output
                },
                {
                    L_,                       // line
                    123.123,                  // input
                    3,                        // num total digits
                    0,                        // num fractions digits
                    "123"                     // output
                },
                {
                    L_,                       // line
                    123.123,                  // input
                    4,                        // num total digits
                    0,                        // num fractions digits
                    "123"                     // output
                },
                {
                    L_,                       // line
                    123.123,                  // input
                    5,                        // num total digits
                    0,                        // num fractions digits
                    "123"                     // output
                },
                {
                    L_,                       // line
                    123.123,                  // input
                    0,                        // num total digits
                    1,                        // num fractions digits
                    "123"                     // output
                },
                {
                    L_,                       // line
                    123.123,                  // input
                    1,                        // num total digits
                    1,                        // num fractions digits
                    "123"                     // output
                },
                {
                    L_,                       // line
                    123.123,                  // input
                    2,                        // num total digits
                    2,                        // num fractions digits
                    "123"                     // output
                },
                {
                    L_,                       // line
                    123.123,                  // input
                    3,                        // num total digits
                    3,                        // num fractions digits
                    "123"                     // output
                },
                {
                    L_,                       // line
                    123.123,                  // input
                    4,                        // num total digits
                    4,                        // num fractions digits
                    "123.1"                   // output
                },
                {
                    L_,                       // line
                    123.123,                  // input
                    4,                        // num total digits
                    3,                        // num fractions digits
                    "123.1"                   // output
                },
                {
                    L_,                       // line
                    123.123,                  // input
                    4,                        // num total digits
                    2,                        // num fractions digits
                    "123.1"                   // output
                },
                {
                    L_,                       // line
                    123.123,                  // input
                    4,                        // num total digits
                    1,                        // num fractions digits
                    "123.1"                   // output
                },
                {
                    L_,                       // line
                    123.123,                  // input
                    4,                        // num total digits
                    0,                        // num fractions digits
                    "123"                     // output
                },
                {
                    L_,                       // line
                    123456789012345.0,        // input
                    0,                        // num total digits
                    0,                        // num fractions digits
                    "123456789012345"         // output
                },
                {
                    L_,                       // line
                    -1.1,                     // input
                    0,                        // num total digits
                    0,                        // num fractions digits
                    "-1"                      // output
                },
                {
                    L_,                       // line
                    -0.123456789012345,       // input
                    0,                        // num total digits
                    0,                        // num fractions digits
                    "0"                       // output
                },
                {
                    L_,                       // line
                    -1.23456789012345e-105,   // input
                    0,                        // num total digits
                    0,                        // num fractions digits
                    "0"                       // output
                },
                {
                    L_,                       // line
                    0.0,                      // input
                    0,                        // num total digits
                    0,                        // num fractions digits
                    "0"                       // output
                },
                {
                    L_,                       // line
                    123.123,                  // input
                    0,                        // num total digits
                    0,                        // num fractions digits
                    "123"                     // output
                },
                {
                    L_,                       // line
                    123456789012345.0,        // input
                    0,                        // num total digits
                    0,                        // num fractions digits
                    "123456789012345"         // output
                },

#if BSLS_PLATFORM_OS_SOLARIS
                {
                    L_,                       // line
                    DBL_MAX,                  // input
                    0,                        // num total digits
                    0,                        // num fractions digits
                    "17976931348623157081452742373170435679807056752584499659"
                    "89174768031572607800285387605895586327668781715404589535"
                    "14382464234321326889464182768467546703537516986049910576"
                    "55128207624549009038932894407586850845513394230458323690"
                    "32229481658085593321233482747978262041447231687381771809"
                    "19299881250404026184124858368"  // output
                },
                {
                    L_,                       // line
                    DBL_MAX,                  // input
                    1,                        // num total digits
                    0,                        // num fractions digits
                    "17976931348623157081452742373170435679807056752584499659"
                    "89174768031572607800285387605895586327668781715404589535"
                    "14382464234321326889464182768467546703537516986049910576"
                    "55128207624549009038932894407586850845513394230458323690"
                    "32229481658085593321233482747978262041447231687381771809"
                    "19299881250404026184124858368"  // output
                },
                {
                    L_,                       // line
                    DBL_MAX,                  // input
                    100,                        // num total digits
                    0,                        // num fractions digits
                    "17976931348623157081452742373170435679807056752584499659"
                    "89174768031572607800285387605895586327668781715404589535"
                    "14382464234321326889464182768467546703537516986049910576"
                    "55128207624549009038932894407586850845513394230458323690"
                    "32229481658085593321233482747978262041447231687381771809"
                    "19299881250404026184124858368"  // output
                },
                {
                    L_,                       // line
                    DBL_MAX,                  // input
                    400,                        // num total digits
                    0,                        // num fractions digits
                    "17976931348623157081452742373170435679807056752584499659"
                    "89174768031572607800285387605895586327668781715404589535"
                    "14382464234321326889464182768467546703537516986049910576"
                    "55128207624549009038932894407586850845513394230458323690"
                    "32229481658085593321233482747978262041447231687381771809"
                    "19299881250404026184124858368" // output
                },
                {
                    L_,                       // line
                    DBL_MAX,                  // input
                    0,                        // num total digits
                    1,                        // num fractions digits
                    "17976931348623157081452742373170435679807056752584499659"
                    "89174768031572607800285387605895586327668781715404589535"
                    "14382464234321326889464182768467546703537516986049910576"
                    "55128207624549009038932894407586850845513394230458323690"
                    "32229481658085593321233482747978262041447231687381771809"
                    "19299881250404026184124858368"  // output
                },
                {
                    L_,                       // line
                    DBL_MAX,                  // input
                    1,                        // num total digits
                    1,                        // num fractions digits
                    "17976931348623157081452742373170435679807056752584499659"
                    "89174768031572607800285387605895586327668781715404589535"
                    "14382464234321326889464182768467546703537516986049910576"
                    "55128207624549009038932894407586850845513394230458323690"
                    "32229481658085593321233482747978262041447231687381771809"
                    "19299881250404026184124858368"  // output
                },
                {
                    L_,                       // line
                    DBL_MAX,                  // input
                    100,                        // num total digits
                    1,                        // num fractions digits
                    "17976931348623157081452742373170435679807056752584499659"
                    "89174768031572607800285387605895586327668781715404589535"
                    "14382464234321326889464182768467546703537516986049910576"
                    "55128207624549009038932894407586850845513394230458323690"
                    "32229481658085593321233482747978262041447231687381771809"
                    "19299881250404026184124858368" // output
                },
                {
                    L_,                       // line
                    DBL_MAX,                  // input
                    400,                        // num total digits
                    1,                        // num fractions digits
                    "17976931348623157081452742373170435679807056752584499659"
                    "89174768031572607800285387605895586327668781715404589535"
                    "14382464234321326889464182768467546703537516986049910576"
                    "55128207624549009038932894407586850845513394230458323690"
                    "32229481658085593321233482747978262041447231687381771809"
                    "19299881250404026184124858368"  // output
                },
                {
                    L_,                       // line
                    DBL_MAX,                  // input
                    0,                        // num total digits
                    100,                        // num fractions digits
                    "17976931348623157081452742373170435679807056752584499659"
                    "89174768031572607800285387605895586327668781715404589535"
                    "14382464234321326889464182768467546703537516986049910576"
                    "55128207624549009038932894407586850845513394230458323690"
                    "32229481658085593321233482747978262041447231687381771809"
                    "19299881250404026184124858368"  // output
                },
                {
                    L_,                       // line
                    DBL_MAX,                  // input
                    1,                        // num total digits
                    100,                        // num fractions digits
                    "17976931348623157081452742373170435679807056752584499659"
                    "89174768031572607800285387605895586327668781715404589535"
                    "14382464234321326889464182768467546703537516986049910576"
                    "55128207624549009038932894407586850845513394230458323690"
                    "32229481658085593321233482747978262041447231687381771809"
                    "19299881250404026184124858368" // output
                },
                {
                    L_,                       // line
                    DBL_MAX,                  // input
                    100,                        // num total digits
                    100,                        // num fractions digits
                    "17976931348623157081452742373170435679807056752584499659"
                    "89174768031572607800285387605895586327668781715404589535"
                    "14382464234321326889464182768467546703537516986049910576"
                    "55128207624549009038932894407586850845513394230458323690"
                    "32229481658085593321233482747978262041447231687381771809"
                    "19299881250404026184124858368" // output
                },
                {
                    L_,                       // line
                    DBL_MAX,                  // input
                    400,                        // num total digits
                    100,                        // num fractions digits
                    "17976931348623157081452742373170435679807056752584499659"
                    "89174768031572607800285387605895586327668781715404589535"
                    "14382464234321326889464182768467546703537516986049910576"
                    "55128207624549009038932894407586850845513394230458323690"
                    "32229481658085593321233482747978262041447231687381771809"
                    "19299881250404026184124858368" // output
                },
#else BSLS_PLATFORM_OS_AIX
                {
                    L_,                       // line
                    DBL_MAX,                  // input
                    0,                        // num total digits
                    0,                        // num fractions digits
                    "17976931348623157081452742373200000000000000000000000000"
                    "00000000000000000000000000000000000000000000000000000000"
                    "00000000000000000000000000000000000000000000000000000000"
                    "00000000000000000000000000000000000000000000000000000000"
                    "00000000000000000000000000000000000000000000000000000000"
                    "00000000000000000000000000000"  // output
                },
                {
                    L_,                       // line
                    DBL_MAX,                  // input
                    1,                        // num total digits
                    0,                        // num fractions digits
                    "17976931348623157081452742373200000000000000000000000000"
                    "00000000000000000000000000000000000000000000000000000000"
                    "00000000000000000000000000000000000000000000000000000000"
                    "00000000000000000000000000000000000000000000000000000000"
                    "00000000000000000000000000000000000000000000000000000000"
                    "00000000000000000000000000000"  // output
                },
                {
                    L_,                       // line
                    DBL_MAX,                  // input
                    100,                        // num total digits
                    0,                        // num fractions digits
                    "17976931348623157081452742373200000000000000000000000000"
                    "00000000000000000000000000000000000000000000000000000000"
                    "00000000000000000000000000000000000000000000000000000000"
                    "00000000000000000000000000000000000000000000000000000000"
                    "00000000000000000000000000000000000000000000000000000000"
                    "00000000000000000000000000000"  // output
                },
                {
                    L_,                       // line
                    DBL_MAX,                  // input
                    400,                        // num total digits
                    0,                        // num fractions digits
                    "17976931348623157081452742373200000000000000000000000000"
                    "00000000000000000000000000000000000000000000000000000000"
                    "00000000000000000000000000000000000000000000000000000000"
                    "00000000000000000000000000000000000000000000000000000000"
                    "00000000000000000000000000000000000000000000000000000000"
                    "00000000000000000000000000000"  // output
                },
                {
                    L_,                       // line
                    DBL_MAX,                  // input
                    0,                        // num total digits
                    1,                        // num fractions digits
                    "17976931348623157081452742373200000000000000000000000000"
                    "00000000000000000000000000000000000000000000000000000000"
                    "00000000000000000000000000000000000000000000000000000000"
                    "00000000000000000000000000000000000000000000000000000000"
                    "00000000000000000000000000000000000000000000000000000000"
                    "00000000000000000000000000000"  // output
                },
                {
                    L_,                       // line
                    DBL_MAX,                  // input
                    1,                        // num total digits
                    1,                        // num fractions digits
                    "17976931348623157081452742373200000000000000000000000000"
                    "00000000000000000000000000000000000000000000000000000000"
                    "00000000000000000000000000000000000000000000000000000000"
                    "00000000000000000000000000000000000000000000000000000000"
                    "00000000000000000000000000000000000000000000000000000000"
                    "00000000000000000000000000000"  // output
                },
                {
                    L_,                       // line
                    DBL_MAX,                  // input
                    100,                        // num total digits
                    1,                        // num fractions digits
                    "17976931348623157081452742373200000000000000000000000000"
                    "00000000000000000000000000000000000000000000000000000000"
                    "00000000000000000000000000000000000000000000000000000000"
                    "00000000000000000000000000000000000000000000000000000000"
                    "00000000000000000000000000000000000000000000000000000000"
                    "00000000000000000000000000000"  // output
                },
                {
                    L_,                       // line
                    DBL_MAX,                  // input
                    400,                        // num total digits
                    1,                        // num fractions digits
                    "17976931348623157081452742373200000000000000000000000000"
                    "00000000000000000000000000000000000000000000000000000000"
                    "00000000000000000000000000000000000000000000000000000000"
                    "00000000000000000000000000000000000000000000000000000000"
                    "00000000000000000000000000000000000000000000000000000000"
                    "00000000000000000000000000000"  // output
                },
                {
                    L_,                       // line
                    DBL_MAX,                  // input
                    0,                        // num total digits
                    100,                        // num fractions digits
                    "17976931348623157081452742373200000000000000000000000000"
                    "00000000000000000000000000000000000000000000000000000000"
                    "00000000000000000000000000000000000000000000000000000000"
                    "00000000000000000000000000000000000000000000000000000000"
                    "00000000000000000000000000000000000000000000000000000000"
                    "00000000000000000000000000000"  // output
                },
                {
                    L_,                       // line
                    DBL_MAX,                  // input
                    1,                        // num total digits
                    100,                        // num fractions digits
                    "17976931348623157081452742373200000000000000000000000000"
                    "00000000000000000000000000000000000000000000000000000000"
                    "00000000000000000000000000000000000000000000000000000000"
                    "00000000000000000000000000000000000000000000000000000000"
                    "00000000000000000000000000000000000000000000000000000000"
                    "00000000000000000000000000000"  // output
                },
                {
                    L_,                       // line
                    DBL_MAX,                  // input
                    100,                        // num total digits
                    100,                        // num fractions digits
                    "17976931348623157081452742373200000000000000000000000000"
                    "00000000000000000000000000000000000000000000000000000000"
                    "00000000000000000000000000000000000000000000000000000000"
                    "00000000000000000000000000000000000000000000000000000000"
                    "00000000000000000000000000000000000000000000000000000000"
                    "00000000000000000000000000000"  // output
                },
                {
                    L_,                       // line
                    DBL_MAX,                  // input
                    400,                        // num total digits
                    100,                        // num fractions digits
                    "17976931348623157081452742373200000000000000000000000000"
                    "00000000000000000000000000000000000000000000000000000000"
                    "00000000000000000000000000000000000000000000000000000000"
                    "00000000000000000000000000000000000000000000000000000000"
                    "00000000000000000000000000000000000000000000000000000000"
                    "00000000000000000000000000000"  // output
                },
#endif //#if BSLS_PLATFORM_OS_SOLARIS
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int   LINE   = DATA[i].d_lineNum;
                const Type  INPUT  = DATA[i].d_input;
                const int   TOTAL  = DATA[i].d_totalDigits;
                const int   FRACTION = DATA[i].d_fractionDigits;
                const char *RESULT = DATA[i].d_result;

                balxml::EncoderOptions options;
                options.setMaxDecimalTotalDigits(TOTAL);
                options.setMaxDecimalFractionDigits(FRACTION);

                bsl::stringstream ss;

                Util::printDefault(ss, INPUT, &options);

                LOOP3_ASSERT(LINE, ss.str(), RESULT, RESULT == ss.str());
            }
        }

#endif // #if 0

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

                LOOP2_ASSERT(LINE, ss.str(), RESULT == ss.str());
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

                LOOP2_ASSERT(LINE, ss.str(), RESULT == ss.str());
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

                LOOP2_ASSERT(LINE, ss.str(), RESULT == ss.str());
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
                const int   LINE          = DATA[i].d_lineNum;
                const char *INPUT         = DATA[i].d_input;
                const char *RESULT        = DATA[i].d_result;
                const int   INPUT_LENGTH  = bsl::strlen(INPUT);

                Type mX(INPUT, INPUT + INPUT_LENGTH); const Type& X = mX;
                bsl::stringstream ss;

                Util::printDefault(ss, X);

                LOOP2_ASSERT(LINE, ss.str(), RESULT == ss.str());
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

                LOOP2_ASSERT(LINE, ss.str(), RESULT == ss.str());
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

                LOOP2_ASSERT(LINE, ss.str(), RESULT == ss.str());
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

                LOOP2_ASSERT(LINE, ss.str(), RESULT == ss.str());
            }
        }

        if (verbose) cout << "\nEnd of Test." << endl;
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

        if (verbose) cout << "\nTesting 'printText' Functions"
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

                LOOP2_ASSERT(LINE, ss.str(), RESULT == ss.str());
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

                LOOP3_ASSERT(LINE, RESULT, ss.str(), RESULT == ss.str());
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

                LOOP3_ASSERT(LINE, RESULT, ss.str(), RESULT == ss.str());
                LOOP_ASSERT(LINE, ss.fail());
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
                    LOOP3_ASSERT(LINE, RESULT, ss.str(), RESULT == ss.str());
                    LOOP_ASSERT(LINE, ss.good());

                    char mINPUT2[64];             const char *INPUT2 = mINPUT2;
                    bsl::strcpy(mINPUT2, HEADER.c_str());
                    bsl::strcat(mINPUT2, INPUT);
                    char mRESULT2[64];          const char *RESULT2 = mRESULT2;
                    bsl::strcpy(mRESULT2, HEADER.c_str());
                    bsl::strcat(mRESULT2, RESULT);
                    ss.str("");
                    Util::printText(ss, INPUT2);
                    LOOP3_ASSERT(LINE, RESULT2, ss.str(), RESULT2 == ss.str());
                    LOOP_ASSERT(LINE, ss.good());

                    char mINPUT3[64];             const char *INPUT3 = mINPUT3;
                    bsl::strcpy(mINPUT3, INPUT);
                    bsl::strcat(mINPUT3, TRAILER.c_str());
                    char mRESULT3[64];          const char *RESULT3 = mRESULT3;
                    bsl::strcpy(mRESULT3, RESULT);
                    bsl::strcat(mRESULT3, TRAILER.c_str());
                    ss.str("");
                    Util::printText(ss, INPUT3);
                    LOOP3_ASSERT(LINE, RESULT3, ss.str(), RESULT3 == ss.str());
                    LOOP_ASSERT(LINE, ss.good());

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
                    LOOP3_ASSERT(LINE, RESULT4, ss.str(), RESULT4 == ss.str());
                    LOOP_ASSERT(LINE, ss.good());
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
                    LOOP3_ASSERT(LINE, RESULT, ss.str(), RESULT == ss.str());
                    LOOP_ASSERT(LINE, ss.good());

                    const Type INPUT2 = HEADER + INPUT;
                    const Type RESULT2 = HEADER + RESULT;
                    ss.str("");
                    Util::printText(ss, INPUT2);
                    LOOP3_ASSERT(LINE, RESULT2, ss.str(), RESULT2 == ss.str());
                    LOOP_ASSERT(LINE, ss.good());

                    const Type INPUT3 = INPUT + TRAILER;
                    const Type RESULT3 = RESULT + TRAILER;
                    ss.str("");
                    Util::printText(ss, INPUT3);
                    LOOP3_ASSERT(LINE, RESULT3, ss.str(), RESULT3 == ss.str());
                    LOOP_ASSERT(LINE, ss.good());

                    const Type INPUT4 = HEADER + INPUT + TRAILER;
                    const Type RESULT4 = HEADER + RESULT + TRAILER;
                    ss.str("");
                    Util::printText(ss, INPUT4);
                    LOOP3_ASSERT(LINE, RESULT4, ss.str(), RESULT4 == ss.str());
                    LOOP_ASSERT(LINE, ss.good());
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
                    LOOP3_ASSERT(LINE, RESULT, ss.str(), RESULT == ss.str());
                    LOOP_ASSERT(LINE, ss.good());

                    const bsl::string INPUT2_STR = HEADER + INPUT;
                    const bsl::string RESULT2_STR = HEADER + RESULT;
                    const Type INPUT2 = INPUT2_STR;
                    const Type RESULT2 = RESULT2_STR;
                    ss.str("");
                    Util::printText(ss, INPUT2);
                    LOOP3_ASSERT(LINE, RESULT2, ss.str(), RESULT2 == ss.str());
                    LOOP_ASSERT(LINE, ss.good());

                    const bsl::string INPUT3_STR = INPUT + TRAILER;
                    const bsl::string RESULT3_STR = RESULT + TRAILER;
                    const Type INPUT3 = INPUT3_STR;
                    const Type RESULT3 = RESULT3_STR;
                    ss.str("");
                    Util::printText(ss, INPUT3);
                    LOOP3_ASSERT(LINE, RESULT3, ss.str(), RESULT3 == ss.str());
                    LOOP_ASSERT(LINE, ss.good());

                    const bsl::string INPUT4_STR = HEADER + INPUT + TRAILER;
                    const bsl::string RESULT4_STR = HEADER + RESULT + TRAILER;
                    const Type INPUT4 = INPUT4_STR;
                    const Type RESULT4 = RESULT4_STR;
                    ss.str("");
                    Util::printText(ss, INPUT4);
                    LOOP3_ASSERT(LINE, RESULT4, ss.str(), RESULT4 == ss.str());
                    LOOP_ASSERT(LINE, ss.good());
                }
            }

            if (verbose)
                cout << "\nUsing 'vector<char>' on valid strings." << endl;
            {
                typedef bsl::vector<char> Type;

                for (int i = 0; i < NUM_DATA; ++i) {
                    const int LENGTH = bsl::strlen(VALID_DATA[i].d_input);

                    const int         LINE   = VALID_DATA[i].d_lineNum;
                    const char       *CINPUT = VALID_DATA[i].d_input;
                    const Type        INPUT  ( CINPUT, CINPUT+LENGTH );
                    const bsl::string RESULT = VALID_DATA[i].d_result;

                    bsl::stringstream ss;
                    Util::printText(ss, INPUT);
                    LOOP3_ASSERT(LINE, RESULT, ss.str(), RESULT == ss.str());
                    LOOP_ASSERT(LINE, ss.good());

                    Type mINPUT2;                 const Type& INPUT2 = mINPUT2;
                    mINPUT2.insert(mINPUT2.end(),
                                   mHEADER.begin(), mHEADER.end());
                    mINPUT2.insert(mINPUT2.end(), INPUT.begin(), INPUT.end());
                    const bsl::string RESULT2 = HEADER + RESULT;
                    ss.str("");
                    Util::printText(ss, INPUT2);
                    LOOP3_ASSERT(LINE, RESULT2, ss.str(), RESULT2 == ss.str());
                    LOOP_ASSERT(LINE, ss.good());

                    Type mINPUT3;                 const Type& INPUT3 = mINPUT3;
                    mINPUT3.insert(mINPUT3.end(), INPUT.begin(), INPUT.end());
                    mINPUT3.insert(mINPUT3.end(),
                                   mTRAILER.begin(), mTRAILER.end());
                    const bsl::string RESULT3 = RESULT + TRAILER;
                    ss.str("");
                    Util::printText(ss, INPUT3);
                    LOOP3_ASSERT(LINE, RESULT3, ss.str(), RESULT3 == ss.str());
                    LOOP_ASSERT(LINE, ss.good());

                    Type mINPUT4;                 const Type& INPUT4 = mINPUT4;
                    mINPUT4.insert(mINPUT4.end(),
                                   mHEADER.begin(), mHEADER.end());
                    mINPUT4.insert(mINPUT4.end(), INPUT.begin(), INPUT.end());
                    mINPUT4.insert(mINPUT4.end(),
                                   mTRAILER.begin(), mTRAILER.end());
                    const bsl::string RESULT4 = HEADER + RESULT + TRAILER;
                    ss.str("");
                    Util::printText(ss, INPUT4);
                    LOOP3_ASSERT(LINE, RESULT4, ss.str(), RESULT4 == ss.str());
                    LOOP_ASSERT(LINE, ss.good());
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
                    LOOP3_ASSERT(LINE, RESULT, ss.str(), RESULT == ss.str());
                    LOOP_ASSERT(LINE, ss.good());

                    const Type INPUT2 ( HEADER + INPUT.toString() );
                    bsl::string RESULT2 = HEADER + RESULT;
                    ss.str("");
                    Util::printText(ss, INPUT2);
                    LOOP3_ASSERT(LINE, RESULT2, ss.str(), RESULT2 == ss.str());
                    LOOP_ASSERT(LINE, ss.good());

                    const Type INPUT3 ( INPUT.toString() + TRAILER );
                    bsl::string RESULT3 = RESULT + TRAILER;
                    ss.str("");
                    Util::printText(ss, INPUT3);
                    LOOP3_ASSERT(LINE, RESULT3, ss.str(), RESULT3 == ss.str());
                    LOOP_ASSERT(LINE, ss.good());

                    const Type INPUT4 ( HEADER + INPUT.toString() + TRAILER );
                    bsl::string RESULT4 = HEADER + RESULT + TRAILER;
                    ss.str("");
                    Util::printText(ss, INPUT4);
                    LOOP3_ASSERT(LINE, RESULT4, ss.str(), RESULT4 == ss.str());
                    LOOP_ASSERT(LINE, ss.good());
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
                    LOOP3_ASSERT(LINE, RESULT, ss.str(), RESULT == ss.str());
                    LOOP_ASSERT(LINE, ss.fail());

                    char mINPUT2[64];             const char *INPUT2 = mINPUT2;
                    bsl::strcpy(mINPUT2, HEADER.c_str());
                    bsl::strcat(mINPUT2, INPUT);
                    char mRESULT2[64];          const char *RESULT2 = mRESULT2;
                    bsl::strcpy(mRESULT2, HEADER.c_str());
                    ss.clear(); ASSERT(ss.good()); ss.str("");
                    Util::printText(ss, INPUT2);
                    LOOP3_ASSERT(LINE, RESULT2, ss.str(), RESULT2 == ss.str());
                    LOOP_ASSERT(LINE, ss.fail());

                    char mINPUT3[64];             const char *INPUT3 = mINPUT3;
                    bsl::strcpy(mINPUT3, INPUT);
                    bsl::strcat(mINPUT3, TRAILER.c_str());
                    char mRESULT3[64] = "";     const char *RESULT3 = mRESULT3;
                    ss.clear(); ASSERT(ss.good()); ss.str("");
                    Util::printText(ss, INPUT3);
                    LOOP3_ASSERT(LINE, RESULT3, ss.str(), RESULT3 == ss.str());
                    LOOP_ASSERT(LINE, ss.fail());

                    char mINPUT4[64];             const char *INPUT4 = mINPUT4;
                    bsl::strcpy(mINPUT4, HEADER.c_str());
                    bsl::strcat(mINPUT4, INPUT);
                    bsl::strcat(mINPUT4, TRAILER.c_str());
                    char mRESULT4[64];          const char *RESULT4 = mRESULT4;
                    bsl::strcpy(mRESULT4, HEADER.c_str());
                    ss.clear(); ASSERT(ss.good()); ss.str("");
                    Util::printText(ss, INPUT4);
                    LOOP3_ASSERT(LINE, RESULT4, ss.str(), RESULT4 == ss.str());
                    LOOP_ASSERT(LINE, ss.fail());
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
                    LOOP3_ASSERT(LINE, RESULT, ss.str(), RESULT == ss.str());
                    LOOP_ASSERT(LINE, ss.fail());

                    const Type INPUT2 = HEADER + INPUT;
                    const Type RESULT2 = HEADER + Type(RESULT);
                    ss.clear(); ASSERT(ss.good()); ss.str("");
                    Util::printText(ss, INPUT2);
                    LOOP3_ASSERT(LINE, RESULT2, ss.str(), RESULT2 == ss.str());
                    LOOP_ASSERT(LINE, ss.fail());

                    const Type INPUT3 = INPUT + TRAILER;
                    const Type RESULT3 = RESULT;
                    ss.clear(); ASSERT(ss.good()); ss.str("");
                    Util::printText(ss, INPUT3);
                    LOOP3_ASSERT(LINE, RESULT3, ss.str(), RESULT3 == ss.str());
                    LOOP_ASSERT(LINE, ss.fail());

                    const Type INPUT4 = HEADER + INPUT + TRAILER;
                    const Type RESULT4 = HEADER + Type(RESULT);
                    ss.clear(); ASSERT(ss.good()); ss.str("");
                    Util::printText(ss, INPUT4);
                    LOOP3_ASSERT(LINE, RESULT4, ss.str(), RESULT4 == ss.str());
                    LOOP_ASSERT(LINE, ss.fail());
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
                    LOOP3_ASSERT(LINE, RESULT, ss.str(), RESULT == ss.str());
                    LOOP_ASSERT(LINE, ss.fail());

                    const bsl::string INPUT2_STR = HEADER + INPUT;
                    const bsl::string RESULT2_STR = HEADER + RESULT;
                    const Type INPUT2 = INPUT2_STR;
                    const Type RESULT2 = RESULT2_STR;
                    ss.clear(); ASSERT(ss.good()); ss.str("");
                    Util::printText(ss, INPUT2);
                    LOOP3_ASSERT(LINE, RESULT2, ss.str(), RESULT2 == ss.str());
                    LOOP_ASSERT(LINE, ss.fail());

                    const bsl::string INPUT3_STR = INPUT + TRAILER;
                    const Type INPUT3 = INPUT3_STR;
                    const Type RESULT3 = RESULT;
                    ss.clear(); ASSERT(ss.good()); ss.str("");
                    Util::printText(ss, INPUT3);
                    LOOP3_ASSERT(LINE, RESULT3, ss.str(), RESULT3 == ss.str());
                    LOOP_ASSERT(LINE, ss.fail());

                    const bsl::string INPUT4_STR = HEADER + INPUT + TRAILER;
                    const bsl::string RESULT4_STR = HEADER + RESULT;
                    const Type INPUT4 = INPUT4_STR;
                    const Type RESULT4 = RESULT4_STR;
                    ss.clear(); ASSERT(ss.good()); ss.str("");
                    Util::printText(ss, INPUT4);
                    LOOP3_ASSERT(LINE, RESULT4, ss.str(), RESULT4 == ss.str());
                    LOOP_ASSERT(LINE, ss.fail());
                }
            }

            if (verbose)
                cout << "\nUsing 'vector<char>' on invalid strings." << endl;
            {
                typedef bsl::vector<char> Type;

                for (int i = 0; i < NUM_DATA; ++i) {
                    const int LENGTH = bsl::strlen(INVALID_DATA[i].d_input);

                    const int         LINE   = INVALID_DATA[i].d_lineNum;
                    const char       *CINPUT = INVALID_DATA[i].d_input;
                    const Type        INPUT  ( CINPUT, CINPUT+LENGTH );
                    const bsl::string RESULT;

                    bsl::stringstream ss;
                    Util::printText(ss, INPUT);
                    LOOP3_ASSERT(LINE, RESULT, ss.str(), RESULT == ss.str());
                    LOOP_ASSERT(LINE, ss.fail());

                    Type mINPUT2;                 const Type& INPUT2 = mINPUT2;
                    mINPUT2.insert(mINPUT2.end(),
                                   mHEADER.begin(), mHEADER.end());
                    mINPUT2.insert(mINPUT2.end(), INPUT.begin(), INPUT.end());
                    const bsl::string RESULT2 = HEADER;
                    ss.clear(); ASSERT(ss.good()); ss.str("");
                    Util::printText(ss, INPUT2);
                    LOOP3_ASSERT(LINE, RESULT2, ss.str(), RESULT2 == ss.str());
                    LOOP_ASSERT(LINE, ss.fail());

                    Type mINPUT3;                 const Type& INPUT3 = mINPUT3;
                    mINPUT3.insert(mINPUT3.end(), INPUT.begin(), INPUT.end());
                    mINPUT3.insert(mINPUT3.end(),
                                   mTRAILER.begin(), mTRAILER.end());
                    const bsl::string RESULT3;
                    ss.clear(); ASSERT(ss.good()); ss.str("");
                    Util::printText(ss, INPUT3);
                    LOOP3_ASSERT(LINE, RESULT3, ss.str(), RESULT3 == ss.str());
                    LOOP_ASSERT(LINE, ss.fail());

                    Type mINPUT4;                 const Type& INPUT4 = mINPUT4;
                    mINPUT4.insert(mINPUT4.end(),
                                   mHEADER.begin(), mHEADER.end());
                    mINPUT4.insert(mINPUT4.end(), INPUT.begin(), INPUT.end());
                    mINPUT4.insert(mINPUT4.end(),
                                   mTRAILER.begin(), mTRAILER.end());
                    const bsl::string RESULT4 = HEADER;
                    ss.clear(); ASSERT(ss.good()); ss.str("");
                    Util::printText(ss, INPUT4);
                    LOOP3_ASSERT(LINE, RESULT4, ss.str(), RESULT4 == ss.str());
                    LOOP_ASSERT(LINE, ss.fail());
                }
            }

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
                    LOOP3_ASSERT(LINE, RESULT, ss.str(), RESULT == ss.str());
                    LOOP_ASSERT(LINE, ss.fail());

                    const Type INPUT2 ( HEADER + INPUT.toString() );
                    const char *RESULT2 = HEADER.c_str();
                    ss.clear(); ASSERT(ss.good()); ss.str("");
                    Util::printText(ss, INPUT2);
                    LOOP3_ASSERT(LINE, RESULT2, ss.str(), RESULT2 == ss.str());
                    LOOP_ASSERT(LINE, ss.fail());

                    const Type INPUT3 ( INPUT.toString() + TRAILER );
                    const char *RESULT3 = RESULT;
                    ss.clear(); ASSERT(ss.good()); ss.str("");
                    Util::printText(ss, INPUT3);
                    LOOP3_ASSERT(LINE, RESULT3, ss.str(), RESULT3 == ss.str());
                    LOOP_ASSERT(LINE, ss.fail());

                    const Type INPUT4 ( HEADER + INPUT.toString() + TRAILER );
                    const char *RESULT4 = HEADER.c_str();
                    ss.clear(); ASSERT(ss.good()); ss.str("");
                    Util::printText(ss, INPUT4);
                    LOOP3_ASSERT(LINE, RESULT4, ss.str(), RESULT4 == ss.str());
                    LOOP_ASSERT(LINE, ss.fail());
                }
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

                Util::printText(ss, INPUT);

                LOOP3_ASSERT(LINE, RESULT, ss.str(), RESULT == ss.str());
                LOOP_ASSERT(LINE, ss.good());
            }
        }

        if (verbose) cout << "\nEnd of Test." << endl;
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING 'printList' FUNCTIONS
        //
        // Concerns:
        //
        // Plan:
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting 'printList' Functions"
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

                LOOP2_ASSERT(LINE, ss.str(), RESULT == ss.str());
            }
        }

        if (verbose) cout << "\nEnd of Test." << endl;
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING 'printHex' FUNCTIONS
        //
        // Concerns:
        //
        // Plan:
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting 'printHex' Functions"
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
                const int   LINE          = DATA[i].d_lineNum;
                const char *INPUT         = DATA[i].d_input;
                const char *RESULT        = DATA[i].d_result;
                const int   INPUT_LENGTH  = bsl::strlen(INPUT);

                Type mX(INPUT, INPUT + INPUT_LENGTH); const Type& X = mX;
                bsl::stringstream ss;

                Util::printHex(ss, X);

                LOOP2_ASSERT(LINE, ss.str(), RESULT == ss.str());
            }
        }

        if (verbose) cout << "\nUsing 'bslstl::StringRef'." << endl;
        {
            typedef bslstl::StringRef Type;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int   LINE          = DATA[i].d_lineNum;
                const char *INPUT         = DATA[i].d_input;
                const char *RESULT        = DATA[i].d_result;
                const int   INPUT_LENGTH  = bsl::strlen(INPUT);

                Type mX(INPUT, INPUT + INPUT_LENGTH); const Type& X = mX;
                bsl::stringstream ss;

                Util::printHex(ss, X);

                LOOP2_ASSERT(LINE, ss.str(), RESULT == ss.str());
            }
        }

        if (verbose) cout << "\nUsing 'bsl::vector<char>'." << endl;
        {
            typedef bsl::vector<char> Type;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int   LINE          = DATA[i].d_lineNum;
                const char *INPUT         = DATA[i].d_input;
                const char *RESULT        = DATA[i].d_result;
                const int   INPUT_LENGTH  = bsl::strlen(INPUT);

                Type mX(INPUT, INPUT + INPUT_LENGTH); const Type& X = mX;
                bsl::stringstream ss;

                Util::printHex(ss, X);

                LOOP2_ASSERT(LINE, ss.str(), RESULT == ss.str());
            }
        }

        if (verbose) cout << "\nEnd of Test." << endl;
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING 'printDecimal' FUNCTIONS
        //
        // Concerns:
        //
        // Plan:
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting 'printDecimal' Functions"
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

                LOOP2_ASSERT(LINE, ss.str(), RESULT == ss.str());
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

                LOOP2_ASSERT(LINE, ss.str(), RESULT == ss.str());
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

                LOOP2_ASSERT(LINE, ss.str(), RESULT == ss.str());
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

                LOOP2_ASSERT(LINE, ss.str(), RESULT == ss.str());
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
#if defined(BSLS_PLATFORM_CPU_64_BIT) && !defined(BSLS_PLATFORM_OS_WINDOWS)
                //line    input                     result
                //----    -----                     ------
                { L_,     static_cast<Type>(-9223372036854775808ULL),
                                                    "-9223372036854775808"   },
                { L_,     -9223372036854775807LL,   "-9223372036854775807"   },
                { L_,     -1LL,                     "-1"                     },
                { L_,     0LL,                      "0"                      },
                { L_,     1LL,                      "1"                      },
                { L_,     9223372036854775806LL,    "9223372036854775806"    },
                { L_,     9223372036854775807LL,    "9223372036854775807"    },
#else
                //line    input            result
                //----    -----            ------
                { L_,     -2147483647-1,   "-2147483648"   },
                { L_,     -2147483647,     "-2147483647"   },
                { L_,     -1,              "-1"            },
                { L_,     0,               "0"             },
                { L_,     1,               "1"             },
                { L_,     2147483646,      "2147483646"    },
                { L_,     2147483647,      "2147483647"    },
#endif
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int   LINE   = DATA[i].d_lineNum;
                const Type  INPUT  = DATA[i].d_input;
                const char *RESULT = DATA[i].d_result;

                bsl::stringstream ss;

                Util::printDecimal(ss, INPUT);

                LOOP3_ASSERT(LINE, ss.str(), RESULT, RESULT == ss.str());
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
                //line    input                     result
                //----    -----                     ------
                { L_,     static_cast<Type>(-9223372036854775808ULL),
                                                    "-9223372036854775808"   },
                { L_,     -9223372036854775807LL,   "-9223372036854775807"   },
                { L_,     -1LL,                     "-1"                     },
                { L_,     0LL,                      "0"                      },
                { L_,     1LL,                      "1"                      },
                { L_,     9223372036854775806LL,    "9223372036854775806"    },
                { L_,     9223372036854775807LL,    "9223372036854775807"    },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int   LINE   = DATA[i].d_lineNum;
                const Type  INPUT  = DATA[i].d_input;
                const char *RESULT = DATA[i].d_result;

                bsl::stringstream ss;

                Util::printDecimal(ss, INPUT);

                LOOP2_ASSERT(LINE, ss.str(), RESULT == ss.str());
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

                LOOP2_ASSERT(LINE, ss.str(), RESULT == ss.str());
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

                LOOP2_ASSERT(LINE, ss.str(), RESULT == ss.str());
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
                { L_,     4294967294,      "4294967294"    },
                { L_,     4294967295,      "4294967295"    },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int   LINE   = DATA[i].d_lineNum;
                const Type  INPUT  = DATA[i].d_input;
                const char *RESULT = DATA[i].d_result;

                bsl::stringstream ss;

                Util::printDecimal(ss, INPUT);

                LOOP2_ASSERT(LINE, ss.str(), RESULT == ss.str());
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
#if defined(BSLS_PLATFORM_CPU_64_BIT) && !defined(BSLS_PLATFORM_OS_WINDOWS)
                //line    input                     result
                //----    -----                     ------
                { L_,     0LL,                      "0"                      },
                { L_,     1LL,                      "1"                      },
                { L_,     18446744073709551614ULL,  "18446744073709551614"   },
                { L_,     18446744073709551615ULL,  "18446744073709551615"   },
#else
                //line    input            result
                //----    -----            ------
                { L_,     0,               "0"             },
                { L_,     1,               "1"             },
                { L_,     4294967294,      "4294967294"    },
                { L_,     4294967295,      "4294967295"    },
#endif
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int   LINE   = DATA[i].d_lineNum;
                const Type  INPUT  = DATA[i].d_input;
                const char *RESULT = DATA[i].d_result;

                bsl::stringstream ss;

                Util::printDecimal(ss, INPUT);

                LOOP3_ASSERT(LINE, ss.str(), RESULT, RESULT == ss.str());
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

                LOOP2_ASSERT(LINE, ss.str(), RESULT == ss.str());
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
                { L_,     test::MyEnumeration::VALUE1,  "1"         },
                { L_,     test::MyEnumeration::VALUE2,  "2"         },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int   LINE   = DATA[i].d_lineNum;
                const Type  INPUT  = DATA[i].d_input;
                const char *RESULT = DATA[i].d_result;

                bsl::stringstream ss;

                Util::printDecimal(ss, INPUT);

                LOOP2_ASSERT(LINE, ss.str(), RESULT == ss.str());
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

                LOOP2_ASSERT(LINE, ss.str(), RESULT == ss.str());
            }
        }

        if (verbose) cout << "\nUsing 'float'." << endl;
        {
            typedef float Type;

            static const struct {
                int         d_lineNum;
                Type        d_input;
                const char *d_result;
            } DATA[] = {
                //line    input       result
                //----    -----       ------
                { L_,     -1.0f,      "-1.0XXXXX"      },
                { L_,     -0.1f,      "-0.1XXXXX"      },
                { L_,     -0.123456f, "-0.123456"      },
                { L_,     0.0f,       "0.0XXXXX"       },
                { L_,     0.1f,       "0.1XXXXX"       },
                { L_,     1.0f,       "1.0XXXXX"       },
                { L_,     1234567.0f, "1234567.0X"     },
                { L_,     123.4567f,  "123.4567XX"     },
                { L_,  bsl::numeric_limits<float>::infinity(), "+INF"},
                { L_, -bsl::numeric_limits<float>::infinity(), "-INF"},
                { L_,  bsl::numeric_limits<float>::signaling_NaN(), "NaN"},
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int   LINE   = DATA[i].d_lineNum;
                const Type  INPUT  = DATA[i].d_input;
                const char *RESULT = DATA[i].d_result;

                bsl::stringstream ss;

                Util::printDecimal(ss, INPUT);

                if (bdlb::Float::isNan(INPUT) ||
                    bdlb::Float::isInfinite(INPUT)) {

                    LOOP2_ASSERT(LINE, ss.str(), !ss);
                }
                else {
                    bool rc = testFloatPointResult(ss.str().c_str(), RESULT);

                    LOOP3_ASSERT(LINE, ss.str(), RESULT, rc);
                }
            }
        }

        if (verbose) cout << "\nUsing 'double'." << endl;
        {
            typedef double Type;

            static const struct {
                int         d_lineNum;
                Type        d_input;
                const char *d_result;
            } DATA[] = {
                //line    input       result
                //----    -----       ------
                { L_,     -1.0,             "-1.0XXXXXXXXXXXXXX"   },
                { L_,     -0.1,             "-0.1XXXXXXXXXXXXXX"   },
                { L_, -0.123456789012345,   "-0.123456789012345"   },
                { L_,     0.0,              "0.0XXXXXXXXXXXXXX"    },
                { L_,     0.1,              "0.1XXXXXXXXXXXXXX"    },
                { L_,     1.0,              "1.0XXXXXXXXXXXXXX"    },
                { L_,     123.4567f,        "123.4567XXXXXXXXXXX"  },
                { L_, 123456789012345.0,    "123456789012345.0XX"  },
                { L_, 1234567890123456.0,   "1234567890123456.0X"  },
                { L_, 12345678901234567.0,  "1234567890123456X.X"  },
                { L_, 123456789012345678.0, "1234567890123456XX.X" },
                { L_,  bsl::numeric_limits<double>::infinity(), "+INF"},
                { L_, -bsl::numeric_limits<double>::infinity(), "-INF"},
                { L_,  bsl::numeric_limits<double>::signaling_NaN(), "NaN"},
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int   LINE   = DATA[i].d_lineNum;
                const Type  INPUT  = DATA[i].d_input;
                const char *RESULT = DATA[i].d_result;

                bsl::stringstream ss;

                balxml::EncoderOptions options;
                Util::printDecimal(ss, INPUT, &options);

                if (bdlb::Float::isNan(INPUT) ||
                    bdlb::Float::isInfinite(INPUT)) {

                    LOOP2_ASSERT(LINE, ss.str(), !ss);
                }
                else {
                    bool rc = testFloatPointResult(ss.str().c_str(), RESULT);

                    LOOP3_ASSERT(LINE, ss.str(), RESULT, rc);
                }
            }
        }

        if (verbose) cout << "\nUsing 'double' with encoder options" << endl;
        {
            typedef double Type;

            static const struct {
                int         d_lineNum;
                Type        d_input;
                int         d_maxTotalDigits;
                int         d_maxFractionDigits;
                const char *d_result;
            } DATA[] = {
        //line    input           TD     FD             result
        //----    -----           --     --             ------
         { L_,      0,             0,     0,           "0.0"    },
         { L_,      1,             0,     0,           "1.0"    },
         { L_,     -1,             0,     0,           "-1.0"   },

         { L_,      0,             0,     1,           "0.0"    },
         { L_,      1,             0,     1,           "1.0"    },
         { L_,     -1,             0,     1,           "-1.0"   },

         { L_,      0,             1,     0,           "0.0"    },
         { L_,      1,             1,     0,           "1.0"    },
         { L_,     -1,             1,     0,           "-1.0"   },

         { L_,      0,             1,     1,           "0.0"    },
         { L_,      1,             1,     1,           "1.0"    },
         { L_,     -1,             1,     1,           "-1.0"   },

         { L_,      0,             2,     1,           "0.0"    },
         { L_,      1,             2,     1,           "1.0"    },
         { L_,     -1,             2,     1,           "-1.0"   },

         { L_,      0,             2,     2,           "0.0"    },
         { L_,      1,             2,     2,           "1.0"    },
         { L_,     -1,             2,     2,           "-1.0"   },

         { L_,      0,             2,     3,           "0.0"    },
         { L_,      1,             2,     3,           "1.0"    },
         { L_,     -1,             2,     3,           "-1.0"   },

         { L_,      0,             3,     1,           "0.0"    },
         { L_,      1,             3,     1,           "1.0"    },
         { L_,     -1,             3,     1,           "-1.0"   },

         { L_,      0,             3,     2,           "0.00"   },
         { L_,      1,             3,     2,           "1.00"   },
         { L_,     -1,             3,     2,           "-1.00"  },

         { L_,      0,             3,     3,           "0.00"   },
         { L_,      1,             3,     3,           "1.00"   },
         { L_,     -1,             3,     3,           "-1.00"  },

         { L_,      0,             4,     2,           "0.00"   },
         { L_,      1,             4,     2,           "1.00"   },
         { L_,     -1,             4,     2,           "-1.00"  },

         { L_,      0,             4,     3,           "0.000"   },
         { L_,      1,             4,     3,           "1.000"   },
         { L_,     -1,             4,     3,           "-1.000"  },

         { L_,      0.0,           2,     0,           "0.0"    },
         { L_,      1.0,           2,     0,           "1.0"    },
         { L_,     -1.0,           2,     0,           "-1.0"   },

         { L_,      0.0,           2,     1,           "0.0"    },
         { L_,      1.0,           2,     1,           "1.0"    },
         { L_,     -1.0,           2,     1,           "-1.0"   },

         { L_,      0.0,           2,     2,           "0.0"    },
         { L_,      1.0,           2,     2,           "1.0"    },
         { L_,     -1.0,           2,     2,           "-1.0"   },

         { L_,      0.0,           2,     3,           "0.0"    },
         { L_,      1.0,           2,     3,           "1.0"    },
         { L_,     -1.0,           2,     3,           "-1.0"   },

         { L_,      0.0,           3,     2,           "0.00"   },
         { L_,      1.0,           3,     2,           "1.00"   },
         { L_,     -1.0,           3,     2,           "-1.00"  },

         { L_,      0.0,           3,     3,           "0.00"   },
         { L_,      1.0,           3,     3,           "1.00"   },
         { L_,     -1.0,           3,     3,           "-1.00"  },

         { L_,      0.1,           2,     0,           "0.1"    },
         { L_,     -0.1,           2,     0,           "-0.1"   },

         { L_,      0.1,           2,     1,           "0.1"    },
         { L_,     -0.1,           2,     1,           "-0.1"   },

         { L_,      0.1,           2,     2,           "0.1"    },
         { L_,     -0.1,           2,     2,           "-0.1"   },

         { L_,      0.1,           2,     3,           "0.1"    },
         { L_,     -0.1,           2,     3,           "-0.1"   },

         { L_,      0.1234,        0,     0,           "0.1"    },
         { L_,     -0.1234,        0,     0,           "-0.1"   },

         { L_,      0.1234,        1,     0,           "0.1"    },
         { L_,     -0.1234,        1,     0,           "-0.1"   },

         { L_,      0.1234,        0,     1,           "0.1"    },
         { L_,     -0.1234,        0,     1,           "-0.1"   },

         { L_,      0.1234,        1,     1,           "0.1"    },
         { L_,     -0.1234,        1,     1,           "-0.1"   },

         { L_,      79.864,        0,     0,           "79.9"    },
         { L_,     -63.234,        0,     0,           "-63.2"   },

         { L_,      79.864,        1,     0,           "79.9"    },
         { L_,     -63.234,        1,     0,           "-63.2"   },

         { L_,      79.864,        1,     1,           "79.9"    },
         { L_,     -63.234,        1,     1,           "-63.2"   },

         { L_,      79.864,        2,     1,           "79.9"    },
         { L_,     -63.234,        2,     1,           "-63.2"   },

         { L_,      79.864,        2,     2,           "79.9"    },
         { L_,     -63.234,        2,     2,           "-63.2"   },

         { L_,      79.864,        2,     3,           "79.9"    },
         { L_,     -63.234,        2,     3,           "-63.2"   },

         { L_,      79.864,        3,     3,           "79.8"    },
         { L_,     -63.234,        3,     3,           "-63.2"   },

         { L_,      79.864,        3,     2,           "79.8"    },
         { L_,     -63.234,        3,     2,           "-63.2"   },

         { L_,      79.864,        5,     3,           "79.864"  },
         { L_,     -63.234,        5,     3,           "-63.234" },

         { L_,      79.864,        5,     4,           "79.864"  },
         { L_,     -63.234,        5,     4,           "-63.234" },

         { L_,      79.864,        6,     3,           "79.864"  },
         { L_,     -63.234,        6,     3,           "-63.234" },

         { L_,      79.864,        6,     4,           "79.8640"  },
         { L_,     -63.234,        6,     4,           "-63.2340" },

         { L_, 123456789012345.0,  14,     0, "123456789012345.0"    },
         { L_, 123456789012345.0,  15,     0, "123456789012345.0"    },
         { L_, 123456789012345.0,  16,     0, "123456789012345.0"    },
         { L_, 123456789012345.0,  17,     0, "123456789012345.0"    },
         { L_, 123456789012345.0,  18,     0, "123456789012345.0"    },
         { L_, 123456789012345.0,  19,     0, "123456789012345.0"    },
         { L_, 123456789012345.0,  20,     0, "123456789012345.0"    },

         { L_, 123456789012345.0,  14,     1, "123456789012345.0"    },
         { L_, 123456789012345.0,  15,     1, "123456789012345.0"    },
         { L_, 123456789012345.0,  16,     1, "123456789012345.0"    },
         { L_, 123456789012345.0,  17,     1, "123456789012345.0"    },
         { L_, 123456789012345.0,  18,     1, "123456789012345.0"    },
         { L_, 123456789012345.0,  19,     1, "123456789012345.0"    },
         { L_, 123456789012345.0,  20,     1, "123456789012345.0"    },

         { L_, 123456789012345.0,  14,     2, "123456789012345.0"     },
         { L_, 123456789012345.0,  15,     2, "123456789012345.0"     },
         { L_, 123456789012345.0,  16,     2, "123456789012345.0"     },
         { L_, 123456789012345.0,  17,     2, "123456789012345.00"    },
         { L_, 123456789012345.0,  18,     2, "123456789012345.00"    },
         { L_, 123456789012345.0,  19,     2, "123456789012345.00"    },
         { L_, 123456789012345.0,  20,     2, "123456789012345.00"    },

         { L_, 123456789012345.0,  15,     0, "123456789012345.0"    },
         { L_, 123456789012345.0,  16,     0, "123456789012345.0"    },
         { L_, 123456789012345.0,  17,     0, "123456789012345.0"    },
         { L_, 123456789012345.0,  18,     0, "123456789012345.0"    },
         { L_, 123456789012345.0,  19,     0, "123456789012345.0"    },
         { L_, 123456789012345.0,  20,     0, "123456789012345.0"    },

         { L_, 123456789.012345,  14,     0, "123456789.0"          },
         { L_, 123456789.012345,  15,     0, "123456789.0"          },
         { L_, 123456789.012345,  16,     0, "123456789.0"          },
         { L_, 123456789.012345,  17,     0, "123456789.0"          },
         { L_, 123456789.012345,  18,     0, "123456789.0"          },
         { L_, 123456789.012345,  19,     0, "123456789.0"          },
         { L_, 123456789.012345,  20,     0, "123456789.0"          },

         { L_, 123456789.012345,  14,     1, "123456789.0"          },
         { L_, 123456789.012345,  15,     1, "123456789.0"          },
         { L_, 123456789.012345,  16,     1, "123456789.0"          },
         { L_, 123456789.012345,  17,     1, "123456789.0"          },
         { L_, 123456789.012345,  18,     1, "123456789.0"          },
         { L_, 123456789.012345,  19,     1, "123456789.0"          },
         { L_, 123456789.012345,  20,     1, "123456789.0"          },

         { L_, 123456789.012345,  14,     2, "123456789.01"          },
         { L_, 123456789.012345,  15,     2, "123456789.01"          },
         { L_, 123456789.012345,  16,     2, "123456789.01"          },
         { L_, 123456789.012345,  17,     2, "123456789.01"          },
         { L_, 123456789.012345,  18,     2, "123456789.01"          },
         { L_, 123456789.012345,  19,     2, "123456789.01"          },
         { L_, 123456789.012345,  20,     2, "123456789.01"          },

         { L_, 123456789.012345,  14,     6, "123456789.01234"       },
         { L_, 123456789.012345,  15,     6, "123456789.012345"      },
         { L_, 123456789.012345,  16,     6, "123456789.012345"      },
         { L_, 123456789.012345,  17,     6, "123456789.012345"      },
         { L_, 123456789.012345,  18,     6, "123456789.012345"      },
         { L_, 123456789.012345,  19,     6, "123456789.012345"      },
         { L_, 123456789.012345,  20,     6, "123456789.012345"      },

         { L_, 123456789.012345,  14,     7, "123456789.01234"       },
         { L_, 123456789.012345,  15,     7, "123456789.012345"      },
         { L_, 123456789.012345,  16,     7, "123456789.0123450"     },
         { L_, 123456789.012345,  17,     7, "123456789.0123450"     },
         { L_, 123456789.012345,  18,     7, "123456789.0123450"     },
         { L_, 123456789.012345,  19,     7, "123456789.0123450"     },
         { L_, 123456789.012345,  20,     7, "123456789.0123450"     },

         { L_,  bsl::numeric_limits<double>::infinity(), 0, 0, "+INF"},
         { L_, -bsl::numeric_limits<double>::infinity(), 0, 0, "-INF"},
         { L_,  bsl::numeric_limits<double>::signaling_NaN(), 0, 0, "NaN"},
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int   LINE   = DATA[i].d_lineNum;
                const Type  INPUT  = DATA[i].d_input;
                const int   TD     = DATA[i].d_maxTotalDigits;
                const int   FD     = DATA[i].d_maxFractionDigits;
                const char *RESULT = DATA[i].d_result;

                balxml::EncoderOptions options;
                options.setMaxDecimalTotalDigits(TD);
                options.setMaxDecimalFractionDigits(FD);

                bsl::stringstream ss;

                Util::printDecimal(ss, INPUT, &options);

                if (bdlb::Float::isNan(INPUT) ||
                    bdlb::Float::isInfinite(INPUT)) {

                    LOOP2_ASSERT(LINE, ss.str(), !ss);
                }
                else {
                    bool rc = testFloatPointResult(ss.str().c_str(), RESULT);

                    LOOP3_ASSERT(LINE, ss.str(), RESULT, rc);
                }
            }
        }

        if (verbose) cout << "\nEnd of Test." << endl;
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING 'printBase64' FUNCTIONS
        //
        // Concerns:
        //
        // Plan:
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting 'printBase64' Functions"
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
                const int   LINE          = DATA[i].d_lineNum;
                const char *INPUT         = DATA[i].d_input;
                const char *RESULT        = DATA[i].d_result;
                const int   INPUT_LENGTH  = bsl::strlen(INPUT);

                Type mX(INPUT, INPUT + INPUT_LENGTH); const Type& X = mX;
                bsl::stringstream ss;

                Util::printBase64(ss, X);

                LOOP2_ASSERT(LINE, ss.str(), RESULT == ss.str());
            }
        }

        if (verbose) cout << "\nUsing 'bslstl::StringRef'." << endl;
        {
            typedef bslstl::StringRef Type;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int   LINE          = DATA[i].d_lineNum;
                const char *INPUT         = DATA[i].d_input;
                const char *RESULT        = DATA[i].d_result;
                const int   INPUT_LENGTH  = bsl::strlen(INPUT);

                Type mX(INPUT, INPUT + INPUT_LENGTH); const Type& X = mX;
                bsl::stringstream ss;

                Util::printBase64(ss, X);

                LOOP2_ASSERT(LINE, ss.str(), RESULT == ss.str());
            }
        }

        if (verbose) cout << "\nUsing 'bsl::vector<char>'." << endl;
        {
            typedef bsl::vector<char> Type;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int   LINE          = DATA[i].d_lineNum;
                const char *INPUT         = DATA[i].d_input;
                const char *RESULT        = DATA[i].d_result;
                const int   INPUT_LENGTH  = bsl::strlen(INPUT);

                Type mX(INPUT, INPUT + INPUT_LENGTH); const Type& X = mX;
                bsl::stringstream ss;

                Util::printBase64(ss, X);

                LOOP2_ASSERT(LINE, ss.str(), RESULT == ss.str());
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

          {
              bsl::stringstream ss;

              balxml::TypesPrintUtil::print(ss, TestEnum::VALUE2,
                                    bdlat_FormattingMode::e_DEFAULT);
              LOOP_ASSERT(ss.str(), "VALUE2" == ss.str());
          }

          {
              bsl::stringstream ss;

              balxml::TypesPrintUtil::print(ss, TestEnum::VALUE2,
                                    bdlat_FormattingMode::e_DEC);
              LOOP_ASSERT(ss.str(), "2" == ss.str());
          }

          {
              bsl::vector<char> vec;
              bsl::stringstream ss;

              vec.push_back('a');
              vec.push_back('b');
              vec.push_back('c');
              vec.push_back('d');

              balxml::TypesPrintUtil::printBase64(ss, vec);

              LOOP_ASSERT(ss.str(), "YWJjZA==" == ss.str());
          }

          {
              bsl::vector<char> vec;
              bsl::stringstream ss;

              vec.push_back('a');
              vec.push_back('b');
              vec.push_back('c');
              vec.push_back('d');

              balxml::TypesPrintUtil::printHex(ss, vec);

              LOOP_ASSERT(ss.str(), "61626364" == ss.str());
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
